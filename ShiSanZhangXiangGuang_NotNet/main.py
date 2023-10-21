# -*- coding: utf-8 -*-
# Created by: Raf
# Modify by: Vincentzyx

import GameHelper as gh
from GameHelper import GameHelper
import socketTool
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
from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPixmapItem, QInputDialog, \
    QMessageBox, QLabel
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import QTime, QEventLoop
from MainWindow import Ui_Form
from client import Client
from douzero.env.game import GameEnv
from douzero.evaluation.deep_agent import DeepAgent
import traceback
import random
import time
import pygame
import os
import BidModel
import LandlordModel
import FarmerModel
from ctypes import *
import json
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
        self.Players = [self.RPlayer, self.Player, self.LPlayer]
        self.counter = QTime()
        pygame.mixer.init()

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
        self.bFastEnable = False
        self.bReSortCard=False
        self.turnCardReal = ''
        self.turnCard_colors = []
        self.allDisCardData = ''
        self.bSanDayiStart = False
        self.bHaveShiBie=False
        self.bHavePass=False
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
        self.OutCardBtnPos = (234, 924, 200, 60)
        self.FirstOutCardBtnPos = (460, 320, 200, 150)
        self.changePlayerBtnPos = (353, 671, 160, 70)
        self.zhengChangBiPaiBtnPos = (96, 660, 160, 70)
        self.tipBtnPos = (454, 321, 200, 100)
        # 信号量
        self.shouldExit = 0  # 通知上一轮记牌结束
        self.canRecord = threading.Lock()  # 开始记牌

        self.readJson()
        self.fenFaQi = Client()
        self.fenFaQi.SetMsgCall(self.RecvPlayerMsg)
        self.otherPlayerData = [[],[],[]]
        self.otherPlayerText = [self.LPlayedCard, self.RPlayedCard, self.MPlayedCard]
        # self.sleep(100)
        # self.startGameEx()
        self.beforeStart()
        self.switch_mode()
        self.startgame.setVisible(False)
        self.lock = threading.Lock()
        self.connected = False
        self.startLoginGame()
        self.detect_image=[]
        self.initMPlayedCard()
        #carddata=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D]
        #self.ShowPlayerCardEx(2,carddata)
    def init_display(self):
        #self.WinRate.setText("评分")
        self.InitCard.setText("开始")
        self.UserHandCards.setText("手牌")
        self.LPlayedCard.setText("上家出牌区域")
        self.RPlayedCard.setText("下家出牌区域")
        self.PredictedCard.setText("AI出牌区域")
        #self.ThreeLandlordCards.setText("十三张")
        self.SwitchMode.setText("自动下一局" if self.AutoPlay else "单局")
        for player in self.Players:
            player.setStyleSheet('background-color: rgba(255, 0, 0, 0);')
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
    def handCardMsgHelpEx(self,data):
        #if data.wChairID!=self.fenFaQi.GetChaiID():
        if True:
            tmpdata = []
            self.lock.acquire()
            if len(self.otherPlayerData) >1:
                self.otherPlayerData.pop()
            if len(self.otherPlayerData) >1:
                self.otherPlayerData.pop()
            for i in range(0, 39):
                if data.cbCardDataEx[i]>0 and self.isInMyHandata(data.cbCardDataEx[i])==False:
                    tmpdata.append(data.cbCardDataEx[i])
                if len(tmpdata)==13:
                    self.ShowPlayerCard(len(self.otherPlayerData) - 1, tmpdata)
                    self.otherPlayerData.append(tmpdata)
                    tmpdata = []

            if len(self.otherPlayerData[0])>0 and len(self.otherPlayerData)==3:
                tmpDict = dict.fromkeys(AllCardList, 1)
                for i in range(0, len(self.otherPlayerData)):
                    for j in range(0, len(self.otherPlayerData[i])):
                        if self.otherPlayerData[i][j]==15:
                            a=4
                        tmpDict[self.otherPlayerData[i][j]] -= 1
                tmpHandData = []
                for i in range(0, len(AllCardList)):
                    if tmpDict[AllCardList[i]] == 1:
                        tmpHandData.append(AllCardList[i])
                        if len(tmpHandData) >= 13:
                            break
                self.ShowPlayerCardEx(len(self.otherPlayerData)-1, tmpHandData)
                tmpStr = '最后一家的牌：'
                for i in range(0, len(tmpHandData)):
                    tmpStr = tmpStr + str(tmpHandData[i]) + ','
                print(tmpStr)
                self.bSanDayiStart = True
            self.lock.release()
    def RecvPlayerMsg(self,wSubCmdID, data):
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
            self.handCardMsgHelpEx(dataBuffer)
        elif wSubCmdID == -1:
            print("close")
            self.connected=False
            return
        elif wSubCmdID == socketTool.SUB_C_RESET_TABLE:
            dataBuffer = socketTool.cmd_reSetTable.from_buffer(data)
            tmpStr = '座位号:' + str(dataBuffer.wChairID)
            print("重置桌子状态:", tmpStr)
            return
        elif wSubCmdID==socketTool.SUB_GR_USER_SIT_SUCCESS:
            if data.wChairID==self.fenFaQi.GetChaiID():
                self.connected = True
        else:
            print("其他无关消息")

        a = 4
    def ShowPlayerCard(self,id,data):
        print("ShowPlayerCard",id)
        if id>=0 and id<5 :
            self.otherPlayerText[id].setText("...")
            tempData=data.copy()
            self.turnCardReal, self.turnCard_colors = self.changeDataIn(tempData)
            action_message, colors,duoZhongBaiFa,returnCardData = self.dllCall(self.turnCardReal, self.turnCard_colors, '',
                                                  self.allDisCardData, self.bHavePass)
            tmpCardstr = ""
            for i in range(0, len(action_message)):
                tmpCardstr += COLOR_LIST[colors[i]] + action_message[i]
                if i == 2 or i == 7:
                    tmpCardstr += "\n "
            self.otherPlayerText[id].setText(tmpCardstr if (len(tmpCardstr) > 0) else "算法异常")
    def ShowPlayerCardEx(self,id,data):
        print("ShowPlayerCard",id)
        if id>=0 and id<5 :
            self.otherPlayerText[id].setText("...")
            tempData=data.copy()
            self.turnCardReal, self.turnCard_colors = self.changeDataIn(tempData)
            action_message, colors,duoZhongBaiFa,returnCardData = self.dllCall(self.turnCardReal, self.turnCard_colors, '',
                                                  self.allDisCardData, self.bHavePass)
            spaceX=20
            spaceY=20
            for i in range(0, len(action_message)):
                spaceX += 50
                self.show_img(returnCardData[i],i)
                if i == 2 or i == 7:
                    spaceX=20
                    spaceY+=50
            #self.MPlayedCard.deleteLater()

    def initMPlayedCard(self):
        spaceX = 20
        spaceY = 20
        for i in range(0, 13):
            spaceX += 50
            self.detect_image.append(QLabel(self.MPlayedCard))
            self.detect_image[i].resize(100, 100)
            self.detect_image[i].move(spaceX, spaceY)
            if i == 2 or i == 7:
                spaceX = 20
                spaceY += 50
    def show_img(self,data,index):
        pix = QPixmap("./pics/"+str(data)+".png")  # 需额外添加"\\"否则输出为C:\123\picture093056.jpg
        self.detect_image[index].setPixmap(pix)
        self.detect_image[index].setScaledContents(True)

    def startLoginGame(self):
        self.fenFaQi.start()
    def reSetTableStatus(self):
        self.fenFaQi.ReSetTable()
    def startGameEx(self):
        self.fenFaQi.userReady()
    def switch_mode(self):
        self.AutoPlay = not self.AutoPlay
        self.SwitchMode.setText("自动下一局" if self.AutoPlay else "单局")
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
    def OhterSearchCard(self):
        for j in range(2, 4):
            helper.setFindStr(str(j))
            print("OhterSearchCard",j)
            testCount = 0
            other_played_cards_real,other_hand_colors = self.find_my_cards(self.MyHandCardsPos)  # '2AKQQJJT99877753'
            while len(other_hand_colors) != MAX_CARD_COUNT or len(
                    other_played_cards_real) != MAX_CARD_COUNT or self.IsSameCard():
                testCount += 1
                dence = 0.95
                if testCount >= 2:
                    dence = 0.94
                other_played_cards_real, other_hand_colors = self.find_my_cards(self.MyHandCardsPos, dence)
                self.sleep(1000)
                print("OhterSearchCard-ShiBieWenTi", len(other_played_cards_real))
            action_message, colors, duoZhongBaiFa, returnCardData = self.dllCall(other_played_cards_real,
                                                                                 other_hand_colors, '',
                                                                                 self.allDisCardData, self.bHavePass)
            self.otherPlayerData[j-1] = self.changeDataOut(other_played_cards_real, other_hand_colors)
            tmpCardstr = ""
            for i in range(0, len(action_message)):
                tmpCardstr += COLOR_LIST[colors[i]] + action_message[i]
                if i == 2 or i == 7:
                    tmpCardstr += "\n "
            self.otherPlayerText[j-2].setText(tmpCardstr if (len(tmpCardstr) > 0) else "算法异常")
        #处理剩下牌
        if len(self.otherPlayerData[0]) > 0 and len(self.otherPlayerData) == 3:
            tmpDict = dict.fromkeys(AllCardList, 1)
            for i in range(0, len(self.otherPlayerData)):
                for j in range(0, len(self.otherPlayerData[i])):
                    if self.otherPlayerData[i][j] == 15:
                        a = 4
                    tmpDict[self.otherPlayerData[i][j]] -= 1
            tmpHandData = []
            for i in range(0, len(AllCardList)):
                if tmpDict[AllCardList[i]] == 1:
                    tmpHandData.append(AllCardList[i])
                    if len(tmpHandData) >= 13:
                        break
            self.ShowPlayerCardEx(len(self.otherPlayerData) - 1, tmpHandData)
            tmpStr = '最后一家的牌：'
            for i in range(0, len(tmpHandData)):
                tmpStr = tmpStr + str(tmpHandData[i]) + ','
            print(tmpStr)
            self.bSanDayiStart = True
        helper.setFindStr(str(1))
    def gameInit(self):
        self.InitCard.setEnabled(False)
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
        # if self.connected==False:
        #     print("你的账号没有登陆，请联系Q：460000713，进行购买")
        #     self.sleep(1000)
        #     return
        self.game_over = False
        self.shengYuPaiShow(self.allDisCardData)

        helper.bTest = False
        # 识别玩家手牌
        # temp=self.have_white(self.RPlayedCardsPos)
        # self.turnCardReal = self.find_other_cards(self.RPlayedCardsPos)
        # tmpHandCard = self.changeDataOut('2AKQQJJT99877753')
        # tmpHandCardStr = self.changeDataIn(tmpHandCard)
        # if self.bReSortCard==False :
        #     image, windowPos = helper.Screenshot()
        #     handCardsInfo, states, _ = helper.GetCards(image, MAX_CARD_COUNT,0.94)
        #     testCount = 0
        #     while (len(handCardsInfo) > 0):
        #         randnum1 = random.randint(0, len(handCardsInfo) - 1)
        #         randnum2 = random.randint(0, len(handCardsInfo) - 1)
        #         randnum = random.randint(1, 3)
        #         print("随机切换牌:", )
        #         if randnum1 != randnum2:
        #             helper.LeftClickEX(handCardsInfo[randnum1][1], handCardsInfo[randnum2][1])
        #         self.sleep(500 * randnum)
        #         testCount += 1
        #         if testCount >= 3:
        #             break
        testCount = 0
        self.user_hand_cards_real, self.user_hand_colors = self.find_my_cards(self.MyHandCardsPos)  # '2AKQQJJT99877753'
        while len(self.user_hand_colors) != MAX_CARD_COUNT or len(self.user_hand_cards_real) != MAX_CARD_COUNT or self.IsSameCard():
            testCount += 1
            dence=0.95
            if testCount>=2:
                dence=0.94
            self.user_hand_cards_real, self.user_hand_colors = self.find_my_cards(self.MyHandCardsPos,dence)
            self.sleep(1000)
            print("hangCountShiBieWenTi", len(self.user_hand_cards_real))
        # self.user_hand_cards_real =  'AKKKKQJJT9988'
        # self.user_hand_colors=[0,1,2,3,0,1,0,1,3,2,1,2,0]
        # self.turnCardReal='665544'
        # self.allDisCardData='665544'
        tmpCardstr = ""
        for i in range(0, len(self.user_hand_cards_real)):
            tmpCardstr += COLOR_LIST[self.user_hand_colors[i]] + self.user_hand_cards_real[i]

        self.UserHandCards.setText(tmpCardstr)
        # 识别玩家的角色
        if self.sandayi==1 and self.bReSortCard==False:
            self.otherPlayerData[0] = self.changeDataOut(self.user_hand_cards_real, self.user_hand_colors)
            while self.bSanDayiStart==False:
                print("等待其他个玩家上传手牌")
                self.OhterSearchCard()
                if self.RunGame==False:
                    print("游戏已经停止")
                    print("请按开始按钮")
                    return False
                break
                self.sleep(2000)
        print("开始对局")
        print("手牌:", self.user_hand_cards_real)

        print("开始对局")
        print("手牌花色:", tmpCardstr)
        print("手牌:", self.user_hand_cards_real)
        # 生成手牌结束，校验手牌数量
        # 得到出牌顺序
        self.play_order = 0  # self.find_landlord()
        return  True
    def init_cards(self):
        self.game_over=False
        try:
            self.start()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            print("游戏出现异常请重新开始")
            self.stop()
            self.sleep(2000)
            #self.init_cards()
    def start(self):
        print("开始出牌\n")
        while not self.game_over:
            # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
            if self.gameInit()==False:
                break
            if self.play_order == 0:
                self.shengYuPaiShow(self.user_hand_cards_real)
                self.PredictedCard.setText("...")
                action_message,colors ,duoZhongBaiFa,returnCardData= self.dllCall(self.user_hand_cards_real,self.user_hand_colors,self.turnCardReal,self.allDisCardData,self.bHavePass)
                tmpCardstr = ""
                for i in range(0,len(action_message)):
                    tmpCardstr +=COLOR_LIST[colors[i]]+action_message[i]
                    if i==2 or i==7 :
                       tmpCardstr += "\n "
                self.PredictedCard.setText(tmpCardstr if (len(tmpCardstr) > 0) else "不出")
                #self.WinRate.setText("评分：" + '0')
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
                        image, windowPos = helper.Screenshot()
                        handCardsInfo, states, _ = helper.GetCards(image, MAX_CARD_COUNT)
                        tmpCardArray=[]
                        randnum1 = random.randint(1, 6)
                        for c in self.user_hand_cards_real:
                            tmpCardArray.append(c[0])
                        for i in range(0,len(action_message)):
                            outCardStr+=action_message[i]
                            tempColors.append(colors[i])
                            for j  in range(0,len(tmpCardArray)):
                                if action_message[i]==tmpCardArray[j] and self.user_hand_colors[j]==colors[i] and i!=j:
                                    randnum = random.randint(2, 4)
                                    #if randnum1>=3 :
                                        #randnum = random.randint(2, 4)
                                    if self.bFastEnable or duoZhongBaiFa==1 :
                                        randnum=random.randint(1, 2)
                                    self.sleep(1000 * randnum)
                                    print("切换牌:",colors[i], action_message[i],)
                                    helper.LeftClickEX(handCardsInfo[i][1], handCardsInfo[j][1])
                                    tmpCard= tmpCardArray[j]
                                    tmpCardArray[j]=tmpCardArray[i]
                                    tmpCardArray[i]=tmpCard

                                    tmpColor=self.user_hand_colors[j]
                                    self.user_hand_colors[j]=self.user_hand_colors[i]
                                    self.user_hand_colors[i] =tmpColor
                                    break
                            if i==7:
                                break
                self.sleep(1000 * 2)
                bBreakGame=False
                tmpuser_hand_cards_real, tmpuser_hand_colors = self.find_my_cards(self.MyHandCardsPos)
                for j  in range(0,8):
                    if action_message[j]!=tmpuser_hand_cards_real[j]:
                        self.bReSortCard=True
                        bBreakGame=True
                        break
                if bBreakGame==False:
                    # pygame.mixer.music.load('win.mp3')
                    # pygame.mixer.music.play()
                    randnum = random.randint(1, 2)
                    if self.bReSortCard == True:
                        randnum=0.5
                    self.sleep(1000 * randnum)
                    if self.onlyTip:
                        a = 4
                    else:
                         helper.ClickOnImage("go_btn", region=tempOutCardPos, confidence=0.80)
                    # 更新界面

                    self.allDisCardData=self.allDisCardData+action_message
                    #self.user_hand_cards_real = self.DeleteCard(self.user_hand_cards_real, action_message)
                    print(self.play_order,"handcount0：", self.handCardCount[0])
                    print(self.play_order,"handcount1：", self.handCardCount[1])
                    self.sleep(300 )
                    result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.80)
                    while result is not None :
                        result2 = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos, confidence=0.80)
                        if result2 is not None:
                            pygame.mixer.music.load('common_alert.wav')
                            pygame.mixer.music.play()
                            bZhengBi=False
                            if action_message[8]==action_message[9]and action_message[8]==action_message[10]and action_message[8]==action_message[11]:
                                bZhengBi = True
                            if action_message[8]=="A" and action_message[9]=="K"and action_message[10]=="Q"and action_message[11]=="J"and action_message[12]=="T" and colors[8] ==colors[9]and colors[8] ==colors[10]and colors[8] ==colors[11]and colors[8] ==colors[12]:
                                bZhengBi = True
                            if bZhengBi:
                                    helper.ClickOnImage("zhengChangBi", region=self.zhengChangBiPaiBtnPos, confidence=0.80)
                            else:
                                helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos, confidence=0.80)

                            self.sleep(500)
                            break
                        result3 = helper.LocateOnScreen("reSort_btn", region=self.changePlayerBtnPos, confidence=0.80)
                        if result3 is not None:
                            helper.ClickOnImage("reSort_btn", region=self.changePlayerBtnPos, confidence=0.80)
                            self.sleep(500)
                            self.bReSortCard = True
                            break
                        else:
                            self.bReSortCard = False
                        result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.80)
                        self.sleep(50)
                #self.play_order = 2
                #self.sleep(200)
                self.detect_start_btn()
            elif self.play_order == 2:
                self.play_order = 0
                self.detect_start_btn()

            else:
                pass
            self.sleep(50)

        print("{}胜，本局结束!\n")
        # QMessageBox.information(self, "本局结束", "{}胜！".format("农民" if self.env.winner == "farmer" else "地主"),
        #                         QMessageBox.Yes, QMessageBox.Yes)
        #self.detect_start_btn()

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
        print(".........")
        tmpCardstr = ""
        for i in range(0, len(self.user_hand_cards_real)):
            tmpCardstr += COLOR_LIST[self.user_hand_colors[i]] + self.user_hand_cards_real[i]
        print("手牌：",tmpCardstr)
        tmpCardstr = ""
        for i in range(0, len(TurnCardData)):
            tmpCardstr += COLOR_LIST[self.turnCard_colors[i]] + TurnCardData[i]
        print("turn牌：", tmpCardstr)
        print(".........")
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
            self.InitCard.setEnabled(True)
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
