#include "Stdafx.h"
#include "AndroidUserItemSink.h"

// 捕获信息
struct tagCatchInfo
{
	bool						bFishTrack;				// 追踪鱼
	uint						nFishKey;				// 鱼关键值
	uint						nFishType;				// 鱼类型
};

// 捕获鱼排序
bool CatchInfoCompare( tagCatchInfo & TCatchInfoOne, tagCatchInfo & TCatchInfoTwo )
{
	return TCatchInfoOne.bFishTrack || TCatchInfoTwo.nFishType < TCatchInfoOne.nFishType;
}

//////////////////////////////////////////////////////////////////////////
#define  GLCOLOR_RGB(r,g,b)	    ((COLORREF)((((255)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

// 游戏时间
#define IDI_UPDATA					(1)									// 更新定时

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	// 个人信息
	m_wMeChairID = INVALID_CHAIR;
	m_lAndroidScore = 0;
	m_lCooling = 0;
	m_lCoolingTime = 0;
	m_nTrackFishIndex = int_max;
	m_bFreeFire = rand()%2 == 0;
	m_nFireTime = rand()%1000 + 300;
	m_nBulletKey = 0;
	m_lAndroidScoreMax=0;
	// 活动时间
	m_dwSecondTime = 0;
	m_nLaserTime = 0;
	m_nSpeedTime = 0;
	m_nSceneTime = 0;
	m_nScoreRatio=1;
	// 玩家位置
	m_PointPlay[S_TOP_LEFT].x = 403;
	m_PointPlay[S_TOP_LEFT].y = 64;
	m_PointPlay[S_TOP_RIGHT].x = 878;
	m_PointPlay[S_TOP_RIGHT].y = 64;
	m_PointPlay[S_BOTTOM_LEFT].x = 403;
	m_PointPlay[S_BOTTOM_LEFT].y = 736;
	m_PointPlay[S_BOTTOM_RIGHT].x = 878;
	m_PointPlay[S_BOTTOM_RIGHT].y = 736;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	m_FishInfo.RemoveAll();
	m_FishInfo.FreeMemory();
	m_BulletInfo.RemoveAll();
	m_BulletInfo.FreeMemory();
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	// 更新定时
	if ( nTimerID == IDI_UPDATA )
	{
		// 锁定鱼
		bool bHaveTrack = false;
		int nTrackFishIndex = int_max; 
		byte cbTrackFishType = 255;
		CPoint PointFish( rand()%DEFAULE_WIDTH, rand()%DEFAULE_HEIGHT );

		// 获取流逝时间
		uint dwCurrentTime = GetTickCount();

		// 计算鱼位置
		for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); )
		{
			// 获取鱼信息
			tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );
			
			// 判断存活
			dword dwCurrentTime = GetTickCount();

			// 生成时间
			uint unGenerateTime = (dwCurrentTime - nStartTime);

			// 未到创建时间
			if ( unGenerateTime < TFishAndroid.unCreateTime )
			{
				nFishIndex++;

				continue;
			}

			// 以前停留时间
			uint nBeforeStayTime = 0;

			// 遍历时间
			for( int nStayIndex = 0; nStayIndex < TFishAndroid.ArrayStayInfo.GetCount(); ++nStayIndex )
			{
				// 获取信息
				tagStayInfo & nStayInfo = TFishAndroid.ArrayStayInfo.GetAt(nStayIndex);

				// 获取时间
				uint nStayStart = nStayInfo.nStayStart;				
				uint nStayTime = nStayInfo.nStayTime;

				// 比较信息
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

			// 减去时间
			unGenerateTime -= nBeforeStayTime;

			// 游动索引
			int  nMoveIndex = 0;
			bool bSuccess = false;

			// 错误信息
			ASSERT( unGenerateTime >= TFishAndroid.unCreateTime );
			unGenerateTime = max(unGenerateTime, TFishAndroid.unCreateTime);

			// 游动时间
			uint unSwimTime = unGenerateTime - TFishAndroid.unCreateTime;
			uint unSwimCurrent = unSwimTime;
			uint unPhaseTime = 0;

			// 总时间
			uint unTotalTime = 0;

			// 计算总时间
			for ( int nBezierIndex = 0; nBezierIndex < TFishAndroid.nBezierCount ; ++nBezierIndex )
			{
				// 获取信息
				tagBezierPoint * pBezierPoint = &TFishAndroid.TBezierPoint[nBezierIndex];

				// 添加时间
				unTotalTime += pBezierPoint->Time;
			}

			// 获取游动索引
			for ( int nBezierIndex = 0; nBezierIndex < TFishAndroid.nBezierCount ; ++nBezierIndex )
			{
				// 获取信息
				tagBezierPoint * pBezierPoint = &TFishAndroid.TBezierPoint[nBezierIndex];

				// 路径耗时
				unPhaseTime += pBezierPoint->Time;

				// 查找当前路径
				if ( unSwimTime <= unPhaseTime )
				{
					// 设置路径
					nMoveIndex = nBezierIndex;

					// 查找成功
					bSuccess = true;

					break;
				}

				// 计算当前时间
				unSwimCurrent -= pBezierPoint->Time;
			}

			// 异常错误
			if ( !bSuccess )
			{
				// 结束游动
				m_FishInfo.RemoveAt( nFishIndex );
				continue;
			}

			// 计算贝塞尔
			double dProportion = (double)(unSwimCurrent)/(double)(TFishAndroid.TBezierPoint[nMoveIndex].Time);
			CDoublePoint ptPosition(0.0, 0.0);
			ptPosition = PointOnCubicBezier( &TFishAndroid.TBezierPoint[nMoveIndex], dProportion );

			// 记录上次
			TFishAndroid.ptLast = TFishAndroid.ptPosition;

			// 计算旋转
			TFishAndroid.ptPosition = Rotate(TFishAndroid.TBezierPoint[0].BeginPoint, TFishAndroid.fRotateAngle, ptPosition );

			// 偏移
			TFishAndroid.ptPosition.x += TFishAndroid.PointOffSet.x;
			TFishAndroid.ptPosition.y += TFishAndroid.PointOffSet.y;

			// 有效位置鱼
			if ( TFishAndroid.ptPosition.x > 0 && TFishAndroid.ptPosition.x < DEFAULE_WIDTH 
				&& TFishAndroid.ptPosition.y > 0 && TFishAndroid.ptPosition.y < DEFAULE_HEIGHT && m_bStopFire==false)
			{
				// 计算追踪
				if ( m_nTrackFishIndex == int_max )
				{
					if ( ( cbTrackFishType == 255 || TFishAndroid.nFishType == FishType_BaoXiang || (cbTrackFishType < TFishAndroid.nFishType && rand()%4 == 0) )
						&& ( TFishAndroid.wHitChair == INVALID_CHAIR || TFishAndroid.wHitChair == m_wMeChairID ) )
					{
						// 设置信息
						nTrackFishIndex = TFishAndroid.nFishKey;
						cbTrackFishType = TFishAndroid.nFishType;
						PointFish.SetPoint( (int)TFishAndroid.ptPosition.x, (int)TFishAndroid.ptPosition.y );

						// 有效最终
						bHaveTrack = true;
					}
				}
				else if( m_nTrackFishIndex == TFishAndroid.nFishKey )
				{
					// 设置信息
					nTrackFishIndex = TFishAndroid.nFishKey;
					cbTrackFishType = TFishAndroid.nFishType;
					PointFish.SetPoint( (int)TFishAndroid.ptPosition.x, (int)TFishAndroid.ptPosition.y );

					// 有效最终
					bHaveTrack = true;
				}
			}

			++nFishIndex;

			continue;
		}

		// 计算冷却
		if ( m_lCooling > 0 && (long)( GetTickCount() - m_lCoolingTime ) >= m_lCooling )
		{
			m_lCooling = 0;
		}
		
		// 计算金币
		long lBulletInvest = m_nMultipleValue[m_nMultipleIndex] * QianPao_Bullet;

		// 没钱，离开
		if ( m_lAndroidScore < lBulletInvest)
		{
			int nMultipleIndex = m_nMultipleIndex;
			while(m_lAndroidScore<m_nMultipleValue[nMultipleIndex])
			{
				if(nMultipleIndex>0)	nMultipleIndex--;
				else
				{		
					//结束定时器
					m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
					m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
					m_bStopFire = true;
					ReSetScore();
					return true;
				}
			}

			//修改倍数			
			ModifyMultiple(false, nMultipleIndex );
		}

		// 更新追踪
		m_nTrackFishIndex = nTrackFishIndex;

		// 有效子弹
		if ( bHaveTrack && dwCurrentTime - m_nSceneTime > 7000 && m_lCooling == 0 && m_nTrackFishIndex != int_max && m_lAndroidScore >= lBulletInvest && m_bStopFire==false)
		{
			// 计算角度
			float fRadian = atan2((float)(m_PointPlay[m_wMeChairID].y - PointFish.y) , (float)(PointFish.x - m_PointPlay[m_wMeChairID].x));

			// 格式角度
			if( m_wMeChairID == S_BOTTOM_LEFT || m_wMeChairID == S_BOTTOM_RIGHT )
			{
				// 范围格式
				if ( fRadian <= -GL_PI / 2.f )
				{
					fRadian = GL_PI;
				}
				else if ( fRadian < 0.00f )
				{
					fRadian = 0.00f;
				}

				// 格式范围
				fRadian = -fRadian;
			}
			else
			{
				fRadian = -fRadian;
			}

			// 算出最后点
			CDoublePoint PointAim = Rotate(CDoublePoint(m_PointPlay[m_wMeChairID]), fRadian, CDoublePoint( m_PointPlay[m_wMeChairID].x + 1000, m_PointPlay[m_wMeChairID].y));

			// 扣住金币
			m_lAndroidScore -= lBulletInvest;

			// 关键值增加
			m_nBulletKey++;
			if( m_nBulletKey == int_max )
				m_nBulletKey = 0;

			// 重置冷却时间

			if(cbTrackFishType!=255 && cbTrackFishType>=FishType_Moderate_Big_Max)
				m_lCooling = m_nBulletCoolingTime;
			else
				m_lCooling = m_nBulletCoolingTime*(rand()%3+1);			
			m_lCoolingTime = GetTickCount();

			// 速度强化
			if ( m_nSpeedTime > 0 )
			{
				m_lCooling /= 2;
			}

			// 追踪索引
			int nTrackFishIndex = m_bFreeFire ? int_max : m_nTrackFishIndex;

			// 定义消息
			CMD_C_Fire Fire;
			Fire.nMultipleIndex = m_nMultipleIndex;
			Fire.nTrackFishIndex = nTrackFishIndex;
			Fire.nBulletKey = m_nBulletKey;
			Fire.ptPos.SetPoint( (SHORT)PointAim.x, (SHORT)PointAim.y );

			// 发送消息
			m_pIAndroidUserItem->SendSocketData( SUB_C_FIRE, &Fire, sizeof(Fire));

			// 追踪炮台
			if( !m_bFreeFire )
			{
				// 添加到子弹
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
				// 计算距离
				double dFireDistance = sqrt(pow(abs(m_PointPlay[m_wMeChairID].x - PointFish.x),2) + pow(abs(m_PointPlay[m_wMeChairID].y - PointFish.y),2));

				// 计算速度
				double dBulletVelocity = m_nBulletVelocity;

				// 加速强化
				if ( m_nSpeedTime > 0 )
					dBulletVelocity *= 2.0;

				// 计算时间
				uint unTime = static_cast<uint>( abs(dFireDistance) / (dBulletVelocity/1000.0));

				// 添加到子弹
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

		// 计算子弹移动
		for( int nBulletIndex = 0; nBulletIndex < m_BulletInfo.GetCount(); )
		{
			// 获取子弹信息
			tagBulletAndroid & TBulletAndroid = m_BulletInfo.GetAt( nBulletIndex );

			// 子弹捕获
			bool bBulletCatch = false;

			// 捕获鱼
			CArray< tagCatchInfo > ArrayCatchInfo;

			// 有效追踪
			if ( TBulletAndroid.nTrackFishIndex != int_max )
			{
				// 判断子弹跟踪
				bool bValidTrack = false;
				uint nTrackFishKey = 0;
				byte nTrackFishType = 0;
				CDoublePoint ptFishPosition;
				for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
				{
					// 获取鱼信息
					tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );

					// 判断鱼
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

				// 有追踪
				if ( bValidTrack )
				{
					// 移动距离
					double dFireDistance = sqrt(pow(abs( TBulletAndroid.ptPosition.x - ptFishPosition.x ), 2) + pow(abs(TBulletAndroid.ptPosition.y - ptFishPosition.y), 2));

					// 计算速度
					double dBulletVelocity = m_nBulletVelocity;

					// 加速强化
					if ( TBulletAndroid.bSpecialt )
						dBulletVelocity *= 2.0;

					// 移动时间
					uint unTime = static_cast<uint>( abs(dFireDistance) / (dBulletVelocity/1000.0) );

					// 需要移动
					if ( unTime != 0 )
					{
						CDoublePoint ptOffset( (ptFishPosition.x - TBulletAndroid.ptPosition.x)/unTime, (ptFishPosition.y - TBulletAndroid.ptPosition.y)/unTime );
						TBulletAndroid.ptPosition += ptOffset;
					}

					// 捕获到
					if ( unTime == 0 )
					{
						// 捕获信息
						tagCatchInfo TCatchInfo;
						TCatchInfo.bFishTrack = true;
						TCatchInfo.nFishKey = nTrackFishKey;
						TCatchInfo.nFishType = nTrackFishType;

						// 添加捕获
						ArrayCatchInfo.Add( TCatchInfo );

						// 设置捕获
						bBulletCatch = true;
					}
				}

				// 无追踪， 直接散开
				if ( !bValidTrack )
				{
					bBulletCatch = true;
				}
			}
			// 无追踪
			else
			{	
				// 移动位置
				TBulletAndroid.ptPosition += TBulletAndroid.ptOffset;

				// 反弹子弹
				if( TBulletAndroid.ptPosition.x <= 0.0 || TBulletAndroid.ptPosition.x >= DEFAULE_WIDTH )
				{
					// 设置方向
					TBulletAndroid.ptOffset.x = -TBulletAndroid.ptOffset.x;

					// 设置反弹
					TBulletAndroid.bEjection = true;
				}
				if( TBulletAndroid.ptPosition.y <= 0.0 || TBulletAndroid.ptPosition.y >= DEFAULE_HEIGHT )
				{
					// 设置方向
					TBulletAndroid.ptOffset.y = -TBulletAndroid.ptOffset.y;

					// 设置反弹
					TBulletAndroid.bEjection = true;
				}

				// 判断子弹跟踪
				for( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
				{
					// 获取鱼信息
					tagFishAndroid & TFishAndroid = m_FishInfo.GetAt( nFishIndex );

					// 判断鱼
					if( TFishAndroid.ptPosition.x > 0 && TFishAndroid.ptPosition.x < DEFAULE_WIDTH 
						&& TFishAndroid.ptPosition.y > 0 && TFishAndroid.ptPosition.y < DEFAULE_HEIGHT )
					{
						// 鱼大小
						float fRotationAngle = (float)atan2((TFishAndroid.ptPosition.y - TFishAndroid.ptLast.y) , (TFishAndroid.ptLast.x - TFishAndroid.ptPosition.x));
						fRotationAngle = -fRotationAngle - GL_PI / 2;
						TFishAndroid.FTFish.p.Set( (float)TFishAndroid.ptPosition.x, (float)TFishAndroid.ptPosition.y );
						TFishAndroid.FTFish.q.Set( fRotationAngle );

						// 子弹位置
						TBulletAndroid.FTBullet.p.Set( (float)TBulletAndroid.ptPosition.x, (float)TBulletAndroid.ptPosition.y );
						TBulletAndroid.FTBullet.q.Set( 0.f );

						// 计算位置
						if( ShapeOverlap( &TFishAndroid.PSFish, &TBulletAndroid.SPBullet, TFishAndroid.FTFish, TBulletAndroid.FTBullet ) )
						{
							// 捕获信息
							tagCatchInfo TCatchInfo;
							TCatchInfo.bFishTrack = false;
							TCatchInfo.nFishKey = TFishAndroid.nFishKey;
							TCatchInfo.nFishType = TFishAndroid.nFishType;

							// 添加捕获
							ArrayCatchInfo.Add( TCatchInfo );

							// 设置捕获
							bBulletCatch = true;

							break;
						}
					}
				}
			}

			// 撒网
			if ( bBulletCatch && ArrayCatchInfo.GetCount() )
			{
				// 排序信息
				std::sort( ArrayCatchInfo.GetData(), ArrayCatchInfo.GetData() + ArrayCatchInfo.GetSize(), CatchInfoCompare );

				// 定义消息
				CMD_C_CatchFish CMDCCatchFish;
				ZeroMemory( &CMDCCatchFish, sizeof(CMDCCatchFish) );
				CMDCCatchFish.nBulletKey = TBulletAndroid.nBulletKey;
				for( int nFishIndex = 0; nFishIndex < FishCatch_Max && nFishIndex < ArrayCatchInfo.GetCount(); ++nFishIndex )
				{
					// 获取信息
					tagCatchInfo & TCatchInfo = ArrayCatchInfo.GetAt( nFishIndex );

					// 赋值信息
					CMDCCatchFish.nFishKey[nFishIndex] = TCatchInfo.nFishKey;
				}

				m_pIAndroidUserItem->SendSocketData( SUB_C_CATCH_FISH, &CMDCCatchFish, sizeof(CMDCCatchFish));
			}

			//========清除捕鱼信息
			ArrayCatchInfo.RemoveAll();

			// 撒网
			if ( bBulletCatch )
			{
				// 删除
				m_BulletInfo.RemoveAt(nBulletIndex);

				continue;
			}

			// 下一个
			++nBulletIndex;
			continue;
		}


		// 秒间隔
		if (GetTickCount() - m_dwSecondTime >= 1000)
		{
			// 保存时间
			m_dwSecondTime += 1000;

			// 射击类型
			if( m_nFireTime > 0 )
			{
				m_nFireTime--;

				if ( m_nFireTime == 0 )
				{
					// 射击类型
					m_bFreeFire = rand()%3 >= 1;
					m_nFireTime = rand()%1000 + 300;
				}
			}
		}

		//发炮间隔
		if(long(GetTickCount()-m_lStopFireTime)>m_lStopFireModify)
		{
			m_lStopFireTime = GetTickCount();
			//停止
			if(m_bStopFire==false)
			{
				m_bStopFire=true;
				m_lStopFireModify = ((rand() % 5) + 1) * 1000;
				//停火的时候解除索引
				m_nTrackFishIndex = int_max;
			}
			else if(m_bStopFire)
			{
				m_bStopFire=false;
				m_lStopFireModify = ((rand() % 16) + 15) * 1000;
			}
		}
		
		// 切换倍数
		ModifyMultiple(true);

		//AI分数超过限定值 退出游戏
		if(m_lAndroidScore>=m_lAndroidScoreMax)
		{
			//结束定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
			m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
			m_bStopFire = true;
			ReSetScore();
		}
		// 添加定时器
		m_pIAndroidUserItem->KillGameTimer( IDI_UPDATA );
		m_pIAndroidUserItem->SetGameTimer( IDI_UPDATA, 1 );

		return true;
	}

	return false;
}

//游戏消息
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
			// 效验数据
			ASSERT( wDataSize == sizeof(CMD_S_Multiple) );
			if ( wDataSize != sizeof(CMD_S_Multiple) ) return false;
			
			// 获取信息
			CMD_S_Multiple * pMultiple = (CMD_S_Multiple*) pBuffer;
			//倍数更换成功
			if(pMultiple->wChairID == m_wMeChairID)
			{
				m_nMultipleIndex = pMultiple->nMultipleIndex;
			}
			return true;
		}
	case SUB_S_ANDROID_CONFIG:
		{
			// 效验数据
			ASSERT( wDataSize == sizeof(CMD_S_AndriodConfig) );
			if ( wDataSize != sizeof(CMD_S_AndriodConfig) ) return false;

			// 获取信息
			CMD_S_AndriodConfig * pConfig = (CMD_S_AndriodConfig*) pBuffer;
			CopyMemory(m_lAndroidAreaScore,pConfig->lAndroidScore,sizeof(m_lAndroidAreaScore));
			CopyMemory(m_nAndroidMultiple,pConfig->nAndroidMultiple,sizeof(m_nAndroidMultiple));
			m_lAndroidScoreMax=pConfig->lAndroidScoreMax;
			if(m_lAndroidScoreMax<=0) m_lAndroidScoreMax=5000000;
			
			//修改倍数
			ModifyMultiple(false);

			return true;
		}
	case SUB_S_ANDROID_LEAVE://AI离场
		{
			//结束定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_UPDATA);
			m_pIAndroidUserItem->SendSocketData( SUB_C_APPLY_LEAVE, NULL, NULL );
			m_bStopFire = true;
			ReSetScore();
			return true;
		}

	}

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:	
	case GAME_STATUS_PLAY:
		{
			// 效验数据
			ASSERT( wDataSize == sizeof(GameScene) );
			if ( wDataSize != sizeof(GameScene) ) return false;

			// 消息处理
			GameScene * pGameScene = (GameScene *)pData;

			// 个人时间
			m_wMeChairID = m_pIAndroidUserItem->GetChairID();

			// 创建时间
			nStartTime = GetTickCount();

			// 自己分数
			m_lAndroidScore = pGameScene->lPlayScore;
			m_nScoreRatio=pGameScene->lCellScore;
			if(m_nScoreRatio>=-1 && m_nScoreRatio<=1) m_nScoreRatio=1;

			// 更新倍数
			CopyMemory(m_nMultipleValue, pGameScene->nMultipleValue, sizeof(m_nMultipleValue));
			m_nMultipleIndex = pGameScene->nMultipleIndex[m_wMeChairID];
			
			// 必要子弹配置
			m_nBulletCoolingTime = pGameScene->nBulletCoolingTime;
			m_nBulletVelocity = pGameScene->nBulletVelocity;

			// 添加定时器
			m_pIAndroidUserItem->KillGameTimer( IDI_UPDATA );
			m_pIAndroidUserItem->SetGameTimer( IDI_UPDATA, 60 );

			// 更新时间
			m_dwSecondTime = GetTickCount();

			// 射击类型
			m_bFreeFire = rand()%3 >= 1;
			m_nFireTime = rand()%1000 + 300;

			//m_nSpeedTime = pGameScene->nSpeedPlayTime[m_wMeChairID];
			//m_nLaserTime = pGameScene->nLaserPlayTime[m_wMeChairID];

			// 倍数时间
			m_nMultipleTime = GetTickCount();
			m_nMultipleModify = ((rand() % 60) + 60*2) * 1000;

			m_lStopFireTime = GetTickCount();
			m_lStopFireModify = ((rand() % 13) + 3) * 1000;
			m_bStopFire = false;
			
			//修改倍数
			ModifyMultiple(false);

			// 场景时间
			m_nSceneTime = GetTickCount();

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//用户进入
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	// 判断用户
	if( bLookonUser )
		return;

	// 判断自己
	if ( pIAndroidUserItem->GetUserID() == m_pIAndroidUserItem->GetUserID() )
	{
		// 自己分数
		m_lAndroidScore = pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	}

	return;
}

//用户离开
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

// 同步消息
bool CAndroidUserItemSink::OnSubSynchronizationTime(const void * pBuffer, word wDataSize)
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_Synchronous) );
	if ( wDataSize != sizeof(CMD_S_Synchronous) ) return false;

	// 获取信息
	CMD_S_Synchronous * pSynchronous = (CMD_S_Synchronous*) pBuffer;

	// 时间偏移
	ASSERT( nStartTime > pSynchronous->nOffSetTime );
	nStartTime -= pSynchronous->nOffSetTime;

	return true;
}

// 鱼鱼创建
bool CAndroidUserItemSink::OnSubCreateFish( const void * pBuffer, word wDataSize )
{
	// 效验数据
	ASSERT( wDataSize%sizeof(CMD_S_FishCreate) == 0 && wDataSize > 0 );
	if ( wDataSize%sizeof(CMD_S_FishCreate) != 0 || wDataSize == 0 ) return false;

	// 设置记录
	word wRecordCount = wDataSize / sizeof(CMD_S_FishCreate);
	for (word wIndex = 0; wIndex < wRecordCount; wIndex++ ) 
	{
		CMD_S_FishCreate * pFishCreate = (((CMD_S_FishCreate *)pBuffer)+wIndex);
		
		// 添加鱼
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

		// 判断特殊
		if ( TFishAndroid.nFishState != FishState_Normal )
		{
			TFishAndroid.PSFish.Set( KnifeVec, MONSTER_VEC );
		}
		else
		{
			// 设置区域
			TFishAndroid.PSFish.Set( MonsterVec[TFishAndroid.nFishType], MONSTER_VEC );
		}

		// 添加鱼
		m_FishInfo.Add(TFishAndroid);
	}

	return true;
}


// 鱼捕获
bool CAndroidUserItemSink::OnSubCatchFish( const void * pBuffer, word wDataSize )
{
	// 效验数据
	ASSERT( wDataSize%sizeof(CMD_S_CatchFish) == 0 && wDataSize > 0 );
	if ( wDataSize%sizeof(CMD_S_CatchFish) != 0 || wDataSize == 0 ) return false;

	// 删除鱼
	word wCatchFishCount = wDataSize / sizeof(CMD_S_CatchFish);
	for (word wIndex = 0; wIndex < wCatchFishCount; wIndex++ ) 
	{
		CMD_S_CatchFish * pCatchFish = (((CMD_S_CatchFish *)pBuffer)+wIndex);
		for ( int nFishIndex = 0; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
		{
			// 捕获信息
			tagFishAndroid & TFish = m_FishInfo.GetAt( nFishIndex );
			
			// 捕获中鱼
			if ( TFish.nFishKey == pCatchFish->nFishKey )
			{
				// 自己捕获
				if ( pCatchFish->wChairID == m_pIAndroidUserItem->GetChairID() )
				{
					m_lAndroidScore += pCatchFish->lScoreCount;
				}

				// 删除鱼
				m_FishInfo.RemoveAt(nFishIndex);
				break;
			}
		}
	}

	return true;
}

// 所有鱼快速移动
bool CAndroidUserItemSink::OnSubExchangeScene( const void * pBuffer, WORD wDataSize )
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ExchangeScene));
	if (wDataSize!=sizeof(CMD_S_ExchangeScene)) return false;

	// 时间同步
	nStartTime = GetTickCount();

	// 删除所有鱼
	m_FishInfo.RemoveAll();
	m_nTrackFishIndex = int_max;
	// 场景时间
	m_nSceneTime = GetTickCount();

	return true;
}

