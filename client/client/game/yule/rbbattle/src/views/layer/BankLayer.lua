local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.rbbattle.src"
local cmd = appdf.req(module_pre..".models.CMD_Game")

local BankLayer = class("BankLayer", cc.Layer)

function BankLayer:ctor(parent)
    self.editNum = 0
    self.parent = parent
    --加载csb资源
    self.csbNode = ExternalFun.loadCSB("bank/BankLayer.csb", self)
    
    --遮罩层
    self.csbNode:getChildByName("Panel_1"):addClickEventListener(function (sender) ExternalFun.playClickEffect() parent:removeBankerLayer() end)

   

    --取款按钮
    btn = self.csbNode:getChildByName("btn_take"):addClickEventListener(function(sender) ExternalFun.playClickEffect()
                                                                        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
                                                                        local szPass = self.editboxPw:getText()

                                                                        if not self:canUseBank() then 
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

                                                                        if not self:canUseBank() then 
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
                                                                        if lOperateScore<10000 or lOperateScore> 1000000000 then
                                                                            showToast(self,"单次存入必须大于1万，小于10亿！",2)
                                                                            return
                                                                        end
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
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        :setPlaceHolder("请输入取款金额")
    self:addChild(editbox)
    self.editboxNum = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --取款密码
    tmp = self.csbNode:getChildByName("editBg2")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sp_editBg.png")
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
    self.csbNode:getChildByName("Text_userCion"):setString(myScore)

    --银行游戏币
    local bankScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(bankScore) > 19 then
        bankScore = string.sub(bankScore, 1, 19)
    end
    self.csbNode:getChildByName("Text_insureCion"):setString(bankScore)

    local btnName = {"btn_1w" , "btn_10w" , "btn_100w"}
    self.btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}

    for i=1 , 3 do
        local btn = self.csbNode:getChildByName(btnName[i])
        btn:setTag(i)
        btn:addClickEventListener(function(sender) 
            ExternalFun.playClickEffect()
            self.editNum = self.btnNum[sender:getTag()] 
            self:addLabelAtlas(btn, self.btnNum[i])  
            self.editboxNum:setText(self.editNum)
        end)
    end

    local btn = self.csbNode:getChildByName("btn_allSave")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = GlobalUserItem.tabAccountInfo.lUserScore
        local playerJettonTotal = parent.m_CurrentJettonCount[cmd.RED_INDEX] + parent.m_CurrentJettonCount[cmd.BLACK_INDEX] + parent.m_CurrentJettonCount[cmd.POKER_TYPE_INDEX]
        print("parent.m_CurrentBetNum ==",playerJettonTotal)
        self.editboxNum:setText(self.btnNum[4] - playerJettonTotal)
    end)

    local btn = self.csbNode:getChildByName("btn_allTake")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = self.btnNum[5]
        self.editboxNum:setText(self.btnNum[5])
    end)

    local btn = self.csbNode:getChildByName("btn_del")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = 0
        self.editboxNum:setText("")
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
    self.csbNode:getChildByName("Text_userCion"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.csbNode:getChildByName("Text_insureCion"):setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

    self.editNum = 0
    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate,lBankOPButtonArray)
    local str = "温馨提示：取款将扣除"..rate.."‰的手续费"
    self.csbNode:getChildByName("tips"):setString(str)

    self.btnNum = lBankOPButtonArray
    dump(self.btnNum)
    local btnName = {"btn_1w" , "btn_10w" , "btn_100w"}
    for i=1 , 3 do
        local btn = self.csbNode:getChildByName(btnName[i])
        self:addLabelAtlas(btn, self.btnNum[i])
    end
end 

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/num.png",19, 26, string.byte("."))
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
        lab:setPositionX(parent:getContentSize().width/2-13)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 2)

    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-13)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 -4 , parent:getContentSize().height/2 +2)
    end
end

function BankLayer:canUseBank()
    if self.parent.m_HasJetton == true then 
        showToast(self,"已下注不可进行银行操作",1)
        return false
    end
    if self.parent:isMeChair(self.parent.m_wBankerUser) and self.parent.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
        showToast(self,"非空闲时间段不可进行银行操作",1)
        return false
    end
    return true
end
return BankLayer	