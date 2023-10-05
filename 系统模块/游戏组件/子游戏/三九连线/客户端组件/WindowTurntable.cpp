#include "StdAfx.h"
#include ".\windowturntable.h"

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowTurntable, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_LBUTTONDOWN()
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowTurntable::CWindowTurntable()
{
	// 变量信息
	m_bSpeedUp = false;
	m_bElementMove = false;
	m_nKeepTurn = false;
	m_nAcceptedElement = false;
	ZeroMemory( m_nElementMoveCount, sizeof(m_nElementMoveCount) );

	// 生成初始宝石
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			m_nElementIndex[nWidthIndex][nHeightIndex] = nWidthIndex + nHeightIndex * 3;
		}
	}

	// 提示动画
	m_bLinePrompt = false;
	m_bNineLinePrompt = false;
	ZeroMemory( &m_bThreeLinePrompt, sizeof(m_bThreeLinePrompt) );
	ZeroMemory( &m_bThreeLineElement, sizeof(m_bThreeLineElement) );

	// 声音缓冲
	m_LCSoundTime.Initialization();
}

// 析构函数
CWindowTurntable::~CWindowTurntable()
{
	// 清空元素
	EmptyElementMove();
}

// 窗口创建 
void CWindowTurntable::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建图片
	m_TextureElement[Element_A].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_1") );
	m_TextureElement[Element_B].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_2") );
	m_TextureElement[Element_C].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_3") );
	m_TextureElement[Element_D].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_4") );
	m_TextureElement[Element_E].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_5") );
	m_TextureElement[Element_F].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_6") );
	m_TextureElement[Element_G].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_7") );
	m_TextureElement[Element_H].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_8") );
	m_TextureElement[Element_I].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_9") );
	m_TextureElement[Element_Deform].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("元素_资源"), _T("资源"), _T("元素_癞子") );
	
	// 图片大小
	m_SizeElementTexture.SetSize( m_TextureElement[Element_A].GetWidth(), m_TextureElement[Element_A].GetHeight() );
	m_SizeElementArea.SetSize( nWidth / TURN_WIDTH_COUNT, nHeight / TURN_HEIGHT_COUNT );

	// 元素基准位置
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			m_PointDatumElement[nWidthIndex][nHeightIndex].SetPoint( nWidth / 6 * (nWidthIndex * 2 + 1), nHeight / 6 * (nHeightIndex * 2 + 1) );
		}
	}
}

