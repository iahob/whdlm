
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

-- 疯狂三张私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local ClubRoomCreateLayer = class("ClubRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
local QueryDialog = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubQueryDialog")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")

local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubGameSelectLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubGameSelectLayer")



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_HELP",             -- 帮助
    "BTN_CHARGE",           -- 充值
    "BTN_MYROOM",           -- 自己房间
    "BTN_CREATE",           -- 设置玩法
    "BTN_ENTERGAME",        -- 进入游戏
    "BTN_XUNZHANG",          -- 贡献开关
    "BTN_ALLGX",
    "BTN_MAXGX",
    "BTN_GXMODE1",
    "BTN_GXMODE2",

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
    "BTN_GONGXIAN",                 --贡献值

    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置
    "BTN_IMG1",
    "BTN_IMG2",
    "BTN_IMG3",
    "BTN_IMG4",
    "BTN_IMG5",
    "BTN_DELETE",
    "BTN_CHANGE",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_RULEBEGIN = 200
local CBT_BEGIN = 300

local CBT_PLAYER = 400
local CBT_JIFEN=500
local CBT_DIFEN=600
local CBT_MENPAI=700
local CBT_BIPAI=800
local CBT_XIAZHU=900

local dropDownMenuYPox = {20, 60, 100, 140, 180}
local dropDownBgHeight = {40, 80, 120, 160, 200}

-- 创建界面
function ClubRoomCreateLayer:ctor( scene,param,level )
    local this = self
    ClubRoomCreateLayer.super.ctor(self, scene, param, level)

    self.param = param

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/ClubRoomCreate.csb", self )
    self.m_csbNode = csbNode

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( btncallback )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    local ScrollView = spbg:getChildByName("ScrollView")
    
    self.m_spBg = spbg

    --关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener(btncallback)

    self.m_gameInfo = scene:getEnterGameInfo()

    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0

    self.gx_flag = true

    self.m_isCreate = true

    self.m_nodeScore = ScrollView:getChildByName("Panel_xz")
    self.m_nodeRule = ScrollView:getChildByName("Panel_rule")

    self.m_btnGx = spbg:getChildByName("btn_xunzhang")
    self.m_btnGx:setTag(TAG_ENUM.BTN_XUNZHANG)
    self.m_btnGx:addTouchEventListener(btncallback)

    spbg:getChildByName("gameName"):setString(self.m_gameInfo._GameName.."")

    local btn = spbg:getChildByName("btn_delete")
    btn:setTag(TAG_ENUM.BTN_DELETE)
    btn:addTouchEventListener(btncallback)
    btn:setVisible(false)

    self.lastData = {}
    if self.param.tabConfigInfo then
        self.m_isCreate = false
        btn:setVisible(true)
        self.gx_flag = self.param.tabConfigInfo.cbPlayMode == 1 and true or false
    else
        self.gx_flag = self.param.cbClubCurPlayMode == 1 and true or false

        for i=1,12 do
            self.lastData[i] = cc.UserDefault:getInstance():getIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."threemadnessData"..i, -1)
        end
    end


    -- if self.param.cbMedalMode ==0 then
    --     self.gx_flag = false
    --     self.m_btnGx:setVisible(false)
    -- end

    local btn = spbg:getChildByName("Button_change")
    btn:setTag(TAG_ENUM.BTN_CHANGE)
    btn:addTouchEventListener(btncallback)

    if not self.m_isCreate then
        btn:setVisible(false)
    end

    self.m_gxMode = 1  --贡献模式
    self.m_gxType = 1   --贡献来源
    -- 下拉菜单1
    btn = self.m_nodeScore:getChildByName("Image_6_0")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(true)
    self.m_btnDropDown1 = btn

    self.m_gxTxt = btn:getChildByName("Text_8_0")

    -- 下拉背景1
    local panel = self.m_nodeScore:getChildByName("Panel_3")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_1)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown1 = panel

    local btn = panel:getChildByName("Button_8")
    btn:setTag(TAG_ENUM.BTN_ALLGX)
    btn:addTouchEventListener(btncallback)


    local btn = panel:getChildByName("Button_8_0")
    btn:setTag(TAG_ENUM.BTN_MAXGX)
    btn:addTouchEventListener(btncallback)

    
    self.m_btnMode1 = self.m_nodeScore:getChildByName("btn_gx1")
    self.m_btnMode1:setTag(TAG_ENUM.BTN_GXMODE1)
    self.m_btnMode1:addTouchEventListener(btncallback)

    self.m_btnMode2 = self.m_nodeScore:getChildByName("btn_gx2")
    self.m_btnMode2:setTag(TAG_ENUM.BTN_GXMODE2)
    self.m_btnMode2:addTouchEventListener(btncallback)

    self:updateGongxianType(self.m_gxType)
    self:updateGongxianMode(self.m_gxMode)


    self.m_editBox ={}
    self.m_editText = {}
    self.m_editImg = {}
    
    local epoint = {cc.p(362,146),cc.p(864,146),cc.p(720,99),cc.p(864,58),cc.p(788,556)} 

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    self.m_oldText = {0,0,0,0,self.m_gameInfo._GameName}

    for i=1,5 do 
            -- 编辑框
        local editbox = ccui.EditBox:create(cc.size(212, 52),"public/public_blank.png")
                :setPosition(epoint[i])
                :setFontColor(cc.c3b(255, 231, 69))
                :setAnchorPoint(cc.p(0,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/FZHTJW.TTF", 29)
                :setFont("fonts/FZHTJW.TTF", 29) 
                :setMaxLength(12)
                --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
                --:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)  
        editbox:setTag(i)     
        self.m_editBox[i] = editbox
        self.m_editBox[i]:registerScriptEditBoxHandler(editHanlder)


        if i == 5 then
            self.m_spBg:addChild(editbox)
            self.m_editText[i]= spbg:getChildByName("editTxt5")
            self.m_editText[i]:setString(self.m_gameInfo._GameName.."")
            self.m_editBox[i]:setMaxLength(6)
            self.m_editBox[i]:setFontColor(cc.c3b(255, 255, 255))

            self.m_editImg[i] = spbg:getChildByName("img5")    
            -- self.m_editImg[i]:setPositionX(self.m_editText[i]:getPositionX() + self.m_editText[i]:getContentSize().width +30)
        else
            self.m_nodeScore:addChild(editbox)
            self.m_editText[i]= self.m_nodeScore:getChildByName("editTxt"..i)
            self.m_editText[i]:setString("0")
            self.m_editImg[i] = self.m_nodeScore:getChildByName("img"..i)
            self.m_editBox[i]:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        end

        self.m_editImg[i]:setTag(TAG_ENUM.BTN_IMG1+(i-1))
        self.m_editImg[i]:addTouchEventListener( btncallback )
        self.m_editImg[i]:setTouchEnabled(true)
    end

    self.m_editBox[3]:setVisible(false)

    self.m_exitSet = self.m_nodeScore:getChildByName("CheckBox_1")
    self.m_exitSet:setTouchEnabled(true)
    self.m_exitSet:setSelected(true)
    -- self.m_exitSet:addTouchEventListener(function(ref, tType)
    --             if tType == ccui.TouchEventType.ended then
    --                 --self.m_exitSet:setSelected(not self.m_exitSet:isSelected())
    --                 dump({self.m_exitSet:isSelected()})
    --             end
    --         end)

    self:updateXunZhang()

    --玩法
    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabRuleCheckBox = {}
    
    --最大下注轮数
    for i = 1, 3 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_2"):getChildByName("pay_2_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_XIAZHU + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_XIAZHU + i] = checkbx
        end
    end    
    self.selectXiaZhu = CBT_XIAZHU + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.selectXiaZhu]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --至少闷牌轮数
    for i = 1, 3 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_3"):getChildByName("pay_3_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_MENPAI + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_MENPAI + i] = checkbx
        end
    end    
    self.selectMenpai = CBT_MENPAI + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.selectMenpai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --允许比牌轮数
    for i = 1, 3 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_4"):getChildByName("pay_4_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_BIPAI + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_BIPAI + i] = checkbx
        end
    end
    self.selectBipai = CBT_BIPAI + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.selectBipai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --房间人数
    for i = 1, 4 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_5"):getChildByName("pay_5_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_PLAYER + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_PLAYER + i] = checkbx
        end
    end    
    self.m_nPlayerNumSelectIdx = CBT_PLAYER + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.m_nPlayerNumSelectIdx]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --每局积分
    for i = 1, 3 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_6"):getChildByName("pay_6_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_JIFEN + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_JIFEN + i] = checkbx
        end
    end    
    self.selectJifen = CBT_JIFEN + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.selectJifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --底注
    for i = 1, 3 do
        local checkbx = self.m_nodeRule:getChildByName("node_config_7"):getChildByName("pay_7_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_DIFEN + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            self.m_tabRuleCheckBox[CBT_DIFEN + i] = checkbx
        end
    end    
    self.selectDifen = CBT_DIFEN + 1
    -- self.m_tabRuleCheckBox[self.m_nSelectDiZhuIdx]:setSelected(true)
    self.m_tabRuleCheckBox[self.selectDifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")


    self.m_btnCreate = spbg:getChildByName("btn_ok")
    self.m_btnCreate:setTag(TAG_ENUM.BTN_CREATE)
    self.m_btnCreate:addTouchEventListener(btncallback)

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
function ClubRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    --self.m_txtCardNum:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
end

-- 刷新费用列表
function ClubRoomCreateLayer:onRefreshFeeList(isBtn)
    PriRoom:getInstance():dismissPopWait()
    local this = self


    self.m_tabFeeConfigList = self.gx_flag and PriRoom:getInstance().m_tabGoldRoomConfigList or PriRoom:getInstance().m_tabFeeConfigList
    
    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}

    local nConfig = #self.m_tabFeeConfigList
    -- 局数
    for i = 1, nConfig do
        local config = self.m_tabFeeConfigList[i]
        local checkbx = self.m_nodeRule:getChildByName("node_config_1"):getChildByName("pay_1_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_BEGIN + i)
            checkbx:addTouchEventListener(cbtlistener)
            self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            checkbx:getChildByName("Text_22"):setString(config.dwDrawCountLimit.."局")
            checkbx:getChildByName("Text_23"):setString("("..config.lFeeScore.."钻石)")
        end

    end
    -- 选择的玩法    
    self.m_nSelectIdx = CBT_BEGIN + 1
    self.m_tabSelectConfig = nil
    if nConfig > 0 then
        self.m_tabSelectConfig = self.m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]

        -- 更新费用
        self:updateCreateFee()

    end

    local bLimitTimeFree =   PriRoom:getInstance().m_bLimitTimeFree
    if self.gx_flag then
        bLimitTimeFree = PriRoom:getInstance().m_bGoldLimitTimeFree
    end
    -- 免费判断
    if bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- -- 免费时间
        -- local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        -- self.m_spBg:getChildByName("txt_feetime"):setString(szfree)
        for k,v in pairs(self.m_tabCheckBox) do
            if v then
                v:getChildByName("Text_23"):setString("(免费)")
            end
        end
    end


    --入场，离场分数设置默认值
    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
    if bPirGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore
        self.m_nSelectScore = self.m_cellScore

        if nil ~= PriRoom:getInstance().m_tabGoldRoomOption then

            self.m_editText[1]:setString(self.m_cellScore)
            self.m_editText[2]:setString(self.m_enterScore)
            self.m_editText[3]:setString(self.m_exitScore)
            self.m_oldText = {self.m_cellScore,self.m_enterScore,self.m_exitScore,0,self.m_gameInfo._GameName}
        end
    end

    if not self.m_isCreate and not isBtn  then
        self:updateConfigSetting()
    end 

end

function ClubRoomCreateLayer:onRefreshRule()
    if self.m_isCreate and self.lastData[1] ~= -1 then 
        self:updateLastData()
    end
end

function ClubRoomCreateLayer:updateConfigSetting()
    self.gx_flag = self.param.tabConfigInfo.cbPlayMode == 1 and true or false
    self:updateXunZhang()

    self.m_oldText = {self.param.tabConfigInfo.lCellScore,
    self.param.tabConfigInfo.lEnterScore,
    self.param.tabConfigInfo.lLeaveScore,
    self.param.tabConfigInfo.dwTaxCount,
    self.param.tabConfigInfo.szConfigName}
    for i=1,5 do
        self.m_editText[i]:setString(self.m_oldText[i].."")
    end
    -- self.m_editImg[5]:setPositionX(self.m_editText[5]:getPositionX() + self.m_editText[5]:getContentSize().width +30)

    self:updateGongxianMode(math.floor(self.param.tabConfigInfo.cbTaxMode/3)+1)
    self:updateGongxianType(math.floor(self.param.tabConfigInfo.cbTaxMode%2) ==1 and 1 or 2)

    self.m_nSelectScore = self.param.tabConfigInfo.lCellScore
    self.m_enterScore = self.param.tabConfigInfo.lEnterScore

    local count = #self.m_tabFeeConfigList
    for i = 1, (count >5 and 5 or count) do
        local config = self.m_tabFeeConfigList[i]
        -- local checkbx = self.m_nodeRule:getChildByName("pay" .. i)
        local checkbx = self.m_nodeRule:getChildByName("node_config_1"):getChildByName("pay_1_" .. i)
        if checkbx ~= nil then
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
        end

        if self.param.tabConfigInfo.dwDrawCountLimit == config.dwDrawCountLimit then
            self.m_nSelectIdx = CBT_BEGIN + i
            self.m_tabSelectConfig = self.m_tabFeeConfigList[i]
            self:updateCreateFee()
        end 
    end

    self.m_exitSet:setSelected(self.param.tabConfigInfo.cbLessScoreDismiss == 1 and true or false)

    --游戏规则
    local cbGameRule = self.param.tabConfigInfo.cbGameRule[1];
    --最大下注轮数
    local XiaZhuTab = {10,15,20}
    local index = 1;
    for i = 1, #XiaZhuTab do
        if cbGameRule[8] == XiaZhuTab[i] then
            index = i;
        end
    end
    self.m_tabRuleCheckBox[self.selectXiaZhu]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectXiaZhu = index + CBT_XIAZHU
    self.m_tabRuleCheckBox[self.selectXiaZhu]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --至少闷牌轮数
    self.m_tabRuleCheckBox[self.selectMenpai]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectMenpai = cbGameRule[7] + CBT_MENPAI
    self.m_tabRuleCheckBox[self.selectMenpai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --允许比牌轮数
    self.m_tabRuleCheckBox[self.selectBipai]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectBipai = cbGameRule[6] + CBT_BIPAI
    self.m_tabRuleCheckBox[self.selectBipai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --房间人数
    self.m_tabPlayNumSelect = {2, 3, 4, 5}
    local index = 1;
    for i = 1, #self.m_tabPlayNumSelect do
        if cbGameRule[3] == self.m_tabPlayNumSelect[i] then
            index = i;
        end
    end
    self.m_tabRuleCheckBox[self.m_nPlayerNumSelectIdx]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.m_nPlayerNumSelectIdx = index + CBT_PLAYER
    self.m_tabRuleCheckBox[self.m_nPlayerNumSelectIdx]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --每局积分
    self.m_tabRuleCheckBox[self.selectJifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectJifen = cbGameRule[4] + CBT_JIFEN
    self.m_tabRuleCheckBox[self.selectJifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --底注
    self.m_tabRuleCheckBox[self.selectDifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectDifen = cbGameRule[5] + CBT_DIFEN
    self.m_tabRuleCheckBox[self.selectDifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
end

function ClubRoomCreateLayer:updateLastData()
    self.m_oldText = {self.lastData[1],
    self.lastData[4],
    self.lastData[5],
    self.lastData[7],
    self.m_gameInfo._GameName}

    for i=1,5 do
        self.m_editText[i]:setString(self.m_oldText[i].."")
    end
    --self.m_editImg[5]:setPositionX(self.m_editText[5]:getPositionX() + self.m_editText[5]:getContentSize().width +30)

    self:updateGongxianMode(math.floor(self.lastData[6]/3)+1)
    self:updateGongxianType(math.floor(self.lastData[6]%2) ==1 and 1 or 2)

    self.m_nSelectScore = self.lastData[1]
    self.m_enterScore = self.lastData[4]

    local count = #self.m_tabFeeConfigList
    for i = 1, (count >5 and 5 or count) do
        local config = self.m_tabFeeConfigList[i]
        local checkbx = self.m_nodeRule:getChildByName("pay" .. i)
        if checkbx ~= nil then
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
        end

        if self.lastData[2] == config.dwDrawCountLimit then
            self.m_nSelectIdx = CBT_BEGIN + i
            self.m_tabSelectConfig = self.m_tabFeeConfigList[i]
            self:updateCreateFee()
        end 
    end
    --游戏规则
    --最大下注轮数
    self.m_tabRuleCheckBox[self.selectXiaZhu]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectXiaZhu = self.lastData[12] + CBT_XIAZHU
    self.m_tabRuleCheckBox[self.selectXiaZhu]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --至少闷牌轮数
    self.m_tabRuleCheckBox[self.selectMenpai]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectMenpai = self.lastData[11] + CBT_MENPAI
    self.m_tabRuleCheckBox[self.selectMenpai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --允许比牌轮数
    self.m_tabRuleCheckBox[self.selectBipai]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectBipai = self.lastData[10] + CBT_BIPAI
    self.m_tabRuleCheckBox[self.selectBipai]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --房间人数
    self.m_tabRuleCheckBox[self.m_nPlayerNumSelectIdx]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.m_nPlayerNumSelectIdx = self.lastData[3] + CBT_PLAYER
    self.m_tabRuleCheckBox[self.m_nPlayerNumSelectIdx]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --每局积分
    self.m_tabRuleCheckBox[self.selectJifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectJifen = self.lastData[8] + CBT_JIFEN
    self.m_tabRuleCheckBox[self.selectJifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")

    --底注

    self.m_tabRuleCheckBox[self.selectDifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    self.selectDifen = self.lastData[9] + CBT_DIFEN
    self.m_tabRuleCheckBox[self.selectDifen]:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
end

function ClubRoomCreateLayer:sendGroupConfig()
            local buffer = nil
            if self.m_isCreate then
                buffer = CCmd_Data:create()--ExternalFun.create_netdata(cmd_Club.SUB_GA_AppendConfig)
                buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_APPEND_CONFIG)
            else
                buffer = CCmd_Data:create()--ExternalFun.create_netdata(cmd_Club.SUB_GA_ModifyConfig)
                buffer:setcmdinfo(cmd_Club.MDM_GA_LOGIC_SERVICE,cmd_Club.SUB_GA_MODIFY_CONFIG)
            end

            buffer:pushdword(GlobalUserItem.tabAccountInfo.dwUserID) --玩家ID
            buffer:pushdword(ClubFrame:getInstance().m_nCurrentClub) --俱乐部ID
            print(ClubFrame:getInstance().m_nCurrentClub,"====")

            if not self.m_isCreate then
                buffer:pushdword(self.param.tabConfigInfo.dwConfigID) --玩法ID
            end
            buffer:pushstring(self.m_editText[5]:getString(),13)  --玩法名
            if self.m_isCreate then
                buffer:pushword(self.m_gameInfo._KindID)    --游戏ID
            end
            buffer:pushbyte(self.gx_flag and 1 or 0) --结算模式0积分，1勋章
            buffer:pushscore(self.gx_flag and self.m_nSelectScore or 1) --底分设置
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawTimeLimit or 10)
            self.m_tabPlayNumSelect = {2, 3, 4, 5}
            
            if self.gx_flag then
                buffer:pushword(self.m_tabPlayNumSelect[4])    --游戏人数
            else
                buffer:pushword(self.m_tabPlayNumSelect[self.m_nPlayerNumSelectIdx -CBT_PLAYER])    --游戏人数
            end

            if self.m_isCreate then
                buffer:pushdword(0)   
            end

            buffer:pushbyte(0)--无密码
            buffer:pushbyte(0)--支付方式，一人支付

            -- 附加信息
            buffer:pushbyte(1)
            buffer:pushbyte(0)

            local XiaZhuTab = {10,15,20}
            
            if self.gx_flag then
                buffer:pushbyte(self.m_tabPlayNumSelect[4])    --游戏人数
            else
                buffer:pushbyte(self.m_tabPlayNumSelect[self.m_nPlayerNumSelectIdx -CBT_PLAYER])
            end
            
            buffer:pushbyte(self.selectJifen-CBT_JIFEN)
            buffer:pushbyte(self.selectDifen-CBT_DIFEN)
            buffer:pushbyte(self.selectBipai-CBT_BIPAI)
            buffer:pushbyte(self.selectMenpai-CBT_MENPAI)
            buffer:pushbyte(XiaZhuTab[self.selectXiaZhu-CBT_XIAZHU])

            cc.UserDefault:getInstance():setIntegerForKey("land_baseScore", self.selectDifen-CBT_DIFEN)

            for i = 1, 92 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)
            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)
            if self.m_isCreate then
                buffer:pushbyte(1)
            end

            buffer:pushbyte((self.m_gxMode == 1 and 0 or 2)  + self.m_gxType)   --税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家

            buffer:pushdword(tonumber(self.m_editText[4]:getString()))   --税收值

            buffer:pushbyte(self.m_exitSet:isSelected() and 1 or 0)

            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendGroupConfigInfo(buffer)

            --self:removeFromParent()
            local lastData = {}
            --通用
            table.insert(lastData,self.gx_flag and self.m_nSelectScore or 1)
            table.insert(lastData,self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            table.insert(lastData,self.m_nPlayerNumSelectIdx -CBT_PLAYER)
            table.insert(lastData,self.m_enterScore)
            table.insert(lastData,self.m_exitScore)
            table.insert(lastData,(self.m_gxMode == 1 and 0 or 2)  + self.m_gxType)   --税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家
            table.insert(lastData,tonumber(self.m_editText[4]:getString()))   --税收值

            --玩法
            table.insert(lastData,self.selectJifen-CBT_JIFEN);
            table.insert(lastData,self.selectDifen-CBT_DIFEN);

            table.insert(lastData,self.selectBipai-CBT_BIPAI);
            table.insert(lastData,self.selectMenpai-CBT_MENPAI);
            table.insert(lastData,self.selectXiaZhu-CBT_XIAZHU);

            if self.m_isCreate then --保存当前配置
                for i=1, 12 do
                    cc.UserDefault:getInstance():setIntegerForKey(GlobalUserItem.tabAccountInfo.dwUserID.."threemadnessData"..i, lastData[i])
                end
            end
end


function ClubRoomCreateLayer:onConfigSuccess()
        self:removeFromParent()
end

function ClubRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local dwRoomID = roomDetailInfo.dwRoomID or 0
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0
    local baseScore = "底分:"..cc.UserDefault:getInstance():getIntegerForKey("land_baseScore",0)..","

    local shareTxt = "局数:" .. turnCount .. "局。"..baseScore.."疯狂三张游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=56&a=1" .. "&p=" .. passwd
    print("获取分享信息===》", dwRoomID,shareTxt,  url)
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function ClubRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("疯狂三张, 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
end

function ClubRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function ClubRoomCreateLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ClubRoomCreateLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ClubRoomCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubRoomCreateLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubRoomCreateLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubRoomCreateLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

function ClubRoomCreateLayer:updateXunZhang()

    local res1= self.gx_flag and "room/switch2.png" or "room/switch1.png"
    self.m_btnGx:loadTextureNormal(res1)
    self.m_btnGx:loadTexturePressed(res1)
    self.m_nodeScore:setVisible(self.gx_flag)
    self.m_nodeRule:setPositionY(self.gx_flag and 510 or self.m_nodeScore:getPositionY()-30)

    local node_config_5 = self.m_nodeRule:getChildByName("node_config_5");
    local node_config_6 = self.m_nodeRule:getChildByName("node_config_6");
    local node_config_7 = self.m_nodeRule:getChildByName("node_config_7");
    node_config_5:setVisible(not self.gx_flag)
    node_config_6:setVisible(not self.gx_flag)
    node_config_7:setVisible(not self.gx_flag)

end

function ClubRoomCreateLayer:updateGongxianMode(gxMode)

    self.m_gxMode = gxMode
    self.m_btnMode1:getChildByName("Image_8"):setVisible(gxMode==1 and true or false)
    self.m_btnMode2:getChildByName("Image_8"):setVisible(gxMode==2 and true or false)

    self.m_nodeScore:getChildByName("Sprite_7_0"):setVisible(gxMode==1 and true or false)
    self.m_nodeScore:getChildByName("per"):setVisible(gxMode==2 and true or false)
    self.m_nodeScore:getChildByName("Panel_3"):getChildByName("Text_19"):setString(gxMode==1 and "所有玩家贡献" or "所有赢家贡献")
    self.m_gxTxt:setString(self.m_gxType==1 and (self.m_gxMode == 1 and "所有玩家贡献" or "所有赢家贡献") or "大赢家贡献")


end

function ClubRoomCreateLayer:updateGongxianType(gxType)
    
    self.m_gxTxt:setString(gxType==1 and (self.m_gxMode == 1 and "所有玩家贡献" or "所有赢家贡献") or "大赢家贡献")
    self.m_gxType = gxType
    self.m_maskDropDown1:setVisible(false)

end

function ClubRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        if self.m_bBeginEdit == true then
            self.m_bBeginEdit = false
            return
        end
        self:removeFromParent()
    elseif TAG_ENUM.BTN_XUNZHANG == tag then
        self.gx_flag = not self.gx_flag
        self:updateXunZhang()
        self:onRefreshFeeList(true)
    elseif TAG_ENUM.BTN_DELETE == tag then
        self.m_Query = QueryDialog:create("确认是否删除该玩法！", function(ok)
            if ok then
                ClubFrame:getInstance():sendDeleteGroupConfig(ClubFrame:getInstance().m_nCurrentClub,self.param.tabConfigInfo.dwConfigID)
                self:removeFromParent()
            end
            self.m_Query = nil
        end, 26, QueryDialog.QUERY_SURE_CANCEL)
        :setCanTouchOutside(false)
        :addTo(self._scene)
    elseif TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
    elseif TAG_ENUM.BTN_CREATE == tag then 

        if nil == self.m_tabSelectConfig or table.nums(self.m_tabSelectConfig) == 0 then
            showToast(self, "未选择玩法配置!", 2)
            return
        end

        if true == ExternalFun.isContainBadWords(self.m_editText[5]:getString()) then
            showToast(self, "大联盟名称包含敏感词汇!", 3)
            return
        end 

        --PriRoom:getInstance():setViewFrame(self)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = false--(TAG_ENUM.BTN_CREATE_1 == tag)
        --私有房模式
        PriRoom:getInstance(). m_bPlayModel = 0
        -- 创建房间
        --PriRoom:getInstance():showPopWait()
        self:sendGroupConfig()

    elseif TAG_ENUM.MENU_DROPDOWN_1 == tag then

        self.m_maskDropDown1:setVisible(true)

    elseif TAG_ENUM.MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(false)

    elseif TAG_ENUM.BTN_ALLGX == tag then
        self:updateGongxianType(1)
    elseif TAG_ENUM.BTN_MAXGX == tag then
        self:updateGongxianType(2)
    elseif TAG_ENUM.BTN_GXMODE1 == tag then
        self:updateGongxianMode(1)
    elseif TAG_ENUM.BTN_GXMODE2 == tag then
        self:updateGongxianMode(2)
    elseif TAG_ENUM.BTN_IMG1 <= tag and TAG_ENUM.BTN_IMG5 >= tag then
        local index = tag - TAG_ENUM.BTN_IMG1 +1
        self.m_editBox[index]:touchDownAction(self.m_editBox[index], ccui.TouchEventType.ended)
        self.m_editImg[index]:setEnabled(false)
    elseif TAG_ENUM.BTN_CHANGE == tag then
        local param = {}
        param.cbMedalMode = self.param.cbMedalMode
        local pa = ClubGameSelectLayer:create(self._scene, param, ClubGameSelectLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
        self:removeFromParent()
    end
end

--玩法选择
function ClubRoomCreateLayer:onRuleSelectedEvent(tag, sender)
    print("==========================>", tag)

    if tag > CBT_PLAYER and tag < CBT_PLAYER+10 then --人数
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_PLAYER and k < CBT_PLAYER+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.m_nPlayerNumSelectIdx = tag
    end

    if tag > CBT_JIFEN and tag < CBT_JIFEN+10 then --积分
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_JIFEN and k < CBT_JIFEN+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.selectJifen = tag
    end

    if tag > CBT_DIFEN and tag < CBT_DIFEN+10 then --底注
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_DIFEN and k < CBT_DIFEN+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.selectDifen = tag
    end

    if tag > CBT_MENPAI and tag < CBT_MENPAI+10 then --闷牌
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_MENPAI and k < CBT_MENPAI+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.selectMenpai = tag
    end

    if tag > CBT_BIPAI and tag < CBT_BIPAI+10 then --比牌
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_BIPAI and k < CBT_BIPAI+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.selectBipai = tag
    end

    if tag > CBT_XIAZHU and tag < CBT_XIAZHU+10 then --下注
        for k,v in pairs(self.m_tabRuleCheckBox) do
            if k ~= tag and k > CBT_XIAZHU and k < CBT_XIAZHU+10 then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
            end
            if k == tag then
                v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
            end
        end
        self.selectXiaZhu = tag
    end

end

function ClubRoomCreateLayer:onEditEvent(event, editbox)
      local index = editbox:getTag()
      local text = self.m_editText[index]
      local img = self.m_editImg[index]
      local oldTxt = self.m_oldText[index]

        local src = editbox:getText() 
        if event == "return" then
            print("return"..src)
            --self.m_bBeginEdit = false
            text:setVisible(true)
            img:setEnabled(true)
            text:setString(src)
            if text:getContentSize().width == 0 then --没有输入
                text:setString(oldTxt.."")
            else
                self:setConfigString(index,tonumber(src))
            end

            -- if index ==5 then

            --     img:setPositionX(text:getPositionX() + text:getContentSize().width +30)
            --     self.m_oldText[5] = text:getString()
            -- end
            
        elseif event == "changed" then     
            print("changed"..src)  
            if string.len(src) ~= 0 and src ~= "" then
                text:setString(src)
            else
                text:setString(oldTxt.."")
            end
            -- if index ==5 then
            --     img:setPositionX(text:getPositionX() + text:getContentSize().width +30)
            -- end

        elseif event == "began" then
            print("began"..src)
            self.m_bBeginEdit = true
            --editbox:setText(oldTxt.."")
            text:setVisible(false)
            -- if string.len(src) ~= 0 and src ~= "" then
            --     text:setString(src)
            -- else
            --     if index ==5 then
            --         text:setString(self.m_gameInfo._GameName)       
            --     else
            --         text:setString("0")          
            --     end
            -- end
        end

end


--输入框确定时调用，用来设置数据
function ClubRoomCreateLayer:setConfigString(tag, score)
    if 1 == tag then
        --设置了底分，入场和离场也会变化
        self.m_cellScore = score
        if self.m_cellScore > PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore
        elseif self.m_cellScore < PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        end
        self.m_nSelectScore = self.m_cellScore

        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore


        self.m_oldText[1] =self.m_cellScore
        self.m_oldText[2] =self.m_enterScore
        self.m_oldText[3] =self.m_exitScore
        for i=1,3 do
            self.m_editText[i]:setString(self.m_oldText[i].."")
            -- self.m_editBox[i]:setText(self.m_oldText[i].."")
            self.m_editBox[i]:setText("")
        end
        
    elseif 2 == tag then
        --入场设定要判断底分,大于最小值才生效
        if score >= self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore then
            self.m_enterScore = score
             
            --入场设定设置了，会改变离场设定
            self.m_exitScore = self.m_enterScore
        end
        self.m_oldText[2] =self.m_enterScore
        self.m_oldText[3] =self.m_exitScore
        for i=2,3 do
            self.m_editText[i]:setString(self.m_oldText[i].."")
            -- self.m_editBox[i]:setText(self.m_oldText[i].."")
            self.m_editBox[i]:setText("")
        end
    elseif 3 == tag then
        --计算最小值，可能不能整除
        local minExit = math.floor(self.m_enterScore/2)
        if (self.m_enterScore - minExit *2) ~= 0 then --不能整除
            minExit = math.floor(self.m_enterScore/2) +1   --不能整除要往上
        end
        if score < self.m_enterScore then
            self.m_exitScore = score
            if self.m_exitScore < minExit then
                self.m_exitScore = minExit
            end
            
        end
        self.m_editText[3]:setString(self.m_exitScore)
        self.m_oldText[3] =self.m_exitScore
        -- self.m_editBox[3]:setText(self.m_exitScore.."")
        self.m_editBox[3]:setText("")
    elseif  4 == tag then
        self.m_oldText[4] =score
        -- self.m_editBox[4]:setText(score.."")
        self.m_editBox[4]:setText("")
    elseif  5 == tag then
        self.m_oldText[5] =score
        -- self.m_editBox[4]:setText(score.."")
        self.m_editBox[5]:setText("")
    end
end

function ClubRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end

function ClubRoomCreateLayer:onSelectedEvent(tag, sender)
    if self.m_nSelectIdx == tag then
        return
    end

    self.m_nSelectIdx = tag
    self.m_tabSelectConfig = self.m_tabFeeConfigList[tag - CBT_BEGIN]

    -- 更新费用
    self:updateCreateFee()
end

function ClubRoomCreateLayer:updateCreateFee()

    for k,v in pairs(self.m_tabCheckBox) do

        if v then
            v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
        end
    end

    local btn = self.m_tabCheckBox[self.m_nSelectIdx]
    btn:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")


    self.m_bLow = false
    if GlobalUserItem.tabAccountInfo.lDiamond < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end

end

return ClubRoomCreateLayer