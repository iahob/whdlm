#include "StdAfx.h"
#include ".\servercontrol.h"


// 声明变量
CServerControl CServerControl::m_ServerControl;

// 构造函数
CServerControl::CServerControl()
{
	// 设置变量
	m_bCreateControl = false;
	m_pdRoomStock = NULL;
	m_pdRoomIdle = NULL;
	m_pdRoomLotteryPool = NULL;
	ZeroMemory(m_szLogoPersonalScore, sizeof(m_szLogoPersonalScore));
	ZeroMemory(m_szLogoOperate, sizeof(m_szLogoOperate));
}

// 析构函数
CServerControl::~CServerControl()
{
}

// 判断接口
bool CServerControl::ISNull()
{
	return !m_bCreateControl;
}

// 释放接口
void CServerControl::Release()
{
	// 打开文件
	file * pFile = _tfopen( m_szLogoPersonalScore, _T("wb") );	
	if( pFile != NULL )	
	{
		// 遍历信息
		POSITION Pos = m_MapPersonalScore.GetStartPosition();
		while ( Pos )
		{
			// 获取信息
			uint nPlayID = 0;
			tagPersonalScore TPersonalScore;
			m_MapPersonalScore.GetNextAssoc( Pos, nPlayID, TPersonalScore );

			// 写入文件
			fwrite( &nPlayID, sizeof(nPlayID), 1, pFile );
			fwrite( &TPersonalScore, sizeof(TPersonalScore), 1, pFile );
		}

		// 关闭文件
		fflush( pFile );
		fclose( pFile );

		// 清空信息
		m_MapPersonalScore.RemoveAll();
	}

	// 删除内存
	//delete this;
}

// 创建函数
bool CServerControl::Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo )
{
	// 设置变量
	m_pdRoomStock = pdRoomStock;
	m_pdRoomIdle = pdRoomIdle;
	m_pdRoomLotteryPool = pdRoomLotteryPool;
	_sntprintf(m_szLogoPersonalScore, CountArray(m_szLogoPersonalScore), _T("%s"), pszLogoPersonalScore );
	_sntprintf(m_szLogoOperate, CountArray(m_szLogoOperate), _T("%s"), pszOperateLogo );

	// 打开文件
	file * pFile = _tfopen( m_szLogoPersonalScore, _T("rb") );	
	if( pFile != NULL )	
	{
		// 读取信息
		uint nPlayID = 0;
		tagPersonalScore TPersonalScore;
		ZeroMemory(&TPersonalScore, sizeof(TPersonalScore));
		while( fread(&nPlayID, sizeof(nPlayID), 1, pFile ) == 1 && fread(&TPersonalScore, sizeof(TPersonalScore), 1, pFile ) == 1 )
		{
			// 设置信息
			m_MapPersonalScore.SetAt( nPlayID, TPersonalScore );
		}

		// 关闭文件
		fclose( pFile );
	}

	// 设置标识
	m_bCreateControl = true;

	return true;
}

