#include "StdAfx.h"
#include ".\servercontrol.h"


// ��������
CServerControl CServerControl::m_ServerControl;

// ���캯��
CServerControl::CServerControl()
{
	// ���ñ���
	m_bCreateControl = false;
	m_pdRoomStock = NULL;
	m_pdRoomIdle = NULL;
	m_pdRoomLotteryPool = NULL;
	ZeroMemory(m_szLogoPersonalScore, sizeof(m_szLogoPersonalScore));
	ZeroMemory(m_szLogoOperate, sizeof(m_szLogoOperate));
}

// ��������
CServerControl::~CServerControl()
{
}

// �жϽӿ�
bool CServerControl::ISNull()
{
	return !m_bCreateControl;
}

// �ͷŽӿ�
void CServerControl::Release()
{
	// ���ļ�
	file * pFile = _tfopen( m_szLogoPersonalScore, _T("wb") );	
	if( pFile != NULL )	
	{
		// ������Ϣ
		POSITION Pos = m_MapPersonalScore.GetStartPosition();
		while ( Pos )
		{
			// ��ȡ��Ϣ
			uint nPlayID = 0;
			tagPersonalScore TPersonalScore;
			m_MapPersonalScore.GetNextAssoc( Pos, nPlayID, TPersonalScore );

			// д���ļ�
			fwrite( &nPlayID, sizeof(nPlayID), 1, pFile );
			fwrite( &TPersonalScore, sizeof(TPersonalScore), 1, pFile );
		}

		// �ر��ļ�
		fflush( pFile );
		fclose( pFile );

		// �����Ϣ
		m_MapPersonalScore.RemoveAll();
	}

	// ɾ���ڴ�
	//delete this;
}

// ��������
bool CServerControl::Create( double * pdRoomStock, double * pdRoomIdle, double * pdRoomLotteryPool, const tchar * pszLogoPersonalScore, const tchar * pszOperateLogo )
{
	// ���ñ���
	m_pdRoomStock = pdRoomStock;
	m_pdRoomIdle = pdRoomIdle;
	m_pdRoomLotteryPool = pdRoomLotteryPool;
	_sntprintf(m_szLogoPersonalScore, CountArray(m_szLogoPersonalScore), _T("%s"), pszLogoPersonalScore );
	_sntprintf(m_szLogoOperate, CountArray(m_szLogoOperate), _T("%s"), pszOperateLogo );

	// ���ļ�
	file * pFile = _tfopen( m_szLogoPersonalScore, _T("rb") );	
	if( pFile != NULL )	
	{
		// ��ȡ��Ϣ
		uint nPlayID = 0;
		tagPersonalScore TPersonalScore;
		ZeroMemory(&TPersonalScore, sizeof(TPersonalScore));
		while( fread(&nPlayID, sizeof(nPlayID), 1, pFile ) == 1 && fread(&TPersonalScore, sizeof(TPersonalScore), 1, pFile ) == 1 )
		{
			// ������Ϣ
			m_MapPersonalScore.SetAt( nPlayID, TPersonalScore );
		}

		// �ر��ļ�
		fclose( pFile );
	}

	// ���ñ�ʶ
	m_bCreateControl = true;

	return true;
}

// ��Ϣ����
bool CServerControl::OnControlMessage( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, const tchar * pszServerUserName, uint nMessageID, void * pData, uint nSize )
{
	// ��Ч�ӿ�
	if ( pIServerControlCallback == NULL ) 
	{
		return false;
	}

	// �ж���Ϣ
	switch( nMessageID )
	{
		// ����ѯ
	case SUBC_C_STOCK_INQUIRY:	
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_StockInquiry) );
			if ( nSize != sizeof(CMDC_C_StockInquiry) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_StockInquiry * pStockInquiry = (CMDC_C_StockInquiry *)pData;

			// ��ȡ��Ϣ
			double & dRoomStock = *m_pdRoomStock;
			double & dRoomIdle = *m_pdRoomIdle;
			double & dRoomLotteryPool = *m_pdRoomLotteryPool;

			// ������Ϣ
			CMDC_S_StockInquiry CMDCSStockInquiry;
			CMDCSStockInquiry.lGameStock = static_cast<longlong>(dRoomStock) + static_cast<longlong>(dRoomIdle);
			CMDCSStockInquiry.lGameJackpot = static_cast<longlong>(dRoomLotteryPool);
			CMDCSStockInquiry.bMessageBox = pStockInquiry->bActiveInquiry;

			// ������Ϣ
			pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_STOCK_INQUIRY, &CMDCSStockInquiry, sizeof(CMDCSStockInquiry) );

			return true;
		}
		// �������
	case SUBC_C_STOCK_SET:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_StockSet) );
			if ( nSize != sizeof(CMDC_C_StockSet) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_StockSet * pStockSet = (CMDC_C_StockSet *)pData;

			// ��ȡ��Ϣ
			double & dRoomStock = *m_pdRoomStock;
			double & dRoomIdle = *m_pdRoomIdle;
			double & dRoomLotteryPool = *m_pdRoomLotteryPool;

			// ��ͬ���
			double dDifficultyValue = 0.0;
			if( fabs( dRoomStock - static_cast<double>(pStockSet->lGameStock) ) < 0.00001 && fabs( dRoomLotteryPool - static_cast<double>(pStockSet->lGameJackpot) ) < 0.00001 )
			{
				return true;
			}

			// ���ÿ��
			dRoomStock = static_cast<double>(pStockSet->lGameStock);
			dRoomIdle = 0.0;
			dRoomLotteryPool = static_cast<double>(pStockSet->lGameJackpot);

			// �����־
			AppendLog( pszServerUserName, _T("���ÿ��%I64d, �ʳ�%I64d��"), pStockSet->lGameStock, pStockSet->lGameJackpot );

			// ������Ϣ
			SendText( pIServerControlCallback, pIServerUserItem, _T("���ÿ��ɹ���") );

			return true;
		}
		// �Ѷ�����
	case SUBC_C_DIFFICULTY_SET:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_DifficultySet) );
			if ( nSize != sizeof(CMDC_C_DifficultySet) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_DifficultySet * pDifficultySet = (CMDC_C_DifficultySet *)pData;

			// ��ͬ�Ѷ�ֵ
			double dDifficultyValue = 0.0;
			if( m_MapPersonalDifficulty.Lookup(pDifficultySet->nDifficultyID, dDifficultyValue) && fabs( dDifficultyValue - pDifficultySet->dDifficultyValue ) < 0.00001 )
			{
				return true;
			}

			// �����Ѷ�
			m_MapPersonalDifficulty.SetAt( pDifficultySet->nDifficultyID, pDifficultySet->dDifficultyValue );

			// �����־
			AppendLog( pszServerUserName, _T("�������%u�Ѷ�%0.5lf��"), pDifficultySet->nDifficultyID, pDifficultySet->dDifficultyValue );

			// ������Ϣ
			SendText( pIServerControlCallback, pIServerUserItem, _T("��������Ѷȳɹ���") );

			return true;
		}
		// �Ѷ�ɾ��
	case SUBC_C_DIFFICULTY_DELETE:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_DifficultyDelete) );
			if ( nSize != sizeof(CMDC_C_DifficultyDelete) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_DifficultyDelete * pDifficultySet = (CMDC_C_DifficultyDelete *)pData;

			// �����Ѷ�
			if( m_MapPersonalDifficulty.PLookup(pDifficultySet->nDifficultyID) )
			{
				m_MapPersonalDifficulty.RemoveKey( pDifficultySet->nDifficultyID );

				// �����־
				AppendLog( pszServerUserName, _T("ɾ�����%u�Ѷȡ�"), pDifficultySet->nDifficultyID );

				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("ɾ������Ѷȳɹ���") );
			}
			else
			{
				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("����Ӧ��ҡ�") );
			}

			return true;
		}
		// �ѶȲ�ѯ
	case SUBC_C_DIFFICULTY_INQUIRY:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_DifficultyInquiry) );
			if ( nSize != sizeof(CMDC_C_DifficultyInquiry) ) return false;

			// �Ѷ�����
			if ( m_MapPersonalDifficulty.GetCount() == 0 )
			{
				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("��ǰ������Ѷ����á�") );

				return true;
			}

			// �����Ѷ�
			POSITION Pos = m_MapPersonalDifficulty.GetStartPosition();
			while( Pos )
			{
				// ������Ϣ
				uint nPlayID = 0;
				double dDifficulty = 0.0;
				m_MapPersonalDifficulty.GetNextAssoc( Pos, nPlayID, dDifficulty );

				// ������Ϣ
				CMDC_S_DifficultyInquiry CMDCSDifficultyInquiry;
				CMDCSDifficultyInquiry.nDifficultyID = nPlayID;
				CMDCSDifficultyInquiry.dDifficultyValue = dDifficulty;

				// ������Ϣ
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_DIFFICULTY_INQUIRY, &CMDCSDifficultyInquiry, sizeof(CMDCSDifficultyInquiry) );
			}

			return true;
		}
		// �Ѷ����
	case SUBC_C_DIFFICULTY_CLEAR:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_DifficultyClear) );
			if ( nSize != sizeof(CMDC_C_DifficultyClear) ) return false;

			// �Ѷ�����
			if ( m_MapPersonalDifficulty.GetCount() > 0 )
			{
				// ����Ѷ�
				m_MapPersonalDifficulty.RemoveAll();

				// �����־
				AppendLog( pszServerUserName, _T("�������Ѷȡ�") );
			}

			// ������Ϣ
			SendText( pIServerControlCallback, pIServerUserItem, _T("�������Ѷȳɹ���") );

			return true;
		}
		// ��������
	case SUBC_C_GIFT_SET:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_GiftSet) );
			if ( nSize != sizeof(CMDC_C_GiftSet) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_GiftSet * pGiftSet = (CMDC_C_GiftSet *)pData;

			// ������Ϣ
			tagGiftInfo TGiftInfo;
			if( m_MapPersonalGift.Lookup( pGiftSet->nGiftID, TGiftInfo ) && TGiftInfo.nGiftCount == pGiftSet->nGiftCount && TGiftInfo.nGiftType == pGiftSet->nGiftType )
			{
				return true;
			}

			// ������Ϣ
			TGiftInfo.nGiftType = pGiftSet->nGiftType;
			TGiftInfo.nGiftCount = pGiftSet->nGiftCount;
			
			// ��������
			m_MapPersonalGift.SetAt( pGiftSet->nGiftID, TGiftInfo );

			// �����־
			AppendLog( pszServerUserName, _T("�������%u����%u��%s��"), pGiftSet->nGiftID, pGiftSet->nGiftCount, GiftType(pGiftSet->nGiftType) );
			
			// ������Ϣ
			SendText( pIServerControlCallback, pIServerUserItem, _T("����������ͳɹ���") );

			return true;
		}
		// ����ɾ��
	case SUBC_C_GIFT_DELETE:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_GiftDelete) );
			if ( nSize != sizeof(CMDC_C_GiftDelete) ) return false;

			// ��Ϣ��Ϣ
			CMDC_C_GiftDelete * pGiftDelete = (CMDC_C_GiftDelete *)pData;

			// �����Ѷ�
			if( m_MapPersonalGift.PLookup(pGiftDelete->nGiftID) )
			{
				m_MapPersonalGift.RemoveKey( pGiftDelete->nGiftID );

				// �����־
				AppendLog( pszServerUserName, _T("ɾ�����%u���͡�"), pGiftDelete->nGiftID );

				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("ɾ��������ͳɹ���") );
			}
			else
			{
				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("����Ӧ��ҡ�") );
			}

			return true;
		}
		// ���Ͳ�ѯ
	case SUBC_C_GIFT_INQUIRY:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_GiftInquiry) );
			if ( nSize != sizeof(CMDC_C_GiftInquiry) ) return false;

			// ��ѯ����
			if ( m_MapPersonalGift.GetCount() == 0 )
			{
				// ������Ϣ
				SendText( pIServerControlCallback, pIServerUserItem, _T("��ǰ������������á�") );

				return true;
			}

			// ��������
			POSITION Pos = m_MapPersonalGift.GetStartPosition();
			while( Pos )
			{
				// ������Ϣ
				uint nPlayID = 0;
				tagGiftInfo TGiftInfo;
				m_MapPersonalGift.GetNextAssoc( Pos, nPlayID, TGiftInfo );

				// ������Ϣ
				CMDC_S_GiftInquiry CMDCSGiftInquiry;
				CMDCSGiftInquiry.nGiftID = nPlayID;
				CMDCSGiftInquiry.nGiftType = TGiftInfo.nGiftType;
				CMDCSGiftInquiry.nGiftCount = TGiftInfo.nGiftCount;

				// ������Ϣ
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_GIFT_INQUIRY, &CMDCSGiftInquiry, sizeof(CMDCSGiftInquiry) );
			}

			return true;
		}
		// �������
	case SUBC_C_GIFT_CLEAR:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_GiftClear) );
			if ( nSize != sizeof(CMDC_C_GiftClear) ) return false;

			// �Ѷ�����
			if ( m_MapPersonalGift.GetCount() > 0 )
			{
				// ����Ѷ�
				m_MapPersonalGift.RemoveAll();

				// �����־
				AppendLog( pszServerUserName, _T("���������͡�") );
			}

			// ������Ϣ
			SendText( pIServerControlCallback, pIServerUserItem, _T("���������ͳɹ���") );

			return true;
		}
		// ��Ӯ����
	case SUBC_C_WIN_LOSE_UPDATE:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_WinLoseUpdate) );
			if ( nSize != sizeof(CMDC_C_WinLoseUpdate) ) return false;

			// ��ȡ��ǰʱ��
			CTime TimeCurrent( CTime::GetCurrentTime() );

			// ������Ӯ
			POSITION Pos = m_MapPersonalScore.GetStartPosition();
			while( Pos )
			{
				// ������Ϣ
				uint nPlayID = 0;
				tagPersonalScore TPersonalScore;
				m_MapPersonalScore.GetNextAssoc( Pos, nPlayID, TPersonalScore );

				// �ж�ʱ��
				CTime TimePlay( TPersonalScore.TimeModify );
				if ( TimeCurrent - TimePlay > CTimeSpan(0, 0, 5, 0) )
				{
					continue;
				}

				// ������Ϣ
				CMDC_S_WinLoseUpdate CMDCSWinLoseUpdate;
				CMDCSWinLoseUpdate.nPlayID = nPlayID;
				CMDCSWinLoseUpdate.lPlayScore = TPersonalScore.lPlayScore;
				CopyMemory( CMDCSWinLoseUpdate.szPlayName, TPersonalScore.szPlayName, sizeof(CMDCSWinLoseUpdate.szPlayName) );

				// ������Ϣ
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_WIN_LOSE_UPDATE, &CMDCSWinLoseUpdate, sizeof(CMDCSWinLoseUpdate) );
			}

			// ������Ϣ
			pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_WIN_LOSE_UPDATE, NULL, 0);

			return true;
		}
		// ��������
	case SUBC_C_LOG_UPDATE:
		{
			// Ч������
			ASSERT( nSize == sizeof(CMDC_C_LogUpdate) );
			if ( nSize != sizeof(CMDC_C_LogUpdate) ) return false;

			// ������־
			for( INT_PTR nIndex = max(0, m_ArrayLog.GetCount() - 20);  nIndex < m_ArrayLog.GetCount(); ++nIndex )
			{
				// ��ȡ��Ϣ
				tagLogoInfo & TLogoInfo = m_ArrayLog.GetAt(nIndex);

				// ������Ϣ
				CMDC_S_LogUpdate CMDCSLogUpdate;
				CMDCSLogUpdate.TimeModify = TLogoInfo.TimeModify;
				_sntprintf( CMDCSLogUpdate.szPlayName, CountArray(CMDCSLogUpdate.szPlayName), _T("%s"), TLogoInfo.szPlayName );
				_sntprintf( CMDCSLogUpdate.szMessage, CountArray(CMDCSLogUpdate.szMessage), _T("%s"), TLogoInfo.szMessage );
			
				// ������Ϣ
				pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_LOG_UPDATE, &CMDCSLogUpdate, sizeof(CMDCSLogUpdate) );
			}

			return true;
		}
	}


	return true;
}

