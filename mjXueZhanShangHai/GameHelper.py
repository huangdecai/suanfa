# -*- coding: utf-8 -*-
# Created by: Vincentzyx
import win32gui
import win32ui
import win32api
from ctypes import windll
from PIL import Image
import cv2
import pyautogui
import matplotlib.pyplot as plt
import numpy as np
import os
import time
import win32con
import threading
from win32con import WM_LBUTTONDOWN, MK_LBUTTON, WM_LBUTTONUP, WM_MOUSEMOVE,WM_LBUTTONDBLCLK,WM_MBUTTONDBLCLK
import multiprocessing as mp

from PyQt5 import QtGui, QtWidgets, QtCore
from PyQt5.QtCore import QTime, QEventLoop

Pics = {}
ReqQueue = mp.Queue()
ResultQueue = mp.Queue()
Processes = []
SCREEN_WIDTH=1320
SCREEN_HEGIHT=755
MAX_CARD_COUNT=14
def GetSingleCardQueue(reqQ, resQ, Pics):
    while True:
        while not reqQ.empty():
            image, i, sx, sy, sw, sh, checkSelect = reqQ.get()
            result = GetSingleCard(image, i, sx, sy, sw, sh, checkSelect, Pics)
            del image
            if result is not None:
                resQ.put(result)
        time.sleep(0.01)

def ShowImg(image):
    plt.imshow(image)
    plt.show()

