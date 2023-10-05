#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DirectUIButtonEx.h"
#include "ClientControl.h"
#include "MyUIThread.h"
#include "Animation.h"
#include "Quicken.h"
#include "MyUIThread.h"
#include "AnimationScroll.h"
#include "D3DTextureex.h"
#include ".\directuibuttonexx.h"
////////////////////////////////////////////////////////////////////////////////// 
#define IDM_GAME_OPERATE			(WM_USER+208)						//������Ϣ
#define IDM_GAME_OPTION				(WM_USER+209)						//��Ϸ����
#define IDM_GAME_SOUND				(WM_USER+210)						//��Ϸ����
#define IDM_GAME_CLOSE				(WM_USER+211)						//��Ϸ�ر�
#define IDM_GAME_BET				(WM_USER+212)						//��Ϸ��ע

//��������
#define	ONE_NULL					0									//��
#define	ONE_GUANDONG				1									//����
#define	ONE_ZHONGJIANG				2									//�н���
#define	ONE_KAIJIANG				3									//����
#define	ONE_KAIJIANG_END			4									//����
#define	ONE_SHUO_MING				5									//��Ϸ˵��

#define	TWO_XIA_ZHU					6									//��ע
#define	TWO_KAI_JIAN				7									//����

#define	THREE_GUANDONG				9									//����
#define	THREE_KAIJIANG				10									//����
#define	THREE_KAIJIANG_END			11									//����

class CGameClientEngine;
// ��������
enum EnumClientView
{
	ClientView_SelectMap = 0,			// ѡ���ͼ
	ClientView_Game = 1,				// ��Ϸ��
	ClientView_Max = 2,					// ���ֵ
};

//������Ϣ
enum EnumGameMessage
{
	Message_Start = (WM_USER+201),	// ������Ϸ
	Message_OpenAdmin,				// �򿪿���
};

////////////////////////////////////////////////////////////////////////////////// 

// ��Ϸ��ͼ
class CGameClientView : public dex::CDirectUIWindow
{
	//��ʱ��
	UINT							m_uAccuracy;
	UINT							m_uTimerID;

	CString							m_strTip;
	bool							m_bDrawTop;							//������
	bool							m_bDrawFrame;						//���߿�
	
	//��Ϸ����
	bool							m_bShowShuoMing;					//��ʾ˵��
	BYTE							m_cbShuoMingIndex;					//˵������
	BYTE							m_cbPlayIndex;						//��������
	BYTE							m_cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	bool							m_bZhongJiang[ITEM_Y_COUNT][ITEM_X_COUNT];	//�н���Ϣ
	BYTE							m_cbZhongJiangXian;				//�н���
	CPoint							m_ptZhongJiang[ITEM_X_COUNT];	//�н�λ��
	bool							m_bAutoGame;					//�Զ���Ϸ

	BYTE							m_cbGameStatus;					//��Ϸ״̬

	//Ѻ�߱���
	BYTE							m_cbWaitMode;					//�ȴ�����
	SCORE							m_lOneScoreTemp;				//��ʱ����
	SCORE							m_lSaveScore;					//�������
	SCORE							m_lOneGredit;					//���Ϸ���
	SCORE							m_lOneTotalWin;					//��Ӯ����
	SCORE							m_lLine;						//Ѻ������
	SCORE							m_lBet;							//��ע��
	SCORE							m_lOneTotalBet;					//����ע��
	SCORE							m_lOneGameScore;				//��Ϸ�÷�


	//�ȱ�����
	BYTE							m_cbUser[3];					//��ұ���
	SCORE							m_lTwoGredit;					//���Ϸ���
	SCORE							m_lTwoTotalWin;					//��Ӯ����
	SCORE							m_lTwoTotalBet;					//����ע��
	SCORE							m_lTwoGameScore;				//��Ϸ�÷�
	BYTE							m_cbBetIndex;					//��עλ��
	bool							m_bShowSice[2];					//��ʾ����
	BYTE							m_cbSice[2];					//���Ӵ�С

	//С��������
	SCORE							m_lThreeGredit;					//���Ϸ���
	SCORE							m_lThreeTotalWin;				//��Ӯ����
	SCORE							m_lThreeTotalBet;				//����ע��
	SCORE							m_lThreeGameScore;				//��Ϸ�÷�
	SCORE							m_lThreeTimes;					//��Ϸ�÷�
	BYTE							m_cbItemType[24];				//ͼ������
	BYTE							m_cbThreeItemInfo[4];				//ͼ������
	bool							m_bThreeZhongJiang;				//�Ƿ��н�
	BYTE							m_cbThreeIndex;					//��������
	BYTE							m_cbThreeZhongtimes;			//�н�λ�� 0�� 1 ��500�� 2��



