--
-- Author: luo
-- Date: 2016年12月30日 15:18:32
--
--设置界面
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local BankLayer = class("BankLayer", cc.Layer)

--构造
function BankLayer:ctor( _scene )
    self._inputNum = { 10000, 100000, 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}
    self._scene = _scene
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("bank/BankLayer.csb", self)
    self._csbNode:setPosition(0,0)

    self._editText = self._csbNode:getChildByName("AtlasLabel_3")
    self._editText:setString("")

    --取款金额
    local tmp = self._csbNode:getChildByName("editBG_score")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/editBG.png",0)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款金额")
    self._csbNode:addChild(editbox)
    self.m_editNumber = editbox
    -- tmp:removeFromParent()
    self.m_editNumber:registerScriptEditBoxHandler(function(event,editbox)
        self:onEditEvent(event, editbox)
    end)

    --取款密码
    tmp = self._csbNode:getChildByName("editBG_password")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/editBG.png",0)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款密码")
    self._csbNode:addChild(editbox)
    self.m_editPasswd = editbox
    -- tmp:removeFromParent()

    self.inputNum = 0
    self._editText:setZOrder(self.m_editNumber:getZOrder()+1)

    self:initBtn()
    self:refreshBankScore()
end

function BankLayer:initBtn()
    --关闭按钮
    local btn = self._csbNode:getChildByName("Button_close")
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            self:removeFromParent()
        end
    end)

    -- 输入
    self.bankBtns = {}
    for i = 1, 5 do
        local btn = self._csbNode:getChildByName("Button_"..i)
        btn:addTouchEventListener(function (ref, eventType)
            if eventType == ccui.TouchEventType.ended then
                ExternalFun.playClickEffect()
                if i < 4 then
                    self:addLabelAtlas(btn,self._inputNum[i])
                    table.insert(self.bankBtns,btn)
                end
                self.inputNum = self._inputNum[i]
                self._editText:setString(self.inputNum)
                self.m_editNumber:setText("")
                self.m_editNumber:setPlaceHolder("")
            end
        end)
    end

    -- 清空
    local btn = self._csbNode:getChildByName("Button_Clear")
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            
            self.inputNum = 0
            self._editText:setString(self.inputNum)
        end
    end)

    -- 取出
    local btn = self._csbNode:getChildByName("Button_out")
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            if self.inputNum ~= nil then--and GlobalUserItem.tabAccountInfo.lUserInsure >= self.inputNum then
                self:onTakeScore()
                self.inputNum = 0
                self._editText:setString(self.inputNum)
            end
        end
    end)

    -- 取出
    local btn = self._csbNode:getChildByName("Button_in")
    btn:addTouchEventListener(function (ref, eventType)
        if eventType == ccui.TouchEventType.ended then
            ExternalFun.playClickEffect()
            if self.inputNum ~= nil then--and GlobalUserItem.tabAccountInfo.lUserInsure >= self.inputNum then
                self:onSaveScore()
                self.inputNum = 0
                self._editText:setString(self.inputNum)
            end
        end
    end)
end

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/num.png",21, 29, string.byte("+"))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2  )
    local num = str
    lab:setString(num)
    if num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
    end
end
--取款
function BankLayer:onTakeScore()
    --参数判断
    local szPass = self.m_editPasswd:getText()

    if self.inputNum<1 then
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

    -- self:showPopWait()
    self._scene:sendTakeScore(self.inputNum,szPass)
end
--cun款
function BankLayer:onSaveScore()
    --参数判断
    if self._scene.meGetBag == true then
        showToast(self,"抢红包中不能存款",1)
        return
    end

    if self.inputNum<1 then
        showToast(self,"请输入正确金额！",2)
        return
    end

    -- self:showPopWait()
    self._scene:sendSaveScore(self.inputNum)
end

function BankLayer:onEditEvent(event, editbox)
    local src = editbox:getText() 
    if event == "return" then
        print("onEditEvent", src)

        self.m_editNumber:setText("")
        self.m_editNumber:setPlaceHolder("")
        self.inputNum = tonumber(src) or 0
        if self.inputNum > GlobalUserItem.tabAccountInfo.lUserInsure then
            self.inputNum = GlobalUserItem.tabAccountInfo.lUserInsure
        end
        self._editText:setString(self.inputNum)
        self._editText:setVisible(true)
    elseif event == "changed" then
    elseif event == "began" then
        self.m_editNumber:setText(src)
        self._editText:setVisible(false)
    end
end

--刷新银行游戏币
function BankLayer:refreshBankScore( )
    --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    -- str = str:gsub(",", ".")
    self._csbNode:getChildByName("myScore"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    -- str = str:gsub(",", ".")
    self._csbNode:getChildByName("bankScore"):setString(str)

    self.m_editNumber:setText("")
    self.m_editPasswd:setText("")
end

function BankLayer:refreshBank(rate, list)
    self._csbNode:getChildByName("Text_1"):setString("温馨提示：银行取款将收取".. rate.. "‰的服务费")
    self._inputNum = list
    for i = 1, 3 do
        local btn = self._csbNode:getChildByName("Button_"..i)
        self:addLabelAtlas(btn, self._inputNum[i])
    end
end

--触摸回调
function BankLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function BankLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self._csbNode:getChildByName("BG")
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

return BankLayer