def DrawRectWithText(image, rect, text):
    img = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
    x, y, w, h = rect
    img2 = cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
    img2 = cv2.putText(img2, text, (x, y + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)
    return Image.fromarray(cv2.cvtColor(img2, cv2.COLOR_BGR2RGB))

def CompareCard(card):
    order = {"3": 0, "4": 1, "5": 2, "6": 3, "7": 4, "8": 5, "9": 6, "T": 7, "J": 8, "Q": 9, "K": 10, "A": 11, "2": 12,
             "X": 13, "D": 14}
    return order[card]

def CompareCardInfo(card):
    order = {"3": 0, "4": 1, "5": 2, "6": 3, "7": 4, "8": 5, "9": 6, "T": 7, "J": 8, "Q": 9, "K": 10, "A": 11, "2": 12,
             "X": 13, "D": 14}
    return order[card[0]]

def CompareCards(cards1, cards2):
    if len(cards1) != len(cards2):
        return False
    cards1.sort(key=CompareCard)
    cards2.sort(key=CompareCard)
    for i in range(0, len(cards1)):
        if cards1[i] != cards2[i]:
            return False
    return True

def GetListDifference(l1, l2):
    temp1 = []
    temp1.extend(l1)
    temp2 = []
    temp2.extend(l2)
    for i in l2:
        if i in temp1:
            temp1.remove(i)
    for i in l1:
        if i in temp2:
            temp2.remove(i)
    return temp1, temp2

def FindImage(fromImage, template, threshold=0.9):
    w, h, _ = template.shape
    fromImage = cv2.cvtColor(np.asarray(fromImage), cv2.COLOR_RGB2BGR)
    res = cv2.matchTemplate(fromImage, template, cv2.TM_CCOEFF_NORMED)
    loc = np.where(res >= threshold)
    points = []
    for pt in zip(*loc[::-1]):
        points.append(pt)
    return points

def GetSingleCard(image, i, sx, sy, sw, sh, checkSelect, Pics):
    cardSearchFrom = 0
    AllCardsNC = ['rD', 'bX', '2', 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3']
    currCard = ""
    ci = cardSearchFrom
    while ci < len(AllCardsNC):
        if "r" in AllCardsNC[ci] or "b" in AllCardsNC[ci]:
            result = pyautogui.locate(needleImage=Pics["m" + AllCardsNC[ci]], haystackImage=image,
                                      region=(sx + 50 * i, sy - checkSelect * 25, sw, sh), confidence=0.9)
            if result is not None:
                cardPos = (sx + 50 * i + sw // 2, sy - checkSelect * 25 + sh // 2)
                cardSearchFrom = ci
                currCard = AllCardsNC[ci][1]
                cardInfo = (currCard, cardPos)
                return cardInfo
                break
        else:
            outerBreak = False
            for card_type in ["r", "b"]:
                result = pyautogui.locate(needleImage=Pics["m" + card_type + AllCardsNC[ci]],
                                          haystackImage=image,
                                          region=(sx + 50 * i, sy - checkSelect * 25, sw, sh), confidence=0.9)
                if result is not None:
                    cardPos = (sx + 50 * i + sw // 2, sy - checkSelect * 25 + sh // 2)
                    cardSearchFrom = ci
                    currCard = AllCardsNC[ci]
                    cardInfo = (currCard, cardPos)
                    outerBreak = True
                    return cardInfo
                    break
            if outerBreak:
                break
            if ci == len(AllCardsNC) - 1 and checkSelect == 0:
                checkSelect = 1
                ci = cardSearchFrom - 1
        ci += 1
    return None

def RunThreads():
    for file in os.listdir("pics"):
        info = file.split(".")
        if info[1] == "png":
            tmpImage = Image.open("pics/" + file)
            Pics.update({info[0]: tmpImage})
    for ti in range(20):
        p = mp.Process(target=GetSingleCardQueue, args=(ReqQueue, ResultQueue, Pics))
        p.start()


def LocateOnImage(image, template, region=None, confidence=0.9):
    if region is not None:
        x, y, w, h = region
        imgShape = image.shape
        image = image[y:y+h, x:x+w,:]
    res = cv2.matchTemplate(image, template, cv2.TM_CCOEFF_NORMED)
    if (res >= confidence).any():
        return True
    else:
        return None

def LocateAllOnImage(image, template, region=None, confidence=0.9):
    if region is not None:
        x, y, w, h = region
        imgShape = image.shape
        image = image[y:y+h, x:x+w,:]
    w, h = image.shape[1], image.shape[0]
    res = cv2.matchTemplate(image, template, cv2.TM_CCOEFF_NORMED)
    loc = np.where( res >= confidence)
    points = []
    for pt in zip(*loc[::-1]):
        points.append((pt[0], pt[1], w, h))
    return points


class GameHelper:
    def __init__(self):
        self.ScreenZoomRate = 1.0
        self.Pics = {}
        self.PicsCV = {}
        self.Handle = win32gui.FindWindow("LDPlayerMainFrame", "雷电模拟器")
        self.Interrupt = False
        self.RealRate = (SCREEN_WIDTH, SCREEN_HEGIHT)
        for file in os.listdir("./pics"):
            info = file.split(".")
            if info[1] == "png":
                tmpImage = Image.open("./pics/" + file)
                imgCv = cv2.imread("./pics/" + file)
                self.Pics.update({info[0]: tmpImage})
                self.PicsCV.update({info[0]: imgCv})
        self.duokaiStr = "雷电模拟器"
    def setFindStr(self,findStr):
        if findStr!='0':
            self.duokaiStr = "雷电模拟器"
            self.duokaiStr=self.duokaiStr+"-"+(findStr)
            tmp=4
    def Screenshot(self, region=None):  # -> (im, (left, top))
        self.Handle = win32gui.FindWindow("LDPlayerMainFrame", self.duokaiStr)
        hwnd = self.Handle
        if self.bTest == True:
            im = Image.open(r"testRunFast2.png")
            im = im.resize((SCREEN_WIDTH, SCREEN_HEGIHT))
            return im, (0, 0)
        left, top, right, bot = win32gui.GetWindowRect(hwnd)
        width = right - left
        height = bot - top
        self.RealRate = (width, height)
        width = int(width / self.ScreenZoomRate)
        height = int(height / self.ScreenZoomRate)
        hwndDC = win32gui.GetWindowDC(hwnd)
        mfcDC = win32ui.CreateDCFromHandle(hwndDC)
        saveDC = mfcDC.CreateCompatibleDC()
        saveBitMap = win32ui.CreateBitmap()
        saveBitMap.CreateCompatibleBitmap(mfcDC, width, height)
        saveDC.SelectObject(saveBitMap)
        result = windll.user32.PrintWindow(hwnd, saveDC.GetSafeHdc(), 0)
        bmpinfo = saveBitMap.GetInfo()
        bmpstr = saveBitMap.GetBitmapBits(True)
        im = Image.frombuffer(
            "RGB",
            (bmpinfo['bmWidth'], bmpinfo['bmHeight']),
            bmpstr, 'raw', 'BGRX', 0, 1)
        win32gui.DeleteObject(saveBitMap.GetHandle())
        saveDC.DeleteDC()
        mfcDC.DeleteDC()
        win32gui.ReleaseDC(hwnd, hwndDC)
        im = im.resize((SCREEN_WIDTH, SCREEN_HEGIHT))
        im.save("testRunFast.png")
        if region is not None:
            im = im.crop((region[0], region[1], region[0] + region[2], region[1] + region[3]))
        if result:
            return im, (left, top)
        else:
            return None, (0, 0)

    def LocateOnScreen(self, templateName, region, confidence=0.9):
        image, _ = self.Screenshot()
        return pyautogui.locate(needleImage=self.Pics[templateName],
                                haystackImage=image, region=region, confidence=confidence)

    def ClickOnImage(self, templateName, region=None, confidence=0.9):
        image, _ = self.Screenshot()
        result = pyautogui.locate(needleImage=self.Pics[templateName], haystackImage=image, confidence=confidence, region=region)
        if result is not None:
            self.LeftClick((result[0]+20, result[1]-15))
    def ClickOnImageEx(self, templateName, region=None, confidence=0.9):
        image, _ = self.Screenshot()
        result = pyautogui.locate(needleImage=self.Pics[templateName], haystackImage=image, confidence=confidence, region=region)
        if result is not None:
            self.LeftClick((result[0]+20, result[1]+15))
    def GetCardsState(self, image,handCount):
        st = time.time()
        imgCv = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
        states = []
        cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                        region=(2, 470, 730, 90), confidence=0.80)
        # if cardStartPos is None:
        #     cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge2"], haystackImage=image,
        #                                     region=(2, 465, 900, 90), confidence=0.83)
        if cardStartPos is None:
            return []
        sx = cardStartPos[0]+8 #+ 23
        cardSearchFrom = 0
        sy, sw, sh = 160, 66, 90
        spaceX = 63
        spaceY = 476
        for i in range(0, MAX_CARD_COUNT):
            temp_x = sx + (spaceX - 1) * (i)
            if i == 13:
                temp_x += 5
            if temp_x >= (SCREEN_WIDTH - 80):
                break
            print("GetCardsState-LocateOnImage:", temp_x, sw)
            checkSelect = 0
            result = LocateOnImage(imgCv, self.PicsCV["card_overlap"], region=(temp_x ,spaceY-35, 80, 20), confidence=0.80)
            if (result is not None) :
                checkSelect = 1
            states.append(checkSelect)
        print("GetStates Costs ", time.time()-st)
        return states

    def \
            GetCards(self, image,bHaveAction=False):
        st = time.time()
        imgCv = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
        tryCount = 10
        cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                        region=(2, 615, 780, 30), confidence=0.80)
        # if cardStartPos is None:
        #     cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge2"], haystackImage=image,
        #                                     region=(2, 465, 900, 90), confidence=0.75)
        while cardStartPos is None and tryCount > 0:
            cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                            region=(2, 615, 780, 30), confidence=0.80)
            print("找不到手牌起始位置")
            tryCount -= 1
            #time.sleep(150)
        print("start pos", cardStartPos)
        if cardStartPos is None:
            return [],[]
        sx = cardStartPos[0]+6 #+ 23
        spaceIndex=13
        if sx>285 :
            spaceIndex=10
        if sx>540 :
            spaceIndex = 7
        if sx > 790:
            spaceIndex = 4
        if sx > 1020:
            spaceIndex = 1
        AllCardsNC = ['33','32','31','30','29','28','27','26','25','24','23','22','21','20','19','18','17','16','15','14','13','12', '11', '10', '9', '8', '7', '6', '5', '4', '3', '2', '1','0']
        #AllCardsNC=AllCardsNC[::-1]
        hand_cards = []
        select_map = []
        laiZi_cards = []
        cardSearchFrom = 0
        sy, sw, sh = 160, 86, 120
        spaceX=sw
        #
        spaceY = 618 #

        for i in range(0, MAX_CARD_COUNT):

            temp_x = sx + (spaceX-1) *( i)
            if i==10 :
                a=4
            if i==spaceIndex :
                temp_x+=19
            if temp_x >= (SCREEN_WIDTH - 80):
                break
            checkSelect = 0
            result = LocateOnImage(imgCv, self.PicsCV["card_overlap"], region=(temp_x ,spaceY-35, 80, 20), confidence=0.80)
            if (result is not None) :
                checkSelect = 0
            select_map.append(checkSelect)
            currCard = ""
            forBreak = False
            ci = 0
            while ci < len(AllCardsNC):
                print("GetCards： ",ci,temp_x,spaceY - checkSelect * 25,sw,)
                outerBreak = False
                result = LocateOnImage(imgCv, self.PicsCV[AllCardsNC[ci]], region=( temp_x, spaceY - checkSelect * 25, sw, sh), confidence=0.85)
                if result is not None:
                    cardPos = (sx + spaceX * i + sw // 2, spaceY - checkSelect * 25 + sh // 2)
                    cardSearchFrom = ci
                    currCard = AllCardsNC[ci]
                    cardInfo = (currCard, cardPos)
                    hand_cards.append(cardInfo)
                    # if i<=1 or  i==spaceIndex :
                    #     laiZiresult = LocateOnImage(imgCv, self.PicsCV["laizi"], region=(temp_x, spaceY, 73, 50),
                    #                            confidence=0.75)
                    #     if laiZiresult:
                    #         if len(laiZi_cards)>0:
                    #             if laiZi_cards[0]!=currCard:
                    #                 laiZi_cards.append(currCard)
                    #         else:
                    #             laiZi_cards.append(currCard)
                    break
                ci += 1
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 10)
        print("GetCards Costs ", time.time()-st)
        print("cardPos:", len(hand_cards))
        for i in range(0,len(hand_cards)):
            x,y=hand_cards[i][1]
            print("cardPos:",i,x,y)
        return hand_cards, laiZi_cards

    def LeftClick(self, pos):
        x, y = pos
        #x = (x / 1796) * self.RealRate[0]
        #y = (y / 1047) * self.RealRate[1]
        x = int(x)
        y = int(y)
        print("LeftClick:", x, y)
        lParam = win32api.MAKELONG(x, y)
        tmpHandle = self.Handle
        hwndChildList = []
        win32gui.EnumChildWindows(tmpHandle, lambda hwnd, param: param.append(hwnd), hwndChildList)
        tmpHandle = hwndChildList[0]
        win32gui.PostMessage(tmpHandle, WM_MOUSEMOVE, MK_LBUTTON, lParam)
        win32gui.PostMessage(tmpHandle, WM_LBUTTONDOWN, MK_LBUTTON, lParam)
        win32gui.PostMessage(tmpHandle, WM_LBUTTONUP, MK_LBUTTON, lParam)

    def click_drag(self,pos) :
        """Click at pixel xy."""
        x, y = pos
        x = int(x)
        y = int(y)
        lParam = win32api.MAKELONG(x, y)
        lParam2 = win32api.MAKELONG(x, y - 200)
        tmpHandle = self.Handle
        hwndChildList = []
        win32gui.EnumChildWindows(tmpHandle, lambda hwnd, param: param.append(hwnd), hwndChildList)
        tmpHandle = hwndChildList[0]
        # MOUSEMOVE event is required for game to register clicks correctly
        win32gui.PostMessage(tmpHandle, win32con.WM_MOUSEMOVE, 0, lParam)
        time.sleep(0.1)
        win32gui.PostMessage(tmpHandle, win32con.WM_LBUTTONDOWN,
                             win32con.MK_LBUTTON, lParam)
        time.sleep(0.1)
        win32gui.PostMessage(tmpHandle, win32con.WM_MOUSEMOVE, 0, lParam2)
        time.sleep(0.1)
        win32gui.PostMessage(tmpHandle, win32con.WM_LBUTTONUP,
                             win32con.MK_LBUTTON, lParam2)
    def DoubleClick(self, pos):
        x, y = pos
        # x = (x / 1796) * self.RealRate[0]
        # y = (y / 1047) * self.RealRate[1]
        x = int(x)
        y = int(y)
        print("LeftClick:", x, y)
        lParam = win32api.MAKELONG(x, y)
        tmpHandle = self.Handle
        hwndChildList = []
        win32gui.EnumChildWindows(tmpHandle, lambda hwnd, param: param.append(hwnd), hwndChildList)
        tmpHandle = hwndChildList[0]
        #win32gui.PostMessage(tmpHandle, WM_MOUSEMOVE, MK_LBUTTON, lParam)
        win32gui.PostMessage(tmpHandle, WM_LBUTTONDBLCLK, MK_LBUTTON, lParam)
        #win32gui.PostMessage(tmpHandle, WM_LBUTTONUP, MK_LBUTTON, lParam)
    def SelectCardsEx(self, cards,handCount):
        image, windowPos = self.Screenshot()
        while image.size[0] == 0:
            image, windowPos = self.Screenshot()
        handCardsInfo, states = self.GetCards(image)
        cardSelectMap = []
        for j in range(0, len(handCardsInfo)):
            c = handCardsInfo[j][0]  # card[0]
            bExist = False
            for i in range(0, len(cards)):
                if c == cards[i]:
                    print("双击1", handCardsInfo[j][1])
                    self.click_drag(handCardsInfo[j][1])
                    #self.LeftClick(handCardsInfo[i][1])
                    #self.LeftClick(handCardsInfo[i][1])
                    tmpPos=handCardsInfo[j][1]
                    bExist=True
                    break
            if bExist:
                break
    def LeftClickEX(self, pos):
        x, y = pos
        #x = (x / 1796) * self.RealRate[0]
        #y = (y / 1047) * self.RealRate[1]
        x = int(x)
        y = int(y)
        lParam = win32api.MAKELONG(x, y)
        tmpHandle = self.Handle
        hwndChildList = []
        win32gui.EnumChildWindows(tmpHandle, lambda hwnd, param: param.append(hwnd), hwndChildList)
        tmpHandle=hwndChildList[0]
        win32gui.PostMessage(tmpHandle, WM_MOUSEMOVE, MK_LBUTTON, lParam)
        win32gui.PostMessage(tmpHandle, WM_LBUTTONDOWN, MK_LBUTTON, lParam)
        win32gui.PostMessage(tmpHandle, WM_LBUTTONUP, MK_LBUTTON, lParam)

    def SelectCards(self, cards,handCount, no_check=False):
        print("选择牌", cards)
        image, windowPos = self.Screenshot()
        while image.size[0] == 0:
            image, windowPos = self.Screenshot()
        handCardsInfo, states, colors = self.GetCards(image)
        cardSelectMap = []
        for j in range(0, len(handCardsInfo)):
            c = handCardsInfo[j][0]  # card[0]
            bExist = False
            for i in range(0, len(cards)):
                if c == cards[i]:
                    print("双击1", handCardsInfo[j][1])
                    # self.click_drag(handCardsInfo[j][1])
                    self.LeftClick(handCardsInfo[i][1])
                    time.sleep(0.5)
                    self.LeftClick(handCardsInfo[i][1])
                    #self.DoubleClick(handCardsInfo[i][1])
                    tmpPos = handCardsInfo[j][1]
                    bExist = True
                    break
            if bExist:
                break
        QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 1500)