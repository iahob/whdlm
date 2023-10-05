/*
 *	3D��Ⱦ�Ļ�������
 */
#ifndef RenderCanvas_h__
#define RenderCanvas_h__

class GameManager;
class UIManager;
class CameraManager;

class RenderCanvas
{
	RenderCanvas();
	~RenderCanvas();
public:
	static void Create();
	static void Destroy();

	static RenderCanvas* Instance() {return m_Instance;}

	irr::IrrlichtDevice*	GetDevice() {return m_pDevice;}
	GameManager*			GetGameManager() {return m_pGameManager;}
	UIManager*				GetUIManager() {return m_pUIManager;}
	CameraManager*	        GetCameraManager() {return m_pCameraManager;}
	bool	Init(HWND hWnd);
	void	OnIdle();

	bool	IsValid() {return m_pDevice != NULL;}


	void OnLButtonDown(UINT nFlags, CPoint Point);	
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint Point);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	void	OnSize(HWND hWnd, int cx, int cy);

	void OnSubGameStart(const void * pBuffer, WORD wDataSize);		//��Ϸ��ʼ
	void OnSubGameFree(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameEnd(const void * pBuffer, WORD wDataSize);		//��Ϸ����
	void OnSubGameRecord(const void * pBuffer, WORD wDataSize);		//��Ϸ��¼
	void OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);	//�û���ע
	void OnSubJettonClear(const void * pBuffer, WORD wDataSize);	//��ע���
	void OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);//��עʧ��
	
	void OnGSFree(const void * pBuffer, WORD wDataSize);
	void OnGSPlaying(const void * pBuffer, WORD wDataSize);
	void OnGSEnd(const void * pBuffer, WORD wDataSize);

private:
	bool	CreateScene();			// ��������

	//void	UpdataCamera();

private:
	static RenderCanvas*	m_Instance;
	irr::IrrlichtDevice*	m_pDevice;

	GameManager*	m_pGameManager;
	UIManager*		m_pUIManager;
	CameraManager*	m_pCameraManager;

	irr::scene::ISceneNode*		m_pModel;

	// ʱ�����
	unsigned int m_LastFrameTime;

	// Camera���
	//irr::scene::ICameraSceneNode*	m_pCamera;
// 	bool	m_IsLButtonDown;
// 	CPoint	m_MousePoint;
// 
// 	float	m_CameraRadius;
// 	float	m_roll, m_pitch;
};

#endif // RenderCanvas_h__