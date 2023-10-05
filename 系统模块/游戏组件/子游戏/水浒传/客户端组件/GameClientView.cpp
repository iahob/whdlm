#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
////////////////////////////////////////////////////////////////////////////////// 

// 信息定义
#define CAPTION_TOP_SIZE			25							// 标题大小

void CALLBACK TimerCallProc(UINT uTimerID, UINT msg, DWORD dwData, DWORD dwa, DWORD dwb)
{
	CGameClientView *pGameClientView = (CGameClientView *)dwData;
	pGameClientView->CartoonMovie();
}

// 按钮定义
enum
{
	ButtonID = 100,
	ButtonID_WinClose,											// 关闭
	ButtonID_WinMax,											// 最大化
	ButtonID_WinMin,											// 最小化
	ButtonID_Sound,												// 声音
	ButtonID_WinSet,											// 设置
	ButtonID_Admin,												// 控制
	ButtonID_AddBet,											// 加注
	ButtonID_SubBet,											// 减注
};							

// 消息定义
BEGIN_WHDUI_MESSAGE_MAP(CGameClientView, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_KEYDOWN()
	ON_WHDUI_WINDOW_UPDATA_TEXTURE_BUFFER()
	ON_WHDUI_MOUSEMOVE()
	ON_WHDUI_LBUTTONDOWN()
	ON_WHDUI_LBUTTONUP()
	ON_WHDUI_WINDOW_SIZE()
	ON_WHDUI_WINDOW_SHOW()
	ON_CONRTOL_BN_CLICKED(ButtonID_WinClose, OnButtonWinClose)
	ON_CONRTOL_BN_CLICKED(ButtonID_WinMin, OnButtonWinMin)
	ON_CONRTOL_BN_CLICKED(ButtonID_Sound, OnButtonSound)
	ON_CONRTOL_BN_CLICKED(ButtonID_WinSet, OnButtonWinSet)
	ON_CONRTOL_BN_CLICKED(ButtonID_Admin,OnButtonAdmin)
	ON_CONRTOL_BN_CLICKED(ButtonID_AddBet,OnButtonAddBet)
	ON_CONRTOL_BN_CLICKED(ButtonID_SubBet,OnButtonSubBet)
	
END_WHDUI_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////// 
// 构造函数
CGameClientView::CGameClientView()
{
	m_uAccuracy = 0;
	m_uTimerID = 0;
	m_bDrawTop = false;
	m_bDrawFrame = false;

	m_cbGameStatus = GAME_SCENE_FREE;
	m_bAutoGame = false;
	m_cbShuoMingLoadPos = 0;
	m_cbShuoMingIndex = 0;
	m_bShowShuoMing = true;
	m_lOneGredit = 0;
	m_lOneTotalWin = 0;
	m_lOneScoreTemp = 0;
	m_cbWaitMode = 0;
	m_lLine = 9;
	m_lBet = 1;
	m_lOneTotalBet = m_lBet ;
	m_lSaveScore = 0;

	m_bInitImage = false;
	m_cbImageLoadPos = 0;


	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			m_cbItemInfo[i][j] = CT_SHUIHUZHUAN;
			m_bZhongJiang[i][j] = false;
		}
	}
	for (int j = 0; j < ITEM_X_COUNT; j++)
	{
		m_ptZhongJiang[j].SetPoint(0xFF, 0xFF);
	}
	m_cbZhongJiangXian = 0xFF;
	m_cbPlayIndex = ONE_NULL;

	//比倍变量
	m_cbUser[0] = 0xFF;
	m_cbUser[1] = 0xFF;
	m_cbUser[2] = 0xFF;
	m_lTwoGredit = 0;
	m_lTwoTotalWin = 0;
	m_lTwoTotalBet = 0;
	m_lTwoGameScore = 0;
	m_cbBetIndex = 0xFF;
	m_cbSice[0] = 0xFF;	
	m_cbSice[1] = 0xFF;	
	m_bShowSice[0] = false;
	m_bShowSice[1] = false;


	m_lThreeGredit = 0;
	m_lThreeTotalWin= 0;
	m_lThreeTotalBet= 0;
	m_lThreeGameScore= 0;
	m_lThreeTimes = 0;

	m_cbItemType[0] = CT_LU;
	m_cbItemType[1] = CT_YINGQIANG;
	m_cbItemType[2] = CT_ZHONGYITANG;
	m_cbItemType[3] = CT_FUTOU;
	m_cbItemType[4] = CT_DADAO;
	m_cbItemType[5] = CT_SHUIHUZHUAN;
	m_cbItemType[6] = CT_SONG;
	m_cbItemType[7] = CT_LU;
	m_cbItemType[8] = CT_YINGQIANG;
	m_cbItemType[9] = CT_FUTOU;
	m_cbItemType[10] = CT_TITIANXINGDAO;
	m_cbItemType[11] = CT_SHUIHUZHUAN;
	m_cbItemType[12] = CT_LIN;
	m_cbItemType[13] = CT_DADAO;
	m_cbItemType[14] = CT_FUTOU;
	m_cbItemType[15] = CT_LU;
	m_cbItemType[16] = CT_SONG;
	m_cbItemType[17] = CT_SHUIHUZHUAN;
	m_cbItemType[18] = CT_LIN;
	m_cbItemType[19] = CT_YINGQIANG;
	m_cbItemType[20] = CT_FUTOU;
	m_cbItemType[21] = CT_DADAO;
	m_cbItemType[22] = CT_TITIANXINGDAO;
	m_cbItemType[23] = CT_SHUIHUZHUAN;

	m_cbThreeItemInfo[0] = CT_SHUIHUZHUAN;
	m_cbThreeItemInfo[1] = CT_SHUIHUZHUAN;
	m_cbThreeItemInfo[2] = CT_SHUIHUZHUAN;
	m_cbThreeItemInfo[3] = CT_SHUIHUZHUAN;

	m_bThreeZhongJiang = false;
	m_cbThreeIndex = CT_SHUIHUZHUAN;
	m_cbThreeZhongtimes = 0xFF;

	m_pClientUserItem = NULL;
	m_pGameClientEngine = NULL;
	m_pMyUIThread = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = NULL;
	m_bShowClientControlDlg = false;
}

// 析构函数
CGameClientView::~CGameClientView()
{
	if(m_uTimerID > 0)
	{
		timeKillEvent(m_uTimerID);
		m_uTimerID = 0;
	}

	if (m_uAccuracy > 0)
	{
		timeEndPeriod(m_uAccuracy);
		m_uAccuracy = 0;
	}

	if(m_pMyUIThread != NULL)
	{
		TerminateThread(m_pMyUIThread->m_hThread, 0);
		m_pMyUIThread = NULL;
	}

	if( m_pClientControlDlg != NULL)
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hInst != NULL)
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

// 重置界面
void CGameClientView::ResetGameView()
{
}

