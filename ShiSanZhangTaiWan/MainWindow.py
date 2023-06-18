# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MainWindow.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(703, 421)
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(9)
        font.setBold(True)
        font.setItalic(False)
        font.setWeight(75)
        Form.setFont(font)
        Form.setWindowOpacity(1.0)
        self.WinRate = QtWidgets.QLabel(Form)
        self.WinRate.setGeometry(QtCore.QRect(480, 250, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.WinRate.setFont(font)
        self.WinRate.setAlignment(QtCore.Qt.AlignCenter)
        self.WinRate.setObjectName("WinRate")
        self.InitCard = QtWidgets.QPushButton(Form)
        self.InitCard.setGeometry(QtCore.QRect(80, 360, 121, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.InitCard.setFont(font)
        self.InitCard.setStyleSheet("")
        self.InitCard.setObjectName("InitCard")
        self.UserHandCards = QtWidgets.QLabel(Form)
        self.UserHandCards.setGeometry(QtCore.QRect(40, 160, 421, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.UserHandCards.setFont(font)
        self.UserHandCards.setAlignment(QtCore.Qt.AlignCenter)
        self.UserHandCards.setObjectName("UserHandCards")
        self.LPlayer = QtWidgets.QFrame(Form)
        self.LPlayer.setGeometry(QtCore.QRect(20, 80, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(9)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.LPlayer.setFont(font)
        self.LPlayer.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.LPlayer.setFrameShadow(QtWidgets.QFrame.Raised)
        self.LPlayer.setObjectName("LPlayer")
        self.LPlayedCard = QtWidgets.QLabel(self.LPlayer)
        self.LPlayedCard.setGeometry(QtCore.QRect(0, 0, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.LPlayedCard.setFont(font)
        self.LPlayedCard.setAlignment(QtCore.Qt.AlignCenter)
        self.LPlayedCard.setObjectName("LPlayedCard")
        self.RPlayer = QtWidgets.QFrame(Form)
        self.RPlayer.setGeometry(QtCore.QRect(250, 80, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(16)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.RPlayer.setFont(font)
        self.RPlayer.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.RPlayer.setFrameShadow(QtWidgets.QFrame.Raised)
        self.RPlayer.setObjectName("RPlayer")
        self.RPlayedCard = QtWidgets.QLabel(self.RPlayer)
        self.RPlayedCard.setGeometry(QtCore.QRect(0, 0, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.RPlayedCard.setFont(font)
        self.RPlayedCard.setAlignment(QtCore.Qt.AlignCenter)
        self.RPlayedCard.setObjectName("RPlayedCard")
        self.Player = QtWidgets.QFrame(Form)
        self.Player.setGeometry(QtCore.QRect(480, 80, 201, 61))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(9)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.Player.setFont(font)
        self.Player.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.Player.setFrameShadow(QtWidgets.QFrame.Raised)
        self.Player.setObjectName("Player")
        self.PredictedCard = QtWidgets.QLabel(Form)
        self.PredictedCard.setGeometry(QtCore.QRect(490, 20, 201, 200))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(24)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.PredictedCard.setFont(font)
        self.PredictedCard.setStyleSheet("")
        self.PredictedCard.setFrameShape(QtWidgets.QFrame.Panel)
        self.PredictedCard.setLineWidth(1)
        self.PredictedCard.setAlignment(QtCore.Qt.AlignCenter)
        self.PredictedCard.setObjectName("PredictedCard")
        self.ThreeLandlordCards = QtWidgets.QLabel(Form)
        self.ThreeLandlordCards.setGeometry(QtCore.QRect(100, 0, 300, 100))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(16)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.ThreeLandlordCards.setFont(font)
        self.ThreeLandlordCards.setAlignment(QtCore.Qt.AlignCenter)
        self.ThreeLandlordCards.setObjectName("ThreeLandlordCards")
        self.Stop = QtWidgets.QPushButton(Form)
        self.Stop.setGeometry(QtCore.QRect(230, 360, 111, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.Stop.setFont(font)
        self.Stop.setStyleSheet("")
        self.Stop.setObjectName("Stop")
        self.SwitchMode = QtWidgets.QPushButton(Form)
        self.SwitchMode.setGeometry(QtCore.QRect(370, 360, 121, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.SwitchMode.setFont(font)
        self.SwitchMode.setStyleSheet("")
        self.SwitchMode.setObjectName("SwitchMode")
        self.AutoStart = QtWidgets.QPushButton(Form)
        self.AutoStart.setGeometry(QtCore.QRect(520, 360, 111, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.AutoStart.setFont(font)
        self.AutoStart.setStyleSheet("")
        self.AutoStart.setObjectName("AutoStart")

        self.AutoFast = QtWidgets.QPushButton(Form)
        #self.AutoFast.setGeometry(QtCore.QRect(520, 360, 111, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(14)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.AutoFast.setFont(font)
        self.AutoFast.setStyleSheet("")
        self.AutoFast.setObjectName("AutoFast")

        self.BidWinrate = QtWidgets.QLabel(Form)
        self.BidWinrate.setGeometry(QtCore.QRect(50, 220, 241, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.BidWinrate.setFont(font)
        self.BidWinrate.setObjectName("BidWinrate")
        self.PreWinrate = QtWidgets.QLabel(Form)
        self.PreWinrate.setGeometry(QtCore.QRect(50, 270, 241, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.PreWinrate.setFont(font)
        self.PreWinrate.setObjectName("PreWinrate")

        self.retranslateUi(Form)
        self.InitCard.clicked.connect(Form.init_cards)
        self.Stop.clicked.connect(Form.stop)
        self.SwitchMode.clicked.connect(Form.switch_mode)
        self.AutoStart.clicked.connect(Form.beforeStart)
        self.AutoFast.clicked.connect(Form.AutoFastEnalbe)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Hi"))
        self.WinRate.setText(_translate("Form", "评分"))
        self.InitCard.setText(_translate("Form", "开始"))
        self.UserHandCards.setText(_translate("Form", "手牌"))
        self.LPlayedCard.setText(_translate("Form", "上家出牌区域"))
        self.LPlayedCard.setVisible(False)
        self.RPlayedCard.setText(_translate("Form", "下家出牌区域"))
        self.RPlayedCard.setVisible(False)
        self.PredictedCard.setText(_translate("Form", "AI出牌区域"))
        self.ThreeLandlordCards.setText(_translate("Form", "十三张"))
        self.Stop.setText(_translate("Form", "停止"))
        self.SwitchMode.setText(_translate("Form", "单局"))
        self.AutoStart.setText(_translate("Form", "只提示"))
        self.BidWinrate.setText(_translate("Form", ""))
        self.PreWinrate.setText(_translate("Form", "局前预估胜率："))
        self.AutoFast.setText(_translate("Form", "快速关闭"))
