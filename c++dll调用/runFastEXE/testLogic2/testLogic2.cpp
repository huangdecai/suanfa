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
	BYTE    tempCard[] = { 0x33, 0x23 };
	//BYTE    tempCard[] = { 33, 45, 60, 11, 42, 9, 40, 8, 22, 36, 20, 4, 2 };

	//BYTE    tempCard[] = { 0x09 };
	//BYTE    tempCard[] = { 0x02 };
	CopyMemory(cbHandCardData, tempCard, sizeof(tempCard));
	tagOutCardResultNew  OutCardResult;
	BYTE cbTurnCardData[] = { 0x33,0x23 };
	int cbTurnCardCount = sizeof(cbTurnCardData);
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



	/*clock_t start, fihst;
	start = clock();*/

	//int cbCurrentLandScore = 0;
	//double tempScore = m_GameLogicNew.LandScore(tempCard, sizeof(tempCard), cbCurrentLandScore);
	//int a = 4;
	////
	int total = m_GameLogicNew.GetCMNSort(52, 2);

	for (int i = 0; i < 1000;i++)
	{
		BYTE    tempCard1[] = { 0x21, 0x01 };
		float gailv = m_GameLogicNew.GetCardTurnPercent(tempCard1, sizeof(tempCard1), tempCard1, sizeof(tempCard1), cbDiscardCard, 0);
		int a = 4;
	}

	for (int i = 0; i < 500;i++)
	{
		clock_t start, fihst;
		start = clock();
		BYTE bCardData[DISPATCH_COUNT] = { 0 };
		m_GameLogicNew.RandCardList(bCardData, DISPATCH_COUNT);
		////BYTE    tempCard[] = { 0x2d, 0x0d, 0x1b, 0x0b, 0x2a, 0x0a, 0x19, 0x37, 0x17, 0x36, 0x26, 0x33, 0x03, };
		CopyMemory(tempCard, bCardData, sizeof(tempCard));
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