// 活动消息
bool CAndroidUserItemSink::OnSubSupply( const void * pBuffer, WORD wDataSize )
{
	// 效验数据
	ASSERT( wDataSize == sizeof(CMD_S_Supply) );
	if ( wDataSize != sizeof(CMD_S_Supply) ) return false;

	// 消息处理
	CMD_S_Supply * pSupply = (CMD_S_Supply *)pBuffer;

	// 执行补给
	if( pSupply->wChairID == m_wMeChairID && pSupply->nSupplyType == EST_Speed )
	{
		// 激活加速
		m_nSpeedTime = (int)pSupply->lSupplyCount;
	}

	return true;
}

// 停留鱼
bool CAndroidUserItemSink::OnSubStayFish( const void * pData, WORD wDataSize )
{
	// 效验数据
	ASSERT(wDataSize==sizeof(CMD_S_StayFish));
	if (wDataSize!=sizeof(CMD_S_StayFish)) return false;

	// 定义变量
	CMD_S_StayFish * pStayFish = (CMD_S_StayFish*)pData;

	// 遍历删除
	for ( int nFishIndex = 0 ; nFishIndex < m_FishInfo.GetCount(); ++nFishIndex )
	{
		// 获取鱼的信息
		tagFishAndroid & TFishAndroid = m_FishInfo.GetAt(nFishIndex);

		// 判断删除鱼
		if ( TFishAndroid.nFishKey != pStayFish->nFishKey )
		{
			continue;
		}

		// 无效设置
		if( pStayFish->nStayStart < TFishAndroid.unCreateTime )
		{
			ASSERT( FALSE );
			return true;
		}

		// 获取鱼数据
		CArrayStay & ArrayStayInfo = TFishAndroid.ArrayStayInfo;

		// 对比最后一个
		if ( ArrayStayInfo.GetCount() > 0 )
		{
			// 获取最后一个
			tagStayInfo & nStaylast = ArrayStayInfo.GetAt( ArrayStayInfo.GetCount() - 1 );

			// 相同时间
			if ( nStaylast.nStayStart == pStayFish->nStayStart )
			{
				// 更新时间
				nStaylast.nStayTime = pStayFish->nStayTime;

				return true;
			}
		}

		// 设置信息
		tagStayInfo nStayInfo;
		nStayInfo.nStayStart = pStayFish->nStayStart;
		nStayInfo.nStayTime = pStayFish->nStayTime;

		// 添加信息
		ArrayStayInfo.Add(nStayInfo);

		// 结束信息
		return true;
	}

	// 结束信息
	return true;
}

