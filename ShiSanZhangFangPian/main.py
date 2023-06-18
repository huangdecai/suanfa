# -*- coding: utf-8 -*-
# Created by: Raf
# Modify by: Vincentzyx

import GameHelper as gh
from GameHelper import GameHelper
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
from MainWindow import Ui_Form

from douzero.env.game import GameEnv
from douzero.evaluation.deep_agent import DeepAgent
import traceback
import random
import BidModel
import LandlordModel
import FarmerModel
from ctypes import *
MAX_COUNT=20
FULL_COUNT=54
EnvCard2RealCard = {3: '3', 4: '4', 5: '5', 6: '6', 7: '7',
                    8: '8', 9: '9', 10: 'T', 11: 'J', 12: 'Q',
                    13: 'K', 14: 'A', 17: '2', 20: 'X', 30: 'D'}

RealCard2EnvCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 14, '2': 17, 'X': 20, 'D': 30}

AllEnvCard = [3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
              8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
              12, 12, 12, 13, 13, 13, 13, 14, 14, 14,17]

AllCards = ['rD', 'bX', 'b2', 'r2', 'bA', 'rA', 'bK', 'rK', 'bQ', 'rQ', 'bJ', 'rJ', 'bT', 'rT',
            'b9', 'r9', 'b8', 'r8', 'b7', 'r7', 'b6', 'r6', 'b5', 'r5', 'b4', 'r4', 'b3', 'r3']
COLOR_LIST = ["♦", "♣", "♥", "♠"]
helper = GameHelper()
helper.ScreenZoomRate = 1.0  # 请修改屏幕缩放比
MAX_CARD_COUNT=13
class MyPyQT_Form(QtWidgets.QWidget, Ui_Form):
    def __init__(self):
        super(MyPyQT_Form, self).__init__()
        self.setupUi(self)
        self.setWindowFlags(QtCore.Qt.WindowMinimizeButtonHint |  # 使能最小化按钮
                            QtCore.Qt.WindowCloseButtonHint)  # 窗体总在最前端 QtCore.Qt.WindowStaysOnTopHint
        self.setFixedSize(self.width(), self.height())  # 固定窗体大小
        # self.setWindowIcon(QIcon('pics/favicon.ico'))
        window_pale = QtGui.QPalette()
        # window_pale.setBrush(self.backgroundRole(), QtGui.QBrush(QtGui.QPixmap("pics/bg.png")))
        self.setPalette(window_pale)
        self.Players = [self.RPlayer, self.Player, self.LPlayer]
        self.counter = QTime()

        # 参数
        self.MyConfidence = 0.95  # 我的牌的置信度
        self.OtherConfidence = 0.87  # 别人的牌的置信度
        self.WhiteConfidence = 0.70  # 检测白块的置信度
        self.LandlordFlagConfidence = 0.9  # # 检测地主标志的置信度
        self.ThreeLandlordCardsConfidence = 0.9  # 检测地主底牌的置信度
        self.PassConfidence = 0.60
        self.WaitTime = 1  # 等待状态稳定延时
        self.MyFilter = 40  # 我的牌检测结果过滤参数
        self.OtherFilter = 25  # 别人的牌检测结果过滤参数
        self.SleepTime = 0.1  # 循环中睡眠时间
        self.yaoBuQi=True
        self.RunGame = False
        self.AutoPlay = False
        self.game_over = False
        self.onlyTip=True
        self.BidThreshold1 = 65  # 叫地主阈值
        self.BidThreshold2 = 75  # 抢地主阈值
        self.JiabeiThreshold = (
            (80, 65),  # 叫地主 超级加倍 加倍 阈值
            (85, 70)   # 叫地主 超级加倍 加倍 阈值  (在地主是抢来的情况下)
        )
        # 坐标
        self.MyHandCardsPos = (10, 380, 900, 100)  # 我的截图区域
        self.LPlayedCardsPos = (130, 150, 600, 80)  # 左边截图区域
        self.RPlayedCardsPos = (400, 150, 500, 80)  # 右边截图区域
        self.PassBtnPos = (670, 150, 200, 80)
        self.GeneralBtnPos = (447, 464, 576, 137)
        self.OutCardBtnPos = (219, 615, 160, 100)
        self.FirstOutCardBtnPos = (219, 615, 160, 100)
        self.changePlayerBtnPos = (350, 620, 160, 70)
        self.tipBtnPos = (454, 321, 200, 100)
        # 信号量
        self.shouldExit = 0  # 通知上一轮记牌结束
        self.canRecord = threading.Lock()  # 开始记牌

    def init_display(self):
        self.WinRate.setText("评分")
        self.InitCard.setText("开始")
        self.UserHandCards.setText("手牌")
        self.LPlayedCard.setText("上家出牌区域")
        self.RPlayedCard.setText("下家出牌区域")
        self.PredictedCard.setText("AI出牌区域")
        self.ThreeLandlordCards.setText("十三张")
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")
        for player in self.Players:
            player.setStyleSheet('background-color: rgba(255, 0, 0, 0);')

    def switch_mode(self):
        self.AutoPlay = not self.AutoPlay
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")

    def init_cards(self):
        self.RunGame = True
        GameHelper.Interrupt = False
        self.init_display()
        # 玩家手牌
        self.user_hand_cards_real = ""
        self.user_hand_colors=[]
        self.turnCardReal=''
        self.other_played_cards_real=''
        self.allDisCardData=''
        self.handCardCount=[MAX_CARD_COUNT,MAX_CARD_COUNT,MAX_CARD_COUNT]
        self.bHavePass=False
        #self.detect_start_btn()
        #helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos)
        self.env = None
        self.game_over= False
        self.shengYuPaiShow(self.allDisCardData)
        #helper.ClickOnImageEX("go_btn", region=self.OutCardBtnPos, confidence=0.85)
        # 识别玩家手牌
        #temp=self.have_white(self.RPlayedCardsPos)
        #self.turnCardReal = self.find_other_cards(self.RPlayedCardsPos)
        #tmpHandCard = self.changeDataOut('2AKQQJJT99877753')
        #tmpHandCardStr = self.changeDataIn(tmpHandCard)
        self.user_hand_cards_real,self.user_hand_colors = self.find_my_cards(self.MyHandCardsPos) #'2AKQQJJT99877753'
        while len(self.user_hand_cards_real)!=MAX_CARD_COUNT :
            self.user_hand_cards_real,self.user_hand_colors = self.find_my_cards(self.MyHandCardsPos)
            self.sleep(2000)
            print("hangCountShiBieWenTi",len(self.user_hand_cards_real))
        #self.user_hand_cards_real =  'AKKKKQJJT9988744'
        #self.turnCardReal='665544'
        #self.allDisCardData='665544'
        tmpCardstr = ""
        for i in range(0, len(self.user_hand_cards_real)):
            tmpCardstr += COLOR_LIST[self.user_hand_colors[i]] + self.user_hand_cards_real[i]
        self.UserHandCards.setText(tmpCardstr)
        # 识别玩家的角色

        print("开始对局")
        print("手牌:",self.user_hand_cards_real)
        # 生成手牌结束，校验手牌数量


        # 得到出牌顺序
        self.play_order =0# self.find_landlord()
        try:
            self.start()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            self.stop()

    def sleep(self, ms):
        self.counter.restart()
        while self.counter.elapsed() < ms:
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 50)
    def DeleteCard(self, src,sub):
        tmpstr = src
        for i in range(0, len(sub)):
            for j in range(0, len(src)):
                if sub[i] == src[j]:
                    tmpstr = tmpstr.replace(sub[i], '', 1)
                    break
        return tmpstr
    def isGameOver(self):
        result = helper.LocateOnScreenEX("change_player_btn", region=self.changePlayerBtnPos)
        while result is None:
            print("等待下一局\n")
            if self.RunGame== False:
                break
            result = helper.LocateOnScreenEX("change_player_btn", region=self.changePlayerBtnPos)
            self.sleep(1000)
        return True
    def start(self):
        print("开始出牌\n")
        while not self.game_over:
            # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
            if self.play_order == 0:
                self.shengYuPaiShow(self.user_hand_cards_real)
                self.PredictedCard.setText("...")
                action_message,colors = self.dllCall(self.user_hand_cards_real,self.user_hand_colors,self.turnCardReal,self.allDisCardData,self.bHavePass)
                tmpCardstr = ""
                for i in range(0,len(action_message)):
                    tmpCardstr +=COLOR_LIST[colors[i]]+action_message[i]
                    if i==2 or i==7 :
                       tmpCardstr += "\n "
                self.PredictedCard.setText(tmpCardstr if (len(tmpCardstr) > 0) else "不出")
                self.WinRate.setText("评分：" + '0')
                print(self.play_order,"\nuser_hand_cards_real：", self.user_hand_cards_real)
                print(self.play_order,"\nturnCardReal：", self.turnCardReal)
                print(self.play_order,"\nallDisCardData：", self.allDisCardData)
                print(self.play_order,"\n出牌：", action_message)
                hand_cards_str = self.user_hand_cards_real
                tempOutCardPos=self.OutCardBtnPos

                if len(action_message) == 0:
                    #helper.ClickOnImage("pass_btn", region=self.PassBtnPos)
                    self.sleep(2000)
                    print(self.play_order,"pass_btn")
                else:
                    #if self.onlyTip :
                        #a=4
                    #else:
                        outCardStr=""
                        tempColors=[]
                        for i in range(0,len(action_message)):
                            outCardStr+=action_message[i]
                            tempColors.append(colors[i])
                            if i==2 :
                                helper.SelectCards(outCardStr, self.handCardCount[0],tempColors)
                                result = helper.LocateOnScreen("out", region=(270, 290, 100, 100),
                                                               confidence=0.70)
                                if result is not None:
                                    helper.ClickOnImage("out", region=(270, 290, 100, 100), confidence=0.70)
                                    outCardStr = ""
                                    tempColors = []
                                    self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(
                                        outCardStr)

                            elif i==7 :
                                helper.SelectCards(outCardStr,  self.handCardCount[0],tempColors)
                                result = helper.LocateOnScreen("out2", region=(270, 290+110, 100, 100),
                                                               confidence=0.70)
                                if result is not None:
                                    helper.ClickOnImage("out2", region=(270, 290+110, 100, 100),confidence=0.70)
                                    outCardStr = ""
                                    tempColors = []
                                    self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(
                                        outCardStr)
                                    break
                            self.sleep(500)

                        self.sleep(1500)
                # 更新界面
                randnum = random.randint(2, 6)
                self.sleep(1000 * randnum)
                if self.onlyTip:
                    a = 4
                else:
                    helper.ClickOnImage("go_btn", region=tempOutCardPos, confidence=0.80)
                self.allDisCardData=self.allDisCardData+action_message
                #self.user_hand_cards_real = self.DeleteCard(self.user_hand_cards_real, action_message)
                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])

                result = helper.LocateOnScreenEX("go_btn", region=tempOutCardPos)
                while result is not None :
                    result = helper.LocateOnScreenEX("go_btn", region=tempOutCardPos)
                    self.sleep(50)
                #self.play_order = 2
                #self.sleep(200)
                self.detect_start_btn()
            elif self.play_order == 1:
                self.RPlayedCard.setText("...")
                pass_flag = helper.LocateOnScreen('pass',
                                                  region=self.LPlayedCardsPos,
                                                  confidence=self.PassConfidence)
                while self.RunGame and self.have_white(self.LPlayedCardsPos) == 0 and pass_flag is None:
                    print("等待下家出牌")
                    self.sleep(100)
                    pass_flag = helper.LocateOnScreen('pass', region=self.LPlayedCardsPos,
                                                      confidence=self.PassConfidence)
                # 未找到"不出"
                if pass_flag is None:
                    # 识别下家出牌
                    self.RPlayedCard.setText("等待动画")
                    self.RPlayedCard.setText("识别中")
                    self.sleep(300)
                    self.turnCardReal = self.find_other_cards(self.LPlayedCardsPos)
                    if len(self.turnCardReal)>0 :
                        self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(self.turnCardReal)
                    print("下家出牌：", self.turnCardReal)
                    self.other_played_cards_real=self.other_played_cards_real+self.turnCardReal
                    self.allDisCardData = self.allDisCardData + self.turnCardReal
                # 找到"不出"
                else:
                    self.turnCardReal= ""
                    self.bHavePass=True
                    print("要不起")
                self.shengYuPaiShow(self.allDisCardData)
                self.play_order = 0
                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])
                self.sleep(800)
                self.detect_start_btn()
            elif self.play_order == 2:
                self.RPlayedCard.setText("...")
                pass_flag = helper.LocateOnScreen('pass',
                                                  region=self.PassBtnPos,
                                                  confidence=self.PassConfidence)
                while self.RunGame and self.have_white(self.RPlayedCardsPos) == 0 and pass_flag is None:
                    print(self.play_order,"等待下家出牌")
                    self.sleep(100)
                    pass_flag = helper.LocateOnScreen('pass', region=self.PassBtnPos,
                                                      confidence=self.PassConfidence)
                # 未找到"不出"
                if pass_flag is None:
                    # 识别下家出牌
                    self.RPlayedCard.setText("等待动画")
                    self.RPlayedCard.setText("识别中")
                    self.sleep(500)
                    self.turnCardReal = self.find_other_cards(self.RPlayedCardsPos)
                    if len(self.turnCardReal) > 0:
                        self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(
                            self.turnCardReal)
                    print("下家出牌：", self.turnCardReal)
                    self.other_played_cards_real = self.other_played_cards_real + self.turnCardReal
                    self.allDisCardData = self.allDisCardData + self.turnCardReal
                    self.yaoBuQi = False
                # 找到"不出"
                else:
                    self.turnCardReal = ""
                    self.bHavePass = True
                    self.yaoBuQi=True

                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])
                self.shengYuPaiShow(self.allDisCardData)
                self.sleep(800)
                self.play_order = 0
                self.detect_start_btn()

            else:
                pass
            self.sleep(50)

        print("{}胜，本局结束!\n")
        # QMessageBox.information(self, "本局结束", "{}胜！".format("农民" if self.env.winner == "farmer" else "地主"),
        #                         QMessageBox.Yes, QMessageBox.Yes)
        #self.detect_start_btn()

    def find_landlord(self):
            result = helper.LocateOnScreen("play_card", region=self.FirstOutCardBtnPos,
                                           confidence=self.LandlordFlagConfidence)
            if result is not None:
                return 0
            return 2

    def detect_start_btn(self):
        if  self.isGameOver():
            print("点击换对手")
            self.stop()
            self.sleep(500)
            if self.AutoPlay:
                self.sleep(2000)
                helper.ClickOnImageEX("change_player_btn", region=self.changePlayerBtnPos)
                self.sleep(1000)
                self.initStart()
    def shengYuPaiShow(self, cards):
        #self.ThreeLandlordCards.resize(300,100)
        AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 1, '2': 2, 'X': 20, 'D': 30}
        AllCardCount = {3: 4, 4: 4, 5: 4, 6: 4, 7: 4,
                   8: 4, 9: 4, 10: 4, 11: 4, 12: 4,
                   13: 4, 1: 4, 2: 4, 16: 0, 17: 0,20: 0,30: 0}
        AllCardStr = '3456789TJQKA2XD'
        tmpCard = [AllCard[c] for c in list(cards)]
        for i in range(0, len(tmpCard)):
            AllCardCount[tmpCard[i]]-=1
        tmpCardstr=""
        tmpCardCountStr=" "
        for i in range(len(AllCardStr)-1,-1,-1,):
            tmpCardstr +=AllCardStr[i]+" "
            tmpCardCountStr+=str((AllCardCount[AllCard[AllCardStr[i]]]))+" "
            if 'X'==AllCardStr[i]:
                tmpCardCountStr += " "
        self.ThreeLandlordCards.setText(tmpCardstr+'\n'+tmpCardCountStr)
    def changeDataOut(self, cards,colors):
        # 转换外面传进来的数据
        AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 1, '2': 2, 'X': 20, 'D': 30}
        AllCardCount = {3: 0, 4: 0, 5: 0, 6: 0, 7: 0,
                   8: 0, 9: 0, 10: 0, 11: 0, 12: 0,
                   13: 0, 1: 0, 2: 3, 16: 0, 17: 0}
        tmpCard = [AllCard[c] for c in list(cards)]
        AllcardData=[]
        for i in range(0, len(tmpCard)):
            AllcardData.append(tmpCard[i]+16*colors[i])
            AllCardCount[tmpCard[i]]+=1
        return AllcardData

    def changeDataIn(self, cards):
        # 转换外面传进来的数据
        AllCard = {3: '3', 4: '4', 5: '5', 6: '6', 7: '7',
                            8: '8', 9: '9', 10: 'T', 11: 'J', 12: 'Q',
                            13: 'K', 1: 'A', 2: '2', 20: 'X', 30: 'D'}
        colors=[]
        for i in range(0, len(cards)):
            colors.append(int(cards[i] / 16))
            cards[i]= cards[i]%16

        tmpCard = [AllCard[c] for c in list(cards)]
        tmpCardstr="".join([card[0] for card in tmpCard])
        return tmpCardstr,colors
    def dllCall(self,HandCardData,colors,TurnCardData,DiscardData,bPass):

        class tagInPyhonNew(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* 20),
                        ("cbHandCardCount", c_ubyte),
                        ("cbTurnCardData", c_ubyte* 20),
                        ("cbTurnCardCount", c_ubyte ),
                        ("DiscardCard", c_ubyte * 54),
                        ("cbDiscardCardCount", c_ubyte),
                        ("cbRangCardCount", c_ubyte ),
                        ("cbOthreRangCardCount", c_ubyte),
                        ("cbCardCount", c_ubyte),
                        ("cbResultCard", c_ubyte * 20)
                        ]
        pDll = CDLL("./testC++.dll")
        arg1 = tagInPyhonNew()
        #tmpHandCard=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x11,0x12,0x13,0x25,0x35,0x16,0x17]
        tmpHandCard=self.changeDataOut(HandCardData,colors)
        tmpTurnCard=self.changeDataOut(TurnCardData,colors)
        tmpDiscard=self.changeDataOut(DiscardData,colors)
        tmparray=c_ubyte * MAX_COUNT
        tmparray2 = c_ubyte * FULL_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbTurnCardData = tmparray()
        arg1.cbResultCard = tmparray()

        arg1.DiscardCard = tmparray2()

        for i in range(0,len(tmpHandCard)):
            arg1.cbHandCardData[i]=tmpHandCard[i]
        for i in range(0, len(tmpTurnCard)):
            arg1.cbTurnCardData[i] = tmpTurnCard[i]
        for i in range(0, len(tmpDiscard)):
            arg1.DiscardCard[i] = tmpDiscard[i]

        for i in range(len(tmpHandCard), 20):
            arg1.cbHandCardData[i] = 0
        for i in range(len(tmpTurnCard), 20):
            arg1.cbTurnCardData[i] = 0
        for i in range(len(tmpDiscard), 54):
            arg1.DiscardCard[i] = 0

        arg1.cbHandCardCount = len(tmpHandCard)
        arg1.cbTurnCardCount = len(tmpTurnCard)
        arg1.cbDiscardCardCount = len(tmpDiscard)

        arg1.cbCardCount=0
        arg1.cbOthreRangCardCount=0
        if bPass :
            arg1.cbRangCardCount = 1
        else:
            arg1.cbRangCardCount = 0

        func=pDll.fntestPython2
        func.restype = c_int32
        func.argtypes = [POINTER(tagInPyhonNew)]
        result = func(byref(arg1))
        print("action:")
        returnCardData=[]
        for i in range(0,arg1.cbCardCount):
            returnCardData.append(arg1.cbResultCard[i])
            print(arg1.cbResultCard[i])

        return self.changeDataIn(returnCardData)
    def find_my_cards(self, pos):
        user_hand_cards_real = ""
        img, _ = helper.Screenshot()
        cards, states,colors = helper.GetCards(img,self.handCardCount[0])
        for c in cards:
            user_hand_cards_real += c[0]
        return user_hand_cards_real,colors

    def find_other_cards(self, pos):
        other_played_cards_real = ""
        img, _ = helper.Screenshot()
        imgCv = cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)
        for card in AllCards:
            result = gh.LocateAllOnImage(imgCv, helper.PicsCV['o' + card], region=pos, confidence=self.OtherConfidence)
            if len(result) > 0:
                other_played_cards_real += card[1] * self.cards_filter(list(result), self.OtherFilter)
        if other_played_cards_real == "DX":
            other_played_cards_real = "XD"
        return other_played_cards_real

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

    def have_white(self, pos):  # 是否有白块
        img, _ = helper.Screenshot()
        result = pyautogui.locate(needleImage=helper.Pics["white"], haystackImage=img,
                                  region=pos, confidence=self.WhiteConfidence)
        if result is None:
            return 0
        else:
            return 1

    def stop(self):
        try:
            self.RunGame = False
            self.game_over = True
            self.env.reset()
            self.init_display()
            self.PreWinrate.setText("局前预估胜率：")
            self.BidWinrate.setText("")
        except AttributeError as e:
            pass
        if self.AutoPlay:
            play_btn = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
            while play_btn is None and self.AutoPlay:
                play_btn = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
                self.sleep(100)
            if play_btn is not None:
                helper.LeftClick((play_btn[0], play_btn[1]))
                self.initStart()

    def beforeStart(self):
        self.onlyTip = not self.onlyTip
        self.AutoStart.setText("只提示" if self.onlyTip else "代打")
    def initStart(self):
        self.RunGame = True
        self.yaoBuQi=True
        GameHelper.Interrupt = True
        self.sleep(2000)
        self.init_cards()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    app.setStyleSheet("""
    QPushButton{
        text-align : center;
        background-color : white;
        font: bold;
        border-color: gray;
        border-width: 2px;
        border-radius: 10px;
        padding: 6px;
        height : 14px;
        border-style: outset;
        font : 14px;
    }
    QPushButton:hover{
        background-color : light gray;
    }
    QPushButton:pressed{
        text-align : center;
        background-color : gray;
        font: bold;
        border-color: gray;
        border-width: 2px;
        border-radius: 10px;
        padding: 6px;
        height : 14px;
        border-style: outset;
        font : 14px;
        padding-left:9px;
        padding-top:9px;
    }
    QComboBox{
        background:transparent;
        border: 1px solid rgba(200, 200, 200, 100);
        font-weight: bold;
    }
    QComboBox:drop-down{
        border: 0px;
    }
    QComboBox QAbstractItemView:item{
        height: 30px;
    }
    QLabel{
        background:transparent;
        font-weight: bold;
    }
    """)
    my_pyqt_form = MyPyQT_Form()
    my_pyqt_form.show()
    sys.exit(app.exec_())
