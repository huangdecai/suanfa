// testLogic2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;
#include "GameLogicNew.h"
#include <time.h>
CGameLogicNew  m_GameLogicNew;

void TestGameLogic()
{

	BYTE	cbHandCardData[NORMAL_COUNT] = { 0 };	//手上扑克
	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { };
	//BYTE    tempCard[] = { 0x2B, 0x0B, 0x05, 0x1D, 0x0D, 0x2D, 0x32, 0x33, 0x26, 0x16, 0x34, 0x14, 0x04 };
	//BYTE    tempCard[] = { 0x3C, 0x32, S0x02, 0x3A, 0x09, 0x04, 0x24, 0x03, 0x1D, 0x1B, 0x1A, 0x17, 0x13 };
	//BYTE    tempCard[] = { 0x1C, 0x12, 0x02S, 0x38, 0x27, 0x05, 0x23, 0x03, 0x19, 0x28, 0x37, 0x26, 0x25 };
	BYTE    tempCard[] = { 0x39, 0x29, 0x3C, 0x3D, 0x0D, 0x1D, 0x14, 0x35, 0x16, 0x27, 0x18, 0x09, 0x3A };

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

	//int cbCurrentLandScore = 0;
	//double tempScore = m_GameLogicNew.LandScore(tempCard, sizeof(tempCard), cbCurrentLandScore);
	//int a = 4;
	////


	for (int i = 0; i < 500;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[DISPATCH_COUNT] = { 0 };
		m_GameLogicNew.RandCardList(bCardData, DISPATCH_COUNT);
		////BYTE    tempCard[] = { 0x2d, 0x0d, 0x1b, 0x0b, 0x2a, 0x0a, 0x19, 0x37, 0x17, 0x36, 0x26, 0x33, 0x03, };
		//CopyMemory(tempCard, bCardData, sizeof(tempCard));
		m_GameLogicNew.SearchOutCardErRen(tempCard, sizeof(tempCard), cbTurnCardData, cbTurnCardCount, cbDiscardCard, sizeof(cbDiscardCard), cbRangCardCount, cbOthreRangCardCount, OutCardResult);
		fihst = clock();
		double duration = (double)(fihst - start);
		if (duration>2000)
		{
			int a=4;
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