// 旋转点
CDoublePoint CAndroidUserItemSink::Rotate( CDoublePoint & ptCircle, double dRadian, CDoublePoint & ptSome ) 
{ 
	CDoublePoint temp; 
	ptSome.x -= ptCircle.x; 
	ptSome.y -= ptCircle.y; 
	temp.x = ptSome.x*cos(dRadian) - ptSome.y*sin(dRadian) + ptCircle.x; 
	temp.y = ptSome.y*cos(dRadian) + ptSome.x*sin(dRadian) + ptCircle.y; 
	return temp; 
} 

// 贝塞尔曲线
CDoublePoint CAndroidUserItemSink::PointOnCubicBezier( tagBezierPoint* cp, double t )
{
	double   ax = 0.f, bx = 0.f, cx = 0.f;
	double   ay = 0.f, by = 0.f, cy = 0.f;
	double   tSquared = 0.f, tCubed = 0.f;
	CDoublePoint result;

	// 计算系数
	cx = 3.f * (cp->KeyOne.x - cp->BeginPoint.x);
	bx = 3.f * (cp->KeyTwo.x - cp->KeyOne.x) - cx;
	ax = cp->EndPoint.x - cp->BeginPoint.x - cx - bx;

	cy = 3.f * (cp->KeyOne.y - cp->BeginPoint.y);
	by = 3.f * (cp->KeyTwo.y - cp->KeyOne.y) - cy;
	ay = cp->EndPoint.y - cp->BeginPoint.y - cy - by;

	// 计算曲线点
	tSquared = t * t;
	tCubed = tSquared * t;

	result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp->BeginPoint.x;
	result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp->BeginPoint.y;

	return result;
}

