local CompareView  = class("CompareView",function(config)
		local compareView =  display.newLayer(cc.c4b(0, 0, 0, 0))
		
    return compareView
end)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")

local SpineObj = appdf.req("client.src.external.SpineObj")



function CompareView:ctor(config)

	local this = self
	self.m_config = config


	-- 屏蔽层
    local mask = ccui.Layout:create()
    mask:setContentSize(yl.WIDTH, yl.HEIGHT)
    mask:setBackGroundColorType(LAYOUT_COLOR_SOLID)
    mask:setBackGroundColor(cc.c3b(0, 0, 0))
    mask:setBackGroundColorOpacity(200)
    mask:setTouchEnabled(true)
    self:addChild(mask)

	local spine = sp.SkeletonAnimation:create("compare/vs/skeleton.json","compare/vs/skeleton.atlas")
    spine:setAnimation(0, "animation", false)
	spine:setPosition(cc.p(667,375))
    self:addChild(spine)

	
	self.m_FirstCard = display.newSprite("compare/cardBack.png")
			:setAnchorPoint(cc.p(0.5,0.5))
			:move(330, 410)
			:setVisible(false)
			:addTo(self)
	self.m_SecondCard = display.newSprite("compare/cardBack.png")
			:move(1004, 330)
			:setAnchorPoint(cc.p(0.5,0.5))
			:setVisible(false)
			:addTo(self)
	
	self.m_bFirstWin = false



	

	

	self.m_UserInfo = {}
	self.m_UserInfo[1] = {}
	self.m_UserInfo[1].head = HeadSprite:createNormal({}, 72)
		:move(500,410)
		:setVisible(false)
		:addTo(self)
	self.m_UserInfo[1].name = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
		:move(500,490)
		:addTo(self)

	self.m_UserInfo[1].bg = display.newSprite("game_res/headBg_2.png")
			:move(500,410)
			:setVisible(false)
			:addTo(self)

	self.m_UserInfo[2] = {}
	self.m_UserInfo[2].head = HeadSprite:createNormal({}, 72)
		:move(834,330)
		:setVisible(false)
		:addTo(self)
	self.m_UserInfo[2].name = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
		:move(834,410)
		:addTo(self)

	self.m_UserInfo[2].bg = display.newSprite("game_res/headBg_2.png")
		:move(834,330)
		:setVisible(false)
		:addTo(self)

	self.m_AniCallBack = nil
end


function CompareView:CompareCard(firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
	self.m_AniCallBack = callback
	self.m_FirstCard:setVisible(true)
	self.m_SecondCard:setVisible(true)
	

	self.m_bFirstWin = bfirstwin

	self.m_UserInfo[1].head:updateHead(firstuser)
	self.m_UserInfo[2].head:updateHead(seconduser)

	self.m_UserInfo[1].head:setVisible(true)
	self.m_UserInfo[2].head:setVisible(true)

	local nickname 
	if firstuser and firstuser.szNickName then
		nickname = firstuser.szNickName
	else
		nickname = "游戏玩家"
	end

	self.m_UserInfo[1].name:setString(string.EllipsisByConfig(nickname,105, self.m_config))


	if seconduser and seconduser.szNickName then
		nickname = seconduser.szNickName
	else
		nickname = "游戏玩家"
	end

	self.m_UserInfo[2].name:setString(string.EllipsisByConfig(nickname,105, self.m_config))

	self.m_UserInfo[2].bg:setVisible(true)
	self.m_UserInfo[1].bg:setVisible(true)

	local this = self
	

	self:runAction(cc.Sequence:create(
						cc.DelayTime:create(0.3),
						cc.CallFunc:create(function(ref)
							local spine = sp.SkeletonAnimation:create("compare/head/skeleton.json", "compare/head/skeleton.atlas")
    						spine:setAnimation(0, "animation", false)
    						if bfirstwin then
								spine:setPosition(cc.p(834,330))
							else
								spine:setPosition(cc.p(500,410))
							end
    						this:addChild(spine)
						end),
						cc.DelayTime:create(0.3),
						cc.CallFunc:create(function(ref)
							if bfirstwin then
								this.m_SecondCard:setSpriteFrame(display.newSprite("compare/loseCard.png"):getSpriteFrame())
							else
								this.m_FirstCard:setSpriteFrame(display.newSprite("compare/loseCard.png"):getSpriteFrame())
							end
						end),
						cc.DelayTime:create(0.3),
						cc.CallFunc:create(function(ref)
							this.m_AniCallBack()
							this:removeFromParent()
						end)
						))

	

end




return CompareView