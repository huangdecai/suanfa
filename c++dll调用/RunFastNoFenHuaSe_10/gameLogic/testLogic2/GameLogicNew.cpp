#include "StdAfx.h"
#include "GameLogicNew.h"

#include<thread>
#include<numeric>
#include<functional>
//////////////////////////////////////////////////////////////////////////
//��̬����
//#define ASSERT(f)    ((void)0)
//#define VERIFY(f)          ((void)(f))
////��������
const BYTE cbIndexCount=5;

//�˿�����
const BYTE	CGameLogicNew::m_cbCardData[FULL_COUNT]=
{
	//0x01,     0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	//0x11,     0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	//0x21,     0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	//     0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x01, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x01, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//÷�� A - K
	0x01, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x01, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	//0x4E,0x4F,
};


//////////////////////////////////////////////////////////////////////////

void CGameLogicNew::SetDiscardCard(BYTE cbCardData[], BYTE cbCardCount)
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	CopyMemory(m_cbDiscardCard , cbCardData, cbCardCount);
	m_cbDiscardCardCount = cbCardCount;
}

void CGameLogicNew::ReSetData()
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbMaxCard, sizeof(m_cbMaxCard));
	m_cbDiscardCardCount = 0;
	m_cbOthreRangCardCount = 0;
	m_cbFirstCard = 0;
	m_bHavePass = false;
}

//���캯��
CGameLogicNew::CGameLogicNew()
{
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));
	ZeroMemory(m_cbUserCardCount, sizeof(m_cbUserCardCount));
	
}

//��������
CGameLogicNew::~CGameLogicNew()
{
}



//��ȡ����
BYTE CGameLogicNew::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//�����˿�
	BYTE cbTempCardData[FULL_COUNT] = { 0 };
	CopyMemory(cbTempCardData, cbCardData, sizeof(BYTE)*cbCardCount);
	//��С����
	SortCardList(cbTempCardData, cbCardCount, ST_ORDER);
	//������
	switch (cbCardCount)
	{
	case 0:	//����
		{
			return CT_ERROR;
		}
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//���ƻ��
		{
			//�����ж�
		if ((cbTempCardData[0] == 0x4F) && (cbTempCardData[1] == 0x4E)) return CT_MISSILE_CARD;
		if (GetCardLogicValue(cbTempCardData[0]) == GetCardLogicValue(cbTempCardData[1])) return CT_DOUBLE;
			
			return CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResultNew AnalyseResult;
	AnalysebCardData(cbTempCardData, cbCardCount, AnalyseResult);

	//�����ж�
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//�����ж�
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		/*if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 7)) return CT_FOUR_TAKE_THREE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;*/
		if ((AnalyseResult.cbBlockCount[3] == 1) && (cbCardCount == 5)) return CT_THREE_TAKE_TWO;
		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//�����ж�
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//��������
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue>=15) return CT_ERROR;
			if (G_THREE_TAKE_TWO_EX)
			{
				 //�����ж�
				tagSearchCardResult tmpSearchCardResult = {};
				SearchLineCardType(cbTempCardData, cbCardCount, 0, 3, (cbCardCount / 5), &tmpSearchCardResult);
				if ((tmpSearchCardResult.cbCardCount[0] * 5 / 3) == cbCardCount )
					return CT_THREE_TAKE_TWO;
			}
				
			//�����ж�
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//�����ж�
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;
		if (G_THREE_TAKE_TWO_DAN) 
		{
			if
				(((AnalyseResult.cbBlockCount[2] * 5) == cbCardCount &&
				((AnalyseResult.cbBlockCount[0]) / 2 + AnalyseResult.cbBlockCount[1]) ==
				AnalyseResult.cbBlockCount[2]))
				{
					return CT_THREE_TAKE_TWO;

			}
		}
			
		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.cbBlockCount[1]>=2)
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.cbBlockCount[0]>=3)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//�����˿�
VOID CGameLogicNew::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			bool bEnable = false;
			if (cbSortType == ST_ORDER)
			{
				bEnable = cbSortValue[i]<cbSortValue[i + 1];
			}
			else
			{
				bEnable = cbSortValue[i]>cbSortValue[i + 1];
			}
			if ((bEnable) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//����Ȩλ
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//��Ŀ����
	if (cbSortType==ST_COUNT)
	{
		//��������
		BYTE cbCardIndex=0;

		//�����˿�
		tagAnalyseResultNew AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//��ȡ�˿�
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//�����˿�
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//��������
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

	return;
}

//�����˿�
VOID CGameLogicNew::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

int CGameLogicNew::GetACardCount(const BYTE cbCardBuffer[], const BYTE cbBufferCount, BYTE cbCard)
{
	int count = 0;
	for (int i = 0; i < cbBufferCount; i++)
	{
		if (GetCardLogicValue(cbCardBuffer[i]) == GetCardLogicValue(cbCard))
		{
			count++;
		}
	}
	return count;
}

//ɾ���˿�
bool CGameLogicNew::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount = 0, cbTempCardData[FULL_COUNT] = { 0 };
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}
	for (int i = cbCardPos; i < cbCardCount; i++)
	{
		cbCardData[cbCardPos++] = 0;
	}

	return true;
}

int CGameLogicNew::ClearReLogicValue(BYTE cbCardData[], BYTE cbCardCount)
{
	SortCardList(cbCardData, cbCardCount, ST_ORDER);
	BYTE tmpCardData[FULL_COUNT] = { 0 };
	int  count = 0;
	tmpCardData[count++] = cbCardData[0];
	for (int i = 1; i < cbCardCount;i++)
	{
		if (GetCardLogicValue(cbCardData[i]) != GetCardLogicValue(tmpCardData[count-1]))
		{
			tmpCardData[count++] = cbCardData[i];
		}
	}
	for (int i = count; i < cbCardCount; i++)
	{
		cbCardData[i] = 0;
	}
	CopyMemory(cbCardData, tmpCardData, count);
	return count;
}

//��Ч�ж�
bool CGameLogicNew::IsValidCard(BYTE cbCardData)
{
	//��ȡ����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//��Ч�ж�
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//�߼���ֵ
BYTE CGameLogicNew::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//�Ա��˿�
bool CGameLogicNew::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//��ȡ����
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//�����ж�
	if (cbNextType==CT_ERROR) return false;
	if (cbNextType==CT_MISSILE_CARD) return true;

	//ը���ж�
	if ((cbFirstType!=CT_BOMB_CARD)&&(cbNextType==CT_BOMB_CARD)) return true;
	if ((cbFirstType==CT_BOMB_CARD)&&(cbNextType!=CT_BOMB_CARD)) return false;

	//�����ж�
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) return false;

	//��ʼ�Ա�
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResultNew NextResult;
			tagAnalyseResultNew FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	//case CT_FOUR_TAKE_ONE:
	//case CT_FOUR_TAKE_TWO:
	case CT_FOUR_TAKE_THREE:
		{
			//�����˿�
			tagAnalyseResultNew NextResult;
			tagAnalyseResultNew FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[3][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[3][0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}
	
	return false;
}

//��������
BYTE CGameLogicNew::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagSearchCardResult *pSearchCardResult, bool bNoSearchBomb)
{
	//���ý��
	ASSERT(pSearchCardResult != NULL);
	if (pSearchCardResult == NULL) return 0;

	ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//��������
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//�����˿�
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//��ȡ����
	BYTE cbTurnOutType = GetCardType(cbTurnCardData, cbTurnCardCount);

	//���Ʒ���
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//��������
	{
		//��ȡ��������һ��
		ASSERT(pSearchCardResult);
		if (!pSearchCardResult) return 0;

		//�Ƿ�һ�ֳ���
		int tmpType = GetCardType(cbCardData, cbCardCount);
		if ((tmpType != CT_ERROR && (tmpType != CT_THREE_TAKE_ONE)) || (tmpType == CT_THREE_TAKE_ONE&&cbCardCount==4))
		{
			pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], cbCardData,
				sizeof(BYTE)*cbCardCount);
			cbResultCount++;
		}

		//�����С�Ʋ��ǵ��ƣ�����ȡ
		BYTE cbSameCount = 0;
		if (cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount - 1]) == GetCardValue(cbCardData[cbCardCount - 2]))
		{
			cbSameCount = 1;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount - 1];
			BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount - 1]);
			for (int i = cbCardCount - 2; i >= 0; i--)
			{
				if (GetCardValue(cbCardData[i]) == cbCardValue)
				{
					pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
				}
				else break;
			}

			pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
			cbResultCount++;
		}

		//����
		BYTE cbTmpCount = 0;
		if (cbSameCount != 1)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 1, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//����
		if (cbSameCount != 2)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 2, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//����
		if (cbSameCount != 3)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 3, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//����һ��
		//cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 3, 1, &tmpSearchCardResult);
		//if (cbTmpCount > 0)
		//{
		//	pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
		//	CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
		//		sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
		//	cbResultCount++;
		//}

		//����һ��
		cbTmpCount = SearchTakeCardType(cbCardData, cbCardCount, 0, 3, 2, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//����
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 1, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//����
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 2, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		//����
		cbTmpCount = SearchLineCardType(cbCardData, cbCardCount, 0, 3, 0, &tmpSearchCardResult);
		if (cbTmpCount > 0)
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
			cbResultCount++;
		}

		////�ɻ�
		//cbTmpCount = SearchThreeTwoLine( cbCardData,cbCardCount,&tmpSearchCardResult );
		//if( cbTmpCount > 0 )
		//{
		//	pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
		//	CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
		//		sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
		//	cbResultCount++;
		//}

		//ը��
		if (cbSameCount != 4)
		{
			cbTmpCount = SearchSameCard(cbCardData, cbCardCount, 0, 4, &tmpSearchCardResult);
			if (cbTmpCount > 0)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0]);
				cbResultCount++;
			}
		}

		//�������
		if ((cbCardCount >= 2) && (cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E))
		{
			//���ý��
			pSearchCardResult->cbCardCount[cbResultCount] = 2;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
			pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

			cbResultCount++;
		}

		pSearchCardResult->cbSearchCount = cbResultCount;
		return cbResultCount;
	}
	case CT_SINGLE:					//��������
	case CT_DOUBLE:					//��������
	case CT_THREE:					//��������
	{
		//��������
		BYTE cbReferCard = cbTurnCardData[0];
		BYTE cbSameCount = 1;
		if (cbTurnOutType == CT_DOUBLE) cbSameCount = 2;
		else if (cbTurnOutType == CT_THREE) cbSameCount = 3;

		//������ͬ��
		cbResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, pSearchCardResult);

		break;
	}
	case CT_SINGLE_LINE:		//��������
	case CT_DOUBLE_LINE:		//��������
	case CT_THREE_LINE:				//��������
	{
		//��������
		BYTE cbBlockCount = 1;
		if (cbTurnOutType == CT_DOUBLE_LINE) cbBlockCount = 2;
		else if (cbTurnOutType == CT_THREE_LINE) cbBlockCount = 3;

		BYTE cbLineCount = cbTurnCardCount / cbBlockCount;

		//��������
		cbResultCount = SearchLineCardType(cbCardData, cbCardCount, cbTurnCardData[0], cbBlockCount, cbLineCount, pSearchCardResult);

		break;
	}
	case CT_THREE_TAKE_ONE:	//����һ��
	case CT_THREE_TAKE_TWO:	//����һ��
	{
		//Ч������
		if (cbCardCount < cbTurnCardCount) break;
		int cbLineResultCount = cbResultCount;
		//���������һ��������
		if (cbTurnCardCount == 4 || cbTurnCardCount == 5)
		{
			BYTE cbTakeCardCount = cbTurnOutType == CT_THREE_TAKE_ONE ? 1 : 2;

			//������������
			cbResultCount = SearchTakeCardType(cbCardData, cbCardCount, cbTurnCardData[2], 3, cbTakeCardCount, pSearchCardResult);
		}
		else
		{
			//��������
			BYTE cbBlockCount = 3;
			BYTE cbLineCount = cbTurnCardCount / (cbTurnOutType == CT_THREE_TAKE_ONE ? 4 : 5);
			BYTE cbTakeCardCount = cbTurnOutType == CT_THREE_TAKE_ONE ? 1 : 2;

			//��������
			BYTE cbTmpTurnCard[FULL_COUNT] = {};
			CopyMemory(cbTmpTurnCard, cbTurnCardData, sizeof(BYTE)*cbTurnCardCount);
			SortOutCardList(cbTmpTurnCard, cbTurnCardCount);
			cbLineResultCount = SearchLineCardType(cbCardData, cbCardCount, cbTmpTurnCard[0], cbBlockCount, cbLineCount, pSearchCardResult);
			
			//��ȡ����
			bool bAllDistill = true;
			if (G_THREE_TAKE_TWO_DAN&&cbCardCount>=cbTurnCardCount)
			{
				
				for (BYTE i = 0; i < cbLineResultCount; i++)
				{

					//��������
					BYTE cbTmpCardData[FULL_COUNT];
					BYTE cbTmpCardCount = cbCardCount;

					//ɾ������
					CopyMemory(cbTmpCardData, cbCardData, sizeof(BYTE)*cbCardCount);
					//ȥ������
					BYTE cbRemainCardData[FULL_COUNT];
					CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
					BYTE cbRemainCardCount = cbHandCardCount - 3*cbLineCount;
					RemoveCard(pSearchCardResult->cbResultCard[i], pSearchCardResult->cbCardCount[i], cbRemainCardData, cbHandCardCount);
					int cbDistillCount = cbTakeCardCount*cbLineCount;
					//�������
					BYTE cbComCard[8];
					BYTE cbComResCard[MAX_RESULT_COUNT][8];
					int cbComResLen = 0;
					BYTE cbSingleCardCount = cbLineCount*2;
					cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
					if (cbRemainCardCount>8)
					{
						cbRemainCardCount = 8;
					}
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
					tagSearchCardResult SameCardResult = {};
					CopyMemory(&SameCardResult, pSearchCardResult,
						sizeof(SameCardResult));
					int cbSameCount = cbLineCount * 3;
					for (BYTE m = 0; m < cbComResLen; m++)
					{
						//���ƴ���
						CopyMemory(&SameCardResult.cbResultCard[i][cbSameCount], cbComResCard[m],
							cbDistillCount);
						SameCardResult.cbCardCount[i] = cbTurnCardCount;
						//���ƽ��
						CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
							cbTurnCardCount);
						pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
						cbResultCount++;
					}
				}
				break;
			}
			else
			{
				for (BYTE i = 0; i < cbLineResultCount; i++)
				{
					BYTE cbResultIndex = i;

					//��������
					BYTE cbTmpCardData[FULL_COUNT];
					BYTE cbTmpCardCount = cbCardCount;

					//ɾ������
					CopyMemory(cbTmpCardData, cbCardData, sizeof(BYTE)*cbCardCount);
					VERIFY(RemoveCard(pSearchCardResult->cbResultCard[cbResultIndex], pSearchCardResult->cbCardCount[cbResultIndex],
						cbTmpCardData, cbTmpCardCount));
					cbTmpCardCount -= pSearchCardResult->cbCardCount[cbResultIndex];

					//������
					tagAnalyseResultNew  TmpResult = {};
					AnalysebCardData(cbTmpCardData, cbTmpCardCount, TmpResult);

					//��ȡ��
					BYTE cbDistillCard[FULL_COUNT] = {};
					BYTE cbDistillCount = 0;
					for (BYTE j = cbTakeCardCount - 1; j < CountArray(TmpResult.cbBlockCount); j++)
					{
						if (TmpResult.cbBlockCount[j] > 0)
						{
							if (j + 1 == cbTakeCardCount && TmpResult.cbBlockCount[j] >= cbLineCount)
							{
								BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
								CopyMemory(cbDistillCard, &TmpResult.cbCardData[j][(cbTmpBlockCount - cbLineCount)*(j + 1)],
									sizeof(BYTE)*(j + 1)*cbLineCount);
								cbDistillCount = (j + 1)*cbLineCount;
								break;
							}
							else
							{
								for (BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++)
								{
									BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
									CopyMemory(&cbDistillCard[cbDistillCount], &TmpResult.cbCardData[j][(cbTmpBlockCount - k - 1)*(j + 1)],
										sizeof(BYTE)*cbTakeCardCount);
									cbDistillCount += cbTakeCardCount;

									//��ȡ���
									if (cbDistillCount == cbTakeCardCount*cbLineCount) break;
								}
							}
						}

						//��ȡ���
						if (cbDistillCount == cbTakeCardCount*cbLineCount) break;
					}

					//��ȡ���
					if (cbDistillCount == cbTakeCardCount*cbLineCount)
					{
						//���ƴ���
						BYTE cbCount = pSearchCardResult->cbCardCount[cbResultIndex];
						CopyMemory(&pSearchCardResult->cbResultCard[cbResultIndex][cbCount], cbDistillCard,
							sizeof(BYTE)*cbDistillCount);
						pSearchCardResult->cbCardCount[cbResultIndex] += cbDistillCount;
					}
					//����ɾ������
					else
					{
						bAllDistill = false;
						pSearchCardResult->cbCardCount[cbResultIndex] = 0;
					}
				}

			}
			
			//�������
			if (!bAllDistill)
			{
				pSearchCardResult->cbSearchCount = cbResultCount;
				cbResultCount = 0;
				for (BYTE i = 0; i < pSearchCardResult->cbSearchCount; i++)
				{
					if (pSearchCardResult->cbCardCount[i] != 0)
					{
						tmpSearchCardResult.cbCardCount[cbResultCount] = pSearchCardResult->cbCardCount[i];
						CopyMemory(tmpSearchCardResult.cbResultCard[cbResultCount], pSearchCardResult->cbResultCard[i],
							sizeof(BYTE)*pSearchCardResult->cbCardCount[i]);
						cbResultCount++;
					}
				}
				tmpSearchCardResult.cbSearchCount = cbResultCount;
				CopyMemory(pSearchCardResult, &tmpSearchCardResult, sizeof(tagSearchCardResult));
			}
		}

		break;
	}
	//case CT_FOUR_TAKE_ONE:		//�Ĵ�����
	//case CT_FOUR_TAKE_TWO:		//�Ĵ���˫
	//{
	//	BYTE cbTakeCount = cbTurnOutType == CT_FOUR_TAKE_ONE ? 1 : 2;

	//	BYTE cbTmpTurnCard[MAX_COUNT] = {};
	//	CopyMemory(cbTmpTurnCard, cbTurnCardData, sizeof(BYTE)*cbTurnCardCount);
	//	SortOutCardList(cbTmpTurnCard, cbTurnCardCount);

	//	//��������
	//	cbResultCount = SearchTakeCardType(cbCardData, cbCardCount, cbTmpTurnCard[0], 4, cbTakeCount, pSearchCardResult);

	//	break;
	//}
	}
	if (bNoSearchBomb==false)
	{
		//����ը��
		if ((cbCardCount >= 4) && (cbTurnOutType != CT_MISSILE_CARD))
		{
			//��������
			BYTE cbReferCard = 0;
			if (cbTurnOutType == CT_BOMB_CARD) cbReferCard = cbTurnCardData[0];

			//����ը��
			BYTE cbTmpResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, 4, &tmpSearchCardResult);
			for (BYTE i = 0; i < cbTmpResultCount; i++)
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSearchCardResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i]);
				cbResultCount++;
			}
		}

		//�������
		if (cbTurnOutType != CT_MISSILE_CARD && (cbCardCount >= 2) && (cbCardData[0] == 0x4F) && (cbCardData[1] == 0x4E))
		{
			//���ý��
			pSearchCardResult->cbCardCount[cbResultCount] = 2;
			pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
			pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

			cbResultCount++;
		}
	}
	

	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}



float CGameLogicNew::GetHandScore(vector<tagOutCardResultNew> &CardTypeResult, int minTypeCount)
{
	float score = 0;
	int cbHandCardCount = 0;
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		float tempScore = GetCardTypeScore(CardTypeResult[i]);
		CardTypeResult[i].cbCardScore = tempScore;
		score += tempScore;

	}
	//�����С������С��10�Ļ�,Ӧ���ʵ��ӷ�
	score = score - CardTypeResult.size() * 7;
	
	return score;
}

