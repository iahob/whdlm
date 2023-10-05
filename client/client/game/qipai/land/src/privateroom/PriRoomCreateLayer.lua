--
-- Author: zhong
-- Date: 2016-12-17 14:07:02
--
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

-- 斗地主私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- 斗地主AA制提示界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_HELP",             -- 帮助
    "BTN_CHARGE",           -- 充值
    "BTN_MYROOM",           -- 自己房间
    "BTN_CREATE",           -- 立即开局
    "BTN_CREATE_1",         -- 代人开房
    "BTN_ENTERGAME",        -- 进入游戏
    "CBT_ONE_PAY",          -- 一人支付
    "CBT_AA_PAY",           -- AA

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    "MENU_DROPDOWN_1",      -- 下拉菜单1
    "MASK_DROPDOWN_1",      -- 下拉背景1
    "MENU_DROPDOWN_2",      -- 下拉菜单2
    "MASK_DROPDOWN_2",      -- 下拉背景2

    "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式
    "BTN_CELLSCORE",                -- 底注
    "BTN_ENTERCONDITION",           -- 入场设定
    "BTN_EXITCONDITION",            -- 离场设定
    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_RULEBEGIN = 200
local CBT_BEGIN = 400

local dropDownMenuYPox = {20, 60, 100, 140, 180}
local dropDownBgHeight = {40, 80, 120, 160, 200}

-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    local this = self
    PriRoomCreateLayer.super.ctor(self, scene, param, level)

    self.param = param

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 遮罩
    -- local mask = csbNode:getChildByName("panel_mask")
    -- mask:setTag(TAG_ENUM.TAG_MASK)
    -- mask:addTouchEventListener( btncallback )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    --self.m_spBg = spbg

    -- 关闭
    -- local btn = spbg:getChildByName("btn_close")
    -- btn:setTag(TAG_ENUM.BTN_CLOSE)
    -- btn:addTouchEventListener(btncallback)

    --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    -- self.m_btnScoreType = spbg:getChildByName("ckBox_score")
    -- self.m_btnScoreType:setTag(TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnScoreType:addEventListener(gamemodelistener)
    -- local txtScoreType = self.m_btnScoreType:getChildByName("txt")
    -- txtScoreType:setColor(cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    --self.m_btnScoreType:setVisible(false)
    
    -- self.m_btnGoldType = spbg:getChildByName("ckBox_gold")
    -- self.m_btnGoldType:setTag(TAG_ENUM.CBT_GOLDMODE)
    -- self.m_btnGoldType:addEventListener(gamemodelistener)
    
    

    --self.m_btnGoldType:setVisible(false)

    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0

-----------------------------------------------------------------------------------------------------
                                    --积分房卡--
--------------------------------------------------------------------------------------------------------
    --默认是积分房卡
    self.m_bIsNormal = false --true 为积分房卡，false为金币房卡
    local nodeNormal = spbg:getChildByName("node_normal")
    self.m_spBg = nodeNormal

    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        this:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    local checkbx = nodeNormal:getChildByName("check_consume_1")
    checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = nodeNormal:getChildByName("check_consume_2")
    checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx

    --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then

        elseif 1 == self.param.cbPayTypeMask then  --AA支付
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
        this:onPasswdModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPasswdModeBox = {}
    -- 需要密码
    checkbx = nodeNormal:getChildByName("check_passwdmode_1")
    checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx
    -- 不需要密码
    checkbx = nodeNormal:getChildByName("check_passwdmode_2")
    checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

    -- 立即创建
    -- btn = nodeNormal:getChildByName("btn_createroom")
    -- btn:setTag(TAG_ENUM.BTN_CREATE)
    -- btn:addTouchEventListener(btncallback)

    -- 创建房间（代人开房）
    btn = nodeNormal:getChildByName("btn_createroom")
    -- btn:setTag(TAG_ENUM.BTN_CREATE_1)
    -- btn:addTouchEventListener(btncallback)
     btn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            btn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE_1,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            btn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then
            if btn:getNumberOfRunningActions() >0 then
                btn:stopAllActions()
                self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE,ref)
            end
        end
    end)

    -- 必须参与
    if nil~= PriRoom:getInstance().m_tabRoomOption and 1 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame then
        btn:setEnabled(false)
        btn:setOpacity(200)
    end

    -- 下拉菜单1
    btn = nodeNormal:getChildByName("btn_dropdown_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown1 = btn
    -- 下拉箭头1
    btn = nodeNormal:getChildByName("btn_dropdown_arrow_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow1 = btn
    -- 下拉背景1
    local panel = nodeNormal:getChildByName("dropdown_panel_1")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_1)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown1 = panel
    -- 菜单背景1
    self.m_imageDropDownBg1 = panel:getChildByName("dropdown_1")

    -- 下拉菜单2
    btn = nodeNormal:getChildByName("btn_dropdown_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown2 = btn
    -- 下拉箭头2
    btn = nodeNormal:getChildByName("btn_dropdown_arrow_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow2 = btn
    -- 下拉背景2
    panel = nodeNormal:getChildByName("dropdown_panel_2")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_2)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown2 = panel
    -- 菜单背景2
    self.m_imageDropDownBg2 = panel:getChildByName("dropdown_2")

    -- 选择规则
    self.m_txtSelectRule = nodeNormal:getChildByName("txt_selectrule")
    self.m_txtSelectRule:setString("")

    -- 选择局数
    self.m_txtSelectRound = nodeNormal:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    -- 创建费用
    self.m_txtFee = self.m_spBg:getChildByName("txt_createfee")
    self.m_txtFee:setString("")


-----------------------------------------------------------------------------------------------------
                                    --金币房卡--
--------------------------------------------------------------------------------------------------------
    local nodeGold = spbg:getChildByName("node_gold")
    self.m_nodeGold = nodeGold

    local btn = nodeGold:getChildByName("btn_cellscore")
    btn:setTag(TAG_ENUM.BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)

     local btn = nodeGold:getChildByName("btn_entercondition")
    btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = nodeGold:getChildByName("btn_exitcondition")
    btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = nodeGold:getChildByName("btn_createroom")
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)

        -- 是否密码
    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    -- self.m_nGoldPasswdModeIdx = TAG_ENUM.CBT_NOPASSWD
    -- local Goldpasswdmodelistener = function (sender,eventType)
    --     this:onGoldPasswdModeSelectedEvent(sender:getTag(),sender)
    -- end
    -- self.m_tabGoldPasswdModeBox = {}
    -- -- 需要密码
    -- checkbx = nodeGold:getChildByName("check_passwdmode_1")
    -- checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    -- checkbx:addEventListener(Goldpasswdmodelistener)
    -- checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    -- self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx
    -- -- 不需要密码
    -- checkbx = nodeGold:getChildByName("check_passwdmode_2")
    -- checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    -- checkbx:addEventListener(Goldpasswdmodelistener)
    -- checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    -- checkbx:setSelected(true)
    -- self.m_tabGoldPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

    --是否允许陌生人加入
    self.m_nOpenMode = true
    self.m_bIsNormal = self.param.cbGameMode or false  -- true 为积分房卡
    local openmodelistener = function (sender,eventType)
        this:onOpenModeSelectedEvent(sender:getTag(),sender)
    end
    local checkbx = nodeGold:getChildByName("check_set")
    checkbx:setTag(TAG_ENUM.CBT_SETTING)
    checkbx:setSelected(true)
    checkbx:addEventListener(openmodelistener)

    self.m_txtCellScoreLimit = self.m_nodeGold:getChildByName("txt_tips1")
    self.m_txtEnterLimit = self.m_nodeGold:getChildByName("txt_tips2")
    self.m_txtExitLimit = self.m_nodeGold:getChildByName("txt_tips3")
    self.m_cellScoreLimit = self.m_nodeGold:getChildByName("Text_Limit")

    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%", bPirGoldMode, bPirScoreMode, GlobalUserItem.nCurGameKind)

    --通过属性判断界面显示那些创建的支持
    if not bPirScoreMode then  --不支持积分房卡
        -- self.m_btnScoreType:setVisible(false)
        -- self.m_btnGoldType:setVisible(false)
        self.m_spBg:setVisible(false)
        self.m_bIsNormal = false
    elseif not bPirGoldMode then  --不支持积金币房卡
        -- self.m_btnScoreType:setVisible(false)
        -- self.m_btnGoldType:setVisible(false)
        self.m_nodeGold:setVisible(false)
        self.m_bIsNormal = true
    end

    self.m_spBg:setVisible(self.m_bIsNormal)
    self.m_nodeGold:setVisible(not self.m_bIsNormal)



    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    -- self:scaleTransitionIn(self)
    self:noActionTransitionIn(self)
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    --self.m_txtCardNum:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
end

-- 刷新费用列表
function PriRoomCreateLayer:onRefreshFeeList()
    PriRoom:getInstance():dismissPopWait()
    local this = self

    self.m_tabScoreList = {}
    local scoreList = clone(PriRoom:getInstance().m_tabCellScoreList)
    if type(scoreList) ~= "table" then
        scoreList = {}
    end
    dump(scoreList, "斗地主底分配置", 10)
    dump(PriRoom:getInstance().m_tabFeeConfigList, "玩法信息", 10)
    local nScoreList = 0--#scoreList
    if 0 == nScoreList then
        scoreList = {10, 20, 30, 40, 50}
        nScoreList = 5
    end
    self.m_scoreList = scoreList
    -- 规则选择
    local rulelistener = function (sender,eventType)
        this:onSelectedScoreEvent(sender:getTag(),sender)
    end
    local yPos = 51
    local bgHeight = 0
    for i = 1, nScoreList do 
        -- y轴位置
        yPos = dropDownMenuYPox[nScoreList - i + 1] or 51

        local score = scoreList[i] or 0
        local checkbx = self.m_imageDropDownBg1:getChildByName("check_rule_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_RULEBEGIN + i)
            checkbx:addEventListener(rulelistener)
            -- 设置位置
            checkbx:setPositionY(yPos)
            self.m_tabScoreList[CBT_RULEBEGIN + i] = checkbx
        end

        local txtScore = self.m_imageDropDownBg1:getChildByName("txt_rule_" .. i)
        if nil ~= txtScore then
            -- 设置底分
            txtScore:setString(score .. "分")
            txtScore:setVisible(true)
            -- 设置位置
            txtScore:setPositionY(yPos)
        end
        if score == 0 then
            checkbx:setVisible(false)
            txtScore:setVisible(false)
        end
    end
    
    -- 默认选择底分  
    if nScoreList > 0 then
        self.m_nSelectScoreIdx = CBT_RULEBEGIN + 1
        self.m_tabScoreList[self.m_nSelectScoreIdx]:setSelected(true)
        self.m_nSelectScore = scoreList[1]

        self.m_txtSelectRule:setString(string.format("%d分", self.m_nSelectScore))

        bgHeight = dropDownBgHeight[nScoreList] or 0
        self.m_imageDropDownBg1:setContentSize(self.m_imageDropDownBg1:getContentSize().width, bgHeight)
    end

    yPos = 51
    bgHeight = 0
    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}

    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    -- 局数
    for i = 1, nConfig do
        yPos = dropDownMenuYPox[nConfig - i + 1] or 51
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
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
        self.m_spBg:getChildByName("txt_feetime"):setString(szfree)

        -- 消耗隐藏
        self.m_spBg:getChildByName("txt_createfee_0"):setVisible(false)
        -- 费用隐藏
        self.m_txtFee:setVisible(false)
        -- 钻石隐藏
        self.m_spBg:getChildByName("sp_diamond"):setVisible(false)
    end

    -- 激活按钮
    self.m_btnDropDown1:setEnabled(true)
    self.m_btnDropArrow1:setEnabled(true)
    self.m_btnDropDown2:setEnabled(true)
    self.m_btnDropArrow2:setEnabled(true)

    --入场，离场分数设置默认值
    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
    if bPirGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore
        self.m_nSelectScore = self.m_cellScore

        if nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
            self.m_txtCellScoreLimit:setString(self.m_cellScore)
            self.m_txtEnterLimit:setString(self.m_enterScore)
            self.m_txtExitLimit:setString(self.m_exitScore)
        end
        self.m_cellScoreLimit:setString("底分范围:"..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
    end
end

function PriRoomCreateLayer:onLoginPriRoomFinish()
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end

    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE 
        or meUser.cbUserStatus == yl.US_NULL 
        or meUser.cbUserStatus == yl.US_PLAYING
        or meUser.cbUserStatus == yl.US_SIT)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            -- 创建登陆
            local buffer = ExternalFun.create_netdata(126) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_nSelectScore)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawTimeLimit or 10)
            buffer:pushword(3)
            buffer:pushdword(0)
            -- 密码设置
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

            -- 附加信息
            buffer:pushbyte(1)
            buffer:pushbyte(3)
            buffer:pushbyte(3)
            -- 附加分数
            buffer:pushbyte(self.m_nSelectScore)
            print("创建时设置的底分----------------------------->", self.m_nSelectScore)
            --记录数据
            cc.UserDefault:getInstance():setIntegerForKey("land_baseScore", self.m_nSelectScore)

            for i = 1, 96 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)
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
    local baseScore = "底分:"..cc.UserDefault:getInstance():getIntegerForKey("land_baseScore",0)..","

    local shareTxt = "局数:" .. turnCount .. "局。"..baseScore.."斗地主游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=200&a=1" .. "&p=" .. passwd
    print("获取分享信息===》", dwRoomID,shareTxt,  url)
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("斗地主, 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function PriRoomCreateLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PriRoomCreateLayer:onTransitionOutBegin()
    self:sendHideEvent()
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
    elseif TAG_ENUM.BTN_CREATE == tag
    or TAG_ENUM.BTN_CREATE_1 == tag then 
        if self.m_bLow and self.param.clubID == nil then --俱乐部创房不判断金币（为空为普通开房）
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
        if nil == self.m_nSelectScore or table.nums(self.m_scoreList) == 0 then
            showToast(self, "未选择游戏底分!", 2)
            return
        end
        PriRoom:getInstance():setViewFrame(self)
        print(TAG_ENUM.BTN_CREATE_1 == tag)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        --私有房模式
        PriRoom:getInstance(). m_bPlayModel = 0
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    elseif TAG_ENUM.MENU_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.BTN_TYPE == tag then
        if self.m_bIsNormal then
            self.m_bIsNormal = false
            --设置按钮状态
            -- self.m_btnType:loadTextureDisabled("private/pri_btn_gold.png")
            -- self.m_btnType:loadTexturePressed("private/pri_btn_score.png")
            -- self.m_btnType:loadTextureNormal("private/pri_btn_gold.png")

            self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
            self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
        else
            self.m_bIsNormal = true
            --设置按钮状态
            -- self.m_btnType:loadTextureDisabled("private/pri_btn_score.png")
            -- self.m_btnType:loadTexturePressed("private/pri_btn_gold.png")
            -- self.m_btnType:loadTextureNormal("private/pri_btn_score.png")
        end
        self.m_nodeGold:setVisible(not self.m_bIsNormal)
        self.m_spBg:setVisible(self.m_bIsNormal)
    elseif TAG_ENUM.BTN_CELLSCORE == tag or TAG_ENUM.BTN_ENTERCONDITION == tag or TAG_ENUM.BTN_EXITCONDITION == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, PrivateScoreInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pl)
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
    end
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


function PriRoomCreateLayer:onSelectedScoreEvent(tag, sender)
    if self.m_nSelectScoreIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectScoreIdx = tag
    for k,v in pairs(self.m_tabScoreList) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self.m_nSelectScore = self.m_scoreList[self.m_nSelectScoreIdx - CBT_RULEBEGIN]
    self.m_txtSelectRule:setString(string.format("%d分", self.m_nSelectScore))
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


function PriRoomCreateLayer:onGoldPasswdModeSelectedEvent( tag, sender )
    if self.m_nGoldPasswdModeIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nGoldPasswdModeIdx = tag
    for k,v in pairs(self.m_tabGoldPasswdModeBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= sender.nPasswdMode then
        self.m_nGoldSelectPasswd = sender.nPasswdMode
    end
end

function PriRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    -- self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)
    local color = (tag == TAG_ENUM.CBT_SCOREMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    -- local txtScoreType = self.m_btnScoreType:getChildByName("txt")
    -- txtScoreType:setColor(color)
    local color1 = (tag == TAG_ENUM.CBT_GOLDMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    local txtGoldType = self.m_btnGoldType:getChildByName("txt")
    txtGoldType:setColor(color1)
    if tag == TAG_ENUM.CBT_SCOREMODE then
        self.m_nSelectScore = self.m_scoreList[self.m_nSelectScoreIdx - CBT_RULEBEGIN]
    end

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_spBg:setVisible(self.m_bIsNormal)
end



function PriRoomCreateLayer:onSelectedEvent(tag, sender)
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
    dump(self._param,"@@@@@@@@@@@@@@@@@@@@@@@@@", 1)
    local roomid = self._param.dwRoomId or 0
    spbg:getChildByName("txt_roomid"):setString(string.format("%06d", roomid))

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString(consume .. "")

    -- 局数
    local ncount = self._param.dwDrawCountLimit or 0
    spbg:getChildByName("txt_jushu"):setString(ncount .. "局")
    local buffer = self._param.buffer

    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读底分
        --print("---------------------->", buffer:readbyte(), buffer:readbyte())
        buffer:readbyte()
        buffer:readbyte()
        local lscore = buffer:readbyte()
        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(lscore .. "")
    end
    self:scaleTransitionIn(spbg)
end

function PriRoomAAPayLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        -- 断开
        PriRoom:getInstance():closeGameSocket()

        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_ENTERGAME then
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