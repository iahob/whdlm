#include "StdAfx.h"
#include ".\windowturntable.h"

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowTurntable, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowTurntable::CWindowTurntable()
{
	// ������Ϣ
	m_bSpeedUp = false;
	m_bElementMove = false;
	m_nKeepTurn = false;
	m_nAcceptedElement = false;
	ZeroMemory( m_nElementMoveCount, sizeof(m_nElementMoveCount) );

	// ���ɳ�ʼ��ʯ
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			m_nElementIndex[nWidthIndex][nHeightIndex] = nWidthIndex + nHeightIndex * 3;
		}
	}

	// ��ʾ����
	m_bLinePrompt = false;
	m_bNineLinePrompt = false;
	ZeroMemory( &m_bThreeLinePrompt, sizeof(m_bThreeLinePrompt) );
	ZeroMemory( &m_bThreeLineElement, sizeof(m_bThreeLineElement) );

	// ��������
	m_LCSoundTime.Initialization();
}

// ��������
CWindowTurntable::~CWindowTurntable()
{
	// ���Ԫ��
	EmptyElementMove();
}

// ���ڴ��� 
void CWindowTurntable::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����ͼƬ
	m_TextureElement[Element_A].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_1") );
	m_TextureElement[Element_B].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_2") );
	m_TextureElement[Element_C].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_3") );
	m_TextureElement[Element_D].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_4") );
	m_TextureElement[Element_E].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_5") );
	m_TextureElement[Element_F].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_6") );
	m_TextureElement[Element_G].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_7") );
	m_TextureElement[Element_H].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_8") );
	m_TextureElement[Element_I].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_9") );
	m_TextureElement[Element_Deform].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ԫ��_��Դ"), _T("��Դ"), _T("Ԫ��_���") );
	
	// ͼƬ��С
	m_SizeElementTexture.SetSize( m_TextureElement[Element_A].GetWidth(), m_TextureElement[Element_A].GetHeight() );
	m_SizeElementArea.SetSize( nWidth / TURN_WIDTH_COUNT, nHeight / TURN_HEIGHT_COUNT );

	// Ԫ�ػ�׼λ��
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			m_PointDatumElement[nWidthIndex][nHeightIndex].SetPoint( nWidth / 6 * (nWidthIndex * 2 + 1), nHeight / 6 * (nHeightIndex * 2 + 1) );
		}
	}
}

