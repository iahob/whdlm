#pragma once
#include "ObjectMove.h"

// 宏定义
#define ARRAY_GOLD_MAX				2			// 金币数据
#define TEXTRUE_GOLD_MAX			10			// 金币图片

// 三连信息
#define TEXTRUE_THREE_BACK			17			// 三连背景
#define TEXTRUE_THREE_STAR			15			// 三连星星

// 九连信息
#define TEXTRUE_NINE_BACK			12			// 九连背景
#define TEXTRUE_NINE_LOGO			2			// 九连LOGO
#define TEXTRUE_NINE_LIGHT			5			// 九连扫光

// 数字信息
struct tagWinNumberInfo
{
	int				nNumberIndex;					// 数字索引
	int				nImageFrame;					// 图片帧数
	float			fNumberSize;					// 数字大小
	CDoublePoint	PointNumber;					// 绘画位置
};

// 赢窗口
class CWindowWin : public dex::CDirectUIWindow
{

	// 背景动画
public:
	int										m_nBackIndex;					// 背景索引
	int										m_nBackTime;					// 背景时间
	dex::CLapseCount						m_nLCBackTime;					// 背景流逝
	int										m_nMultipleIndex;				// 奖励倍数
	EnumMultipleType						m_nMultipleType;				// 奖励类型

	// 任务信息
public:
	int										m_nTaskTarget[TASK_INDEX_MAX];	// 任务目标
	bool									m_bTaskComplete[TASK_INDEX_MAX];// 任务完成

	// 金币动画
public:
	bool									m_bHeldDrop;					// 保持掉落
	int										m_nGoldCount;					// 金币动画
	CWHArray< CGoldMove * >					m_ArrayGold[ARRAY_GOLD_MAX];	// 金币动画
	CWHArray< tagWinNumberInfo >			m_ArrayNumberWin;				// 赢数字
	longlong								m_lWinScore;					// 赢分数
	longlong								m_lTaskScore;					// 任务分数

	// 闪光对象
public:
	CWHArray< CDoublePoint >				m_ArrayPointFlash;				// 灯光点
	CWHArray< CLightMove * >				m_ArrayFlash;					// 闪光对象
	dex::CLapseCount						m_nLCFlashTime;					// 闪光流逝

	// 三连动画
public:
	CWHArray< tagThreeStarMovie * >			m_ArrayThreeStarMovie;			// 三连动画

	// 九连动画
public:
	int										m_nNineLogoIndex;				// 索引显示

	// 资源信息
public:
	// 三连图片
	dex::CD3DTexture						m_TextureThreeBack[TEXTRUE_THREE_BACK];	// 三连背景
	dex::CD3DTexture						m_TextureThreeStar[TEXTRUE_THREE_STAR]; // 三连星星
	dex::CD3DTexture						m_TextureThreeRay;				// 三连光晕

	// 九连图片
	dex::CD3DTexture						m_TextureNineBack[TEXTRUE_NINE_BACK];	// 九连背景
	dex::CD3DTexture						m_TextureNineLogo[TEXTRUE_NINE_LOGO];	// 九连旗帜
	dex::CD3DTexture						m_TextureNineLight[TEXTRUE_NINE_LIGHT];	// 九连扫光
	dex::CD3DTexture						m_TextureNineRay;				// 九连光晕
	dex::CD3DTexture						m_TextureNineNumber;			// 九连数字

	// 通用图片
	dex::CD3DTexture						m_TextureGold[TEXTRUE_GOLD_MAX];// 金币图片
	dex::CD3DTexture						m_TextureNumber1;				// 数字图片
	dex::CD3DTexture						m_TextureNumber2;				// 数字图片
	dex::CD3DTexture						m_TextureNumber3;				// 数字图片
	dex::CD3DTexture						m_TextureNumber4;				// 数字图片
	dex::CD3DTexture						m_TextureNumber5;				// 数字图片
	dex::CD3DTexture						m_TextureComma1;				// 逗号图片
	dex::CD3DTexture						m_TextureComma2;				// 逗号图片
	dex::CD3DTexture						m_TextureComma3;				// 逗号图片
	dex::CD3DTexture						m_TextureComma4;				// 逗号图片
	dex::CD3DTexture						m_TextureLight;					// 灯光图片
	dex::CD3DTexture						m_TextureTaskLogo;				// 任务标识
	dex::CD3DTexture						m_TextureTaskNumber;			// 任务数字

	// 类函数
public:
	// 构造函数
	CWindowWin();
	// 析构函数
	virtual ~CWindowWin();

	// 消息函数
public:
	// 窗口创建 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 窗口动画
	void OnWindowMovie();
	// 窗口绘画 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// 绘画金币
public:
	// 绘画分数
	void DrawScore( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// 绘画分数
	void DrawScore( dex::CD3DDevice * pD3DDevice, CWHArray< tagWinNumberInfo > & ArrayNumberInfo, dex::CD3DTexture * pImageNumber, dex::CD3DTexture * pImageComma, CPoint PointOffset );
	// 绘画金币
	void DrawGold( dex::CD3DDevice * pD3DDevice, CWHArray< CGoldMove * > & ArrayGold );

	// 控制函数
public:
	// 添加金币
	void AppendGold( bool bHeldDrop );
	// 设置任务目标
	void SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] );
	// 设置金币
	void SetWinScore( int nMultipleIndex, longlong lWinScore, longlong lTaskScore, bool bTaskComplete[TASK_INDEX_MAX] );

	// 内部函数
public:
	// 生成数字
	void GenerateNumber( CWHArray< tagWinNumberInfo > & ArrayNumberInfo, CPoint PointCenter, longlong lNumber, float fSize );

	// 声明消息
public:
	// 声明消息
	DECLARE_WHDUI_MESSAGE_MAP()
};
