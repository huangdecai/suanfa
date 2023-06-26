# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainText.ui'
#
# Created: Tue Mar 03 23:55:32 2015
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
        Dialog.resize(510, 442)
        self.pushButton_openFolder = QtGui.QPushButton(Dialog)
        self.pushButton_openFolder.setGeometry(QtCore.QRect(0, 0, 75, 31))
        self.pushButton_openFolder.setObjectName(_fromUtf8("pushButton_openFolder"))
        self.pushButton_openFile = QtGui.QPushButton(Dialog)
        self.pushButton_openFile.setGeometry(QtCore.QRect(80, 0, 75, 31))
        self.pushButton_openFile.setObjectName(_fromUtf8("pushButton_openFile"))
        self.pushButton_config = QtGui.QPushButton(Dialog)
        self.pushButton_config.setGeometry(QtCore.QRect(160, 0, 75, 31))
        self.pushButton_config.setObjectName(_fromUtf8("pushButton_config"))
        self.tabWidget = QtGui.QTabWidget(Dialog)
        self.tabWidget.setGeometry(QtCore.QRect(10, 70, 491, 141))
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.listWidgetSource = QtGui.QListWidget(self.tab)
        self.listWidgetSource.setGeometry(QtCore.QRect(-5, 0, 491, 121))
        self.listWidgetSource.setObjectName(_fromUtf8("listWidgetSource"))
        self.tabWidget.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.listWidgetResult = QtGui.QListWidget(self.tab_2)
        self.listWidgetResult.setGeometry(QtCore.QRect(0, 0, 491, 121))
        self.listWidgetResult.setObjectName(_fromUtf8("listWidgetResult"))
        self.tabWidget.addTab(self.tab_2, _fromUtf8(""))
        self.checkBox_backups = QtGui.QCheckBox(Dialog)
        self.checkBox_backups.setGeometry(QtCore.QRect(10, 40, 81, 16))
        self.checkBox_backups.setObjectName(_fromUtf8("checkBox_backups"))
        self.checkBox_case = QtGui.QCheckBox(Dialog)
        self.checkBox_case.setGeometry(QtCore.QRect(100, 40, 81, 16))
        self.checkBox_case.setChecked(True)
        self.checkBox_case.setObjectName(_fromUtf8("checkBox_case"))
        self.checkBox_childFolder = QtGui.QCheckBox(Dialog)
        self.checkBox_childFolder.setGeometry(QtCore.QRect(190, 40, 91, 16))
        self.checkBox_childFolder.setObjectName(_fromUtf8("checkBox_childFolder"))
        self.pushButton_replace = QtGui.QPushButton(Dialog)
        self.pushButton_replace.setGeometry(QtCore.QRect(140, 400, 75, 23))
        self.pushButton_replace.setObjectName(_fromUtf8("pushButton_replace"))
        self.textEdit_find = QtGui.QTextEdit(Dialog)
        self.textEdit_find.setGeometry(QtCore.QRect(10, 240, 491, 61))
        self.textEdit_find.setObjectName(_fromUtf8("textEdit_find"))
        self.textEdit_replace = QtGui.QTextEdit(Dialog)
        self.textEdit_replace.setGeometry(QtCore.QRect(10, 330, 491, 61))
        self.textEdit_replace.setObjectName(_fromUtf8("textEdit_replace"))
        self.pushButton_recover = QtGui.QPushButton(Dialog)
        self.pushButton_recover.setGeometry(QtCore.QRect(240, 0, 75, 31))
        self.pushButton_recover.setObjectName(_fromUtf8("pushButton_recover"))
        self.label = QtGui.QLabel(Dialog)
        self.label.setGeometry(QtCore.QRect(10, 220, 54, 12))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setGeometry(QtCore.QRect(10, 310, 54, 12))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.pushButton_close = QtGui.QPushButton(Dialog)
        self.pushButton_close.setGeometry(QtCore.QRect(320, 400, 75, 23))
        self.pushButton_close.setObjectName(_fromUtf8("pushButton_close"))
        self.pushButton_find = QtGui.QPushButton(Dialog)
        self.pushButton_find.setGeometry(QtCore.QRect(230, 400, 75, 23))
        self.pushButton_find.setObjectName(_fromUtf8("pushButton_find"))
        self.checkBox_regex = QtGui.QCheckBox(Dialog)
        self.checkBox_regex.setGeometry(QtCore.QRect(290, 40, 101, 16))
        self.checkBox_regex.setObjectName(_fromUtf8("checkBox_regex"))

        self.retranslateUi(Dialog)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QObject.connect(self.pushButton_close, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.accept)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "超级文本处理工具", None))
        self.pushButton_openFolder.setText(_translate("Dialog", "打开文件夹", None))
        self.pushButton_openFile.setText(_translate("Dialog", "打开文件", None))
        self.pushButton_config.setText(_translate("Dialog", "处理配置", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("Dialog", "替换或查找源目标", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("Dialog", "查找结果", None))
        self.checkBox_backups.setText(_translate("Dialog", "替换前备份", None))
        self.checkBox_case.setText(_translate("Dialog", "区分大小写", None))
        self.checkBox_childFolder.setText(_translate("Dialog", "包含子文件夹", None))
        self.pushButton_replace.setText(_translate("Dialog", "替换", None))
        self.pushButton_recover.setText(_translate("Dialog", "恢复", None))
        self.label.setText(_translate("Dialog", "查找内容", None))
        self.label_2.setText(_translate("Dialog", "替换内容", None))
        self.pushButton_close.setText(_translate("Dialog", "关闭", None))
        self.pushButton_find.setText(_translate("Dialog", "查找", None))
        self.checkBox_regex.setText(_translate("Dialog", "使用正则表达式", None))

