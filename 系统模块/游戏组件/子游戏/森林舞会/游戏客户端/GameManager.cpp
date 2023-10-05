#include "Stdafx.h"
#include "GameManager.h"
#include "Animal.h"
#include "RenderCanvas.h"
//#include ".\log\log.h"
using namespace irr;


class MyAnimationEndCallBack : public scene::IAnimationEndCallBack
{
public:
	virtual void OnAnimationEnd(scene::IAnimatedMeshSceneNode* node)
	{
		if(node == m_pGameManager->m_pNodeGaiZi)
		{
			m_pGameManager->m_pNodeGaiZi->setCurrentFrame(0);
			m_pGameManager->m_pNodeGaiZi->setFrameLoop(1000, 1000);
		}
	 	else if(node == m_pGameManager->m_pXingXing)
		{
			m_pGameManager->m_pXingXing->setCurrentFrame(0);
			m_pGameManager->m_pXingXing->setFrameLoop(1000, 1000);
		} 
			
			 
	}
	
	GameManager*	m_pGameManager;
};

MyAnimationEndCallBack g_AnimationEndCallBack;


GameManager::GameManager( IrrlichtDevice* pDevice ) :
m_pDevice(pDevice)
{
	m_ZhiZhengRotation = core::vector3df(0.0f, 0.0f, 0.0f);

	m_RotateStartAngle = m_RotateEndAngle = 0;
	m_RotateTimePassed = m_RotateTime = 0;

	m_AnimalRotateTime = m_AnimalRotateTimePassed = 0;
	m_AnimalRotateEndAngle = m_AnimalRotateStartAngle = 0;

	m_GameStateRemainTime = 0;
	m_GameStateTime = 0;
	m_GameState=-1;

	m_WinIndex = 0;

	g_AnimationEndCallBack.m_pGameManager = this;
	m_pWinAnimal=NULL;
	enableshansui=false;

	m_STAnimalPrize.ePrizeMode=eAnimalPrizeMode_Invalid;

	for(int i=0;i<6;i++)
	{
		m_NodeMode[i]=NULL;
	}

	intervalmode=0;
	rotationnum=0;
	mode1texindex=0;
	qiangbitexnum=0;
    isSelAniSound=false;
}

GameManager::~GameManager()
{
	 GameSound::Instance()->Destroy();

	for(int i = 0; i < ANIMALCOUNT; i++)
	{
		delete m_pAnimal[i];
	}
}

bool GameManager::InitGame()
{
	GameSound::Instance()->Play(GameSound::START);

	scene::ISceneManager* smgr = m_pDevice->getSceneManager();
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();

	// 载入场景
 	if(!LoadBoard())
 		return false;

	// 载入动物
	if(!LoadAnimal())
		return false;
	
	loadmode0();
	loadmode1();
	loadmode2();
	loadmode3();
	loadmode4();
	loadmode5();

	SetQiangbiAnimation(true);

	//SetZhiZhengRot( (int)m_NodeZhiZhengRoot->getRotation().Y, 4000, 20*1000);
	return true;
}

bool GameManager::LoadBoard()
{

	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;

	//driver->setAmbientLight(video::SColor(0,160,160,160));

	//pSceneManager->addLightSceneNode(0, core::vector3df(0,200,0),
	//	video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 800.0f);

	scene::ISceneNodeAnimator* pTmpNodeAni = NULL;
	// 指针
	m = pSceneManager->getMesh( DXPATH"Scene/zhizhen02.x" );			// 指针1
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodeZhiZheng1 = pSceneManager->addAnimatedMeshSceneNode(m);

	m = pSceneManager->getMesh( DXPATH"Scene/zhizheng02.x" );			// 指针2
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodeZhiZheng2 = pSceneManager->addAnimatedMeshSceneNode(m);

	m = pSceneManager->getMesh(DXPATH"Scene/gaizi.md2");				// 盖子
 	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodeGaiZi = pSceneManager->addAnimatedMeshSceneNode(m);
	m_pNodeGaiZi->setFrameLoop(0,0);
//	m_pNodeGaiZi->setScale(core::vector3df(1.05f,1.05f,1.0f));
	//video::ITexture* pTexture = driver->getTexture(DXPATH"Scene/gaizi.tga");
	//m_pNodeGaiZi->setMaterialTexture(0, pTexture);

	video::ITexture* pTexture = driver->getTexture(DXPATH"Scene/gaizi.tga");
	m_pNodeGaiZi->setMaterialTexture(0, pTexture);
	//video::ITexture* pTexture22 = driver->getTexture(DXPATH"Scene/gaizi.tga");
	//m_pNodeGaiZi->setMaterialTexture(1, pTexture22);
	//m_pNodeGaiZi->setMaterialType(video::EMT_SPHERE_MAP);

	m_pNodeGaiZi->setAnimationEndCallback(&g_AnimationEndCallBack);
	

/*	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang4/quanquan.md2");				// 盖子
 	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodequan = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang4/guanghuan01.tga");
    m_pNodequan->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	 
 	m_pNodequan->setMaterialTexture(0, pTexture1);
//	 video::ITexture* pTexture2 = driver->getTexture(DXPATH"Scene/kaijiang4/kaijiang04-1.tga");
//	 m_pNodequan->setMaterialTexture(1, pTexture2);
//	  m_pNodequan->setMaterialType(video::EMT_REFLECTION_2_LAYER);
//// 	m_pNodequan->setMaterialTexture(0, pTexture1);
////	m_pNodequan->setAnimationEndCallback(&g_AnimationEndCallBack);
//	
 	m_pNodequan->setFrameLoop(0, 200);
//	//m_pNodequan->setCurrentFrame(17);
////	m_pNodequan->setCurrentFrame(1010);
 	m_pNodequan->setAnimationSpeed(100);
// 
 //	m_pNodequan->setLoopMode(false);
     core::vector3df EndPos(0.0f, 90.0f, 0.0f);
// 
 		m_pNodequan->setPosition( EndPos );
 	m_pNodequan->setScale(core::vector3df(0.8f, 0.8f, 0.8f));*/

	m_NodeZhiZhengRoot = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);
	m_NodeZhiZhengRoot->addChild(m_pNodeZhiZheng1);
	m_NodeZhiZhengRoot->addChild(m_pNodeZhiZheng2);
	m_NodeZhiZhengRoot->addChild(m_pNodeGaiZi);
 	pZhiZhengRotAni = pSceneManager->createRotationAnimator(core::vector3df(0.0f, 0.0f, 0.0f));
 	m_NodeZhiZhengRoot->addAnimator(pZhiZhengRotAni);
 	pZhiZhengRotAni->drop();

	m_NodeZhiZhengRoot->setRotation(m_ZhiZhengRotation);

	// 底座1
	m = pSceneManager->getMesh( DXPATH"Scene/dizuo03.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	pSceneManager->addAnimatedMeshSceneNode(m);

	// 底座2
	m = pSceneManager->getMesh( DXPATH"Scene/dizuo02.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	pSceneManager->addAnimatedMeshSceneNode(m);
	
	// 转盘
	m = pSceneManager->getMesh( DXPATH"Scene/yuanpan01.x" );		// 转盘1
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodeZhuangPan1 =	pSceneManager->addAnimatedMeshSceneNode(m);
	m_pNodeZhuangPan1->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	pTmpNodeAni = pSceneManager->createRotationAnimator(core::vector3df(0.0f, 1.0f, 0.0f));	// 转盘1旋转
	m_pNodeZhuangPan1->addAnimator(pTmpNodeAni);
	pTmpNodeAni->drop();

	m = pSceneManager->getMesh( DXPATH"Scene/yuanpan02.x" );		// 转盘2
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pNodeZhuangPan2 = pSceneManager->addAnimatedMeshSceneNode(m);
	m_pNodeZhuangPan2->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	pTmpNodeAni = pSceneManager->createRotationAnimator(core::vector3df(0.0f, -1.0f, 0.0f));	// 转盘2旋转
	m_pNodeZhuangPan2->addAnimator(pTmpNodeAni);
	pTmpNodeAni->drop();

	// 墙壁1
	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/qiangbi01.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);

	m_pNodeqiangbi_1=m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m);

	m_qiangbi_1=driver->getTexture(DXPATH"Scene/qiangbi001.tga");

	m_qiangbi_1Texs[0]=driver->getTexture(DXPATH"Scene/qiangbi001/qiangbi001_1.tga");
	m_qiangbi_1Texs[1]=driver->getTexture(DXPATH"Scene/qiangbi001/qiangbi001_2.tga");
	m_qiangbi_1Texs[2]=driver->getTexture(DXPATH"Scene/qiangbi001/qiangbi001_3.tga");
	m_qiangbi_1Texs[3]=driver->getTexture(DXPATH"Scene/qiangbi001/qiangbi001_4.tga");
	m_qiangbi_1Texs[4]=driver->getTexture(DXPATH"Scene/qiangbi001/qiangbi001_5.tga");

	// 墙壁2
	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/qiangbi02.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);

	m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m);

	// 墙壁3
	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/qiangbi03.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);
     m_pNodeqiangbi=m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m);
	m_qiangbi3=driver->getTexture(DXPATH"Scene/qiangbi03_2.tga");

	m_qiangbiTexs[0]=driver->getTexture(DXPATH"Scene/qiangbi3/qiangbi03_1.tga");
	m_qiangbiTexs[1]=driver->getTexture(DXPATH"Scene/qiangbi3/qiangbi03_2.tga");
	m_qiangbiTexs[2]=driver->getTexture(DXPATH"Scene/qiangbi3/qiangbi03_3.tga");
	m_qiangbiTexs[3]=driver->getTexture(DXPATH"Scene/qiangbi3/qiangbi03_4.tga");
	m_qiangbiTexs[4]=driver->getTexture(DXPATH"Scene/qiangbi3/qiangbi03_5.tga");

   

	// 色板(有四种颜色的)和底板
	m_pNodeSeBanRoot = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,58.0f,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);
	m_pNodeAnimalRoot = pSceneManager->addMeshSceneNode(NULL, m_pNodeSeBanRoot, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);
	m_pNodeAni = pSceneManager->createRotationAnimator(core::vector3df(0.0f, 0.0f, 0.0f));
	m_pNodeAnimalRoot->addAnimator(m_pNodeAni);
	m_pNodeAni->drop();

	m_pGreenTexture = driver->getTexture(DXPATH"Scene/green.png");			// 四色贴图
	m_pRedTexture = driver->getTexture(DXPATH"Scene/red.tga");
	m_pYellowTexture = driver->getTexture(DXPATH"Scene/yellow.tga");

	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/seban01.x" );	// 色板1
	m->setMaterialFlag(video::EMF_LIGHTING, false);

	for(int i = 0; i < ANIMALCOUNT; i++)
	{
		m_pNodeSeBan1[i] = m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m, m_pNodeSeBanRoot);
		float x = SeBan_Radius * sin( core::PI * 2.0f * i / ANIMALCOUNT );
		float z = SeBan_Radius * cos( core::PI * 2.0f * i / ANIMALCOUNT );

		m_pNodeSeBan1[i]->setPosition(core::vector3df(x, 0.0f, z));
		m_pNodeSeBan1[i]->setRotation(core::vector3df(0.0f, 360.0f * i / ANIMALCOUNT, 0.0f));
		m_pNodeSeBan1[i]->setScale(core::vector3df(0.75f, 0.75f ,0.75f));
	}
	
	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/seban02.x" );	// 色板2
	m->setMaterialFlag(video::EMF_LIGHTING, false);

	for(int i = 0; i < ANIMALCOUNT; i++)
	{
		m_pNodeSeBan2[i] = m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m, m_pNodeSeBan1[i]);
		float x = SeBan_Radius * sin( core::PI * 2.0f * i / ANIMALCOUNT );
		float z = SeBan_Radius * cos( core::PI * 2.0f * i / ANIMALCOUNT );

		int ClrIndex = rand() * 2.9 / RAND_MAX;
		SetSeBanColor(i, ClrIndex);
		m_pNodeSeBan2[i]->setMaterialType(video::EMT_LIGHTMAP_M2);
	}


	m = m_pDevice->getSceneManager()->getMesh( DXPATH"Scene/diban01.x" );	// 底板
	m->setMaterialFlag(video::EMF_LIGHTING, false);

	for(int i = 0; i < ANIMALCOUNT; i++)		
	{
		m_pNodeDiBan[i] = m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m, m_pNodeAnimalRoot);
		float x = DiBan_Radius * sin( core::PI * 2.0f * i / ANIMALCOUNT );
		float z = DiBan_Radius * cos( core::PI * 2.0f * i / ANIMALCOUNT );

		m_pNodeDiBan[i]->setPosition(core::vector3df(x, 0.0f, z));
		m_pNodeDiBan[i]->setRotation(core::vector3df(0.0f, 360.0f * i / ANIMALCOUNT, 0.0f));
		m_pNodeDiBan[i]->setScale(core::vector3df(0.8f, 0.8f, 0.8f));
	}

	return true;
}

