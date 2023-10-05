#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "CardControl.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//动画参数
#define ROCKET_CARTOON_COUNT		1									//火箭数目
#define BOMB_CARTOON_COUNT			14									//爆炸数目
#define LINE_CARTOON_COUNT			5									//顺子数目
#define DOUBLE_LINE_CARTOON_COUNT			12						//连对数目
#define THREE_LINE_CARTOON_COUNT			13						//三同张数目
#define THREE_TAKE_CARTOON_COUNT			14						//三带牌数目
#define THREE_TAKE_LINE_CARTOON_COUNT			16				//三连带数目
#define PLAN_CARTOON_COUNT			3									//飞机数目
#define CARD_WARNING_COUNT			2									//报警数目

#define REVERSAL_CARD_COUNT			4									//翻牌数目

//洗牌动画
#define RIFFLE_CARD_COUNT_X			4									//洗牌数目
#define RIFFLE_CARD_COUNT_Y			6									//洗牌数目
#define RIFFLE_CARD_COUNT_ALL		RIFFLE_CARD_COUNT_X*RIFFLE_CARD_COUNT_Y

//发牌动画
#define DISPATCH_CARD_TIMES			6									//发牌次数
#define DISPATCH_CARD_SPACE			2									//发牌间隔
#define DISPATCH_HEAP_DISTANCE		3									//牌堆间距

//////////////////////////////////////////////////////////////////////////////////

//提示按钮
#define IDC_BT_START				100									//开始按钮
#define IDC_BT_SCORE				101									//查分按钮

//叫分按钮
#define IDC_CALL_SCORE_NONE			200									//叫分按钮
#define IDC_CALL_SCORE_1			201									//叫分按钮
#define IDC_CALL_SCORE_2			202									//叫分按钮
#define IDC_CALL_SCORE_3			203									//叫分按钮

#define IDC_XUAN						210
#define IDC_NO_XUAN				211
#define IDC_ASK						212
#define IDC_NO_ASK					213
#define IDC_RESPOND				214
#define IDC_NO_RESPOND			215
#define IDC_ADD						216
#define IDC_NO_ADD				217

#define IDC_SELECT_FRIEND		218
//游戏按钮
#define IDC_BT_OUT_CARD				300									//出牌按钮
#define IDC_BT_PASS_CARD			301									//PASS按钮
#define IDC_BT_OUT_PROMPT           302                             	//提示按钮

//功能按钮
#define IDC_BT_TRUSTEE				400									//托管控制
#define IDC_BT_CANCEL_TRUSTEE		401									//托管控制
#define IDC_BT_SORT_CARD			402									//扑克排序
#define IDC_BT_LAST_TURN			403									//上轮扑克
#define IDC_BT_REMEMBER_CARD		404									//开牌器

//成绩窗口
#define IDC_GAME_SCORE				600									//成绩窗口

//记牌器窗口
#define IDC_REMEMBER_CARD_WND		700									//记牌器窗口
//////////////////////////////////////////////////////////////////////////////////
#define CardW 40
#define CardH 50

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewD3D)

	//系统消息
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//控制按钮
	ON_BN_CLICKED(IDC_BT_START, OnStart)
	ON_BN_CLICKED(IDC_BT_SCORE, OnLookScore)
	ON_BN_CLICKED(IDC_BT_SORT_CARD, OnSortCard)
	ON_BN_CLICKED(IDC_BT_LAST_TURN, OnLastTurn)
	ON_BN_CLICKED(IDC_BT_TRUSTEE, OnTrustee)
	ON_BN_CLICKED(IDC_BT_CANCEL_TRUSTEE, OnCancelTrustee)
	//游戏按钮
	ON_BN_CLICKED(IDC_BT_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_BT_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_BT_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_SELECT_FRIEND, OnSelectFriend)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_XUAN, IDC_NO_XUAN, OnXuanZhan)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ASK, IDC_NO_RESPOND, OnAskFriend)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ADD,IDC_NO_ADD, OnAddTimes)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
