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
class CMD_GR_LogonUserID(Structure):
    _fields_ = [("dwUserID", c_int64 )
                ]
class CMD_GR_LogonSuccess(Structure):
    _fields_ = [("wTableID", c_ulong),
                ("wChairID", c_ulong),
                ]

class CMD_GR_LogonFailure(Structure):
    _fields_ = [("lErrorCode", c_ulong),
                ("szDescribeString", c_wchar*128),
                ]

class cmd_cardDataInfo(Structure):
    _fields_ = [
                ("wChairID", c_ushort),
                ("cbCardData", c_ubyte* 20),
                ("cbCardCount", c_ubyte),
                ("cbCardDataEx", c_ubyte * 200),
                ("cbCardExCount", c_ubyte),
                ]

class CMD_GR_UserSitDown(Structure):
    _fields_ = [("wTableID", c_ushort),
                ("wChairID", c_ushort),
                ]
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
            cardData = cmd_cardDataInfo()
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
        loginData = CMD_GR_UserSitDown()
        loginData.wTableID = self.tableid
        loginData.wChairID = 0
        socketTool.sendData(self.client, socketTool.MDM_GR_USER, socketTool.SUB_GR_USER_SITDOWN, loginData)
    def userLogin(self):
        loginData = CMD_GR_LogonUserID()
        tmp=sizeof(loginData)

        loginData.dwUserID =self.userid
        socketTool.sendData(self.client, socketTool.MDM_GR_LOGON, socketTool.SUB_GR_LOGON_MOBILE, loginData)
    def userReady(self):
        loginData = CMD_GR_LogonUserID()
        loginData.dwUserID = self.userid
        socketTool.sendData(self.client, socketTool.MDM_GF_FRAME, socketTool.SUB_GF_USER_READY, loginData)
    # 主程序入口函数
    # 处理接收并打印服务端向自己发送的消息
    def recvGameData(self,msg_head,data):
        if msg_head.wMainCmdID==socketTool.MDM_GR_LOGON:
            if msg_head.wSubCmdID==socketTool.SUB_GR_LOGON_SUCCESS:
                dataBuffer = CMD_GR_LogonSuccess.from_buffer(data)
                print("登陆成功")
            elif msg_head.wSubCmdID==socketTool.SUB_GR_LOGON_FAILURE:
                dataBuffer = CMD_GR_LogonFailure.from_buffer(data)
                print("登陆失败:",dataBuffer.szDescribeString)
        elif msg_head.wMainCmdID==socketTool.MDM_GR_USER:
            if msg_head.wSubCmdID == socketTool.SUB_GR_REQUEST_FAILURE:
                print("坐下失败")
            elif msg_head.wSubCmdID == socketTool.SUB_GR_USER_SIT_SUCCESS:
                dataBuffer = CMD_GR_UserSitDown.from_buffer(data)
                print("坐下成功,桌子号:",dataBuffer.wTableID,"椅子号:",dataBuffer.wChairID)
                self.wChairID=dataBuffer.wChairID
        elif msg_head.wMainCmdID == socketTool.MDM_GF_GAME:
            if msg_head.wSubCmdID==socketTool.SUB_C_SEND_CARD:
                dataBuffer = cmd_cardDataInfo.from_buffer(data)
                tmpStr='座位号:'+str(dataBuffer.wChairID)
                print("游戏消息1:", tmpStr)
                tmpStr=''
                for i in range(0,dataBuffer.cbCardCount):
                    tmpStr=tmpStr+str(dataBuffer.cbCardData[i])+','
                print("游戏消息2:", tmpStr)
                tmpStr=''
                for i in range(0,dataBuffer.cbCardExCount):
                    tmpStr=tmpStr+str(dataBuffer.cbCardDataEx[i])+','
                print("游戏消息3:", tmpStr)
                if self.msgCall:
                    self.msgCall(dataBuffer)
        else:
            print("ohter")
    def accept(self,client,otherData):
        rlist = [client]
        wlist = []
        elist = []
        while True:
            rs, ws, es = select.select(rlist, wlist, elist)
            for r in rs:
                if r is client:
                    head_info, complete_data = socketTool.recveData(client)
                    if head_info:
                        self.recvGameData(head_info, complete_data)

    def readJson(self):
        with open("data_file.json", "r") as read_file:
            self.configData = json.load(read_file)
        self.userid = self.configData["userid"]
        self.tableid=self.configData["tableid"]
        self.port=self.configData["port"]
    def start(self):
        isLogin = self.GetScoketHandle()
        if isLogin[0]:
            self.client = isLogin[1]
            otherData=1#这里一定要传入一个无组,所以给他一个额外的变量
            # p = Process(target=self.accept, args=(self.client,otherData))
            # p.daemon = True
            # p.run()
            t = threading.Thread(target=self.accept, args=(self.client,otherData))
            t.start()
            testCount=1
            while True:
                try:
                    # 处理客户端键盘输入并需要发送的消息
                    if testCount==1:
                        a=4
                        self.userLogin()
                        self.userSitDown()
                        self.userReady()
                        #tmpHandCard=[0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D]
                        # tmpHandCard = [0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D]
                        #ohtherCard=[0x0D, 0x0D, 0x0D]
                        #self.userCardData(tmpHandCard,ohtherCard)
                    testCount=0
                    print("在连接")
                    break
                    #time.sleep(20)
                except Exception as e:
                    exc_type, exc_obj, exc_tb = sys.exc_info()
                    print(e)
                    traceback.print_tb(exc_tb)
                    # 如果遇到退出/中止信号，发送退出信息，关闭套接字，结束子进程，退出程序
                    self.client.close()
                    threading.Thread._Thread__stop(t)
                    #p.terminate()
                    break
        else:
            print("网络错误...")