// 窗口动画
void CWindowTurntable::OnWindowMovie()
{
	// 旋转动画
	if ( m_bElementMove )
	{
		// 移动数量
		int nMoveCountMax[TURN_WIDTH_COUNT] = { 10, (10 + (m_bSpeedUp ? 10 : 24)), (10 + (m_bSpeedUp ? 10 : 24) * 2) };
// 		for (int i = 0; i < TURN_WIDTH_COUNT; i++)
// 		{
// 			nMoveCountMax[i] = nMoveCountMax[i] * 2 /( m_bSpeedUp ? 3 : 2);
// 		}

		// 遍历信息
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			// 计算速度
			int nSpeedMove = 0;

			// 保持移动
			if ( m_nKeepTurn )
			{	
				nSpeedMove =  28;
			}
			// 计算速度
			else if ( nMoveCountMax[nWidthIndex] > m_nElementMoveCount[nWidthIndex] )
			{
				nSpeedMove = nMoveCountMax[nWidthIndex] - m_nElementMoveCount[nWidthIndex];
				nSpeedMove = nSpeedMove * 28 / 10;
				nSpeedMove = min( nSpeedMove, 28 );
				nSpeedMove = max( nSpeedMove, 8 );
			}

			// 有速度才移动
			if ( nSpeedMove != 0 )
			{
				// 遍历信息
				for( int nIndex = 0; nIndex < m_TElementMoveInfo[nWidthIndex].GetCount();)
				{
					// 获取信息
					tagElementMove * pElementMove = m_TElementMoveInfo[nWidthIndex].GetAt( nIndex );

					// 向下移动
					pElementMove->PointSite.y += nSpeedMove;

					// 结束移动
					if ( pElementMove->PointSite.y >= m_PointDatumElement[nWidthIndex][TURN_HEIGHT_COUNT -1].y + m_SizeElementArea.cy )
					{
						// 获取最后一个信息
						tagElementMove * pElementLast = m_TElementMoveInfo[nWidthIndex].GetAt( m_TElementMoveInfo[nWidthIndex].GetCount() - 1 );

						// 新建元素
						pElementMove->nElementIndex = rand() % Element_Max;
						pElementMove->PointSite.x = pElementLast->PointSite.x;
						pElementMove->PointSite.y = pElementLast->PointSite.y - m_SizeElementArea.cy;

						// 最后3个
						int nLastThreeIndex = nMoveCountMax[nWidthIndex] - m_nElementMoveCount[nWidthIndex] - 2;
						if ( nLastThreeIndex < 3 && nLastThreeIndex >= 0 )
						{
							pElementMove->nElementIndex = m_nElementIndex[nWidthIndex][ nLastThreeIndex ];
						}

						// 删除元素
						m_TElementMoveInfo[nWidthIndex].RemoveAt( nIndex );

						// 添加元素
						m_TElementMoveInfo[nWidthIndex].Add( pElementMove );

						// 保持移动不会计算数量
						if( !m_nKeepTurn )
						{
							// 添加数量
							m_nElementMoveCount[nWidthIndex]++;
						}

						// 播放声音
						if( m_LCSoundTime.GetLapseCount( 60 ) )
						{
							CSound::_Object()->PlaySound( _T("ElementMove.ogg") );
						}

						// 移动结束
						if ( nMoveCountMax[nWidthIndex] == m_nElementMoveCount[nWidthIndex] )
						{
							// 更新位置
							for( int nSiteIndex = 0; nSiteIndex < 3 && nSiteIndex < m_TElementMoveInfo[nWidthIndex].GetCount(); ++nSiteIndex )
							{
								// 获取信息
								tagElementMove * pElementSite = m_TElementMoveInfo[nWidthIndex].GetAt( nSiteIndex );

								// 设置位置
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

		// 移动结束
		if ( nMoveCountMax[0] == m_nElementMoveCount[0] && nMoveCountMax[1] == m_nElementMoveCount[1] && nMoveCountMax[2] == m_nElementMoveCount[2] )
		{
			// 设置变量
			m_bElementMove = false;

			// 发送结束消息
			CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );
		}
	}
}


// 窗口绘画 
void CWindowTurntable::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 获取绘画
	dex::CD3DDrawBasic * pD3DDrawBasic = dex::CD3DDrawBasic::_Object();

	// 绘画状态
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		{
			// 设置状态
			m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].SetEffectType( dex::TextureEffect_WindowBack );
		}
	}

	// 普通绘画
	if ( !m_bElementMove )
	{
		// 绘画元素
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
			{
				// 计算位置
				CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

				// 绘画元素
				m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y );
			}
		}

		//// 提示动画
		//if ( m_bLinePrompt )
		//{
		//	// 绘画状态
		//	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//	{
		//		for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//		{
		//			// 设置状态
		//			m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].SetEffectType( dex::TextureEffect_Shine );
		//		}
		//	}

		//	// 9连绘画
		//	if ( m_bNineLinePrompt )
		//	{
		//		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//		{
		//			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//			{
		//				// 计算位置
		//				CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

		//				// 绘画元素
		//				m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y, (byte)abs( (int)(GetTickCount() % 500 - 250) ) );
		//			}
		//		}
		//	}
		//	// 3连绘画
		//	else
		//	{
		//		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		//		{
		//			for ( int nHeightIndex = 0; nHeightIndex < TURN_HEIGHT_COUNT; ++nHeightIndex )
		//			{
		//				// 提示绘画
		//				if ( m_bThreeLineElement[nWidthIndex][nHeightIndex] )
		//				{
		//					// 计算位置
		//					CPoint PointTexture( PointTextureElement(m_PointDatumElement[nWidthIndex][nHeightIndex]) );

		//					// 绘画元素
		//					m_TextureElement[m_nElementIndex[nWidthIndex][nHeightIndex]].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y, (byte)abs( (int)(GetTickCount() % 500 - 250) ) );
		//				}
		//			}
		//		}
		//	}
		//}
	}
	// 移动绘画
	else
	{
		// 遍历信息
		for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
		{
			// 遍历信息
			for( int nIndex = 0; nIndex < m_TElementMoveInfo[nWidthIndex].GetCount(); ++nIndex )
			{
				// 获取信息
				tagElementMove *& pElementMove = m_TElementMoveInfo[nWidthIndex].GetAt( nIndex );

				// 计算位置
				CPoint PointTexture( PointTextureElement(pElementMove->PointSite) );

				// 绘画元素
				m_TextureElement[pElementMove->nElementIndex].DrawTexture( pD3DDevice, PointTexture.x, PointTexture.y );
			}
		}
	}

	
}

// 左键点下 
void CWindowTurntable::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{

}


