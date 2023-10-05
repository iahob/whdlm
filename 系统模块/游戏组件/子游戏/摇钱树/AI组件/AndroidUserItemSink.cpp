#include "Stdafx.h"
#include "AndroidUserItemSink.h"

// ������Ϣ
struct tagCatchInfo
{
	bool						bFishTrack;				// ׷����
	uint						nFishKey;				// ��ؼ�ֵ
	uint						nFishType;				// ������
};

// ����������
bool CatchInfoCompare( tagCatchInfo & TCatchInfoOne, tagCatchInfo & TCatchInfoTwo )
{
	return TCatchInfoOne.bFishTrack || TCatchInfoTwo.nFishType < TCatchInfoOne.nFishType;
}

//////////////////////////////////////////////////////////////////////////
#define  GLCOLOR_RGB(r,g,b)	    ((COLORREF)((((255)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

// ��Ϸʱ��
#define IDI_UPDATA					(1)									// ���¶�ʱ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	// ������Ϣ
	m_wMeChairID = INVALID_CHAIR;
	m_lAndroidScore = 0;
	m_lCooling = 0;
	m_lCoolingTime = 0;
	m_nTrackFishIndex = int_max;
	m_bFreeFire = rand()%2 == 0;
	m_nFireTime = rand()%1000 + 300;
	m_nBulletKey = 0;
	m_lAndroidScoreMax=0;
	// �ʱ��
	m_dwSecondTime = 0;
	m_nLaserTime = 0;
	m_nSpeedTime = 0;
	m_nSceneTime = 0;
	m_nScoreRatio=1;
	// ���λ��
	m_PointPlay[S_TOP_LEFT].x = 403;
	m_PointPlay[S_TOP_LEFT].y = 64;
	m_PointPlay[S_TOP_RIGHT].x = 878;
	m_PointPlay[S_TOP_RIGHT].y = 64;
	m_PointPlay[S_BOTTOM_LEFT].x = 403;
	m_PointPlay[S_BOTTOM_LEFT].y = 736;
	m_PointPlay[S_BOTTOM_RIGHT].x = 878;
	m_PointPlay[S_BOTTOM_RIGHT].y = 736;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	m_FishInfo.RemoveAll();
	m_FishInfo.FreeMemory();
	m_BulletInfo.RemoveAll();
	m_BulletInfo.FreeMemory();
	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	// ���¶�ʱ
	if ( nTimerID == IDI_UPDATA )
	{
		// ������
		bool bHaveTrack = false;
		int nTrackFishIndex = int_max; 
		byte cbTrackFishType = 255;
		CPoint PointFish( rand()%DEFAULE_WIDTH, rand()%DEFAULE_HEIGHT );

		// ��ȡ����ʱ��
		uint dwCurrentTime = GetTickCount();

		// ������λ��
		for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); )
		{
			// ��ȡ����Ϣ
			tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );
			
			// �жϴ��
			dword dwCurrentTime = GetTickCount();

			// ����ʱ��
			uint unGenerateTime = (dwCurrentTime - nStartTime);

			// δ������ʱ��
			if ( unGenerateTime < TFishAndroid.unCreateTime )
			{
				nFishIndex++;

				continue;
			}

			// ��ǰͣ��ʱ��
			uint nBeforeStayTime = 0;

			// ����ʱ��
			for( int nStayIndex = 0; nStayIndex < TFishAndroid.ArrayStayInfo.GetCount(); ++nStayIndex )
			{
				// ��ȡ��Ϣ
				tagStayInfo & nStayInfo = TFishAndroid.ArrayStayInfo.GetAt(nStayIndex);

				// ��ȡʱ��
				uint nStayStart = nStayInfo.nStayStart;				
				uint nStayTime = nStayInfo.nStayTime;

				// �Ƚ���Ϣ
				if( unGenerateTime <= nStayStart )
				{
					break;
				}
				else if( unGenerateTime > nStayStart && unGenerateTime < nStayStart + nStayTime )
				{
					unGenerateTime = nStayStart;

					break;
				}
				else if ( unGenerateTime >= nStayStart + nStayTime )
				{
					nBeforeStayTime += nStayTime;
				}
			}

			// ��ȥʱ��
			unGenerateTime -= nBeforeStayTime;

			// �ζ�����
			int  nMoveIndex = 0;
			bool bSuccess = false;

			// ������Ϣ
			ASSERT( unGenerateTime >= TFishAndroid.unCreateTime );
			unGenerateTime = max(unGenerateTime, TFishAndroid.unCreateTime);

			// �ζ�ʱ��
			uint unSwimTime = unGenerateTime - TFishAndroid.unCreateTime;
			uint unSwimCurrent = unSwimTime;
			uint unPhaseTime = 0;

			// ��ʱ��
			uint unTotalTime = 0;

			// ������ʱ��
			for ( int nBezierIndex = 0; nBezierIndex < TFishAndroid.nBezierCount ; ++nBezierIndex )
			{
				// ��ȡ��Ϣ
				tagBezierPoint * pBezierPoint = &TFishAndroid.TBezierPoint[nBezierIndex];

				// ���ʱ��
				unTotalTime += pBezierPoint->Time;
			}

			// ��ȡ�ζ�����
			for ( int nBezierIndex = 0; nBezierIndex < TFishAndroid.nBezierCount ; ++nBezierIndex )
			{
				// ��ȡ��Ϣ
				tagBezierPoint * pBezierPoint = &TFishAndroid.TBezierPoint[nBezierIndex];

				// ·����ʱ
				unPhaseTime += pBezierPoint->Time;

				// ���ҵ�ǰ·��
				if ( unSwimTime <= unPhaseTime )
				{
					// ����·��
					nMoveIndex = nBezierIndex;

					// ���ҳɹ�
					bSuccess = true;

					break;
				}

				// ���㵱ǰʱ��
				unSwimCurrent -= pBezierPoint->Time;
			}

			// �쳣����
			if ( !bSuccess )
			{
				// �����ζ�
				m_FishInfo.RemoveAt( nFishIndex );
				continue;
			}

			// ���㱴����
			double dProportion = (double)(unSwimCurrent)/(double)(TFishAndroid.TBezierPoint[nMoveIndex].Time);
			CDoublePoint ptPosition(0.0, 0.0);
			ptPosition = PointOnCubicBezier( &TFishAndroid.TBezierPoint[nMoveIndex], dProportion );

			// ��¼�ϴ�
			TFishAndroid.ptLast = TFishAndroid.ptPosition;

			// ������ת
			TFishAndroid.ptPosition = Rotate(TFishAndroid.TBezierPoint[0].BeginPoint, TFishAndroid.fRotateAngle, ptPosition );

			// ƫ��
			TFishAndroid.ptPosition.x += TFishAndroid.PointOffSet.x;
			TFishAndroid.ptPosition.y += TFishAndroid.PointOffSet.y;

			// ��Чλ����
			if ( TFishAndroid.ptPosition.x > 0 && TFishAndroid.ptPosition.x < DEFAULE_WIDTH 
				&& TFishAndroid.ptPosition.y > 0 && TFishAndroid.ptPosition.y < DEFAULE_HEIGHT && m_bStopFire==false)
			{
				// ����׷��
				if ( m_nTrackFishIndex == int_max )
				{
					if ( ( cbTrackFishType == 255 || TFishAndroid.nFishType == FishType_BaoXiang || (cbTrackFishType < TFishAndroid.nFishType && rand()%4 == 0) )
						&& ( TFishAndroid.wHitChair == INVALID_CHAIR || TFishAndroid.wHitChair == m_wMeChairID ) )
					{
						// ������Ϣ
						nTrackFishIndex = TFishAndroid.nFishKey;
						cbTrackFishType = TFishAndroid.nFishType;
						PointFish.SetPoint( (int)TFishAndroid.ptPosition.x, (int)TFishAndroid.ptPosition.y );

						// ��Ч����
						bHaveTrack = true;
					}
				}
				else if( m_nTrackFishIndex == TFishAndroid.nFishKey )
				{
					// ������Ϣ
					nTrackFishIndex = TFishAndroid.nFishKey;
					cbTrackFishType = TFishAndroid.nFishType;
					PointFish.SetPoint( (int)TFishAndroid.ptPosition.x, (int)TFishAndroid.ptPosition.y );

					// ��Ч����
					bHaveTrack = true;
				}
			}

			++nFishIndex;

			continue;
		}

		// ������ȴ
		if ( m_lCooling > 0 && (long)( GetTickCount() - m_lCoolingTime ) >= m_lCooling )
		{
			m_lCooling = 0;
		}
		
		// ������
		long lBulletInvest = m_nMultipleValue[m_nMultipleIndex] * QianPao_Bullet;

		// ûǮ���뿪
		if ( m_lAndroidScore < lBulletInvest)
		{
			int nMultipleIndex = m_nMultipleIndex;
			while(m_lAndroidScore<m_nMultipleValue[nMultipleIndex])
			{
				if(nMultipleIndex>0)	nMultipleIndex--;
				else
				{		
					//������ʱ��
					m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
					m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
					m_bStopFire = true;
					ReSetScore();
					return true;
				}
			}

			//�޸ı���			
			ModifyMultiple(false, nMultipleIndex );
		}

		// ����׷��
		m_nTrackFishIndex = nTrackFishIndex;

		// ��Ч�ӵ�
		if ( bHaveTrack && dwCurrentTime - m_nSceneTime > 7000 && m_lCooling == 0 && m_nTrackFishIndex != int_max && m_lAndroidScore >= lBulletInvest && m_bStopFire==false)
		{
			// ����Ƕ�
			float fRadian = atan2((float)(m_PointPlay[m_wMeChairID].y - PointFish.y) , (float)(PointFish.x - m_PointPlay[m_wMeChairID].x));

			// ��ʽ�Ƕ�
			if( m_wMeChairID == S_BOTTOM_LEFT || m_wMeChairID == S_BOTTOM_RIGHT )
			{
				// ��Χ��ʽ
				if ( fRadian <= -GL_PI / 2.f )
				{
					fRadian = GL_PI;
				}
				else if ( fRadian < 0.00f )
				{
					fRadian = 0.00f;
				}

				// ��ʽ��Χ
				fRadian = -fRadian;
			}
			else
			{
				fRadian = -fRadian;
			}

			// �������
			CDoublePoint PointAim = Rotate(CDoublePoint(m_PointPlay[m_wMeChairID]), fRadian, CDoublePoint( m_PointPlay[m_wMeChairID].x + 1000, m_PointPlay[m_wMeChairID].y));

			// ��ס���
			m_lAndroidScore -= lBulletInvest;

			// �ؼ�ֵ����
			m_nBulletKey++;
			if( m_nBulletKey == int_max )
				m_nBulletKey = 0;

			// ������ȴʱ��

			if(cbTrackFishType!=255 && cbTrackFishType>=FishType_Moderate_Big_Max)
				m_lCooling = m_nBulletCoolingTime;
			else
				m_lCooling = m_nBulletCoolingTime*(rand()%3+1);			
			m_lCoolingTime = GetTickCount();

			// �ٶ�ǿ��
			if ( m_nSpeedTime > 0 )
			{
				m_lCooling /= 2;
			}

			// ׷������
			int nTrackFishIndex = m_bFreeFire ? int_max : m_nTrackFishIndex;

			// ������Ϣ
			CMD_C_Fire Fire;
			Fire.nMultipleIndex = m_nMultipleIndex;
			Fire.nTrackFishIndex = nTrackFishIndex;
			Fire.nBulletKey = m_nBulletKey;
			Fire.ptPos.SetPoint( (SHORT)PointAim.x, (SHORT)PointAim.y );

			// ������Ϣ
			m_pIAndroidUserItem->SendSocketData( SUB_C_FIRE, &Fire, sizeof(Fire));

			// ׷����̨
			if( !m_bFreeFire )
			{
				// ��ӵ��ӵ�
				tagBulletAndroid TBulletAndroid;
				TBulletAndroid.nBulletKey = m_nBulletKey;
				TBulletAndroid.nTrackFishIndex = nTrackFishIndex;
				TBulletAndroid.ptPosition = m_PointPlay[m_wMeChairID];
				TBulletAndroid.ptOffset.SetPoint( 0, 0 );
				TBulletAndroid.bEjection = false;
				TBulletAndroid.bSpecialt = m_nSpeedTime > 0;
				TBulletAndroid.SPBullet.SetAsBox( 2.f, 2.f );
				m_BulletInfo.Add( TBulletAndroid );
			}
			else
			{
				// �������
				double dFireDistance = sqrt(pow(abs(m_PointPlay[m_wMeChairID].x - PointFish.x),2) + pow(abs(m_PointPlay[m_wMeChairID].y - PointFish.y),2));

				// �����ٶ�
				double dBulletVelocity = m_nBulletVelocity;

				// ����ǿ��
				if ( m_nSpeedTime > 0 )
					dBulletVelocity *= 2.0;

				// ����ʱ��
				uint unTime = static_cast<uint>( abs(dFireDistance) / (dBulletVelocity/1000.0));

				// ��ӵ��ӵ�
				tagBulletAndroid TBulletAndroid;
				TBulletAndroid.nBulletKey = m_nBulletKey;
				TBulletAndroid.nTrackFishIndex = int_max;
				TBulletAndroid.ptPosition = m_PointPlay[m_wMeChairID];
				TBulletAndroid.ptOffset.SetPoint( (double)(PointFish.x - m_PointPlay[m_wMeChairID].x) / (double)unTime, (double)(PointFish.y - m_PointPlay[m_wMeChairID].y) / (double)unTime );
				TBulletAndroid.bEjection = false;
				TBulletAndroid.bSpecialt = m_nSpeedTime > 0;
				TBulletAndroid.SPBullet.SetAsBox( 2.f, 2.f );
				m_BulletInfo.Add( TBulletAndroid );
			}
		}

		// �����ӵ��ƶ�
		for( int nBulletIndex = 0; nBulletIndex < m_BulletInfo.GetCount(); )
		{
			// ��ȡ�ӵ���Ϣ
			tagBulletAndroid & TBulletAndroid = m_BulletInfo.GetAt( nBulletIndex );

			// �ӵ�����
			bool bBulletCatch = false;

			// ������
			CArray< tagCatchInfo > ArrayCatchInfo;

			// ��Ч׷��
			if ( TBulletAndroid.nTrackFishIndex != int_max )
			{
				// �ж��ӵ�����
				bool bValidTrack = false;
				uint nTrackFishKey = 0;
				byte nTrackFishType = 0;
				CDoublePoint ptFishPosition;
				for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
				{
					// ��ȡ����Ϣ
					tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );

					// �ж���
					if( TBulletAndroid.nTrackFishIndex == TFishAndroid.nFishKey 
						&& TFishAndroid.ptPosition.x > 0 && TFishAndroid.ptPosition.x < DEFAULE_WIDTH 
						&& TFishAndroid.ptPosition.y > 0 && TFishAndroid.ptPosition.y < DEFAULE_HEIGHT )
					{
						bValidTrack = true;
						nTrackFishKey = TFishAndroid.nFishKey;
						nTrackFishType = TFishAndroid.nFishType;
						ptFishPosition = TFishAndroid.ptPosition;
					}
				}

				// ��׷��
				if ( bValidTrack )
				{
					// �ƶ�����
					double dFireDistance = sqrt(pow(abs( TBulletAndroid.ptPosition.x - ptFishPosition.x ), 2) + pow(abs(TBulletAndroid.ptPosition.y - ptFishPosition.y), 2));

					// �����ٶ�
					double dBulletVelocity = m_nBulletVelocity;

					// ����ǿ��
					if ( TBulletAndroid.bSpecialt )
						dBulletVelocity *= 2.0;

					// �ƶ�ʱ��
					uint unTime = static_cast<uint>( abs(dFireDistance) / (dBulletVelocity/1000.0) );

					// ��Ҫ�ƶ�
					if ( unTime != 0 )
					{
						CDoublePoint ptOffset( (ptFishPosition.x - TBulletAndroid.ptPosition.x)/unTime, (ptFishPosition.y - TBulletAndroid.ptPosition.y)/unTime );
						TBulletAndroid.ptPosition += ptOffset;
					}

					// ����
					if ( unTime == 0 )
					{
						// ������Ϣ
						tagCatchInfo TCatchInfo;
						TCatchInfo.bFishTrack = true;
						TCatchInfo.nFishKey = nTrackFishKey;
						TCatchInfo.nFishType = nTrackFishType;

						// ��Ӳ���
						ArrayCatchInfo.Add( TCatchInfo );

						// ���ò���
						bBulletCatch = true;
					}
				}

				// ��׷�٣� ֱ��ɢ��
				if ( !bValidTrack )
				{
					bBulletCatch = true;
				}
			}
			// ��׷��
			else
			{	
				// �ƶ�λ��
				TBulletAndroid.ptPosition += TBulletAndroid.ptOffset;

				// �����ӵ�
				if( TBulletAndroid.ptPosition.x <= 0.0 || TBulletAndroid.ptPosition.x >= DEFAULE_WIDTH )
				{
					// ���÷���
					TBulletAndroid.ptOffset.x = -TBulletAndroid.ptOffset.x;

					// ���÷���
					TBulletAndroid.bEjection = true;
				}
				if( TBulletAndroid.ptPosition.y <= 0.0 || TBulletAndroid.ptPosition.y >= DEFAULE_HEIGHT )
				{
					// ���÷���
					TBulletAndroid.ptOffset.y = -TBulletAndroid.ptOffset.y;

					// ���÷���
					TBulletAndroid.bEjection = true;
				}

				// �ж��ӵ�����
				for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
				{
					// ��ȡ����Ϣ
					tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );

					// �ж���
					if( TFishAndroid.ptPosition.x > 0 && TFishAndroid.ptPosition.x < DEFAULE_WIDTH 
						&& TFishAndroid.ptPosition.y > 0 && TFishAndroid.ptPosition.y < DEFAULE_HEIGHT )
					{
						// ���С
						float fRotationAngle = (float)atan2((TFishAndroid.ptPosition.y - TFishAndroid.ptLast.y) , (TFishAndroid.ptLast.x - TFishAndroid.ptPosition.x));
						fRotationAngle = -fRotationAngle - GL_PI / 2;
						TFishAndroid.FTFish.p.Set( (float)TFishAndroid.ptPosition.x, (float)TFishAndroid.ptPosition.y );
						TFishAndroid.FTFish.q.Set( fRotationAngle );

						// �ӵ�λ��
						TBulletAndroid.FTBullet.p.Set( (float)TBulletAndroid.ptPosition.x, (float)TBulletAndroid.ptPosition.y );
						TBulletAndroid.FTBullet.q.Set( 0.f );

						// ����λ��
						if( ShapeOverlap( &TFishAndroid.PSFish, &TBulletAndroid.SPBullet, TFishAndroid.FTFish, TBulletAndroid.FTBullet ) )
						{
							// ������Ϣ
							tagCatchInfo TCatchInfo;
							TCatchInfo.bFishTrack = false;
							TCatchInfo.nFishKey = TFishAndroid.nFishKey;
							TCatchInfo.nFishType = TFishAndroid.nFishType;

							// ��Ӳ���
							ArrayCatchInfo.Add( TCatchInfo );

							// ���ò���
							bBulletCatch = true;

							break;
						}
					}
				}
			}

			// ����
			if ( bBulletCatch && ArrayCatchInfo.GetCount() )
			{
				// ������Ϣ
				std::sort( ArrayCatchInfo.GetData(), ArrayCatchInfo.GetData() + ArrayCatchInfo.GetSize(), CatchInfoCompare );

				// ������Ϣ
				CMD_C_CatchFish CMDCCatchFish;
				ZeroMemory( &CMDCCatchFish, sizeof(CMDCCatchFish) );
				CMDCCatchFish.nBulletKey = TBulletAndroid.nBulletKey;
				for( int nFishIndex = 0; nFishIndex < FishCatch_Max && nFishIndex < ArrayCatchInfo.GetCount(); ++nFishIndex )
				{
					// ��ȡ��Ϣ
					tagCatchInfo & TCatchInfo = ArrayCatchInfo.GetAt( nFishIndex );

					// ��ֵ��Ϣ
					CMDCCatchFish.nFishKey[nFishIndex] = TCatchInfo.nFishKey;
				}

				m_pIAndroidUserItem->SendSocketData( SUB_C_CATCH_FISH, &CMDCCatchFish, sizeof(CMDCCatchFish));
			}

			//========���������Ϣ
			ArrayCatchInfo.RemoveAll();

			// ����
			if ( bBulletCatch )
			{
				// ɾ��
				m_BulletInfo.RemoveAt(nBulletIndex);

				continue;
			}

			// ��һ��
			++nBulletIndex;
			continue;
		}


		// ����
		if (GetTickCount() - m_dwSecondTime >= 1000)
		{
			// ����ʱ��
			m_dwSecondTime += 1000;

			// �������
			if( m_nFireTime > 0 )
			{
				m_nFireTime--;

				if ( m_nFireTime == 0 )
				{
					// �������
					m_bFreeFire = rand()%3 >= 1;
					m_nFireTime = rand()%1000 + 300;
				}
			}
		}

		//���ڼ��
		if(long(GetTickCount()-m_lStopFireTime)>m_lStopFireModify)
		{
			m_lStopFireTime = GetTickCount();
			//ֹͣ
			if(m_bStopFire==false)
			{
				m_bStopFire=true;
				m_lStopFireModify = ((rand() % 5) + 1) * 1000;
				//ͣ���ʱ��������
				m_nTrackFishIndex = int_max;
			}
			else if(m_bStopFire)
			{
				m_bStopFire=false;
				m_lStopFireModify = ((rand() % 16) + 15) * 1000;
			}
		}
		
		// �л�����
		ModifyMultiple(true);

		//AI���������޶�ֵ �˳���Ϸ
		if(m_lAndroidScore>=m_lAndroidScoreMax)
		{
			//������ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
			m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
			m_bStopFire = true;
			ReSetScore();
		}
		// ��Ӷ�ʱ��
		m_pIAndroidUserItem->KillGameTimer( IDI_UPDATA );
		m_pIAndroidUserItem->SetGameTimer( IDI_UPDATA, 1 );

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_S_SYNCHRONOUS:
		return OnSubSynchronizationTime(pBuffer, wDataSize);
	case SUB_S_FISH_CREATE:
		return OnSubCreateFish(pBuffer, wDataSize);
	case SUB_S_FISH_CATCH:
		return OnSubCatchFish(pBuffer, wDataSize);
	case SUB_S_EXCHANGE_SCENE:
		return OnSubExchangeScene(pBuffer, wDataSize);
	case SUB_S_DELAY_BEGIN:
		return m_pIAndroidUserItem->SendSocketData(SUB_C_DELAY, NULL, NULL);
	case SUB_S_SUPPLY:
		return OnSubSupply(pBuffer, wDataSize);
	case SUB_S_MULTIPLE:
		{
			// Ч������
			ASSERT( wDataSize == sizeof(CMD_S_Multiple) );
			if ( wDataSize != sizeof(CMD_S_Multiple) ) return false;
			
			// ��ȡ��Ϣ
			CMD_S_Multiple * pMultiple = (CMD_S_Multiple*) pBuffer;
			//���������ɹ�
			if(pMultiple->wChairID == m_wMeChairID)
			{
				m_nMultipleIndex = pMultiple->nMultipleIndex;
			}
			return true;
		}
	case SUB_S_ANDROID_CONFIG:
		{
			// Ч������
			ASSERT( wDataSize == sizeof(CMD_S_AndriodConfig) );
			if ( wDataSize != sizeof(CMD_S_AndriodConfig) ) return false;

			// ��ȡ��Ϣ
			CMD_S_AndriodConfig * pConfig = (CMD_S_AndriodConfig*) pBuffer;
			CopyMemory(m_lAndroidAreaScore,pConfig->lAndroidScore,sizeof(m_lAndroidAreaScore));
			CopyMemory(m_nAndroidMultiple,pConfig->nAndroidMultiple,sizeof(m_nAndroidMultiple));
			m_lAndroidScoreMax=pConfig->lAndroidScoreMax;
			if(m_lAndroidScoreMax<=0) m_lAndroidScoreMax=5000000;
			
			//�޸ı���
			ModifyMultiple(false);

			return true;
		}
	case SUB_S_ANDROID_LEAVE://AI�볡
		{
			//������ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
			m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
			m_bStopFire = true;
			ReSetScore();
			return true;
		}

	}

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:	
	case GAME_STATUS_PLAY:
		{
			// Ч������
			ASSERT( wDataSize == sizeof(GameScene) );
			if ( wDataSize != sizeof(GameScene) ) return false;

			// ��Ϣ����
			GameScene * pGameScene = (GameScene *)pData;

			// ����ʱ��
			m_wMeChairID = m_pIAndroidUserItem->GetChairID();

			// ����ʱ��
			nStartTime = GetTickCount();

			// �Լ�����
			m_lAndroidScore = pGameScene->lPlayScore;
			m_nScoreRatio=pGameScene->lCellScore;
			if(m_nScoreRatio>=-1 && m_nScoreRatio<=1) m_nScoreRatio=1;

			// ���±���
			CopyMemory(m_nMultipleValue, pGameScene->nMultipleValue, sizeof(m_nMultipleValue));
			m_nMultipleIndex = pGameScene->nMultipleIndex[m_wMeChairID];
			
			// ��Ҫ�ӵ�����
			m_nBulletCoolingTime = pGameScene->nBulletCoolingTime;
			m_nBulletVelocity = pGameScene->nBulletVelocity;

			// ��Ӷ�ʱ��
			m_pIAndroidUserItem->KillGameTimer( IDI_UPDATA );
			m_pIAndroidUserItem->SetGameTimer( IDI_UPDATA, 60 );

			// ����ʱ��
			m_dwSecondTime = GetTickCount();

			// �������
			m_bFreeFire = rand()%3 >= 1;
			m_nFireTime = rand()%1000 + 300;

			//m_nSpeedTime = pGameScene->nSpeedPlayTime[m_wMeChairID];
			//m_nLaserTime = pGameScene->nLaserPlayTime[m_wMeChairID];

			// ����ʱ��
			m_nMultipleTime = GetTickCount();
			m_nMultipleModify = ((rand() % 60) + 60*2) * 1000;

			m_lStopFireTime = GetTickCount();
			m_lStopFireModify = ((rand() % 13) + 3) * 1000;
			m_bStopFire = false;
			
			//�޸ı���
			ModifyMultiple(false);

			// ����ʱ��
			m_nSceneTime = GetTickCount();

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�û�����
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	// �ж��û�
	if( bLookonUser )
		return;

	// �ж��Լ�
	if ( pIAndroidUserItem->GetUserID() == m_pIAndroidUserItem->GetUserID() )
	{
		// �Լ�����
		m_lAndroidScore = pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	}

	return;
}

