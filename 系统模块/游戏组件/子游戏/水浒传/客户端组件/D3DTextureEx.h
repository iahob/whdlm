#pragma once

class CD3DTextureEx :
	public dex::CD3DTexture
{

	static int						m_nShowWidth;						//显示大小 
	static int						m_nShowHeight;						//显示大小 
	static int						m_nWidth;							//实际大小 
	static int						m_nHeight;							//实际大小 
public:
	CD3DTextureEx(void);
	~CD3DTextureEx(void);


	//设置显示
	static VOID SetShowWindow(int nShowWidth, int nShowHeight) { m_nShowWidth = nShowWidth; m_nShowHeight = nShowHeight; }


	// 绘画函数
public:
	// 绘画图片
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest);
	// 绘画图片
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource);
	// 绘画图片
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	// 绘画图片
	bool DrawTexture(dex::CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, byte cbAlpha);
};