bool GameManager::LoadAnimal()
{
	irr::scene::ISceneNode* m_pModel = NULL;
	io::path filename(DXPATH"Monkey/Monkey.x");

	// 载入模型文件
	m_pMonkey = m_pDevice->getSceneManager()->getMesh( DXPATH"Monkey/Monkey.x" );
	m_pPanda = m_pDevice->getSceneManager()->getMesh( DXPATH"Panda/Panda.X" );
	m_pLion = m_pDevice->getSceneManager()->getMesh( DXPATH"Lion/Lion.X" );
	m_pRabbit = m_pDevice->getSceneManager()->getMesh( DXPATH"Rabbit/Rabbit.X" );

	for(int i = 0; i < ANIMALCOUNT; i++)
	{
		m_pNodeAnimal[i] = m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m_pPanda, m_pNodeDiBan[i]);
		m_pNodeAnimal[i]->setMaterialFlag(video::EMF_LIGHTING, false);
		m_pNodeAnimal[i]->setAnimationSpeed(30);
		m_pNodeAnimal[i]->setCurrentFrame((float)rand() * 100 / RAND_MAX);
		m_pNodeAnimal[i]->setScale(core::vector3df(0.3f, 0.3f, 0.3f));
		m_pNodeAnimal[i]->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

		//m_pNodeAnimal[i]->setFrameLoop()

		m_pAnimal[i] = new Animal;
		m_pAnimal[i]->SetAnimateNode(m_pNodeAnimal[i]);
		m_pAnimal[i]->SetMonkey(m_pMonkey);
		m_pAnimal[i]->SetPanda(m_pPanda);
		m_pAnimal[i]->SetLion(m_pLion);
		m_pAnimal[i]->SetRabbit(m_pRabbit);

		//m_pAnimal[i]->SetAnimate( 1 );	// 初始待机动作
	}

	m_OriginalScale = m_pAnimal[0]->GetAnimateNode()->getScale().X;

	// 动物分布
// 	int LionCount = 4;
// 	int PandaCount = 4;
// 	int MonkeyCount = 8;
// 	int RabbitCount = 8;
// 
// 	for(int AnimalIndex = 0; AnimalIndex < ANIMALCOUNT; AnimalIndex++)
// 	{
// 		int WhichAnimal = rand() * 4 / RAND_MAX;
// 
// 		if(WhichAnimal == eAnimalType_Lion && LionCount > 0)
// 		{	// 狮子
// 			m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Lion);
// 			LionCount--;
// 		}
// 		else if(WhichAnimal == eAnimalType_Panda && PandaCount > 0)
// 		{	// 熊猫
// 			m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Panda);
// 			PandaCount--;
// 		}
// 		else if(WhichAnimal == eAnimalType_Monkey && MonkeyCount > 0)
// 		{	// 猴子
// 			m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Monkey);
// 			MonkeyCount--;
// 		}
// 		else if(WhichAnimal == eAnimalType_Rabbit && RabbitCount > 0)
// 		{	// 兔子
// 			m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Rabbit);
// 			RabbitCount--;
// 		}
// 		else
// 		{
// 			if(LionCount > 0)
// 			{
// 				m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Lion);
// 				LionCount--;
// 			}
// 			else if( PandaCount > 0 )
// 			{
// 				m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Panda);
// 				PandaCount--;
// 			}
// 			else if( MonkeyCount > 0 )
// 			{
// 				m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Monkey);
// 				MonkeyCount--;
// 			}
// 			else if( RabbitCount > 0 )
// 			{
// 				m_pAnimal[AnimalIndex]->SetAnimal(eAnimalType_Rabbit);
// 				RabbitCount--;
// 			} 
// 			else
// 				_ASSERT(false);
// 		}	
// 	}
// 	_ASSERT(LionCount == 0 && PandaCount == 0 && MonkeyCount == 0 && RabbitCount == 0);
	
	m_pAnimal[0]->SetAnimal(eAnimalType_Lion);
	m_pAnimal[1]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[2]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[3]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[4]->SetAnimal(eAnimalType_Panda);
	m_pAnimal[5]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[6]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[7]->SetAnimal(eAnimalType_Lion);
	m_pAnimal[8]->SetAnimal(eAnimalType_Panda);
	m_pAnimal[9]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[10]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[11]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[12]->SetAnimal(eAnimalType_Lion);
	m_pAnimal[13]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[14]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[15]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[16]->SetAnimal(eAnimalType_Panda);
	m_pAnimal[17]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[18]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[19]->SetAnimal(eAnimalType_Lion);
	m_pAnimal[20]->SetAnimal(eAnimalType_Panda);
	m_pAnimal[21]->SetAnimal(eAnimalType_Monkey);
	m_pAnimal[22]->SetAnimal(eAnimalType_Rabbit);
	m_pAnimal[23]->SetAnimal(eAnimalType_Monkey);

	//for(int i = 0; i < ANIMALCOUNT;i++)
	//{
	//	m_pAnimal[i]->SetAnimal(eAnimalType_Lion);
	//	if(i == 0)
	//		m_pAnimal[i]->SetAnimal(eAnimalType_Lion);
	//	else if(i==1)
	//		m_pAnimal[i]->SetAnimal(eAnimalType_Panda);
	//	else if(i==2)
	//		m_pAnimal[i]->SetAnimal(eAnimalType_Monkey);
	//}

	SetAnimalsAnimation(1);
	return true;
}

void GameManager::Tick( unsigned int LastTime, unsigned int FrameTime )
{
	// 游戏状态和时间
	CStringW StateStr;
	CStringW TimeStr;
	switch(RenderCanvas::Instance()->GetGameManager()->GetGameState())
	{
	case SUB_S_GAME_FREE:	// free
		TickFree(LastTime, FrameTime);
		break;
	case SUB_S_GAME_START: // Start
		TickStart(LastTime, FrameTime);
		break;
	case SUB_S_GAME_END: // end
		TickEnd(LastTime, FrameTime);
		break;
	}

	// 游戏状态时间显示
	if(FrameTime>1000)
		FrameTime=1000;

	m_GameStateRemainTime -= FrameTime;

	/*char str[62];
	_snprintf(str,62," FrameTime Tick %d,m_GameStateRemainTime id %d",FrameTime,m_GameStateRemainTime);
	LOG_FILE(str);*/


	// 指针旋转控制
  	if(m_RotateTime > 0 && m_RotateTimePassed < m_RotateTime && m_RotateStartAngle != m_RotateEndAngle)
	{
		float passedTime;
		float Time2 = (float)m_RotateTime/2;
		if( m_RotateTimePassed > Time2 )
		{
			float p = 1.0f -  pow( ( ((float)m_RotateTime -  m_RotateTimePassed ) / Time2 ), 3);
			passedTime = Time2 * p + Time2;
		}
		else
		{
			float p = pow( ( (m_RotateTimePassed) / Time2 ), 3 );
			passedTime = Time2 * p;
		}

		float CurAngle = passedTime / m_RotateTime * (m_RotateEndAngle - m_RotateStartAngle);

		// m_NodeZhiZhengRoot->setRotation(core::vector3df(0.0f, m_RotateStartAngle + CurAngle, 0.0f));
         m_NodeZhiZhengRoot->setRotation(core::vector3df(0.0f,-( m_RotateStartAngle + CurAngle), 0.0f));
		m_RotateTimePassed += FrameTime;
	}  

	// 动物旋转控制
	if(m_AnimalRotateTime > 0 && m_AnimalRotateTimePassed < m_AnimalRotateTime && m_AnimalRotateStartAngle != m_AnimalRotateEndAngle)
	{
		float passedTime;
		float Time2 = (float)m_AnimalRotateTime/2;
		if( m_AnimalRotateTimePassed > Time2 )
		{
			float p = 1.0f -  ( ((float)m_AnimalRotateTime -  m_AnimalRotateTimePassed ) / Time2 ) * 
				( ((float)m_AnimalRotateTime - m_AnimalRotateTimePassed ) / Time2 );
			passedTime = Time2 * p + Time2;
		}
		else
		{
			float p = ( (m_AnimalRotateTimePassed) / Time2 ) * ( (m_AnimalRotateTimePassed) / Time2 );
			passedTime = Time2 * p;
		}

		float CurAngle = passedTime / m_AnimalRotateTime * (m_AnimalRotateEndAngle - m_AnimalRotateStartAngle);

		m_pNodeAnimalRoot->setRotation(core::vector3df(0.0f, m_AnimalRotateStartAngle + CurAngle, 0.0f));

		m_AnimalRotateTimePassed += FrameTime;

		if(m_AnimalRotateTimePassed>0 && m_AnimalRotateTimePassed>m_AnimalRotateTime-200 && bsetsound)
		{
			isSelAniSound=true; 
			bsetsound=false;
		}
	}

    QiangbiAnimationUpdate(FrameTime);
}

void GameManager::TickStart(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::WAIT,true);
	// 色板变色
	int passTime = m_GameStateTime - m_GameStateRemainTime;
	if(passTime < (m_GameStateTime/10))
	{
		int clrpos = passTime * 22 / (m_GameStateTime/10);
		//for(int i = 0; i < clrpos; i++)
		//{
		//	SetSeBanColor(i, m_SeBanClr[i]);
		//}
	 
		SetSeBanColor(clrpos, m_SeBanClr[clrpos]);
	}
	else
	if(m_StartChangeClr)
	{
		m_StartChangeClr = false;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			SetSeBanColor(i, m_SeBanClr[i]);
		}
	}

}

void GameManager::TickFree(unsigned int LastTime, unsigned int FrameTime)
{
	if(m_pWinAnimal==NULL)
		return;
 
	switch(m_STAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			TickFreemode0(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			TickFreemode1(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			TickFreemode2(LastTime,FrameTime);  
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			TickFreemode3(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			TickFreemode4(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			TickFreemode5(LastTime,FrameTime); 
		}
		break;
	}

}

void GameManager::TickEnd(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::ROTATELOOP,true);
	if(m_pWinAnimal==NULL)
		return;

	switch(m_STAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			TickEndmode0(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			 TickEndmode1(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			 TickEndmode2(LastTime,FrameTime);  
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			 TickEndmode3(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			 TickEndmode4(LastTime,FrameTime); 
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			 TickEndmode5(LastTime,FrameTime); 
		}
		break;
	}

 	if(m_STAnimalPrize.ePrizeMode!=eAnimalPrizeMode_RepeatTime
		&& m_STAnimalPrize.ePrizeMode!=eAnimalPrizeMode_AllAnimalSingleColr
		&& m_STAnimalPrize.ePrizeMode!=eAnimalPrizeMode_AllColorSingleAnimal)
	{
		if(isSelAniSound)
		{
			int index=3*m_STAnimalPrize.stAnimalInfo.eAnimal+m_STAnimalPrize.stAnimalInfo.eColor;
            GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::R_LION+(int)index));
			isSelAniSound=false;
		}
	}

	if(m_STAnimalPrize.ePrizeMode==eAnimalPrizeMode_RepeatTime)
	{
		if(isSelAniSound)
		{
			if(repeatcount==-100)
			{
				int index=3*m_STAnimalPrize.stAnimalInfo.eAnimal+m_STAnimalPrize.stAnimalInfo.eColor;
				GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::R_LION+(int)index));
				isSelAniSound=false;
			}
			else if(repeatcount>=0 && repeatcount<m_STAnimalPrize.qwFlag)
			{
				int index=3*m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eAnimal+m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eColor;
				GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::R_LION+(int)index));
				isSelAniSound=false;
			}
		}
	}
}

void GameManager::SetSeBanColor( int index, int ClrIndex )
{
	if(index < 0 || index >= ANIMALCOUNT)
		return;

	switch(ClrIndex)
	{
	case eColorType_Red:		// 红
		m_pNodeSeBan2[index]->setMaterialTexture(1, m_pRedTexture);
		break;
	case eColorType_Yellow:		// 黄
		m_pNodeSeBan2[index]->setMaterialTexture(1, m_pYellowTexture);
		break;
	case eColorType_Green:		// 绿
		m_pNodeSeBan2[index]->setMaterialTexture(1,m_pGreenTexture); 
		break;

	default:
		_ASSERT(false);
		break;
	}
}


void GameManager::SetAnimalsAnimation( int ani )
{
	for(int i = 0; i < ANIMALCOUNT; i++)
	{
		m_pAnimal[i]->SetAnimate( ani );
	}
}

void GameManager::SetZhiZhengRot( int StartAngle, int EndAngle, int time )
{
	/*char str[62];
	_snprintf(str,62,"SetZhiZhengRot is %d---%d time =%d",StartAngle,EndAngle,time);
	LOG_FILE(str);*/

	m_RotateTime = time;
	m_RotateTimePassed = 0;
	m_RotateStartAngle = StartAngle;
	m_RotateEndAngle = EndAngle;
}

void GameManager::SetAnimalRot( int StartAngle, int EndAngle, int time )
{
	m_AnimalRotateTime = time;
	m_AnimalRotateTimePassed = 0;
	m_AnimalRotateStartAngle = StartAngle;
	m_AnimalRotateEndAngle = EndAngle;
	bsetsound=true;
	isSelAniSound=false;
}

void GameManager::ChangeGameState( int GameState, int time )
{
	//if(m_GameState == GameState)
	//{
	//	_ASSERT(false);
	//	return;
	//}
	/*char str[62];
	_snprintf(str,62,"ChangeGameState is %d,GameState is %d",time,GameState);
	LOG_FILE(str);*/

	m_GameState = GameState;
	m_GameStateTime = time * 1000;
	m_GameStateRemainTime = time * 1000;
}

void GameManager::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	GameSound::Instance()->Reset();
	
	SetQiangbiAnimation(false);
	SetModeNodesVisible(false);
	CMD_S_GameStart* pGameStart = (CMD_S_GameStart*)pBuffer;
	if(pGameStart == NULL)
		return;	

	/*char str[32];
	_snprintf(str,32,"OnSubGameStart is %d",pGameStart->cbTimeLeave);
	LOG_FILE(str);*/
	ChangeGameState(SUB_S_GAME_START, pGameStart->cbTimeLeave);

	// 设置色板
	m_StartChangeClr = true;

	int RedCount = pGameStart->arrColorRate[eColorType_Red];
	int GreenCount = pGameStart->arrColorRate[eColorType_Green];
	int YellowCount = 24 - GreenCount - RedCount;
	_ASSERT(YellowCount == pGameStart->arrColorRate[eColorType_Yellow]);

	_RPT3(_CRT_WARN, "clr r=%d g=%d y=%d\n", RedCount, GreenCount, YellowCount);

	int SeBanIndex = 0;
	while(1)
	{
		int clr = (rand() * 3 + 1) / RAND_MAX;
		if(clr == 0 && RedCount > 0)
		{	// 红
			m_SeBanClr[SeBanIndex] = eColorType_Red;
			RedCount--;
		}
		else if(clr == 1 && GreenCount > 0)
		{	// 绿
			m_SeBanClr[SeBanIndex] = eColorType_Green;
			GreenCount--;
		}
		else if(clr == 2 && YellowCount > 0)
		{	// 黄
			m_SeBanClr[SeBanIndex] = eColorType_Yellow;
			YellowCount--;
		}
		else
		{
			if(RedCount > 0)
			{
				m_SeBanClr[SeBanIndex] = eColorType_Red;
				RedCount--;
			}
			else if( GreenCount > 0 )
			{
				m_SeBanClr[SeBanIndex] = eColorType_Green;
				GreenCount--;
			}
			else if( YellowCount > 0 )
			{
				m_SeBanClr[SeBanIndex] = eColorType_Yellow;
				YellowCount--;
			}
			else
				_ASSERT(false);
		}

		SeBanIndex++;

		if(RedCount == 0 && GreenCount == 0 && YellowCount == 0 || SeBanIndex >= ANIMALCOUNT)
			break;
	}
	
	// 胜利动物回位
   if(m_pWinAnimal!=NULL)
   {
	m_pWinAnimal->GetAnimateNode()->setPosition( m_OriginalAnimalPos );
	m_pWinAnimal->GetAnimateNode()->setRotation(core::vector3df(0.0f,0.0f, 0.0f));
	m_pNodeDiBan[m_WinIndex]->addChild(m_pWinAnimal->GetAnimateNode());
   }
	// 动物们动起来
	SetAnimalsAnimation(1);
	
	
	// 指针回位
	//SetZhiZhengRot( (int)m_NodeZhiZhengRoot->getRotation().Y, 0, pGameStart->cbTimeLeave*1000/10);

	// 盖子关上
	m_pNodeGaiZi->setFrameLoop(0, 0);
}

void GameManager::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	GameSound::Instance()->Stop(GameSound::WAIT);
	intervalmode=0;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	ChangeGameState(SUB_S_GAME_END, pGameEnd->dwTimeLeave);

	m_STAnimalPrize=pGameEnd->stWinAnimal;
 
	// 开始转了，动物停止动作
	SetAnimalsAnimation(0);

	int ZhiZhengRottime=0;
	int AnimalRottime=0;


	switch(m_STAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			 ZhiZhengRottime=(int)((pGameEnd->dwTimeLeave * 1000)*0.8);
			 AnimalRottime=(int)((pGameEnd->dwTimeLeave * 1000)-500);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{ 
			GameSound::Instance()->Play(GameSound::Pre_1);
			SetQiangbiAnimation(true);
			ZhiZhengRottime=(int)((pGameEnd->dwTimeLeave * 1000)*0.8);
			AnimalRottime=(int)((pGameEnd->dwTimeLeave * 1000));
			StartGaizi();
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			GameSound::Instance()->Play(GameSound::Pre_1);
			SetQiangbiAnimation(true);
			ZhiZhengRottime=(int)((pGameEnd->dwTimeLeave * 1000)*0.8);
			AnimalRottime=(int)((pGameEnd->dwTimeLeave * 1000));
			StartGaizi();
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			GameSound::Instance()->Play(GameSound::Pre_1);
			SetQiangbiAnimation(true);
			ZhiZhengRottime=(int)((pGameEnd->dwTimeLeave * 1000)*0.8);
			AnimalRottime=(int)((pGameEnd->dwTimeLeave * 1000));
			StartGaizi();
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			GameSound::Instance()->Play(GameSound::Pre_1);
			SetQiangbiAnimation(true);
			StartGaizi();
			int t=(int)(pGameEnd->dwTimeLeave*1000-(RepeatTime* pGameEnd->stWinAnimal.qwFlag)); 
			if(t<0) t=0;

			ZhiZhengRottime=(int)((t )*0.8);
			AnimalRottime=(int)((t ));
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			GameSound::Instance()->Play(GameSound::Pre_1);
			SetQiangbiAnimation(true);
			ZhiZhengRottime=(int)((pGameEnd->dwTimeLeave * 1000)*0.8);
			AnimalRottime=(int)((pGameEnd->dwTimeLeave * 1000));
			//StartGaizi();
		}
		break;
	}

	// 指针开始转,转到哪
	int ZhiZhengEndAngle = 0;
	{
		int clrCount = 0;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			if(m_SeBanClr[i] == pGameEnd->stWinAnimal.stAnimalInfo.eColor)
				clrCount++;
		}
		int WhichClr = rand() * clrCount / RAND_MAX;
		int clrIndex = 0;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			if(m_SeBanClr[i] == pGameEnd->stWinAnimal.stAnimalInfo.eColor)
			{
				if(WhichClr <= 0)
				{
					clrIndex = i;
					break;
				}
				WhichClr--;
			}
		}
		ZhiZhengEndAngle = ((clrIndex) * 15 + 180);
		// SetZhiZhengRot( (int)m_NodeZhiZhengRoot->getRotation().Y, (ZhiZhengEndAngle  +(ZhiZhengRottime/2000)*360), ZhiZhengRottime);	

	 	int rot=int(m_NodeZhiZhengRoot->getRotation().Y)%360;
		if(rot<0)
			rot=360+rot;
		m_NodeZhiZhengRoot->setRotation(core::vector3df(0.0f,rot, 0.0f));
		SetZhiZhengRot((360-(int)m_NodeZhiZhengRoot->getRotation().Y), ((360-ZhiZhengEndAngle)  +(ZhiZhengRottime/2000)*360), ZhiZhengRottime); 
	}

	// 动物开始转，转到哪
	{
		int clrCount = 0;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			if(m_pAnimal[i]->GetAnimal() == pGameEnd->stWinAnimal.stAnimalInfo.eAnimal)
				clrCount++;
		}
		int WhichClr = rand() * clrCount / RAND_MAX;
		int clrIndex = 0;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			if(m_pAnimal[i]->GetAnimal() == pGameEnd->stWinAnimal.stAnimalInfo.eAnimal)
			{
				if(WhichClr <= 0)
				{
					clrIndex = i;
					break;
				}
				WhichClr--;
			}
		}
		m_pWinAnimal = m_pAnimal[clrIndex];
		m_WinIndex = clrIndex;
		SetAnimalRot( (int)m_pNodeAnimalRoot->getRotation().Y, (ZhiZhengEndAngle - clrIndex * 15 + 180 +(AnimalRottime/3000)*360),AnimalRottime);	
	}
}