//�û��뿪
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

// ͬ����Ϣ
bool CAndroidUserItemSink::OnSubSynchronizationTime(const void * pBuffer, word wDataSize)
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_Synchronous) );
	if ( wDataSize != sizeof(CMD_S_Synchronous) ) return false;

	// ��ȡ��Ϣ
	CMD_S_Synchronous * pSynchronous = (CMD_S_Synchronous*) pBuffer;

	// ʱ��ƫ��
	ASSERT( nStartTime > pSynchronous->nOffSetTime );
	nStartTime -= pSynchronous->nOffSetTime;

	return true;
}

// ���㴴��
bool CAndroidUserItemSink::OnSubCreateFish( const void * pBuffer, word wDataSize )
{
	// Ч������
	ASSERT( wDataSize%sizeof(CMD_S_FishCreate) == 0 && wDataSize > 0 );
	if ( wDataSize%sizeof(CMD_S_FishCreate) != 0 || wDataSize == 0 ) return false;

	// ���ü�¼
	word wRecordCount = wDataSize / sizeof(CMD_S_FishCreate);
	for (word wIndex = 0; wIndex < wRecordCount; wIndex++ ) 
	{
		CMD_S_FishCreate * pFishCreate = (((CMD_S_FishCreate *)pBuffer)+wIndex);
		
		// �����
		tagFishAndroid TFishAndroid;
		TFishAndroid.nFishKey = pFishCreate->nFishKey;
		TFishAndroid.nFishType = pFishCreate->nFishType;
		TFishAndroid.wHitChair = pFishCreate->wHitChair;
		TFishAndroid.nFishState = pFishCreate->nFishState;
		TFishAndroid.unCreateTime = pFishCreate->unCreateTime;
		TFishAndroid.fRotateAngle = pFishCreate->fRotateAngle;
		TFishAndroid.PointOffSet = pFishCreate->PointOffSet;
		TFishAndroid.nBezierCount = pFishCreate->nBezierCount;
		CopyMemory( TFishAndroid.TBezierPoint, pFishCreate->TBezierPoint, sizeof(tagBezierPoint) * pFishCreate->nBezierCount );
		TFishAndroid.PSFish.Set( MonsterVec[TFishAndroid.nFishType], MONSTER_VEC );

		// �ж�����
		if ( TFishAndroid.nFishState != FishState_Normal )
		{
			TFishAndroid.PSFish.Set( KnifeVec, MONSTER_VEC );
		}
		else
		{
			// ��������
			TFishAndroid.PSFish.Set( MonsterVec[TFishAndroid.nFishType], MONSTER_VEC );
		}

		// �����
		m_FishInfo.Add(TFishAndroid);
	}

	return true;
}


