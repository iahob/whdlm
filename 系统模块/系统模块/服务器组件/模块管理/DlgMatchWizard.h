#ifndef DLG_MATCH_WIZARD_HEAD_FILE
#define DLG_MATCH_WIZARD_HEAD_FILE

#pragma once

//引入文件
#include "MatchInfoManager.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CDlgMatchItem;
class CDlgMatchWizard;
class CDlgMatchWizardStep1;
class CDlgMatchWizardStep2;

//////////////////////////////////////////////////////////////////////////

//比赛子项类
class MODULE_MANAGER_CLASS CDlgMatchItem : public CDialog
{
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;

	//变量定义
protected:		
	WORD							m_wKindID;							//游戏类型
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//模块信息
	CRewardInfoBuffer *				m_pRewardInfoBuffer;				//奖励数据	
	tagGameMatchOption *			m_pGameMatchOption;					//比赛配置

	//函数定义
protected:
	//构造函数
	CDlgMatchItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgMatchItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//接口函数
protected:	
	//保存输入
	virtual bool SaveInputInfo()=NULL;	
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//配置函数
protected:	
	//设置比赛
	VOID SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer);
	//设置奖励
	VOID SetRewardInfoBuffer(CRewardInfoBuffer * pRewardInfoBuffer);
	//设置配置
	VOID SetGameMatchOption(tagGameMatchOption * pGameMatchOption);
	//获取配置
	tagGameMatchOption * GetGameMatchOption() { return m_pGameMatchOption; }
	//设置id
	VOID SetGameKindID(WORD wKindID) { m_wKindID = wKindID; }

	//功能函数
private:
	//显示子项
	bool ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd); 
	//创建子项
	VOID CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd);

	//事件函数
public:
	//类型变更
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//报名配置
class MODULE_MANAGER_CLASS CDlgMatchSignup1 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

    //控件变量
protected:
    CRewardListControl			m_RewardListControl;			//列表控件

    //重载函数
protected:
   
    //控件绑定
    virtual VOID DoDataExchange(CDataExchange * pDX);

    //辅助函数
protected:
    //设置配置
    VOID SetMatchRewardOption(tagMatchRewardInfo * pMatchRewardInfo);
    //获取配置
    bool GetMatchRewardOption(tagMatchRewardInfo & MatchRewardInfo, WORD & wStartRankID, WORD & wEndRankID);

    //消息函数
public:
    //添加奖励
    VOID OnBnClickedAddReward();
    //修改奖励
    VOID OnBnClickedModifyReward();
    //删除奖励
    VOID OnBnClickedDeleteReward();

    //控件消息
protected:
    //子项变更
    VOID OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult);


public:
    afx_msg void OnEnKillfocusRankidEnd();

	//函数定义
public:
	//构造函数
	CDlgMatchSignup1();
	//析构函数
	virtual ~CDlgMatchSignup1();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//事件函数
public:
	//类型变更
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//消息函数
protected:
	//选择变更
	void OnCbnSelchangeComboFromMatch();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//报名配置
class MODULE_MANAGER_CLASS CDlgMatchSignup2 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchSignup2();
	//析构函数
	virtual ~CDlgMatchSignup2();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//撮合配置
class MODULE_MANAGER_CLASS CDlgMatchDistribute : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchDistribute();
	//析构函数
	virtual ~CDlgMatchDistribute();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//排名方式
class MODULE_MANAGER_CLASS CDlgMatchRankingMode : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchRankingMode();
	//析构函数
	virtual ~CDlgMatchRankingMode();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//事件函数
public:
	//类型变更
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);
};

//////////////////////////////////////////////////////////////////////////

//AI基本配置
class MODULE_MANAGER_CLASS CDlgMatchAndroidBase : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchAndroidBase();
	//析构函数
	virtual ~CDlgMatchAndroidBase();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckAndroidWin();
    afx_msg void OnEnKillfocusEditInitAndroidMax();

    afx_msg void OnEnKillfocusEditTotalSection();
    afx_msg void OnEnChangeEditTotalSection();
   
};


//////////////////////////////////////////////////////////////////////////

//定时赛类
class MODULE_MANAGER_CLASS CDlgMatchLockTime : public CDlgMatchItem
{
    friend class CDlgMatchWizardStep1;
	//函数定义
public:
	//构造函数
	CDlgMatchLockTime();
	//析构函数
	virtual ~CDlgMatchLockTime();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnEnKillfocusInitScore();
    afx_msg void OnEnKillfocusMinMatchUser();
};

//////////////////////////////////////////////////////////////////////////

//及时赛类
class MODULE_MANAGER_CLASS CDlgMatchImmediate : public CDlgMatchItem
{
    friend class CDlgMatchWizardStep1;
	//函数定义
public:
	//构造函数
	CDlgMatchImmediate();
	//析构函数
	virtual ~CDlgMatchImmediate();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnEnKillfocusInitialScore1();
   
