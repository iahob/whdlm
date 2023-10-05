local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun");
local BankLayer = class("BankLayer", cc.Layer);
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")

BankLayer.BT_CLOSE = 1;
BankLayer.BT_MASK = 2;
BankLayer.BT_TAKE_OUT = 3;

local BANK_BTN_NUM = {0,0,0,0,0}

--构造
function BankLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("csb/BankLayer.csb", self);
    csbNode:setPosition(0,0);
    self._csbNode = csbNode;
    local sp_bg = csbNode:getChildByName("sp_bg")

    ------
    --按钮事件
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(), sender)
        end
    end
    --关闭按钮
    local btn = sp_bg:getChildByName("close_btn")
    btn:setTag(BankLayer.BT_CLOSE)
    btn:addTouchEventListener(btnEvent)

    local layout_bg = csbNode:getChildByName("layout_bg")
    layout_bg:setTag(BankLayer.BT_MASK)
    layout_bg:addTouchEventListener(btnEvent)

    --取款按钮
    btn = sp_bg:getChildByName("out_btn")
    btn:setTag(BankLayer.BT_TAKE_OUT)
    btn:addTouchEventListener(btnEvent)

    --存款按钮
    -- btn = sp_bg:getChildByName("in_btn")
    -- btn:setTag(TAG_ENUM.BT_SAVESCORE)
    -- btn:addTouchEventListener(btnEvent)
    ------

    ------
    --编辑框
    --取款金额
    local tmp = sp_bg:getChildByName("count_temp")
    local editbox = ccui.EditBox:create(tmp:getContentSize(),"game_bank_v2_plist/im_bank_edit.png",0)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入取款金额")
    sp_bg:addChild(editbox)
    self.m_editNumber = editbox
    tmp:removeFromParent()

    --取款密码
    tmp = sp_bg:getChildByName("passwd_temp")
    editbox = ccui.EditBox:create(tmp:getContentSize(),"game_bank_v2_plist/im_bank_edit.png",0)
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
    self.m_editPasswd = editbox
    tmp:removeFromParent()
    ------

    --当前游戏币
    self.textCurrent = sp_bg:getChildByName("text_current")

    --银行游戏币
    self.textBank = sp_bg:getChildByName("text_bank")

    --取款费率
    self.m_textTips = sp_bg:getChildByName("text_tips")

    self.bankBtns = {}
    for i = 1, 6 do 
        local btn = sp_bg:getChildByName("btn_"..i)
        btn:setTag(i)
        if i == 6 then
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_editNumber:setText("")
            end)
        else
            if i < 4 then
                self:addLabelAtlas(btn,BANK_BTN_NUM[i])
                table.insert(self.bankBtns,btn)
            end
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_editNumber:setText(BANK_BTN_NUM[sender:getTag()])
            end)
        end
    end


    -- --回调方法
    -- local cbtlistener = function (sender,eventType)
    --     if eventType == ccui.TouchEventType.ended then
    --         self:OnButtonClickedEvent(sender:getTag(),sender);
    --     end
    -- end

    -- --遮罩
    -- local btn = csbNode:getChildByName("Panel_1");
    -- btn:setTag(BankLayer.BT_MASK);
    -- btn:addTouchEventListener(cbtlistener);

    -- --背景
    -- local sp_bg = csbNode:getChildByName("bank_bg");
    -- self.m_spBg = sp_bg;

    -- --关闭btn
    -- local bt_close = csbNode:getChildByName("bt_close");
    -- bt_close:setTag(BankLayer.BT_CLOSE);
    -- bt_close:addTouchEventListener(cbtlistener);

    -- local btn_take_out = csbNode:getChildByName("btn_take_out");
    -- btn_take_out:setTag(BankLayer.BT_TAKE_OUT);
    -- btn_take_out:addTouchEventListener(cbtlistener);

    -- --当前游戏币
    -- self.textCurrent =  csbNode:getChildByName("my_score")

    -- --银行存款
    -- self.textBank  =  csbNode:getChildByName("bank_score")
  
    -- self:refreshScore();

    -- local take = csbNode:getChildByName("lab_gold");
    -- local password = csbNode:getChildByName("lab_password");
    -- take:setVisible(false);
    -- password:setVisible(false);
    -- ------------------------------------EditBox---------------------------------------------------


    -- --取款金额
    -- local editbox = ccui.EditBox:create(cc.size(350, 47),"")
    --     -- :setPosition(cc.p(30,take:getPositionY()))
    --     :setPosition(cc.p(772,310))
    --     :setFontName("fonts/round_body.ttf")
    --     :setPlaceholderFontName("fonts/round_body.ttf")
    --     :setFontSize(24)
    --     :setPlaceholderFontSize(24)
    --     :setMaxLength(32)
    --     :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
    --     :setPlaceHolder("请输入取款金额")
    -- csbNode:addChild(editbox)
    -- self.m_editNumber = editbox
  

    -- --取款密码
    -- local editbox1 = ccui.EditBox:create(cc.size(350, 47),"")
    --     -- :setPosition(cc.p(30,password:getPositionY()))
    --     :setPosition(cc.p(772,230))
    --     :setFontName("fonts/round_body.ttf")
    --     :setPlaceholderFontName("fonts/round_body.ttf")
    --     :setFontSize(24)
    --     :setPlaceholderFontSize(24)
    --     :setMaxLength(32)
    --     :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
    --     :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
    --     :setPlaceHolder("请输入取款密码")
    -- csbNode:addChild(editbox1)
    -- self.m_editPasswd = editbox1;
---------------------------------------------------------------------------------------------------------
end

function BankLayer:showLayer(var)
    self:setVisible(var);
end

--刷新银行金额
function BankLayer:refreshScore(curScore, bankScore)
    if curScore == nil or bankScore == nil then
        return;
    end

    local my_score_icon =  self._csbNode:getChildByName("my_score_icon");
    local bank_score_icon =  self._csbNode:getChildByName("bank_score_icon");

    -- local scoreStr, frame, isShow = self:formatScore(curScore);
    -- self.textCurrent:setString(scoreStr);

    self.textCurrent:setString(curScore);

    -- my_score_icon:setTexture(frame);
    -- my_score_icon:setVisible(isShow);

    -- local scoreStr, frame, isShow = self:formatScore(bankScore);
    -- self.textBank:setString(scoreStr);

    self.textBank:setString(bankScore);

    -- bank_score_icon:setTexture(frame);
    -- bank_score_icon:setVisible(isShow);
end

function BankLayer:refreshBankRate(rate, lBankOPButtonArray)
    -- local str = "温馨提示：取款将扣除"..rate.."%的手续费"
    -- self.csbNode:getChildByName("Text_notice"):setString(str)
    BANK_BTN_NUM = lBankOPButtonArray
    for i = 1, 3 do
        self:addLabelAtlas(self.bankBtns[i], BANK_BTN_NUM[i])
    end

    -- 取款收费比例
    local str = string.format("提示:银行取出将扣除%d‰的手续费。", rate);
    self.m_textTips:setString(str);
end 

function BankLayer:formatScore(score)
    local nWinScore = tonumber(score);
    local scoreStr = "";
    local isShow = false;
    local frame = "bank/wan.png";
    if nWinScore < 10000 then
        scoreStr = nWinScore;
        isShow = false;
    elseif nWinScore >=10000 and nWinScore < 100000000 then
        scoreStr = string.format("%.1f", nWinScore/10000)
        frame = "bank/wan.png";
        isShow = true;
    else
        scoreStr = string.format("%.1f", nWinScore/100000000)
        frame = "bank/yi.png";
        isShow = true;
    end
    return scoreStr, frame, isShow;
end

function BankLayer:onTakeScore()
    --参数判断
    local szScore = string.gsub( self.m_editNumber:getText(),"([^0-9])","")
    local szPass =   self.m_editPasswd:getText()

    if #szScore < 1 then 
        showToast(cc.Director:getInstance():getRunningScene(),"请输入操作金额！",2)
        return
    end

    local lOperateScore = tonumber(szScore)
    if lOperateScore<1 then
        showToast(cc.Director:getInstance():getRunningScene(),"请输入正确金额！",2)
        return
    end

    if #szPass < 1 then 
        showToast(cc.Director:getInstance():getRunningScene(),"请输入银行密码！",2)
        return
    end
    if #szPass <6 then
        showToast(cc.Director:getInstance():getRunningScene(),"密码必须大于6个字符，请重新输入！",2)
        return
    end

    -- showPopWait()
    self.m_parent._scene:showPopWait();
    self:sendTakeScore(lOperateScore,szPass)
end

function BankLayer:sendTakeScore(lScore, szPassword)
    local cmddata = ExternalFun.create_netdata(game_cmd.CMD_GR_C_TakeScoreRequest)
    cmddata:setcmdinfo(game_cmd.MDM_GR_INSURE, game_cmd.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushbyte(game_cmd.SUB_GR_TAKE_SCORE_REQUEST)
    cmddata:pushscore(lScore)
    cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)
    self.m_parent._scene:sendNetData(cmddata)
end

--按钮回调方法
function BankLayer:OnButtonClickedEvent( tag, sender )
    if BankLayer.BT_TAKE_OUT == tag then
        self:onTakeScore();
    elseif BankLayer.BT_CLOSE    == tag then
        self.m_parent.bankLayer = nil;
        self:removeFromParent();
    elseif BankLayer.BT_MASK == tag then
        self.m_parent.bankLayer = nil;
        self:removeFromParent();
    end
end

--触摸回调
function BankLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation();
    local m_spBg = self.m_spBg;
    pos = m_spBg:convertToNodeSpace(pos);
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height);
    if false == cc.rectContainsPoint(rec, pos) then
        local fadeout = cc.FadeOut:create(0.5);
        local call = cc.CallFunc:create(function( )
            self:removeFromParent();
        end)
        self._csbNode:runAction(cc.Sequence:create(fadeout, call));
    end
end

function BankLayer:onTouchEnded(touch, event)
    
end

function BankLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","game_bank_v2_plist/num2.png",19, 27, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2 + 4 )
    local num = str
    lab:setString(num)
    if num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("game_bank_v2_plist/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2+ 3)
    end
end
return BankLayer