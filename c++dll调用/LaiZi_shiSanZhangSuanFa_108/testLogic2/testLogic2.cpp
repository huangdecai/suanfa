// testLogic2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
using namespace std;
#include "GameLogicNew.h"
#include <time.h>
CGameLogicNew  m_GameLogicNew;
#include <objbase.h>
#include <map>
#include <ctime>
#include<sys/timeb.h>
static int getRandEx(int nMin, int nMax)
{
	if (nMax <= nMin)
		return nMin;
	static std::default_random_engine generator(time(0));
	std::uniform_int_distribution<int> dis(nMin, nMax);
	return dis(generator);
}

void TestGameLogic()
{
	
	BYTE	cbHandCardData[NORMAL_COUNT] = { 0 };	//手上扑克
	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { };
	//BYTE    tempCard[] = { 0x2B, 0x0B, 0x05, 0x1D, 0x0D, 0x2D, 0x32, 0x33, 0x26, 0x16, 0x34, 0x14, 0x04 };
	//BYTE    tempCard[] = { 0x3C, 0x32, 0x02, 0x3A, 0x09, 0x04, 0x24, 0x03, 0x1D, 0x1B, 0x1A, 0x17, 0x13 };
	//BYTE    tempCard[] = { 1, 1, 3, 3, 19, 35, 7, 8, 9, 19, 11, 11, 13 };
	BYTE    tempCard[] = { 0x3C, 0x23, 0x03, 0x3B, 0x0B, 0x28, 0x27, 0x34, 0x4f, 0x4e, 0x1D, 0x1B, 0x1A };

	//BYTE    tempCard[] = { 4, 38, 8, 56, 10, 9, 11, 43, 12, 44, 60, 1, 33 };

	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { 0x02 };
	CopyMemory(cbHandCardData, tempCard, sizeof(tempCard));
	tagOutCardResultNew  OutCardResult;
	BYTE cbTurnCardData[] = { 0x37 };
	int cbTurnCardCount = 0;
	int cbRangCardCount = 0;
	int cbOthreRangCardCount = 0;
	BYTE cbDiscardCard[] = { 0x01 };
	WORD wBankerUser = 0;
	WORD wUndersideUser = (wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	m_GameLogicNew.SetDiscardCard(cbDiscardCard, sizeof(cbDiscardCard));
	m_GameLogicNew.SetBanker(wBankerUser);

	m_GameLogicNew.SetUserCard(wBankerUser, tempCard, sizeof(tempCard));
	m_GameLogicNew.SetUserCard(wUndersideUser, tempCard, sizeof(tempCard));
	m_GameLogicNew.SetUserCard(wUpsideUser, tempCard, sizeof(tempCard));

	tagOutCardResultNew CardTypeResult1;
	CardTypeResult1.cbCardCount = 5;
	BYTE    tempCard1[] = { 0x13, 0x23, 0x4f, 0x26, 0x16 };
	BYTE    tempCard2[] = { 0x09, 0x0A, 0x3B, 0x3C, 0x0D };
	BYTE    tempCard3[] = { 0x01, 0x0A, 0x3B, 0x3C, 0x0D };
	CopyMemory(CardTypeResult1.cbResultCard, tempCard1, CardTypeResult1.cbCardCount);
	float score1 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);
	CopyMemory(CardTypeResult1.cbResultCard, tempCard2, CardTypeResult1.cbCardCount);
	float score2 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);
	CopyMemory(CardTypeResult1.cbResultCard, tempCard3, CardTypeResult1.cbCardCount);
	float score3 = m_GameLogicNew.GetCardTypeScore(CardTypeResult1);

	/*clock_t start, fihst;
	start = clock();*/
	BYTE maxCard = 0;
	int tmpType= m_GameLogicNew.GetCardType(tempCard1, 5, maxCard);

	for (int i = 0; i < 500;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[DISPATCH_COUNT] = { 0 };
		m_GameLogicNew.RandCardList(bCardData, DISPATCH_COUNT);
		////BYTE    tempCard[] = { 0x2d, 0x0d, 0x1b, 0x0b, 0x2a, 0x0a, 0x19, 0x37, 0x17, 0x36, 0x26, 0x33, 0x03, };
		CopyMemory(tempCard, bCardData, sizeof(tempCard));
		string str;
		for (int j=0;j<sizeof(tempCard);j++)
		{
			str += to_string(tempCard[j])+",";
		}
		cout << str<< endl;
		m_GameLogicNew.SearchOutCardErRen(tempCard, sizeof(tempCard), cbTurnCardData, cbTurnCardCount, cbDiscardCard, sizeof(cbDiscardCard), cbRangCardCount, cbOthreRangCardCount, OutCardResult);
		str = "result:";
		for (int j = 0; j < OutCardResult.cbCardCount; j++)
		{
			str += to_string(OutCardResult.cbResultCard[j]) + ",";
		}
		cout << str << endl;
		int count = 0;
		for (int j = 0; j < NORMAL_COUNT;j++)
		{
			if (OutCardResult.cbResultCard[j]==0x4F)
			{
				count++;
			}
		}
		if (count > 0)
		{
			int a = 4;
		}
		else if (count>1)
		{
			int a = 4;
		}
		fihst = clock();
		double duration = (double)(fihst - start);
		
		if (duration>2000)
		{
			cout << duration << endl;
		}
	}
	system("pause");
	
	//CString str;
	//str.Format(L"time:%f", duration);
	//CTraceService::TraceString(str, TraceLevel_Exception);

}

int _tmain(int argc, _TCHAR* argv[])
{
	TestGameLogic(); 
	return 0;
}

