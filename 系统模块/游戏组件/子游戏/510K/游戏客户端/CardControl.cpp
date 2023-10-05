#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////////////

//�ɷ�����
#define DISPATCH_MAX_INDEX			6									//�������
#define DISPATCH_DELAY_INDEX		2									//��ʱ����

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CCardResource CCardControl::m_CardResource;								//�˿���Դ

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCardResource::CCardResource()
{
	//���ñ���
	m_CardSize.SetSize(0,0);

	return;
}

//��������
CCardResource::~CCardResource()
{
}

//������Դ
VOID CCardResource::Initialize(CD3DDevice * pD3DDevice)
{
	//������Դ
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_TextureCard.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_CARD"),TEXT("PNG"));
	m_TextureSmallCard.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_CARD_SMALL"),TEXT("PNG"));
	m_TextureNum.LoadImage(pD3DDevice,hResInstance,TEXT("REMNANT_CARD_NUMBER"),TEXT("PNG"));
	//��ȡ��С
	m_CardSize.SetSize(m_TextureCard.GetWidth()/13,m_TextureCard.GetHeight()/5);
	m_CardSizeSmall.SetSize(m_TextureSmallCard.GetWidth()/13,m_TextureSmallCard.GetHeight()/5);
	
	return;
}

//��ȡ��С
VOID CCardResource::GetCardSize(bool bSamllMode, CSize & CardSize)
{
	//���ñ���
	CardSize=(bSamllMode==false)?m_CardSize:m_CardSizeSmall;

	return;
}

//��ȡ��Դ
VOID CCardResource::GetCardTexture(bool bSmallMode, CD3DTexture * * pD3DTexture)
{
	//���ñ���
	if (bSmallMode==false)
	{
		(*pD3DTexture)=&m_TextureCard;
	}
	else
	{
		(*pD3DTexture)=&m_TextureSmallCard;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCardControl::CCardControl()
{
	//���Ա���
	m_bSmallMode=false;
	m_bHorizontal=true;
	m_bPositively=false;
	m_bDisplayItem=false;
	m_cbBackGround=CARD_BOOR;
	m_bShowNum=true;
	//ѡ�����
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//���Ʊ���
	m_bOutCard=false;
	m_wOutCardIndex=0;

	//�˿�����
	m_cbCardCount=0;
	m_cbShowCount=MAX_CARD_COUNT;
	ZeroMemory(m_CardItemPoint,sizeof(m_CardItemPoint));
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	//λ�ñ���
	m_YCollocateMode=enYTop;
	m_XCollocateMode=enXLeft;
	m_DispatchPos.SetPoint(0,0);
	m_BenchmarkPos.SetPoint(0,0);

	//�������
	m_nXDistance=DEF_X_DISTANCE;
	m_nYDistance=DEF_Y_DISTANCE;
	m_nShootDistance=DEF_SHOOT_DISTANCE;
	
	return;
}

//��������
CCardControl::~CCardControl()
{
}

//�����˿�
bool CCardControl::SetCardData(BYTE cbCardCount)
{
	if(cbCardCount>CountArray(m_CardItemArray))
	{
		int tmp = 4;
	}
	//Ч�����
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//���ñ���
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//���ñ���
	m_cbCardCount=cbCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}

//�����˿�
bool CCardControl::SetCardData(BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//���ñ���
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//�˿���Ŀ
	m_cbCardCount=cbCardCount;

	//�����˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
		m_CardItemArray[i].bFlag=false;
	}

	return true;
}

//�����˿�
bool CCardControl::SetShootArea(BYTE cbHeadIndex, BYTE cbTailIndex)
{
	//���ñ���
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if ((i>=cbHeadIndex)&&(i<=cbTailIndex))
		{
			m_CardItemArray[i].bShoot=!m_CardItemArray[i].bShoot;
		}
	}

	return true;
}

//�����˿�
bool CCardControl::SetSelectIndex(BYTE cbHeadIndex, BYTE cbTailIndex)
{
	//���ñ���
	m_cbTailIndex=cbTailIndex;
	m_cbHeadIndex=cbHeadIndex;

	return true;
}

//�����˿�
bool CCardControl::SetShootCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	bool bChangeStatus=false;

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//�����˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		for (BYTE j=0;j<m_cbCardCount;j++)
		{
			if ((m_CardItemArray[j].bShoot==false)&&(m_CardItemArray[j].cbCardData==cbCardData[i])) 
			{
				bChangeStatus=true;
				m_CardItemArray[j].bShoot=true;
				break;
			}
		}
	}

	return bChangeStatus;
}

