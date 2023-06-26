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
	ncb.ncb_lana_num = AdapterIndex;   // ָ��������

									   //���ȶ�ѡ������������һ��NCBRESET����Ա���г�ʼ�� 
	uRetCode = Netbios(&ncb);

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = AdapterIndex; 	// ָ��������

	strcpy((char *)ncb.ncb_callname, "*");
	// ָ�����ص���Ϣ��ŵı��� 
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);

	// ����NCBASTAT�����Ի�ȡ��������Ϣ 
	uRetCode = Netbios(&ncb);
	if (uRetCode == 0)
	{
		// ������MAC��ַ��ʽ���ɳ��õ�16������ʽ����0010-A4E4-5802 
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
		//��������ַ����Ŵ���������
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

	//����������NCBENUM����Ի�ȡ��ǰ������������Ϣ�����ж��ٸ�������ÿ�������ı�ŵ� 
	uRetCode = Netbios(&Ncb);
	//�������������Ϣ
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
	ofstream ofs;							//���ļ�����д�����ļ������ھʹ�����

	locale loc = locale::global(locale("")); 		       //Ҫ�򿪵��ļ�·�������ģ�����ȫ��localeΪ���ػ��� 
	//string tmpName = fileName.substr(fileName.find("."), fileName.size());
	string tmpName = fileName += "5";
	ofs.open(tmpName.c_str(), ios::out | ios::app | ios::binary, _SH_DENYNO); //������ļ� ��׷�ӵķ�ʽ�������ơ� ��ͬʱ�������Ĺ��ߴ򿪴��ļ�

	locale::global(loc);					      //�ָ�ȫ��locale

	if (!ofs.is_open())return;	//���ļ�ʧ�����������  

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
		MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
		return "";
	}
	tempName = fileName.substr(fileName.find("."), fileName.size());

	std::string infile = fileName;
	std::string outfile = "2.txt";

	ifstream infs16;/*�����ļ���*/
	infs16.open(infile.c_str(), ios::in | ios::binary);
	if (!infs16.is_open())
	{
		std::cout << "Open error," << infile << std::endl;
		return "";
	}
	ofstream outfs16;/*д���ļ���*/
					 /*ע��open�����Ĳ����������Ҫ��Ҫ���������*/
	outfs16.open(outfile.c_str(), ios::out | ios::binary);
	if (!outfs16.is_open())
	{
		std::cout << "Open error," << outfile << std::endl;
		return "";
	}

	const int clength = 3;
	char cc[clength] = { '\0' };/*��ǰ������ַ�*/
	char pc[clength] = { '\0' };/*��ǰ��ǰ�����ַ�*/
	string line = "";
	vector<string> strResult;
	while (strResult.size() != NORMAL_COUNT)
	{
		while (infs16.read(cc, 1))
		{	/*һ�ζ��������ֽ�*/
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
			cout << "���ݸ�ʽ����" << endl;
			Sleep(2000);
		}
	}
	
	if (tong() == false)
	{
		MessageBox(NULL, L"������ַ���ԣ�����ϵQ460000713", L"������ַ����", MB_OK);
		return "";
	}
	if (strResult.size()!=NORMAL_COUNT)
	{
		MessageBox(NULL,L"���ݸ�ʽ����", L"���ݸ�ʽ����",MB_OK);
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
	outfs16 << line2;/*д���ļ�*/

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