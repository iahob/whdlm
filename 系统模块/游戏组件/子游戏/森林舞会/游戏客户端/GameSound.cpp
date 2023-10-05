#include "Stdafx.h"
#include <atlconv.h>

#include ".\gamesound.h"
#include "DirectSound.h"

GameSound* GameSound::m_Instance=NULL;

GameSound::GameSound(void)
{
	 
	TCHAR tstrCurrentPath[MAX_PATH];
    memset(tstrCurrentPath, 0, MAX_PATH);
    GetCurrentDirectory(MAX_PATH, tstrCurrentPath);
	m_CurDir=(tstrCurrentPath);

	m_SoundNames[START]="Start.wav";
	 
	m_SoundNames[WAIT]= "Wait.wav" ;  
	m_SoundNames[ROTATELOOP]="Rotate_Loop.wav";

	m_SoundNames[ZHUANG]="zhuang.wav";
	m_SoundNames[XIAN]="xian.wav"; 
	m_SoundNames[HE]="he.wav"; 

	m_SoundNames[Lion]="ani_lion.wav";
	m_SoundNames[Panda]="ani_panda.wav";
	m_SoundNames[Monkey]="ani_monkey.wav";
	m_SoundNames[Rabbit]="ani_rabbit.wav";

	m_SoundNames[R_LION]="r_lion.wav";
	m_SoundNames[G_LION]="g_lion.wav";
	m_SoundNames[Y_LION]="y_lion.wav";

	m_SoundNames[R_Panda]="r_panda.wav";
	m_SoundNames[G_Panda]="g_panda.wav";
	m_SoundNames[Y_Panda]="y_panda.wav";

	m_SoundNames[R_Monkey]="r_monkey.wav";
	m_SoundNames[G_Monkey]="g_monkey.wav";
	m_SoundNames[Y_Monkey]="y_monkey.wav";

	m_SoundNames[R_Rabbit]="r_rabbit.wav";
	m_SoundNames[G_Rabbit]="g_rabbit.wav";
	m_SoundNames[Y_Rabbit]="y_rabbit.wav"; 

	m_SoundNames[COL_R]="color_r.wav";
	m_SoundNames[COL_G]="color_g.wav";
	m_SoundNames[COL_Y]="color_y.wav";

	m_SoundNames[Prize_Bonus]="prize_bonus.wav";
	m_SoundNames[Prize_Light]="prize_light.wav";
	m_SoundNames[Prize_Double]="prize_double.wav";

	m_SoundNames[Mode0]="Win_1.wav";
	m_SoundNames[Mode1]="Win_2.wav";
	m_SoundNames[Mode2]="S_Aninmal.wav";
	m_SoundNames[Mode3]="Bonus.wav";
	m_SoundNames[Mode4]="Win_4.wav";
	m_SoundNames[Mode5]="KJ_Back.wav";

	m_SoundNames[Pre_1]="Pre_1.wav";
	m_SoundNames[Lightning]="lightning.wav";

	m_bEnable=false;
	Reset();
}

GameSound::~GameSound(void)
{
	for (SoundMapIte iter = m_Soundmap.begin(); iter != m_Soundmap.end(); iter++)
	{
		CDirectSound* psound=(CDirectSound*)iter->second;
		delete psound;
		psound=NULL;
	}
	m_Soundmap.clear();

}

GameSound* GameSound::Instance()
{
	if(!m_Instance)
	{
		return m_Instance=new GameSound();
	}
	return m_Instance;
}

void GameSound::Destroy()
{
	
	if(m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

void GameSound::Play(SoundIndex index,bool bLoop,bool bOnce)
{
	if(!m_bEnable) 
		return;

	if(index>=SoundMaxCount) return;
    if(m_PlayOnce[index]) return;
	m_PlayOnce[index]=bOnce;

	CString name=(CString)m_SoundNames[index];
    SoundMapIte  ite=m_Soundmap.find(name);
    if(ite==m_Soundmap.end())
	{
		CDirectSound* psound=new CDirectSound();
		CString path=m_CurDir+"\\3DSLWG\\Music\\"+name;
 
		CFile   file;
		bool hr=false;
        hr=file.Open(path,CFile::modeRead); 
		if(!hr)
		{
			file.Close();
			return;
		}
		ULONGLONG   filelength   =   file.GetLength(); 
		CString   filebuffer; 
		file.SeekToBegin(); 
		file.Read(filebuffer.GetBufferSetLength(filelength),filelength); 

		VERIFY(psound->Create((LPVOID)filebuffer.GetBuffer()));
		m_Soundmap.insert(SoundMapVal(name,psound));
		psound->Play(0,bLoop);
		file.Close();
	}
	else
	{
		CDirectSound* psound=(CDirectSound*)ite->second;
		psound->Play(0,bLoop);
	}
}

void GameSound::Stop(SoundIndex index)
{
	if(index>=SoundMaxCount) return;

	CString strname=(CString)m_SoundNames[index];
	SoundMapIte  ite=m_Soundmap.find(strname);
	if(ite!=m_Soundmap.end())
	{
		CDirectSound* psound=(CDirectSound*)ite->second;
		psound->Stop();
	}
}

void GameSound::Reset()
{
	for(int i=0;i<SoundMaxCount;i++)
	{
		m_PlayOnce[i]=false;
	}
}

void GameSound::Reset(SoundIndex index)
{
	if(index>=SoundMaxCount) return;
	m_PlayOnce[index]=false;
}

void GameSound::SetEnable(bool enable)
{
	if(m_bEnable==enable)
		return;

	m_bEnable=enable;
	if(!m_bEnable)
	{
       for(SoundMapIte ite=m_Soundmap.begin();ite!=m_Soundmap.end();ite++)
	   {
			CDirectSound* sound=(CDirectSound*)ite->second;
			sound->Stop();
	   }
	}
}