--
-- Author: David
-- Date: 2017-8-11 14:07:02
--
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- 斗地主私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)

-- 广东麻将AA制提示界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
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
    "CBT_AA_PAY",           -- AA--

    -- 玩法配置
    "CBT_HAVEZICARD",         -- 带风
    "CBT_NOMAGICDOUBLE",      -- 无鬼翻倍
    "CBT_QIANGGANGHU",        -- 抢杠胡
    "CBT_HUQIDUI",            -- 胡七对

    -- 鬼牌
    "CBT_MAGIC1",          -- 无鬼
    "CBT_MAGIC2",          -- 白板鬼
    "CBT_MAGIC3",          -- 一鬼
    "CBT_MAGIC4",          -- 双鬼

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    --码数
    "CBT_MANUM",            -- 0马

    "MENU_DROPDOWN_2",      -- 下拉菜单2 鬼牌
    "MASK_DROPDOWN_2",      -- 下拉背景2
    "MENU_DROPDOWN_3",      -- 下拉菜单3 码数
    "MASK_DROPDOWN_3",      -- 下拉背景3
    "MENU_DROPDOWN_5",      -- 下拉菜单3
    "MASK_DROPDOWN_5",      -- 下拉背景3

     "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式
    "BTN_CELLSCORE",                -- 底注
    "BTN_ENTERCONDITION",           -- 入场设定
    "BTN_EXITCONDITION",            -- 离场设定
    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_BEGIN = 400
local CBT_RULEBEGIN = 10      -- 玩法
local CBT_MAGICBEGIN = 20     -- 鬼牌
local CBT_MABEGIN = 30        -- 马数

local dropDownMenuYPox = {20, 60, 100, 140, 180}
local dropDownBgHeight = {40, 80, 120, 160, 200}

-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    local this = self
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode
self.param = param
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    self.m_nodeNormal = spbg:getChildByName("node_normal")
    self.m_nodeGold = spbg:getChildByName("node_gold")
    --self.m_nodeGold:setVisible(false)

    --默认是积分房卡
    self.m_bIsNormal = self.param.cbGameMode or false --true 为积分房卡，false为金币房卡


 --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    self.m_btnScoreType = spbg:getChildByName("ckBox_score")
    self.m_btnScoreType:setTag(TAG_ENUM.CBT_SCOREMODE)
    self.m_btnScoreType:addEventListener(gamemodelistener)
    local txtScoreType = self.m_btnScoreType:getChildByName("Text_1")
    txtScoreType:setColor(cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    

    self.m_btnGoldType = spbg:getChildByName("ckBox_gold")
    self.m_btnGoldType:setTag(TAG_ENUM.CBT_GOLDMODE)
    self.m_btnGoldType:addEventListener(gamemodelistener)

    -- local pos = self.m_btnScoreType:getPositionY()
    -- self.m_btnScoreType:setPositionY(self.m_btnGoldType:getPositionY())
    -- self.m_btnGoldType:setPositionY(pos)

    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0
 
    -- 帮助按钮
    -- local btn = spbg:getChildByName("btn_tips")
    -- btn:setTag(TAG_ENUM.BTN_HELP)
    -- btn:addTouchEventListener(btncallback)  

   

    
  
    

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
            local txt_consume_2 = self.m_nodeNormal:getChildByName("txt_consume_2")
            txt_consume_2:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = self.m_nodeNormal:getChildByName("txt_consume_1")
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

    -- 创建按钮
    self.m_createBtn = self.m_nodeNormal:getChildByName("btn_createroom")
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

    -- 下拉菜单5 --局数
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_5")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_5)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown5 = btn
    -- 下拉箭头3
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_5")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_5)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow5 = btn
    -- 下拉背景3
    panel = self.m_nodeNormal:getChildByName("dropdown_panel_5")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_5)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown5 = panel
    -- 菜单背景3
    self.m_imageDropDownBg5 = panel:getChildByName("dropdown_5")

    -- 选择局数
    self.m_txtSelectRound = self.m_nodeNormal:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    -- 创建费用
    self.m_txtFee = self.m_nodeNormal:getChildByName("txt_createfee")
    self.m_txtFee:setString("")
    self.m_txtFee:setVisible(false)

    self.m_iconFee = self.m_nodeNormal:getChildByName("sp_diamond")

    self.m_iconTxtFee = self.m_nodeNormal:getChildByName("tet_fee")

    self.m_iconFee:setVisible(false)
    self.m_iconTxtFee:setVisible(false)

    --金币房卡

    local btn = self.m_nodeGold:getChildByName("btn_cellscore")
    btn:setTag(TAG_ENUM.BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)

     local btn = self.m_nodeGold:getChildByName("btn_entercondition")
    btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = self.m_nodeGold:getChildByName("btn_exitcondition")
    btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = self.m_nodeGold:getChildByName("btn_createroom")
     btn:setPositionX(510)
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)

    self.m_cellScoreLimit =  self.m_nodeGold:getChildByName("txt_config2_1")

        -- 是否密码
    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD

    --是否允许陌生人加入
    self.m_nOpenMode = true
    local openmodelistener = function (sender,eventType)
        this:onOpenModeSelectedEvent(sender:getTag(),sender)
    end
    local checkbx = self.m_nodeGold:getChildByName("check_set")
    checkbx:setTag(TAG_ENUM.CBT_SETTING)
    checkbx:setSelected(true)
    checkbx:addEventListener(openmodelistener)

    self.m_txtCellScoreLimit = self.m_nodeGold:getChildByName("txt_tips1")
    self.m_txtEnterLimit = self.m_nodeGold:getChildByName("txt_tips2")
    self.m_txtExitLimit = self.m_nodeGold:getChildByName("txt_tips3")



    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%", bPirGoldMode, bPirScoreMode, GlobalUserItem.nCurGameKind)

    
    
    self:onGameModeSelectedEvent(self.m_bIsNormal and TAG_ENUM.CBT_SCOREMODE or TAG_ENUM.CBT_GOLDMODE)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self)