	//�������
	bool							m_bInitImage;				//����ͼƬ
	BYTE							m_cbImageLoadPos;			//���ؽ���
	BYTE							m_cbShuoMingLoadPos;		//���ؽ���
	CString							m_strPathImage;				//ͼƬĿ¼

	//λ�ñ���
protected:
	CPoint							m_ptItemImage[ITEM_Y_COUNT][ITEM_X_COUNT];	//ͼƬλ��
	CPoint							m_ptBianKuang[ITEM_Y_COUNT][ITEM_X_COUNT];	//ͼƬλ��
	CPoint							m_ptGredit;						//���Ϸ���
	CPoint							m_ptTotalWin;					//��Ӯ����
	CPoint							m_ptLine;						//Ѻ������
	CPoint							m_ptBet;						//��ע��
	CPoint							m_ptTotalBet;					//����ע��

	CPoint							m_ptTwoGredit;					//���Ϸ���
	CPoint							m_ptTwoTotalWin;				//��Ӯ����
	CPoint							m_ptTwoTotalBet;				//����ע��
	CPoint							m_ptTwoJingYuanBao[3];			//����ע��
	CPoint							m_ptDice0[2];					//����ע��
	CPoint							m_ptDice1[2];					//����ע��

	CPoint							m_ptThreeGredit;				//���Ϸ���
	CPoint							m_ptThreeTotalWin;				//��Ӯ����
	CPoint							m_ptThreeTotalBet;				//����ע��
	CPoint							m_ptThreeTotalTimes;			//����ע��
	CPoint							m_ptThreeItem[24];				//ͼ��λ��
	CPoint							m_ptThreeItemImage[4];				//ͼ��λ��
	CPoint							m_ptThreeItemTimes[8];				//ͼ��λ��
	CPoint							m_ptThreeTimes[3];					//ͼ��λ��

	// ��ԴͼƬ
public:



	// ��ť��Դ
public:
	CDirectUIButtonExx			m_ButtonWinClose;								// �ر�
	//CDirectUIButtonExx			m_ButtonWinMax;									// ���
	CDirectUIButtonExx			m_ButtonWinMin;									// ��С��
	CDirectUIButtonExx			m_ButtonSound;									// ����
	CDirectUIButtonExx			m_ButtonWinSet;									// ����
	CDirectUIButtonExx			m_ButtonAdmin;									// ����
	
	CDirectUIButtonExx			m_ButtonAddBet;									// ����
	CDirectUIButtonExx			m_ButtonSubBet;									// ����
	//�߿���Դ
protected:
	CD3DTextureEx				m_ImageGameFrame[8];							//��Ϸ���	

public:
	
	// ��Դ����
public:
	dex::CD3DFont					m_D3DFont;									// ��������

	//�ؼ�����
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�

	//������Դ
protected:
	CD3DTextureEx				m_TextureShuoMingPosBack;			//��Դ����
	CD3DTextureEx				m_TextureShuoMingPos;				//��Ϣ����
	CD3DTextureEx				m_TextureExitShuoMing;				//�˳�˵��
	CAnimation						m_AnimationShuoMing;				//˵��


	CAnimation						m_AnimationDaGu;					//��Ķ���
	CAnimation						m_AnimationZhongJiangXian;			//�н���
	CAnimation						m_AnimationWaitQi;					//�ȴ���ʼ״̬�µ�Ʈ��
	CAnimation						m_AnimationKaiQi;					//����״̬�µ�ҡ��
	CAnimation						m_AnimationShuiHuZhuang;			//����ˮ䰴�3����������
	CAnimation						m_AnimationSanGuang;				//���⶯��
	CAnimation						m_AnimationBianKuang;				//�߿򶯻�
	CAnimationScroll				m_AnimationScrollGunDong;			//��������
	CAnimation						m_AnimationStaticImage;				//��̬ͼ��
	CAnimation						m_AnimationXuLieImage[ITEM_COUNT];	//���ж���
	CAnimation						m_AnimationDouDongImage[ITEM_COUNT];//��������
	CAnimation						m_AnimationOneTip;					//��ʾͼ��


	CQuicken						m_Quicken[ITEM_Y_COUNT * ITEM_X_COUNT];	//����ʱ��
	CQuicken						m_QuickenThreeItem[4];					//����ʱ��
	CQuicken						m_QuickenThree;							//����ʱ��
	CQuicken						m_QuickenSan;							//����ʱ��
	CQuicken						m_QuickenViewSan;						//����ʱ��


	CAnimation						m_AnimationHeGuan[6];			//�ɹٶ���
	CAnimation						m_AnimationZuoHeGuan[4];		//������Ӹ��ֱ���
	CAnimation						m_AnimationYouHeGuan[4];		//�ұ����Ӹ��ֱ���
	CAnimation						m_AnimationDice[2];				//����
	CAnimation						m_AnimationZuoZhiDouDong;		//����
	CAnimation						m_AnimationJingYuanBaoDouDong;	//��Ԫ��
	CAnimation						m_AnimationTwoSanGuang;			//��Ԫ��
	CAnimation						m_AnimationTwoTip;				//��ʾͼ��

	//��Դ����
protected:
	CD3DTextureEx				m_TextureOneBack;				//��Դ����
	CD3DTextureEx				m_TextureOneTipNumber;			//��ʾ����
	CD3DTextureEx				m_TextureTwoTipNumber;			//��ʾ����
	CD3DTextureEx				m_TextureCellNumber;			//��Ϣ����
	CD3DTextureEx				m_TextureCellNumberS;			//��Ϣ����
	CD3DTextureEx				m_TextureWin;					//��Ϣ����
	CD3DTextureEx				m_TextureWinNumber;				//��Ϣ����

	CD3DTextureEx				m_TextureTwoBack;				//��Դ����
	CD3DTextureEx				m_TextureTwoJingYuanBao;		//��Դ����
	CD3DTextureEx				m_TextureTwoJingYuanBaoEx;		//��Դ����

	//��������
	CD3DTextureEx				m_TextureThreeBack;				//��Դ����
	CD3DTextureEx				m_TextureTimesNumber;			//��Ϣ����
	CAnimation						m_AnimationThreeDeng;			//���ܵ�ͼ��
	CAnimation						m_AnimationThreeTimes;			//����
	CAnimation						m_AnimationThreeTimes20L;		//����
	CAnimation						m_AnimationThreeTimes20R;		//����
	CAnimation						m_AnimationThreeTimes500;		//����

	//�߳�
public:
	dex::CD3DDevice *				m_pD3DDevice;
	CMyUIThread*					m_pMyUIThread;

	//���
public:
	CString							m_StrGameText;									// ����
	IClientUserItem *				m_pClientUserItem;								//
	CGameClientEngine*				m_pGameClientEngine;
	HINSTANCE						m_hInst;
	IClientControlDlg*				m_pClientControlDlg;	
	bool							m_bShowClientControlDlg;

	// �������� 
public:
	// ���캯��
	CGameClientView();
	// ��������
	virtual ~CGameClientView();

