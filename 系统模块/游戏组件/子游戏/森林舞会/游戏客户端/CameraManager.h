/*
 *	Camera������
 */
#ifndef CameraManager_h__
#define CameraManager_h__

class CameraManager
{
public:
	CameraManager(irr::IrrlichtDevice* pDevice);
	~CameraManager();

	bool InitGame();

	void Tick(unsigned int LastTime, unsigned int FrameTime);

	void OnSubGameStart(const void * pBuffer, WORD wDataSize);		//��Ϸ��ʼ
	void OnSubGameFree(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameEnd(const void * pBuffer, WORD wDataSize);		//��Ϸ����

	void OnLButtonDown(UINT nFlags, CPoint Point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint Point);
	void OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	void reset();
private:
	void UpdataCamera();

	void TickStart(unsigned int LastTime, unsigned int FrameTime);
	void TickFree(unsigned int LastTime, unsigned int FrameTime);
	void TickEnd(unsigned int LastTime, unsigned int FrameTime);

private:
	irr::IrrlichtDevice*	m_pDevice;

	irr::scene::ICameraSceneNode*	m_pCamera;

	bool	m_IsLButtonDown;
	CPoint	m_MousePoint;

	float	m_CameraRadius;
	float	m_roll, m_pitch;
	irr::core::vector3df m_CameraLookAtPos;

	float	m_CameraRadius1, m_CameraRadius2, m_CameraRadius3;
	float	m_roll1, m_roll2, m_roll3;
	float	m_pitch1, m_pitch2, m_pitch3;
	irr::core::vector3df m_CameraLookAtPos1, m_CameraLookAtPos2, m_CameraLookAtPos3;

	bool	m_IsFreeMode;
};



#endif // CameraManager_h__