TCHAR* szColor[4]={_T("方块"),_T("梅花"),_T("红桃"),_T("黑桃")};
TCHAR* szValue[13]={_T("A"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),_T("10"),_T("J"),_T("Q"),_T("K")};
//构造函数
CGameClientView::CGameClientView()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//爆炸动画
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//飞机动画
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//顺子动画
	m_bCartoonLine=false;
	m_wCartoonLineIndex=0L;



	//翻牌动画
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//发牌动画
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	m_bMoveMouseEx=false;
	m_bSelectCardEx=false;
	m_bSwitchCardEx=false;
	m_cbHoverCardItemEx=INVALID_ITEM;
	m_cbMouseDownItemEx=INVALID_ITEM;
	m_cbSwitchCardItemEx=INVALID_ITEM;
	//游戏变量
	m_lCellScore=0L;
	//发牌数据
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//位置变量
	ZeroMemory(&m_ptUserFrame, sizeof(m_ptUserFrame));
	ZeroMemory(&m_ptPass, sizeof(m_ptPass));
	ZeroMemory(&m_ptBanker, sizeof(m_ptBanker));
	ZeroMemory(&m_ptCountWarn, sizeof(m_ptCountWarn));
	ZeroMemory(&m_ptCallScore, sizeof(m_ptCallScore));
	ZeroMemory(&m_ptCardFrame,sizeof(m_ptCardFrame));
	//设置变量
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//报警动画
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	ZeroMemory(m_cbGameScore,sizeof(m_cbGameScore));
	m_cbTurnScore=0;
	ZeroMemory(m_cbXuanZhanFlag,sizeof(m_cbXuanZhanFlag));
	ZeroMemory(m_cbAskFlag,sizeof(m_cbAskFlag));
	ZeroMemory(m_cbAddTimesFlag,sizeof(m_cbAddTimesFlag));
	ZeroMemory(m_cbFriendFlag,sizeof(m_cbFriendFlag));
	ZeroMemory(m_cbTrusteeFlag,sizeof(m_cbTrusteeFlag));
	m_bShowFindFriend=false;
	m_bWaitXuanZhan=false;
	m_bWaitFindFriend=false;
	m_bWaitAskFriend=false;
	m_bWaitAddTimes=false;
	m_b2Biggest=false;
	m_cbSelectedCardData=0;
	m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//爆炸动画
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//飞机动画
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//顺子动画
	m_bCartoonLine=false;
	m_wCartoonLineIndex=0L;



	//翻牌动画
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//发牌动画
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//游戏变量
	m_lCellScore=0L;

	//发牌数据
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//设置变量
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//报警动画
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	//设置界面
	m_btScore.ShowWindow(false);
	m_btStart.ShowWindow(false);
	m_btOutCard.ShowWindow(false);
	m_btPassCard.ShowWindow(false);
	m_btOutPrompt.ShowWindow(false);

	//游戏控件
	m_btTrustee.ShowWindow(true);
	m_btCancelTrustee.ShowWindow(false);
	m_btTrustee.EnableWindow(false);
	m_btLastTurn.EnableWindow(false);
	m_btSortCard.EnableWindow(false);


	//游戏控件
	m_ScoreControl.CloseControl();
	m_HandCardControl[ME_VIEW_ID].SetPositively(false);
	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(NULL,0);
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);
	}

	//销毁资源
	if (m_TextureUserPass.IsNull()==false) m_TextureUserPass.Destory();
	if (m_TextureCountWarn.IsNull()==false) m_TextureCountWarn.Destory();
	if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
	if (m_TextureCartoonLine.IsNull()==false) m_TextureCartoonLine.Destory();
	if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();
	if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();

	//按钮图片
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(&m_D3DDevice,TEXT("BT_SORT_CARD_ORDER"),TEXT("PNG"),hResInstance);

	ZeroMemory(m_cbGameScore,sizeof(m_cbGameScore));
	m_cbTurnScore=0;
	ZeroMemory(m_cbAddTimesFlag,sizeof(m_cbAddTimesFlag));
	ZeroMemory(m_cbXuanZhanFlag,sizeof(m_cbXuanZhanFlag));
	ZeroMemory(m_cbAskFlag,sizeof(m_cbAskFlag));
	m_bShowFindFriend=false;
	m_bWaitXuanZhan=false;
	m_bWaitFindFriend=false;
	m_bWaitAskFriend=false;
	m_bWaitAddTimes=false;
	return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	//用户背景
	m_ptUserFrame[0].SetPoint((nWidth - m_TextureUserBG.GetWidth())/2+250, 5);
	m_ptUserFrame[1].SetPoint(5, (nHeight - m_TextureUserBG.GetHeight())/2);
	m_ptUserFrame[2].SetPoint(5, nHeight-m_TextureUserBG.GetHeight() - 100);
	m_ptUserFrame[3].SetPoint(nWidth-50 - m_TextureUserBG.GetWidth(),(nHeight - m_TextureUserBG.GetHeight())/2);

	//头像位置	
	m_ptAvatar[0].SetPoint(m_ptUserFrame[0].x + 13, m_ptUserFrame[0].y + 13);
	m_ptAvatar[1].SetPoint(m_ptUserFrame[1].x + 13, m_ptUserFrame[1].y + 13);
	m_ptAvatar[2].SetPoint(m_ptUserFrame[2].x + 13, m_ptUserFrame[2].y + 13);
	m_ptAvatar[3].SetPoint(m_ptUserFrame[3].x + 13, m_ptUserFrame[3].y + 13);

	//帐号位置
	//m_ptNickName[0].SetPoint(m_ptAvatar[0].x+FACE_CX+33, m_ptAvatar[0].y+7);
	m_ptNickName[0].SetPoint(m_ptAvatar[0].x+11, m_ptAvatar[0].y-20);
	m_ptNickName[1].SetPoint(m_ptAvatar[1].x+11, m_ptAvatar[1].y-20);
	m_ptNickName[2].SetPoint(m_ptAvatar[2].x+11, m_ptAvatar[2].y-20);
	m_ptNickName[3].SetPoint(m_ptAvatar[3].x+11, m_ptAvatar[3].y-20);

	//时间位置
	m_ptClock[0].SetPoint(m_ptUserFrame[0].x-170, 32);
	m_ptClock[1].SetPoint(15, m_ptUserFrame[1].y - 130);
	m_ptClock[2].SetPoint((nWidth-74)/2, nHeight-355);
	m_ptClock[3].SetPoint(nWidth - 89, m_ptUserFrame[3].y - 130);

	//准备位置
	m_ptReady[0].SetPoint((nWidth - 70)/2 +25 ,200);
	m_ptReady[1].SetPoint(220,(nHeight - 30)/2);
	m_ptReady[2].SetPoint((nWidth - 70)/2 +25 ,nHeight - 230);
	m_ptReady[3].SetPoint(nWidth - 270,(nHeight - 30)/2);

	//放弃位置
	m_ptPass[0].SetPoint((nWidth - 64)/2 ,172);
	m_ptPass[1].SetPoint(220,(nHeight - 34)/2);
	m_ptPass[2].SetPoint((nWidth - 64)/2-5  ,nHeight - 280);
	m_ptPass[3].SetPoint(nWidth - 284,(nHeight - 34)/2);

	//叫分位置
	m_ptCallScore[0].SetPoint((nWidth - 54)/2 ,200);
	m_ptCallScore[1].SetPoint(220,(nHeight - 34)/2);
	m_ptCallScore[2].SetPoint((nWidth - 54)/2  ,nHeight - 260);
	m_ptCallScore[3].SetPoint(nWidth - 284,(nHeight - 34)/2);

	//庄家标志
	m_ptBanker[0].SetPoint(m_ptAvatar[0].x-10, m_ptAvatar[0].y+5);
	m_ptBanker[1].SetPoint(m_ptAvatar[1].x-10, m_ptAvatar[1].y+5);
	m_ptBanker[2].SetPoint(m_ptAvatar[2].x-10, m_ptAvatar[2].y +5);
	m_ptBanker[3].SetPoint(m_ptAvatar[3].x-10, m_ptAvatar[3].y +5);

	m_ptTrustee[0].SetPoint(m_ptAvatar[0].x+70, m_ptAvatar[0].y);
	m_ptTrustee[1].SetPoint(m_ptAvatar[1].x, m_ptAvatar[1].y-60);
	m_ptTrustee[2].SetPoint(m_ptAvatar[2].x, m_ptAvatar[2].y - 60);
	m_ptTrustee[3].SetPoint(m_ptAvatar[3].x, m_ptAvatar[3].y-60);
	
	//报警位置
	m_ptCountWarn[0].SetPoint(m_ptUserFrame[0].x +175, 35);
	m_ptCountWarn[1].SetPoint(25, m_ptUserFrame[1].y + 120);
	m_ptCountWarn[2].SetPoint(m_ptUserFrame[2].x-60-114, nHeight - 123);
	m_ptCountWarn[3].SetPoint(nWidth - 25-114, m_ptUserFrame[3].y + 120);

	//剩余牌位置
	m_ptCardFrame[1].SetPoint(m_ptUserFrame[1].x + 91,m_ptUserFrame[1].y + (m_TextureUserBG.GetHeight() - 116)/2);
	m_ptCardFrame[3].SetPoint(m_ptUserFrame[3].x -69,m_ptUserFrame[3].y + (m_TextureUserBG.GetHeight() - 116)/2);
	m_ptCardFrame[0].SetPoint(m_ptUserFrame[0].x + (m_TextureIcon.GetWidth() - 99)/2 ,m_ptUserFrame[0].y + 83);
	//出牌位置
	m_UserCardControl[0].SetDispatchPos(nWidth/2,50);
	m_UserCardControl[1].SetDispatchPos(115,nHeight/2-95);
	m_UserCardControl[2].SetDispatchPos(nWidth/2,nHeight-50);
	m_UserCardControl[3].SetDispatchPos(nWidth-120,nHeight/2-95);

	//选队友牌控件
	m_SelectFriendCardControl.SetBenchmarkPos(nWidth/2,nHeight-250,enXCenter,enYBottom);
	//用户扑克
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2+20,30,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(95,nHeight/2-50,enXLeft,enYCenter);
	m_HandCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-50,enXCenter,enYBottom);
	m_HandCardControl[3].SetBenchmarkPos(nWidth-105,nHeight/2-50,enXRight,enYCenter);

	//出牌扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2+20,210,enXCenter,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(nWidth*17/80,nHeight/2-50,enXLeft,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-270,enXCenter,enYCenter);
	m_UserCardControl[3].SetBenchmarkPos(nWidth*63/80,nHeight/2-50,enXRight,enYCenter);

	//虚拟按钮
	if (m_bInitD3D==true)
	{
		//查分按钮
		CRect rcScore;
		m_btScore.GetClientRect(rcScore);
		m_btScore.SetWindowPos(nWidth-rcScore.Width()-5,8,0,0,SWP_NOSIZE);

		//开始按钮
		CRect rcStart;
		m_btStart.GetClientRect(rcStart);
		m_btStart.SetWindowPos((nWidth-rcStart.Width())/2,nHeight-rcStart.Height()-195 - 30,0,0,SWP_NOSIZE);

		//游戏按钮
		CRect rcGameButton;
		m_btPassCard.GetClientRect(rcGameButton);

		m_btOutCard.SetWindowPos(nWidth/2+rcGameButton.Width()/2+20, nHeight-rcGameButton.Height()-220 - 15, 0, 0, SWP_NOSIZE);
		m_btOutPrompt.SetWindowPos(nWidth/2-rcGameButton.Width()/2, nHeight-rcGameButton.Height()-220- 15, 0 , 0, SWP_NOSIZE);
		m_btPassCard.SetWindowPos(nWidth/2-rcGameButton.Width()*3/2-20, nHeight-rcGameButton.Height()-220- 15, 0, 0, SWP_NOSIZE);

		m_btXuan.SetWindowPos(nWidth/2-170,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);
		m_btNoXuan.SetWindowPos(nWidth/2+30,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);

		m_btAsk.SetWindowPos(nWidth/2-170,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);
		m_btNoAsk.SetWindowPos(nWidth/2+30,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);

		m_btAdd.SetWindowPos(nWidth/2-170,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);
		m_btNoAdd.SetWindowPos(nWidth/2+30,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);

		m_btRespond.SetWindowPos(nWidth/2-170,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);
		m_btNoRespond.SetWindowPos(nWidth/2+30,nHeight-rcGameButton.Height()-235,0,0,SWP_NOSIZE);

		m_btSelectFriend.SetWindowPos(nWidth/2-45,nHeight-rcGameButton.Height()-195,0,0,SWP_NOSIZE);
		//功能按钮
		CRect rcTrustee;
		m_btTrustee.GetClientRect(rcTrustee);
		m_btSortCard.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*2 -20,0,0,SWP_NOSIZE);
		//m_btLastTurn.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*2 - 20,0,0,SWP_NOSIZE);
		m_btTrustee.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*3 - 30,0,0,SWP_NOSIZE);
		m_btCancelTrustee.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*3 - 30,0,0,SWP_NOSIZE);
		m_btRememberCard.SetWindowPos(10,nHeight - rcTrustee.Height() - 10,0,0,SWP_NOSIZE);

		//成绩窗口
		if (m_ScoreControl.IsWindowActive()==true)
		{
			CRect rcScoreControl;
			m_ScoreControl.GetWindowRect(rcScoreControl);
			m_ScoreControl.SetWindowPos((nWidth-rcScoreControl.Width())/2,(nHeight-rcScoreControl.Height())*2/5 - 17,0,0,SWP_NOSIZE);
		}

		//记牌器窗口
		if(m_RememberCardWnd.IsWindowActive() == true)
		{
			CRect rcWnd;
			m_RememberCardWnd.GetWindowRect(rcWnd);
			m_RememberCardWnd.SetWindowPos(10,nHeight - 50 - rcWnd.Height(),0,0,SWP_NOSIZE);
		}
	}

	return;
}

