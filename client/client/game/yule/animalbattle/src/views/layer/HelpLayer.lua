
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.animalbattle.src"
local PopupLayer=appdf.req(module_pre..".views.layer.PopupLayer")

local HelpLayer=class("HelpLayer",PopupLayer)

function HelpLayer:ctor(parentNode)

	self._parentNode=parentNode

	HelpLayer.super.ctor(self)
	self.csbNode=ExternalFun.loadCSB("HelpLayer.csb",self)
	
	appdf.getNodeByName(self.csbNode,"Button_1"):addClickEventListener(function() self:removeSelf() end)

	-- self.withdrawBtn=appdf.getNodeByName(self.csbNode,"btn_qu")
	-- self.bg=appdf.getNodeByName(self.csbNode,"bg")
	-- self.withdrawBtn:addClickEventListener(function() self:onTakeScore() end)

	-- self.curCoinLabel=appdf.getNodeByName(self.csbNode,"my_gold")
 --    self.insureLabel=appdf.getNodeByName(self.csbNode,"bank_gold")
 --    self.m_textTips=appdf.getNodeByName(self.csbNode,"tips")
end


return HelpLayer