#pragma once


////////////////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	SCORE									lStorageStartTable;			//桌子库存起始值
	SCORE									lStorageDeductRoom;			//全局衰减值
	SCORE									lStorageMax1Room;			//全局库存封顶值1
	SCORE									lStorageMul1Room;			//全局赢分百分比1
	SCORE									lStorageMax2Room;			//全局库存封顶值2
	SCORE									lStorageMul2Room;			//全局赢分百分比2
	WORD									wGameTwo;					//比倍概率
	WORD									wGameTwoDeduct;				//比倍概率
	WORD									wGameThree;					//小玛丽概率
	WORD									wGameThreeDeduct;			//小玛丽概率
	BYTE									cbBetCount;					//下注数量
	SCORE									lBetScore[8];				//下注大小
};

class CDlgCustomRule : public CDialog
{
	DECLARE_MESSAGE_MAP()
	//函数定义
public:
	//构造函数
	CDlgCustomRule();
	//析构函数
	virtual ~CDlgCustomRule();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//更新控件
	bool FillDataToControl();
	//更新数据
	bool FillControlToData();

	//配置函数
public:
	//读取配置
	bool GetCustomRule(tagCustomRule & CustomRule);
	//设置配置
	bool SetCustomRule(tagCustomRule & CustomRule);

	//配置变量
protected:
	tagCustomRule					m_CustomRule;						//配置结构
};
////////////////////////////////////////////////////////////////////////////////////////////