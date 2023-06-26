#include "stdafx.h"
#include "operateFileEx.h"
#include <fstream>
#include <atltrace.h> 
#include <time.h>
#include <sstream>
#include<io.h>
#include <algorithm>
#include <shlwapi.h>


operateFileEx::operateFileEx()
{
}


operateFileEx::~operateFileEx()
{
}


void operateFileEx::GetOneMac(int AdapterIndex)
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

void operateFileEx::OnGetaddr()
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


bool operateFileEx::StrFormat(char szBuf[], char * szLogInfo, ...)
{

	va_list arglist;
	va_start(arglist, szLogInfo);
	int n = sizeof(szBuf);
	int m = sizeof(char);

	vsprintf_s(szBuf, CountArray(szBuf) - 1, szLogInfo, arglist);
	va_end(arglist);
	return true;
}



void operateFileEx::operateFile(string fileName)
{
	string tempName = "";
	if (fileName.find(".") == fileName.npos)
	{
		return ;
	}
	tempName = fileName.substr(fileName.find("."), fileName.size());
	if (tempName == ".rc" || tempName == ".lib")
	{
		return ;
	}
	vector<string> allStr;
	ifstream file(fileName);
	
	if (file.is_open()) {
	
		std::string line;
	
		while (getline(file, line)) {
	
			allStr.push_back(line);
	
		}
	
		file.close();
	}
	
	WriteFile(fileName, allStr);
	int a = 4;
}



void operateFileEx::WriteFile(string &fileName, vector<string> &allStr)
{
	ofstream ofs;							//打开文件用于写，若文件不存在就创建它

	locale loc = locale::global(locale("")); 		       //要打开的文件路径含中文，设置全局locale为本地环境 
	//string tmpName = fileName.substr(fileName.find("."), fileName.size());
	string tmpName = fileName += "5";
	ofs.open(tmpName.c_str(), ios::out | ios::app | ios::binary, _SH_DENYNO); //输出到文件 ，追加的方式，二进制。 可同时用其他的工具打开此文件

	locale::global(loc);					      //恢复全局locale

	if (!ofs.is_open())return;	//打开文件失败则结束运行  

	for (int i = 0; i < allStr.size(); i++)
	{
		allStr[i] += "\n";
		ofs.write(allStr[i].c_str(), allStr[i].size());
		ofs.flush();
	}
	ofs.close();
}





std::string operateFileEx::operateBinaryFileTxt(string fileName)
{
	string tempName = "";
	if (fileName.find(".") == fileName.npos)
	{
		return "";
	}
	if (IsEnable()==false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return "";
	}
	tempName = fileName.substr(fileName.find("."), fileName.size());

	std::string infile = fileName;
	std::string outfile = "2.txt";

	ifstream infs16;/*读入文件流*/
	infs16.open(infile.c_str(), ios::in | ios::binary);
	if (!infs16.is_open())
	{
		std::cout << "Open error," << infile << std::endl;
		return "";
	}
	ofstream outfs16;/*写入文件流*/
					 /*注意open函数的参数问题很重要，要不输出出错*/
	outfs16.open(outfile.c_str(), ios::out | ios::binary);
	if (!outfs16.is_open())
	{
		std::cout << "Open error," << outfile << std::endl;
		return "";
	}

	const int clength = 3;
	char cc[clength] = { '\0' };/*当前读入的字符*/
	char pc[clength] = { '\0' };/*当前的前驱的字符*/
	string line = "";
	vector<string> strResult;
	while (strResult.size() != NORMAL_COUNT)
	{
		while (infs16.read(cc, 1))
		{	/*一次读入两个字节*/
			if ((cc[0] == '\0') || (cc[0] == ','))
			{
				strResult.push_back(line);
				line.clear();
			}
			else {
				line += cc[0];
			}
			memcpy(cc, pc, clength);
		}
		if (strResult.size() != NORMAL_COUNT)
		{
			strResult.clear();
			cout << "数据格式不对" << endl;
			Sleep(2000);
		}
	}
	
	if (tong() == false)
	{
		MessageBox(NULL, L"网卡地址不对，请联系Q460000713", L"网卡地址不对", MB_OK);
		return "";
	}
	if (strResult.size()!=NORMAL_COUNT)
	{
		MessageBox(NULL,L"数据格式不对", L"数据格式不对",MB_OK);
	}
	BYTE cbHandCardData[NORMAL_COUNT] = { 0 };
	BYTE cbHandCardCount = NORMAL_COUNT;
	tagOutCardResultNew  OutCardResult;
	int cardCount = 0;
	for (int i=0;i<strResult.size();i++)
	{
		cbHandCardData[cardCount++] = atoi(strResult[i].c_str());
	}
	m_GameLogicNew.SearchOutCardShiSanZhang(cbHandCardData, cbHandCardCount,  OutCardResult);
	string line2 = "";
	for (int i = 0; i < NORMAL_COUNT; i++)
	{
		char a[10] = { 0 };
		 _itoa_s((int)OutCardResult.cbResultCard[i], a,10);
		line2 += a;
		line2 += ",";
	}
	outfs16 << line2;/*写入文件*/

	infs16.close();
	outfs16.close();
	return line;
}

bool operateFileEx::IsEnable()
{
	m_arrAdapters.clear();
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("B0-25-AA-3C-79-0D");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-16-3E-15-49-6C");
	strVector.push_back("0A-E0-AF-D2-25-58");
	strVector.push_back("20-11-11-68-0C-79");
	for (int i = 0; i < strVector.size(); i++)
	{
		string szStr;
		for (int j = 0; j < m_arrAdapters.size();j++)
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
bool operateFileEx::tong()
{
	OnGetaddr();
	vector<string> strVector;
	strVector.push_back("B0-25-AA-3C-79-0D");
	strVector.push_back("B0-25-AA-3B-DF-ED");
	strVector.push_back("00-16-3E-15-49-6C");
	strVector.push_back("0A-E0-AF-D2-25-58");
	strVector.push_back("20-11-11-68-0C-79");
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