//调整控件
void CGameClientView::RectifyControl(uint nWidth, uint nHeight)
{
	//移动控件
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//顶层按钮
	m_ButtonWinClose.SetWindowPos((Screen_Width - 10-28*1) * nWidth / Screen_Width, (5) * nHeight / Screen_Height, (125/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);
	//m_ButtonWinMax.SetWindowPos((nWidth-5-45*2) * nWidth / Screen_Width, (6) * nHeight / Screen_Height, (125/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);
	m_ButtonWinMin.SetWindowPos((Screen_Width - 10-28*2) * nWidth / Screen_Width, (5) * nHeight / Screen_Height, (125/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);
	m_ButtonSound.SetWindowPos((Screen_Width - 10-28*3) * nWidth / Screen_Width, (5) * nHeight / Screen_Height, (125/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);
	m_ButtonWinSet.SetWindowPos((Screen_Width - 10-28*4) * nWidth / Screen_Width, (5) * nHeight / Screen_Height, (125/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);
	m_ButtonAdmin.SetWindowPos((Screen_Width - 10-28*6 - 10) * nWidth / Screen_Width, (3) * nHeight / Screen_Height, (285/5) * nWidth / Screen_Width, (25) * nHeight / Screen_Height, uFlags);

	m_ButtonSubBet.SetWindowPos((Screen_Width - 470) * nWidth / Screen_Width, (Screen_Height - 40) * nHeight / Screen_Height, (130/5) * nWidth / Screen_Width, (30) * nHeight / Screen_Height, uFlags);
	m_ButtonAddBet.SetWindowPos((Screen_Width - 318) * nWidth / Screen_Width, (Screen_Height - 40) * nHeight / Screen_Height, (130/5) * nWidth / Screen_Width, (30) * nHeight / Screen_Height, uFlags);

	m_bDrawFrame = true;
	int nFrame = 0;

	CD3DTextureEx::SetShowWindow(nWidth, nHeight);

	if(m_bDrawFrame)
	{
		nWidth = Screen_Width;
		nHeight = Screen_Height;
		nFrame = 30;
	}
	else
	{
		nWidth = 1280;
		nHeight = 768;
	}

	m_AnimationDaGu.SetBenchmarkPos(0, 0 + nFrame, enXLeft, enYTop);
	//m_AnimationLong.SetBenchmarkPos(nWidth / 2, nHeight / 2);
	m_AnimationZhongJiangXian.SetBenchmarkPos(0, 0 + nFrame, enXLeft, enYTop);
	m_AnimationWaitQi.SetBenchmarkPos(nWidth, 0 + nFrame, enXRight, enYTop);
	m_AnimationKaiQi.SetBenchmarkPos(nWidth, 0 + nFrame, enXRight, enYTop);
	m_AnimationShuiHuZhuang.SetBenchmarkPos(nWidth / 2 - 5, 1 + nFrame, enXCenter, enYTop);
	m_AnimationOneTip.SetBenchmarkPos(nWidth / 2, 154 + nFrame, enXCenter, enYCenter);

	int nPosX = 100;
	int nPosY = 179 + nFrame;
	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		nPosX = 101;
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			m_ptItemImage[i][j].SetPoint(nPosX + j * 220, nPosY + 165 * i);
			m_ptBianKuang[i][j].SetPoint(nPosX + j * 220, nPosY + 1 + 164 * i);
		}
	}

	m_AnimationHeGuan[0].SetBenchmarkPos(nWidth / 2, nHeight / 2 - 94 + nFrame / 2);
	m_AnimationHeGuan[1].SetBenchmarkPos(nWidth / 2, nHeight / 2 - 92 + nFrame / 2);
	m_AnimationHeGuan[2].SetBenchmarkPos(nWidth / 2, nHeight / 2 - 101 + nFrame / 2);
	m_AnimationHeGuan[3].SetBenchmarkPos(nWidth / 2, nHeight / 2 - 111 + nFrame / 2);
	m_AnimationHeGuan[4].SetBenchmarkPos(nWidth / 2 + 5, nHeight / 2 - 102 + nFrame / 2);
	m_AnimationHeGuan[5].SetBenchmarkPos(nWidth / 2, nHeight / 2 - 84 + nFrame / 2);

	m_AnimationZuoHeGuan[0].SetBenchmarkPos(0, 165 + nFrame, enXLeft, enYTop);
	m_AnimationZuoHeGuan[1].SetBenchmarkPos(0, 160 + nFrame, enXLeft, enYTop);
	m_AnimationZuoHeGuan[2].SetBenchmarkPos(0, 175 + nFrame, enXLeft, enYTop);
	m_AnimationZuoHeGuan[3].SetBenchmarkPos(0, 155 + nFrame, enXLeft, enYTop);

	m_AnimationYouHeGuan[0].SetBenchmarkPos(nWidth, 175 + nFrame, enXRight, enYTop);
	m_AnimationYouHeGuan[1].SetBenchmarkPos(nWidth, 192 + nFrame, enXRight, enYTop);
	m_AnimationYouHeGuan[2].SetBenchmarkPos(nWidth, 175 + nFrame, enXRight, enYTop);
	m_AnimationYouHeGuan[3].SetBenchmarkPos(nWidth, 202 + nFrame, enXRight, enYTop);

	m_AnimationDice[0].SetBenchmarkPos(nWidth / 2, 462 + nFrame, enXCenter, enYTop);
	m_AnimationDice[1].SetBenchmarkPos(nWidth / 2, 540 + nFrame, enXCenter, enYTop);
	m_AnimationTwoTip.SetBenchmarkPos(nWidth / 2, 500 + nFrame, enXCenter, enYCenter);

	m_AnimationZuoZhiDouDong.SetBenchmarkPos(0, nHeight - 92, enXLeft, enYBottom);
	m_AnimationJingYuanBaoDouDong.SetBenchmarkPos(nWidth / 2 - 48, nHeight - 230, enXCenter, enYBottom);
	m_AnimationTwoSanGuang.SetBenchmarkPos(nWidth / 2 + 2, nHeight - 12, enXCenter, enYBottom);

	m_ptGredit.SetPoint(200, 730 + nFrame);
	m_ptTotalWin.SetPoint(520, 730 + nFrame);
	m_ptLine.SetPoint(754, 730 + nFrame);
	m_ptBet.SetPoint(900, 730 + nFrame);
	m_ptTotalBet.SetPoint(1100, 730 + nFrame);


	m_ptTwoGredit.SetPoint(220, 725 + nFrame);
	m_ptTwoTotalWin.SetPoint(640, 725 + nFrame);
	m_ptTwoTotalBet.SetPoint(1030, 725 + nFrame);

	m_ptTwoJingYuanBao[0].SetPoint(220, 575 + nFrame);
	m_ptTwoJingYuanBao[1].SetPoint(580, 575 + nFrame);
	m_ptTwoJingYuanBao[2].SetPoint(940, 575 + nFrame);

	m_ptDice0[0].SetPoint(nWidth / 2 - 12, 460 + nFrame);
	m_ptDice0[1].SetPoint(nWidth / 2 + 42, 460 + nFrame);

	m_ptDice1[0].SetPoint(nWidth / 2 - 40, 522 + nFrame);
	m_ptDice1[1].SetPoint(nWidth / 2 + 70, 522 + nFrame);


	m_ptThreeGredit.SetPoint(422, 607 + nFrame);
	m_ptThreeTotalWin.SetPoint(745, 607 + nFrame);
	m_ptThreeTotalBet.SetPoint(995, 607 + nFrame);
	m_ptThreeTotalTimes.SetPoint(685, 342 + nFrame);

	m_ptThreeItem[0].SetPoint(201, 25 + nFrame);
	m_ptThreeItem[1].SetPoint(381, 27 + nFrame);
	m_ptThreeItem[2].SetPoint(560, 9 + nFrame);
	m_ptThreeItem[3].SetPoint(742, 27 + nFrame);
	m_ptThreeItem[4].SetPoint(920, 26 + nFrame);
	m_ptThreeItem[5].SetPoint(1099, 27 + nFrame);

	m_ptThreeItem[6].SetPoint(1102, 133 + nFrame);
	m_ptThreeItem[7].SetPoint(1102, 240 + nFrame);
	m_ptThreeItem[8].SetPoint(1099, 348 + nFrame);
	m_ptThreeItem[9].SetPoint(1103, 453 + nFrame);
	m_ptThreeItem[10].SetPoint(1099, 562 + nFrame);
	m_ptThreeItem[11].SetPoint(1102, 667 + nFrame);

	m_ptThreeItem[12].SetPoint(921, 668 + nFrame);
	m_ptThreeItem[13].SetPoint(741, 667 + nFrame);
	m_ptThreeItem[14].SetPoint(562, 667 + nFrame);
	m_ptThreeItem[15].SetPoint(379, 667 + nFrame);
	m_ptThreeItem[16].SetPoint(203, 668 + nFrame);
	m_ptThreeItem[17].SetPoint(19, 667 + nFrame);

	m_ptThreeItem[18].SetPoint(20, 560 + nFrame);
	m_ptThreeItem[19].SetPoint(19, 453 + nFrame);
	m_ptThreeItem[20].SetPoint(21, 346 + nFrame);
	m_ptThreeItem[21].SetPoint(20, 240 + nFrame);
	m_ptThreeItem[22].SetPoint(19, 133 + nFrame);
	m_ptThreeItem[23].SetPoint(19, 26 + nFrame);

	m_ptThreeItemImage[0].SetPoint(220, 444 + nFrame);
	m_ptThreeItemImage[1].SetPoint(435, 444 + nFrame);
	m_ptThreeItemImage[2].SetPoint(650, 444 + nFrame);
	m_ptThreeItemImage[3].SetPoint(860, 444 + nFrame);

	m_ptThreeItemTimes[0].SetPoint(200, 224 + nFrame);
	m_ptThreeItemTimes[1].SetPoint(310, 224 + nFrame);
	m_ptThreeItemTimes[2].SetPoint(420, 224 + nFrame);
	m_ptThreeItemTimes[3].SetPoint(530, 224 + nFrame);
	m_ptThreeItemTimes[4].SetPoint(640, 224 + nFrame);
	m_ptThreeItemTimes[5].SetPoint(750, 224 + nFrame);
	m_ptThreeItemTimes[6].SetPoint(860, 224 + nFrame);
	m_ptThreeItemTimes[7].SetPoint(970, 224 + nFrame);

	m_ptThreeTimes[0].SetPoint(226, 392 + nFrame);
	m_ptThreeTimes[1].SetPoint(510, 392 + nFrame);
	m_ptThreeTimes[2].SetPoint(796, 390 + nFrame);

}


// 银行消息
bool CGameClientView::OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize)
{
	return true;
}


// 配置创建
void CGameClientView::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	//设定随机种子
	srand(GetTickCount());
	
	m_pD3DDevice = pD3DDevice;

	//边框资源
	m_ImageGameFrame[0].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_TL"));
	m_ImageGameFrame[1].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_TM"));
	m_ImageGameFrame[2].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_TR"));
	m_ImageGameFrame[3].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_RM"));
	m_ImageGameFrame[4].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_LM"));
	m_ImageGameFrame[5].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_BM"));
	m_ImageGameFrame[6].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_BL"));
	m_ImageGameFrame[7].LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("FRAME_BR"));

	m_TextureShuoMingPosBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("说明界面-0"), _T("资源"), _T("progress_bar_bg"));
	m_TextureShuoMingPos.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("说明界面-0"), _T("资源"), _T("progress_bar"));
	m_TextureExitShuoMing.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("说明界面-0"), _T("资源"), _T("退出帮助"));
	m_AnimationShuoMing.InitAnimation(pD3DDevice, TEXT("说明界面"), TEXT(""), 5,  2);
	m_AnimationShuoMing.ShowAnimation(true, 1000, enCycle);

	
	// 顶部按钮
	m_ButtonWinClose.ActiveWindow( NULL,CRect(CPoint(0, 0),CSize(0, 0) ),NULL,ButtonID_WinClose, this );
	//m_ButtonWinMax.ActiveWindow( NULL,CRect(CPoint( 0, 0 ),CSize(0, 0)),NULL, ButtonID_WinMax, this );
	m_ButtonWinMin.ActiveWindow( NULL,CRect(CPoint( 0, 0 ),CSize(0, 0)), NULL, ButtonID_WinMin, this );
	m_ButtonSound.ActiveWindow( NULL,CRect(CPoint( 0, 0 ),CSize(0, 0)), NULL, ButtonID_Sound, this );
	m_ButtonWinSet.ActiveWindow( NULL,CRect(CPoint( 0, 0 ),CSize(0, 0)), NULL, ButtonID_WinSet, this );
	m_ButtonAdmin.ActiveWindow(NULL, CRect( CPoint(0,0), CSize(0, 0) ), NULL, ButtonID_Admin, this);

	m_ButtonAddBet.ActiveWindow(NULL, CRect( CPoint(0,0), CSize(0, 0) ), NULL, ButtonID_AddBet, this);
	m_ButtonSubBet.ActiveWindow(NULL, CRect( CPoint(0,0), CSize(0, 0) ), NULL, ButtonID_SubBet, this);

	m_ButtonWinClose.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), _T("BT_CLOSE"));
	//m_ButtonWinMax.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), _T("BT_MAX"));
	m_ButtonWinMin.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), _T("BT_MIN"));
	CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
	m_ButtonSound.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), !pGlabalUnits->m_bMuteStatuts ? _T("BT_SOUND_OPEN") : _T("BT_SOUND_CLOSE"));
	m_ButtonWinSet.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), _T("BT_OPTION"));
	m_ButtonAdmin.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("共用"), _T("资源"), _T("BT_ADMIN"));

	m_ButtonAddBet.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("第一部分"), _T("资源"), _T("BT_ADD_BET"));
	m_ButtonSubBet.SetButtonTexture( pD3DDevice,CGlobal::_Object()->hModuleResource,_T("第一部分"), _T("资源"), _T("BT_SUB_BET"));

	m_D3DFont.CreateFont( 12, 400, TEXT("宋体"));

	RectifyControl(nWidth,nHeight);

	m_pMyUIThread = (CMyUIThread *)AfxBeginThread(RUNTIME_CLASS(CMyUIThread));
	m_pMyUIThread->SetGameClientView(this);
	ThreadInfo();

	//控制
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("WaterMarginClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(m_pGameClientEngine));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}

	TIMECAPS tc;
	m_uAccuracy = 0;
	m_uTimerID = 0;
	//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续 
	if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR) 	
	{ 
		// 分辨率的值不能超出系统的取值范围  
		m_uAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
		//调用timeBeginPeriod函数设置定时器的分辨率 
		timeBeginPeriod(m_uAccuracy); 
	}

	m_uTimerID = timeSetEvent(1, m_uAccuracy,(LPTIMECALLBACK) TimerCallProc, (DWORD)this, TIME_PERIODIC);
}