	// ���ú���
public:
	// ���ý���
	void ResetGameView();
	//�����ؼ�
	void RectifyControl(uint nWidth, uint nHeight);
	// ���к���
public:
	// ������Ϣ
	bool OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize);

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ��� 
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���»��� 
	void OnWindowUpdataTextureBuffer( dex::CD3DDevice * pD3DDevice );
	// ��������
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// ������ʾ 
	void OnWindowShow(dex::CD3DDevice * pD3DDevice, bool bVisible );

	//�滭���
	void DrawGameFrame(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ������Ϣ
public:
	// ����ƶ� 
	void OnMouseMove( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// ������� 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// ���̧�� 
	void OnLButtonUp( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite );
	// ��Ϣ����
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// ��Ϣ����
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// ���ڴ�С 
	void OnWindowSize(dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// ���ú���
public:
	// ������Ϸ����
	void SetGameText( CString StrGameText ) { m_StrGameText = StrGameText; }
	// �������ָ��
	void SetClientUserItem(IClientUserItem * pClientUserItem ) { m_pClientUserItem = pClientUserItem; };
	// ������Ϸ����
	void SetGameClientEngine(CGameClientEngine * pGameClientEngine ) { m_pGameClientEngine = pGameClientEngine; }

	// ��ť��Ϣ
public:
	// �ر� 
	void OnButtonWinClose();
	// ��� 
	void OnButtonWinMax();
	// ��С�� 
	void OnButtonWinMin();	
	// ���� 
	void OnButtonSound();
	// ���� 
	void OnButtonWinSet();
	// ��ע 
	void OnButtonAddBet();
	// ��ע 
	void OnButtonSubBet();
	// ����
	void OnButtonAdmin();
	// ��ʼ������
	void ResetView(bool bResetEngine=false);

	//��������
public:
	//��������
	VOID CartoonMovie();
	//��ʼ����
	VOID StartOneGuanDong(bool bStart, BYTE cbItemInfo[][ITEM_X_COUNT] = NULL);
	//��ʼ����
	VOID StartZhongJiangXian(BYTE cbZhongJiangXian, CPoint ptZhongJiang[ITEM_X_COUNT] = NULL);
	//��ʼ����
	VOID StartKaiJang(bool bZhongJiang[][ITEM_X_COUNT] = NULL);
	//��ʼ����
	VOID StartKaiJangEnd(SCORE lScore, bool bLong = false);
	//��ʼ�ȱ�
	VOID StartTwoWiatAddScore(bool bStart);
	//��ʼ����
	VOID StartThreeGunDong(BYTE cbItemType[4]);
	//��ʼ����
	VOID StartThreeGunDongEnd(SCORE lScore, BYTE cbPos);
	//��ʼ����
	VOID StartThreeKaiJian(BYTE cbIndex);
	//��ʼ����
	VOID StartThreeKaiJianEnd(SCORE lScore);



	//���ƺ���
public:
	//���ÿ���
	VOID ShowClientControlDlg(bool bShowClientControlDlg);
	//����˵������
	BYTE GetShuoMingPos() { return m_cbShuoMingLoadPos;}
	//����˵������
	VOID SetShuoMingPos(BYTE cbShuoMingPos) { m_cbShuoMingLoadPos = cbShuoMingPos;}
	//����˵������
	VOID SetShuoMingIndex(BYTE cbIndex) { m_cbShuoMingIndex = cbIndex; };
	//ȡ��˵������
	BYTE GetShuoMingIndex() { return m_cbShuoMingIndex; }
	//ȡ��˵������
	VOID ShowShuoMing(bool bShowShuoMing);
	//�����Զ�
	VOID SetAutoGame(bool bAutoGame) { m_bAutoGame = bAutoGame; }
	//������ע
	VOID SetOneBet(SCORE lBet, SCORE lOneTotalBet) { m_lBet = lBet; m_lOneTotalBet = lOneTotalBet; }
	//������ע
	VOID SetSaveScore(SCORE lSaveScore);
	//��ʼ����
	VOID SetOneScore(SCORE lScore) { m_lOneGameScore = lScore; }
	//��ʼ����
	VOID SetOneTotalWin(SCORE lTotalWin, SCORE lScoreXian) { m_lOneTotalWin = lTotalWin; m_lOneScoreTemp = lScoreXian;}
	//�ȴ�����
	VOID SetWaitMode(BYTE cbWaitMode) { m_cbWaitMode = cbWaitMode; }
	//��ʼ����
	VOID SetItemInfo(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//������ע
	VOID SetUser(BYTE cbUser[3], bool bOne, BYTE cbExp);
	//������ע
	VOID SetTwoDouDong();
	//������ע
	VOID SetTwoBetIndex(BYTE cbBetIndex);
	//������ע
	VOID SetTwoSice(BYTE cbSice[2]);
	//������ע
	VOID SetTwoShowSice(bool bSice0, bool bSice1);
	//��ʼ����
	VOID SetTwoScore(SCORE lScore) { m_lTwoGameScore = lScore; }
	//��ʼ����
	VOID SetThreeScore(SCORE lScore) { m_lThreeGameScore = lScore; }
	//��ʼ����
	VOID SetThreeTotalWin(SCORE lTotalWin) { m_lThreeTotalWin += lTotalWin; }
	//��ʼ����
	VOID SetThreeTimes(SCORE lThreeTimes) { m_lThreeTimes = lThreeTimes; }
	//���÷���
	VOID SetOneScore(SCORE lTotalWin, SCORE lTotalBetScore);
	//���÷���
	VOID SetTwoScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lExpScore = 0);
	//���÷���
	VOID SetThreeScore(SCORE lTotalWin, SCORE lTotalBet, SCORE lTimes, SCORE lExpScore = 0);

	//�˳��ȱ�
	VOID ExitTwo();
	//��������
	VOID SetTip(CString str) {m_strTip = str;}

	//״̬����
public:
	//����״̬
	VOID SetGameStatus(BYTE cbGameStatus);
	//���ؽ���
	BYTE GetImageLoad() { return m_bInitImage;}
	//���볡��
	VOID EnterScene(BYTE cbEnterScene);
	//ȡ��ʱ��
	WORD GetUserClock(WORD wChairID);

	//����ͼƬ
public:
	//����ͼƬ
	bool LoadImage();
	//����ͼƬ
	VOID ThreadInfo();
	//���°�ť
	VOID UpDateTop(bool bDrawTop);

	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

#endif