# -*- coding: utf-8 -*-
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4 import uic
import re
import chardet
import codecs
import sys, os, shutil, filecmp
import SuperProcessText ,ConfigTextTool
from codecs import lookup,StreamReaderWriter

MAXVERSIONS = 100
class TextToolDialog(QDialog,SuperProcessText.Ui_Dialog):
    def __init__(self,parent=None):
        super(TextToolDialog,self).__init__(parent)
        self.setupUi(self)
        self.bOperateOriginalFile=False;
        self.fileNames=[]
        self.floders=[]
        self.sourceFileOrPathList=[]
        self.listItemSource=[]
        self.listItemResult=[]
        #checkBox选项
        self.bBackup=False
        self.bCase=False
        self.bChildrenFloder=False
        self.bRegex=False
        self.backupFiles=[]
        self.dlg=ConfigTextTool.ConfigTextDialog()
        self.fileType=[]
        self.fileType=self.dlg.GetFileType()
        #按钮信号连接:
        self.connect(self.pushButton_openFolder,SIGNAL("clicked()"),self.ButtonopenFolder)
        self.connect(self.pushButton_openFile,SIGNAL("clicked()"),self.ButtonopenFile)
        self.connect(self.pushButton_config,SIGNAL("clicked()"),self.ButtonopenConfigDialog)
        self.connect(self.pushButton_recover,SIGNAL("clicked()"),self.ButtonRecover)
        self.connect(self.pushButton_replace,SIGNAL("clicked()"),self.ButtonReplace)
        self.connect(self.pushButton_find,SIGNAL("clicked()"),self.ButtonFind)
     
    
        #列表响应消息
        self.connect(self.listWidgetSource,SIGNAL("itemDoubleClicked(QListWidgetItem*)"),self.DoubleClickListSource)
        self.connect(self.listWidgetResult,SIGNAL("itemDoubleClicked(QListWidgetItem*)"),self.DoubleClickListSource)
        #self.connect(self.listWidgetSource,SIGNAL("itemClicked(QListWidgetItem*)"),self.ClickedListSource)
        #设置列表右键响应消息
        self.listWidgetSource.setContextMenuPolicy(Qt.CustomContextMenu)
        self.listWidgetSource.customContextMenuRequested[QPoint].connect(self.ListSourceWidgetMenu)
        
    def ButtonopenFile(self):
        fileNames=QFileDialog.getOpenFileNames(self,"Open File Dialog","/","All file(*);;Text Files (*.txt);;Python file(*.py)")
        if not len(fileNames):
           return
        #注意这里,返回来的是列表不能用append()函数,否则就变成了列表的列表了
        self.fileNames+=fileNames
        self.sourceFileOrPathList+=fileNames
        #添加文件到列表
        for lst in fileNames:
            self.listItemSource.append(QListWidgetItem(lst));
            self.listWidgetSource.addItem( self.listItemSource[-1])
    def ButtonopenFolder(self):
        options = QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly
        floder = QFileDialog.getExistingDirectory(self,"","", options=options)
        if floder.isEmpty():
           return
        self.floders.append(floder)
        self.sourceFileOrPathList.append(floder)
        lst=floder
        self.listItemSource.append(QListWidgetItem(lst));
        self.listWidgetSource.addItem( self.listItemSource[-1])
    def ButtonopenConfigDialog(self):
        self.dlg.exec_()
    def ButtonRecover(self):
        #QMessageBox.information(QWidget, QString, QString, QMessageBox.StandardButtons buttons=QMessageBox.Ok, QMessageBox.StandardButton defaultButton=QMessageBox.NoButton)
        if(QMessageBox.Ok==QMessageBox.question(self,u"恢复",u"你确定你是要进行上一次替换操作的恢复吗?",QMessageBox.Ok|QMessageBox.No)):
            for file in self.backupFiles:
                if(os.path.isfile(file)):
                    newfile=file[0:len(file)-4]
                    shutil.copy(file, newfile)

        return
    def initTabWidge(self):
        return
    def listItemAdd(self):
        self.listWidgetSource.clear()
        for lst in self.sourceFileOrPathList:
            self.listItemSource.append(QListWidgetItem(lst));
        for i in range(len( self.listItemSource)):
            self.listItemSource[i].setCheckState(Qt.Checked);
            self.listWidgetSource.addItem( self.listItemSource[i])
    def ClearListWidge(self):
        return
    def ButtonReplace(self):
        replaceText=self.QstringToPythonString(self.textEdit_replace.toPlainText())
        subjectText=self.QstringToPythonString(self.textEdit_find.toPlainText())
        #先检查选项
        self.CheckBoxOption()
        self.backupFiles=[]

        #文件处理
        for filePath in self.fileNames:
            changeFilePath= self.QstringToPythonString(filePath)
            if(self.bBackup==True):
                self.backupFile(changeFilePath)
            self.NoRegexReadUnicodeOrAscII(replaceText,subjectText,changeFilePath)

        #文件夹处理:
        dirFileList=[]
        if(self.bChildrenFloder==True):
            for path in self.floders:
                changeFilePath= self.QstringToPythonString(path)
                self.IncludeSubDirfindfiles(changeFilePath,dirFileList)
        else:
            for path in self.floders:
                changeFilePath= self.QstringToPythonString(path)
                self.NoSubDirfindfiles(changeFilePath,dirFileList)
        if(self.bBackup==True):
            self.backup(dirFileList)
        for file in dirFileList:
            if os.path.isfile(file):

                self.NoRegexReadUnicodeOrAscII(replaceText,subjectText,file)

    def ButtonFind(self):
        self.listWidgetResult.clear()
        replaceText=self.QstringToPythonString(self.textEdit_replace.toPlainText())
        subjectText=self.QstringToPythonString(self.textEdit_find.toPlainText())
        #先检查选项
        self.CheckBoxOption()
        #文件处理
        for filePath in self.fileNames:
            changeFilePath= self.QstringToPythonString(filePath)
            if(self.FindText(subjectText,changeFilePath)==True):
                
                self.listWidgetResult.addItem(QListWidgetItem(filePath))
        #文件夹处理:
        dirFileList=[]
        if(self.bChildrenFloder==True):
            for path in self.floders:
                changeFilePath= self.QstringToPythonString(path)
                self.IncludeSubDirfindfiles(changeFilePath,dirFileList)
        else:
            for path in self.floders:
                changeFilePath= self.QstringToPythonString(path)
                self.NoSubDirfindfiles(changeFilePath,dirFileList)
        for file in dirFileList:
            if os.path.isfile(file):
                if(self.FindText(subjectText,file)==True):
                    self.listWidgetResult.addItem(QListWidgetItem(file))
        return
    def DoubleClickListSource(self,item):
        text=self.QstringToPythonString(item.text())
        if os.path.isdir(text):
            os.startfile(text)
        else:
            os.popen(text)
       
    def ClickedListSource(self):
        return
    def ListSourceWidgetMenu(self,point):
        addFloderAction = self.createAction(u"添加文件夹", self.ButtonopenFolder)
        addFileAction = self.createAction(u"添加文件", self.ButtonopenFile)
        addDeleteAction = self.createAction(u"删除", self.DeleteListSourceItem)
        addClearAction = self.createAction(u"清空", self.ClearListSourceItems)
        popMenu =QMenu()
        popMenu.addAction(addFloderAction)
        popMenu.addAction(addFileAction)
        popMenu.addAction(addDeleteAction)
        popMenu.addAction(addClearAction)
        popMenu.exec_(QCursor.pos())
    def DeleteListSourceItem(self):
        for item in self.listItemSource:
            if self.listWidgetSource.isItemSelected(item):
               self.listWidgetSource.takeItem(self.listWidgetSource.row(item))
               self.listItemSource.remove(item)
               self.sourceFileOrPathList.remove(item.text())
               itemText=item.text()
               if os.path.isdir(self.QstringToPythonString(itemText)):
                   self.floders.remove(itemText)
               else:
                   self.fileNames.remove(itemText)
                  
                   
        return
    def ClearListSourceItems(self):
        self.listWidgetSource.clear()
        self.listItemSource=[]
        self.sourceFileOrPathList=[]
        self.fileNames=[]
        self.floders=[]
        return
    def createAction(self,text,slot=None,shortcut=None, icon=None,
               tip=None,checkable=False,signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon("./images/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action
    def QstringToPythonString(self, text):
        str=unicode(text.toUtf8(), 'utf-8', 'ignore')
        return str
    def CheckBoxOption(self):
        if (Qt.Checked==self.checkBox_backups.checkState()):
            self.bBackup=True;
        else:
            self.bBackup=False;
        if (Qt.Checked==self.checkBox_case.checkState()):
            self.bCase=True;
        else:
            self.bCase=False;
        if (Qt.Checked==self.checkBox_childFolder.checkState()):
            self.bChildrenFloder=True;
        else:
            self.bChildrenFloder=False;
        if (Qt.Checked==self.checkBox_regex.checkState()):
            self.bRegex=True;
        else:
            self.bRegex=False;
    def RegexReplaceText(self,replaceText,subjectText,TextContent):
        pattern=re.M;
        if (self.bCase==False):
            pattern=pattern|re.I
        regex = re.compile(subjectText,pattern)
        result, number = re.subn(regex, replaceText, TextContent)
        return result,number
    def FindText(self,text,filePath):
        # 将正则表达式编译成Pattern对象
        TextContent=""
        with codecs.open(filePath) as f: 
            d = f.readline()
            charSetMode=chardet.detect(d)['encoding']
            f.close()
        with codecs.open(filePath,"r+",charSetMode) as f: 
            TextContent = f.read()
        pattern=re.M;
        if (self.bCase==False):
            pattern=pattern|re.I
        regex = re.compile(text,pattern)

        # 使用search()查找匹配的子串，不存在能匹配的子串时将返回None
        # 这个例子中使用match()无法成功匹配
        match = regex.search(TextContent)
 
        if match:
            # 使用Match获得分组信息
            match.group()
            return True;
 
            ### 输出 ###
            # world
        else:
            return False
    def NoRegexReadUnicodeOrAscII(self,replaceText,subjectText,filePath,errors='strict',buffering=1):

        f=open(filePath,'r+')
        d = f.readline()

        encoding=chardet.detect(d)['encoding']

        #f.seek(0)
        f.close();
        # info = lookup(encoding)
        # srw = StreamReaderWriter(f, info.streamreader, info.streamwriter, errors)
        # srw.encoding = encoding
        # d=srw.read()
        # f.truncate(0)
        with codecs.open(filePath,"r+",encoding) as f:
            d = f.read()
            strText=""
            if(self.bRegex==True or self.bCase==False):
                strText,num=self.RegexReplaceText(replaceText,subjectText,d)
            else:
                strText=d.replace(subjectText, replaceText)
            f.seek(0)
            outDir,bOperateOriginalFile=self.dlg.GetFileDir()
            if(bOperateOriginalFile==False and outDir!=""):
                outDir=self.QstringToPythonString(outDir)
                f.close()
                newFilePath=outDir+'\\'+os.path.basename(filePath)
                with codecs.open(newFilePath,"w",encoding) as f:
                    f.write(strText);
                    f.close()
            else:
                f.truncate(0)
                f.write(strText)
                f.close()
    def NoSubDirfindfiles(self,path, fileList):
        files = os.listdir(path);
        for f in files:
            sufix = os.path.splitext(f)[1][1:]
            for extension in self.fileType:
                if sufix==extension:
                    npath = path + '\\' + f;
                    if(os.path.isfile(npath)):
                        fileList.append(npath);
                        break;
    def IncludeSubDirfindfiles(self,path, fileList):
        files = os.listdir(path);
        for f in files:
            npath = path + '\\' + f;
            if(os.path.isfile(npath)):
                sufix = os.path.splitext(npath)[1][1:]
                for extension in self.fileType:
                    if sufix==extension:
                        fileList.append(npath);
                        break
            if(os.path.isdir(npath)):
                if (f[0] == '.'):
                    pass;
                else:
                    self.IncludeSubDirfindfiles(npath, fileList);
    def backupFile(self,file):
        if os.path.isfile(file):
            backup = '%s.###' % (file)
            shutil.copy(file, backup)
            self.backupFiles.append(backup)
    def backup(self,files):
        for file in files[:]:
            regex = re.compile("###")
            match = regex.search(file)
            if match:
                files.remove(file)
        for file in files[:]:
            filepath = file
            destpath = file
            #check if the old version exist
            for index in xrange(MAXVERSIONS):
                backup = '%s.###' % (destpath)
                if not os.path.exists(backup):
                    break
            if index > 0:
            #there is no need to backup if the file is the same as the new version
                old_backup = '%s.###' %(destpath)
                abspath = os.path.abspath(filepath)
                try:
                    if os.path.isfile(old_backup) and filecmp.cmp(abspath, old_backup,shallow = False):
                        continue
                except OSError:
                    pass
            try:
                shutil.copy(filepath, backup)
                self.backupFiles.append(backup)
            except OSError:
                pass
app=QApplication(sys.argv)
dialog=TextToolDialog()
dialog.show()
app.exec_()