// ����Ѷ�
bool CServerControl::GetPersonalDifficulty( uint nPlayID, double & dDifficulty )
{
	// �����Ѷ�
	double dPersonalDifficulty = 0.0;
	if ( !m_MapPersonalDifficulty.Lookup(nPlayID, dPersonalDifficulty) )
	{
		return false;
	}

	// �����Ѷ�
	dDifficulty = dPersonalDifficulty;

	return true;
}

// �������
bool CServerControl::GetPersonalGift( uint nPlayID, uint & nGiftType )
{
	// ��������
	tagGiftInfo TPersonalGift;
	if ( !m_MapPersonalGift.Lookup(nPlayID, TPersonalGift) )
	{
		return false;
	}

	// ��������
	nGiftType = TPersonalGift.nGiftType;

	// ���ٴ���
	TPersonalGift.nGiftCount--;

	// �жϴ���
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

// �޸���ҷ���
void CServerControl::PersonalScore( uint nPlayID, const tchar * pszPlayName, longlong lPlayScore )
{
	// ��ѯ��Ϣ
	tagPersonalScore TPersonalScore;
	if ( !m_MapPersonalScore.Lookup( nPlayID, TPersonalScore ) )
	{
		// ���ýṹ
		_sntprintf(TPersonalScore.szPlayName, CountArray(TPersonalScore.szPlayName), _T("%s"), pszPlayName );
	}

	// ���ýṹ
	TPersonalScore.lPlayScore += lPlayScore;
	TPersonalScore.TimeModify = CTime::GetCurrentTime().GetTime();

	// ������Ϣ
	m_MapPersonalScore.SetAt( nPlayID, TPersonalScore );
}

// �����־
void CServerControl::AppendLog( const tchar * pszPlayName, tchar * pText, ... )
{
	// ��Ϣ����
	tchar szBuffer[1024] = { _T("") };

	// ת����Ϣ
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);		

	// ��ȡ��Ϣ
	tagLogoInfo TLogoInfo;
	TLogoInfo.TimeModify = CTime::GetCurrentTime().GetTime();
	_sntprintf( TLogoInfo.szPlayName, CountArray(TLogoInfo.szPlayName), _T("%s"), pszPlayName );
	_sntprintf( TLogoInfo.szMessage, CountArray(TLogoInfo.szMessage), _T("%s"), szBuffer );
	
	// �����־
	m_ArrayLog.Add( TLogoInfo );

	// ������������
	tchar * old_locale = _tcsdup( _tsetlocale(LC_CTYPE, NULL) );
	_tsetlocale( LC_CTYPE, _T("chs") );

	// ���ļ�
	file * pFile = NULL;
	pFile = _tfopen( m_szLogoOperate, _T("a") );	
	if(pFile != NULL)	
	{
		_ftprintf( pFile, _T("%s ������%s \n"), pszPlayName, szBuffer );	
		fflush( pFile ); 
		fclose( pFile ); 
	}

	// ��ԭ�����趨
	_tsetlocale( LC_CTYPE, old_locale );
	free( old_locale );
}

