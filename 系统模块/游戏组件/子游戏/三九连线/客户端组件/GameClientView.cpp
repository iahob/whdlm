#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////////////

#define CAPTION_TOP_SIZE			30							// �����С

// ��Ϸ��ť
#define IDC_BT_START_GAME			100							// ��ʼ��ť
#define IDC_BT_STOP_GAME			101							// ֹͣ��ť
#define IDC_BT_START_HOSTED			102							// �йܿ�ʼ
#define IDC_BT_STOP_HOSTED			103							// �й�ֹͣ
#define IDC_BT_SCORE_LESS			104							// ��������
#define IDC_BT_SCORE_ADD			105							// ��������
#define IDC_BT_SCORE_MAX			106							// �������
#define IDC_BT_ROAD					107							// ·��
#define IDC_BT_BACK					108							// ����

// ������ť
#define IDC_BT_LINE_CHECK_1			111		
#define IDC_BT_LINE_CHECK_2			112	
#define IDC_BT_LINE_CHECK_3			113	
#define IDC_BT_LINE_CHECK_4			114	
#define IDC_BT_LINE_CHECK_5			115	
#define IDC_BT_LINE_CHECK_6			116	
#define IDC_BT_LINE_CHECK_7			117	
#define IDC_BT_LINE_CHECK_8			118	
#define IDC_BT_LINE_UN_CHECK_1		119		
#define IDC_BT_LINE_UN_CHECK_2		120	
#define IDC_BT_LINE_UN_CHECK_3		121	
#define IDC_BT_LINE_UN_CHECK_4		122	
#define IDC_BT_LINE_UN_CHECK_5		123	
#define IDC_BT_LINE_UN_CHECK_6		124	
#define IDC_BT_LINE_UN_CHECK_7		125	
#define IDC_BT_LINE_UN_CHECK_8		126	

// ���߰�ť
#define IDC_BT_THREE_LINE			127	
#define IDC_BT_NINE_LINE			128	

// �˵���ť
#define IDC_BT_HELP					130	
#define IDC_BT_SOUND_ON				131	
#define IDC_BT_SOUND_OFF			132	

// ϵͳ��ť
#define IDC_MIN						201							// ��С��ť
#define IDC_CLOSE					202							// �رհ�ť
#define IDC_MAX						203							// ���ť
#define IDC_REDUCTION				204							// ��ԭ��ť

// ����ID
#define IDC_BT_CONTROL              1501

// ��Ϣ����
BEGIN_WHDUI_MESSAGE_MAP(CGameClientView, dex::CDirectUIWindow)
	ON_WHDUI_WINDOW_CREATE()
	ON_WHDUI_LBUTTONDOWN()
	ON_WHDUI_RBUTTONDOWN()
	ON_WHDUI_WINDOW_MOVIE()
	ON_WHDUI_WINDOW_RENDER()
	ON_WHDUI_KEYDOWN()
	ON_WHDUI_KEYUP()
	ON_CONRTOL_BN_CLICKED( IDC_BT_START_GAME, OnClickedStartGame )
	ON_CONRTOL_BN_CLICKED( IDC_BT_STOP_GAME, OnClickedStopGame )
	ON_CONRTOL_BN_CLICKED( IDC_BT_START_HOSTED, OnClickedStartHosted )
	ON_CONRTOL_BN_CLICKED( IDC_BT_STOP_HOSTED, OnClickedStopHosted )
	ON_CONRTOL_BN_CLICKED( IDC_BT_SCORE_LESS, OnClickedScoreLess )
	ON_CONRTOL_BN_CLICKED( IDC_BT_SCORE_ADD, OnClickedScoreAdd )
	ON_CONRTOL_BN_CLICKED( IDC_BT_SCORE_MAX, OnClickedScoreMax )
	ON_CONRTOL_BN_CLICKED( IDC_BT_ROAD, OnClickedRoad )
	ON_CONRTOL_BN_CLICKED( IDC_BT_BACK, OnClickedBack )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_1, OnClickedLineCheck1 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_2, OnClickedLineCheck2 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_3, OnClickedLineCheck3 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_4, OnClickedLineCheck4 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_5, OnClickedLineCheck5 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_6, OnClickedLineCheck6 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_7, OnClickedLineCheck7 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_CHECK_8, OnClickedLineCheck8 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_1, OnClickedLineUnCheck1 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_2, OnClickedLineUnCheck2 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_3, OnClickedLineUnCheck3 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_4, OnClickedLineUnCheck4 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_5, OnClickedLineUnCheck5 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_6, OnClickedLineUnCheck6 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_7, OnClickedLineUnCheck7 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_LINE_UN_CHECK_8, OnClickedLineUnCheck8 )
	ON_CONRTOL_BN_CLICKED( IDC_BT_THREE_LINE, OnClickedThreeLine )
	ON_CONRTOL_BN_CLICKED( IDC_BT_NINE_LINE, OnClickedNineLine )
	ON_CONRTOL_BN_CLICKED( IDC_BT_HELP, OnClickedHelp )
	ON_CONRTOL_BN_CLICKED( IDC_BT_SOUND_ON, OnClickedSoundOn )
	ON_CONRTOL_BN_CLICKED( IDC_BT_SOUND_OFF, OnClickedSoundOff )
	ON_CONRTOL_BN_CLICKED( IDC_MIN, OnClickedWinMin )
	ON_CONRTOL_BN_CLICKED( IDC_MAX, OnClickedWinMax )
	ON_CONRTOL_BN_CLICKED( IDC_REDUCTION, OnClickedWinRevert )
	ON_CONRTOL_BN_CLICKED( IDC_CLOSE, OnClickedWinClose )
	ON_CONRTOL_BN_CLICKED( IDC_BT_CONTROL, OnBnClickedOpenControl )
END_WHDUI_MESSAGE_MAP()


// ���캯��
CGameClientView::CGameClientView( CGameFrameView * pGameFrameView ) : m_pGameFrameView( pGameFrameView )
{
	// ��ȡϵͳ��Ϣ
	m_bWinMax = false;
	ZeroMemory(&m_DefaultDevmode, sizeof(m_DefaultDevmode));
	m_DefaultDevmode.dmSize = sizeof(DEVMODE);
	::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_DefaultDevmode);

	// ��Ϸ��Ϣ
	m_lRoomLotteryPool = 0;
	m_nEnumLineShow = MultipleType_Three;

	// ������Ϣ
	ZeroMemory(m_nThreeLineMultiple, sizeof(m_nThreeLineMultiple));
	ZeroMemory(m_nNineLineMultiple, sizeof(m_nNineLineMultiple));

	// ������Ϣ
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	ZeroMemory(m_nTaskCount, sizeof(m_nTaskCount));
	ZeroMemory(m_nTaskScore, sizeof(m_nTaskScore));

	// ��ע��Ϣ
	m_nBetScore = 0;
	m_nBetLineCount = 0;
	ZeroMemory(m_bBetLine, sizeof(m_bBetLine));

	// Ӯ��Ϣ
	m_lPlayWinScore = 0;
	ZeroMemory(m_bPlayWinLine, sizeof(m_bPlayWinLine));
	ZeroMemory(m_bPlayWinMultiple, sizeof(m_bPlayWinMultiple));

	// �������
	m_nLightBackTime = 1000;

	// �û���Ϣ
	m_pIClientOneSelf = NULL;
}

// ��������
CGameClientView::~CGameClientView()
{
	// ɾ�����
	DeleteArray(m_ArrayLightMove);
	
	// ɾ�����
	DeleteArray(m_ArrayLightBack);

	// ɾ������
	DeleteArray(m_ArrayFlashBack);

	// �񽱶���
	DeleteArray(m_ArrayAward);

	// �ͷ�����
	CGlobal::_Object()->ReleaseTexture();
}

// ���ý���
void CGameClientView::ResetGameView()
{

}

// ������Ϣ
bool CGameClientView::OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_GR_USER_INSURE_INFO:
		{
			// Ч�����
			ASSERT(wDataSize == sizeof(CMD_GR_S_UserInsureInfo));
			if (wDataSize < sizeof(CMD_GR_S_UserInsureInfo)) return false;

			// ��������
			CMD_GR_S_UserInsureInfo * pUserInsureInfo=(CMD_GR_S_UserInsureInfo *)pData;

			// ���½���
			m_WindowBank.SetBankScore( pUserInsureInfo->lUserScore, pUserInsureInfo->lUserInsure );
		}
		break;
	case SUB_GR_USER_INSURE_SUCCESS:
		{
			// ��������
			CMD_GR_S_UserInsureSuccess *pUserInsureSuccess = (CMD_GR_S_UserInsureSuccess *)pData;
			ASSERT(wDataSize >= (sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
			if (wDataSize < (sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;


			// ���½���
			m_WindowBank.SetBankScore( pUserInsureSuccess->lUserScore, pUserInsureSuccess->lUserInsure );

			// ��ʾ��Ϣ
			if ( pUserInsureSuccess->szDescribeString[0] !=0 )
			{
				CGlobal::_Object()->MessageBox( pUserInsureSuccess->szDescribeString );
			}
		}
		break;
	case SUB_GR_USER_INSURE_FAILURE:
		{
			// ��������
			CMD_GR_S_UserInsureFailure *pUserInsureFailure = (CMD_GR_S_UserInsureFailure *)pData;
			ASSERT(wDataSize >= (sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
			if (wDataSize < (sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

			// ��ʾ��Ϣ
			if ( pUserInsureFailure->szDescribeString[0] != 0 )
			{
				CGlobal::_Object()->MessageBox( pUserInsureFailure->szDescribeString );
			}
		}
		break;
	}

	return true;
}

// ���ڴ��� 
void CGameClientView::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// ��������
	CGlobal::_Object()->CreateTexture( pD3DDevice );

	// ��������
	m_D3DFontWidth12.CreateFont( 12, 800, _T("����") );
	m_D3DFontNormal12.CreateFont( 12, 400, _T("����") );

	// ����ͼƬ
	m_TextureBack1_1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("����_1_1") );
	m_TextureBack1_2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("����_1_2") );
	m_TextureBackFlash.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("��������") );
	m_TextureBack2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("����_2") );
	m_TextureLightLeft.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("�����_���") );
	m_TextureLightRight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("�����_�ұ�") );
	m_TextureLightPoint.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("���"), _T("��Դ"), _T("�ƶ����") );
	m_TextureBetScore.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("��ע����") );
	m_TextureLotteryPoolNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�ʳ�����") );
	m_TextureThreeBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("������") );
	m_TextureNineBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("������") );
	m_TextureNineAll.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("����_ȫ��") );
	m_TextureMultipleNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("��������") );
	m_TextureMultipleSign.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("�����˺�") );
	m_TextureAwardBox.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("��_��") );
	m_TextureTaskNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("����_����") );
	m_TextureTaskReward.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("����_����") );
	m_TextureTaskGrand.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("����_�ۼ�") );
	m_TextureTaskGold.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("������"), _T("��Դ"), _T("����_��") );

	// ������ť
	m_ButtonStartGame.ActiveWindow( NULL, CRect( 663, 602, 0, 0 ), WS_VISIBLE, IDC_BT_START_GAME, this );
	m_ButtonStopGame.ActiveWindow( NULL, CRect( 663, 602, 0, 0 ), 0, IDC_BT_STOP_GAME, this );
	m_ButtonStartGame.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("��ʼ") );
	m_ButtonStopGame.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("����"), _T("��Դ"), _T("ֹͣ") );

	// �йܰ�ť
	m_ButtonStartHosted.ActiveWindow( NULL, CRect( 568, 670, 0, 0 ), WS_VISIBLE, IDC_BT_START_HOSTED, this );
	m_ButtonStopHosted.ActiveWindow( NULL, CRect( 568, 670, 0, 0 ), 0, IDC_BT_STOP_HOSTED, this );
	m_ButtonStartHosted.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("�й�") );
	m_ButtonStopHosted.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("ȡ���й�") );

	// ��������
	m_ButtonScoreLess.ActiveWindow( NULL, CRect( 262, 664, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_LESS, this );
	m_ButtonScoreAdd.ActiveWindow( NULL, CRect( 439, 664, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_ADD, this );
	m_ButtonScoreMax.ActiveWindow( NULL, CRect( 475, 670, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_MAX, this );
	m_ButtonScoreLess.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ע����") );
	m_ButtonScoreAdd.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ע����") );
	m_ButtonScoreMax.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ע���") );

	// ���ư�ť
	m_ButtonRoad.ActiveWindow( NULL, CRect( 21, 679, 0, 0 ), WS_VISIBLE, IDC_BT_ROAD, this );
	m_ButtonBank.ActiveWindow( NULL, CRect( 134, 679, 0, 0 ), WS_VISIBLE, IDC_BT_BACK, this );
	m_ButtonRoad.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("·��") );
	m_ButtonBank.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("����") );

	// ������ť
	m_ButtonLineCheck[LINE_WIDTH_1].ActiveWindow( NULL, CRect( 251, 134, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_1, this );
	m_ButtonLineCheck[LINE_WIDTH_2].ActiveWindow( NULL, CRect( 251, 282, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_2, this );
	m_ButtonLineCheck[LINE_WIDTH_3].ActiveWindow( NULL, CRect( 251, 429, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_3, this );
	m_ButtonLineCheck[LINE_HEIGHT_4].ActiveWindow( NULL, CRect( 335, 44, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_4, this );
	m_ButtonLineCheck[LINE_HEIGHT_5].ActiveWindow( NULL, CRect( 495, 44, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_5, this );
	m_ButtonLineCheck[LINE_HEIGHT_6].ActiveWindow( NULL, CRect( 655, 44, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_6, this );
	m_ButtonLineCheck[LINE_OBLIQUE_7].ActiveWindow( NULL, CRect( 742, 45, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_7, this );
	m_ButtonLineCheck[LINE_OBLIQUE_8].ActiveWindow( NULL, CRect( 743, 520, 0, 0 ), WS_VISIBLE, IDC_BT_LINE_CHECK_8, this );
	m_ButtonLineUnCheck[LINE_WIDTH_1].ActiveWindow( NULL, CRect( 251, 134, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_1, this );
	m_ButtonLineUnCheck[LINE_WIDTH_2].ActiveWindow( NULL, CRect( 251, 282, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_2, this );
	m_ButtonLineUnCheck[LINE_WIDTH_3].ActiveWindow( NULL, CRect( 251, 429, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_3, this );
	m_ButtonLineUnCheck[LINE_HEIGHT_4].ActiveWindow( NULL, CRect( 335, 44, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_4, this );
	m_ButtonLineUnCheck[LINE_HEIGHT_5].ActiveWindow( NULL, CRect( 495, 44, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_5, this );
	m_ButtonLineUnCheck[LINE_HEIGHT_6].ActiveWindow( NULL, CRect( 655, 44, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_6, this );
	m_ButtonLineUnCheck[LINE_OBLIQUE_7].ActiveWindow( NULL, CRect( 742, 45, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_7, this );
	m_ButtonLineUnCheck[LINE_OBLIQUE_8].ActiveWindow( NULL, CRect( 743, 520, 0, 0 ), 0, IDC_BT_LINE_UN_CHECK_8, this );
	m_ButtonLineCheck[LINE_WIDTH_1].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_1") );
	m_ButtonLineCheck[LINE_WIDTH_2].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_2") );
	m_ButtonLineCheck[LINE_WIDTH_3].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_3") );
	m_ButtonLineCheck[LINE_HEIGHT_4].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_4") );
	m_ButtonLineCheck[LINE_HEIGHT_5].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_5") );
	m_ButtonLineCheck[LINE_HEIGHT_6].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_6") );
	m_ButtonLineCheck[LINE_OBLIQUE_7].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_7") );
	m_ButtonLineCheck[LINE_OBLIQUE_8].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_ѡ��_8") );
	m_ButtonLineUnCheck[LINE_WIDTH_1].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_1") );
	m_ButtonLineUnCheck[LINE_WIDTH_2].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_2") );
	m_ButtonLineUnCheck[LINE_WIDTH_3].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_3") );
	m_ButtonLineUnCheck[LINE_HEIGHT_4].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_4") );
	m_ButtonLineUnCheck[LINE_HEIGHT_5].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_5") );
	m_ButtonLineUnCheck[LINE_HEIGHT_6].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_6") );
	m_ButtonLineUnCheck[LINE_OBLIQUE_7].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_7") );
	m_ButtonLineUnCheck[LINE_OBLIQUE_8].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��_δѡ��_8") );

	// ������
	m_ButtonThreeLine.ActiveWindow( NULL, CRect( 807, 70, 0, 0 ), 0, IDC_BT_THREE_LINE, this );
	m_ButtonNineLine.ActiveWindow( NULL, CRect( 913, 70, 0, 0 ), WS_VISIBLE, IDC_BT_NINE_LINE, this );
	m_ButtonThreeLine.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("������") );
	m_ButtonNineLine.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("������") );

	// ���ڰ�ť
	m_ButtonHelp.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 5, 2, 0, 0 ), WS_VISIBLE, IDC_BT_HELP, this );
	m_ButtonSoundOn.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 4, 2, 0, 0 ), WS_VISIBLE, IDC_BT_SOUND_ON, this );
	m_ButtonSoundOff.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 4, 2, 0, 0 ), 0, IDC_BT_SOUND_OFF, this );
	m_ButtonWinMin.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 3, 2, 0, 0 ), WS_VISIBLE, IDC_MIN, this );
	m_ButtonWinMax.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 2, 2, 0, 0 ), WS_VISIBLE|WS_DISABLED, IDC_MAX, this );
	m_ButtonWinRevert.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 2, 2, 0, 0 ), 0, IDC_REDUCTION, this );
	m_ButtonWinClose.ActiveWindow( NULL, CRect( nWidth - 5 - 26, 2, 0, 0 ), WS_VISIBLE, IDC_CLOSE, this );
	m_ButtonHelp.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_����") );
	m_ButtonSoundOn.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������") );
	m_ButtonSoundOff.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������") );
	m_ButtonWinMin.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������_��С��") );
	m_ButtonWinMax.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������_���") );
	m_ButtonWinRevert.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������_��ԭ") );
	m_ButtonWinClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("��ť_��Դ"), _T("��Դ"), _T("��ť_������_�ر�") );

	// ���ư�ť
	m_ButtonControl.ActiveWindow( NULL, CRect( CPoint(1024 / 2 - 80 / 2, 5), CSize(80, 20) ), 0, IDC_BT_CONTROL, this );
	m_ButtonControl.SetButtonText( _T("���Ʋ���") );

#ifdef _DEBUG
	m_ButtonControl.ShowWindow( true );
#endif
	

	// ��������
	m_WindowTurntable.ActiveWindow( NULL, CRect(273, 79, 753, 523), WS_VISIBLE|WS_DISABLED, 1001, this );
	m_WindowLine.ActiveWindow( NULL, CRect(230, 30, 800, 600), WS_VISIBLE|WS_DISABLED, 1002, this );
	m_WindowEgg.ActiveWindow( NULL, CRect(0, 0, nWidth, nHeight), WS_DISABLED, 1003, this );
	m_WindowWin.ActiveWindow( NULL, CRect(0, 0, nWidth, nHeight), WS_DISABLED, 1004, this );
	m_WindowRoad.ActiveWindow( NULL, CRect(127, 99, 0, 0), 0, 1005, this );
	m_WindowBank.ActiveWindow( NULL, CRect(262, 148, 0, 0), 0, 1006, this );
	m_WindowHelp.ActiveWindow( NULL, CRect(201, 109, 0, 0), 0, 1007, this );

	// Ψһ����
	m_ArrayWindowUnique.Add( &m_WindowRoad );
	m_ArrayWindowUnique.Add( &m_WindowBank );
	m_ArrayWindowUnique.Add( &m_WindowHelp );

	// ��ӹ��
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(269, 137), CLightMove::UnlimitedTime, 0.8f, 0.8f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(117, 124), CLightMove::UnlimitedTime, 0.8f, 0.3f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(97, 175), CLightMove::UnlimitedTime, 0.8f, 0.4f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(318, 185), CLightMove::UnlimitedTime, 0.8f, 0.9f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(750, 185), CLightMove::UnlimitedTime, 0.8f, 0.8f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(960, 175), CLightMove::UnlimitedTime, 0.8f, 0.4f ) );

	// ���ý���
	SetFocus();
}

// ���ڶ��� 
void CGameClientView::OnWindowMovie()
{
	// ����ƶ�
	int nLightMoveIndex = 0;

	// ������
	while( nLightMoveIndex < m_ArrayLightMove.GetCount() )
	{
		// ��ȡ��Ϣ
		CLightMove *& pLightMove = m_ArrayLightMove.GetAt( nLightMoveIndex );

		// �ж��ƶ�
		if( !pLightMove->Move() )
		{
			// ɾ����Ϣ
			SafeDelete(pLightMove);

			// �Ƴ���Ϣ
			m_ArrayLightMove.RemoveAt(nLightMoveIndex);
		}
		else
		{
			// ��һ��
			nLightMoveIndex++;
		}
	}

	// ����ƶ�
	int nLightBackIndex = 0;

	// ������
	while( nLightBackIndex < m_ArrayLightBack.GetCount() )
	{
		// ��ȡ��Ϣ
		CLightMove *& pLightMove = m_ArrayLightBack.GetAt( nLightBackIndex );

		// �ж��ƶ�
		if( !pLightMove->Move() )
		{
			// ɾ����Ϣ
			SafeDelete(pLightMove);

			// �Ƴ���Ϣ
			m_ArrayLightBack.RemoveAt(nLightBackIndex);
		}
		else
		{
			// ��һ��
			nLightBackIndex++;
		}
	}

	// �����ƶ�
	int nFlashBackIndex = 0;

	// ������
	while( nFlashBackIndex < m_ArrayFlashBack.GetCount() )
	{
		// ��ȡ��Ϣ
		CLightMove *& pFlashMove = m_ArrayFlashBack.GetAt( nFlashBackIndex );

		// �ж��ƶ�
		if( !pFlashMove->Move() )
		{
			// ɾ����Ϣ
			SafeDelete(pFlashMove);

			// �Ƴ���Ϣ
			m_ArrayFlashBack.RemoveAt(nFlashBackIndex);
		}
		else
		{
			// ��һ��
			nFlashBackIndex++;
		}
	}

	// �񽱴���
	int nAwardIndex = 0;

	// �񽱶���
	while( nAwardIndex < m_ArrayAward.GetCount() )
	{
		// ��ȡ��Ϣ
		CWindowAward *& pWindowAwardMove = m_ArrayAward.GetAt( nAwardIndex );

		// �ж��ƶ�
		if( !pWindowAwardMove->IsWindowVisible() && pWindowAwardMove->GetCurrentAlpha() < 0.001 )
		{
			// ɾ����Ϣ
			SafeDelete(pWindowAwardMove);

			// �Ƴ���Ϣ
			m_ArrayAward.RemoveAt(nAwardIndex);
		}
		else
		{
			// ��һ��
			nAwardIndex++;
		}
	}

	// ��ӹ�米������
	AppendLightBack();

	// ����ƶ�����
	if ( m_LCLightMove.GetLapseCount(1000 * 60) )
	{
		// ��ӹ���ƶ�
		AppendLightMove();
	}
}

// ���ڻ滭 
void CGameClientView::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭���ⱳ��
	DrawFlashBack( pD3DDevice, nWidth, nHeight );

	// �ʳػ滭
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureLotteryPoolNumber, _T("0123456789"), m_lRoomLotteryPool, 218, 60, DN_RIGHT );

	// �滭��ע
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_lPlayWinScore, 644, 617, DN_RIGHT );
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_nBetScore * m_nBetLineCount, 437, 617, DN_RIGHT );
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_nBetScore, 423, 685, DN_RIGHT );

	// �滭����Ϣ
	DrawLineInfo( pD3DDevice, nWidth, nHeight );

	// �滭������Ϣ
	DrawTaskInfo( pD3DDevice, nWidth, nHeight );

	// �滭���
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLightPoint, m_ArrayLightMove );

	// �Լ���Ϣ
	if ( m_pIClientOneSelf )
	{
		// �滭ͷ��
		m_TextureAvatar.DrawTexture( pD3DDevice, 17, 600, 58, 58, 0, 0, m_TextureAvatar.GetWidth(), m_TextureAvatar.GetHeight() );

		// �滭����
		m_D3DFontWidth12.DrawText( pD3DDevice, m_pIClientOneSelf->GetNickName(), CRect( CPoint(88, 613), CSize(122, 15) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_D3DFontWidth12.DrawText( pD3DDevice, m_pIClientOneSelf->GetNickName(), CRect( CPoint(88, 612), CSize(122, 15) ), D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

		// ��ҷ���
		m_D3DFontWidth12.DrawText( pD3DDevice, CGlobal::_Object()->AppendComma( m_pIClientOneSelf->GetUserScore() ), CRect( CPoint(88, 639), CSize(122, 15) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_D3DFontWidth12.DrawText( pD3DDevice, CGlobal::_Object()->AppendComma( m_pIClientOneSelf->GetUserScore() ), CRect( CPoint(88, 638), CSize(122, 15) ), D3DCOLOR_ARGB(255, 204, 255, 163), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	// �滭����
	m_D3DFontNormal12.DrawBorderText( pD3DDevice, m_StrGameText, 45, 8, D3DCOLOR_ARGB(255, 255, 255, 255), D3DCOLOR_ARGB(255, 102, 103, 98), DT_NOCLIP);
}

// �滭���ⱳ��
void CGameClientView::DrawFlashBack( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// �滭����
	m_TextureBack1_1.DrawTexture(pD3DDevice, 0, 0);

	// ����Ч��
	m_TextureBackFlash.SetEffectType( dex::TextureEffect_Shine );

	// �滭���
	for( int nLightIndex = 0; nLightIndex < m_ArrayFlashBack.GetCount(); ++nLightIndex )
	{
		// ��ȡ��Ϣ
		CLightMove *& pFlashMove = m_ArrayFlashBack.GetAt( nLightIndex );

		// ���̫С
		if( pFlashMove->fAlphaCurrent < 0.0001 )
			continue;

		// �滭ͼƬ
		m_TextureBackFlash.DrawTexture(pD3DDevice, (int)pFlashMove->ptPosition.x, (int)pFlashMove->ptPosition.y, (byte)(pFlashMove->fAlphaCurrent * 255.f) );
	} 

	// �滭����
	m_TextureBack1_2.DrawTexture(pD3DDevice, 0, 0);

	// �滭���
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLightPoint, m_ArrayLightBack );

	// �滭����
	m_TextureBack2.DrawTexture(pD3DDevice, 0, 0);

	// ���������
	if ( GetTickCount() / 400 % 2 == 0 )
	{
		m_TextureLightLeft.DrawTexture(pD3DDevice, 5, 35 );
		m_TextureLightRight.DrawTexture(pD3DDevice, 783, 35 );
	}
}

// �滭����Ϣ
void CGameClientView::DrawLineInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// Ԫ��λ��
	CPoint PointElement( 815, 142 );

	// Ԫ�ش�С
	CSize SizeElement( 36, 36 );

	// �滭����
	if ( m_nEnumLineShow == MultipleType_Three )
	{
		// �滭����
		m_TextureThreeBack.DrawTexture(pD3DDevice, 808, 70);

		// �滭Ԫ��
		for ( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			EnumMultipleThree nMultipleThree = (EnumMultipleThree)nIndex;
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointElement.x, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointElement.x + 35, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointElement.x + 35 * 2, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
		}

		// �滭����
		for( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			// �滭����
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureMultipleNumber, _T("0123456789"), m_nThreeLineMultiple[nIndex], 1007, PointElement.y + nIndex * 38, DN_RIGHT|DN_VCENTER );
		
			// �滭�˺�
			m_TextureMultipleSign.DrawTexture( pD3DDevice, RectNumber.left - 15, RectNumber.bottom - 14 );
		}
	}
	else if ( m_nEnumLineShow == MultipleType_Nine )
	{
		// �滭����
		m_TextureNineBack.DrawTexture(pD3DDevice, 821, 70);

		// �滭Ԫ��
		for ( int nIndex = 0; nIndex < MultipleNine_Max; ++nIndex )
		{
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleNineToTextureElement( (EnumMultipleNine)nIndex );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, 855, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			m_TextureNineAll.DrawTexture( pD3DDevice, PointElement.x, PointElement.y + nIndex * 38 - m_TextureNineAll.GetHeight() / 2 );
		}

		// �滭����
		for( int nIndex = 0; nIndex < MultipleNine_Max; ++nIndex )
		{
			// �滭����
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureMultipleNumber, _T("0123456789"), m_nNineLineMultiple[nIndex], 1007, PointElement.y + nIndex * 38, DN_RIGHT|DN_VCENTER );

			// �滭�˺�
			m_TextureMultipleSign.DrawTexture( pD3DDevice, RectNumber.left - 15, RectNumber.bottom - 14 );
		}
	}

	// �滭Ӯ�ı���
	for( int nMultipleIndex = 0; nMultipleIndex < MultipleThree_Max + MultipleNine_Max; ++nMultipleIndex )
	{
		// ��Ӯ��Ϣ
		if( !m_bPlayWinMultiple[nMultipleIndex] )
			continue;

		// ����λ��
		CPoint PointBox( PointElement.x - 6, PointElement.y - 21 );
		if( nMultipleIndex < MultipleThree_Max ) PointBox.y += nMultipleIndex * 38;
		else if ( nMultipleIndex < MultipleThree_Max + MultipleNine_Max ) PointBox.y += ( nMultipleIndex - MultipleThree_Max ) * 38;

		// �滭ͼƬ
		byte cbAlpha = (byte)(abs( (long)(GetTickCount() / 2 % 400 - 200) ) + 55);
		m_TextureAwardBox.DrawTexture( pD3DDevice, PointBox.x, PointBox.y, cbAlpha );
	}
}

// �滭������Ϣ
void CGameClientView::DrawTaskInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// Ԫ�ش�С
	CSize SizeElement( 24, 24 );
	CPoint PointTask[TASK_INDEX_MAX] = { CPoint(916, 608), CPoint(916, 654), CPoint(916, 700) };

	// ��������
	for( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		// ��׼λ��
		CPoint PointDatum(PointTask[nIndex]);

		// �滭Ԫ��
		EnumMultipleThree nMultipleThree = (EnumMultipleThree)m_nTaskTarget[nIndex];
		EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );
		CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointDatum.x - 80, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );
		CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointDatum.x - 80 + 24, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );
		CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointDatum.x - 80 + 24 * 2, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );

		// �滭����
		CString StrProgress;
		StrProgress.Format( _T("%d/%d"), m_nTaskProgress[nIndex], m_nTaskCount[nIndex] );
		CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskNumber, _T("0123456789/"), StrProgress, PointDatum.x + 86, PointDatum.y - 2, DN_RIGHT|DN_BOTTOM );
	
		// �滭����
		m_TextureTaskGrand.DrawTexture( pD3DDevice, PointDatum.x - 73, PointDatum.y + 4 );
		m_TextureTaskGold.DrawTexture( pD3DDevice, PointDatum.x + 68, PointDatum.y + 4 );
		CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskReward, _T("0123456789"), m_nTaskScore[nIndex], PointDatum.x + 15, PointDatum.y + 6, DN_CENTER );
	}
}

// ������� 
void CGameClientView::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
	// ģ�����
	if ( ptMouseSite.y <= CAPTION_TOP_SIZE )
	{
		AfxGetMainWnd()->PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(ptMouseSite.x,ptMouseSite.y) );

		return;
	}
}

// �Ҽ����� 
void CGameClientView::OnRButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite)
{
}

// �������� 
void CGameClientView::OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
{
	switch( nChar )
	{
	case VK_LEFT:
		{
			// �������� 
			OnClickedScoreLess();
		}
		break;
	case VK_RIGHT:
		{
			// �������� 
			OnClickedScoreAdd();
		}
		break;	
	}
}

// ����̸�� 
void CGameClientView::OnKeyUp(uint nChar, uint nRepCnt, uint nFlags)
{
	switch( nChar )
	{
	case VK_SPACE:
		{
			// �жϰ�ť
			if( m_ButtonStartGame.IsWindowVisible() )
			{
				// �����ť
				OnClickedStartGame();
			}
			else if ( m_ButtonStopGame.IsWindowVisible() )
			{
				// ֹͣ��ť 
				OnClickedStopGame();
			}
		}
		break;
	}
}


// �����Լ�ָ��
void CGameClientView::SetClientOneSelf( IClientUserItem * pIClientUserItem )
{
	// ����ָ��
	m_pIClientOneSelf = pIClientUserItem;
	
	// ��������
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	// �Զ���ͷ��
	if ( pUserInfo->dwCustomID != 0 && pCustomFaceInfo->dwDataSize != 0)
	{
		// ��ɫ����
		D3DCOLOR dwTempCustomFace[FACE_CX*FACE_CY] = {0};

		//��������
		for ( int nYImagePos = 0;nYImagePos < FACE_CY; nYImagePos++)
		{
			for (int nXImagePos = 0;nXImagePos < FACE_CX; nXImagePos++)
			{
				// ������ɫ
				dword dwImageTarget = nYImagePos * FACE_CX + nXImagePos;
				colorref crImageTarget = pCustomFaceInfo->dwCustomFace[dwImageTarget];

				// ��ȡ��ɫ
				byte cbColorR = GetRValue(crImageTarget);
				byte cbColorG = GetGValue(crImageTarget);
				byte cbColorB = GetBValue(crImageTarget);

				// ������ɫ
				dwTempCustomFace[dwImageTarget] = D3DCOLOR_ARGB(255, cbColorB, cbColorG, cbColorR);
			}
		}

		// ����ͼƬ
		m_TextureAvatar.LoadTexture( dex::CD3DDevice::_Object(), FACE_CX, FACE_CY, dwTempCustomFace );
	}
	else
	{
		// ��ȡ����
		ASSERT(CGlobalUnits::GetInstance()!=NULL);
		CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

		// ��ȡ��Դ
		tagAvatarResource AvatarResource;
		pGlobalUnits->m_FaceItemControlModule->GetAvatarResource(AvatarResource);

		// ������Դ
		dex::CD3DTexture TextureAvatar;
		TextureAvatar.LoadTexture( dex::CD3DDevice::_Object(), AvatarResource.hResInstance, AvatarResource.pszResource, _T("PNG") );

		// ��������
		word wD3DLineCount = TextureAvatar.GetWidth()/FACE_CX;
		word wD3DFullCount = wD3DLineCount*TextureAvatar.GetHeight()/(FACE_CY*2);

		// ��������
		word wFaceID = pUserInfo->wFaceID;
		if ( wFaceID >= wD3DFullCount ) wFaceID=0;
		INT nXImagePos = wFaceID%wD3DLineCount;
		INT nYImagePos = (wFaceID/wD3DLineCount)*2L;
		CRect RectImage( CPoint(nXImagePos*FACE_CX,nYImagePos*FACE_CY), CSize(FACE_CX,FACE_CY) ) ;

		// ����ͷ��
		m_TextureAvatar.LoadTexture( TextureAvatar, RectImage );
	}
}

// ���òʳ�
void CGameClientView::SetRoomLotteryPool( longlong lRoomLotteryPool )
{
	m_lRoomLotteryPool = lRoomLotteryPool;
}

// ������ע��
void CGameClientView::SetBetScore( int nBetScore )
{
	m_nBetScore = nBetScore;
}

// ������ע��
void CGameClientView::SetBetLine( bool bBetLine[LINE_INDEX_MAX] )
{
	// ��ֵ��Ϣ
	CopyMemory( m_bBetLine, bBetLine, sizeof(m_bBetLine) );

	// ��������
	m_nBetLineCount = 0;
	for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		m_nBetLineCount += ( m_bBetLine[nIndex] ) ? 1 : 0;
}

// �������Ӯ����
void  CGameClientView::SetPlayWinScore( longlong lPlayWinScore )
{
	m_lPlayWinScore = lPlayWinScore;
}

// �������Ӯ����
void  CGameClientView::SetPlayWinLine( bool bPlayWinLine[LINE_INDEX_MAX] )
{
	CopyMemory( m_bPlayWinLine, bPlayWinLine, sizeof(m_bPlayWinLine) );
}

// ��������ʾ
void CGameClientView::SetLineShow( EnumMultipleType nEnumLineShow )
{
	// ���ñ���
	m_nEnumLineShow = nEnumLineShow;

	// ���ð�ť
	m_ButtonThreeLine.ShowWindow( m_nEnumLineShow != MultipleType_Three );
	m_ButtonNineLine.ShowWindow( m_nEnumLineShow != MultipleType_Nine );
}

// ���ñ���
void CGameClientView::SetThreeLineMultiple( int nThreeLineMultiple[MultipleThree_Max] )
{
	CopyMemory( m_nThreeLineMultiple, nThreeLineMultiple, sizeof(m_nThreeLineMultiple) );
}

// ���ñ���
void CGameClientView::SetNineLineMultiple( int nNineLineMultiple[MultipleNine_Max] )
{
	CopyMemory( m_nNineLineMultiple, nNineLineMultiple, sizeof(m_nNineLineMultiple) );
}

// �����������
void CGameClientView::SetTaskProgress( int nTaskProgress[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskProgress, nTaskProgress, sizeof(m_nTaskProgress) );
}

// ��������Ŀ��
void CGameClientView::SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] )
{
	m_WindowWin.SetTaskTarget( nTaskTarget );
	CopyMemory( m_nTaskTarget, nTaskTarget, sizeof(m_nTaskTarget) );
}

// ������������
void CGameClientView::SetTaskCount( int nTaskCount[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskCount, nTaskCount, sizeof(m_nTaskCount) );
}

// ����������
void CGameClientView::SetTaskScore( int nTaskScore[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskScore, nTaskScore, sizeof(m_nTaskScore) );
}

// ����н�
void CGameClientView::ClearMineAward()
{
	ZeroMemory(m_bPlayWinMultiple, sizeof(m_bPlayWinMultiple));
}

// ����н�
void CGameClientView::AppendMineAward( int nMultipleIndex )
{
	// �ж���Ϣ
	if( nMultipleIndex < 0 || nMultipleIndex > MultipleThree_Max + MultipleNine_Max ) 
		return;

	// �����Ϣ
	m_bPlayWinMultiple[nMultipleIndex] = true;
}

// ��ӹ�米������
void CGameClientView::AppendLightBack()
{
	// ��Ч���
	if( m_LCLightBack.GetLapseCount(m_nLightBackTime) > 0 )
	{
		// ��������
		for( int nCount = 0; nCount < rand() % 3 + 1; ++nCount )
		{
			// ���λ��
			CDoublePoint PointLight( rand() % 103 * 10, rand()%27 * 10 + 59 );

			// ���ʱ��
			uint nLightTime = rand() % 5000 + 1601;

			// ����С
			float fLightSize = min( (float)( rand() % 8000 + 5000 ) / 10000.f, 1.f );

			// ���ɹ��
			CLightMove * pLightMove = new CLightMove( PointLight, nLightTime, 1.f, fLightSize );

			// ��ӹ��
			m_ArrayLightBack.Add( pLightMove );

			// ����ʱ��
			m_nLightBackTime = rand()%3000 + 1;
		}
	}

	// �������
	for( int nCount = 0; nCount < rand() % 3 + 1; ++nCount )
	{
		// ���λ��
		CDoublePoint PointFlash( rand() % 103 * 10, rand()%45 * 10 );

		// ���ʱ��
		uint nFlashTime = rand() % 3000 + 1601;

		// ���ɹ��
		CLightMove * pFlashMove = new CLightMove( PointFlash, nFlashTime, 1.f, 1.f );

		// ��ӹ��
		m_ArrayFlashBack.Add( pFlashMove );
	}
}

// ��ӹ���ƶ�
void CGameClientView::AppendLightMove()
{
	// ��ӹ��
	switch( rand()%4 )
	{
	case 0:
		m_ArrayLightMove.Add( new CLightMove( CDoublePoint(223, 114), CDoublePoint(223, 576), 5000, 0.8f, 0.8f ) );
		break;
	case 1:
		m_ArrayLightMove.Add( new CLightMove( CDoublePoint(801, 114), CDoublePoint(801, 576), 5000, 0.8f, 0.8f ) );
		break;
	case 2:
		m_ArrayLightMove.Add( new CLightMove( CDoublePoint(5, 582), CDoublePoint(205, 582), 3000, 1.0f, 0.6f ) );
		break;
	case 3:
		m_ArrayLightMove.Add( new CLightMove( CDoublePoint(1021, 582), CDoublePoint(829, 582), 3000, 1.0f, 0.6f ) );
		break;
	}
}

// ����н�
void CGameClientView::AppendAward( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore )
{
	// �½�����
	CWindowAward * pWindowAward = new CWindowAward;

	// ������Ϣ
	if( pWindowAward->SetAwardInfo( pszPlayName, nMultipleIndex, lAwardScore ) )
	{
		// ��������
		pWindowAward->SetLayerIndex( 0 );
		pWindowAward->ActiveWindow( NULL, CRect(8, 155, 0, 0), 0, 2000, this );
		pWindowAward->ShowWindow( true, dex::WindowShow_Size );

		// ��Ӵ���
		m_ArrayAward.InsertAt( 0, pWindowAward );

		// �ƶ�����
		for ( int nIndex = 1; nIndex < m_ArrayAward.GetCount() && nIndex < 5; ++nIndex )
		{
			// ��ȡ����
			CWindowAward * pWindowAwardMove = m_ArrayAward.GetAt( nIndex );

			// ����λ��
			CRect RectWindow;
			pWindowAwardMove->GetWindowRect( RectWindow );

			// �ƶ�����
			pWindowAwardMove->MoveWindow( CPoint(RectWindow.left, 155 + 83 * nIndex), 300 );
		}

		// ��С����
		for ( int nIndex = 5; nIndex < m_ArrayAward.GetCount(); ++nIndex )
		{
			// ��ȡ����
			CWindowAward * pWindowAwardMove = m_ArrayAward.GetAt( nIndex );

			// �رմ���
			pWindowAwardMove->ShowWindow( false, dex::WindowShow_Size );
		}
	}
	// ��Ӵ���
	else
	{
		SafeDelete( pWindowAward );
	}
}