// AI倍数选择
int CAndroidUserItemSink::SelectMultiple(bool bChange)
{
	//倍数确定
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

//AI离桌重置分数
void CAndroidUserItemSink::ReSetScore()
{
	//合法性校验
	if (m_pIAndroidUserItem == NULL) return;
	//获取配置
	tagRoomAIParameter *pAndroidParameter = m_pIAndroidUserItem->GetRoomAIParameter();

	//判定范围
	SCORE lUserScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	SCORE lAndroidScore = m_nScoreRatio >= 0 ? m_lAndroidScore*m_nScoreRatio : m_lAndroidScore / (-m_nScoreRatio);
	if (lAndroidScore >= pAndroidParameter->lAICarryScoreMinMultiple[0] * m_nScoreRatio && lAndroidScore <= pAndroidParameter->lAICarryScoreMaxMultiple[4] * m_nScoreRatio) return;

	//设定分数
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

//修改倍数
bool CAndroidUserItemSink::ModifyMultiple(bool bMultipleModifyTime, int nMultipleIndex)
{
	if ( (bMultipleModifyTime && GetTickCount() - m_nMultipleTime > m_nMultipleModify  || !bMultipleModifyTime) && m_nLaserTime == 0 && m_nSpeedTime == 0 )
	{   
		// 判断是否有宝船
		for ( int nIndex = 0; nIndex < m_FishInfo.GetCount(); ++nIndex)
		{
			// 获取鱼信息
			tagFishAndroid &pFishAndroid = m_FishInfo.GetAt(nIndex);

			// 判断宝船
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
			// 倍数时间
			m_nMultipleTime = GetTickCount();
			m_nMultipleModify = ((rand() % 60) + 60*2) * 1000;
		}

		// 发送倍数
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
