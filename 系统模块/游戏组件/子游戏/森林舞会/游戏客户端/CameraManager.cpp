#include "Stdafx.h"
#include "RenderCanvas.h"
#include "GameManager.h"
#include "UIManager.h"
#include "CameraManager.h"


using namespace irr;

CameraManager::CameraManager(irr::IrrlichtDevice* pDevice) :
m_pDevice(pDevice),
m_pCamera(NULL),
m_IsLButtonDown(false)
{
	m_IsFreeMode = false;
	
	m_CameraRadius1 = 260.0f;
	m_CameraRadius2 = 225.0f;

	m_CameraRadius = m_CameraRadius1; 

	m_roll1 = core::PI;
	m_roll2 = core::PI;

	m_roll = m_roll1;

	m_pitch1 = 0.85f;
	m_pitch2 = 0.88f;
	m_pitch = m_pitch1;


	//m_CameraLookAtPos1 = core::vector3df(0.0f, 0.0f, -10.0f);
	//m_CameraLookAtPos2 = core::vector3df(0.0f, 10.0f, 0.0f);

	m_CameraLookAtPos1 = core::vector3df(0.0f, 20.0f, 0.0f);
	m_CameraLookAtPos2 = core::vector3df(0.0f, 20.0f, 0.0f);

	m_CameraLookAtPos = m_CameraLookAtPos1;
}

CameraManager::~CameraManager()
{

}

void CameraManager::reset()
{
	video::IVideoDriver*  driver = m_pDevice->getVideoDriver();
	core::dimension2di   viewsize(driver->getScreenSize());
    f32 Aspect=((float)viewsize.Width)/viewsize.Height;
	
	m_pCamera->setAspectRatio(Aspect);
}

bool CameraManager::InitGame()
{
	scene::ISceneManager* smgr = m_pDevice->getSceneManager();
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();

	_ASSERT(!m_pCamera);
	m_pCamera = smgr->addCameraSceneNode();
	m_pCamera->setFOV( core::PI / 3.0f);

	UpdataCamera();
	return true;
}

void CameraManager::Tick(unsigned int LastTime, unsigned int FrameTime)
{
	// 游戏状态和时间
	CStringW StateStr;
	CStringW TimeStr;
	switch(RenderCanvas::Instance()->GetGameManager()->GetGameState())
	{
	case SUB_S_GAME_FREE:	// free
		StateStr = L"GameFree";
		TickFree(LastTime, FrameTime);
		break;
	case SUB_S_GAME_START: // Start
		StateStr = L"GameStart";
		TickStart(LastTime, FrameTime);
		break;
	case SUB_S_GAME_END: // end
		StateStr = L"GameEnd";
		TickEnd(LastTime, FrameTime);
		break;
	}

}

void CameraManager::TickStart( unsigned int LastTime, unsigned int FrameTime )
{
	if(!m_IsFreeMode)
	{
		if(RenderCanvas::Instance()->GetUIManager())
		{
		core::position2d<irr::s32> hidePos = RenderCanvas::Instance()->GetUIManager()->GetPanelHidePos();

		float pro = (float)hidePos.Y / 160;

		m_CameraRadius = (m_CameraRadius2 - m_CameraRadius1) * pro + m_CameraRadius1;
		m_pitch = (m_pitch2 - m_pitch1) * pro + m_pitch1;
		m_CameraLookAtPos = (m_CameraLookAtPos2 - m_CameraLookAtPos1) * pro + m_CameraLookAtPos1;

		UpdataCamera();
		}
	}
}

void CameraManager::TickEnd( unsigned int LastTime, unsigned int FrameTime )
{
	if(!m_IsFreeMode)
	{
		if(RenderCanvas::Instance()->GetUIManager())
		{
			core::position2d<irr::s32> hidePos = RenderCanvas::Instance()->GetUIManager()->GetPanelHidePos();

			float pro = (float)hidePos.Y / 160;

			m_CameraRadius = (m_CameraRadius2 - m_CameraRadius1) * pro + m_CameraRadius1;
			m_pitch = (m_pitch2 - m_pitch1) * pro + m_pitch1;
			m_CameraLookAtPos = (m_CameraLookAtPos2 - m_CameraLookAtPos1) * pro + m_CameraLookAtPos1;

			UpdataCamera();
		}
	
	}
}

void CameraManager::TickFree( unsigned int LastTime, unsigned int FrameTime )
{
	if(!m_IsFreeMode && RenderCanvas::Instance()->GetGameManager()->GetCurPrizeMode()==eAnimalPrizeMode_SingleAnimalSingleColor)
	{
		int RemainTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateRemainTime();
		int AllTime = RenderCanvas::Instance()->GetGameManager()->GetGameStateTime();
		int PassTime = AllTime - RemainTime;

		if(PassTime < AllTime * 0.4f)
		{
			float pro = (float)PassTime / (AllTime * 0.4f);
			m_CameraRadius = (70.0f - m_CameraRadius3) * pro + m_CameraRadius3;
			m_pitch = (0.25f - m_pitch3) * pro + m_pitch3;
			m_roll = (core::PI - m_roll3) * pro + m_roll3;
			m_CameraLookAtPos = (core::vector3df(0.0f, 80.0f, 0.0f) - m_CameraLookAtPos3) * pro + m_CameraLookAtPos3;
			UpdataCamera();
		}		
	}
}


void CameraManager::UpdataCamera()
{
	m_pCamera->setUpVector(core::vector3df(0.0f, 1.0f, 0.0f));
	m_pCamera->setTarget(m_CameraLookAtPos);

	core::vector3df CamPos;
	CamPos.X = m_CameraRadius * cos(m_pitch) * sin(m_roll);
	CamPos.Z = m_CameraRadius * cos(m_pitch) * cos(m_roll);
	CamPos.Y = m_CameraRadius * sin(m_pitch);

	CamPos += m_CameraLookAtPos;

	m_pCamera->setPosition(CamPos);
}

void CameraManager::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if(!m_IsFreeMode)
		return;

	m_IsLButtonDown = true;
	m_MousePoint = Point;
}

void CameraManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_IsFreeMode)
		return;

	m_IsLButtonDown = false;
}

void CameraManager::OnMouseMove(UINT nFlags, CPoint Point)
{
	if(!m_IsFreeMode)
		return;

	if(m_IsLButtonDown)
	{
		m_roll += (float)(Point.x - m_MousePoint.x) / 100.0f;
		m_pitch += (float)(Point.y - m_MousePoint.y) / 100.0f;
		if(m_pitch > core::HALF_PI - 0.01f)
			m_pitch = core::HALF_PI - 0.01f;
		else if( m_pitch < -core::HALF_PI + 0.01f )
			m_pitch = -core::HALF_PI + 0.01f;

		UpdataCamera();

		m_MousePoint = Point;
	}
}

void CameraManager::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if(!m_IsFreeMode)
		return;

	m_CameraRadius -= zDelta * 0.1f;
	if(m_CameraRadius < 10.0f)
		m_CameraRadius = 10.0f;

	UpdataCamera();
}

void CameraManager::OnSubGameStart( const void * pBuffer, WORD wDataSize )
{

}

void CameraManager::OnSubGameFree( const void * pBuffer, WORD wDataSize )
{
	m_CameraRadius3 = m_CameraRadius;
	m_roll3 = m_roll;
	m_pitch3 = m_pitch;
	m_CameraLookAtPos3 = m_CameraLookAtPos;
}

void CameraManager::OnSubGameEnd( const void * pBuffer, WORD wDataSize )
{

}
