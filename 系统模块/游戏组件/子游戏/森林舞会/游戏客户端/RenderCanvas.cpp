#include "Stdafx.h"
#include "RenderCanvas.h"
#include "GameManager.h"
#include "UIManager.h"
#include "CameraManager.h"
//#include ".\log\log.h"

using namespace irr;

RenderCanvas* RenderCanvas::m_Instance = NULL;

RenderCanvas::RenderCanvas() : 
m_pDevice(NULL),
m_pModel(NULL),
//m_IsLButtonDown(false),
//m_pCamera(NULL),
// m_roll(core::PI),
// m_pitch(0.8f),
// m_CameraRadius(200.0f),
m_pGameManager(NULL),
m_LastFrameTime(0)
{
}

RenderCanvas::~RenderCanvas()
{
	delete m_pGameManager;
	delete m_pUIManager;
	delete m_pCameraManager;
}

void RenderCanvas::Create()
{
	_ASSERT(!m_Instance);
	m_Instance = new RenderCanvas;
}

void RenderCanvas::Destroy()
{
	if(m_Instance->m_pDevice)
	{
		m_Instance->m_pDevice->closeDevice();
		m_Instance->m_pDevice->drop();
		m_Instance->m_pDevice = NULL;
	}

	_ASSERT(m_Instance);
	delete m_Instance;
	m_Instance = NULL;
}

bool RenderCanvas::Init(HWND hWnd)
{
	irr::SIrrlichtCreationParameters param;
	param.DriverType = video::EDT_DIRECT3D9;
	param.WindowId = reinterpret_cast<void*>(hWnd);
	param.Bits=32;

	_ASSERT(!m_pDevice);
	m_pDevice = irr::createDeviceEx(param);
	if(m_pDevice==NULL)
	{
	    AfxMessageBox(_T("D3D设备创建失败！"));
		//LOG_FILE("D3D设备创建失败！");
		return false;
	}

    //m_pDevice->getFileSystem()->addZipFileArchive("./3DSLWG/Res.xun",true,true,"12345678asd");

	m_pDevice->getFileSystem()->addZipFileArchive("./3DSLWG/Res.zip");

	if(!CreateScene())
		return false;
	
	m_pGameManager = new GameManager(m_pDevice);
	if(!m_pGameManager->InitGame())
		return false;

	m_pUIManager = new UIManager(m_pDevice);
	if(!m_pUIManager->InitGame())
		return false;

	m_pCameraManager = new CameraManager(m_pDevice);
	if(!m_pCameraManager->InitGame())
		return false;

	return true;
}

bool RenderCanvas::CreateScene()
{
	scene::ISceneManager* smgr = m_pDevice->getSceneManager();
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();

	// 添加摄像机
	//_ASSERT(!m_pCamera);
	//m_pCamera = smgr->addCameraSceneNode();
	
	// 原点的box
 	scene::ISceneNode* cube = smgr->addCubeSceneNode(20);
 
 	cube->setMaterialTexture(0, driver->getTexture(DXPATH"wall.bmp"));
 	cube->setMaterialTexture(1, driver->getTexture(DXPATH"water.jpg"));
 	cube->setMaterialFlag( video::EMF_LIGHTING, false );
 	cube->setMaterialType( video::EMT_REFLECTION_2_LAYER );
	cube->setScale(core::vector3df(0.2f, 0.2f, 0.2f));

	// 天空盒
	smgr->addSkyBoxSceneNode(
		driver->getTexture(DXPATH"Scene/waiqiang.tga"),
		driver->getTexture(DXPATH"Scene/waiqiang.tga"),
		driver->getTexture(DXPATH"Scene/waiqiang.tga"),
		driver->getTexture(DXPATH"Scene/waiqiang.tga"),
		driver->getTexture(DXPATH"Scene/waiqiang.tga"),
		driver->getTexture(DXPATH"Scene/waiqiang.tga"));

	//UpdataCamera();
	return true;
}

void RenderCanvas::OnIdle()
{
	if(!m_pDevice)
		return;

	m_pDevice->getTimer()->tick();

	// 渲染Tick
	unsigned int CurFrameTime = m_pDevice->getTimer()->getTime();

	m_pGameManager->Tick(CurFrameTime, CurFrameTime - m_LastFrameTime);

	m_pUIManager->Tick(CurFrameTime, CurFrameTime - m_LastFrameTime);

	m_pCameraManager->Tick(CurFrameTime, CurFrameTime - m_LastFrameTime);

	m_LastFrameTime = CurFrameTime;

	// 渲染
	m_pDevice->getVideoDriver()->beginScene(true, true, video::SColor(255, 50, 50, 50));

	m_pDevice->getSceneManager()->drawAll();
	m_pDevice->getGUIEnvironment()->drawAll();

	m_pDevice->getVideoDriver()->endScene();

}

void RenderCanvas::OnSize(HWND hWnd, int cx, int cy )
{
	if(!m_pDevice)
		return;

	m_pDevice->ResizeProcess();
	m_pUIManager->ReSetUI();
	m_pCameraManager->reset();
}

void RenderCanvas::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if(m_pCameraManager)
		m_pCameraManager->OnLButtonDown(nFlags, Point);
}

void RenderCanvas::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_pCameraManager)
		m_pCameraManager->OnLButtonUp(nFlags, point);
}

void RenderCanvas::OnMouseMove(UINT nFlags, CPoint Point)
{
	if(m_pCameraManager)
		m_pCameraManager->OnMouseMove(nFlags, Point);

}

void RenderCanvas::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if(m_pCameraManager)
		m_pCameraManager->OnMouseWheel(nFlags, zDelta, pt);

}

void RenderCanvas::OnSubGameStart(const void * pBuffer, WORD wDataSize)	//游戏开始
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;

	m_pGameManager->OnSubGameStart(pBuffer, wDataSize);
	m_pUIManager->OnSubGameStart(pBuffer, wDataSize);
	m_pCameraManager->OnSubGameStart(pBuffer, wDataSize);
}

void RenderCanvas::OnSubGameFree(const void * pBuffer, WORD wDataSize)	//游戏空闲
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;

	m_pGameManager->OnSubGameFree(pBuffer, wDataSize);
	m_pUIManager->OnSubGameFree(pBuffer, wDataSize);
	m_pCameraManager->OnSubGameFree(pBuffer, wDataSize);
}

void RenderCanvas::OnSubGameEnd(const void * pBuffer, WORD wDataSize)	//游戏结束
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;

	m_pGameManager->OnSubGameEnd(pBuffer, wDataSize);
	m_pUIManager->OnSubGameEnd(pBuffer, wDataSize);
	m_pCameraManager->OnSubGameEnd(pBuffer, wDataSize);
}

void RenderCanvas::OnSubGameRecord(const void * pBuffer, WORD wDataSize)	// 游戏记录
{
	if(!m_pUIManager)
		return;

	m_pUIManager->OnSubGameRecord(pBuffer, wDataSize);
}

void RenderCanvas::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)	// 用户加注
{
	if(!m_pGameManager || !m_pUIManager)
		return;

	m_pGameManager->OnSubPlaceJetton(pBuffer, wDataSize);
	m_pUIManager->OnSubPlaceJetton(pBuffer, wDataSize);
}

void RenderCanvas::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	if(!m_pGameManager || !m_pUIManager)
		return;

	m_pGameManager->OnSubPlaceJettonFail(pBuffer, wDataSize);
	m_pUIManager->OnSubPlaceJettonFail(pBuffer, wDataSize);
}

void RenderCanvas::OnGSFree(const void * pBuffer, WORD wDataSize)
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;
	m_pGameManager->OnGSFree(pBuffer, wDataSize);
	m_pUIManager->OnGSFree(pBuffer, wDataSize);
}

void RenderCanvas::OnGSPlaying(const void * pBuffer, WORD wDataSize)
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;

	m_pGameManager->OnGSEnd(pBuffer, wDataSize);
	m_pGameManager->OnGSPlaying(pBuffer, wDataSize);

	m_pUIManager->OnGSEnd(pBuffer, wDataSize);//一样的消息，处理面板。
	m_pUIManager->OnGSPlaying(pBuffer, wDataSize);
}

void RenderCanvas::OnGSEnd(const void * pBuffer, WORD wDataSize)
{
	if(!m_pGameManager || !m_pUIManager || !m_pCameraManager)
		return;
	m_pGameManager->OnGSEnd(pBuffer, wDataSize);
	m_pUIManager->OnGSEnd(pBuffer, wDataSize);
}

void RenderCanvas::OnSubJettonClear(const void * pBuffer, WORD wDataSize)
{
	if(!m_pGameManager || !m_pUIManager)
		return;

	m_pUIManager->OnSubJettonClear(pBuffer, wDataSize);
}

void RenderCanvas::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(!m_pGameManager || !m_pUIManager)
		return;

	m_pUIManager->OnKeyDown(nChar, nRepCnt, nFlags);
}

void RenderCanvas::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(!m_pGameManager || !m_pUIManager)
		return;

	m_pUIManager->OnKeyUp(nChar, nRepCnt, nFlags);
}