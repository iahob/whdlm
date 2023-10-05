#include "StdAfx.h"
#include ".\windowwin.h"

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CWindowWin, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// ���캯��
CWindowWin::CWindowWin()
{
	// ��������
	m_nBackIndex = 0;
	m_nBackTime = 60;
	m_nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
	m_nMultipleType = MultipleType_Max;

	// ������Ϣ
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	ZeroMemory(m_bTaskComplete, sizeof(m_bTaskComplete));

	// ��Ҷ���
	m_nGoldCount = int_max;
	m_bHeldDrop = true;
	m_lWinScore = 0;
	m_lTaskScore = 0;
	m_nNineLogoIndex = 0;
}

// ��������
CWindowWin::~CWindowWin()
{
	// ��ұ���
	for( int nArrayIndex = 0; nArrayIndex < ARRAY_GOLD_MAX; ++nArrayIndex )
	{
		// ɾ�����
		DeleteArray(m_ArrayGold[nArrayIndex]);
	}

	// ɾ������
	DeleteArray(m_ArrayFlash);
	DeleteArray(m_ArrayThreeStarMovie);
}


// ���ڴ��� 
void CWindowWin::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ����ͼƬ
	m_TextureThreeBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_1") );
	m_TextureThreeBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_2") );
	m_TextureThreeBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_3") );
	m_TextureThreeBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_4") );
	m_TextureThreeBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_5") );
	m_TextureThreeBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_6") );
	m_TextureThreeBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_7") );
	m_TextureThreeBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_8") );
	m_TextureThreeBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_9") );
	m_TextureThreeBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_10") );
	m_TextureThreeBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_11") );
	m_TextureThreeBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_12") );
	m_TextureThreeBack[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_13") );
	m_TextureThreeBack[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_14") );
	m_TextureThreeBack[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_15") );
	m_TextureThreeBack[15].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_16") );
	m_TextureThreeBack[16].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_17") );
	m_TextureThreeStar[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_1") );
	m_TextureThreeStar[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_2") );
	m_TextureThreeStar[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_3") );
	m_TextureThreeStar[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_4") );
	m_TextureThreeStar[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_5") );
	m_TextureThreeStar[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_6") );
	m_TextureThreeStar[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_7") );
	m_TextureThreeStar[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_8") );
	m_TextureThreeStar[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_9") );
	m_TextureThreeStar[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_10") );
	m_TextureThreeStar[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_11") );
	m_TextureThreeStar[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_12") );
	m_TextureThreeStar[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_13") );
	m_TextureThreeStar[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_14") );
	m_TextureThreeStar[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����_15") );
	m_TextureThreeRay.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_2"), _T("��Դ"), _T("����_����") );
	
	m_TextureNineBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_1") );
	m_TextureNineBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_2") );
	m_TextureNineBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_3") );
	m_TextureNineBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_4") );
	m_TextureNineBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_5") );
	m_TextureNineBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_6") );
	m_TextureNineBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_7") );
	m_TextureNineBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_8") );
	m_TextureNineBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_9") );
	m_TextureNineBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_10") );
	m_TextureNineBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_11") );
	m_TextureNineBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_12") );
	m_TextureNineLogo[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_����_1") );
	m_TextureNineLogo[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����_����_2") );
	m_TextureNineLight[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_ɨ��_1") );
	m_TextureNineLight[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_ɨ��_2") );
	m_TextureNineLight[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_ɨ��_3") );
	m_TextureNineLight[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_ɨ��_4") );
	m_TextureNineLight[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_ɨ��_5") );
	m_TextureNineRay.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����") );
	m_TextureNineNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����_1"), _T("��Դ"), _T("����_����") );

	m_TextureGold[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_1") );
	m_TextureGold[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_2") );
	m_TextureGold[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_3") );
	m_TextureGold[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_4") );
	m_TextureGold[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_5") );
	m_TextureGold[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_6") );
	m_TextureGold[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_7") );
	m_TextureGold[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_8") );
	m_TextureGold[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_9") );
	m_TextureGold[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("���_10") );
	m_TextureNumber1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������_1") );
	m_TextureNumber2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������_2") );
	m_TextureNumber3.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������_3") );
	m_TextureNumber4.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������_4") );
	m_TextureNumber5.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������_5") );
	m_TextureComma1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("����_1") );
	m_TextureComma2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("����_2") );
	m_TextureComma3.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("����_3") );
	m_TextureComma4.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("����_4") );
	m_TextureLight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("���"), _T("��Դ"), _T("�ƶ����") );
	m_TextureTaskLogo.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("������") );
	m_TextureTaskNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("Ӯ����_����"), _T("��Դ"), _T("��������") );
}

// ���ڶ���
void CWindowWin::OnWindowMovie()
{
	// ��ӽ��
	if( m_nGoldCount < ( m_bHeldDrop ? 70 : 35) )
	{
		// ��ȡ���ڴ�С
		CRect RectWindow;
		GetClientRect(RectWindow);

		// ��ӽ��
		m_ArrayGold[0].Add( new CGoldMove( CDoublePoint(RectWindow.Width() / 2 - 170 + rand() % 340, RectWindow.Height() / 2 - 50 + rand() % 220) ) );
		m_ArrayGold[1].Add( new CGoldMove( CDoublePoint(RectWindow.Width() / 2 - 170 + rand() % 340, RectWindow.Height() / 2 - 50 + rand() % 220) ) );

		// �������
		m_nGoldCount++;
	}

	// ��ұ���
	for( int nArrayIndex = 0; nArrayIndex < ARRAY_GOLD_MAX; ++nArrayIndex )
	{
		// ����ƶ�
		int nGoldIndex = 0;

		// ������
		while( nGoldIndex < m_ArrayGold[nArrayIndex].GetCount() )
		{
			// ��ȡ��Ϣ
			CGoldMove *& pGoldMove = m_ArrayGold[nArrayIndex].GetAt( nGoldIndex );

			// �ж��ƶ�
			if( !pGoldMove->Move() )
			{
				// ɾ����Ϣ
				SafeDelete(pGoldMove);

				// �Ƴ���Ϣ
				m_ArrayGold[nArrayIndex].RemoveAt(nGoldIndex);
			}
			else
			{
				// ��һ��
				nGoldIndex++;
			}
		}
	}

	// �����ƶ�
	int nFlashIndex = 0;

	// ������
	while( nFlashIndex < m_ArrayFlash.GetCount() )
	{
		// ��ȡ��Ϣ
		CLightMove *& pFlashMove = m_ArrayFlash.GetAt( nFlashIndex );

		// �ж��ƶ�
		if( !pFlashMove->Move() )
		{
			// ɾ����Ϣ
			SafeDelete(pFlashMove);

			// �Ƴ���Ϣ
			m_ArrayFlash.RemoveAt(nFlashIndex);
		}
		else
		{
			// ��һ��
			nFlashIndex++;
		}
	}

	// ��������
	int nThreeStarIndex = 0;
	while( nThreeStarIndex < m_ArrayThreeStarMovie.GetCount() )
	{
		// ��ȡ��Ϣ
		tagThreeStarMovie *& pThreeStarMovie = m_ArrayThreeStarMovie.GetAt( nThreeStarIndex );

		// �ж��ƶ�
		if( !pThreeStarMovie->ExecuteFrame() )
		{
			// ɾ����Ϣ
			SafeDelete(pThreeStarMovie);

			// �Ƴ���Ϣ
			m_ArrayThreeStarMovie.RemoveAt( nThreeStarIndex );

			// ��һ��
			continue;
		}

		// ��һ��
		nThreeStarIndex++;
	}
	

	// ��������
	if ( m_nLCBackTime.GetLapseCount( m_nBackTime ) > 0 )
	{
		// �޸�����
		switch( m_nMultipleType )
		{
		case MultipleType_Three:
			{
				// ��������
				m_nBackIndex = min(m_nBackIndex + 1, TEXTRUE_THREE_BACK - 1);

				// ������⶯��
				if ( m_nBackIndex == 3 )
				{
					// ��������
					tagThreeStarMovie * pThreeStarMovie = new tagThreeStarMovie;
					pThreeStarMovie->ptPosition.SetPoint( SCREEN_CX / 2, SCREEN_CY / 2 - 75 );
					pThreeStarMovie->StartFrame( TEXTRUE_THREE_STAR, 60, 1 );

					// ��Ӷ���
					m_ArrayThreeStarMovie.Add( pThreeStarMovie );
				}
			}
			break;
		case MultipleType_Nine:
			{
				// ���Ų���
				if ( m_nBackIndex == TEXTRUE_NINE_BACK - 1 )
				{
					// ѭ������
					m_nBackIndex = TEXTRUE_NINE_BACK - 2;
				}
				else
				{
					// ��������
					m_nBackIndex = min(m_nBackIndex + 1, TEXTRUE_NINE_BACK - 1);
				}

				// ����ʱ��
				if( m_nBackIndex == TEXTRUE_NINE_BACK - 1 )
				{
					m_nBackTime = 200;
				}
			}
			break;
		}
	}

	// �ƹ����
	if ( m_nLCFlashTime.GetLapseCount(3000) > 0 && m_ArrayPointFlash.GetCount() > 0 )
	{
		// λ������
		int nPointIndex[3];
		nPointIndex[0] = rand() % (int)m_ArrayPointFlash.GetCount();
		nPointIndex[1] = (nPointIndex[0] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();
		nPointIndex[2] = (nPointIndex[1] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();

		// ��ӹ��
		for ( int nCountIndex = 0; nCountIndex < rand()%2 + 2; ++nCountIndex )
		{
			m_ArrayFlash.Add( new CLightMove(m_ArrayPointFlash[nPointIndex[nCountIndex]], rand() % 500 + 3000, 0.8f, 0.8f ));
		}
	}
}


// ���ڻ滭 
void CWindowWin::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	switch( m_nMultipleType )
	{
		// �����滭
	case MultipleType_Three:
		{
			// ��ת�Ƕ�
			Static float fRotation = 0.f;

			// ����ͼƬ
			m_TextureThreeRay.SetRotation( dex::TextureRotation_Z, m_TextureThreeRay.GetWidth() / 2, m_TextureThreeRay.GetHeight() / 2, (fRotation += 0.02f) );
			m_TextureThreeRay.SetEffectType( dex::TextureEffect_WindowBack );

			// �滭����
			m_TextureThreeRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureThreeRay.GetWidth() / 2, nHeight / 2 - m_TextureThreeRay.GetHeight() / 2);
		}
		break;
		// �����滭
	case MultipleType_Nine:
		{
			// ��ת�Ƕ�
			Static float fRotation = 0.f;

			// ����ͼƬ
			m_TextureNineRay.SetRotation( dex::TextureRotation_Z, m_TextureNineRay.GetWidth() / 2, m_TextureNineRay.GetHeight() / 2, (fRotation -= 0.003f) );

			// ����ͼƬ
			m_TextureNineRay.SetEffectType( dex::TextureEffect_WindowBack );

			// �滭����
			m_TextureNineRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineRay.GetWidth() / 2, nHeight / 2 - m_TextureNineRay.GetHeight() / 2 );

			// ����ͼƬ
			m_TextureNineRay.SetEffectType( dex::TextureEffect_Shine );

			// ����͸��
			byte cbAlpha = (byte)(abs( (int)(150 - (GetTickCount() / 3 % 300)) ) );

			// �滭����
			m_TextureNineRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineRay.GetWidth() / 2, nHeight / 2 - m_TextureNineRay.GetHeight() / 2, cbAlpha);

		}
		break;
	}
	
	// �滭���	
	DrawGold( pD3DDevice, m_ArrayGold[0] );

	// �滭����
	switch( m_nMultipleType )
	{
		// �����滭
	case MultipleType_Three:
		{
			// �滭����
			m_TextureThreeBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureThreeBack[m_nBackIndex].GetWidth() / 2, nHeight / 2 - m_TextureThreeBack[m_nBackIndex].GetHeight() / 2);

			// �滭Ԫ��
			EnumMultipleThree nMultipleThree = (EnumMultipleThree)m_nMultipleIndex;
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );

			// Ԫ�ش�С
			CSize SizeElementBig( 160, 160 );
			CSize SizeElementSmall( 140, 140 );
	
			// �滭ͼƬ
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, nWidth / 2 - SizeElementBig.cx / 2 - SizeElementSmall.cx + 45, nHeight / 2 - SizeElementSmall.cy + 5, SizeElementSmall.cx, SizeElementSmall.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, nWidth / 2 + SizeElementBig.cx / 2 - 45, nHeight / 2 - SizeElementSmall.cy + 5, SizeElementSmall.cx, SizeElementSmall.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, nWidth / 2 - SizeElementBig.cx / 2, nHeight / 2 - SizeElementBig.cy + 5, SizeElementBig.cx, SizeElementBig.cy );	
	
			// �滭����
			if( m_lTaskScore > 0 )
			{
				// ��������
				tchar szTaskScore[64];
				_sntprintf( szTaskScore, CountArray(szTaskScore), TEXT("%I64d"), m_lTaskScore );

				// ���ֿ��
				int nNumberLen = m_TextureTaskNumber.GetWidth() / 10 * lstrlen(szTaskScore);
				int nDrawLen = nNumberLen + 20 + m_TextureTaskLogo.GetWidth();

				// �滭��ʶ
				m_TextureTaskLogo.DrawTexture( pD3DDevice, nWidth / 2 - nDrawLen / 2, nHeight / 2 + 90 );
				CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskNumber, _T("0123456789"), szTaskScore, nWidth / 2 - nDrawLen / 2 + 20 + m_TextureTaskLogo.GetWidth(), nHeight / 2 + 90 );
			}

			// ��ʾ����
			for( int nThreeStarIndex = 0; nThreeStarIndex < m_ArrayThreeStarMovie.GetCount(); nThreeStarIndex++ )
			{
				// ��ȡ��Ϣ
				tagThreeStarMovie *& pThreeStarMovie = m_ArrayThreeStarMovie.GetAt( nThreeStarIndex );

				// �滭ͼƬ
				dex::CD3DTexture & TextureThreeStar = m_TextureThreeStar[pThreeStarMovie->nFrameCurrent];
				TextureThreeStar.DrawTexture( pD3DDevice, (int)pThreeStarMovie->ptPosition.x - TextureThreeStar.GetWidth() / 2, (int)pThreeStarMovie->ptPosition.y - TextureThreeStar.GetHeight() / 2 );
			}

			// �滭����
			DrawScore( pD3DDevice, nWidth, nHeight );
		}
		break;
		// �����滭
	case MultipleType_Nine:
		{
			// �滭����
			m_TextureNineBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineBack[m_nBackIndex].GetWidth() / 2, nHeight / 2 - m_TextureNineBack[m_nBackIndex].GetHeight() / 2);

			// �滭Ԫ��
			EnumMultipleNine nMultipleNine = (EnumMultipleNine)(m_nMultipleIndex - MultipleThree_Max);
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleNineToTextureElement( nMultipleNine );

			// Ԫ�ش�С
			CSize SizeElement( 190, 190 );

			// �滭ͼƬ
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, nWidth / 2 - SizeElement.cx / 2, nHeight / 2 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );	
			
			// �滭��־
			m_TextureNineLogo[m_nNineLogoIndex].DrawTexture( pD3DDevice, nWidth / 2 - m_TextureNineLogo[m_nNineLogoIndex].GetWidth() / 2, nHeight / 2 + 50 );

			// �ƹ�����
			if( GetTickCount() % (60 * TEXTRUE_NINE_LIGHT * 6) < 60 * TEXTRUE_NINE_LIGHT )
			{
				int nLightIndex = GetTickCount() / 60 % TEXTRUE_NINE_LIGHT;
				m_TextureNineLight[nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineLight[nLightIndex].GetWidth() / 2 + 5, nHeight / 2 + 60 );
			}

			// �滭����
			CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNineNumber, _T("0123456789,"), CGlobal::_Object()->AppendComma(m_lWinScore), nWidth / 2, nHeight / 2 + 193, DN_CENTER|DN_VCENTER );

		}
		break;
	}

	// �滭���
	m_TextureLight.SetEffectType( dex::TextureEffect_Shine );
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLight, m_ArrayFlash );

	// �滭���
	DrawGold( pD3DDevice, m_ArrayGold[1] );
}

// �滭����
void CWindowWin::DrawScore( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber5, NULL, CPoint(0, 6) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber4, &m_TextureComma4, CPoint(0, 6) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber3, &m_TextureComma3, CPoint(0, 0) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber2, &m_TextureComma2, CPoint(0, 0) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber1, &m_TextureComma1, CPoint(0, 0) );
}

// �滭����
void CWindowWin::DrawScore( dex::CD3DDevice * pD3DDevice, CWHArray< tagWinNumberInfo > & ArrayNumberInfo, dex::CD3DTexture * pImageNumber, dex::CD3DTexture * pImageComma, CPoint PointOffset )
{
	for( int nIndex = 0; nIndex < ArrayNumberInfo.GetCount(); ++nIndex )
	{
		// ��ȡ��Ϣ
		tagWinNumberInfo & TWinNumberInfo = ArrayNumberInfo.GetAt( nIndex );

		// �ж���Ϣ
		int nImageIndex = (TWinNumberInfo.nNumberIndex != -1) ? TWinNumberInfo.nNumberIndex : 0;
		float fNumberSize = TWinNumberInfo.fNumberSize;
		dex::CD3DTexture * pImageInfo = (TWinNumberInfo.nNumberIndex != -1) ? pImageNumber : pImageComma;

		// ��ЧͼƬ
		if ( pImageInfo == NULL )
			continue;

		// �����С
		CSize SizeImage( pImageInfo->GetWidth() / TWinNumberInfo.nImageFrame, pImageInfo->GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * fNumberSize, (float)SizeImage.cy * fNumberSize );

		// �滭ͼƬ
		pImageInfo->DrawTexture(pD3DDevice, (int)(TWinNumberInfo.PointNumber.x - SizeDest.fWidth / 2.f) + PointOffset.x, (int)(TWinNumberInfo.PointNumber.y - SizeDest.fHeight / 2.f) + PointOffset.y, (int)SizeDest.fWidth, (int)SizeDest.fHeight, nImageIndex * SizeImage.cx, 0, SizeImage.cx, SizeImage.cy );
	}
}

// �滭���
void CWindowWin::DrawGold( dex::CD3DDevice * pD3DDevice, CWHArray< CGoldMove * > & ArrayGold )
{
	// �滭���
	for( int nGoldIndex = 0; nGoldIndex < ArrayGold.GetCount(); ++nGoldIndex )
	{
		// ��ȡ��Ϣ
		CGoldMove *& pGoldMove = ArrayGold.GetAt( nGoldIndex );

		// ���̫С
		if( pGoldMove->fAlpha < 0.01 || pGoldMove->fSize < 0.01 )
			continue;

		// ͼƬ����
		dex::CD3DTexture * pTextureGold = &m_TextureGold[ pGoldMove->cbFrameIndex%TEXTRUE_GOLD_MAX ];

		// ͼƬ��С
		CSize SizeImage( pTextureGold->GetWidth(), pTextureGold->GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * pGoldMove->fSize, (float)SizeImage.cy * pGoldMove->fSize );

		// �滭ͼƬ
		pTextureGold->DrawTexture(pD3DDevice, (int)(pGoldMove->ptPosition.x - SizeDest.fWidth / 2.f), (int)(pGoldMove->ptPosition.y - SizeDest.fHeight / 2.f), (int)SizeDest.fWidth, (int)SizeDest.fHeight, 0, 0, SizeImage.cx, SizeImage.cy, (byte)(pGoldMove->fAlpha * 255.f) );
	} 
}


// ��ӽ��
void CWindowWin::AppendGold( bool bHeldDrop )
{
	m_nGoldCount = 0;
	m_bHeldDrop = bHeldDrop;
}

// ��������Ŀ��
void CWindowWin::SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskTarget, nTaskTarget, sizeof(m_nTaskTarget) );
}

// ���ý��
void CWindowWin::SetWinScore( int nMultipleIndex, longlong lWinScore, longlong lTaskScore, bool bTaskComplete[TASK_INDEX_MAX] )
{
	// ��������
	m_nBackIndex = 0;
	m_nBackTime = 60;
	m_nLCBackTime.Initialization();

	// ɾ������
	m_ArrayPointFlash.RemoveAll();
	m_ArrayNumberWin.RemoveAll();

	// ������Ϣ
	m_nMultipleIndex = nMultipleIndex;
	CopyMemory( m_bTaskComplete, bTaskComplete, sizeof(m_bTaskComplete) );

	// �жϽ���
	if( m_nMultipleIndex < MultipleThree_Max )
	{
		// �����н�
		m_nMultipleType = MultipleType_Three;
	}
	else if ( m_nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// �����н�
		m_nMultipleType = MultipleType_Nine;
	
		// ��������
		m_nNineLogoIndex = (m_nNineLogoIndex + 1) % TEXTRUE_NINE_LOGO;
	}

	// ɾ������
	DeleteArray(m_ArrayFlash);

	// ��ȡ����
	CRect RectWindow;
	GetWindowRect( RectWindow );

	// ����λ��
	CPoint PointWinScore;
	switch( m_nMultipleType )
	{
	case MultipleType_Three:
		{
			PointWinScore.SetPoint( RectWindow.Width() / 2, RectWindow.Height() / 2 + 10 );
		}
		break;
	case MultipleType_Nine:
		{
			PointWinScore.SetPoint( RectWindow.Width() / 2, RectWindow.Height() / 2 );
		}
		break;
	}

	// ���ɷ���
	m_lWinScore = lWinScore;
	GenerateNumber(m_ArrayNumberWin, PointWinScore, lWinScore, 1.f );

	// �������
	m_lTaskScore = lTaskScore;

	// ������Ϣ
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 - 182, RectWindow.Height() / 2 - 35) );
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 + 21, RectWindow.Height() / 2 - 93) );
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 + 195, RectWindow.Height() / 2 - 74) );

	// ��ӹ��
	m_nLCFlashTime.Initialization();

	// λ������
	int nPointIndex[3];
	nPointIndex[0] = rand() % (int)m_ArrayPointFlash.GetCount();
	nPointIndex[1] = (nPointIndex[0] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();
	nPointIndex[2] = (nPointIndex[1] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();

	// ��ӹ��
	for ( int nCountIndex = 0; nCountIndex < rand()%2 + 2; ++nCountIndex )
	{
		m_ArrayFlash.Add( new CLightMove(m_ArrayPointFlash[nPointIndex[nCountIndex]], rand() % 500 + 3000, 0.8f, 0.8f ));
	}

}

// ��������
void CWindowWin::GenerateNumber( CWHArray< tagWinNumberInfo > & ArrayNumberInfo, CPoint PointCenter, longlong lNumber, float fSize )
{
	// ������Ϣ
	tchar szNumber[64] = { _T("") };
	tchar szOutNumber[64] = { _T("") };
	tchar szImageNumber[64] = { _T("0123456789") };
	_sntprintf( szNumber, CountArray(szNumber), TEXT("%I64d"), lNumber );
	_sntprintf( szOutNumber, CountArray(szOutNumber), TEXT("%s"), CGlobal::_Object()->AppendComma( lNumber ) );

	// ���ֳ���
	int nNumberLen = lstrlen(szNumber);
	int nOutNumberLen = lstrlen(szOutNumber);
	int nImageNumberLen = lstrlen(szImageNumber);

	// ���ֿ��
	int nNumberWidth = (int)((float)(m_TextureNumber1.GetWidth() / nImageNumberLen - 5) * fSize);
	int nCommaWidth = (int)((float)(m_TextureComma1.GetWidth() - 25) * fSize);

	// �滭λ��
	int nDrawLen = nNumberWidth * nNumberLen + (nOutNumberLen - nNumberLen) * nCommaWidth;
	CPoint PointNumber(PointCenter.x - nDrawLen / 2 + (int)((float)(m_TextureNumber1.GetWidth() / nImageNumberLen) * fSize) / 2, PointCenter.y );

	// ��������
	for ( int nOutIndex = 0; nOutIndex < nOutNumberLen; ++nOutIndex )
	{
		// ��ӹ��
		if ( nOutIndex == 0 )
		{
			m_ArrayPointFlash.Add( CDoublePoint(PointNumber.x, PointNumber.y + 28) );
		}
		else if( nOutIndex == nOutNumberLen - 1 )
		{
			m_ArrayPointFlash.Add( CDoublePoint(PointNumber.x + 22, PointNumber.y - 3) );
		}

		// ��������
		for ( int nImageIndex = 0; nImageIndex < nImageNumberLen; ++nImageIndex )
		{
			// ���ֻ滭
			if ( szOutNumber[nOutIndex] == szImageNumber[nImageIndex] && szOutNumber[nOutIndex] != '\0' )
			{
				// �����Ϣ
				tagWinNumberInfo TWinNumberInfo;
				TWinNumberInfo.nNumberIndex = nImageIndex;
				TWinNumberInfo.nImageFrame = nImageNumberLen;
				TWinNumberInfo.fNumberSize = fSize;
				TWinNumberInfo.PointNumber.SetPoint( PointNumber.x, PointNumber.y );
				ArrayNumberInfo.Add( TWinNumberInfo );

				// ƫ��λ��
				PointNumber.Offset( nNumberWidth, 0 );

				break;
			}
			// ���Ż滭
			else if ( szOutNumber[nOutIndex] == ',' )
			{
				// �����Ϣ
				tagWinNumberInfo TWinNumberInfo;
				TWinNumberInfo.nNumberIndex = -1;
				TWinNumberInfo.nImageFrame = 1;
				TWinNumberInfo.fNumberSize = fSize;
				TWinNumberInfo.PointNumber.SetPoint( PointNumber.x - 8, PointNumber.y + 25);
				ArrayNumberInfo.Add( TWinNumberInfo );

				// ƫ��λ��
				PointNumber.Offset( nCommaWidth, 0 );

				break;
			}
		}
	}
}





