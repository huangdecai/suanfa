#ifndef GAME_LOGIC_EW_HEAD_FILE
#define GAME_LOGIC_EW_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>
#include <algorithm>

//#include"CMD_Game.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_CUSTOM					2									//�Զ�����
#define ST_ASCENDING				3									//С��������
#define TOU_SHI                     false                               //�������Ƿ�͸�ӱ��˵���
#define JUE_DUI_DA                 2
#define XIANG_DUI_DA                 1
//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResultNew
{
	BYTE 							cbBlockCount[4];					//�˿���Ŀ
	BYTE							cbCardData[4][FULL_COUNT];			//�˿�����
};
//���ƽ��
struct tagInPyhonNew
{
	BYTE							cbHandCardData[MAX_COUNT];
	BYTE							cbHandCardCount;
	BYTE							cbTurnCardData[MAX_COUNT];
	BYTE							cbTurnCardCount;
	BYTE							cbDiscardCard[54];
	BYTE							cbDiscardCardCount;
	BYTE							cbOtherDiscardCard[54];
	BYTE							cbOtherDiscardCardCount;
	BYTE							cbCardDataEx[MAX_COUNT];
	BYTE							cbMaxCard[MAX_COUNT];
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

//���ƽ��
struct tagInPyhonCardType
{
	BYTE							cbHandCardData[MAX_COUNT];
	BYTE							cbHandCardCount;
	BYTE							cbType;
};


//���ƽ��
struct tagOutCardResultNew
{
	BYTE							cbCardType;						   //�˿�����
	float							    cbCardScore;					   //���ͷ�ֵ
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

//�ֲ���Ϣ
struct tagDistributingNew
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbDistributing[15][6];				//�ֲ���Ϣ
};

#define MAX_TYPE_COUNT 256
struct tagOutCardTypeResultNew 
{
	BYTE							cbCardType;							//�˿�����
	int							cbCardTypeCount;					//������Ŀ
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//ÿ�ָ���
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//�˿�����
};


struct tagCardIndex
{
	BYTE		cbIndex;						//�˿�����
	BYTE		cbCardData;			//�˿�����
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogicNew
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����

	//AI����
public:
	BYTE                            m_cbMaxCard[MAX_COUNT];
	BYTE                            m_cbCardDataEx[MAX_COUNT];
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE							m_cbUserCardCount[GAME_PLAYER];		//�˿���Ŀ
	BYTE							m_cbDiscardCard[FULL_COUNT];//������
	BYTE                            m_cbOtherDiscardCard[MAX_COUNT];
	WORD							m_wBankerUser=0;	
	WORD                            m_wMeChairID = 0;
	BYTE                            m_cbDiscardCardCount = 0;
	BYTE                            m_cbOtherDiscardCount = 0;
	BYTE                            m_cbFirstCard = 0;
	BYTE                            m_cbCardTypeCount = 0;//��16�Ż���15��
	bool                            m_bHavePass = false;
	bool                            m_bShenBao = false;
	bool                            m_bTrunMode = false;

	//��������
public:
	//���캯��
	CGameLogicNew();
	//��������
	virtual ~CGameLogicNew();
	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//��ȡĳ�����ж��ٸ�
	int GetACardCount(const BYTE cbCardBuffer[], const BYTE cbBufferCount, BYTE cbCard);
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//�����ظ��߼�ֵ������
	int ClearReLogicValue(  BYTE cbCardData[], BYTE cbCardCount);
	//�߼�����
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//��������
	BYTE SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb=false);

	//�ڲ�����
public:
	//�����˿�
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResultNew & AnalyseResult);
	//�����ֲ�
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributingNew & Distributing);

	//////////////////////////////////////////////////////////////////////////
public:
	//�����˿�
	VOID SetUserCard(WORD wChairID, const BYTE cbCardData[], BYTE cbCardCount);
	//���õ���
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//�Ƿ���ڳ��ҵ��ƺ������Ƶ�ը��
	void SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbOtherCardData[], BYTE cbOtherCardCount);
	void ReSetData();
	//����ׯ��
	VOID SetBanker(WORD wBanker) ;
	//�����˿�
	VOID SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//ɾ���˿�
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;
	//��������
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	//��������
public:
	//����㷨
	VOID Combination(BYTE cbCombineCardData[], int cbResComLen, BYTE cbResultCardData[MAX_RESULT_COUNT][MAX_COLS], int &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	bool IsReCombination(BYTE cbResultCardData[][MAX_COLS], BYTE cbCardData[MAX_COLS], int cbResCardLen, int cbNeedCardCount);
	//����ը��
	VOID GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//����˳��
	VOID GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount, tagOutCardTypeResultNew   *CardTypeResult=NULL);
	//��������
	VOID GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//��������
	VOID GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//��������
	VOID GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);

	//��Ҫ����
public:
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
	//�������ƣ��ȳ��ƣ�
	VOID BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	//�������ƣ�����ƣ�
	VOID BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);

	//�����ϼң��ȳ��ƣ�
	VOID UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
	//�����ϼң�����ƣ�
	VOID UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);
	//�����¼ң��ȳ��ƣ�
	VOID UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
	//�����¼ң�����ƣ�
	VOID UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);
