#ifndef GAME_LOGIC_EW_HEAD_FILE
#define GAME_LOGIC_EW_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>
#include <algorithm>
//#include"CMD_Game.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
const int douNum[DOU_NUM] = { DOU_HAND_COUNT, DOU_HAND_COUNT, 3 };
//�߼�����
#define CT_INVALID					0									//��������
#define CT_SINGLE					1									//��������
#define CT_ONE_DOUBLE					2								//��������
#define CT_TWO_DOUBLE					3								//��������
#define CT_THREE						4								//��������
#define CT_FIVE_MIXED_FLUSH_NO_A		5								//��ͨ˳��
#define CT_FIVE_MIXED_FLUSH_FIRST_A		6								//Aǰ˳��
#define CT_FIVE_MIXED_FLUSH_BACK_A		7								//A��˳��
#define CT_FIVE_FLUSH					8								//ͬ��
#define CT_FIVE_THREE_DEOUBLE			9								//����һ��
#define CT_FIVE_FOUR_ONE				10								//�Ĵ�һ��

#define CT_FIVE_STRAIGHT_FLUSH			11								//ͬ��˳��
#define CT_FIVE_STRAIGHT_FLUSH_FIRST_A	12								//Aͬ��˳
#define CT_FIVE_STRAIGHT_FLUSH_BACK_A	13						        //Aͬ��˳��
#define CT_FIVE_TONG				14								    //�������
#define CT_SIX_TONG	                    15						        //6ͬ
#define CT_TYPE_COUNT				16									//�������

#define CT_MISSILE_CARD				20								    //�������

//��������
#define CT_EX_INVALID					14								//��������
#define CT_EX_SANTONGHUA				15								//��ͬ��
#define CT_EX_SANSHUNZI					16								//��˳��
#define CT_EX_LIUDUIBAN					17								//���԰�
#define CT_EX_WUDUISANTIAO				18								//�������
#define CT_EX_SITAOSANTIAO				19								//��������
#define CT_EX_SHUANGGUAICHONGSAN		20								//˫�ֳ���
#define CT_EX_COUYISE					21								//��һɫ
#define CT_EX_QUANXIAO					22								//ȫС
#define CT_EX_QUANDA					23								//ȫ��
#define CT_EX_SANFENGTIANXIA			24								//��������
#define CT_EX_SANTONGHUASHUN			25								//��ͬ��˳
#define CT_EX_SHIERHUANGZU				26								//ʮ������
#define CT_EX_YITIAOLONG				27								//һ����
#define CT_EX_ZHIZUNQINGLONG			28								//��������
#define	CT_EX_QUANHEIYIDIANHONG			29								//ȫ��һ���
#define	CT_EX_QUANHONGYIDIANHEI			30								//ȫ��һ���

const int CARD_TYPE_SCORE[CT_TYPE_COUNT] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 110, 140, 150 };
#define  G_THREE_TAKE_TWO_DAN true
#define  G_THREE_TAKE_TWO_EX false

//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_CUSTOM					2									//�Զ�����
#define ST_ASCENDING				3									//С��������
#define TOU_SHI                     false                               //�������Ƿ�͸�ӱ��˵���

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResultNew
{
	BYTE 							cbBlockCount[8];					//�˿���Ŀ
	BYTE							cbCardData[8][MAX_COUNT];			//�˿�����
	BYTE							cbKingCount;
};
//���ƽ��
struct tagInPyhonNew
{
	BYTE							cbHandCardData[MAX_COUNT];
	BYTE							cbHandCardCount;
	BYTE							cbTurnCardData[MAX_COUNT];
	BYTE							cbTurnCardCount;
	BYTE							DiscardCard[54];
	BYTE							cbDiscardCardCount;
	BYTE							cbRangCardCount;	//�˿�����
	BYTE							cbOthreRangCardCount;	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
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

#define MAX_TYPE_COUNT 300
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
	WORD							m_wBankerUser;	
	WORD                            m_wMeChairID = 0;
	BYTE                            m_cbDiscardCardCount = 0;
	BYTE                            m_cbOthreRangCardCount = 0;
	bool                            m_bSanDaYi = false;
	bool                            m_bHavePass = false;

