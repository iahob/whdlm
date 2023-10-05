local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local headFrame = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.views.layer.HeadFrame")

local PlayerSite = class("PlayerSite", cc.Sprite)
local ZORDER_HEAD = 1
local ZORDER_HEADBUTTON = 2


function PlayerSite:ctor(userItem)
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Site_node.csb", self)
	self.csbNode = csbNode 
	self.userInfo = userItem
	self.dwUserID = userItem.dwUserID


	self.csbNode:getChildByName("Text_name"):setVisible(false)
	self.csbNode:getChildByName("name_node"):setVisible(false)

	--记录弹出的头像框
	self:loaResource(userItem)
	self.headFrameLayer = nil



end

function PlayerSite:loaResource(userItem)
	--头像
	-- self.head = PopupInfoHead:createClipHead(userItem, 100, "head_frame/avatar_size.png" ):addTo(self.csbNode, ZORDER_HEAD)

	self.headBg = display.newSprite("game/image29.png")
    self.headBg:setAnchorPoint(cc.p(0.5, 0.5))



	self.head = PopupInfoHead:createClipHead(userItem, 100, "head_frame/avatar_size.png" )
	local headsp = display.newSprite("game/head.png")
	self.head:addChild(headsp)

    self.csbNode:addChild(self.headBg)
    self.csbNode:addChild(self.head)

    local name =  string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
	self.csbNode:getChildByName("Text_name"):setString(name)
	self.csbNode:getChildByName("Text_name"):setPositionY(-70)
	self.csbNode:getChildByName("name_node"):setPositionY(-70)
	-- csbNode:getChildByName("Text_gold"):setString(userItem.lScore)

	self.csbNode:getChildByName("Text_name"):setPositionY(-70)
	self.csbNode:getChildByName("name_node"):setPositionY(-70)
end

function PlayerSite:getUser()
	return self.userInfo
end

function PlayerSite:updateScore(lScore)
	self.userInfo.lScore = lScore
	-- self.csbNode:getChildByName("Text_gold"):setString(lScore)
end


function PlayerSite:headAnimation()
	-- body
	self.head:setVisible(false)
	self.headBg:setVisible(true)


 --    self.headBg:runAction(cc.Sequence:create(cc.Hide:create(),cc.CallFunc:create(function()
	-- 	self.headBg:setScaleX(-1)
	-- end),cc.Show:create(),cc.ScaleTo:create(0.9, 1, 1)))

	-- local frontSeq = cc.Sequence:create(cc.Hide:create(),cc.DelayTime:create(0.45),cc.Show:create(),cc.CallFunc:create(function()
	-- 	self.head:setScaleX(0)
	-- end),cc.ScaleTo:create(0.45, 1, 1))
	-- self.head:runAction(frontSeq)

    local animation = cc.Animation:create()
    for i = 1, 11 do
        animation:addSpriteFrameWithFile("head_frame/"..i..".png")
    end
    animation:setDelayPerUnit(0.05)
    animation:setRestoreOriginalFrame(false)

    self.headBg:runAction(cc.Animate:create(animation))

	self.head:runAction(cc.Sequence:create({cc.Hide:create(),cc.DelayTime:create(0.5),cc.Show:create(),cc.FadeIn:create(0.2)}))
end

return PlayerSite