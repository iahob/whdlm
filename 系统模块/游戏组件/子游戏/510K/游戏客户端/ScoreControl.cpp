#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"

//////////////////////////////////////////////////////////////////////////////////

//控件变量
#define IDC_BT_CLOSE				100									//关闭
#define IDC_BT_AGAIN				101									//再来一局
//动画数目
#define SCORE_CARTOON_COUNT			4									//动画数目

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CScoreControl::CScoreControl()
{
	//设置变量
	m_cbAlphaIndex=0;
	m_cbCartoonIndex=0;

	//设置变量
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	return;
}

//析构函数
CScoreControl::~CScoreControl()
{
}

//动画消息
VOID CScoreControl::OnWindowMovie()
{
	//成绩动画
	if ((IsWindowVisible()==true)&&(m_CartoonLapseCount.GetLapseCount(400)>0L))
	{
		m_cbCartoonIndex=(m_cbCartoonIndex+1)%SCORE_CARTOON_COUNT;
	}

	//灰度动画
	if ((IsWindowVisible()==true)&&((m_cbAlphaIndex<110L)&&(m_AlphaLapseCount.GetLapseCount(20)>0L)))
	{
		m_cbAlphaIndex+=5L;
	}

	return;
}

//创建消息
VOID CScoreControl::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	m_D3DFont.CreateFont(120,TEXT("宋体"),0L);

	//关闭按钮
	CRect rcCreate(0,0,0,0);
	m_btClose.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_CLOSE,GetVirtualEngine(),this);
	m_btClose.SetButtonImage(pD3DDevice,TEXT("BT_CLOSE"),TEXT("PNG"),AfxGetInstanceHandle());

	m_btAgain.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_AGAIN,GetVirtualEngine(),this);
	m_btAgain.SetButtonImage(pD3DDevice,TEXT("BT_AGAIN"),TEXT("PNG"),AfxGetInstanceHandle());
	//加载资源
	m_TextureGameScoreBG.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("GAME_SCORE_BG"),TEXT("PNG"));
	m_TextureWinFlag.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("WIN_FLAG"),TEXT("PNG"));
	m_TextureLostFlag.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("LOST_FLAG"),TEXT("PNG"));

	//获取大小
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_TextureGameScoreBG.GetWidth(),m_TextureGameScoreBG.GetHeight());

	//移动窗口
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	//按钮位置
	CRect rcButton;
	m_btClose.GetWindowRect(rcButton);
	m_btClose.SetWindowPos(SizeGameScore.cx-rcButton.Width()-100,335,0,0,SWP_NOSIZE);
	m_btAgain.SetWindowPos(100,335,0,0,SWP_NOSIZE);
	return;
}

//销毁消息
VOID CScoreControl::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	//删除字体
	m_D3DFont.DeleteFont();

	//销户资源
	if (m_TextureGameScoreBG.IsNull()==false)
		m_TextureGameScoreBG.Destory();
	if(m_TextureWinFlag.IsNull() == false)
		m_TextureWinFlag.Destory();
	if(m_TextureLostFlag.IsNull() == false)
		m_TextureLostFlag.Destory();
	if(m_TextureEqualFlag.IsNull() == false)
		m_TextureEqualFlag.Destory();
	if(m_TextureWinNum.IsNull() == false)
		m_TextureWinNum.Destory();
	if(m_TextureLostNum.IsNull() == false)
		m_TextureLostNum.Destory();
	return;
}

//鼠标事件
VOID CScoreControl::OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)
{
	return;
}

//按钮事件
VOID CScoreControl::OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)
{
	//关闭窗口
	if (uButtonID==IDC_BT_CLOSE)
	{
		//CloseControl();
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return;
	}
	else if(uButtonID == IDC_BT_AGAIN)
	{
		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(WM_USER+200,0,0L);
	}

	CloseControl();
	return;
}

//绘画窗口
VOID CScoreControl::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	//绘画背景
	m_TextureGameScoreBG.DrawImage(pD3DDevice,nXOriginPos,nYOriginPos);

	//绘画信息
	int offy=52;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//位置计算
		CRect rcNiceName(nXOriginPos+40,nYOriginPos+120+i*offy,nXOriginPos+190,nYOriginPos+150+i*offy);//昵称
		CRect rcScore(nXOriginPos+270,nYOriginPos+120+i*offy,nXOriginPos+370,nYOriginPos+150+i*offy);//得分
		CRect rcTimes(nXOriginPos+405,nYOriginPos+120+i*offy,nXOriginPos+505,nYOriginPos+150+i*offy);//倍数
		CRect rcGameScore(nXOriginPos+470,nYOriginPos+120+i*offy,nXOriginPos+595,nYOriginPos+150+i*offy);//输赢

		//用户帐号
		LPCTSTR pszNiceName=m_ScoreInfo.szNickName[i];				
		m_D3DFont.DrawText(pD3DDevice,pszNiceName,rcNiceName,DT_CENTER|DT_BOTTOM,D3DCOLOR_XRGB(255, 255, 255));
		
		//得分
		TCHAR  strtemp[MAX_PATH]={0};
		_sntprintf(strtemp,CountArray(strtemp),_T("%d"),m_ScoreInfo.cbScore[i]);
		m_D3DFont.DrawText(pD3DDevice,strtemp,rcScore,DT_LEFT|DT_BOTTOM,D3DCOLOR_XRGB(255, 255, 255));
		//倍数
		_sntprintf(strtemp,CountArray(strtemp),_T("%d"),m_ScoreInfo.cbUserTimes[i]);
		m_D3DFont.DrawText(pD3DDevice,strtemp,rcTimes,DT_LEFT|DT_BOTTOM,D3DCOLOR_XRGB(255, 255, 255));
		//输赢
		_sntprintf(strtemp,CountArray(strtemp),_T("%+d"),m_ScoreInfo.lGameScore[i]);
		m_D3DFont.DrawText(pD3DDevice,strtemp,rcGameScore,DT_CENTER|DT_BOTTOM,D3DCOLOR_XRGB(255, 255, 255));

		//if(i == m_ScoreInfo.wMeChairID)
		{
			if(m_ScoreInfo.lGameScore[i]>0)
				m_TextureWinFlag.DrawImage(pD3DDevice,rcNiceName.right-20,rcNiceName.top);
			else
				m_TextureLostFlag.DrawImage(pD3DDevice,rcNiceName.right-20,rcNiceName.top);
		}
	}
	return;
}

//隐藏窗口
VOID CScoreControl::CloseControl()
{
	//关闭窗口
	if (IsWindowActive()==true)
	{
		ShowWindow(FALSE);
		//关闭窗口
		//DeleteWindow();
		//设置数据
		ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));
	}

	return;
}

//设置积分
VOID CScoreControl::SetScoreInfo(tagScoreInfo & ScoreInfo)
{
	//设置变量
	m_ScoreInfo=ScoreInfo;

	//灰化动画
	m_cbAlphaIndex=0;
	m_AlphaLapseCount.Initialization();

	//成绩动画
	m_cbCartoonIndex=0;
	m_CartoonLapseCount.Initialization();

	return;
}

// 绘画数字
void CScoreControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);

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

//////////////////////////////////////////////////////////////////////////////////