// �㲶��
bool CAndroidUserItemSink::OnSubCatchFish( const void * pBuffer, word wDataSize )
{
	// Ч������
	ASSERT( wDataSize%sizeof(CMD_S_CatchFish) == 0 && wDataSize > 0 );
	if ( wDataSize%sizeof(CMD_S_CatchFish) != 0 || wDataSize == 0 ) return false;

	// ɾ����
	word wCatchFishCount = wDataSize / sizeof(CMD_S_CatchFish);
	for (word wIndex = 0; wIndex < wCatchFishCount; wIndex++ ) 
	{
		CMD_S_CatchFish * pCatchFish = (((CMD_S_CatchFish *)pBuffer)+wIndex);
		for ( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
		{
			// ������Ϣ
			tagFishAndroid & TFish = m_FishInfo.GetAt( nFishIndex );
			
			// ��������
			if ( TFish.nFishKey == pCatchFish->nFishKey )
			{
				// �Լ�����
				if ( pCatchFish->wChairID == m_pIAndroidUserItem->GetChairID() )
				{
					m_lAndroidScore += pCatchFish->lScoreCount;
				}

				// ɾ����
				m_FishInfo.RemoveAt(nFishIndex);
				break;
			}
		}
	}

	return true;
}

// ����������ƶ�
bool CAndroidUserItemSink::OnSubExchangeScene( const void * pBuffer, WORD wDataSize )
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ExchangeScene));
	if (wDataSize!=sizeof(CMD_S_ExchangeScene)) return false;

	// ʱ��ͬ��
	nStartTime = GetTickCount();

	// ɾ��������
	m_FishInfo.RemoveAll();
	m_nTrackFishIndex = int_max;
	// ����ʱ��
	m_nSceneTime = GetTickCount();

	return true;
}

// ���Ϣ
bool CAndroidUserItemSink::OnSubSupply( const void * pBuffer, WORD wDataSize )
{
	// Ч������
	ASSERT( wDataSize == sizeof(CMD_S_Supply) );
	if ( wDataSize != sizeof(CMD_S_Supply) ) return false;

	// ��Ϣ����
	CMD_S_Supply * pSupply = (CMD_S_Supply *)pBuffer;

	// ִ�в���
	if( pSupply->wChairID == m_wMeChairID && pSupply->nSupplyType == EST_Speed )
	{
		// �������
		m_nSpeedTime = (int)pSupply->lSupplyCount;
	}

	return true;
}

// ͣ����
bool CAndroidUserItemSink::OnSubStayFish( const void * pData, WORD wDataSize )
{
	// Ч������
	ASSERT(wDataSize==sizeof(CMD_S_StayFish));
	if (wDataSize!=sizeof(CMD_S_StayFish)) return false;

	// �������
	CMD_S_StayFish * pStayFish = (CMD_S_StayFish*)pData;

	// ����ɾ��
	for ( int nFishIndex = 0 ; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
	{
		// ��ȡ�����Ϣ
		tagFishAndroid & TFishAndroid = m_FishInfo.GetAt(nFishIndex);

		// �ж�ɾ����
		if ( TFishAndroid.nFishKey != pStayFish->nFishKey )
		{
			continue;
		}

		// ��Ч����
		if( pStayFish->nStayStart < TFishAndroid.unCreateTime )
		{
			ASSERT( FALSE );
			return true;
		}

		// ��ȡ������
		CArrayStay & ArrayStayInfo = TFishAndroid.ArrayStayInfo;

		// �Ա����һ��
		if ( ArrayStayInfo.GetCount() > 0 )
		{
			// ��ȡ���һ��
			tagStayInfo & nStaylast = ArrayStayInfo.GetAt( ArrayStayInfo.GetCount() - 1 );

			// ��ͬʱ��
			if ( nStaylast.nStayStart == pStayFish->nStayStart )
			{
				// ����ʱ��
				nStaylast.nStayTime = pStayFish->nStayTime;

				return true;
			}
		}

		// ������Ϣ
		tagStayInfo nStayInfo;
		nStayInfo.nStayStart = pStayFish->nStayStart;
		nStayInfo.nStayTime = pStayFish->nStayTime;

		// �����Ϣ
		ArrayStayInfo.Add(nStayInfo);

		// ������Ϣ
		return true;
	}

	// ������Ϣ
	return true;
}

// ��ת��
CDoublePoint CAndroidUserItemSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome ) 
{ 
	CDoublePoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x; 
	temp.y = ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y; 
	return temp; 
} 

// ����������
CDoublePoint CAndroidUserItemSink::PointOnCubicBezier( tagBezierPoint* cp, double t )
{
	double   ax = 0.f, bx = 0.f, cx = 0.f;
	double   ay = 0.f, by = 0.f, cy = 0.f;
	double   tSquared = 0.f, tCubed = 0.f;
	CDoublePoint result;

	// ����ϵ��
	cx = 3.f * (cp->KeyOne.x - cp->BeginPoint.x);
	bx = 3.f * (cp->KeyTwo.x - cp->KeyOne.x) - cx;
	ax = cp->EndPoint.x - cp->BeginPoint.x - cx - bx;

	cy = 3.f * (cp->KeyOne.y - cp->BeginPoint.y);
	by = 3.f * (cp->KeyTwo.y - cp->KeyOne.y) - cy;
	ay = cp->EndPoint.y - cp->BeginPoint.y - cy - by;

	// �������ߵ�
	tSquared = t * t;
	tCubed = tSquared * t;

	result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp->BeginPoint.x;
	result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp->BeginPoint.y;

	return result;
}

// AI����ѡ��
int CAndroidUserItemSink::SelectMultiple(bool bChange)
{
	//����ȷ��
	int nMultipleValue = 0;
	if(m_lAndroidScore<=m_lAndroidAreaScore[0]) nMultipleValue=m_nAndroidMultiple[0];
	else if(m_lAndroidScore<=m_lAndroidAreaScore[1]) nMultipleValue=m_nAndroidMultiple[1];
	else if(m_lAndroidScore<=m_lAndroidAreaScore[2]) nMultipleValue=m_nAndroidMultiple[2];
	else if(m_lAndroidScore<=m_lAndroidAreaScore[3]) nMultipleValue=m_nAndroidMultiple[3];	
	else nMultipleValue=m_nAndroidMultiple[4];

	int nMultipleIndex=0;
	for(int i=0;i<Multiple_Max;i++)
	{
		if(nMultipleValue<m_nMultipleValue[i]) break;

		if(nMultipleValue>=m_nMultipleValue[i])
		{
			nMultipleIndex = i;
		}
	}

	if(nMultipleIndex == m_nMultipleIndex && bChange)
	{
		if(rand()%100<80&&nMultipleIndex>0) nMultipleIndex--; 
	}
	
	return nMultipleIndex;
}

