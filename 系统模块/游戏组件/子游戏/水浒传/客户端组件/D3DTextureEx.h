#pragma once

class CD3DTextureEx :
	public dex::CD3DTexture
{

	static int						m_nShowWidth;						//��ʾ��С 
	static int						m_nShowHeight;						//��ʾ��С 
	static int						m_nWidth;							//ʵ�ʴ�С 
	static int						m_nHeight;							//ʵ�ʴ�С 
public:
	CD3DTextureEx(void);
	~CD3DTextureEx(void);


	//������ʾ
	static VOID SetShowWindow(int nShowWidth, int nShowHeight) { m_nShowWidth = nShowWidth; m_nShowHeight = nShowHeight; }


	// �滭����
public:
	// �滭ͼƬ
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// �滭ͼƬ
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// �滭ͼƬ
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// �滭ͼƬ
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);
};
