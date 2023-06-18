// testC++.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "testC++.h"
#include <stdio.h>
#include "nb30.h"
#include "cwritelog.h"

static int testCount = 0;
cwritelog writeLog;
// 这是导出变量的一个示例
typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff[30];
}ASTAT, *PASTAT;
typedef struct AdapterStrct
{
	int nIndex;			//网卡编号
	string strMac;		//Mac地址（字符串形式）
}ADPTSTRCT;
vector<ADPTSTRCT> m_arrAdapters;
void GetOneMac(int AdapterIndex)
{
	NCB ncb;
	UCHAR uRetCode;
	ASTAT Adapter;
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = AdapterIndex;   // 指定网卡号

	//首先对选定的网卡发送一个NCBRESET命令，以便进行初始化 
	uRetCode = Netbios(&ncb);

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = AdapterIndex; 	// 指定网卡号

	strcpy((char *)ncb.ncb_callname, "*");
	// 指定返回的信息存放的变量 
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);

	// 发送NCBASTAT命令以获取网卡的信息 
	uRetCode = Netbios(&ncb);
	if (uRetCode == 0)
	{
		// 把网卡MAC地址格式化成常用的16进制形式，如0010-A4E4-5802 
		string strMacAddr;

		char szLog[1024] = {};
		sprintf(szLog, "%02X-%02X-%02X-%02X-%02X-%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]);
		strMacAddr = szLog;
		//将网卡地址和序号存入数组中
		ADPTSTRCT AdptSt;
		AdptSt.nIndex = AdapterIndex;
		AdptSt.strMac = strMacAddr;
		m_arrAdapters.push_back(AdptSt);
	}

	string szStr = m_arrAdapters.at(0).strMac;
}

void OnGetaddr()
{
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM lenum;
	int i = 0;

	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);

	//向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等 
	uRetCode = Netbios(&Ncb);
	//获得所有网卡信息
	for (i = 0; i < lenum.length; i++)
	{
		GetOneMac(lenum.lana[i]);
	}
}
bool IsEnable()
{
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-50-56-B1-4E-E9");
	strVector.push_back("90-2B-34-07-1B-14");
	for (int i = 0; i < strVector.size(); i++)
	{
		string szStr;
		for (int j = 0; j < m_arrAdapters.size(); j++)
		{
			szStr = m_arrAdapters.at(j).strMac;
			if (szStr == strVector[i])
			{
				return true;
			}
		}

	}

	return false;
}
bool tong()
{
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-50-56-B1-4E-E9");
	strVector.push_back("90-2B-34-07-1B-14");
	for (int i = 0; i < strVector.size(); i++)
	{
		string szStr;
		for (int j = 0; j < m_arrAdapters.size(); j++)
		{
			szStr = m_arrAdapters.at(j).strMac;
			if (szStr == strVector[i])
			{
				return true;
			}
		}

	}

	return false;
}
bool StrFormat(char szBuf[], char * szLogInfo, ...)
{

	va_list arglist;
	va_start(arglist, szLogInfo);
	int n = sizeof(szBuf);
	int m = sizeof(char);

	vsprintf_s(szBuf, CountArray(szBuf) - 1, szLogInfo, arglist);
	va_end(arglist);
	return true;
}
CGameLogicNew m_GameLogicNew;
// 这是导出函数的一个示例。
TESTC_API int __stdcall fntestC(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbCardDataEx[], BYTE cbMaxCard[], BYTE &cbCardCount, BYTE cbResultCard[])
{
	testCount++;
	if (testCount >= 200)
	{
		return 0;
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
    writeLog.log("cbHandCardCount:%d\n", cbHandCardCount);
	writeLog.log("\ntestCount:%d\n", testCount);
	writeLog.log(cbHandCardData, cbHandCardCount);
	writeLog.log("\nbTurnCardCount:%d\n", cbTurnCardCount);
	writeLog.log(cbTurnCardData, cbTurnCardCount);
	writeLog.log("\n cbDiscardCardCount:%d,\n", cbDiscardCardCount);
	writeLog.log(DiscardCard, cbDiscardCardCount);
	writeLog.log("\n cbCardDataEx:%d,\n", cbDiscardCardCount);
	writeLog.log(cbCardDataEx, MAX_COUNT);
	writeLog.log("\n cbMaxCard:%d,\n", cbDiscardCardCount);
	writeLog.log(cbMaxCard, MAX_COUNT);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, DiscardCard, cbDiscardCardCount, cbCardDataEx, cbMaxCard, OutCardResult);
	/*cbCardType = OutCardResult.cbCardType;
	cbCardScore = OutCardResult.cbCardType;*/
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	cbCardCount = OutCardResult.cbCardCount;
	CopyMemory(cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	writeLog.log(cbResultCard, cbCardCount);
	writeLog.log("End\n");
	return 1;
}

TESTC_API int __stdcall countScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCardScore)
{

	float score= m_GameLogicNew.LandScoreErRen(cbHandCardData, cbHandCardCount, cbCardScore);
	if (cbCardScore<0)
	{
		cbCardScore = 0;
		return 0;
	}

	return score;
}

