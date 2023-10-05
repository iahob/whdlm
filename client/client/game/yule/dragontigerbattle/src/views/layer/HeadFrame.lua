local HeadFrame = class("HeadFrame", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC.."HeadSprite")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

local ZORDERLAYER = {
	HEAD = 1,
	HEADFRAME = 2,
}
function HeadFrame:ctor(userItem, pos)
	ExternalFun.registerNodeEvent(self, true)	
	local rootLayer, csbNode = ExternalFun.loadRootCSB("head.csb", self)
	self.csbNode = csbNode
	csbNode:getChildByName("Panel_mask"):addClickEventListener(function(sender) 
			self:setVisible(false)
		end)

	--更改整体界面的位置
	local tmpNode = csbNode:getChildByName("Image_frame")
	tmpNode:move(pos)

	local head = tmpNode:getChildByName("head"):setLocalZOrder(ZORDERLAYER.HEADFRAME)

    -- self.head = 
    -- local headsp = display.newSprite("game/head.png")
	local img = PopupInfoHead:createClipHead(userItem, 105, "head_frame/avatar_size.png" )
	img:setLocalZOrder(ZORDERLAYER.HEAD)
    img:setPosition(cc.p(80,160))
    img:addTo(tmpNode)

    local name =  string.EllipsisByConfig(userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
    
	 tmpNode:getChildByName("Text_name"):setString(name)
	 tmpNode:getChildByName("Text_ID"):setString(userItem.dwGameID)

   local lScore =  tostring(userItem.lScore)
   if userItem.lScore >= 100000000 then
        tmpNode:getChildByName("Text_wan"):setVisible(false)
        tmpNode:getChildByName("Text_yi"):setVisible(true)
        lScore = string.format("%.2f", userItem.lScore  / 100000000)
    elseif userItem.lScore >= 10000 then
        tmpNode:getChildByName("Text_wan"):setVisible(true)
        tmpNode:getChildByName("Text_yi"):setVisible(false)
        lScore = string.format("%.2f", userItem.lScore  / 10000)
    else
        tmpNode:getChildByName("Text_wan"):setVisible(false)
        tmpNode:getChildByName("Text_yi"):setVisible(false)
    end

    local gold_panel = tmpNode:getChildByName("Text_gold")
    lScore = string.gsub(lScore,'%.','/')
    gold_panel:setString(lScore)
    tmpNode:getChildByName("Text_wan"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)
    tmpNode:getChildByName("Text_yi"):setPositionX(gold_panel:getPositionX() + gold_panel:getContentSize().width)

end

return HeadFrame