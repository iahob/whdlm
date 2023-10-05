local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.attlhp.src";
local cmd = appdf.req(module_pre..".models.CMD_Game")

local BankLayer = class("BankLayer", cc.Layer)

BankLayer.BT_MASK = 1
BankLayer.BT_CLOSE = 2
BankLayer.BT_TAKE = 3
BankLayer.BT_SAVE = 4

function BankLayer:ctor(parent)
    self.m_parent = parent
     --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);

    self.editNum = 0

    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("bank/BankLayer.csb", self)
    
     --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --关闭
    local btn_close = self._csbNode:getChildByName("Button_close");
    btn_close:setTag(BankLayer.BT_CLOSE);
    btn_close:addTouchEventListener(btnEvent);

    --取款
    local btn_take = self._csbNode:getChildByName("Button_take");
    btn_take:setTag(BankLayer.BT_TAKE);
    btn_take:addTouchEventListener(btnEvent);

    --存款
    local btn_save = self._csbNode:getChildByName("Button_save");
    btn_save:setTag(BankLayer.BT_SAVE);
    btn_save:addTouchEventListener(btnEvent);

    --取款金额
    -- local input_score = self._csbNode:getChildByName("input_score");
    -- self.input_score = input_score;

   

    -- 编辑框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    --取款金额
    local tmp = self._csbNode:getChildByName("editBG_gold")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/editBG.png")
        :setPosition(tmp:getPosition())
        -- :setPosition(cc.p(tmp:getContentSize().width/2, tmp:getContentSize().height/2))
        -- :setTransformationMethod
        :setFontName("fonts/round_body.ttf")
        -- :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(28)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        -- :setPlaceHolder("    请输入取款金额")
        :setPlaceholderFontColor(cc.c3b(73, 53, 29))
    self:addChild(editbox)
    self.editboxNum = editbox
    -- editbox:registerScriptEditBoxHandler(editHanlder)

    --输入金币
    -- local gold_num = cc.LabelAtlas:create("","fonts/font_bank_num_2.png",21,28,string.byte("."));
    -- gold_num:setAnchorPoint(0.0,0.5);
    -- gold_num:addTo(self);
    -- gold_num:setPosition(input_score:getPosition());
    -- self.input_score = gold_num;

    --取款密码
    tmp = self._csbNode:getChildByName("editBG_password")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/editBG.png")
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        -- :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(28)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("    存款不需要密码")
        :setPlaceholderFontColor(cc.c3b(73, 53, 29))
    self:addChild(editbox)
    self.editboxPw = editbox
    ------

    local myScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    --当前游戏币
    if string.len(myScore) > 19 then
        myScore = string.sub(myScore, 1, 19)
    end
    self._csbNode:getChildByName("txt_lscore"):setString(myScore)

    --银行游戏币
    local bankScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(bankScore) > 19 then
        bankScore = string.sub(bankScore, 1, 19)
    end
    self._csbNode:getChildByName("txt_insure"):setString(bankScore)

    local btnName = {"Button_1" , "Button_2" , "Button_3"}
    local btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}

    for i=1 , 3 do
        local btn = self._csbNode:getChildByName(btnName[i])
        btn:setTag(i)
        self:setAtlasLabel(btn, btnNum[i])
        btn:addClickEventListener(function(sender) 
            ExternalFun.playClickEffect()
            self.editNum = btnNum[sender:getTag()]   
            self.editboxNum:setText(self.editNum)

            -- self.input_score:setString(self.editNum)
            -- self.editboxNum:setText("")
        end)
    end

    local btn = self._csbNode:getChildByName("Button_4")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = btnNum[4] --GlobalUserItem.tabAccountInfo.lUserScore
        self.editboxNum:setText(self.editNum)
        -- self.editboxNum:setText("")
        -- self.input_score:setString(self.editNum)
    end)

    local btn = self._csbNode:getChildByName("Button_5")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = btnNum[5] --GlobalUserItem.tabAccountInfo.lUserInsure
        self.editboxNum:setText(self.editNum)
        -- self.editboxNum:setText("")
        -- self.input_score:setString(self.editNum)
    end)

    local btn = self._csbNode:getChildByName("Button_clear")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = 0
        self.editboxNum:setText("")
        -- self.input_score:setString("")
    end)
    