//动画驱动
VOID CGameClientView::CartoonMovie()
{
	//扑克动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].CartoonMovie();
		m_UserCardControl[i].CartoonMovie();
	}

	//if(m_bShowFindFriend)
		m_SelectFriendCardControl.CartoonMovie();

	//飞机动画
	if ((m_bCartoonPlan==true)&&(m_CartoonPlanLapseCount.GetLapseCount(40)>0L))
	{
		//设置变量
		m_wCartoonPlanIndex++;

		//设置位置	
		m_ptCartoonPlan.x -= 30;

		//停止判断
		if (m_ptCartoonPlan.x < -200 )
		{
			//设置变量
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			m_ptCartoonPlan.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();
		}
	}


	//爆炸动画
	if ((m_bCartoonBomb==true)&&(m_CartoonBombLapseCount.GetLapseCount(150)>0L))
	{
		//设置变量
		m_wCartoonBombIndex++;

		//停止判断
		if (m_wCartoonBombIndex>=BOMB_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
		}
	}

	//顺子动画
	if ((m_bCartoonLine==true)&&(m_CartoonLineLapseCount.GetLapseCount(200)>0L))
	{
		//设置变量
		m_wCartoonLineIndex++;

		//停止判断
		if (m_wCartoonLineIndex >= LINE_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonLine = false;
			m_wCartoonLineIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonLine.IsNull()==false);
			if (m_TextureCartoonLine.IsNull()==false) m_TextureCartoonLine.Destory();
		}
	}
	//连对动画
	if ((m_bCartoonDoubleLine==true)&&(m_CartoonDoubleLineLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wCartoonDoubleLineIndex++;

		//停止判断
		if (m_wCartoonDoubleLineIndex >= DOUBLE_LINE_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonDoubleLine = false;
			m_wCartoonDoubleLineIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonDoubleLine.IsNull()==false);
			if (m_TextureCartoonDoubleLine.IsNull()==false) m_TextureCartoonDoubleLine.Destory();
		}
	}
	//三同张动画
	if ((m_bCartoonThreeLine==true)&&(m_CartoonThreeLineLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wCartoonThreeLineIndex++;

		//停止判断
		if (m_wCartoonThreeLineIndex >= THREE_LINE_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonThreeLine = false;
			m_wCartoonThreeLineIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeLine.IsNull()==false);
			if (m_TextureCartoonThreeLine.IsNull()==false) m_TextureCartoonThreeLine.Destory();
		}
	}
	//三带牌动画
	if ((m_bCartoonThreeTake==true)&&(m_CartoonThreeTakeLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wCartoonThreeTakeIndex++;

		//停止判断
		if (m_wCartoonThreeTakeIndex >= THREE_TAKE_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonThreeTake = false;
			m_wCartoonThreeTakeIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeTake.IsNull()==false);
			if (m_TextureCartoonThreeTake.IsNull()==false) m_TextureCartoonThreeTake.Destory();
		}
	}
	//三连带动画
	if ((m_bCartoonThreeTakeLine==true)&&(m_CartoonThreeTakeLineLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wCartoonThreeTakeLineIndex++;

		//停止判断
		if (m_wCartoonThreeTakeLineIndex >= THREE_TAKE_LINE_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonThreeTakeLine = false;
			m_wCartoonThreeTakeLineIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeTakeLine.IsNull()==false);
			if (m_TextureCartoonThreeTakeLine.IsNull()==false) m_TextureCartoonThreeTakeLine.Destory();
		}
	}
	//洗牌动画
	if ((m_bRiffleCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wDispatchCardIndex++;

		//停止判断
		if (m_wDispatchCardIndex>=(RIFFLE_CARD_COUNT_ALL+6))
		{
			//设置变量
			m_bRiffleCard=false;
			m_bDispatchCard=true;
			m_wDispatchCardIndex=0L;

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}
	}

	//发牌动画
	if ((m_bDispatchCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(10)>0L))
	{
		//设置变量
		m_wDispatchCardIndex++;

		//变量定义
		WORD wBaseIndex=DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES;

		//设置扑克
		if ((m_wDispatchCardIndex>=wBaseIndex)&&((m_wDispatchCardIndex-wBaseIndex)%DISPATCH_CARD_SPACE==0))
		{
			//计算索引
			WORD wCardIndex=m_wDispatchCardIndex-wBaseIndex;
			WORD wUserIndex=(m_wStartUser+wCardIndex/DISPATCH_CARD_SPACE)%GAME_PLAYER;

			//设置扑克
			if (wUserIndex==ME_VIEW_ID)
			{
				//获取扑克
				BYTE cbCardData[MAX_CARD_COUNT];
				BYTE cbCardCount=m_HandCardControl[wUserIndex].GetCardData(cbCardData,CountArray(cbCardData));

				//插入扑克
				cbCardData[cbCardCount]=m_cbHandCardData[cbCardCount++];
				m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

				//设置扑克
				m_HandCardControl[wUserIndex].SetCardData(cbCardData,cbCardCount);
			}
			else
			{
				//玩家扑克
				m_HandCardControl[wUserIndex].SetCardData(m_HandCardControl[wUserIndex].GetCardCount()+1);
			}
		}

		//停止判断
		if (m_wDispatchCardIndex>=DISPATCH_COUNT*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES)
		{
			//设置变量
			m_bDispatchCard=false;
			m_wDispatchCardIndex=0L;

			//卸载资源
			ASSERT(m_TextureDispatchCard.IsNull()==false);
			if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

			//发送消息
			SendEngineMessage(IDM_DISPATCH_FINISH,0L,0L);
		}
	}

	//报警动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_bCountWarn[i]==true)&&(m_CountWarnLapseCount[i].GetLapseCount(180)>0L))
		{
			//设置变量
			m_wCountWarnIndex[i]=(m_wCountWarnIndex[i]+1)%CARD_WARNING_COUNT;
		}
	}

	return;
}

//配置设备
VOID CGameClientView::InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//游戏按钮
	m_btStart.ActiveWindow(rcCreate,0,IDC_BT_START,&m_VirtualEngine,NULL);
	m_btScore.ActiveWindow(rcCreate,0,IDC_BT_SCORE,&m_VirtualEngine,NULL);
	m_btOutCard.ActiveWindow(rcCreate,0,IDC_BT_OUT_CARD,&m_VirtualEngine,NULL);
	m_btPassCard.ActiveWindow(rcCreate,0,IDC_BT_PASS_CARD,&m_VirtualEngine,NULL);
	m_btOutPrompt.ActiveWindow(rcCreate,0,IDC_BT_OUT_PROMPT,&m_VirtualEngine,NULL);

	m_btXuan.ActiveWindow(rcCreate,0,IDC_XUAN,&m_VirtualEngine,NULL);
	m_btNoXuan.ActiveWindow(rcCreate,0,IDC_NO_XUAN,&m_VirtualEngine,NULL);
	m_btAsk.ActiveWindow(rcCreate,0,IDC_ASK,&m_VirtualEngine,NULL);
	m_btNoAsk.ActiveWindow(rcCreate,0,IDC_NO_ASK,&m_VirtualEngine,NULL);
	m_btAdd.ActiveWindow(rcCreate,0,IDC_ADD,&m_VirtualEngine,NULL);
	m_btNoAdd.ActiveWindow(rcCreate,0,IDC_NO_ADD,&m_VirtualEngine,NULL);
	m_btRespond.ActiveWindow(rcCreate,0,IDC_RESPOND,&m_VirtualEngine,NULL);
	m_btNoRespond.ActiveWindow(rcCreate,0,IDC_NO_RESPOND,&m_VirtualEngine,NULL);
	m_btSelectFriend.ActiveWindow(rcCreate,0,IDC_SELECT_FRIEND,&m_VirtualEngine,NULL);
	//功能按钮
	m_btCancelTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_CANCEL_TRUSTEE,&m_VirtualEngine,NULL);
	m_btTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_TRUSTEE,&m_VirtualEngine,NULL);
	m_btLastTurn.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_LAST_TURN,&m_VirtualEngine,NULL);
	m_btSortCard.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_SORT_CARD,&m_VirtualEngine,NULL);
	m_btCancelTrustee.ShowWindow(false);
	m_btRememberCard.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_REMEMBER_CARD,&m_VirtualEngine,NULL);

	//游戏按钮
	m_btStart.SetButtonImage(pD3DDevice,TEXT("BT_START"),TEXT("PNG"),hResInstance);
	m_btScore.SetButtonImage(pD3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btOutCard.SetButtonImage(pD3DDevice,TEXT("BT_OUT_CARD"),TEXT("PNG"),hResInstance);
	m_btPassCard.SetButtonImage(pD3DDevice,TEXT("BT_PASS_CARD"),TEXT("PNG"),hResInstance);
	m_btOutPrompt.SetButtonImage(pD3DDevice,TEXT("BT_OUT_PROMPT"),TEXT("PNG"),hResInstance);

	//功能按钮
	m_btLastTurn.SetButtonImage(pD3DDevice,TEXT("BT_LAST_TURN"),TEXT("PNG"),hResInstance);
	m_btTrustee.SetButtonImage(pD3DDevice,TEXT("BT_START_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btCancelTrustee.SetButtonImage(pD3DDevice,TEXT("BT_STOP_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(pD3DDevice,TEXT("BT_SORT_CARD_COUNT"),TEXT("PNG"),hResInstance);
	m_btRememberCard.SetButtonImage(pD3DDevice,TEXT("BT_REMEMBERCARD"),TEXT("PNG"),hResInstance);


	m_btXuan.SetButtonImage(pD3DDevice,TEXT("BT_XUAN"),TEXT("PNG"),hResInstance);
	m_btNoXuan.SetButtonImage(pD3DDevice,TEXT("BT_NO_XUAN"),TEXT("PNG"),hResInstance);
	m_btAsk.SetButtonImage(pD3DDevice,TEXT("BT_ASK"),TEXT("PNG"),hResInstance);
	m_btNoAsk.SetButtonImage(pD3DDevice,TEXT("BT_NO_ASK"),TEXT("PNG"),hResInstance);
	m_btAdd.SetButtonImage(pD3DDevice,TEXT("BT_ADD"),TEXT("PNG"),hResInstance);
	m_btNoAdd.SetButtonImage(pD3DDevice,TEXT("BT_NO_ADD"),TEXT("PNG"),hResInstance);
	m_btRespond.SetButtonImage(pD3DDevice,TEXT("BT_RESPOND"),TEXT("PNG"),hResInstance);
	m_btNoRespond.SetButtonImage(pD3DDevice,TEXT("BT_NO_RESPOND"),TEXT("PNG"),hResInstance);
	m_btSelectFriend.SetButtonImage(pD3DDevice,TEXT("BT_SELECT_FRIEND"),TEXT("PNG"),hResInstance);
	//扑克资源
	CCardControl::LoadCardResource(pD3DDevice);

	//背景资源
	m_TextureBack.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_BACK"),TEXT("PNG"));
	m_TextureFill.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_FILL"),TEXT("PNG"));
	m_TextureUserBG.LoadImage(pD3DDevice,hResInstance,TEXT("USER_FRAME_PLAYER_V"),TEXT("PNG"));
	m_TextureIcon.LoadImage(pD3DDevice,hResInstance,TEXT("ICON"),TEXT("PNG"));
	m_TextureIconNum.LoadImage(pD3DDevice,hResInstance,TEXT("ICON_NUM"),TEXT("PNG"));
	m_TextureTurnScoreBG.LoadImage(pD3DDevice,hResInstance,TEXT("TURN_SCORE_BG"),TEXT("PNG"));
	m_TextureTurnScoreNum.LoadImage(pD3DDevice,hResInstance,TEXT("TURN_SCORE_NUM"),TEXT("PNG"));
	m_TextureGameInfo.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_INFO_BG"),TEXT("PNG"));
	m_TextureBankerFlag.LoadImage(pD3DDevice,hResInstance,TEXT("BANKER_FLAG"),TEXT("PNG"));
	m_TextureFriendFlag.LoadImage(pD3DDevice,hResInstance,TEXT("FRIEND_FLAG"),TEXT("PNG"));
	m_TextureRemnantCardNumber.LoadImage(pD3DDevice,hResInstance,TEXT("REMNANT_CARD_NUMBER"),TEXT("PNG"));
	m_TextureClockNum.LoadImage(pD3DDevice,hResInstance,TEXT("CLOCK_NUM"),TEXT("PNG"));
	m_TextureClockBack.LoadImage(pD3DDevice,hResInstance,TEXT("CLOCK_BACK"),TEXT("PNG"));
	m_TextureSelectFriendBg.LoadImage(pD3DDevice,hResInstance,TEXT("SELECT_FRIEND_BG"),TEXT("PNG"));
	m_TextureTrustee.LoadImage(pD3DDevice,hResInstance,TEXT("TRUSTEE"),TEXT("PNG"));
	m_TextureAddTimes.LoadImage(pD3DDevice,hResInstance,TEXT("ADDTIMES"),TEXT("PNG"));
	m_TextureNoAddTimes.LoadImage(pD3DDevice,hResInstance,TEXT("NO_ADDTIMES"),TEXT("PNG"));
	m_TextureAsk.LoadImage(pD3DDevice,hResInstance,TEXT("ASK"),TEXT("PNG"));
	m_TextureNoAsk.LoadImage(pD3DDevice,hResInstance,TEXT("NO_ASK"),TEXT("PNG"));
	m_TextureXuanZhan.LoadImage(pD3DDevice,hResInstance,TEXT("XUANZHAN"),TEXT("PNG"));
	m_TextureNoXuanZhan.LoadImage(pD3DDevice,hResInstance,TEXT("NO_XUANZHAN"),TEXT("PNG"));
	m_TextureRespond.LoadImage(pD3DDevice,hResInstance,TEXT("RESPOND"),TEXT("PNG"));
	m_TextureNoRespond.LoadImage(pD3DDevice,hResInstance,TEXT("NO_RESPOND"),TEXT("PNG"));
	m_TextureUserPass.LoadImage(&m_D3DDevice,hResInstance,TEXT("USER_PASS"),TEXT("PNG"));
	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//玩家扑克
		if (0 == i)
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetSmallMode(true);
			m_HandCardControl[i].SetCardDistance(15, 0, 0);
		}
		else if ((1 == i) || (3 == i))
		{
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetSmallMode(true);
			m_HandCardControl[i].SetCardDistance(0 ,DEF_Y_DISTANCE, 0);
		}
		else
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetSmallMode(false);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
			m_HandCardControl[i].SetShowNum(false);
		}

		//出牌扑克
		m_UserCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetCardDistance(DEF_X_DISTANCE,0,0);
		m_UserCardControl[i].SetShowNum(false);
	}

	//选朋友牌控件
	m_SelectFriendCardControl.SetDirection(true);
	m_SelectFriendCardControl.SetSmallMode(false);
	m_SelectFriendCardControl.SetCardDistance(30,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
	m_SelectFriendCardControl.SetShowNum(false);

	m_ScoreControl.ActiveWindow(rcCreate,0,IDC_GAME_SCORE,&m_VirtualEngine,NULL);
	//启动渲染
	StartRenderThread();

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//获取大小
	CSize SizeBack;
	CSize SizeUserUserMySelf;
	SizeBack.SetSize(m_TextureBack.GetWidth(),m_TextureBack.GetHeight());

	//绘画背景
	DrawViewImage(pD3DDevice,m_TextureFill,DRAW_MODE_SPREAD);
	m_TextureBack.DrawImage(pD3DDevice,(nWidth-SizeBack.cx)/2,(nHeight-SizeBack.cy)*2/5,SizeBack.cx,SizeBack.cy,0,0);

	m_TextureGameInfo.DrawImage(pD3DDevice,5,15);
	//游戏信息
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("%d"), m_lCellScore);
	DrawTextString(pD3DDevice,szOutNumT,85,27,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	//房间设置
	_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("%s"), m_b2Biggest?_T("2大"):_T("A大"));
	DrawTextString(pD3DDevice,szOutNumT,85,50,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	//选择的牌
	if(m_cbSelectedCardData != 0)
	{
		BYTE color=(m_cbSelectedCardData&MASK_COLOR)>>4;
		BYTE value=m_cbSelectedCardData&MASK_VALUE;
		_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("庄家选择:%s%s"),szColor[color],szValue[value-1]);
		DrawTextString(pD3DDevice,szOutNumT,15,80,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	}

	//本轮游戏分
	m_TextureTurnScoreBG.DrawImage(pD3DDevice,m_nWidth/2-140,m_nHeight/2-10);
	DrawNumber(pD3DDevice,&m_TextureTurnScoreNum,_T("0123456789"),m_cbTurnScore,m_nWidth/2+80,m_nHeight/2-4,DT_LEFT);
	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_TextureUserBG.DrawImage(pD3DDevice, m_ptUserFrame[i].x+12, m_ptUserFrame[i].y+12);
		m_TextureIcon.DrawImage(pD3DDevice,m_ptUserFrame[i].x,m_ptUserFrame[i].y+68);
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//用户属性
		
		DrawUserAvatar(pD3DDevice,m_ptAvatar[i].x,m_ptAvatar[i].y,pIClientUserItem);

		//位置定义
		CRect rcNickName(m_ptUserFrame[i].x ,m_ptUserFrame[i].y,m_ptUserFrame[i].x+80,m_ptUserFrame[i].y+12);
		CRect rcUserScore(m_ptUserFrame[i].x+25,m_ptUserFrame[i].y+27+43,m_ptUserFrame[i].x+100,m_ptUserFrame[i].y+39+43);
		CRect rcGameScore(m_ptUserFrame[i].x,m_ptUserFrame[i].y+54+35,m_ptUserFrame[i].x+100,m_ptUserFrame[i].y+66+35);
		
		//绘画用户
		LPCTSTR pszNickName=pIClientUserItem->GetNickName();
		DrawTextString(pD3DDevice,pszNickName,rcNickName,DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(153,255,0),D3DCOLOR_XRGB(0,50,0));

		//用户积分
		TCHAR szUserScore[64]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),SCORE_STRING,pIClientUserItem->GetUserScore());
		//DrawTextString(pD3DDevice,szUserScore,rcUserScore,DT_LEFT|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
		DrawNumber(pD3DDevice,&m_TextureIconNum,_T("0123456789"),pIClientUserItem->GetUserScore(),rcUserScore.left,rcUserScore.top,DT_LEFT);
		//绘画当前得分
		TCHAR szGameScore[64]=TEXT("");
		_sntprintf(szGameScore,CountArray(szGameScore),_T("当前得分:%d"),m_cbGameScore[i]);
		DrawTextString(pD3DDevice,szGameScore,rcGameScore,DT_LEFT,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
		
		//庄家标志
		if (m_wBankerUser!=INVALID_CHAIR && i == m_wBankerUser)
		{
			m_TextureBankerFlag.DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y);
		}

		//同盟标识
		if(m_cbFriendFlag[i]>0)
		{
			m_TextureFriendFlag.DrawImage(pD3DDevice,m_ptAvatar[i].x+41,m_ptAvatar[i].y+5,23,39,(m_cbFriendFlag[i]-1)*23,0);
		}
		if(m_cbTrusteeFlag[i]>0)
		{
			m_TextureTrustee.DrawImage(pD3DDevice,m_ptTrustee[i].x,m_ptTrustee[i].y);
		}
	}
	


	if(m_bWaitXuanZhan)
		DrawTextString(pD3DDevice,_T("等待其他玩家宣战"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitFindFriend)
		DrawTextString(pD3DDevice,_T("等待玩家选择队友"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitAskFriend)
		DrawTextString(pD3DDevice,_T("等待玩家询问同盟"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitAddTimes)
		DrawTextString(pD3DDevice,_T("等待玩家加倍"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));

	//绘画扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].DrawCardControl(pD3DDevice);
		m_UserCardControl[i].DrawCardControl(pD3DDevice);
	}
	
	if(m_bShowFindFriend)
	{
		CRect rect;
		m_btSelectFriend.GetWindowRect(rect);
		m_TextureSelectFriendBg.DrawImage(pD3DDevice,rect.left-260,rect.top-160);

		m_SelectFriendCardControl.DrawCardControl(pD3DDevice);
	}
	
	//用户标志
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) 
			continue;
		DrawClock(pD3DDevice,GetUserClock(i),i);
		//同意标志
		BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
		if (cbUserStatus==US_READY)	
			DrawUserReady(pD3DDevice,m_ptReady[i].x,m_ptReady[i].y);

		if (m_bUserPass[i]==true)
		{
			CSize SizeUserPass;
			SizeUserPass.SetSize(m_TextureUserPass.GetWidth(),m_TextureUserPass.GetHeight());
			m_TextureUserPass.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
		}

		if(m_cbXuanZhanFlag[i]>0)
		{
			if(m_cbXuanZhanFlag[i]==1)
				m_TextureXuanZhan.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else
				m_TextureNoXuanZhan.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
		}

		if(m_cbAskFlag[i]>0)
		{
			if(m_cbAskFlag[i]==1)
				m_TextureAsk.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else if(m_cbAskFlag[i]==2)
				m_TextureNoAsk.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else if(m_cbAskFlag[i] == 3)//答应
				m_TextureRespond.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else if(m_cbAskFlag[i] == 4)//不答应
				m_TextureNoRespond.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
		}

		if(m_cbAddTimesFlag[i]>0)
		{
			if(m_cbAddTimesFlag[i]==1)
				m_TextureAddTimes.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else
				m_TextureNoAddTimes.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
		}

	}


	//飞机动画
	if (m_bCartoonPlan==true)
	{
		//获取大小
		CSize SizeCartoonPlan;
		SizeCartoonPlan.SetSize(m_TextureCartoonPlan.GetWidth()/PLAN_CARTOON_COUNT,m_TextureCartoonPlan.GetHeight());

		//绘画效果
		m_TextureCartoonPlan.DrawImage(pD3DDevice,m_ptCartoonPlan.x,m_ptCartoonPlan.y - SizeCartoonPlan.cy/2,SizeCartoonPlan.cx,SizeCartoonPlan.cy,
			SizeCartoonPlan.cx*(m_wCartoonPlanIndex%PLAN_CARTOON_COUNT),0);
	}


	//爆炸动画
	if (m_bCartoonBomb==true)
	{
		//获取大小
		CSize SizeCartoonBomb;
		SizeCartoonBomb.SetSize(m_TextureCartoonBomb.GetWidth()/7,m_TextureCartoonBomb.GetHeight()/2);

		//绘画效果
		INT Ximage = m_wCartoonBombIndex%7;
		INT Yimage = m_wCartoonBombIndex/7;

		m_TextureCartoonBomb.DrawImage(pD3DDevice,(nWidth-SizeCartoonBomb.cx)/2,(nHeight-SizeCartoonBomb.cy)/2,SizeCartoonBomb.cx,SizeCartoonBomb.cy,
			SizeCartoonBomb.cx*Ximage,SizeCartoonBomb.cy*Yimage);
	}

	//顺子动画
	if (m_bCartoonLine==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonLine.GetWidth()/LINE_CARTOON_COUNT, m_TextureCartoonLine.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonLineIndex%LINE_CARTOON_COUNT), 0);
	}
	//连对动画
	if (m_bCartoonDoubleLine==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonDoubleLine.GetWidth()/DOUBLE_LINE_CARTOON_COUNT, m_TextureCartoonDoubleLine.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonDoubleLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonDoubleLineIndex%DOUBLE_LINE_CARTOON_COUNT), 0);
	}
	//连对动画
	if (m_bCartoonDoubleLine==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonDoubleLine.GetWidth()/DOUBLE_LINE_CARTOON_COUNT, m_TextureCartoonDoubleLine.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonDoubleLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonDoubleLineIndex%DOUBLE_LINE_CARTOON_COUNT), 0);
	}
	//三同张动画
	if (m_bCartoonThreeLine==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeLine.GetWidth()/THREE_LINE_CARTOON_COUNT, m_TextureCartoonThreeLine.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeLineIndex%THREE_LINE_CARTOON_COUNT), 0);
	}
	//三带牌动画
	if (m_bCartoonThreeTake==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeTake.GetWidth()/THREE_TAKE_CARTOON_COUNT, m_TextureCartoonThreeTake.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeTake.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeTakeIndex%THREE_TAKE_CARTOON_COUNT), 0);
	}
	//三连带动画
	if (m_bCartoonThreeTakeLine==true)
	{
		//获取大小
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeTakeLine.GetWidth()/THREE_TAKE_LINE_CARTOON_COUNT, m_TextureCartoonThreeTakeLine.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeTakeLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeTakeLineIndex%THREE_TAKE_LINE_CARTOON_COUNT), 0);
	}
	//洗牌动画
	if (m_bRiffleCard==true)
	{
		//洗牌完毕
		if (m_wDispatchCardIndex>=RIFFLE_CARD_COUNT_ALL)
		{
			//获取大小
			CSize SizeCardItem;
			CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

			CD3DTexture * pD3DTexture=NULL;
			CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

			//计算位置
			INT nDistance=__min(DISPATCH_HEAP_DISTANCE-2, m_wDispatchCardIndex-RIFFLE_CARD_COUNT_ALL)+2;
			INT nXSourcePos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*nDistance)/2;

			//显示牌堆
			for (BYTE i=0;i<DISPATCH_COUNT;i++)
			{
				INT nXDrawPos=nXSourcePos+i*nDistance;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx,SizeCardItem.cy*4);
			}
		}
		else
		{
			//获取大小
			CSize SizeDispatchCard;
			SizeDispatchCard.SetSize(m_TextureDispatchCard.GetWidth()/RIFFLE_CARD_COUNT_X,m_TextureDispatchCard.GetHeight()/RIFFLE_CARD_COUNT_Y);

			//绘画效果
			m_TextureDispatchCard.DrawImage(pD3DDevice,(nWidth-SizeDispatchCard.cx)/2,nHeight/2-220,SizeDispatchCard.cx,SizeDispatchCard.cy,
				SizeDispatchCard.cx*(m_wDispatchCardIndex%RIFFLE_CARD_COUNT_X),SizeDispatchCard.cy*(m_wDispatchCardIndex/RIFFLE_CARD_COUNT_X));
		}
	}

	//发牌动画
	if (m_bDispatchCard==true)
	{
		//变量定义
		CSize SizeControl[GAME_PLAYER];
		CPoint PointControl[GAME_PLAYER];

		//获取大小
		CSize SizeCardItem;
		CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

		//获取纹理
		CD3DTexture * pD3DTexture=NULL;
		CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

		//计算位置
		INT nYHeapPos=nHeight/2-200;
		INT nXHeapPos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*DISPATCH_HEAP_DISTANCE)/2;

		//获取位置
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_HandCardControl[i].GetControlSize(SizeControl[i]);
			m_HandCardControl[i].GetOriginPoint(PointControl[i]);
		}

		//绘画牌堆
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((DISPATCH_COUNT-i)*DISPATCH_CARD_SPACE>m_wDispatchCardIndex)
			{
				INT nXDrawPos=nXHeapPos+i*DISPATCH_HEAP_DISTANCE;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,0,SizeCardItem.cy*4);
			}
		}

		//绘画发牌
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((m_wDispatchCardIndex>=(i+1)*DISPATCH_CARD_SPACE)&&(m_wDispatchCardIndex<(i+1)*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES))
			{
				//变量定义
				WORD wSendUser=(m_wStartUser+i)%GAME_PLAYER;
				WORD wSendIndex=m_wDispatchCardIndex-(i+1)*DISPATCH_CARD_SPACE;

				//目标位置
				INT nXTargetPos=PointControl[wSendUser].x+SizeControl[wSendUser].cx/2;
				INT nYTargetPos=PointControl[wSendUser].y+SizeControl[wSendUser].cy/2;

				//起点位置
				INT nYSourcePos=nYHeapPos+SizeCardItem.cy/2;
				INT nXSourcePos=nXHeapPos+(DISPATCH_COUNT-i-1)*DISPATCH_HEAP_DISTANCE+SizeCardItem.cx/2;

				//绘画扑克
				INT nXDrawPos=nXSourcePos+wSendIndex*(nXTargetPos-nXSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cx/2;
				INT nYDrawPos=nYSourcePos+wSendIndex*(nYTargetPos-nYSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cy/2;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nYDrawPos,SizeCardItem.cx,SizeCardItem.cy,0,SizeCardItem.cy*4);
			}
		}
	}

	//报警动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			//获取大小
			CSize SizeCountWarn;
			SizeCountWarn.SetSize(m_TextureCountWarn.GetWidth()/CARD_WARNING_COUNT,m_TextureCountWarn.GetHeight());

			//绘画效果
			m_TextureCountWarn.DrawImage(pD3DDevice,m_ptCountWarn[i].x,m_ptCountWarn[i].y,SizeCountWarn.cx,SizeCountWarn.cy,
				SizeCountWarn.cx*m_wCountWarnIndex[i],0);
		}
	}

	//历史积分
	if (m_bShowScore==true)
	{
		//获取大小
		CSize SizeHistoryScore;
		SizeHistoryScore.SetSize(m_TextureHistoryScore.GetWidth(),m_TextureHistoryScore.GetHeight());

		//积分背景
		m_TextureHistoryScore.DrawImage(pD3DDevice,nWidth-SizeHistoryScore.cx-40,5);

		//计算位置
		INT nYBenchmark=10;
		INT nXBenchmark=nWidth-SizeHistoryScore.cx-40;

		//绘画信息
		for (WORD i=0;i<GAME_PLAYER;i++)
		{			
			//获取用户
			IClientUserItem * pIClientUserItem=GetClientUserItem(i);
			if ((pIClientUserItem==NULL)||(m_pHistoryScore[i]==NULL)) continue;

			//位置计算
			CRect rcAccounts(nXBenchmark+13,nYBenchmark+23+i*18,nXBenchmark+83,nYBenchmark+35+i*18);
			CRect rcTurnScore(nXBenchmark+85,nYBenchmark+23+i*18,nXBenchmark+135,nYBenchmark+35+i*18);
			CRect rcCollectScore(nXBenchmark+139,nYBenchmark+23+i*18,nXBenchmark+185,nYBenchmark+35+i*18);

			//构造信息
			TCHAR szTurnScore[16]=TEXT("");
			TCHAR szCollectScore[16]=TEXT("");
			_sntprintf(szTurnScore,CountArray(szTurnScore),SCORE_STRING,m_pHistoryScore[i]->lTurnScore);
			_sntprintf(szCollectScore,CountArray(szCollectScore),SCORE_STRING,m_pHistoryScore[i]->lCollectScore);

			//绘画信息
			D3DCOLOR crColor1=D3DCOLOR_XRGB(255,153,0);
			D3DCOLOR crColor2=D3DCOLOR_XRGB(0,255,153);
			LPCTSTR pszNickName=pIClientUserItem->GetNickName();
			DrawTextString(pD3DDevice,pszNickName,&rcAccounts,DT_END_ELLIPSIS|DT_LEFT,D3DCOLOR_XRGB(255,255,255));
			DrawTextString(pD3DDevice,szTurnScore,&rcTurnScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lTurnScore>=0)?crColor1:crColor2);
			DrawTextString(pD3DDevice,szCollectScore,&rcCollectScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lCollectScore>=0)?crColor1:crColor2);
		}
	}
	
	return;
}

//单元积分
VOID CGameClientView::SetCellScore(LONG lCellScore)
{
	if (m_lCellScore!=lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		RefreshGameView();
	}

	return;
}

//设置庄家
VOID CGameClientView::SetBankerUser(WORD wBankerUser)
{
	if (m_wBankerUser!=wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		RefreshGameView();
	}

	return;
}

//设置结束
VOID CGameClientView::ShowGameScoreInfo(tagScoreInfo & ScoreInfo)
{
	//设置信息
	m_ScoreControl.SetScoreInfo(ScoreInfo);

	//创建控件
	//if (m_ScoreControl.IsWindowActive()==false)
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);
		
		m_ScoreControl.ShowWindow(true);
		//移动位置
		CRect rcScoreControl;
		m_ScoreControl.GetWindowRect(rcScoreControl);
		m_ScoreControl.SetWindowPos((rcClient.Width()-rcScoreControl.Width())/2,(rcClient.Height()-rcScoreControl.Height())*2/5 - 17,0,0,SWP_NOSIZE);
	}

	return;
}



// 绘画数字
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// 绘画数字
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// 绘画数字
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
	// 加载资源
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

//更新视图
VOID CGameClientView::RefreshGameView()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//飞机动画
VOID CGameClientView::ShowCartoonPlan(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonPlan=true;
		m_wCartoonPlanIndex=0L;

		//启动时间
		m_CartoonPlanLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonPlan.IsNull()==true)
		{
			m_TextureCartoonPlan.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_PLAN"),TEXT("PNG"));
		}

		//窗口大小
		CRect rcClient;
		GetClientRect(&rcClient);

		//设置位置
		m_ptCartoonPlan.SetPoint( rcClient.Width() , rcClient.Height()/2 );
	
		//更新界面
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//停止动画
		if (m_bCartoonPlan==true)
		{
			//设置变量
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			//设置位置
			m_ptCartoonPlan.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}

