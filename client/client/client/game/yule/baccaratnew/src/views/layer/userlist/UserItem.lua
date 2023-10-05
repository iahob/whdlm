--
-- Author: zhong
-- Date: 2016-07-07 18:55:48
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"

local UserItem = class("UserItem", cc.Node)

function UserItem:ctor(m_parent)
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("playlist/playerListItem.csb", self)
	self.m_csbNode = csbNode

	self.m_parent = m_parent;
	
	--头像
	local sp_head = csbNode:getChildByName("sp_head")	
	self.sp_head = sp_head
	-- tmp:removeFromParent()

	--昵称
	local text_name = csbNode:getChildByName("text_name")
	text_name:setString("")
	-- local clipText = g_var(ClipText):createClipText(tmp:getContentSize(), "")
	-- clipText:setTextFontSize(30)
	-- clipText:setAnchorPoint(tmp:getAnchorPoint())
	-- clipText:setPosition(tmp:getPosition())
	-- csbNode:addChild(clipText)
	-- tmp:removeFromParent()
	self.m_clipText = text_name

	-- --抢庄标志
	-- local rob = csbNode:getChildByName("sp_rob")
	-- rob:setVisible(false)
	-- self.m_spRob = rob

	--游戏币1
	local coin = csbNode:getChildByName("text_coin")
	coin:setString("")
	self.m_textCoin = coin

	self.score_icon = csbNode:getChildByName("score_icon");
end

function UserItem.getSize(  )
	return 656,80
end

function UserItem:refresh( useritem,var_bRob, yPer)
	if nil == useritem then
		return
	end
	--更新头像
	if nil ~= self.m_head and nil ~= self.m_head:getParent() then
		self.m_head:removeFromParent()
		self.m_head = nil
	end
	self.m_head = g_var(PopupInfoHead):createNormal(useritem, self.sp_head:getContentSize().width)
	self.m_head:setPosition(cc.p(self.sp_head:getContentSize().width/2,self.sp_head:getContentSize().height/2))
	self.sp_head:addChild(self.m_head)
	-- self.m_head:enableInfoPop(true, cc.p(350,220), cc.p(1.0, yPer))

	--更新昵称
	local szNick = ""
	if nil ~= useritem.szNickName then
		szNick = useritem.szNickName
	end
	self.m_clipText:setString(szNick)

	-- --更新抢庄标志
	-- local bRob = var_bRob or false
	-- self.m_spRob:setVisible(bRob)

	--更新游戏币
	local coin = 0
	if nil ~= useritem.lScore then
		coin = useritem.lScore
	end
	-- local str = ExternalFun.numberThousands(coin)
	-- if string.len(str) > 11 then
	-- 	str = string.sub(str, 1, 7) .. "..."
	-- end
	-- self.m_textCoin:setString(str)

	local scoreStr, scoreType = self.m_parent:formatScoreText(coin);
    self.m_parent:formatScoreShow(scoreStr, scoreType, self.m_textCoin, self.score_icon, "fonts/icon_player_wan.png", "fonts/icon_player_yi.png");
end

return UserItem