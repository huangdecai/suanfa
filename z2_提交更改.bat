@echo off

rem chcp 65001 就是换成UTF-8代码页 
rem chcp 936 可以换回默认的GBK 
rem chcp 437 是美国英语 

chcp 65001

echo 提交代码前需要解决本地冲突

rem 打印空白行
echo,

git add .

git status

rem 打印空白行
echo,

rem set /p input=请输入提交注释:

git commit -m sssssss

git push

pause