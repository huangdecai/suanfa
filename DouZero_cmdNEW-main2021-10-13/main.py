# -*- coding: utf-8 -*-
# Created by: Raf
# Modify by: Vincentzyx

import os
import sys
import time
import threading
import pyautogui
import win32gui
from PIL import Image
import multiprocessing as mp
import cv2
import numpy as np

from PyQt5 import QtGui, QtWidgets, QtCore
from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPixmapItem, QInputDialog, QMessageBox
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import QTime, QEventLoop

from douzero.env.game import GameEnv
from douzero.evaluation.deep_agent import DeepAgent
import traceback

import BidModel
import LandlordModel

EnvCard2RealCard = {3: '3', 4: '4', 5: '5', 6: '6', 7: '7',
                    8: '8', 9: '9', 10: 'T', 11: 'J', 12: 'Q',
                    13: 'K', 14: 'A', 17: '2', 20: 'X', 30: 'D'}

RealCard2EnvCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 14, '2': 17, 'X': 20, 'D': 30}

AllEnvCard = [3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
              8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
              12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 17, 17, 17, 17, 20, 30]

AllCards = ['rD', 'bX', 'b2', 'r2', 'bA', 'rA', 'bK', 'rK', 'bQ', 'rQ', 'bJ', 'rJ', 'bT', 'rT',
            'b9', 'r9', 'b8', 'r8', 'b7', 'r7', 'b6', 'r6', 'b5', 'r5', 'b4', 'r4', 'b3', 'r3']
tmp_position = ['landlord_up', 'landlord', 'landlord_down']