// 消息函数
bool CServerControl::OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize )
{
	// 无效接口
	if ( pIServerControlCallback == NULL ) 
	{
		return false;
	}

	// 判断消息
	switch( nMessageID )
	{
		// 库存查询
	case SUBC_C_STOCK_INQUIRY:	
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_StockInquiry) );
			if ( nSize != sizeof(CMDC_C_StockInquiry) ) return false;

			// 消息信息
			CMDC_C_StockInquiry * pStockInquiry = (CMDC_C_StockInquiry *)pData;

			// 获取信息
			double & dRoomStock = *m_pdRoomStock;
			double & dRoomIdle = *m_pdRoomIdle;
			double & dRoomLotteryPool = *m_pdRoomLotteryPool;

			// 定义消息
			CMDC_S_StockInquiry CMDCSStockInquiry;
			CMDCSStockInquiry.lGameStock = static_cast<longlong>(dRoomStock) + static_cast<longlong>(dRoomIdle);
			CMDCSStockInquiry.lGameJackpot = static_cast<longlong>(dRoomLotteryPool);
			CMDCSStockInquiry.bMessageBox = pStockInquiry->bActiveInquiry;

			// 发送信息
			pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_STOCK_INQUIRY, &CMDCSStockInquiry, sizeof(CMDCSStockInquiry) );

			return true;
		}
		// 库存设置
	case SUBC_C_STOCK_SET:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_StockSet) );
			if ( nSize != sizeof(CMDC_C_StockSet) ) return false;

			// 消息信息
			CMDC_C_StockSet * pStockSet = (CMDC_C_StockSet *)pData;

			// 获取信息
			double & dRoomStock = *m_pdRoomStock;
			double & dRoomIdle = *m_pdRoomIdle;
			double & dRoomLotteryPool = *m_pdRoomLotteryPool;

			// 相同库存
			double dDifficultyValue = 0.0;
			if( fabs( dRoomStock - static_cast<double>(pStockSet->lGameStock) ) < 0.00001 && fabs( dRoomLotteryPool - static_cast<double>(pStockSet->lGameJackpot) ) < 0.00001 )
			{
				return true;
			}

			// 设置库存
			dRoomStock = static_cast<double>(pStockSet->lGameStock);
			dRoomIdle = 0.0;
			dRoomLotteryPool = static_cast<double>(pStockSet->lGameJackpot);

			// 添加日志
			AppendLog( pszServerUserName, _T("设置库存%I64d, 彩池%I64d。"), pStockSet->lGameStock, pStockSet->lGameJackpot );

			// 发送消息
			SendText( pIServerControlCallback, pIServerUserItem, _T("设置库存成功。") );

			return true;
		}
		// 难度设置
	case SUBC_C_DIFFICULTY_SET:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_DifficultySet) );
			if ( nSize != sizeof(CMDC_C_DifficultySet) ) return false;

			// 消息信息
			CMDC_C_DifficultySet * pDifficultySet = (CMDC_C_DifficultySet *)pData;

			// 相同难度值
			double dDifficultyValue = 0.0;
			if( m_MapPersonalDifficulty.Lookup(pDifficultySet->nDifficultyID, dDifficultyValue) && fabs( dDifficultyValue - pDifficultySet->dDifficultyValue ) < 0.00001 )
			{
				return true;
			}

			// 设置难度
			m_MapPersonalDifficulty.SetAt( pDifficultySet->nDifficultyID, pDifficultySet->dDifficultyValue );

			// 添加日志
			AppendLog( pszServerUserName, _T("设置玩家%u难度%0.5lf。"), pDifficultySet->nDifficultyID, pDifficultySet->dDifficultyValue );

			// 发送消息
			SendText( pIServerControlCallback, pIServerUserItem, _T("设置玩家难度成功。") );

			return true;
		}
		// 难度删除
	case SUBC_C_DIFFICULTY_DELETE:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_DifficultyDelete) );
			if ( nSize != sizeof(CMDC_C_DifficultyDelete) ) return false;

			// 消息信息
			CMDC_C_DifficultyDelete * pDifficultySet = (CMDC_C_DifficultyDelete *)pData;

			// 设置难度
			if( m_MapPersonalDifficulty.PLookup(pDifficultySet->nDifficultyID) )
			{
				m_MapPersonalDifficulty.RemoveKey( pDifficultySet->nDifficultyID );

				// 添加日志
				AppendLog( pszServerUserName, _T("删除玩家%u难度。"), pDifficultySet->nDifficultyID );

				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("删除玩家难度成功。") );
			}
			else
			{
				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("无相应玩家。") );
			}

			return true;
		}
		// 难度查询
	case SUBC_C_DIFFICULTY_INQUIRY:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_DifficultyInquiry) );
			if ( nSize != sizeof(CMDC_C_DifficultyInquiry) ) return false;

			// 难度数量
			if ( m_MapPersonalDifficulty.GetCount() == 0 )
			{
				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("当前无玩家难度设置。") );

				return true;
			}

			// 遍历难度
			POSITION Pos = m_MapPersonalDifficulty.GetStartPosition();
			while( Pos )
			{
				// 遍历信息
				uint nPlayID = 0;
				double dDifficulty = 0.0;
				m_MapPersonalDifficulty.GetNextAssoc( Pos, nPlayID, dDifficulty );

				// 定义消息
				CMDC_S_DifficultyInquiry CMDCSDifficultyInquiry;
				CMDCSDifficultyInquiry.nDifficultyID = nPlayID;
				CMDCSDifficultyInquiry.dDifficultyValue = dDifficulty;

				// 发送信息
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_DIFFICULTY_INQUIRY, &CMDCSDifficultyInquiry, sizeof(CMDCSDifficultyInquiry) );
			}

			return true;
		}
		// 难度清除
	case SUBC_C_DIFFICULTY_CLEAR:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_DifficultyClear) );
			if ( nSize != sizeof(CMDC_C_DifficultyClear) ) return false;

			// 难度数量
			if ( m_MapPersonalDifficulty.GetCount() > 0 )
			{
				// 清除难度
				m_MapPersonalDifficulty.RemoveAll();

				// 添加日志
				AppendLog( pszServerUserName, _T("清空玩家难度。") );
			}

			// 发送消息
			SendText( pIServerControlCallback, pIServerUserItem, _T("清空玩家难度成功。") );

			return true;
		}
		// 赠送设置
	case SUBC_C_GIFT_SET:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_GiftSet) );
			if ( nSize != sizeof(CMDC_C_GiftSet) ) return false;

			// 消息信息
			CMDC_C_GiftSet * pGiftSet = (CMDC_C_GiftSet *)pData;

			// 赠送信息
			tagGiftInfo TGiftInfo;
			if( m_MapPersonalGift.Lookup( pGiftSet->nGiftID, TGiftInfo ) && TGiftInfo.nGiftCount == pGiftSet->nGiftCount && TGiftInfo.nGiftType == pGiftSet->nGiftType )
			{
				return true;
			}

			// 设置信息
			TGiftInfo.nGiftType = pGiftSet->nGiftType;
			TGiftInfo.nGiftCount = pGiftSet->nGiftCount;
			
			// 设置赠送
			m_MapPersonalGift.SetAt( pGiftSet->nGiftID, TGiftInfo );

			// 添加日志
			AppendLog( pszServerUserName, _T("设置玩家%u赠送%u次%s。"), pGiftSet->nGiftID, pGiftSet->nGiftCount, GiftType(pGiftSet->nGiftType) );
			
			// 发送消息
			SendText( pIServerControlCallback, pIServerUserItem, _T("设置玩家赠送成功。") );

			return true;
		}
		// 赠送删除
	case SUBC_C_GIFT_DELETE:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_GiftDelete) );
			if ( nSize != sizeof(CMDC_C_GiftDelete) ) return false;

			// 消息信息
			CMDC_C_GiftDelete * pGiftDelete = (CMDC_C_GiftDelete *)pData;

			// 设置难度
			if( m_MapPersonalGift.PLookup(pGiftDelete->nGiftID) )
			{
				m_MapPersonalGift.RemoveKey( pGiftDelete->nGiftID );

				// 添加日志
				AppendLog( pszServerUserName, _T("删除玩家%u赠送。"), pGiftDelete->nGiftID );

				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("删除玩家赠送成功。") );
			}
			else
			{
				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("无相应玩家。") );
			}

			return true;
		}
		// 赠送查询
	case SUBC_C_GIFT_INQUIRY:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_GiftInquiry) );
			if ( nSize != sizeof(CMDC_C_GiftInquiry) ) return false;

			// 查询数量
			if ( m_MapPersonalGift.GetCount() == 0 )
			{
				// 发送消息
				SendText( pIServerControlCallback, pIServerUserItem, _T("当前无玩家赠送设置。") );

				return true;
			}

			// 遍历赠送
			POSITION Pos = m_MapPersonalGift.GetStartPosition();
			while( Pos )
			{
				// 遍历信息
				uint nPlayID = 0;
				tagGiftInfo TGiftInfo;
				m_MapPersonalGift.GetNextAssoc( Pos, nPlayID, TGiftInfo );

				// 定义消息
				CMDC_S_GiftInquiry CMDCSGiftInquiry;
				CMDCSGiftInquiry.nGiftID = nPlayID;
				CMDCSGiftInquiry.nGiftType = TGiftInfo.nGiftType;
				CMDCSGiftInquiry.nGiftCount = TGiftInfo.nGiftCount;

				// 发送信息
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_GIFT_INQUIRY, &CMDCSGiftInquiry, sizeof(CMDCSGiftInquiry) );
			}

			return true;
		}
		// 赠送清除
	case SUBC_C_GIFT_CLEAR:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_GiftClear) );
			if ( nSize != sizeof(CMDC_C_GiftClear) ) return false;

			// 难度数量
			if ( m_MapPersonalGift.GetCount() > 0 )
			{
				// 清除难度
				m_MapPersonalGift.RemoveAll();

				// 添加日志
				AppendLog( pszServerUserName, _T("清空玩家赠送。") );
			}

			// 发送消息
			SendText( pIServerControlCallback, pIServerUserItem, _T("清空玩家赠送成功。") );

			return true;
		}
		// 输赢更新
	case SUBC_C_WIN_LOSE_UPDATE:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_WinLoseUpdate) );
			if ( nSize != sizeof(CMDC_C_WinLoseUpdate) ) return false;

			// 获取当前时间
			CTime TimeCurrent( CTime::GetCurrentTime() );

			// 遍历输赢
			POSITION Pos = m_MapPersonalScore.GetStartPosition();
			while( Pos )
			{
				// 遍历信息
				uint nPlayID = 0;
				tagPersonalScore TPersonalScore;
				m_MapPersonalScore.GetNextAssoc( Pos, nPlayID, TPersonalScore );

				// 判断时间
				CTime TimePlay( TPersonalScore.TimeModify );
				if ( TimeCurrent - TimePlay > CTimeSpan(0, 0, 5, 0) )
				{
					continue;
				}

				// 定义消息
				CMDC_S_WinLoseUpdate CMDCSWinLoseUpdate;
				CMDCSWinLoseUpdate.nPlayID = nPlayID;
				CMDCSWinLoseUpdate.lPlayScore = TPersonalScore.lPlayScore;
				CopyMemory( CMDCSWinLoseUpdate.szPlayName, TPersonalScore.szPlayName, sizeof(CMDCSWinLoseUpdate.szPlayName) );

				// 发送信息
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_WIN_LOSE_UPDATE, &CMDCSWinLoseUpdate, sizeof(CMDCSWinLoseUpdate) );
			}

			// 发送信息
			pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_WIN_LOSE_UPDATE, NULL, 0);

			return true;
		}
		// 日历更新
	case SUBC_C_LOG_UPDATE:
		{
			// 效验数据
			ASSERT( nSize == sizeof(CMDC_C_LogUpdate) );
			if ( nSize != sizeof(CMDC_C_LogUpdate) ) return false;

			// 遍历日志
			for( INT_PTR nIndex = max(0, m_ArrayLog.GetCount() - 20);  nIndex < m_ArrayLog.GetCount(); ++nIndex )
			{
				// 获取信息
				tagLogoInfo & TLogoInfo = m_ArrayLog.GetAt(nIndex);

				// 定义消息
				CMDC_S_LogUpdate CMDCSLogUpdate;
				CMDCSLogUpdate.TimeModify = TLogoInfo.TimeModify;
				_sntprintf( CMDCSLogUpdate.szPlayName, CountArray(CMDCSLogUpdate.szPlayName), _T("%s"), TLogoInfo.szPlayName );
				_sntprintf( CMDCSLogUpdate.szMessage, CountArray(CMDCSLogUpdate.szMessage), _T("%s"), TLogoInfo.szMessage );
			
				// 发送信息
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_LOG_UPDATE, &CMDCSLogUpdate, sizeof(CMDCSLogUpdate) );
			}

			return true;
		}
	}


	return true;
}