TESTC_API int fntestPython2(tagInPyhonNew *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" 打头区分这是在.so 里面输出的
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	testCount++;
	if (testCount >= 200)
	{
		return 0;
	}
	writeLog.log("\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);
	writeLog.log("\ntestCount:%d\n", testCount);
	writeLog.log( pythonIn->cbHandCardData,pythonIn->cbHandCardCount);
	writeLog.log("\nbTurnCardCount:%d\n", pythonIn->cbTurnCardCount);
	writeLog.log(pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount);
	writeLog.log("\n cbDiscardCardCount:%d,\n", pythonIn->cbDiscardCardCount);
	writeLog.log(pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount);
	writeLog.log("\n cbCardDataEx:%d,\n", MAX_COUNT);
	writeLog.log(pythonIn->cbCardDataEx, MAX_COUNT);
	writeLog.log("\n cbMaxCard:%d,\n", MAX_COUNT);
	writeLog.log(pythonIn->cbMaxCard, MAX_COUNT);
	tagOutCardResultNew  OutCardResult;
	m_GameLogicNew.SearchOutCardErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbTurnCardData, pythonIn->cbTurnCardCount, pythonIn->cbDiscardCard, pythonIn->cbDiscardCardCount, pythonIn->cbCardDataEx, pythonIn->cbMaxCard, OutCardResult);
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	pythonIn->cbCardCount = OutCardResult.cbCardCount;
	CopyMemory(pythonIn->cbResultCard, OutCardResult.cbResultCard, MAX_COUNT);
	writeLog.log("cbResultCard:%d,\n", OutCardResult.cbCardCount);
	writeLog.log(pythonIn->cbResultCard, pythonIn->cbCardCount);
	writeLog.log("\nEnd\n");
	return 1;
}
TESTC_API int fntestPythonType(tagInPyhonCardType *pythonIn)
{

	if (NULL == pythonIn) {
		return 0;       // # "C --" 打头区分这是在.so 里面输出的
	}
	if (IsEnable() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	testCount++;
	if (testCount >= 200)
	{
		return 0;
	}
	writeLog.log("\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	writeLog.log("\ntestCount:%d\n", testCount);
	writeLog.log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	pythonIn->cbType = m_GameLogicNew.GetCardType(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return 0;
	}
	writeLog.log("type:,%d\n", pythonIn->cbType);
	writeLog.log("End\n");
	return 1;
}
TESTC_API int  PythonCountScore(tagInPyhonCardScore* pythonIn)
{
	writeLog.log("\ncbHandCardCount:%d\n", pythonIn->cbHandCardCount);

	writeLog.log(pythonIn->cbHandCardData, pythonIn->cbHandCardCount);
	float score = m_GameLogicNew.LandScoreErRen(pythonIn->cbHandCardData, pythonIn->cbHandCardCount, pythonIn->cbCardScore);
	if (score < 0)
	{
		score = 0;
	}
	writeLog.log("PythonCountScore:%d\n", score);

	return score;
}