local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local BankLayer = class("BankLayer", cc.Layer)


function BankLayer:ctor(parent)
    self.editNum = 0
    -- 按钮配置值
    self.btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("bank/BankLayer.csb", self)
    
    --遮罩层
    csbNode:getChildByName("Panel_1"):addClickEventListener(function (sender) ExternalFun.playClickEffect() self:setVisible(false) end)

   self.csbNode = csbNode;

    --取款按钮
    btn = self.csbNode:getChildByName("btn_take"):addClickEventListener(function(sender) ExternalFun.playClickEffect()
                                                                        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
                                                                        local szPass = self.editboxPw:getText()

                                                                        if not parent:canUseBank() then
                                                                            return
                                                                        end
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

                                                                        parent:getParentNode():showPopWait()  
                                                                        parent:getParentNode():sendTakeScore(szScore,szPass) 
                                                                    end)

    --存款按钮
    btn = self.csbNode:getChildByName("btn_save"):addClickEventListener(function(sender) ExternalFun.playClickEffect()

                                                                        if not parent:canUseBank() then
                                                                            return
                                                                        end
                                                                        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")

                                                                        if #szScore < 1 then 
                                                                            showToast(self,"请输入操作金额！",2)
                                                                            return
                                                                        end

                                                                        local lOperateScore = tonumber(szScore)
                                                                        if lOperateScore<1 then
                                                                            showToast(self,"请输入正确金额！",2)
                                                                            return
                                                                        end
                                                                        -- if lOperateScore<10000 or lOperateScore> 1000000000 then
                                                                        --     showToast(self,"单次存入必须大于1万，小于10亿！",2)
                                                                        --     return
                                                                        -- end
                                                                        parent:getParentNode():showPopWait() 
                                                                        parent:getParentNode():onSaveScore(szScore) 
                                                                    end)

    -- 编辑框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    --取款金额
    local tmp = self.csbNode:getChildByName("editBg1")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sp_editBg.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setPlaceholderFontSize(26)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        :setPlaceHolder("  请输入取款金额")
        -- :setFontColor(cc.c3b(161,112,76))
    self.csbNode:addChild(editbox)
    self.editboxNum = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --取款密码
    tmp = self.csbNode:getChildByName("editBg2")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sp_editBg.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setPlaceholderFontSize(26)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("  存款不需要密码")
        -- :setFontColor(cc.c3b(161,112,76))
    self.csbNode:addChild(editbox)
    self.editboxPw = editbox
    ------
    -- local myScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    --当前游戏币
    -- if string.len(myScore) > 19 then
    --     myScore = string.sub(myScore, 1, 19)
    -- end
    -- self.csbNode:getChildByName("cur_score"):setString(myScore)

    local cur_score = self.csbNode:getChildByName("cur_score");
    local cur_score_icon = self.csbNode:getChildByName("cur_score_icon");
    local scoreStr, scoreType = parent:formatScoreText(GlobalUserItem.tabAccountInfo.lUserScore);
    parent:formatScoreShow(scoreStr, scoreType, cur_score, cur_score_icon, "bank/icon_wan_2.png", "bank/icon_yi_2.png");

    --银行游戏币
    -- local bankScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    -- if string.len(bankScore) > 19 then
    --     bankScore = string.sub(bankScore, 1, 19)
    -- end
    -- self.csbNode:getChildByName("bank_score"):setString(bankScore)

    local bank_score = self.csbNode:getChildByName("bank_score");
    local bank_score_icon = self.csbNode:getChildByName("bank_score_icon");
    local scoreStr, scoreType = parent:formatScoreText(GlobalUserItem.tabAccountInfo.lUserInsure);
    parent:formatScoreShow(scoreStr, scoreType, bank_score, bank_score_icon, "bank/icon_wan_2.png", "bank/icon_yi_2.png");

    local btnName = {"btn_1" , "btn_2" , "btn_3"}

    for i=1 , 3 do
        local btn = self.csbNode:getChildByName(btnName[i])
        btn:setTag(i)
        self:addLabelAtlas(btn, self.btnNum[i])
        btn:addClickEventListener(function(sender) 
            ExternalFun.playClickEffect()
            self.editNum = self.btnNum[sender:getTag()]   
            self.editboxNum:setText(self.editNum)
        end)
    end

    local btn = self.csbNode:getChildByName("btn_all_save")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = self.btnNum[4] - parent.m_lHaveJetton -- GlobalUserItem.tabAccountInfo.lUserScore
        self.editboxNum:setText(self.editNum)
    end)

    local btn = self.csbNode:getChildByName("btn_all_take")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = self.btnNum[5] -- GlobalUserItem.tabAccountInfo.lUserInsure
        self.editboxNum:setText(self.editNum)
    end)

    local btn = self.csbNode:getChildByName("btn_clean")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = 0
        self.editboxNum:setText("")
    end)

    local btn_close = self.csbNode:getChildByName("btn_close")
    btn_close:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self:setVisible(false)
    end)

    parent:getParentNode():sendRequestBankInfo()
end

function BankLayer:onEditEvent(event, editbox)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    local src = editbox:getText()
    local tag = editbox:getTag()
    if event == "began" then
        
    elseif event == "changed" then
        
        self.editNum = tonumber(src)
        print("改变金额",self.editNum)
                
    elseif event == "return" then

    end
end

function BankLayer:refreshBankScore()
    --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.csbNode:getChildByName("cur_score"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.csbNode:getChildByName("bank_score"):setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

    self.editNum = 0
    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate, lBankOPButtonArray)
    local str = "(温馨提示：取款将扣除"..rate.."‰的手续费)"
    self.csbNode:getChildByName("tips"):setString(str)

    self.btnNum = lBankOPButtonArray
    for i=1 , 3 do
        local btn = self.csbNode:getChildByName("btn_"..i)
        self:addLabelAtlas(btn, self.btnNum[i])
    end
end 

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/font_num_1.png",24, 35, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 0)
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/icon_yi_1.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 0)
    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/icon_wan_1.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 0)
    end
end
return BankLayer	