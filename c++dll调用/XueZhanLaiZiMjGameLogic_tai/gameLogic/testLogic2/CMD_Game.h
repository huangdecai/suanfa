#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE
typedef	unsigned	__int64		uint64;
///////////////////////���ܿ���
#define G_SHOW_TING_DATA	  TRUE		//���Ƹ�����Ϣ

#define G_SHOW_HU_DATA		 TRUE			//������ʾ

#define G_CAN_CHI_CARD 		 TRUE			//����

#define G_CAN_TING_CARD		 FALSE			//����

#define G_QIANG_GANG_HU		 TRUE			//���ܺ�

#define G_GANG_SHANG_PAO	 FALSE			//������

#define G_CHI_HU						 TRUE			//�Ժ�

#define G_ZI_SHUN					FALSE			//���Ƴ�˳ ���ƣ�������������������˳ ���ƣ��з��׳�˳

#define G_GEN_ZHUANG			TRUE			//��ׯ

#define G_ONLY_ONE_HU			TRUE			//ֻ��һ���˺�

#define G_HUANG_ZHUANG_CARD_NUM			17			//��ׯ����
//////////////////////////////////////////////////////////////////////////

#define KIND_ID						302									//��Ϸ I D
#define GAME_NAME					TEXT("Ѫս�齫")					//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define MAX_HUA_INDEX			41									//��������
#define MAX_HUA_COUNT			8									//���Ƹ���
#define MAX_MA_COUNT			8									//���ץ����
#define ZI_PAI_COUNT	7								//���Ƹ���

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					42									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				136									//�����

#define MAX_MAGIC_COUNT            2
//�˿˶���
#define HEAP_FULL_COUNT				26									//����ȫ��

#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����	
#define MAX_CHANGE_CARDCOUNT		3									//--������������
//--�Ծ���ˮ�嵥
#define MAX_CHART_COUNT             40                                 //
#define MAX_RECORD_COUNT            32                                 //
//--������
#define CLOCKWISE_CHANGE			0									//--˳ʱ�뻻��
#define ANTI_CLOCKWISE_CHANGE		1									//--��ʱ�뻻��
#define OPPOSITE_CHANGE				2									//--�Լһ���
#define MAX_CHANGE_TYPE				3									//--�����ŷ�ʽ����
#define MAX_CHANGE_CARDCOUNT		3									//--������������
//ȱ�Ŷ���
#define M_WAN						0x00
#define M_SUO						0x10
#define M_TONG						0x20
//////////////////////////////////////////////////////////////////////////
//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
#define ASSERT(f)    ((void)0)
#define VERIFY(f)          ((void)(f))
//�������
//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbCardData[4];								//�齫����
};

//���ƽ��
struct tagGangCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[4];						//�˿�����
	int                             GangKind;
};


//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	bool                          bMagicEye;                          //�����Ƿ�������
	BYTE							cbWeaveKind[MAX_WEAVE];				//�������
	BYTE						    cbCenterCard[MAX_WEAVE];			//�����˿�
	BYTE                        cbCardData[MAX_WEAVE][4];           //ʵ���˿�
};
//���Ƹ�����Ϣ
struct CMD_S_TING_DATA
{
	//���ļ�������
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
	//�����ܺ��ļ�����
	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][HEAP_FULL_COUNT];
	//����ʣ����
	BYTE							cbHuCardRemainingCount[MAX_COUNT][HEAP_FULL_COUNT];
	//���Ʒ���
	BYTE							cbHuFan[MAX_COUNT][HEAP_FULL_COUNT];
};
//���Ƹ�����Ϣ
struct CMD_S_ER_XING_TING_DATA
{
	//���ļ�������
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[200];
	//�����ܺ��ļ�����
	BYTE							cbHuCardCount[200];
	BYTE							cbHuCardData[200][HEAP_FULL_COUNT];
	//����ʣ����
	BYTE							cbHuCardRemainingCount[200][HEAP_FULL_COUNT];
	//���Ʒ���
	BYTE							cbHuFan[200][HEAP_FULL_COUNT];
};
//////////////////////////////////////////////////////////////////////////
//���ƽ��
struct tagInPyhonNew
{
	BYTE							cbHandCardData[20];
	BYTE							cbHandCardCount;
	BYTE							DiscardCard[144];
	BYTE                            cbDiscardCardCount;
	BYTE							cbWeaveKind[GAME_PLAYER*MAX_WEAVE];						//�������
	BYTE							cbCenterCard[GAME_PLAYER*MAX_WEAVE];						//�����˿�
	BYTE							cbPublicCard[GAME_PLAYER*MAX_WEAVE];						//������־
	BYTE							wProvideUser[GAME_PLAYER*MAX_WEAVE];						//��Ӧ�û�
	BYTE                            cbWeaveCount[GAME_PLAYER];
	BYTE                            cbCardDataEx[20];
	BYTE                            wMeChairId;
	BYTE                            wCurrentUser;
	BYTE                            cbActionMask;
	BYTE                            cbActionCard;
	BYTE							cbOperateCode;						//�˿���Ŀ
	BYTE							cbResultCard[20];			//����˿�
};
//��ȱ���
struct tagInPyhonCallCard
{
	BYTE							cbHandCardData[20];
	BYTE							cbHandCardCount;
	BYTE							cbType;
};
enum CHARTTYPE
{
	    INVALID_CHARTTYPE = 0,
		GUAFENG_TYPE = 22,
		BEIGUAFENG_TYPE = 23,
		XIAYU_TYPE = 24,
		BEIXIAYU_TYPE = 25,
		DIANPAO_TYPE = 26,
		BEIDIANPAO_TYPE = 27,
		ZIMO_TYPE = 28,
		BEIZIMO_TYPE = 29,
		HUJIAOZHUANYI_TYPE = 30,
		TUISHUI_TYPE = 31,
		CHAHUAZHU_TYPE = 32,
		CHADAJIAO_TYPE = 33,
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbOperateCode;						//���ƴ���
	BYTE							cbOperateCard;						//����ֵ
};

//���Ʒ������
struct tagTingCardResult
{
	BYTE bAbandonCount;													//�ɶ�������
	BYTE bAbandonCard[MAX_COUNT];										//�ɶ�����
	BYTE bTingCardCount[MAX_COUNT];										//������Ŀ
	BYTE bTingCard[MAX_COUNT][MAX_COUNT - 1];								//����
	BYTE bRemainCount[MAX_COUNT];										//�����Ƶ�ʣ����
};

//////////////////////////////////////////////////////////////////////////

#endif
