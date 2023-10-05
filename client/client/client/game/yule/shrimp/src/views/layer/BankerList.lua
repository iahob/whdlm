


local BankerList = class("BankerList",function ()
	-- body
	return cc.Layer:create()
end)

local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")
local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
function BankerList:ctor(scene)
	-- body
	self._scene = scene
	self:initUI()
end


function BankerList:initUI()
	-- body

	local csbnode = cc.CSLoader:createNode("ui/BankerList.csb");
	self:addChild(csbnode);


	local panel = csbnode:getChildByName("Panel")
	self.closeBtn = panel:getChildByName("close")
	self.shangzhuangBtn = panel:getChildByName("shuangzhuang")
	self.xiazhuang = panel:getChildByName("Button_1_0")
	self.quxiaoshangzhuang = panel:getChildByName("Button_1")
	self.listView = panel:getChildByName("ListView")
	self.text = panel:getChildByName("Text")

	self.closeBtn:addClickEventListener(function ()
		-- body
		self:removeFromParent()
	end)

	self.shangzhuangBtn:addClickEventListener(function ()
		-- body
		local userItem = self._scene:GetMeUserItem()
		if userItem.lScore < self._scene.lApplyBankerCondition then 
			showToast(self._scene, "金币不足!", 1)
		else
			self._scene:sendApplyBanker()
		end
		-- self:removeFromParent()
	end)

	self.quxiaoshangzhuang:addClickEventListener(function ()
		-- body
		self._scene:sendCancelApplyBanker()
		-- self:removeFromParent()
	end)

	self.xiazhuang:addClickEventListener(function ()
		-- body
		-- if self._scene.gameStatus ~= cmd.GAME_STATUS_FREE then 
		-- 	showToast(self._scene, "游戏过程中不能下庄!", 1)
		-- 	return
		-- end
		self._scene:sendCancelApplyBanker()
		self:removeFromParent()
	end)

	self.text:setString("携带金币大于"..self._scene.lApplyBankerCondition.."方可申请上庄")
end

function BankerList:showList(data)
	-- body

	-- dump(data)
	self.listView:removeAllChildren()

	if self._scene.bankerid == self._scene:GetMeChairID() then 
		self.xiazhuang:setVisible(true)
		self.quxiaoshangzhuang:setVisible(false)
		self.shangzhuangBtn:setVisible(false)
	else
		self.xiazhuang:setVisible(false)
		self.quxiaoshangzhuang:setVisible(false)
		self.shangzhuangBtn:setVisible(true)	
	end

	for k,v in pairs(data) do
		local userItem = self._scene:getUserInfoByChairID(v)
		if userItem then 
			self.listView:pushBackCustomItem(self:createItem(userItem))
			if v == self._scene:GetMeChairID() then
				-- 取消上庄
				self.quxiaoshangzhuang:setVisible(true)
				self.shangzhuangBtn:setVisible(false)
				self.xiazhuang:setVisible(false)
			end
		end
	end




end

function BankerList:createItem(data)
	-- body

	local item = ccui.Layout:create()
	item:setContentSize({width = 618, height = 100})
	item:setTouchEnabled(false)
	
	local csbnode = cc.CSLoader:createNode("ui/BankerItem.csb");
	item:addChild(csbnode);

	local panel = csbnode:getChildByName("Panel_2")
	local name = panel:getChildByName("name")
	local money = panel:getChildByName("money")
	local sp = money:getChildByName("Sprite_5")

	local moneyText = ""
	if data.lScore > 10^8 then 
		moneyText = string.format('%.2f', data.lScore/10^8)
		sp:setTexture("banker/yi.png")
	elseif data.lScore > 10^4 then
		moneyText = string.format('%.2f', data.lScore/10^4)
		sp:setTexture("banker/wan.png")
	else
		moneyText = string.format('%d', data.lScore)
		sp:setVisible(false)
	end
	-- moneyText = 50
	name:setString("")
	money:setString(moneyText)

	self.m_clipUserName = ClipText:createClipText(cc.size(200,50), data.szNickName, "fonts/round_body.ttf", 45)
	self.m_clipUserName:setTextColor(cc.c3b(62, 50, 50))
	self.m_clipUserName:setAnchorPoint(cc.p(0, 0.5))
	self.m_clipUserName:setPosition(name:getPosition())
	panel:addChild(self.m_clipUserName)

	local av = panel:getChildByName("Sprite_2")
	local x = av:getPositionX()
	local y = av:getPositionY()
	av:setVisible(false)

	-- if 

	av = HeadSprite:createClipHead( data, 90, "banker/avatar.png" )
	panel:addChild(av)
	av:setPosition(cc.p(x,y))

	local sp = cc.Sprite:create("banker/avatar.png")
	panel:addChild(sp)
	sp:setPosition(x,y)

	sp = cc.Sprite:create("public/banker_line.png")
	panel:addChild(sp)
	sp:setPosition(618/2,0)

	return item

end




return BankerList