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
FULL_COUNT=144
GAME_PLAYER=4
MAX_WEAVE=4

AllEnvCard = [3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
              8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
              12, 12, 12, 13, 13, 13, 13, 14, 14, 14,17]
AllCardOutStr = {'': '','0': '一万', '1': '二万', '2': '三万', '3': '四万', '4': '五万', '5': '六万',
              '6': '七万', '7': '八万', '8': '九万', '9': '一条', '10': '二条', '11': '三条',
              '12': '四条', '13': '五条', '14': '六条', '15': '七条', '16': '八条', '17': '九条',
              '18': '一筒', '19': '二筒', '20': '三筒', '21': '四筒', '22': '五筒', '23': '六筒',
              '24': '七筒', '25': '八筒', '26': '九筒', '27': '东', '28': '南', '29': '西',
              '30': '北', '31': '中', '32': '发', '33': '白'}
AllCardOut = {'': 0,'0': 0x01, '1': 0x02, '2': 0x03, '3': 0x04, '4': 0x05, '5': 0x06,
              '6': 0x07, '7': 0x08, '8': 0x09, '9': 0x11, '10': 0x12, '11': 0x13,
              '12': 0x14, '13': 0x15, '14': 0x16, '15': 0x17, '16': 0x18, '17': 0x19,
              '18': 0x21, '19': 0x22, '20': 0x23, '21': 0x24, '22': 0x25, '23': 0x26,
              '24': 0x27, '25': 0x28, '26': 0x29, '27': 0x31, '28': 0x32, '29': 0x33,
              '30': 0x34, '31': 0x35, '32': 0x36, '33': 0x37}
AllCards = ['rD', 'bX', 'b2', 'r2', 'bA', 'rA', 'bK', 'rK', 'bQ', 'rQ', 'bJ', 'rJ', 'bT', 'rT',
            'b9', 'r9', 'b8', 'r8', 'b7', 'r7', 'b6', 'r6', 'b5', 'r5', 'b4', 'r4', 'b3', 'r3']
COLOR_LIST = ["♦", "♣", "♥", "♠"]
helper = GameHelper()
helper.ScreenZoomRate = 1.0  # 请修改屏幕缩放比
MAX_CARD_COUNT=14
WIK_NULL	=				0x00								#//没有类型
WIK_LEFT=					0x01								#//左吃类型
WIK_CENTER	=				0x02								#//中吃类型
WIK_RIGHT=					0x04								#//右吃类型
WIK_PENG=					0x08								#//碰牌类型
WIK_GANG=					0x10								#//杠牌类型
WIK_LISTEN	=				0x20								#//吃牌类型
WIK_CHI_HU	=				0x40								#//吃胡类型
WIK_ZI_MO	=				0x80								#//自摸
WIK_CHI_XUAN	=			0x100								#//吃选

