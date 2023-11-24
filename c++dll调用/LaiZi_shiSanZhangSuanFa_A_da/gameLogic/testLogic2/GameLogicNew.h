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
//逻辑类型
#define CT_INVALID					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_ONE_DOUBLE					2								//对牌类型
#define CT_TWO_DOUBLE					3								//两对牌型
#define CT_THREE						4								//三张牌型
#define CT_FIVE_MIXED_FLUSH_NO_A		5								//普通顺子
#define CT_FIVE_MIXED_FLUSH_FIRST_A		6								//A前顺子
#define CT_FIVE_MIXED_FLUSH_BACK_A		7								//A后顺子
#define CT_FIVE_FLUSH					8								//同花
#define CT_FIVE_THREE_DEOUBLE			9								//三条一对
#define CT_FIVE_FOUR_ONE				10								//四带一张

#define CT_FIVE_STRAIGHT_FLUSH			11								//同花顺牌
#define CT_FIVE_STRAIGHT_FLUSH_FIRST_A	12								//A同花顺
#define CT_FIVE_STRAIGHT_FLUSH_BACK_A	13						        //A同花顺牌
#define CT_FIVE_TONG				14								    //火箭类型
#define CT_SIX_TONG	                    15						        //6同
#define CT_TYPE_COUNT				16									//最大类型

#define CT_MISSILE_CARD				20								    //火箭类型

//特殊类型
#define CT_EX_INVALID					14								//非特殊牌
#define CT_EX_SANTONGHUA				15								//三同花
#define CT_EX_SANSHUNZI					16								//三顺子
#define CT_EX_LIUDUIBAN					17								//六对半
#define CT_EX_WUDUISANTIAO				18								//五对三条
#define CT_EX_SITAOSANTIAO				19								//四套三条
#define CT_EX_SHUANGGUAICHONGSAN		20								//双怪冲三
#define CT_EX_COUYISE					21								//凑一色
#define CT_EX_QUANXIAO					22								//全小
#define CT_EX_QUANDA					23								//全大
#define CT_EX_SANFENGTIANXIA			24								//三分天下
#define CT_EX_SANTONGHUASHUN			25								//三同花顺
#define CT_EX_SHIERHUANGZU				26								//十二皇族
#define CT_EX_YITIAOLONG				27								//一条龙
#define CT_EX_ZHIZUNQINGLONG			28								//至尊清龙
#define	CT_EX_QUANHEIYIDIANHONG			29								//全黑一点红
#define	CT_EX_QUANHONGYIDIANHEI			30								//全红一点黑

const int CARD_TYPE_SCORE[CT_TYPE_COUNT] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 110, 140, 150 };
#define  G_THREE_TAKE_TWO_DAN true
#define  G_THREE_TAKE_TWO_EX false

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_CUSTOM					2									//自定排序
#define ST_ASCENDING				3									//小到大排序
#define TOU_SHI                     false                               //机器人是否透视别人的牌

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResultNew
{
	BYTE 							cbBlockCount[8];					//扑克数目
	BYTE							cbCardData[8][MAX_COUNT];			//扑克数据
	BYTE							cbKingCount;
};
//出牌结果
struct tagInPyhonNew
{
	BYTE							cbHandCardData[MAX_COUNT];
	BYTE							cbHandCardCount;
	BYTE							cbTurnCardData[MAX_COUNT];
	BYTE							cbTurnCardCount;
	BYTE							DiscardCard[54];
	BYTE							cbDiscardCardCount;
	BYTE							cbRangCardCount;	//扑克类型
	BYTE							cbOthreRangCardCount;	//扑克类型
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};
//出牌结果
struct tagOutCardResultNew
{
	BYTE							cbCardType;						   //扑克类型
	float							    cbCardScore;					   //类型分值
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//分布信息
struct tagDistributingNew
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbDistributing[15][6];				//分布信息
};

#define MAX_TYPE_COUNT 300
struct tagOutCardTypeResultNew 
{
	BYTE							cbCardType;							//扑克类型
	int							cbCardTypeCount;					//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//每手个数
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//扑克数据
};


struct tagCardIndex
{
	BYTE		cbIndex;						//扑克索引
	BYTE		cbCardData;			//扑克数据
};
//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogicNew
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据

	//AI变量
public:
	WORD							m_wBankerUser;	
	WORD                            m_wMeChairID = 0;
	BYTE                            m_cbDiscardCardCount = 0;
	BYTE                            m_cbOthreRangCardCount = 0;
	bool                            m_bSanDaYi = false;
	bool                            m_bHavePass = false;

	//函数定义
public:
	//构造函数
	CGameLogicNew();
	//析构函数
	virtual ~CGameLogicNew();
	//类型函数
public:
	//获取类型
	//获取类型
	BYTE GetCardType(BYTE bCardData[],  BYTE bCardCount, BYTE & bMaxCardData);
	//分析扑克
	void AnalyseCard(const BYTE bCardDataList[], const BYTE bCardCount, tagAnalyseData& AnalyseData);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//获取某个牌有多少个
	int GetACardCount(const BYTE cbCardBuffer[], const BYTE cbBufferCount, BYTE cbCard);
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//移除王的计数
	int RemoveCardWang(BYTE bCardData[], BYTE bCardCount, BYTE bWangResultCardData[]);
	//清理重复逻辑值的数组
	int ClearReLogicValue(  BYTE cbCardData[], BYTE cbCardCount);
	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetMaxCardLogic(BYTE bCardData[], BYTE bCardCount, bool bEnable);
	//对比扑克
	//比较扑克
	int CompareCard(BYTE bInFirstList[], BYTE bInNextList[], BYTE bFirstCount, BYTE bNextCount, bool bCompareLogic, bool bCompareHuaSe);

	//出牌搜索
	BYTE SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb=false);

	//内部函数
public:
	//构造扑克
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResultNew & AnalyseResult);
	//分析分布
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributingNew & Distributing);
	int GetTakeWangCount(BYTE bCardData[], BYTE bCardCount);
	BYTE GetTakeWangCardType(BYTE bCardData[], BYTE bCardCount, BYTE & bMaxCardData, tagAnalyseData &AnalyseData);
	bool IsContainA(BYTE cbCardData[], BYTE cbCardCount);
	//////////////////////////////////////////////////////////////////////////
public:
	//设置扑克
	VOID SetUserCard(WORD wChairID, const BYTE cbCardData[], BYTE cbCardCount);
	//设置底牌
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//是否存在除我的牌和牌面牌的炸弹
	void SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount);
	void ReSetDiscardCard();
	//设置庄家
	VOID SetBanker(WORD wBanker) ;
	//叫牌扑克
	VOID SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//删除扑克
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;
	//连牌搜索
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	BYTE SearchAllLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult);
	//辅助函数
	void GetNextLineCard(BYTE cbIndexSatart, BYTE cbIndexEnd, BYTE cbLineCount, int needLaiZiCount, BYTE cbWangCount, BYTE cbWangCardData[MAX_COUNT], vector<BYTE> & lineArray, tagDistributingNew& Distributing, tagSearchCardResult *pSearchCardResult);
	int GetCardMaxHuaSe(BYTE lineArray[], int lineArrayCount, int &color);
public:
	//组合算法
	VOID Combination(BYTE cbCombineCardData[], int cbResComLen,  BYTE cbResultCardData[500][8], int &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	//分析炸弹
	VOID GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//分析顺子
	VOID GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount, tagOutCardTypeResultNew   *CardTypeResult=NULL);
	//分析三条
	VOID GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//分析对子
	VOID GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	//分析单牌
	VOID GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, tagOutCardTypeResultNew   *CardTypeResult = NULL);
	
	//主要函数
public:
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
public:

	//搜索其他人手上是否有相同牌
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
	//找出扑克组合成最小的手数
	int FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, vector<vector<tagOutCardResultNew>>   &CardResultShao, float &MinTypeScore, vector<float> &MinTypeScoreShao, bool &bBiYing, bool bSearchBiYing = true);
	//清除单牌的重复结果
	void clearSingleReResult(tagOutCardTypeResultNew * CardTypeResult);
	//填充组合
	void FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, BYTE * cbTmpHReminCardData, BYTE const cbHandCardCount, vector<vector<tagOutCardResultNew> > &resultAllVec);
	//
	//排列扑克
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);
	//同牌搜索
	BYTE SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
		tagSearchCardResult *pSearchCardResult);
	//带牌类型搜索(三带一，四带一等)
	BYTE SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount,
		tagSearchCardResult *pSearchCardResult);
	//搜索飞机
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult);
	//检测台面炸弹概率
	bool CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount);
	//检测两个数组是否相交
	bool CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount);
	//检测单双牌或者三带中是否是最小的牌
	void OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew>& vecMinTypeCardResult);
public:
	//必赢分析
	bool isBiYing(vector<tagOutCardResultNew > &  CardTypeResult);
	//通用单双压牌策略
	void CommonSDTurnCardSearch(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbTurnCardLogic, tagOutCardResultNew &OutCardResult);
	//无敌检测
	bool  WuDiCheck(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE * cbTurnCardData, BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult);
	//检测剩下牌是否必赢
	bool CheckOutOneTypeWillWin(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult);
	//出牌剩余分牌推算
	bool OutCardShengYuFenCheck(BYTE cbHandCardCount, const BYTE * cbHandCardData, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, float MinTypeScoreVec, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);
	//判断是否是最大类型带上拖一类型
	bool FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);
		
public:
	//
	//叫分判断
	int LandScore(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	//叫分判断
	int LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	float calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult);
	//出牌搜索
	bool SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, tagOutCardResultNew & OutCardResult);
	VOID SearchOutCardShiSanZhang(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	VOID SearchOutCardShiSanZhangTurn(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult, tagOutCardResultNew & TurnOutCardResult, tagOutCardResultNew & FirstOutCardResult);

	int TrunCheck(BYTE  ArrayTurn[DOU_NUM][DOU_HAND_COUNT], BYTE  ArrayFirst[DOU_NUM][DOU_HAND_COUNT], int &resultFirst, tagOutCardResultNew &FirstOutCardResult);

	bool JiaoYanWuLong(BYTE Array[DOU_NUM][DOU_HAND_COUNT]);

	VOID shengChengSanDou(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);

	void ShiSanZhangOutCardCeLue(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE  Array[DOU_NUM][DOU_HAND_COUNT], tagOutCardTypeResultNew * CardTypeResult);

	bool checkSpecial(const BYTE * cbHandCardData, BYTE cbHandCardCount, tagOutCardTypeResultNew &NoReDoubleCardTypeResult, BYTE  Array[DOU_NUM][DOU_HAND_COUNT]);

	void BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData);

	//--三拖1变换3带2来拖
	bool ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	bool ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	int FindCardType(int Count, vector<tagOutCardResultNew>& vecMinTypeCardResult, vector<bool> &bOpate);
	void SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count);
	float GetAllTypePercent(BYTE cbCardData[], BYTE cbCardCount, BYTE cbDouData[], BYTE cbDouCount, int cbCardType);
	//判断是否是重复结果 
	bool IsReResult(tagOutCardTypeResultNew * CardTypeResult, int type, BYTE cbCardData[], BYTE cbCardCount );
	bool GetNoReResult(tagOutCardTypeResultNew * CardTypeResult, int type, tagOutCardTypeResultNew &noReCardTypeResult );
	//判断特殊牌型
	//必须是13张
	BYTE GetSpecialType(const BYTE bHandCardData[], BYTE bCardCount, BYTE cbReusltData[]);
	//设置顺子
	bool SetLinkCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbLineCardData[3][5]);
	//是否顺子
	bool IsLinkCard(const BYTE cbCardData[], BYTE cbCardCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE cbTemp[], BYTE bCardCount);
	//是否同花顺
	bool IsStraightDragon(const BYTE cbCardData[], BYTE bCardCount);
	//是否同花
	bool IsSameColorCard(const BYTE cbCardData[], BYTE cbCardCount);
	bool IsChouYiSe(BYTE cbCardData[], BYTE bCardCount);
	bool IsLiuDuiBan(BYTE cbCardData[], BYTE bCardCount, tagAnalyseData& AnalyseData);
	bool IsSanShunZi(BYTE * bCardData, BYTE bCardCount, BYTE cbReusltData[]);

	bool IsSanTongHua(BYTE * cbCardData, BYTE bCardCount, BYTE cbReusltData[]);
	//搜索同花
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