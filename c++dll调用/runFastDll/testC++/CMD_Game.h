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
#define FULL_COUNT					48									//全牌数目
#define MAX_RESULT_COUNT					500									//全牌数目

//逻辑数目
#define NORMAL_COUNT				16									//常规数目
#define DISPATCH_COUNT				51									//派发数目
#define GOOD_CARD_COUTN				38									//好牌数目
#define MING_PAI                    1                                   //是否明牌,0是不,1是明
//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码
#define  G_THREE_TAKE_TWO_DAN true
#define  G_THREE_TAKE_TWO_EX true
//逻辑类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_FOUR_TAKE_THREE			11									//四带两对
#define CT_BOMB_CARD				12									//炸弹类型
#define CT_MISSILE_CARD				13								//火箭类型
#define CT_TYPE_COUNT				14									//最大类型
const int CARD_TYPE_SCORE[CT_TYPE_COUNT] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 110, 140, 150 };
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
	BYTE							cbSearchCount;						//结果数目
	BYTE							cbCardCount[MAX_RESULT_COUNT];				//扑克数目
	BYTE							cbResultCard[MAX_RESULT_COUNT][MAX_COUNT];	//结果扑克
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif