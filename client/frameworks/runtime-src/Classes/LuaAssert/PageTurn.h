#ifndef __PAGETURN_H__
#define __PAGETURN_H__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocos-ext.h"
#include "editor-support/cocostudio/CCSGUIReader.h"
#include "ui/UILayout.h"
#include "ui/GUIExport.h"
#include "ui/CocosGUI.h"


USING_NS_CC;
typedef std::function<void(Ref*)> _Callback;
int register_all_PageTurn();
int register_toLua_moveCardEnd();
int luaopen_lua_game(lua_State *L);
class PageTurn : public cocos2d::Layer{
public:
	static PageTurn* create(const char *cardBgPath, const char *cardVluePath, Vec2 cardPoint);
	virtual bool init(const char *cardBgPath, const char *cardVluePath, Vec2 cardPoint);
	//void setVertex(const Vec2& position, const Vec3& vertex, NodeGrid *pTarget);
	Vec3 originalVertex(const Vec2& position, NodeGrid *pTarget);
	Vec3 vertex(const Vec2& position, NodeGrid *pTarget);
	void startWithTarget(NodeGrid *pTarget);
	GridBase* getGrid(NodeGrid *pTarget);//bool bReverse, const CCSize& size);
	void calculateVerticalVertexPoints(float offsetY, NodeGrid *pTarget);
	void calculateVerticalVertexPointXs(float offsetX, NodeGrid *pTarget);
	void calculateVerticalVertexPointXR(float offsetX, NodeGrid *pTarget);
	void showCardValue();
	void setVertex(const Vec2& position, const Vec3& vertex, NodeGrid *pTarget);
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void updateCard(float offsety);
	void updateCardX(float offsetx, bool bRight);
public:
	NodeGrid* _gridCardValue;
	NodeGrid* _gridCardBg;
	bool m_bTurnEnd;
	ui::Layout *layout;
	const char* m_cCardBgPath;
	const char* m_cCardValuePath;
	Size m_sGridSize;
	Size m_sCardSize;
	Vec2 m_vCardPoint;
	_Callback  m_callBack;
	Vec2 m_pBeganPoint;
	int m_nMoveType;
	float mHOffsetY;
	float mHOffsetX;

};

#endif // __PAGETURN_H__