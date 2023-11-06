@echo off

rem chcp 65001 就是换成UTF-8代码页 
rem chcp 936 可以换回默认的GBK 
rem chcp 437 是美国英语 

chcp 65001

echo 更新代码需要本地代码已经提交到自己的分支上

rem 打印空白行
echo,

git pull

pause