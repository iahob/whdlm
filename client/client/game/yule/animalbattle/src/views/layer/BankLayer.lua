
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.animalbattle.src"
local PopupLayer=appdf.req(module_pre..".views.layer.PopupLayer")

local BankLayer=class("BankLayer",PopupLayer)

function BankLayer:ctor(parentNode)

	self._parentNode=parentNode
    self.btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}

	BankLayer.super.ctor(self)
	self.csbNode=ExternalFun.loadCSB("BankLayer.csb",self)
	
	appdf.getNodeByName(self.csbNode,"Button_1"):addClickEventListener(function() self:removeSelf() end)

	self.withdrawBtn=appdf.getNodeByName(self.csbNode,"btn_qu")
	self.bg=appdf.getNodeByName(self.csbNode,"bg")
	self.withdrawBtn:addClickEventListener(function() self:onTakeScore() end)
    self.withdrawBtn:setPositionX(600)

	self.curCoinLabel=appdf.getNodeByName(self.csbNode,"my_gold")
    self.insureLabel=appdf.getNodeByName(self.csbNode,"bank_gold")
    self.m_textTips=appdf.getNodeByName(self.csbNode,"tips")

	local editHanlder = function(event,editbox)
		self:onEditEvent(event,editbox)
	end

	--金额输入
	self.edit_Score = ccui.EditBox:create(cc.size(327,61), ccui.Scale9Sprite:create("bank/bankshadow.png"))
		:move(465,295)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(13)
		:setFontColor(cc.c4b(211,198,178,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("输入取款金额")
        :setPlaceholderFontColor(cc.c4b(211,198,178,255))
		:addTo(self.bg)
	self.edit_Score:registerScriptEditBoxHandler(editHanlder)

	--密码输入	
	self.edit_Password = ccui.EditBox:create(cc.size(327,61), ccui.Scale9Sprite:create("bank/bankshadow.png"))
		:move(465,218)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setMaxLength(32)
		:setFontColor(cc.c4b(211,198,178,255))
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("输入取款密码")
        :setPlaceholderFontColor(cc.c4b(211,198,178,255))
		:addTo(self.bg)

    self.btn_clean = appdf.getNodeByName(self.csbNode,"btn_clean")
    self.btn_all_qu = appdf.getNodeByName(self.csbNode,"btn_all_qu")
    self.btn_all_cun = appdf.getNodeByName(self.csbNode,"btn_all_cun")
    self.btn_wan_100 = appdf.getNodeByName(self.csbNode,"btn_wan_100")
    self.btn_wan_10 = appdf.getNodeByName(self.csbNode,"btn_wan_10")
    self.btn_wan_1 = appdf.getNodeByName(self.csbNode,"btn_wan_1")
    self.btn_cun = appdf.getNodeByName(self.csbNode,"btn_cun")
    self.btn_cun:setVisible(true)

    self:addLabelAtlas(self.btn_wan_1, self.btnNum[1])
    self:addLabelAtlas(self.btn_wan_10, self.btnNum[2])
    self:addLabelAtlas(self.btn_wan_100, self.btnNum[3])


    self.btn_clean:addClickEventListener(function()
        -- body
        self.edit_Score:setText("")
    end)
    self.btn_all_qu:addClickEventListener(function()
        -- body
        self.edit_Score:setText(self.btnNum[5])
    end)
    self.btn_all_cun:addClickEventListener(function()
        -- body
        self.edit_Score:setText(self.btnNum[4] - parentNode._scene:getMyBets() )
    end)
    self.btn_wan_100:addClickEventListener(function()
        -- body
        self.edit_Score:setText(self.btnNum[3])
    end)
    self.btn_wan_10:addClickEventListener(function()
        -- body
        self.edit_Score:setText(self.btnNum[2])
    end)
    self.btn_wan_1:addClickEventListener(function()
        -- body
        self.edit_Score:setText(self.btnNum[1])
    end)
    self.btn_cun:addClickEventListener(function()
        -- body
        self:onSaveScore()
    end)

	self._parentNode:getParent():sendRequestBankInfo()
end

--输入框监听
function BankLayer:onEditEvent(event,editbox)

end

function BankLayer:onSaveScore()

    if not self._parentNode:canUseBank() then
        return
    end
    local szScore = string.gsub(self.edit_Score:getText(),"([^0-9])","")

    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end

    local lOperateScore = tonumber(szScore)
    if lOperateScore<1 then
        showToast(self,"请输入正确金额！",2)
        return
    end

    -- parent:getParentNode():showPopWait() 
    self._parentNode:getParent():sendSaveScore(szScore) 
end

--取款
function BankLayer:onTakeScore()
    if not self._parentNode:canUseBank() then
        return
    end
    --参数判断
    local szScore = string.gsub(self.edit_Score:getText(),"([^0-9])","")
    local szPass = self.edit_Password:getText()

    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end

    local lOperateScore = tonumber(szScore)
    if lOperateScore<1 then
        showToast(self,"请输入正确金额！",2)
        return
    end

    if lOperateScore > GlobalUserItem.tabAccountInfo.lUserInsure then
        showToast(self,"您银行游戏币的数目余额不足,请重新输入游戏币数量！",2)
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

    self._parentNode:getParent():sendTakeScore(szScore,szPass)
end

--刷新银行游戏币
function BankLayer:refreshBankScore( )
    --携带游戏币
    local str = ""..GlobalUserItem.tabAccountInfo.lUserScore
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.curCoinLabel:setString(str)

    --银行存款
    dump(GlobalUserItem.tabAccountInfo,"GlobalUserItem.tabAccountInfo",12)
    str = GlobalUserItem.tabAccountInfo.lUserInsure..""
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.insureLabel:setString(str)

    self.edit_Score:setText("")
    self.edit_Password:setText("")
end
------

--银行操作成功
function BankLayer:onBankSuccess( )
    local bank_success = self._parentNode:getParent().bank_success
    if nil == bank_success then
        return
    end

    self:refreshBankScore()

    showToast(self, bank_success.szDescribrString, 2)
end

--银行操作失败
function BankLayer:onBankFailure( )
    local bank_fail = self._parentNode:getParent().bank_fail
    if nil == bank_fail then
        return
    end

    showToast(self, bank_fail.szDescribeString, 2)
end

--银行资料
function BankLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    local str = "温馨提示:取款将扣除" .. bankinfo.wRevenueTake .. "‰的手续费"
    self.m_textTips:setString(str)


    --携带游戏币
    local str = ""..bankinfo.lUserScore
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.curCoinLabel:setString(str)
    --银行存款
    str = bankinfo.lUserInsure..""
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.insureLabel:setString(str)

    self.btnNum = bankinfo.lBankOPButtonArray[1]
    self:addLabelAtlas(self.btn_wan_1, self.btnNum[1])
    self:addLabelAtlas(self.btn_wan_10, self.btnNum[2])
    self:addLabelAtlas(self.btn_wan_100, self.btnNum[3])

end

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/num.png",23, 37, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 2 )
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/yi.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 2)

    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 -4 , parent:getContentSize().height/2 +2)
    end
end

return BankLayer