// 转动元素
void CWindowTurntable::TurnElement()
{
	// 清空元素
	EmptyElementMove();

	// 动画开始
	m_nKeepTurn = true;
	m_bElementMove = true;
	m_nAcceptedElement = false;
	ZeroMemory( m_nElementMoveCount, sizeof(m_nElementMoveCount) );

	// 提示动画
	m_bLinePrompt = false;
	m_bNineLinePrompt = false;
	ZeroMemory( &m_bThreeLinePrompt, sizeof(m_bThreeLinePrompt) );
	ZeroMemory( &m_bThreeLineElement, sizeof(m_bThreeLineElement) );

	// 声音缓冲
	m_LCSoundTime.Initialization();

	// 设置元素
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		for ( int nHeightIndex = TURN_HEIGHT_COUNT - 1; nHeightIndex >= 0; --nHeightIndex )
		{
			// 新建元素
			tagElementMove * pElementMove = new tagElementMove;
			pElementMove->nElementIndex = m_nElementIndex[nWidthIndex][nHeightIndex];
			pElementMove->PointSite = m_PointDatumElement[nWidthIndex][nHeightIndex];

			// 添加元素
			m_TElementMoveInfo[nWidthIndex].Add( pElementMove );	
		}
	}

	// 添加末尾元素
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		// 新建元素
		tagElementMove * pElementMove = new tagElementMove;
		pElementMove->nElementIndex = rand() % Element_Max;
		pElementMove->PointSite.x = m_PointDatumElement[nWidthIndex][0].x;
		pElementMove->PointSite.y = m_PointDatumElement[nWidthIndex][0].y - m_SizeElementArea.cy;

		// 添加元素
		m_TElementMoveInfo[nWidthIndex].Add( pElementMove );	
	}
}

// 停止元素
void CWindowTurntable::StopElement( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], bool bStopNew )
{
	// 保存信息
	m_nAcceptedElement = true;
	CopyMemory( m_nElementIndex, nElementIndex, sizeof(m_nElementIndex) );

	// 动画中
	if ( m_bElementMove )
	{
		// 立即停止
		if( bStopNew )
		{
			// 设置变量
			m_nKeepTurn = false;
			m_bElementMove = false;

			// 发送结束消息
			CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );
		}
		else if( m_nKeepTurn )
		{
			// 减慢旋转
			m_nKeepTurn = false;
		}
	}
}

// 连线提示
bool CWindowTurntable::LinePrompt()
{
	// 未接受到元素
	if ( !m_nAcceptedElement )
		return false;

	// 判断9连
	if ( CGameLogic::_Object()->ISNineLine(m_nElementIndex) )
	{
		// 设置变量
		m_bLinePrompt = true;
		m_bNineLinePrompt = true;
	}
	else
	{
		// 判断3连
		m_bThreeLinePrompt[LINE_WIDTH_1] = CGameLogic::_Object()->ISThreeLineWidth1(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_WIDTH_2] = CGameLogic::_Object()->ISThreeLineWidth2(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_WIDTH_3] = CGameLogic::_Object()->ISThreeLineWidth3(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_4] = CGameLogic::_Object()->ISThreeLineHeight4(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_5] = CGameLogic::_Object()->ISThreeLineHeight5(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_HEIGHT_6] = CGameLogic::_Object()->ISThreeLineHeight6(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_OBLIQUE_7] = CGameLogic::_Object()->ISThreeLineOblique7(m_nElementIndex) != MultipleThree_Invalid;
		m_bThreeLinePrompt[LINE_OBLIQUE_8] = CGameLogic::_Object()->ISThreeLineOblique8(m_nElementIndex) != MultipleThree_Invalid;

		// 判断提示元素
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

		// 判断动画
		m_bLinePrompt = m_bThreeLinePrompt[LINE_WIDTH_1] || m_bThreeLinePrompt[LINE_WIDTH_2] || m_bThreeLinePrompt[LINE_WIDTH_3] || m_bThreeLinePrompt[LINE_HEIGHT_4]
						|| m_bThreeLinePrompt[LINE_HEIGHT_5] || m_bThreeLinePrompt[LINE_HEIGHT_6] || m_bThreeLinePrompt[LINE_OBLIQUE_7] || m_bThreeLinePrompt[LINE_OBLIQUE_8];
	}

	return true;
}

// 停止元素
bool CWindowTurntable::ForcedStopElement()
{
	// 未接受到元素
	if ( !m_nAcceptedElement )
		return false;
	
	// 设置变量
	m_nKeepTurn = false;
	m_bElementMove = false;

	// 发送结束消息
	CGameFrameEngine::GetInstance()->SendMessage( IDM_TURN_OVER, 0, 0 );

	return true;
}

// 计算图片位置
CPoint CWindowTurntable::PointTextureElement( CPoint PointElement )
{
	return CPoint( PointElement.x - m_SizeElementTexture.cx / 2, PointElement.y - m_SizeElementTexture.cy / 2 );
}

// 清空动画
void CWindowTurntable::EmptyElementMove()
{
	// 遍历信息
	for ( int nWidthIndex = 0; nWidthIndex < TURN_WIDTH_COUNT; ++nWidthIndex )
	{
		// 清空信息
		DeleteArray(m_TElementMoveInfo[nWidthIndex]);
	}
}




