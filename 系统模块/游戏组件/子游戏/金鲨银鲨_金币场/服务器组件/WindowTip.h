#pragma once

//����͸������
class CWindowTip : public CVirtualWindow
{
	//��������
public:
	CPoint									m_ptBenchmark;						//��׼λ��
	CSize									m_sizeStage;						//��̨��С
	BOOL									m_bKeepOut;							//�����ڵ�	
	int										m_nKeepOutAlpha;					//���ֽ�����

	CD3DTexture*						m_pImageAnimal[ANIMAL_MAX];			//���ﰴťͼƬ
	BOOL									m_bAnimalFlicker[ANIMAL_MAX];		//��������
	CPoint									m_ptAnimalSite[ANIMAL_MAX];			//����λ��


	//ͼƬ����
public:
	//
	CD3DTexture						m_ImageKeepOut;						//����

	//���캯��
public:
	CWindowTip(void);
	~CWindowTip(void);

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos);

	//���ú���
public:
	//���û�׼λ��
	VOID SetBenchmark(CPoint ptBenchmark) { m_ptBenchmark = ptBenchmark; }
	//������̨��С
	VOID SetStage(CSize	sizeStage) { m_sizeStage = sizeStage; }
	//�����ڵ�
	VOID SetKeepOut(BOOL bKeepOut) { m_bKeepOut = bKeepOut; }
	//����ͼƬ
	VOID SetImageAnimal(CD3DTexture* pImageAnimal[ANIMAL_MAX]) { CopyMemory(m_pImageAnimal, pImageAnimal, sizeof(m_pImageAnimal)); }
	//����ͼƬλ��
	VOID SetAnimalSite(CPoint ptAnimalSite[ANIMAL_MAX]) { CopyMemory(m_ptAnimalSite, ptAnimalSite, sizeof(m_ptAnimalSite)); }
	//��������
	VOID SetAnimalFlicker( int nAnimalIndex ) { if(nAnimalIndex < ANIMAL_MAX) { m_bAnimalFlicker[nAnimalIndex] = TRUE; } }
	//�ر�����
	VOID CloseFlicker() { ZeroMemory(m_bAnimalFlicker, sizeof(m_bAnimalFlicker)); }
};