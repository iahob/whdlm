
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

-- 龙虎斗私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local ClubRoomCreateLayer = class("ClubRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
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

    "BTN_BANKER_1",         -- 上庄局数1
    "BTN_BANKER_2",         -- 上庄局数2
    "BTN_BANKER_3",         -- 上庄局数3
    "BTN_BANKER_4",         -- 上庄局数4
    "BTN_BANKER_5",         -- 上庄局数5

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

    self.lMaxUserLimitScore = 10000;
    self.lMaxApplyBankerCondition = 10000;
    self.wBankerTimeArray = {5,10,15,20,25};
    self.lIniUserLimitScore = 100;
    self.lIniApplyBankerCondition = 100;


    self.gx_flag = true

    self.m_isCreate = true

    self.m_nodeScore = ScrollView:getChildByName("Panel_xz")

    spbg:getChildByName("gameName"):setString(self.m_gameInfo._GameName.."")

    local btn = spbg:getChildByName("btn_delete")
    btn:setTag(TAG_ENUM.BTN_DELETE)
    btn:addTouchEventListener(btncallback)
    btn:setVisible(false)

    if self.param.tabConfigInfo then
        self.m_isCreate = false
        btn:setVisible(true)
        self.gx_flag = self.param.tabConfigInfo.cbPlayMode == 1 and true or false
    end

    if self.param.cbMedalMode ==0 then
        self.gx_flag = false
    end

    local btn = spbg:getChildByName("Button_change")
    btn:setTag(TAG_ENUM.BTN_CHANGE)
    btn:addTouchEventListener(btncallback)

    if not self.m_isCreate then
        btn:setVisible(false)
    end

    self.m_gxMode = 2  --贡献模式、比例贡献
    self.m_gxType = 1   --贡献来源
    -- 下拉菜单1
    local btn = self.m_nodeScore:getChildByName("Image_6")
    -- btn:setTag(TAG_ENUM.MENU_DROPDOWN_1)
    -- btn:addTouchEventListener(btncallback)
    -- btn:setEnabled(true)
    -- self.m_btnDropDown1 = btn

    local Sprite_9 = btn:getChildByName("Sprite_9")
    local Image_11 = btn:getChildByName("Image_11")
    Sprite_9:setVisible(false);
    Image_11:setVisible(false);

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

    self:updateGongxianType(self.m_gxType)
    self.m_banker_num = 1;  --上庄局数

    self.m_editBox ={}
    self.m_editText = {}
    self.m_editImg = {}
    local epoint = {cc.p(554,28),cc.p(244,-64),cc.p(244,-134)} 

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    self.m_oldText = {0,0,0}

    local node_edit = self.m_nodeScore:getChildByName("node_edit");
    for i=1,3 do 
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
        node_edit:addChild(editbox)    
        editbox:setTag(i)     
        self.m_editBox[i] = editbox
        self.m_editBox[i]:registerScriptEditBoxHandler(editHanlder)

        self.m_editText[i]= self.m_nodeScore:getChildByName("editTxt_"..i)
        self.m_editText[i]:setString("0")
        self.m_editImg[i] = self.m_nodeScore:getChildByName("img_"..i)
        self.m_editBox[i]:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)

        self.m_editImg[i]:setTag(TAG_ENUM.BTN_IMG1+(i-1))
        self.m_editImg[i]:addTouchEventListener( btncallback )
        self.m_editImg[i]:setTouchEnabled(true)
    end

    --玩法
    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabRuleCheckBox = {}
    
    --个人下注限制
    --上庄配置
    --上庄局数
    -- 下拉菜单1
    local node_config = self.m_nodeScore:getChildByName("node_config_3");
    local btn = node_config:getChildByName("Image_6")
    btn:setTag(TAG_ENUM.MENU_DROPDOWN_2)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(true)
    self.m_btnDropDown2 = btn

    self.m_gxTxt_banker = btn:getChildByName("Text_8_0")

    -- 下拉背景1
    local panel = node_config:getChildByName("Panel_4")
    panel:setTag(TAG_ENUM.MASK_DROPDOWN_2)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    self.m_maskDropDown2 = panel

    for i = 1, 5 do
        local btn = panel:getChildByName("Button_" .. i);
        btn:setTag(TAG_ENUM.BTN_BANKER_1 + i - 1);
        btn:addTouchEventListener(btncallback);
        local Text_1 = panel:getChildByName("Text_" .. i);
        Text_1:setString("");
    end
    --游戏时间

    self.m_btnCreate = spbg:getChildByName("btn_ok")
    self.m_btnCreate:setTag(TAG_ENUM.BTN_CREATE)
    self.m_btnCreate:addTouchEventListener(btncallback)

    --初始化规则
    self:onInintRule();

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
    -- 游戏时间
    for i = 1, 5 do
        local config = self.m_tabFeeConfigList[i];
        local checkbx = self.m_nodeScore:getChildByName("node_config_4"):getChildByName("pay_4_" .. i);
        if i > nConfig then
            checkbx:setVisible(false);
        else
            if nil ~= checkbx then
                checkbx:setVisible(true)
                checkbx:setTouchEnabled(true)
                checkbx:setTag(CBT_BEGIN + i)
                checkbx:addTouchEventListener(cbtlistener)
                self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
                checkbx:getChildByName("Sprite_11"):setTexture("room/dt_check_1.png")
                checkbx:getChildByName("Text_22"):setString(config.dwDrawTimeLimit.."分钟")
                checkbx:getChildByName("Text_23"):setString("("..config.lFeeScore.."钻石)")
            end
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
    end

    if not self.m_isCreate and not isBtn  then
        self:updateConfigSetting()
    end 

end

--游戏特殊配置
function ClubRoomCreateLayer:onRefreshRule()
     local ruleT = 
    {
        { k = "lIniUserLimitScore",t = "score"},  --个人下注限制初始值
        { k = "lMaxUserLimitScore",t = "score"},  --个人下注限制
        { k = "lIniApplyBankerCondition",t = "score"},  --上庄配置初始值
        { k = "lMaxApplyBankerCondition",t = "score"},  --上庄配置
        { k = "wBankerTimeArray",t = "word",l = {5}},  --5个上庄局数
    }
    -- print("================="..PriRoom:getInstance().m_tabRule:getlen())
    local cmd_table1 = ExternalFun.read_netdata(ruleT, PriRoom:getInstance().m_tabRule) --积分房配置

    local lastLen = 1024 - PriRoom:getInstance().m_tabRule:getcurlen() + self._cmd_pri.define.LEN_PERSONAL_OTHER

    ExternalFun.read_netdata({{ k = "emptyType",t = "byte",l = {lastLen}}}, PriRoom:getInstance().m_tabRule)--去掉中间多余字节

    local cmd_table2 = ExternalFun.read_netdata(ruleT, PriRoom:getInstance().m_tabRule)--金币房配置
  
    self.ruleData = cmd_table2
    dump(self.ruleData,"-----------游戏特殊配置-------", 6)

    self.lMaxUserLimitScore = cmd_table2.lMaxUserLimitScore;
    self.lMaxApplyBankerCondition = cmd_table2.lMaxApplyBankerCondition;
    self.wBankerTimeArray = cmd_table2.wBankerTimeArray[1];
    self.lIniUserLimitScore = cmd_table2.lIniUserLimitScore;
    self.lIniApplyBankerCondition = cmd_table2.lIniApplyBankerCondition;
    self:onInintRule();
end

function ClubRoomCreateLayer:onInintRule()
    self.m_oldText = {0,self.lIniUserLimitScore,self.lIniApplyBankerCondition}

    --刷新上庄局数
    for i = 1, 5 do
        local Panel = self.m_nodeScore:getChildByName("node_config_3"):getChildByName("Panel_4");
        local Image_1 = Panel:getChildByName("Image_" .. i);
        local Text_1 = Panel:getChildByName("Text_" .. i);
        local Button_1 = Panel:getChildByName("Button_" .. i);

        if self.wBankerTimeArray[i] > 0 then
            Image_1:setVisible(true);
            Text_1:setVisible(true);
            Button_1:setVisible(true);
            Text_1:setString(self.wBankerTimeArray[i] .. "局");
        else
            Image_1:setVisible(false);
            Text_1:setVisible(false);
            Button_1:setVisible(false);
        end
    end

    if self.m_isCreate  then
        self.m_banker_num = self.wBankerTimeArray[1];
        self:selectShangZhuangJuShu(self.m_banker_num);

        --刷新下注、上庄
        self.m_editText[2]:setString(self.lIniUserLimitScore)
        self.m_editText[3]:setString(self.lIniApplyBankerCondition)
    end
end

--编辑玩法
function ClubRoomCreateLayer:updateConfigSetting()
    -- dump(self.param.tabConfigInfo,"------------编辑玩法-----------",6)
    local buffer = self.param.tabConfigInfo.cbGameRule[1];
    local lIniUserLimitScore = ExternalFun.bytesToInt(buffer, 2, 9);
    local lIniApplyBankerCondition = ExternalFun.bytesToInt(buffer, 10, 17);
    local bankerNum = ExternalFun.bytesToInt(buffer, 18, 21);

    self.m_oldText = {self.param.tabConfigInfo.dwTaxCount,
    lIniUserLimitScore,
    lIniApplyBankerCondition  }

    self.m_banker_num = bankerNum;
    
    --贡献、下注、上庄配置
    for i=1,3 do
        self.m_editText[i]:setString(self.m_oldText[i].."")
    end
    
    --贡献方式
    self:updateGongxianType(math.floor(self.param.tabConfigInfo.cbTaxMode%2) ==1 and 1 or 2)

    --上庄局数
    self:selectShangZhuangJuShu(self.m_banker_num)

    self.m_nSelectScore = self.param.tabConfigInfo.lCellScore
    self.m_enterScore = self.param.tabConfigInfo.lEnterScore

    --游戏时间
    local count = #self.m_tabFeeConfigList
    for i = 1, (count >5 and 5 or count) do
        local config = self.m_tabFeeConfigList[i]
        local checkbx = self.m_nodeScore:getChildByName("node_config_4"):getChildByName("pay_4_" .. i)
        if checkbx ~= nil then
            checkbx:getChildByName("Sprite_11"):setTexture("room/dt_check_1.png")
        end

        if self.param.tabConfigInfo.dwDrawTimeLimit == config.dwDrawTimeLimit then
            self.m_nSelectIdx = CBT_BEGIN + i
            self.m_tabSelectConfig = self.m_tabFeeConfigList[i]
            self:updateCreateFee()
        end 
    end
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
            buffer:pushstring("龙虎斗",13)  --玩法名
            if self.m_isCreate then
                buffer:pushword(self.m_gameInfo._KindID)    --游戏ID
            end
            buffer:pushbyte(1) --结算模式0积分，1勋章
            buffer:pushscore(self.gx_flag and self.m_nSelectScore or 1) --底分设置
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawTimeLimit or 10)
            self.m_tabPlayNumSelect = {2, 3, 4, 5}
            
            buffer:pushword(20)    --游戏人数

            if self.m_isCreate then
                buffer:pushdword(0)   
            end

            buffer:pushbyte(0)--无密码
            buffer:pushbyte(0)--支付方式，一人支付

            -- 附加信息
            buffer:pushbyte(1) -- 额外规则

            -- 个人下注限制
            buffer:pushscore(tonumber(self.m_editText[2]:getString()))
            --上庄条件
            buffer:pushscore(tonumber(self.m_editText[3]:getString()))
            --上庄局数
            buffer:pushword(self.m_banker_num or 1)
            -- 游戏时间

            for i = 1, 81 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)
            -- buffer:pushbyte(1)
            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)
            if self.m_isCreate then
                buffer:pushbyte(1)
            end

            buffer:pushbyte((self.m_gxMode == 1 and 0 or 2)  + self.m_gxType)   --税收模式：1固定模式所有玩家，2固定模式大赢家，3比例模式所有玩家，4比例模式大赢家

            buffer:pushdword(tonumber(self.m_editText[1]:getString()))   --税收值

            buffer:pushbyte(0)  -- 离场设定

            ClubFrame:getInstance().m_bBattleGameCreate = self.m_gameInfo._KindID
            ClubFrame:getInstance():setViewFrame(self)
            ClubFrame:getInstance():sendGroupConfigInfo(buffer)

end





function ClubRoomCreateLayer:onConfigSuccess()
        self:removeFromParent()
end

function ClubRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local dwRoomID = roomDetailInfo.dwRoomID or 0
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0
    local baseScore = "底分:"..cc.UserDefault:getInstance():getIntegerForKey("land_baseScore",0)..","

    local shareTxt = "局数:" .. turnCount .. "局。"..baseScore.."龙虎斗游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=56&a=1" .. "&p=" .. passwd
    print("获取分享信息===》", dwRoomID,shareTxt,  url)
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function ClubRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("龙虎斗, 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
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

end

function ClubRoomCreateLayer:updateGongxianMode(gxMode)
    self.m_gxMode = gxMode
    self.m_nodeScore:getChildByName("Sprite_7_0"):setVisible(gxMode==1 and true or false)
    self.m_nodeScore:getChildByName("per"):setVisible(gxMode==2 and true or false)
    self.m_nodeScore:getChildByName("Panel_3"):getChildByName("Text_19"):setString(gxMode==1 and "所有玩家贡献" or "所有赢家贡献")
    self.m_gxTxt:setString(self.m_gxType==1 and (self.m_gxMode == 1 and "所有玩家贡献" or "所有赢家贡献") or "大赢家贡献")
end
--贡献方式
function ClubRoomCreateLayer:updateGongxianType(gxType)
    self.m_gxTxt:setString(gxType==1 and (self.m_gxMode == 1 and "所有玩家贡献" or "所有赢家贡献") or "大赢家贡献")
    self.m_gxType = gxType
    self.m_maskDropDown1:setVisible(false)
end
--上庄局数
function ClubRoomCreateLayer:selectShangZhuangJuShu(bankerNum)
    self.m_gxTxt_banker:setString(bankerNum .. "局")
    self.m_banker_num = bankerNum
    self.m_maskDropDown2:setVisible(false)
end

function ClubRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        if self.m_bBeginEdit == true then
            self.m_bBeginEdit = false
            return
        end
        self:removeFromParent()
    elseif TAG_ENUM.BTN_XUNZHANG == tag then
       
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

        -- if true == ExternalFun.isContainBadWords(self.m_editText[5]:getString()) then
        --     showToast(self, "大联盟名称包含敏感词汇!", 3)
        --     return
        -- end 

        --PriRoom:getInstance():setViewFrame(self)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = false--(TAG_ENUM.BTN_CREATE_1 == tag)
        --私有房模式
        PriRoom:getInstance(). m_bPlayModel = 0
        -- 创建房间
        --PriRoom:getInstance():showPopWait()
        self:sendGroupConfig()

    elseif TAG_ENUM.MENU_DROPDOWN_1 == tag then -- 贡献方式
        self.m_maskDropDown1:setVisible(true)
    elseif TAG_ENUM.MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(false)
    elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then --上庄局数
        self.m_maskDropDown2:setVisible(true)
    elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(false)
    elseif TAG_ENUM.BTN_ALLGX == tag then
        self:updateGongxianType(1)
    elseif TAG_ENUM.BTN_MAXGX == tag then
        self:updateGongxianType(2)
    elseif TAG_ENUM.BTN_BANKER_1 <= tag and TAG_ENUM.BTN_BANKER_5 >= tag then --上庄局数
        local index = tag - TAG_ENUM.BTN_BANKER_1 + 1;
        local bankerNum = self.wBankerTimeArray[index];
        self:selectShangZhuangJuShu(bankerNum);
    elseif TAG_ENUM.BTN_GXMODE2 == tag then
        -- self:updateGongxianMode(2)
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

    -- if tag > CBT_PLAYER and tag < CBT_PLAYER+10 then --人数
    --     for k,v in pairs(self.m_tabRuleCheckBox) do
    --         if k ~= tag and k > CBT_PLAYER and k < CBT_PLAYER+10 then
    --             v:getChildByName("Sprite_11"):setTexture("room/dt_dian1.png")
    --         end
    --         if k == tag then
    --             v:getChildByName("Sprite_11"):setTexture("room/dt_dian2.png")
    --         end
    --     end
    --     self.m_nPlayerNumSelectIdx = tag
    -- end
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
        elseif event == "changed" then     
            print("changed"..src)  
            if string.len(src) ~= 0 and src ~= "" then
                text:setString(src)
            else
                text:setString(oldTxt.."")
            end
        elseif event == "began" then
            print("began"..src)
            self.m_bBeginEdit = true
            text:setVisible(false)
        end
end

--输入框确定时调用，用来设置数据
function ClubRoomCreateLayer:setConfigString(tag, score)
    if 1 == tag  then
        self.m_oldText[tag] =score
        self.m_editBox[tag]:setText("")
    elseif 2 == tag then
        self.m_oldText[tag] =score
        if score > self.lMaxUserLimitScore then
            self.m_oldText[tag] =self.lMaxUserLimitScore
        end
        self.m_editBox[tag]:setText("")
    elseif 3 == tag then
        self.m_oldText[tag] =score
        if score > self.lMaxApplyBankerCondition then
            self.m_oldText[tag] =self.lMaxApplyBankerCondition
        end
        self.m_editBox[tag]:setText("")
    end
end

function ClubRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end
--游戏时间
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
            v:getChildByName("Sprite_11"):setTexture("room/dt_check_1.png")
        end
    end
    local btn = self.m_tabCheckBox[self.m_nSelectIdx]
    btn:getChildByName("Sprite_11"):setTexture("room/dt_check_2.png")

    self.m_bLow = false
    if GlobalUserItem.tabAccountInfo.lDiamond < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end
end

return ClubRoomCreateLayer