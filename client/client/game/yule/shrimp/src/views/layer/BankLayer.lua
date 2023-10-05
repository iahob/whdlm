
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local TAG_ENUM = 
{
	BT_CLOSEBANK		= 100,
	BT_TAKESCORE		= 101,
}

local BankLayer = class("BankLayer",function ()
	-- body
	return cc.Layer:create()
end)

function BankLayer:ctor( scene )
	-- body
	self._scene = scene
	self:createBankLayer()
end

------
--银行节点
function BankLayer:createBankLayer()
	local this = self
	self.m_bankLayer = cc.Node:create()
	self:addChild(self.m_bankLayer)

	--加载csb资源
	local csbNode = ExternalFun.loadCSB("ui/BankLayer.csb", self.m_bankLayer)
	local sp_bg = csbNode:getChildByName("sp_bg")

	------
	--按钮事件
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			this:onButtonClickedEvent(sender:getTag(), sender)
		end
	end	
	--关闭按钮
	local btn = sp_bg:getChildByName("close_btn")
	btn:setTag(TAG_ENUM.BT_CLOSEBANK)
	btn:addTouchEventListener(btnEvent)

	--取款按钮
	btn = sp_bg:getChildByName("out_btn")
	btn:setTag(TAG_ENUM.BT_TAKESCORE)
	btn:addTouchEventListener(btnEvent)
	------

	------
	--编辑框
	--取款金额
	local tmp = sp_bg:getChildByName("count_temp")
	local editbox = ccui.EditBox:create(tmp:getContentSize(),"kuang.png",UI_TEX_TYPE_LOCAL)
		:setPosition(tmp:getPosition())
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入取款金额")
	sp_bg:addChild(editbox)
	self.m_bankLayer.m_editNumber = editbox
	tmp:removeFromParent()

	--取款密码
	tmp = sp_bg:getChildByName("passwd_temp")
	editbox = ccui.EditBox:create(tmp:getContentSize(),"kuang.png",UI_TEX_TYPE_LOCAL)
		:setPosition(tmp:getPosition())
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入取款密码")
	sp_bg:addChild(editbox)
	self.m_bankLayer.m_editPasswd = editbox
	tmp:removeFromParent()
	------

	--当前游戏币
	self.m_bankLayer.m_textCurrent = sp_bg:getChildByName("text_current")

	--银行游戏币
	self.m_bankLayer.m_textBank = sp_bg:getChildByName("text_bank")

	--取款费率
	self.m_bankLayer.m_textTips = sp_bg:getChildByName("text_tips")
	self._scene:sendRequestBankInfo()
	self:refreshScore()
end

--取款
function BankLayer:onTakeScore()
	--参数判断
	local szScore = string.gsub(self.m_bankLayer.m_editNumber:getText(),"([^0-9])","")
	local szPass = self.m_bankLayer.m_editPasswd:getText()

	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end

	local lOperateScore = tonumber(szScore)
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",2)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	self._scene:showPopWait()	
	self._scene:sendTakeScore(szScore,szPass)
end

--刷新游戏币
function BankLayer:refreshScore()
	--携带游戏币
	local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	self.m_bankLayer.m_textCurrent:setString(str)

	--银行存款
	str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	self.m_bankLayer.m_textBank:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

	self.m_bankLayer.m_editNumber:setText("")
	self.m_bankLayer.m_editPasswd:setText("")
end
------

function BankLayer:onButtonClickedEvent(tag,ref)

	if tag == TAG_ENUM.BT_CLOSEBANK then
		self:removeFromParent()
	elseif tag == TAG_ENUM.BT_TAKESCORE then
		self:onTakeScore()		
	else
		showToast(self,"功能尚未开放！",1)
	end
end



return BankLayer