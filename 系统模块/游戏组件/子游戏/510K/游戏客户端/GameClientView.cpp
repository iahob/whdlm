#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "CardControl.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//��������
#define ROCKET_CARTOON_COUNT		1									//�����Ŀ
#define BOMB_CARTOON_COUNT			14									//��ը��Ŀ
#define LINE_CARTOON_COUNT			5									//˳����Ŀ
#define DOUBLE_LINE_CARTOON_COUNT			12						//������Ŀ
#define THREE_LINE_CARTOON_COUNT			13						//��ͬ����Ŀ
#define THREE_TAKE_CARTOON_COUNT			14						//��������Ŀ
#define THREE_TAKE_LINE_CARTOON_COUNT			16				//��������Ŀ
#define PLAN_CARTOON_COUNT			3									//�ɻ���Ŀ
#define CARD_WARNING_COUNT			2									//������Ŀ

#define REVERSAL_CARD_COUNT			4									//������Ŀ

//ϴ�ƶ���
#define RIFFLE_CARD_COUNT_X			4									//ϴ����Ŀ
#define RIFFLE_CARD_COUNT_Y			6									//ϴ����Ŀ
#define RIFFLE_CARD_COUNT_ALL		RIFFLE_CARD_COUNT_X*RIFFLE_CARD_COUNT_Y

//���ƶ���
#define DISPATCH_CARD_TIMES			6									//���ƴ���
#define DISPATCH_CARD_SPACE			2									//���Ƽ��
#define DISPATCH_HEAP_DISTANCE		3									//�ƶѼ��

//////////////////////////////////////////////////////////////////////////////////

//��ʾ��ť
#define IDC_BT_START				100									//��ʼ��ť
#define IDC_BT_SCORE				101									//��ְ�ť

//�зְ�ť
#define IDC_CALL_SCORE_NONE			200									//�зְ�ť
#define IDC_CALL_SCORE_1			201									//�зְ�ť
#define IDC_CALL_SCORE_2			202									//�зְ�ť
#define IDC_CALL_SCORE_3			203									//�зְ�ť

#define IDC_XUAN						210
#define IDC_NO_XUAN				211
#define IDC_ASK						212
#define IDC_NO_ASK					213
#define IDC_RESPOND				214
#define IDC_NO_RESPOND			215
#define IDC_ADD						216
#define IDC_NO_ADD				217

#define IDC_SELECT_FRIEND		218
//��Ϸ��ť
#define IDC_BT_OUT_CARD				300									//���ư�ť
#define IDC_BT_PASS_CARD			301									//PASS��ť
#define IDC_BT_OUT_PROMPT           302                             	//��ʾ��ť

//���ܰ�ť
#define IDC_BT_TRUSTEE				400									//�йܿ���
#define IDC_BT_CANCEL_TRUSTEE		401									//�йܿ���
#define IDC_BT_SORT_CARD			402									//�˿�����
#define IDC_BT_LAST_TURN			403									//�����˿�
#define IDC_BT_REMEMBER_CARD		404									//������

//�ɼ�����
#define IDC_GAME_SCORE				600									//�ɼ�����

//����������
#define IDC_REMEMBER_CARD_WND		700									//����������
//////////////////////////////////////////////////////////////////////////////////
#define CardW 40
#define CardH 50

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewD3D)

	//ϵͳ��Ϣ
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//���ư�ť
	ON_BN_CLICKED(IDC_BT_START, OnStart)
	ON_BN_CLICKED(IDC_BT_SCORE, OnLookScore)
	ON_BN_CLICKED(IDC_BT_SORT_CARD, OnSortCard)
	ON_BN_CLICKED(IDC_BT_LAST_TURN, OnLastTurn)
	ON_BN_CLICKED(IDC_BT_TRUSTEE, OnTrustee)
	ON_BN_CLICKED(IDC_BT_CANCEL_TRUSTEE, OnCancelTrustee)
	//��Ϸ��ť
	ON_BN_CLICKED(IDC_BT_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_BT_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_BT_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_SELECT_FRIEND, OnSelectFriend)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_XUAN, IDC_NO_XUAN, OnXuanZhan)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ASK, IDC_NO_RESPOND, OnAskFriend)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ADD,IDC_NO_ADD, OnAddTimes)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
TCHAR* szColor[4]={_T("����"),_T("÷��"),_T("����"),_T("����")};
TCHAR* szValue[13]={_T("A"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6"),_T("7"),_T("8"),_T("9"),_T("10"),_T("J"),_T("Q"),_T("K")};
//���캯��
CGameClientView::CGameClientView()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;

	//��ը����
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//�ɻ�����
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//˳�Ӷ���
	m_bCartoonLine=false;
	m_wCartoonLineIndex=0L;



	//���ƶ���
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//���ƶ���
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//�ƶ�����
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
	//��Ϸ����
	m_lCellScore=0L;
	//��������
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//λ�ñ���
	ZeroMemory(&m_ptUserFrame, sizeof(m_ptUserFrame));
	ZeroMemory(&m_ptPass, sizeof(m_ptPass));
	ZeroMemory(&m_ptBanker, sizeof(m_ptBanker));
	ZeroMemory(&m_ptCountWarn, sizeof(m_ptCountWarn));
	ZeroMemory(&m_ptCallScore, sizeof(m_ptCallScore));
	ZeroMemory(&m_ptCardFrame,sizeof(m_ptCardFrame));
	//���ñ���
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//��������
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

//��������
CGameClientView::~CGameClientView()
{
}

//���ý���
VOID CGameClientView::ResetGameView()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;

	//��ը����
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//�ɻ�����
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//˳�Ӷ���
	m_bCartoonLine=false;
	m_wCartoonLineIndex=0L;



	//���ƶ���
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//���ƶ���
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//�ƶ�����
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//��Ϸ����
	m_lCellScore=0L;

	//��������
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//���ñ���
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//��������
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	//���ý���
	m_btScore.ShowWindow(false);
	m_btStart.ShowWindow(false);
	m_btOutCard.ShowWindow(false);
	m_btPassCard.ShowWindow(false);
	m_btOutPrompt.ShowWindow(false);

	//��Ϸ�ؼ�
	m_btTrustee.ShowWindow(true);
	m_btCancelTrustee.ShowWindow(false);
	m_btTrustee.EnableWindow(false);
	m_btLastTurn.EnableWindow(false);
	m_btSortCard.EnableWindow(false);


	//��Ϸ�ؼ�
	m_ScoreControl.CloseControl();
	m_HandCardControl[ME_VIEW_ID].SetPositively(false);
	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(NULL,0);
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);
	}

	//������Դ
	if (m_TextureUserPass.IsNull()==false) m_TextureUserPass.Destory();
	if (m_TextureCountWarn.IsNull()==false) m_TextureCountWarn.Destory();
	if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
	if (m_TextureCartoonLine.IsNull()==false) m_TextureCartoonLine.Destory();
	if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();
	if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();

	//��ťͼƬ
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

