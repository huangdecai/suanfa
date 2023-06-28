import binascii
import os, sys
import socket
import select
import settings
import common
import time
import traceback
from ctypes import *
import socketTool
import json
from multiprocessing import Process, Queue
import threading
import struct

MAX_COUNT=20

class Client:
    def __init__(self):
        self.client=None
        self.userid=0
        self.wChairID=0
        self.tableid=0
        self.port = 0
        self.readJson()
        self.msgCall=None
    def GetChaiID(self):
        return self.wChairID
    def SetMsgCall(self,fucCall):
        self.msgCall=fucCall
    def GetScoketHandle(self):
        ADDR=settings.HOST,self.port
        client = common.createclient(ADDR)
        return (True, client)

    def userCardData(self,tmpHandCard,ohtherCard):
        if self.client:
            cardData = socketTool.cmd_cardDataInfo()
            tmparray = c_ubyte * MAX_COUNT
            cardData.cbCardData = tmparray()
            for i in range(0, len(tmpHandCard)):
                cardData.cbCardData[i] = tmpHandCard[i]
            for i in range(len(tmpHandCard), MAX_COUNT):
                cardData.cbCardData[i] = 0
            cardData.cbCardCount = len(tmpHandCard)

            tmparrayEx = c_ubyte * 200
            cardData.cbCardDataEx = tmparrayEx()
            for i in range(0, len(ohtherCard)):
                cardData.cbCardDataEx[i] = ohtherCard[i]
            for i in range(len(ohtherCard), 200):
                cardData.cbCardDataEx[i] = 0
            cardData.cbCardExCount = len(ohtherCard)
            cardData.wChairID=self.wChairID
            socketTool.sendData(self.client,socketTool.MDM_GF_GAME,socketTool.SUB_C_SEND_CARD,cardData)
    def userSitDown(self):
        loginData = socketTool.CMD_GR_UserSitDown()
        loginData.wTableID = self.tableid
        loginData.wChairID = 0
        socketTool.sendData(self.client, socketTool.MDM_GR_USER, socketTool.SUB_GR_USER_SITDOWN, loginData)
    def userLogin(self):
        loginData = socketTool.CMD_GR_LogonUserID()
        tmp=sizeof(loginData)
        loginData.dwUserID =self.userid
        loginData.password = self.passWord
        socketTool.sendData(self.client, socketTool.MDM_GR_LOGON, socketTool.SUB_GR_LOGON_MOBILE, loginData)
    def userReady(self):
        loginData = socketTool.CMD_GR_LogonUserID()
        loginData.dwUserID = self.userid
        socketTool.sendData(self.client, socketTool.MDM_GF_FRAME, socketTool.SUB_GF_USER_READY, loginData)
    # 主程序入口函数
    # 处理接收并打印服务端向自己发送的消息
    def recvGameData(self,msg_head,data):
        if msg_head.wMainCmdID==socketTool.MDM_GR_LOGON:
            if msg_head.wSubCmdID==socketTool.SUB_GR_LOGON_SUCCESS:
                dataBuffer = socketTool.CMD_GR_LogonSuccess.from_buffer(data)
                print("登陆成功")
            elif msg_head.wSubCmdID==socketTool.SUB_GR_LOGON_FAILURE:
                dataBuffer = socketTool.CMD_GR_LogonFailure.from_buffer(data)
                print("登陆失败:",dataBuffer.szDescribeString)
        elif msg_head.wMainCmdID==socketTool.MDM_GR_USER:
            if msg_head.wSubCmdID == socketTool.SUB_GR_REQUEST_FAILURE:
                print("坐下失败")
            elif msg_head.wSubCmdID == socketTool.SUB_GR_USER_SIT_SUCCESS:
                dataBuffer = socketTool.CMD_GR_UserSitDown.from_buffer(data)
                print("坐下成功,桌子号:",dataBuffer.wTableID,"椅子号:",dataBuffer.wChairID)
                self.wChairID=dataBuffer.wChairID
                if self.msgCall:
                    self.msgCall(msg_head.wSubCmdID, dataBuffer)
        elif msg_head.wMainCmdID == socketTool.MDM_GF_GAME:
            if self.msgCall:
                self.msgCall(msg_head.wSubCmdID,data)
        else:
            print("ohter")
    def accept(self,client,parent):
        rlist = [client]
        wlist = []
        elist = []
        while rlist:
            rs, ws, es = select.select(rlist, wlist, elist)
            for r in rs:
                if r is client:
                    try:
                        head_info, complete_data = socketTool.recveData(client)
                        if head_info:
                            self.recvGameData(head_info, complete_data)
                    except Exception as e:
                        print(e.__str__())
                        rlist.remove(client)
                        parent.client.close()
                        parent.client=None
                        self.msgCall(-1,1)

    def readJson(self):
        with open("data_file.json", "r") as read_file:
            self.configData = json.load(read_file)
        self.userid = self.configData["userid"]
        self.tableid=self.configData["tableid"]
        self.port=self.configData["port"]
        self.passWord=self.configData["password"]
    def start(self):
        isLogin = self.GetScoketHandle()
        if isLogin[0]:
            self.client = isLogin[1]
            otherData=1#这里一定要传入一个无组,所以给他一个额外的变量
            # p = Process(target=self.accept, args=(self.client,otherData))
            # p.daemon = True
            # p.run()
            t = threading.Thread(target=self.accept, args=(self.client,self))
            t.setDaemon(True)
            t.start()
            testCount=1
            while True:
                try:
                    # 处理客户端键盘输入并需要发送的消息
                    if testCount==1:
                        testCount = 0
                        a=4
                        self.userLogin()
                        time.sleep(1)
                        self.userSitDown()
                        time.sleep(1)
                        self.userReady()
                        #tmpHandCard=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D]
                        # tmpHandCard = [0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D]
                        # ohtherCard=[0x0D, 0x0D, 0x0D]
                        # self.userCardData(tmpHandCard,ohtherCard)

                    print("在连接")
                    break
                    time.sleep(10)
                except Exception as e:
                    exc_type, exc_obj, exc_tb = sys.exc_info()
                    print(e)
                    traceback.print_tb(exc_tb)
                    # 如果遇到退出/中止信号，发送退出信息，关闭套接字，结束子进程，退出程序
                    if self.client:
                        self.client.close()
                    #p.terminate()
                    break
        else:
            print("网络错误...")


