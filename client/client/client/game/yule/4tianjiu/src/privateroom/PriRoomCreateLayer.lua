-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

-- 牌九私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面

-- 牌九AA制提示界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local TAG_START             = 100
local CBT_RULEBEGIN = 200
local CBT_BEGIN = 400
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

    --游戏模式
    "CBT_GAME_MODE_1",           --大牌九
    "CBT_GAME_MODE_2",           --小牌九
    "CBT_GAME_MODE_3",           --加锅牌九

    --牌型选择
    "CBT_CARD_TYPE_1",           --炸弹
    "CBT_CARD_TYPE_2",           --鬼子
    "CBT_CARD_TYPE_3",           --天九王
    "CBT_CARD_TYPE_4",           --地九娘娘

    --道的选择
    "CBT_DAO_1",           --2道
    "CBT_DAO_2",           --3道

    --庄的选择
    "CBT_BANKER_TYPE_1",           --轮庄
    "CBT_BANKER_TYPE_2",           --抢庄庄
    "CBT_BANKER_TYPE_3",           --霸王庄

    --庄家亮牌
    "CBT_BANKER_SHOW_1",           --不亮
    "CBT_BANKER_SHOW_2",           --亮2张
    "CBT_BANKER_SHOW_3",           --亮3张

    --选分
    "CBT_SCORE_SELECT_1",           --每次选分
    "CBT_SCORE_SELECT_2",           --固定选分
    

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    "MENU_DROPDOWN_1",      -- 下拉菜单1
    "MASK_DROPDOWN_1",      -- 下拉背景1
    "MENU_DROPDOWN_2",      -- 下拉菜单2
    "MASK_DROPDOWN_2",      -- 下拉背景2


    "CBT_DAPAIJIU",
    "CBT_XIAOPAIJIU",

    "CBT_PLAYERNUM",

    "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式
    "BTN_CELLSCORE",                -- 底注
    "BTN_ENTERCONDITION",           -- 入场设定
    "BTN_EXITCONDITION",            -- 离场设定
    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置
}

local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_GAME_MODE = 10      -- 游戏模式
local CBT_CARD_TYPE = 20     -- 牌型选择
local CBT_DAO = 30        -- 道的选择
local CBT_BANKER_TYPE = 40        -- 庄的选择
local CBT_BANKER_SHOW = 50        -- 庄家亮牌

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
    self.m_spBg = spbg

    local ScrollView = spbg:getChildByName("ScrollView")

    self.m_nodeNormal = ScrollView:getChildByName("node_normal")
    self.m_nodeGold = ScrollView:getChildByName("node_gold")
    --self.m_nodeGold:setVisible(false)

    --默认是积分房卡
    self.m_bIsNormal = self.param.cbGameMode or false --true 为积分房卡，false为金币房卡

    --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    -- self.m_btnScoreType = spbg:getChildByName("ckBox_score")
    -- self.m_btnScoreType:setTag(TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnScoreType:addEventListener(gamemodelistener)
    -- local txtScoreType = self.m_btnScoreType:getChildByName("Text_1")
    -- txtScoreType:setColor(cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    
    -- self.m_btnGoldType = spbg:getChildByName("ckBox_gold")
    -- self.m_btnGoldType:setTag(TAG_ENUM.CBT_GOLDMODE)
    -- self.m_btnGoldType:addEventListener(gamemodelistener)

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
    self.m_createBtn = self.m_spBg:getChildByName("btn_createroom_1")
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

    -- 下拉菜单1
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown1 = btn
    -- 下拉箭头1
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_1")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow1 = btn
    -- 下拉背景1
    local panel = self.m_nodeNormal:getChildByName("dropdown_panel_1")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_1)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown1 = panel
    -- 菜单背景1
    self.m_imageDropDownBg1 = panel:getChildByName("dropdown")

    -- 下拉菜单2
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_3")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown2 = btn
    -- 下拉箭头2
    btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_3")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropArrow2 = btn
    -- 下拉背景2
    panel = self.m_nodeNormal:getChildByName("dropdown_panel_3")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_2)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown2 = panel
    -- 菜单背景2
    self.m_imageDropDownBg2 = panel:getChildByName("dropdown")
    -- -- 下拉菜单5 --局数
    -- btn = self.m_nodeNormal:getChildByName("btn_dropdown_5")
    -- btn:setTag(TAG_ENUM.MENU_DROPDOWN_5)
    -- btn:addTouchEventListener(btncallback)
    -- btn:setEnabled(false)
    -- self.m_btnDropDown5 = btn
    -- -- 下拉箭头3
    -- btn = self.m_nodeNormal:getChildByName("btn_dropdown_arrow_5")
    -- btn:setTag(TAG_ENUM.MENU_DROPDOWN_5)
    -- btn:addTouchEventListener(btncallback)
    -- btn:setEnabled(false)
    -- self.m_btnDropArrow5 = btn
    -- -- 下拉背景3
    -- panel = self.m_nodeNormal:getChildByName("dropdown_panel_5")
    -- panel:setTag(TAG_ENUM.MASK_DROPDOWN_5)
    -- panel:addTouchEventListener( btncallback )
    -- panel:setVisible(false)
    -- self.m_maskDropDown5 = panel
    -- -- 菜单背景3
    -- self.m_imageDropDownBg5 = panel:getChildByName("dropdown_5")

    -- 选择局数
    self.m_txtSelectRound = self.m_nodeNormal:getChildByName("txt_selectround_3")
    self.m_txtSelectRound:setString("")

    -- 创建费用
    self.m_txtFee = self.m_spBg:getChildByName("txt_createfee")
    self.m_txtFee:setString("")
    -- self.m_txtFee:setVisible(false)

    self.m_iconFee = self.m_spBg:getChildByName("sp_diamond")

    self.m_iconTxtFee = self.m_spBg:getChildByName("tet_fee")

    -- self.m_iconFee:setVisible(false)
    -- self.m_iconTxtFee:setVisible(false)

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

    local btn = self.m_spBg:getChildByName("btn_createroom_3")
    self.m_createBtn_gold = btn;
    btn:setPositionX(627)
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)

    self.m_cellScoreLimit =  self.m_nodeGold:getChildByName("txt_tips_1_1")

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

    self.m_txtCellScoreLimit = self.m_nodeGold:getChildByName("txt_tips_1")
    self.m_txtEnterLimit = self.m_nodeGold:getChildByName("txt_tips_2")
    self.m_txtExitLimit = self.m_nodeGold:getChildByName("txt_tips_3")

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
function PriRoomCreateLayer:onGameModeSelectedEvent(tag, sender)
    -- self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    -- self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)
    -- local color = (tag == TAG_ENUM.CBT_SCOREMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    -- local txtScoreType = self.m_btnScoreType:getChildByName("txt")
    -- txtScoreType:setColor(color)
    -- local color1 = (tag == TAG_ENUM.CBT_GOLDMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    -- local txtGoldType = self.m_btnGoldType:getChildByName("txt")
    -- txtGoldType:setColor(color1)

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_nodeNormal:setVisible(self.m_bIsNormal)


    --共同规则
    local node = nil
    if self.m_bIsNormal then
        node = self.m_nodeNormal
        self.m_createBtn:setVisible(true);
        self.m_createBtn_gold:setVisible(false);
    else
        node = self.m_nodeGold
        self.m_createBtn:setVisible(false);
        self.m_createBtn_gold:setVisible(true);
    end

    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end

    self.m_tabRuleCheckBox = {}

    -- 游戏模式
    for i = 1, 3 do
        local checkbx = node:getChildByName("check_rule_4_" ..i);
        if nil ~= checkbx then
            checkbx:setVisible(true);
            checkbx:setTag(CBT_GAME_MODE + i);
            checkbx:addEventListener(cbtlistener);
            checkbx:setSelected(false);
            self.m_tabRuleCheckBox[CBT_GAME_MODE + i] = checkbx;
        end
    end
    self.m_nSelectGameMode = CBT_GAME_MODE + 1;
    self.m_tabRuleCheckBox[self.m_nSelectGameMode]:setSelected(true);

    -- 牌型选择
    for i = 1, 4 do
        local checkbx = node:getChildByName("check_rule_5_" ..i);
        if nil ~= checkbx then
            checkbx:setVisible(true);
            checkbx:setTag(CBT_CARD_TYPE + i);
            checkbx:addEventListener(cbtlistener);
            checkbx:setSelected(true);
            self.m_tabRuleCheckBox[CBT_CARD_TYPE + i] = checkbx;
        end
    end
    self.m_nSelectConfigIdx = {1, 1, 1, 1}
    -- self.m_nSelectConfigIdx[3] = CBT_CARD_TYPE + 3  --默认勾选可抢杠胡
    -- self.m_tabRuleCheckBox[self.m_nSelectConfigIdx[3]]:setSelected(true)

    -- 道的选择
    for i = 1, 2 do
        local checkbx = node:getChildByName("check_rule_6_" ..i);
        if nil ~= checkbx then
            checkbx:setVisible(true);
            checkbx:setTag(CBT_DAO + i);
            checkbx:addEventListener(cbtlistener);
            checkbx:setSelected(false);
            self.m_tabRuleCheckBox[CBT_DAO + i] = checkbx;
        end
    end
    self.m_nSelectDao = CBT_DAO + 1;
    self.m_tabRuleCheckBox[self.m_nSelectDao]:setSelected(true);
    
    -- 庄的选择
    for i = 1, 3 do
        local checkbx = node:getChildByName("check_rule_7_" ..i);
        if nil ~= checkbx then
            checkbx:setVisible(true);
            checkbx:setTag(CBT_BANKER_TYPE + i);
            checkbx:addEventListener(cbtlistener);
            checkbx:setSelected(false);
            self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i] = checkbx;
        end
    end
    self.m_nSelectBankerType = CBT_BANKER_TYPE + 1;
    self.m_tabRuleCheckBox[self.m_nSelectBankerType]:setSelected(true);

    -- 庄家亮牌
    for i = 1, 3 do
        local checkbx = node:getChildByName("check_rule_8_" ..i);
        if nil ~= checkbx then
            checkbx:setVisible(true);
            checkbx:setTag(CBT_BANKER_SHOW + i);
            checkbx:addEventListener(cbtlistener);
            checkbx:setSelected(false);
            self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i] = checkbx;
        end
    end
    self.m_nSelectBankerShow = CBT_BANKER_SHOW + 1;
    self.m_tabRuleCheckBox[self.m_nSelectBankerShow]:setSelected(true);

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
    -- local this = self

    -- self.m_tabScoreList = {}
    -- local scoreList = clone(PriRoom:getInstance().m_tabCellScoreList)
    -- if type(scoreList) ~= "table" then
    --     scoreList = {}
    -- end
    -- dump(scoreList, "斗地主底分配置", 10)
    -- dump(PriRoom:getInstance().m_tabFeeConfigList, "玩法信息", 10)
    -- local nScoreList = #scoreList
    -- if 0 == nScoreList then
    --     scoreList = {10, 20, 30, 40, 50}
    --     nScoreList = 5
    -- end
    -- self.m_scoreList = scoreList
    -- -- 规则选择
    -- local rulelistener = function (sender,eventType)
    --     this:onSelectedScoreEvent(sender:getTag(),sender)
    -- end
    -- local yPos = 51
    -- local bgHeight = 0
    -- for i = 1, nScoreList do 
    --     -- y轴位置
    --     yPos = dropDownMenuYPox[nScoreList - i + 1] or 51

    --     local score = scoreList[i] or 0
    --     local checkbx = self.m_imageDropDownBg1:getChildByName("check_rule_" .. i)
    --     if nil ~= checkbx then
    --         checkbx:setVisible(true)
    --         checkbx:setTag(CBT_RULEBEGIN + i)
    --         checkbx:addEventListener(rulelistener)
    --         -- 设置位置
    --         checkbx:setPositionY(yPos)
    --         self.m_tabScoreList[CBT_RULEBEGIN + i] = checkbx
    --     end

    --     local txtScore = self.m_imageDropDownBg1:getChildByName("txt_rule_" .. i)
    --     if nil ~= txtScore then
    --         -- 设置底分
    --         txtScore:setString(score .. "分")
           
    --         txtScore:setVisible(true)
    --         -- 设置位置
    --         txtScore:setPositionY(yPos)
    --     end
    --     if score == 0 then
    --         checkbx:setVisible(false)
    --         txtScore:setVisible(false)
    --     end
    -- end
    
    -- -- 默认选择底分  
    -- if nScoreList > 0 then
    --     self.m_nSelectScoreIdx = CBT_RULEBEGIN + 1
    --     self.m_tabScoreList[self.m_nSelectScoreIdx]:setSelected(true)
    --     self.m_nSelectScore = scoreList[1]

    --     self.m_txtSelectRule:setString(string.format("%d分", self.m_nSelectScore))

    --     bgHeight = dropDownBgHeight[nScoreList] or 0
    --     self.m_imageDropDownBg1:setContentSize(self.m_imageDropDownBg1:getContentSize().width, bgHeight)
    -- end

    yPos = 51
    bgHeight = 0
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
        self.m_nodeNormal:getChildByName("txt_createfee"):setVisible(false)
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
    -- dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
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

-- function PriRoomCreateLayer:onSelectedScoreEvent(tag, sender)
--      if self.m_nSelectScoreIdx == tag then
--         sender:setSelected(true)
--         return
--     end
--     self.m_nSelectScoreIdx = tag
--     for k,v in pairs(self.m_tabScoreList) do
--         if k ~= tag then
--             v:setSelected(false)
--         end
--     end
--     self.m_nSelectScore = self.m_scoreList[self.m_nSelectScoreIdx - CBT_RULEBEGIN]
--     self.m_txtSelectRule:setString(string.format("%d分", self.m_nSelectScore))
-- end

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


function PriRoomCreateLayer:onLoginPriRoomFinish()
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
            local buffer = ExternalFun.create_netdata(126) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)

            buffer:pushscore(self.m_nSelectScore)
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
            buffer:pushbyte(4)--(self.m_nPlayerNumIdx - TAG_ENUM.CBT_PLAYERNUM)==1 and 4 or 6) --当前房间人数
            buffer:pushbyte(4) --允许的最大人数
--            buffer:pushbyte(self.m_nPlayModeIdx==TAG_ENUM.CBT_DAPAIJIU and 1 or 0)
            
            --游戏模式   1大牌九，2小牌九，3加锅牌九
            buffer:pushbyte(self.m_nSelectGameMode - CBT_GAME_MODE)
            
            --牌型选择   炸弹、鬼子、地九娘娘、天九王
            for i=1,4 do
                if self.m_nSelectConfigIdx[i] ~= 0  then
                    buffer:pushbyte(1)
                else
                    buffer:pushbyte(0)
                end
            end
            --道的选择   道的选择2,3
            buffer:pushbyte(self.m_nSelectDao - CBT_DAO +1)
            --庄的选择   0轮庄、1抢庄、2霸王庄
            buffer:pushbyte(self.m_nSelectBankerType - CBT_BANKER_TYPE -1)
            --庄家亮牌   0不亮、2亮2张、3亮3张
            if self.m_nSelectBankerShow - CBT_BANKER_SHOW == 1 then
                buffer:pushbyte(0)
            else
                buffer:pushbyte(self.m_nSelectBankerShow - CBT_BANKER_SHOW)
            end

            for i = 1, 95-6 do
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

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_nodeNormal)

        -- 通知更新    
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    elseif TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
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
        -- if nil == self.m_nSelectScore or table.nums(self.m_scoreList) == 0 then
        --     showToast(self, "未选择游戏底分!", 2)
        --     return
        -- end
        PriRoom:getInstance():setViewFrame(self)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        --私有房模式
        PriRoom:getInstance().m_bPlayModel = 0
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    elseif TAG_ENUM.MENU_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.BTN_TYPE == tag then
        if self.m_bIsNormal then
            self.m_bIsNormal = false
            --设置按钮状态
            self.m_btnType:loadTextureDisabled("private/pri_btn_gold.png")
            self.m_btnType:loadTextureNormal("private/pri_btn_score.png")
            self.m_btnType:loadTextureNormal("private/pri_btn_gold.png")
        else
            self.m_bIsNormal = true
            --设置按钮状态
            self.m_btnType:loadTextureDisabled("private/pri_btn_score.png")
            self.m_btnType:loadTextureNormal("private/pri_btn_gold.png")
            self.m_btnType:loadTextureNormal("private/pri_btn_score.png")
        end
        self.m_nodeGold:setVisible(not self.m_bIsNormal)
        self.m_nodeNormal:setVisible(self.m_bIsNormal)
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
    if tag > CBT_CARD_TYPE and tag < CBT_CARD_TYPE + 10 then --牌型选择，可以多选
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
            self.m_nSelectConfigIdx[tag - CBT_CARD_TYPE] = tag
            self.m_tabRuleCheckBox[tag]:setSelected(true)
        end
    end

    if tag > CBT_GAME_MODE and tag < CBT_GAME_MODE+10 then --游戏模式
        self.m_tabRuleCheckBox[self.m_nSelectGameMode]:setSelected(false)
        self.m_nSelectGameMode = tag
        self.m_tabRuleCheckBox[self.m_nSelectGameMode]:setSelected(true)
        if tag == CBT_GAME_MODE + 2 then --小牌九
            for i = 1, 3 do
                self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i]:setSelected(false)
                self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i]:setColor(cc.c3b(127,127,127))
                self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i]:setEnabled(false)
            end
            self.m_nSelectBankerShow = CBT_BANKER_SHOW + 1
            self.m_tabRuleCheckBox[self.m_nSelectBankerShow]:setSelected(true)
        else
            for i = 1, 3 do
                self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i]:setEnabled(true)
                self.m_tabRuleCheckBox[CBT_BANKER_SHOW + i]:setColor(cc.c3b(255, 255, 255))
            end

        end

        if tag == CBT_GAME_MODE + 3 then --加锅牌九
            for i = 1, 3 do
                self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i]:setSelected(false)
                self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i]:setEnabled(false)
                self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i]:setColor(cc.c3b(127,127,127))
                
            end
            self.m_nSelectBankerType = CBT_BANKER_TYPE + 2
            self.m_tabRuleCheckBox[self.m_nSelectBankerType]:setSelected(true)
        else
            for i = 1, 3 do
                self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i]:setEnabled(true)
                self.m_tabRuleCheckBox[CBT_BANKER_TYPE + i]:setColor(cc.c3b(255, 255, 255))
            end
        end
    end

    if tag > CBT_DAO and tag < CBT_DAO+10 then --道的选择
        self.m_tabRuleCheckBox[self.m_nSelectDao]:setSelected(false)
        self.m_nSelectDao = tag
        self.m_tabRuleCheckBox[self.m_nSelectDao]:setSelected(true)
    end

    if tag > CBT_BANKER_TYPE and tag < CBT_BANKER_TYPE+10 then --庄的选择
        self.m_tabRuleCheckBox[self.m_nSelectBankerType]:setSelected(false)
        self.m_nSelectBankerType = tag
        self.m_tabRuleCheckBox[self.m_nSelectBankerType]:setSelected(true)
    end

     if tag > CBT_BANKER_SHOW and tag < CBT_BANKER_SHOW+10 then --庄家亮牌
        self.m_tabRuleCheckBox[self.m_nSelectBankerShow]:setSelected(false)
        self.m_nSelectBankerShow = tag
        self.m_tabRuleCheckBox[self.m_nSelectBankerShow]:setSelected(true)
    end

    -- if tag > CBT_MAGICBEGIN and tag < CBT_MABEGIN then --鬼牌，可以多选
    --     if self.m_nSelectMagicIdx == tag then
    --         sender:setSelected(true)
    --         return
    --     end
    --     self.m_nSelectMagicIdx = tag
    --     if self.m_nSelectMagicIdx -CBT_MAGICBEGIN == 1 then  --第一项，无鬼，选择则无鬼加倍不可选
    --         self.m_nSelectConfigIdx[2] = 0  --如果之前已经选择，变为未选
    --         self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setSelected(false)
    --         self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setEnabled(false)
    --         self.m_strNoMagicDouble:setColor(cc.c3b(127,127,127))
    --     else
    --         self.m_tabRuleCheckBox[CBT_RULEBEGIN +2]:setEnabled(true)
    --         self.m_strNoMagicDouble:setColor(cc.c3b(255, 255, 255))
    --     end
    --     for k,v in pairs(self.m_tabRuleCheckBox) do
    --         if k ~= tag  and  k > CBT_MAGICBEGIN and k < CBT_MABEGIN then
    --             v:setSelected(false)
    --         end
    --     end

    --     -- local strMagic = {"无鬼", "白板做鬼", "翻鬼", "翻双鬼"}
    --     -- self.m_txtSelectMagic:setString(strMagic[self.m_nSelectMagicIdx - CBT_MAGICBEGIN])
    -- end
    
end

--支付
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

--游戏模式


-- 更新费用
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
    print("fee:",fee)
    self.m_txtFee:setString("x"..fee)
end

function PriRoomCreateLayer:onTouch(eventType, x, y)
	if eventType == "began" then
        self.m_imageDropDownBg1:setVisible(false)
        self.m_imageDropDownBg2:setVisible(false)
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self.priroomBg:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
    	self:scaleTransitionOut(self.m_nodeNormal)
    end

    return true
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function PriRoomCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_nodeNormal)
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

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local dwRoomID = roomDetailInfo.dwRoomID or 0
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0
    local playMode = "游戏模式:"..(cc.UserDefault:getInstance():getIntegerForKey("sangong_playmode",0) == 0 and "九点上庄" or "抢庄")..","

    local shareTxt = "局数:" .. turnCount .. "局。"..playMode..cmd.GAME_NAME.."游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k="..cmd.KIND_ID.."&a=1" .. "&p=" .. passwd
    print("获取分享信息===》", dwRoomID,shareTxt,  url)
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format(cmd.GAME_NAME..", 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
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
        dump(buffer, "创建玩法", 5)
        local renshu = buffer:readbyte()
        buffer:readbyte()

        spbg:getChildByName("txt_wanfa"):setString(renshu .. "人");
        -- 额外玩法
        --游戏模式
        local gameMode = buffer:readbyte();
        --牌型选择
        local zadan = buffer:readbyte();
        local guizi = buffer:readbyte();
        local dijiuniangniang = buffer:readbyte();
        local tianjiuwang = buffer:readbyte();
        --道的选择
        local dao = buffer:readbyte();
        --庄的选择
        local bankerType = buffer:readbyte();
        --庄家亮牌
        local bankerShow = buffer:readbyte();


        local ruleStr = "";
        if gameMode == 1 then
            ruleStr = "大牌九、";
        elseif gameMode == 2 then
            ruleStr = "小牌九、";
        elseif gameMode == 3 then
            ruleStr = "加锅牌九、";
        end

        if zadan ~= 0 then
            ruleStr = ruleStr .. "炸弹、";
        else
            ruleStr = ruleStr .. "";
        end

        if guizi ~= 0 then
            ruleStr = ruleStr .. "鬼子、";
        else
            ruleStr = ruleStr .. "";
        end

        if dijiuniangniang ~= 0 then
            ruleStr = ruleStr .. "地九娘娘、";
        else
            ruleStr = ruleStr .. "";
        end

        if tianjiuwang ~= 0 then
            ruleStr = ruleStr .. "天九王、";
        else
            ruleStr = ruleStr .. "";
        end

        if dao == 2 then
            ruleStr = ruleStr .. "2道、";
        elseif dao == 3 then
            ruleStr = ruleStr .. "3道、";
        end

        if bankerType == 0 then
            ruleStr = ruleStr .. "轮庄、";
        elseif bankerType == 1 then
            ruleStr = ruleStr .. "抢庄、";
        elseif bankerType == 2 then
            ruleStr = ruleStr .. "霸王庄、";
        end

        if bankerShow == 0 then
            ruleStr = ruleStr .. "不亮";
        elseif bankerShow == 2 then
            ruleStr = ruleStr .. "亮2张";
        elseif bankerShow == 3 then
            ruleStr = ruleStr .. "亮3张";

        end

        -- 玩法
        spbg:getChildByName("txt_guize"):setString(ruleStr);
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