void GameManager::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	GameSound::Instance()->Stop(GameSound::ROTATELOOP);
	if(!m_pWinAnimal)
		return ;

	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	ChangeGameState(SUB_S_GAME_FREE, pGameFree->cbTimeLeave);

	//开奖动画，动物高兴
	SetAnimalsAnimation(0);
	
	switch(m_STAnimalPrize.ePrizeMode)
	{
	case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
		{
			if(m_pWinAnimal)
			{
				m_OriginalAnimalPos = m_pWinAnimal->GetAnimateNode()->getPosition();
				m_OriginalAnimalAbsPos = m_pWinAnimal->GetAnimateNode()->getAbsolutePosition();
				m_OriginalRotation=m_pWinAnimal->GetAnimateNode()->getAbsoluteTransformation().getRotationDegrees();

				scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
				pSceneManager->getRootSceneNode()->addChild( m_pWinAnimal->GetAnimateNode() );
				m_pWinAnimal->GetAnimateNode()->setPosition( m_OriginalAnimalAbsPos );
				m_pWinAnimal->GetAnimateNode()->setRotation(m_OriginalRotation);
			    m_pWinAnimal->GetAnimateNode()->setScale(core::vector3df(m_OriginalScale, m_OriginalScale, m_OriginalScale));
		  }
			StartGaizi();
			SetAnimalsAnimation(2);
			m_pWinAnimal->SetAnimate(0);
			SetQiangbiAnimation(true);
		}
		break;
	case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
		{
			m_pM1PenQi3->setFrameLoop(0, 1000);
			m_pM1PenQi3->setAnimationSpeed(800);
			m_pM1PenQi3->setLoopMode(true);

			m_pM1PenQi1->setMaterialTexture(0, m_M1PenQTexs[m_STAnimalPrize.stAnimalInfo.eColor]);
			m_pM1PenQi3->setMaterialTexture(0, m_M1PenQTexs[m_STAnimalPrize.stAnimalInfo.eColor]);

			GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::COL_R+(int)m_STAnimalPrize.stAnimalInfo.eColor));
		}
		break;
	case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
		{
			 
		}
		break;
	case eAnimalPrizeMode_SysPrize:					// 系统奖分
		{
			SetQiangbiAnimation(true);
			//StartGaizi();
			m_pWinAnimal->SetAnimate(2);
			GameSound::Instance()->Play(GameSound::Prize_Bonus);
		}
		break;
	case eAnimalPrizeMode_RepeatTime:				// 重复开奖
		{
			 
		}
		break;
	case eAnimalPrizeMode_Flash:					// 倍数
		{
			if(m_pWinAnimal)
			{
				m_OriginalAnimalPos = m_pWinAnimal->GetAnimateNode()->getPosition();
				m_OriginalAnimalAbsPos = m_pWinAnimal->GetAnimateNode()->getAbsolutePosition();
				m_OriginalRotation=m_pWinAnimal->GetAnimateNode()->getAbsoluteTransformation().getRotationDegrees();

				scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
				pSceneManager->getRootSceneNode()->addChild( m_pWinAnimal->GetAnimateNode() );
				m_pWinAnimal->GetAnimateNode()->setPosition( m_OriginalAnimalAbsPos );
				m_pWinAnimal->GetAnimateNode()->setRotation(m_OriginalRotation);
			}
			StartGaizi(); 
			SetAnimalsAnimation(2);
			m_pWinAnimal->SetAnimate(0);
		}
		break;
	}
}

void GameManager::OnSubPlaceJetton( const void * pBuffer, WORD wDataSize )
{

}

void GameManager::OnSubPlaceJettonFail( const void * pBuffer, WORD wDataSize )
{

}

void GameManager::OnGSFree(const void * pBuffer, WORD wDataSize)
{
//	CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
//	ChangeGameState(SUB_S_GAME_FREE, pStatusFree->cbTimeLeave);
}
void GameManager::OnGSPlaying(const void * pBuffer, WORD wDataSize)
{
	CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
	ChangeGameState(SUB_S_GAME_START, pStatusPlay->cbTimeLeave);
	/*char str[32];
	_snprintf(str,32,"OnGSPlaying is %d",pStatusPlay->cbTimeLeave);
	LOG_FILE(str);*/
	SetQiangbiAnimation(false);
}
void GameManager::OnGSEnd(const void * pBuffer, WORD wDataSize)
{
	intervalmode=0;
	SetQiangbiAnimation(false);
	CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
	ChangeGameState(SUB_S_GAME_END, pStatusPlay->cbTimeLeave);
    m_STAnimalPrize = pStatusPlay->stWinAnimal;

	// 设置色板
	m_StartChangeClr = true;

 	int RedCount = pStatusPlay->arrColorRate[eColorType_Red];
	int GreenCount = pStatusPlay->arrColorRate[eColorType_Green];
	int YellowCount = 24 - GreenCount - RedCount;
	_ASSERT(YellowCount == pStatusPlay->arrColorRate[eColorType_Yellow]);

	_RPT3(_CRT_WARN, "clr r=%d g=%d y=%d\n", RedCount, GreenCount, YellowCount);

	int SeBanIndex = 0;
	while(1)
	{
		int clr = (rand() * 3 + 1) / RAND_MAX;
		if(clr == 0 && RedCount > 0)
		{	// 红
			m_SeBanClr[SeBanIndex] = eColorType_Red;
			RedCount--;
		}
		else if(clr == 1 && GreenCount > 0)
		{	// 绿
			m_SeBanClr[SeBanIndex] = eColorType_Green;
			GreenCount--;
		}
		else if(clr == 2 && YellowCount > 0)
		{	// 黄
			m_SeBanClr[SeBanIndex] = eColorType_Yellow;
			YellowCount--;
		}
		else
		{
			if(RedCount > 0)
			{
				m_SeBanClr[SeBanIndex] = eColorType_Red;
				RedCount--;
			}
			else if( GreenCount > 0 )
			{
				m_SeBanClr[SeBanIndex] = eColorType_Green;
				GreenCount--;
			}
			else if( YellowCount > 0 )
			{
				m_SeBanClr[SeBanIndex] = eColorType_Yellow;
				YellowCount--;
			}
			else
				_ASSERT(false);
		}

		SeBanIndex++;

		if(RedCount == 0 && GreenCount == 0 && YellowCount == 0 || SeBanIndex >= ANIMALCOUNT)
			break;
	}

	if(m_StartChangeClr)
	{
		m_StartChangeClr = false;
		for(int i = 0; i < ANIMALCOUNT; i++)
		{
			SetSeBanColor(i, m_SeBanClr[i]);
		}
	}

	if(pStatusPlay->cbGameStatus==GS_GAME_END)
	{
		// 开始转了，动物停止动作
		SetAnimalsAnimation(0);

		int ZhiZhengRottime=0;
		int AnimalRottime=0;


		switch(m_STAnimalPrize.ePrizeMode)
		{
		case eAnimalPrizeMode_SingleAnimalSingleColor:	// 单动物单颜色
			{
				ZhiZhengRottime=(int)((pStatusPlay->cbTimeLeave * 1000)*0.8);
				AnimalRottime=(int)((pStatusPlay->cbTimeLeave * 1000)-500);
			}
			break;
		case eAnimalPrizeMode_AllAnimalSingleColr:		// 全动物单颜色
			{
				ZhiZhengRottime=(int)((pStatusPlay->cbTimeLeave * 1000)*0.8);
				AnimalRottime=(int)((pStatusPlay->cbTimeLeave * 1000));
				StartGaizi();
			}
			break;
		case eAnimalPrizeMode_AllColorSingleAnimal:		// 单动物全颜色
			{
				ZhiZhengRottime=(int)((pStatusPlay->cbTimeLeave * 1000)*0.8);
				AnimalRottime=(int)((pStatusPlay->cbTimeLeave * 1000));
				StartGaizi();
			}
			break;
		case eAnimalPrizeMode_SysPrize:					// 系统奖分
			{
				ZhiZhengRottime=(int)((pStatusPlay->cbTimeLeave * 1000)*0.8);
				AnimalRottime=(int)((pStatusPlay->cbTimeLeave * 1000));
				//StartGaizi();
			}
			break;
		case eAnimalPrizeMode_RepeatTime:				// 重复开奖
			{
				StartGaizi();
				int t=(int)(pStatusPlay->cbTimeLeave*1000-(RepeatTime* pStatusPlay->stWinAnimal.qwFlag)); 
				if(t<0) t=pStatusPlay->cbTimeLeave*1000;

				ZhiZhengRottime=(int)((t )*0.8);
				AnimalRottime=(int)((t ));
			}
			break;
		case eAnimalPrizeMode_Flash:					// 倍数
			{
				ZhiZhengRottime=(int)((pStatusPlay->cbTimeLeave * 1000)*0.8);
				AnimalRottime=(int)((pStatusPlay->cbTimeLeave * 1000));
				//StartGaizi();
			}
			break;
		}

		// 指针开始转,转到哪
		int ZhiZhengEndAngle = 0;
		{
			int clrCount = 0;
			for(int i = 0; i < ANIMALCOUNT; i++)
			{
				if(m_SeBanClr[i] == pStatusPlay->stWinAnimal.stAnimalInfo.eColor)
					clrCount++;
			}
			int WhichClr = rand() * clrCount / RAND_MAX;
			int clrIndex = 0;
			for(int i = 0; i < ANIMALCOUNT; i++)
			{
				if(m_SeBanClr[i] == pStatusPlay->stWinAnimal.stAnimalInfo.eColor)
				{
					if(WhichClr <= 0)
					{
						clrIndex = i;
						break;
					}
					WhichClr--;
				}
			}
			ZhiZhengEndAngle = clrIndex * 15 + 180;
			// SetZhiZhengRot( (int)m_NodeZhiZhengRoot->getRotation().Y, (ZhiZhengEndAngle + 360), ZhiZhengRottime);	
		 	int rot=int(m_NodeZhiZhengRoot->getRotation().Y)%360;
			if(rot<0)
				rot=360+rot;
			m_NodeZhiZhengRoot->setRotation(core::vector3df(0.0f,rot, 0.0f));
			SetZhiZhengRot((360-(int)m_NodeZhiZhengRoot->getRotation().Y), ((360-ZhiZhengEndAngle)  +(ZhiZhengRottime/2000)*360), ZhiZhengRottime);
	 	}

		// 动物开始转，转到哪
		{
			int clrCount = 0;
			for(int i = 0; i < ANIMALCOUNT; i++)
			{
				if(m_pAnimal[i]->GetAnimal() == pStatusPlay->stWinAnimal.stAnimalInfo.eAnimal)
					clrCount++;
			}
			int WhichClr = rand() * clrCount / RAND_MAX;
			int clrIndex = 0;
			for(int i = 0; i < ANIMALCOUNT; i++)
			{
				if(m_pAnimal[i]->GetAnimal() == pStatusPlay->stWinAnimal.stAnimalInfo.eAnimal)
				{
					if(WhichClr <= 0)
					{
						clrIndex = i;
						break;
					}
					WhichClr--;
				}
			}
			//SetAnimalRot( (int)m_pNodeAnimalRoot->getRotation().Y, (ZhiZhengEndAngle - clrIndex * 15 + 180 + 360*2),AnimalRottime);	
			m_pWinAnimal = m_pAnimal[clrIndex];
			m_WinIndex = clrIndex;
			SetAnimalRot( (int)m_pNodeAnimalRoot->getRotation().Y, (ZhiZhengEndAngle - clrIndex * 15 + 180 +(AnimalRottime/3000)*360),AnimalRottime);	
		}
	} 

 
}



void GameManager::SetQiangbiAnimation(bool bEnble,int begintex)
{
	enableshansui=bEnble;
	intervaltime=1000;
	qiangbitexnum=begintex;

	intervaltime_1=1000;
	qiangbitexnum_1=0;

	if(!enableshansui)
	{
		m_pNodeqiangbi->setMaterialTexture(0,m_qiangbi3);
		m_pNodeqiangbi_1->setMaterialTexture(0,m_qiangbi_1);
	}
}

void GameManager::QiangbiAnimationUpdate(unsigned int FrameTime)
{
	if(!enableshansui)
		return;
    intervaltime+=FrameTime;
	intervaltime_1+=FrameTime;
    if(intervaltime>800)
	{
		intervaltime=0;
        m_pNodeqiangbi->setMaterialTexture(0, m_qiangbiTexs[qiangbitexnum]);
		qiangbitexnum++;
		qiangbitexnum=qiangbitexnum%5;
	}

	if(intervaltime>100)
	{
		intervaltime_1=0;
	    m_pNodeqiangbi_1->setMaterialTexture(0, m_qiangbi_1Texs[qiangbitexnum]);
		qiangbitexnum_1++;
		qiangbitexnum_1=qiangbitexnum_1%5;
	}
}

void GameManager::StartGaizi()
{
	if(m_pNodeGaiZi)
	{
	// 盖子打开
	m_pNodeGaiZi->setFrameLoop(0, 1000);
	m_pNodeGaiZi->setAnimationSpeed(300);
	m_pNodeGaiZi->setLoopMode(false);
	}
}

void  GameManager::loadmode0()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[0] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang0/xingxing.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pXingXing = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang0/xingxing01.tga");
	m_pXingXing->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	m_pXingXing->setMaterialTexture(0, pTexture1);
	m_pXingXing->setFrameLoop(0, 0);
	//m_pXingXing->setLoopMode(false);
	m_pXingXing->setScale(core::vector3df(0.5f, 0.5f, 0.5f));
	m_pXingXing->setAnimationEndCallback(&g_AnimationEndCallBack);

    m_NodeMode[0]->addChild(m_pXingXing);
	core::vector3df EndPos(0.0f, 60.0f, 0.0f);
	m_NodeMode[0]->setPosition( EndPos );
	m_NodeMode[0]->setVisible(false);
}

void  GameManager::loadmode1()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[1] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);
    m_NodeZhiZhengRoot->addChild(m_NodeMode[1]);

	m = m_pDevice->getSceneManager()->getMesh(DXPATH"Scene/kaijiang1/penqi1.x" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM1PenQi1 = pSceneManager->addAnimatedMeshSceneNode(m);
	 
	m_M1PenQTexs[0]= driver->getTexture(DXPATH"Scene/kaijiang1/kaijiang003.tga");
	m_M1PenQTexs[1]= driver->getTexture(DXPATH"Scene/kaijiang1/kaijiang001.tga");
	m_M1PenQTexs[2]= driver->getTexture(DXPATH"Scene/kaijiang1/kaijiang002.tga");

//	m_pM1PenQi1->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	m_pM1PenQi1->setMaterialTexture(0, m_M1PenQTexs[0]);
//    m_pM1PenQi1->setFrameLoop(0, 1000);
//    m_pM1PenQi1->setAnimationSpeed(100);

	//m = m_pDevice->getSceneManager()->getMesh(DXPATH"Scene/kaijiang1/penqi2.md2" );
	//m->setMaterialFlag(video::EMF_LIGHTING, false);
	//m_pM1PenQi2 = pSceneManager->addAnimatedMeshSceneNode(m);
	//pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang1/kaijiang003.tga");
	//m_pM1PenQi2->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	//m_pM1PenQi2->setMaterialTexture(0, pTexture1);
	//m_pM1PenQi2->setFrameLoop(0, 1000);
	//m_pM1PenQi2->setAnimationSpeed(100);

	m = m_pDevice->getSceneManager()->getMesh(DXPATH"Scene/kaijiang1/penqi3.md2" );
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM1PenQi3 = pSceneManager->addAnimatedMeshSceneNode(m);
	//pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang1/kaijiang003.tga");
	m_pM1PenQi3->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	m_pM1PenQi3->setMaterialTexture(0, m_M1PenQTexs[0]);
	m_pM1PenQi3->setFrameLoop(0, 0);
	m_pM1PenQi3->setLoopMode(false);
	//m_pM1PenQi3->setAnimationSpeed(800);

    m_NodeMode[1]->addChild(m_pM1PenQi1);
	//m_NodeMode[1]->addChild(m_pM1PenQi2);
	m_NodeMode[1]->addChild(m_pM1PenQi3);

	m_NodeMode[1]->setScale(core::vector3df(0.12f, 0.12f, 0.12f));
	core::vector3df EndPos(0.0f, 80.0f, 0.0f);
	

	m_NodeMode[1]->setPosition( EndPos );
 	m_NodeMode[1]->setVisible(false);
}

void  GameManager::loadmode2()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[2] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang2/quanquan.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM2Quan = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang2/guanghuan01.tga");
	m_pM2Quan->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	m_pM2Quan->setMaterialTexture(0, pTexture1);
	m_pM2Quan->setFrameLoop(0, 1200);
 	m_pM2Quan->setAnimationSpeed(200);
	m_pM2Quan->setScale(core::vector3df(0.16f, 0.16f, 0.16f));
 	m_pM2Quan->setPosition(core::vector3df(0,10,0));

    m_pM2Animal = m_pDevice->getSceneManager()->addAnimatedMeshSceneNode(m_pPanda);
	m_pM2Animal->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM2Animal->setAnimationSpeed(30);
	m_pM2Animal->setCurrentFrame((float)rand() * 100 / RAND_MAX);
	m_pM2Animal->setScale(core::vector3df(0.2f, 0.2f, 0.2f));
	m_pM2Animal->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	
	m_NodeMode[2]->addChild(m_pM2Quan);
	m_NodeMode[2]->addChild(m_pM2Animal);
	core::vector3df EndPos(0.0f, 80.0f, 0.0f);
	m_NodeMode[2]->setPosition( EndPos );
	m_NodeMode[2]->setVisible(false);
}

void  GameManager::loadmode3()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[3] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang3/caijin.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM3Caijin = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang3/caijin01.tga");
	m_pM3Caijin->setMaterialTexture(0, pTexture1);
	m_pM3Caijin->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	m_pM3Caijin->setFrameLoop(0, 1200);
	m_pM3Caijin->setAnimationSpeed(50);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang3/caijin2.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM3Caijin1 = pSceneManager->addAnimatedMeshSceneNode(m);
	pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang3/caijin03.tga");
	m_pM3Caijin1->setMaterialTexture(0, pTexture1);
	m_pM3Caijin1->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	m_pM3Caijin1->setFrameLoop(0, 600);
	m_pM3Caijin1->setAnimationSpeed(50);

	m_NodeMode[3]->addChild(m_pM3Caijin);
	m_NodeMode[3]->addChild(m_pM3Caijin1);

	m_NodeMode[3]->setScale(core::vector3df(0.2f, 0.2f, 0.2f));
	core::vector3df EndPos(0.0f, 100.0f, 0.0f);
	m_NodeMode[3]->setPosition( EndPos );
	m_NodeMode[3]->setVisible(false);
}

