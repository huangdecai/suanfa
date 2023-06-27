#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include <vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include "fastHu.h"
//���ڲ����ת����������ƿ��Դ����������ʹ�ã�����Ϊ����������������ΪMAX_INDEX. ע:����滻����������,������.
#define	INDEX_REPLACE_CARD					MAX_INDEX//33 (��Щ�ط��ǰװ���ľ���33
   
//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE						0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#define WIK_GANERAL					0x00								//��ͨ����
#define WIK_MING_GANG				0x01								//���ܣ������ٸܣ�
#define WIK_FANG_GANG				0x02								//�Ÿ�
#define WIK_AN_GANG					0x03								//����

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER				0x02								//�г�����
#define WIK_RIGHT				0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG				0x10								//��������
#define WIK_LISTEN				0x20								//��������
#define WIK_CHI_HU				0x40								//�Ժ�����
#define WIK_FANG_PAO		0x80								//����

#define WIK_YOU_JIN		0x01								//�ν�
#define WIK_ER_YOU		0x02								//����
#define WIK_SAN_YOU		0x04								//����
//#define WIK_QIANG_JIN		0x08								//����
#define WIK_SAN_JIN_DAO		0x10								//����
//////////////////////////////////////////////////////////////////////////
//���ƶ���
#define CHK_MASK_SMALL			0x000000FF							//С������
#define CHK_MASK_BIG				0xFFFFFF00							//�������


//�������

//С������
#define CHR_PING_HU                       0x0000000000000001							//ƽ��
#define CHR_PENG_PENG                     0x0000000000000002						//������	
#define CHR_QI_DUI                        0x0000000000000004							//�߶�
#define CHR_QI_DA_DUI                     0x0000000000000008							//�ߴ��
#define CHR_SHUANG_QI_DA_DUI              0x0000000000000010							//˫�ߴ��
#define CHR_SAN_QI_DA_DUI                 0x0000000000000020							//���ߴ��
#define CHR_QING_YI_SE                    0x0000000000000040							//��һɫ
#define CHR_HUN_YI_SE                     0x0000000000000080							//��һɫ
#define CHR_ZI_YI_SE                      0x0000000000000100						    //��һɫ
#define CHR_SHI_BA_LUO_HAN                0x0000000000000200						    //ʮ���޺�
#define CHR_YAO_JIU_HU                     0x0000000000000400						    //�۾ź�
#define CHR_QING_YAO_JIU                  0x0000000000000800						    //���۾�
#define CHR_SHI_SAN_YAO                   0x0000000000001000						    //ʮ����
//--��������
#define CHR_TIAN_HU                       0x0000000000100000							//���
#define CHR_DI_HU                         0x0000000000200000							//�غ�
#define CHR_HAI_DI_PAO                    0x0000000000400000						    //--������
#define CHR_HAI_DI_LAO_YUE                0x0000000000800000						   // --��������
#define CHR_QUAN_QIU_REN                  0x0000000001000000							//ȫ����
#define CHR_QIANG_GANG_HU                 0x0000000002000000							//���ܺ�
#define CHR_GANG_SHANG_HUA                0x0000000004000000							//���Ͽ���
#define CHR_GANG_SHANG_PAO				  0x0000000008000000							//������
#define CHR_MEN_QIANG_QING				  0x0000000010000000							//��ǰ��
#define CHR_QUAN_QIU_PAO				  0x0000000020000000						    //--ȫ����
#define CHR_ZI_MO						  0x0000000040000000						        ////����
//δʹ��

//
//#define CHR_SHI_SAN_LAN			    0x04000000							//ʮ����
//#define CHR_DA_QI_DUI				0x08000000							//���߶�

//////////////////////////////////////////////////////////////////////////////////

//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbValidIndex[3];					//ʵ���˿�����
	BYTE							cbMagicCount;						//��������
};




//////////////////////////////////////////////////////////////////////////


#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	Ȩλ�ࡣ
//  ע�⣬�ڲ�����λʱ���ֻ��������Ȩλ.����
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)������������޶���ġ�
//  ֻ�ܵ�������:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//��̬����
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//Ȩλ����
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//���캯��
	CChiHuRight();

	//���������
public:
	//��ֵ��
	CChiHuRight & operator = (DWORD dwRight);

	//�����
	CChiHuRight & operator &= (DWORD dwRight);
	//�����
	CChiHuRight & operator |= (DWORD dwRight);

	//��
	CChiHuRight operator & (DWORD dwRight);
	CChiHuRight operator & (DWORD dwRight) const;

	//��
	CChiHuRight operator | (DWORD dwRight);
	CChiHuRight operator | (DWORD dwRight) const;

	//���
	bool operator == (DWORD dwRight) const;
	bool operator == (const CChiHuRight chr) const;

	//�����
	bool operator != (DWORD dwRight) const;
	bool operator != (const CChiHuRight chr) const;

	//���ܺ���
public:
	//�Ƿ�ȨλΪ��
	bool IsEmpty();

	//����ȨλΪ��
	void SetEmpty();

	//��ȡȨλ��ֵ
	BYTE GetRightData(DWORD dwRight[], BYTE cbMaxCount);

	//����Ȩλ��ֵ
	bool SetRightData(const DWORD dwRight[], BYTE cbRightCount);

private:
	//���Ȩλ�Ƿ���ȷ
	bool IsValidRight(DWORD dwRight);
};

//////////////////////////////////////////////////////////////////////////////////

//����˵��


//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//�˿�����
	BYTE									m_cbMagicIndex[2];						//��������
	BOOL								m_bHuQiDui;									//�ܷ���߶�
	fastHu                              m_fastHu;
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount);
	VOID JianYiSeCardList(BYTE cbCardData[], BYTE cardIndex);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//����,������ֵ����
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//���ò���
	void SetMagicIndex( BYTE cbMagicIndex[] ); 
	//�����ж�
	bool IsMagicCard(BYTE cbCardData);
	//��ȡ�������
	BYTE GetMajicCount(const BYTE cbCardIndex[MAX_INDEX]);
	//�����ж�
	bool IsHuaCard(BYTE cbCardData);
	//�����ж�
	BYTE IsHuaCard(BYTE cbCardIndex[MAX_INDEX]);
	//�����Զ������
	VOID SetUserRule(BOOL bHuQiDui);
	VOID SetBanZiMO(BOOL bBanZiMo);
	VOID SetYouJinBiYou(BOOL bEanble);
	//�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(BYTE cbUserAction);
	//���Ƶȼ�
	WORD GetChiHuActionRank(const CChiHuRight & ChiHuRight);

	//�Զ�����
	BYTE AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount);

	//�����ж�
public:
	//�����ж�
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

public://�����ж�
	//���Ʒ���
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult, BYTE cbCurrentCard, BYTE cbCallCard);
	//�Ժ�����
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);
	//���Ʒ���
	BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, bool bNoCheckMagic=false);
	//��ȡ�������ݣ����ƺ����
	BYTE GetHuCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[]);
	//��ȡ��������,����Щ������
	BYTE GetTingData(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE& cbOutCardCount, BYTE cbOutCardData[]);
	//��ȡ�������ݣ�����Щ���������Լ��ܺ���Щ��
	BYTE GetTingDataEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][HEAP_FULL_COUNT],BYTE cbHuFan[][HEAP_FULL_COUNT]);

public://ת������
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//�˿�ת��
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//�˿�ת��
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
	
public://��������
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�˿���Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);	
	//����˿�
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//�����齫Ҫ��
	bool IsCanHuPai(int beiShu,const BYTE cbCardIndex[MAX_INDEX], bool fangPao=false);
	//�����������
	BYTE GenerateShunZi(BYTE cbCardData,bool bAdd,int space);
	//����㷨
	int ChouGuanCount(int score);
	int GetHuaCardCount(const BYTE cbCardIndex[MAX_INDEX]);

	//��
private://�ڲ�����
	bool AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree);
	//�����˿�
	bool AnalyseCard(const BYTE cbCardIndex1[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, vector<tagAnalyseItem> & AnalyseItemArray);

protected://��������

	//ƽ��
	bool IsPingHu(const tagAnalyseItem *pAnalyseItem);
	//����
	bool IsJiHu(const tagAnalyseItem *pAnalyseItem);
	//��һɫ
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//��һɫ
	bool IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//��һɫ��
	bool IsZiYiSe(tagAnalyseItem * pAnalyseItem);
	bool IsZiYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	//����Ԫ
	bool IsDaSanYuan(tagAnalyseItem * pAnalyseItem);
	//С��Ԫ
	bool IsXiaoSanYuan(const BYTE cbCardIndex[MAX_INDEX],tagAnalyseItem * pAnalyseItem);
	//����ϲ
	bool IsDaSiXi(tagAnalyseItem * pAnalyseItem);
	//С��ϲ
	bool IsXiaoSiXi(tagAnalyseItem * pAnalyseItem);
	//ʮ����
	bool IsShiSanYao( const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//������
	bool IsPengPeng(const tagAnalyseItem *pAnalyseItem);
	//�߶�ϵ��
	bool IsQiDui(const BYTE cbCardIndex[MAX_INDEX], BYTE cbWeaveCount);
	//�ߴ��
	bool IsQiDaDui(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);
	//ʮ����ϵ��
	bool IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);
	//����
	bool IsQiXing(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);
	//������
	bool IsJingDiaoJiang(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount);
	//�Ƿ�˫�ν�
	bool IsShouShuangYou(const tagAnalyseItem *pAnalyseItem, const BYTE cbCardIndex[MAX_INDEX]);
	public:
	BYTE GetCallCard(const  BYTE cardIndex[MAX_INDEX], BYTE callCard);
	void GetACardShengYuCount(BYTE cbDiscardCard[], BYTE cbDiscardCount, BYTE cbCardIndex[]);

	void CombinatoryAnalysisEx(const  BYTE cbCardIndex[MAX_INDEX], vector<tagAnalyseItem>& AnalyseItemArray, const tagWeaveItem WeaveItem[], BYTE cbWeaveICount);
	bool IsQingYaoJiu(tagAnalyseItem * pAnalyseItem);
	bool IsYaoJiu(tagAnalyseItem * pAnalyseItem);
	CChiHuRight ClearRepeateFan(CChiHuRight chrAll);
	int GetUserHuFan(CChiHuRight chr, int lianGang=0);
	int GetCommonFan(CChiHuRight chr, int lianGang = 0);

	int GetColorCount(const  BYTE cbCardIndex[MAX_INDEX],  int color);
};

//////////////////////////////////////////////////////////////////////////////////

#endif