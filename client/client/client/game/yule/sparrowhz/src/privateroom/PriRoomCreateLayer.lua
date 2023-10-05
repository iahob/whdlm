--
-- Author: David
-- Date: 2017-8-11 14:07:02
--
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 斗地主私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面

-- 广东麻将AA制提示界面
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
    "BTN_CREATE1",           -- 加入房间
    "BTN_CREATE2",           -- 加入房间
    "BTN_ENTERGAME",        -- 进入游戏
    "CBT_ONE_PAY",          -- 一人支付
    "CBT_AA_PAY",           -- AA

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    --下拉
    "CBT_DOWN1",         
    "CBT_DOWN2",
    "CBT_DOWN3",

    --码数
    "CBT_MANUM",            -- 0马

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
local CBT_PLAYER = 20     -- 人数
local CBT_MABEGIN = 30        -- 马数

local this
-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("privateRoom/RoomCardLayer.csb", self )
    self.m_csbNode = csbNode
    self.param = param

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self.m_spBg = {}
    -- 底板
    --local spbg = csbNode:getChildByName("Panel_score")
    self.m_spBg[1] = csbNode:getChildByName("Panel_score")
    self.m_spBg[2] = csbNode:getChildByName("Panel_gold")

            --是否允许陌生人加入
    self.m_nOpenMode = true
        --默认是积分房卡
    self.m_bIsNormal = self.param.cbGameMode or false --true 为积分房卡，false为金币房卡
    this = self
            --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0


    
    self.downBg = {}

    local cbtlistener = function (sender,eventType)
        self:onRuleSelectedEvent(sender:getTag(),sender)
    end
    
    self.m_tabRuleCheckBox = {}
    for n=1,2 do
        self.m_tabRuleCheckBox[n] = {}
        self.downBg[n] = self.m_spBg[n]:getChildByName("downBg")
        -- 码数选项
        for i = 1, 6 do
            local checkbx = self.downBg[n]:getChildByName("down1"):getChildByName("check".. i)

            if nil ~= checkbx then
                checkbx:setVisible(true)
                checkbx:setTag(CBT_MABEGIN + i)
                checkbx:addEventListener(cbtlistener)
                checkbx:setSelected(false)
                self.m_tabRuleCheckBox[n][CBT_MABEGIN + i] = checkbx
            end
        end

        -- 选择的马数    
        self.m_nSelectMaIdx = CBT_MABEGIN + 2
        self.m_tabRuleCheckBox[n][self.m_nSelectMaIdx]:setSelected(true)


      --人数
        for i=2,4 do
            local checkbx = self.downBg[n]:getChildByName("down3"):getChildByName("check".. (i-1))
            if nil ~= checkbx then
                checkbx:setVisible(true)
                checkbx:setTag(CBT_PLAYER + i)
                checkbx:addEventListener(cbtlistener)
                checkbx:setSelected(false)
                self.m_tabRuleCheckBox[n][CBT_PLAYER + i] = checkbx
            end

        end
        self.m_nSelectPlayerIdx = CBT_PLAYER + 2
        self.m_tabRuleCheckBox[n][self.m_nSelectPlayerIdx]:setSelected(true)


        btn = self.m_spBg[n]:getChildByName("btn_dropdown_1")
        btn:setTag(TAG_ENUM.CBT_DOWN1)
        btn:addTouchEventListener(btncallback)

        btn = self.m_spBg[n]:getChildByName("btn_dropdown_2")
        if btn then
            btn:setTag(TAG_ENUM.CBT_DOWN2)
            btn:addTouchEventListener(btncallback)
        end

        btn = self.m_spBg[n]:getChildByName("btn_dropdown_3")
        btn:setTag(TAG_ENUM.CBT_DOWN3)
        btn:addTouchEventListener(btncallback)


    
         self.downBg[n]:setTouchEnabled(false)
         self.downBg[n]:addClickEventListener(function() 
                self.downBg[1]:setTouchEnabled(false)
                self.downBg[1]:setVisible(false)
                self.downBg[2]:setTouchEnabled(false)
                self.downBg[2]:setVisible(false)

                for i=1,3 do
                    local down = self.m_spBg[n]:getChildByName("btn_dropdown_"..i)
                        if down  then
                            down:getChildByName("arrow"):setTexture("privateRoom/btn_down1.png")
                        end
                end

            end)

    end


    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        self:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    checkbx = self.m_spBg[1]:getChildByName("cbx_zhifu1")
    checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = self.m_spBg[1]:getChildByName("cbx_zhifu2")
    checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx


        --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then

        elseif 1 == self.param.cbPayTypeMask then  --群主支付
            self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY]:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY]:getChildByName("Text_maCount_2")
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
    checkbx = self.m_spBg[1]:getChildByName("cbx_mima2")
    checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx:setSelected(false)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx
    -- 不需要密码
    checkbx = self.m_spBg[1]:getChildByName("cbx_mima1")
    checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

    -- 创建按钮
    btn = self.m_spBg[1]:getChildByName("bt_createRoom2")
    btn:setTag(TAG_ENUM.BTN_CREATE1)
    btn:setVisible(false)

    self.m_createBtn = self.m_spBg[1]:getChildByName("bt_createRoom1")
    self.m_createBtn:setTag(TAG_ENUM.BTN_CREATE2)
    self.m_createBtn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            self.m_createBtn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE1,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            self.m_createBtn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then
            if self.m_createBtn:getNumberOfRunningActions() >0 then
                self.m_createBtn:stopAllActions()
                self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE2,ref)
            end
        end
    end)

    btn = self.m_spBg[2]:getChildByName("bt_createRoom2")
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)
    btn:setPositionX(600)

    local btn = self.m_spBg[2]:getChildByName("btn_difen")
    btn:setTag(TAG_ENUM.BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)

     local btn = self.m_spBg[2]:getChildByName("btn_enter")
    btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = self.m_spBg[2]:getChildByName("btn_exit")
    btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)

        local btn = self.m_spBg[2]:getChildByName("cbx_add")
    btn:setTag(TAG_ENUM.CBT_SETTING)
    btn:addTouchEventListener(btncallback)


    self.m_txtCellScoreLimit = self.m_spBg[2]:getChildByName("txt_tips1")
    self.m_txtEnterLimit = self.m_spBg[2]:getChildByName("txt_tips2")
    self.m_txtExitLimit = self.m_spBg[2]:getChildByName("txt_tips3")

    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)

    dump({bPirGoldMode,bPirScoreMode},"=================")
    

    self.m_spBg[1]:setVisible(self.m_bIsNormal)
    self.m_spBg[2]:setVisible(not self.m_bIsNormal)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self)
end

------da
-- 继承/覆盖ad
------ad
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
   self.m_tabCheckBox = {}
    -- 局数
    for i = 1, #PriRoom:getInstance().m_tabFeeConfigList do
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local checkbx = self.downBg[1]:getChildByName("down2"):getChildByName("check".. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_BEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
        end

        local txtcount = checkbx:getChildByName("txt")
        if nil ~= txtcount then
            txtcount:setString(config.dwDrawCountLimit .. "局")
        end
    end
    
    -- 选择的局数
    self.m_nSelectIdx = CBT_BEGIN + 1
    dump(PriRoom:getInstance().m_tabFeeConfigList , "PriRoom:getInstance().m_tabFeeConfigList")
    if nil ~= PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN] then
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]
        self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)

        self.m_bLow = false
        -- 创建费用
        self.m_txtFee = self.m_spBg[1]:getChildByName("txt_fee")
        self.m_txtFee:setString("")
        -- 更新费用
        self:updateCreateFee()
    end
    if self.m_tabSelectConfig then
        local txt = self.m_spBg[1]:getChildByName("btn_dropdown_2"):getChildByName("txt")
           txt:setString(self.m_tabSelectConfig.dwDrawCountLimit.."局")
       end
       -- 免费判断
    if  PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        self.m_spBg[1]:getChildByName("Text_freetime"):setString(szfree)

        -- 消耗隐藏
        self.m_spBg[1]:getChildByName("Text_xiaohao"):setVisible(false)
        -- 费用隐藏
        self.m_txtFee:setVisible(false)
        -- 钻石隐藏
        self.m_spBg[1]:getChildByName("zuan"):setVisible(false)
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
            self.m_txtCellScoreLimit:setString(self.m_cellScore)
            self.m_txtEnterLimit:setString(self.m_enterScore)
            self.m_txtExitLimit:setString(self.m_exitScore)
            self.m_spBg[2]:getChildByName("txt_cellRange"):setString("底分范围："..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
        end
    end

end

function PriRoomCreateLayer:onLoginPriRoomFinish()
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end

    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE 
        or meUser.cbUserStatus == yl.US_NULL 
        or meUser.cbUserStatus == yl.US_PLAYING
        or meUser.cbUserStatus == yl.US_SIT)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            print("gggggggggggggggggggggg")
            -- 创建登陆
            local buffer = ExternalFun.create_netdata(126) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_cellScore)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawCountLimit or 10)
            buffer:pushdword(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.dwDrawTimeLimit or 10)
            buffer:pushword(4)
            buffer:pushdword(0)
            -- 密码设置
            if self.m_bIsNormal then
                buffer:pushbyte(self.m_nSelectPasswd)
            else
                buffer:pushbyte(self._cmd_pri.define.tabPasswdMode.NO_PASSWD)
            end
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

            --游戏额外规则
            buffer:pushbyte(1)

            buffer:pushbyte(self.m_nSelectPlayerIdx -CBT_PLAYER)
            buffer:pushbyte(4)

            buffer:pushbyte(self.m_nSelectMaIdx -CBT_MABEGIN)
          

            for i = 1, 96 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)

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

    local shareTxt = "局数:" .. turnCount .. "局。红中麻将精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=389&a=1" .. "&p=" .. passwd
    
    return {title = "【约战房间: " .. dwRoomID .. "】", content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = "红中麻将, 房号[" .. dwRoomID .. "],您的好友喊你打牌了!"}
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function PriRoomCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg[1])
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

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    -- self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    -- self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    -- self.m_btnScoreType:getChildByName("txt"):setColor(tag == TAG_ENUM.CBT_SCOREMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    -- self.m_btnGoldType:getChildByName("txt"):setColor(tag == TAG_ENUM.CBT_GOLDMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))

    self.m_spBg[1]:setVisible(self.m_bIsNormal)
    self.m_spBg[2]:setVisible(not self.m_bIsNormal)
end


function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_spBg[1])
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    elseif TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
    elseif TAG_ENUM.BTN_CREATE1 == tag or TAG_ENUM.BTN_CREATE2 == tag then 
        if self.m_bLow and not PriRoom:getInstance().m_bLimitTimeFree and self.param.clubID == nil then
            local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
            local query = QueryDialog:create("您的钻石数量不足，是否前往商城充值！", function(ok)
                if ok == true then
                    if nil ~= self._scene and nil ~= self._scene.popShopLayer then
                        self._scene:popShopLayer()
                    end
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
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE1 == tag)
        PriRoom:getInstance(). m_bPlayModel = 0

        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
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
    elseif TAG_ENUM.CBT_DOWN1 == tag then
        
        self:updateDownBg(1)
    elseif TAG_ENUM.CBT_DOWN2 == tag then
        
        self:updateDownBg(2)
    elseif TAG_ENUM.CBT_DOWN3 == tag then
        
        self:updateDownBg(3)
    elseif TAG_ENUM.BTN_CELLSCORE == tag or TAG_ENUM.BTN_ENTERCONDITION == tag or TAG_ENUM.BTN_EXITCONDITION == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, self._scene:getPopLevel())
        self._scene:addPopLayer(pl)
    elseif TAG_ENUM.CBT_SETTING == tag then
        self.m_nOpenMode = not  self.m_nOpenMode
        sender:setSelected(self.m_nOpenMode  and true or false)
    end

