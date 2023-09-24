
from ctypes import *
import settings

MDM_GR_LOGON							= 1				#--登录信息
SUB_GR_LOGON_MOBILE					= 2				#--手机登录
SUB_GR_LOGON_SUCCESS=100									#//登录成功
SUB_GR_LOGON_FAILURE=101									#//登录失败

MDM_GR_USER					=3									#//用户信息
SUB_GR_USER_SITDOWN	=3									#//坐下请求
SUB_GR_USER_STANDUP=4									#//起立请求
SUB_GF_USER_READY=2							#		//用户准备
SUB_GR_REQUEST_FAILURE	=103									#//请求失败
SUB_GR_USER_SIT_SUCCESS=105									#//用户游戏数据
MDM_GF_FRAME=100									#//框架命令

MDM_GF_GAME					= 200
SUB_C_SEND_CARD = 1									  # // --用户叫庄
SUB_C_RESET_TABLE = 2									  # // --用户叫庄
SOCKET_TCP_BUFFER=16384
DK_MAPPED		=0x01								#//映射类型
DK_ENCRYPT		=0x02								#//加密类型
DK_COMPRESS		=0x04								#//压缩类型

MDM_KN_COMMAND = 0  # //内核命令
SUB_KN_DETECT_SOCKET = 1  # //检测命令
# 处理用户登录
import struct
class TCP_Head(Structure):
    _fields_ = [("cbDataKind", c_ubyte ),
                ("cbCheckCode", c_ubyte),
                ("wPacketSize", c_ushort ),
                ("wMainCmdID", c_ushort),
                ("wSubCmdID", c_ushort)
                ]
    _pack_ = 1
class CMD_GR_LogonUserID(Structure):
    _fields_ = [("dwUserID", c_int64 ),
                ("password", c_int64),
                ("szMachineID", c_wchar * 128)
                ]
    _pack_ = 1
class CMD_GR_LogonSuccess(Structure):
    _fields_ = [("wTableID", c_ulong),
                ("wChairID", c_ulong),
                ]
    _pack_ = 1

class CMD_GR_LogonFailure(Structure):
    _fields_ = [("lErrorCode", c_ulong),
                ("szDescribeString", c_wchar*128),
                ]
    _pack_ = 1

class cmd_cardDataInfo(Structure):
    _fields_ = [
                ("wChairID", c_ushort),
                ("cbCardData", c_ubyte* 20),
                ("cbCardCount", c_ubyte),
                ("cbCardDataEx", c_ubyte * 200),
                ("cbCardExCount", c_ubyte),
                ]
    _pack_ = 1
class cmd_reSetTable(Structure):
    _fields_ = [
                ("wChairID", c_ushort),
                ("status", c_ubyte),
                ]
    _pack_ = 1
class CMD_GR_UserSitDown(Structure):
    _fields_ = [("wTableID", c_ushort),
                ("wChairID", c_ushort),
                ]
    _pack_ = 1
def to_byte(num, bits=None):
    if bits is None:
        bits = 8 # 给一个默认的的位长
    return num.to_bytes(bits // 8, byteorder='big')
def sendData(client,wMainCmdID,wSubCmdID,data):
    if not client:
        return
    tcp_info = TCP_Head()
    headSize=(sizeof(tcp_info))
    loginSize=(sizeof(data))
    totalSize=headSize+loginSize
    tcp_info.cbDataKind=DK_MAPPED
    tcp_info.wMainCmdID=wMainCmdID
    tcp_info.wSubCmdID = wSubCmdID
    tcp_info.wPacketSize=totalSize
    tmpData2=bytearray(tcp_info)+bytearray(data)
    #加密用的
    gameData=EncryptBuffer(tmpData2,4,True)
    client.send(gameData)
    a=4
def EncryptBuffer(data,index,bClient):
    gameData = bytearray()
    for i in range(0, len(data)):
        if i < index:
            gameData = gameData + data[i].to_bytes(1, byteorder='little', signed=False)
        else:
            number=0
            if bClient:
                number = settings.g_SendByteMap[data[i]]
            else:
                number = settings.g_RecvByteMap[data[i]]
            byte_number = number.to_bytes(1, byteorder='little', signed=False)
            gameData = gameData + byte_number
    return gameData
def recveData(client):
    header = client.recv(8)
    if not header:
        client.close()
        return None, None
    tmpdata = bytearray() + header
    if len(tmpdata)<8:
        return None,None
    head_info = TCP_Head.from_buffer(tmpdata)
    # message_length=struct.unpack("<2B3H", header)
    total_length = head_info.wPacketSize-8
    if total_length == 0 or head_info.wMainCmdID==MDM_KN_COMMAND :
        return None,None
    data_list = []
    received_length = 0
    while received_length < total_length:
        data = client.recv(total_length - received_length)
        received_length += len(data)
        data_list.append(data)
    complete_data = bytearray().join(data_list)
    gameData = EncryptBuffer(complete_data,0, False)
    return  head_info,gameData
    a=4
