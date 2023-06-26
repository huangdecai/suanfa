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
#include "cwritelog.h"
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
	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { };
	//BYTE    tempCard[] = { 0x2B, 0x0B, 0x05, 0x1D, 0x0D, 0x2D, 0x32, 0x33, 0x26, 0x16, 0x34, 0x14, 0x04 };
	//BYTE    tempCard[] = { 0x3C, 0x32, 0x02, 0x3A, 0x09, 0x04, 0x24, 0x03, 0x1D, 0x1B, 0x1A, 0x17, 0x13 };
	//BYTE    tempCard[] = { 0x1C, 0x12, 0x02, 0x38, 0x27, 0x05, 0x23, 0x03, 0x19, 0x28, 0x37, 0x26, 0x25 };
	BYTE    tempCard[] = { 2, 2, 12, 12, 11, 11, 11, 8, 7 };
	//BYTE    tempCard[] = { 4, 38, 8, 56, 10, 9, 11, 43, 12, 44, 60, 1, 33 };

	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { 0x02 };
	tagOutCardResultNew  OutCardResult;
	BYTE cbTurnCardData[] = {6 };
	int cbTurnCardCount = 1;// sizeof(cbTurnCardData);
	int cbRangCardCount = 0;
	int cbOthreRangCardCount = 0;
	BYTE cbDiscardCard[] = { 9, 8, 8, 8, 10, 10, 10, 5, 13, 13, 13, 10, 7, 7, 6, 6, 5, 5, 12, 12, 1, 1, 9, 1, 78, 79, 6 };
	BYTE cbDiscardCount = sizeof(cbDiscardCard);
	BYTE maxCard[MAX_COUNT] = { 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	BYTE cbCardDataEx[MAX_COUNT] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	WORD wBankerUser = 0;
	WORD wUndersideUser = (wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	m_GameLogicNew.SetBanker(wBankerUser);

	m_GameLogicNew.SetUserCard(wBankerUser, tempCard, sizeof(tempCard));
	//m_GameLogicNew.SetUserCard(wUndersideUser, tempCard, sizeof(tempCard));
	//m_GameLogicNew.SetUserCard(wUpsideUser, tempCard, sizeof(tempCard));

	tagOutCardResultNew CardTypeResult1;
	CardTypeResult1.cbCardCount = 5;
	BYTE    tempCard1[] = { 0x01, 0x02, 0x33, 0x34, 0x05 };
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

	int cbCurrentLandScore = 0;
	double tempScore = m_GameLogicNew.LandScoreErRen(tempCard, sizeof(tempCard), cbCurrentLandScore);
	//int a = 4;
	////
	BYTE    tempCardEX[3] = { tempCard[0],tempCard[1],tempCard[2] };
	//int turnCount = m_GameLogicNew.CalCardTurnCount(tempCardEX, sizeof(tempCardEX), tempCard, sizeof(tempCard), NULL, 0,1);

	for (int i = 0; i < 1000;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[DISPATCH_COUNT] = { 0 };
		m_GameLogicNew.RandCardList(bCardData, DISPATCH_COUNT);
		////BYTE    tempCard[] = { 0x2d, 0x0d, 0x1b, 0x0b, 0x2a, 0x0a, 0x19, 0x37, 0x17, 0x36, 0x26, 0x33, 0x03, };
		//CopyMemory(tempCard, bCardData, MAX_COUNT);
		m_GameLogicNew.SearchOutCardErRen(tempCard, sizeof(tempCard), cbTurnCardData, cbTurnCardCount, cbDiscardCard, cbDiscardCount, cbCardDataEx, maxCard, OutCardResult);
		fihst = clock();
		double duration = (double)(fihst - start);
		if (duration>2000)
		{
			int a=4;
			cout << duration << endl;
		}
	}
	
	
	//CString str;
	//str.Format(L"time:%f", duration);
	//CTraceService::TraceString(str, TraceLevel_Exception);

}

int _tmain(int argc, _TCHAR* argv[])
{
	TestGameLogic(); 
	return 0;
}