float  CGameLogicNew::GetCardTypeScore(tagOutCardResultNew& CardTypeResult)
{
	//-- 2. maxCard�����ʹ�С����ֵ�����źͶ��Ӿ�������˳�Ӻ���������������ƣ������������ŵ��Ǹ��ƣ������������⣬��ͬ�����ͱȽϴ�С�����ǱȽ�maxCard
	BYTE cbCardData[MAX_COUNT] = { 0 };
	int  cbCardCount = CardTypeResult.cbCardCount;
	CopyMemory(cbCardData, CardTypeResult.cbResultCard, cbCardCount);
	int type = GetCardType(cbCardData, cbCardCount);

	if (type == CT_ERROR)
	{
		return 0;
	}
	else if (type == CT_SINGLE)
	{
		return  (float)(GetCardLogicValue(cbCardData[0]) - 10);
	}
	else if (type == CT_DOUBLE)
	{
		float score = (float)GetCardLogicValue(cbCardData[0]) / 100;
		float tempscore = (float)(GetCardLogicValue(cbCardData[0]) - 10);
		return score+tempscore ;
	}
	else if (type == CT_THREE)
	{
		return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 ;
	}
	else if (type == CT_SINGLE_LINE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) - 10 + 1;
		return  tempscore + ((float)(cbCardCount - 5) * 0.6);
	}
	else if (type == CT_DOUBLE_LINE)
	{
		float tempscore = (float)GetCardLogicValue(cbCardData[0]) - 10 + 1;
		return  tempscore;
	}
	else if (type == CT_THREE_LINE)
	{
		return (((float)GetCardLogicValue(cbCardData[0]) - 3 + 7) / 2) + (float)((cbCardCount - 3) * 1);
	}
	else if (type == CT_THREE_TAKE_TWO || type == CT_THREE_TAKE_ONE)
	{
		if (cbCardCount <= 5)
		{
			int cardCount = 0;
			if (type == CT_THREE_TAKE_ONE)
				cardCount = cbCardCount / 4;
			else
				cardCount = (cbCardCount / 5) * 2;

			float score = 0;
			vector<int> TwoCardLogic;
			for (int i = 0; i < cardCount; i++)
			{
				float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
				score = score + logic / 100;
				TwoCardLogic.push_back(logic);
			}
			if (TwoCardLogic.size()>=2 && TwoCardLogic[1] == TwoCardLogic[0])
			{
				score = score + 0.12;
			}

			return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 +  score;
		}
		else
		{
			int cardCount = 0;
			if (type == CT_THREE_TAKE_ONE)
				cardCount = cbCardCount / 4;
			else
				cardCount = (cbCardCount / 5) * 2;

			float score = 0;
			vector<int> TwoCardLogic;
			for (int i = 0; i < cardCount; i++)
			{
				float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
				score = score + logic / 100;
				TwoCardLogic.push_back(logic);
			}
			if (TwoCardLogic.size() >= 2&&TwoCardLogic[1] == TwoCardLogic[0])
			{
				score = score + 0.12;
			}

			return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 7) + 5 + score;
		}

	}

	else if (type == CT_BOMB_CARD )
	{
		return (float)GetCardLogicValue(cbCardData[0]) -3+7+9;

	}
	else if ( type == CT_MISSILE_CARD)
	{
		return (float)GetCardLogicValue(cbCardData[0]) + 7 + 20;

	}
	else if (type == CT_FOUR_TAKE_TWO )
	{
		int tempCardCount = 4;
		float score = 0;
		vector<int> TwoCardLogic;
		for (int i = 0; i < tempCardCount; i++)
		{
			float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
			score = score + logic / 100;
			TwoCardLogic.push_back(logic);
		}
		if (TwoCardLogic[1] == TwoCardLogic[0])
		{
			score = score + 0.12;
		}
		return ((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 + 9+score;
	}
	else if ( type == CT_FOUR_TAKE_ONE)
	{
		int tempCardCount =2;
		float score = 0;
		vector<int> TwoCardLogic;
		for (int i = 0; i < tempCardCount; i++)
		{
			float logic = GetCardLogicValue(cbCardData[cbCardCount - i - 1]);
			score = score + logic / 100;
			TwoCardLogic.push_back(logic);
		}
		if (TwoCardLogic[1] == TwoCardLogic[0])
		{
			score = score + 0.12;
		}
		return((float)GetCardLogicValue(cbCardData[0]) - 3 + 1) / 2 + 7 + score;
	}

	return (float)GetCardLogicValue(cbCardData[0]) - 10;
}

int CGameLogicNew::RemoveTypeCard(tagOutCardTypeResultNew & CardTypeResult, BYTE * cbTmpHReminCardData, int &iLeftCardCount)
{
	for (int i = CardTypeResult.cbCardTypeCount-1; i >=0; i--)
	{
		if (RemoveCard(CardTypeResult.cbCardData[i], CardTypeResult.cbEachHandCardCount[i], cbTmpHReminCardData, iLeftCardCount))
		{
			iLeftCardCount -= CardTypeResult.cbEachHandCardCount[i];
		}
	}	
	return iLeftCardCount;
}

void CGameLogicNew::GetMissileCardResult(BYTE * cbHandCardData,BYTE cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 2)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	if ((cbTmpCardData[0] == 0x4F) && (cbTmpCardData[1] == 0x4E))
	{
		BYTE Index = CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount;
		CardTypeResult[CT_MISSILE_CARD].cbCardType = CT_MISSILE_CARD;
		CardTypeResult[CT_MISSILE_CARD].cbCardData[Index][0] = cbTmpCardData[0];
		CardTypeResult[CT_MISSILE_CARD].cbCardData[Index][1] = cbTmpCardData[1];
		CardTypeResult[CT_MISSILE_CARD].cbEachHandCardCount[Index] = 2;
		CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount++;
	}
}

void CGameLogicNew::GetFourTakeOneResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	/*if (cbHandCardCount < 6)
	{
		return;
	}*/
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	BYTE cbFirstCard = 0;
	//ȥ������
	for (BYTE i = 0; i < cbHandCardCount; ++i)	if (GetCardColor(cbTmpCardData[i]) != 0x40)	{ cbFirstCard = i; break; }

	BYTE cbHandAllFourCardData[MAX_COUNT];
	BYTE cbHandAllFourCardCount = 0;
	//��ȡ����
	GetAllBomCard(cbTmpCardData + cbFirstCard, cbHandCardCount - cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount);

	//�Ƴ�����
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount = cbHandCardCount - cbHandAllFourCardCount;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount);
	cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
	for (BYTE Start = 0; Start < cbHandAllFourCardCount; Start += 4)
	{
		BYTE Index;
		//�������
		BYTE cbComCard[8];
		BYTE cbComResCard[MAX_RESULT_COUNT][8];
		int cbComResLen = 0;
		//�������
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbRemainCardCount, 2);
		for (BYTE i = 0; i < cbComResLen; ++i)
		{
			//���ܴ���
			if (GetCardValue(cbComResCard[i][0]) == GetCardValue(cbComResCard[i][1])) continue;

			Index = CardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount;
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardType = CT_FOUR_TAKE_ONE;
			CopyMemory(CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index], cbHandAllFourCardData + Start, 4);
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index][4] = cbComResCard[i][0];
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardData[Index][4 + 1] = cbComResCard[i][1];
			CardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardCount[Index] = 6;
			CardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount++;

			ASSERT(CardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount < MAX_TYPE_COUNT);
		}
	}
}

void CGameLogicNew::GetFourTakeTwoResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	BYTE cbFirstCard = 0;
	//ȥ������
	for (BYTE i = 0; i < cbHandCardCount; ++i)	if (GetCardColor(cbTmpCardData[i]) != 0x40)	{ cbFirstCard = i; break; }

	BYTE cbHandAllFourCardData[MAX_COUNT];
	BYTE cbHandAllFourCardCount = 0;

	//��ȡ����
	GetAllBomCard(cbTmpCardData + cbFirstCard, cbHandCardCount - cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount);

	//�Ƴ�����
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount = cbHandCardCount - cbHandAllFourCardCount;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount);

	for (BYTE Start = 0; Start < cbHandAllFourCardCount; Start += 4)
	{
		//��ȡ����
		BYTE cbAllDoubleCardData[MAX_COUNT];
		BYTE cbAllDoubleCardCount = 0;
		GetAllDoubleCard(cbRemainCard, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);

		BYTE cbDoubleCardIndex[10]; //�����±�
		for (BYTE i = 0, j = 0; i < cbAllDoubleCardCount; i += 2, ++j)
			cbDoubleCardIndex[j] = i;

		//�������
		BYTE cbComCard[8];
		BYTE cbComResCard[MAX_RESULT_COUNT][8];
		int cbComResLen = 0;

		//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount / 2, 2);
		for (BYTE i = 0; i < cbComResLen; ++i)
		{
			BYTE Index = CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount;
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardType = CT_FOUR_TAKE_TWO;
			CopyMemory(CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index], cbHandAllFourCardData + Start, 4);

			//�������
			for (BYTE j = 0, k = 0; j < 4; ++j, k += 2)
			{
				CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + k] = cbAllDoubleCardData[cbComResCard[i][j]];
				CardTypeResult[CT_FOUR_TAKE_TWO].cbCardData[Index][4 + k + 1] = cbAllDoubleCardData[cbComResCard[i][j] + 1];
			}

			CardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardCount[Index] = 8;
			CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount++;

			ASSERT(CardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount < MAX_TYPE_COUNT);
		}
	}
}

void CGameLogicNew::GetThreeResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 3)
	{
		return;
	}
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 3, &SameCardResult);
	for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount;
		CardTypeResult[CT_THREE].cbCardType = CT_THREE;
		CopyMemory(CardTypeResult[CT_THREE].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
		CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = SameCardResult.cbCardCount[i];
		CardTypeResult[CT_THREE].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetDoubleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 2)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbTmpCardData, cbHandCardCount, 0, 2, &SameCardResult);
	for (BYTE i = 0; i < SameCardResult.cbSearchCount; i++)
	{
		BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount;
		CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE;
		CopyMemory(CardTypeResult[CT_DOUBLE].cbCardData[Index], SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i]);
		CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = SameCardResult.cbCardCount[i];
		CardTypeResult[CT_DOUBLE].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetSingleResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	for (BYTE i = 0; i < cbHandCardCount; ++i)
	{
		if (GetACardCount(cbHandCardData,cbHandCardCount,cbHandCardData[i])==1)
		{
			BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
			CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
			CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCardData[i];
			CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
			CardTypeResult[CT_SINGLE].cbCardTypeCount++;
		}
	}
}

void CGameLogicNew::GetBombCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount<4)
	{
		return;
	}
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);
	BYTE cbFourCardData[MAX_COUNT];
	BYTE cbFourCardCount = 0;
/*	if (cbHandCardCount >= 2 && 0x4F == cbTmpCardData[0] && 0x4E == cbTmpCardData[1])
	{
		BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
		CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCardData[0];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCardData[1];
		CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2;
		CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;
		GetAllBomCard(cbTmpCardData + 2, cbHandCardCount - 2, cbFourCardData, cbFourCardCount);
	}
	else*/ GetAllBomCard(cbTmpCardData, cbHandCardCount, cbFourCardData, cbFourCardCount);
	for (BYTE i = 0; i < cbFourCardCount; i += 4)
	{
		BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
		CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbFourCardData[i];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbFourCardData[i + 1];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][2] = cbFourCardData[i + 2];
		CardTypeResult[CT_BOMB_CARD].cbCardData[Index][3] = cbFourCardData[i + 3];
		CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
		CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

	}
}

void CGameLogicNew::GetThreeTakeTwoCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 5)
	{
		return;
	}
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	BYTE cbHandThreeCard[MAX_COUNT] = { 0 };
	BYTE cbHandThreeCount = 0;
	BYTE cbRemainCarData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = 0;

	//��ȡ����
	GetAllThreeCard(cbTmpCardData, cbHandCardCount, cbHandThreeCard, cbHandThreeCount);

	

	//������һ��
	for (BYTE i = 0; i < cbHandThreeCount; i += 3)
	{
	
		//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
		CopyMemory(cbRemainCarData, cbTmpCardData, cbHandCardCount);
		RemoveCard(&cbHandThreeCard[i], 3, cbRemainCarData, cbHandCardCount);
		cbRemainCardCount = cbHandCardCount - 3;
		//��ȡ����
		BYTE cbAllDoubleCardData[MAX_COUNT] = { 0 };
		BYTE cbAllDoubleCardCount = 0;
		
		{
			GetAllDoubleCard(cbRemainCarData, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);

			////������һ��
			for (BYTE j = 0; j < cbAllDoubleCardCount; j += 2)
			{
				BYTE Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][0] = cbHandThreeCard[i];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][1] = cbHandThreeCard[i + 1];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][2] = cbHandThreeCard[i + 2];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][3] = cbAllDoubleCardData[j];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][4] = cbAllDoubleCardData[j + 1];
				CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = 5;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;
			}
		}
		if (G_THREE_TAKE_TWO_DAN)
		{
			//�������
			BYTE cbComCard[8];
			BYTE cbComResCard[MAX_RESULT_COUNT][8];
			int cbComResLen = 0;
			BYTE cbSingleCardCount = 2;
			cbRemainCardCount = ClearReLogicValue(cbRemainCarData, cbRemainCardCount);
			Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCarData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
			for (BYTE m = 0; m < cbComResLen; m++)
			{
				BYTE Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][0] = cbHandThreeCard[i];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][1] = cbHandThreeCard[i + 1];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][2] = cbHandThreeCard[i + 2];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][3] = cbComResCard[m][0];
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][4] = cbComResCard[m][1];
				CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = 5;
				CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;
			}
		}
		
	}

	//��������
	BYTE cbLeftThreeCardCount = cbHandThreeCount;
	bool bFindThreeLine = true;
	BYTE cbLastIndex = 0;
	if (GetCardLogicValue(cbHandThreeCard[0]) == 15) cbLastIndex = 3;
	while (cbLeftThreeCardCount >= 6 && bFindThreeLine)
	{
		BYTE cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
		BYTE cbThreeLineCard[MAX_COUNT] = { 0 };
		BYTE cbThreeLineCardCount = 3;
		cbThreeLineCard[0] = cbHandThreeCard[cbLastIndex];
		cbThreeLineCard[1] = cbHandThreeCard[cbLastIndex + 1];
		cbThreeLineCard[2] = cbHandThreeCard[cbLastIndex + 2];

		bFindThreeLine = false;
		for (BYTE j = 3 + cbLastIndex; j < cbLeftThreeCardCount; j += 3)
		{
			//�����ж�
			if (1 != (cbLastLogicCard - (GetCardLogicValue(cbHandThreeCard[j]))))
			{
				cbLastIndex = j;
				if (cbLeftThreeCardCount - j >= 6) bFindThreeLine = true;

				break;
			}

			cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[j]);
			cbThreeLineCard[cbThreeLineCardCount] = cbHandThreeCard[j];
			cbThreeLineCard[cbThreeLineCardCount + 1] = cbHandThreeCard[j + 1];
			cbThreeLineCard[cbThreeLineCardCount + 2] = cbHandThreeCard[j + 2];
			cbThreeLineCardCount += 3;
		}
		if (cbThreeLineCardCount > 3)
		{
			//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
			CopyMemory(cbRemainCarData, cbTmpCardData, cbHandCardCount);
			RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbRemainCarData, cbHandCardCount);
			cbRemainCardCount = cbHandCardCount - cbThreeLineCardCount;
			//��ȡ����
			BYTE cbAllDoubleCardData[MAX_COUNT]={ 0 };
			BYTE cbAllDoubleCardCount = 0;
			if (G_THREE_TAKE_TWO_DAN)
			{
				cbAllDoubleCardCount = cbRemainCardCount;
				
				CopyMemory(cbAllDoubleCardData, cbRemainCarData, cbRemainCardCount);
				SortCardList(cbAllDoubleCardData, cbRemainCardCount, ST_ASCENDING);
				if (cbAllDoubleCardCount >=10)
				{
					cbAllDoubleCardCount = 8;
				}
			}
			else
			{
				GetAllDoubleCard(cbRemainCarData, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);
			}
			BYTE Index;

			for (BYTE start = 0; start < cbThreeLineCardCount - 3; start += 3)
			{
				//��˳��Ŀ
				BYTE cbThisTreeLineCardCount = cbThreeLineCardCount - start;
				//��������
				BYTE cbDoubleCardCount = ((cbThisTreeLineCardCount) / 3);

				//�������
				BYTE cbComCard[8] = { 0 };
				BYTE cbComResCard[MAX_RESULT_COUNT][8] = { 0 };
				int cbComResLen = 0;
				BYTE cbSingleCardCount = 2 * cbDoubleCardCount;
				//cbRemainCardCount = ClearReLogicValue(cbRemainCarData, cbRemainCardCount);
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbAllDoubleCardData, cbSingleCardCount, cbAllDoubleCardCount, cbSingleCardCount);
				
				for (BYTE i = 0; i < cbComResLen; ++i)
				{
					Index = CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
					CardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
					////��������
					CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index], cbThreeLineCard + start, cbThisTreeLineCardCount);
					CopyMemory(&CardTypeResult[CT_THREE_TAKE_TWO].cbCardData[Index][cbThisTreeLineCardCount], cbComResCard[i], cbSingleCardCount);
					CardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[Index] = cbThisTreeLineCardCount + 2 * cbDoubleCardCount;
					CardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;

				}

			}
			//�Ƴ�����
			bFindThreeLine = true;
			RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount);
			cbLeftThreeCardCount -= cbThreeLineCardCount;
		}
	}
}

void CGameLogicNew::GetThreeTakeOneCardResult(BYTE * cbHandCardData,  BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 4)
	{
		return;
	}
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	BYTE cbHandThreeCard[MAX_COUNT] = { 0 };
	BYTE cbHandThreeCount = 0;

	//�Ƴ�ը��
	BYTE cbAllBomCardData[MAX_COUNT];
	BYTE cbAllBomCardCount = 0;

	/*�Ż�*/
		BYTE cbTestHandThreeCard[MAX_COUNT] = { 0 };
	BYTE cbTestHandThreeCount = 0;
	GetAllThreeCard(cbTmpCardData, cbHandCardCount - cbAllBomCardCount, cbTestHandThreeCard, cbTestHandThreeCount);
	//if (cbHandCardCount > NORMAL_COUNT )
	//{
	//	GetAllBomCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount);
	//	RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount);
	//}


	GetAllThreeCard(cbTmpCardData, cbHandCardCount - cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount);

	{
		
		//������һ��
		for (BYTE i = 0; i < cbHandThreeCount; i += 3)
		{
			//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
			BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
			CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

			//ȥ������
			BYTE cbRemainCardData[FULL_COUNT];
			CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
			BYTE cbRemainCardCount = cbHandCardCount - 3;
			RemoveCard(&cbHandThreeCard[i], 3, cbRemainCardData, cbHandCardCount);
			cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
			//������һ��
			for (BYTE j = 0; j < cbRemainCardCount; ++j)
			{
				{
					BYTE Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][0] = cbHandThreeCard[i];
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][1] = cbHandThreeCard[i + 1];
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][2] = cbHandThreeCard[i + 2];
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index][3] = cbRemainCardData[j];
					CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = 4;
					CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;
				}
			}
		}
	}
	//��������
	BYTE cbLeftThreeCardCount = cbHandThreeCount;
	bool bFindThreeLine = true;
	BYTE cbLastIndex = 0;
	if (GetCardLogicValue(cbHandThreeCard[0]) == 15) cbLastIndex = 3;
	while (cbLeftThreeCardCount >= 6 && bFindThreeLine)
	{
		BYTE cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
		BYTE cbThreeLineCard[MAX_COUNT];
		BYTE cbThreeLineCardCount = 3;
		cbThreeLineCard[0] = cbHandThreeCard[cbLastIndex];
		cbThreeLineCard[1] = cbHandThreeCard[cbLastIndex + 1];
		cbThreeLineCard[2] = cbHandThreeCard[cbLastIndex + 2];

		bFindThreeLine = false;
		for (BYTE j = 3 + cbLastIndex; j < cbLeftThreeCardCount; j += 3)
		{
			//�����ж�
			if (1 != (cbLastLogicCard - (GetCardLogicValue(cbHandThreeCard[j]))))
			{
				cbLastIndex = j;
				if (cbLeftThreeCardCount - j >= 6) bFindThreeLine = true;

				break;
			}

			cbLastLogicCard = GetCardLogicValue(cbHandThreeCard[j]);
			cbThreeLineCard[cbThreeLineCardCount] = cbHandThreeCard[j];
			cbThreeLineCard[cbThreeLineCardCount + 1] = cbHandThreeCard[j + 1];
			cbThreeLineCard[cbThreeLineCardCount + 2] = cbHandThreeCard[j + 2];
			cbThreeLineCardCount += 3;
		}
		if (cbThreeLineCardCount > 3)
		{
			BYTE Index;

			BYTE cbRemainCard[MAX_COUNT];
			BYTE cbRemainCardCount = cbHandCardCount - cbAllBomCardCount - cbHandThreeCount;


			//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
			CopyMemory(cbRemainCard, cbTmpCardData, (cbHandCardCount - cbAllBomCardCount)*sizeof(BYTE));
			RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCard, cbHandCardCount - cbAllBomCardCount);
			//���㵥��
			BYTE cbSingleCard[MAX_COUNT] = { 0 };
			BYTE cbSingleCardCount = 0;
			GetAllSingleCard(cbRemainCard, cbRemainCardCount, cbSingleCard, cbSingleCardCount);
			//�����Ż�
			if (cbSingleCardCount >= 2 && cbHandCardCount >= 17)
			{
				ZeroMemory(cbRemainCard, MAX_COUNT);
				CopyMemory(cbRemainCard, cbSingleCard, cbSingleCardCount);
				cbRemainCardCount = cbSingleCardCount;
			}
			cbRemainCardCount = ClearReLogicValue(cbRemainCard, cbRemainCardCount);
			SortCardList(cbRemainCard, cbRemainCardCount, ST_ASCENDING);
			for (BYTE start = 0; start < cbThreeLineCardCount - 3; start += 3)
			{
				//��˳��Ŀ
				BYTE cbThisTreeLineCardCount = cbThreeLineCardCount - start;
				//���Ƹ���
				BYTE cbSingleCardCount = (cbThisTreeLineCardCount) / 3;

				//���Ʋ���
				if (cbRemainCardCount < cbSingleCardCount) continue;

				//�������
				BYTE cbComCard[8];
				BYTE cbComResCard[MAX_RESULT_COUNT][8];
				int cbComResLen = 0;

				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
			
				for (BYTE i = 0; i < cbComResLen; ++i)
				{
					vector<BYTE> vecCard;
					for (int k = 0; k < cbThisTreeLineCardCount; k++)
					{
						vecCard.push_back(cbThreeLineCard[start+k]);
					}
					for (int k = 0; k < cbSingleCardCount; k++)
					{
						vecCard.push_back(cbComResCard[i][k]);
					}
					{
						Index = CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
						CardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
						//��������
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index], cbThreeLineCard + start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//���浥��
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].cbCardData[Index] + cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount);


						CardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[Index] = cbThisTreeLineCardCount + cbSingleCardCount;
						CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;

						ASSERT(CardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount < MAX_TYPE_COUNT);
					}
				}

			}

			//�Ƴ�����
			bFindThreeLine = true;
			RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount);
			cbLeftThreeCardCount -= cbThreeLineCardCount;
		}
	}
}

void CGameLogicNew::GetThreeLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 6)
	{
		return;
	}
	//��������
	//��������
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//��������
	int cbBlockCount = 3;
	//��������
	tagSearchCardResult tmpSearchCardResult = {};
	//��������
	for (int i = 2; i < 7; i++)
	{
		SearchLineCardType(cbTmpCardData, cbHandCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
		if (tmpSearchCardResult.cbSearchCount > 0)
		{
			//��������
			for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
			{
				int Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount;
				CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE;
				CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
				CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
				CardTypeResult[CT_THREE_LINE].cbCardTypeCount++;
			}
		}
		else
		{
			break;
		}
	}
}

void CGameLogicNew::GetDoubleLineCardResult(BYTE *cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 4)
	{
		return;
	}
	//��������
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
		CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

		 //��������
		int cbBlockCount = 2;
		//��������
		tagSearchCardResult tmpSearchCardResult = {};
			//��������
		for (int i = 2; i < 11; i++)
		{
			SearchLineCardType(cbTmpCardData, cbHandCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
			if (tmpSearchCardResult.cbSearchCount > 0)
			{
				 //��������
				for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
				{
					int Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount;
					CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE;
					CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
					CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
					CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++;
				}
			}
			else
			{
				break;
			}
		}
}

void CGameLogicNew::GetSingleLineCardResult(BYTE * cbHandCardData, BYTE const cbHandCardCount, tagOutCardTypeResultNew * CardTypeResult)
{
	if (cbHandCardCount < 3)
	{
		return;
	}
	 //��������
	int cbBlockCount = 1;
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	BYTE  cbTmpCardData[MAX_COUNT] = { 0 };
	int   tmpCardCount = 0;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

	//ȥ��2����
	for (BYTE i = 0; i < cbHandCardCount; ++i)
	{
		if (GetCardLogicValue(cbTmpCardData[i]) <15)
		{
			cbTmpCardData[tmpCardCount++] = cbHandCardData[i];
		}
	}
		 //��������
		 tagSearchCardResult tmpSearchCardResult = {};
		   int i = 3;
			while (i <= 12)
			{
				SearchLineCardType(cbTmpCardData, tmpCardCount, 0, cbBlockCount, i, &tmpSearchCardResult);
				if (tmpSearchCardResult.cbSearchCount > 0)
				{
					//�����Ż�, �����˳�����ص�����, �����������ص�
					//��������
					//��������
					for (int j = 0; j < tmpSearchCardResult.cbSearchCount; j++)
					{
						int Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
						CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
						CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], tmpSearchCardResult.cbResultCard[j], tmpSearchCardResult.cbCardCount[j]);
						CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = tmpSearchCardResult.cbCardCount[j];
						CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
						
					}
				}
				else
				{
					break;
				}
				i++;

			}
			
	
}


//
//void com(vector<int> &candidates, int start, int sum, int target, vector<int> &path,  vector<vector<int> > &res)
//{
//	if (sum > target)//����Ŀ��ֵ �˳�
//		return;
//	if (sum == target)// �ҵ�һ�ֽ�
//	{
//			res.push_back(path);
//		
//		return;
//	}
//
//	for (int i = start; i < candidates.size(); i++)
//	{
//		path.push_back(i);//��ŵ�ǰֵ
//		com(candidates, i + 1, sum + candidates[i], target, path, res);
//		path.pop_back();//����
//
//	}
//
//}
void FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	bool bSuccess = true;
	bool tableRe[100] = { false };
	for (int j = 0; j < tempVecSearch.size(); j++)
	{
		for (int i = 0; i < tempVecSearch[j].cbCardCount; i++)
		{
			if (tableRe[tempVecSearch[j].cbResultCard[i]] != true)
			{
				tableRe[tempVecSearch[j].cbResultCard[i]] = true;
			}
			else
			{
				return;
			}
		}
	}
	resultAllVec.push_back(tempVecSearch);

}
void com(vector<int> &candidates, int start, int sum, int target, int path[], int &pathCount, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec, BYTE const cbHandCardData[], BYTE const cbHandCardCount )
{
	if (sum > target)//����Ŀ��ֵ �˳�
		return;
	if (pathCount >= 2)
	{
		int tableRe[100] = { 0 };
		ZeroMemory(tableRe, sizeof(tableRe));
		for (int i = 0; i < cbHandCardCount; i++)
		{
			tableRe[cbHandCardData[i]]++;
		}
		int tableRe2[100] = { 0 };
		ZeroMemory(tableRe2, sizeof(tableRe2));
		for (int j = 0; j < pathCount; j++)
		{
			for (int i = 0; i < resultVec[path[j]].cbCardCount; i++)
			{
				BYTE tempIndex = resultVec[path[j]].cbResultCard[i];
				tableRe2[tempIndex] ++;
				if (tableRe2[tempIndex]>tableRe[tempIndex])
				{
					return;
				}
			}
		}
	}
	if (sum == target)// �ҵ�һ�ֽ�
	{
		vector<tagOutCardResultNew> tempVecSearch;
		for (int i = 0; i < pathCount; i++)
		{
			tempVecSearch.push_back(resultVec[path[i]]);
		}
		resultAllVec.push_back(tempVecSearch);
		//FillCombinatory(tempVecSearch, resultAllVec);
		return;
	}
	
	for (int i = start; i < candidates.size(); i++)
	{
		path[pathCount++] = i;//��ŵ�ǰֵ
		com(candidates, i + 1, sum + candidates[i], target, path, pathCount, resultVec, resultAllVec, cbHandCardData,cbHandCardCount);
		path[pathCount--] = 0;//����

	}

}
void comThread(vector<int> &candidates, int start, int end, int sum, int target, int path[], int &pathCount, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec, BYTE const cbHandCardData[], BYTE const cbHandCardCount)
{
	if (sum > target)//����Ŀ��ֵ �˳�
		return;
	if (pathCount >= 2)
	{
		int tableRe[100] = { 0 };
		ZeroMemory(tableRe, sizeof(tableRe));
		for (int i = 0; i < cbHandCardCount; i++)
		{
			tableRe[cbHandCardData[i]]++;
		}
		int tableRe2[100] = { 0 };
		ZeroMemory(tableRe2, sizeof(tableRe2));
		for (int j = 0; j < pathCount; j++)
		{
			for (int i = 0; i < resultVec[path[j]].cbCardCount; i++)
			{
				BYTE tempIndex = resultVec[path[j]].cbResultCard[i];
				tableRe2[tempIndex] ++;
				if (tableRe2[tempIndex]>tableRe[tempIndex])
				{
					return;
				}
			}
		}

	}
	if (sum == target)// �ҵ�һ�ֽ�
	{
		vector<tagOutCardResultNew> tempVecSearch;
		for (int i = 0; i < pathCount; i++)
		{
			tempVecSearch.push_back(resultVec[path[i]]);
		}
		resultAllVec.push_back(tempVecSearch);
		//FillCombinatory(tempVecSearch, resultAllVec);
		return;
	}
	//if (res.size() > 5000000)
	//{
	//	//�ﵽ6����ʱ,�ڴ�ᱩ��,������5�����ֽ���,����Ҳ��Ӱ��һ�ֺ��Ƶ����
	//	return;
	//}

	for (int i = start; i < candidates.size(); i++)
	{
		if (pathCount == 0 && i >= end)
		{
			break;
		}
		path[pathCount++]=i;//��ŵ�ǰֵ
		comThread(candidates, i + 1, end, sum + candidates[i], target, path, pathCount, resultVec, resultAllVec, cbHandCardData,cbHandCardCount);
		path[pathCount--] = 0;//����

	}

}
struct accumulate_block{
	void operator () (vector<int> &candidates, int start, int end, int sum, int target, vector<tagOutCardResultNew>& resultVec, vector<vector<tagOutCardResultNew> > &resultAllVec, BYTE const cbHandCardData[], BYTE const cbHandCardCount){
		int   vecTempIndex[MAX_COUNT] = { 0 };
		int   pathCount = 0;
		comThread(candidates, start, end, 0, target, vecTempIndex, pathCount, resultVec, resultAllVec, cbHandCardData, cbHandCardCount);
	}
};

int multithreadSort(vector<int> &vecHandCardCount, int sum, vector<tagOutCardResultNew>& resultVec, vector<vector<vector<tagOutCardResultNew> >> &resultAllVec, BYTE const cbHandCardData[], BYTE const cbHandCardCount){
	unsigned long const min_per_thread = 10;//ÿ���̵߳������д�С  
	unsigned long const max_threads = 4;
	int num_threads = vecHandCardCount.size();
	if (num_threads > 4)
	{
		num_threads = 4;
	}
	vector<thread> threads(num_threads);

	for (int i = 0; i < num_threads; i++){//����ֻ�������̵߳���������,���߳�Ҳ��ִ��һ����������
		int start = 0;
		int end = 0;
		if (i == 0)
		{
			start = 0;
			end = 1;
		}
		else if (i == 1)
		{
			start = 1;
			end = 2;
		}
		else if (i == 2)
		{
			start = 2;
			end = 3;
		}
		else if (i >= 3)
		{
			start = 3;
			end = vecHandCardCount.size();
		}
		threads[i] = thread(accumulate_block(), std::ref(vecHandCardCount), start, end, 0, sum, std::ref(resultVec), std::ref(resultAllVec[i]),cbHandCardData, cbHandCardCount);//ÿ�����̵߳������з���  
	}
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::join));//�ȴ����߳����  
	return 0;

}
int CGameLogicNew::FindCardKindMinNum(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT], vector<tagOutCardResultNew>   &CardResult, float &MinTypeScore, bool & bBiYing, bool bSearchBiYing)
{
	
	ZeroMemory(CardTypeResult, sizeof(tagOutCardTypeResultNew) * CT_TYPE_COUNT);
	if (cbHandCardCount==1)
	{

		tagOutCardResultNew OutCardResult;
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardType = CT_SINGLE;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		CardResult.push_back(OutCardResult);
		MinTypeScore = GetCardTypeScore(OutCardResult);
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbHandCardData[0];
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++;
		return 1;
	}

	BYTE cbTmpHReminCardData[MAX_COUNT] = { 0 };
	//�����˿ˣ���ֹ����ʱ�ı��˿�
	BYTE cbReserveCardData[MAX_COUNT];
	BYTE cbReserveCardCount=cbHandCardCount;
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount);
	//�������
	GetMissileCardResult(cbReserveCardData, cbReserveCardCount, CardTypeResult);
	BYTE MISSILE_CARD[] = { 0x4E, 0x4F };
	if (RemoveCard(MISSILE_CARD, sizeof(MISSILE_CARD), cbReserveCardData, cbReserveCardCount))
	{
		cbReserveCardCount -= sizeof(MISSILE_CARD);
	}
	int iLeftCardCount = cbReserveCardCount;
	SortCardList(cbReserveCardData, cbReserveCardCount, ST_ORDER);
	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	//ը������
	GetBombCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	iLeftCardCount = RemoveTypeCard(CardTypeResult[CT_BOMB_CARD], cbTmpHReminCardData, iLeftCardCount);
		//--�ܵÿ�����ը�����ͼ�ֵ�Ƚϸ�, ������Ҫ��ը�����Ƴ���
	CopyMemory(cbReserveCardData, cbTmpHReminCardData, iLeftCardCount);
	cbReserveCardCount = iLeftCardCount;
	//����һ��
	GetThreeTakeTwoCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//��������
	GetThreeLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//��������
	//if (cbHandCardCount == 3)
	{
		GetThreeResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	}
	//˫������
	GetDoubleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//��������
	GetSingleLineCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//����һ��
	//if (cbHandCardCount == 4)
	{
		GetThreeTakeOneCardResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	}
	//��������
	GetDoubleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	//��������
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	
	//if (iLeftCardCount<=10)
	{
		//�Ĵ�����
		//if (CT_FOUR_TAKE_ONE>0)
		//{
		//	GetFourTakeOneResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
		//}
		////�Ĵ�����
		//if (CT_FOUR_TAKE_TWO>0)
		//{
		//	GetFourTakeTwoResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
		//}
	}

	//��������
	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	iLeftCardCount = cbReserveCardCount;
	RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_DOUBLE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	GetDoubleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	iLeftCardCount = cbReserveCardCount;
	RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_THREE_LINE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	CopyMemory(cbTmpHReminCardData, cbReserveCardData, cbReserveCardCount);
	iLeftCardCount = cbReserveCardCount;
	RemoveTypeCard(CardTypeResult[CT_SINGLE_LINE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_THREE], cbTmpHReminCardData, iLeftCardCount);
	RemoveTypeCard(CardTypeResult[CT_SINGLE], cbTmpHReminCardData, iLeftCardCount);
	GetSingleResult(cbTmpHReminCardData, iLeftCardCount, CardTypeResult);

	BYTE tempSingle[MAX_COUNT] = { 0 };
	BYTE tempSingleCount = 0;
	for (BYTE i = 0; i < CardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
	{
		for (BYTE j = 0; j < CardTypeResult[CT_SINGLE].cbEachHandCardCount[i]; j++)
		{
			tempSingle[tempSingleCount++] = CardTypeResult[CT_SINGLE].cbCardData[i][0];
		}
	}
	//����һ������,���������ڵ���5��,˫�Ƶ�Ҳ�ӽ�ȥ
	for (BYTE i = 0; i < CardTypeResult[CT_DOUBLE].cbCardTypeCount; ++i)
	{
		if (CardTypeResult[CT_SINGLE].cbCardTypeCount >= 17)
		{
			break;
		}
		for (BYTE j = 0; j < CardTypeResult[CT_DOUBLE].cbEachHandCardCount[i]; j++)
		{
			//if (GetACardCount(tempSingle, tempSingleCount, CardTypeResult[CT_DOUBLE].cbCardData[i][j]) == 0)
			{
				BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
				CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
				CardTypeResult[CT_SINGLE].cbCardData[Index][0] = CardTypeResult[CT_DOUBLE].cbCardData[i][j];
				CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
				CardTypeResult[CT_SINGLE].cbCardTypeCount++;
			}
		}
		
	}
	clearSingleReResult(CardTypeResult);

	vector<int>   vecHandCardCount;
	vector<tagOutCardResultNew> resultVec;
	for (int i = CT_TYPE_COUNT-1; i >=0; i--)
	{
		if (CardTypeResult[i].cbCardType > 0)
		{
			for (int j = 0; j < CardTypeResult[i].cbCardTypeCount; j++)
			{
				tagOutCardResultNew typeResult;
				ZeroMemory(&typeResult, sizeof(typeResult));
				typeResult.cbCardType = CardTypeResult[i].cbCardType;
				typeResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[j];
				CopyMemory(typeResult.cbResultCard, CardTypeResult[i].cbCardData[j], typeResult.cbCardCount);
				resultVec.push_back(typeResult);
				vecHandCardCount.push_back(typeResult.cbCardCount);
			}
		}
	}
	
	//////////////////////////////////////�Ƕ��̰߳汾
	#define NO_CONDITION_THREAD 1
#ifdef NO_CONDITION_THREAD0
	int   vecTempIndex[MAX_COUNT] = { 0 };
	int   pathCount = 0;
	int MinTypeCount = INT_MAX;
	int index = -1;
	vector<vector<tagOutCardResultNew> > resultAllVec;
	com(vecHandCardCount, 0, 0, cbHandCardCount, vecTempIndex, pathCount, resultVec, resultAllVec);
	for (int i = 0; i < resultAllVec.size(); i++)
	{
		int tempScore = GetHandScore(resultAllVec.at(i), MinTypeCount);
		if (resultAllVec.at(i).size() < MinTypeCount || ((resultAllVec.at(i).size() == MinTypeCount && tempScore > MinTypeScore)))
		{
			MinTypeScore = GetHandScore(resultAllVec.at(i), MinTypeCount);
			index = i;
			MinTypeCount = resultAllVec.at(i).size();
		}
		if (cbHandCardCount < 18 && bSearchBiYing == true && isBiYing(resultAllVec[i]))
		{
			if (bBiYing != true || (bBiYing && resultAllVec[i].size() <= MinTypeCount && tempScore>MinTypeScore))
			{
				bBiYing = true;
				MinTypeScore = tempScore;
				index = i;
				MinTypeCount = resultAllVec[i].size();
			}

	}

	}
	if (resultAllVec.size() > 0)
	{
		CardResult = resultAllVec[index];
		sort(CardResult.begin(), CardResult.end(), [this](tagOutCardResultNew & first, tagOutCardResultNew& second)
		{ if (first.cbCardType > second.cbCardType)
		{
			return true;
		}
		if (first.cbCardType == second.cbCardType)
		{
			if (GetCardLogicValue(first.cbResultCard[0]) > GetCardLogicValue(second.cbResultCard[0]))
			{
				return true;
			}

		}
		return false; });
	}
	/////////////////////////////////////////////////////���̰߳汾/////////////////////////////////////
#else
	vector<vector<vector<tagOutCardResultNew> >> resultAllVec(4);
	multithreadSort(vecHandCardCount, cbHandCardCount,  resultVec, resultAllVec,cbHandCardData,cbHandCardCount);
	int MinTypeCount = INT_MAX;
	int index_x = -1;
	int index_y = -1;
	for (int j = 0; j < 4;j++)
	{
		for (int i = 0; i < resultAllVec[j].size(); i++)
		{
			float tempScore = GetHandScore(resultAllVec[j].at(i), MinTypeCount);
			if (resultAllVec[j].at(i).size() < MinTypeCount || ((resultAllVec[j].at(i).size() == MinTypeCount && tempScore > MinTypeScore)))
			{
				MinTypeScore = GetHandScore(resultAllVec[j].at(i), MinTypeCount);
				index_x = j;
				index_y = i;
				MinTypeCount = resultAllVec[j].at(i).size();
			}
			if (cbHandCardCount < 18 && bSearchBiYing == true && isBiYing(resultAllVec[j][i]))
			{
				if (bBiYing != true || (bBiYing && resultAllVec[j][i].size() <= MinTypeCount && tempScore>MinTypeScore))
				{
					bBiYing = true;
					MinTypeScore = tempScore;
					index_x = j;
					index_y = i;
					MinTypeCount = resultAllVec[j][i].size();
				}

			}

		}
	}
	if (index_x != -1 && index_y!=-1)
	{
		
		CardResult = resultAllVec[index_x][index_y];
		sort(CardResult.begin(), CardResult.end(), [this](tagOutCardResultNew & first, tagOutCardResultNew& second)
		{ 
			if (first.cbCardType > second.cbCardType)
			{
				return true;
			}
			if (first.cbCardType == second.cbCardType)
			{
				if (GetCardLogicValue(first.cbResultCard[0]) > GetCardLogicValue(second.cbResultCard[0]))
				{
					return true;
				}

			}
			return false;
		});
	}
#endif // SDK_CONDITION
	/////////////////////////////////////////////////////���̰߳汾/////////////////////////////////////
	return MinTypeCount;

	return 0;
}