//爆炸动画
VOID CGameClientView::ShowCartoonBomb(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonBomb=true;
		m_wCartoonBombIndex=0L;

		//启动时间
		m_CartoonBombLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonBomb.IsNull()==true)
		{
			m_TextureCartoonBomb.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_BOMB"),TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonBomb==true)
		{
			//设置变量
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}

//顺子动画
VOID CGameClientView::ShowCartoonLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonLine = true;
		m_wCartoonLineIndex = 0L;

		//启动时间
		m_CartoonLineLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonLine.IsNull()==true)
		{
			m_TextureCartoonLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_LINE"), TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonLine == true)
		{
			//设置变量
			m_bCartoonLine = false;
			m_wCartoonLineIndex = 0L;

			//卸载资源
			ASSERT(m_TextureCartoonLine.IsNull() == false);
			if (m_TextureCartoonLine.IsNull() == false) m_TextureCartoonLine.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}
VOID CGameClientView::ShowCartoonDoubleLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonDoubleLine = true;
		m_wCartoonDoubleLineIndex = 0L;

		//启动时间
		m_CartoonDoubleLineLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonDoubleLine.IsNull()==true)
		{
			m_TextureCartoonDoubleLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_DOUBLE_LINE"), TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonDoubleLine == true)
		{
			//设置变量
			m_bCartoonDoubleLine = false;
			m_wCartoonDoubleLineIndex = 0L;

			//卸载资源
			ASSERT(m_TextureCartoonDoubleLine.IsNull() == false);
			if (m_TextureCartoonDoubleLine.IsNull() == false) m_TextureCartoonDoubleLine.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}


VOID CGameClientView::ShowCartoonThreeLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonThreeLine = true;
		m_wCartoonThreeLineIndex = 0L;

		//启动时间
		m_CartoonThreeLineLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonThreeLine.IsNull()==true)
		{
			m_TextureCartoonThreeLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_LINE"), TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonThreeLine == true)
		{
			//设置变量
			m_bCartoonThreeLine = false;
			m_wCartoonThreeLineIndex = 0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeLine.IsNull() == false);
			if (m_TextureCartoonThreeLine.IsNull() == false) m_TextureCartoonThreeLine.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}

VOID CGameClientView::ShowCartoonThreeTake(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonThreeTake = true;
		m_wCartoonThreeTakeIndex = 0L;

		//启动时间
		m_CartoonThreeTakeLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonThreeTake.IsNull()==true)
		{
			m_TextureCartoonThreeTake.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_TAKE"), TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonThreeTake == true)
		{
			//设置变量
			m_bCartoonThreeTake = false;
			m_wCartoonThreeTakeIndex = 0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeTake.IsNull() == false);
			if (m_TextureCartoonThreeTake.IsNull() == false) m_TextureCartoonThreeTake.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}

VOID CGameClientView::ShowCartoonThreeTakeLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonThreeTakeLine = true;
		m_wCartoonThreeTakeLineIndex = 0L;

		//启动时间
		m_CartoonThreeTakeLineLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonThreeTakeLine.IsNull()==true)
		{
			m_TextureCartoonThreeTakeLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_TAKE_LINE"), TEXT("PNG"));
		}

		//更新界面
		RefreshGameView();
	}
	else
	{
		//停止动画
		if (m_bCartoonThreeTakeLine == true)
		{
			//设置变量
			m_bCartoonThreeTakeLine = false;
			m_wCartoonThreeTakeLineIndex = 0L;

			//卸载资源
			ASSERT(m_TextureCartoonThreeTakeLine.IsNull() == false);
			if (m_TextureCartoonThreeTakeLine.IsNull() == false) m_TextureCartoonThreeTakeLine.Destory();

			//更新界面
			RefreshGameView();
		}
	}

	return;
}

//设置上轮
VOID CGameClientView::SetLastTurnState(bool bLastTurnState)
{

	return;
}

//设置放弃
VOID CGameClientView::SetUserPassState(WORD wChairID, bool bUserPass)
{
	//变量定义
	bool bLoadImage=false;

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		//设置变量
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bUserPass[i]=bUserPass;
		}
	}
	else
	{
		//设置变量
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置警告
VOID CGameClientView::SetUserCountWarn(WORD wChairID, bool bCountWarn)
{
	//变量定义
	bool bLoadImage=false;

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		//设置变量
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bCountWarn[i]=bCountWarn;
		}
	}
	else
	{
		//设置变量
		ASSERT(wChairID<GAME_PLAYER);
		m_bCountWarn[wChairID]=bCountWarn;
	}

	//状态检测
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	//资源管理
	if (bLoadImage==true)
	{
		if (m_TextureCountWarn.IsNull()==true)
		{
			m_TextureCountWarn.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("COUNT_WARN"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureCountWarn.IsNull()==false)
		{
			m_TextureCountWarn.Destory();
		}
	}

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置积分
VOID CGameClientView::SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	m_pHistoryScore[wChairID]=pHistoryScore;

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

VOID CGameClientView::SetXuanZhan(WORD wChairID,bool bXuanZhan)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbXuanZhanFlag,sizeof(m_cbXuanZhanFlag));
	else
	{
		if(bXuanZhan)
			m_cbXuanZhanFlag[wChairID] = 1;
		else
			m_cbXuanZhanFlag[wChairID] = 2;
	}
}

