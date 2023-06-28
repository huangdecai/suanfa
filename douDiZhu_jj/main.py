# -*- coding: utf-8 -*-
# Created by: Raf
# Modify by: Vincentzyx

import GameHelper as gh
import socketTool
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
from client import Client
from PyQt5 import QtGui, QtWidgets, QtCore
from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPixmapItem, QInputDialog, QMessageBox
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import QTime, QEventLoop
from MainWindow import Ui_Form

from douzero.env.game import GameEnv
from douzero.evaluation.deep_agent import DeepAgent
import traceback
import json
import BidModel
import LandlordModel
import FarmerModel
from ctypes import *
MAX_COUNT=20
NORMAL_COUNT=17
FULL_COUNT=54

AllEnvCard = [3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
              8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
              12, 12, 12, 13, 13, 13, 13, 14, 14, 14,17]

AllCards = ['rD', 'bX', 'b2', 'r2', 'bA', 'rA', 'bK', 'rK', 'bQ', 'rQ', 'bJ', 'rJ', 'bT', 'rT',
            'b9', 'r9', 'b8', 'r8', 'b7', 'r7', 'b6', 'r6', 'b5', 'r5', 'b4', 'r4', 'b3', 'r3']

helper = GameHelper()
helper.ScreenZoomRate = 1.0  # 请修改屏幕缩放比
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
        self.WhiteConfidence = 0.85  # 检测白块的置信度
        self.LandlordFlagConfidence = 0.95  # # 检测地主标志的置信度
        self.ThreeLandlordCardsConfidence = 0.9  # 检测地主底牌的置信度
        self.PassConfidence = 0.60
        self.WaitTime = 1  # 等待状态稳定延时
        self.MyFilter = 40  # 我的牌检测结果过滤参数
        self.OtherFilter = 25  # 别人的牌检测结果过滤参数
        self.SleepTime = 0.1  # 循环中睡眠时间
        self.yaoBuQi=True
        self.RunGame = True
        self.AutoPlay = True
        self.game_over = True
        self.onlyTip=False
        self.mylastCard=""
        self.myHaveOutCard = ""
        self.rightHaveOutCard = ""
        self.myHavePassType = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        self.JiabeiThreshold = (
            (80, 65),  # 叫地主 超级加倍 加倍 阈值
            (85, 70)   # 叫地主 超级加倍 加倍 阈值  (在地主是抢来的情况下)
        )
        # 坐标
        self.MyHandCardsPos = (180, 277, 800, 100)  # 我的截图区域
        self.LPlayedCardsPos = (130, 150, 600, 80)  # 左边截图区域
        self.RPlayedCardsPos = (189, 195, 840, 150)  # 右边截图区域
        self.RPlayedPassPos = (905, 118, 200, 80)
        self.otherPassPos = (906, 122, 200, 80)
        self.yaoBuQiBtnPos = (555, 406, 200, 80)
        self.buChuBtnPos = (300, 400, 200, 80)
        self.buJiaoBtnPos = (430, 400, 576, 137)
        self.jiaoDiZhuBtnPos = (717, 400, 576, 137)
        self.outCardBtnPos = (830, 400, 200, 150)
        self.changePlayerBtnPos = (728, 617, 250, 90)
        self.tipBtnPos = (616, 406, 200, 100)
        self.rangCardPos = (468, 489, 200, 45)
        # 信号量
        self.fenFaQi = Client()
        self.fenFaQi.SetMsgCall(self.RecvPlayerMsg)
        self.configData=None
        self.readJson()
        self.connected=False
        #self.init_cards()

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
            for i in range(0, dataBuffer.cbCardExCount):
                tmpStr = tmpStr + str(dataBuffer.cbCardDataEx[i]) + ','
            print("游戏消息3:", tmpStr)
        elif wSubCmdID == -1:
            print("close")
            self.connected=False
            return
        elif wSubCmdID==socketTool.SUB_GR_USER_SIT_SUCCESS:
            if data.wChairID==self.fenFaQi.GetChaiID():
                self.connected = True

    def startLoginGame(self):
        self.fenFaQi.start()
    def init_display(self):
        self.yuanShiZhangShu = self.MAX_CARD_COUNT
        self.rangCount = 0
        self.beiRangCount = 0
        self.WinRate.setText("评分")
        self.InitCard.setText("开始")
        self.UserHandCards.setText("手牌")
        self.LPlayedCard.setText("")
        self.RPlayedCard.setText("")
        self.jiaoDiZhuFen.setText("叫地主分数:")
        self.QiangDiZhuFen.setText("抢地主分数:")
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")
        self.myHaveOutCard = ""
        self.rightHaveOutCard = ""
        for player in self.Players:
            player.setStyleSheet('background-color: rgba(255, 0, 0, 0);')
    def setRangCount(self):
        tmpStr=["rang0","rang1","rang2","rang3","rang4"]
        while True:
            for i in range(0,len(tmpStr)):
                result = helper.LocateOnScreen(tmpStr[i], region=self.rangCardPos, confidence=0.80)
                print("setRangCount....")
                if result:
                    if self.play_order == 0:
                        self.rangCount=i+1
                        print("setRangCount1",self.rangCount)
                        return
                    else:
                        self.beiRangCount = i + 1
                        print("beiRangCount", self.beiRangCount)
                        return
            self.sleep(30)
    def switch_mode(self):
        self.AutoPlay = not self.AutoPlay
        self.SwitchMode.setText("自动" if self.AutoPlay else "单局")
    def readJson(self):
        with open("data_file.json", "r") as read_file:
            self.configData = json.load(read_file)
        self.userid = self.configData["userid"]
        self.tableid = self.configData["tableid"]
        self.port = self.configData["port"]
        self.m_duokai = str(self.configData["duokai"])
        self.setWindowTitle(self.m_duokai + '号机')
        helper.setFindStr(self.m_duokai)
        self.MAX_CARD_COUNT=self.configData["zhang"]
        if self.MAX_CARD_COUNT<=0:
            self.MAX_CARD_COUNT=NORMAL_COUNT
        a=4
    def init_cards(self):
        # 得到出牌顺序

        try:
            self.RunGame =True
            self.game_over=True
            self.start()
        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            print(e)
            traceback.print_tb(exc_tb)
            self.stop()

    def gameStart(self):
        GameHelper.Interrupt = False
        self.init_display()
        # 玩家手牌
        self.user_hand_cards_real = ""
        self.turnCardReal=''
        self.other_played_cards_real=''
        self.myHavePassType = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.allDisCardData=''
        self.shengYuCardData=''
        self.handCardCount=[NORMAL_COUNT,NORMAL_COUNT,NORMAL_COUNT]
        self.bHavePass=False
        #self.detect_start_btn()
        #helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos)
        self.env = None
        self.game_over= False
        helper.bTest = False

        # 识别玩家手牌
        if self.connected==False:
            print("你的账号没有登陆，请联系Q：460000713，进行购买")
            return
        #tmpHandCard = self.changeDataOut('2AKQQJJT99877753')
        #tmpHandCardStr = self.changeDataIn(tmpHandCard)

        #result = self.getTipBtnrResult()
        # self.handCardCount[0]=16
        # helper.SelectCards("77", self.handCardCount[0], True)
        #self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)
        # self.bHavePass=True
        # self.user_hand_cards_real='DX222QQ88'
        # self.turnCardReal='KKK7'
        # self.myHaveOutCard = '72AKQJT666A'
        # self.rightHaveOutCard='AKKK7'
        # self.rangCount=1
        # self.handCardCount[0] = MAX_COUNT
        # self.yuanShiZhangShu = MAX_COUNT
        #
        # action_message = self.dllCall(self.user_hand_cards_real, self.turnCardReal, self.myHaveOutCard,
        #                               self.rightHaveOutCard, self.bHavePass, self.myHavePassType)
        helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos, confidence=0.70)
        if helper.bTest==True:
           result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.75)
           self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)
           c = 0
        #self.play_order = self.find_landlord()
        self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos) #'2AKQQJJT99877753'
        while len(self.user_hand_cards_real)<NORMAL_COUNT :
            self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)
            self.sleep(500)
            print("hangCountShiBieWenTi",len(self.user_hand_cards_real))
        #self.user_hand_cards_real =  'AKKKKQJJT9988744'
        #self.turnCardReal='665544'
        #self.allDisCardData='665544'
        self.UserHandCards.setText(self.user_hand_cards_real)
        # 识别玩家的角色
        print("开始对局")
        print("手牌:",self.user_hand_cards_real)
        self.jiaoDiZhuCheck()
        self.play_order = self.find_landlord()
        if self.play_order==0:
            self.handCardCount[0] = MAX_COUNT
            self.yuanShiZhangShu =MAX_COUNT
            self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)  # '2AKQQJJT99877753'
            while len(self.user_hand_cards_real) < self.handCardCount[0]:
                self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)
                self.sleep(500)
                print("handDataCountShiBieWenTi2", len(self.user_hand_cards_real))
            self.UserHandCards.setText(self.user_hand_cards_real)
            while self.RunGame and self.have_white(self.RPlayedCardsPos) == 0 :
                print(self.play_order, "等待出牌按钮")
                self.sleep(10)
        else:
            self.yuanShiZhangShu = NORMAL_COUNT
            self.handCardCount[1] = MAX_COUNT
        self.setRangCount()
        self.shengYuCardData=self.shengYuCardData+self.user_hand_cards_real
        self.shengYuPaiShow(self.shengYuCardData)
    def sleep(self, ms):
        self.counter.restart()
        while self.counter.elapsed() < ms:
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 50)
    def getTipBtnrResult(self):
        result = helper.LocateOnScreen("tip_btn", region=self.tipBtnPos, confidence=0.75)
        result2 = helper.LocateOnScreen("tip_btn1", region=self.tipBtnPos, confidence=0.75)
        return  result or result2
    def DeleteCard(self, src,sub):
        tmpstr = src
        for i in range(0, len(sub)):
            for j in range(0, len(src)):
                if sub[i] == src[j]:
                    tmpstr = tmpstr.replace(sub[i], '', 1)
                    break
        return tmpstr
    def DeleteCard(self, src,sub):
        tmpstr = src
        for i in range(0, len(sub)):
            for j in range(0, len(src)):
                if sub[i] == src[j]:
                    tmpstr = tmpstr.replace(sub[i], '', 1)
                    break
        return tmpstr
    def isGameOver(self):
        return (self.handCardCount[0]-self.beiRangCount)==0 or (self.handCardCount[1]-self.rangCount)==0
    def start(self):
        print("开始出牌\n")
        while self.RunGame:
            # 玩家出牌时就通过智能体获取action，否则通过识别获取其他玩家出牌
            if self.game_over:
               self.gameStart()
            if self.play_order == 0:
                self.PredictedCard.setText("...")
                if len(self.turnCardReal)==0 :
                    self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)

                action_message = self.dllCall(self.user_hand_cards_real,self.turnCardReal,self.myHaveOutCard,self.rightHaveOutCard,self.bHavePass,self.myHavePassType)
                self.PredictedCard.setText(action_message if (len(action_message) > 0) else "不出")
                self.WinRate.setText("别人出牌：" + self.turnCardReal)
                print(self.play_order,"\nuser_hand_cards_real：", self.user_hand_cards_real)
                print(self.play_order,"\nturnCardReal：", self.turnCardReal)
                print(self.play_order,"\nallDisCardData：", self.allDisCardData)
                print(self.play_order,"\n我出牌：", action_message)
                hand_cards_str = self.user_hand_cards_real
                tempOutCardPos=self.outCardBtnPos

                bPass=False
                if len(action_message) == 0:
                    result = helper.LocateOnScreen("pass_btn", region=self.buChuBtnPos, confidence=0.85)
                    if result:
                        helper.ClickOnImage("pass_btn", region=self.buChuBtnPos)
                        print(self.play_order, "pass_btn")
                    else:
                        helper.ClickOnImage("yaobuqi", region=self.yaoBuQiBtnPos)
                        print(self.play_order, "yaobuqi")
                    bPass=True
                else:
                    if self.onlyTip :
                        print(self.play_order, "提示模式，请手动打牌")
                    else:
                        if len(hand_cards_str) == 0 and len(action_message) == 1:
                            print("SelectCards1",action_message)
                            helper.SelectCards(action_message,self.yuanShiZhangShu, True)
                        else:
                            print("SelectCards2", action_message)
                            helper.SelectCards(action_message,self.yuanShiZhangShu)
                        #self.sleep(1000)
                        tryCount = 20
                        result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.85)
                        while result is None :
                            if not self.RunGame:
                                break
                            print(self.play_order,"等待出牌按钮",tempOutCardPos[0])
                            self.game_over=self.detect_start_btn()
                            if self.game_over:
                                break
                            tryCount -= 1
                            result = helper.LocateOnScreen("go_btn", region=tempOutCardPos, confidence=0.85)
                            self.sleep(50)
                        helper.ClickOnImage("go_btn", region=tempOutCardPos, confidence=0.85)

                # 更新界面
                #tmpCard=self.find_other_cardsEx(self.MyHandCardsPos)
                self.mylastCard =action_message
                if len(action_message)>0:
                   self.myHaveOutCard=self.myHaveOutCard+action_message
                   self.RPlayedCard.setText("我方牌："+self.myHaveOutCard)
                self.handCardCount[self.play_order]=self.handCardCount[self.play_order]-len(action_message)
                self.allDisCardData=self.allDisCardData+action_message
                self.user_hand_cards_real = self.DeleteCard(self.user_hand_cards_real, action_message)
                self.UserHandCards.setText("手牌：" + self.user_hand_cards_real)
                print(self.play_order,"handcount0：", self.handCardCount[0])
                print(self.play_order,"handcount1：", self.handCardCount[1])
                #这个地方不要轻易打开，不同的平台消失这个按钮速度不同，慢的可以打开，不慢的就不要打开，否则下家又出完牌了，你才识别出来
                # result = self.getTipBtnrResult()
                # while (result is not None) and bPass==False :
                #     result = self.getTipBtnrResult()
                #     print(self.play_order, "wait--tip_btn")
                #     self.sleep(20)
                self.play_order = 1

                self.game_over=self.detect_start_btn()
            elif self.play_order == 1:
                self.sleep(100)
                pass_flag = helper.LocateOnScreen('pass',
                                                  region=self.otherPassPos,
                                                  confidence=self.PassConfidence)
                bGameOver=False
                GameOverCheckOut=0
                while self.RunGame and self.have_white(self.RPlayedCardsPos) == 0 and pass_flag is None:
                    print(self.play_order,"等待下家出牌")
                    self.sleep(10)
                    pass_flag = helper.LocateOnScreen('pass', region=self.otherPassPos,
                                                      confidence=self.PassConfidence)
                    GameOverCheckOut=GameOverCheckOut+1
                    if GameOverCheckOut%3==0:
                        result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
                        if (result is not None) :
                            bGameOver=True
                            break
                if bGameOver :
                    self.game_over=self.detect_start_btn()
                else:
                    # 未找到"不出"
                    if pass_flag is None:
                        # 识别下家出牌
                        self.sleep(200)
                        self.turnCardReal = self.find_other_cardsEx(self.RPlayedCardsPos)
                        if len(self.turnCardReal) > 0:
                            self.handCardCount[self.play_order] = self.handCardCount[self.play_order] - len(
                                self.turnCardReal)
                        print("下家出牌：", self.turnCardReal)
                        self.other_played_cards_real = self.other_played_cards_real + self.turnCardReal
                        self.allDisCardData = self.allDisCardData + self.turnCardReal
                        self.shengYuCardData = self.shengYuCardData + self.turnCardReal
                        self.yaoBuQi = False
                        self.rightHaveOutCard = self.rightHaveOutCard  + self.turnCardReal
                        self.LPlayedCard.setText("对家牌："+self.rightHaveOutCard)
                    # 找到"不出"
                    else:
                        self.FillPassType(self.mylastCard)
                        self.turnCardReal = ""
                        self.bHavePass = True
                        self.yaoBuQi=True
                        print("下家不要")

                    print(self.play_order,"handcount0：", self.handCardCount[0])
                    print(self.play_order,"handcount1：", self.handCardCount[1])

                    self.shengYuPaiShow(self.shengYuCardData)
                    self.sleep(800)
                    self.play_order = 0
                    self.game_over=self.detect_start_btn()

            else:
                pass
            self.sleep(50)

        print("游戏停止结束!\n")
        # QMessageBox.information(self, "本局结束", "{}胜！".format("农民" if self.env.winner == "farmer" else "地主"),
        #                         QMessageBox.Yes, QMessageBox.Yes)
        #self.detect_start_btn()

    def jiaoDiZhuCheck(self):
        operateCount=0
        while self.isJiaoState() is None :

            resultJ = helper.LocateOnScreen("jiaodizhu", region=self.jiaoDiZhuBtnPos,
                                            confidence=self.LandlordFlagConfidence)
            resultQ = helper.LocateOnScreen("qiangdizhu", region=self.jiaoDiZhuBtnPos,
                                           confidence=self.LandlordFlagConfidence)
            print("jiaoDiZhuCheck...")
            if resultJ :
                score=self.getCardScore(self.user_hand_cards_real)
                print("jiaoDiZhuCheck...0",score)
                self.jiaoDiZhuFen.setText("叫地主分数:"+str(score))
                if score>=5 and operateCount==0:
                    helper.ClickOnImage("jiaodizhu", region=self.jiaoDiZhuBtnPos)
                    operateCount+=1
                else:
                    helper.ClickOnImage("bujiao", region=self.buJiaoBtnPos)
                    operateCount=0
            elif resultQ:
                score = self.getCardScore(self.user_hand_cards_real)
                self.QiangDiZhuFen.setText("抢地主分数:"+str(score))
                if score >= 20 and operateCount==0:
                    helper.ClickOnImage("qiangdizhu", region=self.jiaoDiZhuBtnPos)
                    operateCount += 1
                elif score >= 60 and operateCount<=1:
                    helper.ClickOnImage("qiangdizhu", region=self.jiaoDiZhuBtnPos)
                    operateCount += 1
                elif score >= 100 and operateCount<=2:
                    helper.ClickOnImage("qiangdizhu", region=self.jiaoDiZhuBtnPos)
                    operateCount += 1
                elif score >= 150 and operateCount<=3:
                    helper.ClickOnImage("qiangdizhu", region=self.jiaoDiZhuBtnPos)
                    operateCount += 1
                else:
                    helper.ClickOnImage("buqiang", region=self.buJiaoBtnPos)
                    operateCount = 0
                print("jiaoDiZhuCheck...1",score)
            self.sleep(200)
            result = self.getTipBtnrResult()
            if result:
                break
            elif operateCount==0:
                self.user_hand_cards_real = self.find_my_cards(self.MyHandCardsPos)
                self.UserHandCards.setText(self.user_hand_cards_real)

    def isJiaoState(self):
        result = helper.LocateOnScreen("land0", region=(1200, 183, 50, 50),
                                           confidence=self.LandlordFlagConfidence)
        if result :
            return result
        result1 = helper.LocateOnScreen("land1", region=(1200, 183, 50, 50),
                                            confidence=self.LandlordFlagConfidence)
        return result1
    def find_landlord(self):
          while True:
            result = helper.LocateOnScreen("land0", region=(1200,183,50,50),
                                           confidence=self.LandlordFlagConfidence)
            result1 = helper.LocateOnScreen("land1", region=(1200,183,50,50),
                                           confidence=self.LandlordFlagConfidence)
            print("find_landlord...")
            if result :
                print("find_landlord...1")
                return 1
            elif result1:
                print("find_landlord...0")
                return  0
            self.sleep(100)


    def detect_start_btn(self):
        result = helper.LocateOnScreen("change_player_btn", region=self.changePlayerBtnPos)
        if (result is not None) or self.isGameOver():
            print("点击换对手")
            print("{}胜，本局结束!\n")
            #self.stop()
            if result :
                self.sleep(2500)
            else:
                self.sleep(5000)
            if self.AutoPlay:
                self.sleep(2500)
                helper.ClickOnImage("change_player_btn", region=self.changePlayerBtnPos, confidence=0.70)
                helper.LeftClick((self.changePlayerBtnPos[0] + 60, self.changePlayerBtnPos[1] + 30))
                self.sleep(500)
            return True
    def shengYuPaiShow(self, cards):
        #self.ThreeLandlordCards.resize(300,100)
        AllCard = {'3': 3, '4': 4, '5': 5, '6': 6, '7': 7,
                    '8': 8, '9': 9, 'T': 10, 'J': 11, 'Q': 12,
                    'K': 13, 'A': 1, '2': 2, 'X': 14, 'D': 15}
        AllCardCount = {3: 4, 4: 4, 5: 4, 6: 4, 7: 4,
                   8: 4, 9: 4, 10: 4, 11: 4, 12: 4,
                   13: 4, 1: 4, 2: 4, 14: 1, 15: 1}
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
            if len(cards)>=10:
                return
            tmtype = self.getCardType(cards)
            print("FillPassType:",tmtype)
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
                       'K': 13, 'A': 1, '2': 2, 'X': 14, 'D': 15}
            AllCardCount = {3: 0, 4: 0, 5: 0, 6: 0, 7: 0,
                            8: 0, 9: 0, 10: 0, 11: 0, 12: 0,
                            13: 0, 1: 0, 2: 0, 14: 0, 15: 0}
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
                    'K': 13, 'A': 1, '2': 2, 'X': 14, 'D': 15}
        AllCardCount = {3: 0, 4: 0, 5: 0, 6: 0, 7: 0,
                   8: 0, 9: 0, 10: 0, 11: 0, 12: 0,
                   13: 0, 1: 0, 2: 0, 14: 0, 15: 0}
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
    def dllCall(self,HandCardData,TurnCardData,myDiscardData,otherDiscardData,bPass,HavePassType):
        print("dllCall:")
        print(HandCardData)
        print(TurnCardData)
        print(myDiscardData)
        print(otherDiscardData)
        print(bPass)
        print(HavePassType)
        class tagInPyhonNew(Structure):
            _fields_ = [("cbHandCardData", c_ubyte* 20),
                        ("cbHandCardCount", c_ubyte),
                        ("cbTurnCardData", c_ubyte* 20),
                        ("cbTurnCardCount", c_ubyte ),
                        ("cbDiscardCard", c_ubyte * 54),
                        ("cbDiscardCardCount", c_ubyte),
                        ("cbOtherDiscardCard", c_ubyte * 54),
                        ("cbOtherDiscardCount", c_ubyte),
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
        tmpDiscard=self.changeDataOut(myDiscardData)
        tmpOtherDiscard = self.changeDataOut(otherDiscardData)
        tmparray=c_ubyte * MAX_COUNT
        tmparray2 = c_ubyte * FULL_COUNT
        arg1.cbHandCardData=tmparray()
        arg1.cbTurnCardData = tmparray()
        arg1.cbResultCard = tmparray()

        arg1.cbDiscardCard = tmparray2()
        arg1.cbOtherDiscardCard = tmparray2()
        arg1.cbCardDataEx = tmparray()
        arg1.cbMaxCard = tmparray()
        for i in range(0,len(tmpHandCard)):
            arg1.cbHandCardData[i]=tmpHandCard[i]
        for i in range(0, len(tmpTurnCard)):
            arg1.cbTurnCardData[i] = tmpTurnCard[i]
        for i in range(0, len(tmpDiscard)):
            arg1.cbDiscardCard[i] = tmpDiscard[i]
        for i in range(0, len(tmpOtherDiscard)):
            arg1.cbOtherDiscardCard[i] = tmpOtherDiscard[i]

        for i in range(len(tmpHandCard), 20):
            arg1.cbHandCardData[i] = 0
        for i in range(len(tmpTurnCard), 20):
            arg1.cbTurnCardData[i] = 0
        for i in range(len(tmpTurnCard), 20):
             arg1.cbCardDataEx[i] = 0
        for i in range(0, 20):
            arg1.cbMaxCard[i] = HavePassType[i]
        for i in range(len(tmpDiscard), 54):
            arg1.cbDiscardCard[i] = 0
        for i in range(len(tmpOtherDiscard), 54):
            arg1.cbOtherDiscardCard[i] = 0

        arg1.cbHandCardCount = len(tmpHandCard)
        arg1.cbTurnCardCount = len(tmpTurnCard)
        arg1.cbDiscardCardCount = len(tmpDiscard)
        arg1.cbOtherDiscardCount = len(tmpOtherDiscard)
        arg1.cbCardCount=0
        if bPass :
            arg1.cbCardDataEx[0] = 1
        arg1.cbCardDataEx[1]=self.rangCount
        arg1.cbCardDataEx[2] = self.beiRangCount
        arg1.cbCardDataEx[3]=self.yuanShiZhangShu


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
                        ("cbCardScore", c_int32 ),
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
        print("cbCardScore:",arg1.cbCardScore)

        return arg1.cbCardScore
    def find_my_cards(self, pos):
        user_hand_cards_real = ""
        img, _ = helper.Screenshot()
        cards, _ = helper.GetCards(img,self.yuanShiZhangShu)
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
        # result = pyautogui.locate(needleImage=helper.Pics["card_edge2"], haystackImage=img,
        #                           region=pos, confidence=self.WhiteConfidence)
        result=self.getTipBtnrResult()
        result2 = helper.LocateOnScreen("yaobuqi", region=self.yaoBuQiBtnPos)
        if result or result2 :
            return 1
        else:
            return 0

    def stop(self):
        try:
            self.RunGame = False
            GameHelper.Interrupt = True
            self.env.reset()
            self.init_display()

        except AttributeError as e:
            pass

    def beforeStart(self):
        self.onlyTip = not self.onlyTip
        self.AutoStart.setText("只提示" if self.onlyTip else "代打")


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
