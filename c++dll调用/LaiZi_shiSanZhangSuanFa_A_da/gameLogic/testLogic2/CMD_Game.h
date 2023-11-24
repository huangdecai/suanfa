#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						200									//游戏 I D
#define GAME_NAME					TEXT("斗地主")						//游戏名字

//组件属性
#define GAME_PLAYER					3									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
//数目定义
#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目
#define MAX_RESULT_COUNT					500									//全牌数目
#define DOU_NUM                         3               //道数
#define DOU_HAND_COUNT                  5               //每道最大手牌数

//逻辑数目
#define NORMAL_COUNT				13									//常规数目
#define DISPATCH_COUNT				54									//派发数目
#define GOOD_CARD_COUTN				38									//好牌数目
#define MING_PAI                    1                                   //是否明牌,0是不,1是明
//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码
#define  G_THREE_TAKE_TWO_DAN true
#define  G_THREE_TAKE_TWO_EX false
//逻辑类型
#define CT_INVALID					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_ONE_DOUBLE					2									//对牌类型
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
//////////////////////////////////////////////////////////////////////////////////
//状态定义
//状态定义
#define ASSERT(f)    ((void)0)
#define VERIFY(f)          ((void)(f))
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//叫分状态
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//游戏进行



//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_CALL_SCORE			101									//用户叫分
#define SUB_S_BANKER_INFO			102									//庄家信息
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//用户放弃
#define SUB_S_GAME_CONCLUDE			105									//游戏结束
#define SUB_S_SET_BASESCORE			106									//设置基数
#define SUB_S_CHEAT_CARD			107									//作弊扑克
#define SUB_S_TRUSTEE				108									//托管


//用户出牌
struct CMD_S_OutCard
{
	WORD							wOutCardUser;
	int                            msgID;
	BYTE							cbCardCount;						//出牌数目						//出牌玩家
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_CALL_SCORE			1									//用户叫分
#define SUB_C_OUT_CARD				2									//用户出牌
#define SUB_C_PASS_CARD				3									//用户放弃
#define SUB_C_TRUSTEE				4									//用户托管


//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[MAX_COUNT];				//扑克数据
};

//搜索结果
struct tagSearchCardResult
{
	int							cbSearchCount;						//结果数目
	BYTE							cbCardCount[MAX_RESULT_COUNT];				//扑克数目
	BYTE							cbResultCard[MAX_RESULT_COUNT][MAX_COUNT];	//结果扑克
};
//分析结构
struct tagAnalyseData
{
	BYTE							bOneCount;								//单张数目
	BYTE							bTwoCount;								//两张数目 
	BYTE							bThreeCount;							//三张数目
	BYTE							bFourCount;								//四张数目
	BYTE							bOneFirst[MAX_COUNT];							//单牌位置
	BYTE							bTwoFirst[MAX_COUNT];							//对牌位置
	BYTE							bThreeFirst[MAX_COUNT];						//三条位置
	BYTE							bFourFirst[MAX_COUNT];							//四张位置
	bool							bSameColor;								//是否同花
	BYTE							cbKingCount;
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif