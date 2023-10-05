#pragma once

#define IDM_USER_ACTION_FINISH		(WM_USER+101)						//动画结束

//X 排列方式
enum enXCollocateMode
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//动画播放模式
enum enPlayMode 
{ 
	enStatic,						//静态
	enOnce,							//一次
	enCycle,						//循环
};


class CAnimation
{
	bool							m_bSendFinish;						//通知完成

protected:
	bool							m_bInit;							//加载图片
	bool							m_bShowImage;						//显示图片
	int								m_nImageCount;						//图片数量
	int								m_nImageX;							//单张图片
	int								m_nImageY;							//单张图片

	//位置变量
public:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	bool							m_bSan;								//闪
	bool							m_bSanShow;							//闪显
	int								m_nLapseCount;						//时间间隔
	int								m_nLapseIndex;						//时间间隔
	int								m_nCurIndex;						//当前索引
	int								m_nMaxIndex;						//最大索引
	int								m_nSendIndex;						//通知索引
	enPlayMode						m_enPlayMode;						//播放模式
	CD3DTextureEx *				m_pTextureImage;					//图片指针

public:
	CAnimation(void);
	~CAnimation(void);

	//删除图片
	VOID DeleteImamge();
	//加载图片
	bool IsInitImamge() { return m_bInit; }

	//界面函数
public:
	//动画驱动
	bool CartoonMovie();
	//配置设备
	bool InitAnimation(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageCount, int nImagePageCount);
	//配置设备
	bool InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY);
	//绘画界面
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice);
	//绘画界面
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nDrawIndex);
	//绘画界面
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//绘画界面
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);

	//启动动画
	VOID ShowAnimation(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//启动动画
	VOID ShowAnimationEx(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//停止动画
	VOID StopAnimation();
	//停止动画
	VOID StopAnimationSan(bool bSan) { m_bSan = bSan;};
	//通知索引
	VOID SetSendIndex(int nSendIndex);
	//显示动画
	bool IsShow() { return m_bShowImage; }

	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);

};