VOID CGameClientView::SetAsk(WORD wChairID,bool bAsk,bool bRespond/*=false*/)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbAskFlag,sizeof(m_cbAskFlag));
	else
	{
		if(bAsk)
		{
			if(!bRespond)
				m_cbAskFlag[wChairID] = 1;
			else
				m_cbAskFlag[wChairID] = 3;//答应
		}
		else
		{
			if(!bRespond)
				m_cbAskFlag[wChairID] = 2;
			else
				m_cbAskFlag[wChairID] = 2;//如果要显示不答应，这里改成4
		}
	}
}

//发牌动画
VOID CGameClientView::StopDispatchCard()
{
	//停止动画
	if ((m_bRiffleCard==true)||(m_bDispatchCard==true))
	{
		//设置变量
		m_bRiffleCard=false;
		m_bDispatchCard=false;
		m_wDispatchCardIndex=0L;

		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i==ME_VIEW_ID)
			{
				m_HandCardControl[i].SetCardData(m_cbHandCardData,CountArray(m_cbHandCardData));
			}
			else
			{
				m_HandCardControl[i].SetCardData(NORMAL_CARD_COUNT);
			}
		}

		//卸载资源
		ASSERT(m_TextureDispatchCard.IsNull()==false);
		if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//发牌动画
VOID CGameClientView::ShowDispatchCard(BYTE cbCardData[], BYTE cbCardCount, WORD wStartUser)
{
	//设置变量
	m_bWaitXuanZhan=false;
	m_bWaitFindFriend=false;
	m_bWaitAskFriend=false;
	m_bWaitAddTimes=false;

	m_bRiffleCard=true;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=RIFFLE_CARD_COUNT_ALL;

	//设置变量
	m_wStartUser=wStartUser;
	CopyMemory(m_cbHandCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(0);
	}

	//启动时间
	m_DispatchCardLapseCount.Initialization();

	//加载资源
	ASSERT(m_TextureDispatchCard.IsNull()==true);
	if (m_TextureDispatchCard.IsNull()==true)
		m_TextureDispatchCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"),TEXT("PNG"));

	//更新界面
	RefreshGameView();

	//播放声音
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"));

	return;
}

//开始按钮
VOID CGameClientView::OnStart()
{
	//发送消息
	SendEngineMessage(IDM_START,0,0);

	return;
}

//积分按钮
VOID CGameClientView::OnLookScore()
{
	//设置变量
	m_bShowScore=!m_bShowScore;

	//加载资源
	if (m_bShowScore==true)
	{
		ASSERT(m_TextureHistoryScore.IsNull()==true);
		m_TextureHistoryScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("HISTORY_SCORE"),TEXT("PNG"));
	}
	else
	{
		ASSERT(m_TextureHistoryScore.IsNull()==false);
		if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();
	}

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,(m_bShowScore==true)?TEXT("BT_CLOSE_SCORE"):TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);

	return;
}

//出牌按钮
VOID CGameClientView::OnOutCard()
{
	//发送消息
	SendEngineMessage(IDM_OUT_CARD,0,0);

	return;
}

//排序按钮
VOID CGameClientView::OnSortCard()
{
	//发送消息
	SendEngineMessage(IDM_SORT_HAND_CARD,0,0);

	return;
}

//上轮按钮
VOID CGameClientView::OnLastTurn()
{
	//发送消息
	SendEngineMessage(IDM_LAST_TURN_CARD,0,0);

	return;
}

//托管按钮
VOID CGameClientView::OnTrustee()
{
	//发送消息
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//托管按钮
VOID CGameClientView::OnCancelTrustee()
{
	//发送消息
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//PASS按钮
VOID CGameClientView::OnPassCard()
{
	//发送消息
	SendEngineMessage(IDM_PASS_CARD,0,0);

	return;
}

//提示按钮
VOID CGameClientView::OnOutPrompt()
{
	//发送消息
	SendEngineMessage(IDM_OUT_PROMPT,0,0);

	return;
}

//叫分按钮
VOID CGameClientView::OnCallScore(UINT nCtrlID)
{
	UINT nCurSel = nCtrlID - IDC_CALL_SCORE_NONE;
	if ((nCurSel >= 1) && (nCurSel <= 3))
	{
		SendEngineMessage(IDM_CALL_SCORE,nCurSel, nCurSel);
	}
	else
	{
		SendEngineMessage(IDM_CALL_SCORE, 0xFF, 0xFF);
	}

}

VOID CGameClientView::OnXuanZhan(UINT nCtrlID)
{
	if (nCtrlID == IDC_XUAN)
	{
		SendEngineMessage(IDM_XUAN_ZHAN,1, 1);
	}
	else
	{
		SendEngineMessage(IDM_XUAN_ZHAN, 0, 0);
	}
}

VOID CGameClientView::OnAskFriend(UINT nCtrlID)
{
	if (nCtrlID == IDC_ASK || nCtrlID == IDC_RESPOND)
	{
		SendEngineMessage(IDM_ASK_FRIEND,1, 1);
	}
	else
	{
		SendEngineMessage(IDM_ASK_FRIEND, 0, 0);
	}
}

VOID CGameClientView::OnAddTimes(UINT nCtrlID)
{
	if (nCtrlID == IDC_ADD)
	{
		SendEngineMessage(IDM_ADD_TIMES,1, 1);
	}
	else
	{
		SendEngineMessage(IDM_ADD_TIMES, 0, 0);
	}
}
//鼠标消息
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//发送出牌
	if ( m_HandCardControl[ME_VIEW_ID].GetPositively() )
	{
		SendEngineMessage(IDM_OUT_CARD,1,1);
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	if(m_bShowFindFriend)
	{
		//m_SelectFriendCardControl.SetShootCard(NULL,0);

		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_SelectFriendCardControl.SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItemEx) pCardItem=m_SelectFriendCardControl.GetCardFromPoint(Point);

		//设置扑克
		if (pCardItem!=NULL)
		{
			//设置扑克
			pCardItem->bShoot=!pCardItem->bShoot;
			
			//更新界面
			RefreshGameView();
		}

		//获取弹起数量扑克
		BYTE cbOldShootCardEx[MAX_CARD_COUNT];
		BYTE count = m_SelectFriendCardControl.GetShootCard(cbOldShootCardEx, MAX_CARD_COUNT);

		m_btSelectFriend.EnableWindow(count == 1);

		//设置变量
		m_bMoveMouseEx=false;
		m_bSwitchCardEx=false;
		m_bSelectCardEx=false;
		m_cbMouseDownItemEx=INVALID_ITEM;
		m_cbHoverCardItemEx=INVALID_ITEM;

		return;
	}

	//获取弹起数量扑克
	BYTE cbOldShootCard[MAX_CARD_COUNT];
	BYTE cbOldShootCrad = m_HandCardControl[ME_VIEW_ID].GetShootCard(cbOldShootCard, MAX_CARD_COUNT);

	//默认处理
	if (m_bMoveMouse==false)
	{
		//获取扑克
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItem) pCardItem=m_HandCardControl[ME_VIEW_ID].GetCardFromPoint(Point);

		//设置扑克
		if (pCardItem!=NULL)
		{
			//设置扑克
			pCardItem->bShoot=!pCardItem->bShoot;

			//发送消息
			SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

			//更新界面
			RefreshGameView();
		}
	}

	//选择处理
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		if (m_cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[ME_VIEW_ID].SetShootArea(m_cbMouseDownItem,m_cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[ME_VIEW_ID].SetShootArea(m_cbHoverCardItem,m_cbMouseDownItem);
		}

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		RefreshGameView();
	}

	//交换处理
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		RefreshGameView();
	}

	//获取扑克
	BYTE cbShootCard[MAX_CARD_COUNT];
	ZeroMemory(cbShootCard, sizeof(cbShootCard));
	BYTE cbShootCrad = m_HandCardControl[ME_VIEW_ID].GetShootCard(cbShootCard, MAX_CARD_COUNT);

	//弹起顺子
	if ( cbOldShootCrad == 1 && cbShootCrad == 2 && !m_bSwitchCard && !m_bMoveMouse &&m_HandCardControl[ME_VIEW_ID].SetShootOrderCard(cbShootCard[0], cbShootCard[1]) )
	{
		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		RefreshGameView();
	}

	//释放鼠标
	ReleaseCapture();


	//设置变量
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//设置索引
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbHoverCardItem=INVALID_ITEM;

	return;
}

//鼠标消息
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
	__super::OnMouseMove(nFlags, MousePoint);

	//变量定义
	BYTE cbHoverCardItem=INVALID_ITEM;

	//移动判断
	if (m_cbHoverCardItem!=INVALID_ITEM)
	{
		//扑克大小
		CSize ControlSize;
		m_HandCardControl[ME_VIEW_ID].GetControlSize(ControlSize);

		//扑克位置
		CPoint OriginPoint;
		m_HandCardControl[ME_VIEW_ID].GetOriginPoint(OriginPoint);

		//横行调整
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//获取索引
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		cbHoverCardItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(MousePoint);

		//移动变量
		if (cbHoverCardItem!=m_cbHoverCardItem) m_bMoveMouse=true;
	}

	//选择扑克
	if ((m_bSelectCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	{
		//设置扑克
		if (cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[ME_VIEW_ID].SetSelectIndex(m_cbMouseDownItem,cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[ME_VIEW_ID].SetSelectIndex(cbHoverCardItem,m_cbMouseDownItem);
		}

		//更新界面
		RefreshGameView();
	}

	//设置变量
	m_cbHoverCardItem=cbHoverCardItem;

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//m_GameLogic.Set2Biggest();
// 	int count=12;
//  	BYTE card2[NORMAL_CARD_COUNT]={0x15,0x15,0x16,0x16,0x17,0x17,0x17,0x28,0x32,0x39,0x34};
//  	BYTE card1[NORMAL_CARD_COUNT]={0x22,0x22,0x32,0x33};
// 	m_GameLogic.SortCardList(card2,11,ST_ORDER);
// 	m_GameLogic.SortCardList(card1,4,ST_ORDER);
// 	BYTE res1=m_GameLogic.GetCardType(card1,count);
// 	BYTE res2=m_GameLogic.GetCardType(card2,count);
// 	bool res3 =m_GameLogic.CompareCard(card1,card2,count,count);

// 	tagSearchCardResult result;
// 	m_GameLogic.SearchOutCard(card2,11,card1,4,&result);

	//状态判断
	if(m_bShowFindFriend==true)
	{
		if (m_SelectFriendCardControl.GetPositively()==true)
		{
			//获取扑克
			m_cbMouseDownItemEx=m_SelectFriendCardControl.SwitchCardPoint(Point);

			//设置扑克
			if (m_cbMouseDownItemEx!=INVALID_ITEM)
			{
				//获取扑克
				tagCardItem * pCardItem=m_SelectFriendCardControl.GetCardFromIndex(m_cbMouseDownItemEx);

				//设置变量
				m_bMoveMouseEx=false;
				m_cbHoverCardItemEx=m_cbMouseDownItemEx;

				//操作变量
				if (pCardItem->bShoot==true)
				{
					//设置变量
					m_bSwitchCard=true;
					m_cbSwitchCardItem=0;

					//设置状态
					for (BYTE i=0;i<m_cbMouseDownItemEx;i++)
					{
						tagCardItem * pCardItemTemp=m_SelectFriendCardControl.GetCardFromIndex(i);
						if ((pCardItemTemp!=NULL)&&(pCardItemTemp->bShoot==true)) m_cbSwitchCardItemEx++;
					}
				}
				else
				{
					m_bSelectCardEx=true;
				}

				//设置鼠标
				SetCapture();
			}
		}

		return;
	}


	if (m_HandCardControl[ME_VIEW_ID].GetPositively()==true)
	{
		//获取扑克
		m_cbMouseDownItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(Point);

		//设置扑克
		if (m_cbMouseDownItem!=INVALID_ITEM)
		{
			//获取扑克
			tagCardItem * pCardItem=m_HandCardControl[ME_VIEW_ID].GetCardFromIndex(m_cbMouseDownItem);

			//设置变量
			m_bMoveMouse=false;
			m_cbHoverCardItem=m_cbMouseDownItem;

			//操作变量
			if (pCardItem->bShoot==true)
			{
				//设置变量
				m_bSwitchCard=true;
				m_cbSwitchCardItem=0;

				//设置状态
				for (BYTE i=0;i<m_cbMouseDownItem;i++)
				{
					tagCardItem * pCardItemTemp=m_HandCardControl[ME_VIEW_ID].GetCardFromIndex(i);
					if ((pCardItemTemp!=NULL)&&(pCardItemTemp->bShoot==true)) m_cbSwitchCardItem++;
				}
			}
			else
			{
				m_bSelectCard=true;
			}

			//设置鼠标
			SetCapture();
		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//获取扑克
	tagCardItem * pCardItem = m_HandCardControl[ME_VIEW_ID].GetCardFromPoint(Point);

	//设置扑克
	if ( pCardItem == NULL )
	{
		//禁止按钮
		m_btOutCard.EnableWindow(FALSE);

		//收起扑克
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//更新界面
		RefreshGameView();
	}
	else if ( m_HandCardControl[ME_VIEW_ID].GetPositively())
	{
		//收起扑克
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//弹起同点
		m_HandCardControl[ME_VIEW_ID].SetShootSameCard(pCardItem->cbCardData);

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		RefreshGameView();
	}

	return;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_HandCardControl[ME_VIEW_ID].OnEventSetCursor(MousePoint)==true)
	{
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//消息命令
BOOL CGameClientView::OnCommand( WPARAM wParam, LPARAM lParam )
{
	return __super::OnCommand(wParam,lParam);
}

//绘画时钟
VOID CGameClientView::DrawClock(CD3DDevice * pD3DDevice,WORD wUserClock,WORD wChair)
{
	if(wUserClock > 0)
	{
		int posx=m_nWidth/2-53;
		int posy=m_nHeight/2-160;
		if(m_pGameClientDlg->GetGameStatus() == GS_PLAY)
			posy += 60;
		int clockW=m_TextureClockBack.GetWidth()/4;
		int clockH=m_TextureClockBack.GetHeight();
		//时钟背景
		if(wChair == 0)
		{
			m_TextureClockBack.DrawImage(pD3DDevice,posx,posy,clockW,clockH,2*clockW,0);
		}
		else if(wChair == 1)
		{
			m_TextureClockBack.DrawImage(pD3DDevice,posx,posy,clockW,clockH,clockW,0);
		}
		else if(wChair == 2)
		{
			m_TextureClockBack.DrawImage(pD3DDevice,posx,posy,clockW,clockH,0,0);
		}
		else if(wChair == 3)
		{
			m_TextureClockBack.DrawImage(pD3DDevice,posx,posy,clockW,clockH,3*clockW,0);
		}
		//数字
		DrawNumber(pD3DDevice,&m_TextureClockNum, TEXT("0123456789"),wUserClock,posx+51,posy+38,DT_CENTER);
	}
}


VOID CGameClientView::SetAddTimes(WORD wChairID,bool bAddTimes)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbAddTimesFlag,sizeof(m_cbAddTimesFlag));
	else
	{
		if(bAddTimes)
			m_cbAddTimesFlag[wChairID] = 1;
		else
			m_cbAddTimesFlag[wChairID] = 2;
	}
}

VOID CGameClientView::SetGameScore(WORD wChairID,int cbScore)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbGameScore,sizeof(m_cbGameScore));
	else
		m_cbGameScore[wChairID]=cbScore;
}

VOID CGameClientView::SetTurnScore(BYTE cbTurnScore)
{
	m_cbTurnScore = cbTurnScore;
}

VOID CGameClientView::SetFriendFlag(WORD wChairID,BYTE cbFlag)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbFriendFlag,sizeof(m_cbFriendFlag));
	else
		m_cbFriendFlag[wChairID] = cbFlag;
}

VOID CGameClientView::SetTrusteeFlag(WORD wChairID,BYTE cbFlag)
{
	if(wChairID == INVALID_CHAIR)
		ZeroMemory(m_cbTrusteeFlag,sizeof(m_cbTrusteeFlag));
	else
		m_cbTrusteeFlag[wChairID] = cbFlag;
}

VOID CGameClientView::ShowFindFriend(bool bShow)
{
	m_bShowFindFriend = bShow;

	m_btSelectFriend.ShowWindow(m_bShowFindFriend);
	m_btSelectFriend.EnableWindow(false);
	BYTE card[12]={0x35,0x25,0x15,0x05,0x3A,0x2A,0x1A,0x0A,0x3D,0x2D,0x1D,0x0D};
	m_SelectFriendCardControl.SetDisplayItem(m_bShowFindFriend);
	m_SelectFriendCardControl.SetPositively(m_bShowFindFriend);
	m_SelectFriendCardControl.SetCardData(card,12);
	//标记自己手中牌
	BYTE cbHandCard[NORMAL_CARD_COUNT]={0};
	m_HandCardControl[ME_VIEW_ID].GetCardData(cbHandCard,NORMAL_CARD_COUNT);
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<NORMAL_CARD_COUNT;j++)
		{
			if(card[i] == cbHandCard[j])
			{
				m_SelectFriendCardControl.SetFlag(i);
				break;
			}
		}
	}

	m_bMoveMouseEx=false;
	m_bSelectCardEx=false;
	m_bSwitchCardEx=false;
	m_cbHoverCardItemEx=INVALID_ITEM;
	m_cbMouseDownItemEx=INVALID_ITEM;
	m_cbSwitchCardItemEx=INVALID_ITEM;
}

VOID CGameClientView::OnSelectFriend()
{
	BYTE cbOldShootCardEx[MAX_CARD_COUNT];
	BYTE count = m_SelectFriendCardControl.GetShootCard(cbOldShootCardEx, MAX_CARD_COUNT);
	SendEngineMessage(IDM_FIND_FRIEND,cbOldShootCardEx[0],cbOldShootCardEx[0]);

	ShowFindFriend(false);
}




//////////////////////////////////////////////////////////////////////////////////
