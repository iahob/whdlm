#include "PageTurn.h"
#include <math.h>
#include "2d/CCGrid.h"
#include "2d/CCNodeGrid.h"
#include "base/CCDirector.h"

#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#endif

USING_NS_CC;
static lua_State *GLL = NULL;
PageTurn* PageTurn::create(const char *cardBgPath, const char *cardVluePath, Vec2 cardPoint)
{
	PageTurn *ret = new (std::nothrow) PageTurn();
	if (ret && ret->init(cardBgPath, cardVluePath, cardPoint))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}


bool PageTurn::init(const char *cardBgPath, const char *cardVluePath, Vec2 cardPoint)
{

	if (!Layer::init())
	{
		return false;
	}

	//m_callBack = callback;

	Size winSize = Director::getInstance()->getWinSize();

	m_cCardBgPath = cardBgPath;
	m_cCardValuePath = cardVluePath;
	m_vCardPoint = cardPoint;

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	//this->setSwallowsTouches(true);
	layout = ui::Layout::create();
	layout->setContentSize(Size(winSize.width, winSize.height));
	//layout->setBackGroundColorOpacity(100);
	layout->setPosition(Vec2(0, 0));
	layout->setAnchorPoint(Vec2(0, 0));
	layout->setTag(12);

	auto sprite3 = Sprite::create(String::createWithFormat("%s%s",m_cCardValuePath,"_e.png")->getCString());
	//sprite->setRotation(90);
	sprite3->setAnchorPoint(Vec2(0.5, 0.5));
	m_sCardSize = sprite3->getContentSize();
	sprite3->setPosition(m_vCardPoint);//旋转，宽高互换
	sprite3->setRotation(90);
	sprite3->setScaleY(-1);
	//sprite3->setTag(100);
	//this->addChild(sprite3, 0);



	_gridCardValue = NodeGrid::create(Rect(m_vCardPoint.x - m_sCardSize.height / 2, m_vCardPoint.y - m_sCardSize.width / 2, m_sCardSize.height, m_sCardSize.width));
	//_gridNodeTarget->setRotation(90);
	_gridCardValue->setAnchorPoint(Vec2(0, 0));
	// position the sprite on the center of the screen
	_gridCardValue->setPosition(Vec2(0, 0));

	_gridCardValue->addChild(sprite3, 0);


	auto sprite4 = Sprite::create(m_cCardBgPath);
	//sprite4->setRotation(90);
	sprite4->setAnchorPoint(Vec2(0.5, 0.5));
	sprite4->setPosition(m_vCardPoint);
	sprite4->setRotation(90);
	sprite4->setTag(100);

	_gridCardBg = NodeGrid::create(Rect(m_vCardPoint.x - m_sCardSize.height / 2, m_vCardPoint.y - m_sCardSize.width / 2, m_sCardSize.height, m_sCardSize.width));
	//_gridNodeTarget->setRotation(90);
	_gridCardBg->setAnchorPoint(Vec2(0, 0));
	// position the sprite on the center of the screen
	_gridCardBg->setPosition(Vec2(0, 0));
	_gridCardBg->addChild(sprite4, 0);
	//this->addChild(sprite4, 0);



	this->addChild(_gridCardBg, 2, 2);


	m_sGridSize = Size(32, 46);
	m_pBeganPoint = Vec2(0, 0);

	startWithTarget(_gridCardValue);
	startWithTarget(_gridCardBg);


	layout->addChild(_gridCardValue);
	layout->setClippingEnabled(true);
	layout->setVisible(false);

	this->addChild(layout, 1, 1);



	mHOffsetY = 0;
	m_bTurnEnd = false;
	return true;
}

void PageTurn::showCardValue()
{
	
	auto card = Sprite::create(String::createWithFormat("%s%s", m_cCardValuePath, ".png")->getCString());
	card->setAnchorPoint(Vec2(0.5, 0.5));
	card->setRotation(90);
	card->setPosition(m_vCardPoint);
	this->addChild(card, 100, 100);
	layout->setVisible(false);
	_gridCardBg->setVisible(false);
	_gridCardValue->setVisible(false);
}

