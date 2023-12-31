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
        Form.resize(703, 418)
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(9)
        font.setBold(True)
        font.setItalic(False)
        font.setWeight(75)
        Form.setFont(font)
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
        self.title = QtWidgets.QLabel(Form)
        self.title.setGeometry(QtCore.QRect(270, 10, 161, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(16)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.title.setFont(font)
        self.title.setAlignment(QtCore.Qt.AlignCenter)
        self.title.setObjectName("title")
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
        self.jiShu = QtWidgets.QLabel(Form)
        self.jiShu.setGeometry(QtCore.QRect(10, 70, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.jiShu.setFont(font)
        self.jiShu.setObjectName("jiShu")
        self.beitou = QtWidgets.QLabel(Form)
        self.beitou.setGeometry(QtCore.QRect(10, 110, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.beitou.setFont(font)
        self.beitou.setObjectName("beitou")
        self.textEdit_base = QtWidgets.QTextEdit(Form)
        self.textEdit_base.setGeometry(QtCore.QRect(110, 70, 104, 31))
        self.textEdit_base.setObjectName("textEdit_base")
        self.textEdit_beitou = QtWidgets.QTextEdit(Form)
        self.textEdit_beitou.setGeometry(QtCore.QRect(110, 110, 104, 31))
        self.textEdit_beitou.setObjectName("textEdit_beitou")
        self.textEdit_zhisuan = QtWidgets.QTextEdit(Form)
        self.textEdit_zhisuan.setGeometry(QtCore.QRect(110, 150, 104, 31))
        self.textEdit_zhisuan.setObjectName("textEdit_zhisuan")
        self.zhisuan = QtWidgets.QLabel(Form)
        self.zhisuan.setGeometry(QtCore.QRect(10, 150, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.zhisuan.setFont(font)
        self.zhisuan.setObjectName("zhisuan")
        self.zhiying = QtWidgets.QLabel(Form)
        self.zhiying.setGeometry(QtCore.QRect(10, 190, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.zhiying.setFont(font)
        self.zhiying.setObjectName("zhiying")
        self.textEdit_zhiying = QtWidgets.QTextEdit(Form)
        self.textEdit_zhiying.setGeometry(QtCore.QRect(110, 190, 104, 31))
        self.textEdit_zhiying.setObjectName("textEdit_zhiying")
        self.benjin = QtWidgets.QLabel(Form)
        self.benjin.setGeometry(QtCore.QRect(10, 240, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.benjin.setFont(font)
        self.benjin.setObjectName("benjin")
        self.textEdit_benjin = QtWidgets.QTextEdit(Form)
        self.textEdit_benjin.setGeometry(QtCore.QRect(110, 240, 104, 31))
        self.textEdit_benjin.setObjectName("textEdit_benjin")
        self.label_tip1 = QtWidgets.QLabel(Form)
        self.label_tip1.setGeometry(QtCore.QRect(250, 80, 401, 51))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.label_tip1.setFont(font)
        self.label_tip1.setObjectName("label_tip1")
        self.label_tip2 = QtWidgets.QLabel(Form)
        self.label_tip2.setGeometry(QtCore.QRect(250, 130, 401, 31))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.label_tip2.setFont(font)
        self.label_tip2.setObjectName("label_tip2")
        self.label_btnV0 = QtWidgets.QLabel(Form)
        self.label_btnV0.setGeometry(QtCore.QRect(240, 300, 51, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_btnV0.setFont(font)
        self.label_btnV0.setObjectName("label_btnV0")
        self.textEdit_btnV0 = QtWidgets.QTextEdit(Form)
        self.textEdit_btnV0.setGeometry(QtCore.QRect(240, 260, 51, 31))
        self.textEdit_btnV0.setObjectName("textEdit_btnV0")
        self.textEdit_btnV1 = QtWidgets.QTextEdit(Form)
        self.textEdit_btnV1.setGeometry(QtCore.QRect(320, 260, 51, 31))
        self.textEdit_btnV1.setObjectName("textEdit_btnV1")
        self.label_btnV1 = QtWidgets.QLabel(Form)
        self.label_btnV1.setGeometry(QtCore.QRect(320, 300, 51, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_btnV1.setFont(font)
        self.label_btnV1.setObjectName("label_btnV1")
        self.textEdit_btnV2 = QtWidgets.QTextEdit(Form)
        self.textEdit_btnV2.setGeometry(QtCore.QRect(400, 260, 51, 31))
        self.textEdit_btnV2.setObjectName("textEdit_btnV2")
        self.label_btnV1_2 = QtWidgets.QLabel(Form)
        self.label_btnV1_2.setGeometry(QtCore.QRect(400, 300, 51, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_btnV1_2.setFont(font)
        self.label_btnV1_2.setObjectName("label_btnV1_2")
        self.textEdit_btnV3 = QtWidgets.QTextEdit(Form)
        self.textEdit_btnV3.setGeometry(QtCore.QRect(480, 260, 51, 31))
        self.textEdit_btnV3.setObjectName("textEdit_btnV3")
        self.label_btnV3 = QtWidgets.QLabel(Form)
        self.label_btnV3.setGeometry(QtCore.QRect(480, 300, 51, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_btnV3.setFont(font)
        self.label_btnV3.setObjectName("label_btnV3")
        self.textEdit_btnV4 = QtWidgets.QTextEdit(Form)
        self.textEdit_btnV4.setGeometry(QtCore.QRect(570, 260, 51, 31))
        self.textEdit_btnV4.setObjectName("textEdit_btnV4")
        self.label_btnV5 = QtWidgets.QLabel(Form)
        self.label_btnV5.setGeometry(QtCore.QRect(570, 300, 51, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_btnV5.setFont(font)
        self.label_btnV5.setObjectName("label_btnV5")
        self.label_tip3 = QtWidgets.QLabel(Form)
        self.label_tip3.setGeometry(QtCore.QRect(250, 210, 351, 51))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.label_tip3.setFont(font)
        self.label_tip3.setObjectName("label_tip3")
        self.label_duokai = QtWidgets.QLabel(Form)
        self.label_duokai.setGeometry(QtCore.QRect(470, 170, 81, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_duokai.setFont(font)
        self.label_duokai.setObjectName("label_duokai")
        self.textEdit_duokai = QtWidgets.QTextEdit(Form)
        self.textEdit_duokai.setGeometry(QtCore.QRect(570, 170, 104, 31))
        self.textEdit_duokai.setObjectName("textEdit_duokai")
        self.textEdit_xiazhuStyle = QtWidgets.QTextEdit(Form)
        self.textEdit_xiazhuStyle.setGeometry(QtCore.QRect(630, 40, 41, 31))
        self.textEdit_xiazhuStyle.setPlaceholderText("")
        self.textEdit_xiazhuStyle.setObjectName("textEdit_xiazhuStyle")
        self.label_xiazhu = QtWidgets.QLabel(Form)
        self.label_xiazhu.setGeometry(QtCore.QRect(250, 40, 381, 41))
        font = QtGui.QFont()
        font.setFamily("等线")
        font.setPointSize(12)
        font.setBold(False)
        font.setItalic(False)
        font.setWeight(50)
        self.label_xiazhu.setFont(font)
        self.label_xiazhu.setObjectName("label_xiazhu")

        self.retranslateUi(Form)
        self.InitCard.clicked.connect(Form.init_cards)
        self.Stop.clicked.connect(Form.stop)
        self.SwitchMode.clicked.connect(Form.switch_mode)
        self.AutoStart.clicked.connect(Form.beforeStart)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Hi"))
        self.InitCard.setText(_translate("Form", "开始"))
        self.title.setText(_translate("Form", "自动控制版本"))
        self.Stop.setText(_translate("Form", "停止"))
        self.SwitchMode.setText(_translate("Form", "单局"))
        self.AutoStart.setText(_translate("Form", "自动开始"))
        self.jiShu.setText(_translate("Form", "设置基数："))
        self.beitou.setText(_translate("Form", "倍投次数："))
        self.zhisuan.setText(_translate("Form", "止损："))
        self.zhiying.setText(_translate("Form", "止赢："))
        self.benjin.setText(_translate("Form", "本金："))
        self.label_tip1.setText(_translate("Form", "此轮下注金额：0"))
        self.label_tip2.setText(_translate("Form", "当前倍投次数：0"))
        self.label_btnV0.setText(_translate("Form", "按钮1："))
        self.label_btnV1.setText(_translate("Form", "按钮2："))
        self.label_btnV1_2.setText(_translate("Form", "按钮3："))
        self.label_btnV3.setText(_translate("Form", "按钮4："))
        self.label_btnV5.setText(_translate("Form", "按钮5："))
        self.label_tip3.setText(_translate("Form", "下方按钮值必须对应游戏里面的按钮值"))
        self.label_duokai.setText(_translate("Form", "设置多开："))
        self.label_xiazhu.setText(_translate("Form", "<html><head/><body><p>下注方式：0是一闲一庄,1是二庄一闲，2是三庄三闲：</p></body></html>"))
