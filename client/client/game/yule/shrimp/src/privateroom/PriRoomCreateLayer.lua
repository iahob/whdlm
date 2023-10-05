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

    "MENU_DROPDOWN",      -- 下拉菜单2
    "MASK_DROPDOWN",      -- 下拉背景2

    "GAMEPLAY_1",
    "GAMEPLAY_2",

    "CAP_CHECKBX_1",
    "CAP_CHECKBX_2",
    "CAP_CHECKBX_3",

    "CAP_TEXT_1",
    "CAP_TEXT_2",
    "CAP_TEXT_3",

    "BTN_CREATEGOLD",
    "BTN_CELLSCORE",
    "BTN_ENTERCONDITION",
    "BTN_EXITCONDITION",
    "CBT_SETTING",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_RULEBEGIN = 200
local CBT_BEGIN = 400

local dropDownMenuYPox = {30.00, 76.50, 123.00, 169.50, 216}
local dropDownBgHeight = {60.00, 105.00, 153.00, 200.00, 249}

-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    print(debug.traceback())
    -- os.execute("pause")
    
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode
    self.param = param

        --是否允许陌生人加入
    self.m_nOpenMode = true

    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0
    self.capNum = 0

    self.m_bIsNormal = self.param.cbGameMode or false
    self:initNormal(csbNode)
    self:initGold(csbNode)
    if self.m_bIsNormal then 
        self.panel_normal:setVisible(true)
        self.panel_gold:setVisible(false)
    else
        self.panel_normal:setVisible(false)
        self.panel_gold:setVisible(true)
    end

end

function PriRoomCreateLayer:initNormal(csbNode)
    local this = self
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg_2")
    self.m_spBg = spbg
    self.panel_normal = self.m_spBg:getChildByName("Panel_normal")
    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        this:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    local checkbx = self.panel_normal:getChildByName("check_consume_1")
    checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = self.panel_normal:getChildByName("check_consume_2")
    checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx

    --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then
            
        elseif 1 == self.param.cbPayTypeMask then  --AA支付
            self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY]:setVisible(false)
            local txt_consume_2 = self.panel_normal:getChildByName("txt_consume_2")
            txt_consume_2:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = self.panel_normal:getChildByName("txt_consume_1")
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
    checkbx = self.panel_normal:getChildByName("check_passwdmode_1")
    checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx
    -- 不需要密码
    checkbx = self.panel_normal:getChildByName("check_passwdmode_2")
    checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

    -- 立即创建
    btn = self.panel_normal:getChildByName("btn_createroom")
    btn:setTag(TAG_ENUM.BTN_CREATE)
    btn:addTouchEventListener(btncallback)

    -- 下拉菜单2
    btn = self.panel_normal:getChildByName("btn_dropdown_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown2 = btn
    -- 下拉箭头2
    btn = self.panel_normal:getChildByName("btn_dropdown_arrow_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow2 = btn
    -- 下拉背景2
    panel = self.panel_normal:getChildByName("dropdown_panel_2")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown2 = panel
    -- 菜单背景2
    self.m_imageDropDownBg2 = panel:getChildByName("dropdown_2")

    -- 选择局数
    self.m_txtSelectRound = self.panel_normal:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    -- 创建费用
    self.m_txtFee = self.panel_normal:getChildByName("txt_createfee")
    self.m_txtFee:setString("")

    -- 玩法
    local gamePlaylistener = function (sender,eventType)
        this:onGamePlaySelectedEvent(sender:getTag(),sender)
    end
    self.m_tagGamePlayBox = {}
    self.m_tagGamePlayIndex = TAG_ENUM.GAMEPLAY_1
    checkbx = self.panel_normal:getChildByName("check_passwdmode_2_0_0")
    checkbx:setTag(TAG_ENUM.GAMEPLAY_1)
    checkbx:addEventListener(gamePlaylistener)
    checkbx:setSelected(true)
    checkbx:setEnabled(false)
    self.m_tagGamePlayBox[TAG_ENUM.GAMEPLAY_1] = checkbx

    checkbx = self.panel_normal:getChildByName("check_passwdmode_1_0_0")
    checkbx:setTag(TAG_ENUM.GAMEPLAY_2)
    checkbx:addEventListener(gamePlaylistener)
    checkbx:setSelected(false)
    checkbx:setEnabled(false)
    self.m_tagGamePlayBox[TAG_ENUM.GAMEPLAY_2] = checkbx


    -- 封顶
    local capboxlistener = function (sender,eventType)
        this:onCapBoxSelectedEvent(sender:getTag(),sender)
    end

    self.m_tabCapBox = {}
    self.m_capBoxIndex = TAG_ENUM.CAP_CHECKBX_1
    checkbx = self.panel_normal:getChildByName("check_passwdmode_2_0")
    checkbx:setTag(TAG_ENUM.CAP_CHECKBX_1)
    checkbx:addEventListener(capboxlistener)
    checkbx:setSelected(true)
    checkbx:setEnabled(false)
    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_1] = checkbx

    checkbx = self.panel_normal:getChildByName("check_passwdmode_1_0")
    checkbx:setTag(TAG_ENUM.CAP_CHECKBX_2)
    checkbx:addEventListener(capboxlistener)
    checkbx:setSelected(false)
    checkbx:setEnabled(false)
    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_2] = checkbx

    checkbx = self.panel_normal:getChildByName("check_passwdmode_1_0_1")
    checkbx:setTag(TAG_ENUM.CAP_CHECKBX_3)
    checkbx:addEventListener(capboxlistener)
    checkbx:setSelected(false)
    checkbx:setEnabled(false)
    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_3] = checkbx

    self.m_tabCapText = {}
    local text = self.panel_normal:getChildByName("txt_passwdmode_2_0")
    text:setTag(TAG_ENUM.CAP_TEXT_1)
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_1] = text

    text = self.panel_normal:getChildByName("txt_passwdmode_1_0")
    text:setTag(TAG_ENUM.CAP_TEXT_2)
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_2] = text

    text = self.panel_normal:getChildByName("txt_passwdmode_1_0_1")
    text:setTag(TAG_ENUM.CAP_TEXT_3)
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_3] = text

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self.panel_normal)
end

function PriRoomCreateLayer:initGold(csbNode )
    -- body
    local spbg = csbNode:getChildByName("sp_bg_2")
    self.panel_gold = spbg:getChildByName("Panel_gold")
    local this = self
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 创建房间
    btn = self.panel_gold:getChildByName("btn_createroom_1")
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)
    self.create_gold_room = btn
    btn:setEnabled(false)
    btn:setPositionX(627)

    -- 低分
    local btn = self.panel_gold:getChildByName("btn_cell")
    btn:setTag(TAG_ENUM.BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)
    self.m_txtCellScoreLimit = btn:getChildByName("txt_tip1")
    self.m_cellScoreLimit = btn:getChildByName("txt_tip1_0")
    -- self.m_txtCellScoreLimit:setString("123456")

    -- 入场分数
    local btn = self.panel_gold:getChildByName("btn_enter")
    btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)
    self.m_txtEnterLimit = btn:getChildByName("txt_tip2")

    -- 离场分数
    local btn = self.panel_gold:getChildByName("btn_exit")
    btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)
    self.m_txtExitLimit = btn:getChildByName("txt_tip3")

    -- 是否应许私人房
    local btn = self.panel_gold:getChildByName("check_rule_add")
    btn:setTag(TAG_ENUM.CBT_SETTING)
    btn:addTouchEventListener(btncallback)
    btn:setSelected(true)

    local limitlistener = function ( sender,eventType )
        local tag = sender:getTag()
        for k,v in pairs(self.m_limit_gold) do
            if v:getTag() == tag then
                v:setSelected(true)
            else
                v:setSelected(false)
            end
        end

        if  self.capData and #self.capData > 0 then 
            self.capNum = self.capData[tag]
        end

    end

    self.m_limit_gold = {}
    self.m_limit_gold_txt = {}
    for i=1,3 do
        local checkbox = self.panel_gold:getChildByName("check_limit_"..i)
        self.m_limit_gold[i] = checkbox
        checkbox:setTag(i)
        checkbox:addEventListener(limitlistener)
        checkbox:setSelected(false)

        local txt = checkbox:getChildByName("txt_rule_1")
        self.m_limit_gold_txt[i] = txt
    end
    self.m_limit_gold[1]:setSelected(true)

    -- 房间玩法
    local outlistener = function (sender,eventType)
        local tag = sender:getTag()
        for k,v in pairs(self.m_gold_game_mode) do
            if v:getTag() == tag then
                v:setSelected(true)
            else
                v:setSelected(false)
            end
        end
        self.m_tagGamePlayIndex = tag

        if self.cmd_table then 
            if tag == TAG_ENUM.GAMEPLAY_1 then 
                self.capData = self.cmd_table.lDoubleSiceAreLimit[1]
            elseif tag == TAG_ENUM.GAMEPLAY_2 then 
                self.capData = self.cmd_table.lThreeSiceAreLimit[1]
            end

            self.m_limit_gold_txt[1]:setString(self.capData[1].."封顶")
            self.m_limit_gold_txt[2]:setString(self.capData[2].."封顶")
            self.m_limit_gold_txt[3]:setString(self.capData[3].."封顶")

            self.capNum = self.capData[tag - TAG_ENUM.GAMEPLAY_1 + 1]
        end
    end

    self.m_gold_game_mode = {}
    for i=1,2 do
        local checkbox = self.panel_gold:getChildByName("check_rule_"..i)
        if checkbox ~= nil then
            self.m_gold_game_mode[i] = checkbox
            checkbox:setTag(TAG_ENUM.GAMEPLAY_1+(i - 1))
            checkbox:addEventListener(outlistener)
            checkbox:setSelected(false)
        end
    end 
    self.m_gold_game_mode[1]:setSelected(true)  
    self.m_tagGamePlayIndex = TAG_ENUM.GAMEPLAY_1

end
------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    --self.m_txtCardNum:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
end

function PriRoomCreateLayer:onRefreshRule()
    local tag =
    {
        {t = "score", k = "lDoubleSiceAreLimit", l = {3}},
        {t = "score", k = "lThreeSiceAreLimit",  l = {3}},
        {t = "int", k = "iDoubleSiceFee",  l = {2}},
        {t = "int", k = "iThreeSiceFee",  l = {2}},
    }

    local cmd_table1 = ExternalFun.read_netdata(tag, PriRoom:getInstance().m_tabRule) --积分房配置

    local lastLen = 1024 - PriRoom:getInstance().m_tabRule:getcurlen() + self._cmd_pri.define.LEN_PERSONAL_OTHER

    ExternalFun.read_netdata({{ k = "emptyType",t = "byte",l = {lastLen}}}, PriRoom:getInstance().m_tabRule)--去掉中间多余字节

    local cmd_table2 = ExternalFun.read_netdata(tag, PriRoom:getInstance().m_tabRule)--金币房配置

    local cmd_table = self.m_bIsNormal and clone(cmd_table1) or clone(cmd_table2)

    --local cmd_table = ExternalFun.read_netdata( tag, PriRoom:getInstance().m_tabRule )
    self.cmd_table = cmd_table
    dump(self.cmd_table,"cmd_table=============",12)
 
    local data = self.cmd_table.lDoubleSiceAreLimit[1]
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_1]:setString(data[1].."封顶")
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_2]:setString(data[2].."封顶")
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_3]:setString(data[3].."封顶")

    self.m_limit_gold_txt[1]:setString(data[1].."封顶")
    self.m_limit_gold_txt[2]:setString(data[2].."封顶")
    self.m_limit_gold_txt[3]:setString(data[3].."封顶")
    
    self.capNum = data[1]
    self.capData = data

    self.m_tagGamePlayBox[TAG_ENUM.GAMEPLAY_1]:setEnabled(true)
    self.m_tagGamePlayBox[TAG_ENUM.GAMEPLAY_2]:setEnabled(true)

    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_1]:setEnabled(true)
    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_2]:setEnabled(true)
    self.m_tabCapBox[TAG_ENUM.CAP_CHECKBX_3]:setEnabled(true)


    self.create_gold_room:setEnabled(true)

end

--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)
    print("tag  ===> ",tag)
    if TAG_ENUM.BTN_CELLSCORE == tag then
        print("BTN_CELLSCORE  ===> ")
        --设置了底分，入场和离场也会变化
        dump(PriRoom:getInstance().m_tabGoldRoomOption,"PriRoom:getInstance().m_tabGoldRoomOption",12)

        self.m_cellScore = scoere
        if self.m_cellScore > PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore
        elseif self.m_cellScore < PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        end

        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore

        self.m_txtCellScoreLimit:setString(self.m_cellScore)
        self.m_txtEnterLimit:setString(self.m_enterScore)
        self.m_txtExitLimit:setString(self.m_exitScore)

    elseif TAG_ENUM.BTN_ENTERCONDITION == tag then
        --入场设定要判断底分,大于最小值才生效
        print("BTN_ENTERCONDITION  ===> ")
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
        print("BTN_ENTERCONDITION  ===> ")
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

-- 刷新费用列表
function PriRoomCreateLayer:onRefreshFeeList()
    PriRoom:getInstance():dismissPopWait()

    dump(PriRoom:getInstance().m_tabFeeConfigList,"PriRoom:getInstance().m_tabFeeConfigList",12)
    local this = self

    local yPos = 51
    local bgHeight = 0
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

    self.m_btnDropDown2:setEnabled(true)
    self.m_btnDropArrow2:setEnabled(true)

    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)

    dump(PriRoom:getInstance().m_tabGoldRoomOption,"金币约战配置",12)

    if bPirGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore
        -- self.m_nSelectScore[2] = self.m_cellScore

        if nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
            self.m_txtCellScoreLimit:setString(self.m_cellScore)
            self.m_txtEnterLimit:setString(self.m_enterScore)
            self.m_txtExitLimit:setString(self.m_exitScore)
        end

        self.m_cellScoreLimit:setString("底分范围："..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)

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
            local buffer = ExternalFun.create_netdata(124) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_cellScore)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(3)
            buffer:pushdword(0)
            -- 密码设置@@
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

            -- 附加信息
            buffer:pushbyte(1)
            -- 附加分数
            buffer:pushbyte(11)
            buffer:pushbyte(11)

            local gameMode = self.m_tagGamePlayIndex == TAG_ENUM.GAMEPLAY_1 and 1 or 0
            buffer:pushbyte(gameMode)
            buffer:pushbyte(self.capNum/1000)
            buffer:pushbyte(self.m_tabSelectConfig.dwDrawCountLimit)
            --记录数据
            -- cc.UserDefault:getInstance():setIntegerForKey("land_baseScore", self.m_nSelectScore)

            for i = 1, 94 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig.cbGameMode)

            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)

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
            dump(self.param,"self.param",12)
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

end

function PriRoomCreateLayer:onExit()
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

function PriRoomCreateLayer:onGamePlaySelectedEvent(tag, sender)

    if self.m_tagGamePlayIndex == tag then
        sender:setSelected(true)
    end

    self.m_tagGamePlayIndex = tag
    for k,v in pairs(self.m_tagGamePlayBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end

    if tag == TAG_ENUM.GAMEPLAY_1 then 
        self.capData = self.cmd_table.lDoubleSiceAreLimit[1]
    elseif tag == TAG_ENUM.GAMEPLAY_2 then 
        self.capData = self.cmd_table.lThreeSiceAreLimit[1]
    end

    self.m_tabCapText[TAG_ENUM.CAP_TEXT_1]:setString(self.capData[1].."封顶")
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_2]:setString(self.capData[2].."封顶")
    self.m_tabCapText[TAG_ENUM.CAP_TEXT_3]:setString(self.capData[3].."封顶")

end

function PriRoomCreateLayer:onCapBoxSelectedEvent(tag, sender)
    -- body

    if self.m_capBoxIndex == tag then
        sender:setSelected(true)
    end
    self.m_capBoxIndex = tag
    for k,v in pairs(self.m_tabCapBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end

    if tag == TAG_ENUM.CAP_CHECKBX_1 then 
        self.capNum = self.capData[1]
    elseif tag == TAG_ENUM.CAP_CHECKBX_2 then 
        self.capNum = self.capData[2]
    elseif tag == TAG_ENUM.CAP_CHECKBX_3 then
        self.capNum = self.capData[3] 
    end

    print("self.capNum = ",self.capNum)

end

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
    elseif TAG_ENUM.BTN_CREATE == tag
    or TAG_ENUM.BTN_CREATE_1 == tag then 
        print("self.param.cbPayTypeMask",self.param.cbPayTypeMask)
        if self.m_bLow and self.param.cbPayTypeMask ~= 1 then
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
        -- if nil == self.m_nSelectScore or table.nums(self.m_scoreList) == 0 then
        --     showToast(self, "未选择游戏底分!", 2)
        --     return
        -- end
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()

    elseif TAG_ENUM.MENU_DROPDOWN == tag then
        self.m_maskDropDown2:setVisible(true)
        -- 更新箭头
        -- self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        -- self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        -- self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN == tag then
        self.m_maskDropDown2:setVisible(false)
        -- 更新箭头
        -- self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        -- self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        -- self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_1.png")

    elseif TAG_ENUM.BTN_CREATEGOLD == tag then 
        -- 创建房间
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

    elseif TAG_ENUM.BTN_CELLSCORE == tag or TAG_ENUM.BTN_ENTERCONDITION == tag or TAG_ENUM.BTN_EXITCONDITION == tag then 
        -- 低分，入场，离场分数
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, self._scene:getPopLevel())
        self._scene:addPopLayer(pl)
    elseif TAG_ENUM.CBT_SETTING == tag then 
        -- 是否应许私人房
        self.m_nOpenMode = not  self.m_nOpenMode
        sender:setSelected(self.m_nOpenMode  and true or false)
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
    print("sender.nPasswdMode = ",sender.nPasswdMode)
    if nil ~= sender.nPasswdMode then

        self.m_nSelectPasswd = sender.nPasswdMode
    end
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

    if not self.m_bIsNormal then
        return
    end
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

    spbg:getChildByName("txt_wanfa"):setVisible(false)
    spbg:getChildByName("txt_1"):setVisible(false)

    -- 房间id
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
        local lscore = buffer:readbyte()
        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(lscore .. "")
        spbg:getChildByName("txt_wanfa"):setVisible(false)
    end
    self:noActionTransitionIn(spbg)
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
            print("PriRoomAAPayLayer:onButtonClickedEvent ~~")

            PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
            PriRoom:getInstance():showPopWait()
            -- PriRoom:getInstance():closeGameSocket()
            PriRoom:getInstance():getNetFrame():sendEnterPrivateGame()
        end
        self:scaleTransitionOut(self.m_spBg)
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