	//��������
public:
	//���캯��
	CGameLogicNew();
	//��������
	virtual ~CGameLogicNew();
	//���ͺ���
public:
	//��ȡ����
	//��ȡ����
	BYTE GetCardType(BYTE bCardData[],  BYTE bCardCount, BYTE & bMaxCardData);
	//�����˿�
	void AnalyseCard(const BYTE bCardDataList[], const BYTE bCardCount, tagAnalyseData& AnalyseData);
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
	//�Ƴ����ļ���
	int RemoveCardWang(BYTE bCardData[], BYTE bCardCount, BYTE bWangResultCardData[]);
	//�����ظ��߼�ֵ������
	int ClearReLogicValue(  BYTE cbCardData[], BYTE cbCardCount);
	//�߼�����
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetMaxCardLogic(BYTE bCardData[], BYTE bCardCount, bool bEnable);
	//�Ա��˿�
	//�Ƚ��˿�
	int CompareCard(BYTE bInFirstList[], BYTE bInNextList[], BYTE bFirstCount, BYTE bNextCount, bool bCompareLogic, bool bCompareHuaSe);

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
	int GetTakeWangCount(BYTE bCardData[], BYTE bCardCount);
	BYTE GetTakeWangCardType(BYTE bCardData[], BYTE bCardCount, BYTE & bMaxCardData, tagAnalyseData &AnalyseData);
	bool IsContainA(BYTE cbCardData[], BYTE cbCardCount);
	//////////////////////////////////////////////////////////////////////////
public:
	//�����˿�
	VOID SetUserCard(WORD wChairID, const BYTE cbCardData[], BYTE cbCardCount);
	//���õ���
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//�Ƿ���ڳ��ҵ��ƺ������Ƶ�ը��
	void SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount);
	void ReSetDiscardCard();
	//����ׯ��
	VOID SetBanker(WORD wBanker) ;
	//�����˿�
	VOID SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//ɾ���˿�
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;
	//��������
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	BYTE SearchAllLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult);
	//��������
	void GetNextLineCard(BYTE cbIndexSatart, BYTE cbIndexEnd, BYTE cbLineCount, int needLaiZiCount, BYTE cbWangCount, BYTE cbWangCardData[MAX_COUNT], vector<BYTE> & lineArray, tagDistributingNew& Distributing, tagSearchCardResult *pSearchCardResult);
	int GetCardMaxHuaSe(BYTE lineArray[], int lineArrayCount, int &color);
public:
	//����㷨
	VOID Combination(BYTE cbCombineCardData[], int cbResComLen,  BYTE cbResultCardData[500][8], int &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
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
public:

	//���������������Ƿ�����ͬ��
	bool SearchOtherHandCardSame(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType);

	float GetHandScore(vector<tagOutCardResultNew>   &CardTypeResult,int minTypeCount);

	float GetCardTypeScore(tagOutCardResultNew &CardTypeResult);

	int RemoveTypeCard(tagOutCardTypeResultNew & CardTypeResult, BYTE * cbTmpHReminCardData, int &iLeftCardCount);

	void GetMissileCardResult(BYTE * cbHandCardData, BYTE cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetFourTakeOneResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetFourTakeTwoResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetDoubleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetSingleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetTongHuaResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetBombCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount,  tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeTakeTwoCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeTakeOneCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetThreeLineCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetDoubleLineCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);

	void GetSingleLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult);
	//�ҳ��˿���ϳ���С������
	int FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, vector<vector<tagOutCardResultNew>>   &CardResultShao, float &MinTypeScore, vector<float> &MinTypeScoreShao, bool &bBiYing, bool bSearchBiYing = true);
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
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult);
	//���̨��ը������
	bool CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount);
	//������������Ƿ��ཻ
	bool CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount);
	//��ⵥ˫�ƻ����������Ƿ�����С����
	void OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew>& vecMinTypeCardResult);
