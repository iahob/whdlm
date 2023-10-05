local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BankLayer = class("BankLayer", cc.Layer)
local GAME_CMD = appdf.req(appdf.GAME_SRC.."yule.dragontigerbattle.src.models.CMD_Game")

function BankLayer:ctor(parent)
    self.editNum = nil
    self.btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}
    --加载csb资源
    self.csbNode = ExternalFun.loadCSB("bank/bankLayer.csb", self)
    
    --遮罩层
    self.csbNode:getChildByName("Panel_1"):addClickEventListener(function (sender) ExternalFun.playClickEffect() parent:removeBankerLayer() end)

    --关闭按钮
    local btn = self.csbNode:getChildByName("close_btn"):addClickEventListener(function (sender) ExternalFun.playClickEffect() parent:removeBankerLayer() end)

    --取款按钮
    btn = self.csbNode:getChildByName("out_btn"):addClickEventListener(function(sender) ExternalFun.playClickEffect()

        if parent.gameState ~= GAME_CMD.GAME_SCENE_FREE and parent:isMeIsBanker() then 
            -- assert(false)
            return showToast(parent,"非空闲时间段不可进行银行操作",1)   
        end

        if parent.isMyBet then
            return showToast(parent,"已下注不可进行银行操作",1)   
        end

        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
        local szPass = self.editboxPw:getText()

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
    btn = self.csbNode:getChildByName("chun_btn"):addClickEventListener(function(sender) ExternalFun.playClickEffect()

        if parent.gameState ~= GAME_CMD.GAME_SCENE_FREE and parent:isMeIsBanker() then 
            return showToast(parent,"非空闲时间段不可进行银行操作",2)   
        end

        if parent.isMyBet then
            return showToast(parent,"已下注不可进行银行操作",2)   
        end

        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
        -- local szPass = self.editboxPw:getText()

        if #szScore < 1 then 
            showToast(parent,"请输入操作金额！",2)
            return
        end

        local lOperateScore = tonumber(szScore)
        if lOperateScore<1 then
            showToast(parent,"请输入正确金额！",2)
            return
        end

        parent:showPopWait()  
        parent:getParentNode():sendSaveScore(szScore) 
    end)

    --取款金额
    local tmp = self.csbNode:getChildByName("count_temp")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/image10.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        :setPlaceHolder("请输入取款金额")
    self:addChild(editbox)
    self.editboxNum = editbox

    --取款密码
    tmp = self.csbNode:getChildByName("passwd_temp")
    editbox = ccui.EditBox:create(tmp:getContentSize(), "bank/image10.png")
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

    parent:getParentNode():sendRequestBankInfo()

    -- dump(GlobalUserItem.tabAccountInfo,"GlobalUserItem.tabAccountInfo",12)
    -- 一万
    self.csbNode:getChildByName("btn_1"):addClickEventListener(function()
        self.editboxNum:setText(self.btnNum[1])
    end)

    -- 十万
    self.csbNode:getChildByName("btn_2"):addClickEventListener(function()
        -- body
        self.editboxNum:setText(self.btnNum[2])
    end)

    -- 百万
    self.csbNode:getChildByName("btn_3"):addClickEventListener(function()
        -- body
        self.editboxNum:setText(self.btnNum[3])
    end)
    for i=1 , 3 do
        local btn = self.csbNode:getChildByName("btn_"..i)
        self:addLabelAtlas(btn, self.btnNum [i])
    end

    -- 全部
    self.csbNode:getChildByName("btn_quan_qu"):addClickEventListener(function()
        -- body
        self.editboxNum:setText(self.btnNum[5])--GlobalUserItem.tabAccountInfo.lUserInsure)
    end)

    -- 全部
    self.csbNode:getChildByName("btn_quan_chun"):addClickEventListener(function()
        -- body
        self.editboxNum:setText(self.btnNum[4] - parent.myAllJettonScore)--self.csbNode:getChildByName("cur_gold"):getString())
    end)


    -- 清除
    self.csbNode:getChildByName("clear"):addClickEventListener(function()
        -- body
        self.editboxNum:setText("")
    end)

    self.csbNode:getChildByName("out_btn"):setEnabled(true)
    self.csbNode:getChildByName("chun_btn"):setEnabled(true)


end

function BankLayer:refreshBankScore()
    -- --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.csbNode:getChildByName("cur_gold"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.csbNode:getChildByName("bank_gold"):setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate)
    local str = "温馨提示：取款将扣除"..rate.wRevenueTake.."‰的手续费"
    self.csbNode:getChildByName("Text_notice"):setString(str)

    -- dump(rate,"'rate",12)
    self.csbNode:getChildByName("cur_gold"):setString(rate.lUserScore)
    self.csbNode:getChildByName("bank_gold"):setString(rate.lUserInsure)
    self.btnNum = rate.lBankOPButtonArray[1]
    for i=1 , 3 do
        local btn = self.csbNode:getChildByName("btn_"..i)
        self:addLabelAtlas(btn, self.btnNum [i])
    end

end 

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/num.png",19, 32, string.byte("."))
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
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 +2)
    end
end
return BankLayer	