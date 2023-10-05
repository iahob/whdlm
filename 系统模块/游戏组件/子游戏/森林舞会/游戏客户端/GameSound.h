#pragma once
#include <map>

class CDirectSound;


class GameSound
{
public:
	enum SoundIndex
	{
		START =0,     //游戏开始
	    WAIT,         //压分等待；
        ROTATELOOP,   //旋转；

		ZHUANG,
		XIAN,
		HE,

		Lion,
		Panda,
		Monkey,
		Rabbit,

		R_LION,
		G_LION,
		Y_LION,

		R_Panda,
		G_Panda,
		Y_Panda,

		R_Monkey,
		G_Monkey,
		Y_Monkey,

		R_Rabbit,
		G_Rabbit,
		Y_Rabbit,

		COL_R,
		COL_G,
		COL_Y,
        
        Prize_Bonus,
		Prize_Light,
		Prize_Double,

		Mode0,
		Mode1,
		Mode2,
		Mode3,
		Mode4,
		Mode5, 
       
		Pre_1,
		Lightning,
        SoundMaxCount
	};
public:
	GameSound(void);
	~GameSound(void);
	static GameSound* Instance();
    static void Destroy();

	void Play(SoundIndex index,bool bLoop=false,bool bOnce=true);
	void Stop(SoundIndex index);
	void SetEnable(bool enable);

	void Reset();
	void Reset(SoundIndex index);
private:
	typedef std::map<CString,CDirectSound*>   SoundMap;
    typedef std::map<CString,CDirectSound*>::value_type  SoundMapVal;
 	typedef std::map<CString,CDirectSound*>::iterator    SoundMapIte;

	SoundMap          m_Soundmap;
	static GameSound* m_Instance;
	CString          m_SoundNames[SoundMaxCount];
	bool              m_PlayOnce  [SoundMaxCount];  //在一个周期内已经播放过一次；
	CString          m_CurDir;
	bool              m_bEnable;
};