//������ͬ����
bool CCardControl::SetShootSameCard(BYTE cbCardData)
{
	//��������
	bool bChangeStatus=false;

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if( m_GameLogic.GetCardValue(m_CardItemArray[i].cbCardData) == m_GameLogic.GetCardValue(cbCardData) )
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=true;
		}
	}

	return bChangeStatus;
}

//����˳��
bool CCardControl::SetShootOrderCard( BYTE cbCardOne, BYTE cbCardTwo )
{
	//��������
	bool bChangeStatus = false;

	//��������
	BYTE cbCradOneValue = m_GameLogic.GetCardLogicValue(cbCardOne);
	BYTE cbCradTwoValue = m_GameLogic.GetCardLogicValue(cbCardTwo);
	BYTE cbBeginCradValue = min(cbCradOneValue, cbCradTwoValue);
	BYTE cbEndCradValue = max(cbCradOneValue, cbCradTwoValue);
	BYTE cbOrderAllCount = cbEndCradValue - cbBeginCradValue + 1;

	//����˳��
	if ( cbOrderAllCount < 5 )
		return false;

	//˳�ӱ���
	BYTE cbOrderCard[MAX_CARD_COUNT];
	BYTE cbOrderCount = 0;
	ZeroMemory(cbOrderCard, sizeof(cbOrderCard));

	//ԭʼ����
	BYTE cbCardData[MAX_CARD_COUNT];
	ZeroMemory(cbCardData, sizeof(cbCardData));

	//�����˿�
	for ( BYTE i = 0; i < m_cbCardCount; i++ )
		cbCardData[i] = m_CardItemArray[i].cbCardData;

	//����
	m_GameLogic.SortCardList(cbCardData, m_cbCardCount, ST_ORDER);

	//Ѱ��˳��
	for ( BYTE i = 0; i < m_cbCardCount; i++ )
	{
		if( m_GameLogic.GetCardLogicValue(cbCardData[i]) == cbEndCradValue - cbOrderCount )
		{
			cbOrderCard[cbOrderCount] = cbCardData[i];
			cbOrderCount++;

			if ( m_GameLogic.GetCardValue(cbCardData[i]) == 0x02 )
			{
				return false;
			}
			if ( cbOrderCount == cbOrderAllCount )
			{
				break;
			}
		}
	}

	//����˳��
	BYTE cbShootCount = 0;

	//����˳��
	for (BYTE i = 0;i < cbOrderCount; i++)
	{
		for (BYTE j = 0;j < m_cbCardCount; j++)
		{
			if ( m_CardItemArray[j].cbCardData == cbOrderCard[i] ) 
			{
				cbShootCount++;
				break;
			}
		}
	}

	//����˳��
	if ( cbShootCount == cbOrderAllCount )
	{
		//�����˿�
		for ( BYTE i = 0; i < m_cbCardCount; i++ )
		{
			m_CardItemArray[i].bShoot = false;
		}

		//����˳��
		for (BYTE i = 0;i < cbOrderCount; i++)
		{
			for (BYTE j = 0;j < m_cbCardCount; j++)
			{
				if ( m_CardItemArray[j].cbCardData == cbOrderCard[i] ) 
				{
					m_CardItemArray[j].bShoot = true;
					break;
				}
			}
		}

		return true;
	}

	return false;
}

//�ɷ��˿�
bool CCardControl::ShowOutCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT((cbCardCount>0)&&(cbCardCount<=CountArray(m_CardItemArray)));
	if ((cbCardCount==0)||(cbCardCount>CountArray(m_CardItemArray))) return false;

	//����״̬
	m_bOutCard=true;
	m_wOutCardIndex=0;

	//���ñ���
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//�˿���Ŀ
	m_cbCardCount=cbCardCount;

	//�����˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	//����ʱ��
	m_OutCardLapseCount.Initialization();

	return true;
}

//ɾ���˿�
bool CCardControl::RemoveShootItem()
{
	//���ñ���
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//�����˿�
	tagCardItem	CardItemArray[MAX_CARD_COUNT];
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_cbCardCount);

	//ɾ���˿�
	WORD wRemoveCount=0;
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if (CardItemArray[i].bShoot==true)
		{
			wRemoveCount++;
			CardItemArray[i].cbCardData=0x00;
		}
	}

	//�����˿�
	if (wRemoveCount>0)
	{
		//�����˿�
		BYTE cbInsertCount=0;
		for (BYTE i=0;i<m_cbCardCount;i++)
		{
			if (CardItemArray[i].cbCardData!=0x00)
			{
				m_CardItemArray[cbInsertCount++]=CardItemArray[i];
			}
		}

		//���ñ���
		m_cbCardCount=m_cbCardCount-wRemoveCount;
	}

	return true;
}

//�ƶ��˿�
bool CCardControl::MoveCardItem(BYTE cbTargerItem)
{
	//Ч�����
	ASSERT(cbTargerItem<m_cbCardCount);
	if (cbTargerItem>=m_cbCardCount) return false;

	//ͳ���˿�
	BYTE cbShootCount=0;
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			cbShootCount++;
		}
	}

	//�ƶ��˿�
	if ((cbShootCount>0)&&(cbShootCount<m_cbCardCount))
	{
		//��������
		cbTargerItem=__min(m_cbCardCount-cbShootCount,cbTargerItem);

		//��������
		BYTE cbShootIndex=cbTargerItem;
		BYTE cbNormalIndex=(cbTargerItem==0)?(cbTargerItem+cbShootCount):0;

		//��ȡ�˿�
		tagCardItem CardItemTemp[MAX_CARD_COUNT];
		CopyMemory(CardItemTemp,m_CardItemArray,sizeof(tagCardItem)*m_cbCardCount);

		//�����˿�
		for (BYTE i=0;i<m_cbCardCount;i++)
		{
			if (CardItemTemp[i].bShoot==false)
			{
				//�����˿�
				m_CardItemArray[cbNormalIndex++]=CardItemTemp[i];

				//��������
				if (cbNormalIndex==cbTargerItem) cbNormalIndex=cbTargerItem+cbShootCount;
			}
			else
			{
				//�����˿�
				m_CardItemArray[cbShootIndex++]=CardItemTemp[i];
			}
		}

		return true;
	}

	return false;
}

//�����˿�
bool CCardControl::SwitchCardItem(BYTE cbSourceItem, BYTE cbTargerItem)
{
	//Ч�����
	if (cbSourceItem==cbTargerItem) return false;
	if ((cbSourceItem>=m_cbCardCount)||(cbTargerItem>=m_cbCardCount)) return false;

	//�����˿�
	tagCardItem CardItem=m_CardItemArray[cbSourceItem];

	//�ƶ��˿�
	if (cbSourceItem>cbTargerItem)
	{
		BYTE cbMoveCount=cbSourceItem-cbTargerItem;
		MoveMemory(&m_CardItemArray[cbTargerItem+1],&m_CardItemArray[cbTargerItem],sizeof(tagCardItem)*cbMoveCount);
	}
	else
	{
		BYTE cbMoveCount=cbTargerItem-cbSourceItem;
		MoveMemory(&m_CardItemArray[cbSourceItem],&m_CardItemArray[cbSourceItem+1],sizeof(tagCardItem)*cbMoveCount);
	}

	//����Ŀ��
	m_CardItemArray[cbTargerItem]=CardItem;

	return true;
}

//��ȡ�˿�
tagCardItem * CCardControl::GetCardFromIndex(BYTE cbIndex)
{
	return (cbIndex<m_cbCardCount)?&m_CardItemArray[cbIndex]:NULL;
}

//��ȡ�˿�
tagCardItem * CCardControl::GetCardFromPoint(CPoint & MousePoint)
{
	WORD cbIndex=SwitchCardPoint(MousePoint);
	return (cbIndex!=INVALID_ITEM)?&m_CardItemArray[cbIndex]:NULL;
}

//��ȡ�˿�
BYTE CCardControl::GetCardData(BYTE cbCardData[], BYTE cbBufferCount)
{
	//Ч�����
	ASSERT(cbBufferCount>=m_cbCardCount);
	if (cbBufferCount<m_cbCardCount) return 0;

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		cbCardData[i]=m_CardItemArray[i].cbCardData;
	}

	return m_cbCardCount;
}

//��ȡ�˿�
BYTE CCardControl::GetShootCard(BYTE cbCardData[], BYTE cbBufferCount)
{
	//��������
	BYTE cbShootCount=0;

	//�����˿�
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		//Ч�����
		ASSERT(cbBufferCount>cbShootCount);
		if (cbBufferCount<=cbShootCount) break;

		//�����˿�
		if (m_CardItemArray[i].bShoot==true) cbCardData[cbShootCount++]=m_CardItemArray[i].cbCardData;
	}

	return cbShootCount;
}

//������ʾ
VOID CCardControl::SetShowCount(BYTE cbShowCount)
{
	//���ñ���
	m_cbShowCount=cbShowCount;

	return;
}

//���ñ���
VOID CCardControl::SetBackGround(BYTE cbBackGround)
{
	//���ñ���
	m_cbBackGround=cbBackGround;

	return;
}

//���þ���
VOID CCardControl::SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance)
{
	//���ñ���
	m_nXDistance=nXDistance;
	m_nYDistance=nYDistance;
	m_nShootDistance=nShootDistance;

	return;
}

//��ȡ����
VOID CCardControl::GetCenterPoint(CPoint & CenterPoint)
{
	//��ȡԭ��
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//��ȡλ��
	CSize ControlSize;
	GetControlSize(ControlSize);

	//��������
	CenterPoint.x=OriginPoint.x+ControlSize.cx/2;
	CenterPoint.y=OriginPoint.y+ControlSize.cy/2;

	return;
}

//�ɷ�λ��
VOID CCardControl::SetDispatchPos(INT nXPos, INT nYPos)
{
	//��������
	if (m_bSmallMode==false)
	{
		m_DispatchPos.x=nXPos-m_CardResource.m_CardSize.cx/2;
		m_DispatchPos.y=nYPos-m_CardResource.m_CardSize.cy/2;
	}
	else
	{
		m_DispatchPos.x=nXPos-m_CardResource.m_CardSizeSmall.cx/2;
		m_DispatchPos.y=nYPos-m_CardResource.m_CardSizeSmall.cy/2;
	}

	return;
}

//��׼λ��
VOID CCardControl::SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//���ñ���
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//ִ�ж���
VOID CCardControl::CartoonMovie()
{
	if ((m_bOutCard==true)&&(m_OutCardLapseCount.GetLapseCount(20)>0L))
	{
		//���ñ���
		m_wOutCardIndex++;

		//ֹͣ�ж�
		if ((m_cbCardCount==0)||(m_wOutCardIndex>=((m_cbCardCount-1)*DISPATCH_DELAY_INDEX+DISPATCH_MAX_INDEX)))
		{
			//���ñ���
			m_bOutCard=false;
			m_wOutCardIndex=0L;

			//������Ϣ
			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
			if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_OUT_CARD_FINISH,(WPARAM)this,0L);
		}
	}

	return;
}

//�����Ϣ
bool CCardControl::OnEventSetCursor(CPoint Point)
{
	//��괦��
	if (m_bPositively==true)
	{
		//��ȡ����
		BYTE cbHoverItem=SwitchCardPoint(Point);

		//�����ж�
		if (cbHoverItem!=INVALID_ITEM)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return true;
		}
	}

	return false;
}

//�滭�˿�
VOID CCardControl::DrawCardControl(CD3DDevice * pD3DDevice)
{
	//��ȡλ��
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//����λ��
	CSize ControlSize;
	GetControlSize(ControlSize);

	//��������
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;

	//��Դ����
	CSize CardSize;
	CD3DTexture * pD3DTexture=NULL;
	m_CardResource.GetCardSize(m_bSmallMode,CardSize);
	m_CardResource.GetCardTexture(m_bSmallMode,&pD3DTexture);

	//�滭�˿�
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		//��ȡ�˿�
		bool bShoot=m_CardItemArray[i].bShoot;
		BYTE cbCardData=m_CardItemArray[i].cbCardData;

		//��϶����
		if (i>=m_cbShowCount) continue;
		//if (cbCardData==SPACE_CARD_DATA) continue;

		//ͼƬλ��
		if ((m_bDisplayItem==true)&&(cbCardData!=0))
		{
			if ((cbCardData==0x4E)||(cbCardData==0x4F))
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)%14)*CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*CardSize.cy;
			}
			else
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*CardSize.cy;
			}
		}
		else
		{
			nYImagePos=CardSize.cy*4;
			nXImagePos=0;
		}

		//��Ļλ��
		if (m_bHorizontal==true)
		{
			nXDrawPos=m_nXDistance*i;
			nYDrawPos=(bShoot==false)?m_nShootDistance:0;
		}
		else
		{
			nXDrawPos=0;
			nYDrawPos=m_nYDistance*i;
		}

		//��Ļ���
		if (m_bOutCard==true)
		{
			//����λ��
			if (m_wOutCardIndex>=(i*DISPATCH_DELAY_INDEX))
			{
				//��������
				INT nIndex=__min(DISPATCH_MAX_INDEX,m_wOutCardIndex-(i*DISPATCH_DELAY_INDEX));

				//���λ��
				INT nXSource=m_DispatchPos.x;
				INT nYSource=m_DispatchPos.y;
				INT nXDispatch=nXSource+(OriginPoint.x+nXDrawPos-nXSource)*nIndex/DISPATCH_MAX_INDEX;
				INT nYDispatch=nYSource+(OriginPoint.y+nYDrawPos-nYSource)*nIndex/DISPATCH_MAX_INDEX;

				//�滭�˿�
				pD3DTexture->DrawImage(pD3DDevice,nXDispatch,nYDispatch,CardSize.cx,CardSize.cy,nXImagePos,nYImagePos);
			}
		}
		else
		{
			//�滭�˿�
			pD3DTexture->DrawImage(pD3DDevice,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,CardSize.cx,CardSize.cy,nXImagePos,nYImagePos);
			if(m_bShowNum && i == (m_cbCardCount-1))
				DrawNumber(pD3DDevice,&(m_CardResource.m_TextureNum),TEXT("0123456789"),m_cbCardCount,OriginPoint.x+nXDrawPos+CardSize.cx/2,OriginPoint.y+nYDrawPos+CardSize.cy/2-15,DT_CENTER);
			//�滭ѡ��
			if ( ( (m_cbHeadIndex<=i)&&(i<=m_cbTailIndex) ) || m_CardItemArray[i].bFlag)
			{
				pD3DTexture->DrawImage(pD3DDevice,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,CardSize.cx,CardSize.cy,CardSize.cx*12,CardSize.cy*4);
			}
		}
	}
	return;
}

