#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						200									//��Ϸ I D
#define GAME_NAME					TEXT("������")						//��Ϸ����

//�������
#define GAME_PLAYER					3									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////
//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
//��Ŀ����
#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ
#define MAX_RESULT_COUNT					500									//ȫ����Ŀ
#define DOU_NUM                         3               //����
#define DOU_HAND_COUNT                  5               //ÿ�����������

//�߼���Ŀ
#define NORMAL_COUNT				13									//������Ŀ
#define DISPATCH_COUNT				54									//�ɷ���Ŀ
#define GOOD_CARD_COUTN				38									//������Ŀ
#define MING_PAI                    1                                   //�Ƿ�����,0�ǲ�,1����
//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����
#define  G_THREE_TAKE_TWO_DAN true
#define  G_THREE_TAKE_TWO_EX false
//�߼�����
#define CT_INVALID					0									//��������
#define CT_SINGLE					1									//��������
#define CT_ONE_DOUBLE					2									//��������
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
//////////////////////////////////////////////////////////////////////////////////
//״̬����
//״̬����
#define ASSERT(f)    ((void)0)
#define VERIFY(f)          ((void)(f))
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//�з�״̬
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//��Ϸ����



//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_SCORE			101									//�û��з�
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//�û�����
#define SUB_S_PASS_CARD				104									//�û�����
#define SUB_S_GAME_CONCLUDE			105									//��Ϸ����
#define SUB_S_SET_BASESCORE			106									//���û���
#define SUB_S_CHEAT_CARD			107									//�����˿�
#define SUB_S_TRUSTEE				108									//�й�


//�û�����
struct CMD_S_OutCard
{
	WORD							wOutCardUser;
	int                            msgID;
	BYTE							cbCardCount;						//������Ŀ						//�������
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_CALL_SCORE			1									//�û��з�
#define SUB_C_OUT_CARD				2									//�û�����
#define SUB_C_PASS_CARD				3									//�û�����
#define SUB_C_TRUSTEE				4									//�û��й�


//�û�����
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿�����
};

//�������
struct tagSearchCardResult
{
	int							cbSearchCount;						//�����Ŀ
	BYTE							cbCardCount[MAX_RESULT_COUNT];				//�˿���Ŀ
	BYTE							cbResultCard[MAX_RESULT_COUNT][MAX_COUNT];	//����˿�
};
//�����ṹ
struct tagAnalyseData
{
	BYTE							bOneCount;								//������Ŀ
	BYTE							bTwoCount;								//������Ŀ 
	BYTE							bThreeCount;							//������Ŀ
	BYTE							bFourCount;								//������Ŀ
	BYTE							bOneFirst[MAX_COUNT];							//����λ��
	BYTE							bTwoFirst[MAX_COUNT];							//����λ��
	BYTE							bThreeFirst[MAX_COUNT];						//����λ��
	BYTE							bFourFirst[MAX_COUNT];							//����λ��
	bool							bSameColor;								//�Ƿ�ͬ��
	BYTE							cbKingCount;
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif