#include "Stdafx.h"
#include "Animal.h"

using namespace irr;

Animal::Animal() :
m_pAnimateNode(NULL),
m_pMonkey(NULL),
m_pPanda(NULL),
m_pLion(NULL),
m_pRabbit(NULL)
{
	m_CurrentAnimal = 0;
	m_CurAnimate=0;
}

Animal::~Animal()
{

}

void Animal::SetAnimateNode( irr::scene::IAnimatedMeshSceneNode* pAnimateNode )
{
	m_pAnimateNode = pAnimateNode;
}

void Animal::SetAnimal(int which)
{
	m_CurrentAnimal = which;
	_ASSERT(m_pAnimateNode);
	switch(which)
	{
	case eAnimalType_Lion:	// Ê¨×Ó
		m_pAnimateNode->setMesh(m_pLion);
		break;
	case eAnimalType_Panda: // ÐÜÃ¨
		m_pAnimateNode->setMesh(m_pPanda);
		break;
	case eAnimalType_Monkey: // ºï×Ó
		m_pAnimateNode->setMesh(m_pMonkey);
		break;
	case eAnimalType_Rabbit: // ÍÃ×Ó
		m_pAnimateNode->setMesh(m_pRabbit);
		break;
	default:
		_ASSERT(false);
		break;
	}

	m_pAnimateNode->setMaterialFlag(video::EMF_LIGHTING, false);
	m_pAnimateNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
}

void Animal::SetAnimate( int ani )
{
	if(ani==1)
	{
		m_pAnimateNode->setAnimationSpeed(15);
	}
	else
	{
		m_pAnimateNode->setAnimationSpeed(30);
	}

	m_CurAnimate=ani;
	switch(m_CurrentAnimal)
	{
	case eAnimalType_Lion: // Ê¨×Ó
		{
			switch(ani)
			{
			case 0:
				m_pAnimateNode->setFrameLoop(0, 0);
				break;
			case 1: // ´ý»ú
				m_pAnimateNode->setFrameLoop(0, 34);
				m_pAnimateNode->setCurrentFrame((float)rand() * 34 / RAND_MAX);
				break;
			case 2: // Çì×£
				m_pAnimateNode->setFrameLoop(0, 34);
				m_pAnimateNode->setCurrentFrame((float)rand() * 34 / RAND_MAX);
				break;
			}
		}
		break;
	case eAnimalType_Panda:	// ÐÜÃ¨
		{
			switch(ani)
			{
			case 0:
				m_pAnimateNode->setFrameLoop(34, 34);
				break;
			case 1: // ´ý»ú
				m_pAnimateNode->setFrameLoop(34, 67);
				m_pAnimateNode->setCurrentFrame((float)rand() * (67-34) / RAND_MAX + 34);
				break;
			case 2: // Çì×£
				m_pAnimateNode->setFrameLoop(0, 40);
				m_pAnimateNode->setCurrentFrame((float)rand() * 34 / RAND_MAX);
				break;
			}
		}
		break;
	case eAnimalType_Monkey:	// ºï×Ó
		{
			switch(ani)
			{
			case 0:
				m_pAnimateNode->setFrameLoop(70, 70);
				break;
			case 1: // ´ý»ú
				m_pAnimateNode->setFrameLoop(70, 113);
				m_pAnimateNode->setCurrentFrame((float)rand() * (113-70) / RAND_MAX + 70);
				break;
			case 2:	// Çì×£
				m_pAnimateNode->setFrameLoop(0, 66);
				m_pAnimateNode->setCurrentFrame((float)rand() * (66) / RAND_MAX);
				break;
			}
		}
		break;
	case eAnimalType_Rabbit: // ÍÃ×Ó
		{
			switch(ani)
			{
			case 0:
				m_pAnimateNode->setFrameLoop(50, 50);
				break;
			case 1: // ´ý»ú
				m_pAnimateNode->setFrameLoop(0, 60);
				m_pAnimateNode->setCurrentFrame((float)rand() * (60-40) / RAND_MAX + 40);
				break;
			case 2: // Çì×£
				m_pAnimateNode->setFrameLoop(0, 50);
				m_pAnimateNode->setCurrentFrame((float)rand() * (34) / RAND_MAX);
				break;
			}
		}
		break;
	}
}