//AI�������÷���
void CAndroidUserItemSink::ReSetScore()
{
	//�Ϸ���У��
	if (m_pIAndroidUserItem == NULL) return;
	//��ȡ����
	tagRoomAIParameter *pAndroidParameter = m_pIAndroidUserItem->GetRoomAIParameter();

	//�ж���Χ
	SCORE lUserScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	SCORE lAndroidScore = m_nScoreRatio >= 0 ? m_lAndroidScore*m_nScoreRatio : m_lAndroidScore / (-m_nScoreRatio);
	if (lAndroidScore >= pAndroidParameter->lAICarryScoreMinMultiple[0] * m_nScoreRatio && lAndroidScore <= pAndroidParameter->lAICarryScoreMaxMultiple[4] * m_nScoreRatio) return;

	//�趨����
	SCORE lTakeScore = 0;
	RAND_MIN_MAX(lTakeScore, pAndroidParameter->lAICarryScoreMinMultiple[0] * m_nScoreRatio, lAndroidScore <= pAndroidParameter->lAICarryScoreMaxMultiple[4] * m_nScoreRatio);

	if(lAndroidScore>lTakeScore)
	{	
		m_pIAndroidUserItem->PerformSaveScore(__min(lUserScore, lAndroidScore - lTakeScore));
	}
	else
	{
		m_pIAndroidUserItem->PerformTakeScore(lTakeScore-lAndroidScore);
	}
}