// 玩家难度
bool CServerControl::GetPersonalDifficulty( uint nPlayID, double & dDifficulty )
{
	// 个人难度
	double dPersonalDifficulty = 0.0;
	if ( !m_MapPersonalDifficulty.Lookup(nPlayID, dPersonalDifficulty) )
	{
		return false;
	}

	// 更新难度
	dDifficulty = dPersonalDifficulty;

	return true;
}

// 玩家赠送
bool CServerControl::GetPersonalGift( uint nPlayID, uint & nGiftType )
{
	// 个人赠送
	tagGiftInfo TPersonalGift;
	if ( !m_MapPersonalGift.Lookup(nPlayID, TPersonalGift) )
	{
		return false;
	}

	// 更新赠送
	nGiftType = TPersonalGift.nGiftType;

	// 减少次数
	TPersonalGift.nGiftCount--;

	// 判断次数
	if( TPersonalGift.nGiftCount == 0 )
	{
		m_MapPersonalGift.RemoveKey( nPlayID );
	}
	else
	{
		m_MapPersonalGift.SetAt( nPlayID, TPersonalGift );
	}

	return true;
}

// 修改玩家分数
void CServerControl::PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore )
{
	// 查询信息
	tagPersonalScore TPersonalScore;
	if ( !m_MapPersonalScore.Lookup( nPlayID, TPersonalScore ) )
	{
		// 设置结构
		_sntprintf(TPersonalScore.szPlayName, CountArray(TPersonalScore.szPlayName), _T("%s"), pszPlayName );
	}

	// 设置结构
	TPersonalScore.lPlayScore += lPlayScore;
	TPersonalScore.TimeModify = CTime::GetCurrentTime().GetTime();

	// 设置信息
	m_MapPersonalScore.SetAt( nPlayID, TPersonalScore );
}

