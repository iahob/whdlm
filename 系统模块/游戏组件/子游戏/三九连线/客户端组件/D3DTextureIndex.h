#pragma once

class CD3DTextureIndex : public CD3DTexture
{
	// ��������
public:
	int						m_nWidthIndex;				// �������
	int						m_nHeightIndex;				// �߶�����

	// ���캯��
public:
	CD3DTextureIndex(void);
	~CD3DTextureIndex(void);

	// ͼƬ��Ϣ
public:
	// �������
	INT GetWidthIndex() { return m_nWidthIndex; }
	// �߶�����
	INT GetHeightIndex() { return m_nHeightIndex; }

	// ���뺯��
public:
	// ��������
	bool LoadImage(CD3DDevice * pD3DDevice, HINSTANCE hInstance, int nWidthIndex, int nHeightIndex, LPCTSTR pszResource, LPCTSTR pszTypeName);

	// �滭����
public:
	// �����滭
	bool DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex );
	// �����滭
	bool DrawImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// �����滭
	bool DrawImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex );
	// �����滭
	bool DrawImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );

	// ��ɫ�滭
public:
	// �����滭
	bool DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, INT nIndex );
	// �����滭
	bool DrawColourFilterImageIndex( CD3DDevice * pD3DDevice, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// �����滭
	bool DrawColourFilterImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, INT nIndex );
	// �����滭
	bool DrawColourFilterImageIndex(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
};