// �رմ���
void CGameClientView::CloseAllWindow( dex::CDirectUIWindow * pExcludeWindow )
{
	// ��������
	for( int nIndex = 0; nIndex < m_ArrayWindowUnique.GetCount(); ++nIndex )
	{
		// ��ȡ����
		dex::CDirectUIWindow * pDirectUIWindow = m_ArrayWindowUnique.GetAt(nIndex);

		// �رմ���
		if( pDirectUIWindow != pExcludeWindow )
			pDirectUIWindow->ShowWindow( false, dex::WindowShow_Size );
	}
}


// ��ʼ��Ϣ 
void CGameClientView::OnClickedStartGame()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("StartLogo.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_START_GAME, 0, 0 );
}

// ֹͣ��ť 
void CGameClientView::OnClickedStopGame()
{
	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_STOP_GAME, 0, 0 );
}

// �йܿ�ʼ 
void CGameClientView::OnClickedStartHosted()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_START_HOSTED, 0, 0 );
}

// �й�ֹͣ 
void CGameClientView::OnClickedStopHosted()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_STOP_HOSTED, 0, 0 );
}

// �������� 
void CGameClientView::OnClickedScoreLess()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_LESS, 0, 0 );
}	

// �������� 
void CGameClientView::OnClickedScoreAdd()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_ADD, 0, 0 );
}	

// �������
void CGameClientView::OnClickedScoreMax()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// ������Ϣ
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_MAX, 0, 0 );
}

// ·�� 
void CGameClientView::OnClickedRoad()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	
	// �رմ���
	CloseAllWindow( &m_WindowRoad );

	// ��ʾ����
	m_WindowRoad.ShowWindow( !m_WindowRoad.IsWindowVisible(), dex::WindowShow_Size );
}

// ����
void CGameClientView::OnClickedBack()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �رմ���
	CloseAllWindow( &m_WindowBank );

	// ��ʾ����
	m_WindowBank.ShowWindow( !m_WindowBank.IsWindowVisible(), dex::WindowShow_Size );
}

// ������ѡ��
//#define LINE_CHECK

// ��ѡ��
void CGameClientView::OnClickedLineCheck1()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_1 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck2()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_2 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck3()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_3 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck4()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_4 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck5()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_5 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck6()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_6 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck7()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_OBLIQUE_7 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineCheck8()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_OBLIQUE_8 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck1()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_1 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck2()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_2 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck3()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_3 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck4()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_4 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck5()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_5 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck6()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_6 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck7()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_OBLIQUE_7 );
#endif
}

// ��ѡ��
void CGameClientView::OnClickedLineUnCheck8()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_OBLIQUE_8 );
#endif
}

// ���߽���
void CGameClientView::OnClickedThreeLine()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// �л�����
	SetLineShow( MultipleType_Three );

	// ����н�
	ClearMineAward();
}

// ���߽���
void CGameClientView::OnClickedNineLine()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// �л�����
	SetLineShow( MultipleType_Nine );

	// ����н�
	ClearMineAward();
}

// ����
void CGameClientView::OnClickedHelp()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �رմ���
	CloseAllWindow( &m_WindowHelp );

	// ��ʾ����
	m_WindowHelp.ShowWindow( !m_WindowHelp.IsWindowVisible(), dex::WindowShow_Size );
}

// ������
void CGameClientView::OnClickedSoundOn()
{
	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �ر�����
	g_GameOption.bSound = FALSE;
	g_GameOption.SaveParameter();

	// ��������
	CMusic::_Object()->StopMusic();

	// �л���ť
	m_ButtonSoundOn.ShowWindow( false, dex::WindowShow_Size );
	m_ButtonSoundOff.ShowWindow( true, dex::WindowShow_Size );
}

// ������
void CGameClientView::OnClickedSoundOff()
{
	// ��������
	g_GameOption.bSound = TRUE;
	g_GameOption.SaveParameter();

	// ��������
	CMusic::_Object()->PlayMusic();

	// ������Ч
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// �л���ť
	m_ButtonSoundOn.ShowWindow( true, dex::WindowShow_Size );
	m_ButtonSoundOff.ShowWindow( false, dex::WindowShow_Size );
}

// ������С��
void CGameClientView::OnClickedWinMin()
{
	AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
}

// �������
void CGameClientView::OnClickedWinMax()
{
	if( !m_bWinMax )
	{
		DEVMODE   lpDevMode;
		CopyMemory( &lpDevMode, &m_DefaultDevmode, sizeof(lpDevMode) );
		lpDevMode.dmPelsWidth = SCREEN_CX;									// �ֱ���XΪ1024
		lpDevMode.dmPelsHeight = SCREEN_CY;									// �ֱ���YΪ768
		lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT;
		::ChangeDisplaySettings(&lpDevMode, CDS_FULLSCREEN);
		m_bWinMax = true;

		// �ƶ�����
		AfxGetMainWnd()->SetWindowPos(NULL, 0, 0, SCREEN_CX, SCREEN_CY, SWP_NOZORDER);

		// ��ť��ʾ
		m_ButtonWinMin.EnableWindow( false );
		m_ButtonWinMax.ShowWindow( false );
		m_ButtonWinRevert.ShowWindow( true );
	}
}

// �����ԭ
void CGameClientView::OnClickedWinRevert()
{
	if( m_bWinMax )
	{
		// ��ԭĬ������
		::ChangeDisplaySettings( &m_DefaultDevmode, CDS_UPDATEREGISTRY);
		m_bWinMax = false;

		// ���ھ���
		CRect rcArce;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArce, SPIF_SENDCHANGE);

		//��ȡλ��
		CSize SizeRestrict(SCREEN_CX, SCREEN_CY);

		//����λ��
		CRect rcNormalSize;			

		//λ�õ���
		SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
		SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

		//�ƶ�����
		rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
		rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
		rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
		rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;
		AfxGetMainWnd()->SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

		// ��ť��ʾ
		m_ButtonWinMin.EnableWindow( true );
		m_ButtonWinMax.ShowWindow( true );
		m_ButtonWinRevert.ShowWindow( false );
	}
}

// ������ر�
void CGameClientView::OnClickedWinClose()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}


// �򿪿���
void CGameClientView::OnBnClickedOpenControl()
{
	m_pGameFrameView->SendEngineMessage(IDM_OPEN_CONTROL, 0, 0);
}