public:

	//�ж��Ƿ��������
	bool SearchOtherHandCardThan(const BYTE cbHandCardData[], BYTE cbHandCardCount, bool bNoSearchBomb);
	//���������������Ƿ�����ͬ��
	bool SearchOtherHandCardSame(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType);
	//��ȡ��Ի��߾��Դ���
	int  IsJueDuiDaPai(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType, bool bNoSearchBomb=true);

	float GetHandScore(vector<tagOutCardResultNew>   &CardTypeResult,int minTypeCount);

	float GetCardTypeScore(tagOutCardResultNew &CardTypeResult);

	int RemoveTypeCard(tagOutCardTypeResultNew & CardTypeResult, BYTE * cbTmpHReminCardData, int &iLeftCardCount);

	void GetMissileCardResult(BYTE * cbHandCardData, BYTE cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetFourTakeOneResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetFourTakeTwoResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetDoubleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetSingleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetBombCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount,  tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeTakeTwoCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeTakeOneCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeLineCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetDoubleLineCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetSingleLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);
	//�ҳ��˿���ϳ���С������
	int FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, float &MinTypeScore, bool &bBiYing, bool bSearchBiYing = true);
	//������Ƶ��ظ����
	void clearSingleReResult(tagOutCardTypeResultNew * CardTypeResult);
	//������
	void FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, BYTE * cbTmpHReminCardData, BYTE const cbHandCardCount, vector<vector<tagOutCardResultNew> > &resultAllVec);
	//
	//�����˿�
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);
	//ͬ������
	BYTE SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
		tagSearchCardResult *pSearchCardResult);
	//������������(����һ���Ĵ�һ��)
	BYTE SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
		tagSearchCardResult *pSearchCardResult);
	//�����ɻ�
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbLineCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult);
	//���̨��ը������
	bool CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount);
	//������������Ƿ��ཻ
	bool CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount);
	//��ⵥ˫�ƻ����������Ƿ�����С����
	void OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> vecMinTypeCardResult);
public:
	//���Ʋ����㷨
	//���ȴ����ջ�������
	int YouXianDaNengShouHuiCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>&  vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu);
	//ѹ��һ���ƺ��Ƿ�����ʤģʽ
	int IsBiShengTurnCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu);
	
	int DanShuangNengShouHuiLaiCheck(vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);

	//���ȳ�����
	int SearchMutilType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>&  vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);
	//��Ӯ����
	bool isBiYing(vector<tagOutCardResultNew > &  CardTypeResult);
	//ͨ�õ�˫ѹ�Ʋ���
	void CommonSDTurnCardSearch(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbTurnCardLogic, tagOutCardResultNew &OutCardResult);
	//�޵м��
	bool  WuDiCheck(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE * cbTurnCardData, BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult);
	//���ʣ�����Ƿ��Ӯ
	bool CheckOutOneTypeWillWin(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult);
	//����ʣ���������
	bool OutCardShengYuFenCheck(BYTE cbHandCardCount, const BYTE * cbHandCardData, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, float MinTypeScoreVec, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	bool DanDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing);
	bool ShuangDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing);
	//���Ʋ�ּ��
	bool DoubleChaiFenCheck(vector<tagOutCardResultNew> &TempMinTypeCardResult, const BYTE * cbHandCardData, BYTE cbHandCardCount, WORD wOutCardUser);
	//ũ���ǵ����ж�
	bool NoOutCardUserHandCardCountCheck(WORD wOutCardUser, BYTE cbCardCount,bool bXiaoYu=true,bool bYu=true,bool bOnlyDengYu=false);
	//������һ�������������ҳ���С�Ķ�
	vector<BYTE> SearchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult);
	//������һ�����������н�����С�Ĵ���
	bool SwitchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult);
	//�ж��Ƿ���������ʹ�����һ����
	bool FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);

	//����һ�������϶��ֲ����
	void FourTakeOneOrTwoFenChaiCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult);
	//���Ƴ������ڶ�����
	void OutCardDaoShuDiErDaCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult);
	//--��˫����С����
	void OutSingleOrDoubleMinCard(const BYTE * cbHandCardData, BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult, int type);
	//�ж��Ƿ����׳�
	bool FindFirstType(const BYTE cbHandCardData[], BYTE cbHandCardCount,  vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);
public:
	//
	//�з��ж�
	//�з��ж�
	int LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	float calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult);
	//��������
	bool SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount, BYTE	cbOtherDiscardCard[], BYTE	cbOtherDiscardCount, BYTE cbCardDataEx[], BYTE MaxCard[], tagOutCardResultNew & OutCardResult);
	//�������ƣ��ȳ��ƣ�
	VOID ErZhuDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	//�������ƣ�����ƣ�
	VOID ErBeiDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);

	void BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData);

	//--����1�任3��2����
	bool ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	bool ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	void SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count);

	float GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount);
	/*--��������Ҫ�õ�������ϵı��ʽ
		-- ע��(m > n)
		--C(m, n) = (m*(m - 1)*(m - 2)��(m - n + 1)) / (n*(n - 1)*�� * 1)*/
	int GetCMNSort(int m, int n);

	bool isAllDoubleType(BYTE * cbComResCard, BYTE CardCount);

};

//////////////////////////////////////////////////////////////////////////

#endif