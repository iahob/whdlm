#pragma once
#include "..\��Ϣ����\CMD_Baccarat.h"
#include <vector>
#include "AnimalCfg.h"

//class CAnimalCfg;
class CLog;

enum eAnimalModePrizeType
{
	eAnimalModePrizeType_Invalid =-1,
	eAnimalModePrizeType_Lost,
	eAnimalModePrizeType_Win,
	eAnimalModePrizeType_Equ,
	eAnimalModePrizeType_AllValid,

	eAnimalModePrizeType_Max,
};

enum eGamePrizeModeType
{
	eGamePrizeModeType_Invalid = -1,
	eGamePrizeModeType_StaticRang = 0, //��̬Ȧ��
	eGamePrizeModeType_DynamicRang = 1,//��̬Ȧ��
};


//��Ϸ�߼�
class CGameLogic
{
public:
	CGameLogic(void);
	~CGameLogic(void);


public:	
	virtual eGambleType GetGambleType()  =0;
	virtual const char* GetGambleName() = 0;
public:
	void CheckTimes();
	bool Init(CAnimalCfg* poAnimalCfg,CLog* poLog);
	UINT32 GetBigTimes() { return m_dwBigTimes;}
	void   AddBigTimes() 
	{ 
		m_dwBigTimes++;
	}
	void   SetBigTimes(UINT32 dwTimes) { m_dwBigTimes = dwTimes;}

	virtual UINT32 GetNextKaiJiangIndex()
	{
		return 0;
	}

	virtual UINT32 GetNextGamePrizeMode() = 0;
	UINT32 GetCurKaiJiangIndex() { return m_dwNextKaiJiangIndex;}

	UINT32 GetSmallTimes() { return m_dwSmallTimes;}
	void   AddSamllTimes() { m_dwSmallTimes++;}
	void   SetSamllTimes(UINT32 dwTimes) {m_dwSmallTimes = dwTimes;}

	UINT32 GetSysWinTimes(){ return m_dwSysWinTimes;}
	void AddSysWinTims() { m_dwSysWinTimes++;}
	void SetSysWinTims(UINT32 dwTimes)  { m_dwSysWinTimes = dwTimes;}
	UINT32 GetSysLostTimes(){ return m_dwSysLostTimes;}
	void AddSysLostTimes() { m_dwSysLostTimes++;}
	void SetSysLostTimes(UINT32 dwTimes) { m_dwSysLostTimes = dwTimes;}

	UINT64 GetTotalSysWin() { return m_qwTotalSysWin;}
	void AddTotalSysWin(UINT64 qwWin) { m_qwTotalSysWin += qwWin;}
	void SetTotalSysWin(UINT64 qwWin) { m_qwTotalSysWin = qwWin;}

	UINT64 GetTotalSysLost() { return m_qwTotalSysLost;}
	void AddTotalSysLost(UINT64 qwLost) { m_qwTotalSysLost += qwLost;}
	void SetTotalSysLost(UINT64 qwLost) { m_qwTotalSysLost = qwLost;}

	UINT64 GetCurSysWin() { return m_qwCurSysWin;}
	void SetCurSysWin(UINT64 qwSysWin) { m_qwCurSysWin = qwSysWin;}

	UINT64 GetCurSysLost() { return m_qwCurSysLost;}
	void SetCurSysLost(UINT64 qwSysLost) { m_qwCurSysLost = qwSysLost;}

	inline CAnimalCfg* GetCfg() { return m_poAnimalCfg;}
	inline CLog* GetLog() { return m_poLog;}
 
	void Reset();
	void SetReloadFlag(bool bFlag) { m_bReloadCfg = bFlag;}

	eGamePrizeModeType GetCurGamePrizeMode( ){ return m_eCurGamePrizeModeType;};
	void SetCurGamePrizeMode(eGamePrizeModeType ePrizeModeType) { m_eCurGamePrizeModeType = ePrizeModeType;}
protected:
	virtual void _Reset() = 0;
	UINT16 m_dwNextKaiJiangIndex;
	bool m_bReloadCfg;
	bool m_isCheat;
private:
	UINT32 m_dwBigTimes;	//����
	UINT32 m_dwSmallTimes;  //����

	UINT32 m_dwSysWinTimes; //ϵͳӮ����
	UINT32 m_dwSysLostTimes;//ϵͳ�����

	UINT64 m_qwTotalSysWin;  //�ӿ�ʼ������Ӯ����
	UINT64 m_qwTotalSysLost; //�ӿ�ʼ������Ӯ����
	UINT64 m_qwCurSysWin;   //��ǰ��ϵͳ�����
	UINT64 m_qwCurSysLost;  //��ǰ��ϵͳ�����

	eGamePrizeModeType m_eCurGamePrizeModeType;

	CAnimalCfg* m_poAnimalCfg;
	CLog	  * m_poLog;
};
//////////////////////////////////////////////////////////////////////////

typedef struct tagSTAnimalPrizeInfo
{
	eAnimalType eAnimal;
	eColorType eColor;
	eAnimalPrizeMode ePrizeMode;
	//UINT64  qwMinPrize;
	//UINT64  qwMaxPrize;
}STAnimalPrizeInfo;


typedef struct tagSTModePrizeValue
{
	UINT64  qwMinPrize;
	UINT64  qwMaxPrize;
	bool    bValid;
	eAnimalModePrizeType ePrizeType;
	UINT32 dwRepeatTime;
}STModePrizeValue;

typedef struct tagSTSingleAnimalSingleColorPrizeValue
{
	UINT64		qwPrize;
	eAnimalType eAnimal;
	eColorType eColor;
}STSingleAnimalSingleColorPrizeValue;

typedef std::vector<STAnimalPrizeInfo> VECTOR_STANIMALPRIZEINFO;
typedef VECTOR_STANIMALPRIZEINFO::iterator VECTOR_STANIMALPRIZEINFOIt;

typedef std::vector<STSingleAnimalSingleColorPrizeValue> VECOTR_SASCPRIZEVALUE;
typedef VECOTR_SASCPRIZEVALUE::iterator				     VECOTR_SASCPRIZEVALUEIt;