end

function PriRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_nodeNormal:setVisible(self.m_bIsNormal)

    self.m_txtFee:setVisible(tag == TAG_ENUM.CBT_SCOREMODE)
    self.m_iconFee:setVisible(tag == TAG_ENUM.CBT_SCOREMODE)
    self.m_iconTxtFee:setVisible(tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_btnScoreType:getChildByName("Text_1"):setColor(tag == TAG_ENUM.CBT_SCOREMODE and cc.c4b(255,255,255,255) or cc.c4b(0xa5, 0x89, 0xbc, 0xff)  )
    self.m_btnGoldType:getChildByName("Text_1"):setColor(tag == TAG_ENUM.CBT_GOLDMODE and cc.c4b(255,255,255,255) or cc.c4b(0xa5, 0x89, 0xbc, 0xff) )

    --积分、底分、闷牌轮、比牌轮self.m_spBg
    local node = nil
    if self.m_bIsNormal then
        node = self.m_nodeNormal
    else
        node = self.m_nodeGold
    end

    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self.m_tabRuleCheckBox = {}

    -- 玩法选项
    for i = 1, 4 do
        local checkbx = node:getChildByName("check_rule_" .. i)
        print("======================>", checkbx, "check_rule_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_RULEBEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_tabRuleCheckBox[CBT_RULEBEGIN + i] = checkbx
        end
    end
    -- 选择的玩法(可以多选)    
    self.m_nSelectConfigIdx = {0, 0, 0, 0}
    self.m_nSelectConfigIdx[3] = CBT_RULEBEGIN + 3  --默认勾选可抢杠胡
    self.m_tabRuleCheckBox[self.m_nSelectConfigIdx[3]]:setSelected(true)

    --无鬼加倍字串
    self.m_strNoMagicDouble = node:getChildByName("txt_rule_2")

    -- 下拉菜单3 --码数
    local btn = node:getChildByName("btn_dropdown_3")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_3)
    btn:addTouchEventListener(btncallback)
    --btn:setEnabled(false)
    self.m_btnDropDown3 = btn
    -- 下拉箭头3
    btn = node:getChildByName("btn_dropdown_arrow_3")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_3)
    btn:addTouchEventListener(btncallback)
    --btn:setEnabled(false)
    self.m_btnDropArrow3 = btn
    -- 下拉背景3
    panel = node:getChildByName("dropdown_panel_3")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_3)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown3 = panel

    -- 菜单背景3
    self.m_imageDropDownBg3 = panel:getChildByName("dropdown_3")

    -- 翻鬼选项
    for i = 1, 4 do
        local checkbx = node:getChildByName("check_rule_2" ..i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_MAGICBEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_tabRuleCheckBox[CBT_MAGICBEGIN + i] = checkbx
        end
    end
    -- 选择的翻鬼    
    self.m_nSelectMagicIdx = CBT_MAGICBEGIN + 1
    self.m_tabRuleCheckBox[self.m_nSelectMagicIdx]:setSelected(true)
    
    --默认无鬼，无鬼翻倍不可选
    self.m_tabRuleCheckBox[CBT_RULEBEGIN + 2]:setEnabled(false)
    self.m_strNoMagicDouble:setColor(cc.c3b(127,127,127))

    -- 选择鬼牌
    -- self.m_txtSelectMagic = spbg:getChildByName("txt_selectmagic")
    -- self.m_txtSelectMagic:setString("无鬼")


    -- 码数选项
    for i = 1, 5 do
        local checkbx = self.m_imageDropDownBg3:getChildByName("check_rule_3".. i)
        print("======================>", checkbx, "check_rule_3" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_MABEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_tabRuleCheckBox[CBT_MABEGIN + i] = checkbx
        end
    end
    -- 选择的马数    
    self.m_nSelectMaIdx = CBT_MABEGIN + 1
    self.m_tabRuleCheckBox[self.m_nSelectMaIdx]:setSelected(true)

    -- 选择码数
    self.m_txtSelectMa = node:getChildByName("txt_selectma")
    self.m_txtSelectMa:setString("无马")

end

--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)
    print("输入框回调",tag,scoere)
print("lMaxCellScore",PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
print("lMinCellScore",PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore)
print("lMinEnterScore",PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore)
print("lMaxEnterScore",PriRoom:getInstance().m_tabGoldRoomOption.lMaxEnterScore)
    if TAG_ENUM.BTN_CELLSCORE == tag then
        --设置了底分，入场和离场也会变化
        self.m_cellScore = scoere
        print(self.m_cellScore , PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
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

function PriRoomCreateLayer:onRefreshFeeList()
    PriRoom:getInstance():dismissPopWait()
    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender)
    end

    yPos = 51
    bgHeight = 0
   self.m_tabCheckBox = {}
    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    -- 局数
    for i = 1, nConfig do
        yPos = dropDownMenuYPox[nConfig - i + 1] or 51
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local checkbx = self.m_imageDropDownBg5:getChildByName("check_round_" .. i)
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

        local txtcount = self.m_imageDropDownBg5:getChildByName("txt_round_" .. i)
        if nil ~= txtcount then
            -- 设置位置
            txtcount:setPositionY(yPos)
            txtcount:setString(config.dwDrawCountLimit .. "局")
            txtcount:setVisible(true)
        end
    end
    -- 选择的局数    
    self.m_nSelectIdx = CBT_BEGIN + 1
    self.m_tabSelectConfig = nil
    if nConfig > 0 then
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]
        self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)
        self.m_txtSelectRound:setString(string.format("%d局", self.m_tabSelectConfig.dwDrawCountLimit))

        -- 更新费用
        self:updateCreateFee()

        bgHeight = dropDownBgHeight[nConfig] or 0
        self.m_imageDropDownBg5:setContentSize(self.m_imageDropDownBg5:getContentSize().width, bgHeight)
    end

    -- 免费判断
    if PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        self.m_spBg:getChildByName("txt_feetime"):setString(szfree)

        -- 划线
        local draw = cc.DrawNode:create()
        draw:drawSegment(cc.p(560, 75), cc.p(645, 75), 3, cc.c4f(0.18, 0.18, 0.18, 0.67))
        self.m_spBg:addChild(draw)

        -- 免费
        local szfile = "room/sparrowgd_sp_createbtn_free.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            self.m_spBg:getChildByName("sp_create_tips"):setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
        end
 
        -- 字变灰
        self.m_txtFee:setTextColor(cc.c3b(45, 45, 45))
        self.m_txtFee:enableShadow(cc.c4b(0, 255, 0, 0), cc.size(0, -2))
        -- 钻石变灰
        szfile = "shop/itembuy_sp_diamond_gray.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            self.m_spBg:getChildByName("sp_diamond"):setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
        end
    end
    -- 激活按钮
    -- self.m_btnDropDown2:setEnabled(true)
    -- self.m_btnDropArrow2:setEnabled(true)
    self.m_btnDropDown3:setEnabled(true)
    self.m_btnDropArrow3:setEnabled(true)
    self.m_btnDropDown5:setEnabled(true)
    self.m_btnDropArrow5:setEnabled(true)

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
        self.m_cellScoreLimit:setString("底分范围："..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
    end
end

function PriRoomCreateLayer:onLoginPriRoomFinish()
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end
    -- -- 通知更新    
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
            local buffer = ExternalFun.create_netdata(126) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_nSelectScore)
            buffer:pushdword(self.m_tabSelectConfig and self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig and self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(4)
            buffer:pushdword(0)
            -- 密码设置
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

            --游戏额外规则
            buffer:pushbyte(1)
            buffer:pushbyte(4)
            buffer:pushbyte(4)

            --马
            local maTable = {0 , 2, 4, 6, 8}
            buffer:pushbyte(maTable[self.m_nSelectMaIdx -CBT_MABEGIN])
            --记录数据
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_ma", maTable[self.m_nSelectMaIdx -CBT_MABEGIN])

            --翻鬼模式
            local magicTable = {0 , 1, 2, 3}
            buffer:pushbyte(magicTable[self.m_nSelectMagicIdx -CBT_MAGICBEGIN])
            --记录数据
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_magic", magicTable[self.m_nSelectMagicIdx -CBT_MAGICBEGIN])

            --玩法配置
            if self.m_nSelectConfigIdx[1] ~= 0  then
                buffer:pushbyte(0)
                print("创建房卡房间，没有字牌")
            else
                buffer:pushbyte(1)
                print("创建房卡房间，有字牌")
            end
            --记录数据
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_ziCard", self.m_nSelectConfigIdx[1])

            for i=2,4 do
                if self.m_nSelectConfigIdx[i] ~= 0  then
                    buffer:pushbyte(1)
                else
                    buffer:pushbyte(0)
                end
            end
            --记录数据
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_NoMagicDouble", self.m_nSelectConfigIdx[2])
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_QiangGangHU", self.m_nSelectConfigIdx[3])
            cc.UserDefault:getInstance():setIntegerForKey("sparrowgdex_QiDui", self.m_nSelectConfigIdx[4])

            for i = 1, 91 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)
            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)
            print("----------------------------->", self.m_bIsNormal)
            if self.m_nOpenMode then
                buffer:pushbyte(1)
                print("fa fa fa 1111111111111")
            else
                buffer:pushbyte(0)
                print("fa fa fa 00000000000000")
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
    local dwRoomID = roomDetailInfo.dwRoomID or "" 
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0

    local ma = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_ma",0)
    local strMa = ma == 0 and "无马," or ma.."马,"

    local mode = {"无鬼,", "白板做鬼,", "翻鬼,", "翻双鬼,",}
    local magicModel = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_magic",0)
    local strMagicMode = mode[magicModel +1]

    local strQiangGangHu = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_QiangGangHU",0) ~= 0 and "可抢杠胡," or ""
    local strHuQiDui = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_QiDui",0) ~= 0 and "可胡七对," or ""
    local strHaveZiCard = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_ziCard",0) ~= 0 and "不带风," or "带风,"
    local strNoMagicDouble = cc.UserDefault:getInstance():getIntegerForKey("sparrowgdex_NoMagicDouble",0) ~= 0 and "无鬼翻倍," or ""

    local shareTxt = "局数:" .. turnCount .. "局。"..strMa..strMagicMode..strQiangGangHu..strHuQiDui..strHaveZiCard..strNoMagicDouble.."广东麻将精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=391&a=1" .. "&p=" .. passwd
     print("获取分享信息===", shareTxt,  url)
    return {title = "【约战房间: " .. dwRoomID .. "】", content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = "广东麻将, 房号[" .. dwRoomID .. "],您的好友喊你打牌了!"}
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
    elseif TAG_ENUM.BTN_CREATE == tag
    or TAG_ENUM.BTN_CREATE_1 == tag then 
        if self.m_bLow and self.param.clubID == nil then
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
        PriRoom:getInstance():setViewFrame(self)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        PriRoom:getInstance(). m_bPlayModel = 0
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    --  elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then
    --     self.m_maskDropDown2:setVisible(true)
    --     -- 更新箭头
    --     self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
    --     self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
    --     self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    -- elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
    --     self.m_maskDropDown2:setVisible(false)
    --     -- 更新箭头
    --     self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
    --     self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
    --     self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_3 == tag then
        if not self.m_maskDropDown3:isVisible() then
            self.m_maskDropDown3:setVisible(true)
            -- 更新箭头
            self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrow3:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
        else
            self.m_maskDropDown3:setVisible(false)
            -- 更新箭头
            self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow3:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
        end
    elseif TAG_ENUM.MASK_DROPDOWN_3 == tag then
        self.m_maskDropDown3:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow3:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_5 == tag then

        if not self.m_maskDropDown5:isVisible() then
            self.m_maskDropDown5:setVisible(true)
            -- 更新箭头
            self.m_btnDropArrow5:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrow5:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrow5:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
        else
            self.m_maskDropDown5:setVisible(false)
            -- 更新箭头
            self.m_btnDropArrow5:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow5:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow5:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
        end
    elseif TAG_ENUM.MASK_DROPDOWN_5 == tag then
        self.m_maskDropDown5:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow5:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow5:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow5:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
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

function PriRoomCreateLayer:onRuleSelectedEvent(tag, sender)
    print("==========================>", tag)
    if tag > CBT_RULEBEGIN and tag < CBT_MAGICBEGIN then --玩法，可以多选
        local num = #self.m_nSelectConfigIdx
        local isSelect = false
        for i=1, num do
            if self.m_nSelectConfigIdx[i] == tag then
                self.m_nSelectConfigIdx[i] = 0
                self.m_tabRuleCheckBox[tag]:setSelected(false)
                isSelect = true
            end
        end
        if not isSelect then --之前没有选
            self.m_nSelectConfigIdx[tag - CBT_RULEBEGIN] = tag
            self.m_tabRuleCheckBox[tag]:setSelected(true)
        end
    end

    if tag > CBT_MAGICBEGIN and tag < CBT_MABEGIN then --鬼牌，可以多选
        if self.m_nSelectMagicIdx == tag then
            sender:setSelected(true)
            return
        end
        self.m_nSelectMagicIdx = tag
        if self.m_nSelectMagicIdx -CBT_MAGICBEGIN == 1 then  --第一项，无鬼，选择则无鬼加倍不可选
            self.m_nSelectConfigIdx[2] = 0  --如果之前已经选择，变为未选
            self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setSelected(false)
            self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setEnabled(false)
            self.m_strNoMagicDouble:setColor(cc.c3b(127,127,127))
        else
            self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setEnabled(true)
            self.m_strNoMagicDouble:setColor(cc.c3b(255, 255, 255))
        end
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag  and  k > CBT_MAGICBEGIN and k < CBT_MABEGIN then
                v:setSelected(false)
            end
        end

        -- local strMagic = {"无鬼", "白板做鬼", "翻鬼", "翻双鬼"}
        -- self.m_txtSelectMagic:setString(strMagic[self.m_nSelectMagicIdx - CBT_MAGICBEGIN])
    end
    if tag > CBT_MABEGIN then --码数，可以多选
        self.m_maskDropDown3:setVisible(false)
        -- 更新箭头
            self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow3:loadTexturePressed("room/land_pribtn_arrow_down_1.png")

        if self.m_nSelectMaIdx == tag then
            sender:setSelected(true)
            return
        end
        self.m_nSelectMaIdx = tag
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag  and  k > CBT_MABEGIN  then
                v:setSelected(false)
            end
        end

        local strMa = {"无马", "2马", "4马", "6马", "8马"}
        self.m_txtSelectMa:setString(strMa[self.m_nSelectMaIdx -CBT_MABEGIN])

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

function PriRoomCreateLayer:onSelectedEvent(tag, sender)
    self.m_maskDropDown5:setVisible(false)
    self.m_btnDropArrow5:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow5:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrow5:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
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
dump(self._param , "AA信息")
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
        -- 读前两个规则
        buffer:readbyte()
        buffer:readbyte()

        local strConfig = ""
        --马
        local maTable = {0 , 2, 4, 6, 8}
        local cbMa = buffer:readbyte()
        strConfig = strConfig..cbMa.."马"

        --翻鬼模式 从0开始
        local magicTable = {"、无鬼" , "、白板做鬼", "、翻鬼", "、翻双鬼"}
        local cbMagic = buffer:readbyte()
        strConfig = strConfig..magicTable[cbMagic +1]

        --字牌配置
        local strRule = ""
        local cbHaveZiCard = buffer:readbyte()
        if 0 == cbHaveZiCard then
            strRule = strRule.."不带风"
        else
            strRule = strRule.."带风"
        end
        --无鬼翻倍
        local cbNoMagicDouble = buffer:readbyte()
        if 1 == cbNoMagicDouble then
            strRule = strRule.."、无鬼翻倍"
        end
        --抢杠胡
        local cbQiangGangHu = buffer:readbyte()
        if 1 == cbQiangGangHu then
            strRule = strRule.."、可抢杠胡"
        end
        --七对
        local cbQiDui = buffer:readbyte()
        if 1 == cbQiDui then
            strRule = strRule.."、可胡七对"
        end

        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(strConfig)
        -- 规则
        spbg:getChildByName("txt_guize"):setString(strRule)
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