//动画驱动
VOID CGameClientView::CartoonMovie()
{
	if (m_cbGameStatus == GAME_SCENE_ONE || m_cbGameStatus == GAME_SCENE_FREE)
	{
		if (m_cbPlayIndex == ONE_SHUO_MING )
		{
			
		}
		else
		{
			if (m_cbPlayIndex == ONE_KAIJIANG)
			{
				//动画序列
				for (int i = 0; i < ITEM_COUNT; i++)
				{
					m_AnimationXuLieImage[i].CartoonMovie();
				}	

				//边框动画
				m_AnimationBianKuang.CartoonMovie();

			}
			else if (m_cbPlayIndex == ONE_GUANDONG)
			{
				//滚动序列
				for (int i = 0; i < ITEM_Y_COUNT; i++)
				{
					for (int j = 0; j < ITEM_X_COUNT; j++)
					{
						m_Quicken[i * ITEM_X_COUNT + j].CartoonMovie();
					}
				}
			}
			else if (m_cbPlayIndex == ONE_ZHONGJIANG)
			{
				//闪光动画
				m_AnimationSanGuang.CartoonMovie();
				//边框动画
				m_AnimationBianKuang.CartoonMovie();
			}

			//打鼓
			m_AnimationDaGu.CartoonMovie();

			//顶部水浒传3个字体闪光
			m_AnimationShuiHuZhuang.CartoonMovie();

			//等待开始状态下的飘旗
			m_AnimationWaitQi.CartoonMovie();

			//滚动状态下的摇旗
			m_AnimationKaiQi.CartoonMovie();			
		}
	}	
	else if (m_cbGameStatus == GAME_SCENE_TWO)
	{
		for (int i = 0; i < 6; i++)
		{
			m_AnimationHeGuan[i].CartoonMovie();

			if (i < 4)
			{
				m_AnimationZuoHeGuan[i].CartoonMovie();
				m_AnimationYouHeGuan[i].CartoonMovie();
			}
		}

		m_AnimationZuoZhiDouDong.CartoonMovie();
		m_AnimationJingYuanBaoDouDong.CartoonMovie();
		m_AnimationTwoSanGuang.CartoonMovie();
	}

	else if (m_cbGameStatus == GAME_SCENE_THREE)
	{
		if (m_cbPlayIndex == THREE_GUANDONG)
		{
			for (int i = 0; i < 4; i++)
			{
				m_QuickenThreeItem[i].CartoonMovie();
			}
		}
		else if (m_cbPlayIndex == THREE_KAIJIANG)
		{				
			m_QuickenThree.CartoonMovie();
		}
		else if (m_cbPlayIndex == THREE_KAIJIANG_END)
		{				
			m_QuickenSan.CartoonMovie();
		}
		m_QuickenViewSan.CartoonMovie();
	}

	return;
}

// 动画驱动
void CGameClientView::OnWindowMovie()
{	
}

