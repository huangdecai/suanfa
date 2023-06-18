# -* - coding: UTF-8 -* -
import ConfigParser
from PyQt4.QtGui import *
from PyQt4.QtCore import *

from PyQt4 import uic
import sys
import ConfigText

class ConfigTextDialog(QDialog,ConfigText.Ui_Dialog):
    def __init__(self,parent=None):
        super(ConfigTextDialog,self).__init__(parent)
        self.setupUi(self)
        self.bOperateOriginalFile=True;
        self.pushButton_browse.setEnabled(False);
        self.fileDir=""
        self.defaultItem=['txt','cpp','txt','c','h','py']
        self.item=['txt','cpp','txt','c','h','py']
        self.listItem=[]
        self.lineEdit.setEnabled(False);
        self.radioButton_originalfile.setChecked(True);
        #读取配置文件
        self.ReadConfig();
        self.listItemAdd()
        
        #信号连接
        self.connect(self.radioButton_outFolder,SIGNAL("clicked()"),self.SetOutFloderType)
        self.connect(self.radioButton_originalfile,SIGNAL("clicked()"),self.SetOutOriginalType)
        self.connect(self.pushButton_browse,SIGNAL("clicked()"),self.openFolder)
        #操作列表的按钮信号:
        self.connect(self.pushButton_add,SIGNAL("clicked()"),self.buttonAdd)
        self.connect(self.pushButton_allSelect,SIGNAL("clicked()"),self.buttonAllSelect)
        self.connect(self.pushButton_noSelect,SIGNAL("clicked()"),self.buttonNoSelect)
        self.connect(self.pushButton_deleteSelect,SIGNAL("clicked()"),self.buttonDeleteSelect)
        self.connect(self.pushButton_defaultConfig,SIGNAL("clicked()"),self.buttonDefaultConfig)
        self.connect(self.pushButton_ok,SIGNAL("clicked()"),self.buttonOk)
       
        
    def SetOutFloderType(self):
        self.bOperateOriginalFile=False;
        self.pushButton_browse.setEnabled(True);
        self.lineEdit.setEnabled(True);
    def SetOutOriginalType(self):
        self.bOperateOriginalFile=True;
        self.pushButton_browse.setEnabled(False);
        self.lineEdit.setEnabled(False);
    def openFolder(self):
        self.fileDir=QFileDialog.getExistingDirectory()
        self.lineEdit.setText(self.fileDir);
        #添加文件格式到列表
    def listItemAdd(self):
        
        #listItem=[]
        for lst in self.item:
            self.listItem.append(QListWidgetItem(lst));
        for i in range(len( self.listItem)):
            self.listItem[i].setCheckState(Qt.Checked);
            self.listWidget.addItem( self.listItem[i])
    def buttonAdd(self):
        name,ok=QInputDialog.getText(self,(u"输入窗口"),(u"请输入新的文件类型:"),
        QLineEdit.Normal,"")
        if name.isEmpty():
            return
        if ok:
            for i in self.item:
                if i==name:
                    return
        self.item.append(name)   
        self.listItem.append(QListWidgetItem(name));
        self.listItem[-1].setCheckState(Qt.Checked);
        self.listWidget.addItem( self.listItem[-1])
    def buttonAllSelect(self):
        for i in range(len( self.listItem)):
            self.listItem[i].setCheckState(Qt.Checked);
    def buttonNoSelect(self):
        for i in range(len( self.listItem)):
            self.listItem[i].setCheckState(Qt.Unchecked);
    #删除列表
    def buttonDeleteSelect(self):
        dellist = []
        for item in self.listItem[:]:
               state=item.checkState()
               if (state==Qt.Checked):
                  self.listWidget.takeItem(self.listWidget.row(item))
                  self.listItem.remove(item)
    def buttonDefaultConfig(self):
        self.listWidget.clear();
        self.listItem=[]
        self.item=self.defaultItem[:];
        for lst in self.item:
            self.listItem.append(QListWidgetItem(lst));
        for i in range(len( self.listItem)):
            self.listItem[i].setCheckState(Qt.Checked);
            self.listWidget.addItem( self.listItem[i])
    def buttonOk(self):
        self.fileDir=self.lineEdit.text();
        self.WriteConfig();
        self.accept()
    def GetFileType(self):
        return self.item
    def GetFileDir(self):
        return self.fileDir,self.bOperateOriginalFile
    def WriteConfig(self):
                #生成config对象
        conf = ConfigParser.ConfigParser()
        conf.add_section('fileTypeSection')
        for i in range(len( self.item)):
            conf.set('fileTypeSection', 'fileType%s'%(i), self.item[i])
        #写回配置文件
        conf.write(open("FileTypeConfig.cfg", "w"))
    def ReadConfig(self):
           #生成config对象
        conf = ConfigParser.ConfigParser()
        #用config对象读取配置文件
        conf.read("FileTypeConfig.cfg")
        #以列表形式返回所有的section
        sections = conf.sections()
        #得到指定section的所有option
        options = conf.options("fileTypeSection")
        self.item=[]
        for i in options:
        #指定section，option读取值
            str_val = conf.get("fileTypeSection", i)
            self.item.append(str_val)