// ���ڶ���
void CWindowTurntable::OnWindowMovie()
{
	// ��ת����
	if ( m_bElementMove )
	{
		// �ƶ�����
		int nMoveCountMax[TURN_WIDTH_COUNT] = { 10, (10 + (m_bSpeedUp ? 10 : 24)), (10 + (m_bSpeedUp ? 10 : 24) * 2) };
// 		for (int i = 0; i < TURN_WIDTH_COUNT; i++)
// 		{
// 			nMoveCountMax[i] = nMoveCountMax[i] * 2 /( m_bSpeedUp ? 3 : 2);
// 		}

		// ������Ϣ
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			// �����ٶ�
			int nSpeedMove = 0;

			// �����ƶ�
			if ( m_nKeepTurn )
			{	
				nSpeedMove =  28;
			}
			// �����ٶ�
			else if ( nMoveCountMax[nWidthIndex] > m_nElementMoveCount[nWidthIndex] )
			{
				nSpeedMove = nMoveCountMax[nWidthIndex] - m_nElementMoveCount[nWidthIndex];
				nSpeedMove = nSpeedMove * 28 / 10;
				nSpeedMove = min( nSpeedMove, 28 );
				nSpeedMove = max( nSpeedMove, 8 );
			}

			// ���ٶȲ��ƶ�
			if ( nSpeedMove != 0 )
			{
				// ������Ϣ
				for( int nIndex = 0; nIndex < m_TElementMoveInfo[nWidthIndex].GetCount();)
				{
					// ��ȡ��Ϣ
					tagElementMove * pElementMove = m_TElementMoveInfo[nWidthIndex].GetAt( nIndex );

					// �����ƶ�
					pElementMove->PointSite.y += nSpeedMove;

					// �����ƶ�
					if ( pElementMove->PointSite.y >= m_PointDatumElement[nWidthIndex][TURN_HEIGHT_COUNT -1].y + m_SizeElementArea.cy )
					{
						// ��ȡ���һ����Ϣ
						tagElementMove * pElementLast = m_TElementMoveInfo[nWidthIndex].GetAt( m_TElementMoveInfo[nWidthIndex].GetCount() - 1 );

						// �½�Ԫ��
						pElementMove->nElementIndex = rand() % Element_Max;
						pElementMove->PointSite.x = pElementLast->PointSite.x;
						pElementMove->PointSite.y = pElementLast->PointSite.y - m_SizeElementArea.cy;

						// ���3��
						int nLastThreeIndex = nMoveCountMax[nWidthIndex] - m_nElementMoveCount[nWidthIndex] - 2;
						if ( nLastThreeIndex < 3 && nLastThreeIndex >= 0 )
						{
							pElementMove->nElementIndex = m_nElementIndex[nWidthIndex][ nLastThreeIndex ];
						}

						// ɾ��Ԫ��
						m_TElementMoveInfo[nWidthIndex].RemoveAt( nIndex );

						// ���Ԫ��
						m_TElementMoveInfo[nWidthIndex].Add( pElementMove );

						// �����ƶ������������
						if( !m_nKeepTurn )
						{
							// �������
							m_nElementMoveCount[nWidthIndex]++;
						}

						// ��������
						if( m_LCSoundTime.GetLapseCount( 60 ) )
						{
							CSound::_Object()->PlaySound( _T("ElementMove.ogg") );
						}

						// �ƶ�����
						if ( nMoveCountMax[nWidthIndex] == m_nElementMoveCount[nWidthIndex] )
						{
							// ����λ��
							for( int nSiteIndex = 0; nSiteIndex < 3 && nSiteIndex < m_TElementMoveInfo[nWidthIndex].GetCount(); ++nSiteIndex )
							{
								// ��ȡ��Ϣ
								tagElementMove * pElementSite = m_TElementMoveInfo[nWidthIndex].GetAt( nSiteIndex );

								// ����λ��
								pElementSite->PointSite = m_PointDatumElement[nWidthIndex][ 3 - nSiteIndex - 1 ];
							}

							break;
						}
					}
					else
					{
						++nIndex;
					}
				}
			}
		}

		// �ƶ�����
		if ( nMoveCountMax[0] == m_nElementMoveCount[0] && nMoveCountMax[1] == m_nElementMoveCount[1] && nMoveCountMax[2] == m_nElementMoveCount[2] )
		{
			// ���ñ���
			m_bElementMove = false;

			// ���ͽ�����Ϣ
			CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );
		}
	}
}


// ���ڻ滭 
void CWindowTurntable::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��ȡ�滭
	dex::CD3DDrawBasic * pD3DDrawBasic = dex::CD3DDrawBasic::_Object();

	// �滭״̬
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			// ����״̬
			m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].SetEffectType( dex::TextureEffect_WindowBack );
		}
	}

	// ��ͨ�滭
	if ( !m_bElementMove )
	{
		// �滭Ԫ��
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
			{
				// ����λ��
				CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

				// �滭Ԫ��
				m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y );
			}
		}

		//// ��ʾ����
		//if ( m_bLinePrompt )
		//{
		//	// �滭״̬
		//	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//	{
		//		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//		{
		//			// ����״̬
		//			m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].SetEffectType( dex::TextureEffect_Shine );
		//		}
		//	}

		//	// 9���滭
		//	if ( m_bNineLinePrompt )
		//	{
		//		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//		{
		//			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//			{
		//				// ����λ��
		//				CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

		//				// �滭Ԫ��
		//				m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y, (byte)abs( (int)(GetTickCount() % 500 - 250) ) );
		//			}
		//		}
		//	}
		//	// 3���滭
		//	else
		//	{
		//		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//		{
		//			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//			{
		//				// ��ʾ�滭
		//				if ( m_bThreeLineElement[nWidthIndex][nHeightIndex] )
		//				{
		//					// ����λ��
		//					CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

		//					// �滭Ԫ��
		//					m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y, (byte)abs( (int)(GetTickCount() % 500 - 250) ) );
		//				}
		//			}
		//		}
		//	}
		//}
	}
	// �ƶ��滭
	else
	{
		// ������Ϣ
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			// ������Ϣ
			for( int nIndex = 0; nIndex < m_TElementMoveInfo[nWidthIndex].GetCount(); ++nIndex )
			{
				// ��ȡ��Ϣ
				tagElementMove *& pElementMove = m_TElementMoveInfo[nWidthIndex].GetAt( nIndex );

				// ����λ��
				CPoint PointTexture( PointTextureElement(pElementMove->PointSite) );

				// �滭Ԫ��
				m_TextureElement[pElementMove->nElementIndex].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y );
			}
		}
	}

	
}