//��ȡ��С
VOID CCardControl::GetControlSize(CSize & ControlSize)
{
	//�˿˴�С
	CSize CardSize;
	m_CardResource.GetCardSize(m_bSmallMode,CardSize);

	//��ȡ��С
	if (m_bHorizontal==true)
	{
		ControlSize.cy=CardSize.cy+m_nShootDistance;
		ControlSize.cx=(m_cbCardCount>0)?(CardSize.cx+(m_cbCardCount-1)*m_nXDistance):0;
	}
	else
	{
		ControlSize.cx=CardSize.cx;
		ControlSize.cy=(m_cbCardCount>0)?(CardSize.cy+(m_cbCardCount-1)*m_nYDistance):0;
	}

	return;
}

//��ȡԭ��
VOID CCardControl::GetOriginPoint(CPoint & OriginPoint)
{
	//��ȡλ��
	CSize ControlSize;
	GetControlSize(ControlSize);

	//����λ��
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ OriginPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//����λ��
	switch (m_YCollocateMode)
	{
	case enYTop:	{ OriginPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	return;
}

//����ת��
BYTE CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//����ģʽ
	if ((m_bHorizontal==true)&&(m_cbCardCount>0))
	{
		//��ȡλ��
		CSize ControlSize;
		CPoint OriginPoint;
		GetControlSize(ControlSize);
		GetOriginPoint(OriginPoint);

		//�˿˴�С
		CSize CardSize;
		m_CardResource.GetCardSize(m_bSmallMode,CardSize);

		//��׼λ��
		INT nXPos=MousePoint.x-OriginPoint.x;
		INT nYPos=MousePoint.y-OriginPoint.y;

		//Խ���ж�
		if ((nXPos<0)||(nXPos>ControlSize.cx)) return INVALID_ITEM;
		if ((nYPos<0)||(nYPos>ControlSize.cy)) return INVALID_ITEM;

		//��������
		BYTE cbCardIndex=nXPos/m_nXDistance;
		if (cbCardIndex>=m_cbCardCount) cbCardIndex=(m_cbCardCount-1);

		//�˿�����
		for (BYTE i=0;i<=cbCardIndex;i++)
		{
			//��������
			BYTE cbCurrentIndex=cbCardIndex-i;

			//��ʾ�ж�
			if (cbCurrentIndex>=m_cbShowCount) continue;

			//�������
			if (nXPos>(INT)(cbCurrentIndex*m_nXDistance+CardSize.cx)) break;

			//�������
			bool bShoot=m_CardItemArray[cbCurrentIndex].bShoot;
			if ((bShoot==true)&&(nYPos<=CardSize.cy)) return cbCurrentIndex;
			if ((bShoot==false)&&(nYPos>=(INT)m_nShootDistance)) return cbCurrentIndex;
		}
	}

	return INVALID_ITEM;
}

//������Դ
VOID CCardControl::LoadCardResource(CD3DDevice * pD3DDevice)
{
	//������Դ
	m_CardResource.Initialize(pD3DDevice);

	return;
}

// �滭����
VOID CCardControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// �滭����
VOID CCardControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// �滭����
VOID CCardControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
	// ������Դ
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

bool CCardControl::SetFlag(BYTE cbIndex)
{
	m_CardItemArray[cbIndex].bFlag=true;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
