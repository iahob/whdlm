
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)


local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面

PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_HELP",             -- 帮助
    "BTN_CHARGE",           -- 充值
    "BTN_MYROOM",           -- 自己房间
    "BTN_CREATE",           -- 加入房间
    "BTN_CREATE_1",
    "BTN_ENTERGAME",        -- 进入游戏
    "CBT_ONE_PAY",          -- 一人支付
    "CBT_AA_PAY",           -- AA

    -- 玩法配置
    "CBT_HAVEZICARD",         -- 带风
    "CBT_NOMAGICDOUBLE",      -- 无鬼翻倍
    "CBT_QIANGGANGHU",        -- 抢杠胡
    "CBT_HUQIDUI",            -- 胡七对

    -- 鬼牌
    "MENU_DROPDOWN_2",      -- 下拉菜单2
    "MASK_DROPDOWN_2",      -- 下拉背景2

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    --码数
    "CBT_MANUM",            -- 0马

    "CBT_SETTING",

    "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式
    "BTN_CREATEGOLD",

     "BTN_CELLSCORE",                -- 底注
    "BTN_ENTERCONDITION",           -- 入场设定
    "BTN_EXITCONDITION",            -- 离场设定
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_BEGIN = 400
local CBT_RULEBEGIN = 10      -- 玩法
local CBT_FANXINGBEGIN = 20     -- 番型

local dropDownMenuYPox = {20, 60, 100, 140, 180}
local dropDownBgHeight = {40, 80, 120, 160, 200}