//�����ؼ�
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	//�û�����
	m_ptUserFrame[0].SetPoint((nWidth - m_TextureUserBG.GetWidth())/2+250, 5);
	m_ptUserFrame[1].SetPoint(5, (nHeight - m_TextureUserBG.GetHeight())/2);
	m_ptUserFrame[2].SetPoint(5, nHeight-m_TextureUserBG.GetHeight() - 100);
	m_ptUserFrame[3].SetPoint(nWidth-50 - m_TextureUserBG.GetWidth(),(nHeight - m_TextureUserBG.GetHeight())/2);

	//ͷ��λ��	
	m_ptAvatar[0].SetPoint(m_ptUserFrame[0].x + 13, m_ptUserFrame[0].y + 13);
	m_ptAvatar[1].SetPoint(m_ptUserFrame[1].x + 13, m_ptUserFrame[1].y + 13);
	m_ptAvatar[2].SetPoint(m_ptUserFrame[2].x + 13, m_ptUserFrame[2].y + 13);
	m_ptAvatar[3].SetPoint(m_ptUserFrame[3].x + 13, m_ptUserFrame[3].y + 13);

	//�ʺ�λ��
	//m_ptNickName[0].SetPoint(m_ptAvatar[0].x+FACE_CX+33, m_ptAvatar[0].y+7);
	m_ptNickName[0].SetPoint(m_ptAvatar[0].x+11, m_ptAvatar[0].y-20);
	m_ptNickName[1].SetPoint(m_ptAvatar[1].x+11, m_ptAvatar[1].y-20);
	m_ptNickName[2].SetPoint(m_ptAvatar[2].x+11, m_ptAvatar[2].y-20);
	m_ptNickName[3].SetPoint(m_ptAvatar[3].x+11, m_ptAvatar[3].y-20);

	//ʱ��λ��
	m_ptClock[0].SetPoint(m_ptUserFrame[0].x-170, 32);
	m_ptClock[1].SetPoint(15, m_ptUserFrame[1].y - 130);
	m_ptClock[2].SetPoint((nWidth-74)/2, nHeight-355);
	m_ptClock[3].SetPoint(nWidth - 89, m_ptUserFrame[3].y - 130);

	//׼��λ��
	m_ptReady[0].SetPoint((nWidth - 70)/2 +25 ,200);
	m_ptReady[1].SetPoint(220,(nHeight - 30)/2);
	m_ptReady[2].SetPoint((nWidth - 70)/2 +25 ,nHeight - 230);
	m_ptReady[3].SetPoint(nWidth - 270,(nHeight - 30)/2);

	//����λ��
	m_ptPass[0].SetPoint((nWidth - 64)/2 ,172);
	m_ptPass[1].SetPoint(220,(nHeight - 34)/2);
	m_ptPass[2].SetPoint((nWidth - 64)/2-5  ,nHeight - 280);
	m_ptPass[3].SetPoint(nWidth - 284,(nHeight - 34)/2);

	//�з�λ��
	m_ptCallScore[0].SetPoint((nWidth - 54)/2 ,200);
	m_ptCallScore[1].SetPoint(220,(nHeight - 34)/2);
	m_ptCallScore[2].SetPoint((nWidth - 54)/2  ,nHeight - 260);
	m_ptCallScore[3].SetPoint(nWidth - 284,(nHeight - 34)/2);

	//ׯ�ұ�־
	m_ptBanker[0].SetPoint(m_ptAvatar[0].x-10, m_ptAvatar[0].y+5);
	m_ptBanker[1].SetPoint(m_ptAvatar[1].x-10, m_ptAvatar[1].y+5);
	m_ptBanker[2].SetPoint(m_ptAvatar[2].x-10, m_ptAvatar[2].y +5);
	m_ptBanker[3].SetPoint(m_ptAvatar[3].x-10, m_ptAvatar[3].y +5);

	m_ptTrustee[0].SetPoint(m_ptAvatar[0].x+70, m_ptAvatar[0].y);
	m_ptTrustee[1].SetPoint(m_ptAvatar[1].x, m_ptAvatar[1].y-60);
	m_ptTrustee[2].SetPoint(m_ptAvatar[2].x, m_ptAvatar[2].y - 60);
	m_ptTrustee[3].SetPoint(m_ptAvatar[3].x, m_ptAvatar[3].y-60);
	
	//����λ��
	m_ptCountWarn[0].SetPoint(m_ptUserFrame[0].x +175, 35);
	m_ptCountWarn[1].SetPoint(25, m_ptUserFrame[1].y + 120);
	m_ptCountWarn[2].SetPoint(m_ptUserFrame[2].x-60-114, nHeight - 123);
	m_ptCountWarn[3].SetPoint(nWidth - 25-114, m_ptUserFrame[3].y + 120);

	//ʣ����λ��
	m_ptCardFrame[1].SetPoint(m_ptUserFrame[1].x + 91,m_ptUserFrame[1].y + (m_TextureUserBG.GetHeight() - 116)/2);
	m_ptCardFrame[3].SetPoint(m_ptUserFrame[3].x -69,m_ptUserFrame[3].y + (m_TextureUserBG.GetHeight() - 116)/2);
	m_ptCardFrame[0].SetPoint(m_ptUserFrame[0].x + (m_TextureIcon.GetWidth() - 99)/2 ,m_ptUserFrame[0].y + 83);
	//����λ��
	m_UserCardControl[0].SetDispatchPos(nWidth/2,50);
	m_UserCardControl[1].SetDispatchPos(115,nHeight/2-95);
	m_UserCardControl[2].SetDispatchPos(nWidth/2,nHeight-50);
	m_UserCardControl[3].SetDispatchPos(nWidth-120,nHeight/2-95);

	//ѡ�����ƿؼ�
	m_SelectFriendCardControl.SetBenchmarkPos(nWidth/2,nHeight-250,enXCenter,enYBottom);
	//�û��˿�
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2+20,30,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(95,nHeight/2-50,enXLeft,enYCenter);
	m_HandCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-50,enXCenter,enYBottom);
	m_HandCardControl[3].SetBenchmarkPos(nWidth-105,nHeight/2-50,enXRight,enYCenter);

	//�����˿�
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2+20,210,enXCenter,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(nWidth*17/80,nHeight/2-50,enXLeft,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-270,enXCenter,enYCenter);
	m_UserCardControl[3].SetBenchmarkPos(nWidth*63/80,nHeight/2-50,enXRight,enYCenter);

	//���ⰴť
	if (m_bInitD3D==true)
	{
		//��ְ�ť
		CRect rcScore;
		m_btScore.GetClientRect(rcScore);
		m_btScore.SetWindowPos(nWidth-rcScore.Width()-5,8,0,0,SWP_NOSIZE);

		//��ʼ��ť
		CRect rcStart;
		m_btStart.GetClientRect(rcStart);
		m_btStart.SetWindowPos((nWidth-rcStart.Width())/2,nHeight-rcStart.Height()-195 - 30,0,0,SWP_NOSIZE);

		//��Ϸ��ť
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
		//���ܰ�ť
		CRect rcTrustee;
		m_btTrustee.GetClientRect(rcTrustee);
		m_btSortCard.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*2 -20,0,0,SWP_NOSIZE);
		//m_btLastTurn.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*2 - 20,0,0,SWP_NOSIZE);
		m_btTrustee.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*3 - 30,0,0,SWP_NOSIZE);
		m_btCancelTrustee.SetWindowPos(nWidth - 10 - rcTrustee.Width(),nHeight-rcTrustee.Height()*3 - 30,0,0,SWP_NOSIZE);
		m_btRememberCard.SetWindowPos(10,nHeight - rcTrustee.Height() - 10,0,0,SWP_NOSIZE);

		//�ɼ�����
		if (m_ScoreControl.IsWindowActive()==true)
		{
			CRect rcScoreControl;
			m_ScoreControl.GetWindowRect(rcScoreControl);
			m_ScoreControl.SetWindowPos((nWidth-rcScoreControl.Width())/2,(nHeight-rcScoreControl.Height())*2/5 - 17,0,0,SWP_NOSIZE);
		}

		//����������
		if(m_RememberCardWnd.IsWindowActive() == true)
		{
			CRect rcWnd;
			m_RememberCardWnd.GetWindowRect(rcWnd);
			m_RememberCardWnd.SetWindowPos(10,nHeight - 50 - rcWnd.Height(),0,0,SWP_NOSIZE);
		}
	}

	return;
}

//��������
VOID CGameClientView::CartoonMovie()
{
	//�˿˶���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].CartoonMovie();
		m_UserCardControl[i].CartoonMovie();
	}

	//if(m_bShowFindFriend)
		m_SelectFriendCardControl.CartoonMovie();

	//�ɻ�����
	if ((m_bCartoonPlan==true)&&(m_CartoonPlanLapseCount.GetLapseCount(40)>0L))
	{
		//���ñ���
		m_wCartoonPlanIndex++;

		//����λ��	
		m_ptCartoonPlan.x -= 30;

		//ֹͣ�ж�
		if (m_ptCartoonPlan.x < -200 )
		{
			//���ñ���
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			m_ptCartoonPlan.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();
		}
	}


	//��ը����
	if ((m_bCartoonBomb==true)&&(m_CartoonBombLapseCount.GetLapseCount(150)>0L))
	{
		//���ñ���
		m_wCartoonBombIndex++;

		//ֹͣ�ж�
		if (m_wCartoonBombIndex>=BOMB_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
		}
	}

	//˳�Ӷ���
	if ((m_bCartoonLine==true)&&(m_CartoonLineLapseCount.GetLapseCount(200)>0L))
	{
		//���ñ���
		m_wCartoonLineIndex++;

		//ֹͣ�ж�
		if (m_wCartoonLineIndex >= LINE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonLine = false;
			m_wCartoonLineIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonLine.IsNull()==false);
			if (m_TextureCartoonLine.IsNull()==false) m_TextureCartoonLine.Destory();
		}
	}
	//���Զ���
	if ((m_bCartoonDoubleLine==true)&&(m_CartoonDoubleLineLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wCartoonDoubleLineIndex++;

		//ֹͣ�ж�
		if (m_wCartoonDoubleLineIndex >= DOUBLE_LINE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonDoubleLine = false;
			m_wCartoonDoubleLineIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonDoubleLine.IsNull()==false);
			if (m_TextureCartoonDoubleLine.IsNull()==false) m_TextureCartoonDoubleLine.Destory();
		}
	}
	//��ͬ�Ŷ���
	if ((m_bCartoonThreeLine==true)&&(m_CartoonThreeLineLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wCartoonThreeLineIndex++;

		//ֹͣ�ж�
		if (m_wCartoonThreeLineIndex >= THREE_LINE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonThreeLine = false;
			m_wCartoonThreeLineIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeLine.IsNull()==false);
			if (m_TextureCartoonThreeLine.IsNull()==false) m_TextureCartoonThreeLine.Destory();
		}
	}
	//�����ƶ���
	if ((m_bCartoonThreeTake==true)&&(m_CartoonThreeTakeLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wCartoonThreeTakeIndex++;

		//ֹͣ�ж�
		if (m_wCartoonThreeTakeIndex >= THREE_TAKE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonThreeTake = false;
			m_wCartoonThreeTakeIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeTake.IsNull()==false);
			if (m_TextureCartoonThreeTake.IsNull()==false) m_TextureCartoonThreeTake.Destory();
		}
	}
	//����������
	if ((m_bCartoonThreeTakeLine==true)&&(m_CartoonThreeTakeLineLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wCartoonThreeTakeLineIndex++;

		//ֹͣ�ж�
		if (m_wCartoonThreeTakeLineIndex >= THREE_TAKE_LINE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonThreeTakeLine = false;
			m_wCartoonThreeTakeLineIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeTakeLine.IsNull()==false);
			if (m_TextureCartoonThreeTakeLine.IsNull()==false) m_TextureCartoonThreeTakeLine.Destory();
		}
	}
	//ϴ�ƶ���
	if ((m_bRiffleCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wDispatchCardIndex++;

		//ֹͣ�ж�
		if (m_wDispatchCardIndex>=(RIFFLE_CARD_COUNT_ALL+6))
		{
			//���ñ���
			m_bRiffleCard=false;
			m_bDispatchCard=true;
			m_wDispatchCardIndex=0L;

			//��������
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}
	}

	//���ƶ���
	if ((m_bDispatchCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(10)>0L))
	{
		//���ñ���
		m_wDispatchCardIndex++;

		//��������
		WORD wBaseIndex=DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES;

		//�����˿�
		if ((m_wDispatchCardIndex>=wBaseIndex)&&((m_wDispatchCardIndex-wBaseIndex)%DISPATCH_CARD_SPACE==0))
		{
			//��������
			WORD wCardIndex=m_wDispatchCardIndex-wBaseIndex;
			WORD wUserIndex=(m_wStartUser+wCardIndex/DISPATCH_CARD_SPACE)%GAME_PLAYER;

			//�����˿�
			if (wUserIndex==ME_VIEW_ID)
			{
				//��ȡ�˿�
				BYTE cbCardData[MAX_CARD_COUNT];
				BYTE cbCardCount=m_HandCardControl[wUserIndex].GetCardData(cbCardData,CountArray(cbCardData));

				//�����˿�
				cbCardData[cbCardCount]=m_cbHandCardData[cbCardCount++];
				m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

				//�����˿�
				m_HandCardControl[wUserIndex].SetCardData(cbCardData,cbCardCount);
			}
			else
			{
				//����˿�
				m_HandCardControl[wUserIndex].SetCardData(m_HandCardControl[wUserIndex].GetCardCount()+1);
			}
		}

		//ֹͣ�ж�
		if (m_wDispatchCardIndex>=DISPATCH_COUNT*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES)
		{
			//���ñ���
			m_bDispatchCard=false;
			m_wDispatchCardIndex=0L;

			//ж����Դ
			ASSERT(m_TextureDispatchCard.IsNull()==false);
			if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

			//������Ϣ
			SendEngineMessage(IDM_DISPATCH_FINISH,0L,0L);
		}
	}

	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_bCountWarn[i]==true)&&(m_CountWarnLapseCount[i].GetLapseCount(180)>0L))
		{
			//���ñ���
			m_wCountWarnIndex[i]=(m_wCountWarnIndex[i]+1)%CARD_WARNING_COUNT;
		}
	}

	return;
}

