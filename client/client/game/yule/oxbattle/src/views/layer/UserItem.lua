--
-- Author: zhong
-- Date: 2016-07-07 18:55:48
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

local UserItem = class("UserItem", cc.Node)

function UserItem:ctor()
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("Node_user_list_item.csb", self)
	self.m_csbNode = csbNode

	-- --头像
	self.spFace = csbNode:getChildByName("Sprite_face")

	--昵称
	tmp = csbNode:getChildByName("Text_name")
	local clipText = ClipText:createClipText(tmp:getContentSize(), "")
	clipText:setTextFontSize(24)
	clipText:setAnchorPoint(tmp:getAnchorPoint())
	clipText:setPosition(tmp:getPosition())
	clipText:setTextColor(cc.c3b(0,0,0))
	csbNode:addChild(clipText)
	tmp:removeFromParent()
	self.m_clipText = clipText

	--抢庄标志
	local rob = csbNode:getChildByName("Sprite_rob")
	rob:setVisible(false)
	self.m_spRob = rob

	--游戏币1
	local coin = csbNode:getChildByName("al_gold")
	coin:setString("")
	self.m_textCoin = coin
end

function UserItem.getSize( )
	return 958,113
end

--type == 1表示上庄申请列表
function UserItem:refresh( useritem,var_bRob, yPer, showtype)
	if nil == useritem then
		return
	end
	local showtype = showtype
	showtype = nil == showtype and 0 or 1
	--更新头像
	self.spFace:removeAllChildren()

	self.m_head = PopupInfoHead:createNormal(useritem, 90)
	self.m_head:setPosition(44, 46)
	self.spFace:addChild(self.m_head)

	local showpos = cc.p(360,320)
	local anchor = cc.p(1.0, yPer)
	if showtype == 1 then
		showpos = cc.p(10, 280)
	end
	self.m_head:enableInfoPop(true, showpos, anchor)

	--更新昵称
	local szNick = ""
	if nil ~= useritem.szNickName then
		szNick = useritem.szNickName
	end
	self.m_clipText:setString(szNick)

	--更新抢庄标志
	local bRob = var_bRob or false
	self.m_spRob:setVisible(bRob)

	--更新游戏币
	local coin = 0
	if nil ~= useritem.lScore then
		coin = useritem.lScore
	end
	-- local str = ExternalFun.formatScoreText(coin)
	-- self.m_textCoin:setString(str)

	local str = ExternalFun.numberThousands(coin)
    str = str:gsub(",", ".")
	self.m_textCoin:setString(str)
end

return UserItem