// 绘画界面
void CGameClientView::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	int nDesWidth = nWidth;
	int nDesHeight = nHeight;

	if(m_bDrawFrame)
	{
		nWidth = 1280;
		nHeight = 798;
	}
	else
	{
		nWidth = 1280;
		nHeight = 768;
	}

	int nFrame = m_bDrawFrame ? 30 : 0;
	if(m_bShowShuoMing)
	{
		//静态图标
		m_AnimationShuoMing.DrawAnimation(pD3DDevice, 0, nFrame, m_cbShuoMingIndex, enXLeft, enYTop);
		if (!m_bInitImage || m_cbShuoMingLoadPos < 100)
		{
			m_TextureShuoMingPosBack.DrawTexture(pD3DDevice, (nWidth - m_TextureShuoMingPosBack.GetWidth()) / 2, nHeight - m_TextureShuoMingPosBack.GetHeight() - 5);
			m_TextureShuoMingPos.DrawTexture(pD3DDevice,(nWidth - m_TextureShuoMingPosBack.GetWidth()) / 2, nHeight - m_TextureShuoMingPosBack.GetHeight() - 5,
				m_TextureShuoMingPos.GetWidth() * m_cbShuoMingLoadPos / 100, m_TextureShuoMingPos.GetHeight(),0, 0,m_TextureShuoMingPos.GetWidth() * m_cbShuoMingLoadPos / 100, m_TextureShuoMingPos.GetHeight());
		}
		if(m_bInitImage)
		{
			m_TextureExitShuoMing.DrawTexture(pD3DDevice, 5, 5 + nFrame);
		}
	}
	else
	{
		if (m_cbGameStatus == GAME_SCENE_ONE || m_cbGameStatus == GAME_SCENE_FREE)
		{
			if (m_cbPlayIndex == ONE_SHUO_MING && m_cbGameStatus == GAME_SCENE_FREE)
			{
				//静态图标
				m_AnimationShuoMing.DrawAnimation(pD3DDevice, 0, 0, m_cbShuoMingIndex, enXLeft, enYTop);
				if (!m_bInitImage || m_cbShuoMingLoadPos < 100)
				{
					m_TextureShuoMingPosBack.DrawTexture(pD3DDevice, (nWidth - m_TextureShuoMingPosBack.GetWidth()) / 2, nHeight - m_TextureShuoMingPosBack.GetHeight() - 5);
					m_TextureShuoMingPos.DrawTexture(pD3DDevice,(nWidth - m_TextureShuoMingPosBack.GetWidth()) / 2, nHeight - m_TextureShuoMingPosBack.GetHeight() - 5,
						m_TextureShuoMingPos.GetWidth() * m_cbShuoMingLoadPos / 100, m_TextureShuoMingPos.GetHeight(),0, 0,m_TextureShuoMingPos.GetWidth() * m_cbShuoMingLoadPos / 100, m_TextureShuoMingPos.GetHeight());
				}
			}
			else
			{
				//开奖动画
				for (int i = 0; i < ITEM_Y_COUNT; i++)
				{
					for (int j = 0; j < ITEM_X_COUNT; j++)
					{
						if(m_cbPlayIndex == ONE_NULL)
						{
							//静态图标
							m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, m_cbItemInfo[i][j], enXLeft, enYTop);
						}
						else if (m_cbPlayIndex == ONE_GUANDONG)
						{
							//滚动动画
							LONGLONG lCurIndex = m_Quicken[i * ITEM_X_COUNT + j].GetCurIndex();
							LONGLONG lMaxIndex = m_Quicken[i * ITEM_X_COUNT + j].GetMaxIndex();
							if (lCurIndex + 50 * 5 >= lMaxIndex)
							{
								//抖动动画
								m_AnimationDouDongImage[m_cbItemInfo[i][j]].DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, 
									(50 * 5 - (lMaxIndex - lCurIndex + 1) ) / 50, enXLeft, enYTop);	
							}
							else
							{
								m_AnimationScrollGunDong.DrawAnimationScrollPos(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, 
									lCurIndex % 1410, enXLeft, enYTop);
							}
						}
						else if (m_cbPlayIndex == ONE_ZHONGJIANG)
						{
							//中奖线
							if (m_cbZhongJiangXian != 0xFF)
							{
								//闪光动画
								if (m_ptZhongJiang[j].x == i && m_ptZhongJiang[j].y == j)
								{
									//静态图标
									m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, m_cbItemInfo[i][j] + 18, enXLeft, enYTop);
									m_AnimationSanGuang.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, enXLeft, enYTop);
								}
								else
								{
									//静态图标
									m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, m_cbItemInfo[i][j] + 9, enXLeft, enYTop);
								}
							}
						}
						else if (m_cbPlayIndex == ONE_KAIJIANG)
						{
							if (m_bZhongJiang[i][j] && m_AnimationXuLieImage[m_cbItemInfo[i][j]].IsShow())
							{
								//序列动画
								m_AnimationXuLieImage[m_cbItemInfo[i][j]].DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, enXLeft, enYTop);	
							}
							else
							{
								//静态图标
								m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, m_cbItemInfo[i][j], enXLeft, enYTop);
							}

						}
						else if(m_cbPlayIndex == ONE_KAIJIANG_END)
						{
							//静态图标
							m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptItemImage[i][j].x, m_ptItemImage[i][j].y, m_cbItemInfo[i][j], enXLeft, enYTop);
						}
					}
				}

				m_TextureOneBack.DrawTexture(pD3DDevice, 0, 0 + nFrame);

				// 			//提示
				// 			if (!m_strTip.IsEmpty())
				// 			{
				// 				CRect rc(381, 184, 381 + 521, 181 + 36);
				// 				DrawTextString(pD3DDevice, m_strTip, RGB(255,255,255), RGB(0,0,0), rc, DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
				// 			}

				if(m_cbPlayIndex == ONE_NULL)
				{
					if (GetUserClock(0) > 0)
					{
						m_AnimationOneTip.DrawAnimation(pD3DDevice, 1);
						CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureOneTipNumber, TEXT("0123456789"), GetUserClock(0), nWidth / 2 + 12, 145 + nFrame);
					}
					else
					{
						m_AnimationOneTip.DrawAnimation(pD3DDevice, 0);
					}
				}
				else if (m_cbPlayIndex == ONE_GUANDONG)
				{
					m_AnimationOneTip.DrawAnimation(pD3DDevice, 2);
				}
				else if (m_cbPlayIndex == ONE_ZHONGJIANG)
				{
					m_AnimationOneTip.DrawAnimation(pD3DDevice, 3);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureOneTipNumber, TEXT("0123456789"), m_lOneScoreTemp, nWidth / 2 + 10, 146 + nFrame, DT_LEFT);
				}
				else if (m_cbPlayIndex == ONE_KAIJIANG)
				{
					m_AnimationOneTip.DrawAnimation(pD3DDevice, 3);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureOneTipNumber, TEXT("0123456789"), m_lOneScoreTemp, nWidth / 2 + 10, 146 + nFrame, DT_LEFT);
				}
				else if(m_cbPlayIndex == ONE_KAIJIANG_END)
				{
					if (m_cbWaitMode == GAME_SCENE_TWO)
					{
						if (GetUserClock(0) > 0)
						{
							m_AnimationOneTip.DrawAnimation(pD3DDevice, 4);
							CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureOneTipNumber, TEXT("0123456789"), GetUserClock(0), nWidth / 2 + 137, 145 + nFrame);
						}
					}
					else if (m_cbWaitMode == GAME_SCENE_THREE)
					{
						if (GetUserClock(0) > 0)
						{
							m_AnimationOneTip.DrawAnimation(pD3DDevice, 5);
							CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureOneTipNumber, TEXT("0123456789"), GetUserClock(0), nWidth / 2 + 50, 145 + nFrame);
						}
					}
				}

				//顶部水浒传3个字体闪光
				m_AnimationShuiHuZhuang.DrawAnimation(pD3DDevice);

				//打鼓
				m_AnimationDaGu.DrawAnimation(pD3DDevice);

				//中大奖飞龙出画
				//m_AnimationLong.DrawAnimation(pD3DDevice);


				if (m_cbPlayIndex > ONE_NULL && m_cbPlayIndex < ONE_KAIJIANG_END)
				{
					//滚动状态下的摇旗
					m_AnimationKaiQi.DrawAnimation(pD3DDevice);
				}
				else
				{
					//等待开始状态下的飘旗
					m_AnimationWaitQi.DrawAnimation(pD3DDevice);
				}

				//边框动画
				if (m_cbPlayIndex == ONE_KAIJIANG)
				{
					for (int i = 0; i < ITEM_Y_COUNT; i++)
					{
						for (int j = 0; j < ITEM_X_COUNT; j++)
						{
							if (m_bZhongJiang[i][j])
							{
								m_AnimationBianKuang.DrawAnimation(pD3DDevice, m_ptBianKuang[i][j].x - 14, m_ptBianKuang[i][j].y - 12, enXLeft, enYTop);
							}
						}
					}
				}
				else if (m_cbPlayIndex == ONE_ZHONGJIANG)
				{
					for (int i = 0; i < ITEM_Y_COUNT; i++)
					{
						for (int j = 0; j < ITEM_X_COUNT; j++)
						{
							//中奖线
							if (m_cbZhongJiangXian != 0xFF)
							{
								//闪光动画
								if (m_ptZhongJiang[j].x == i && m_ptZhongJiang[j].y == j)
								{
									m_AnimationBianKuang.DrawAnimation(pD3DDevice, m_ptBianKuang[i][j].x - 14, m_ptBianKuang[i][j].y - 12, 0, enXLeft, enYTop);
								}
							}
						}
					}
				}

				//中奖线
				if (m_cbZhongJiangXian != 0xFF)
				{
					m_AnimationZhongJiangXian.DrawAnimation(pD3DDevice, m_cbZhongJiangXian);
				}

				if (m_lOneGredit < 10000000000 && m_lOneTotalWin < 10000000000 && m_lBet < 10000000 && m_lOneTotalBet < 100000000)
				{
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lOneGredit, m_ptGredit.x, m_ptGredit.y);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lOneTotalWin, m_ptTotalWin.x, m_ptTotalWin.y);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lLine, m_ptLine.x, m_ptLine.y);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lBet, m_ptBet.x, m_ptBet.y);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lOneTotalBet, m_ptTotalBet.x, m_ptTotalBet.y);
				}
 				else
 				{
 					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lOneGredit, m_ptGredit.x, m_ptGredit.y + 7);
 					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lOneTotalWin, m_ptTotalWin.x, m_ptTotalWin.y + 7);
 					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lLine, m_ptLine.x, m_ptLine.y + 7);
 					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lBet, m_ptBet.x, m_ptBet.y + 7);
 					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lOneTotalBet, m_ptTotalBet.x, m_ptTotalBet.y + 7);
 				}

				if(m_cbPlayIndex == ONE_KAIJIANG_END && m_lOneGameScore > 0)
				{
					TCHAR szOutNumT[128] = {0};
					_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%I64d"),m_lOneGameScore);
					INT nLeftPos=nWidth / 2 - (m_TextureWinNumber.GetWidth()/lstrlen(TEXT("0123456789")) * lstrlen(szOutNumT)+m_TextureWin.GetWidth()) / 2;

					m_TextureWin.DrawTexture(pD3DDevice, nLeftPos, nHeight / 2 - (m_TextureWin.GetHeight() - m_TextureWinNumber.GetHeight()) / 2);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureWinNumber, TEXT("0123456789"), szOutNumT, nLeftPos + m_TextureWin.GetWidth(), nHeight / 2, DT_LEFT);
				}
			}
		}
		else if (m_cbGameStatus == GAME_SCENE_TWO)
		{
			m_TextureTwoBack.DrawTexture(pD3DDevice, 0, 0 + nFrame);

			m_AnimationZuoZhiDouDong.DrawAnimation(pD3DDevice); 		

			m_AnimationZuoHeGuan[m_cbUser[0]].DrawAnimation(pD3DDevice);
			m_AnimationHeGuan[m_cbUser[1]].DrawAnimation(pD3DDevice);		
			m_AnimationYouHeGuan[m_cbUser[2]].DrawAnimation(pD3DDevice);

			m_AnimationJingYuanBaoDouDong.DrawAnimation(pD3DDevice);
			m_AnimationTwoSanGuang.DrawAnimation(pD3DDevice, nWidth / 2 - 15, nHeight - 58, enXCenter, enYBottom);


			CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lTwoGredit, m_ptTwoGredit.x, m_ptTwoGredit.y);
			CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lTwoTotalWin, m_ptTwoTotalWin.x, m_ptTwoTotalWin.y);
			CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lTwoTotalBet, m_ptTwoTotalBet.x, m_ptTwoTotalBet.y);

			if (m_cbBetIndex < 3)
			{
				if (m_lTwoGameScore > 0)
				{
					m_TextureTwoJingYuanBaoEx.DrawTexture(pD3DDevice, m_ptTwoJingYuanBao[m_cbBetIndex].x - 110, m_ptTwoJingYuanBao[m_cbBetIndex].y - 20);
				}
				else if(m_cbPlayIndex == TWO_XIA_ZHU)
				{
					m_TextureTwoJingYuanBao.DrawTexture(pD3DDevice, m_ptTwoJingYuanBao[m_cbBetIndex].x, m_ptTwoJingYuanBao[m_cbBetIndex].y);
				}
			}
			if(m_bShowSice[0])
			{
				if (m_cbSice[0] <= 6)
				{
					m_AnimationDice[0].DrawAnimation(pD3DDevice, m_ptDice0[0].x, m_ptDice0[0].y, m_cbSice[0] - 1, enXCenter, enYTop);
				}

				if (m_cbSice[1] <= 6)
				{
					m_AnimationDice[0].DrawAnimation(pD3DDevice, m_ptDice0[1].x, m_ptDice0[1].y, m_cbSice[1] - 1, enXCenter, enYTop);
				}
			}

			if(m_bShowSice[1])
			{
				if (m_cbSice[0] <= 6)
				{
					m_AnimationDice[1].DrawAnimation(pD3DDevice, m_ptDice1[0].x, m_ptDice1[0].y, m_cbSice[0] - 1, enXCenter, enYTop);
				}

				if (m_cbSice[1] <= 6)
				{
					m_AnimationDice[1].DrawAnimation(pD3DDevice, m_ptDice1[1].x, m_ptDice1[1].y, m_cbSice[1] - 1, enXCenter, enYTop);
				}
			}

			if(m_lTwoGameScore > 0)
			{
				TCHAR szOutNumT[128] = {0};
				_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%I64d"),m_lTwoGameScore);
				INT nLeftPos=nWidth / 2 - (m_TextureWinNumber.GetWidth()/lstrlen(TEXT("0123456789")) * lstrlen(szOutNumT)+m_TextureWin.GetWidth()) / 2;

				m_TextureWin.DrawTexture(pD3DDevice, nLeftPos, nHeight / 2 - (m_TextureWin.GetHeight() - m_TextureWinNumber.GetHeight()) / 2);
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureWinNumber, TEXT("0123456789"), szOutNumT, nLeftPos + m_TextureWin.GetWidth(), nHeight / 2, DT_LEFT);

				if(m_cbUser[1] == 5)
				{
					m_AnimationTwoTip.DrawAnimation(pD3DDevice);
					CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureTwoTipNumber, TEXT("0123456789"), GetUserClock(0), nWidth / 2 + 98, 488 + nFrame);
				}
			}
		}
		else if (m_cbGameStatus == GAME_SCENE_THREE)
		{
			if(m_cbPlayIndex == THREE_GUANDONG)
			{
				for (int i = 0; i < 4; i++)
				{
					//滚动动画
					LONGLONG lCurIndex = m_QuickenThreeItem[i].GetCurIndex();
					LONGLONG lMaxIndex = m_QuickenThreeItem[i].GetMaxIndex();
					if (lCurIndex + 50 * 5 >= lMaxIndex)
					{
						//抖动动画
						m_AnimationDouDongImage[m_cbThreeItemInfo[i]].DrawAnimation(pD3DDevice, m_ptThreeItemImage[i].x, m_ptThreeItemImage[i].y, 
							(50 * 5 - (lMaxIndex - lCurIndex + 1) ) / 50, enXLeft, enYTop);	
					}
					else
					{
						m_AnimationScrollGunDong.DrawAnimationScrollPos(pD3DDevice, m_ptThreeItemImage[i].x, m_ptThreeItemImage[i].y, 
							lCurIndex % 1410, enXLeft, enYTop);
					}
				}			
			}
			else
			{
				//静态图标
				for (int i = 0; i < 4; i++)
				{
					m_AnimationStaticImage.DrawAnimation(pD3DDevice, m_ptThreeItemImage[i].x, m_ptThreeItemImage[i].y, m_cbThreeItemInfo[i], enXLeft, enYTop);
				}		
			}

			m_TextureThreeBack.DrawTexture(pD3DDevice, 0, 0 + nFrame);

			if(m_cbPlayIndex != THREE_GUANDONG)
			{
				if (m_QuickenViewSan.GetCurIndex() % 2 == 0)
				{
					if (m_cbThreeZhongtimes == 0)
					{
						m_AnimationThreeTimes20L.DrawAnimation(pD3DDevice, m_ptThreeTimes[0].x, m_ptThreeTimes[0].y, 1, enXLeft, enYTop);
					}
					else if (m_cbThreeZhongtimes == 1)
					{
						m_AnimationThreeTimes500.DrawAnimation(pD3DDevice, m_ptThreeTimes[1].x, m_ptThreeTimes[1].y, 1, enXLeft, enYTop);
					}
					else if (m_cbThreeZhongtimes == 2)
					{
						m_AnimationThreeTimes20R.DrawAnimation(pD3DDevice, m_ptThreeTimes[2].x, m_ptThreeTimes[2].y, 1, enXLeft, enYTop);
					}
				}	
			}

			if (m_cbPlayIndex == THREE_KAIJIANG)
			{	
				BYTE cbIndex = m_QuickenThree.GetCurIndex() % 24;
				m_AnimationThreeDeng.DrawAnimation(pD3DDevice, m_ptThreeItem[cbIndex].x, m_ptThreeItem[cbIndex].y, m_cbItemType[cbIndex], enXLeft, enYTop);
			}	
			else if(m_cbPlayIndex == THREE_KAIJIANG_END)
			{
				if (m_QuickenViewSan.GetCurIndex() % 2 == 0)
				{
					BYTE cbIndex = m_QuickenThree.GetCurIndex() % 24;

					m_AnimationThreeDeng.DrawAnimation(pD3DDevice, m_ptThreeItem[cbIndex].x, m_ptThreeItem[cbIndex].y, m_cbItemType[cbIndex], enXLeft, enYTop);			

					if ((m_cbItemType[cbIndex] < CT_SHUIHUZHUAN) && m_bThreeZhongJiang && cbIndex == m_cbThreeIndex)
					{
						m_AnimationThreeTimes.DrawAnimation(pD3DDevice, m_ptThreeItemTimes[8 - m_cbItemType[cbIndex] - 1].x, m_ptThreeItemTimes[8 - m_cbItemType[cbIndex] - 1].y, 8 - m_cbItemType[cbIndex] - 1, enXLeft, enYTop);
					}
				}					
			}	

			if (m_lThreeGredit < 10000000 && m_lThreeTotalWin < 10000000 && m_lThreeTotalBet < 10000)
			{
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lThreeGredit, m_ptThreeGredit.x, m_ptThreeGredit.y);
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lThreeTotalWin, m_ptThreeTotalWin.x, m_ptThreeTotalWin.y);
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumber, TEXT("0123456789"), m_lThreeTotalBet, m_ptThreeTotalBet.x, m_ptThreeTotalBet.y);
			}
			else
			{
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lThreeGredit, m_ptThreeGredit.x, m_ptThreeGredit.y + 7);
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lThreeTotalWin, m_ptThreeTotalWin.x, m_ptThreeTotalWin.y + 7);
				CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureCellNumberS, TEXT("0123456789"), m_lThreeTotalBet, m_ptThreeTotalBet.x, m_ptThreeTotalBet.y + 7);
			}


			CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureTimesNumber, TEXT("0123456789"), m_lThreeTimes, m_ptThreeTotalTimes.x, m_ptThreeTotalTimes.y);


		}

	}



	if(m_bDrawTop || m_bDrawFrame)
	{
		//绘画框架
		DrawGameFrame(pD3DDevice, nWidth, nHeight);

		//标题
		m_D3DFont.DrawText(pD3DDevice, m_StrGameText, 5, 15 * nDesHeight / Screen_Height, D3DCOLOR_ARGB(255,255,255,255), DT_VCENTER|DT_LEFT);	
	}
}

// 更新缓冲 
void CGameClientView::OnWindowUpdataTextureBuffer( dex::CD3DDevice * pD3DDevice )
{

}

// 按键按下
void CGameClientView::OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
{
	//m_pGameClientEngine->SendMessage(IDM_GAME_OPERATE, nChar, 0);

}
// 窗口显示 
void CGameClientView::OnWindowShow(dex::CD3DDevice * pD3DDevice, bool bVisible )
{

}

//绘画框架
void CGameClientView::DrawGameFrame(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight)
{
	if(m_bDrawFrame)
	{
		//中间两边部分
		m_ImageGameFrame[3].DrawTexture(pD3DDevice,0,0,m_ImageGameFrame[3].GetWidth(),nHeight, 0,0,m_ImageGameFrame[3].GetWidth(), 1);
		m_ImageGameFrame[4].DrawTexture(pD3DDevice,nWidth-m_ImageGameFrame[4].GetWidth(),0,m_ImageGameFrame[4].GetWidth(),nHeight, 0,0,m_ImageGameFrame[4].GetWidth(), 1);
		
		//下部分		
		m_ImageGameFrame[6].DrawTexture(pD3DDevice,0,nHeight-m_ImageGameFrame[6].GetHeight(),nWidth,m_ImageGameFrame[6].GetHeight()*2, 0,0,1, m_ImageGameFrame[6].GetHeight());
	}
	
	//上部分
	m_ImageGameFrame[1].DrawTexture(pD3DDevice,0,0,nWidth,m_ImageGameFrame[1].GetHeight(), 0,0,1, m_ImageGameFrame[1].GetHeight());

	

	//左上角
	m_ImageGameFrame[0].DrawTexture(pD3DDevice,0,0);
	//右上角
	m_ImageGameFrame[2].DrawTexture(pD3DDevice,nWidth-m_ImageGameFrame[2].GetWidth(),0);
	
	if(m_bDrawFrame)
	{
		//左下角
		m_ImageGameFrame[5].DrawTexture(pD3DDevice,0,nHeight-m_ImageGameFrame[5].GetHeight());
		//右下角
		m_ImageGameFrame[7].DrawTexture(pD3DDevice,nWidth-m_ImageGameFrame[7].GetWidth(),nHeight-m_ImageGameFrame[7].GetHeight());
	}
}

// 鼠标移动 
void CGameClientView::OnMouseMove( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
	UpDateTop(ptMouseSite.y < 30 && ptMouseSite.y > 0);
}

// 左键点下 
void CGameClientView::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
	// 标题移动
	if ( ptMouseSite.y <= CAPTION_TOP_SIZE || !m_bDrawFrame)
	{
		AfxGetMainWnd()->PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(ptMouseSite.x,ptMouseSite.y) );

		return;
	}


}

// 左键抬起 
void CGameClientView::OnLButtonUp( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{

}

// 消息处理
bool CGameClientView::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	return false;
}

// 消息解释
bool CGameClientView::PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	if(uMessage == WM_MOUSELEAVE)
	{
		UpDateTop(false);
	}

	return false;
}

// 窗口大小 
void CGameClientView::OnWindowSize(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight)
{
	//设置多线程
	pD3DDevice->SetMultiThreaded(true);

	//多重采样
	pD3DDevice->SetEnableMultiSample(false);
	return;
}

// 关闭 
void CGameClientView::OnButtonWinClose()
{
	m_pGameClientEngine->PostMessage(IDM_GAME_CLOSE, 1, 0);
	return;
}

// 最大化 
void CGameClientView::OnButtonWinMax()
{
	
}

// 最小化 
void CGameClientView::OnButtonWinMin()
{
	AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
}

// 声音 
void CGameClientView::OnButtonSound()
{
	m_pGameClientEngine->PostMessage(IDM_GAME_SOUND, 0, 0);
}