-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    self.param=param
    local this = self
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 底板
    local spbg = csbNode:getChildByName("Panel_1")
    self.m_spBg = spbg

    self.m_nodeNormal = spbg:getChildByName("node_normal")
    self.m_nodeGold = spbg:getChildByName("node_gold")
    --self.m_nodeGold:setVisible(false)

    local nodeNormal = self.m_nodeNormal

    --是否允许陌生人加入
    self.m_nOpenMode = true
    local openmodelistener = function (sender,eventType)
        this:onOpenModeSelectedEvent(sender:getTag(),sender)
    end
    local checkbx = self.m_nodeGold:getChildByName("check_set")
    checkbx:setTag(TAG_ENUM.CBT_SETTING)
    checkbx:setSelected(true)
    checkbx:addEventListener(openmodelistener)

     local btn = self.m_nodeGold:getChildByName("btn_cellscore")
    btn:setTag(TAG_ENUM.BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)

     local btn = self.m_nodeGold:getChildByName("btn_entercondition")
    btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = self.m_nodeGold:getChildByName("btn_exitcondition")
    btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)


    --默认是积分房卡
    self.m_bIsNormal = self.param.cbGameMode or false--.true --true 为积分房卡，false为金币房卡

    -- -- 帮助按钮node_normal node_gold
    -- local btn = spbg:getChildByName("btn_tips")
    -- btn:setTag(TAG_ENUM.BTN_HELP)
    -- btn:addTouchEventListener(btncallback)  

    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore=1

    --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end



     -- 代人开房
    for k,v in pairs(spbg:getChildren()) do
        print("spbg child",v:getName())
    end
    local btn = self.m_nodeNormal:getChildByName("btn_createroom_1")
    btn:setTag(TAG_ENUM.BTN_CREATE_1)
    btn:addTouchEventListener(btncallback)
    btn:setVisible(false)
    -- 必须参与
    if 1 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame then
        btn:setEnabled(false)
        btn:setOpacity(200)
    end

    -- 创建按钮
    self.m_createBtn = self.m_nodeNormal:getChildByName("btn_createroom")
    self.m_createBtn:setTag(TAG_ENUM.BTN_CREATE)
    self.m_createBtn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            self.m_createBtn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE_1,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            self.m_createBtn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then
            if self.m_createBtn:getNumberOfRunningActions() >0 then
                self.m_createBtn:stopAllActions()
                self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE,ref)
            end
        end
    end)

     btn = self.m_nodeGold:getChildByName("btn_createroom")
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)
    btn:setPositionX(600)

    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        self:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    checkbx = self.m_nodeNormal:getChildByName("check_consume_1")
    checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = self.m_nodeNormal:getChildByName("check_consume_2")
    checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx

     --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then

        elseif 1 == self.param.cbPayTypeMask then  --群主支付
            self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY]:setVisible(false)
            local txt_consume_2 = nodeNormal:getChildByName("txt_consume_2")
            txt_consume_2:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = nodeNormal:getChildByName("txt_consume_1")
            txt_consume_1:setString("圈主支付")
        end
    end
    
    -- 是否密码
    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    self.m_nPasswdModeIdx = TAG_ENUM.CBT_NOPASSWD
    local passwdmodelistener = function (sender,eventType)
        self:onPasswdModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPasswdModeBox = {}
    -- 需要密码
    checkbx = self.m_nodeNormal:getChildByName("check_passwdmode_1")
    checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx
    -- 不需要密码
    checkbx = self.m_nodeNormal:getChildByName("check_passwdmode_2")
    checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

      -- 创建费用
    self.m_txtFee = self.m_nodeNormal:getChildByName("txt_createfee")
    self.m_txtFee:setString("")


    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%", bPirGoldMode, bPirScoreMode, GlobalUserItem.nCurGameKind)


    self.m_nodeNormal:setVisible(self.m_bIsNormal)
    self.m_nodeGold:setVisible(not self.m_bIsNormal)


    local baseNodes={ self.m_nodeNormal, self.m_nodeGold}

    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabRuleCheckBox = {{},{}}
    self.m_nSelectConfigIdx = {{0, 0},{0, 0}}
    self.m_txtSelectRound={}
    self.m_nSelectFanIdx = {{0,0,0,0},{0,0,0,0}}

    -- 下拉菜单2
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown2 = btn
    -- 下拉箭头2
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow2 = btn
    -- 下拉背景2
    panel = self.m_nodeNormal:getChildByName("dropdown_panel_2")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_2)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown2 = panel
    -- 菜单背景2
    self.m_imageDropDownBg2 = panel:getChildByName("dropdown_2")

    -- 选择局数
    self.m_txtSelectRound = self.m_nodeNormal:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    self.m_txtCellScoreLimit = self.m_nodeGold:getChildByName("txt_tips1")
    self.m_txtEnterLimit = self.m_nodeGold:getChildByName("txt_tips2") 
    self.m_txtExitLimit = self.m_nodeGold:getChildByName("txt_tips3") 
    
    for kth=1,#baseNodes do
        print("kth:",kth)
        -- 玩法选项
        for i = 1, 2 do
            local checkbx = baseNodes[kth]:getChildByName("check_rule_Type" .. i)
           -- print("======================>", checkbx, "check_rule_Type" .. i)
            if nil ~= checkbx then
                checkbx:setVisible(true)
                checkbx:setTag(CBT_RULEBEGIN + i)
                checkbx:addEventListener(cbtlistener)
                checkbx:setSelected(false)
                self.m_tabRuleCheckBox[kth][CBT_RULEBEGIN + i] = checkbx
            end
        end
        -- 选择的玩法(可以多选)    
        
        self.m_nSelectConfigIdx[kth][1] = CBT_RULEBEGIN + 1  --默认勾选换三张
        self.m_tabRuleCheckBox[kth][self.m_nSelectConfigIdx[kth][1]]:setSelected(true)

        -- 番型选项
        for i = 1, 4 do
            local checkbx = baseNodes[kth]:getChildByName("check_rule_play" ..i)
            if nil ~= checkbx then
                checkbx:setVisible(true)
                checkbx:setTag(CBT_FANXINGBEGIN + i)
                checkbx:addEventListener(cbtlistener)
                checkbx:setSelected(false)
                self.m_tabRuleCheckBox[kth][CBT_FANXINGBEGIN + i] = checkbx
            end
        end

        -- 选择的番型   
        
        self.m_nSelectFanIdx[kth][1] = CBT_FANXINGBEGIN + 1  
        self.m_tabRuleCheckBox[kth][self.m_nSelectFanIdx[kth][1]]:setSelected(true)

     
    end

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self)
end

function PriRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end

--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)
    if TAG_ENUM.BTN_CELLSCORE == tag then
        --设置了底分，入场和离场也会变化
        self.m_cellScore = scoere
        if self.m_cellScore > PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore
        elseif self.m_cellScore < PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        end
        self.m_nSelectScore = self.m_cellScore

        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore

        self.m_txtCellScoreLimit:setString(self.m_cellScore)
        self.m_txtEnterLimit:setString(self.m_enterScore)
        self.m_txtExitLimit:setString(self.m_exitScore)

    elseif TAG_ENUM.BTN_ENTERCONDITION == tag then
        --入场设定要判断底分,大于最小值才生效
        if scoere > self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore then
            self.m_enterScore = scoere
            if scoere > self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMaxEnterScore then
                self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMaxEnterScore
            end

            self.m_txtEnterLimit:setString(self.m_enterScore)
            --入场设定设置了，会改变离场设定
            self.m_exitScore = self.m_enterScore
            self.m_txtExitLimit:setString(self.m_exitScore)
        end

    elseif TAG_ENUM.BTN_EXITCONDITION == tag then
        --计算最小值，可能不能整除
        local minExit = math.floor(self.m_enterScore/2)
        if (self.m_enterScore - minExit *2) ~= 0 then --不能整除
            minExit = math.floor(self.m_enterScore/2) +1   --不能整除要往上
        end
        if scoere < self.m_enterScore then
            self.m_exitScore = scoere
            if self.m_exitScore < minExit then
                self.m_exitScore = minExit
            end
            self.m_txtExitLimit:setString(self.m_exitScore)
        end
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    --self.m_txtCardNum:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    
    -- self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_nodeNormal:setVisible(self.m_bIsNormal)

     -- self.m_btnGoldType:getChildByName("Text"):setColor(tag == TAG_ENUM.CBT_GOLDMODE and  cc.c3b(0xe9,0xfb,0xff) or cc.c3b(0xa5,0x89,0xbc))
     -- self.m_btnScoreType:getChildByName("Text"):setColor(tag == TAG_ENUM.CBT_SCOREMODE and  cc.c3b(0xe9,0xfb,0xff) or cc.c3b(0xa5,0x89,0xbc))
end