// 添加日志
void CServerControl::AppendLog( const tchar * pszPlayName, tchar * pText, ... )
{
	// 信息缓冲
	tchar szBuffer[1024] = { _T("") };

	// 转换信息
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// 获取信息
	tagLogoInfo TLogoInfo;
	TLogoInfo.TimeModify = CTime::GetCurrentTime().GetTime();
	_sntprintf( TLogoInfo.szPlayName, CountArray(TLogoInfo.szPlayName), _T("%s"), pszPlayName );
	_sntprintf( TLogoInfo.szMessage, CountArray(TLogoInfo.szMessage), _T("%s"), szBuffer );
	
	// 添加日志
	m_ArrayLog.Add( TLogoInfo );

	// 设置语言区域
	tchar * old_locale = _tcsdup( _tsetlocale(LC_CTYPE, NULL) );
	_tsetlocale( LC_CTYPE, _T("chs") );

	// 打开文件
	file * pFile = NULL;
	pFile = _tfopen( m_szLogoOperate, _T("a") );	
	if(pFile != NULL)	
	{
		_ftprintf( pFile, _T("%s 操作：%s \n"), pszPlayName, szBuffer );	
		fflush( pFile ); 
		fclose( pFile ); 
	}

	// 还原区域设定
	_tsetlocale( LC_CTYPE, old_locale );
	free( old_locale );
}

