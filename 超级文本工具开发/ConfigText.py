# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'configText.ui'
#
# Created: Mon Feb 02 23:35:35 2015
#      by: PyQt4 UI code generator 4.11.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(513, 300)
        self.groupBox = QtGui.QGroupBox(Dialog)
        self.groupBox.setGeometry(QtCore.QRect(20, 20, 231, 261))
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.listWidget = QtGui.QListWidget(self.groupBox)
        self.listWidget.setGeometry(QtCore.QRect(0, 20, 111, 231))
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.pushButton_add = QtGui.QPushButton(self.groupBox)
        self.pushButton_add.setGeometry(QtCore.QRect(130, 20, 75, 23))
        self.pushButton_add.setObjectName(_fromUtf8("pushButton_add"))
        self.pushButton_allSelect = QtGui.QPushButton(self.groupBox)
        self.pushButton_allSelect.setGeometry(QtCore.QRect(130, 60, 75, 23))
        self.pushButton_allSelect.setObjectName(_fromUtf8("pushButton_allSelect"))
        self.pushButton_noSelect = QtGui.QPushButton(self.groupBox)
        self.pushButton_noSelect.setGeometry(QtCore.QRect(130, 100, 75, 23))
        self.pushButton_noSelect.setObjectName(_fromUtf8("pushButton_noSelect"))
        self.pushButton_deleteSelect = QtGui.QPushButton(self.groupBox)
        self.pushButton_deleteSelect.setGeometry(QtCore.QRect(130, 140, 75, 23))
        self.pushButton_deleteSelect.setObjectName(_fromUtf8("pushButton_deleteSelect"))
        self.pushButton_defaultConfig = QtGui.QPushButton(self.groupBox)
        self.pushButton_defaultConfig.setGeometry(QtCore.QRect(130, 180, 75, 23))
        self.pushButton_defaultConfig.setObjectName(_fromUtf8("pushButton_defaultConfig"))
        self.pushButton_ok = QtGui.QPushButton(Dialog)
        self.pushButton_ok.setGeometry(QtCore.QRect(290, 250, 75, 23))
        self.pushButton_ok.setObjectName(_fromUtf8("pushButton_ok"))
        self.pushButton_close = QtGui.QPushButton(Dialog)
        self.pushButton_close.setGeometry(QtCore.QRect(390, 250, 75, 23))
        self.pushButton_close.setObjectName(_fromUtf8("pushButton_close"))
        self.groupBox_2 = QtGui.QGroupBox(Dialog)
        self.groupBox_2.setGeometry(QtCore.QRect(270, 30, 211, 121))
        self.groupBox_2.setObjectName(_fromUtf8("groupBox_2"))
        self.radioButton_originalfile = QtGui.QRadioButton(self.groupBox_2)
        self.radioButton_originalfile.setGeometry(QtCore.QRect(20, 20, 131, 16))
        self.radioButton_originalfile.setObjectName(_fromUtf8("radioButton_originalfile"))
        self.radioButton_outFolder = QtGui.QRadioButton(self.groupBox_2)
        self.radioButton_outFolder.setGeometry(QtCore.QRect(20, 50, 131, 16))
        self.radioButton_outFolder.setObjectName(_fromUtf8("radioButton_outFolder"))
        self.lineEdit = QtGui.QLineEdit(self.groupBox_2)
        self.lineEdit.setGeometry(QtCore.QRect(22, 80, 121, 20))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.pushButton_browse = QtGui.QPushButton(self.groupBox_2)
        self.pushButton_browse.setGeometry(QtCore.QRect(150, 80, 51, 23))
        self.pushButton_browse.setObjectName(_fromUtf8("pushButton_browse"))

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.pushButton_close, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.accept)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "配置", None))
        self.groupBox.setTitle(_translate("Dialog", "文件类型设置", None))
        self.pushButton_add.setText(_translate("Dialog", "增加", None))
        self.pushButton_allSelect.setText(_translate("Dialog", "全选", None))
        self.pushButton_noSelect.setText(_translate("Dialog", "反选", None))
        self.pushButton_deleteSelect.setText(_translate("Dialog", "删除所选", None))
        self.pushButton_defaultConfig.setText(_translate("Dialog", "默认设置", None))
        self.pushButton_ok.setText(_translate("Dialog", "OK", None))
        self.pushButton_close.setText(_translate("Dialog", "Cancel", None))
        self.groupBox_2.setTitle(_translate("Dialog", "输出方式", None))
        self.radioButton_originalfile.setText(_translate("Dialog", "直接在源文件上操作", None))
        self.radioButton_outFolder.setText(_translate("Dialog", "输出到自定义文件夹", None))
        self.pushButton_browse.setText(_translate("Dialog", "浏览", None))