void  GameManager::loadmode4()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[4] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang4/dianshi.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM4Dianshi = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang4/dianshiji01.tga");
	m_pM4Dianshi->setMaterialTexture(0, pTexture1);
	//m_pM4Dianshi->setMaterialType(video::EMT_SOLID);
	m_pM4Dianshi->setFrameLoop(0, 1000);
 	m_pM4Dianshi->setAnimationSpeed(50);

	m_M4shuZiTex[0]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_1.tga");
	m_M4shuZiTex[1]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_2.tga");
	m_M4shuZiTex[2]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_3.tga");
	m_M4shuZiTex[3]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_4.tga");
	m_M4shuZiTex[4]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_5.tga");
	m_M4shuZiTex[5]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_6.tga");
	m_M4shuZiTex[6]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_7.tga");
	m_M4shuZiTex[7]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_8.tga");
    m_M4shuZiTex[8]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_9.tga");
	m_M4shuZiTex[9]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_10.tga");
	m_M4shuZiTex[10]= driver->getTexture(DXPATH"Scene/kaijiang4/m4_11.tga");

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang4/shuzi.md2");		
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM4shuZi = pSceneManager->addAnimatedMeshSceneNode(m);
	m_pM4shuZi->setMaterialTexture(0, m_M4shuZiTex[0]);
	m_pM4shuZi->setMaterialType(video::EMT_SOLID); 
    m_pM4shuZi->setFrameLoop(0, 1000);
	m_pM4shuZi->setAnimationSpeed(50); 

	m_NodeMode[4]->addChild(m_pM4Dianshi);
	m_NodeMode[4]->addChild(m_pM4shuZi);
	m_NodeMode[4]->setScale(core::vector3df(0.2f, 0.2f, 0.2f));
	core::vector3df EndPos(0.0f, 100.0f, 0.0f);
	m_NodeMode[4]->setPosition( EndPos );
 	m_NodeMode[4]->setVisible(false);
}

void  GameManager::loadmode5()
{
	video::IVideoDriver* driver = m_pDevice->getVideoDriver();
	scene::ISceneManager* pSceneManager = m_pDevice->getSceneManager();
	scene::IAnimatedMesh* m = NULL;
	m_NodeMode[5] = pSceneManager->addMeshSceneNode(NULL, 0, -1, core::vector3df(0,0,0), core::vector3df(0,0,0), core::vector3df(1,1,1), true);

	m = pSceneManager->getMesh(DXPATH"Scene/kaijiang5/sandian.md2");				 
	m->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pM5Sandian = pSceneManager->addAnimatedMeshSceneNode(m);
	video::ITexture* pTexture1 = driver->getTexture(DXPATH"Scene/kaijiang5/shandian001.tga");
	m_pM5Sandian->setMaterialTexture(0, pTexture1);
	m_pM5Sandian->setFrameLoop(0, 120);
	//m_pM5Sandian->setAnimationSpeed(50);
	m_pM5Sandian->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	m_NodeMode[5]->addChild(m_pM5Sandian);
	m_NodeMode[5]->setScale(core::vector3df(0.7f, 0.7f, 0.7f));
	core::vector3df EndPos(0.0f, 60.0f, 0.0f);
	m_NodeMode[5]->setPosition( EndPos );
	m_NodeMode[5]->setVisible(false);
}

void GameManager::TickFreemode0(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode0);
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode0);
	}
	// 动物上台
	int PassTime = m_GameStateTime - m_GameStateRemainTime;
	if( PassTime < m_GameStateTime * 0.3f && m_pWinAnimal )
	{
		float UpTo = 20.0f;
		if( PassTime < m_GameStateTime * 0.1f )
		{
			// 上升
			core::vector3df pos = m_OriginalAnimalAbsPos;
			pos.Y += (float)PassTime / (m_GameStateTime * 0.2f) * UpTo;
			m_pWinAnimal->GetAnimateNode()->setPosition( pos );
		}
		else if( PassTime < m_GameStateTime * 0.3f )
		{	// 平移到盖子上
			core::vector3df UpPos = m_OriginalAnimalAbsPos;
			UpPos.Y += UpTo;

			core::vector3df EndPos(0.0f, 70.0f, 0.0f);

			core::vector3df pos = (EndPos - UpPos ) * 
				(PassTime - m_GameStateTime * 0.1f) / (m_GameStateTime * 0.2f) + UpPos;

			m_pWinAnimal->GetAnimateNode()->setPosition( pos );
		 
			float rot=(180+m_OriginalRotation.Y)*(PassTime - m_GameStateTime * 0.1f) / (m_GameStateTime * 0.2f);
			m_pWinAnimal->GetAnimateNode()->setRotation(core::vector3df(0.0f,rot, 0.0f));
		}
	}
	else
	{
		if(!m_NodeMode[0]->isVisible())
		{
			m_NodeMode[0]->setVisible(true);
			m_pXingXing->setCurrentFrame(0);
			m_pXingXing->setFrameLoop(0,1000);
			//m_pXingXing->setLoopMode(false);
			m_pXingXing->setAnimationSpeed(150);
			m_pWinAnimal->SetAnimate(2);

		}
		core::vector3df EndPos(0.0f, 70.0f, 0.0f);
		if(m_pWinAnimal)
		{
			m_pWinAnimal->GetAnimateNode()->setPosition( EndPos );
		    m_pWinAnimal->GetAnimateNode()->setRotation(core::vector3df(0.0f,0.0f, 0.0f));
		}
	}
}

void GameManager::TickFreemode1(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode1);
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode1);
	}
}

void GameManager::TickFreemode2(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode2);
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode2);
	}
}

void GameManager::TickFreemode3(unsigned int LastTime, unsigned int FrameTime)
{
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode3);
	}
}

void GameManager::TickFreemode4(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode4);
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode4);
	}
}

void GameManager::TickFreemode5(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode5);
	if(m_GameStateRemainTime<500)
	{
		GameSound::Instance()->Stop(GameSound::Mode5);
	}
	// 动物上台
	int PassTime = m_GameStateTime - m_GameStateRemainTime;
	if( PassTime < m_GameStateTime * 0.3f && m_pWinAnimal )
	{
		float UpTo = 20.0f;
		if( PassTime < m_GameStateTime * 0.1f )
		{
			// 上升
			core::vector3df pos = m_OriginalAnimalAbsPos;
			pos.Y += (float)PassTime / (m_GameStateTime * 0.2f) * UpTo;
			m_pWinAnimal->GetAnimateNode()->setPosition( pos );
		}
		else if( PassTime < m_GameStateTime * 0.3f )
		{	// 平移到盖子上
			core::vector3df UpPos = m_OriginalAnimalAbsPos;
			UpPos.Y += UpTo;

			core::vector3df EndPos(0.0f, 70.0f, 0.0f);

			core::vector3df pos = (EndPos - UpPos ) * 
				(PassTime - m_GameStateTime * 0.1f) / (m_GameStateTime * 0.2f) + UpPos;

			m_pWinAnimal->GetAnimateNode()->setPosition( pos );

			float rot=(180+m_OriginalRotation.Y)*(PassTime - m_GameStateTime * 0.1f) / (m_GameStateTime * 0.2f);
			m_pWinAnimal->GetAnimateNode()->setRotation(core::vector3df(0.0f,rot, 0.0f));
		}
	}
	else
	{
		if(m_pWinAnimal->GetAnimate()!=2)
		{
			m_pWinAnimal->SetAnimate(2);
		}
		core::vector3df EndPos(0.0f, 70.0f, 0.0f);
		if(m_pWinAnimal)
		{
			m_pWinAnimal->GetAnimateNode()->setPosition( EndPos );
			m_pWinAnimal->GetAnimateNode()->setRotation(core::vector3df(0.0f,0.0f, 0.0f));
		}
	}
}

//--------------------------------------------------------------------------------------------

void  GameManager::TickEndmode0(unsigned int LastTime, unsigned int FrameTime)
{
	// 被选中的动物膨胀
	if( m_GameStateRemainTime <500 && m_pWinAnimal)
	{
        float scale = ( (float)(m_GameStateRemainTime % 300) / 300 * 0.2f + 1.0f ) * m_OriginalScale;
		m_pWinAnimal->GetAnimateNode()->setScale(core::vector3df(scale, scale, scale));
	}
}