-- 刷新费用列表
function PriRoomCreateLayer:onRefreshFeeList()

    PriRoom:getInstance():dismissPopWait()
    local this = self

    local yPos = 51
    local bgHeight = 0
    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}
    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    print("nConfig:",nConfig)
    -- 局数
    for i = 1, nConfig do
        yPos = dropDownMenuYPox[nConfig - i + 1] or 51
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        dump(config,"config"..i,6)
        local checkbx = self.m_imageDropDownBg2:getChildByName("check_round_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_BEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            -- 设置位置
            checkbx:setPositionY(yPos)
            self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
        end

        local txtcount = self.m_imageDropDownBg2:getChildByName("txt_round_" .. i)
        if nil ~= txtcount then
            -- 设置位置
            txtcount:setPositionY(yPos)
            txtcount:setString(config.dwDrawCountLimit .. "局")
            txtcount:setVisible(true)
        end
    end
    -- 选择的玩法    
    self.m_nSelectIdx = CBT_BEGIN + 1
    self.m_tabSelectConfig = nil
    if nConfig > 0 then
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]
        self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)
        self.m_txtSelectRound:setString(string.format("%d局", self.m_tabSelectConfig.dwDrawCountLimit))

        -- 更新费用
        self:updateCreateFee()

        bgHeight = dropDownBgHeight[nConfig] or 0
        self.m_imageDropDownBg2:setContentSize(self.m_imageDropDownBg2:getContentSize().width, bgHeight)
    end

    -- 免费判断
    if PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        self.m_nodeNormal:getChildByName("txt_feetime"):setString(szfree)

        -- 消耗隐藏
        self.m_nodeNormal:getChildByName("txt_createfee_0"):setVisible(false)
        -- 费用隐藏
        self.m_txtFee:setVisible(false)
        -- 钻石隐藏
        self.m_nodeNormal:getChildByName("sp_diamond"):setVisible(false)
    end


    self.m_btnDropDown2:setEnabled(true)
    self.m_btnDropArrow2:setEnabled(true)


     --入场，离场分数设置默认值
    local bPriGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
    if bPriGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore
        self.m_txtCellScoreLimit:setString(self.m_cellScore)
        self.m_txtEnterLimit:setString(self.m_enterScore)
        self.m_txtExitLimit:setString(self.m_exitScore)
        self.m_nodeGold:getChildByName("Text_CellRange"):setString("底分范围："..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
    end
end



function PriRoomCreateLayer:onLoginPriRoomFinish()
    local kth=self.m_nodeNormal:isVisible() and 1 or 2
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end

      -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE 
        or meUser.cbUserStatus == yl.US_NULL 
        or meUser.cbUserStatus == yl.US_PLAYING
        or meUser.cbUserStatus == yl.US_SIT)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            -- 创建登陆
            local buffer = ExternalFun.create_netdata( ) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_cellScore)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawTimeLimit or 10)
            buffer:pushword(4)
            buffer:pushdword(0)
            -- 密码设置
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

             --游戏额外规则
            buffer:pushbyte(1) --支持额外规则
            buffer:pushbyte(4) --当前房间人数
            buffer:pushbyte(4) --允许的最大人数
          
            for i=1,2 do
                buffer:pushbyte(self.m_nSelectConfigIdx[kth][i]==0 and 0 or 1)
                cc.UserDefault:getInstance():setIntegerForKey(string.format("ConfigIdx_%d_sparrowxz", i), (self.m_nSelectConfigIdx[kth][i]==0 and 0 or 1))
            end
            for i=1,4 do
                buffer:pushbyte(self.m_nSelectFanIdx[kth][i]==0 and 0 or 1)
                cc.UserDefault:getInstance():setIntegerForKey(string.format("FanIdx_%d_sparrowxz", i), (self.m_nSelectFanIdx[kth][i]==0 and 0 or 1))
            end

            for i = 1, 91 do
                buffer:pushbyte(0)
            end

            local mode=self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0
       
            buffer:pushbyte(mode)
            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)
            print("----------------------------->", self.m_bIsNormal)
            if self.m_nOpenMode then
                buffer:pushbyte(1)
            else
                buffer:pushbyte(0)
            end
            if self.param.clubID ~= nil then
                buffer:pushdword(self.param.clubID)  --0为非俱乐部，俱乐部创建填俱乐部ID
            else
                buffer:pushdword(0)  --0为非俱乐部，俱乐部创建填俱乐部ID
            end
            buffer:pushdword(0)
            PriRoom:getInstance():getNetFrame():sendGameServerMsg(buffer)
            return true
        end        
    end
    return false