// ������ʾ
void CServerControl::SendText( IServerControlCallback *	pIServerControlCallback, void * pIServerUserItem, tchar * pText, ... )
{
	// ��Ϣ����
	tchar szBuffer[1024] = { _T("") };

	// ת����Ϣ
	va_list VaList;	
	va_start(VaList,pText);	
	_vstprintf(szBuffer, pText, VaList);	
	va_end(VaList);	

	// ������Ϣ
	CMDC_S_TextMessage CMDCSTextMessage;
	_sntprintf( CMDCSTextMessage.szMessage, CountArray(CMDCSTextMessage.szMessage), _T("%s"), szBuffer);

	// ������Ϣ
	pIServerControlCallback->SendControlMessage( pIServerUserItem, SUBC_S_TEXT_MESSAGE, &CMDCSTextMessage, sizeof(CMDCSTextMessage) );
}

// ��������
CString CServerControl::GiftType( uint nGiftType )
{
	if( nGiftType == 0 ) return _T("����_AAA");
	else if( nGiftType == 1 ) return _T("����_BBB");
	else if( nGiftType == 2 ) return _T("����_CCC");
	else if( nGiftType == 3 ) return _T("����_DDD");
	else if( nGiftType == 4 ) return _T("����_EEE");
	else if( nGiftType == 5 ) return _T("����_FFF");
	else if( nGiftType == 6 ) return _T("����_GGG");
	else if( nGiftType == 7 ) return _T("����_BCD");
	else if( nGiftType == 8 ) return _T("����_HHH");
	else if( nGiftType == 9 ) return _T("����_III");
	else if( nGiftType == 10 ) return _T("����_II");
	else if( nGiftType == 11 ) return _T("����_I");
	else if( nGiftType == 12 ) return _T("����_A");
	else if( nGiftType == 13 ) return _T("����_B");
	else if( nGiftType == 14 ) return _T("����_C");
	else if( nGiftType == 15 ) return _T("����_D");
	else if( nGiftType == 16 ) return _T("����_E");
	else if( nGiftType == 17 ) return _T("����_F");
	else if( nGiftType == 18 ) return _T("����_G");
	else if( nGiftType == 19 ) return _T("����_H");
	else if( nGiftType == 20 ) return _T("����_I");
	else return _T("��Ч����");
}