void  GameManager::TickEndmode1(unsigned int LastTime, unsigned int FrameTime)
{
	int PassTime = m_GameStateTime - m_GameStateRemainTime;

	if(PassTime>1000 )
	{
		if(!m_NodeMode[1]->isVisible())
		{
			m_NodeMode[1]->setVisible(true);
			mode1texindex=0;
			m_pM1PenQi3->setFrameLoop(0, 0);
			m_pM1PenQi3->setLoopMode(false);
		}
		if(PassTime<2000)
		{
			float  ff=(PassTime-1000.0f)/1000.0f;
			m_NodeMode[1]->setScale(core::vector3df(ff*0.12f, ff*0.12f, ff*0.12f));
		}
		else
		{
			intervalmode+= FrameTime;
			if(intervalmode>200)
			{
				mode1texindex++;
				mode1texindex=mode1texindex%3;

				intervalmode=0;
				 
				m_pM1PenQi1->setMaterialTexture(0, m_M1PenQTexs[mode1texindex]);
				m_pM1PenQi3->setMaterialTexture(0, m_M1PenQTexs[mode1texindex]);
			}
		}
		
	}
}

void  GameManager::TickEndmode2(unsigned int LastTime, unsigned int FrameTime)
{
    int PassTime = m_GameStateTime - m_GameStateRemainTime;
	int nn=int(0.95*m_GameStateTime/500) ;
	 
	if(PassTime>1000 && PassTime<nn*500 )
	{
		if(!m_NodeMode[2]->isVisible())
		{
			m_NodeMode[2]->setVisible(true);
			m_pM2Quan->setFrameLoop(0, 1200);
			m_pM2Quan->setAnimationSpeed(200);
		}
		intervalmode+= FrameTime;
		if(intervalmode>500)
		{
			intervalmode=0;
			int randani;
			if(PassTime>(nn-1)*500)
			{
				 randani= m_STAnimalPrize.stAnimalInfo.eAnimal;
				 GameSound::Instance()->Play(GameSound::SoundIndex(GameSound::Lion+(int)randani));
			}
			else
			{
				 randani=(int)(rand() * 3.9f / RAND_MAX) ;
			}

			switch(randani)
			{
			case 0:
				{
					m_pM2Animal->setMesh(m_pLion);
						m_pM2Animal->setFrameLoop(0, 0);
				}break;
			case 1:
				{
					m_pM2Animal->setMesh(m_pPanda);
						m_pM2Animal->setFrameLoop(34, 34);
				}break;
			case 2:
				{
					m_pM2Animal->setMesh(m_pMonkey);
						m_pM2Animal->setFrameLoop(70, 70);
				}break;
			case 3:
				{
					m_pM2Animal->setMesh(m_pRabbit);
						m_pM2Animal->setFrameLoop(50, 50);
				}break;

			}
			m_pM2Animal->setMaterialFlag(video::EMF_LIGHTING, false);
			m_pM2Animal->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
		}

		rotationnum = (float)(360*intervalmode)/500;
		m_pM2Animal->setRotation(core::vector3df(0.0f,rotationnum, 0.0f));
	}
	else
	{
		 m_pM2Animal->setRotation(core::vector3df(0.0f,0.0f, 0.0f));
	}
}

void  GameManager::TickEndmode3(unsigned int LastTime, unsigned int FrameTime)
{
	GameSound::Instance()->Play(GameSound::Mode3);
	int PassTime = m_GameStateTime - m_GameStateRemainTime;
	 
    if(PassTime>1100 )
	{
		if(!m_NodeMode[3]->isVisible())
		{
			m_NodeMode[3]->setVisible(true);
			m_pM3Caijin->setFrameLoop(0, 1200);
			m_pM3Caijin->setAnimationSpeed(50);
			m_pM3Caijin->setLoopMode(true);
			m_pM3Caijin1->setFrameLoop(0, 600);
			m_pM3Caijin1->setAnimationSpeed(50);
			m_pM3Caijin1->setLoopMode(true);
		}
	}
}

void  GameManager::TickEndmode4(unsigned int LastTime, unsigned int FrameTime)
{
	int PassTime = m_GameStateTime - m_GameStateRemainTime;
    ASSERT(PassTime>=0);
	ASSERT(m_STAnimalPrize.qwFlag>=0);

    if(PassTime>1000)
	{
		if(!m_NodeMode[4]->isVisible())
		{
			m_NodeMode[4]->setVisible(true);
			m_pM4Dianshi->setFrameLoop(0, 1000);
			m_pM4Dianshi->setAnimationSpeed(50);

			m_pM4shuZi->setFrameLoop(0, 1000);
			m_pM4shuZi->setAnimationSpeed(50);

			repeatcount=-100;
			GameSound::Instance()->Play(GameSound::Prize_Light);
		}
	}

	ASSERT(m_STAnimalPrize.qwFlag<=11);
	 if(PassTime>1000 && PassTime<(m_GameStateTime-RepeatTime*m_STAnimalPrize.qwFlag))
	{
		intervalmode+= FrameTime;
		if(intervalmode>500)
		{
			intervalmode=0;
		    int  randani=(int)(rand() * 10 / RAND_MAX) ;
			m_pM4shuZi->setMaterialTexture(0,m_M4shuZiTex[randani]);
		}
	}

	 if(PassTime>(m_GameStateTime-RepeatTime*m_STAnimalPrize.qwFlag))
	{
		if(repeatcount==-100)
		{
			repeatcount=m_STAnimalPrize.qwFlag;
			SetTickEndmode4();
		}

		intervalmode+= FrameTime;
		if(intervalmode>RepeatTime)
		{
			SetTickEndmode4();
			intervalmode=0;
		}

	}
}

void  GameManager::TickEndmode5(unsigned int LastTime, unsigned int FrameTime)
{
	int PassTime = m_GameStateTime - m_GameStateRemainTime;

    if(!m_NodeMode[5]->isVisible())
	{
			m_NodeMode[5]->setVisible(true);
			GameSound::Instance()->Play(GameSound::Lightning);
			GameSound::Instance()->Play(GameSound::Prize_Double);
    }

	if(PassTime>4000)
	{
		if(m_NodeMode[5]->isVisible())
		{
			m_NodeMode[5]->setVisible(false);
		}
	}
	 
}

void GameManager::SetModeNodesVisible(bool vis)
{
	for(int i=0;i<6;i++)
	{
		if(m_NodeMode[i])
		{ 
			m_NodeMode[i]->setVisible(vis);
		}
	}
}

void GameManager::SetTickEndmode4()
{
	 repeatcount--; //图比repeatcount大一；
   if(repeatcount<0)
	   return;

   m_pM4shuZi->setMaterialTexture(0,m_M4shuZiTex[ repeatcount]);

   // 指针开始转,转到哪
   int ZhiZhengEndAngle = 0;
   {
	   int clrCount = 0;
	   for(int i = 0; i < ANIMALCOUNT; i++)
	   {
		   if(m_SeBanClr[i] == m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eColor)
			   clrCount++;
	   }
	   int WhichClr = rand() * clrCount / RAND_MAX;
	   int clrIndex = 0;
	   for(int i = 0; i < ANIMALCOUNT; i++)
	   {
		   if(m_SeBanClr[i] ==m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eColor)
		   {
			   if(WhichClr <= 0)
			   {
				   clrIndex = i;
				   break;
			   }
			   WhichClr--;
		   }
	   }
	   ZhiZhengEndAngle = clrIndex * 15 + 180;
	  //  SetZhiZhengRot( (int)m_NodeZhiZhengRoot->getRotation().Y, (ZhiZhengEndAngle + 360), RepeatTime*0.8f);	
	   int rot=int(m_NodeZhiZhengRoot->getRotation().Y)%360;
	   if(rot<0)
		   rot=360+rot;
	   m_NodeZhiZhengRoot->setRotation(core::vector3df(0.0f,rot, 0.0f));
	   SetZhiZhengRot((360-(int)m_NodeZhiZhengRoot->getRotation().Y), ((360-ZhiZhengEndAngle)  + 2*360), RepeatTime*0.8f); 
   }

   // 动物开始转，转到哪
   {
	   int clrCount = 0;
	   for(int i = 0; i < ANIMALCOUNT; i++)
	   {
		   if(m_pAnimal[i]->GetAnimal() == m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eAnimal)
			   clrCount++;
	   }
	   int WhichClr = rand() * clrCount / RAND_MAX;
	   int clrIndex = 0;
	   for(int i = 0; i < ANIMALCOUNT; i++)
	   {
		   if(m_pAnimal[i]->GetAnimal() == m_STAnimalPrize.arrstRepeatModePrize[repeatcount].eAnimal)
		   {
			   if(WhichClr <= 0)
			   {
				   clrIndex = i;
				   break;
			   }
			   WhichClr--;
		   }
	   }
	   m_pWinAnimal = m_pAnimal[clrIndex];
	   m_WinIndex = clrIndex;
	   SetAnimalRot( (int)m_pNodeAnimalRoot->getRotation().Y, (ZhiZhengEndAngle - clrIndex * 15 + 180 + 360*2),RepeatTime*0.85f);	
   }

}