class WeaveItem:
    def __init__(self):
        self.cbWeaveKind = ''
        self.cbCenterCard = ''
        self.cbPublicCard = ''
        self.wProvideUser = 0

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
        self.onlyTip=False
        self.callCard=0
        self.cbLaiZi=[]
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
        self.ActionBtnPos = (300, 515, 750, 112)
        self.ActionCardPos = (288, 100, 382, 90)
        self.ActionBtnPosClick =[]
        self.OutCardBtnPos = (418, 418, 200, 150)
        self.FirstOutCardBtnPos = (460, 320, 200, 150)
        self.changePlayerBtnPos = (695, 587, 190, 50)

        self.dingThreeCardBtnPos = (389, 294, 190, 55)#三张牌
        self.dingQueBtnPos = [(310, 308, 130, 140),(430, 308, 130, 140),(540, 308, 130, 140)]#定缺
        self.tipBtnPos = (454, 321, 200, 100)
        # 信号量
        self.shouldExit = 0  # 通知上一轮记牌结束
        self.canRecord = threading.Lock()  # 开始记牌
        self.m_WeaveItem=[[],[],[],[]]
        self.waiteChiAction=WIK_NULL

        a=4
    def init_display(self):
        self.WinRate.setText("评分")
        self.WinRate.setVisible(False)
        self.InitCard.setText("开始")
        self.UserHandCards.setText("手牌")
        self.LPlayedCard.setText("上家出牌区域")
        self.RPlayedCard.setText("下家出牌区域")
        self.PredictedCard.setText("AI出牌区域")
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")
        for player in self.Players:
            player.setStyleSheet('background-color: rgba(255, 0, 0, 0);')

    def switch_mode(self):
        self.AutoPlay = not self.AutoPlay
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")
    def printHandData(self):
        tmpCardstr = ""
        for i in range(0, len(self.user_hand_cards_real)):
            tmpCardstr += str(AllCardOutStr[self.user_hand_cards_real[i]]) + ","
        self.UserHandCards.setText(tmpCardstr)
    def isGameStart(self):
        result = helper.LocateOnScreen("tip", region=(323,380,100,100), confidence=0.75)
        while result is not None:
            if self.RunGame == False:
                break
            self.sleep(30)
            print("isGameStart")
            result = helper.LocateOnScreen("tip", region=(323,380,100,100), confidence=0.75)
        print("isGameStart")
    def switchThreeCard(self):
        result = helper.LocateOnScreen("dingThree", region=self.dingThreeCardBtnPos, confidence=0.75)
        while result is None:
            if self.RunGame == False:
                break
            self.sleep(500)
            print("switchThreeCard")
            result = helper.LocateOnScreen("dingThree", region=self.dingThreeCardBtnPos, confidence=0.75)
        helper.ClickOnImage("dingThree", region=self.dingThreeCardBtnPos, confidence=0.75)
        print("switchThreeCard")
    def dingQueCard(self):
        result0 = helper.LocateOnScreen("jian0", region=self.dingQueBtnPos[0], confidence=0.85)
        result1 = helper.LocateOnScreen("jian1", region=self.dingQueBtnPos[1], confidence=0.85)
        result2 = helper.LocateOnScreen("jian2", region=self.dingQueBtnPos[2], confidence=0.85)
        tryCount=0
        while ((result0 is None)and (result1 is None)and(result2 is None)):
            if self.RunGame == False:
                break
            self.sleep(20)
            result0 = helper.LocateOnScreen("jian0", region=self.dingQueBtnPos[0], confidence=0.85)
            result1 = helper.LocateOnScreen("jian1", region=self.dingQueBtnPos[1], confidence=0.85)
            result2 = helper.LocateOnScreen("jian2", region=self.dingQueBtnPos[2], confidence=0.85)
            tryCount=tryCount+1
            if tryCount>=150 :
                result0 = helper.LocateOnScreen("tip0", region=self.dingQueBtnPos[0], confidence=0.85)
                result1 = helper.LocateOnScreen("tip1", region=self.dingQueBtnPos[1], confidence=0.85)
                result2 = helper.LocateOnScreen("tip2", region=self.dingQueBtnPos[2], confidence=0.85)
            print("dingQueCard")
        tmpStr=["jian0","jian1","jian2"]
        index=0
        indexEx1=1
        indexEx2=2
        if result0 is not None:
            index=0
            indexEx1 = 1
            indexEx2 = 2
            self.callCard=0x03
        elif result1 is not None:
            index=1
            indexEx1 = 2
            indexEx2 = 0
            self.callCard = 0x23
        elif result2 is not None:
            index=2
            indexEx1 = 1
            indexEx2 = 0
            self.callCard = 0x13
        resultEx=None
        tmpStrEx = ["tip0", "tip1", "tip2"]
        helper.ClickOnImageEx(tmpStr[index], region=self.dingQueBtnPos[index], confidence=0.72)
        resultEx = helper.LocateOnScreen(tmpStrEx[indexEx1], region=self.dingQueBtnPos[indexEx1], confidence=0.72)
        if resultEx is None:
            resultEx = helper.LocateOnScreen(tmpStrEx[indexEx2], region=self.dingQueBtnPos[indexEx2], confidence=0.72)
        print("dingQueCard33",self.callCard,indexEx1,index,indexEx2)
        while (resultEx is not None):
            if self.RunGame == False:
                break
            helper.ClickOnImage(tmpStrEx[index], region=self.dingQueBtnPos[index], confidence=0.72)
            self.sleep(20)
            print("dingQueCard2", self.callCard,index,indexEx1)
            resultEx = helper.LocateOnScreen(tmpStrEx[indexEx1], region=self.dingQueBtnPos[indexEx1], confidence=0.72)
            if resultEx is None:
                resultEx = helper.LocateOnScreen(tmpStrEx[indexEx2], region=self.dingQueBtnPos[indexEx2],
                                                 confidence=0.72)
        self.sleep(1000)
    def init_cards(self):
        self.RunGame = True
        GameHelper.Interrupt = False
        self.init_display()
        # 玩家手牌
        self.user_hand_cards_real = []
        self.user_hand_colors=[]
        self.turnCardReal=''
        self.other_played_cards_real=''
        self.allDisCardData=[]
        self.m_WeaveItem = [[], [], [], []]
        self.handCardCount=[MAX_CARD_COUNT,MAX_CARD_COUNT,MAX_CARD_COUNT,MAX_CARD_COUNT]
        self.bHavePass=False
        self.bHaveAction = False
        #self.detect_start_btn()
        #helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos)
        self.env = None
        self.game_over= False
        helper.bTest = False
        #self.shengYuPaiShow(self.allDisCardData)
        # 识别玩家手牌
        #temp=self.have_white(self.RPlayedCardsPos)
        #self.turnCardReal = self.find_other_cards(self.RPlayedCardsPos)
        #tmpHandCard = self.changeDataOut('2AKQQJJT99877753')
        #tmpHandCardStr = self.changeDataIn(tmpHandCard)
        # self.user_hand_cards_real=self.changeDataIn([33, 33, 18, 19, 20, 21, 22, 23, 23, 23, 24, 55, 55, 55])
        # self.allDisCardData=self.changeDataIn([49,49,49,50,53,51,36,40,39,52,39,35])
        # action_message, cbOperateCode = self.dllCall(self.user_hand_cards_real, self.allDisCardData, 0, 0,
        #                                              self.callCard, 0, '33', self.cbLaiZi)
        #result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos, confidence=0.75)
        self.user_hand_cards_real,self.cbLaiZi = self.find_my_cards(self.MyHandCardsPos) #'2AKQQJJT99877753'
        while (len(self.user_hand_cards_real) <MAX_CARD_COUNT-1):
            self.user_hand_cards_real,self.cbLaiZi = self.find_my_cards(self.MyHandCardsPos)
            self.sleep(2000)
            print("hangCountShiBieWenTi",len(self.user_hand_cards_real))
        #self.user_hand_cards_real =  'AKKKKQJJT9988744'
        #self.turnCardReal='665544'
        #self.allDisCardData='665544'
        self.printHandData()
        # 识别玩家的角色

        print("开始对局手牌数：",len(self.user_hand_cards_real))
        print("手牌:",self.user_hand_cards_real)
        # 生成手牌结束，校验手牌数量


        # 得到出牌顺序
        self.play_order = 0#self.find_landlord()
        self.WaitForOtherOperate(False)
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
                    tmpstr.pop(j)
                    break
        return tmpstr
    def isGameOver(self):
        result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
        while result is None:
            if self.RunGame== False:
                break
            self.sleep(1000)
        return True
    def GetStrOperateCode(self,cbOperateCode):
        tmpCardstr = ""
        if cbOperateCode == WIK_CHI_HU:
            tmpCardstr = "胡牌"
        elif cbOperateCode == WIK_LEFT:
            tmpCardstr = "左吃"
        elif cbOperateCode == WIK_CENTER:
            tmpCardstr = "中吃"
        elif cbOperateCode == WIK_RIGHT:
            tmpCardstr = "右吃"
        elif cbOperateCode == WIK_PENG:
            tmpCardstr = "碰牌"
        elif cbOperateCode == WIK_GANG:
            tmpCardstr = "杠牌"
        elif cbOperateCode == WIK_LISTEN:
            tmpCardstr = "听牌"
        return tmpCardstr

    def SwitchIndex(self,cardStr):
        actionCardS = []
        for i in range(0, 34):
            actionCardS.append(0)
        for i in range(0, len(cardStr)):
            actionCardS[int(cardStr[i])] += 1
        return actionCardS

    def WaitForOtherOperate(self,bGameOver):
        self.user_hand_cards_real, self.cbLaiZi = self.find_my_cards(
            self.MyHandCardsPos, self.bHaveAction)  # '2AKQQJJT99877753'
        cbActionMask = 0
        while ((len(self.user_hand_cards_real) % 3) != 2 and cbActionMask == 0) or bGameOver == True:
            cbActionMask = self.have_action(self.ActionBtnPos)
            self.sleep(300)
            self.user_hand_cards_real, self.user_hand_colors = self.find_my_cards(self.MyHandCardsPos, self.bHaveAction)
            result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos, confidence=0.75)
            if (result is not None):
                helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos, confidence=0.75)
                print("gameover-clear")
                self.m_WeaveItem = [[], [], [], []]
                self.allDisCardData = []
                bGameOver = False
            print("waitNex-start", len(self.user_hand_cards_real))
    def start(self):
        print("开始出牌\n")
        while not self.game_over:
            # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
            if self.play_order == 0:
                self.PredictedCard.setText("...")
                cbActionMask=self.have_action(self.ActionBtnPos)
                cbActionCard=""
                tmpLen=len(self.user_hand_cards_real)
                currentUser=0
                bEnable = False
                cardIndex = self.SwitchIndex(self.user_hand_cards_real)
                if (tmpLen%3)==2 :
                   cbActionCard=self.user_hand_cards_real[tmpLen-1]
                elif cbActionMask>0:
                    currentUser=1
                    img, _ = helper.Screenshot()
                    actionSearchPos = ()

                    if (cbActionMask & (WIK_CHI_XUAN)) != 0:
                        tmpPos = self.ActionBtnPosClick[3]
                        bEnable = False
                    elif (cbActionMask & (WIK_LEFT))!=0:
                        tmpPos = self.ActionBtnPosClick[3]
                        bEnable = False
                    elif (cbActionMask & (WIK_CENTER))!=0:
                        tmpPos = self.ActionBtnPosClick[3]
                        bEnable = False
                    elif (cbActionMask & (WIK_RIGHT))!=0:
                        tmpPos = self.ActionBtnPosClick[3]
                        bEnable = False
                    elif (cbActionMask & WIK_PENG)!=0:
                        tmpPos = self.ActionBtnPosClick[2]
                        actionSearchPos = (tmpPos[0] + 72, tmpPos[1]-7, tmpPos[2]+10, tmpPos[3]+10)
                        bEnable = False
                    elif (cbActionMask & WIK_GANG)!=0:
                        tmpPos = self.ActionBtnPosClick[1]
                        actionSearchPos = (tmpPos[0] + 72, tmpPos[1]-7, tmpPos[2]+10, tmpPos[3]+10)
                        bEnable = False

                    elif (cbActionMask & WIK_CHI_HU)!=0:
                        bEnable = True
                    if bEnable==False:
                        while cbActionCard=="":
                            tmpCount=2
                            if cbActionMask&WIK_GANG!=0:
                                tmpCount=3
                            elif cbActionMask&WIK_LEFT!=0:
                                tmpCount=1
                            bExist=False
                            for j in range(33,-1,-1,):
                                if cardIndex[j]>=tmpCount or ((cbActionMask & (WIK_CHI_XUAN|WIK_LEFT)) != 0):
                                    for i in range(0, 3):
                                        # result = pyautogui.locate(needleImage=helper.Pics["action_"+str(j)], haystackImage=img,
                                        #                           region=(160, 332, 95, 130),
                                        #                           confidence=0.70)
                                        tempConfidence=0.77
                                        if j==26 or j==33 or j==18:
                                            tempConfidence=0.67
                                        result = pyautogui.locate(needleImage=helper.Pics[str(j)], haystackImage=img,
                                                                   region=(160, 332, 95, 130),
                                                                   confidence=tempConfidence)
                                        if result is None:
                                            result = pyautogui.locate(needleImage=helper.Pics[str(j)],
                                                                      haystackImage=img,
                                                                      region=(919, 332, 95, 130),
                                                                      confidence=tempConfidence)
                                        if result is not None:
                                            cbActionCard=str(j)
                                            bExist=True
                                            break
                                if bExist:
                                    break
                            self.sleep(500)
                action_message=[]
                cbOperateCode=0
                if bEnable == False:
                    if (cbActionMask & (WIK_CHI_XUAN)) != 0:
                        posAction=[]
                        posChiArray=[]
                        #self.waiteChiAction=1
                        posCount=self.GetChiData(cardIndex,cbActionCard,posAction,posChiArray)
                        for i in range(0,len(posAction)):
                            if posAction[i]==self.waiteChiAction:
                                print("chi-leftClick:",(posChiArray[i][0] + 30, posChiArray[i][1] + 30))
                                helper.LeftClick((posChiArray[i][0] + 30, posChiArray[i][1] + 30))
                                break

                    else:
                        print(self.play_order, "\n.dllCall：", action_message,self.callCard,cbActionMask,cbActionCard,self.cbLaiZi)
                        action_message,cbOperateCode = self.dllCall(self.user_hand_cards_real,self.allDisCardData,0,currentUser,self.callCard,cbActionMask,cbActionCard,self.cbLaiZi)
                else:
                    cbOperateCode=WIK_CHI_HU
                tmpCardstr = self.GetStrOperateCode(cbOperateCode)
                outStr=""
                if len(action_message)>0:
                    outStr=str(AllCardOutStr[action_message[0]])
                self.PredictedCard.setText(tmpCardstr+outStr)
                self.WinRate.setText("评分：" + '0')
                print(self.play_order,"\nuser_hand_cards_real：", self.user_hand_cards_real)
                print(self.play_order,"\nturnCardReal：", action_message)
                print(self.play_order,"\nallDisCardData：", self.allDisCardData)
                print(self.play_order,"\n出牌：", tmpCardstr+ outStr)
                hand_cards_str = self.user_hand_cards_real
                self.bHaveAction=False
                bGameOver=False
                if len(action_message) == 0 or cbActionMask>0:
                    #helper.ClickOnImage("pass_btn", region=self.PassBtnPos)
                    self.sleep(100)
                    print(self.play_order,"action:",cbOperateCode)
                    if cbOperateCode==WIK_LEFT or cbOperateCode==WIK_CENTER or cbOperateCode==WIK_RIGHT:
                         helper.ClickOnImage("chi", region=tmpPos, confidence=0.75)
                         self.bHaveAction=True
                         print("bHaveAction-WIK_CHI:",self.bHaveAction)
                         posAction = []
                         posChiArray = []
                         posCount = self.GetChiData(cardIndex, cbActionCard, posAction, posChiArray)
                         if posCount==1:
                             item = WeaveItem()
                             item.cbWeaveKind = posAction[0]
                             item.cbCenterCard = cbActionCard
                             item.cbPublicCard = cbActionCard
                             item.wProvideUser = 1
                             self.m_WeaveItem[0].append(item)
                         else:
                             self.waiteChiAction = cbOperateCode
                    elif self.waiteChiAction!=0:
                        print("chi——xuan", tmpPos)
                        item = WeaveItem()
                        item.cbWeaveKind = self.waiteChiAction
                        item.cbCenterCard = cbActionCard
                        item.cbPublicCard = cbActionCard
                        item.wProvideUser = 1
                        self.m_WeaveItem[0].append(item)
                        self.waiteChiAction=0
                        print("bHaveAction-WIK_CHI_XUAN:", self.bHaveAction)
                    elif cbOperateCode==WIK_PENG:
                         print("peng", tmpPos)
                         helper.ClickOnImage("peng", region=tmpPos, confidence=0.75)
                         item = WeaveItem()
                         item.cbWeaveKind = WIK_PENG
                         item.cbCenterCard = cbActionCard
                         item.cbPublicCard = cbActionCard
                         item.wProvideUser = 1
                         self.m_WeaveItem[0].append(item)
                         #self.allDisCardData.append(action_message[0])
                         #self.allDisCardData.append(action_message[0])
                         self.bHaveAction=True
                         print("bHaveAction-WIK_PENG:",self.bHaveAction)
                    elif cbOperateCode == WIK_GANG:
                         print("gang",tmpPos)
                         helper.ClickOnImage("gang", region=tmpPos, confidence= 0.75)
                         item = WeaveItem()
                         item.cbWeaveKind = WIK_GANG
                         item.cbCenterCard = cbActionCard
                         item.cbPublicCard = cbActionCard
                         item.wProvideUser = 1
                         #self.allDisCardData.append(action_message[0])
                         gangCardCount=0
                         for j in range(0,len(self.user_hand_cards_real)):
                             if action_message[0]==self.user_hand_cards_real[j]:
                                 gangCardCount=gangCardCount+1
                         if gangCardCount>1:
                            item.wProvideUser = 0
                            #self.allDisCardData.append(action_message[0])
                            #self.allDisCardData.append(action_message[0])
                         self.m_WeaveItem[0].append(item)
                    elif cbOperateCode == WIK_CHI_HU:
                         tmpPos = self.ActionBtnPosClick[0]
                         helper.ClickOnImage("hu", region=tmpPos, confidence= 0.72)
                         bGameOver=True
                    else:
                         helper.ClickOnImage("guo", region=self.ActionBtnPosClick[5], confidence= 0.75)
                elif cbOperateCode==0:
                    if self.onlyTip :
                        a=4
                    else:
                        outCardStr = []
                        outCardStr.append(action_message[0])
                        #for i in range(0,2):
                        helper.SelectCardsEx(outCardStr, self.handCardCount[0])
                        self.sleep(1000)
                        # 更新界面
                        if (len(action_message) > 0):
                            self.allDisCardData.append(action_message[0])
                            self.DeleteCard(self.user_hand_cards_real, action_message)
                            print(self.play_order, "handcount0：", self.handCardCount[0])
                            print(self.play_order, "handcount1：", self.handCardCount[1])



                self.sleep(1000)
                self.WaitForOtherOperate(bGameOver)

                self.bHaveAction=False
                self.printHandData()
                #     self.sleep(50)
                #self.play_order = 2
                #self.sleep(200)
                # self.detect_start_btn()
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
    def GetChiData(self,cardIndex,cbActionCard,posAction,posChiArray):
        tempIndex = int(cbActionCard)
        posCount = 0
        chiPos = [(250, 518, 189, 101), (443, 518, 189, 101), (637, 518, 189, 101)]
        if cardIndex[tempIndex + 1] and cardIndex[tempIndex + 1] > 0 and cardIndex[tempIndex + 2] and cardIndex[
            tempIndex + 2] > 0:
            posAction.append(WIK_LEFT)
            posChiArray.append(chiPos[2 - posCount])
            posCount += 1
        if cardIndex[tempIndex + 1] and cardIndex[tempIndex + 1] > 0 and cardIndex[tempIndex - 1] and cardIndex[
            tempIndex - 1] > 0:
            posAction.append(WIK_CENTER)
            posChiArray.append(chiPos[2 - posCount])
            posCount += 1
        if cardIndex[tempIndex - 1] and cardIndex[tempIndex - 1] > 0 and cardIndex[tempIndex - 2] and cardIndex[
            tempIndex - 2] > 0:
            posAction.append(WIK_RIGHT)
            posChiArray.append(chiPos[2 - posCount])
            posCount += 1


        return posCount
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
                helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos, confidence=0.75)
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
    def changeDataOut(self, cards):
        # 转换外面传进来的数据

        tmpCard = [AllCardOut[c] for c in list(cards)]
        AllcardData=[]
        for i in range(0, len(tmpCard)):
            AllcardData.append(tmpCard[i])
        return AllcardData

    def changeDataIn(self, cards):
        # 转换外面传进来的数据
        AllCard = {0x01:'0' ,0x02:'1' ,0x03:'2' ,0x04:'3' ,0x05:'4' ,0x06:'5' ,
                   0x07:'6' ,0x08:'7' ,0x09:'8', 0x11:'9' ,0x12:'10' ,0x13:'11',
                   0x14: '12' ,0x15:'13' ,0x16:'14' ,0x17:'15' ,0x18:'16' ,0x19:'17' ,
                   0x21:'18' ,0x22:'19' ,0x23:'20' ,0x24:'21' ,0x25:'22' ,0x26:'23' ,
                   0x27:'24' ,0x28:'25' ,0x29:'26' ,0x31:'27' ,0x32:'28' ,0x33:'29' ,
                   0x34:'30' ,0x35:'31' ,0x36:'32' ,0x37:'33' }

        tmpCard = [AllCard[c] for c in list(cards)]
        tmpCardstr= []
        for i in range(0, len(tmpCard)):
            tmpCardstr.append(tmpCard[i])
        return tmpCardstr
    def dllCall(self,HandCardData,DiscardData,wMeChairId,wCurrentUser,cbCallCard,cbActionMask,cbActionCard,cblaiZi):

        class tagInPyhonNew(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* MAX_COUNT),
                        ("cbHandCardCount", c_ubyte),
                        ("DiscardCard", c_ubyte * 144),
                        ("cbDiscardCardCount", c_ubyte),
                        ("cbWeaveKind", c_ubyte * 16),
                        ("cbCenterCard", c_ubyte * 16),
                        ("cbPublicCard", c_ubyte * 16),
                        ("wProvideUser", c_ubyte * 16),
                        ("cbWeaveCount", c_ubyte * GAME_PLAYER),
                        ("cbCardDataEx", c_ubyte * 20),
                        ("wMeChairId", c_ubyte ),
                        ("wCurrentUser", c_ubyte),
                        ("cbActionMask", c_ubyte),
                        ("cbActionCard", c_ubyte),
                        ("cbOperateCode", c_ubyte),
                        ("cbResultCard", c_ubyte * MAX_COUNT)
                        ]
        pDll = CDLL("./testC++.dll")
        arg1 = tagInPyhonNew()
        #tmpHandCard=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x11,0x12,0x13,0x25,0x35,0x16,0x17]
        tmpHandCard=self.changeDataOut(HandCardData)
        tmpDiscard=self.changeDataOut(DiscardData)
        tmparray=c_ubyte * MAX_COUNT
        tmparray2 = c_ubyte * FULL_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbResultCard = tmparray()
        arg1.DiscardCard = tmparray2()
        tmpItem = c_ubyte * 16
        arg1.cbWeaveKind = tmpItem()
        arg1.cbCenterCard = tmpItem()
        arg1.cbPublicCard = tmpItem()
        arg1.wProvideUser = tmpItem()
        tmparray3 = c_ubyte *GAME_PLAYER
        arg1.cbWeaveCount = tmparray3()
        arg1.cbCardDataEx = tmparray()
        arg1.cbCardDataEx[0]=cbCallCard
        for i in range(0,len(cblaiZi)):
            arg1.cbCardDataEx[1+i] = AllCardOut[cblaiZi[i]]
        for i in range(0, GAME_PLAYER):
            arg1.cbWeaveCount[i]=len(self.m_WeaveItem[i])
            tmpLen=len(self.m_WeaveItem[i])
            for j in range(0, tmpLen):
                if self.m_WeaveItem[i][j]!=None:
                   arg1.cbWeaveKind[i*MAX_WEAVE+j] = self.m_WeaveItem[i][j].cbWeaveKind
                   arg1.cbCenterCard[i*MAX_WEAVE+j] = AllCardOut[self.m_WeaveItem[i][j].cbCenterCard]
                   arg1.cbPublicCard[i*MAX_WEAVE+j] = AllCardOut[self.m_WeaveItem[i][j].cbPublicCard]
                   arg1.wProvideUser[i*MAX_WEAVE+j] = self.m_WeaveItem[i][j].wProvideUser
            for j in range(tmpLen, MAX_WEAVE):
                arg1.cbWeaveKind[i * MAX_WEAVE + j] = 0
                arg1.cbCenterCard[i * MAX_WEAVE + j] = 0
                arg1.cbPublicCard[i * MAX_WEAVE + j] = 0
                arg1.wProvideUser[i * MAX_WEAVE + j] = 0

        for i in range(0,len(tmpHandCard)):
            arg1.cbHandCardData[i]=tmpHandCard[i]
        for i in range(0, len(tmpDiscard)):
            arg1.DiscardCard[i] = tmpDiscard[i]

        for i in range(len(tmpHandCard), MAX_COUNT):
            arg1.cbHandCardData[i] = 0
        for i in range(len(tmpDiscard), FULL_COUNT):
            arg1.DiscardCard[i] = 0
        for i in range(len(cblaiZi)+1, 20):
            arg1.cbCardDataEx[i] = 0
        arg1.cbHandCardCount = len(tmpHandCard)
        arg1.cbDiscardCardCount = len(tmpDiscard)
        arg1.wMeChairId=wMeChairId
        arg1.wCurrentUser = wCurrentUser
        arg1.cbActionMask = cbActionMask
        arg1.cbActionCard = AllCardOut[cbActionCard]
        arg1.cbOperateCode = 0

        func=pDll.fntestPython2
        func.restype = c_int32
        func.argtypes = [POINTER(tagInPyhonNew)]
        result = func(byref(arg1))
        print("action:")
        returnCardData=[]
        for i in range(0,1):
            if arg1.cbResultCard[i]>0:
                returnCardData.append(arg1.cbResultCard[i])
                print(arg1.cbResultCard[i])

        return self.changeDataIn(returnCardData), arg1.cbOperateCode
    def find_my_cards(self, pos,bHaveAction=False):
        user_hand_cards_real = []
        img, _ = helper.Screenshot()
        cards, states = helper.GetCards(img,bHaveAction)
        for c in cards:
            user_hand_cards_real.append(c[0])
        return user_hand_cards_real,states

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

    def have_action(self, pos):  # 是否有白块
        img, _ = helper.Screenshot()
        actionStr=["hu","gang","peng","chi","chi2","guo"]
        actionCode=[WIK_CHI_HU,WIK_GANG,WIK_PENG,WIK_LEFT,WIK_CHI_XUAN,WIK_NULL]
        action=WIK_NULL
        self.ActionBtnPosClick=[]
        for i in range(0, len(actionCode)):
            result = pyautogui.locate(needleImage=helper.Pics[actionStr[i]], haystackImage=img,
                                  region=pos, confidence=0.85)
            if result is not None:
                if i<(len(actionStr)-1):
                    action|=actionCode[i]
                self.ActionBtnPosClick.append((result.left,result.top,result.width,result.height))
            else:
                self.ActionBtnPosClick.append((0,0,0,0))
        return  action

    def haveActionCard(self,pos):  # 是否有白块

        actionStr = ["huang0","huang1", "huang2", "huang3"]
        action = WIK_NULL
        ActionPos = []
        bWhile=True

        while bWhile:
            img, _ = helper.Screenshot()
            for i in range(0, len(actionStr)):
                result = pyautogui.locate(needleImage=helper.Pics[actionStr[i]], haystackImage=img,
                                          region=pos, confidence=0.80)
                if result is not None:
                    ActionPos=(result.left, result.top, result.width, result.height)
                    bWhile=False
                    break
            self.sleep(100)
        return ActionPos

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
