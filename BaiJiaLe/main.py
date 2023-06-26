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
import random
from douzero.env.game import GameEnv
from douzero.evaluation.deep_agent import DeepAgent
import traceback
import math
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
AREA_COUNT=5
BTN_COUNT=5
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
        random.seed(10)
        self.queryPos = (100, 430, 380, 320)
        self.queryResultPos = (160, 430, 180, 130)
        self.queryConfidence = 0.83
        self.textEdit_base.setText("1")
        self.textEdit_beitou.setText("3")
        self.textEdit_zhisuan.setText("500")
        self.textEdit_zhiying.setText("1000")
        self.textEdit_benjin.setText("1000")

        self.textEdit_btnV0.setText("1")
        self.textEdit_btnV1.setText("5")
        self.textEdit_btnV2.setText("10")
        self.textEdit_btnV3.setText("50")
        self.textEdit_btnV4.setText("100")
        self.textEdit_duokai.setText("0")
        self.textEdit_xiazhuStyle.setText("0")
        self.xiaZhuQuYUPos = [(199, 461), (343, 461), (267, 541), (207, 685), (339, 685)]
        self.xiaZhuBtnPos = [(124, 818), (197, 818), (278, 818), (352, 818), (432, 818)]
        self.xiaZhuQuBtnVule = [1, 5, 10, 50, 100]
        self.xiaZhuQuVule = [0, 0,0, 0,0]
        self.m_lunNum=0
        self.pushBet=0
        self.totalBet=0
        self.m_xiazhuCount=0
        self.m_xiazhuIndex=0
    def init_display(self):
        self.InitCard.setText("开始")
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")

        self.m_base = int(self.textEdit_base.toPlainText())
        self.m_beiTou = int(self.textEdit_beitou.toPlainText())
        self.m_zhisuan = int(self.textEdit_zhisuan.toPlainText())
        self.m_zhiying = int(self.textEdit_zhiying.toPlainText())
        self.m_benjin = int(self.textEdit_benjin.toPlainText())
        self.m_duokai = (self.textEdit_duokai.toPlainText())
        self.m_xiaZhuStyle = int(self.textEdit_xiazhuStyle.toPlainText())
        if self.m_xiaZhuStyle>2 or self.m_xiaZhuStyle<0 :
            self.m_xiaZhuStyle=0
        helper.setFindStr(self.m_duokai)
        self.xiaZhuQuBtnVule[0] = int(self.textEdit_btnV0.toPlainText())
        self.xiaZhuQuBtnVule[1] = int(self.textEdit_btnV1.toPlainText())
        self.xiaZhuQuBtnVule[2] = int(self.textEdit_btnV2.toPlainText())
        self.xiaZhuQuBtnVule[3] = int(self.textEdit_btnV3.toPlainText())
        self.xiaZhuQuBtnVule[4] = int(self.textEdit_btnV4.toPlainText())
        self.m_pushBet=self.m_base
    def switch_mode(self):
        self.AutoPlay = not self.AutoPlay
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")

    def init_cards(self):
        self.RunGame = True
        GameHelper.Interrupt = False
        self.init_display()
        #helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos)
        self.env = None
        self.game_over= False
        helper.bTest=False
        if helper.bTest==True:
           xwin = helper.LocateOnScreen("xwin", region=self.queryResultPos, confidence=self.queryConfidence)
           tmp=4

        #helper.LeftClick((199, 461))

        try:
            self.start()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            self.stop()
    def push(self,value,pos):
        print("push",value,pos)
        tmpV=value
        while tmpV>0:
            for i in reversed(range(0,BTN_COUNT )):
                if tmpV>=self.xiaZhuQuBtnVule[i]:
                    helper.LeftClick(self.xiaZhuBtnPos[i])
                    print("push-click1", self.xiaZhuBtnPos[i], i)
                    self.sleep(500)
                    helper.LeftClick(pos)
                    self.sleep(500)
                    print("push-click2", pos)
                    tmpV=tmpV-self.xiaZhuQuBtnVule[i]
                    break
            self.sleep(100)

    def start(self):
        print("开始工作\n")
        while not self.game_over:
            self.xiaZhuQuVule = [0, 0, 0, 0, 0]
            xia1 = helper.LocateOnScreen("xiazhu0", region=self.queryPos, confidence=0.85)
            xia2 = helper.LocateOnScreen("xiazhu1", region=self.queryPos, confidence=0.85)
            xia3 = helper.LocateOnScreen("xiazhu2", region=self.queryPos, confidence=0.85)
            per=random.randint(1, 100)
            #posIndex = random.randint(3, 4)
            posIndex = self.m_xiazhuIndex + 3
            tmpXiaZhu=[(1,1),(2,2),(3,3)]
            self.m_xiazhuCount = self.m_xiazhuCount + 1
            if self.m_xiazhuCount>=tmpXiaZhu[self.m_xiaZhuStyle][self.m_xiazhuIndex] :
                self.m_xiazhuCount=0
                if self.m_xiazhuIndex==0:
                    self.m_xiazhuIndex = 1
                else:
                    self.m_xiazhuIndex = 0

            print("m_xiazhuIndex.....",self.m_xiazhuIndex,self.m_xiazhuCount)
            while xia1 is None and xia2 is None and xia3 is None:
                xia1 = helper.LocateOnScreen("xiazhu0", region=self.queryPos, confidence=0.85)
                xia2 = helper.LocateOnScreen("xiazhu1", region=self.queryPos, confidence=0.85)
                xia3 = helper.LocateOnScreen("xiazhu2", region=self.queryPos, confidence=0.85)
                self.sleep(20)
                if self.game_over:
                    break
                print("等待下注.....")
            self.push(self.m_pushBet, self.xiaZhuQuYUPos[posIndex])
            self.xiaZhuQuVule[posIndex]=self.m_pushBet
            tmpStr = ["闲对","庄对","和", "闲", "庄"]
            retsultStr ="此轮下注：" + str(self.m_pushBet)+"下注区域："+tmpStr[posIndex]
            print(retsultStr)
            self.label_tip1.setText(retsultStr)
            zwin= helper.LocateOnScreen("zwin", region=self.queryResultPos, confidence=self.queryConfidence )
            xwin = helper.LocateOnScreen("xwin", region=self.queryResultPos, confidence=self.queryConfidence )
            he = helper.LocateOnScreen("he", region=self.queryResultPos, confidence=self.queryConfidence )
            while zwin is None and xwin is None and he is None:
                zwin = helper.LocateOnScreen("zwin", region=self.queryResultPos, confidence=self.queryConfidence )
                xwin = helper.LocateOnScreen("xwin", region=self.queryResultPos, confidence=self.queryConfidence )
                he = helper.LocateOnScreen("he", region=self.queryResultPos, confidence=self.queryConfidence )
                print("等待结果.....")
                if self.game_over:
                    break
                self.sleep(10)
            bWin=-1
            if self.xiaZhuQuVule[2]>0and he:
                bWin=0
                self.totalBet = self.totalBet + self.m_pushBet*8
            elif self.xiaZhuQuVule[3]>0and xwin:
                bWin = 1
                self.totalBet = self.totalBet + self.m_pushBet
            elif self.xiaZhuQuVule[4]>0 and zwin:
                bWin = 2
                self.totalBet = self.totalBet + self.m_pushBet * 0.95
            else:
                if he is None:
                    self.totalBet=self.totalBet-self.m_pushBet
            resultIndex=0
            if xwin:
                resultIndex=1
            elif zwin:
                resultIndex=2
            tmpStr=["和","闲赢","庄赢"]
            retsultStr="此轮下注："+str(self.m_pushBet)+"结果："+tmpStr[resultIndex]
            self.label_tip1.setText(retsultStr)
            print(retsultStr)
            if he is None:
                if bWin>=0:
                    self.m_pushBet=self.m_base
                    self.m_lunNum=0
                else:
                    self.m_lunNum=self.m_lunNum+1
                    if self.m_lunNum>self.m_beiTou:
                        self.m_lunNum=0
                    self.m_pushBet = self.m_base* math.pow(2, self.m_lunNum )

            retsultStr = "当前倍投次数：" + str(self.m_lunNum) + "赢输：" + str(round(self.totalBet, 2))
            print(retsultStr)
            self.label_tip2.setText(retsultStr)
            if int(self.totalBet)>=self.m_zhiying:
                print("止赢")
                break
            if int(self.totalBet)<=(-self.m_zhisuan):
                print("止损")
                break
            self.sleep(5000)


        print("{}胜，本局结束!\n")
    def stop(self):
        try:
            self.RunGame = False
            self.game_over = True
            self.AutoPlay=False
            self.m_lunNum=0
            self.totalBet=0
            self.init_display()
        except AttributeError as e:
            pass
        if self.AutoPlay:
            self.initStart()
    def sleep(self, ms):
        self.counter.restart()
        while self.counter.elapsed() < ms:
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 50)

    def DeleteCard(self, src, sub):
        tmpstr = src
        for i in range(0, len(sub)):
            for j in range(0, len(src)):
                if sub[i] == src[j]:
                    tmpstr = tmpstr.replace(sub[i], '', 1)
                    break
        return tmpstr

    def isGameOver(self):
        return self.handCardCount[0] == 0 or self.handCardCount[2] == 0
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