// ������� 
void CWindowTurntable::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{

}


// ת��Ԫ��
void CWindowTurntable::TurnElement()
{
	// ���Ԫ��
	EmptyElementMove();

	// ������ʼ
	m_nKeepTurn = true;
	m_bElementMove = true;
	m_nAcceptedElement = false;
	ZeroMemory( m_nElementMoveCount, sizeof(m_nElementMoveCount) );

	// ��ʾ����
	m_bLinePrompt = false;
	m_bNineLinePrompt = false;
	ZeroMemory( &m_bThreeLinePrompt, sizeof(m_bThreeLinePrompt) );
	ZeroMemory( &m_bThreeLineElement, sizeof(m_bThreeLineElement) );

	// ��������
	m_LCSoundTime.Initialization();

	// ����Ԫ��
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = TURN_HEIGHT_COUNT - 1; nHeightIndex >= 0; --nHeightIndex )
		{
			// �½�Ԫ��
			tagElementMove * pElementMove = new tagElementMove;
			pElementMove->nElementIndex = m_nElementIndex[nWidthIndex][nHeightIndex];
			pElementMove->PointSite = m_PointDatumElement[nWidthIndex][nHeightIndex];

			// ���Ԫ��
			m_TElementMoveInfo[nWidthIndex].Add( pElementMove );	
		}
	}

	// ���ĩβԪ��
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		// �½�Ԫ��
		tagElementMove * pElementMove = new tagElementMove;
		pElementMove->nElementIndex = rand() % Element_Max;
		pElementMove->PointSite.x = m_PointDatumElement[nWidthIndex][0].x;
		pElementMove->PointSite.y = m_PointDatumElement[nWidthIndex][0].y - m_SizeElementArea.cy;

		// ���Ԫ��
		m_TElementMoveInfo[nWidthIndex].Add( pElementMove );	
	}
}

// ֹͣԪ��
void CWindowTurntable::StopElement( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], bool bStopNew )
{
	// ������Ϣ
	m_nAcceptedElement = true;
	CopyMemory( m_nElementIndex, nElementIndex, sizeof(m_nElementIndex) );

	// ������
	if ( m_bElementMove )
	{
		// ����ֹͣ
		if( bStopNew )
		{
			// ���ñ���
			m_nKeepTurn = false;
			m_bElementMove = false;

			// ���ͽ�����Ϣ
			CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );
		}
		else if( m_nKeepTurn )
		{
			// ������ת
			m_nKeepTurn = false;
		}
	}
}

