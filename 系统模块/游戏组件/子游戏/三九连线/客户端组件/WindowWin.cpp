#include "StdAfx.h"
#include ".\windowwin.h"

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CWindowWin, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_WINDOW_MOVIE()
END_WHDUI_MESSAGE_MAP()

// 构造函数
CWindowWin::CWindowWin()
{
	// 背景动画
	m_nBackIndex = 0;
	m_nBackTime = 60;
	m_nMultipleIndex = MultipleThree_Max + MultipleNine_Max;
	m_nMultipleType = MultipleType_Max;

	// 任务信息
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	ZeroMemory(m_bTaskComplete, sizeof(m_bTaskComplete));

	// 金币动画
	m_nGoldCount = int_max;
	m_bHeldDrop = true;
	m_lWinScore = 0;
	m_lTaskScore = 0;
	m_nNineLogoIndex = 0;
}

// 析构函数
CWindowWin::~CWindowWin()
{
	// 金币遍历
	for( int nArrayIndex = 0; nArrayIndex < ARRAY_GOLD_MAX; ++nArrayIndex )
	{
		// 删除金币
		DeleteArray(m_ArrayGold[nArrayIndex]);
	}

	// 删除闪光
	DeleteArray(m_ArrayFlash);
	DeleteArray(m_ArrayThreeStarMovie);
}


// 窗口创建 
void CWindowWin::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建图片
	m_TextureThreeBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_1") );
	m_TextureThreeBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_2") );
	m_TextureThreeBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_3") );
	m_TextureThreeBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_4") );
	m_TextureThreeBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_5") );
	m_TextureThreeBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_6") );
	m_TextureThreeBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_7") );
	m_TextureThreeBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_8") );
	m_TextureThreeBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_9") );
	m_TextureThreeBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_10") );
	m_TextureThreeBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_11") );
	m_TextureThreeBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_12") );
	m_TextureThreeBack[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_13") );
	m_TextureThreeBack[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_14") );
	m_TextureThreeBack[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_1"), _T("资源"), _T("三连_背景_15") );
	m_TextureThreeBack[15].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_背景_16") );
	m_TextureThreeBack[16].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_背景_17") );
	m_TextureThreeStar[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_1") );
	m_TextureThreeStar[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_2") );
	m_TextureThreeStar[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_3") );
	m_TextureThreeStar[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_4") );
	m_TextureThreeStar[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_5") );
	m_TextureThreeStar[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_6") );
	m_TextureThreeStar[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_7") );
	m_TextureThreeStar[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_8") );
	m_TextureThreeStar[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_9") );
	m_TextureThreeStar[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_10") );
	m_TextureThreeStar[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_11") );
	m_TextureThreeStar[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_12") );
	m_TextureThreeStar[12].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_13") );
	m_TextureThreeStar[13].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_14") );
	m_TextureThreeStar[14].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_星星_15") );
	m_TextureThreeRay.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_三连_2"), _T("资源"), _T("三连_光晕") );
	
	m_TextureNineBack[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_1") );
	m_TextureNineBack[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_2") );
	m_TextureNineBack[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_3") );
	m_TextureNineBack[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_4") );
	m_TextureNineBack[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_5") );
	m_TextureNineBack[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_6") );
	m_TextureNineBack[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_7") );
	m_TextureNineBack[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_8") );
	m_TextureNineBack[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_9") );
	m_TextureNineBack[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_10") );
	m_TextureNineBack[10].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_11") );
	m_TextureNineBack[11].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_背景_12") );
	m_TextureNineLogo[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_数字_背景_1") );
	m_TextureNineLogo[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_数字_背景_2") );
	m_TextureNineLight[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_扫光_1") );
	m_TextureNineLight[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_扫光_2") );
	m_TextureNineLight[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_扫光_3") );
	m_TextureNineLight[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_扫光_4") );
	m_TextureNineLight[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_扫光_5") );
	m_TextureNineRay.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_光晕") );
	m_TextureNineNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢界面_九连_1"), _T("资源"), _T("九连_数字") );

	m_TextureGold[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_1") );
	m_TextureGold[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_2") );
	m_TextureGold[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_3") );
	m_TextureGold[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_4") );
	m_TextureGold[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_5") );
	m_TextureGold[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_6") );
	m_TextureGold[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_7") );
	m_TextureGold[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_8") );
	m_TextureGold[8].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_9") );
	m_TextureGold[9].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("金币_10") );
	m_TextureNumber1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("分数数字_1") );
	m_TextureNumber2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("分数数字_2") );
	m_TextureNumber3.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("分数数字_3") );
	m_TextureNumber4.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("分数数字_4") );
	m_TextureNumber5.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("分数数字_5") );
	m_TextureComma1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("逗号_1") );
	m_TextureComma2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("逗号_2") );
	m_TextureComma3.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("逗号_3") );
	m_TextureComma4.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("逗号_4") );
	m_TextureLight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("光电"), _T("资源"), _T("移动光电") );
	m_TextureTaskLogo.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("任务奖励") );
	m_TextureTaskNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("赢窗口_零碎"), _T("资源"), _T("奖励数字") );
}

// 窗口动画
void CWindowWin::OnWindowMovie()
{
	// 添加金币
	if( m_nGoldCount < ( m_bHeldDrop ? 70 : 35) )
	{
		// 获取窗口大小
		CRect RectWindow;
		GetClientRect(RectWindow);

		// 添加金币
		m_ArrayGold[0].Add( new CGoldMove( CDoublePoint(RectWindow.Width() / 2 - 170 + rand() % 340, RectWindow.Height() / 2 - 50 + rand() % 220) ) );
		m_ArrayGold[1].Add( new CGoldMove( CDoublePoint(RectWindow.Width() / 2 - 170 + rand() % 340, RectWindow.Height() / 2 - 50 + rand() % 220) ) );

		// 添加数量
		m_nGoldCount++;
	}

	// 金币遍历
	for( int nArrayIndex = 0; nArrayIndex < ARRAY_GOLD_MAX; ++nArrayIndex )
	{
		// 金币移动
		int nGoldIndex = 0;

		// 编译金币
		while( nGoldIndex < m_ArrayGold[nArrayIndex].GetCount() )
		{
			// 获取信息
			CGoldMove *& pGoldMove = m_ArrayGold[nArrayIndex].GetAt( nGoldIndex );

			// 判断移动
			if( !pGoldMove->Move() )
			{
				// 删除信息
				SafeDelete(pGoldMove);

				// 移除信息
				m_ArrayGold[nArrayIndex].RemoveAt(nGoldIndex);
			}
			else
			{
				// 下一个
				nGoldIndex++;
			}
		}
	}

	// 闪光移动
	int nFlashIndex = 0;

	// 编译光电
	while( nFlashIndex < m_ArrayFlash.GetCount() )
	{
		// 获取信息
		CLightMove *& pFlashMove = m_ArrayFlash.GetAt( nFlashIndex );

		// 判断移动
		if( !pFlashMove->Move() )
		{
			// 删除信息
			SafeDelete(pFlashMove);

			// 移除信息
			m_ArrayFlash.RemoveAt(nFlashIndex);
		}
		else
		{
			// 下一个
			nFlashIndex++;
		}
	}

	// 三连动画
	int nThreeStarIndex = 0;
	while( nThreeStarIndex < m_ArrayThreeStarMovie.GetCount() )
	{
		// 获取信息
		tagThreeStarMovie *& pThreeStarMovie = m_ArrayThreeStarMovie.GetAt( nThreeStarIndex );

		// 判断移动
		if( !pThreeStarMovie->ExecuteFrame() )
		{
			// 删除信息
			SafeDelete(pThreeStarMovie);

			// 移除信息
			m_ArrayThreeStarMovie.RemoveAt( nThreeStarIndex );

			// 下一个
			continue;
		}

		// 下一个
		nThreeStarIndex++;
	}
	

	// 背景动画
	if ( m_nLCBackTime.GetLapseCount( m_nBackTime ) > 0 )
	{
		// 修改索引
		switch( m_nMultipleType )
		{
		case MultipleType_Three:
			{
				// 增加索引
				m_nBackIndex = min(m_nBackIndex + 1, TEXTRUE_THREE_BACK - 1);

				// 添加闪光动画
				if ( m_nBackIndex == 3 )
				{
					// 创建动画
					tagThreeStarMovie * pThreeStarMovie = new tagThreeStarMovie;
					pThreeStarMovie->ptPosition.SetPoint( SCREEN_CX / 2, SCREEN_CY / 2 - 75 );
					pThreeStarMovie->StartFrame( TEXTRUE_THREE_STAR, 60, 1 );

					// 添加动画
					m_ArrayThreeStarMovie.Add( pThreeStarMovie );
				}
			}
			break;
		case MultipleType_Nine:
			{
				// 播放操作
				if ( m_nBackIndex == TEXTRUE_NINE_BACK - 1 )
				{
					// 循环播放
					m_nBackIndex = TEXTRUE_NINE_BACK - 2;
				}
				else
				{
					// 增加索引
					m_nBackIndex = min(m_nBackIndex + 1, TEXTRUE_NINE_BACK - 1);
				}

				// 更新时间
				if( m_nBackIndex == TEXTRUE_NINE_BACK - 1 )
				{
					m_nBackTime = 200;
				}
			}
			break;
		}
	}

	// 灯光添加
	if ( m_nLCFlashTime.GetLapseCount(3000) > 0 && m_ArrayPointFlash.GetCount() > 0 )
	{
		// 位置索引
		int nPointIndex[3];
		nPointIndex[0] = rand() % (int)m_ArrayPointFlash.GetCount();
		nPointIndex[1] = (nPointIndex[0] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();
		nPointIndex[2] = (nPointIndex[1] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();

		// 添加光电
		for ( int nCountIndex = 0; nCountIndex < rand()%2 + 2; ++nCountIndex )
		{
			m_ArrayFlash.Add( new CLightMove(m_ArrayPointFlash[nPointIndex[nCountIndex]], rand() % 500 + 3000, 0.8f, 0.8f ));
		}
	}
}


// 窗口绘画 
void CWindowWin::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画光晕
	switch( m_nMultipleType )
	{
		// 三连绘画
	case MultipleType_Three:
		{
			// 旋转角度
			Static float fRotation = 0.f;

			// 设置图片
			m_TextureThreeRay.SetRotation( dex::TextureRotation_Z, m_TextureThreeRay.GetWidth() / 2, m_TextureThreeRay.GetHeight() / 2, (fRotation += 0.02f) );
			m_TextureThreeRay.SetEffectType( dex::TextureEffect_WindowBack );

			// 绘画光晕
			m_TextureThreeRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureThreeRay.GetWidth() / 2, nHeight / 2 - m_TextureThreeRay.GetHeight() / 2);
		}
		break;
		// 九连绘画
	case MultipleType_Nine:
		{
			// 旋转角度
			Static float fRotation = 0.f;

			// 设置图片
			m_TextureNineRay.SetRotation( dex::TextureRotation_Z, m_TextureNineRay.GetWidth() / 2, m_TextureNineRay.GetHeight() / 2, (fRotation -= 0.003f) );

			// 设置图片
			m_TextureNineRay.SetEffectType( dex::TextureEffect_WindowBack );

			// 绘画光晕
			m_TextureNineRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineRay.GetWidth() / 2, nHeight / 2 - m_TextureNineRay.GetHeight() / 2 );

			// 设置图片
			m_TextureNineRay.SetEffectType( dex::TextureEffect_Shine );

			// 计算透明
			byte cbAlpha = (byte)(abs( (int)(150 - (GetTickCount() / 3 % 300)) ) );

			// 绘画光晕
			m_TextureNineRay.DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineRay.GetWidth() / 2, nHeight / 2 - m_TextureNineRay.GetHeight() / 2, cbAlpha);

		}
		break;
	}
	
	// 绘画金币	
	DrawGold( pD3DDevice, m_ArrayGold[0] );

	// 绘画主体
	switch( m_nMultipleType )
	{
		// 三连绘画
	case MultipleType_Three:
		{
			// 绘画背景
			m_TextureThreeBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureThreeBack[m_nBackIndex].GetWidth() / 2, nHeight / 2 - m_TextureThreeBack[m_nBackIndex].GetHeight() / 2);

			// 绘画元素
			EnumMultipleThree nMultipleThree = (EnumMultipleThree)m_nMultipleIndex;
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );

			// 元素大小
			CSize SizeElementBig( 160, 160 );
			CSize SizeElementSmall( 140, 140 );
	
			// 绘画图片
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, nWidth / 2 - SizeElementBig.cx / 2 - SizeElementSmall.cx + 45, nHeight / 2 - SizeElementSmall.cy + 5, SizeElementSmall.cx, SizeElementSmall.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, nWidth / 2 + SizeElementBig.cx / 2 - 45, nHeight / 2 - SizeElementSmall.cy + 5, SizeElementSmall.cx, SizeElementSmall.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, nWidth / 2 - SizeElementBig.cx / 2, nHeight / 2 - SizeElementBig.cy + 5, SizeElementBig.cx, SizeElementBig.cy );	
	
			// 绘画任务
			if( m_lTaskScore > 0 )
			{
				// 任务数字
				tchar szTaskScore[64];
				_sntprintf( szTaskScore, CountArray(szTaskScore), TEXT("%I64d"), m_lTaskScore );

				// 数字宽度
				int nNumberLen = m_TextureTaskNumber.GetWidth() / 10 * lstrlen(szTaskScore);
				int nDrawLen = nNumberLen + 20 + m_TextureTaskLogo.GetWidth();

				// 绘画标识
				m_TextureTaskLogo.DrawTexture( pD3DDevice, nWidth / 2 - nDrawLen / 2, nHeight / 2 + 90 );
				CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskNumber, _T("0123456789"), szTaskScore, nWidth / 2 - nDrawLen / 2 + 20 + m_TextureTaskLogo.GetWidth(), nHeight / 2 + 90 );
			}

			// 显示动画
			for( int nThreeStarIndex = 0; nThreeStarIndex < m_ArrayThreeStarMovie.GetCount(); nThreeStarIndex++ )
			{
				// 获取信息
				tagThreeStarMovie *& pThreeStarMovie = m_ArrayThreeStarMovie.GetAt( nThreeStarIndex );

				// 绘画图片
				dex::CD3DTexture & TextureThreeStar = m_TextureThreeStar[pThreeStarMovie->nFrameCurrent];
				TextureThreeStar.DrawTexture( pD3DDevice, (int)pThreeStarMovie->ptPosition.x - TextureThreeStar.GetWidth() / 2, (int)pThreeStarMovie->ptPosition.y - TextureThreeStar.GetHeight() / 2 );
			}

			// 绘画分数
			DrawScore( pD3DDevice, nWidth, nHeight );
		}
		break;
		// 九连绘画
	case MultipleType_Nine:
		{
			// 绘画背景
			m_TextureNineBack[m_nBackIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineBack[m_nBackIndex].GetWidth() / 2, nHeight / 2 - m_TextureNineBack[m_nBackIndex].GetHeight() / 2);

			// 绘画元素
			EnumMultipleNine nMultipleNine = (EnumMultipleNine)(m_nMultipleIndex - MultipleThree_Max);
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleNineToTextureElement( nMultipleNine );

			// 元素大小
			CSize SizeElement( 190, 190 );

			// 绘画图片
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, nWidth / 2 - SizeElement.cx / 2, nHeight / 2 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );	
			
			// 绘画标志
			m_TextureNineLogo[m_nNineLogoIndex].DrawTexture( pD3DDevice, nWidth / 2 - m_TextureNineLogo[m_nNineLogoIndex].GetWidth() / 2, nHeight / 2 + 50 );

			// 灯光索引
			if( GetTickCount() % (60 * TEXTRUE_NINE_LIGHT * 6) < 60 * TEXTRUE_NINE_LIGHT )
			{
				int nLightIndex = GetTickCount() / 60 % TEXTRUE_NINE_LIGHT;
				m_TextureNineLight[nLightIndex].DrawTexture(pD3DDevice, nWidth / 2 - m_TextureNineLight[nLightIndex].GetWidth() / 2 + 5, nHeight / 2 + 60 );
			}

			// 绘画数字
			CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureNineNumber, _T("0123456789,"), CGlobal::_Object()->AppendComma(m_lWinScore), nWidth / 2, nHeight / 2 + 193, DN_CENTER|DN_VCENTER );

		}
		break;
	}

	// 绘画光电
	m_TextureLight.SetEffectType( dex::TextureEffect_Shine );
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLight, m_ArrayFlash );

	// 绘画金币
	DrawGold( pD3DDevice, m_ArrayGold[1] );
}

// 绘画分数
void CWindowWin::DrawScore( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画分数
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber5, NULL, CPoint(0, 6) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber4, &m_TextureComma4, CPoint(0, 6) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber3, &m_TextureComma3, CPoint(0, 0) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber2, &m_TextureComma2, CPoint(0, 0) );
	DrawScore(pD3DDevice, m_ArrayNumberWin, &m_TextureNumber1, &m_TextureComma1, CPoint(0, 0) );
}

// 绘画分数
void CWindowWin::DrawScore( dex::CD3DDevice * pD3DDevice, CWHArray< tagWinNumberInfo > & ArrayNumberInfo, dex::CD3DTexture * pImageNumber, dex::CD3DTexture * pImageComma, CPoint PointOffset )
{
	for( int nIndex = 0; nIndex < ArrayNumberInfo.GetCount(); ++nIndex )
	{
		// 获取信息
		tagWinNumberInfo & TWinNumberInfo = ArrayNumberInfo.GetAt( nIndex );

		// 判断信息
		int nImageIndex = (TWinNumberInfo.nNumberIndex != -1) ? TWinNumberInfo.nNumberIndex : 0;
		float fNumberSize = TWinNumberInfo.fNumberSize;
		dex::CD3DTexture * pImageInfo = (TWinNumberInfo.nNumberIndex != -1) ? pImageNumber : pImageComma;

		// 无效图片
		if ( pImageInfo == NULL )
			continue;

		// 计算大小
		CSize SizeImage( pImageInfo->GetWidth() / TWinNumberInfo.nImageFrame, pImageInfo->GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * fNumberSize, (float)SizeImage.cy * fNumberSize );

		// 绘画图片
		pImageInfo->DrawTexture(pD3DDevice, (int)(TWinNumberInfo.PointNumber.x - SizeDest.fWidth / 2.f) + PointOffset.x, (int)(TWinNumberInfo.PointNumber.y - SizeDest.fHeight / 2.f) + PointOffset.y, (int)SizeDest.fWidth, (int)SizeDest.fHeight, nImageIndex * SizeImage.cx, 0, SizeImage.cx, SizeImage.cy );
	}
}

// 绘画金币
void CWindowWin::DrawGold( dex::CD3DDevice * pD3DDevice, CWHArray< CGoldMove * > & ArrayGold )
{
	// 绘画金币
	for( int nGoldIndex = 0; nGoldIndex < ArrayGold.GetCount(); ++nGoldIndex )
	{
		// 获取信息
		CGoldMove *& pGoldMove = ArrayGold.GetAt( nGoldIndex );

		// 金币太小
		if( pGoldMove->fAlpha < 0.01 || pGoldMove->fSize < 0.01 )
			continue;

		// 图片定义
		dex::CD3DTexture * pTextureGold = &m_TextureGold[ pGoldMove->cbFrameIndex%TEXTRUE_GOLD_MAX ];

		// 图片大小
		CSize SizeImage( pTextureGold->GetWidth(), pTextureGold->GetHeight() );
		dex::CD3DSize SizeDest( (float)SizeImage.cx * pGoldMove->fSize, (float)SizeImage.cy * pGoldMove->fSize );

		// 绘画图片
		pTextureGold->DrawTexture(pD3DDevice, (int)(pGoldMove->ptPosition.x - SizeDest.fWidth / 2.f), (int)(pGoldMove->ptPosition.y - SizeDest.fHeight / 2.f), (int)SizeDest.fWidth, (int)SizeDest.fHeight, 0, 0, SizeImage.cx, SizeImage.cy, (byte)(pGoldMove->fAlpha * 255.f) );
	} 
}


// 添加金币
void CWindowWin::AppendGold( bool bHeldDrop )
{
	m_nGoldCount = 0;
	m_bHeldDrop = bHeldDrop;
}

// 设置任务目标
void CWindowWin::SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskTarget, nTaskTarget, sizeof(m_nTaskTarget) );
}

// 设置金币
void CWindowWin::SetWinScore( int nMultipleIndex, longlong lWinScore, longlong lTaskScore, bool bTaskComplete[TASK_INDEX_MAX] )
{
	// 背景动画
	m_nBackIndex = 0;
	m_nBackTime = 60;
	m_nLCBackTime.Initialization();

	// 删除数字
	m_ArrayPointFlash.RemoveAll();
	m_ArrayNumberWin.RemoveAll();

	// 设置信息
	m_nMultipleIndex = nMultipleIndex;
	CopyMemory( m_bTaskComplete, bTaskComplete, sizeof(m_bTaskComplete) );

	// 判断奖励
	if( m_nMultipleIndex < MultipleThree_Max )
	{
		// 三连中奖
		m_nMultipleType = MultipleType_Three;
	}
	else if ( m_nMultipleIndex < MultipleThree_Max + MultipleNine_Max )
	{
		// 九连中奖
		m_nMultipleType = MultipleType_Nine;
	
		// 九连设置
		m_nNineLogoIndex = (m_nNineLogoIndex + 1) % TEXTRUE_NINE_LOGO;
	}

	// 删除闪光
	DeleteArray(m_ArrayFlash);

	// 获取窗口
	CRect RectWindow;
	GetWindowRect( RectWindow );

	// 计算位置
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

	// 生成分数
	m_lWinScore = lWinScore;
	GenerateNumber(m_ArrayNumberWin, PointWinScore, lWinScore, 1.f );

	// 任务分数
	m_lTaskScore = lTaskScore;

	// 闪光信息
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 - 182, RectWindow.Height() / 2 - 35) );
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 + 21, RectWindow.Height() / 2 - 93) );
	m_ArrayPointFlash.Add( CDoublePoint(RectWindow.Width() / 2 + 195, RectWindow.Height() / 2 - 74) );

	// 添加光电
	m_nLCFlashTime.Initialization();

	// 位置索引
	int nPointIndex[3];
	nPointIndex[0] = rand() % (int)m_ArrayPointFlash.GetCount();
	nPointIndex[1] = (nPointIndex[0] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();
	nPointIndex[2] = (nPointIndex[1] + rand() % 2 + 1) % (int)m_ArrayPointFlash.GetCount();

	// 添加光电
	for ( int nCountIndex = 0; nCountIndex < rand()%2 + 2; ++nCountIndex )
	{
		m_ArrayFlash.Add( new CLightMove(m_ArrayPointFlash[nPointIndex[nCountIndex]], rand() % 500 + 3000, 0.8f, 0.8f ));
	}

}

// 生成数字
void CWindowWin::GenerateNumber( CWHArray< tagWinNumberInfo > & ArrayNumberInfo, CPoint PointCenter, longlong lNumber, float fSize )
{
	// 数字信息
	tchar szNumber[64] = { _T("") };
	tchar szOutNumber[64] = { _T("") };
	tchar szImageNumber[64] = { _T("0123456789") };
	_sntprintf( szNumber, CountArray(szNumber), TEXT("%I64d"), lNumber );
	_sntprintf( szOutNumber, CountArray(szOutNumber), TEXT("%s"), CGlobal::_Object()->AppendComma( lNumber ) );

	// 数字长度
	int nNumberLen = lstrlen(szNumber);
	int nOutNumberLen = lstrlen(szOutNumber);
	int nImageNumberLen = lstrlen(szImageNumber);

	// 数字宽度
	int nNumberWidth = (int)((float)(m_TextureNumber1.GetWidth() / nImageNumberLen - 5) * fSize);
	int nCommaWidth = (int)((float)(m_TextureComma1.GetWidth() - 25) * fSize);

	// 绘画位置
	int nDrawLen = nNumberWidth * nNumberLen + (nOutNumberLen - nNumberLen) * nCommaWidth;
	CPoint PointNumber(PointCenter.x - nDrawLen / 2 + (int)((float)(m_TextureNumber1.GetWidth() / nImageNumberLen) * fSize) / 2, PointCenter.y );

	// 计算数字
	for ( int nOutIndex = 0; nOutIndex < nOutNumberLen; ++nOutIndex )
	{
		// 添加光电
		if ( nOutIndex == 0 )
		{
			m_ArrayPointFlash.Add( CDoublePoint(PointNumber.x, PointNumber.y + 28) );
		}
		else if( nOutIndex == nOutNumberLen - 1 )
		{
			m_ArrayPointFlash.Add( CDoublePoint(PointNumber.x + 22, PointNumber.y - 3) );
		}

		// 计算数字
		for ( int nImageIndex = 0; nImageIndex < nImageNumberLen; ++nImageIndex )
		{
			// 数字绘画
			if ( szOutNumber[nOutIndex] == szImageNumber[nImageIndex] && szOutNumber[nOutIndex] != '\0' )
			{
				// 添加信息
				tagWinNumberInfo TWinNumberInfo;
				TWinNumberInfo.nNumberIndex = nImageIndex;
				TWinNumberInfo.nImageFrame = nImageNumberLen;
				TWinNumberInfo.fNumberSize = fSize;
				TWinNumberInfo.PointNumber.SetPoint( PointNumber.x, PointNumber.y );
				ArrayNumberInfo.Add( TWinNumberInfo );

				// 偏移位置
				PointNumber.Offset( nNumberWidth, 0 );

				break;
			}
			// 逗号绘画
			else if ( szOutNumber[nOutIndex] == ',' )
			{
				// 添加信息
				tagWinNumberInfo TWinNumberInfo;
				TWinNumberInfo.nNumberIndex = -1;
				TWinNumberInfo.nImageFrame = 1;
				TWinNumberInfo.fNumberSize = fSize;
				TWinNumberInfo.PointNumber.SetPoint( PointNumber.x - 8, PointNumber.y + 25);
				ArrayNumberInfo.Add( TWinNumberInfo );

				// 偏移位置
				PointNumber.Offset( nCommaWidth, 0 );

				break;
			}
		}
	}
}





