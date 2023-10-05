local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BankLayer = class("BankLayer", cc.Layer)

BANK_BTN_NUM = {0,0,0,0,0}

function BankLayer:ctor(parent)
    self.editNum = nil
    --加载csb资源
    self.csbNode = ExternalFun.loadCSB("bank/bankLayer.csb", self)
    
    --遮罩层
    self.csbNode:getChildByName("Panel_1"):addClickEventListener(function (sender) ExternalFun.playClickEffect() parent:removeBankerLayer() end)

    --关闭按钮
    local btn = self.csbNode:getChildByName("close_btn"):addClickEventListener(function (sender) ExternalFun.playClickEffect() parent:removeBankerLayer() end)

    --取款按钮
    btn = self.csbNode:getChildByName("out_btn"):addClickEventListener(function(sender) ExternalFun.playClickEffect()
                                                                        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
                                                                        local szPass = self.editboxPw:getText()

                                                                        if not parent:canUserBank() then
                                                                            return
                                                                        end

                                                                        if #szScore < 1 then 
                                                                            showToast(parent,"请输入操作金额！",2)
                                                                            return
                                                                        end

                                                                        local lOperateScore = tonumber(szScore)
                                                                        if lOperateScore<1 then
                                                                            showToast(parent,"请输入正确金额！",2)
                                                                            return
                                                                        end

                                                                        if #szPass < 1 then 
                                                                            showToast(parent,"请输入银行密码！",2)
                                                                            return
                                                                        end
                                                                        if #szPass <6 then
                                                                            showToast(parent,"密码必须大于6个字符，请重新输入！",2)
                                                                            return
                                                                        end

                                                                        parent:showPopWait()  
                                                                        parent:getParentNode():sendTakeScore(szScore,szPass) 
                                                                    end)

    --存款按钮
    self.csbNode:getChildByName("in_btn"):addClickEventListener(function(sender) ExternalFun.playClickEffect()

                                                                        if not parent:canUserBank() then
                                                                            return
                                                                        end
                                                                        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")

                                                                        if #szScore < 1 then 
                                                                            showToast(parent,"请输入操作金额！",2)
                                                                            return
                                                                        end

                                                                        local lOperateScore = tonumber(szScore)
                                                                        if lOperateScore<1 then
                                                                            showToast(parent,"请输入正确金额！",2)
                                                                            return
                                                                        end

                                                                        parent:getParentNode():showPopWait() 
                                                                        parent:getParentNode():sendSaveScore(szScore) 
                                                                    end)

    --取款金额
    local tmp = self.csbNode:getChildByName("input_6")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/input.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款金额")
    self:addChild(editbox)
    self.editboxNum = editbox

    --取款密码
    tmp = self.csbNode:getChildByName("input_7")
    editbox = ccui.EditBox:create(tmp:getContentSize(), "bank/input.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款密码")
    self:addChild(editbox)
    self.editboxPw = editbox
    ------

    local myScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    --当前游戏币
    if string.len(myScore) > 19 then
        myScore = string.sub(myScore, 1, 19)
    end
    self.csbNode:getChildByName("cur_gold"):setString(myScore)

    --银行游戏币
    local bankScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(bankScore) > 19 then
        bankScore = string.sub(bankScore, 1, 19)
    end
    self.csbNode:getChildByName("bank_gold"):setString(bankScore)

    self.bankBtns = {}
    for i = 1, 6 do 
        local btn = self.csbNode:getChildByName("Button_"..i)
        btn:setTag(i)
        if i == 6 then
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.editboxNum:setText("")
            end)
        else
            if i < 4 then
                self:addLabelAtlas(btn,BANK_BTN_NUM[i])
                table.insert(self.bankBtns,btn)
            end
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.editboxNum:setText(BANK_BTN_NUM[sender:getTag()])
            end)
        end
    end
    self:refreshBankScore()
end

function BankLayer:refreshBankScore()
    --携带游戏币
    -- local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    -- if string.len(str) > 19 then
    --     str = string.sub(str, 1, 19)
    -- end
    local gold = GlobalUserItem.tabAccountInfo.lUserScore
    local sp = self.csbNode:getChildByName("cur_sp"):setVisible(true)
    if gold > 100000000 then
        gold = gold / 100000000
        str = string.format("%.2f",gold)
        str = string.gsub(str, "%D", "/")
        sp:setTexture("bank/goldyi.png")
    elseif gold > 10000 then
        gold = gold / 10000
        str = string.format("%.2f",gold)
        str = string.gsub(str, "%D", "/")
        sp:setTexture("bank/goldwan.png")
    else
        sp:setVisible(false)
    end
    local cur_gold = self.csbNode:getChildByName("cur_gold"):setString(str)
    sp:setPosition( cur_gold:getPositionX()+cur_gold:getContentSize().width + 15, cur_gold:getPositionY())

    --银行存款
    -- str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    -- if string.len(str) > 19 then
    --     str = string.sub(str, 1, 19)
    -- end
    local bankScore = GlobalUserItem.tabAccountInfo.lUserInsure
    local temp = self.csbNode:getChildByName("bank_sp"):setVisible(true)
    if bankScore > 100000000 then
        bankScore = bankScore / 100000000
        str = string.format("%.2f",bankScore)
        str = string.gsub(str, "%D", "/")
        temp:setTexture("bank/goldyi.png")
    elseif bankScore > 10000 then
        bankScore = bankScore / 10000
        str = string.format("%.2f",bankScore)
        str = string.gsub(str, "%D", "/")
        temp:setTexture("bank/goldwan.png")
    else
        temp:setVisible(false)
    end
    local bank_gold = self.csbNode:getChildByName("bank_gold"):setString(str)
    temp:setPosition( bank_gold:getPositionX()+bank_gold:getContentSize().width + 15, bank_gold:getPositionY())

    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate, lBankOPButtonArray)
    local str = "温馨提示：银行取款操作将扣除"..rate.."‰的手续费"
    self.csbNode:getChildByName("Text_notice"):setString(str)
    BANK_BTN_NUM = lBankOPButtonArray
    dump(lBankOPButtonArray,"lBankOPButtonArray")
    for i = 1, 3 do
        self:addLabelAtlas(self.bankBtns[i], BANK_BTN_NUM[i])
    end
end 

function BankLayer:addLabelAtlas(parent, str)
    local tag = 11
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/numBtn.png",23, 35, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2  )
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/yi.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
    end
end

return BankLayer	