// ������ʾ
bool CWindowTurntable::LinePrompt()
{
	// δ���ܵ�Ԫ��
	if ( !m_nAcceptedElement )
		return false;

	// �ж�9��
	if ( CGameLogic::_Object()->ISNineLine(m_nElementIndex) )
	{
		// ���ñ���
		m_bLinePrompt = true;
		m_bNineLinePrompt = true;
	}
	else
	{
		// �ж�3��
		m_bThreeLinePrompt[LINE_WIDTH_1] = CGameLogic::_Object()->ISThreeLineWidth1(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_WIDTH_2] = CGameLogic::_Object()->ISThreeLineWidth2(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_WIDTH_3] = CGameLogic::_Object()->ISThreeLineWidth3(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_4] = CGameLogic::_Object()->ISThreeLineHeight4(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_5] = CGameLogic::_Object()->ISThreeLineHeight5(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_6] = CGameLogic::_Object()->ISThreeLineHeight6(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_OBLIQUE_7] = CGameLogic::_Object()->ISThreeLineOblique7(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_OBLIQUE_8] = CGameLogic::_Object()->ISThreeLineOblique8(m_nElementIndex) != MultipleThree_Invalid;

		// �ж���ʾԪ��
		if ( m_bThreeLinePrompt[LINE_WIDTH_1] )
		{
			m_bThreeLineElement[LINE_WIDTH_1_1] = true;
			m_bThreeLineElement[LINE_WIDTH_1_2] = true;
			m_bThreeLineElement[LINE_WIDTH_1_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_WIDTH_2] )
		{
			m_bThreeLineElement[LINE_WIDTH_2_1] = true;
			m_bThreeLineElement[LINE_WIDTH_2_2] = true;
			m_bThreeLineElement[LINE_WIDTH_2_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_WIDTH_3] )
		{
			m_bThreeLineElement[LINE_WIDTH_3_1] = true;
			m_bThreeLineElement[LINE_WIDTH_3_2] = true;
			m_bThreeLineElement[LINE_WIDTH_3_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_HEIGHT_4] )
		{
			m_bThreeLineElement[LINE_HEIGHT_4_1] = true;
			m_bThreeLineElement[LINE_HEIGHT_4_2] = true;
			m_bThreeLineElement[LINE_HEIGHT_4_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_HEIGHT_5] )
		{
			m_bThreeLineElement[LINE_HEIGHT_5_1] = true;
			m_bThreeLineElement[LINE_HEIGHT_5_2] = true;
			m_bThreeLineElement[LINE_HEIGHT_5_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_HEIGHT_6] )
		{
			m_bThreeLineElement[LINE_HEIGHT_6_1] = true;
			m_bThreeLineElement[LINE_HEIGHT_6_2] = true;
			m_bThreeLineElement[LINE_HEIGHT_6_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_OBLIQUE_7] )
		{
			m_bThreeLineElement[LINE_OBLIQUE_7_1] = true;
			m_bThreeLineElement[LINE_OBLIQUE_7_2] = true;
			m_bThreeLineElement[LINE_OBLIQUE_7_3] = true;
		}
		if ( m_bThreeLinePrompt[LINE_OBLIQUE_8] )
		{
			m_bThreeLineElement[LINE_OBLIQUE_8_1] = true;
			m_bThreeLineElement[LINE_OBLIQUE_8_2] = true;
			m_bThreeLineElement[LINE_OBLIQUE_8_3] = true;
		}

		// �ж϶���
		m_bLinePrompt = m_bThreeLinePrompt[LINE_WIDTH_1] || m_bThreeLinePrompt[LINE_WIDTH_2] || m_bThreeLinePrompt[LINE_WIDTH_3] || m_bThreeLinePrompt[LINE_HEIGHT_4]
						|| m_bThreeLinePrompt[LINE_HEIGHT_5] || m_bThreeLinePrompt[LINE_HEIGHT_6] || m_bThreeLinePrompt[LINE_OBLIQUE_7] || m_bThreeLinePrompt[LINE_OBLIQUE_8];
	}

	return true;
}

// ֹͣԪ��
bool CWindowTurntable::ForcedStopElement()
{
	// δ���ܵ�Ԫ��
	if ( !m_nAcceptedElement )
		return false;
	
	// ���ñ���
	m_nKeepTurn = false;
	m_bElementMove = false;

	// ���ͽ�����Ϣ
	CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );

	return true;
}

// ����ͼƬλ��
CPoint CWindowTurntable::PointTextureElement( CPoint PointElement )
{
	return CPoint( PointElement.x - m_SizeElementTexture.cx / 2, PointElement.y - m_SizeElementTexture.cy / 2 );
}

// ��ն���
void CWindowTurntable::EmptyElementMove()
{
	// ������Ϣ
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		// �����Ϣ
		DeleteArray(m_TElementMoveInfo[nWidthIndex]);
	}
}




