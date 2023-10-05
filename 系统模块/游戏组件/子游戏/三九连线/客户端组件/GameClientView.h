#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "WindowTurntable.h"
#include "WindowWin.h"
#include "WindowBank.h"
#include "WindowAward.h"
#include "WindowRoad.h"
#include "WindowMessage.h"
#include "WindowHelp.h"
#include "WindowLine.h"
#include "WindowEgg.h"

//////////////////////////////////////////////////////////////////////////////////

// ��ͼλ��
#define MYSELF_VIEW_ID				0									// ��ͼλ��

// ��Ϸ��ͼ
class CGameClientView : public dex::CDirectUIWindow
{
	// ��Ϸ��Ϣ
protected:
	CString							m_StrGameText;							// ��Ϸ����
	longlong						m_lRoomLotteryPool;						// ����ʳ�
	EnumMultipleType				m_nEnumLineShow;						// ������ʾ

	// ������Ϣ
protected:
	int								m_nThreeLineMultiple[MultipleThree_Max];// 3������
	int								m_nNineLineMultiple[MultipleNine_Max];	// 9������

	// ������Ϣ
protected:
	int								m_nTaskProgress[TASK_INDEX_MAX];		// �������
	int								m_nTaskTarget[TASK_INDEX_MAX];			// ����Ŀ��
	int								m_nTaskCount[TASK_INDEX_MAX];			// ��������
	int								m_nTaskScore[TASK_INDEX_MAX];			// ������

	// ��ע��Ϣ
protected:
	int								m_nBetScore;							// ��ע����
	int								m_nBetLineCount;						// ��������
	bool							m_bBetLine[LINE_INDEX_MAX];				// ��ע����

	// Ӯ��Ϣ
protected:
	longlong						m_lPlayWinScore;						// ���Ӯ����
	bool							m_bPlayWinLine[LINE_INDEX_MAX];			// ���Ӯ����
	bool							m_bPlayWinMultiple[MultipleThree_Max + MultipleNine_Max];	// Ӯ�ı���

	// �������
public:
	uint							m_nLightBackTime;						// �������ʱ��
	dex::CLapseCount				m_LCLightBack;							// �������ʱ��
	CWHArray< CLightMove * >		m_ArrayLightBack;						// ����������

	// ��������
public:
	CWHArray< CLightMove * >		m_ArrayFlashBack;						// �����������

	// ��������
public:
	dex::CLapseCount				m_LCLightMove;							// ����ƶ���ʱ
	CWHArray< CLightMove * >		m_ArrayLightMove;						// ����ƶ�����

	// �������
public:
	dex::CD3DFont					m_D3DFontWidth12;						// ������Ϣ
	dex::CD3DFont					m_D3DFontNormal12;						// ������Ϣ

	// ��Դ����
public:
	dex::CD3DTexture				m_TextureBack1_1;						// ����ͼƬ
	dex::CD3DTexture				m_TextureBack1_2;						// ����ͼƬ
	dex::CD3DTexture				m_TextureBackFlash;						// ����ͼƬ
	dex::CD3DTexture				m_TextureBack2;							// ����ͼƬ
	dex::CD3DTexture				m_TextureLightLeft;						// ���ͼƬ
	dex::CD3DTexture				m_TextureLightRight;					// �ҹ�ͼƬ
	dex::CD3DTexture				m_TextureLightPoint;					// �Ƶ�ͼƬ
	dex::CD3DTexture				m_TextureBetScore;						// ��ע����
	dex::CD3DTexture				m_TextureLotteryPoolNumber;				// �ʳ�����	
	dex::CD3DTexture				m_TextureThreeBack;						// ��������	
	dex::CD3DTexture				m_TextureNineBack;						// ��������	
	dex::CD3DTexture				m_TextureNineAll;						// ����ȫ��
	dex::CD3DTexture				m_TextureMultipleNumber;				// ��������
	dex::CD3DTexture				m_TextureMultipleSign;					// �����˺�
	dex::CD3DTexture				m_TextureAwardBox;						// �񽱿�

	// �������
public:
	dex::CD3DTexture				m_TextureTaskNumber;					// ��������
	dex::CD3DTexture				m_TextureTaskReward;					// ���񽱽�
	dex::CD3DTexture				m_TextureTaskGrand;						// �����ۼ�
	dex::CD3DTexture				m_TextureTaskGold;						// �����

	// �������
public:
	dex::CDirectUIButton			m_ButtonStartGame;						// ��ʼ��Ϸ
	dex::CDirectUIButton			m_ButtonStopGame;						// ֹͣ��Ϸ
	dex::CDirectUIButton			m_ButtonStartHosted;					// �йܿ�ʼ
	dex::CDirectUIButton			m_ButtonStopHosted;						// �й�ֹͣ
	dex::CDirectUIButton			m_ButtonScoreLess;						// ��������
	dex::CDirectUIButton			m_ButtonScoreAdd;						// ��������
	dex::CDirectUIButton			m_ButtonScoreMax;						// �������
	dex::CDirectUIButton			m_ButtonRoad;							// ·����ʾ
	dex::CDirectUIButton			m_ButtonBank;							// ������ʾ
	dex::CDirectUIButton			m_ButtonLineCheck[LINE_INDEX_MAX];		// ��ѡ��
	dex::CDirectUIButton			m_ButtonLineUnCheck[LINE_INDEX_MAX];	// ѡδѡ��
	dex::CDirectUIButton			m_ButtonThreeLine;						// ������ť
	dex::CDirectUIButton			m_ButtonNineLine;						// ������ť

	// �������
public:
	dex::CDirectUIButton			m_ButtonHelp;							// ����
	dex::CDirectUIButton			m_ButtonSoundOn;						// ������
	dex::CDirectUIButton			m_ButtonSoundOff;						// ������
	dex::CDirectUIButton			m_ButtonWinMin;							// ������С��
	dex::CDirectUIButton			m_ButtonWinMax;							// �������
	dex::CDirectUIButton			m_ButtonWinRevert;						// �����ԭ
	dex::CDirectUIButton			m_ButtonWinClose;						// ������ر�
	dex::CDirectUIButton			m_ButtonControl;						// ���ư�ť

	// �������
public:
	CWindowEgg						m_WindowEgg;							// ��ӵ�����
	CWindowWin						m_WindowWin;							// Ӯ����
	CWindowLine						m_WindowLine;							// �߽���
	CWindowTurntable				m_WindowTurntable;						// ת�̽���
	CWindowBank						m_WindowBank;							// ����
	CWindowRoad						m_WindowRoad;							// ·������
	CWindowHelp						m_WindowHelp;							// ����
	CWHArray< dex::CDirectUIWindow * > m_ArrayWindowUnique;					// Ψһ����
	CWHArray< CWindowAward * >		m_ArrayAward;							// �н�����

	// �û���Ϣ
public:
	IClientUserItem *				m_pIClientOneSelf;						// �Լ���ָ��
	dex::CD3DTexture				m_TextureAvatar;						// ͷ������

	// ԭʼ����
public:
	CGameFrameView *				m_pGameFrameView;						// ������

	// ���ڱ���
protected:
	bool							m_bWinMax;								// ���
	DEVMODE							m_DefaultDevmode;						// ������Ϣ


	// ��������
public:
	// ���캯��
	CGameClientView( CGameFrameView * pGameFrameView );
	// ��������
	virtual ~CGameClientView();

	// ���ú���
public:
	// ���ý���
	void ResetGameView();

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