void CGameLogicNew::clearSingleReResult(tagOutCardTypeResultNew * CardTypeResult)
{
	vector<BYTE> tempSingleCard;
	BYTE sinlgeCount = 0;
	for (BYTE i = 0; i < CardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
	{
		for (BYTE j = 0; j < CardTypeResult[CT_SINGLE].cbEachHandCardCount[i]; j++)
		{
			tempSingleCard.push_back(CardTypeResult[CT_SINGLE].cbCardData[i][0]);
		}
	}

	sort(tempSingleCard.begin(), tempSingleCard.end(), [](BYTE first, BYTE second)  {
		return first > second;
	});  // ����

	int ArrayIndex = unique(tempSingleCard.begin(), tempSingleCard.end())-tempSingleCard.begin() ;   // ȥ��
	ZeroMemory(&CardTypeResult[CT_SINGLE], sizeof(CardTypeResult[CT_SINGLE]));
	for (int i = 0; i < ArrayIndex; i++)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = tempSingleCard[i];
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++;

	}
}


void CGameLogicNew::FillCombinatory(vector<tagOutCardResultNew> &tempVecSearch, BYTE * cbTmpHReminCardData, BYTE const cbHandCardCount, vector<vector<tagOutCardResultNew> > &resultAllVec)
{
	bool bSuccess = true;
	for (int j = 0; j < tempVecSearch.size(); j++)
	{
		if (RemoveCard(tempVecSearch.at(j).cbResultCard, tempVecSearch.at(j).cbCardCount, cbTmpHReminCardData, cbHandCardCount) == false)
		{
			bSuccess = false;
			break;
		}
	}
	if (bSuccess)
	{
		resultAllVec.push_back(tempVecSearch);
	}
}


VOID CGameLogicNew::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//��ȡ����
	BYTE cbCardType = GetCardType(cbCardData, cbCardCount);

	if (cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO)
	{
		//������
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[2] * 3;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[2], sizeof(BYTE)*cbCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 2) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}
	else if (cbCardType == CT_FOUR_TAKE_ONE || cbCardType == CT_FOUR_TAKE_TWO)
	{
		//������
		tagAnalyseResultNew AnalyseResult = {};
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		cbCardCount = AnalyseResult.cbBlockCount[3] * 4;
		CopyMemory(cbCardData, AnalyseResult.cbCardData[3], sizeof(BYTE)*cbCardCount);
		for (int i = CountArray(AnalyseResult.cbBlockCount) - 1; i >= 0; i--)
		{
			if (i == 3) continue;

			if (AnalyseResult.cbBlockCount[i] > 0)
			{
				CopyMemory(&cbCardData[cbCardCount], AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i + 1)*AnalyseResult.cbBlockCount[i]);
				cbCardCount += (i + 1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}

	return;
}

BYTE CGameLogicNew::SearchSameCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//�����˿�
	tagAnalyseResultNew AnalyseResult = {};
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	BYTE cbReferLogicValue = cbReferCard == 0 ? 0 : GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount - 1;
	do
	{
		for (BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++)
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex] - i - 1)*(cbBlockIndex + 1);
			if (GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue)
			{
				if (pSearchCardResult == NULL) return 1;

				ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

				//�����˿�
				CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], &AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE));
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	} while (cbBlockIndex < CountArray(AnalyseResult.cbBlockCount));

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

BYTE CGameLogicNew::SearchTakeCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//Ч��
	ASSERT(cbSameCount == 3 || cbSameCount == 4);
	ASSERT(cbTakeCardCount == 1 || cbTakeCardCount == 2);
	if (cbSameCount != 3 && cbSameCount != 4)
		return cbResultCount;
	if (cbTakeCardCount != 1 && cbTakeCardCount != 2)
		return cbResultCount;

	//�����ж�
	if (cbSameCount == 4 && cbHandCardCount < cbSameCount + cbTakeCardCount * 2 ||
		cbHandCardCount < cbSameCount + cbTakeCardCount)
		return cbResultCount;

	//�����˿�
	BYTE cbCardData[FULL_COUNT];
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard(cbCardData, cbCardCount, cbReferCard, cbSameCount, &SameCardResult);

	if (cbSameCardResultCount > 0)
	{
		//�����˿�
		tagAnalyseResultNew AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//��Ҫ����
		BYTE cbNeedCount = cbSameCount + cbTakeCardCount;
		if (cbSameCount == 4) cbNeedCount += cbTakeCardCount;
		int startValue = cbTakeCardCount - 1;
		if (cbSameCount==3&&cbTakeCardCount == 2 && G_THREE_TAKE_TWO_DAN)
		{
			startValue = 0;
		}
				
		//��ȡ����
		for (BYTE i = 0; i < cbSameCardResultCount; i++)
		{
			bool bMerge = false;

			for (BYTE j = startValue; j < CountArray(AnalyseResult.cbBlockCount); j++)
			{
				for (BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++)
				{
					//��С����
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j] - k - 1)*(j + 1);

					//������ͬ��
					if (GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]))
						continue;
					if (cbHandCardCount<=MAX_COUNT)
					{
						if ((cbTakeCardCount == 1 && (cbSameCount == 3 || cbSameCount == 4)) || (G_THREE_TAKE_TWO_DAN&&cbTakeCardCount == 2 && cbSameCount==3))
						{
							//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
							BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
							CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

							BYTE cbHandThreeCard[FULL_COUNT] = { 0 };
							BYTE cbHandThreeCount = 0;

							//ȥ������
							BYTE cbRemainCardData[FULL_COUNT];
							CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
							BYTE cbRemainCardCount = cbHandCardCount - SameCardResult.cbCardCount[i];
							RemoveCard(SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i], cbRemainCardData, cbHandCardCount);
							if (cbSameCount == 4 || (G_THREE_TAKE_TWO_DAN&&cbTakeCardCount == 2 && cbSameCount == 3))
							{
								//�������
								BYTE cbComCard[8];
								BYTE cbComResCard[MAX_RESULT_COUNT][8];
								int cbComResLen = 0;
								BYTE cbSingleCardCount = 2;
								//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
								Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
								for (BYTE m = 0; m < cbComResLen; m++)
								{
									//���ƴ���
									CopyMemory(&SameCardResult.cbResultCard[i][cbSameCount], cbComResCard[m],
										sizeof(BYTE)*(cbNeedCount - cbSameCount));
									SameCardResult.cbCardCount[i] = cbNeedCount;
									//���ƽ��
									CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
										sizeof(BYTE)*SameCardResult.cbCardCount[i]);
									pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
									cbResultCount++;
								}
							}
							else
							{
								for (int m = 0; m < cbRemainCardCount; m += cbTakeCardCount)
								{
									//���ƴ���
									CopyMemory(&SameCardResult.cbResultCard[i][cbSameCount], &cbRemainCardData[m],
										sizeof(BYTE)*(cbNeedCount - cbSameCount));
									SameCardResult.cbCardCount[i] = cbNeedCount;
									//���ƽ��
									CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
										sizeof(BYTE)*SameCardResult.cbCardCount[i]);
									pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
									cbResultCount++;
								}
							}

							bMerge = true;

							//��һ���
							break;
						}
						else if (cbTakeCardCount == 2 && cbSameCount == 3)
						{
							//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
							BYTE  cbTmpCardData[FULL_COUNT] = { 0 };
							CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount);

							BYTE cbHandThreeCard[FULL_COUNT] = { 0 };
							BYTE cbHandThreeCount = 0;

							//ȥ������
							BYTE cbRemainCardData[FULL_COUNT];
							CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount);
							BYTE cbRemainCardCount = cbHandCardCount - SameCardResult.cbCardCount[i];
							RemoveCard(SameCardResult.cbResultCard[i], SameCardResult.cbCardCount[i], cbRemainCardData, cbHandCardCount);
							tagSearchCardResult tmpSearchCardResult = {};
						
							int	cbTmpCount = SearchSameCard(cbRemainCardData, cbRemainCardCount, 0, 2, &tmpSearchCardResult);

							for (int m = 0; m < tmpSearchCardResult.cbSearchCount; m++)
							{
								//���ƴ���
								CopyMemory(&SameCardResult.cbResultCard[i][3], tmpSearchCardResult.cbResultCard[m],
									sizeof(BYTE)*cbTakeCardCount);
								SameCardResult.cbCardCount[i] = cbSameCount + 2;
								//���ƽ��
								CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
									sizeof(BYTE)*SameCardResult.cbCardCount[i]);
								pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
								cbResultCount++;
							}
							bMerge = true;

							//��һ���
							break;
						}
					}
					else
					{
						//���ƴ���
						BYTE cbCount = SameCardResult.cbCardCount[i];
						CopyMemory(&SameCardResult.cbResultCard[i][cbCount], &AnalyseResult.cbCardData[j][cbIndex],
							sizeof(BYTE)*cbTakeCardCount);
						SameCardResult.cbCardCount[i] += cbTakeCardCount;

						if (SameCardResult.cbCardCount[i] < cbNeedCount) continue;

						if (pSearchCardResult == NULL) return 1;

						//���ƽ��
						CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], SameCardResult.cbResultCard[i],
							sizeof(BYTE)*SameCardResult.cbCardCount[i]);
						pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
						cbResultCount++;

						bMerge = true;

						//��һ���
						break;
					}
				
				}

				if (bMerge) break;
			}
		}
	}

	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

BYTE CGameLogicNew::SearchThreeTwoLine(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));

	//��������
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//��������
	cbTmpResultCount = SearchLineCardType(cbHandCardData, cbHandCardCount, 0, 3, 0, &tmpSearchResult);

	if (cbTmpResultCount > 0)
	{
		//��ȡ����
		for (BYTE i = 0; i < cbTmpResultCount; i++)
		{
			//��������
			BYTE cbTmpCardData[FULL_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//������
			if (cbHandCardCount - tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i] / 3)
			{
				BYTE cbNeedDelCount = 3;
				while (cbHandCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3)
					cbNeedDelCount += 3;
				//��������
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if (pSearchCardResult == NULL) return 1;

			//ɾ������
			CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
				cbTmpCardData, cbTmpCardCount));
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//��Ϸɻ�
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i] / 3;
			ASSERT(cbNeedCount <= cbTmpCardCount);

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory(tmpSingleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
			CopyMemory(&tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount - cbNeedCount], sizeof(BYTE)*cbNeedCount);
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbNeedCount;

			//���������
			if (cbTmpCardCount < tmpSearchResult.cbCardCount[i] / 3 * 2)
			{
				BYTE cbNeedDelCount = 3;
				while (cbTmpCardCount + cbNeedDelCount - tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 * 2)
					cbNeedDelCount += 3;
				//��������
				if ((tmpSearchResult.cbCardCount[i] - cbNeedDelCount) / 3 < 2)
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard(tmpSearchResult.cbResultCard[i], cbNeedDelCount, tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i]);
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//����ɾ������
				CopyMemory(cbTmpCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				VERIFY(RemoveCard(tmpSearchResult.cbResultCard[i], tmpSearchResult.cbCardCount[i],
					cbTmpCardData, cbTmpCardCount));
				cbTmpCardCount = cbHandCardCount - tmpSearchResult.cbCardCount[i];
			}

			//������
			tagAnalyseResultNew  TmpResult = {};
			AnalysebCardData(cbTmpCardData, cbTmpCardCount, TmpResult);

			//��ȡ���
			BYTE cbDistillCard[FULL_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i] / 3;
			for (BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++)
			{
				if (TmpResult.cbBlockCount[j] > 0)
				{
					if (j + 1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount)
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory(cbDistillCard, &TmpResult.cbCardData[j][(cbTmpBlockCount - cbLineCount)*(j + 1)],
							sizeof(BYTE)*(j + 1)*cbLineCount);
						cbDistillCount = (j + 1)*cbLineCount;
						break;
					}
					else
					{
						for (BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++)
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory(&cbDistillCard[cbDistillCount], &TmpResult.cbCardData[j][(cbTmpBlockCount - k - 1)*(j + 1)],
								sizeof(BYTE) * 2);
							cbDistillCount += 2;

							//��ȡ���
							if (cbDistillCount == 2 * cbLineCount) break;
						}
					}
				}

				//��ȡ���
				if (cbDistillCount == 2 * cbLineCount) break;
			}

			//��ȡ���
			if (cbDistillCount == 2 * cbLineCount)
			{
				//���Ƴ��
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory(tmpDoubleWing.cbResultCard[cbResultCount], tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i]);
				CopyMemory(&tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard, sizeof(BYTE)*cbDistillCount);
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i] + cbDistillCount;
			}
		}

		//���ƽ��
		for (BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for (BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++)
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory(pSearchCardResult->cbResultCard[cbResultCount], tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i]);
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult == NULL ? 0 : pSearchCardResult->cbSearchCount;
}

int CGameLogicNew::YouXianDaNengShouHuiCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult, bool &bZhiJieChu)
{
	int typeCount[CT_TYPE_COUNT] = { 0 };
	int xiangDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int juDuiMaxIndex[CT_TYPE_COUNT] = { 0 };
	int NoXiangDuiDaPaiMaxCount = 0;
	vector<vector<int>> MaxIndexSet(CT_TYPE_COUNT);
	vector<int> allMaxCardIndex;
	int DoubleReMaxCount = 0;
	int bombCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
		bool bExistMax = false;
		BYTE tmpTurnCard[MAX_COUNT] = { 0 };
		int tmpTurnCardCount = vecMinTypeCardResult[i].cbCardCount;
		CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
		bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
		if (cbHandCardCount >= NORMAL_COUNT)
		{
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
		}
		else{
			bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
		}
		if (type == CT_DOUBLE && bExistMax && juDuiMaxIndex[type] <= 0 && vecMinTypeCardResult.size()>2 && vecMinTypeCardResult[vecMinTypeCardResult.size()-1].cbCardType==CT_SINGLE)
		{
			//--�Ż�, ���������ҳ�������
			BYTE maxTrunCardLogic = GetCardLogicValue(tmpTurnCard[0]);
			int doubleMaxIndex =- 1;
			for (int j = vecMinTypeCardResult.size()-1; j >=0; j--)
			{
				if (vecMinTypeCardResult[j].cbCardType == CT_DOUBLE_LINE)
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
				else if ( vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO )
				{
					if (GetCardLogicValue(vecMinTypeCardResult[j].cbResultCard[0]) > maxTrunCardLogic)
					{
						ZeroMemory(tmpTurnCard, sizeof(tmpTurnCard));
						tmpTurnCard[0] = vecMinTypeCardResult[j].cbResultCard[0];
						tmpTurnCard[1] = vecMinTypeCardResult[j].cbResultCard[1];
						doubleMaxIndex = j;
						break;
					}
				}
			}
			bool bExistDoubleMax = false;
				if (cbHandCardCount >= NORMAL_COUNT)
				{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
				}
				else{
					bExistDoubleMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, bCheck);
				}
				if ((bExistDoubleMax != true) || GetCardLogicValue(tmpTurnCard[0])==13)
				{
					DoubleReMaxCount++;
					{
						xiangDuiMaxIndex[type]++;
					}
					if (MaxIndexSet[type].size() == 0)
					{
						MaxIndexSet[type].push_back(doubleMaxIndex);
						for (int j = 0; j < vecMinTypeCardResult.size(); j++)
						{
							if (doubleMaxIndex != j && type == vecMinTypeCardResult[j].cbCardType)
							{
								MaxIndexSet[type].push_back(j);
							}
						}
					}
				}
		}
		else if (type == CT_SINGLE && bExistMax && juDuiMaxIndex[type] <= 0 && vecMinTypeCardResult.size() > 2 && vecMinTypeCardResult[vecMinTypeCardResult.size() - 1].cbCardType == CT_SINGLE)
		{
			//--�Ż�, ���������ҳ�������
			BYTE maxTrunCardLogic = GetCardLogicValue(tmpTurnCard[0]);
			int SingleMaxIndex = -1;
			for (int j = vecMinTypeCardResult.size() - 1; j >= 0; j--)
			{
				if (vecMinTypeCardResult[j].cbCardType == CT_SINGLE_LINE)
				{
					SingleMaxIndex = j;
					for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount-3;k++)
					{
						bool bExistSingleMax = SearchOtherHandCardThan(vecMinTypeCardResult[j].cbResultCard, 1, true);

						if (bExistSingleMax == false)
						{
							if (SearchOtherHandCardSame(vecMinTypeCardResult[j].cbResultCard, 1, type) == false)
							{
								juDuiMaxIndex[type]++;
								if (MaxIndexSet[type].size() == 0)
								{
									MaxIndexSet[type].push_back(SingleMaxIndex);
									for (int j = 0; j < vecMinTypeCardResult.size(); j++)
									{
										if (SingleMaxIndex != j && type == vecMinTypeCardResult[j].cbCardType)
										{
											MaxIndexSet[type].push_back(j);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (bExistMax != true)
		{
			
			allMaxCardIndex.push_back(i);
			if (SearchOtherHandCardSame(tmpTurnCard, tmpTurnCardCount, type) == false)
			{
				juDuiMaxIndex[type]++;
			}
			else
			{
				xiangDuiMaxIndex[type]++;
			}
			if (MaxIndexSet[type].size() == 0)
			{
				MaxIndexSet[type].push_back(i);
				for (int j = 0; j < vecMinTypeCardResult.size(); j++)
				{
					if (i != j && type == vecMinTypeCardResult[j].cbCardType)
					{
						MaxIndexSet[type].push_back(j);
					}
				}
			}
		}
		else
		{
			if (vecMinTypeCardResult[i].cbCardCount>NoXiangDuiDaPaiMaxCount)
			{
				NoXiangDuiDaPaiMaxCount = vecMinTypeCardResult[i].cbCardCount;
			}
		}

	/*	if (type >= CT_BOMB_CARD)
		{
			bombCount++;
		}*/
	}
	int  maxTypeCount = 0;
	int   resultIndex = -1;
	for (int i = 0; i < CT_TYPE_COUNT; i++)
	{
		maxTypeCount += (juDuiMaxIndex[i] + xiangDuiMaxIndex[i]);
	}
	maxTypeCount -= DoubleReMaxCount;
	if ((maxTypeCount + bombCount) >= vecMinTypeCardResult.size() - 1)
	{
		bZhiJieChu = true;
	}
	if (bZhiJieChu==false)
	{
		int tempAllCount = 0;
		for (int i = 0; i < allMaxCardIndex.size(); i++)
		{
			tempAllCount += vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
		}
		if ((tempAllCount + NoXiangDuiDaPaiMaxCount) >= cbHandCardCount - m_cbOthreRangCardCount)
		{
			bZhiJieChu = true;
		}
	}
	if (bZhiJieChu)
	{
		if (allMaxCardIndex.size() > 0)
		{
			int maxTypeCount = 0;
			resultIndex = allMaxCardIndex[allMaxCardIndex.size() - 1];
			for (int i = 0; i < allMaxCardIndex.size(); i++)
			{
				if ((vecMinTypeCardResult[allMaxCardIndex[i]].cbCardType != CT_MISSILE_CARD) && vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount>maxTypeCount)
				{
					maxTypeCount = vecMinTypeCardResult[allMaxCardIndex[i]].cbCardCount;
					resultIndex = allMaxCardIndex[i];
				}
			}
		}
	}
	if (resultIndex == -1)
	{
		int tempTypeCount = 0;
		for (int i = 0; i < CT_TYPE_COUNT; i++)
		{
			int tempSize = MaxIndexSet[i].size();
			if (juDuiMaxIndex[i] > 0 && typeCount[i] > 1 && tempSize > 0)
			{
				resultIndex = MaxIndexSet[i][tempSize - 1];
				tempTypeCount = typeCount[i];
				break;
			}
		}
		//
		if (resultIndex==-1)
		{
			for (int i = 0; i < CT_TYPE_COUNT; i++)
			{
				int tempSize = MaxIndexSet[i].size();
				if (xiangDuiMaxIndex[i] > 0 && typeCount[i] > 1 && tempSize > 0)
				{
					resultIndex = MaxIndexSet[i][tempSize - 1];
					break;
				}

			}
		}
		else
		{   	//���Դ���ֻ������,����Դ���������ʱ,��Դ��Ƶ��ȳ�
			if (tempTypeCount<=2)
			{
				for (int i = 0; i < CT_TYPE_COUNT; i++)
				{
					int tempSize = MaxIndexSet[i].size();
					if (xiangDuiMaxIndex[i] > 0 && typeCount[i] > 1 && typeCount[i] > tempTypeCount && tempSize > 0)
					{
						resultIndex = MaxIndexSet[i][tempSize - 1];
						break;
					}

				}
			}
		}
	
	}
	 if (cbHandCardCount == 10 && resultIndex != -1 && typeCount[CT_SINGLE] == 2 && vecMinTypeCardResult[0].cbCardType!=CT_BOMB_CARD&&(vecMinTypeCardResult[0].cbCardCount + vecMinTypeCardResult[1].cbCardCount) >= 8)
	{
		 if (vecMinTypeCardResult[0].cbCardType == vecMinTypeCardResult[1].cbCardType)
		 {
			 resultIndex = 1;
		 }
		 else{
			 resultIndex = 0;
		 }
	}
	if (resultIndex!=-1)
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[resultIndex].cbCardCount;
		OutCardResult.cbCardType = vecMinTypeCardResult[resultIndex].cbCardType;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[resultIndex].cbResultCard, OutCardResult.cbCardCount);
	}
	return resultIndex;
}

int CGameLogicNew::SearchMutilType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew>& vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	int MaxTypeCount = 0;
	int resultIndex = -1;
	int resultIndexType = -1;
	int cardIndexCount = -1;
	int maxCard = INT_MAX;
	int typeCount[CT_TYPE_COUNT] = { 0 };
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		typeCount[type]++;
	}
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		int type = vecMinTypeCardResult[i].cbCardType;
		bool bSpecal = false;
		if ((type == CT_FOUR_TAKE_ONE || type == CT_FOUR_TAKE_TWO)&&vecMinTypeCardResult.size()<=2)
		{
			bSpecal = true;
		}
		if ((type>CT_SINGLE && type <= CT_THREE_TAKE_TWO) || bSpecal)
		{
			if (vecMinTypeCardResult[i].cbCardCount > cardIndexCount && !((vecMinTypeCardResult[i].cbCardType == CT_THREE || vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_ONE || vecMinTypeCardResult[i].cbCardType == CT_THREE_TAKE_TWO) && GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) >= 12))
		    {
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
		    }
			else if (vecMinTypeCardResult[i].cbCardCount == cardIndexCount && typeCount[type] > MaxTypeCount)
			{
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
			else if
				(vecMinTypeCardResult[i].cbCardCount == cardIndexCount && typeCount[type] == MaxTypeCount &&
				GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < maxCard
				)
			{
				if (type == CT_SINGLE_LINE&&vecMinTypeCardResult.size()==2)
				{
					continue;
				}
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
		
			else if (
				resultIndexType == CT_SINGLE_LINE && vecMinTypeCardResult[i].cbCardType == CT_SINGLE_LINE &&
				vecMinTypeCardResult[i].cbCardCount <= vecMinTypeCardResult[resultIndex].cbCardCount
				)
			{
				cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
				resultIndex = i;
				MaxTypeCount = typeCount[type];
				resultIndexType = vecMinTypeCardResult[i].cbCardType;
				maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			}
						 
		}
	}
	if (resultIndex==-1)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			int type = vecMinTypeCardResult[i].cbCardType;
			if (type== CT_SINGLE )
			{
				if
					(
					GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < maxCard
					)
				{
					cardIndexCount = vecMinTypeCardResult[i].cbCardCount;
					resultIndex = i;
					MaxTypeCount = typeCount[type];
					resultIndexType = vecMinTypeCardResult[i].cbCardType;
					maxCard = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
				}
			}
		}
	}
	if (resultIndex != -1)
	{
		
			OutCardResult.cbCardCount = vecMinTypeCardResult[resultIndex].cbCardCount;
			CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[resultIndex].cbResultCard, OutCardResult.cbCardCount);
			int tmpType = vecMinTypeCardResult[resultIndex].cbCardType;
			bool bExist = false;
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				int type = vecMinTypeCardResult[i].cbCardType;
				if (typeCount[type] >= 1 && (tmpType >= CT_THREE_TAKE_ONE && tmpType <= CT_THREE_TAKE_TWO && type >= CT_THREE_TAKE_ONE && type <= CT_THREE_TAKE_TWO))
				{
					if (GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) < GetCardLogicValue(OutCardResult.cbResultCard[0]))
					{
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
						OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
						CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
						bExist = true;
					}
				}

			}
			if (bExist == false && (tmpType == CT_THREE_TAKE_TWO || tmpType == CT_THREE_TAKE_ONE || tmpType == CT_DOUBLE))
			{
				if ((cbHandCardCount - m_cbOthreRangCardCount - OutCardResult.cbCardCount) <= 1)
				{
					bExist = true;
				}
			}
			if (bExist == false && (tmpType == CT_THREE_TAKE_TWO || tmpType == CT_THREE_TAKE_ONE || tmpType == CT_DOUBLE ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1)
		   {
				
				for (int i = 0; i < CT_TYPE_COUNT; i++)
				{
					if (typeCount[i]>1)
					{
						for (int j = vecMinTypeCardResult.size()-1; j >=0; j--)
						{
							if (vecMinTypeCardResult[j].cbCardType==i)
							{
								ZeroMemory(&OutCardResult, sizeof(OutCardResult));
								OutCardResult.cbCardCount = vecMinTypeCardResult[j].cbCardCount;
								CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[j].cbResultCard, OutCardResult.cbCardCount);
								bExist = true;
								break;
							}
						}
						if (bExist)
						{
							break;
						}
					}

				}
		   }
			if (bExist == false && (tmpType == CT_DOUBLE_LINE) && (typeCount[CT_BOMB_CARD]>0 || GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) ==14 ) && GetCardLogicValue(vecMinTypeCardResult[resultIndex].cbResultCard[0]) >= 11 && typeCount[tmpType] == 1)
			{
				if (m_cbUserCardCount[1] >4)
				{
					for (int i = 0; i < CT_TYPE_COUNT; i++)
					{
						if (typeCount[i] > 1)
						{
							for (int j = vecMinTypeCardResult.size() - 1; j >= 0; j--)
							{
								if (vecMinTypeCardResult[j].cbCardType == i)
								{
									ZeroMemory(&OutCardResult, sizeof(OutCardResult));
									OutCardResult.cbCardCount = vecMinTypeCardResult[j].cbCardCount;
									CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[j].cbResultCard, OutCardResult.cbCardCount);
									bExist = true;
									break;
								}
							}
							if (bExist)
							{
								break;
							}
						}
					}
				}
				
			}
			if (bExist == false && (tmpType == CT_SINGLE_LINE) && OutCardResult.cbCardCount>5&&(cbHandCardCount<NORMAL_COUNT))
			{
					BYTE tmpTurnCard[MAX_COUNT] = { 0 };
				int tmpTurnCardCount = 1;
				CopyMemory(tmpTurnCard, &OutCardResult.cbResultCard[0],1);
				bool	bExistMax = SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, true);
				if (bExistMax==false)
				{
					OutCardResult.cbResultCard[0] = OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1];
					OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1] = 0;
					OutCardResult.cbCardCount -= 1;
					bExist = true;
					SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
				}
				
			}
	}
	return resultIndex;
}

bool CGameLogicNew::isBiYing(vector<tagOutCardResultNew > & CardTypeResult)
{
	//--���ȴ����ջ�������
	bool bExistJuiMax = true;
	int xiangDuiDaPaiCount = 0;
	for (int i = 0; i < CardTypeResult.size(); i++)
	{
		int type = GetCardType(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount);
		if (type != 0)
		{
			bool bExistMax = false;
			bool bExist = true;
			bExistMax = SearchOtherHandCardThan(CardTypeResult[i].cbResultCard, CardTypeResult[i].cbCardCount, false);
			if (bExistMax != true)
			{
				xiangDuiDaPaiCount = xiangDuiDaPaiCount + 1;
			}
		}
	}

	if (xiangDuiDaPaiCount > 0 && xiangDuiDaPaiCount >= (CardTypeResult.size() - 1))
		return true;

	return false;
}

//�����˿�
BYTE CGameLogicNew::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//�����˿�
VOID CGameLogicNew::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResultNew & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//��������
		i+=cbSameCount-1;
	}

	return;
}

//�����ֲ�
VOID CGameLogicNew::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributingNew & Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing,sizeof(Distributing));

	//���ñ���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//��ȡ����
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////����ΪAI����

//��������
bool CGameLogicNew::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResultNew & OutCardResult)
{
	//����ж�
	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//�����¼�
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//�����ϼ�
	m_wMeChairID = wMeChairID;
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//�ȳ���
	if(cbTurnCardCount==0)
	{
		//��������
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, OutCardResult) ;
		//�����¼�
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount,wMeChairID,  OutCardResult) ;
		//�����ϼ�
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, OutCardResult) ;
		//���� ID
		else ASSERT(false) ;
	}
	//ѹ��
	else
	{	
		//��������
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//�����¼�
		else if (wMeChairID == wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult);
		//�����ϼ�
		else if (wMeChairID == wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult);
		//���� ID
		else ASSERT(false) ;

	}
	return true ;
}

//����ը��
VOID CGameLogicNew::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagOutCardTypeResultNew   *CardTypeResult )
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<4) return ;

	//˫��ը��
	//if(0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
	//{
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
	//	cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	//}

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
			if (CardTypeResult != NULL)
			{
				int Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
				CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], &cbTmpCardData[i], sizeof(BYTE)*cbSameCount);
				CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbSameCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
			}
		}

		//��������
		i+=cbSameCount-1;
	}
}

//����˳��
VOID CGameLogicNew::GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCard[MAX_COUNT] ;
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	//��С����
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;

	cbLineCardCount = 0 ;

	//����У��
	if(cbHandCardCount<5) return ;

	BYTE cbFirstCard = 0 ;
	//ȥ��2����
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

	BYTE cbSingleLineCard[12] ;
	BYTE cbSingleLineCount=0 ;
	BYTE cbLeftCardCount = cbHandCardCount ;
	bool bFindSingleLine = true ;

	//�����ж�
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount=1 ;
		bFindSingleLine = false ;
		BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
		cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTmpCard[i];

			//�����ж�
			if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTmpCard[i] ;
				if(cbSingleLineCount<5) 
				{
					cbSingleLineCount = 1 ;
					cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
					continue ;
				}
				else break ;
			}
			//ͬ���ж�
			else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData ;
				cbSingleLineCard[cbSingleLineCount] = cbCardData ;
				++cbSingleLineCount ;
			}					
		}

		//��������
		if(cbSingleLineCount>=5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
			memcpy(cbLineCardData+cbLineCardCount , cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount) ;
			cbLineCardCount += cbSingleLineCount ;
			cbLeftCardCount -= cbSingleLineCount ;
			bFindSingleLine = true ;
			if (CardTypeResult!=NULL)
			{
				int Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
				CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount);
				CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbSingleLineCount;
				CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
			}
			
		}
	}
}

//��������
VOID CGameLogicNew::GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbThreeCardCount = 0 ;

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount>=3)
		{
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+1] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+2] ;	
		}

		//��������
		i+=cbSameCount-1;
	}
}

//��������
VOID CGameLogicNew::GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbDoubleCardCount = 0 ;

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount>=2)
		{
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i] ;
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i+1] ;
		}

		//��������
		i+=cbSameCount-1;
	}
}

//��������
VOID CGameLogicNew::GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, tagOutCardTypeResultNew   *CardTypeResult)
{
	cbSingleCardCount =0 ;

	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount==1)
		{
			cbSingleCardData[cbSingleCardCount++] = cbTmpCardData[i] ;
		}

		//��������
		i+=cbSameCount-1;
	}
}




/********************************************************************
��������Combination
������
cbCombineCardData���洢��������Ͻ��
cbResComLen���ѵõ�����ϳ��ȣ���ʼ����ʱ�˲���Ϊ0
cbResultCardData���洢���е���Ͻ��
cbResCardLen��cbResultCardData�ĵ�һ�±�ĳ��ȣ���Ͻ���ĸ���
cbSrcCardData����Ҫ����ϵ�����
cbSrcLen��cbSrcCardData��������Ŀ
cbCombineLen2��cbCombineLen1����ϵĳ��ȣ���ʼ����ʱ������ȡ�
*********************************************************************/
//����㷨
VOID CGameLogicNew::Combination(BYTE cbCombineCardData[], int cbResComLen,  BYTE cbResultCardData[500][8], int &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2)
{

	if( cbResComLen == cbCombineLen2 )
	{
		CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen) ;
		++cbResCardLen ;

		ASSERT(cbResCardLen<255) ;

	}
	else
	{ 
		if(cbCombineLen1 >= 1 && cbSrcLen > 0 && (cbSrcLen+1) >= 0 ){
			cbCombineCardData[cbCombineLen2-cbCombineLen1] =  cbSrcCardData[0];
			++cbResComLen ;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1-1, cbSrcLen-1, cbCombineLen2);

			--cbResComLen;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1, cbSrcLen-1, cbCombineLen2);
		}
	}
}

//�����˿�
VOID CGameLogicNew::SetUserCard(WORD wChairID,const BYTE cbCardData[], BYTE cbCardCount)
{
	ZeroMemory(m_cbAllCardData[wChairID], cbCardCount);
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//���õ���
VOID CGameLogicNew::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//����ׯ��
VOID CGameLogicNew::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
}
//ɾ���˿�
VOID CGameLogicNew::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

BYTE CGameLogicNew::SearchLineCardType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount, tagSearchCardResult *pSearchCardResult)
{
	//���ý��
	if (pSearchCardResult)
		ZeroMemory(pSearchCardResult, sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�������
	BYTE cbLessLineCount = 0;
	if (cbLineCount == 0)
	{
		if (cbBlockCount == 1)
			cbLessLineCount = 5;
		else if (cbBlockCount == 2)
			cbLessLineCount = 2;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if (cbReferCard != 0)
	{
		ASSERT(GetCardLogicValue(cbReferCard) - cbLessLineCount >= 2);
		cbReferIndex = GetCardLogicValue(cbReferCard) - cbLessLineCount + 1;
	}
	//����A
	if (cbReferIndex + cbLessLineCount > 14) return cbResultCount;

	//�����ж�
	if (cbHandCardCount < cbLessLineCount*cbBlockCount) return cbResultCount;

	//�����˿�
	BYTE cbCardData[FULL_COUNT] = { 0 };
	BYTE cbCardCount = cbHandCardCount;
	CopyMemory(cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData, cbCardCount, ST_ORDER);

	//�����˿�
	tagDistributingNew Distributing = {};
	AnalysebDistributing(cbCardData, cbCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	//����˳��
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	for (cbValueIndex = cbReferIndex; cbValueIndex < 14; cbValueIndex++)
	{
		//�����ж�
		if (Distributing.cbDistributing[cbValueIndex][cbIndexCount] < cbBlockCount)
		{
			if (cbTmpLinkCount < cbLessLineCount)
			{
				cbTmpLinkCount = 0;
				continue;
			}
			else cbValueIndex--;
		}
		else
		{
			cbTmpLinkCount++;
			//Ѱ�����
			if (cbLineCount == 0) continue;
		}

		if (cbTmpLinkCount >= cbLessLineCount)
		{
			if (pSearchCardResult == NULL) return 1;

			ASSERT(cbResultCount < CountArray(pSearchCardResult->cbCardCount));

			//�����˿�
			BYTE cbCount = 0;
			for (BYTE cbIndex = cbValueIndex + 1 - cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++)
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex = 0; cbColorIndex < 4; cbColorIndex++)
				{
					for (BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3 - cbColorIndex]; cbColorCount++)
					{
						if (cbIndex==13)
						{
							pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(0, 3 - cbColorIndex);
						}
						else{
							pSearchCardResult->cbResultCard[cbResultCount][cbCount++] = MakeCardData(cbIndex, 3 - cbColorIndex);
						}

						if (++cbTmpCount == cbBlockCount) break;
					}
					if (cbTmpCount == cbBlockCount) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if (cbLineCount != 0)
			{
				cbTmpLinkCount--;
			}
			else
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	
	for (int i = 0; i < cbResultCount;i++)
	{
		SortCardList(pSearchCardResult->cbResultCard[i], pSearchCardResult->cbCardCount[i], ST_ORDER);
	}
	if (pSearchCardResult)
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}


bool CGameLogicNew::OutCardShengYuFenCheck(BYTE cbHandCardCount, const BYTE * cbHandCardData, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, float MinTypeScoreVec, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	tagSearchCardResult SearchCardResult;
	SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, &SearchCardResult);
	int minTypeCount = 0;
	int resultIndex = -1;
	float MinTypeScore = INT_MIN;
	vector<tagOutCardResultNew>   vecMinTypeCardResultBak;
	float tableScore[300] = { 0 };
	bool bExistBiYing = false;
	vector<vector<tagOutCardResultNew>>  TempMinTypeCardResult(SearchCardResult.cbSearchCount);
	vector<BYTE> singleData;
	vector<BYTE> doubleData;
	int bombCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size();i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			singleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			doubleData.push_back(vecMinTypeCardResult[i].cbResultCard[1]);
		}
	}
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{

		BYTE tempType = GetCardType(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i]);
		if (tempType >= CT_BOMB_CARD)
		{
			bombCount++;
		}
		int   tempCardCount = SearchCardResult.cbCardCount[i];
		bool bExistMax = SearchOtherHandCardThan(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], false);
		BYTE cbReserveCardData[MAX_COUNT] = { 0 };
		BYTE cbReserveCardCount = cbHandCardCount;
		CopyMemory(cbReserveCardData, cbHandCardData, cbReserveCardCount);
		RemoveCard(
			SearchCardResult.cbResultCard[i],
			tempCardCount,
			cbReserveCardData,
			cbReserveCardCount
			);
		cbReserveCardCount = cbReserveCardCount - tempCardCount;
	
		tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
		ZeroMemory(tmpCardTypeResult, sizeof(tmpCardTypeResult));

		if (cbReserveCardCount > 0)
		{
			int tempMinTypeCount = 0;
			float tempMinTypeScore = 0;
			bool biYing = false;
			tempMinTypeCount = FindCardKindMinNum(cbReserveCardData, cbReserveCardCount, tmpCardTypeResult, TempMinTypeCardResult[i], tempMinTypeScore, biYing, true);
			tableScore[i] = tempMinTypeScore;
			if (tempMinTypeScore > MinTypeScore)
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
			}

			if (biYing &&  bExistMax != true)
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				bExistBiYing = biYing;
				if (tempType==CT_BOMB_CARD)
				{
					break;
				}
			}
			else if (bExistMax != true && TempMinTypeCardResult[i].size()<=3)
			{
				bool btempExistMax = true;
				bool bExistCount = false;
				for (int j = 0; j < TempMinTypeCardResult[i].size();j++)
				{
					BYTE tmpTurnCard[MAX_COUNT] = { 0 };
					int tmpTurnCardCount = TempMinTypeCardResult[i][j].cbCardCount;
					CopyMemory(tmpTurnCard, TempMinTypeCardResult[i][j].cbResultCard, TempMinTypeCardResult[i][j].cbCardCount);
					 if (SearchOtherHandCardThan(tmpTurnCard, tmpTurnCardCount, false) == false)
					{
						btempExistMax = false;
					}
					 else
					 {
						 if (TempMinTypeCardResult[i][j].cbCardCount <= 5 && NoOutCardUserHandCardCountCheck(m_wMeChairID, TempMinTypeCardResult[i][j].cbCardCount, false, false, true) == true)
						 {
							 bExistCount = true;
						 }
					 }
				}
				if (btempExistMax == false && bExistCount==false)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					vecMinTypeCardResultBak = TempMinTypeCardResult[i];
					if (tempType == CT_BOMB_CARD)
					{
						bExistBiYing = true;
						break;
					}
				}
				
			}
			else if (tempMinTypeCount == 1 && bExistMax != true)
			{
				MinTypeScore = tempMinTypeScore;
				minTypeCount = tempMinTypeCount;
				resultIndex = i;
				vecMinTypeCardResultBak = TempMinTypeCardResult[i];
				break;
			}
			else if (NoOutCardUserHandCardCountCheck(m_wMeChairID, 1))
			{

				int tempTypeCount = 0;
				int tempAllCount = 0;
				for (int j = 0; j < TempMinTypeCardResult[i].size(); j++)
				{
					if (TempMinTypeCardResult[i][j].cbCardCount >= 2)
					{
						tempAllCount += TempMinTypeCardResult[i][j].cbCardCount;
					}
				}
				if (cbHandCardCount - SearchCardResult.cbCardCount[i] - tempAllCount - m_cbOthreRangCardCount <= 1)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					vecMinTypeCardResultBak = TempMinTypeCardResult[i];
					bExistBiYing = true;
					if (tempType == CT_BOMB_CARD)
					{
						break;
					}
				}
			}
			else if (NoOutCardUserHandCardCountCheck(m_wMeChairID,3))
			{
				bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
				if (bCheck)
				{
					int tempTypeCount = 0;
					for (int j = 0; j < TempMinTypeCardResult[i].size(); j++)
					{
						if (NoOutCardUserHandCardCountCheck(m_wMeChairID, TempMinTypeCardResult[i][j].cbCardCount,false,false))
						{
							tempTypeCount++;
						}
					}
					if (tempTypeCount <= 1)
					{
						MinTypeScore = tempMinTypeScore;
						minTypeCount = tempMinTypeCount;
						resultIndex = i;
						vecMinTypeCardResultBak = TempMinTypeCardResult[i];
						bExistBiYing = true;
					}
				}
			}
		

		}

	}	
	if ((resultIndex != -1) && bExistBiYing==false)
	{
		int tempCardType= GetCardType(SearchCardResult.cbResultCard[resultIndex], SearchCardResult.cbCardCount[resultIndex]);
		float tempCardScore = 20;

		if (false)
		{
		}
		else{
			if (tempCardType >= CT_BOMB_CARD)
			{
				
			}
			else
			{
				if (tempCardType==CT_SINGLE)
				{
					bExistBiYing= DanDingPaiCeLue(tempCardType, vecMinTypeCardResultBak, SearchCardResult, resultIndex, singleData, OutCardResult, bExistBiYing);

					if (bExistBiYing == false && vecMinTypeCardResultBak.size() == 2 && vecMinTypeCardResultBak[1].cbResultCard[0] == SearchCardResult.cbResultCard[resultIndex][0])
					{
						if (vecMinTypeCardResult[0].cbCardType == CT_SINGLE_LINE)
						{
							for (int i = vecMinTypeCardResultBak[0].cbCardCount - 1; i >= 0;i--)
							{
								if (GetCardLogicValue(vecMinTypeCardResultBak[0].cbResultCard[i])>GetCardLogicValue(vecMinTypeCardResultBak[1].cbResultCard[0]))
								{
									OutCardResult.cbCardCount = 1;
									OutCardResult.cbResultCard[0] = vecMinTypeCardResultBak[0].cbResultCard[i];
									return true;
								}
								
							}
						
						}
					}
					else if (bExistBiYing == false && vecMinTypeCardResultBak.size() == 1 && vecMinTypeCardResultBak[0].cbCardCount >3 )
					{
						if (vecMinTypeCardResult[0].cbCardType == CT_SINGLE_LINE)
						{
							if (GetCardLogicValue(vecMinTypeCardResultBak[0].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
							{
								OutCardResult.cbCardCount = 1;
								OutCardResult.cbResultCard[0] = vecMinTypeCardResultBak[0].cbResultCard[0];
								return true;
							}
						}
					}
				}else if (tempCardType==CT_DOUBLE)
				{
					bExistBiYing = ShuangDingPaiCeLue(tempCardType, vecMinTypeCardResultBak, SearchCardResult, resultIndex, doubleData, OutCardResult, bExistBiYing);
				}
				if (bExistBiYing&&OutCardResult.cbCardCount>0)
				{
					return bExistBiYing;
				}
					
			}
		}
		
	}

	if (resultIndex != -1)
	{
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = SearchCardResult.cbCardCount[resultIndex];
			CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[resultIndex], OutCardResult.cbCardCount);
		}
		
	}
	return bExistBiYing;
}




bool CGameLogicNew::DanDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing)
{
	if (tempCardType == CT_SINGLE)
	{
		int count = 0;
		int index = 0;
		for (int i = 0; i < vecMinTypeCardResultBak.size(); i++)
		{
			if (vecMinTypeCardResultBak[i].cbCardType == CT_SINGLE)
			{
				count++;
				index = i;
			}
		}
		if (count == 1 && (GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]) != 14))
		{
			if (singleData.size() == 2 && GetCardValue(singleData[0]) == 2 && (GetCardLogicValue(singleData[1]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, &singleData[1], OutCardResult.cbCardCount);
				return true;
			}
			else if (GetCardLogicValue(vecMinTypeCardResultBak[index].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[index].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (count == 0 || singleData.size() == 1)
		{
			if ((singleData.size() == 1) && (GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::ShuangDingPaiCeLue(int tempCardType, vector<tagOutCardResultNew> &vecMinTypeCardResultBak, tagSearchCardResult &SearchCardResult, int resultIndex, vector<BYTE> &singleData, tagOutCardResultNew &OutCardResult, bool bExistBiYing)
{
	if (tempCardType == CT_DOUBLE)
	{
		int count = 0;
		int index = 0;
		for (int i = 0; i < vecMinTypeCardResultBak.size(); i++)
		{
			if (vecMinTypeCardResultBak[i].cbCardType == CT_DOUBLE)
			{
				count++;
				index = i;
			}
		}
		if (count > 1 && (GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]) != 14))
		{
			if (singleData.size() == 4 && GetCardValue(singleData[0]) == 1 && (GetCardLogicValue(singleData[2]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, &singleData[2], OutCardResult.cbCardCount);
				return true;
			}
			else if (GetCardLogicValue(vecMinTypeCardResultBak[index].cbResultCard[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0]))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResultBak[index].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (count == 0 || (singleData.size() >= 1 && GetCardLogicValue(singleData[0])<=11))
		{
			if ((singleData.size() >= 1) && (GetCardLogicValue(singleData[0]) > GetCardLogicValue(SearchCardResult.cbResultCard[resultIndex][0])))
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbResultCard, &singleData[0], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::DoubleChaiFenCheck(vector<tagOutCardResultNew> &TempMinTypeCardResult, const BYTE * cbHandCardData, BYTE cbHandCardCount, WORD wOutCardUser)
{
	bool bExistMax = true;
	bool bXiangDuiDa = true;
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < TempMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (TempMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = TempMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (TempMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = TempMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	bool bCheck = CheckBombPercent(cbHandCardData, cbHandCardCount, m_cbDiscardCard, m_cbDiscardCardCount);
	if (bCheck && vecDoubleData.size() == 1 && (vecDoubleData.size() * 2 >= vecSingleData.size()))
	{
		if (NoOutCardUserHandCardCountCheck(wOutCardUser, 3))
		{
			bExistMax = SearchOtherHandCardThan(&vecDoubleData[0].cbCardData, 1, true);
		}
		else
		{
			bExistMax = SearchOtherHandCardThan(&vecDoubleData[0].cbCardData, 1, false);
		}
		if (bExistMax == false)
		{
			bXiangDuiDa = SearchOtherHandCardSame(&vecDoubleData[0].cbCardData, 1, CT_SINGLE);
		}
	}
	if (bExistMax==false && bExistMax==false)
	{
		return true;
	}
	else{
		return false;
	}
}

bool CGameLogicNew::NoOutCardUserHandCardCountCheck(WORD wOutCardUser, BYTE cbCardCount, bool bXiaoYu, bool bYu, bool bOnlyDengYu)
{
	if (bXiaoYu)
	{
		if (m_wBankerUser != wOutCardUser&&m_cbUserCardCount[2] != 0)
		{
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[m_wBankerUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			else if (m_cbUserCardCount[m_wBankerUser] <= cbCardCount)
			{
				return true;
			}
			else{
				return false;
			}
		}
		else
		{
			WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
			WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

			if (m_cbUserCardCount[2] == 0)
			{
				wUpsideUser = wUndersideUser;
			}
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[wUndersideUser] == cbCardCount||m_cbUserCardCount[wUpsideUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (bYu)
			{
				if (m_cbUserCardCount[wUndersideUser] <= cbCardCount&&m_cbUserCardCount[wUpsideUser] <= cbCardCount)
				{
					return true;
				}
			}
			else
			{
				if (m_cbUserCardCount[wUndersideUser] <= cbCardCount || m_cbUserCardCount[wUpsideUser] <= cbCardCount)
				{
					return true;
				}
			}
				return false;
		}
	}
	else
	{
		if (m_wBankerUser != wOutCardUser&&m_cbUserCardCount[2] != 0)
		{
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[m_wBankerUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (m_cbUserCardCount[m_wBankerUser] >= cbCardCount)
			{
				return true;
			}
			else{
				return false;
			}
		}
		else
		{
			WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
			WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

			if (m_cbUserCardCount[2] == 0)
			{
				wUpsideUser = wUndersideUser;
			}
			if (bOnlyDengYu)
			{
				if (m_cbUserCardCount[wUndersideUser] == cbCardCount||m_cbUserCardCount[wUpsideUser] == cbCardCount)
				{
					return true;
				}
				else{
					return false;
				}
			}
			if (bYu)
			{
				if (m_cbUserCardCount[wUndersideUser] >= cbCardCount&&m_cbUserCardCount[wUpsideUser] >= cbCardCount)
				{
					return true;
				}
			}
			else
			{
				if (m_cbUserCardCount[wUndersideUser] >= cbCardCount || m_cbUserCardCount[wUpsideUser] >= cbCardCount)
				{
					return true;
				}
			}

		}
	}

	return false;
}

vector<BYTE> CGameLogicNew::SearchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult)
{
	vector<BYTE> vecResultData;
	for (int j = 0; j < vecMinTypeCardResult.size(); j++)
	{
		if (cbOutCardType == CT_SINGLE )
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_ONE)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 4; k++)
				{
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k]);
				}
			}
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					BYTE firstCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k];
					BYTE secondCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 - k];
					if (GetCardLogicValue(firstCard) != GetCardLogicValue(secondCard))
					{
						vecResultData.push_back(firstCard);
						vecResultData.push_back(secondCard);
					}
				}
			}
		}
		if (cbOutCardType == CT_DOUBLE)
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_THREE_TAKE_TWO)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					BYTE firstCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k];
					BYTE secondCard = vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 - k];
					if ( GetCardLogicValue(firstCard) == GetCardLogicValue(secondCard) )
					{
						vecResultData.push_back(firstCard);
						vecResultData.push_back(secondCard);
					}
				}
			}
		}

	}

	if (vecResultData.size()>0)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		CopyMemory(OutCardResult.cbResultCard, &vecResultData[0], OutCardResult.cbCardCount);
	}
	
	return vecResultData;
}

bool CGameLogicNew::SwitchOneOrTwoFromThreeTake(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbOutCardType, tagOutCardResultNew &OutCardResult)
{
	vector<BYTE> vecResultData;
	for (int j = 0; j < vecMinTypeCardResult.size(); j++)
	{
		if (cbOutCardType == CT_THREE_TAKE_ONE)
		{
			if (vecMinTypeCardResult[j].cbCardType == cbOutCardType)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 4; k++)
				{
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 - k]);
				}
			}

		}
		if (cbOutCardType == CT_THREE_TAKE_TWO)
		{
			if (vecMinTypeCardResult[j].cbCardType == cbOutCardType)
			{
				for (int k = 0; k < vecMinTypeCardResult[j].cbCardCount / 5; k++)
				{
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 1 -2*k]);
					vecResultData.push_back(vecMinTypeCardResult[j].cbResultCard[vecMinTypeCardResult[j].cbCardCount - 2 -2*k]);
				}
			}
		}

	}

	if (vecResultData.size() > 0)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		int count = OutCardResult.cbCardCount / 4;
		int takeCount = 1;
		if (cbOutCardType == CT_THREE_TAKE_TWO)
		{
			count = OutCardResult.cbCardCount / 5;
			takeCount = 2;
		}
		CopyMemory(OutCardResult.cbResultCard + 3 * count, &vecResultData[0], takeCount*count);
	}
	return true;
}

//�������ƣ��ȳ��ƣ�
VOID CGameLogicNew::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult)
{
	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,NULL, 0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	

	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}

	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		return;
	}
	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}

	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);

	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;

	if ((m_cbUserCardCount[wUpsideUser] == CT_SINGLE || m_cbUserCardCount[wUndersideUser] == CT_SINGLE) &&  OutCardResult.cbCardCount == 1)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_SINGLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	else if ((m_cbUserCardCount[wUpsideUser] == 2 || m_cbUserCardCount[wUndersideUser] == 2) && OutCardResult.cbCardCount==2)
	{
		for (int i = vecMinTypeCardResult.size()-1; i >=0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_DOUBLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	//���Ƴ������ڶ�С�Ĳ���
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);

	
	if (OutCardResult.cbCardCount == 0)
	{
		for (int k = 0; k < CT_TYPE_COUNT; k++)
		{
			for (BYTE i = 0; i < CardTypeResult[k].cbCardTypeCount; ++i)
			{

				OutCardResult.cbCardCount = CardTypeResult[k].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[k].cbCardData[i], CardTypeResult[k].cbEachHandCardCount[i]);
				break;
			}
		}
	}
	//����������������һ������С�ڵ���1��,��ô�������������ѹ��
	if (OutCardResult.cbCardCount == 1 && (m_cbUserCardCount[wUndersideUser] == 1))
	{
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
	}

	return;
}

//�������ƣ�����ƣ�
VOID CGameLogicNew::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}


	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}

	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}
	//�����
	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	if (cbOutCardType == CT_SINGLE && OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[wUndersideUser] <= 1 || m_cbUserCardCount[wUpsideUser] <= 1))
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) >= cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	if (cbOutCardType == CT_DOUBLE && OutCardResult.cbCardCount == 0 &&  (m_cbUserCardCount[wUndersideUser] <= 2 || m_cbUserCardCount[wUpsideUser] <= 2))
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) > cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	return;
}

void CGameLogicNew::FourTakeOneOrTwoFenChaiCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult)
{
	if ( vecMinTypeCardResult.size() <= 3)
	{
		BYTE tempCard[MAX_COUNT] = { 0 };
		BYTE tempCardCount = 0;
		
		for (int j = 0; j < vecMinTypeCardResult.size(); j++)
		{
			if (vecMinTypeCardResult[j].cbCardType == CT_FOUR_TAKE_ONE || vecMinTypeCardResult[j].cbCardType == CT_FOUR_TAKE_TWO)
			{
				CopyMemory(tempCard, vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount);
				tempCardCount = vecMinTypeCardResult[j].cbCardCount;
				break;
			}
		}
		BYTE tempType = GetCardType(tempCard, tempCardCount);

		if (tempCardCount>0)
		{
			bool bExistMax = true;
			bool bXiangDuiDa = true;
			BYTE cbNeedType = CT_SINGLE;
			if (tempType == CT_FOUR_TAKE_TWO)
			{
				cbNeedType = CT_DOUBLE;
			}
		
			for (int j = 0; j < vecMinTypeCardResult.size(); j++)
			{
				if (vecMinTypeCardResult[j].cbCardType == cbNeedType)
				{
					bExistMax = SearchOtherHandCardThan(vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount, true);
					if (bExistMax == false)
					{
						bXiangDuiDa = SearchOtherHandCardSame(vecMinTypeCardResult[j].cbResultCard, vecMinTypeCardResult[j].cbCardCount, vecMinTypeCardResult[j].cbCardType);
					}
					break;
				}
			}
			bool bExistBoomMaxE = false;
			bExistBoomMaxE = SearchOtherHandCardThan(tempCard, 4, false);
			if (bExistBoomMaxE == false)
			{
				if (bExistMax == false && bXiangDuiDa == false )
				{
					if (m_wMeChairID == m_wBankerUser)
					{
						WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
						WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
						if (m_cbUserCardCount[wUndersideUser] > cbNeedType && m_cbUserCardCount[wUpsideUser] > cbNeedType)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));
							OutCardResult.cbCardCount = cbNeedType;
							CopyMemory(OutCardResult.cbResultCard, tempCard + 4, OutCardResult.cbCardCount);
						}
					}
					else
					{
						if (m_cbUserCardCount[m_wBankerUser] > cbNeedType)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));
							OutCardResult.cbCardCount = cbNeedType;
							CopyMemory(OutCardResult.cbResultCard, tempCard + 4, OutCardResult.cbCardCount);
						}
					}
				}
			}
		}
	}
}

void CGameLogicNew::OutCardDaoShuDiErDaCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> &vecMinTypeCardResult)
{
	if (OutCardResult.cbCardCount == 1)
	{
		vector<BYTE> vecResultData;
		for (int i = vecMinTypeCardResult.size() - 1; i >= 0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[0]);
			}
		}
		if (vecResultData.size() >= 3 && GetCardLogicValue(vecResultData[1])<=14)
		{
			OutCardResult.cbCardCount = 1;
			CopyMemory(OutCardResult.cbResultCard, &vecResultData[1], OutCardResult.cbCardCount);
		}
	}
}



void CGameLogicNew::OutSingleOrDoubleMinCard(const BYTE * cbHandCardData, BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew &OutCardResult, int type)
{
	vector<BYTE> vecResultData;
	int cardCount = 1;
	if (type == CT_DOUBLE)
	{
		cardCount = 2;
	}
	vecResultData = SearchOneOrTwoFromThreeTake(vecMinTypeCardResult, type, OutCardResult);
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == type)
			for (int j = 0; j < cardCount; j++)
			{
				vecResultData.push_back(vecMinTypeCardResult[i].cbResultCard[j]);
			}
	}
	// --��ֹ���ƴ�����һ�г�ȥ
	if (type == CT_SINGLE)
	{
		vector<BYTE>::iterator itor;
		for (itor = vecResultData.begin(); itor != vecResultData.end(); itor++)
		{
			if (GetACardCount(cbHandCardData, cbHandCardCount, *itor) >= 3)
			{
				itor = vecResultData.erase(itor);
			}
		}
	}

	if (vecResultData.size() > 0)
	{
		sort(vecResultData.begin(), vecResultData.end(), [this](BYTE first, BYTE second)
		{
			if (GetCardLogicValue(first) < GetCardLogicValue(second))
			{
				return true;
			}
			return false;
		});
		CopyMemory(OutCardResult.cbResultCard, &vecResultData[0], OutCardResult.cbCardCount);
	}
}

bool CGameLogicNew::FindFirstType(const BYTE cbHandCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	if (m_cbFirstCard>0&&cbHandCardCount==10)
	{
		for (int i = 0; i < vecMinTypeCardResult.size();i++)
		{
			for (int j = 0; j < vecMinTypeCardResult[i].cbCardCount;j++)
			{
				if (vecMinTypeCardResult[i].cbResultCard[j]==GetCardLogicValue(m_cbFirstCard))
				{
					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
					return true;
				}
			}
		}
	}
	return false;
}

void CGameLogicNew::OutCardSpecialCheck(tagOutCardResultNew &OutCardResult, vector<tagOutCardResultNew> vecMinTypeCardResult)
{
	BYTE  tempType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	if (tempType == CT_DOUBLE )
	{
		SearchOneOrTwoFromThreeTake(vecMinTypeCardResult, tempType, OutCardResult);
	}
	else if (tempType == CT_THREE_TAKE_ONE || tempType == CT_THREE_TAKE_TWO)
	{
		SwitchOneOrTwoFromThreeTake(vecMinTypeCardResult, tempType, OutCardResult);
	}
	else if (tempType == CT_SINGLE_LINE &&vecMinTypeCardResult.size()==2&& vecMinTypeCardResult[1].cbCardType == CT_THREE)
	{
		if (OutCardResult.cbResultCard[0]!=1&&(GetCardLogicValue(OutCardResult.cbResultCard[0]) + 1) == GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[0]))
		{
			OutCardResult.cbResultCard[OutCardResult.cbCardCount] = vecMinTypeCardResult[1].cbResultCard[0];
			OutCardResult.cbCardCount++;
		}
		else if ((GetCardLogicValue(OutCardResult.cbResultCard[OutCardResult.cbCardCount-1]) - 1) == GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[0]))
		{
			OutCardResult.cbResultCard[OutCardResult.cbCardCount] = vecMinTypeCardResult[1].cbResultCard[0];
			OutCardResult.cbCardCount++;
		}
		SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
	}
	else if (m_bShenBao&& tempType == CT_SINGLE_LINE &&vecMinTypeCardResult.size() >= 2 && vecMinTypeCardResult[1].cbCardType == CT_THREE)
	{
		if (OutCardResult.cbResultCard[0] != 1 && (GetCardLogicValue(OutCardResult.cbResultCard[0]) + 1) == GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[0]))
		{
			OutCardResult.cbResultCard[OutCardResult.cbCardCount] = vecMinTypeCardResult[1].cbResultCard[0];
			OutCardResult.cbCardCount++;
		}
		else if ((GetCardLogicValue(OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1]) - 1) == GetCardLogicValue(vecMinTypeCardResult[1].cbResultCard[0]))
		{
			OutCardResult.cbResultCard[OutCardResult.cbCardCount] = vecMinTypeCardResult[1].cbResultCard[0];
			OutCardResult.cbCardCount++;
		}
		SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
	}
	else if (tempType == CT_SINGLE_LINE &&OutCardResult.cbCardCount>=4&&vecMinTypeCardResult.size() >= 2)
	{
		if (vecMinTypeCardResult[vecMinTypeCardResult.size() - 1].cbCardType == CT_SINGLE&&vecMinTypeCardResult[vecMinTypeCardResult.size() - 2].cbCardType == CT_SINGLE)
		{
			SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
			for (int i = 0; i < vecMinTypeCardResult.size();i++)
			{
				if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
				{
					if ((GetCardLogicValue(OutCardResult.cbResultCard[0])) == GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]))
					{
						OutCardResult.cbResultCard[0] = OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1];
						OutCardResult.cbResultCard[OutCardResult.cbCardCount - 1] = 0;
						OutCardResult.cbCardCount--;
						SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);

						break;
					}
				}
			}
			
		}
	
	}

}

bool CGameLogicNew::WuDiCheck(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE * cbTurnCardData, BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult)
{
	tagSearchCardResult SearchCardResult;
	SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, &SearchCardResult);
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{
		bool	bExistMax = SearchOtherHandCardThan(SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], false);
		if (bExistMax == false)
		{
			if (CheckOutOneTypeWillWin(cbHandCardData, cbHandCardCount, SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], OutCardResult))
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
				return true;
			}
		}
		else if (cbHandCardCount - SearchCardResult.cbCardCount[i]-m_cbOthreRangCardCount<=0){
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
			CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
			return true;
		}
	}
	/*if (m_cbUserCardCount[1]<=4&&m_cbUserCardCount[2]==0)
	{
	for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
	{

	if (CheckOutOneTypeWillWin(cbHandCardData, cbHandCardCount, SearchCardResult.cbResultCard[i], SearchCardResult.cbCardCount[i], OutCardResult))
	{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
	CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
	return true;
	}
	}
	else if (cbHandCardCount - SearchCardResult.cbCardCount[i] - m_cbOthreRangCardCount <= 0){
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
	CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
	return true;
	}
	}
	}*/

	return false;
}

bool CGameLogicNew::CheckOutOneTypeWillWin(const BYTE * cbHandCardData, BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew &OutCardResult)
{
	BYTE cbReserveCardData[MAX_COUNT] = { 0 };
	BYTE cbReserveCardCount = cbHandCardCount;
	CopyMemory(cbReserveCardData, cbHandCardData, cbReserveCardCount);
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbTurnCardCount>0)
	{
		RemoveCard(
			cbTurnCardData,
			cbTurnCardCount,
			cbReserveCardData,
			cbReserveCardCount
			);
	}
	cbReserveCardCount -= cbTurnCardCount;

	{
		BYTE tmpTurnCardData[MAX_COUNT] = { 0 };
		BYTE tmpTurnCardCount = 0;
		tagSearchCardResult SearchCardResult;
		SearchOutCard(cbReserveCardData, cbReserveCardCount, tmpTurnCardData, tmpTurnCardCount, &SearchCardResult);
		for (int i = 0; i < SearchCardResult.cbSearchCount; i++)
		{
			if (SearchCardResult.cbCardCount[i] >= cbReserveCardCount - m_cbOthreRangCardCount)
			{
				OutCardResult.cbCardCount = SearchCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, SearchCardResult.cbResultCard[i], OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	
	return false;
}

//�����ϼң��ȳ��ƣ�
VOID CGameLogicNew::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResultNew & OutCardResult)
{
	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,0, 0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}

	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (outIndex == -1 || (m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount==1))
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
	//���Ƴ������ڶ�С�Ĳ���
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);
	if ((m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount == 1))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		return;
	}
		return ;
}

//�����ϼң�����ƣ�
VOID CGameLogicNew::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{

	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;
	BYTE cbBankerType = GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]);

	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);
	BYTE cbBankerLargestLogic = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]);
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	if (m_wBankerUser != wOutCardUser)
	{
		//���ѳ���
		if (cbOutCardType>=CT_SINGLE_LINE)
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		}
		else if (cbOutCardType == CT_SINGLE)
		{
			if ( cbTurnCardLogic<12)
			{
				int count = 0;
				BYTE tempLogic = 12;
				while (count<3)
				{
					count++;
					tempLogic = (cbTurnCardLogic > tempLogic ? cbTurnCardLogic : tempLogic);
					CommonSDTurnCardSearch(vecMinTypeCardResult, tempLogic, OutCardResult);
					if (OutCardResult.cbCardCount == 1)
					{
						break;
					}
					tempLogic -= 3;
				}

			}
			else
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
		}
		else if (cbOutCardType == CT_DOUBLE)
		{
			
		
			if (cbTurnCardLogic>13)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (cbTurnCardLogic >= 8 && vecSingleData.size()>1)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (cbHandCardCount > 10 && cbTurnCardLogic >= 11)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
			else if (m_cbUserCardCount[m_wBankerUser]<=2)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			}
		}
	}
	else
	{
		//��������
		if (cbOutCardType == CT_SINGLE)
		{
			{
				int count = 0;
				BYTE tempLogic = 12;
				while (count<3)
				{
					count++;
					tempLogic = (cbTurnCardLogic > tempLogic ? cbTurnCardLogic : tempLogic);
					CommonSDTurnCardSearch(vecMinTypeCardResult, tempLogic, OutCardResult);
					if (OutCardResult.cbCardCount == 1)
					{
						break;
					}
					tempLogic -= 3;
				}

			}
		}
	}
	//���ѳ��Ʊ�������ѹ��
	if (m_cbUserCardCount[wOutCardUser] == 1 && m_wBankerUser != wOutCardUser)
	{
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	}
	if ((m_cbUserCardCount[m_wBankerUser] == 1 && OutCardResult.cbCardCount == 1))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbResultCard[0] = cbHandCardData[0];
		return;
	}
	return ;
}



//�����¼ң��ȳ��ƣ�
VOID CGameLogicNew::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResultNew & OutCardResult) 
{

	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0, 0,0, vecMinTypeCardResult, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount>0)
	{
		FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	
	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	FourTakeOneOrTwoFenChaiCheck(OutCardResult, vecMinTypeCardResult);
	OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);

	//���Ƴ������ڶ�С�Ĳ���
	OutCardDaoShuDiErDaCheck(OutCardResult, vecMinTypeCardResult);

	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	//�¼ұ������⴦��
	if (m_cbUserCardCount[wUpsideUser] == 1)
	{
		    ZeroMemory(&OutCardResult, sizeof(OutCardResult));
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount - 1];
			return;
	}
	
		return ;
}
//�����¼ң�����ƣ�
VOID CGameLogicNew::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult,tempMinTypeScore,biYing);
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);
	if (bExistBiYing && OutCardResult.cbCardCount != 0)
	{
		return;
	}

	BYTE cbBankerType = GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]);

	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);

	if (m_wBankerUser != wOutCardUser)
	{
		if (cbOutCardType >= CT_SINGLE_LINE)
		{
			ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		}
	}
	else
	{
	}
	//���ѳ��Ʊ�������ѹ��
	if (m_cbUserCardCount[wOutCardUser] == 1 && m_wBankerUser != wOutCardUser)
	{
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	}

	return ;
}


bool CGameLogicNew::FindMaxTypeTakeOneType(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardType, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, vector<tagOutCardResultNew> & vecMinTypeCardResult, tagOutCardResultNew &OutCardResult)
{
	if (m_bShenBao)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardType==CT_BOMB_CARD)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	if (vecMinTypeCardResult.size() != 2)
	{
		if (OutCardResult.cbCardCount == 0 && cbTurnCardCount == 0)
		{
			for (int i = 0; i < vecMinTypeCardResult.size(); i++)
			{
				if (vecMinTypeCardResult[i].cbCardCount + m_cbOthreRangCardCount >= cbHandCardCount)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
					CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
					return true;
				}
			}
		}
		return false;
	}

	for (int i = 0; i < vecMinTypeCardResult.size();i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == CT_BOMB_CARD || vecMinTypeCardResult[i].cbCardType == CT_MISSILE_CARD || cbTurnCardType == 0)
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
			bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, vecMinTypeCardResult[i].cbCardCount, true);
			if (bExistMax==false)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
		else if ((vecMinTypeCardResult[i].cbCardType == cbTurnCardType) &&
			vecMinTypeCardResult[i].cbCardCount == cbTurnCardCount && (GetCardLogicValue(cbTurnCardData[0])>GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0])))
		{
			BYTE tmpTurnCard[MAX_COUNT] = { 0 };
			CopyMemory(tmpTurnCard, vecMinTypeCardResult[i].cbResultCard, vecMinTypeCardResult[i].cbCardCount);
			bool bExistMax = SearchOtherHandCardThan(tmpTurnCard, vecMinTypeCardResult[i].cbCardCount, false);
			if (bExistMax == false)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	if (OutCardResult.cbCardCount == 0 && cbTurnCardCount==0)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount + m_cbOthreRangCardCount > m_cbUserCardCount[1])
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				return true;
			}
		}
	}
	return false;
}

bool CGameLogicNew::SearchOtherHandCardThan(const BYTE cbHandCardData[], BYTE cbHandCardCount, bool bNoSearchBomb)
{
	if (m_bHavePass)
	{
		bNoSearchBomb = true;
	}
	bool bExistMax = false;
	if (TOU_SHI)
	{
		for (int i = 0; i < GAME_PLAYER;i++)
		{
			if (i != m_wMeChairID)
			{
				tagSearchCardResult SearchCardResult;
				SearchOutCard(m_cbAllCardData[i], m_cbUserCardCount[i], cbHandCardData, cbHandCardCount, &SearchCardResult);
				if (SearchCardResult.cbSearchCount > 0)
					bExistMax = true;
			}
				
		}
	}
	else
	{
		//��ȡ����
		BYTE cbTmpType = GetCardType(cbHandCardData, cbHandCardCount);
		{
			bool bEnable = false;
			int indexType = cbTmpType;
			if (m_cbMaxCard[cbTmpType] > 0)
			{
				bEnable = true;
			}
			else if (cbTmpType == CT_DOUBLE&&(m_cbMaxCard[CT_SINGLE]>0))
			{
				bEnable = true;
				indexType = CT_DOUBLE;
			}
			else if ((cbTmpType == CT_THREE || cbTmpType == CT_THREE_TAKE_ONE || cbTmpType == CT_THREE_TAKE_TWO) && ((m_cbMaxCard[CT_SINGLE] > 0) || (m_cbMaxCard[CT_DOUBLE] > 0)))
			{
				bEnable = true;
				for (int i = 0; i < CT_THREE;i++)
				{
					if ((m_cbMaxCard[i] > 0))
					{
						if (GetCardLogicValue(m_cbMaxCard[i]) < GetCardLogicValue(m_cbMaxCard[indexType]))
						{
							indexType = i;
						}
					}
				}
				
			}
			if (m_cbMaxCard[indexType]>0)
			{
				if (bEnable&& GetCardLogicValue(cbHandCardData[0]) >= GetCardLogicValue(m_cbMaxCard[indexType]))
				{
					return false;
				}
			}
			
		}
		
		
		BYTE tempCard[FULL_COUNT] = { 0 };
	
		BYTE tempCardCount = 0;
		CopyMemory(tempCard, m_cbCardData, FULL_COUNT);
		tempCardCount = FULL_COUNT;
		RemoveCard(m_cbDiscardCard, m_cbDiscardCardCount, tempCard, tempCardCount);
		tempCardCount = tempCardCount - m_cbDiscardCardCount;
		/*	if (m_cbUserCardCount[2] == 0)
			{
			BYTE tempCardEx[] = { 0x03, 0x04, 0x13, 0x14, 0x23, 0x24, 0x33, 0x34 };
			RemoveCard(tempCardEx, sizeof(tempCardEx), tempCard, tempCardCount);
			tempCardCount = tempCardCount - sizeof(tempCardEx);
			}*/
		if (RemoveCard(m_cbAllCardData[m_wMeChairID], m_cbUserCardCount[m_wMeChairID], tempCard, tempCardCount))
		{
			tempCardCount = tempCardCount - m_cbUserCardCount[m_wMeChairID];
		}
		tagSearchCardResult SearchCardResult;
		SearchOutCard(tempCard, tempCardCount, cbHandCardData, cbHandCardCount, &SearchCardResult, bNoSearchBomb);
		if (SearchCardResult.cbSearchCount > 0)
			bExistMax = true;
	}
	return bExistMax;
}

bool CGameLogicNew::SearchOtherHandCardSame(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cardType)
{
	if (cardType >= CT_THREE)
	{
		return false;
	}
	bool bExistMax = false;
	if (TOU_SHI)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i != m_wMeChairID)
			{
				int sameCount = 0;
				for (int j = 0; j < m_cbUserCardCount[i];j++)
				{
					int tempLogic = GetCardLogicValue(m_cbAllCardData[i][j]);
					for (int k = 0; k < cbHandCardCount;k++)
					{
						if (GetCardLogicValue(cbHandCardData[k])==tempLogic)
						{
							sameCount++;
						}
					}
					if (sameCount>=cbHandCardCount)
					{
						bExistMax = true;
						break;
					}
				}
				if (bExistMax)
				{
					break;
				}
			}

		}
	}
	else
	{
		//��ȡ����
		BYTE cbTmpType = GetCardType(cbHandCardData, cbHandCardCount);
		{
			bool bEnable = false;
			int indexType = cbTmpType;
			if (m_cbMaxCard[cbTmpType] > 0)
			{
				bEnable = true;
			}
			else if (cbTmpType == CT_DOUBLE && (m_cbMaxCard[CT_SINGLE] > 0))
			{
				bEnable = true;
				indexType = CT_DOUBLE;
			}
			else if ((cbTmpType == CT_THREE || cbTmpType == CT_THREE_TAKE_ONE || cbTmpType == CT_THREE_TAKE_TWO) && ((m_cbMaxCard[CT_SINGLE] > 0) || (m_cbMaxCard[CT_DOUBLE] > 0)))
			{
				bEnable = true;
				for (int i = 0; i < CT_THREE; i++)
				{
					if ((m_cbMaxCard[i] > 0))
					{
						if (GetCardLogicValue(m_cbMaxCard[i]) < GetCardLogicValue(m_cbMaxCard[indexType]))
						{
							indexType = i;
						}
					}
				}

			}
			if (m_cbMaxCard[indexType] > 0)
			{
				if (bEnable&& GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(m_cbMaxCard[indexType]))
				{
					return false;
				}
			}

		}
		BYTE tempCard[FULL_COUNT] = { 0 };

		BYTE tempCardCount = 0;
		CopyMemory(tempCard, m_cbCardData, FULL_COUNT);
		tempCardCount = FULL_COUNT;
		RemoveCard(m_cbDiscardCard, m_cbDiscardCardCount, tempCard, tempCardCount);
		tempCardCount = tempCardCount - m_cbDiscardCardCount;
	/*	if (m_cbUserCardCount[2] == 0)
		{
			BYTE tempCardEx[] = { 0x03, 0x04, 0x13, 0x14, 0x23, 0x24, 0x33, 0x34 };
			RemoveCard(tempCardEx, sizeof(tempCardEx), tempCard, tempCardCount);
			tempCardCount = tempCardCount - sizeof(tempCardEx);
		}*/
		if (RemoveCard(m_cbAllCardData[m_wMeChairID], m_cbUserCardCount[m_wMeChairID], tempCard, tempCardCount))
		{
			tempCardCount = tempCardCount - m_cbUserCardCount[m_wMeChairID];
		}

		int sameCount = 0;
		for (int j = 0; j < tempCardCount; j++)
		{
			int tempLogic = GetCardLogicValue(tempCard[j]);
			for (int k = 0; k < cbHandCardCount; k++)
			{
				if (GetCardLogicValue(cbHandCardData[k]) == tempLogic)
				{
					sameCount++;
				}
			}
			if (sameCount >= cbHandCardCount)
			{
				bExistMax = true;
				break;
			}
		}
	}
	return bExistMax;
}


float CGameLogicNew::calCardScoreEx(vector<tagOutCardResultNew> &vecMinTypeCardResult, float score, BYTE cbHandCardCount, BYTE * cbHandCardData, tagOutCardTypeResultNew * CardTypeResult)
{
	int singleCount = 0;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			singleCount++;
		}
	}
	if (singleCount <=3)
	{
		score += 5 * (3-singleCount);
	}

/*	if (vecMinTypeCardResult[0].cbCardType == CT_MISSILE_CARD)
	{
		score += 45;
	}
	else*/{
		int erCount = 0;
		int aCount = 0;
		for (int i = 0; i < cbHandCardCount; i++)
		{
			if ((GetCardLogicValue(cbHandCardData[i]) == 15))
			{
				erCount++;
			}
			if ((GetCardLogicValue(cbHandCardData[i]) == 14))
			{
				aCount++;
			}
		}
	
		if (cbHandCardData[0] == 0x4F )
		{
			score += 25;
		}
		if (cbHandCardData[0] == 0x4E)
		{
			score += 20;
		}
		if (vecMinTypeCardResult.size() <= 5)
		{
			score += 10;
		}
		if (erCount >= 1)
		{
			score += erCount*10;
		}
		if (aCount >=1)
		{
			score += aCount * 3;
		}
		if (cbHandCardData[0]<0x4E && erCount<2)
		{
			score -= 10;
		}
		if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0)
		{
			score += 10;
		}
		if (score<30&&cbHandCardData[0] == 0x4F && erCount == 2 && aCount >= 1 && singleCount <= 4)
		{
			score += 15;
		}
		if (cbHandCardData[0]<0x02)
		{
			score -= 45;
		}
	}	return score;
}

bool CGameLogicNew::SearchOutCardErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount, BYTE cbRangCardCount, BYTE cbOthreRangCardCount, BYTE MaxCard[], tagOutCardResultNew & OutCardResult)
{
	SetDiscardCard(DiscardCard, cbDiscardCardCount);
	SetUserCard(0, cbHandCardData, cbHandCardCount);
	
	m_cbUserCardCount[1] = (FULL_COUNT - cbHandCardCount - cbDiscardCardCount - 20);
	if (cbRangCardCount>0)
	{
		//m_cbUserCardCount[1] -= cbRangCardCount;
		if (cbRangCardCount=10)
		{
			m_bShenBao = true;
		}
		else{
			m_bHavePass = true;
		}
	}
	else{
		m_bHavePass = false;
		m_bShenBao = false;
	}
	m_cbFirstCard = cbOthreRangCardCount;
	CopyMemory(m_cbMaxCard, MaxCard, sizeof(m_cbMaxCard));
	//����ж�
	WORD wUndersideOfBanker = (m_wBankerUser + 1) % GAME_PLAYER;	//�����¼�
	WORD wUpsideOfBanker = (wUndersideOfBanker + 1) % GAME_PLAYER;	//�����ϼ�

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	SortCardList(cbHandCardData, cbHandCardCount, ST_ORDER);
	SortCardList(cbTurnCardData, cbTurnCardCount, ST_ORDER);
	//�ȳ���
	if (cbTurnCardCount == 0)
	{
		//��������
		ErZhuDongOutCard(cbHandCardData, cbHandCardCount, OutCardResult);

	}
	//ѹ��
	else
	{
		//����������
		ErBeiDongOutCard(cbHandCardData, cbHandCardCount, 1, cbTurnCardData, cbTurnCardCount, OutCardResult);

	}
	return true;
}

int CGameLogicNew::LandScoreErRen(BYTE cbHandCardData[], BYTE cbHandCardCount, int &cbCurrentLandScore)
{
	ReSetData();
	//������Ŀ
	BYTE cbLargeCardCount = 0;
	BYTE Index = 0;
	tagOutCardTypeResultNew   CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (vecMinTypeCardResult.size()==1)
	{
			cbCurrentLandScore = 1;
			return 1;
	}
	tagOutCardResultNew  OutCardResult = {};
	cbCurrentLandScore = 0;
	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (bZhiJieChu&&OutCardResult.cbCardCount > 0)
	{
		cbCurrentLandScore = 1;
		return 1;
	}
	if (vecMinTypeCardResult.size()==2)
	{
		for (int i = 0; i < vecMinTypeCardResult.size();i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount >= 6)
			{
				cbCurrentLandScore = 1;
				return 1;
			}
			else if (vecMinTypeCardResult[i].cbCardType == CT_THREE&& GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]) >= 12)
			{
				cbCurrentLandScore = 1;
				return 1;
			}
		}
		
	}
	//�����з�
	return 0;

}

bool CGameLogicNew::CheckBombPercent(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE DiscardCard[], BYTE cbDiscardCardCount)
{
	for (int i = 0; i < cbHandCardCount; i++)
	{
		if (cbHandCardData[i] >= 0x4E)
		{
			return true;
		}
	}
	for (int i = 0; i < cbDiscardCardCount; i++)
	{
		if (DiscardCard[i] >= 0x4E)
		{
			return true;
		}
	}
	return false;
}

bool CGameLogicNew::CheckTwoArrayIntersect(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount)
{
	for (int i = 0; i < cbHandCardCount; i++)
	{
		for (int j = 0; j < cbTurnCardCount; j++)
		{
			if (cbHandCardData[i] == cbTurnCardData[j])
			{
				return true;
			}
		}
	}
	return false;
}
//�������ƣ��ȳ��ƣ�
VOID CGameLogicNew::ErZhuDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResultNew & OutCardResult)
{
	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);
	if (FindFirstType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult))
	{
		OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
		return;
	}
	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, 0,0, 0, vecMinTypeCardResult, OutCardResult))
	{
		int type = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		return;
				
	}
	
	if (minTypeCount == 1)
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	if (WuDiCheck(cbHandCardData, cbHandCardCount, NULL, 0, OutCardResult))
	{
		BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
		return;
	}
	if (m_bShenBao)
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			if (vecMinTypeCardResult[i].cbCardCount>=6)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
				return;
			}
		}
	}
	int tmpType = GetCardType(cbHandCardData, cbHandCardCount);
	
	if ((tmpType != CT_ERROR && (tmpType != CT_THREE_TAKE_ONE)) || (tmpType == CT_THREE_TAKE_ONE&&cbHandCardCount == 4))
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, OutCardResult.cbCardCount);
		return;
	}
	bool bZhiJieChu = false;
	int outIndex = YouXianDaNengShouHuiCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, bZhiJieChu);
	if (OutCardResult.cbCardCount>0)
	{
		    int  type = OutCardResult.cbCardType;
			if ((type == CT_SINGLE || type == CT_DOUBLE) && (bZhiJieChu != true))
			{
				OutSingleOrDoubleMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult, type);
			}
			else if (bZhiJieChu == true)
			{
				BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
				return;
			}
	}

	if (outIndex == -1)
	{
		outIndex = SearchMutilType(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
		OutCardSpecialCheck(OutCardResult, vecMinTypeCardResult);
	}
	

	if ((m_cbUserCardCount[wUpsideUser] == CT_SINGLE || m_cbUserCardCount[wUndersideUser] == CT_SINGLE) && OutCardResult.cbCardCount == 1)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_SINGLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	else if ((m_cbUserCardCount[wUpsideUser] == 2 || m_cbUserCardCount[wUndersideUser] == 2) && OutCardResult.cbCardCount == 2)
	{
		for (int i = vecMinTypeCardResult.size() - 1; i >0; i--)
		{
			if (vecMinTypeCardResult[i].cbCardType != CT_DOUBLE)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	if (OutCardResult.cbCardCount == 0)
	{
		for (int k = 0; k < CT_TYPE_COUNT; k++)
		{
			for (BYTE i = 0; i < CardTypeResult[k].cbCardTypeCount; ++i)
			{

				OutCardResult.cbCardCount = CardTypeResult[k].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[k].cbCardData[i], CardTypeResult[k].cbEachHandCardCount[i]);
				break;
			}
		}
	}
	////--��������������ʱ, Ҫ����С����
	//if (ThreeTakeOneChangeTwoTake(cbHandCardData,cbHandCardCount, vecMinTypeCardResult, OutCardResult))
	//{
	//}
	//else
	{
		ThreeTakeTwoTakeMinCard(cbHandCardData, cbHandCardCount, vecMinTypeCardResult, OutCardResult);
	}
	//����������������һ������С�ڵ���2��,��ô�������������ѹ��
	if (OutCardResult.cbCardCount == 1 && (m_cbUserCardCount[wUndersideUser] == 1))
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			BYTE tempLogic = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			if (vecMinTypeCardResult[i].cbCardCount > 1)
			{
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
	BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
	return;
}

//�������ƣ�����ƣ�
VOID CGameLogicNew::ErBeiDongOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResultNew & OutCardResult)
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//���±�û��
	tagOutCardTypeResultNew CardTypeResult[CT_TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));

	vector<tagOutCardResultNew>   vecMinTypeCardResult;
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	float tempMinTypeScore = 0;
	bool biYing = false;
	int minTypeCount = FindCardKindMinNum(cbHandCardData, cbHandCardCount, CardTypeResult, vecMinTypeCardResult, tempMinTypeScore, biYing);

	if (FindMaxTypeTakeOneType(cbHandCardData, cbHandCardCount, cbOutCardType, cbTurnCardData, cbTurnCardCount, vecMinTypeCardResult, OutCardResult))
	{
		return;
	}
	if (minTypeCount == 1 && CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount))
	{
		OutCardResult.cbCardCount = vecMinTypeCardResult[0].cbCardCount;
		CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[0].cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	if (WuDiCheck(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult))
	{
		return;
	}
	WORD wUndersideUser = (m_wBankerUser + 1) % GAME_PLAYER;
	WORD wUpsideUser = (wUndersideUser + 1) % GAME_PLAYER;
	bool bExistBiYing = OutCardShengYuFenCheck(cbHandCardCount, cbHandCardData, cbTurnCardData, cbTurnCardCount, wOutCardUser, tempMinTypeScore, vecMinTypeCardResult, OutCardResult);

	if (bExistBiYing && OutCardResult.cbCardCount!=0)
	{
		BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);
		return;
	}
	//�����
	BYTE cbTurnCardLogic = GetCardLogicValue(cbTurnCardData[0]);

	

	if (cbOutCardType == CT_SINGLE && OutCardResult.cbCardCount == 0 && m_cbUserCardCount[wOutCardUser] <= 1)
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) >= cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 1;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	if (cbOutCardType == CT_DOUBLE && OutCardResult.cbCardCount == 0 && m_cbUserCardCount[wOutCardUser] <= 2)
	{
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = 0; i < CardTypeResult[CT_THREE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[CT_THREE].cbCardData[i][0]) > cbTurnCardLogic)
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_THREE].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}

	}
	//����������������һ������С�ڵ���2��,��ô�������������ѹ��
	if (OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[wUndersideUser] <= 2))
	{
		for (int i = 0; i < vecMinTypeCardResult.size(); i++)
		{
			BYTE tempLogic = GetCardLogicValue(vecMinTypeCardResult[i].cbResultCard[0]);
			if (tempLogic > cbTurnCardLogic&& vecMinTypeCardResult[i].cbCardType == cbOutCardType && vecMinTypeCardResult[i].cbCardCount == cbTurnCardCount)
			{
				OutCardResult.cbCardCount = vecMinTypeCardResult[i].cbCardCount;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[i].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
		if (OutCardResult.cbCardCount == 0)
		{
			for (int i = CardTypeResult[cbOutCardType].cbCardTypeCount - 1; i >= 0; i--)
			{
				BYTE tempLogic = GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0]);
				if (tempLogic > cbTurnCardLogic&& CardTypeResult[cbOutCardType].cbEachHandCardCount[i] == cbTurnCardCount)
				{
					OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[i];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[i], OutCardResult.cbCardCount);
					break;
				}
			}
		}
	}
	
	BaoDanJiaoYan(OutCardResult, wUndersideUser, cbHandCardData);

	return;
}

void CGameLogicNew::BaoDanJiaoYan(tagOutCardResultNew &OutCardResult, WORD wUndersideUser, const BYTE * cbHandCardData)
{
	if (OutCardResult.cbCardCount == 1 && (m_cbUserCardCount[wUndersideUser] == 1) )
	{

		
		OutCardResult.cbResultCard[0] = cbHandCardData[0];

	}
}

bool CGameLogicNew::ThreeTakeOneChangeTwoTake(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	if ((outCardType == CT_THREE&&cbHandCardCount <= 3) || (outCardType == CT_THREE_TAKE_ONE&&cbHandCardCount <= 4))
	{
		return false;
	}
	BYTE cbRemainCardData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = cbHandCardCount;
	CopyMemory(cbRemainCardData, cbCardData, cbRemainCardCount);
	if (outCardType == CT_THREE_TAKE_ONE || outCardType == CT_THREE)
	{
			BYTE cbThreeCard[MAX_COUNT] = { 0 };
		cbThreeCard[0] = OutCardResult.cbResultCard[0];
		cbThreeCard[1] = OutCardResult.cbResultCard[1];
		cbThreeCard[2] = OutCardResult.cbResultCard[2];
		RemoveCard(cbThreeCard, 3, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= 3;
		SortCardList(cbRemainCardData, cbRemainCardCount, ST_ORDER);
		int minTypeCount = 0;
		int resultIndex = -1;
		float MinTypeScore = INT_MIN;
		BYTE ResultThreeTakeCard[MAX_COUNT] = { 0 };
		float tableScore[MAX_RESULT_COUNT] = { 0 };
		//CopyMemory(cbRemainCardData, cbThreeCard, 3);
		int cbNeedCardCount = 2;
		//�������
		BYTE cbComCard[8] = { 0 };
		BYTE cbComResCard[MAX_RESULT_COUNT][8];
		int cbComResLen = 0;
		//-- //���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
		cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);

		for (BYTE i = 0; i < cbComResLen; ++i)
		{
			//�������
			BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
			CopyMemory(tempThreeTakeTwo, cbThreeCard, 3);
			for (int j = 0; j < cbNeedCardCount; j++)
			{
				tempThreeTakeTwo[3 + j] = cbComResCard[i][j];
			}
			vector<tagOutCardResultNew>  TempMinTypeCardResult;
			tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
			BYTE searchCard[MAX_COUNT] = { 0 };
			BYTE searchCardCount = cbHandCardCount;
			CopyMemory(searchCard, cbCardData, searchCardCount);
			RemoveCard(tempThreeTakeTwo, 5, searchCard, searchCardCount);
			searchCardCount -= 5;
			if (searchCardCount > 0)
			{
				int tempMinTypeCount = 0;
				float tempMinTypeScore = 0;
				bool biYing = false;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult, tempMinTypeScore, biYing, true);
				tableScore[i] = tempMinTypeScore;
				if (tempMinTypeScore > MinTypeScore)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, 5);
				}

			}
			else{
				CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, 5);
				break;
			}
		}
		ZeroMemory(&OutCardResult, sizeof(OutCardResult));
		OutCardResult.cbCardCount = 5;
		CopyMemory(OutCardResult.cbResultCard, ResultThreeTakeCard, 5);
		return true;
	}
	return false;
}

bool CGameLogicNew::ThreeTakeTwoTakeMinCard(const BYTE cbCardData[], BYTE cbHandCardCount, vector<tagOutCardResultNew> &vecMinTypeCardResult, tagOutCardResultNew & OutCardResult)
{
	return false;
	int outCardType = GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	BYTE cbRemainCardData[MAX_COUNT] = { 0 };
	BYTE cbRemainCardCount = cbHandCardCount;
	CopyMemory(cbRemainCardData, cbCardData, cbRemainCardCount);
	if (outCardType == CT_THREE_TAKE_TWO)
	{
		BYTE cbThreeCard[MAX_COUNT] = { 0 };
		int tempCount = OutCardResult.cbCardCount / 5;
		int cbNeedCardCount = 2 * tempCount;
		for (int i = 0; i < 3 * tempCount; i++)
		{
			cbThreeCard[i] = OutCardResult.cbResultCard[i];
		}
		RemoveCard(cbThreeCard, 3 * tempCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= 3 * tempCount;
		SortCardList(cbRemainCardData, cbRemainCardCount, ST_ORDER);
		int minTypeCount = 0;
		int resultIndex = -1;
		float MinTypeScore = INT_MIN;
		BYTE ResultThreeTakeCard[MAX_COUNT] = { 0 };
		float tableScore[MAX_RESULT_COUNT] = { 0 };
		//CopyMemory(cbRemainCardData, cbThreeCard, 3);
		//�������
		BYTE cbComCard[8] = { 0 };
		BYTE cbComResCard[MAX_RESULT_COUNT][8];
		int cbComResLen = 0;
		//-- //���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
		//cbRemainCardCount = ClearReLogicValue(cbRemainCardData, cbRemainCardCount);
		Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbNeedCardCount, cbRemainCardCount, cbNeedCardCount);

		for (BYTE i = 0; i < cbComResLen; ++i)
		{
			//�������
			BYTE tempThreeTakeTwo[MAX_COUNT] = { 0 };
			CopyMemory(tempThreeTakeTwo, cbThreeCard, 3 * tempCount);
			for (int j = 0; j < cbNeedCardCount; j++)
			{
				tempThreeTakeTwo[3 * tempCount + j] = cbComResCard[i][j];
			}
			vector<tagOutCardResultNew>  TempMinTypeCardResult;
			tagOutCardTypeResultNew tmpCardTypeResult[CT_TYPE_COUNT];
			BYTE searchCard[MAX_COUNT] = { 0 };
			BYTE searchCardCount = cbHandCardCount;
			CopyMemory(searchCard, cbCardData, searchCardCount);
			RemoveCard(tempThreeTakeTwo, OutCardResult.cbCardCount, searchCard, searchCardCount);
			searchCardCount -= OutCardResult.cbCardCount;
			if (searchCardCount > 0)
			{
				int tempMinTypeCount = 0;
				float tempMinTypeScore = 0;
				bool biYing = false;
				tempMinTypeCount = FindCardKindMinNum(searchCard, searchCardCount, tmpCardTypeResult, TempMinTypeCardResult, tempMinTypeScore, biYing, true);
				tableScore[i] = tempMinTypeScore;
				if (tempMinTypeScore > MinTypeScore)
				{
					MinTypeScore = tempMinTypeScore;
					minTypeCount = tempMinTypeCount;
					resultIndex = i;
					CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, OutCardResult.cbCardCount);
				}

			}
			else{
				CopyMemory(ResultThreeTakeCard, tempThreeTakeTwo, OutCardResult.cbCardCount);
				break;
			}
		}
		CopyMemory(OutCardResult.cbResultCard, ResultThreeTakeCard, OutCardResult.cbCardCount);
		return true;
	}
	return false;
}

void CGameLogicNew::CommonSDTurnCardSearch(vector<tagOutCardResultNew> &vecMinTypeCardResult, BYTE cbTurnCardLogic, tagOutCardResultNew &OutCardResult)
{
	vector<tagCardIndex> vecSingleData;
	vector<tagCardIndex> vecDoubleData;
	for (int i = 0; i < vecMinTypeCardResult.size(); i++)
	{
		tagCardIndex  cardIndexSt;
		if (vecMinTypeCardResult[i].cbCardType == CT_SINGLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecSingleData.push_back(cardIndexSt);
		}
		if (vecMinTypeCardResult[i].cbCardType == CT_DOUBLE)
		{
			cardIndexSt.cbIndex = i;
			cardIndexSt.cbCardData = vecMinTypeCardResult[i].cbResultCard[0];
			vecDoubleData.push_back(cardIndexSt);
		}
	}
	for (int i = vecSingleData.size() - 1; i >= 0; i--)
	{
		if (GetCardLogicValue(vecSingleData[i].cbCardData)>cbTurnCardLogic)
		{
			OutCardResult.cbCardCount = 1;
			CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[vecSingleData[i].cbIndex].cbResultCard, OutCardResult.cbCardCount);
			break;
		}
	}
	if (OutCardResult.cbCardCount == 0)
	{
		for (int i = vecDoubleData.size() - 1; i >= 0; i--)
		{
			if (GetCardLogicValue(vecDoubleData[i].cbCardData) > cbTurnCardLogic)
			{
				OutCardResult.cbCardCount = 1;
				CopyMemory(OutCardResult.cbResultCard, vecMinTypeCardResult[vecDoubleData[i].cbIndex].cbResultCard, OutCardResult.cbCardCount);
				break;
			}
		}
	}
}

void CGameLogicNew::SwitchArray(BYTE bInFirstList[], BYTE bInNextList[], int count)
{
	for (int i = 0; i < count; i++)
	{
		BYTE tempTemp = bInFirstList[i];
		bInFirstList[i] = bInNextList[i];
		bInNextList[i] = tempTemp;
	}
}

float CGameLogicNew::GetCardTurnPercent(const BYTE cbCardData[], BYTE cbCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbDisCardData[], BYTE cbDisCardCount)
{
	bool bExistMax = false;

	BYTE allCard[FULL_COUNT] = { 0 };

	BYTE allCardCardCount = sizeof(m_cbCardData);
	CopyMemory(allCard, m_cbCardData, sizeof(m_cbCardData));

	RemoveCard(cbDisCardData, cbDisCardCount, allCard, allCardCardCount);
	allCardCardCount = allCardCardCount - cbDisCardCount;
	if (RemoveCard(cbHandCardData, cbHandCardCount, allCard, allCardCardCount))
	{
		allCardCardCount = allCardCardCount - cbHandCardCount;
	}
	//�����˿�
	tagDistributingNew Distributing = {};
	AnalysebDistributing(allCard, allCardCardCount, Distributing);
	CopyMemory(Distributing.cbDistributing[13], Distributing.cbDistributing[0], sizeof(Distributing.cbDistributing[0]));
	BYTE cbTmpLinkCount = 0;
	BYTE cbValueIndex = 0;
	BYTE cbReferIndex = GetCardLogicValue(cbCardData[0])+1;
	int existEvent = 0;
	for (cbValueIndex = cbReferIndex; cbValueIndex < 14; cbValueIndex++)
	{
		int tempCardCount = Distributing.cbDistributing[cbValueIndex][cbIndexCount];
		if (tempCardCount == cbCardCount)
		{
			existEvent++;
		}
		else if (tempCardCount > cbCardCount)
		{
			existEvent += GetCMNSort(tempCardCount, cbCardCount);
			//ը��Ҳ������һ��
			if (tempCardCount == 4)
			{
				existEvent++;
			}
		}
	}
	int totalEvent = GetCMNSort(allCardCardCount, cbCardCount);
	if (existEvent==0)
	{
		return 1.0;
	}
	float gaiLv = (float) existEvent / totalEvent;
	return gaiLv;
	
}

int CGameLogicNew::GetCMNSort(int m, int n)
{
	int mResult = 1;
	for (int i = m; i > (m - n); i--){
		mResult = mResult*i;
	}
	int nResult = 1;
	for (int i = n; i >= 1; i--)
	{
		nResult = nResult*i;
	}
	return mResult / nResult;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
