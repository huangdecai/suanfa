#ifndef _ANDROIDAI_H_
#define _ANDROIDAI_H_

#pragma once

#include "GameLogic.h"

class CAndroidAIBase
{
public:
	CAndroidAIBase();
	~CAndroidAIBase();
	//������
	void SetCardData( const BYTE cbCardData[],BYTE byCardCount, tagWeaveItem wi[MAX_WEAVE], BYTE cbWeaveCount );	
	//��ȡ�����
	BYTE GetBadlyCard();													
	//��ȡ����
	int GetMaxScore();	
	//˼��
	virtual void Think()=NULL;

protected:
	//����Ƿ��Ǳ�
	bool IsEdge( BYTE byCard1,BYTE byCard2 );
	
	//������ͬ��
	bool SearchSameCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 );
	//��������
	bool SearchLinkCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 );

	//������ֻͬ��
	bool SearchSameCardRemain( BYTE byCardData,BYTE &byIndex,BYTE byStart );
	//�����п�����
	bool SearchLinkCardRemain( BYTE byCardData,BYTE byLinkType,BYTE &byIndex,BYTE byStart );

	//������
	BYTE FindIndex( BYTE byCardData,BYTE byStart = 0 );
	//���Ƴ������ֻ��������
	BYTE FindIndexRemain( BYTE byCardData,BYTE byStart = 0 );

public:
	//�Ƴ���
	bool RemoveCard( BYTE byCardIndex );
	//�Ƴ���ֵ
	bool RemoveCardData( BYTE byCardData );

protected:
	BYTE		m_byCardData[MAX_COUNT];				//ԭʼ��
	BYTE		m_byCardCount;							//ԭʼ����

	bool		m_bSelect[MAX_COUNT];					//���Ƿ�ѡ��

	BYTE		m_byThreeCard[MAX_COUNT];				//���������
	BYTE		m_byThreeCount;							//�����������
	BYTE		m_byGoodThreeCard[MAX_COUNT];			//������������
	BYTE		m_byGoodThreeCount;						//��������������

	BYTE		m_byTwoCard[MAX_COUNT];					//��ֻ�����
	BYTE		m_byTwoCount;							//��ֻ�������
	BYTE		m_byGoodTwoCard[MAX_COUNT];				//�����ֻ�����
	BYTE		m_byGoodTwoCount;						//�����ֻ�������

	BYTE		m_byRemainThree[MAX_COUNT];				//��ȥ�����ֻ��Ϻ����
	BYTE		m_byRemainThreeCount;					//��ȥ�����ֻ��Ϻ������

	BYTE		m_byRemainTwo[MAX_COUNT];				//��ȥ�����ֻ��ϼ���ֻ��Ϻ����
	BYTE		m_byRemainTwoCount;						//��ȥ�����ֻ��ϼ���ֻ��Ϻ������

	int			m_nMaxScoreThree;						//�����ֻ����Ϸ���
	int			m_nMaxScoreTwo;							//�����ֻ����Ϸ���
	int			m_nActionScore;							//ģ������÷�

	int			m_nScoreThree;							//��ʱ��ֻ����Ϸ���
	int			m_nScoreTwo;							//��ʱ��ֻ����Ϸ���

	bool		m_bHaveJiang;							//�Ƿ��н�

	BYTE		m_byBadlyCard;							//�����

	//������������
	BYTE		m_cbColorCount;							//��ɫ��Ŀ
	BYTE		m_cbColor[4];							//��ɫ��Ŀ
	BYTE		m_cbMinColorCount;						//���ٻ�ɫ
};

class CAndroidAI:public CAndroidAIBase
{
public:
	CAndroidAI(void);
	~CAndroidAI(void);
	//˼��
	virtual void Think();
	//�������ֻ����������������
	BYTE GetBadlyIn3Card();
	//�������ֻ����������������
	BYTE GetBadlyIn2Card();
	//���ý�ֹ������
	void SetEnjoinOutCard( const BYTE cbEnjoinOutCard[],BYTE cbEnjoinOutCount );
	//ģ�����
	void SetAction(BYTE byActionMask, BYTE byActionCard, int GangKind=0);
	//���õ÷�
	void ResetScore();
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
protected:
	//��Ȩ
	int AddScore( BYTE byCardData );
	//������ֻ
	void AnalyseThree();
	//������ֻ
	void AnalyseTwo();
	//������ֻ
	void AnalyseOne();
	//����ֻ����з���
	void SearchTwo();
	//�ж����Ƿ��ֹ��
	bool IsEnjoinOutCard( BYTE byCard );
public:
	//��������
	bool SearchOutCard(tagOutCardResult &OutCardResult, WORD wMeChairId, WORD  wCurrentUser, BYTE cbCardDataEx[], BYTE  cbActionMask, BYTE	cbActionCard, BYTE cbCardIndex[], tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[], BYTE cbDiscardCard[], BYTE cbDiscardCount);

	int ActionAfterScore(WORD wMeChairId, BYTE * cbCardIndex, BYTE * cbShengYuIndex, tagWeaveItem * WeaveItemArray, BYTE  cbWeaveCount, BYTE * cbDiscardCard, BYTE cbDiscardCount, BYTE cbActionCard, int  actionType,bool bJianAction);

	void GetTingData(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbShengYuCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CMD_S_TING_DATA &TingData);
	void GetRemainingCount(tagWeaveItem WeaveItemArray[GAME_PLAYER][MAX_WEAVE], BYTE cbWeaveCount[], const BYTE cbCardIndex[MAX_INDEX], BYTE cbShengYuIndex[MAX_INDEX], BYTE cbDiscardCard[], BYTE cbDiscardCount);
	int SearchTingTotalCount(WORD wMeChairId, BYTE cbCardIndex[], BYTE cbShengYuIndex[MAX_INDEX], tagWeaveItem WeaveItemArray[MAX_WEAVE], BYTE cbWeaveCount, BYTE cbDiscardCard[], BYTE cbDiscardCount);
	int SearchTingErXiangTingTotalCount(WORD wMeChairId, BYTE cbCardIndex[], BYTE cbShengYuIndex[MAX_INDEX], tagWeaveItem WeaveItemArray[MAX_WEAVE], BYTE cbWeaveCount, BYTE cbDiscardCard[], BYTE cbDiscardCount)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

		BYTE cbCardCount = m_GameLogic.GetCardCount(cbCardIndexTemp);
		if (((cbCardCount + 1) % 3 != 0)|| cbCardCount==1)
		{
			return 0;
		}
		int totalNum = 0;
		BYTE cbRes = m_GameLogic.AnalyseTingCard(cbCardIndex, WeaveItemArray, cbWeaveCount);
		if (cbRes == WIK_LISTEN)
		{
			return 0;
		}
		
		return totalNum;
	}
protected:
	BYTE m_byEnjoinOutCard[MAX_COUNT];				//��ֹ������
	BYTE m_byEnjoinOutCount;						//��ֹ��������
};

#endif