public:
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
	//�ж��Ƿ���������ʹ�����һ����
	bool FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);
		
public:
	//
	//�з��ж�
	int LandScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	//�з��ж�
	int LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	float calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult);
	//��������
	bool SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, tagOutCardResultNew & OutCardResult);
	VOID SearchOutCardShiSanZhang(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	VOID SearchOutCardShiSanZhangTurn(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult, tagOutCardResultNew & TurnOutCardResult, tagOutCardResultNew & FirstOutCardResult);

	int TrunCheck(BYTE  ArrayTurn[DOU_NUM][DOU_HAND_COUNT], BYTE  ArrayFirst[DOU_NUM][DOU_HAND_COUNT], int &resultFirst, tagOutCardResultNew &FirstOutCardResult);

	bool JiaoYanWuLong(BYTE Array[DOU_NUM][DOU_HAND_COUNT]);

	VOID shengChengSanDou(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);

	void ShiSanZhangOutCardCeLue(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE  Array[DOU_NUM][DOU_HAND_COUNT], tagOutCardTypeResultNew * CardTypeResult);

	bool checkSpecial(const BYTE * cbHandCardData, BYTE cbHandCardCount, tagOutCardTypeResultNew &NoReDoubleCardTypeResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);

	void BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData);

	//--����1�任3��2����
	bool ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	bool ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	int FindCardType(int Count, vector<tagOutCardResultNew>& vecMinTypeCardResult, vector<bool> &bOpate);
	void SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count);
	float GetAllTypePercent(BYTE cbCardData[], BYTE cbCardCount, BYTE cbDouData[], BYTE cbDouCount, int cbCardType);
	//�ж��Ƿ����ظ���� 
	bool IsReResult(tagOutCardTypeResultNew * CardTypeResult, int type, BYTE cbCardData[], BYTE cbCardCount );
	bool GetNoReResult(tagOutCardTypeResultNew * CardTypeResult, int type, tagOutCardTypeResultNew &noReCardTypeResult );
	//�ж���������
	//������13��
	BYTE GetSpecialType(const BYTE bHandCardData[], BYTE bCardCount, BYTE cbReusltData[]);
	//����˳��
	bool SetLinkCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbLineCardData[3][5]);
	//�Ƿ�˳��
	bool IsLinkCard(const BYTE cbCardData[], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE cbTemp[], BYTE bCardCount);
	//�Ƿ�ͬ��˳
	bool IsStraightDragon(const BYTE cbCardData[], BYTE bCardCount);
	//�Ƿ�ͬ��
	bool IsSameColorCard(const BYTE cbCardData[], BYTE cbCardCount);
	bool IsChouYiSe(BYTE cbCardData[], BYTE bCardCount);
	bool IsLiuDuiBan(BYTE cbCardData[], BYTE bCardCount, tagAnalyseData& AnalyseData);
	bool IsSanShunZi(BYTE * bCardData, BYTE bCardCount, BYTE cbReusltData[]);

	bool IsSanTongHua(BYTE * cbCardData, BYTE bCardCount, BYTE cbReusltData[]);
	//����ͬ��
	BYTE SearchSameColorType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbSameCount, tagSearchCardResult *pSearchCardResult);

	float GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount);

	int GetCMNSort(int m, int n);

	int CalSanDouFen(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);

	int CalCardTurnCount( BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount, int dou);

	int SearchSameCardTypeTurnCount(BYTE * allCard, BYTE allCardCardCount, BYTE * cbCardData, BYTE cbCardCount);

	int SearchTongHuaTypeTurnCount(BYTE * allCard, BYTE allCardCardCount, BYTE * cbCardData, BYTE cbCardCount);

	int SearchAllLineCardTypeTurnCount(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbLineCount, BYTE cbReferCard, int type);
	int CheckSingleCardOrder(BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);
};

//////////////////////////////////////////////////////////////////////////

#endif