//�����豸
VOID CGameClientView::InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//��������
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//��Ϸ��ť
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
	//���ܰ�ť
	m_btCancelTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_CANCEL_TRUSTEE,&m_VirtualEngine,NULL);
	m_btTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_TRUSTEE,&m_VirtualEngine,NULL);
	m_btLastTurn.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_LAST_TURN,&m_VirtualEngine,NULL);
	m_btSortCard.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_SORT_CARD,&m_VirtualEngine,NULL);
	m_btCancelTrustee.ShowWindow(false);
	m_btRememberCard.ActiveWindow(rcCreate,WS_DISABLED,IDC_BT_REMEMBER_CARD,&m_VirtualEngine,NULL);

	//��Ϸ��ť
	m_btStart.SetButtonImage(pD3DDevice,TEXT("BT_START"),TEXT("PNG"),hResInstance);
	m_btScore.SetButtonImage(pD3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btOutCard.SetButtonImage(pD3DDevice,TEXT("BT_OUT_CARD"),TEXT("PNG"),hResInstance);
	m_btPassCard.SetButtonImage(pD3DDevice,TEXT("BT_PASS_CARD"),TEXT("PNG"),hResInstance);
	m_btOutPrompt.SetButtonImage(pD3DDevice,TEXT("BT_OUT_PROMPT"),TEXT("PNG"),hResInstance);

	//���ܰ�ť
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
	//�˿���Դ
	CCardControl::LoadCardResource(pD3DDevice);

	//������Դ
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
	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//����˿�
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

		//�����˿�
		m_UserCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetCardDistance(DEF_X_DISTANCE,0,0);
		m_UserCardControl[i].SetShowNum(false);
	}

	//ѡ�����ƿؼ�
	m_SelectFriendCardControl.SetDirection(true);
	m_SelectFriendCardControl.SetSmallMode(false);
	m_SelectFriendCardControl.SetCardDistance(30,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
	m_SelectFriendCardControl.SetShowNum(false);

	m_ScoreControl.ActiveWindow(rcCreate,0,IDC_GAME_SCORE,&m_VirtualEngine,NULL);
	//������Ⱦ
	StartRenderThread();

	return;
}

//�滭����
VOID CGameClientView::DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//��ȡ��С
	CSize SizeBack;
	CSize SizeUserUserMySelf;
	SizeBack.SetSize(m_TextureBack.GetWidth(),m_TextureBack.GetHeight());

	//�滭����
	DrawViewImage(pD3DDevice,m_TextureFill,DRAW_MODE_SPREAD);
	m_TextureBack.DrawImage(pD3DDevice,(nWidth-SizeBack.cx)/2,(nHeight-SizeBack.cy)*2/5,SizeBack.cx,SizeBack.cy,0,0);

	m_TextureGameInfo.DrawImage(pD3DDevice,5,15);
	//��Ϸ��Ϣ
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("%d"), m_lCellScore);
	DrawTextString(pD3DDevice,szOutNumT,85,27,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	//��������
	_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("%s"), m_b2Biggest?_T("2��"):_T("A��"));
	DrawTextString(pD3DDevice,szOutNumT,85,50,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	//ѡ�����
	if(m_cbSelectedCardData != 0)
	{
		BYTE color=(m_cbSelectedCardData&MASK_COLOR)>>4;
		BYTE value=m_cbSelectedCardData&MASK_VALUE;
		_sntprintf(szOutNumT, CountArray(szOutNumT), TEXT("ׯ��ѡ��:%s%s"),szColor[color],szValue[value-1]);
		DrawTextString(pD3DDevice,szOutNumT,15,80,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));
	}

	//������Ϸ��
	m_TextureTurnScoreBG.DrawImage(pD3DDevice,m_nWidth/2-140,m_nHeight/2-10);
	DrawNumber(pD3DDevice,&m_TextureTurnScoreNum,_T("0123456789"),m_cbTurnScore,m_nWidth/2+80,m_nHeight/2-4,DT_LEFT);
	//�滭�û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_TextureUserBG.DrawImage(pD3DDevice, m_ptUserFrame[i].x+12, m_ptUserFrame[i].y+12);
		m_TextureIcon.DrawImage(pD3DDevice,m_ptUserFrame[i].x,m_ptUserFrame[i].y+68);
		//��ȡ�û�
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//�û�����
		
		DrawUserAvatar(pD3DDevice,m_ptAvatar[i].x,m_ptAvatar[i].y,pIClientUserItem);

		//λ�ö���
		CRect rcNickName(m_ptUserFrame[i].x ,m_ptUserFrame[i].y,m_ptUserFrame[i].x+80,m_ptUserFrame[i].y+12);
		CRect rcUserScore(m_ptUserFrame[i].x+25,m_ptUserFrame[i].y+27+43,m_ptUserFrame[i].x+100,m_ptUserFrame[i].y+39+43);
		CRect rcGameScore(m_ptUserFrame[i].x,m_ptUserFrame[i].y+54+35,m_ptUserFrame[i].x+100,m_ptUserFrame[i].y+66+35);
		
		//�滭�û�
		LPCTSTR pszNickName=pIClientUserItem->GetNickName();
		DrawTextString(pD3DDevice,pszNickName,rcNickName,DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(153,255,0),D3DCOLOR_XRGB(0,50,0));

		//�û�����
		TCHAR szUserScore[64]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),SCORE_STRING,pIClientUserItem->GetUserScore());
		//DrawTextString(pD3DDevice,szUserScore,rcUserScore,DT_LEFT|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
		DrawNumber(pD3DDevice,&m_TextureIconNum,_T("0123456789"),pIClientUserItem->GetUserScore(),rcUserScore.left,rcUserScore.top,DT_LEFT);
		//�滭��ǰ�÷�
		TCHAR szGameScore[64]=TEXT("");
		_sntprintf(szGameScore,CountArray(szGameScore),_T("��ǰ�÷�:%d"),m_cbGameScore[i]);
		DrawTextString(pD3DDevice,szGameScore,rcGameScore,DT_LEFT,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
		
		//ׯ�ұ�־
		if (m_wBankerUser!=INVALID_CHAIR && i == m_wBankerUser)
		{
			m_TextureBankerFlag.DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y);
		}

		//ͬ�˱�ʶ
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
		DrawTextString(pD3DDevice,_T("�ȴ����������ս"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitFindFriend)
		DrawTextString(pD3DDevice,_T("�ȴ����ѡ�����"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitAskFriend)
		DrawTextString(pD3DDevice,_T("�ȴ����ѯ��ͬ��"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
	else if(m_bWaitAddTimes)
		DrawTextString(pD3DDevice,_T("�ȴ���Ҽӱ�"),nWidth/2,nHeight/2+30,DT_CENTER,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));

	//�滭�˿�
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
	
	//�û���־
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) 
			continue;
		DrawClock(pD3DDevice,GetUserClock(i),i);
		//ͬ���־
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
			else if(m_cbAskFlag[i] == 3)//��Ӧ
				m_TextureRespond.DrawImage(pD3DDevice,m_ptPass[i].x,m_ptPass[i].y);
			else if(m_cbAskFlag[i] == 4)//����Ӧ
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


	//�ɻ�����
	if (m_bCartoonPlan==true)
	{
		//��ȡ��С
		CSize SizeCartoonPlan;
		SizeCartoonPlan.SetSize(m_TextureCartoonPlan.GetWidth()/PLAN_CARTOON_COUNT,m_TextureCartoonPlan.GetHeight());

		//�滭Ч��
		m_TextureCartoonPlan.DrawImage(pD3DDevice,m_ptCartoonPlan.x,m_ptCartoonPlan.y - SizeCartoonPlan.cy/2,SizeCartoonPlan.cx,SizeCartoonPlan.cy,
			SizeCartoonPlan.cx*(m_wCartoonPlanIndex%PLAN_CARTOON_COUNT),0);
	}


	//��ը����
	if (m_bCartoonBomb==true)
	{
		//��ȡ��С
		CSize SizeCartoonBomb;
		SizeCartoonBomb.SetSize(m_TextureCartoonBomb.GetWidth()/7,m_TextureCartoonBomb.GetHeight()/2);

		//�滭Ч��
		INT Ximage = m_wCartoonBombIndex%7;
		INT Yimage = m_wCartoonBombIndex/7;

		m_TextureCartoonBomb.DrawImage(pD3DDevice,(nWidth-SizeCartoonBomb.cx)/2,(nHeight-SizeCartoonBomb.cy)/2,SizeCartoonBomb.cx,SizeCartoonBomb.cy,
			SizeCartoonBomb.cx*Ximage,SizeCartoonBomb.cy*Yimage);
	}

	//˳�Ӷ���
	if (m_bCartoonLine==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonLine.GetWidth()/LINE_CARTOON_COUNT, m_TextureCartoonLine.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonLineIndex%LINE_CARTOON_COUNT), 0);
	}
	//���Զ���
	if (m_bCartoonDoubleLine==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonDoubleLine.GetWidth()/DOUBLE_LINE_CARTOON_COUNT, m_TextureCartoonDoubleLine.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonDoubleLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonDoubleLineIndex%DOUBLE_LINE_CARTOON_COUNT), 0);
	}
	//���Զ���
	if (m_bCartoonDoubleLine==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonDoubleLine.GetWidth()/DOUBLE_LINE_CARTOON_COUNT, m_TextureCartoonDoubleLine.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonDoubleLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonDoubleLineIndex%DOUBLE_LINE_CARTOON_COUNT), 0);
	}
	//��ͬ�Ŷ���
	if (m_bCartoonThreeLine==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeLine.GetWidth()/THREE_LINE_CARTOON_COUNT, m_TextureCartoonThreeLine.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeLineIndex%THREE_LINE_CARTOON_COUNT), 0);
	}
	//�����ƶ���
	if (m_bCartoonThreeTake==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeTake.GetWidth()/THREE_TAKE_CARTOON_COUNT, m_TextureCartoonThreeTake.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeTake.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeTakeIndex%THREE_TAKE_CARTOON_COUNT), 0);
	}
	//����������
	if (m_bCartoonThreeTakeLine==true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonThreeTakeLine.GetWidth()/THREE_TAKE_LINE_CARTOON_COUNT, m_TextureCartoonThreeTakeLine.GetHeight());

		//�滭Ч��
		INT nYPosScreen=__max(nHeight-280-SizeCartoonLine.cy, 0);
		m_TextureCartoonThreeTakeLine.DrawImage(pD3DDevice, (nWidth-SizeCartoonLine.cx)/2, nYPosScreen, SizeCartoonLine.cx, SizeCartoonLine.cy,
			SizeCartoonLine.cx*(m_wCartoonThreeTakeLineIndex%THREE_TAKE_LINE_CARTOON_COUNT), 0);
	}
	//ϴ�ƶ���
	if (m_bRiffleCard==true)
	{
		//ϴ�����
		if (m_wDispatchCardIndex>=RIFFLE_CARD_COUNT_ALL)
		{
			//��ȡ��С
			CSize SizeCardItem;
			CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

			CD3DTexture * pD3DTexture=NULL;
			CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

			//����λ��
			INT nDistance=__min(DISPATCH_HEAP_DISTANCE-2, m_wDispatchCardIndex-RIFFLE_CARD_COUNT_ALL)+2;
			INT nXSourcePos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*nDistance)/2;

			//��ʾ�ƶ�
			for (BYTE i=0;i<DISPATCH_COUNT;i++)
			{
				INT nXDrawPos=nXSourcePos+i*nDistance;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx,SizeCardItem.cy*4);
			}
		}
		else
		{
			//��ȡ��С
			CSize SizeDispatchCard;
			SizeDispatchCard.SetSize(m_TextureDispatchCard.GetWidth()/RIFFLE_CARD_COUNT_X,m_TextureDispatchCard.GetHeight()/RIFFLE_CARD_COUNT_Y);

			//�滭Ч��
			m_TextureDispatchCard.DrawImage(pD3DDevice,(nWidth-SizeDispatchCard.cx)/2,nHeight/2-220,SizeDispatchCard.cx,SizeDispatchCard.cy,
				SizeDispatchCard.cx*(m_wDispatchCardIndex%RIFFLE_CARD_COUNT_X),SizeDispatchCard.cy*(m_wDispatchCardIndex/RIFFLE_CARD_COUNT_X));
		}
	}

	//���ƶ���
	if (m_bDispatchCard==true)
	{
		//��������
		CSize SizeControl[GAME_PLAYER];
		CPoint PointControl[GAME_PLAYER];

		//��ȡ��С
		CSize SizeCardItem;
		CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

		//��ȡ����
		CD3DTexture * pD3DTexture=NULL;
		CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

		//����λ��
		INT nYHeapPos=nHeight/2-200;
		INT nXHeapPos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*DISPATCH_HEAP_DISTANCE)/2;

		//��ȡλ��
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_HandCardControl[i].GetControlSize(SizeControl[i]);
			m_HandCardControl[i].GetOriginPoint(PointControl[i]);
		}

		//�滭�ƶ�
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((DISPATCH_COUNT-i)*DISPATCH_CARD_SPACE>m_wDispatchCardIndex)
			{
				INT nXDrawPos=nXHeapPos+i*DISPATCH_HEAP_DISTANCE;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,0,SizeCardItem.cy*4);
			}
		}

		//�滭����
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((m_wDispatchCardIndex>=(i+1)*DISPATCH_CARD_SPACE)&&(m_wDispatchCardIndex<(i+1)*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES))
			{
				//��������
				WORD wSendUser=(m_wStartUser+i)%GAME_PLAYER;
				WORD wSendIndex=m_wDispatchCardIndex-(i+1)*DISPATCH_CARD_SPACE;

				//Ŀ��λ��
				INT nXTargetPos=PointControl[wSendUser].x+SizeControl[wSendUser].cx/2;
				INT nYTargetPos=PointControl[wSendUser].y+SizeControl[wSendUser].cy/2;

				//���λ��
				INT nYSourcePos=nYHeapPos+SizeCardItem.cy/2;
				INT nXSourcePos=nXHeapPos+(DISPATCH_COUNT-i-1)*DISPATCH_HEAP_DISTANCE+SizeCardItem.cx/2;

				//�滭�˿�
				INT nXDrawPos=nXSourcePos+wSendIndex*(nXTargetPos-nXSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cx/2;
				INT nYDrawPos=nYSourcePos+wSendIndex*(nYTargetPos-nYSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cy/2;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nYDrawPos,SizeCardItem.cx,SizeCardItem.cy,0,SizeCardItem.cy*4);
			}
		}
	}

	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			//��ȡ��С
			CSize SizeCountWarn;
			SizeCountWarn.SetSize(m_TextureCountWarn.GetWidth()/CARD_WARNING_COUNT,m_TextureCountWarn.GetHeight());

			//�滭Ч��
			m_TextureCountWarn.DrawImage(pD3DDevice,m_ptCountWarn[i].x,m_ptCountWarn[i].y,SizeCountWarn.cx,SizeCountWarn.cy,
				SizeCountWarn.cx*m_wCountWarnIndex[i],0);
		}
	}

	//��ʷ����
	if (m_bShowScore==true)
	{
		//��ȡ��С
		CSize SizeHistoryScore;
		SizeHistoryScore.SetSize(m_TextureHistoryScore.GetWidth(),m_TextureHistoryScore.GetHeight());

		//���ֱ���
		m_TextureHistoryScore.DrawImage(pD3DDevice,nWidth-SizeHistoryScore.cx-40,5);

		//����λ��
		INT nYBenchmark=10;
		INT nXBenchmark=nWidth-SizeHistoryScore.cx-40;

		//�滭��Ϣ
		for (WORD i=0;i<GAME_PLAYER;i++)
		{			
			//��ȡ�û�
			IClientUserItem * pIClientUserItem=GetClientUserItem(i);
			if ((pIClientUserItem==NULL)||(m_pHistoryScore[i]==NULL)) continue;

			//λ�ü���
			CRect rcAccounts(nXBenchmark+13,nYBenchmark+23+i*18,nXBenchmark+83,nYBenchmark+35+i*18);
			CRect rcTurnScore(nXBenchmark+85,nYBenchmark+23+i*18,nXBenchmark+135,nYBenchmark+35+i*18);
			CRect rcCollectScore(nXBenchmark+139,nYBenchmark+23+i*18,nXBenchmark+185,nYBenchmark+35+i*18);

			//������Ϣ
			TCHAR szTurnScore[16]=TEXT("");
			TCHAR szCollectScore[16]=TEXT("");
			_sntprintf(szTurnScore,CountArray(szTurnScore),SCORE_STRING,m_pHistoryScore[i]->lTurnScore);
			_sntprintf(szCollectScore,CountArray(szCollectScore),SCORE_STRING,m_pHistoryScore[i]->lCollectScore);

			//�滭��Ϣ
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

//��Ԫ����
VOID CGameClientView::SetCellScore(LONG lCellScore)
{
	if (m_lCellScore!=lCellScore)
	{
		//���ñ���
		m_lCellScore=lCellScore;

		//���½���
		RefreshGameView();
	}

	return;
}

//����ׯ��
VOID CGameClientView::SetBankerUser(WORD wBankerUser)
{
	if (m_wBankerUser!=wBankerUser)
	{
		//���ñ���
		m_wBankerUser=wBankerUser;

		//���½���
		RefreshGameView();
	}

	return;
}

//���ý���
VOID CGameClientView::ShowGameScoreInfo(tagScoreInfo & ScoreInfo)
{
	//������Ϣ
	m_ScoreControl.SetScoreInfo(ScoreInfo);

	//�����ؼ�
	//if (m_ScoreControl.IsWindowActive()==false)
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);
		
		m_ScoreControl.ShowWindow(true);
		//�ƶ�λ��
		CRect rcScoreControl;
		m_ScoreControl.GetWindowRect(rcScoreControl);
		m_ScoreControl.SetWindowPos((rcClient.Width()-rcScoreControl.Width())/2,(rcClient.Height()-rcScoreControl.Height())*2/5 - 17,0,0,SWP_NOSIZE);
	}

	return;
}



// �滭����
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// �滭����
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// �滭����
VOID CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
	// ������Դ
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

//������ͼ
VOID CGameClientView::RefreshGameView()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//�ɻ�����
VOID CGameClientView::ShowCartoonPlan(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonPlan=true;
		m_wCartoonPlanIndex=0L;

		//����ʱ��
		m_CartoonPlanLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonPlan.IsNull()==true)
		{
			m_TextureCartoonPlan.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_PLAN"),TEXT("PNG"));
		}

		//���ڴ�С
		CRect rcClient;
		GetClientRect(&rcClient);

		//����λ��
		m_ptCartoonPlan.SetPoint( rcClient.Width() , rcClient.Height()/2 );
	
		//���½���
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonPlan==true)
		{
			//���ñ���
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			//����λ��
			m_ptCartoonPlan.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}

//��ը����
VOID CGameClientView::ShowCartoonBomb(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonBomb=true;
		m_wCartoonBombIndex=0L;

		//����ʱ��
		m_CartoonBombLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonBomb.IsNull()==true)
		{
			m_TextureCartoonBomb.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_BOMB"),TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonBomb==true)
		{
			//���ñ���
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}

//˳�Ӷ���
VOID CGameClientView::ShowCartoonLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonLine = true;
		m_wCartoonLineIndex = 0L;

		//����ʱ��
		m_CartoonLineLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonLine.IsNull()==true)
		{
			m_TextureCartoonLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_LINE"), TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonLine == true)
		{
			//���ñ���
			m_bCartoonLine = false;
			m_wCartoonLineIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonLine.IsNull() == false);
			if (m_TextureCartoonLine.IsNull() == false) m_TextureCartoonLine.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}
VOID CGameClientView::ShowCartoonDoubleLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonDoubleLine = true;
		m_wCartoonDoubleLineIndex = 0L;

		//����ʱ��
		m_CartoonDoubleLineLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonDoubleLine.IsNull()==true)
		{
			m_TextureCartoonDoubleLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_DOUBLE_LINE"), TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonDoubleLine == true)
		{
			//���ñ���
			m_bCartoonDoubleLine = false;
			m_wCartoonDoubleLineIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonDoubleLine.IsNull() == false);
			if (m_TextureCartoonDoubleLine.IsNull() == false) m_TextureCartoonDoubleLine.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}


VOID CGameClientView::ShowCartoonThreeLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonThreeLine = true;
		m_wCartoonThreeLineIndex = 0L;

		//����ʱ��
		m_CartoonThreeLineLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonThreeLine.IsNull()==true)
		{
			m_TextureCartoonThreeLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_LINE"), TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonThreeLine == true)
		{
			//���ñ���
			m_bCartoonThreeLine = false;
			m_wCartoonThreeLineIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeLine.IsNull() == false);
			if (m_TextureCartoonThreeLine.IsNull() == false) m_TextureCartoonThreeLine.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}

VOID CGameClientView::ShowCartoonThreeTake(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonThreeTake = true;
		m_wCartoonThreeTakeIndex = 0L;

		//����ʱ��
		m_CartoonThreeTakeLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonThreeTake.IsNull()==true)
		{
			m_TextureCartoonThreeTake.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_TAKE"), TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonThreeTake == true)
		{
			//���ñ���
			m_bCartoonThreeTake = false;
			m_wCartoonThreeTakeIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeTake.IsNull() == false);
			if (m_TextureCartoonThreeTake.IsNull() == false) m_TextureCartoonThreeTake.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}

VOID CGameClientView::ShowCartoonThreeTakeLine(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonThreeTakeLine = true;
		m_wCartoonThreeTakeLineIndex = 0L;

		//����ʱ��
		m_CartoonThreeTakeLineLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonThreeTakeLine.IsNull()==true)
		{
			m_TextureCartoonThreeTakeLine.LoadImage(&m_D3DDevice, AfxGetInstanceHandle(), TEXT("CARTOON_THREE_TAKE_LINE"), TEXT("PNG"));
		}

		//���½���
		RefreshGameView();
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonThreeTakeLine == true)
		{
			//���ñ���
			m_bCartoonThreeTakeLine = false;
			m_wCartoonThreeTakeLineIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonThreeTakeLine.IsNull() == false);
			if (m_TextureCartoonThreeTakeLine.IsNull() == false) m_TextureCartoonThreeTakeLine.Destory();

			//���½���
			RefreshGameView();
		}
	}

	return;
}

//��������
VOID CGameClientView::SetLastTurnState(bool bLastTurnState)
{

	return;
}

//���÷���
VOID CGameClientView::SetUserPassState(WORD wChairID, bool bUserPass)
{
	//��������
	bool bLoadImage=false;

	//���ñ���
	if (wChairID==INVALID_CHAIR)
	{
		//���ñ���
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bUserPass[i]=bUserPass;
		}
	}
	else
	{
		//���ñ���
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���þ���
VOID CGameClientView::SetUserCountWarn(WORD wChairID, bool bCountWarn)
{
	//��������
	bool bLoadImage=false;

	//���ñ���
	if (wChairID==INVALID_CHAIR)
	{
		//���ñ���
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bCountWarn[i]=bCountWarn;
		}
	}
	else
	{
		//���ñ���
		ASSERT(wChairID<GAME_PLAYER);
		m_bCountWarn[wChairID]=bCountWarn;
	}

	//״̬���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	//��Դ����
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

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���û���
VOID CGameClientView::SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//���ñ���
	m_pHistoryScore[wChairID]=pHistoryScore;

	//���½���
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
				m_cbAskFlag[wChairID] = 3;//��Ӧ
		}
		else
		{
			if(!bRespond)
				m_cbAskFlag[wChairID] = 2;
			else
				m_cbAskFlag[wChairID] = 2;//���Ҫ��ʾ����Ӧ������ĳ�4
		}
	}
}

//���ƶ���
VOID CGameClientView::StopDispatchCard()
{
	//ֹͣ����
	if ((m_bRiffleCard==true)||(m_bDispatchCard==true))
	{
		//���ñ���
		m_bRiffleCard=false;
		m_bDispatchCard=false;
		m_wDispatchCardIndex=0L;

		//�����˿�
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

		//ж����Դ
		ASSERT(m_TextureDispatchCard.IsNull()==false);
		if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//���ƶ���
VOID CGameClientView::ShowDispatchCard(BYTE cbCardData[], BYTE cbCardCount, WORD wStartUser)
{
	//���ñ���
	m_bWaitXuanZhan=false;
	m_bWaitFindFriend=false;
	m_bWaitAskFriend=false;
	m_bWaitAddTimes=false;

	m_bRiffleCard=true;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=RIFFLE_CARD_COUNT_ALL;

	//���ñ���
	m_wStartUser=wStartUser;
	CopyMemory(m_cbHandCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(0);
	}

	//����ʱ��
	m_DispatchCardLapseCount.Initialization();

	//������Դ
	ASSERT(m_TextureDispatchCard.IsNull()==true);
	if (m_TextureDispatchCard.IsNull()==true)
		m_TextureDispatchCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"),TEXT("PNG"));

	//���½���
	RefreshGameView();

	//��������
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"));

	return;
}

//��ʼ��ť
VOID CGameClientView::OnStart()
{
	//������Ϣ
	SendEngineMessage(IDM_START,0,0);

	return;
}

//���ְ�ť
VOID CGameClientView::OnLookScore()
{
	//���ñ���
	m_bShowScore=!m_bShowScore;

	//������Դ
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

	//���ð�ť
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,(m_bShowScore==true)?TEXT("BT_CLOSE_SCORE"):TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);

	return;
}

//���ư�ť
VOID CGameClientView::OnOutCard()
{
	//������Ϣ
	SendEngineMessage(IDM_OUT_CARD,0,0);

	return;
}

//����ť
VOID CGameClientView::OnSortCard()
{
	//������Ϣ
	SendEngineMessage(IDM_SORT_HAND_CARD,0,0);

	return;
}

//���ְ�ť
VOID CGameClientView::OnLastTurn()
{
	//������Ϣ
	SendEngineMessage(IDM_LAST_TURN_CARD,0,0);

	return;
}

//�йܰ�ť
VOID CGameClientView::OnTrustee()
{
	//������Ϣ
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//�йܰ�ť
VOID CGameClientView::OnCancelTrustee()
{
	//������Ϣ
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//PASS��ť
VOID CGameClientView::OnPassCard()
{
	//������Ϣ
	SendEngineMessage(IDM_PASS_CARD,0,0);

	return;
}

//��ʾ��ť
VOID CGameClientView::OnOutPrompt()
{
	//������Ϣ
	SendEngineMessage(IDM_OUT_PROMPT,0,0);

	return;
}

//�зְ�ť
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
//�����Ϣ
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//���ͳ���
	if ( m_HandCardControl[ME_VIEW_ID].GetPositively() )
	{
		SendEngineMessage(IDM_OUT_CARD,1,1);
	}

	return;
}

//�����Ϣ
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	if(m_bShowFindFriend)
	{
		//m_SelectFriendCardControl.SetShootCard(NULL,0);

		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_SelectFriendCardControl.SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItemEx) pCardItem=m_SelectFriendCardControl.GetCardFromPoint(Point);

		//�����˿�
		if (pCardItem!=NULL)
		{
			//�����˿�
			pCardItem->bShoot=!pCardItem->bShoot;
			
			//���½���
			RefreshGameView();
		}

		//��ȡ���������˿�
		BYTE cbOldShootCardEx[MAX_CARD_COUNT];
		BYTE count = m_SelectFriendCardControl.GetShootCard(cbOldShootCardEx, MAX_CARD_COUNT);

		m_btSelectFriend.EnableWindow(count == 1);

		//���ñ���
		m_bMoveMouseEx=false;
		m_bSwitchCardEx=false;
		m_bSelectCardEx=false;
		m_cbMouseDownItemEx=INVALID_ITEM;
		m_cbHoverCardItemEx=INVALID_ITEM;

		return;
	}

	//��ȡ���������˿�
	BYTE cbOldShootCard[MAX_CARD_COUNT];
	BYTE cbOldShootCrad = m_HandCardControl[ME_VIEW_ID].GetShootCard(cbOldShootCard, MAX_CARD_COUNT);

	//Ĭ�ϴ���
	if (m_bMoveMouse==false)
	{
		//��ȡ�˿�
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItem) pCardItem=m_HandCardControl[ME_VIEW_ID].GetCardFromPoint(Point);

		//�����˿�
		if (pCardItem!=NULL)
		{
			//�����˿�
			pCardItem->bShoot=!pCardItem->bShoot;

			//������Ϣ
			SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

			//���½���
			RefreshGameView();
		}
	}

	//ѡ����
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//�����˿�
		if (m_cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[ME_VIEW_ID].SetShootArea(m_cbMouseDownItem,m_cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[ME_VIEW_ID].SetShootArea(m_cbHoverCardItem,m_cbMouseDownItem);
		}

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		RefreshGameView();
	}

	//��������
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//�����˿�
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		RefreshGameView();
	}

	//��ȡ�˿�
	BYTE cbShootCard[MAX_CARD_COUNT];
	ZeroMemory(cbShootCard, sizeof(cbShootCard));
	BYTE cbShootCrad = m_HandCardControl[ME_VIEW_ID].GetShootCard(cbShootCard, MAX_CARD_COUNT);

	//����˳��
	if ( cbOldShootCrad == 1 && cbShootCrad == 2 && !m_bSwitchCard && !m_bMoveMouse &&m_HandCardControl[ME_VIEW_ID].SetShootOrderCard(cbShootCard[0], cbShootCard[1]) )
	{
		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		RefreshGameView();
	}

	//�ͷ����
	ReleaseCapture();


	//���ñ���
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//��������
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbHoverCardItem=INVALID_ITEM;

	return;
}

//�����Ϣ
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
	__super::OnMouseMove(nFlags, MousePoint);

	//��������
	BYTE cbHoverCardItem=INVALID_ITEM;

	//�ƶ��ж�
	if (m_cbHoverCardItem!=INVALID_ITEM)
	{
		//�˿˴�С
		CSize ControlSize;
		m_HandCardControl[ME_VIEW_ID].GetControlSize(ControlSize);

		//�˿�λ��
		CPoint OriginPoint;
		m_HandCardControl[ME_VIEW_ID].GetOriginPoint(OriginPoint);

		//���е���
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//��ȡ����
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		cbHoverCardItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(MousePoint);

		//�ƶ�����
		if (cbHoverCardItem!=m_cbHoverCardItem) m_bMoveMouse=true;
	}

	//ѡ���˿�
	if ((m_bSelectCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	{
		//�����˿�
		if (cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[ME_VIEW_ID].SetSelectIndex(m_cbMouseDownItem,cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[ME_VIEW_ID].SetSelectIndex(cbHoverCardItem,m_cbMouseDownItem);
		}

		//���½���
		RefreshGameView();
	}

	//���ñ���
	m_cbHoverCardItem=cbHoverCardItem;

	return;
}

//�����Ϣ
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

	//״̬�ж�
	if(m_bShowFindFriend==true)
	{
		if (m_SelectFriendCardControl.GetPositively()==true)
		{
			//��ȡ�˿�
			m_cbMouseDownItemEx=m_SelectFriendCardControl.SwitchCardPoint(Point);

			//�����˿�
			if (m_cbMouseDownItemEx!=INVALID_ITEM)
			{
				//��ȡ�˿�
				tagCardItem * pCardItem=m_SelectFriendCardControl.GetCardFromIndex(m_cbMouseDownItemEx);

				//���ñ���
				m_bMoveMouseEx=false;
				m_cbHoverCardItemEx=m_cbMouseDownItemEx;

				//��������
				if (pCardItem->bShoot==true)
				{
					//���ñ���
					m_bSwitchCard=true;
					m_cbSwitchCardItem=0;

					//����״̬
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

				//�������
				SetCapture();
			}
		}

		return;
	}


	if (m_HandCardControl[ME_VIEW_ID].GetPositively()==true)
	{
		//��ȡ�˿�
		m_cbMouseDownItem=m_HandCardControl[ME_VIEW_ID].SwitchCardPoint(Point);

		//�����˿�
		if (m_cbMouseDownItem!=INVALID_ITEM)
		{
			//��ȡ�˿�
			tagCardItem * pCardItem=m_HandCardControl[ME_VIEW_ID].GetCardFromIndex(m_cbMouseDownItem);

			//���ñ���
			m_bMoveMouse=false;
			m_cbHoverCardItem=m_cbMouseDownItem;

			//��������
			if (pCardItem->bShoot==true)
			{
				//���ñ���
				m_bSwitchCard=true;
				m_cbSwitchCardItem=0;

				//����״̬
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

			//�������
			SetCapture();
		}
	}

	return;
}

//�����Ϣ
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//��ȡ�˿�
	tagCardItem * pCardItem = m_HandCardControl[ME_VIEW_ID].GetCardFromPoint(Point);

	//�����˿�
	if ( pCardItem == NULL )
	{
		//��ֹ��ť
		m_btOutCard.EnableWindow(FALSE);

		//�����˿�
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//���½���
		RefreshGameView();
	}
	else if ( m_HandCardControl[ME_VIEW_ID].GetPositively())
	{
		//�����˿�
		m_HandCardControl[ME_VIEW_ID].SetShootCard(NULL,0);

		//����ͬ��
		m_HandCardControl[ME_VIEW_ID].SetShootSameCard(pCardItem->cbCardData);

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		RefreshGameView();
	}

	return;
}

//�����Ϣ
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//��ȡ���
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//�˿˲���
	if (m_HandCardControl[ME_VIEW_ID].OnEventSetCursor(MousePoint)==true)
	{
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//��Ϣ����
BOOL CGameClientView::OnCommand( WPARAM wParam, LPARAM lParam )
{
	return __super::OnCommand(wParam,lParam);
}

//�滭ʱ��
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
		//ʱ�ӱ���
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
		//����
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
	//����Լ�������
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