// 设置 
void CGameClientView::OnButtonWinSet()
{
	m_pGameClientEngine->PostMessage(IDM_GAME_OPTION, 0, 0);
}

// 加注 
void CGameClientView::OnButtonAddBet()
{
	m_pGameClientEngine->PostMessage(IDM_GAME_BET, 1, 0);
}

// 减注 
void CGameClientView::OnButtonSubBet()
{
	m_pGameClientEngine->PostMessage(IDM_GAME_BET, 0, 0);
}

// 控制
void CGameClientView::OnButtonAdmin()
{
	if(m_pClientControlDlg != NULL && m_bShowClientControlDlg)
	{
		m_pClientControlDlg->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		m_pClientControlDlg->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		m_pClientControlDlg->SetCheatRight(true);
	}
}

// 初始化变量
void CGameClientView::ResetView(bool bResetEngine)
{
	
}

//开始开牌
VOID CGameClientView::StartOneGuanDong(bool bStart, BYTE cbItemInfo[][ITEM_X_COUNT])
{
	if (bStart)
	{
		m_cbPlayIndex = ONE_GUANDONG;
		CopyMemory(m_cbItemInfo, cbItemInfo, sizeof(m_cbItemInfo));

		int nIndex = 0;
		for (int i = 0; i < ITEM_X_COUNT; i++)
		{
			for (int j = 0; j < ITEM_Y_COUNT; j++)
			{
				m_Quicken[(j * ITEM_X_COUNT + i)].StartQuicken(10, 50, 1409*2 + 156 *2* nIndex, 50);
				nIndex++;
			}
		}
	}
	else
	{
		int nIndex = 0;
		for (int i = 0; i < ITEM_X_COUNT; i++)
		{
			for (int j = 0; j < ITEM_Y_COUNT; j++)
			{
				m_Quicken[(j * ITEM_X_COUNT + i)].StopQuicken();
				nIndex++;
			}
		}
	}
}

//开始开奖
VOID CGameClientView::StartZhongJiangXian(BYTE cbZhongJiangXian, CPoint ptZhongJiang[ITEM_X_COUNT])
{
	m_cbZhongJiangXian = cbZhongJiangXian;
	if (m_cbZhongJiangXian < ITEM_COUNT)
	{
		m_cbPlayIndex = ONE_ZHONGJIANG;
		CopyMemory(m_ptZhongJiang, ptZhongJiang, sizeof(m_ptZhongJiang));
		//闪光动画
		m_AnimationSanGuang.ShowAnimationEx(true, 70);
	}
	else
	{
		m_cbPlayIndex = ONE_NULL;
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			m_ptZhongJiang[j].SetPoint(0xFF, 0xFF);
		}
		//闪光动画
		m_AnimationSanGuang.StopAnimation();
	}
}

//开始开奖
VOID CGameClientView::StartKaiJang(bool bZhongJiang[][ITEM_X_COUNT])
{
	if (bZhongJiang != NULL)
	{
		m_cbPlayIndex = ONE_KAIJIANG;
		CopyMemory(m_bZhongJiang, bZhongJiang, sizeof(m_bZhongJiang));

		//序列动画
		m_AnimationXuLieImage[0].ShowAnimationEx(true, 68);
		m_AnimationXuLieImage[1].ShowAnimationEx(true, 71);
		m_AnimationXuLieImage[2].ShowAnimationEx(true, 91);
		m_AnimationXuLieImage[3].ShowAnimationEx(true, 69);
		m_AnimationXuLieImage[4].ShowAnimationEx(true, 81);
		m_AnimationXuLieImage[5].ShowAnimationEx(true, 77);
		m_AnimationXuLieImage[6].ShowAnimationEx(true, 60);
		m_AnimationXuLieImage[7].ShowAnimationEx(true, 79);
		m_AnimationXuLieImage[8].ShowAnimationEx(true, 50);
	}
	else
	{
		//序列动画
		m_AnimationXuLieImage[0].StopAnimation();
		m_AnimationXuLieImage[1].StopAnimation();
		m_AnimationXuLieImage[2].StopAnimation();
		m_AnimationXuLieImage[3].StopAnimation();
		m_AnimationXuLieImage[4].StopAnimation();
		m_AnimationXuLieImage[5].StopAnimation();
		m_AnimationXuLieImage[6].StopAnimation();
		m_AnimationXuLieImage[7].StopAnimation();
		m_AnimationXuLieImage[8].StopAnimation();
	}

}

//开始开奖
VOID CGameClientView::StartKaiJangEnd(SCORE lScore, bool bLong)
{
	m_cbPlayIndex = ONE_KAIJIANG_END;
	m_lOneGameScore = lScore;
	m_lOneTotalWin = m_lOneGameScore;
	m_lOneScoreTemp = 0;
	if (bLong)
	{
		//中大奖飞龙出画
		//m_AnimationLong.ShowAnimation(true, 150);
	}
}

//开始比倍
VOID CGameClientView::StartTwoWiatAddScore(bool bStart)
{
	if (bStart)
	{
		m_AnimationTwoSanGuang.ShowAnimation(true, 500, enCycle);
		m_AnimationTwoSanGuang.StopAnimationSan(true);

	}
	else
	{
		m_AnimationTwoSanGuang.ShowAnimation(false);
	}	
}

//开始开牌
VOID CGameClientView::StartThreeGunDong(BYTE cbItemType[4])
{
	m_cbPlayIndex = THREE_GUANDONG;
	CopyMemory(m_cbThreeItemInfo, cbItemType, sizeof(m_cbThreeItemInfo));

	for (int i = 0; i < 4; i++)
	{
		m_QuickenThreeItem[i].StartQuicken(10, 50, 1409*4 + 156 * 2 * i, 50);
	}
}

//开始开牌
VOID CGameClientView::StartThreeGunDongEnd(SCORE lScore, BYTE cbPos)
{
	m_lThreeTotalWin += lScore;
	m_cbThreeZhongtimes = cbPos;
	m_QuickenViewSan.StartQuicken(200, 200, 21, 1, 0, true);
}

//开始开牌
VOID CGameClientView::StartThreeKaiJian(BYTE cbIndex)
{
	ASSERT(cbIndex < 24);

	m_cbPlayIndex = THREE_KAIJIANG;

	m_bThreeZhongJiang = false;
	for (int i = 0; i < 4; i++)
	{
		if (m_cbThreeItemInfo[i] == m_cbItemType[cbIndex] && m_cbItemType[cbIndex] != CT_SHUIHUZHUAN)
		{
			m_bThreeZhongJiang = true;
			break;
		}
	}
	m_cbThreeIndex = cbIndex;
	m_QuickenThree.StartQuicken(30, 200, 24 * 6 + m_cbThreeIndex, 1);
	m_QuickenThree.SetSendSteps(true);
}

//开始开牌
VOID CGameClientView::StartThreeKaiJianEnd(SCORE lScore)
{
	m_cbPlayIndex = THREE_KAIJIANG_END;
	m_lThreeGameScore = lScore;
	m_QuickenSan.StartQuicken(200, 200, 20, 1);
}

//设置控制
VOID CGameClientView::ShowClientControlDlg(bool bShowClientControlDlg) 
{
	m_bShowClientControlDlg = bShowClientControlDlg; 
	m_ButtonAdmin.ShowWindow(bShowClientControlDlg && m_bDrawTop && m_pClientControlDlg != NULL);
}

//取得说明索引
VOID CGameClientView::ShowShuoMing(bool bShowShuoMing) 
{ 
	m_bShowShuoMing = bShowShuoMing;
	if ((GAME_SCENE_FREE == m_cbGameStatus || GAME_SCENE_ONE== m_cbGameStatus) && !m_bShowShuoMing)
	{
		m_ButtonAddBet.ShowWindow(true);
		m_ButtonSubBet.ShowWindow(true);
	}
	else
	{
		m_ButtonAddBet.ShowWindow(false);
		m_ButtonSubBet.ShowWindow(false);
	}
}

//设置下注
VOID CGameClientView::SetSaveScore(SCORE lSaveScore) 
{
	m_lSaveScore = lSaveScore;
	if (GAME_SCENE_FREE == m_cbGameStatus && m_cbPlayIndex == ONE_NULL)
	{
		//获取用户
		if (m_pClientUserItem!=NULL)
		{
			m_lOneGredit = m_pClientUserItem->GetUserScore() - m_lSaveScore;
		}
	}
}

//开始开牌
VOID CGameClientView::SetItemInfo(BYTE cbItemInfo[][ITEM_X_COUNT])
{
	CopyMemory(m_cbItemInfo, cbItemInfo, sizeof(m_cbItemInfo));
}

//设置下注
VOID CGameClientView::SetUser(BYTE cbUser[3], bool bOne, BYTE cbExp)
{
	CopyMemory(m_cbUser, cbUser, sizeof(m_cbUser));

	switch(m_cbUser[0])
	{
	case 0:
		{
			m_AnimationZuoHeGuan[0].ShowAnimation(true, 100, enCycle);
			break;
		}
	case 1:
		{	
			m_AnimationZuoHeGuan[1].ShowAnimation(true, 100, enCycle);
			break;
		}
	case 2:
		{
			m_AnimationZuoHeGuan[2].ShowAnimation(true, 100, enCycle);
			break;
		}
	case 3:
		{
			m_AnimationZuoHeGuan[3].ShowAnimation(true, 100, enCycle);
			break;
		}
	}


	switch(m_cbUser[1])
	{
	case 0:
		{
			m_AnimationHeGuan[0].ShowAnimationEx(true, 200, enOnce);
			m_AnimationHeGuan[0].SetSendIndex(5);
			break;
		}
	case 1:
		{
			m_AnimationHeGuan[1].ShowAnimationEx(true, 250, enOnce);
			break;
		}
	case 2:
		{		
			m_AnimationHeGuan[2].ShowAnimationEx(true, 150, enOnce);
			m_AnimationHeGuan[2].SetSendIndex(25);
			break;
		}
	case 3:
		{			
			m_AnimationHeGuan[3].ShowAnimationEx(true, 100, enOnce);
			m_AnimationHeGuan[3].SetSendIndex(21);
			break;
		}
	case 4:
		{			
			m_AnimationHeGuan[4].ShowAnimationEx(true, 100, enOnce);
			m_AnimationHeGuan[4].SetSendIndex(24);
			break;
		}
	case 5:
		{			
			m_AnimationHeGuan[5].ShowAnimationEx(true, 100, bOne ? enOnce : enCycle);
			break;
		}
	}

	switch(m_cbUser[2])
	{
	case 0:
		{
			m_AnimationYouHeGuan[0].ShowAnimation(true, 300, enCycle);
			break;
		}
	case 1:
		{
			m_AnimationYouHeGuan[1].ShowAnimation(true, 200, enCycle);
			break;
		}
	case 2:
		{
			m_AnimationYouHeGuan[2].ShowAnimation(true, 800, enCycle);
			break;
		}
	case 3:
		{
			m_AnimationYouHeGuan[3].ShowAnimation(true, 300, enCycle);
			break;
		}
	}
}

//设置下注
VOID CGameClientView::SetTwoDouDong()
{
	m_AnimationZuoZhiDouDong.ShowAnimation(true, 100, enOnce);
	m_AnimationJingYuanBaoDouDong.ShowAnimation(true, 125, enOnce);
}

//设置分数
VOID CGameClientView::SetOneScore(SCORE lTotalWin, SCORE lTotalBetScore)
{
	//获取用户
	if (m_pClientUserItem!=NULL)
	{
		m_lOneGredit = m_pClientUserItem->GetUserScore() + lTotalWin - lTotalBetScore - m_lSaveScore;
	}
	m_lOneTotalWin = lTotalWin;
	m_lOneScoreTemp = 0;
}

//设置分数
VOID CGameClientView::SetTwoScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lExpScore/* = 0*/)
{
	m_cbPlayIndex = TWO_KAI_JIAN;
	//获取用户
	if (m_pClientUserItem!=NULL)
	{
		m_lTwoGredit = m_pClientUserItem->GetUserScore() - lExpScore - m_lSaveScore;
	}

	m_lTwoTotalWin = lTotalWin;
	m_lTwoTotalBet = lTotalBet;
}

//设置分数
VOID CGameClientView::SetThreeScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lTimes, SCORE lExpScore/* = 0*/)
{
	//获取用户
	if (m_pClientUserItem!=NULL)
	{
		m_lThreeGredit = m_pClientUserItem->GetUserScore() - lExpScore - m_lSaveScore;
	}

	m_lThreeTotalWin = lTotalWin;
	m_lThreeTotalBet = lTotalBet;
	m_lThreeTimes = lTimes;
}

//设置下注
VOID CGameClientView::SetTwoBetIndex(BYTE cbBetIndex)
{
	m_cbPlayIndex = TWO_XIA_ZHU;
	m_cbBetIndex = cbBetIndex;
	m_AnimationTwoSanGuang.ShowAnimation(false);
}

//设置下注
VOID CGameClientView::SetTwoSice(BYTE cbSice[2])
{
	CopyMemory(m_cbSice, cbSice, sizeof(m_cbSice));
}

//设置下注
VOID CGameClientView::SetTwoShowSice(bool bSice0, bool bSice1)
{
	m_bShowSice[0] = bSice0;
	m_bShowSice[1] = bSice1;
}

//退出比倍
VOID CGameClientView::ExitTwo()
{
	//比倍变量
	BYTE cbUser[3] = {0xFF, 0xFF, 0xFF};
	SetUser(cbUser, false, 0);
	m_lTwoGredit = 0;
	m_lTwoTotalWin = 0;
	m_lTwoTotalBet = 0;
	m_lTwoGameScore = 0;
	m_cbBetIndex = 0xFF;
	m_cbSice[0] = 0xFF;	
	m_cbSice[1] = 0xFF;	
}

//设置状态
VOID CGameClientView::SetGameStatus(BYTE cbGameStatus) 
{
	m_cbGameStatus = cbGameStatus;
	if (GAME_SCENE_FREE == m_cbGameStatus)
	{
		m_cbPlayIndex = ONE_NULL;

		//获取用户
		if (m_pClientUserItem!=NULL)
		{
			m_lOneGredit = m_pClientUserItem->GetUserScore() - m_lSaveScore;
		}
	}

	if (GAME_SCENE_FREE == m_cbGameStatus || GAME_SCENE_ONE== m_cbGameStatus)
	{
		m_ButtonAddBet.ShowWindow(true);
		m_ButtonSubBet.ShowWindow(true);
	}
	else
	{
		m_ButtonAddBet.ShowWindow(false);
		m_ButtonSubBet.ShowWindow(false);
	}
}

//进入场景
VOID CGameClientView::EnterScene(BYTE cbEnterScene)
{
	if (cbEnterScene == 0)
	{
		m_AnimationShuoMing.ShowAnimation(true, 1000, enCycle);
	}
	else if (cbEnterScene == 1)
	{
		//打鼓动画
		m_AnimationDaGu.ShowAnimation(true, 100, enCycle);

		//顶部水浒传3个字体闪光
		m_AnimationShuiHuZhuang.ShowAnimation(true, 300, enCycle);

		//中奖线
		m_AnimationZhongJiangXian.ShowAnimation(true, 150);

		//等待开始状态下的飘旗
		m_AnimationWaitQi.ShowAnimation(true, 100, enCycle);

		//滚动状态下的摇旗
		m_AnimationKaiQi.ShowAnimation(true, 50, enCycle);

		//闪光动画
		m_AnimationSanGuang.ShowAnimation(true, 100, enCycle);

		//边框动画
		m_AnimationBianKuang.ShowAnimation(true, 200, enCycle);

		//抖动动画
		m_AnimationDouDongImage[0].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[1].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[2].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[3].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[4].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[5].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[6].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[7].ShowAnimation(true, 50, enCycle);
		m_AnimationDouDongImage[8].ShowAnimation(true, 50, enCycle);

		//静态图标
		m_AnimationStaticImage.ShowAnimation(true, 1000, enCycle);
		m_AnimationOneTip.ShowAnimation(true, 1000, enCycle);
	}
	else if (cbEnterScene == 2)
	{
		m_AnimationDice[0].ShowAnimation(true, 300, enCycle);
		m_AnimationDice[1].ShowAnimation(true, 300, enCycle);
		m_AnimationTwoTip.ShowAnimation(true, 1000, enCycle);
	}
	else if (cbEnterScene == 3)
	{
		m_AnimationThreeDeng.ShowAnimation(true, 1000, enCycle);
		m_AnimationThreeTimes.ShowAnimation(true, 1000, enCycle);
		m_AnimationThreeTimes20L.ShowAnimation(true, 1000, enCycle);
		m_AnimationThreeTimes20R.ShowAnimation(true, 1000, enCycle);
		m_AnimationThreeTimes500.ShowAnimation(true, 1000, enCycle);
	}
}

//取得时间
WORD CGameClientView::GetUserClock(WORD wChairID)
{
	if(m_pGameClientEngine->m_wClockID == INVALID_CHAIR)
	{
		return 0;
	}
	return m_pGameClientEngine->m_wClockCount;
}