end

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local dwRoomID = roomDetailInfo.dwRoomID or 0 
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0

    local configtxt = string.format("%d局 ", turnCount)
    local text = cc.UserDefault:getInstance():getIntegerForKey("ConfigIdx_sparrowxz",0);
    print("text", text)
    if cc.UserDefault:getInstance():getIntegerForKey("ConfigIdx_1_sparrowxz",0) == 1 then
        configtxt = configtxt .. "换三张 "
    end

    if cc.UserDefault:getInstance():getIntegerForKey("ConfigIdx_2_sparrowxz",0) == 1 then
        configtxt = configtxt .. "呼叫转移 "
    end

    if cc.UserDefault:getInstance():getIntegerForKey("FanIdx_1_sparrowxz",0) == 1 then
        configtxt = configtxt .. "自摸加倍 "
    end

    if cc.UserDefault:getInstance():getIntegerForKey("FanIdx_2_sparrowxz",0) == 1 then
        configtxt = configtxt .. "天地胡 "
    end

    if cc.UserDefault:getInstance():getIntegerForKey("FanIdx_3_sparrowxz",0) == 1 then
        configtxt = configtxt .. "海底捞月 "
    end

    if cc.UserDefault:getInstance():getIntegerForKey("FanIdx_4_sparrowxz",0) == 1 then
        configtxt = configtxt .. "门清*2 "
    end

    local shareTxt = configtxt .. "。血战麻将精彩刺激, 一起来玩吧!"
    print("asdgdasdas", shareTxt)

    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=396&a=1" .. "&p=" .. passwd
    
    return {title = "【约战房间: " .. dwRoomID .. "】", content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = "血战麻将, 房号[" .. dwRoomID .. "],您的好友喊你打牌了!"}
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function PriRoomCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PriRoomCreateLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PriRoomCreateLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PriRoomCreateLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end
------
-- 继承/覆盖
------

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_spBg)
          -- -- 通知更新    
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    elseif TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
    elseif TAG_ENUM.BTN_CREATEGOLD == tag then

        if GlobalUserItem.tabAccountInfo.lUserScore < self.m_enterScore then 
            self.m_Query = QueryDialog:create("您的金币数量不足，是否前往商城充值？", function(ok)
                if ok then
                    self._scene:popShopLayer() 
                end
                self.m_Query = nil
            end, nil, QueryDialog.QUERY_SURE_CANCEL)
            :setCanTouchOutside(false)
            :addTo(self._scene)
        end

        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = false
        --私有房模式
        PriRoom:getInstance(). m_bPlayModel = 1
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()

    elseif TAG_ENUM.BTN_CREATE == tag
    or TAG_ENUM.BTN_CREATE_1 == tag then 
        if self.m_bLow and self.param.clubID == nil then
            local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
            local query = QueryDialog:create("您的钻石数量不足，是否前往商城充值！", function(ok)
                if ok == true then
                    self._scene:popShopLayer()           
                end
                query = nil
            end):setCanTouchOutside(false)
                :addTo(self._scene)
            return
        end
        if nil == self.m_tabSelectConfig or table.nums(self.m_tabSelectConfig) == 0 then
            showToast(self, "未选择玩法配置!", 2)
            return
        end

        PriRoom:getInstance(). m_bPlayModel = 0
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()

    elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_1.png")
        self.m_btnDropArrow2:loadTexturePressed("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
    elseif TAG_ENUM.BTN_CELLSCORE == tag or TAG_ENUM.BTN_ENTERCONDITION == tag or TAG_ENUM.BTN_EXITCONDITION == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, PrivateScoreInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pl)
    end
end


function PriRoomCreateLayer:onSelectedEvent(tag, sender)
    --self.m_maskDropDown2:setVisible(false)
    if self.m_nSelectIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectIdx = tag
    for k,v in pairs(self.m_tabCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[tag - CBT_BEGIN]
    if nil == self.m_tabSelectConfig then
        return
    end
    self.m_txtSelectRound:setString(string.format("%d局", self.m_tabSelectConfig.dwDrawCountLimit))

    -- 更新费用
    self:updateCreateFee()
end


function PriRoomCreateLayer:onRuleSelectedEvent(tag, sender)
    local kth=self.m_nodeNormal:isVisible() and 1 or 2
    print("==========================>", tag)
    if tag > CBT_RULEBEGIN and tag < CBT_FANXINGBEGIN then --玩法，可以多选
        local num = #self.m_nSelectConfigIdx[kth]
        local isSelect = false
        for i=1, num do
            if self.m_nSelectConfigIdx[kth][i] == tag then
                self.m_nSelectConfigIdx[kth][i] = 0
                self.m_tabRuleCheckBox[kth][tag]:setSelected(false)
                isSelect = true
            end
        end
        if not isSelect then --之前没有选
            self.m_nSelectConfigIdx[kth][tag - CBT_RULEBEGIN] = tag
            self.m_tabRuleCheckBox[kth][tag]:setSelected(true)
        end
    end

    if tag > CBT_FANXINGBEGIN and tag < CBT_FANXINGBEGIN+6 then --番型，可以多选
        local num = #self.m_nSelectFanIdx[kth]
        local isSelect = false
        for i=1, num do
            if self.m_nSelectFanIdx[kth][i] == tag then
                self.m_nSelectFanIdx[kth][i] = 0
                self.m_tabRuleCheckBox[kth][tag]:setSelected(false)
                isSelect = true
            end
        end
        if not isSelect then --之前没有选
            self.m_nSelectFanIdx[kth][tag - CBT_FANXINGBEGIN] = tag
            self.m_tabRuleCheckBox[kth][tag]:setSelected(true)
        end
    end
    
end
function PriRoomCreateLayer:onPayModeSelectedEvent( tag, sender )
    if self.m_nPayModeIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nPayModeIdx = tag
    for k,v in pairs(self.m_tabPayModeBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= sender.nPayMode then
        self.m_nSelectPayMode = sender.nPayMode
    end
    -- 更新费用
    self:updateCreateFee()
end

function PriRoomCreateLayer:onPasswdModeSelectedEvent( tag, sender )
    if self.m_nPasswdModeIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nPasswdModeIdx = tag
    for k,v in pairs(self.m_tabPasswdModeBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= sender.nPasswdMode then
        self.m_nSelectPasswd = sender.nPasswdMode
    end
end

-- function PriRoomCreateLayer:onSelectedEvent(tag, sender)
--     if self.m_nSelectIdx == tag then
--         sender:setSelected(true)
--         return
--     end
--     self.m_nSelectIdx = tag
--     for k,v in pairs(self.m_tabCheckBox) do
--         if k ~= tag then
--             v:setSelected(false)
--         end
--     end
--     self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[tag - CBT_BEGIN]
--     if nil == self.m_tabSelectConfig then
--         return
--     end

--     -- 更新费用
--     self:updateCreateFee()
-- end

function PriRoomCreateLayer:updateCreateFee()
    self.m_bLow = false
    if GlobalUserItem.tabAccountInfo.lDiamond < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end
    local fee = 0
    if self.m_nSelectPayMode == self._cmd_pri.define.tabPayMode.AA_PAY then
        -- AA付
        fee = self.m_tabSelectConfig.wAAPayFee or 0
    else
        -- 一人付
        fee = self.m_tabSelectConfig.lFeeScore or 0
    end
    self.m_txtFee:setString( "x" .. fee )
end

-- AA制界面
function PriRoomAAPayLayer:ctor( scene, param, level )
    PriRoomAAPayLayer.super.ctor( self, scene, param, level )

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomAAPayLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 进入
    btn = spbg:getChildByName("btn_entergame")
    btn:setTag(TAG_ENUM.BTN_ENTERGAME)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 房间id
    local roomid = self._param.dwRoomId or ""
    spbg:getChildByName("txt_roomid"):setString(roomid)

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString(consume .. "")

    -- 玩法
    local wanfa = spbg:getChildByName("txt_wanfa")

    -- 规则
    local guize = spbg:getChildByName("txt_guize")
    local buffer = self._param.buffer
    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读前两个规则 --支付方式和是否密码
        buffer:readbyte()
        buffer:readbyte()
      
        local tabConfigStr= {"换三张","呼叫转移"}
        local tabRuleStr= {"自摸加倍","天地胡","海底捞月","门清*2"}
        local configS={}
        local ruleS={}
        for i=1,2 do
            if 1==buffer:readbyte() then
                table.insert(configS,tabConfigStr[i])
            end
        end

        for i=1,4 do
            if 1==buffer:readbyte() then
                table.insert(ruleS,tabRuleStr[i])
            end
        end

        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(table.concat(configS,"、"))
        -- 规则
        spbg:getChildByName("txt_guize"):setString(table.concat(ruleS,"、"))
    end

    -- 局数
    local ncount = self._param.dwDrawCountLimit or 0
    spbg:getChildByName("txt_jushu"):setString(ncount .. "局")

    self:scaleTransitionIn(spbg)
end

function PriRoomAAPayLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        -- 断开
        PriRoom:getInstance():closeGameSocket()

        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_ENTERGAME then
        print("self userid ", GlobalUserItem.tabAccountInfo.dwUserID ~= self._param.dwRommerID)
        -- 判断是否密码, 且非房主
        if self._param.bRoomPwd and GlobalUserItem.tabAccountInfo.dwUserID ~= self._param.dwRommerID then
            PriRoom:getInstance():passwdInput()
        else
            PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():getNetFrame():sendEnterPrivateGame()
        end
    end
end

function PriRoomAAPayLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PriRoomAAPayLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PriRoomAAPayLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function PriRoomAAPayLayer:onTransitionOutFinish()
    self:removeFromParent()
end

return PriRoomCreateLayer