end

function PriRoomCreateLayer:updateDownBg(index)

        for n=1,2 do
            for i=1,3 do
                    local down = self.downBg[n]:getChildByName("down"..i)

                    if down then
                        down:setVisible(index == i and true or false)
                    end
            end
            local down  = self.m_spBg[n]:getChildByName("btn_dropdown_"..index)
            if down then
                down:getChildByName("arrow"):setTexture("privateRoom/btn_up1.png")
            end

            self.downBg[n]:setTouchEnabled(true)
            self.downBg[n]:setVisible(true)
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




function PriRoomCreateLayer:onRuleSelectedEvent(tag, sender)
  


    if tag > CBT_PLAYER and tag < CBT_MABEGIN then --人数
        local last = self.m_nSelectPlayerIdx
        for n=1,2 do
            self.m_tabRuleCheckBox[n][last]:setSelected(false)
            self.m_tabRuleCheckBox[n][tag]:setSelected(true)
            self.m_nSelectPlayerIdx = tag

           local txt = self.m_spBg[n]:getChildByName("btn_dropdown_3"):getChildByName("txt")
           txt:setString((self.m_nSelectPlayerIdx -CBT_PLAYER) .."人")
        end

    end

    if tag > CBT_MABEGIN then --码数

          self.m_nSelectMaIdx = tag

           for n=1,2 do

                for k,v in pairs(self.m_tabRuleCheckBox[n]) do
                    if k ~= tag  and  k > CBT_MABEGIN  then
                        v:setSelected(false)
                    end
                end

                local txt = self.m_spBg[n]:getChildByName("btn_dropdown_1"):getChildByName("txt")
                local str = ""

                if self.m_nSelectMaIdx - CBT_MABEGIN == 1 then
                    str = "一码全中"
                else
                    str = (self.m_nSelectMaIdx - CBT_MABEGIN).."个扎码"
                end
                   txt:setString(str)
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

    local txt = self.m_spBg[1]:getChildByName("btn_dropdown_2"):getChildByName("txt")
           txt:setString(self.m_tabSelectConfig.dwDrawCountLimit.."局")

    if nil == self.m_tabSelectConfig then
        return
    end

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
    local rootLayer, csbNode = ExternalFun.loadRootCSB("privateRoom/PrivateRoomAAPayLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    --local mask = csbNode:getChildByName("panel_mask")
    --mask:setTag(TAG_ENUM.TAG_MASK)
    --mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("Sprite_1")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 进入
    btn = spbg:getChildByName("Button_2")
    btn:setTag(TAG_ENUM.BTN_ENTERGAME)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 房间id
   -- local roomid = self._param.szRoomId or ""
    --spbg:getChildByName("txt_roomid"):setString(roomid)

       -- 房间id
    local roomid = self._param.dwRoomId or 0
    spbg:getChildByName("txt_roomid"):setString(string.format("%06d", roomid))

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString("x"..consume)

 
    local buffer = self._param.buffer
    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读前两个规则
        --local playerNum = buffer:readbyte()

        spbg:getChildByName("Text_5"):setString(buffer:readbyte().."人")

        buffer:readbyte()
        local maNum = buffer:readbyte()
        local str = maNum.."个码"
        if maNum == 1 then
            str = "一码全中"
        end
        spbg:getChildByName("Text_5_0"):setString(str)


    end

    -- 局数
    local ncount = self._param.dwDrawCountLimit or 0
    spbg:getChildByName("Text_5_1"):setString(ncount .. "局")

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