end

function BankLayer:onEditEvent(event, editbox)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    local src = editbox:getText()
    local tag = editbox:getTag()
    if event == "return" then
        -- self.input_score:setString(self.editboxNum:getText())
        self.editboxNum:setText("")
        -- self.editboxNum:setText("")
        -- self.editboxNum:setVisible(true)

    elseif event == "began" then
        self.input_score:setString("")
        -- self.editboxNum:setVisible(false)
    elseif event == "changed" then
        -- self.input_score:setString(self.editboxNum:getText())

        self.editNum = tonumber(src)
        print("改变金额",self.editNum)
                
    
    end
end

function BankLayer:refreshBankScore()
    --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self._csbNode:getChildByName("txt_lscore"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self._csbNode:getChildByName("txt_insure"):setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

    self.editNum = 0
    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate)
    local str = "温馨提示：取款将扣除"..rate.."‰的手续费"
    self._csbNode:getChildByName("txt_tip"):setString(str)
end 

--按钮回调方法
function BankLayer:onButtonClickedEvent(tag, sender)
    if BankLayer.BT_MASK == tag or BankLayer.BT_CLOSE == tag then
        ExternalFun.playClickEffect()
        -- self:removeFromParent();
        self.m_parent:removeBankLayer();
    elseif BankLayer.BT_TAKE == tag then
        ExternalFun.playClickEffect()
        local szScore = string.gsub(self.editboxNum:getText(),"([^0-9])","")
        local szPass = self.editboxPw:getText()
        
        if not self.m_parent:canUseBank() then
            return
        end
        --
        local myChairId = self.m_parent._scene:GetMeChairID();
        --当庄时 空闲状态才可存取款
        if self.m_parent.m_wBankerUser == myChairId and self.m_parent.m_cbGameStatus~=0 then
            showToast(self,"庄家空闲状态下才可以使用取款功能！",2);
            return;
        --非庄家时  未下注就可存取款
        elseif self.m_parent.m_wBankerUser ~= myChairId and self.m_parent._scene.m_cbGameStatus==cmd.GAME_SCENE_BET then
            showToast(self,"已下注玩家不可使用取款功能！",2)
            return;
        end

        -- if self.m_parent.m_cbGameStatus~=0 then
        --     showToast(self,"空闲状态下才可以使用取款功能！",2)
        --     return
        -- end

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

        self.m_parent:getParentNode():showPopWait()  
        self.m_parent:getParentNode():sendTakeScore(szScore,szPass) 
    elseif BankLayer.BT_SAVE == tag then
        ExternalFun.playClickEffect()

        if not self.m_parent:canUseBank() then
            return
        end
        -- local myChairId = self.m_parent._scene:GetMeChairID();
        -- --当庄时 空闲状态才可存取款
        -- if self.m_parent.m_wBankerUser == myChairId and self.m_parent.m_cbGameStatus~=0 then
        --     showToast(self,"庄家空闲状态下才可以使用存款功能！",2);
        --     return;
        -- --非庄家时  未下注就可存取款
        -- elseif self.m_parent.m_wBankerUser ~= myChairId and self.m_parent._scene.m_cbGameStatus==cmd.GAME_SCENE_BET then
        --     showToast(self,"已下注玩家不可使用存款功能！",2)
        --     return;
        -- end
        
        -- if self.m_parent.m_cbGameStatus~=0 then
        --     showToast(self,"空闲状态下才可以使用存款功能！",2)
        --     return
        -- end
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
        self.m_parent:getParentNode():showPopWait() 
        self.m_parent:getParentNode():onSaveScore(szScore) 
    end
end

function BankLayer:setAtlasLabel(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/btnNUm.png",26, 39, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 - 2 )
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/btn_yi.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 -3)
    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/btn_wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 -3)
    end
end
return BankLayer	