    afx_msg void OnEnKillfocusStartUsercount();
};

//////////////////////////////////////////////////////////////////////////

//向导基础类
class MODULE_MANAGER_CLASS CDlgMatchWizardItem : public CDialog
{
	//友元定义
	friend class CDlgMatchWizard;

	//状态变量
private:
	bool							m_bSaveData;						//保存标志

	//变量定义
protected:
	CDlgMatchWizard *				m_pDlgMatchWizard;					//向导指针
	tagGameMatchOption *			m_pGameMatchOption;					//比赛配置
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//模块信息
	CRewardInfoBuffer *				m_pRewardInfoBuffer;				//奖励数据	

	//函数定义
protected:
	//构造函数
	CDlgMatchWizardItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgMatchWizardItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//激活子项
	virtual VOID OnMatchItemActive()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//创建向导
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);
public:
	//配置向导
	CDlgMatchWizard *	GetDlgMatchWizard(){ return m_pDlgMatchWizard; }
	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//公共向导
class MODULE_MANAGER_CLASS CDlgMatchWizardStep1 : public CDlgMatchWizardItem
{
	//控件变量
protected:	
	//CDlgMatchItem *					m_pDlgMatchItem[5];				//子项数组
	CDlgMatchItem *					m_pDlgMatchItem[4];				//子项数组
	CDlgMatchSignup1				m_DlgMatchSignup1;				//报名配置
	CDlgMatchSignup2				m_DlgMatchSignup2;				//报名配置
	CDlgMatchDistribute				m_DlgMatchDistribute;			//配桌配置	
	CDlgMatchAndroidBase			m_DlgMatchAndroidBase;			//AI基本配置
	CDlgMatchRankingMode			m_DlgMatchRankingMode;			//排名配置
    CDlgMatchLockTime               m_DlgMatchLockTime;
    CDlgMatchImmediate              m_DlgMatchImmediate;

	//函数定义
public:
	//构造函数
	CDlgMatchWizardStep1();
	//析构函数
	virtual ~CDlgMatchWizardStep1();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//激活子项
	virtual VOID OnMatchItemActive();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//基础配置
	VOID InitBaseOption();
	//保存配置
	bool SaveBaseOption();
	//激活子项
	VOID ActiveOptionItem(BYTE cbItemIndex);

	//消息函数
protected:
	//选择变更
	void OnCbnSelchangeComboMatchType();	


	DECLARE_MESSAGE_MAP()	
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton4();
};

//////////////////////////////////////////////////////////////////////////

//比赛向导
class MODULE_MANAGER_CLASS CDlgMatchWizardStep2 : public CDlgMatchWizardItem
{
	//变量定义
protected:
	CDlgMatchItem *				m_pDlgMatchItem;				//比赛子项
	CDlgMatchLockTime			m_DlgMatchLockTime;				//定时赛
	CDlgMatchImmediate			m_DlgMatchImmediate;			//即时赛

	//函数定义
public:
	//构造函数
	CDlgMatchWizardStep2();
	//析构函数
	virtual ~CDlgMatchWizardStep2();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//激活事件
	virtual VOID OnMatchItemActive();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	
};

//////////////////////////////////////////////////////////////////////////

//比赛向导
class MODULE_MANAGER_CLASS CDlgMatchWizard : public CDialog
{
	//友元定义
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;

	//变量定义
protected:
	WORD							m_wActiveIndex;						//活动索引
	CDlgMatchWizardItem *			m_pWizardItem[2];					//向导指针

	//配置变量
protected:	
	WORD							m_wKindID;							//类型标识
	CMatchInfoBuffer				m_MatchInfoBuffer;					//模块信息
	CRewardInfoBuffer				m_RewardInfoBuffer;					//奖励数据	
	tagGameMatchOption				m_GameMatchOption;					//比赛配置		
	
	//控件变量
protected:
	CDlgMatchWizardStep1			m_MatchWizardStep1;					//设置步骤
	CDlgMatchWizardStep2			m_MatchWizardStep2;					//设置步骤

	//函数定义
public:
	//构造函数
	CDlgMatchWizard();
	//析构函数
	virtual ~CDlgMatchWizard();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//配置函数
public:
	//设置类型
	VOID SetGameKindID(WORD wKindID) { m_wKindID=wKindID; }
	//获取类型
	WORD GetGameKindID() { return m_wKindID; }
	//获取配置
	VOID GetGameMatchOption(tagGameMatchOption & GameMatchOption);	
	//设置配置
	VOID SetGameMatchOption(tagGameMatchOption & GameMatchOption);
	//设置

	//内部函数
private:	
	//激活向导
	bool ActiveWizardItem(WORD wIndex);	
	//奖励转换
	VOID TransferMatchReward(CString & strRewardGold,CString & strRewardMedal,CString & szRewardExperience);

	//消息映射
public:
	//上一步
	VOID OnBnClickedLast();
	//下一步
	VOID OnBnClickedNext();
	//完成按钮
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif