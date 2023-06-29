#ifndef GAME_LOGIC_EW_HEAD_FILE
#define GAME_LOGIC_EW_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>
#include <algorithm>

//#include"CMD_Game.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_CUSTOM					2									//自定排序
#define ST_ASCENDING				3									//小到大排序
#define TOU_SHI                     false                               //机器人是否透视别人的牌
#define JUE_DUI_DA                 2
#define XIANG_DUI_DA                 1
//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResultNew
{
	BYTE 							cbBlockCount[4];					//扑克数目
	BYTE							cbCardData[4][FULL_COUNT];			//扑克数据
};
//出牌结果
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
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//出牌结果
struct tagInPyhonCardType
{
	BYTE							cbHandCardData[MAX_COUNT];
	BYTE							cbHandCardCount;
	BYTE							cbType;
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

#define MAX_TYPE_COUNT 256
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
	BYTE                            m_cbMaxCard[MAX_COUNT];
	BYTE                            m_cbCardDataEx[MAX_COUNT];
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//所有扑克
	BYTE							m_cbUserCardCount[GAME_PLAYER];		//扑克数目
	BYTE							m_cbDiscardCard[FULL_COUNT];//废弃牌
	BYTE                            m_cbOtherDiscardCard[MAX_COUNT];
	WORD							m_wBankerUser=0;	
	WORD                            m_wMeChairID = 0;
	BYTE                            m_cbDiscardCardCount = 0;
	BYTE                            m_cbOtherDiscardCount = 0;
	BYTE                            m_cbFirstCard = 0;
	BYTE                            m_cbCardTypeCount = 0;//是16张还是15张
	bool                            m_bHavePass = false;
	bool                            m_bShenBao = false;
	bool                            m_bTrunMode = false;

	//函数定义
public:
	//构造函数
	CGameLogicNew();
	//析构函数
	virtual ~CGameLogicNew();
	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
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
	//清理重复逻辑值的数组
	int ClearReLogicValue(  BYTE cbCardData[], BYTE cbCardCount);
	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
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

	//////////////////////////////////////////////////////////////////////////
public:
	//设置扑克
	VOID SetUserCard(WORD wChairID, const BYTE cbCardData[], BYTE cbCardCount);
	//设置底牌
	VOID SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//是否存在除我的牌和牌面牌的炸弹
	void SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbOtherCardData[], BYTE cbOtherCardCount);
	void ReSetData();
	//设置庄家
	VOID SetBanker(WORD wBanker) ;
	//叫牌扑克
	VOID SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//删除扑克
	VOID RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;
	//连牌搜索
	BYTE SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult);
	//辅助函数
public:
	//组合算法
	VOID Combination(BYTE cbCombineCardData[], int cbResComLen, BYTE cbResultCardData[MAX_RESULT_COUNT][MAX_COLS], int &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	bool IsReCombination(BYTE cbResultCardData[][MAX_COLS], BYTE cbCardData[MAX_COLS], int cbResCardLen, int cbNeedCardCount);
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
	//地主出牌（先出牌）
	VOID BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	//地主出牌（后出牌）
	VOID BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);

	//地主上家（先出牌）
	VOID UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
	//地主上家（后出牌）
	VOID UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);
	//地主下家（先出牌）
	VOID UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult);
	//地主下家（后出牌）
	VOID UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);
public:

	//判断是否牌面最大
	bool SearchOtherHandCardThan(const BYTE cbHandCardData[], BYTE cbHandCardCount, bool bNoSearchBomb);
	//搜索其他人手上是否有相同牌
	bool SearchOtherHandCardSame(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType);
	//获取相对或者绝对大牌
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
	//找出扑克组合成最小的手数
	int FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, float &MinTypeScore, bool &bBiYing, bool bSearchBiYing = true);
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
	BYTE SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbLineCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult);
	//检测台面炸弹概率
	bool CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE	DiscardCard[], BYTE cbDiscardCardCount);
	//检测两个数组是否相交
	bool CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount);
	//检测单双牌或者三带中是否是最小的牌
	void OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> vecMinTypeCardResult);
public:
	//出牌策略算法
	//优先打能收回来的牌
	int YouXianDaNengShouHuiCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>&  vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu);
	//压出一手牌后是否进入必胜模式
	int IsBiShengTurnCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu);
	
	int DanShuangNengShouHuiLaiCheck(vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);

	//优先出多牌
	int SearchMutilType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>&  vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);
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

	bool DanDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing);
	bool ShuangDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing);
	//对牌拆分检测
	bool DoubleChaiFenCheck(vector<tagOutCardResultNew> &TempMinTypeCardResult, const BYTE * cbHandCardData, BYTE cbHandCardCount, WORD wOutCardUser);
	//农民还是地主判断
	bool NoOutCardUserHandCardCountCheck(WORD wOutCardUser, BYTE cbCardCount,bool bXiaoYu=true,bool bYu=true,bool bOnlyDengYu=false);
	//从三带一或者三带二中找出最小的对
	vector<BYTE> SearchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult);
	//从三带一或者三带二中交换最小的带牌
	bool SwitchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult);
	//判断是否是最大类型带上拖一类型
	bool FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);

	//四拖一或者四拖二分拆带牌
	void FourTakeOneOrTwoFenChaiCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult);
	//出牌出倒数第二大检测
	void OutCardDaoShuDiErDaCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult);
	//--单双出最小的牌
	void OutSingleOrDoubleMinCard(const BYTE * cbHandCardData, BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult, int type);
	//判断是否是首出
	bool FindFirstType(const BYTE cbHandCardData[], BYTE cbHandCardCount,  vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult);
public:
	//
	//叫分判断
	//叫分判断
	int LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore);
	float calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult);
	//出牌搜索
	bool SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount, BYTE	cbOtherDiscardCard[], BYTE	cbOtherDiscardCount, BYTE cbCardDataEx[], BYTE MaxCard[], tagOutCardResultNew & OutCardResult);
	//地主出牌（先出牌）
	VOID ErZhuDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult);
	//地主出牌（后出牌）
	VOID ErBeiDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult);

	void BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData);

	//--三拖1变换3带2来拖
	bool ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	bool ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult);

	void SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count);

	float GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount);
	/*--概率中需要用到排列组合的表达式
		-- 注意(m > n)
		--C(m, n) = (m*(m - 1)*(m - 2)…(m - n + 1)) / (n*(n - 1)*… * 1)*/
	int GetCMNSort(int m, int n);

	bool isAllDoubleType(BYTE * cbComResCard, BYTE CardCount);

};

//////////////////////////////////////////////////////////////////////////

#endif