//加载图片
bool CGameClientView::LoadImage()
{	
	m_cbImageLoadPos = 0;	
	m_TextureCellNumber.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("押分数字")); m_cbImageLoadPos=1;
	m_TextureCellNumberS.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("押分数字小")); m_cbImageLoadPos=2;
	m_TextureWin.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("win")); m_cbImageLoadPos=3;
	m_TextureWinNumber.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("共用"), _T("资源"), _T("中奖数字"));m_cbImageLoadPos=4;
	m_TextureOneBack.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第一部分"), _T("资源"), _T("界面")); m_cbImageLoadPos=5;
	m_TextureOneTipNumber.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第一部分"), _T("资源"), _T("提示数字")); m_cbImageLoadPos=6;

	//动画
	m_AnimationShuiHuZhuang.InitAnimation(m_pD3DDevice, TEXT("顶部水浒传3个字体闪光"), TEXT("aa"), 10, 10); m_cbImageLoadPos=6;
	m_AnimationDaGu.InitAnimation(m_pD3DDevice, TEXT("打鼓"), TEXT("aa"), 29, 29);m_cbImageLoadPos=8;
	m_AnimationZhongJiangXian.InitAnimation(m_pD3DDevice, TEXT("中奖线"), TEXT(""), 9, 2);m_cbImageLoadPos=11;
	m_AnimationWaitQi.InitAnimation(m_pD3DDevice, TEXT("等待开始状态下的飘旗"), TEXT("aa"), 43, 43);m_cbImageLoadPos=13;
	m_AnimationKaiQi.InitAnimation(m_pD3DDevice, TEXT("滚动状态下的摇旗"), TEXT("aa"), 53, 53);m_cbImageLoadPos=15;
	m_AnimationSanGuang.InitAnimationSingle(m_pD3DDevice, TEXT("共用"), TEXT("闪光"), 9, 1);m_cbImageLoadPos=16;
	m_AnimationBianKuang.InitAnimationSingle(m_pD3DDevice, TEXT("第一部分"), TEXT("边框"), 6, 1);m_cbImageLoadPos=17;
	m_AnimationScrollGunDong.InitAnimationSingle(m_pD3DDevice, TEXT("共用"), TEXT("滚动胶片"), 1, 9, false, 8);m_cbImageLoadPos=18;
	m_AnimationStaticImage.InitAnimationSingle(m_pD3DDevice, TEXT("共用"), TEXT("技能图标"), 9, 3);m_cbImageLoadPos=19;
	m_AnimationOneTip.InitAnimationSingle(m_pD3DDevice, TEXT("第一部分"), TEXT("提示"), 1, 6);m_cbImageLoadPos=20;


	//序列动画
	m_AnimationXuLieImage[0].InitAnimation(m_pD3DDevice, TEXT("序列斧头"), TEXT("aa"), 42, 42);m_cbImageLoadPos=21;
	m_AnimationXuLieImage[1].InitAnimation(m_pD3DDevice, TEXT("序列银枪"), TEXT("aa"), 40, 40);m_cbImageLoadPos=23;
	m_AnimationXuLieImage[2].InitAnimation(m_pD3DDevice, TEXT("序列大刀"), TEXT("aa"), 31, 31);m_cbImageLoadPos=25;
	m_AnimationXuLieImage[3].InitAnimation(m_pD3DDevice, TEXT("序列鲁"), TEXT("aa"), 41, 41); m_cbImageLoadPos=27;
	m_AnimationXuLieImage[4].InitAnimation(m_pD3DDevice, TEXT("序列林"), TEXT("aa"), 35, 35);m_cbImageLoadPos=29;
	m_AnimationXuLieImage[5].InitAnimation(m_pD3DDevice, TEXT("序列宋"), TEXT("aa"), 37, 37);m_cbImageLoadPos=31;
	m_AnimationXuLieImage[6].InitAnimation(m_pD3DDevice, TEXT("序列替天行道"), TEXT("aa"), 47, 47);m_cbImageLoadPos=33;
	m_AnimationXuLieImage[7].InitAnimation(m_pD3DDevice, TEXT("序列忠义堂"), TEXT("aa"), 36, 36);m_cbImageLoadPos=35;
	m_AnimationXuLieImage[8].InitAnimation(m_pD3DDevice, TEXT("序列水浒传"), TEXT("aa"), 57, 57);m_cbImageLoadPos=37;

	//抖动动画
	m_AnimationDouDongImage[0].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("斧头"), 5, 5);m_cbImageLoadPos=38;
	m_AnimationDouDongImage[1].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("银枪"), 5, 5);m_cbImageLoadPos=39;
	m_AnimationDouDongImage[2].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("大刀"), 5, 5);m_cbImageLoadPos=40;
	m_AnimationDouDongImage[3].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("鲁"), 5, 5);m_cbImageLoadPos=41;
	m_AnimationDouDongImage[4].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("林"), 5, 5);m_cbImageLoadPos=42;
	m_AnimationDouDongImage[5].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("宋"), 5, 5);m_cbImageLoadPos=43;
	m_AnimationDouDongImage[6].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("替天行道"), 5, 5);m_cbImageLoadPos=44;
	m_AnimationDouDongImage[7].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("忠义堂"), 5, 5);m_cbImageLoadPos=45;
	m_AnimationDouDongImage[8].InitAnimation(m_pD3DDevice, TEXT("抖动"), TEXT("水浒传"), 5, 5);m_cbImageLoadPos=46;

	m_TextureTwoBack.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第二部分"), _T("资源"), _T("界面")); m_cbImageLoadPos=47;
	m_TextureTwoTipNumber.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第二部分"), _T("资源"), _T("提示数字")); m_cbImageLoadPos=48;
	m_TextureTwoJingYuanBao.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第二部分"), _T("资源"), _T("bet_gold")); m_cbImageLoadPos=50;
	m_TextureTwoJingYuanBaoEx.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第二部分"), _T("资源"), _T("中奖金子")); m_cbImageLoadPos=51;
	m_AnimationTwoTip.InitAnimationSingle(m_pD3DDevice, TEXT("第二部分"), TEXT("提示"), 1, 1);m_cbImageLoadPos=61;

	m_AnimationHeGuan[0].InitAnimation(m_pD3DDevice, TEXT("荷官打开骰盅"), TEXT("aa"), 14, 6);m_cbImageLoadPos=53;
	m_AnimationHeGuan[1].InitAnimation(m_pD3DDevice, TEXT("荷官开心表情"), TEXT(""), 7, 6);m_cbImageLoadPos=54;
	m_AnimationHeGuan[2].InitAnimation(m_pD3DDevice, TEXT("荷官哭了"), TEXT(""), 26, 6);m_cbImageLoadPos=56;
	m_AnimationHeGuan[3].InitAnimation(m_pD3DDevice, TEXT("荷官输了愤怒表情"), TEXT(""), 26, 6);m_cbImageLoadPos=58;
	m_AnimationHeGuan[4].InitAnimation(m_pD3DDevice, TEXT("荷官摇骰盅"), TEXT(""), 29, 9);m_cbImageLoadPos=60;
	m_AnimationHeGuan[5].InitAnimation(m_pD3DDevice, TEXT("荷官默认"), TEXT("aa"), 8, 8);m_cbImageLoadPos=61;

	m_AnimationZuoHeGuan[0].InitAnimation(m_pD3DDevice, TEXT("左边瘦子喝彩"), TEXT("aa"), 29, 29);m_cbImageLoadPos=63;
	m_AnimationZuoHeGuan[1].InitAnimation(m_pD3DDevice, TEXT("左边瘦子开心"), TEXT("aa"), 56, 25);m_cbImageLoadPos=65;
	m_AnimationZuoHeGuan[2].InitAnimation(m_pD3DDevice, TEXT("左边瘦子默认"), TEXT("aa"), 27, 27);m_cbImageLoadPos=67;
	m_AnimationZuoHeGuan[3].InitAnimation(m_pD3DDevice, TEXT("左边瘦子伤心"), TEXT("aa"), 36, 25);m_cbImageLoadPos=69;

	m_AnimationYouHeGuan[0].InitAnimation(m_pD3DDevice, TEXT("右边胖子喝彩"), TEXT("aa"), 29, 25); m_cbImageLoadPos=71;
	m_AnimationYouHeGuan[1].InitAnimation(m_pD3DDevice, TEXT("右边胖子开心"), TEXT("aa"), 18, 18); m_cbImageLoadPos=73;
	m_AnimationYouHeGuan[2].InitAnimation(m_pD3DDevice, TEXT("右边胖子默认"), TEXT("aa"), 51, 20);m_cbImageLoadPos=75;
	m_AnimationYouHeGuan[3].InitAnimation(m_pD3DDevice, TEXT("右边胖子伤心"), TEXT("aa"), 26, 26);m_cbImageLoadPos=77;

	m_AnimationDice[0].InitAnimation(m_pD3DDevice, TEXT("第二部分"), TEXT(""), 6, 6);m_cbImageLoadPos=78;
	m_AnimationDice[1].InitAnimation(m_pD3DDevice, TEXT("第二部分"), TEXT("大骰子"), 6, 6);m_cbImageLoadPos=79;

	m_AnimationZuoZhiDouDong.InitAnimation(m_pD3DDevice, TEXT("金元宝震动"), TEXT("桌面晃动"), 5, 5);m_cbImageLoadPos=81;
	m_AnimationJingYuanBaoDouDong.InitAnimation(m_pD3DDevice, TEXT("金元宝震动"), TEXT("金元宝震动"), 4, 4);m_cbImageLoadPos=83;
	m_AnimationTwoSanGuang.InitAnimationSingle(m_pD3DDevice, TEXT("第二部分"), TEXT("大小和中奖那方发光"), 1, 1);m_cbImageLoadPos=85;

	m_TextureThreeBack.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第三部分"), _T("资源"), _T("界面")); m_cbImageLoadPos=87;
	m_TextureTimesNumber.LoadWHGTexture( m_pD3DDevice, CGlobal::_Object()->hModuleResource, _T("第三部分"), _T("资源"), _T("time")); m_cbImageLoadPos=89;
	m_AnimationThreeDeng.InitAnimation(m_pD3DDevice, TEXT("第三部分"), TEXT("图标"), 9, 9);m_cbImageLoadPos=91;
	m_AnimationThreeTimes.InitAnimationSingle(m_pD3DDevice, TEXT("第三部分"), TEXT("times"), 8, 1);m_cbImageLoadPos=93;
	m_AnimationThreeTimes20L.InitAnimationSingle(m_pD3DDevice, TEXT("第三部分"), TEXT("20L"), 2, 1);m_cbImageLoadPos=95;
	m_AnimationThreeTimes20R.InitAnimationSingle(m_pD3DDevice, TEXT("第三部分"), TEXT("20R"), 2, 1);m_cbImageLoadPos=95;
	m_AnimationThreeTimes500.InitAnimationSingle(m_pD3DDevice, TEXT("第三部分"), TEXT("500"), 2, 1); m_cbImageLoadPos=98;

	m_cbImageLoadPos=100;
	m_bInitImage = true;

	return true;
}

//启动线程
VOID CGameClientView::ThreadInfo()
{
	m_pMyUIThread->PostThreadMessage(WM_THREADINFO, 0, 0);
}

//更新按钮
VOID CGameClientView::UpDateTop(bool bDrawTop)
{
	bool bDrawTopTemp = m_bDrawTop;
	m_bDrawTop = bDrawTop;

	if(bDrawTopTemp != m_bDrawTop || m_bDrawFrame)
	{
		//顶层按钮
		m_ButtonWinClose.ShowWindow(m_bDrawTop || m_bDrawFrame);
		//m_ButtonWinMax..ShowWindow(m_bDrawTop || m_bDrawFrame);
		m_ButtonWinMin.ShowWindow(m_bDrawTop || m_bDrawFrame);
		m_ButtonSound.ShowWindow(m_bDrawTop || m_bDrawFrame);
		m_ButtonWinSet.ShowWindow(m_bDrawTop || m_bDrawFrame);
		m_ButtonAdmin.ShowWindow((m_bDrawTop || m_bDrawFrame ) && m_bShowClientControlDlg && m_pClientControlDlg != NULL);
	}
}
////////////////////////////////////////////////////////////////////////////////// 