class CAnimalGame: public CGameLogic
{
public:
	CAnimalGame(void){};
	~CAnimalGame(void){};

public:	
	virtual eGambleType GetGambleType()
	{
		return eGambleType_AnimalGame;
	}
	virtual const char* GetGambleName()
	{
		return "[3d����]";
	}

public:
	bool CaluWin(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	bool RandKaiJiang();


	virtual UINT32 GetNextKaiJiangIndex();

	virtual UINT32 GetNextGamePrizeMode();

	bool RandMul();
	void GetRandMul(UINT32 arrAnimalMul[eAnimalType_Max][eColorType_Max]);
	void GetColorRate(UINT32 arrColorRate[eColorType_Max]);
	
	//���ÿ��ƶ��￪�����
	void SetCheatAnimalPrize(STAnimalPrizeInfo *stCheatAnimalPrize)
	{
		m_isCheat = true;
		m_stCheatAnimalPrize.eAnimal = stCheatAnimalPrize->eAnimal;
		m_stCheatAnimalPrize.eColor = stCheatAnimalPrize->eColor;
		m_stCheatAnimalPrize.ePrizeMode = stCheatAnimalPrize->ePrizeMode;
	};

	//UINT32 GetTotalRepeatModeTimes() { return m_dwTotalRepeatModeTime;}
	//void   SetTotalRepeatModeTimes(UINT32 dwTimes) { m_dwTotalRepeatModeTime = dwTimes;}
	//void  ResetTotalRepeatModeTimes(){ m_dwTotalRepeatModeTime = 0;}

	UINT32 GetCurRepeatModeTimes() { return m_dwCurRepeateModeTime;}
	void   SetCurRepeatModeTimes(UINT32 dwTimes) { m_dwCurRepeateModeTime = dwTimes;}
	void  ResetCurRepeatModeTimes(){ m_dwCurRepeateModeTime = 0;}

	UINT64 GetCurSysModeSysPrize() { return m_qwCurSysModeSysPrize;}
	void   SetCurSysModeSysPrize(UINT64 qwPrize) { m_qwCurSysModeSysPrize = qwPrize;}
	void  ResetCurSysModeSysPrize(){ m_qwCurSysModeSysPrize = 0;}

	UINT32 GetCurFlashModeMul() { return m_dwFlashModeMul;}
	void   SetCurFlashModeMul(UINT32 dwMul) { m_dwFlashModeMul = dwMul;}
	void  ResetCurFlashModeMul(){ m_dwFlashModeMul = 0;}

	//�����ҵĳɼ�
	INT64 GetScore(UINT64 arriPlayerAnimalJettonScore[eAnimalType_Max][eColorType_Max]);

	STAnimalPrizeInfo& GetAnimalPrize() { return m_stCurAnimalPrize;}

	//void GetAnimalRepeatModeAnimalPrize(STAnimalInfo arrstRepeatModePrize[20]);

	bool GetAnimalPrizeEx(STAnimalPrize& stAnimalPrize);


	UINT32 GetKaiJiangCount() { return m_dwKaiJiangCount;}
	void SetKaiJiangCount(UINT32 dwCount) { m_dwKaiJiangCount = dwCount;}

	//�Ƿ���999ģʽֻ�ܿ������ﵥ��ɫģʽ
	bool IsForbiddonDynamicRangOtherPrizeMode() {
		return true;
	}

private:
	inline bool _IsValidAnimal(eAnimalType eAnimal)
	{
		return ((eAnimal > eAnimalType_Invalid )&& (eAnimal < eAnimalType_Max));
	}
	bool _IsValidColor(eColorType eColor)
	{
		return ((eColor > eColorType_Invalid )&& (eColor  < eColorType_Max));
	}
	bool _IsValidAnimalPrizeMode(eAnimalPrizeMode eMode)
	{
		return ((eMode > eAnimalPrizeMode_Invalid )&& (eMode  < eAnimalPrizeMode_Max));
	}
	virtual void _Reset();
private:
	bool _CaluSascPrizeValue(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);

	UINT64 _CaluModePrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	UINT64 _CaluModePrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor,eAnimalPrizeMode eMode, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid,UINT32& dwRepeatTime);
	UINT64 _CaluModePrize_SingleAnimalSingleColor(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid);
	UINT64 _CaluModePrize_AllAnimalSingleColr(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid);
	UINT64 _CaluModePrize_AllColorSingleAnimal(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid);
	UINT64 _CaluModePrize_SysPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid);
	UINT64 _CaluModePrize_RepeatTime(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid,UINT32& dwRepeatTime);
	UINT64 _CaluModePrize_Flash(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max],eAnimalType eAnimal, eColorType eColor, UINT64& dwMinPrize,UINT64& dwMaxPrize,bool& bValid);

	//Ӯ��ģʽ
	void _CaluPrizeType(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	
	bool _CaluList(UINT32 dwMaxWinPerTimeLimit,UINT32 dwMaxLostPerTimeLimit);

	bool _CaluFinalPrizeAnimalMode(UINT64 qwTotalSysWin,UINT64 qwTotalSysLost);

	bool _CaluFinalPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_SingleAnimalSingleColor(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_AllAnimalSingleColr(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_AllColorSingleAnimal(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_SysPrize(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_RepeatTime(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);
	void _CaluFinalPrize_Flash(UINT64 arriTotalAnimalJettonScore[eAnimalType_Max][eColorType_Max]);

	
private:
	//���ж���������ɫ������ģʽ���н��������(�������������ж����Ͷע����ȷ��)
	STModePrizeValue m_arrModePrizeValue[eAnimalType_Max][eColorType_Max][eAnimalPrizeMode_Max];
	UINT32			 m_arrAnimalMul[eAnimalType_Max][eColorType_Max];	//����ı���
	UINT32			 m_arrColorRate[eColorType_Max];	//��ɫ�ֲ� 
	VECTOR_STANIMALPRIZEINFO m_vtMustWin;
	VECTOR_STANIMALPRIZEINFO m_vtMustLost;
	VECTOR_STANIMALPRIZEINFO m_vtEqui;
	VECTOR_STANIMALPRIZEINFO m_vtAllValid;
	VECTOR_STANIMALPRIZEINFO m_vtSpecialLost;
	STAnimalPrizeInfo m_stCurAnimalPrize;
	STAnimalPrizeInfo m_arrstRepeatModePrize[12];


	//UINT32 m_dwTotalRepeatModeTime;
	UINT32 m_dwCurRepeateModeTime;
	UINT64 m_qwCurSysModeSysPrize;
	UINT32 m_dwFlashModeMul;

	VECOTR_SASCPRIZEVALUE	m_vtSascPrizeValue;

	//////////////////////////////////////////////////////////////////////////
	VECTOR_KAIJIANG m_vtKaiJiang;
	UINT32 m_dwKaiJiangCount;

	//���ƶ��￪�����
	STAnimalPrizeInfo m_stCheatAnimalPrize;

};


//////////////////////////////////////////////////////////////////////////

typedef struct tagSTEnjoyGameWinInfo
{
	eEnjoyGameType eEnjoyGame;
	eAnimalModePrizeType ePrizeType;

}STEnjoyGameWinInfo;
typedef std::vector<STEnjoyGameWinInfo> VECTOR_ENJOYGAME;
typedef VECTOR_ENJOYGAME::iterator VECTOR_ENJOYGAMEIt;
class CEnjoyGame: public CGameLogic 
{
public:
	virtual eGambleType GetGambleType()
	{
		return eGambleType_EnjoyGame;
	}
	virtual const char* GetGambleName()
	{
		return "[3dׯ�к�]";
	}
public:
	bool CaluWin(UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max]);

	bool RandMul();
	bool RandZhuangKaiJiang();
	void GetRandMul(UINT32 arrEnjoyGameMul[eEnjoyGameType_Max]);

	UINT32 GetNextKaiJiangIndex();

	virtual UINT32 GetNextGamePrizeMode();

	void SetCheatEnjoyPrize(STEnjoyGameWinInfo *stCheatEnjoyWinInfo)
	{
		m_isCheat = true;
		m_stCheatEnjoyWinInfo.eEnjoyGame = stCheatEnjoyWinInfo->eEnjoyGame;
		m_stCheatEnjoyWinInfo.ePrizeType = stCheatEnjoyWinInfo->ePrizeType;
	};

	eEnjoyGameType GetCurPrizeType() { return m_eCurPrizeType;}

	INT64 GetScore(UINT64 arriPlayerEnjoyGameJettonScore[eEnjoyGameType_Max]);

	UINT32 GetZhuangKaiJiangCount() { return m_dwZhuangKaiJiangCount;}

	void SetZhuangKaiJiangCount(UINT32 dwCount) { m_dwZhuangKaiJiangCount = dwCount;}

protected:
	inline bool _IsValidEnjoyGameType(eEnjoyGameType eEnjoyGame)
	{
		return ((eEnjoyGame > eEnjoyGameType_Invalid )&& (eEnjoyGame < eEnjoyGameType_Max));
	}
	virtual void _Reset();

	bool _CaluPrizeValue(UINT64 arriTotalEnjoyGameJettonScore[eEnjoyGameType_Max]);

	bool _CaluList(UINT32 dwMaxWinPerTimeLimit,UINT32 dwMaxLostPerTimeLimit);
	bool _CaluWinList(UINT32 dwMaxWinPerTimeLimit);
	bool _CaluLostList(UINT32 dwMaxLostPerTimeLimit);
	bool _CaluEquiList(UINT32 dwMaxWinPerTimeLimit,UINT32 dwMaxLostPerTimeLimit);
	bool _CaluAllList(UINT32 dwMaxWinPerTimeLimit,UINT32 dwMaxLostPerTimeLimit);

	bool _CaluFinalPrize(UINT64 qwTotalSysWin,UINT64 qwTotalSysLost);

private:

	INT64  m_arrEnjoyGamePrizeValue[eEnjoyGameType_Max];
	UINT32			 m_arrAnimalMul[eEnjoyGameType_Max];	//����ı���

	eEnjoyGameType m_eCurPrizeType;

	VECTOR_ENJOYGAME m_vtMustWin;
	VECTOR_ENJOYGAME m_vtMustLost;
	VECTOR_ENJOYGAME m_vtEqu;
	VECTOR_ENJOYGAME m_vtAllValid;
	VECTOR_ENJOYGAME m_vtSpecialLost;


	VECTOR_ZHUANGKAIJIANG m_vtZhuangKaiJiang;
	UINT32 m_dwZhuangKaiJiangCount;

	//����ׯ�кͿ������
	STEnjoyGameWinInfo m_stCheatEnjoyWinInfo;
};
