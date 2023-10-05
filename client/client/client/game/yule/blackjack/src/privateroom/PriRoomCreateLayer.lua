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
    "CBT_AA_PAY",           -- AA

   

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    

    "MENU_DROPDOWN_ROUND",      -- 下拉菜单 局数
    "MASK_DROPDOWN_ROUND",      -- 下拉背景 局数

    "MENU_DROPDOWN_PLAYNUM",      -- 下拉菜单 人数
    "MASK_DROPDOWN_PLAYNUM",      -- 下拉背景 人数

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
local CBT_PLAYERNUM_BEGIN = 10      -- 房间人数
local CBT_BANKERMODE = 20           --上庄模式

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


    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0

   
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
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_ROUND)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDownRound = btn
    -- 下拉箭头3
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_2")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_ROUND)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrowRound = btn
    -- 下拉背景3
    panel = self.m_nodeNormal:getChildByName("dropdown_panel_2")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_ROUND)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDownRound = panel
    -- 菜单背景3
    self.m_imageDropDownBgRound = panel:getChildByName("dropdown_2")


    -- 下拉菜单5 --人数
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_PLAYNUM)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDownPlayNum = btn
    -- 下拉箭头3
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_PLAYNUM)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrowPlayNum = btn
    -- 下拉背景3
    panel = self.m_nodeNormal:getChildByName("dropdown_panel_1")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_PLAYNUM)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDownPlayNum = panel
    -- 菜单背景3
    self.m_imageDropDownBgPlayNum = panel:getChildByName("dropdown_1")

    -- 人数选项
    local cbtPlayerNumlistener = function (sender,eventType)
        self:onPlayerNumSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPlayerNumCheckBox = {}

    for i=1,5 do
        local checkbx = self.m_imageDropDownBgPlayNum:getChildByName("check_rule_" .. i)
        print("获取人数checkBox", checkbx, "check_rule_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_PLAYERNUM_BEGIN + i)
            checkbx:addEventListener(cbtPlayerNumlistener)
            checkbx:setSelected(false)
            self.m_tabPlayerNumCheckBox[CBT_PLAYERNUM_BEGIN + i] = checkbx
        end
    end
   -- 选择的人数，默认为第一个    
    self.m_nPlayerNumSelectIdx = CBT_PLAYERNUM_BEGIN + 1
    self.m_tabPlayNumSelect = {2, 3, 4, 5, 6}
    self.m_tabPlayerNumCheckBox[self.m_nPlayerNumSelectIdx]:setSelected(true)

    -- 选择规则
    self.m_txtSelectRule = self.m_nodeNormal:getChildByName("txt_selectrule")
    self.m_txtSelectRule:setString("2人")



    -- 选择局数
    self.m_txtSelectRound = self.m_nodeNormal:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    -- 创建费用
    self.m_txtFee = self.m_nodeNormal:getChildByName("txt_createfee")
    self.m_txtFee:setString("")

    self.m_iconFee = self.m_nodeNormal:getChildByName("sp_diamond")

    self.m_iconTxtFee = self.m_nodeNormal:getChildByName("tetfee")

    

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

function PriRoomCreateLayer:onPlayerNumSelectedEvent(tag, sender)

        self.m_maskDropDownPlayNum:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrowPlayNum:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowPlayNum:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowPlayNum:loadTextureNormal("room/land_pribtn_arrow_down_1.png")

        self.m_maskDropDownRound:setVisible(false)

    if self.m_nPlayerNumSelectIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nPlayerNumSelectIdx = tag
    for k,v in pairs(self.m_tabPlayerNumCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        else
            v:setSelected(true)
        end
    end
    local playerNum = {"2人", "3人", "4人", "5人", "6人",}
    self.m_txtSelectRule:setString(playerNum[tag - CBT_PLAYERNUM_BEGIN])
    --self.m_tabPlayNumSelect = tag - CBT_PLAYERNUM_BEGIN
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_nodeNormal:setVisible(self.m_bIsNormal)

    
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

    self.bankerModeCheckTab = {}

     for i=1,2 do
        local check=node:getChildByName("check_rule_"..i)
                        :setTag(CBT_BANKERMODE+i)
        self.bankerModeCheckTab[i]=check
        if i==1 then 
            check:setSelected(true) 
            self.selectBankerMode=CBT_BANKERMODE+1 
        end
    end

    for i=1,2 do
        self.bankerModeCheckTab[i]:addEventListener(function (sender,eventType)
            self.selectBankerMode=CBT_BANKERMODE+i
            for k,check in pairs(self.bankerModeCheckTab) do
                if check and check:getTag()~=sender:getTag() then 
                    check:setSelected(false) 
                elseif check then
                    check:setSelected(true) 
                end
            end
        end)
    end

end

--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)

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
        local checkbx = self.m_imageDropDownBgRound:getChildByName("check_rule_" .. i)
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

        local txtcount = self.m_imageDropDownBgRound:getChildByName("txt_rule_" .. i)
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
        self.m_imageDropDownBgRound:setContentSize(self.m_imageDropDownBgRound:getContentSize().width, bgHeight)
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
     self.m_btnDropDownRound:setEnabled(true)
     self.m_btnDropArrowRound:setEnabled(true)
    self.m_btnDropDownPlayNum:setEnabled(true)
    self.m_btnDropArrowPlayNum:setEnabled(true)

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
            if self.m_bIsNormal then
                buffer:pushbyte(self.m_tabPlayNumSelect[self.m_nPlayerNumSelectIdx -CBT_PLAYERNUM_BEGIN])
            else
                buffer:pushbyte(6)
            end
            buffer:pushbyte(6)

            buffer:pushbyte(self.selectBankerMode - CBT_BANKERMODE)

            for i = 1, 96 do
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

    local shareTxt = "局数:" .. turnCount .. "局。"..strMa..strMagicMode..strQiangGangHu..strHuQiDui..strHaveZiCard..strNoMagicDouble.."21点精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=241&a=1" .. "&p=" .. passwd
     print("获取分享信息===", shareTxt,  url)
    return {title = "【约战房间: " .. dwRoomID .. "】", content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = "21点, 房号[" .. dwRoomID .. "],您的好友喊你打牌了!"}
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
    elseif TAG_ENUM.MENU_DROPDOWN_PLAYNUM == tag then
        if not self.m_maskDropDownPlayNum:isVisible() then
            self.m_maskDropDownPlayNum:setVisible(true)
            -- 更新箭头
            self.m_btnDropArrowPlayNum:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrowPlayNum:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrowPlayNum:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
        else
            self.m_maskDropDownPlayNum:setVisible(false)
            -- 更新箭头
            self.m_btnDropArrowPlayNum:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowPlayNum:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowPlayNum:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
        end
    elseif TAG_ENUM.MASK_DROPDOWN_PLAYNUM == tag then
        self.m_maskDropDownPlayNum:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrowPlayNum:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowPlayNum:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowPlayNum:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_ROUND == tag then

        if not self.m_maskDropDownRound:isVisible() then
            self.m_maskDropDownRound:setVisible(true)
            -- 更新箭头
            self.m_btnDropArrowRound:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrowRound:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
            self.m_btnDropArrowRound:loadTexturePressed("room/land_pribtn_arrow_up_1.png")
        else
            self.m_maskDropDownRound:setVisible(false)
            -- 更新箭头
            self.m_btnDropArrowRound:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowRound:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowRound:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
        end
    elseif TAG_ENUM.MASK_DROPDOWN_Round == tag then
        self.m_maskDropDownRound:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrowRound:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowRound:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrowRound:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
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
    self.m_maskDropDownRound:setVisible(false)
    self.m_btnDropArrowRound:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowRound:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
            self.m_btnDropArrowRound:loadTexturePressed("room/land_pribtn_arrow_down_1.png")
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
    local roomid = self._param.dwRoomId or 0
    spbg:getChildByName("txt_roomid"):setString(string.format("%06d", roomid))

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString(consume .. "")

    -- 玩法
    local wanfa = spbg:getChildByName("txt_wanfa")

    -- 规则
    local guize = spbg:getChildByName("txt_guize")
    local buffer = self._param.buffer

    --for i=1,100 do
       -- print(buffer:readbyte())
   -- end


    if nil ~= buffer and nil ~= buffer.readbyte then

        local playNum = buffer:readbyte().."人"
        buffer:readbyte()
        local bankerMode = buffer:readbyte()

        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(playNum)
        -- 规则
        spbg:getChildByName("txt_guize"):setString(bankerMode==2 and "BJ上庄" or "霸王庄")

        
        
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