// 发送提示
void CServerControl::SendText( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, tchar * pText, ... )
{
	// 信息缓冲
	tchar szBuffer[1024] = { _T("") };

	// 转换信息
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);	

	// 定义消息
	CMDC_S_TextMessage CMDCSTextMessage;
	_sntprintf( CMDCSTextMessage.szMessage, CountArray(CMDCSTextMessage.szMessage), _T("%s"), szBuffer);

	// 发送消息
	pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_TEXT_MESSAGE, &CMDCSTextMessage, sizeof(CMDCSTextMessage) );
}

// 赠送类型
CString CServerControl::GiftType( uint nGiftType )
{
	if( nGiftType == 0 ) return _T("三连_AAA");
	else if( nGiftType == 1 ) return _T("三连_BBB");
	else if( nGiftType == 2 ) return _T("三连_CCC");
	else if( nGiftType == 3 ) return _T("三连_DDD");
	else if( nGiftType == 4 ) return _T("三连_EEE");
	else if( nGiftType == 5 ) return _T("三连_FFF");
	else if( nGiftType == 6 ) return _T("三连_GGG");
	else if( nGiftType == 7 ) return _T("三连_BCD");
	else if( nGiftType == 8 ) return _T("三连_HHH");
	else if( nGiftType == 9 ) return _T("三连_III");
	else if( nGiftType == 10 ) return _T("三连_II");
	else if( nGiftType == 11 ) return _T("三连_I");
	else if( nGiftType == 12 ) return _T("九连_A");
	else if( nGiftType == 13 ) return _T("九连_B");
	else if( nGiftType == 14 ) return _T("九连_C");
	else if( nGiftType == 15 ) return _T("九连_D");
	else if( nGiftType == 16 ) return _T("九连_E");
	else if( nGiftType == 17 ) return _T("九连_F");
	else if( nGiftType == 18 ) return _T("九连_G");
	else if( nGiftType == 19 ) return _T("九连_H");
	else if( nGiftType == 20 ) return _T("九连_I");
	else return _T("无效类型");
}