//�޸ı���
bool CAndroidUserItemSink::ModifyMultiple(bool bMultipleModifyTime, int nMultipleIndex)
{
	if ( (bMultipleModifyTime && GetTickCount() - m_nMultipleTime > m_nMultipleModify  || !bMultipleModifyTime) && m_nLaserTime == 0 && m_nSpeedTime == 0 )
	{   
		// �ж��Ƿ��б���
		for ( int nIndex = 0; nIndex < m_FishInfo.GetCount(); ++nIndex)
		{
			// ��ȡ����Ϣ
			tagFishAndroid &pFishAndroid = m_FishInfo.GetAt(nIndex);

			// �жϱ���
			if( pFishAndroid.nFishType == FishType_BaoXiang && pFishAndroid.wHitChair == m_wMeChairID )
			{
				return false;
			}
		}

		int nTempMultipleIndex = nMultipleIndex;
		if(nTempMultipleIndex == Multiple_Max)
		{
			nTempMultipleIndex = SelectMultiple(bMultipleModifyTime);
		}

		if(bMultipleModifyTime)
		{
			// ����ʱ��
			m_nMultipleTime = GetTickCount();
			m_nMultipleModify = ((rand() % 60) + 60*2) * 1000;
		}

		// ���ͱ���
		if(nTempMultipleIndex!=m_nMultipleIndex)
		{
			m_nMultipleIndex = nTempMultipleIndex;
			CMD_C_Multiple CMDCMultiple;
			CMDCMultiple.nMultipleIndex = nTempMultipleIndex;
			m_pIAndroidUserItem->SendSocketData( SUB_C_MULTIPLE, &CMDCMultiple, sizeof(CMDCMultiple));
		}

		return true;
	}

	return false;
}





//////////////////////////////////////////////////////////////////////////
