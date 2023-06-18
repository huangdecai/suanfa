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
import threading
from win32con import WM_LBUTTONDOWN, MK_LBUTTON, WM_LBUTTONUP, WM_MOUSEMOVE
import multiprocessing as mp

from PyQt5 import QtGui, QtWidgets, QtCore
from PyQt5.QtCore import QTime, QEventLoop

Pics = {}
ReqQueue = mp.Queue()
ResultQueue = mp.Queue()
Processes = []
SCREEN_WIDTH=1320
SCREEN_HEGIHT=755
MAX_CARD_COUNT=16
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

    def Screenshot(self, region=None):  # -> (im, (left, top))
        self.Handle = win32gui.FindWindow("LDPlayerMainFrame", "雷电模拟器")
        hwnd = self.Handle
        if self.bTest==True:
            im = Image.open(r"testRunFast2.png")
            im = im.resize((SCREEN_WIDTH, SCREEN_HEGIHT))
            return im, (0,0)
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
            self.LeftClick((result[0]+60, result[1]-20))

    def GetCardsState(self, image,handCount):
        st = time.time()
        imgCv = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
        states = []
        cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                        region=(1, 527, 880, 110), confidence=0.80)
        if cardStartPos is None:
            return []
        sx = cardStartPos[0] #+ 23
        cardSearchFrom = 0
        sy, sw, sh = 160, 72, 55
        if handCount==15:
            sw=77
        elif handCount == 14:
                sw = 81
        elif handCount <= 13:
                sw = 86
        spaceX = sw
        spaceY = 528
        for i in range(0, MAX_CARD_COUNT):
            temp_x=sx+6 + spaceX *(i)
            if temp_x>=(SCREEN_WIDTH-50) :
                break
            print("GetCardsState-LocateOnImage:", temp_x, sw)
            checkSelect = 0
            if i==11 :
                a=4
            result = LocateOnImage(imgCv, self.PicsCV["card_overlap"], region=(temp_x, spaceY - 35, spaceX, 30),
                                   confidence=0.80)
            result2 = LocateOnImage(imgCv, self.PicsCV["card_overlap2"], region=(temp_x, spaceY - 15, spaceX, 40),
                                    confidence=0.80)
            # result3 = LocateOnImage(imgCv, self.PicsCV["card_overlap3"], region=(temp_x, spaceY - 10, spaceX, 40),confidence=0.85)
            if (result) and ((result2 is None)):
                checkSelect = 1
            states.append(checkSelect)
        print("GetStates Costs ", time.time()-st)
        return states
    def GetCards(self, image,handCount):
        st = time.time()
        imgCv = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
        tryCount = 10
        cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                        region=(1, 527, 880, 110), confidence=0.80)
        while cardStartPos is None and tryCount > 0:
            cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge"], haystackImage=image,
                                            region=(1, 527, 880, 110), confidence=0.80)
            print("找不到手牌起始位置")
            tryCount -= 1
            #time.sleep(150)
        print("start pos", cardStartPos)
        if cardStartPos is None:
            return [],[]
        sx = cardStartPos[0] #+ 23
        AllCardsNC = ['rD', 'bX', '2', 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3']
        hand_cards = []
        select_map = []
        cardSearchFrom = 0
        sy, sw, sh = 160, 72, 55
        if handCount==15:
            sw=77
        elif handCount == 14:
                sw = 81
        elif handCount <= 13:
                sw = 86
        spaceX=sw
        spaceY = 528

        for i in range(0, MAX_CARD_COUNT):

            temp_x = sx+4 + spaceX *( i)
            if temp_x >= (SCREEN_WIDTH - 50):
                break
            checkSelect = 0
            result = LocateOnImage(imgCv, self.PicsCV["card_overlap"], region=(temp_x, spaceY-35, spaceX, 30), confidence=0.80)
            result2 = LocateOnImage(imgCv, self.PicsCV["card_overlap2"], region=(temp_x, spaceY-15, spaceX, 40), confidence=0.80)
            #result3 = LocateOnImage(imgCv, self.PicsCV["card_overlap3"], region=(temp_x, spaceY - 10, spaceX, 40),confidence=0.85)
            if (result ) and ((result2 is None )):
                checkSelect = 1
            select_map.append(checkSelect)
            currCard = ""
            forBreak = False
            ci = cardSearchFrom
            while ci < len(AllCardsNC):
                if "r" in AllCardsNC[ci] or "b" in AllCardsNC[ci]:
                    outerBreak = False
                    result = LocateOnImage(imgCv, self.PicsCV["m" + AllCardsNC[ci]], region=(sx + spaceX * i, spaceY- checkSelect * 25, sw, 68), confidence=0.89)

                    if result is not None:
                        cardPos = (sx + spaceX * i + sw // 2, spaceY - checkSelect * 25 + sh // 2)
                        cardSearchFrom = ci
                        currCard = AllCardsNC[ci][1]
                        cardInfo = (currCard, cardPos)
                        hand_cards.append(cardInfo)
                        outerBreak = True
                        break
                else:
                    outerBreak = False
                    for card_type in ["r", "b"]:
                        result = LocateOnImage(imgCv, self.PicsCV["m" + card_type + AllCardsNC[ci]], region=(sx + spaceX * i, spaceY - checkSelect * 25, sw, 68), confidence=0.91)
                        if result is not None:
                            cardPos = (sx + spaceX * i + sw // 2, spaceY - checkSelect * 25 + sh // 2)
                            cardSearchFrom = ci
                            currCard = AllCardsNC[ci]
                            cardInfo = (currCard, cardPos)
                            hand_cards.append(cardInfo)
                            outerBreak = True
                            break
                    if outerBreak:
                        break
                    if ci == len(AllCardsNC) - 1 and checkSelect == 0:
                        checkSelect = 1
                        ci = cardSearchFrom - 1
                ci += 1
                if ci == len(AllCardsNC):
                    forBreak = True
            if forBreak:
                break
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 10)
        print("GetCards Costs ", time.time()-st)
        print("cardPos:", len(hand_cards))
        for i in range(0,len(hand_cards)):
            x,y=hand_cards[i][1]
            print("cardPos:",i,x,y)
        return hand_cards, select_map
    def GetCardsEx(self, image, pos):
        st = time.time()
        imgCv = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)
        tryCount = 10
        cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge2"], haystackImage=image,
                                        region=pos, confidence=0.85)
        while cardStartPos is None and tryCount > 0:
            cardStartPos = pyautogui.locate(needleImage=self.Pics["card_edge2"], haystackImage=image,
                                            region=pos, confidence=0.85)
            print("找不到对方手牌起始位置")
            tryCount -= 1
            # time.sleep(150)
        print("start pos", cardStartPos)
        if cardStartPos is None:
            return [], []
        sx = cardStartPos[0]+ 3  # + 23
        AllCardsNC = ['rD', 'bX', '2', 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3']
        hand_cards = []
        select_map = []
        cardSearchFrom = 0
        sy, sw, sh = 160, 28, 55
        spaceX = sw
        spaceY = pos[1]

        for i in range(0, MAX_CARD_COUNT):

            temp_x = sx +  spaceX * (i)
            if temp_x >= (SCREEN_WIDTH - 50):
                break
            currCard = ""
            forBreak = False
            ci = cardSearchFrom
            checkSelect=0
            while ci < len(AllCardsNC):
                if "r" in AllCardsNC[ci] or "b" in AllCardsNC[ci]:
                    outerBreak = False
                    result = LocateOnImage(imgCv, self.PicsCV["o" + AllCardsNC[ci]],
                                           region=(sx + spaceX * i, spaceY - checkSelect * 25, sw, 68), confidence=0.85)

                    if result is not None:
                        cardPos = (sx + spaceX * i + sw // 2, spaceY - checkSelect * 25 + sh // 2)
                        cardSearchFrom = ci
                        currCard = AllCardsNC[ci][1]
                        cardInfo = (currCard, cardPos)
                        hand_cards.append(cardInfo)
                        outerBreak = True
                        break
                else:
                    outerBreak = False
                    for card_type in ["r", "b"]:
                        result = LocateOnImage(imgCv, self.PicsCV["o" + card_type + AllCardsNC[ci]],
                                               region=(sx + spaceX * i, spaceY - checkSelect * 25, sw, 68),
                                               confidence=0.86)
                        if result is not None:
                            cardPos = (sx + spaceX * i + sw // 2, spaceY - checkSelect * 25 + sh // 2)
                            cardSearchFrom = ci
                            currCard = AllCardsNC[ci]
                            cardInfo = (currCard, cardPos)
                            hand_cards.append(cardInfo)
                            outerBreak = True
                            break
                    if outerBreak:
                        break
                ci += 1
                if ci == len(AllCardsNC):
                    forBreak = True
            if forBreak:
                break
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 10)
        print("GetCards Costs ", time.time() - st)
        print("cardPos:", len(hand_cards))
        for i in range(0, len(hand_cards)):
            x, y = hand_cards[i][1]
            print("cardPos:", i, x, y)
        return hand_cards, select_map
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
        cards = [card for card in cards]
        tobeSelected = []
        tobeSelected.extend(cards)
        image, windowPos = self.Screenshot()
        while image.size[0] == 0:
            image, windowPos = self.Screenshot()
        handCardsInfo, states = self.GetCards(image,handCount)
        cardSelectMap = []
        #for card in handCardsInfo:
        #改成倒序的原因是有的平台点第二张，第三张跟着起来
        for i in range(len(handCardsInfo),0,-1 ):
            c = handCardsInfo[i-1][0]
            if c in tobeSelected:
                cardSelectMap.append(1)
                tobeSelected.remove(c)
            else:
                cardSelectMap.append(0)
        cardSelectMap=cardSelectMap[::-1]
        clickMap = []
        handcards = [c[0] for c in handCardsInfo]
        for i in range(0, len(cardSelectMap)):
            if cardSelectMap[i] == states[i]:
                clickMap.append(0)
            else:
                clickMap.append(1)
        while 1 in clickMap:
            for i in range(0, len(clickMap)):
                if clickMap[i] == 1:
                    self.LeftClick(handCardsInfo[i][1])
                    print("点击", handCardsInfo[i][1])
                    break

            time.sleep(0.2)  #这里不能设置过快，否则会造成上面点击的扑克弹起了，但是下面得到的截图是没弹起的
            if self.Interrupt:
                break
            if no_check:
                return
            image, _ = self.Screenshot()
            while image.size[0] == 0:
                image, windowPos = self.Screenshot()
            states = self.GetCardsState(image,handCount)
            clickMap = []
            for i in range(0, len(cardSelectMap)):
                if cardSelectMap[i] == states[i]:
                    clickMap.append(0)
                else:
                    clickMap.append(1)
            QtWidgets.QApplication.processEvents(QEventLoop.AllEvents, 10)