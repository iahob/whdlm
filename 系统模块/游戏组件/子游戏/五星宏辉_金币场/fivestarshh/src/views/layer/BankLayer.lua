local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.fivestarshh.src";
local cmd = appdf.req(module_pre..".models.CMD_Game")

local BankLayer = class("BankLayer", cc.Layer)

BankLayer.BT_MASK = 1
BankLayer.BT_CLOSE = 2
BankLayer.BT_TAKE = 3
BankLayer.BT_SAVE = 4

function BankLayer:ctor(parent)
    self.m_parent = parent
    self.btnNum = {10000 , 100000 , 1000000, GlobalUserItem.tabAccountInfo.lUserScore, GlobalUserItem.tabAccountInfo.lUserInsure}
     --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);

    self.editNum = 0

    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("game/yule/fivestarshh/res/bank/BankLayer.csb", self)
    
     --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(BankLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);

    --关闭
    local btn_close = self._csbNode:getChildByName("btn_close");
    btn_close:setTag(BankLayer.BT_CLOSE);
    btn_close:addTouchEventListener(btnEvent);

    --取款
    local btn_take = self._csbNode:getChildByName("btn_take");
    btn_take:setTag(BankLayer.BT_TAKE);
    btn_take:addTouchEventListener(btnEvent);

    --存款
    local btn_save = self._csbNode:getChildByName("btn_save");
    btn_save:setTag(BankLayer.BT_SAVE);
    btn_save:addTouchEventListener(btnEvent);

    --取款金额
    local input_score = self._csbNode:getChildByName("input_score");
    -- self.input_score = input_score;

   

    -- 编辑框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    --取款金额
    local tmp = self._csbNode:getChildByName("editBg1")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sp_editBg.png")
        :setPosition(tmp:getPosition())
        -- :setPosition(cc.p(tmp:getContentSize().width/2, tmp:getContentSize().height/2))
        -- :setTransformationMethod
        :setFontName("fonts/round_body.ttf")
        -- :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(28)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        -- :setPlaceHolder("    请输入存取金额")
        :setPlaceholderFontColor(cc.c3b(73, 53, 29))
    self:addChild(editbox)
    self.editboxNum = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --输入金币
    local gold_num = cc.LabelAtlas:create("","fonts/font_bank_num_2.png",21,28,string.byte("."));
    gold_num:setAnchorPoint(0.0,0.5);
    gold_num:addTo(self);
    gold_num:setPosition(input_score:getPosition());
    self.input_score = gold_num;

    --取款密码
    tmp = self._csbNode:getChildByName("editBg2")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sp_editBg.png")
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
    self._csbNode:getChildByName("cur_score"):setString(myScore)

    --银行游戏币
    local bankScore = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(bankScore) > 19 then
        bankScore = string.sub(bankScore, 1, 19)
    end
    self._csbNode:getChildByName("bank_score"):setString(bankScore)

    local btnName = {"btn_1" , "btn_2" , "btn_3"}
    -- local btnNum = {10000 , 100000 , 1000000}

    for i=1 , 3 do
        local btn = self._csbNode:getChildByName(btnName[i])
        btn:setTag(i)
        btn:addClickEventListener(function(sender) 
            ExternalFun.playClickEffect()
            --策划、测试让改成不叠加的
            -- self.editNum = (self.editNum or 0) +  btnNum[sender:getTag()]   

            self.editNum = self.btnNum[sender:getTag()];
            -- self.editboxNum:setText(self.editNum)

            self.input_score:setString(self.editNum)
            -- self.editboxNum:setText("")
        end)
    end

    local btn = self._csbNode:getChildByName("btn_all_save")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        local num = 0
        for k,v in pairs(parent.m_CurrentJettonCount) do
            num = num + v
        end
        self.editNum = self.btnNum[4] - num --GlobalUserItem.tabAccountInfo.lUserScore
        -- self.editboxNum:setText(self.editNum)
        -- self.editboxNum:setText("")
        self.input_score:setString(self.editNum)
    end)

    local btn = self._csbNode:getChildByName("btn_all_take")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = self.btnNum[5] --GlobalUserItem.tabAccountInfo.lUserInsure
        -- self.editboxNum:setText(self.editNum)
        -- self.editboxNum:setText("")
        self.input_score:setString(self.editNum)
    end)

    local btn = self._csbNode:getChildByName("btn_clean")
    btn:addClickEventListener(function(sender)
        ExternalFun.playClickEffect()
        self.editNum = 0
        -- self.editboxNum:setText("")
        self.input_score:setString("")
    end)
    
end

function BankLayer:onEditEvent(event, editbox)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    local src = editbox:getText()
    local tag = editbox:getTag()
    if event == "return" then
        self.input_score:setString(self.editboxNum:getText())
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
    self._csbNode:getChildByName("cur_score"):setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self._csbNode:getChildByName("bank_score"):setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

    self.editNum = 0
    self.editboxNum:setText("")
    self.editboxPw:setText("")
end

function BankLayer:refreshBankRate(rate, lBankOPButtonArray)
    local str = "温馨提示：取款将扣除"..rate.."‰的手续费"
    self._csbNode:getChildByName("tips"):setString(str)

    self.btnNum = lBankOPButtonArray
    local btnName = {"btn_1" , "btn_2" , "btn_3"}
    for i=1 , 3 do
        local btn = self._csbNode:getChildByName(btnName[i])
        self:addLabelAtlas(btn, self.btnNum[i])
    end
end 

--按钮回调方法
function BankLayer:onButtonClickedEvent(tag, sender)
    if BankLayer.BT_MASK == tag or BankLayer.BT_CLOSE == tag then
        ExternalFun.playClickEffect()
        -- self:removeFromParent();
        self.m_parent:removeBankLayer();
    elseif BankLayer.BT_TAKE == tag then
        ExternalFun.playClickEffect()
        local szScore = string.gsub(self.input_score:getString(),"([^0-9])","")
        local szPass = self.editboxPw:getText()
        
        if not self.m_parent:canUseBank() then
            return
        end
        --
        local myChairId = self.m_parent._scene:GetMeChairID();
        --当庄时 空闲状态才可存取款
        if self.m_parent.m_wBankerUser == myChairId and self.m_parent.m_cbGameStatus~=0 then
            showToast(self,"庄家非空闲时间段不可进行银行操作",2);
            return;
        --非庄家时  未下注就可存取款
        elseif self.m_parent.m_wBankerUser ~= myChairId and self.m_parent._scene.m_cbGameStatus==cmd.GAME_SCENE_BET then
            showToast(self,"已下注玩家不可进行银行操作",2)
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
        --     showToast(self,"庄家非空闲时间段不可进行银行操作",2);
        --     return;
        -- --非庄家时  未下注就可存取款
        -- elseif self.m_parent.m_wBankerUser ~= myChairId and self.m_parent._scene.m_cbGameStatus==cmd.GAME_SCENE_BET then
        --     showToast(self,"已下注玩家不可进行银行操作",2)
        --     return;
        -- end
        
        -- if self.m_parent.m_cbGameStatus~=0 then
        --     showToast(self,"空闲状态下才可以使用存款功能！",2)
        --     return
        -- end
        local szScore = string.gsub(self.input_score:getString(),"([^0-9])","")

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
        self.m_parent:getParentNode():showPopWait() 
        self.m_parent:getParentNode():onSaveScore(szScore) 
    end
end

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/num.png",23, 26, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 5 )
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/yi.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 5)
    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2 + 5)
    end
end
return BankLayer	