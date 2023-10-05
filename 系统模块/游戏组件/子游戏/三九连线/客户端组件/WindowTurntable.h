#pragma once

// 元素移动
struct tagElementMove
{
	int					nElementIndex;			// 元素索引
	CPoint				PointSite;				// 位置信息
};

// 转盘窗口
class CWindowTurntable : public dex::CDirectUIWindow
{

	// 变量信息
public:
	bool										m_bSpeedUp;														// 加速显示
	bool										m_bElementMove;													// 元素移动
	bool										m_nKeepTurn;													// 保持旋转
	bool										m_nAcceptedElement;												// 接受元素
	int											m_nElementMoveCount[TURN_WIDTH_COUNT];							// 移动数量
	int											m_nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];			// 固定元素

	// 提示动画
public:
	bool										m_bLinePrompt;													// 线数提示
	bool										m_bNineLinePrompt;												// 9连提示
	bool										m_bThreeLinePrompt[LINE_INDEX_MAX];								// 3连提示
	bool										m_bThreeLineElement[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// 3连提示

	// 参考变量
public:
	CSize										m_SizeElementTexture;											// 元素图片大小
	CSize										m_SizeElementArea;												// 元素区域大小
	CPoint										m_PointDatumElement[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// 元素基准位置

	// 动画变量
public:
	CWHArray< tagElementMove * >				m_TElementMoveInfo[TURN_WIDTH_COUNT];		// 元素动画

	// 资源信息
public:
	dex::CLapseCount							m_LCSoundTime;								// 声音缓冲
	dex::CD3DTexture							m_TextureElement[Element_Max];

	// 类函数
public:
	// 构造函数
	CWindowTurntable();
	// 析构函数
	virtual ~CWindowTurntable();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 消息函数
public:
	// 左键点下 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// 功能函数
public:
	// 转动元素
	void TurnElement();
	// 停止元素
	void StopElement( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], bool bStopNew );
	// 连线提示
	bool LinePrompt();
	// 强制停止元素
	bool ForcedStopElement();
	// 设置加速
	void SetSpeedUp(bool bSpeedUp) { m_bSpeedUp = bSpeedUp; }

	// 内部函数
public:
	// 计算图片位置
	CPoint PointTextureElement( CPoint PointElement );
	// 清空动画
	void EmptyElementMove();


	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};
