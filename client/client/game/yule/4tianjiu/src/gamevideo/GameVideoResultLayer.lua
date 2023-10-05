local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.GameLogic")

local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local GameVideoResultLayer = class("GameVideoResultLayer", TransitionLayer)


-- local GameVideoResultLayer = class("ResultLayer", function(scene)
-- 	local resultLayer = cc.CSLoader:createNode(cmd.RES.."video/Layer_VideoResult.csb")
-- 	return resultLayer
-- end)


GameVideoResultLayer.WINNER_ORDER					= 1


function GameVideoResultLayer:onInitData()
	--body
	self.winnerIndex = nil
	self.bShield = false

end

function GameVideoResultLayer:onResetData()
	--body
	self.winnerIndex = nil
	self.bShield = false
end

function GameVideoResultLayer:onExit()
	--重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
    local newPaths = {}
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(self._searchPath) then
            table.insert(newPaths, v)
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths)
    GameVideoResultLayer.super.onExit(self)
end

function GameVideoResultLayer:ctor(scene)
	GameVideoResultLayer.super.ctor(self, scene)

   self._searchPath = device.writablePath.."game/yule/4tianjiu/res/"
   cc.FileUtils:getInstance():addSearchPath(self._searchPath)

    -- 加载csb资源
    self.csbNode = cc.CSLoader:createNode("video/Layer_VideoResult.csb")
    self:addChild(self.csbNode)

   	self.m_bg = self.csbNode:getChildByName("Panel_1")

	self._scene = scene
	self:onInitData()
	ExternalFun.registerTouchEvent(self, true)

	-- 遮罩
    self.m_bg:addTouchEventListener(function(ref, tType)
    	if tType == ccui.TouchEventType.ended then
         	self:scaleTransitionOut(self.m_bg)
        end
	end)

	local btnExit = self.m_bg:getChildByName("Btn_close")
	btnExit:addTouchEventListener(function(ref, tType)
		if tType == ccui.TouchEventType.ended then
         	self:scaleTransitionOut(self.m_bg)
        end
	end)

	-- 加载动画
    self:scaleTransitionIn(self.m_bg)
end

--通过结算信息，绘制结算界面
function GameVideoResultLayer:showLayer(resultList,cbHuCard)
	assert(type(resultList) == "table")
	local width = 44
	local height = 67

    self._cardLayer = CardLayer:create(self)
	for i = 1, cmd.GAME_PLAYER do
		if i <= #resultList then
			--获取node
			local node = self.m_bg:getChildByName("Node_"..string.format("%d", i))
			local infoNode = cc.CSLoader:createNode(cmd.RES.."video/Node_info.csb")--self.csbNode:getChildByName("Node_"..string.format("%d", i))
			self.m_bg:addChild(infoNode)
			infoNode:setPosition(node:getPosition())

			print("获取玩家信息", "Node_"..string.format("%d", i), infoNode)
			if nil ~= infoNode then
				
				--展示麻将
				local nodeCard = infoNode:getChildByName("Node_Card")
				print("获取麻将node", nodeCard)
				local pos = cc.p(-50, 0)

				--展示手牌
				for j=1,#resultList[i].cbCardData do
					local sprCard = self._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, resultList[i].cbCardData[j], false)
					if nil ~= sprCard then
						nodeCard:addChild(sprCard)
						pos = cc.p(pos.x + width, 0)
						sprCard:setPosition(pos)
						print("结算麻将位置1",pos.x, pos.y)
					end
				end
				--显示碰刚的牌
				pos = cc.p(pos.x + 10, 0)
				if nil ~=  resultList[i].cbActiveCardData then
					for j=1,#resultList[i].cbActiveCardData do
						local tagAvtiveCard = resultList[i].cbActiveCardData[j]
						pos = cc.p(pos.x + 6, 0)
						for num=1,tagAvtiveCard.cbCardNum do
							local sprCard = self._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, tagAvtiveCard.cbCardValue[1], false)
							--sprCard:setAnchorPoint(cc.p(0.5, 0))
							if nil ~= sprCard then
								nodeCard:addChild(sprCard)
								if 4 ~= num then
									pos = cc.p(pos.x + width, 0)
									sprCard:setPosition(pos)
								else --第四张放上面，但是不影响POS的值
									local posUp = cc.p(pos.x - width, 8)
									sprCard:setPosition(posUp)
								end
							end
						end
					end
				end

				--显示胡的牌
				local nodeHuCard = infoNode:getChildByName("Node_HuCard")
				if resultList[i].huCard~=0 then
					local sprCard = self._cardLayer:createOutOrActiveCardSprite(cmd.MY_VIEWID, resultList[i].huCard, false)
					if nil ~= sprCard then
						nodeHuCard:addChild(sprCard)
					end
				end

				if nil ~= resultList[i].userItem then
					--ID
					local name = infoNode:getChildByName("Text_id")
					if nil ~= name then
						name:setString(resultList[i].userItem.dwGameID.."")
					end

					--昵称
					local name = infoNode:getChildByName("Text_name")
					if nil ~= name then
						
						local strNickname = string.EllipsisByConfig(resultList[i].userItem.szNickName, 100, 
	                                                            string.getConfig("fonts/round_body.ttf", 20))
						name:setString(strNickname)
					end

					--玩家头像
					local nodeFace = infoNode:getChildByName("Node_face")
			        if nil ~= nodeFace then
			            -- 头像
			            local head = PopupInfoHead:createNormal(resultList[i].userItem, 63)
			            head:enableInfoPop(false)
			            nodeFace:addChild(head)
			        end
		    	end

				local labHu = infoNode:getChildByName("Text_score") 
				if nil ~= labHu then
					labHu:setString(resultList[i].lScore.."")
				end
			end
		end
	end
	self.bShield = true
	self:setVisible(true)
	self:setLocalZOrder(yl.MAX_INT)
end


function GameVideoResultLayer:hideLayer()
	--重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
    local newPaths = {}
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(device.writablePath.."game/yule/4tianjiu/res/") then
            table.insert(newPaths, v)
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths)

	self:onResetData()
	--self._scene.btStart:setVisible(true)
	self:removeFromParent()
end

function GameVideoResultLayer:animationRemove()
    self:scaleTransitionOut(self.m_bg)
end

function GameVideoResultLayer:onTransitionInBegin()
	print("GameVideoResultLayer:onTransitionInBegin")
    self:sendShowEvent()
end

function GameVideoResultLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function GameVideoResultLayer:onTransitionOutFinish()
   self:removeFromParent()
end


return GameVideoResultLayer