	// �滭����
public:
	// �滭���ⱳ��
	void DrawFlashBack( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// �滭����Ϣ
	void DrawLineInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ��������Ϣ
	void DrawTaskInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// �滭���
	void DrawLightMove( dex::CD3DDevice * pD3DDevice, CWHArray< CLightMove * > & pArrayLightMove );

	// ��Ϣ����
public:
	// ������� 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �Ҽ����� 
	void OnRButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �������� 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// ����̸�� 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);

	// ���ܺ���
public:
	// ������Ϸ����
	void SetGameText( CString StrGameText ) { m_StrGameText = StrGameText; }
	// �����Լ�ָ��
	void SetClientOneSelf( IClientUserItem * pIClientUserItem );
	// ���òʳ�
	void SetRoomLotteryPool( longlong lRoomLotteryPool ); 
	// ������ע��
	void SetBetScore( int nBetScore );
	// ������ע��
	void SetBetLine( bool nBetLine[LINE_INDEX_MAX] );
	// �������Ӯ����
	void SetPlayWinScore( longlong lPlayWinScore );
	// �������Ӯ����
	void SetPlayWinLine( bool bPlayWinLine[LINE_INDEX_MAX] );
	// ��������ʾ
	void SetLineShow( EnumMultipleType nEnumLineShow );
	// ���ñ���
	void SetThreeLineMultiple( int nThreeLineMultiple[MultipleThree_Max] );
	// ���ñ���
	void SetNineLineMultiple( int nNineLineMultiple[MultipleNine_Max] );
	// �����������
	void SetTaskProgress( int nTaskProgress[TASK_INDEX_MAX] );
	// ��������Ŀ��
	void SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] );
	// ������������
	void SetTaskCount( int nTaskCount[TASK_INDEX_MAX] );
	// ����������
	void SetTaskScore( int nTaskScore[TASK_INDEX_MAX] );

	// �н�����
public:
	// ����н�
	void ClearMineAward();
	// ����н�
	void AppendMineAward( int nMultipleIndex );

	// ��������
public:
	// ��ӹ�米������
	void AppendLightBack();
	// ��ӹ���ƶ�
	void AppendLightMove();
	// ����н�
	void AppendAward( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore );

	// ���ܺ���
public:
	// �رմ���
	void CloseAllWindow( dex::CDirectUIWindow * pExcludeWindow );

	// ��ť��Ϣ
public:
	// ��ʼ��ť 
	void OnClickedStartGame();
	// ֹͣ��ť 
	void OnClickedStopGame();
	// �йܿ�ʼ 
	void OnClickedStartHosted();
	// �й�ֹͣ 
	void OnClickedStopHosted();
	// �������� 
	void OnClickedScoreLess();
	// �������� 
	void OnClickedScoreAdd();
	// �������
	void OnClickedScoreMax();
	// ·�� 
	void OnClickedRoad();
	// ����
	void OnClickedBack();
	// ��ѡ��
	void OnClickedLineCheck1();
	// ��ѡ��
	void OnClickedLineCheck2();
	// ��ѡ��
	void OnClickedLineCheck3();
	// ��ѡ��
	void OnClickedLineCheck4();
	// ��ѡ��
	void OnClickedLineCheck5();
	// ��ѡ��
	void OnClickedLineCheck6();
	// ��ѡ��
	void OnClickedLineCheck7();
	// ��ѡ��
	void OnClickedLineCheck8();
	// ��ѡ��
	void OnClickedLineUnCheck1();
	// ��ѡ��
	void OnClickedLineUnCheck2();
	// ��ѡ��
	void OnClickedLineUnCheck3();
	// ��ѡ��
	void OnClickedLineUnCheck4();
	// ��ѡ��
	void OnClickedLineUnCheck5();
	// ��ѡ��
	void OnClickedLineUnCheck6();
	// ��ѡ��
	void OnClickedLineUnCheck7();
	// ��ѡ��
	void OnClickedLineUnCheck8();
	// ���߽���
	void OnClickedThreeLine();
	// ���߽���
	void OnClickedNineLine();
	// ����
	void OnClickedHelp();
	// ������
	void OnClickedSoundOn();
	// ������
	void  OnClickedSoundOff();
	// ������С��
	void OnClickedWinMin();
	// �������
	void OnClickedWinMax();
	// �����ԭ
	void OnClickedWinRevert();
	// ������ر�
	void OnClickedWinClose();
	// �򿪿���
	void OnBnClickedOpenControl();

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif