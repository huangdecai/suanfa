@echo "5���Ӻ�Ĭ�Ͽ�ʼ��̨���룺"
@ping 127.0.0.1 -n 1 1>nul 2>nul

set _devenv="%VS90COMNTOOLS%C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com"

DEL releaseOutput.txt

for /r %%i in (*.sln) do (
	@echo Build Release Start!!!
	echo "%%i" >> releaseOutput.txt
	time /T >> releaseOutput.txt
	%_devenv% "%%i" /build "Release|x64" /Out releaseOutput.txt

	@echo Release Build Finish!!!
	time /T >> releaseOutput.txt
)

pause
