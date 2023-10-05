/*
 *	动物对象
 */
#ifndef Animal_h__
#define Animal_h__

class Animal
{
public:
	Animal();
	~Animal();

	irr::scene::IAnimatedMeshSceneNode*	GetAnimateNode() {return m_pAnimateNode;}
	void SetAnimateNode(irr::scene::IAnimatedMeshSceneNode* pAnimateNode);

	void SetMonkey(irr::scene::IAnimatedMesh*	pMonkey) {m_pMonkey = pMonkey;}
	void SetPanda(irr::scene::IAnimatedMesh*	pPanda) {m_pPanda = pPanda;}
	void SetLion(irr::scene::IAnimatedMesh*	pLion) {m_pLion = pLion;}
	void SetRabbit(irr::scene::IAnimatedMesh*	pRabbit) {m_pRabbit = pRabbit;}

	void SetAnimal(int which);		// 1 狮子 2 熊猫 3 猴子 4 兔子
	int	 GetAnimal() {return m_CurrentAnimal;}
	void SetAnimate(int ani);		// 0 无动作1 待机 2 庆祝
    int	 GetAnimate() {return m_CurAnimate;}
private:
	irr::scene::IAnimatedMeshSceneNode* m_pAnimateNode;
	irr::scene::IAnimatedMesh*	m_pMonkey;
	irr::scene::IAnimatedMesh*	m_pPanda;
	irr::scene::IAnimatedMesh*	m_pLion;
	irr::scene::IAnimatedMesh*	m_pRabbit;
	int		m_CurrentAnimal;
	int     m_CurAnimate;

};


#endif // Animal_h__
