#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "HistoryRecord.h"
#include "RememberCardWnd.h"

//////////////////////////////////////////////////////////////////////////////////

//��ͼλ��
#define ME_VIEW_ID					2									//��ͼλ��

//������Ϣ
#define IDM_START					(WM_USER+200)						//��ʼ��Ϣ
#define IDM_OUT_CARD				(WM_USER+201)						//������Ϣ
#define IDM_PASS_CARD				(WM_USER+202)						//PASS��Ϣ
#define IDM_OUT_PROMPT				(WM_USER+203)						//��ʾ��Ϣ
#define IDM_CALL_SCORE				(WM_USER+204)						//�з���Ϣ
#define	IDM_LEFT_HIT_CARD			(WM_USER+205)						//����˿�
#define IDM_SORT_HAND_CARD			(WM_USER+206)						//�����˿�
#define IDM_LAST_TURN_CARD			(WM_USER+207)						//�����˿�
#define IDM_TRUSTEE_CONTROL			(WM_USER+208)						//�йܿ���
#define IDM_XUAN_ZHAN						(WM_USER+209)						//��ս��Ϣ
#define IDM_FIND_FRIEND						(WM_USER+210)						//��ͬ����Ϣ
#define IDM_ASK_FRIEND						(WM_USER+211)						//ѯ��ͬ����Ϣ
#define IDM_ADD_TIMES						(WM_USER+212)						//�ӱ���Ϣ
//������Ϣ
#define IDM_DISPATCH_FINISH			(WM_USER+300)						//�������
#define IDM_SEARCH_CARD				(WM_USER+320)						//��������

#define SEARCH_MISSILE				0									//�������
#define SEARCH_BOMB					1									//����ը��
#define SEARCH_THREE_LINE_TAKE_NTWO	2									//�����ɻ�
#define SEARCH_DOUBLE_LINE			3									//����˫˳
#define SEARCH_SINGLE_LINE			4									//������˳
#define SEARCH_THREE_TAKE_TWO		5									//��������һ��

//////////////////////////////////////////////////////////////////////////////////
class CGameClientEngine;
//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewD3D
{
	//״̬����
public:
	bool							m_bShowScore;						//����״̬
	bool							m_bLastTurnState;					//���ֱ�־
	bool							m_bWaitCallScore;					//�ȴ��з�
	bool							m_bUserPass[GAME_PLAYER];			//������־

	tagHistoryScore	*		m_pHistoryScore[GAME_PLAYER];		//������Ϣ

	bool							m_bWaitXuanZhan;
	bool							m_bWaitFindFriend;
	bool							m_bWaitAskFriend;
	bool							m_bWaitAddTimes;
	bool							m_bShowFindFriend;//��ʾѡ����ѵ���
	bool							m_b2Biggest;						//����2���ģʽ
	BYTE							m_cbSelectedCardData;
	int								m_nWidth;
	int								m_nHeight;
	//��Ϸ����
protected:
	
	LONG						m_lCellScore;						//��Ԫ����
	BYTE							m_cbXuanZhanFlag[GAME_PLAYER];
	BYTE							m_cbAskFlag[GAME_PLAYER];
	BYTE							m_cbAddTimesFlag[GAME_PLAYER];			//�ӱ���Ϣ
	BYTE							m_cbFriendFlag[GAME_PLAYER];				//���ѱ�ǩ
	BYTE							m_cbTrusteeFlag[GAME_PLAYER];			//�йܱ�ǩ
	int								m_cbGameScore[GAME_PLAYER];
	BYTE							m_cbTurnScore;

	WORD							m_wBankerUser;						//ׯ���û�
	//�ƶ�����
protected:
	bool							m_bMoveMouse;						//�ƶ���־
	bool							m_bSwitchCard;						//������־
	bool							m_bSelectCard;						//ѡ���־
	BYTE							m_cbHoverCardItem;					//��������
	BYTE							m_cbMouseDownItem;					//�������
	BYTE							m_cbSwitchCardItem;					//��������


	bool							m_bMoveMouseEx;						//�ƶ���־
	bool							m_bSwitchCardEx;						//������־
	bool							m_bSelectCardEx;						//ѡ���־
	BYTE							m_cbHoverCardItemEx;					//��������
	BYTE							m_cbMouseDownItemEx;					//�������
	BYTE							m_cbSwitchCardItemEx;					//��������
	//λ�ñ���
protected:
	CPoint							m_ptUserFrame[GAME_PLAYER];			//�û�λ��
	CPoint							m_ptPass[GAME_PLAYER];				//����λ��
	CPoint							m_ptBanker[GAME_PLAYER];			//ׯ��λ��
	CPoint							m_ptCountWarn[GAME_PLAYER];			//����λ��
	CPoint							m_ptCallScore[GAME_PLAYER];			//�з�λ��
	CPoint							m_ptCardFrame[GAME_PLAYER];			//�ƿ�λ��
	CPoint							m_ptTrustee[GAME_PLAYER];					//�йܱ�ʶ
	//��ը����
protected:
	bool							m_bCartoonBomb;						//������־
	WORD							m_wCartoonBombIndex;				//֡������
	CLapseCount						m_CartoonBombLapseCount;			//���ż���

	//˳�Ӷ���
protected:
	bool							m_bCartoonLine;						//������־
	WORD							m_wCartoonLineIndex;				//֡������
	CLapseCount						m_CartoonLineLapseCount;			//���ż���
	//���Զ���
protected:
	bool							m_bCartoonDoubleLine;						//������־
	WORD							m_wCartoonDoubleLineIndex;				//֡������
	CLapseCount						m_CartoonDoubleLineLapseCount;			//���ż���
	//��ͬ�Ŷ���
protected:
	bool							m_bCartoonThreeLine;						//������־
	WORD							m_wCartoonThreeLineIndex;				//֡������
	CLapseCount						m_CartoonThreeLineLapseCount;			//���ż���
	//�����ƶ���
protected:
	bool							m_bCartoonThreeTake;						//������־
	WORD							m_wCartoonThreeTakeIndex;				//֡������
	CLapseCount						m_CartoonThreeTakeLapseCount;			//���ż���
	//����������
protected:
	bool							m_bCartoonThreeTakeLine;						//������־
	WORD							m_wCartoonThreeTakeLineIndex;				//֡������
	CLapseCount						m_CartoonThreeTakeLineLapseCount;			//���ż���
	//�ɻ�����
protected:
	bool							m_bCartoonPlan;						//������־
	WORD							m_wCartoonPlanIndex;				//֡������
	CPoint							m_ptCartoonPlan;					//�ɻ�λ��
	CLapseCount						m_CartoonPlanLapseCount;			//���ż���

	//���ƶ���
protected:
	bool							m_bReversalCard;					//������־
	WORD							m_wReversalCardIndex;				//֡������
	CLapseCount						m_ReversalCardLapseCount;			//���ż���

	//���ƶ���
protected:
	bool							m_bRiffleCard;						//������־
	bool							m_bDispatchCard;					//������־
	WORD							m_wDispatchCardIndex;				//֡������
	CLapseCount						m_DispatchCardLapseCount;			//���ż���

	//��������
protected:
	WORD							m_wStartUser;						//��ʼ�û�
	BYTE							m_cbHandCardData[NORMAL_CARD_COUNT];//��������

	//��������
protected:
	bool							m_bCountWarn[GAME_PLAYER];			//������־
	WORD							m_wCountWarnIndex[GAME_PLAYER];		//֡������
	CLapseCount						m_CountWarnLapseCount[GAME_PLAYER];	//���ż���

	//��Ϸ��ť
public:
	CVirtualButton					m_btScore;							//���ְ�ť
	CVirtualButton					m_btStart;							//��ʼ��ť
	CVirtualButton					m_btOutCard;						//���ư�ť
	CVirtualButton					m_btPassCard;						//PASS��ť
	CVirtualButton					m_btOutPrompt;						//��ʾ��ť

	//���ܰ�ť
public:
	CVirtualButton					m_btCancelTrustee;					//ȡ���йܰ�ť
	CVirtualButton					m_btTrustee;						//�йܰ�ť
	CVirtualButton					m_btLastTurn;						//���ְ�ť
	CVirtualButton					m_btSortCard;						//����ť
	CVirtualButton					m_btRememberCard;					//��������ť

	//�зְ�ť
public:
	CVirtualButton					m_btXuan;
	CVirtualButton					m_btNoXuan;
	CVirtualButton					m_btAsk;
	CVirtualButton					m_btNoAsk;
	CVirtualButton					m_btAdd;
	CVirtualButton					m_btNoAdd;
	CVirtualButton					m_btRespond;
	CVirtualButton					m_btNoRespond;
	CVirtualButton					m_btSelectFriend;

	//��Դ����
protected:
	CD3DSprite						m_TextureBankerFlag;				//ׯ�ұ�־
	CD3DSprite						m_TextureFriendFlag;					//���ѱ�ʶ
	
	CD3DTexture						m_TextureFill;						//������Դ
	CD3DTexture						m_TextureBack;						//������Դ
	CD3DTexture						m_TextureGameInfo;						//��������
	CD3DTexture						m_TextureUserBG;				//�û����
	CD3DTexture						m_TextureIcon;						//��ұ�
	CD3DTexture						m_TextureIconNum;				//�����
	CD3DTexture						m_TextureRemnantCardNumber;//ʣ������
	CD3DTexture						m_TextureClockBack;					//ʱ�ӱ���
	CD3DTexture						m_TextureClockNum;					//ʱ������
	CD3DTexture						m_TextureTurnScoreBG;				//���ֱַ���
	CD3DTexture						m_TextureTurnScoreNum;				//���ַ�����
	CD3DTexture						m_TextureSelectFriendBg;			//ѡ���ѱ���
	CD3DTexture						m_TextureUserPass;					//������־
	CD3DTexture						m_TextureCountWarn;					//�˿˾���
	CD3DTexture						m_TextureCartoonRocket;				//�������
	CD3DTexture						m_TextureCartoonRocketLight;		//
	CD3DTexture						m_TextureCartoonBomb;				//ը������
	CD3DTexture						m_TextureCartoonPlan;				//�ɻ�����
	CD3DTexture						m_TextureCartoonLine;				//˳�Ӷ���
	CD3DTexture						m_TextureCartoonDoubleLine;		//���Զ���
	CD3DTexture						m_TextureCartoonThreeLine;		//��ͬ�Ŷ���
	CD3DTexture						m_TextureCartoonThreeTake;		//��������
	CD3DTexture						m_TextureCartoonThreeTakeLine;//����������
	CD3DTexture						m_TextureDispatchCard;				//��Դ����
	CD3DTexture						m_TextureHistoryScore;				//��Դ����
	CD3DTexture						m_TextureTrustee;						//�йܱ�ʶ
	CD3DTexture						m_TextureAddTimes;
	CD3DTexture						m_TextureNoAddTimes;
	CD3DTexture						m_TextureAsk;
	CD3DTexture						m_TextureNoAsk;
	CD3DTexture						m_TextureXuanZhan;
	CD3DTexture						m_TextureNoXuanZhan;
	CD3DTexture						m_TextureRespond;
	CD3DTexture						m_TextureNoRespond;
	//�ؼ�����
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CScoreControl					m_ScoreControl;						//���ֿؼ�
	CGameClientEngine					*m_pGameClientDlg;					//����ָ��
	CRememberCardWnd				m_RememberCardWnd;					//����������

	//�˿˿ؼ�
public:
	CCardControl					m_SelectFriendCardControl;	//ѡ����ѿؼ�
	CCardControl					m_HandCardControl[GAME_PLAYER];		//�û��˿�
	CCardControl					m_UserCardControl[GAME_PLAYER];		//�˿���ͼ

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//���غ���
private:
	//���ý���
	virtual VOID ResetGameView();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//���溯��
protected:
	//��������
	virtual VOID CartoonMovie();
	//�����豸
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//�滭����
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//ׯ������
public:
	//����ׯ��
	VOID SetBankerUser(WORD wBankerUser);

	//���ܺ���
public:
	//ˢ����ͼ
	VOID RefreshGameView();
	//��Ԫ����
	VOID SetCellScore(LONG lCellScore);
	//���ý���
	VOID ShowGameScoreInfo(tagScoreInfo & ScoreInfo);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	VOID DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	// �滭ʱ��
	VOID DrawClock(CD3DDevice * pD3DDevice, WORD wUserClock,WORD wChair);
	//��ʾѡ����
	VOID	 ShowFindFriend(bool bShow);
	//���Ͷ���
public:
	//�ɻ�����
	VOID ShowCartoonPlan(bool bCartoon);
	//��ը����
	VOID ShowCartoonBomb(bool bCartoon);
	//˳�Ӷ���
	VOID ShowCartoonLine(bool bCartoon);
	//���Զ���
	VOID ShowCartoonDoubleLine(bool bCartoon);
	//��ͬ�Ŷ���
	VOID ShowCartoonThreeLine(bool bCartoon);
	//�����ƶ���
	VOID ShowCartoonThreeTake(bool bCartoon);
	//����������
	VOID ShowCartoonThreeTakeLine(bool bCartoon);
	//״̬����
public:
	//��������
	VOID SetLastTurnState(bool bLastTurnState);
	//���÷���
	VOID SetUserPassState(WORD wChairID, bool bUserPass);
	//���þ���
	VOID SetUserCountWarn(WORD wChairID, bool bCountWarn);
	//���û���
	VOID SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore);

	//������ս
	VOID SetXuanZhan(WORD wChairID,bool bXuanZhan);
	//����ѯ��
	VOID SetAsk(WORD wChairID,bool bAsk,bool bRespond=false);
	//���üӱ�
	VOID	 SetAddTimes(WORD wChairID,bool bAddTimes);
	//���õ÷�
	VOID SetGameScore(WORD wChairID,int cbScore);
	//�����Ʒ�
	VOID SetTurnScore(BYTE cbTurnScore);
	//���ѱ�ǩ
	VOID	 SetFriendFlag(WORD wChairID,BYTE cbFlag);
	//�йܱ�ǩ
	VOID SetTrusteeFlag(WORD wChairID,BYTE cbFlag);
	//���ƺ���
public:
	//���ƶ���
	VOID StopDispatchCard();
	//���ƶ���
	VOID ShowDispatchCard(BYTE cbCardData[], BYTE cbCardCount, WORD wStartUser);

	//��ť��Ϣ
public:
	//��ʼ��ť
	VOID OnStart();
	//���ְ�ť
	VOID OnLookScore();
	//���ư�ť
	VOID OnOutCard();
	//����ť
	VOID OnSortCard();
	//���ְ�ť
	VOID OnLastTurn();
	//�йܰ�ť
	VOID OnTrustee();
	//�йܰ�ť
	VOID OnCancelTrustee();
	//PASS��ť
	VOID OnPassCard();
	//��ʾ��ť
	VOID OnOutPrompt();
	//�зְ�ť
	VOID OnCallScore(UINT nCtrlID);
	//��ս��ť
	VOID OnXuanZhan(UINT nCtrlID);
	//��ͬ�˰�ť
	VOID OnAskFriend(UINT nCtrlID);
	//�ӱ���ť
	VOID OnAddTimes(UINT nCtrlID);
	//ѡ����
	VOID	 OnSelectFriend();
	//���غ���
public:
	//��Ϣ����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//��Ϣ����
public:
	//�����Ϣ
	VOID OnRButtonUp(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnMouseMove(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDblClk(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif