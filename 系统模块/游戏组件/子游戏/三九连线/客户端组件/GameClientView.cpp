#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////////////

#define CAPTION_TOP_SIZE			30							// 标题大小

// 游戏按钮
#define IDC_BT_START_GAME			100							// 开始按钮
#define IDC_BT_STOP_GAME			101							// 停止按钮
#define IDC_BT_START_HOSTED			102							// 托管开始
#define IDC_BT_STOP_HOSTED			103							// 托管停止
#define IDC_BT_SCORE_LESS			104							// 分数减少
#define IDC_BT_SCORE_ADD			105							// 分数增加
#define IDC_BT_SCORE_MAX			106							// 分数最大
#define IDC_BT_ROAD					107							// 路单
#define IDC_BT_BACK					108							// 银行

// 线数按钮
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

// 连线按钮
#define IDC_BT_THREE_LINE			127	
#define IDC_BT_NINE_LINE			128	

// 菜单按钮
#define IDC_BT_HELP					130	
#define IDC_BT_SOUND_ON				131	
#define IDC_BT_SOUND_OFF			132	

// 系统按钮
#define IDC_MIN						201							// 最小按钮
#define IDC_CLOSE					202							// 关闭按钮
#define IDC_MAX						203							// 最大按钮
#define IDC_REDUCTION				204							// 还原按钮

// 控制ID
#define IDC_BT_CONTROL              1501

// 消息定义
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


// 构造函数
CGameClientView::CGameClientView( CGameFrameView * pGameFrameView ) : m_pGameFrameView( pGameFrameView )
{
	// 获取系统信息
	m_bWinMax = false;
	ZeroMemory(&m_DefaultDevmode, sizeof(m_DefaultDevmode));
	m_DefaultDevmode.dmSize = sizeof(DEVMODE);
	::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_DefaultDevmode);

	// 游戏信息
	m_lRoomLotteryPool = 0;
	m_nEnumLineShow = MultipleType_Three;

	// 倍数信息
	ZeroMemory(m_nThreeLineMultiple, sizeof(m_nThreeLineMultiple));
	ZeroMemory(m_nNineLineMultiple, sizeof(m_nNineLineMultiple));

	// 倍数信息
	ZeroMemory(m_nTaskProgress, sizeof(m_nTaskProgress));
	ZeroMemory(m_nTaskTarget, sizeof(m_nTaskTarget));
	ZeroMemory(m_nTaskCount, sizeof(m_nTaskCount));
	ZeroMemory(m_nTaskScore, sizeof(m_nTaskScore));

	// 下注信息
	m_nBetScore = 0;
	m_nBetLineCount = 0;
	ZeroMemory(m_bBetLine, sizeof(m_bBetLine));

	// 赢信息
	m_lPlayWinScore = 0;
	ZeroMemory(m_bPlayWinLine, sizeof(m_bPlayWinLine));
	ZeroMemory(m_bPlayWinMultiple, sizeof(m_bPlayWinMultiple));

	// 背景光电
	m_nLightBackTime = 1000;

	// 用户信息
	m_pIClientOneSelf = NULL;
}

// 析构函数
CGameClientView::~CGameClientView()
{
	// 删除光电
	DeleteArray(m_ArrayLightMove);
	
	// 删除光电
	DeleteArray(m_ArrayLightBack);

	// 删除闪光
	DeleteArray(m_ArrayFlashBack);

	// 获奖动画
	DeleteArray(m_ArrayAward);

	// 释放纹理
	CGlobal::_Object()->ReleaseTexture();
}

// 重置界面
void CGameClientView::ResetGameView()
{

}

// 银行消息
bool CGameClientView::OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_GR_USER_INSURE_INFO:
		{
			// 效验参数
			ASSERT(wDataSize == sizeof(CMD_GR_S_UserInsureInfo));
			if (wDataSize < sizeof(CMD_GR_S_UserInsureInfo)) return false;

			// 变量定义
			CMD_GR_S_UserInsureInfo * pUserInsureInfo=(CMD_GR_S_UserInsureInfo *)pData;

			// 更新界面
			m_WindowBank.SetBankScore( pUserInsureInfo->lUserScore, pUserInsureInfo->lUserInsure );
		}
		break;
	case SUB_GR_USER_INSURE_SUCCESS:
		{
			// 变量定义
			CMD_GR_S_UserInsureSuccess *pUserInsureSuccess = (CMD_GR_S_UserInsureSuccess *)pData;
			ASSERT(wDataSize >= (sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
			if (wDataSize < (sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;


			// 更新界面
			m_WindowBank.SetBankScore( pUserInsureSuccess->lUserScore, pUserInsureSuccess->lUserInsure );

			// 显示消息
			if ( pUserInsureSuccess->szDescribeString[0] !=0 )
			{
				CGlobal::_Object()->MessageBox( pUserInsureSuccess->szDescribeString );
			}
		}
		break;
	case SUB_GR_USER_INSURE_FAILURE:
		{
			// 变量定义
			CMD_GR_S_UserInsureFailure *pUserInsureFailure = (CMD_GR_S_UserInsureFailure *)pData;
			ASSERT(wDataSize >= (sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
			if (wDataSize < (sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

			// 显示消息
			if ( pUserInsureFailure->szDescribeString[0] != 0 )
			{
				CGlobal::_Object()->MessageBox( pUserInsureFailure->szDescribeString );
			}
		}
		break;
	}

	return true;
}

// 窗口创建 
void CGameClientView::OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 创建纹理
	CGlobal::_Object()->CreateTexture( pD3DDevice );

	// 创建字体
	m_D3DFontWidth12.CreateFont( 12, 800, _T("宋体") );
	m_D3DFontNormal12.CreateFont( 12, 400, _T("宋体") );

	// 加载图片
	m_TextureBack1_1.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("背景_1_1") );
	m_TextureBack1_2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("背景_1_2") );
	m_TextureBackFlash.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("背景闪光") );
	m_TextureBack2.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("背景_2") );
	m_TextureLightLeft.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("跑马灯_左边") );
	m_TextureLightRight.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("跑马灯_右边") );
	m_TextureLightPoint.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("光电"), _T("资源"), _T("移动光电") );
	m_TextureBetScore.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("下注分数") );
	m_TextureLotteryPoolNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("彩池数字") );
	m_TextureThreeBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("三连奖") );
	m_TextureNineBack.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("九连奖") );
	m_TextureNineAll.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("九连_全部") );
	m_TextureMultipleNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("倍数数字") );
	m_TextureMultipleSign.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("倍数乘号") );
	m_TextureAwardBox.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("获奖_框") );
	m_TextureTaskNumber.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("任务_数字") );
	m_TextureTaskReward.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("任务_奖励") );
	m_TextureTaskGrand.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("任务_累计") );
	m_TextureTaskGold.LoadWHGTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("主界面"), _T("资源"), _T("任务_金") );

	// 创建按钮
	m_ButtonStartGame.ActiveWindow( NULL, CRect( 663, 602, 0, 0 ), WS_VISIBLE, IDC_BT_START_GAME, this );
	m_ButtonStopGame.ActiveWindow( NULL, CRect( 663, 602, 0, 0 ), 0, IDC_BT_STOP_GAME, this );
	m_ButtonStartGame.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("开始") );
	m_ButtonStopGame.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("背景"), _T("资源"), _T("停止") );

	// 托管按钮
	m_ButtonStartHosted.ActiveWindow( NULL, CRect( 568, 670, 0, 0 ), WS_VISIBLE, IDC_BT_START_HOSTED, this );
	m_ButtonStopHosted.ActiveWindow( NULL, CRect( 568, 670, 0, 0 ), 0, IDC_BT_STOP_HOSTED, this );
	m_ButtonStartHosted.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("托管") );
	m_ButtonStopHosted.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("取消托管") );

	// 分数控制
	m_ButtonScoreLess.ActiveWindow( NULL, CRect( 262, 664, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_LESS, this );
	m_ButtonScoreAdd.ActiveWindow( NULL, CRect( 439, 664, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_ADD, this );
	m_ButtonScoreMax.ActiveWindow( NULL, CRect( 475, 670, 0, 0 ), WS_VISIBLE, IDC_BT_SCORE_MAX, this );
	m_ButtonScoreLess.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("下注减少") );
	m_ButtonScoreAdd.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("下注增加") );
	m_ButtonScoreMax.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("下注最大") );

	// 控制按钮
	m_ButtonRoad.ActiveWindow( NULL, CRect( 21, 679, 0, 0 ), WS_VISIBLE, IDC_BT_ROAD, this );
	m_ButtonBank.ActiveWindow( NULL, CRect( 134, 679, 0, 0 ), WS_VISIBLE, IDC_BT_BACK, this );
	m_ButtonRoad.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("路单") );
	m_ButtonBank.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("银行") );

	// 线数按钮
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
	m_ButtonLineCheck[LINE_WIDTH_1].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_1") );
	m_ButtonLineCheck[LINE_WIDTH_2].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_2") );
	m_ButtonLineCheck[LINE_WIDTH_3].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_3") );
	m_ButtonLineCheck[LINE_HEIGHT_4].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_4") );
	m_ButtonLineCheck[LINE_HEIGHT_5].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_5") );
	m_ButtonLineCheck[LINE_HEIGHT_6].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_6") );
	m_ButtonLineCheck[LINE_OBLIQUE_7].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_7") );
	m_ButtonLineCheck[LINE_OBLIQUE_8].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_选中_8") );
	m_ButtonLineUnCheck[LINE_WIDTH_1].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_1") );
	m_ButtonLineUnCheck[LINE_WIDTH_2].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_2") );
	m_ButtonLineUnCheck[LINE_WIDTH_3].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_3") );
	m_ButtonLineUnCheck[LINE_HEIGHT_4].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_4") );
	m_ButtonLineUnCheck[LINE_HEIGHT_5].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_5") );
	m_ButtonLineUnCheck[LINE_HEIGHT_6].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_6") );
	m_ButtonLineUnCheck[LINE_OBLIQUE_7].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_7") );
	m_ButtonLineUnCheck[LINE_OBLIQUE_8].SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("线_未选中_8") );

	// 连奖按
	m_ButtonThreeLine.ActiveWindow( NULL, CRect( 807, 70, 0, 0 ), 0, IDC_BT_THREE_LINE, this );
	m_ButtonNineLine.ActiveWindow( NULL, CRect( 913, 70, 0, 0 ), WS_VISIBLE, IDC_BT_NINE_LINE, this );
	m_ButtonThreeLine.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("三连奖") );
	m_ButtonNineLine.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("九连奖") );

	// 窗口按钮
	m_ButtonHelp.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 5, 2, 0, 0 ), WS_VISIBLE, IDC_BT_HELP, this );
	m_ButtonSoundOn.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 4, 2, 0, 0 ), WS_VISIBLE, IDC_BT_SOUND_ON, this );
	m_ButtonSoundOff.ActiveWindow( NULL, CRect( nWidth - 10 - 26 * 4, 2, 0, 0 ), 0, IDC_BT_SOUND_OFF, this );
	m_ButtonWinMin.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 3, 2, 0, 0 ), WS_VISIBLE, IDC_MIN, this );
	m_ButtonWinMax.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 2, 2, 0, 0 ), WS_VISIBLE|WS_DISABLED, IDC_MAX, this );
	m_ButtonWinRevert.ActiveWindow( NULL, CRect( nWidth - 5 - 26 * 2, 2, 0, 0 ), 0, IDC_REDUCTION, this );
	m_ButtonWinClose.ActiveWindow( NULL, CRect( nWidth - 5 - 26, 2, 0, 0 ), WS_VISIBLE, IDC_CLOSE, this );
	m_ButtonHelp.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_帮助") );
	m_ButtonSoundOn.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_声音开") );
	m_ButtonSoundOff.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_声音关") );
	m_ButtonWinMin.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_主窗口_最小化") );
	m_ButtonWinMax.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_主窗口_最大化") );
	m_ButtonWinRevert.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_主窗口_还原") );
	m_ButtonWinClose.SetButtonTexture( pD3DDevice, CGlobal::_Object()->hModuleResource, _T("按钮_资源"), _T("资源"), _T("按钮_主窗口_关闭") );

	// 控制按钮
	m_ButtonControl.ActiveWindow( NULL, CRect( CPoint(1024 / 2 - 80 / 2, 5), CSize(80, 20) ), 0, IDC_BT_CONTROL, this );
	m_ButtonControl.SetButtonText( _T("控制操作") );

#ifdef _DEBUG
	m_ButtonControl.ShowWindow( true );
#endif
	

	// 创建窗口
	m_WindowTurntable.ActiveWindow( NULL, CRect(273, 79, 753, 523), WS_VISIBLE|WS_DISABLED, 1001, this );
	m_WindowLine.ActiveWindow( NULL, CRect(230, 30, 800, 600), WS_VISIBLE|WS_DISABLED, 1002, this );
	m_WindowEgg.ActiveWindow( NULL, CRect(0, 0, nWidth, nHeight), WS_DISABLED, 1003, this );
	m_WindowWin.ActiveWindow( NULL, CRect(0, 0, nWidth, nHeight), WS_DISABLED, 1004, this );
	m_WindowRoad.ActiveWindow( NULL, CRect(127, 99, 0, 0), 0, 1005, this );
	m_WindowBank.ActiveWindow( NULL, CRect(262, 148, 0, 0), 0, 1006, this );
	m_WindowHelp.ActiveWindow( NULL, CRect(201, 109, 0, 0), 0, 1007, this );

	// 唯一界面
	m_ArrayWindowUnique.Add( &m_WindowRoad );
	m_ArrayWindowUnique.Add( &m_WindowBank );
	m_ArrayWindowUnique.Add( &m_WindowHelp );

	// 添加光电
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(269, 137), CLightMove::UnlimitedTime, 0.8f, 0.8f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(117, 124), CLightMove::UnlimitedTime, 0.8f, 0.3f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(97, 175), CLightMove::UnlimitedTime, 0.8f, 0.4f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(318, 185), CLightMove::UnlimitedTime, 0.8f, 0.9f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(750, 185), CLightMove::UnlimitedTime, 0.8f, 0.8f ) );
	m_ArrayLightBack.Add( new CLightMove( CDoublePoint(960, 175), CLightMove::UnlimitedTime, 0.8f, 0.4f ) );

	// 设置焦点
	SetFocus();
}

// 窗口动画 
void CGameClientView::OnWindowMovie()
{
	// 光电移动
	int nLightMoveIndex = 0;

	// 编译光电
	while( nLightMoveIndex < m_ArrayLightMove.GetCount() )
	{
		// 获取信息
		CLightMove *& pLightMove = m_ArrayLightMove.GetAt( nLightMoveIndex );

		// 判断移动
		if( !pLightMove->Move() )
		{
			// 删除信息
			SafeDelete(pLightMove);

			// 移除信息
			m_ArrayLightMove.RemoveAt(nLightMoveIndex);
		}
		else
		{
			// 下一个
			nLightMoveIndex++;
		}
	}

	// 光电移动
	int nLightBackIndex = 0;

	// 编译光电
	while( nLightBackIndex < m_ArrayLightBack.GetCount() )
	{
		// 获取信息
		CLightMove *& pLightMove = m_ArrayLightBack.GetAt( nLightBackIndex );

		// 判断移动
		if( !pLightMove->Move() )
		{
			// 删除信息
			SafeDelete(pLightMove);

			// 移除信息
			m_ArrayLightBack.RemoveAt(nLightBackIndex);
		}
		else
		{
			// 下一个
			nLightBackIndex++;
		}
	}

	// 闪光移动
	int nFlashBackIndex = 0;

	// 编译光电
	while( nFlashBackIndex < m_ArrayFlashBack.GetCount() )
	{
		// 获取信息
		CLightMove *& pFlashMove = m_ArrayFlashBack.GetAt( nFlashBackIndex );

		// 判断移动
		if( !pFlashMove->Move() )
		{
			// 删除信息
			SafeDelete(pFlashMove);

			// 移除信息
			m_ArrayFlashBack.RemoveAt(nFlashBackIndex);
		}
		else
		{
			// 下一个
			nFlashBackIndex++;
		}
	}

	// 获奖窗口
	int nAwardIndex = 0;

	// 获奖动画
	while( nAwardIndex < m_ArrayAward.GetCount() )
	{
		// 获取信息
		CWindowAward *& pWindowAwardMove = m_ArrayAward.GetAt( nAwardIndex );

		// 判断移动
		if( !pWindowAwardMove->IsWindowVisible() && pWindowAwardMove->GetCurrentAlpha() < 0.001 )
		{
			// 删除信息
			SafeDelete(pWindowAwardMove);

			// 移除信息
			m_ArrayAward.RemoveAt(nAwardIndex);
		}
		else
		{
			// 下一个
			nAwardIndex++;
		}
	}

	// 添加光电背景闪光
	AppendLightBack();

	// 添加移动闪光
	if ( m_LCLightMove.GetLapseCount(1000 * 60) )
	{
		// 添加光电移动
		AppendLightMove();
	}
}

// 窗口绘画 
void CGameClientView::OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画闪光背景
	DrawFlashBack( pD3DDevice, nWidth, nHeight );

	// 彩池绘画
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureLotteryPoolNumber, _T("0123456789"), m_lRoomLotteryPool, 218, 60, DN_RIGHT );

	// 绘画下注
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_lPlayWinScore, 644, 617, DN_RIGHT );
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_nBetScore * m_nBetLineCount, 437, 617, DN_RIGHT );
	CGlobal::_Object()->DrawNumber(pD3DDevice, m_TextureBetScore, _T("0123456789"), m_nBetScore, 423, 685, DN_RIGHT );

	// 绘画线信息
	DrawLineInfo( pD3DDevice, nWidth, nHeight );

	// 绘画任务信息
	DrawTaskInfo( pD3DDevice, nWidth, nHeight );

	// 绘画光电
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLightPoint, m_ArrayLightMove );

	// 自己信息
	if ( m_pIClientOneSelf )
	{
		// 绘画头像
		m_TextureAvatar.DrawTexture( pD3DDevice, 17, 600, 58, 58, 0, 0, m_TextureAvatar.GetWidth(), m_TextureAvatar.GetHeight() );

		// 绘画名字
		m_D3DFontWidth12.DrawText( pD3DDevice, m_pIClientOneSelf->GetNickName(), CRect( CPoint(88, 613), CSize(122, 15) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_D3DFontWidth12.DrawText( pD3DDevice, m_pIClientOneSelf->GetNickName(), CRect( CPoint(88, 612), CSize(122, 15) ), D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

		// 玩家分数
		m_D3DFontWidth12.DrawText( pD3DDevice, CGlobal::_Object()->AppendComma( m_pIClientOneSelf->GetUserScore() ), CRect( CPoint(88, 639), CSize(122, 15) ), D3DCOLOR_ARGB(255, 18, 15, 5), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_D3DFontWidth12.DrawText( pD3DDevice, CGlobal::_Object()->AppendComma( m_pIClientOneSelf->GetUserScore() ), CRect( CPoint(88, 638), CSize(122, 15) ), D3DCOLOR_ARGB(255, 204, 255, 163), DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	// 绘画标题
	m_D3DFontNormal12.DrawBorderText( pD3DDevice, m_StrGameText, 45, 8, D3DCOLOR_ARGB(255, 255, 255, 255), D3DCOLOR_ARGB(255, 102, 103, 98), DT_NOCLIP);
}

// 绘画闪光背景
void CGameClientView::DrawFlashBack( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 绘画背景
	m_TextureBack1_1.DrawTexture(pD3DDevice, 0, 0);

	// 设置效果
	m_TextureBackFlash.SetEffectType( dex::TextureEffect_Shine );

	// 绘画光电
	for( int nLightIndex = 0; nLightIndex < m_ArrayFlashBack.GetCount(); ++nLightIndex )
	{
		// 获取信息
		CLightMove *& pFlashMove = m_ArrayFlashBack.GetAt( nLightIndex );

		// 光电太小
		if( pFlashMove->fAlphaCurrent < 0.0001 )
			continue;

		// 绘画图片
		m_TextureBackFlash.DrawTexture(pD3DDevice, (int)pFlashMove->ptPosition.x, (int)pFlashMove->ptPosition.y, (byte)(pFlashMove->fAlphaCurrent * 255.f) );
	} 

	// 绘画背景
	m_TextureBack1_2.DrawTexture(pD3DDevice, 0, 0);

	// 绘画光电
	CGlobal::_Object()->DrawLightMove( pD3DDevice, m_TextureLightPoint, m_ArrayLightBack );

	// 绘画背景
	m_TextureBack2.DrawTexture(pD3DDevice, 0, 0);

	// 背景跑马灯
	if ( GetTickCount() / 400 % 2 == 0 )
	{
		m_TextureLightLeft.DrawTexture(pD3DDevice, 5, 35 );
		m_TextureLightRight.DrawTexture(pD3DDevice, 783, 35 );
	}
}

// 绘画线信息
void CGameClientView::DrawLineInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 元素位置
	CPoint PointElement( 815, 142 );

	// 元素大小
	CSize SizeElement( 36, 36 );

	// 绘画三九
	if ( m_nEnumLineShow == MultipleType_Three )
	{
		// 绘画背景
		m_TextureThreeBack.DrawTexture(pD3DDevice, 808, 70);

		// 绘画元素
		for ( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			EnumMultipleThree nMultipleThree = (EnumMultipleThree)nIndex;
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointElement.x, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointElement.x + 35, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointElement.x + 35 * 2, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
		}

		// 绘画数字
		for( int nIndex = 0; nIndex < MultipleThree_Max; ++nIndex )
		{
			// 绘画数字
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureMultipleNumber, _T("0123456789"), m_nThreeLineMultiple[nIndex], 1007, PointElement.y + nIndex * 38, DN_RIGHT|DN_VCENTER );
		
			// 绘画乘号
			m_TextureMultipleSign.DrawTexture( pD3DDevice, RectNumber.left - 15, RectNumber.bottom - 14 );
		}
	}
	else if ( m_nEnumLineShow == MultipleType_Nine )
	{
		// 绘画背景
		m_TextureNineBack.DrawTexture(pD3DDevice, 821, 70);

		// 绘画元素
		for ( int nIndex = 0; nIndex < MultipleNine_Max; ++nIndex )
		{
			EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleNineToTextureElement( (EnumMultipleNine)nIndex );
			CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, 855, PointElement.y + nIndex * 38 - SizeElement.cy / 2, SizeElement.cx, SizeElement.cy );
			m_TextureNineAll.DrawTexture( pD3DDevice, PointElement.x, PointElement.y + nIndex * 38 - m_TextureNineAll.GetHeight() / 2 );
		}

		// 绘画数字
		for( int nIndex = 0; nIndex < MultipleNine_Max; ++nIndex )
		{
			// 绘画数字
			CRect RectNumber = CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureMultipleNumber, _T("0123456789"), m_nNineLineMultiple[nIndex], 1007, PointElement.y + nIndex * 38, DN_RIGHT|DN_VCENTER );

			// 绘画乘号
			m_TextureMultipleSign.DrawTexture( pD3DDevice, RectNumber.left - 15, RectNumber.bottom - 14 );
		}
	}

	// 绘画赢的倍数
	for( int nMultipleIndex = 0; nMultipleIndex < MultipleThree_Max + MultipleNine_Max; ++nMultipleIndex )
	{
		// 无赢信息
		if( !m_bPlayWinMultiple[nMultipleIndex] )
			continue;

		// 计算位置
		CPoint PointBox( PointElement.x - 6, PointElement.y - 21 );
		if( nMultipleIndex < MultipleThree_Max ) PointBox.y += nMultipleIndex * 38;
		else if ( nMultipleIndex < MultipleThree_Max + MultipleNine_Max ) PointBox.y += ( nMultipleIndex - MultipleThree_Max ) * 38;

		// 绘画图片
		byte cbAlpha = (byte)(abs( (long)(GetTickCount() / 2 % 400 - 200) ) + 55);
		m_TextureAwardBox.DrawTexture( pD3DDevice, PointBox.x, PointBox.y, cbAlpha );
	}
}

// 绘画任务信息
void CGameClientView::DrawTaskInfo( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight )
{
	// 元素大小
	CSize SizeElement( 24, 24 );
	CPoint PointTask[TASK_INDEX_MAX] = { CPoint(916, 608), CPoint(916, 654), CPoint(916, 700) };

	// 遍历任务
	for( int nIndex = 0; nIndex < TASK_INDEX_MAX; ++nIndex )
	{
		// 基准位置
		CPoint PointDatum(PointTask[nIndex]);

		// 绘画元素
		EnumMultipleThree nMultipleThree = (EnumMultipleThree)m_nTaskTarget[nIndex];
		EnumTextureElement nTextureElement = CGlobal::_Object()->MultipleThreeToTextureElement( nMultipleThree );
		CGlobal::_Object()->DrawElement( pD3DDevice, nTextureElement, PointDatum.x - 80, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );
		CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointDatum.x - 80 + 24, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );
		CGlobal::_Object()->DrawElement( pD3DDevice, ( nMultipleThree == MultipleThree_II || nMultipleThree == MultipleThree_I ) ?  TextureElement_Any : nTextureElement, PointDatum.x - 80 + 24 * 2, PointDatum.y - 22, SizeElement.cx, SizeElement.cy );

		// 绘画任务
		CString StrProgress;
		StrProgress.Format( _T("%d/%d"), m_nTaskProgress[nIndex], m_nTaskCount[nIndex] );
		CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskNumber, _T("0123456789/"), StrProgress, PointDatum.x + 86, PointDatum.y - 2, DN_RIGHT|DN_BOTTOM );
	
		// 绘画任务
		m_TextureTaskGrand.DrawTexture( pD3DDevice, PointDatum.x - 73, PointDatum.y + 4 );
		m_TextureTaskGold.DrawTexture( pD3DDevice, PointDatum.x + 68, PointDatum.y + 4 );
		CGlobal::_Object()->DrawNumber( pD3DDevice, m_TextureTaskReward, _T("0123456789"), m_nTaskScore[nIndex], PointDatum.x + 15, PointDatum.y + 6, DN_CENTER );
	}
}

// 左键点下 
void CGameClientView::OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite )
{
	// 模拟标题
	if ( ptMouseSite.y <= CAPTION_TOP_SIZE )
	{
		AfxGetMainWnd()->PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(ptMouseSite.x,ptMouseSite.y) );

		return;
	}
}

// 右键点下 
void CGameClientView::OnRButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite)
{
}

// 按键点下 
void CGameClientView::OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
{
	switch( nChar )
	{
	case VK_LEFT:
		{
			// 分数减少 
			OnClickedScoreLess();
		}
		break;
	case VK_RIGHT:
		{
			// 分数增加 
			OnClickedScoreAdd();
		}
		break;	
	}
}

// 按键谈起 
void CGameClientView::OnKeyUp(uint nChar, uint nRepCnt, uint nFlags)
{
	switch( nChar )
	{
	case VK_SPACE:
		{
			// 判断按钮
			if( m_ButtonStartGame.IsWindowVisible() )
			{
				// 点击按钮
				OnClickedStartGame();
			}
			else if ( m_ButtonStopGame.IsWindowVisible() )
			{
				// 停止按钮 
				OnClickedStopGame();
			}
		}
		break;
	}
}


// 设置自己指针
void CGameClientView::SetClientOneSelf( IClientUserItem * pIClientUserItem )
{
	// 设置指针
	m_pIClientOneSelf = pIClientUserItem;
	
	// 变量定义
	tagUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	// 自定义头像
	if ( pUserInfo->dwCustomID != 0 && pCustomFaceInfo->dwDataSize != 0)
	{
		// 颜色定义
		D3DCOLOR dwTempCustomFace[FACE_CX*FACE_CY] = {0};

		//创建区域
		for ( int nYImagePos = 0;nYImagePos < FACE_CY; nYImagePos++)
		{
			for (int nXImagePos = 0;nXImagePos < FACE_CX; nXImagePos++)
			{
				// 设置颜色
				dword dwImageTarget = nYImagePos * FACE_CX + nXImagePos;
				colorref crImageTarget = pCustomFaceInfo->dwCustomFace[dwImageTarget];

				// 提取颜色
				byte cbColorR = GetRValue(crImageTarget);
				byte cbColorG = GetGValue(crImageTarget);
				byte cbColorB = GetBValue(crImageTarget);

				// 设置颜色
				dwTempCustomFace[dwImageTarget] = D3DCOLOR_ARGB(255, cbColorB, cbColorG, cbColorR);
			}
		}

		// 载入图片
		m_TextureAvatar.LoadTexture( dex::CD3DDevice::_Object(), FACE_CX, FACE_CY, dwTempCustomFace );
	}
	else
	{
		// 获取对象
		ASSERT(CGlobalUnits::GetInstance()!=NULL);
		CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

		// 获取资源
		tagAvatarResource AvatarResource;
		pGlobalUnits->m_FaceItemControlModule->GetAvatarResource(AvatarResource);

		// 加载资源
		dex::CD3DTexture TextureAvatar;
		TextureAvatar.LoadTexture( dex::CD3DDevice::_Object(), AvatarResource.hResInstance, AvatarResource.pszResource, _T("PNG") );

		// 计算数量
		word wD3DLineCount = TextureAvatar.GetWidth()/FACE_CX;
		word wD3DFullCount = wD3DLineCount*TextureAvatar.GetHeight()/(FACE_CY*2);

		// 计算区域
		word wFaceID = pUserInfo->wFaceID;
		if ( wFaceID >= wD3DFullCount ) wFaceID=0;
		INT nXImagePos = wFaceID%wD3DLineCount;
		INT nYImagePos = (wFaceID/wD3DLineCount)*2L;
		CRect RectImage( CPoint(nXImagePos*FACE_CX,nYImagePos*FACE_CY), CSize(FACE_CX,FACE_CY) ) ;

		// 加载头像
		m_TextureAvatar.LoadTexture( TextureAvatar, RectImage );
	}
}

// 设置彩池
void CGameClientView::SetRoomLotteryPool( longlong lRoomLotteryPool )
{
	m_lRoomLotteryPool = lRoomLotteryPool;
}

// 设置下注数
void CGameClientView::SetBetScore( int nBetScore )
{
	m_nBetScore = nBetScore;
}

// 设置下注线
void CGameClientView::SetBetLine( bool bBetLine[LINE_INDEX_MAX] )
{
	// 赋值信息
	CopyMemory( m_bBetLine, bBetLine, sizeof(m_bBetLine) );

	// 计算线数
	m_nBetLineCount = 0;
	for( int nIndex = 0; nIndex < LINE_INDEX_MAX; ++nIndex )
		m_nBetLineCount += ( m_bBetLine[nIndex] ) ? 1 : 0;
}

// 设置玩家赢分数
void  CGameClientView::SetPlayWinScore( longlong lPlayWinScore )
{
	m_lPlayWinScore = lPlayWinScore;
}

// 设置玩家赢线数
void  CGameClientView::SetPlayWinLine( bool bPlayWinLine[LINE_INDEX_MAX] )
{
	CopyMemory( m_bPlayWinLine, bPlayWinLine, sizeof(m_bPlayWinLine) );
}

// 设置线显示
void CGameClientView::SetLineShow( EnumMultipleType nEnumLineShow )
{
	// 设置变量
	m_nEnumLineShow = nEnumLineShow;

	// 设置按钮
	m_ButtonThreeLine.ShowWindow( m_nEnumLineShow != MultipleType_Three );
	m_ButtonNineLine.ShowWindow( m_nEnumLineShow != MultipleType_Nine );
}

// 设置倍数
void CGameClientView::SetThreeLineMultiple( int nThreeLineMultiple[MultipleThree_Max] )
{
	CopyMemory( m_nThreeLineMultiple, nThreeLineMultiple, sizeof(m_nThreeLineMultiple) );
}

// 设置倍数
void CGameClientView::SetNineLineMultiple( int nNineLineMultiple[MultipleNine_Max] )
{
	CopyMemory( m_nNineLineMultiple, nNineLineMultiple, sizeof(m_nNineLineMultiple) );
}

// 设置任务进度
void CGameClientView::SetTaskProgress( int nTaskProgress[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskProgress, nTaskProgress, sizeof(m_nTaskProgress) );
}

// 设置任务目标
void CGameClientView::SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] )
{
	m_WindowWin.SetTaskTarget( nTaskTarget );
	CopyMemory( m_nTaskTarget, nTaskTarget, sizeof(m_nTaskTarget) );
}

// 设置任务数量
void CGameClientView::SetTaskCount( int nTaskCount[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskCount, nTaskCount, sizeof(m_nTaskCount) );
}

// 设置任务金币
void CGameClientView::SetTaskScore( int nTaskScore[TASK_INDEX_MAX] )
{
	CopyMemory( m_nTaskScore, nTaskScore, sizeof(m_nTaskScore) );
}

// 清空中奖
void CGameClientView::ClearMineAward()
{
	ZeroMemory(m_bPlayWinMultiple, sizeof(m_bPlayWinMultiple));
}

// 添加中奖
void CGameClientView::AppendMineAward( int nMultipleIndex )
{
	// 判断信息
	if( nMultipleIndex < 0 || nMultipleIndex > MultipleThree_Max + MultipleNine_Max ) 
		return;

	// 添加信息
	m_bPlayWinMultiple[nMultipleIndex] = true;
}

// 添加光电背景闪光
void CGameClientView::AppendLightBack()
{
	// 有效光电
	if( m_LCLightBack.GetLapseCount(m_nLightBackTime) > 0 )
	{
		// 创建数量
		for( int nCount = 0; nCount < rand() % 3 + 1; ++nCount )
		{
			// 光电位置
			CDoublePoint PointLight( rand() % 103 * 10, rand()%27 * 10 + 59 );

			// 光电时间
			uint nLightTime = rand() % 5000 + 1601;

			// 光电大小
			float fLightSize = min( (float)( rand() % 8000 + 5000 ) / 10000.f, 1.f );

			// 生成光电
			CLightMove * pLightMove = new CLightMove( PointLight, nLightTime, 1.f, fLightSize );

			// 添加光电
			m_ArrayLightBack.Add( pLightMove );

			// 更新时间
			m_nLightBackTime = rand()%3000 + 1;
		}
	}

	// 添加闪光
	for( int nCount = 0; nCount < rand() % 3 + 1; ++nCount )
	{
		// 光电位置
		CDoublePoint PointFlash( rand() % 103 * 10, rand()%45 * 10 );

		// 光电时间
		uint nFlashTime = rand() % 3000 + 1601;

		// 生成光电
		CLightMove * pFlashMove = new CLightMove( PointFlash, nFlashTime, 1.f, 1.f );

		// 添加光电
		m_ArrayFlashBack.Add( pFlashMove );
	}
}

// 添加光电移动
void CGameClientView::AppendLightMove()
{
	// 添加光电
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

// 添加中奖
void CGameClientView::AppendAward( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore )
{
	// 新建窗口
	CWindowAward * pWindowAward = new CWindowAward;

	// 设置信息
	if( pWindowAward->SetAwardInfo( pszPlayName, nMultipleIndex, lAwardScore ) )
	{
		// 创建窗口
		pWindowAward->SetLayerIndex( 0 );
		pWindowAward->ActiveWindow( NULL, CRect(8, 155, 0, 0), 0, 2000, this );
		pWindowAward->ShowWindow( true, dex::WindowShow_Size );

		// 添加窗口
		m_ArrayAward.InsertAt( 0, pWindowAward );

		// 移动动画
		for ( int nIndex = 1; nIndex < m_ArrayAward.GetCount() && nIndex < 5; ++nIndex )
		{
			// 获取窗口
			CWindowAward * pWindowAwardMove = m_ArrayAward.GetAt( nIndex );

			// 窗口位置
			CRect RectWindow;
			pWindowAwardMove->GetWindowRect( RectWindow );

			// 移动窗口
			pWindowAwardMove->MoveWindow( CPoint(RectWindow.left, 155 + 83 * nIndex), 300 );
		}

		// 缩小动画
		for ( int nIndex = 5; nIndex < m_ArrayAward.GetCount(); ++nIndex )
		{
			// 获取窗口
			CWindowAward * pWindowAwardMove = m_ArrayAward.GetAt( nIndex );

			// 关闭窗口
			pWindowAwardMove->ShowWindow( false, dex::WindowShow_Size );
		}
	}
	// 添加错误
	else
	{
		SafeDelete( pWindowAward );
	}
}


// 关闭窗口
void CGameClientView::CloseAllWindow( dex::CDirectUIWindow * pExcludeWindow )
{
	// 遍历窗口
	for( int nIndex = 0; nIndex < m_ArrayWindowUnique.GetCount(); ++nIndex )
	{
		// 获取窗口
		dex::CDirectUIWindow * pDirectUIWindow = m_ArrayWindowUnique.GetAt(nIndex);

		// 关闭窗口
		if( pDirectUIWindow != pExcludeWindow )
			pDirectUIWindow->ShowWindow( false, dex::WindowShow_Size );
	}
}


// 开始消息 
void CGameClientView::OnClickedStartGame()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("StartLogo.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_START_GAME, 0, 0 );
}

// 停止按钮 
void CGameClientView::OnClickedStopGame()
{
	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_STOP_GAME, 0, 0 );
}

// 托管开始 
void CGameClientView::OnClickedStartHosted()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_START_HOSTED, 0, 0 );
}

// 托管停止 
void CGameClientView::OnClickedStopHosted()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_STOP_HOSTED, 0, 0 );
}

// 分数减少 
void CGameClientView::OnClickedScoreLess()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_LESS, 0, 0 );
}	

// 分数增加 
void CGameClientView::OnClickedScoreAdd()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_ADD, 0, 0 );
}	

// 分数最大
void CGameClientView::OnClickedScoreMax()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 发送消息
	m_pGameFrameView->SendEngineMessage( IDM_SCORE_MAX, 0, 0 );
}

// 路单 
void CGameClientView::OnClickedRoad()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );
	
	// 关闭窗口
	CloseAllWindow( &m_WindowRoad );

	// 显示窗口
	m_WindowRoad.ShowWindow( !m_WindowRoad.IsWindowVisible(), dex::WindowShow_Size );
}

// 银行
void CGameClientView::OnClickedBack()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 关闭窗口
	CloseAllWindow( &m_WindowBank );

	// 显示窗口
	m_WindowBank.ShowWindow( !m_WindowBank.IsWindowVisible(), dex::WindowShow_Size );
}

// 可以线选择
//#define LINE_CHECK

// 线选择
void CGameClientView::OnClickedLineCheck1()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_1 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck2()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_2 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck3()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_WIDTH_3 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck4()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_4 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck5()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_5 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck6()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_HEIGHT_6 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck7()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_OBLIQUE_7 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineCheck8()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, FALSE, LINE_OBLIQUE_8 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck1()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_1 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck2()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_2 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck3()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_WIDTH_3 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck4()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_4 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck5()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_5 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck6()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_HEIGHT_6 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck7()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_OBLIQUE_7 );
#endif
}

// 线选择
void CGameClientView::OnClickedLineUnCheck8()
{
#ifdef LINE_CHECK
	m_pGameFrameView->SendEngineMessage( IDM_LINE_CHECK, TRUE, LINE_OBLIQUE_8 );
#endif
}

// 三线奖励
void CGameClientView::OnClickedThreeLine()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 切换界面
	SetLineShow( MultipleType_Three );

	// 清空中奖
	ClearMineAward();
}

// 九线奖励
void CGameClientView::OnClickedNineLine()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("ButtonDown.ogg") );

	// 切换界面
	SetLineShow( MultipleType_Nine );

	// 清空中奖
	ClearMineAward();
}

// 帮助
void CGameClientView::OnClickedHelp()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 关闭窗口
	CloseAllWindow( &m_WindowHelp );

	// 显示窗口
	m_WindowHelp.ShowWindow( !m_WindowHelp.IsWindowVisible(), dex::WindowShow_Size );
}

// 声音开
void CGameClientView::OnClickedSoundOn()
{
	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 关闭声音
	g_GameOption.bSound = FALSE;
	g_GameOption.SaveParameter();

	// 背景音乐
	CMusic::_Object()->StopMusic();

	// 切换按钮
	m_ButtonSoundOn.ShowWindow( false, dex::WindowShow_Size );
	m_ButtonSoundOff.ShowWindow( true, dex::WindowShow_Size );
}

// 声音关
void CGameClientView::OnClickedSoundOff()
{
	// 开启声音
	g_GameOption.bSound = TRUE;
	g_GameOption.SaveParameter();

	// 背景音乐
	CMusic::_Object()->PlayMusic();

	// 播放音效
	CSound::_Object()->PlaySound( _T("OpenWidnow.ogg") );

	// 切换按钮
	m_ButtonSoundOn.ShowWindow( true, dex::WindowShow_Size );
	m_ButtonSoundOff.ShowWindow( false, dex::WindowShow_Size );
}

// 窗口最小化
void CGameClientView::OnClickedWinMin()
{
	AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
}

// 窗口最大化
void CGameClientView::OnClickedWinMax()
{
	if( !m_bWinMax )
	{
		DEVMODE   lpDevMode;
		CopyMemory( &lpDevMode, &m_DefaultDevmode, sizeof(lpDevMode) );
		lpDevMode.dmPelsWidth = SCREEN_CX;									// 分辨率X为1024
		lpDevMode.dmPelsHeight = SCREEN_CY;									// 分辨率Y为768
		lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT;
		::ChangeDisplaySettings(&lpDevMode, CDS_FULLSCREEN);
		m_bWinMax = true;

		// 移动窗口
		AfxGetMainWnd()->SetWindowPos(NULL, 0, 0, SCREEN_CX, SCREEN_CY, SWP_NOZORDER);

		// 按钮显示
		m_ButtonWinMin.EnableWindow( false );
		m_ButtonWinMax.ShowWindow( false );
		m_ButtonWinRevert.ShowWindow( true );
	}
}

// 窗口最还原
void CGameClientView::OnClickedWinRevert()
{
	if( m_bWinMax )
	{
		// 还原默认设置
		::ChangeDisplaySettings( &m_DefaultDevmode, CDS_UPDATEREGISTRY);
		m_bWinMax = false;

		// 窗口居中
		CRect rcArce;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArce, SPIF_SENDCHANGE);

		//读取位置
		CSize SizeRestrict(SCREEN_CX, SCREEN_CY);

		//正常位置
		CRect rcNormalSize;			

		//位置调整
		SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
		SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

		//移动窗口
		rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
		rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
		rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
		rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;
		AfxGetMainWnd()->SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

		// 按钮显示
		m_ButtonWinMin.EnableWindow( true );
		m_ButtonWinMax.ShowWindow( true );
		m_ButtonWinRevert.ShowWindow( false );
	}
}

// 窗口最关闭
void CGameClientView::OnClickedWinClose()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}


// 打开控制
void CGameClientView::OnBnClickedOpenControl()
{
	m_pGameFrameView->SendEngineMessage(IDM_OPEN_CONTROL, 0, 0);
}

