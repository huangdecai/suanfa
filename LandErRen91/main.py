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
              12, 12, 12, 13, 13, 13, 13, 14, 14, 14,17,20,30]

AllCards = ['rD', 'bX', 'b2', 'r2', 'bA', 'rA', 'bK', 'rK', 'bQ', 'rQ', 'bJ', 'rJ', 'bT', 'rT',
            'b9', 'r9', 'b8', 'r8', 'b7', 'r7', 'b6', 'r6', 'b5', 'r5', 'b4', 'r4', 'b3', 'r3']

helper = GameHelper()
helper.ScreenZoomRate = 1.0  # 请修改屏幕缩放比
MAX_CARD_COUNT=20
NORMAL_COUNT=17
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
        self.OtherConfidence = 0.92  # 别人的牌的置信度
        self.WhiteConfidence = 0.80  # 检测白块的置信度
        self.LandlordFlagConfidence = 0.9  # # 检测地主标志的置信度
        self.ThreeLandlordCardsConfidence = 0.9  # 检测地主底牌的置信度
        self.PassConfidence = 0.80
        self.WaitTime = 1  # 等待状态稳定延时
        self.MyFilter = 40  # 我的牌检测结果过滤参数
        self.OtherFilter = 25  # 别人的牌检测结果过滤参数
        self.SleepTime = 0.1  # 循环中睡眠时间
        self.yaoBuQi=True
        self.RunGame = False
        self.AutoPlay = True
        self.game_over = False
        self.onlyTip=False
        self.mylastCard=""
        self.myHavePassType = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        self.JiabeiThreshold = (
            (80, 65),  # 叫地主 超级加倍 加倍 阈值
            (85, 70)   # 叫地主 超级加倍 加倍 阈值  (在地主是抢来的情况下)
        )
        # 坐标
        self.MyHandCardsPos = (180, 277, 800, 100)  # 我的截图区域
        self.LPlayedCardsPos = (130, 150, 600, 80)  # 左边截图区域
        self.RPlayedCardsPos = (157, 151, 750, 80)  # 右边截图区域
        self.JiaoBtnPos = (296, 315, 200, 80)
        self.QiangBtnPos = (296, 315, 200, 80)
        self.RangBtnPos = (296, 315, 200, 80)
        self.BuRangBtnPos = (535, 315, 200, 80)
        self.BuQiangBtnPos = (535, 315, 200, 80)
        self.BujiaoBtnPos = (535, 315, 200, 80)
        self.PassBtnPos = (206, 315, 200, 80)
        self.GeneralBtnPos = (447, 464, 576, 137)
        self.OutCardBtnPos = (625, 315, 200, 150)
        self.YaoBuQiBtnPos = (390, 315, 200, 150)
        self.leaveBtnPos = (195, 500, 160, 70)
        self.changePlayerBtnPos = (636, 500, 160, 70)
        self.tipBtnPos = (374, 315, 200, 80)

        self.landPos = [(20, 270, 100, 80),(883, 36, 100, 80)]
        self.PassPos = (472, 165, 200, 80)
        self.RangZhangPos = (610, 53, 30, 30)

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
        self.ThreeLandlordCards.setText("跑得快")
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
        self.turnCardReal=''
        self.other_played_cards_real=''
        self.myHavePassType = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.allDisCardData=''
        self.handCardCount=[NORMAL_COUNT,NORMAL_COUNT,NORMAL_COUNT]
        self.bHavePass=False
        self.bLand=False
        self.rangZhuangNum=0
        #self.detect_start_btn()
        #helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos)
        self.env = None
        self.game_over= False
        self.shengYuPaiShow(self.allDisCardData)
        # 识别玩家手牌
        #temp=self.have_white(self.RPlayedCardsPos)

        #tmpHandCard = self.changeDataOut('2AKQQJJT99877753')
        #tmpHandCardStr = self.changeDataIn(tmpHandCard)
        # result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.85)
        # helper.ClickOnImage("tip_btn", region=self.tipBtnPos, confidence=0.85)
        #self.turnCardReal = self.find_other_cards(self.MyHandCardsPos)
        helper.bTest=False
        if helper.bTest==True:
           self.handCardCount[0] = 20

           #helper.SelectCards('2', self.handCardCount[0])
           #b= self.have_white(self.RPlayedCardsPos)
           self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)
           helper.SelectCards('777', self.handCardCount[0], False)

        self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos) #'2AKQQJJT99877753'
        cardNum=len(self.user_hand_cards_real)
        self.UserHandCards.setText("手牌：" + self.user_hand_cards_real)
        print("手牌:", cardNum)
        bRestart = False
        while cardNum!=NORMAL_COUNT :
            self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)
            self.UserHandCards.setText("手牌：" + self.user_hand_cards_real)
            cardNum = len(self.user_hand_cards_real)
            self.sleep(2000)
            resultChange = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
            if resultChange:
                bRestart = True
                break
            print("hangCountShiBieWenTi",len(self.user_hand_cards_real))
        if bRestart == False:
            cardScore = self.getCardScore(self.user_hand_cards_real)
            searIndex = 0
            searchStr = ["jiao", "qiang", "rang", "bujiao", "buqiang", "burang"]
            bHaveOperate = [False, False, False]
            print("resultTip1......:", searIndex)
            resultTip = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.85)
            existLand=self.find_landlordExist()
            TryCount=0
            while resultTip == None and existLand==False:
                result1 = helper.LocateOnScreen(searchStr[searIndex], region=self.QiangBtnPos, confidence=0.85)
                result2 = helper.LocateOnScreen(searchStr[searIndex+3], region=self.QiangBtnPos, confidence=0.85)
                if result1  or result2 :
                    if ((searIndex==1 and  cardScore>30) or(searIndex==0 and  cardScore>10)) and bHaveOperate[searIndex]==False:
                        helper.ClickOnImage(searchStr[searIndex], region=self.QiangBtnPos, confidence=0.85)
                    else:
                        helper.ClickOnImage(searchStr[searIndex+3], region=self.BuQiangBtnPos, confidence=0.85)
                    bHaveOperate[searIndex]=True
                    TryCount=0
                else:
                    TryCount=TryCount+1
                self.sleep(50)
                print("resultTip2......:", searIndex)
                searIndex=searIndex+1
                if searIndex>2 :
                    searIndex=0
                resultTip = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.85)
                existLand = self.find_landlordExist()
                if existLand==True :
                    self.sleep(300)
                    result1 = helper.LocateOnScreen(searchStr[2], region=self.QiangBtnPos, confidence=0.85)
                    result2 = helper.LocateOnScreen(searchStr[2 + 3], region=self.QiangBtnPos,
                                                   confidence=0.85)
                    if result1 or result2:
                        existLand=False
                resultChange = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
                if resultChange:
                    bRestart=True
                    break
        if bRestart==False:
            self.play_order = self.find_landlord()
            self.rangZhuangNum=self.findRangZhang()
            print("play_order-rangZhuangNum......:",self.play_order,self.rangZhuangNum)
            #self.play_order=2
            if self.play_order==0 :
                self.handCardCount[0] = MAX_CARD_COUNT
                self.handCardCount[2] = NORMAL_COUNT
                self.bLand=True
            else:
                self.handCardCount[0] = NORMAL_COUNT
                self.handCardCount[2] = MAX_CARD_COUNT
                self.bLand=False
            self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)  # '2AKQQJJT99877753'
            cardNum = len(self.user_hand_cards_real)
            print("手牌2:", cardNum)
            while cardNum != self.handCardCount[0]:
                self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)
                cardNum = len(self.user_hand_cards_real)
                self.sleep(2000)
                print("hangCountShiBieWenTi2", len(self.user_hand_cards_real))
            #self.user_hand_cards_real =  'AKKKKQJJT9988744'
            #self.turnCardReal='665544'
            #self.allDisCardData='665544'
            self.UserHandCards.setText(self.user_hand_cards_real)
            # 识别玩家的角色
            print("开始对局")
            print("手牌:",self.user_hand_cards_real)
            # 生成手牌结束，校验手牌数量
            # 得到出牌顺序
            try:
                self.start()
            except Exception as e:
                exc_type, exc_obj, exc_tb = sys.exc_info()
                print(e)
                traceback.print_tb(exc_tb)
                self.stop()
        else:
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
        return self.handCardCount[0]==0 or self.handCardCount[2]==0
    def start(self):
        print("开始出牌\n")
        while not self.game_over:
            # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
            if self.play_order == 0:
                self.PredictedCard.setText("...")
                if len(self.turnCardReal)==0 :
                    self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)

                action_message = self.dllCall(self.user_hand_cards_real,self.turnCardReal,self.allDisCardData,self.bHavePass,self.myHavePassType)
                self.PredictedCard.setText(action_message if (len(action_message) > 0) else "不出")

                print(self.play_order,"\nuser_hand_cards_real：", self.user_hand_cards_real)
                print(self.play_order,"\nturnCardReal：", self.turnCardReal)
                print(self.play_order,"\nallDisCardData：", self.allDisCardData)
                print(self.play_order,"\n我出牌：", action_message)
                hand_cards_str = self.user_hand_cards_real
                tempOutCardPos=self.OutCardBtnPos
                # if self.yaoBuQi  :
                #     tempOutCardPos =self.FirstOutCardBtnPos

                bPass=False
                if len(action_message) == 0:
                    helper.ClickOnImage("pass_btn", region=self.PassBtnPos, confidence=0.80)
                    self.sleep(1000)
                    print(self.play_order,"pass_btn")
                    bPass=True
                else:
                    if self.onlyTip :
                        print( "只提示：", action_message)
                    else:
                        print( "自己准备点击出牌：", action_message)
                        if len(hand_cards_str) == 0 and len(action_message) == 1:
                            helper.SelectCards(action_message,self.handCardCount[0], True)
                        else:
                            helper.SelectCards(action_message,self.handCardCount[0])
                        self.sleep(1000)
                        tryCount = 20
                        result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.80)
                        while result is None :
                            if not self.RunGame:
                                break
                            print(self.play_order,"等待出牌按钮",tempOutCardPos[0])
                            self.detect_start_btn()
                            tryCount -= 1
                            result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.80)
                            self.sleep(100)
                        helper.ClickOnImage("go_btn", region=tempOutCardPos, confidence=0.75)

                # 更新界面
                #tmpCard=self.find_other_cardsEx(self.MyHandCardsPos)
                self.mylastCard =action_message
                self.handCardCount[self.play_order]=self.handCardCount[self.play_order]-len(action_message)
                self.allDisCardData=self.allDisCardData+action_message
                self.user_hand_cards_real = self.DeleteCard(self.user_hand_cards_real, action_message)
                self.UserHandCards.setText("手牌：" + self.user_hand_cards_real)
                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])
                print(self.play_order, "handcount2：", self.handCardCount[2])

                result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.75)
                while (result is not None) and bPass==False :
                    result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.75)
                    print(self.play_order, "wait--tip_btn")
                    self.sleep(10)
                self.shengYuPaiShow(self.allDisCardData)
                self.play_order = 2
                #self.sleep(200)
                self.detect_start_btn()
            elif self.play_order == 2:
                self.RPlayedCard.setText("...")
                pass_flag = helper.LocateOnScreen('pass',
                                                  region=self.PassPos,
                                                  confidence=self.PassConfidence)
                whiteCount=self.have_white(self.RPlayedCardsPos)
                bBieRenRang=False
                while self.RunGame and whiteCount == 0 and pass_flag is None:
                    print(self.play_order,"等待下家出牌")
                    whiteCount = self.have_white(self.RPlayedCardsPos)
                    pass_flag = helper.LocateOnScreen('pass', region=self.PassPos,
                                                      confidence=self.PassConfidence)
                    if self.handCardCount[0]==17and whiteCount == 0 and pass_flag is None:
                        result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.80)
                        whiteCount = self.have_white(self.RPlayedCardsPos)
                        pass_flag = helper.LocateOnScreen('pass', region=self.PassPos,
                                                          confidence=self.PassConfidence)
                        if result and whiteCount == 0 and pass_flag is None:
                            bBieRenRang=True
                            break
                    self.sleep(10)
                if bBieRenRang==False:
                    # 未找到"不出"
                    if pass_flag is None:
                        # 识别下家出牌
                        self.RPlayedCard.setText("等待动画")
                        self.RPlayedCard.setText("识别中")
                        self.sleep(500)
                        self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)
                        if len(self.turnCardReal) > 0:
                            self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(
                                self.turnCardReal)
                        print("下家出牌：", self.turnCardReal)
                        self.WinRate.setText("别人出牌：" + self.turnCardReal)
                        self.other_played_cards_real = self.other_played_cards_real + self.turnCardReal
                        self.allDisCardData = self.allDisCardData + self.turnCardReal
                        self.yaoBuQi = False
                    # 找到"不出"
                    else:
                        print("下家出牌：不出")
                        self.FillPassType(self.mylastCard)
                        self.turnCardReal = ""
                        self.bHavePass = True
                        self.yaoBuQi=True

                    print(self.play_order,"handcount0：", self.handCardCount[0])
                    print(self.play_order,"handcount2：", self.handCardCount[2])
                    self.shengYuPaiShow(self.allDisCardData)
                    self.sleep(800)
                    self.detect_start_btn()
                    self.sleep(1000)
                    resultyaobuqi = helper.LocateOnScreen("yaobuqi", region=self.YaoBuQiBtnPos, confidence=0.80)
                    if resultyaobuqi :
                        helper.ClickOnImage("yaobuqi", region=self.YaoBuQiBtnPos, confidence=0.80)
                        self.sleep(1000)
                        while resultyaobuqi:
                            helper.ClickOnImage("yaobuqi", region=self.YaoBuQiBtnPos, confidence=0.80)
                            resultyaobuqi = helper.LocateOnScreen("yaobuqi", region=self.YaoBuQiBtnPos, confidence=0.80)
                            self.sleep(50)
                            print("resultyaobuqi1......:")
                        print("resultyaobuqi2......:")
                        self.sleep(500)
                    else:
                        self.play_order = 0
                else:
                    print("别人让牌......:")
                    self.play_order = 0
            elif self.play_order == 1:
                self.RPlayedCard.setText("...")
                pass_flag = helper.LocateOnScreen('pass',
                                                  region=self.LPlayedCardsPos,
                                                  confidence=self.PassConfidence)
                while self.RunGame and self.have_white(self.LPlayedCardsPos) == 0 and pass_flag is None:
                    print("等待下家出牌")
                    self.sleep(5)
                    pass_flag = helper.LocateOnScreen('pass', region=self.LPlayedCardsPos,
                                                      confidence=self.PassConfidence)
                # 未找到"不出"
                bYiChang=False
                if pass_flag is None:
                    # 识别下家出牌
                    self.RPlayedCard.setText("等待动画")
                    self.RPlayedCard.setText("识别中")
                    self.sleep(100)
                    self.turnCardReal = self.find_other_cards(self.LPlayedCardsPos)
                    if len(self.turnCardReal)>0 :
                        self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(self.turnCardReal)
                        print("下家出牌：", self.turnCardReal)
                    else:
                        print("识别出牌异常：", self.turnCardReal)
                        bYiChang = True
                        self.sleep(1000)
                    self.other_played_cards_real=self.other_played_cards_real+self.turnCardReal
                    self.allDisCardData = self.allDisCardData + self.turnCardReal
                # 找到"不出"
                else:
                    self.turnCardReal= ""
                    self.bHavePass=True
                    print("要不起")
                self.shengYuPaiShow(self.allDisCardData)
                if bYiChang==False:
                   self.play_order = 0
                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])
                self.sleep(800)
                self.detect_start_btn()
            else:
                pass
            self.sleep(50)

        print("{}胜，本局结束!\n")
        # QMessageBox.information(self, "本局结束", "{}胜！".format("农民" if self.env.winner == "farmer" else "地主"),
        #                         QMessageBox.Yes, QMessageBox.Yes)
        #self.detect_start_btn()

    def find_landlordExist(self):
        resultLand1 = helper.LocateOnScreen("land", region=self.landPos[0], confidence=0.85)
        resultLand2 = helper.LocateOnScreen("land", region=self.landPos[1], confidence=0.85)
        if resultLand1 == None and resultLand2 == None:
            return  False
        return  True
    def find_landlord(self):
            while self.find_landlordExist()==False:
                self.sleep(50)
                print("find_landlord......:")
            result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos,
                                           confidence=self.LandlordFlagConfidence)
            if result is not None:
                return 0
            return 2
    def findRangZhang(self):
            print("findRangZhang......:")
            rangNum=["0","1","2","3","4"]
            for i in range(0, 5):
                result = helper.LocateOnScreen("rang"+rangNum[i], region=self.RangZhangPos,
                                               confidence=0.90)
                if result:
                    return int(rangNum[i])
    def detect_start_btn(self):
        result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
        if (result is not None) or self.isGameOver():
            print("点击换对手")
            self.stop()
            self.sleep(2500)
            if self.AutoPlay:
                self.sleep(2000)
                helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos,)
                self.sleep(1000)
                self.initStart()
    def shengYuPaiShow(self, cards):
        #self.ThreeLandlordCards.resize(300,100)
        AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 1, '2': 2, 'X': 20, 'D': 30}
        AllCardCount = {3: 4, 4: 4, 5: 4, 6: 4, 7: 4,
                   8: 4, 9: 4, 10: 4, 11: 4, 12: 4,
                   13: 4, 1: 4, 2: 4, 16: 0, 17: 0,20: 1,30: 1}
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

    def GetCardLogicValue(self, cbCardValue):
        if (cbCardValue <= 2):
            return cbCardValue + 13
        else:
            return cbCardValue
    def FillPassType(self, cards):
            tmtype = self.getCardType(cards)
            value=self.GetMaxCount(cards)
            if self.myHavePassType[tmtype] == 0:
                self.myHavePassType[tmtype] = value
            else:
                if self.GetCardLogicValue(value)< self.GetCardLogicValue(self.myHavePassType[tmtype]):
                   self.myHavePassType[tmtype] = value
    def GetMaxCount(self, cards):
            # 转换外面传进来的数据
            AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                       '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                       'K': 13, 'A': 1, '2': 2, 'X': 20, 'D': 30}
            AllCardCount = {3: 0, 4: 0, 5: 0, 6: 0, 7: 0,
                            8: 0, 9: 0, 10: 0, 11: 0, 12: 0,
                            13: 0, 1: 0, 2: 0, 20: 0, 30: 0}
            tmpCard = [AllCard[c] for c in list(cards)]
            AllcardData = []
            for i in range(0, len(tmpCard)):
                AllCardCount[tmpCard[i]]+=1
            vaule=0
            maxCount=0
            for i in range(1, 14):
                if(AllCardCount[i]>maxCount):
                    maxCount=AllCardCount[i]
                    vaule=i

            return vaule
    def changeDataOut(self, cards):
        # 转换外面传进来的数据
        AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                   '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                   'K': 13, 'A': 1, '2': 2, 'X': 78, 'D': 79}
        AllCardCount = {3: 0, 4: 0, 5: 0, 6: 0, 7: 0,
                        8: 0, 9: 0, 10: 0, 11: 0, 12: 0,
                        13: 0, 1: 0, 2: 3, 14: 0, 15: 0}
        tmpCard = [AllCard[c] for c in list(cards)]
        AllcardData=[]
        for i in range(0, len(tmpCard)):
            AllcardData.append(tmpCard[i])
            #AllCardCount[tmpCard[i]]+=1
        return AllcardData

    def changeDataIn(self, cards):
        # 转换外面传进来的数据
        AllCard = {3: '3', 4: '4', 5: '5', 6: '6', 7: '7',
                   8: '8', 9: '9', 10: 'T', 11: 'J', 12: 'Q',
                   13: 'K', 1: 'A', 2: '2', 14: 'X', 15: 'D'}

        for i in range(0, len(cards)):
            cards[i]= cards[i]%16

        tmpCard = [AllCard[c] for c in list(cards)]
        tmpCardstr="".join([card[0] for card in tmpCard])
        return tmpCardstr
    def dllCall(self,HandCardData,TurnCardData,DiscardData,bPass,HavePassType):

        class tagInPyhonNew(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* 20),
                        ("cbHandCardCount", c_ubyte),
                        ("cbTurnCardData", c_ubyte* 20),
                        ("cbTurnCardCount", c_ubyte ),
                        ("DiscardCard", c_ubyte * 54),
                        ("cbDiscardCardCount", c_ubyte),
                        ("cbCardDataEx", c_ubyte * 20),
                        ("cbMaxCard", c_ubyte * 20),
                        ("cbCardCount", c_ubyte),
                        ("cbResultCard", c_ubyte * 20)
                        ]
        pDll = CDLL("./testC++.dll")
        arg1 = tagInPyhonNew()
        #tmpHandCard=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x11,0x12,0x13,0x25,0x35,0x16,0x17]
        tmpHandCard=self.changeDataOut(HandCardData)
        tmpTurnCard=self.changeDataOut(TurnCardData)
        tmpDiscard=self.changeDataOut(DiscardData)
        tmparray=c_ubyte * MAX_COUNT
        tmparray2 = c_ubyte * FULL_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbTurnCardData = tmparray()
        arg1.cbResultCard = tmparray()

        arg1.DiscardCard = tmparray2()
        arg1.cbCardDataEx = tmparray()
        arg1.cbMaxCard = tmparray()
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
        for i in range(len(tmpTurnCard), 20):
             arg1.cbCardDataEx[i] = 0
        for i in range(0, 20):
            arg1.cbMaxCard[i] = HavePassType[i]
        for i in range(len(tmpDiscard), 54):
            arg1.DiscardCard[i] = 0
        if self.bLand:
            arg1.cbCardDataEx[0]=self.rangZhuangNum
        else:
            arg1.cbCardDataEx[1] = self.rangZhuangNum
        arg1.cbHandCardCount = len(tmpHandCard)
        arg1.cbTurnCardCount = len(tmpTurnCard)
        arg1.cbDiscardCardCount = len(tmpDiscard)

        arg1.cbCardCount=0
        arg1.cbOthreRangCardCount=0


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
    def getCardType(self,HandCardData):

        class tagInPyhonType(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* 20),
                        ("cbHandCardCount", c_ubyte),
                        ("cbType", c_ubyte ),
                        ]
        pDll = CDLL("./testC++.dll")
        arg1 = tagInPyhonType()
        tmpHandCard=self.changeDataOut(HandCardData)
        tmparray=c_ubyte * MAX_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbHandCardCount=len(HandCardData)
        arg1.cbType = 0
        for i in range(0,len(tmpHandCard)):
            arg1.cbHandCardData[i]=tmpHandCard[i]


        func=pDll.fntestPythonType
        func.restype = c_int32
        func.argtypes = [POINTER(tagInPyhonType)]
        result = func(byref(arg1))
        print("type:",arg1.cbType)

        return arg1.cbType
    def getCardScore(self,HandCardData):

        class tagInPyhonType(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* 20),
                        ("cbHandCardCount", c_ubyte),
                        ("cbCardScore", c_int ),
                        ]
        pDll = CDLL("./testC++.dll")
        arg1 = tagInPyhonType()
        tmpHandCard=self.changeDataOut(HandCardData)
        tmparray=c_ubyte * MAX_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbHandCardCount=len(HandCardData)
        arg1.cbCardScore = 0
        for i in range(0,len(tmpHandCard)):
            arg1.cbHandCardData[i]=tmpHandCard[i]


        func=pDll.PythonCountScore
        func.restype = c_int32
        func.argtypes = [POINTER(tagInPyhonType)]
        result = func(byref(arg1))
        print("type:",arg1.cbCardScore)

        return arg1.cbCardScore
    def find_my_cards(self, pos):
        user_hand_cards_real = ""
        img, _ = helper.Screenshot()
        cards, _ = helper.GetCards(img,self.handCardCount[0])
        for c in cards:
            user_hand_cards_real += c[0]
        return user_hand_cards_real
    def find_other_cardsEx(self, pos):
        user_hand_cards_real = ""
        img, _ = helper.Screenshot()
        cards, _ = helper.GetCardsEx(img,pos)
        for c in cards:
            user_hand_cards_real += c[0]
        return user_hand_cards_real
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