class MyPyQT_Form():
    def __init__(self):
        super(MyPyQT_Form, self).__init__()

        self.counter = QTime()

        # 参数
        self.MyConfidence = 0.95  # 我的牌的置信度
        self.OtherConfidence = 0.9  # 别人的牌的置信度
        self.WhiteConfidence = 0.95  # 检测白块的置信度
        self.LandlordFlagConfidence = 0.9  # # 检测地主标志的置信度
        self.ThreeLandlordCardsConfidence = 0.9  # 检测地主底牌的置信度
        self.PassConfidence = 0.85
        self.WaitTime = 1  # 等待状态稳定延时
        self.MyFilter = 40  # 我的牌检测结果过滤参数
        self.OtherFilter = 25  # 别人的牌检测结果过滤参数
        self.SleepTime = 0.1  # 循环中睡眠时间
        self.RunGame = False
        self.AutoPlay = False
        self.BidThreshold1 = 65  # 叫地主阈值
        self.BidThreshold2 = 75  # 抢地主阈值
        # 信号量
        self.shouldExit = 0  # 通知上一轮记牌结束
        self.canRecord = threading.Lock()  # 开始记牌
        self.card_play_model_path_dict = {
            'landlord': "baselines/douzero_ADP/landlord.ckpt",
            'landlord_up': "baselines/douzero_ADP/landlord_up.ckpt",
            'landlord_down': "baselines/douzero_ADP/landlord_down.ckpt"
        }
    def init_cards(self,oldhandCardData,myhandCardData,diPaiCardData,position_code,outCardList):
        self.RunGame = True
        # 玩家手牌
        self.user_hand_cards_real = ""
        self.user_hand_cards_env = []
        # 其他玩家出牌
        self.other_played_cards_real = ""
        self.other_played_cards_env = []
        # 其他玩家手牌（整副牌减去玩家手牌，后续再减掉历史出牌）
        self.other_hand_cards = []
        # 三张底牌
        self.three_landlord_cards_real = ""
        self.three_landlord_cards_env = []
        # 玩家角色代码：0-地主上家, 1-地主, 2-地主下家
        self.user_position_code = None
        self.user_position = ""
        # 开局时三个玩家的手牌
        self.card_play_data_list = {}
        # 出牌顺序：0-玩家出牌, 1-玩家下家出牌, 2-玩家上家出牌
        self.play_order = 0

        self.env = None

        # 识别玩家手牌
        self.user_hand_cards_real = myhandCardData
        self.user_hand_cards_env = [RealCard2EnvCard[c] for c in list(self.user_hand_cards_real)]
        self.user_hand_cards_real_old = oldhandCardData
        self.user_hand_cards_env_old = [RealCard2EnvCard[c] for c in list(self.user_hand_cards_real_old)]
        # 识别三张底牌
        self.three_landlord_cards_real = diPaiCardData
        self.three_landlord_cards_env = [RealCard2EnvCard[c] for c in list(self.three_landlord_cards_real)]

        # 识别玩家的角色
        self.user_position_code = position_code
        self.user_position = ['landlord_up', 'landlord', 'landlord_down'][self.user_position_code]
        # 整副牌减去玩家手上的牌，就是其他人的手牌,再分配给另外两个角色（如何分配对AI判断没有影响）
        for i in set(AllEnvCard):
            self.other_hand_cards.extend([i] * (AllEnvCard.count(i) - self.user_hand_cards_env_old.count(i)))
        self.card_play_data_list.update({
            'three_landlord_cards': self.three_landlord_cards_env,
            ['landlord_up', 'landlord', 'landlord_down'][(self.user_position_code + 0) % 3]:
                self.user_hand_cards_env,
            ['landlord_up', 'landlord', 'landlord_down'][(self.user_position_code + 1) % 3]:
                self.other_hand_cards[0:17] if (self.user_position_code + 1) % 3 != 1 else self.other_hand_cards[17:],
            ['landlord_up', 'landlord', 'landlord_down'][(self.user_position_code + 2) % 3]:
                self.other_hand_cards[0:17] if (self.user_position_code + 1) % 3 == 1 else self.other_hand_cards[17:]
        })
        print("开始对局")
        print("手牌:",self.user_hand_cards_real)
        print("地主牌:",self.three_landlord_cards_real)
        # 生成手牌结束，校验手牌数量
        # 得到出牌顺序
        self.play_order = 0 if self.user_position == "landlord" else 1 if self.user_position == "landlord_up" else 2

        # 创建一个代表玩家的AI
        ai_players = [0, 0]
        ai_players[0] = self.user_position
        ai_players[1] = DeepAgent(self.user_position, self.card_play_model_path_dict[self.user_position])

        self.env = GameEnv(ai_players)
        try:
            self.env.card_play_init(self.card_play_data_list,outCardList,position_code)
            self.env.recordAction(position_code, outCardList)
            return self.start()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            self.stop()

    def sleep(self, ms):
        self.counter.restart()
        while self.counter.elapsed() < ms:
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 50)

    def start(self):
        print("开始出牌\n")
        # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
        if 1:
            print("轮到：", self.user_position)
            action_message = self.env.step(self.user_position)
            # 更新界面
            if action_message["action"] == "":
                print("不出：")
            else:
                hand_cards_str = ''.join([EnvCard2RealCard[c] for c in self.env.info_sets[self.user_position].player_hand_cards])
                print(hand_cards_str)
            print("要出的牌：",action_message["action"])
            return action_message["action"]
    def changeData(self,str,outCardAction):
        #转换外面传进来的数据
        leftCount = 0
        rightCount = 0
        tempIndex = -1
        tempArray = []
        for c in list(str):
            if c == '[':
                leftCount = leftCount + 1
            elif c == ',':
                a = 4
            elif c == ']':
                rightCount = rightCount + 1
                if rightCount == 2 and leftCount == 2:
                    outCardAction.append([tempIndex, tempArray.copy()])
                    leftCount = 0
                    rightCount = 0
                    tempIndex = -1
                    tempArray = []
            elif c == 'T' or c == 'D' or c == 'X' or c == 'J' or c == 'Q' or c == 'K':
                tmps = RealCard2EnvCard[c]
                tempArray.append(RealCard2EnvCard[c])
            elif int(c) >= 0 and int(c) <= 9:
                if leftCount == 1:
                    tempIndex = int(c)
                elif leftCount == 2:
                    tempArray.append(int(c))
    def cards_filter(self, location, distance):  # 牌检测结果滤波
        if len(location) == 0:
            return 0
        locList = [location[0][0]]
        count = 1
        for e in location:
            flag = 1  # “是新的”标志
            for have in locList:
                if abs(e[0] - have) <= distance:
                    flag = 0
                    break
            if flag:
                count += 1
                locList.append(e[0])
        return count



    def stop(self):
        try:
            self.RunGame = False
            self.env.game_over = True
            self.env.reset()
        except AttributeError as e:
            pass
'''
if __name__ == '__main__':
    my_pyqt_form = MyPyQT_Form()
    oldHand='2AKQQJJT998877753'
    #tmpHand = '2AKQQJJT997775'
    tmpHand='2AKQQJJT998877753'
    diHand = '666'
    tmpstr = '[1,[3,3]]'
    outCardAction=[]
    my_pyqt_form.changeData(tmpstr,outCardAction)
    #outCardAction='[[1,[8,8]],[2,[]],[0,[]],[1,[6,6,6,3]],[2,[]],[0,[]]]'
    my_pyqt_form.init_cards(oldHand,tmpHand,diHand,0,outCardAction)

'''
if __name__ == '__main__':
    my_pyqt_form = MyPyQT_Form()
    oldHand=sys.argv[1]
    tmpHand =sys.argv[2]
    diHand = sys.argv[3]
    code = int(sys.argv[4])
    tmpstr=sys.argv[5]
    for i in range(1,6):
        print(sys.argv[i])
    outCardAction=[]
    my_pyqt_form.changeData(tmpstr,outCardAction)
    my_pyqt_form.init_cards(oldHand,tmpHand,diHand,code,outCardAction)
