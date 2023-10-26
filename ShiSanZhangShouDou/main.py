# -*- coding: utf-8 -*-
# Created by: Raf
# Modify by: Vincentzyx

import GameHelper as gh
from GameHelper import GameHelper
import socketTool
import os
import sys
import pyautogui
import win32gui
import cv2
import numpy as np
from PyQt5 import QtGui, QtWidgets, QtCore
from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPixmapItem, QInputDialog, \
    QMessageBox, QLabel
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import QTime, QEventLoop
from MainWindow import Ui_Form
import traceback
from ctypes import *
import json
from client import Client
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
AllCardList=[
  0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,# //方块 A - K
  0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, #//梅花 A - K
  0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,# //红桃 A - K
  0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, # //黑桃 A - K
  #0x4e,0x4f
]
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
        self.connected = False
        self.counter = QTime()
        self.qianCurrentIndex = 0
        self.houCurrentIndex = -1
        # 参数
        self.initQianPoker()
        self.initHouPoker()

        #self.initMPlayedCard()
        #carddata=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D]
        #self.ShowPlayerCardEx(2,carddata)

        self.readJson()
        self.fenFaQi = Client()
        self.fenFaQi.SetMsgCall(self.RecvPlayerMsg)
        self.fenFaQi.start()
    def RecvPlayerMsg(self, wSubCmdID, data):
        if wSubCmdID == socketTool.SUB_C_SEND_CARD:
            dataBuffer = socketTool.cmd_cardDataInfo.from_buffer(data)
            tmpStr = '座位号:' + str(dataBuffer.wChairID)
            print("游戏消息1:", tmpStr)
            tmpStr = ''
            for i in range(0, dataBuffer.cbCardCount):
                tmpStr = tmpStr + str(dataBuffer.cbCardData[i]) + ','
            print("游戏消息2:", tmpStr)
            tmpStr = ''
            for i in range(0, 39):
                tmpStr = tmpStr + str(dataBuffer.cbCardDataEx[i]) + ','
            print("游戏消息3:", tmpStr)
        elif wSubCmdID == -1:
            print("close")
            self.connected = False
            return
        elif wSubCmdID == socketTool.SUB_C_RESET_TABLE:
            dataBuffer = socketTool.cmd_reSetTable.from_buffer(data)
            tmpStr = '座位号:' + str(dataBuffer.wChairID)
            print("重置桌子状态:", tmpStr)
            return
        elif wSubCmdID == socketTool.SUB_GR_USER_SIT_SUCCESS:
            if data.wChairID == self.fenFaQi.GetChaiID():
                print("连接成功")
                self.connected = True
        else:
            print("其他无关消息")

        a = 4
    def switchPushIndex(self,data):
        for i in range(0, len(AllCardList)):
            if AllCardList[i]==data:
                return i

        return -1
    def clearUI(self):
        self.houCurrentIndex = -1
        for i in range(0, MAX_CARD_COUNT):
            self.qianPushData[i] = 0
            self.pokerQianPush[i].setStyleSheet("QPushButton{border-image: url(./pics/0x00.png)}")
        for i in range(0, len(AllCardList)):
            self.pokerHouPush[i].setVisible(True)
            self.pokerHouPush[i].setStyleSheet("QPushButton{border-image: url(./pics/" + str(AllCardList[i]) + ".png)}")
    def initHouPoker(self):
        self.houCurrentIndex = -1
        spaceX = 10
        spaceY = 530
        self.pokerHouPush = []
        for i in range(0, len(AllCardList)):
            self.pokerHouPush.append(QtWidgets.QPushButton(self))
            self.pokerHouPush[i].setGeometry(QtCore.QRect(0, 360, 127, 175))
            self.pokerHouPush[i].setStyleSheet("QPushButton{border-image: url(./pics/" + str(AllCardList[i]) + ".png)}")
            #self.pokerHouPush[i].setStyleSheet("background-color: white;")
            self.pokerHouPush[i].move(spaceX, spaceY)
            self.pokerHouPush[i].clicked.connect(lambda checked, arg=i: self.HouBtOnEvent(arg))
            #btn.clicked.connect(lambda checked, arg=filepath: self.launch(arg))
            spaceX += 127
            if (i+1) % 13 == 0:
                spaceX = 10
                spaceY += 200
    def HouBtOnEvent(self,index):
        self.houCurrentIndex=index
        for i in range(0, MAX_CARD_COUNT):
            if self.qianPushData[i] > 0:
                continue
            self.pokerHouPush[self.houCurrentIndex].setVisible(False)
            self.pokerQianPush[i].setStyleSheet(
                "QPushButton{border-image: url(./pics/" + str(AllCardList[self.houCurrentIndex]) + ".png)}")
            self.qianPushData[i] = AllCardList[self.houCurrentIndex]
            self.houCurrentIndex = -1
            self.qianCurrentIndex = i
            break
        #self.pokerHouPush[index].setStyleSheet("background-color: transparent;")
        #self.pokerHouPush[index].setStyleSheet("background-color: gray;")
        intdex=0
    def QianBtOnEvent(self,index):
        self.qianCurrentIndex = index
        if self.qianPushData[index]>0:
            PushIndex=self.switchPushIndex(self.qianPushData[index])
            self.pokerHouPush[PushIndex].setVisible(True)
            #self.pokerHouPush[PushIndex].setStyleSheet("background-color: white;")
            self.pokerQianPush[index].setStyleSheet("QPushButton{border-image: url(./pics/0x00.png)}")
            self.qianPushData[index]=0
            self.houCurrentIndex=PushIndex
        else:
            if self.houCurrentIndex !=-1:
                self.pokerHouPush[self.houCurrentIndex].setVisible(False)
                self.pokerQianPush[index].setStyleSheet("QPushButton{border-image: url(./pics/" + str(AllCardList[self.houCurrentIndex]) + ".png)}")
                self.qianPushData[index] = AllCardList[self.houCurrentIndex]
                self.houCurrentIndex=-1
        index=0
    def initQianPoker(self):
        spaceX = 300
        spaceY = 0
        self.pokerQianPush = []
        self.qianPushData=[]
        for i in range(0, MAX_CARD_COUNT):
            self.qianPushData.append(0)
            self.pokerQianPush.append(QtWidgets.QPushButton(self))
            self.pokerQianPush[i].setGeometry(QtCore.QRect(0, 360, 127, 175))
            self.pokerQianPush[i].setStyleSheet("QPushButton{border-image: url(./pics/0x00.png)}")
            self.pokerQianPush[i].move(spaceX, spaceY)
            self.pokerQianPush[i].clicked.connect(lambda checked, arg=i: self.QianBtOnEvent(arg))
            spaceX += 127
            if  i == 2 or i == 7 :
                spaceX = 300
                spaceY += 175

    def gameStart(self):
        if self.connected==False:
            print("你的账号没有登陆，请联系Q：460000713，进行购买")
            self.sleep(1000)
            return
        print("开始出牌\n")
        count=0
        for i in range(0, len(self.qianPushData)):
            if self.qianPushData[i]>0:
                count+=1
        if count!=MAX_CARD_COUNT:
            print("当前手牌不够13张")
            return
        self.turnCardReal, self.turnCard_colors = self.changeDataIn(self.qianPushData)
        action_message, colors, duoZhongBaiFa, returnCardData = self.dllCall(self.turnCardReal, self.turnCard_colors,
                                                                             '',
                                                                             self.allDisCardData, self.bHavePass)
        for i in range(0, MAX_CARD_COUNT):
            self.pokerQianPush[i].setStyleSheet(
                "QPushButton{border-image: url(./pics/" + str(returnCardData[i]) + ".png)}")
            self.qianPushData[i]=returnCardData[i]
        print("{}胜，本局结束!\n")
        # QMessageBox.information(self, "本局结束", "{}胜！".format("农民" if self.env.winner == "farmer" else "地主"),
        #                         QMessageBox.Yes, QMessageBox.Yes)
        # self.detect_start_btn()

    def init_display(self):
        #self.WinRate.setText("评分")
        a=4
    def readJson(self):
        with open("data_file.json", "r") as read_file:
            self.configData = json.load(read_file)
        self.userid = self.configData["userid"]
        self.tableid=self.configData["tableid"]
        self.port=self.configData["port"]
        self.m_duokai=str(self.configData["duokai"])
        self.sandayi=self.configData["sandayi"]
        self.setWindowTitle(self.m_duokai+'号机')
        helper.setFindStr(self.m_duokai)
    def isInMyHandata(self,carddata):
        for i in range(0,len(self.otherPlayerData[0])):
            if carddata==self.otherPlayerData[0][i]:
                return  True
        return  False


    def IsSameCard(self):
        tmpCardstr = ""
        for i in range(0, len(self.user_hand_cards_real)):
            tmpCardstr += COLOR_LIST[self.user_hand_colors[i]] + self.user_hand_cards_real[i]
        for i in range(0, len(self.user_hand_cards_real)-1):
            ci = i+1
            while ci < len(self.user_hand_cards_real):
                if self.user_hand_cards_real[i]==self.user_hand_cards_real[ci] and self.user_hand_colors[i]==self.user_hand_colors[ci]:
                    return True
                ci += 1
        return False
    def gameInit(self):
        self.RunGame = True
        GameHelper.Interrupt = False
        self.bReSortCard=False
        self.init_display()
        # 玩家手牌
        self.user_hand_cards_real = ""
        self.user_hand_colors = []
        self.turnCardReal = ''
        self.turnCard_colors = []
        self.allDisCardData = ''
        self.bSanDayiStart=False
        self.other_played_cards_real = ''

        self.handCardCount = [MAX_CARD_COUNT, MAX_CARD_COUNT, MAX_CARD_COUNT]
        self.bHavePass = False
        self.env = None
        self.game_over = False

        helper.bTest = False
        # 识别玩家手牌
        testCount = 0
        # 得到出牌顺序
        self.play_order = 0  # self.find_landlord()

        return  True
    def init_cards(self):
        self.game_over=False
        self.gameInit()
        try:
            self.gameStart()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            print("游戏出现异常请重新开始")
            self.sleep(2000)
            #self.init_cards()

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
        result = helper.LocateOnScreen("go_btn", region=self.OutCardBtnPos, confidence=0.75)
        while result is None:
            if self.RunGame == False:
                break
            print("等待游戏结束")
            result = helper.LocateOnScreen("go_btn", region=self.OutCardBtnPos, confidence=0.75)
            self.sleep(1000)
        return True
    def find_landlord(self):
            result = helper.LocateOnScreen("play_card", region=self.FirstOutCardBtnPos,
                                           confidence=self.LandlordFlagConfidence)
            if result is not None:
                return 0
            return 2

    def detect_start_btn(self):
        if  self.isGameOver():
            print("点击切换对手")
            #self.stop()
            if self.AutoPlay:
                self.sleep(100)
                #self.initStart()
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
        #self.ThreeLandlordCards.setText(tmpCardstr+'\n'+tmpCardCountStr)
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

    def changeDataIn(self, cardsData):
        cards=cardsData.copy()
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
        tmpTurnCard=self.changeDataOut(TurnCardData,self.turnCard_colors)
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
        tmpvale1,vmpvale2=self.changeDataIn(returnCardData)
        return tmpvale1,vmpvale2,arg1.cbOthreRangCardCount,returnCardData
    def find_my_cards(self, pos,dence=0.95):
        user_hand_cards_real = ""
        img, _ = helper.Screenshot()
        cards, states,colors = helper.GetCards(img,self.handCardCount[0],dence)
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
            #self.env.reset()
            self.init_display()
            self.PreWinrate.setText("局前预估胜率：")
            self.BidWinrate.setText("")
        except AttributeError as e:
            pass

    def beforeStart(self):
        self.onlyTip = not self.onlyTip
        self.AutoStart.setText("只提示" if self.onlyTip else "代打")

    def AutoFastEnalbe(self):
        self.bFastEnable = not self.bFastEnable
        self.AutoFast.setText("快速开启" if self.bFastEnable else "快速关闭")
    def initStart(self):
        self.RunGame = True
        self.yaoBuQi=True
        GameHelper.Interrupt = True
        self.sleep(1000)
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