void PageTurn::calculateVerticalVertexPoints(float offsetY, NodeGrid *pTarget)
{
    float R2 = 25;
	float offsetH = 0;
	for (int i = 0; i <= m_sGridSize.width; ++i)
	{
		for(int j = 0; j <= m_sGridSize.height; ++j)
		{
			Vec3 p = originalVertex(ccp(i,j),pTarget);
			float l=offsetY - p.y;
			float alpha =l/R2;
			if (l >= 0) {

				if (alpha >= M_PI){
					p.z=(GLfloat)(2 * R2);
					p.y=(GLfloat)(offsetY + R2 * (alpha - M_PI));
				}
				else if (alpha < M_PI)
				{
					p.z =(GLfloat)((R2 - R2 * cosf(alpha)));
					p.y =(GLfloat)(offsetY - R2 * sinf(alpha));
				}
			}
			setVertex(ccp(i, j), p, pTarget);
		}
	}
	layout->setVisible(false);
	layout->setAnchorPoint(Vec2(0, 0));
	layout->setPosition(Vec2(0, 0));
	_gridCardValue->setPosition(Vec2(0, 0));

	Size winSize = Director::getInstance()->getWinSize();
	float distance = winSize.height / 2;

	if (offsetY <= M_PI*R2 && offsetY > 0.5*M_PI*R2)
	{

		offsetH = (GLfloat)(offsetY - R2 * sinf(offsetY / R2));

	}
	else if (offsetY > M_PI*R2)
	{
		offsetH = (GLfloat)(2 * offsetY - M_PI*R2);
	}

	if (offsetH != 0 && offsetY > 70)
	{
		//计算视角偏移量
		offsetH = distance - (distance / 2) * M_PI*(distance - (offsetH - m_vCardPoint.y + m_sCardSize.width / 2)) / ((distance / 2)* M_PI - 2 * R2);
		layout->setVisible(true);
		layout->setContentSize(Size(winSize.width, offsetH));
	}
	//CCLOG("ppppppppppppppppp,%f,%f", offsetH, offsetY);
}

void PageTurn::calculateVerticalVertexPointXR(float offsetX, NodeGrid *pTarget)
{
    float R2 = 25;
	float offsetW = 0;
	for (int i = 0; i <= m_sGridSize.width; ++i)
	{
		for(int j = 0; j <= m_sGridSize.height; ++j)
		{
			Vec3 p = originalVertex(ccp(i,j),pTarget);
			float l= p.x - offsetX;
			float alpha =l/R2;
			if (l >= 0) {

				if (alpha >= M_PI){
					p.z=(GLfloat)(2 * R2);
					p.x=(GLfloat)(offsetX - R2 * (alpha - M_PI));
				}
				else if (alpha < M_PI)
				{
					p.z =(GLfloat)((R2 - R2 * cosf(alpha)));
					p.x =(GLfloat)(offsetX + R2 * sinf(alpha));
				}
			}
			setVertex(ccp(i, j), p, pTarget);
		}
	}
	Size winSize = Director::getInstance()->getWinSize();
	float distance = winSize.width / 2;
	float offLen = m_vCardPoint.x + m_sCardSize.height/2 - offsetX;

	layout->setVisible(false);
	layout->setAnchorPoint(Vec2(1, 0));
	layout->setPosition(Vec2(winSize.width, 0));
	_gridCardValue->setPosition(Vec2(0, 0));

	offsetX = 2*m_vCardPoint.x - offsetX;
	if (offsetX <= M_PI*R2 && offsetX > 0.5*M_PI*R2)
	{

		offsetW = (GLfloat)(offsetX - R2 * sinf(offsetX / R2));

	}
	else if (offsetX > M_PI*R2)
	{
		offsetW = (GLfloat)(2 * offsetX - M_PI*R2);
	}

	if (offsetW != 0 && offsetX > 50)
	{
		//计算视角偏移量
		offsetW = distance - (distance / 2) * M_PI*(distance - (offsetW - m_vCardPoint.x + m_sCardSize.height / 2)) / ((distance / 2)* M_PI - 2 * R2);
		layout->setVisible(true);
		offsetW = offsetW - 5;
		layout->setContentSize(Size(offsetW , winSize.height));
		_gridCardValue->setPositionX( offsetW - winSize.width);
		// _gridCardValue->setPositionX(m_vCardPoint.x -(winSize.width - offsetW) );
	}
	// CCLOG("ppppppppppppppppp,%f,%f,%f", offsetW, offsetX, (winSize.width - offsetW));
}
void PageTurn::calculateVerticalVertexPointXs(float offsetX, NodeGrid *pTarget)
{
    float R2 = 25;
	float offsetW = 0;
	for (int i = 0; i <= m_sGridSize.width; ++i)
	{
		for(int j = 0; j <= m_sGridSize.height; ++j)
		{
			Vec3 p = originalVertex(ccp(i,j),pTarget);
			float l=offsetX - p.x;
			float alpha =l/R2;
			if (l >= 0) {

				if (alpha >= M_PI){
					p.z=(GLfloat)(2 * R2);
					p.x=(GLfloat)(offsetX + R2 * (alpha - M_PI));
				}
				else if (alpha < M_PI)
				{
					p.z =(GLfloat)((R2 - R2 * cosf(alpha)));
					p.x =(GLfloat)(offsetX - R2 * sinf(alpha));
				}
			}
			setVertex(ccp(i, j), p, pTarget);
		}
	}
	layout->setVisible(false);
	layout->setAnchorPoint(Vec2(0, 0));
	layout->setPosition(Vec2(0, 0));
	_gridCardValue->setPosition(Vec2(0, 0));

	Size winSize = Director::getInstance()->getWinSize();
	float distance = winSize.width / 2;

	if (offsetX <= M_PI*R2 && offsetX > 0.5*M_PI*R2)
	{

		offsetW = (GLfloat)(offsetX - R2 * sinf(offsetX / R2));

	}
	else if (offsetX > M_PI*R2)
	{
		offsetW = (GLfloat)(2 * offsetX - M_PI*R2);
	}

	if (offsetW != 0 && offsetX > 50)
	{
		//计算视角偏移量
		offsetW = distance - (distance / 2) * M_PI*(distance - (offsetW - m_vCardPoint.x + m_sCardSize.height / 2)) / ((distance / 2)* M_PI - 2 * R2);
		layout->setVisible(true);
		layout->setContentSize(Size(offsetW -10, winSize.height));
	}
	// CCLOG("ppppppppppppppppp,%f,%f", offsetW, offsetX);
}


GridBase* PageTurn::getGrid(NodeGrid *pTarget)
{
	//Rect uu = pTarget->getGridRect();

	//return Grid3D::create(m_sGridSize, pTarget->getGridRect());

	auto result = Grid3D::create(m_sGridSize, pTarget->getGridRect());
	result->setNeedDepthTestForBlit(true);
	return result;
}

void PageTurn::startWithTarget(NodeGrid *pTarget)
{

	GridBase *newgrid = this->getGrid(pTarget);

	GridBase *targetGrid = pTarget->getGrid();

	if (targetGrid && targetGrid->getReuseGrid() > 0)
	{
		if (targetGrid->isActive() && targetGrid->getGridSize().width == m_sGridSize.width
			&& targetGrid->getGridSize().height == m_sGridSize.height)
		{
			targetGrid->reuse();
		}
		else
		{
			CCASSERT(0, "Invalid grid parameters!");
		}
	}
	else
	{
		if (targetGrid && targetGrid->isActive())
		{
			targetGrid->setActive(false);
		}

		pTarget->setGrid(newgrid);
		pTarget->getGrid()->setActive(true);

	}
}

Vec3 PageTurn::vertex(const Vec2& position, NodeGrid *pTarget)
{
	Grid3D *g = (Grid3D*)pTarget->getGrid();
	return g->vertex(position);
}

Vec3 PageTurn::originalVertex(const Vec2& position, NodeGrid *pTarget)
{
	Grid3D *g = (Grid3D*)pTarget->getGrid();
	return g->originalVertex(position);
}

void PageTurn::setVertex(const Vec2& position, const Vec3& vertex, NodeGrid *pTarget)
{
	Grid3D *g = (Grid3D*)pTarget->getGrid();
	g->setVertex(position, vertex);
}



bool PageTurn::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	Point point1 = touch->getLocation();
	float a = floor(point1.x);
	float b = floor(point1.y);

	Size winSize = Director::getInstance()->getWinSize();

	if (!m_bTurnEnd &&point1.x > (m_vCardPoint.x - m_sCardSize.height / 2) && point1.x < (m_vCardPoint.x + m_sCardSize.height / 2) && point1.y < m_vCardPoint.x - m_sCardSize.width / 2+ m_sCardSize.width / 5)
	{
		m_pBeganPoint = point1;
		//CCLOG("KKKKKKKKKKKKKKKKKKKKKKKKKK,%f,%f", m_pBeganPoint.x, m_pBeganPoint.y);
	}
	else
	{
		return false;
	}
	return true;
}


void PageTurn::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
	Point point1 = touch->getLocation();
	Size winSize = Director::getInstance()->getWinSize();

	if (point1.y > (m_vCardPoint.y - m_sCardSize.width / 2) && point1.y < (m_vCardPoint.y + m_sCardSize.width / 2))
	{
		float chaX = m_vCardPoint.x - m_sCardSize.height/2;
		//CCLOG("333333333333333333333");
		float cha = m_vCardPoint.y - m_sCardSize.width/2;

		if (m_pBeganPoint.x != 0 && m_pBeganPoint.y != 0)
		{

			if (abs(point1.x - m_pBeganPoint.x) > point1.y - m_pBeganPoint.y){
				if(point1.x - m_pBeganPoint.x < 0){
					mHOffsetX = m_vCardPoint.x + m_sCardSize.height/2 - (m_pBeganPoint.x -point1.x);

					mHOffsetX = mHOffsetX <= 0 ? 0 : mHOffsetX;

					updateCardX(mHOffsetX,true);
					//CCLOG("22222222222,%f,%f", mHOffsetY, m_pBeganPoint.y);
					if (mHOffsetX - chaX <= m_sCardSize.height * 1 / 5 && !m_bTurnEnd)
					{
						m_bTurnEnd = true;
						//m_callBack(this);
						register_toLua_moveCardEnd();
						showCardValue();
					}

				}else{

					mHOffsetX = point1.x - m_pBeganPoint.x + chaX;

					mHOffsetX = mHOffsetX <= 0 ? 0 : mHOffsetX;

					updateCardX(mHOffsetX,false);
					//CCLOG("22222222222,%f,%f", mHOffsetY, m_pBeganPoint.y);
					if (mHOffsetX - chaX > m_sCardSize.height * 4 / 5 && !m_bTurnEnd)
					{
						m_bTurnEnd = true;
						//m_callBack(this);
						register_toLua_moveCardEnd();
						showCardValue();
					}
				}
			}else{

				mHOffsetY = point1.y - m_pBeganPoint.y + cha;

				mHOffsetY = mHOffsetY <= 0 ? 0 : mHOffsetY;

				updateCard(mHOffsetY);
				//CCLOG("22222222222,%f,%f", mHOffsetY, m_pBeganPoint.y);
				if (mHOffsetY - cha > m_sCardSize.width * 4 / 5 && !m_bTurnEnd)
				{
					m_bTurnEnd = true;
					//m_callBack(this);
					register_toLua_moveCardEnd();
					showCardValue();
				}
			}
			
		}
	}

	// if (point1.x > (m_vCardPoint.x - m_sCardSize.height / 2) && point1.x < (m_vCardPoint.x + m_sCardSize.height / 2))
	// {
	// 	//CCLOG("333333333333333333333");
	// 	float cha = m_vCardPoint.y - m_sCardSize.width/2;

	// 	if (m_pBeganPoint.x != 0 && m_pBeganPoint.y != 0)
	// 	{

	// 		mHOffsetY = point1.y - m_pBeganPoint.y + cha;

	// 		mHOffsetY = mHOffsetY <= 0 ? 0 : mHOffsetY;

	// 		updateCard(mHOffsetY);
	// 		//CCLOG("22222222222,%f,%f", mHOffsetY, m_pBeganPoint.y);
	// 		if (mHOffsetY - cha > m_sCardSize.width * 4 / 5 && !m_bTurnEnd)
	// 		{
	// 			m_bTurnEnd = true;
	// 			//m_callBack(this);
	// 			register_toLua_moveCardEnd();
	// 			showCardValue();
	// 		}
	// 	}
	// }
	else
	{
		mHOffsetY = 0;
		updateCard(mHOffsetY);
		m_pBeganPoint = Vec2(0, 0);
	}


}
void PageTurn::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{

	mHOffsetY = 0;
	updateCard(mHOffsetY);
	m_pBeganPoint = Vec2(0, 0);

}

void PageTurn::updateCard(float offsety)
{
	calculateVerticalVertexPoints(offsety, _gridCardBg);
	calculateVerticalVertexPoints(offsety, _gridCardValue);
	
}
void PageTurn::updateCardX(float offsetx, bool bRight)
{
	if(bRight){
		calculateVerticalVertexPointXR(offsetx, _gridCardBg);
		calculateVerticalVertexPointXR(offsetx, _gridCardValue);

	}else{
		calculateVerticalVertexPointXs(offsetx, _gridCardBg);
		calculateVerticalVertexPointXs(offsetx, _gridCardValue);
	}
	
}


#if CC_ENABLE_SCRIPT_BINDING
static int toLua_PageTurn_create(lua_State *tolua_S)
{
	int argc = lua_gettop(tolua_S);
	PageTurn *obj = nullptr;
	char *imgBg = "";
	char *imgValue = "";
	Vec2 point = Vec2(0, 0);
	LUA_FUNCTION handler;
	typedef std::function<void(Ref*)> _Callback;
	_Callback callBack;

	if (argc >= 3)
	{
		const char *imgBg = lua_tostring(tolua_S, 2);
		const char *imgValue = lua_tostring(tolua_S, 3);
		if (argc >= 4)
		{
			
			luaval_to_vec2(tolua_S, 4, &point, "toLua_PageTurn_create");


		}
		if (argc >= 5)
		{

			//callBack = lua_tocfunction(tolua_S, 5);
			//callBack = (void(Ref*))lua_getref(tolua_S, 5);
			//luaval_to_blendfunc(tolua_S, &callBack, 0);

		}

		obj = PageTurn::create(imgBg, imgValue, point);
	}
	int nID = (nullptr != obj) ? obj->_ID : -1;
	int *pLuaID = (nullptr != obj) ? &obj->_luaID : nullptr;
	toluafix_pushusertype_ccobject(tolua_S, nID, pLuaID, (void*)obj, "cc.PageTurn");
	return 1;
}

#endif

int register_all_PageTurn()
{
#if CC_ENABLE_SCRIPT_BINDING
	lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	tolua_usertype(tolua_S, "cc.PageTurn");
	tolua_cclass(tolua_S, "PageTurn", "cc.PageTurn", "cc.Layer", nullptr);

	tolua_beginmodule(tolua_S, "PageTurn");
	tolua_function(tolua_S, "create", toLua_PageTurn_create);
	tolua_endmodule(tolua_S);

	std::string typeName = typeid(PageTurn).name();
	g_luaType[typeName] = "cc.PageTurn";
	g_typeCast["PageTurn"] = "cc.PageTurn";
#endif
	return 1;
}

int register_toLua_moveCardEnd()
{
#if CC_ENABLE_SCRIPT_BINDING
	lua_State* L = GLL;

	lua_getfield(L, LUA_GLOBALSINDEX, "moveCardEnd");
	lua_pushnumber(L, 1);
	lua_call(L, 1, 1); /*  */
	lua_settop(L, 0);
#endif
	return 0;
}

int luaopen_lua_game(lua_State *L)
{
#if CC_ENABLE_SCRIPT_BINDING
	GLL = L;
#endif
	return 0;
}