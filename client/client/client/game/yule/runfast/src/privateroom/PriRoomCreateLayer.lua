--Garry
-- 跑得快私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")

local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")


local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local BTN_CLOSE = 1
local BTN_HELP = 2
local BTN_CREATE = 4
local BTN_CREATE_1 = 5
local BTN_ENTERGAME = 6
local CELL_BEGIN = 100  --底分选择
local PLAYER_BEGIN = 200 --人数选择
local CBT_BEGIN = 300   --局数选择
local CBT_MA = 400      --马 

local CBT_ONE_PAY  = 10          -- 一人支付
local CBT_AA_PAY   = 11           -- AA

    -- 密码配置
local CBT_NEEDPASSWD   = 20       -- 需要密码
local CBT_NOPASSWD     = 21       -- 不需要密码

local CBT_TONGBI    = 30        --经典通比
local CBT_BAWANG    = 31        --霸王庄模式

local CBT_GUN1   =  40          --打枪+1
local CBT_GUN2   = 41           --打枪+2

local CBT_PLAYER2 = 50          --两人场
local CBT_PLAYER3 = 51          --三人场
local CBT_PLAYER4 = 52          --四人场

local MENU_DROPDOWN_1 = 55    -- 下拉菜单1
local MASK_DROPDOWN_1 = 56   -- 下拉背景1
local MENU_DROPDOWN_2 = 57   -- 下拉菜单2
local MASK_DROPDOWN_2 = 58     -- 下拉背景2
local MENU_DROPDOWN_3 = 59   -- 下拉菜单3
local MASK_DROPDOWN_3 = 60     -- 下拉背景3
local MENU_DROPDOWN_4 = 61   -- 下拉菜单4
local MASK_DROPDOWN_4 = 62     -- 下拉背景4

local CBT_SCOREMODE = 70                -- 积分模式
local CBT_GOLDMODE = 71         -- 金币模式

local BTN_CELLSCORE = 72            -- 底注
local BTN_ENTERCONDITION = 73          -- 入场设定
local BTN_EXITCONDITION = 74          -- 离场设定
local BTN_CREATEGOLD = 75             -- 金币房卡创建
local CBT_SETTING = 76                -- 是否允许陌生人进入设置



local dropDownMenuYPox = {22, 58, 93.9, 130, 166}
local dropDownBgHeight = {39, 75, 111, 150, 185}

function PriRoomCreateLayer:ctor( scene,param,level )
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode
    self.param = param


    --是否允许陌生人加入
    self.m_nOpenMode = false

    self.m_enterScore = 0
    self.m_exitScore = 0

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 遮罩
    --local mask = csbNode:getChildByName("panel_mask")
    --mask:setTag(BTN_CLOSE)
    --mask:addTouchEventListener( btncallback )

    -- 底板
    self.m_spBg = csbNode:getChildByName("sp_bg_2")

    self.pNode = {}
    self.pNode[1] = self.m_spBg:getChildByName("Panel_normal"):getChildByName("Panel_1")
    self.pNode[2] = self.m_spBg:getChildByName("Panel_gold"):getChildByName("Panel_1")


            --是否允许陌生人加入
    self.m_nOpenMode = true
        --默认是积分房卡
    -- self.m_bIsNormal = false --true 为积分房卡，false为金币房卡
    self.m_bIsNormal = self.param.cbGameMode or false --true 为积分房卡，false为金币房卡

    this = self
            --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    self.m_btnScoreType = self.m_spBg:getChildByName("ckBox_score")
    self.m_btnScoreType:setTag(CBT_SCOREMODE)
    self.m_btnScoreType:addEventListener(gamemodelistener)
    self.m_btnScoreType:setVisible(false)
    
    self.m_btnGoldType = self.m_spBg:getChildByName("ckBox_gold")
    self.m_btnGoldType:setTag(CBT_GOLDMODE)
    self.m_btnGoldType:addEventListener(gamemodelistener)
    self.m_btnGoldType:setVisible(false)
    self.m_btnGoldType:getChildByName("txt"):setColor(cc.c4b(0xa5, 0x89, 0xbc, 0xff))

    self.m_nSelectScore = {0,0}
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0

     --= spbg

    -- -- 帮助按钮
    -- local btn = spbg:getChildByName("btn_tips")
    -- btn:setTag(BTN_HELP)
    -- btn:addTouchEventListener(btncallback)  

    -- 关闭
    local btn = self.m_spBg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener(btncallback)

    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        self:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    local checkbx = self.pNode[1]:getChildByName("check_consume_1")
    checkbx:setTag(CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = self.pNode[1]:getChildByName("check_consume_2")
    checkbx:setTag(CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[CBT_AA_PAY] = checkbx

    --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then

        elseif 1 == self.param.cbPayTypeMask then  --群主支付
            self.m_tabPayModeBox[CBT_AA_PAY]:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = self.m_tabPayModeBox[CBT_ONE_PAY]:getChildByName("txt_consume_1")
            txt_consume_1:setString("圈主支付")
        end
    end
                    

    
    self.m_nPasswdModeIdx = CBT_NOPASSWD
    local passwdmodelistener = function (sender,eventType)
        self:onPasswdModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPasswdModeBox = {}
    -- 需要密码
    checkbx = self.pNode[1]:getChildByName("check_mima2")
    checkbx:setTag(CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[CBT_NEEDPASSWD] = checkbx
    -- 不需要密码
    checkbx = self.pNode[1]:getChildByName("check_mima1")
    checkbx:setTag(CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[CBT_NOPASSWD] = checkbx
    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD

       -- 下拉菜单1
    btn = self.pNode[1]:getChildByName("btn_dropdown_1")
    btn:setTag(MENU_DROPDOWN_1)
    btn:addTouchEventListener(btncallback)
    btn:setEnabled(false)
    self.m_btnDropDown1 = btn
    -- 下拉箭头1
    self.m_btnDropArrow1 = self.pNode[1]:getChildByName("btn_dropdown_arrow_1")
    
    self.m_maskDropDown={}
    -- 下拉背景1
    local panel = self.m_spBg:getChildByName("Panel_normal"):getChildByName("dropdown_panel_1")
    panel:setTag(MASK_DROPDOWN_1)
    panel:addTouchEventListener( btncallback )
    panel:setVisible(false)
    --panel:setLocalZOrder(5)
    self.m_maskDropDown[1] = panel


    -- 菜单背景1
    self.m_imageDropDownBg1 = self.pNode[1]:getChildByName("dropdown_1")
    --self.m_imageDropDownBg1:setLocalZOrder(6)
    self.m_imageDropDownBg1:setVisible(false)

    self.m_txtSelectRound =self.pNode[1]:getChildByName("txt_selectround")
    self.m_txtSelectRound:setString("")

    --马牌选择
    
    self.m_gameMode = {}
    self.m_playNum = {}
    
    self.scrollView = {}
    self.m_outMode = {}
    self.m_firstMode = {}
    self.m_resultMode = {}

    for n=1 ,2 do            
           self.scrollView[n] = ccui.ScrollView:create()
                              :setContentSize(cc.size(1040,480))
                              :setPosition(cc.p(118,120))
                              :setAnchorPoint(cc.p(0,0))
                              :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
                              :setBounceEnabled(true)
                              :setScrollBarEnabled(false)
                              --:addTo(self.m_spBg:getChildByName("Panel_normal"))
            if n==1 then
                self.m_spBg:getChildByName("Panel_normal"):addChild(self.scrollView[n])
            else
                self.m_spBg:getChildByName("Panel_gold"):addChild(self.scrollView[n])
            end
            self.pNode[n]:retain()
            self.pNode[n]:removeFromParent()
            self.pNode[n]:setPosition(cc.p(0,0))
            self.pNode[n]:setAnchorPoint(cc.p(0,0))
            self.pNode[n]:addTo(self.scrollView[n])
            self.pNode[n]:setName("cellPanel")
            self.pNode[n]:release()
            self.scrollView[n]:setTouchEnabled(true)

            local function scrollView1DidScroll()
                print("scrollView1DidScroll")
            end

            self.scrollView[n]:setInnerContainerSize(cc.size(1040, 640))
            self.scrollView[n]:registerScriptHandler(scrollView1DidScroll)
            self.scrollView[n]:addTouchEventListener(function (ref, tType)
                --print("================"..tType)
                 -- if tType ==0 then
                   

                 -- elseif tType ==1 then
                    

                 -- elseif tType ==2 then
                    
                 -- elseif tType ==3 then
                    
                 -- end
                 self:onButtonClickedEvent(MASK_DROPDOWN_1)
 
            end)
          
            self.m_gameMode[n] = {}
            --玩法选择
            local playlistener = function (sender,eventType)
                local tag = sender:getTag()
                for m=1 ,2 do
                    for k,v in pairs(self.m_gameMode[m]) do
                        if k == tag then
                            v:setSelected(true)
                        else
                            v:setSelected(false)
                        end
                    end
                end
                self._gameModeIndex = tag%2
            end

            
            for i=1,2 do
                local checkbox = self.pNode[n]:getChildByName("check_card"..i)
                if checkbox ~= nil then
                    self.m_gameMode[n][i] = checkbox
                    checkbox:setTag(i)
                    checkbox:addEventListener(playlistener)
                end
            end
            self.m_gameMode[n][1]:setSelected(true)
            self._gameModeIndex = 1



            self.m_playNum[n] = {}
            --人数选择
            local numlistener = function (sender,eventType)
                local tag = sender:getTag()
                for m=1 ,2 do
                    for k,v in pairs(self.m_playNum[m]) do
                        if k == tag then
                            v:setSelected(true)
                        else
                            v:setSelected(false)
                        end
                    end
                end
                self._playerNum = tag +1
            end

            for i=1,2 do
                local checkbox = self.pNode[n]:getChildByName("check_player"..i)
                if checkbox ~= nil then
                    self.m_playNum[n][i] = checkbox
                    checkbox:setTag(i)
                    checkbox:addEventListener(numlistener)
                end
            end
            self.m_playNum[n][1]:setSelected(true)
            self._playerNum= 2



           self.m_outMode[n] = {}
            --出牌选择
            local outlistener = function (sender,eventType)
                local tag = sender:getTag()
                for m=1 ,2 do
                    for k,v in pairs(self.m_outMode[m]) do
                        if k == tag then
                            v:setSelected(true)
                        else
                            v:setSelected(false)
                        end
                    end
                end
                self._outIndex = tag%2
            end

            for i=1,2 do
                local checkbox = self.pNode[n]:getChildByName("check_rule"..i)
                if checkbox ~= nil then
                    self.m_outMode[n][i] = checkbox
                    checkbox:setTag(i)
                    checkbox:addEventListener(outlistener)
                end
            end
            self.m_outMode[n][1]:setSelected(true)
            self._outIndex= 1


            self.m_firstMode[n] = {}
            --首出规则
            local firstlistener = function (sender,eventType)
                local tag = sender:getTag()
                for m=1 ,2 do
                    for k,v in pairs(self.m_firstMode[m]) do
                        if k == tag then
                            v:setSelected(true)
                        else
                            v:setSelected(false)
                        end
                    end
                end
                self._firstIndex = tag%2
            end

            for i=1,2 do
                local checkbox = self.pNode[n]:getChildByName("check_rule"..(i+2))
                if checkbox ~= nil then
                    self.m_firstMode[n][i] = checkbox
                    checkbox:setTag(i)
                    checkbox:addEventListener(firstlistener)
                end
            end
            self.m_firstMode[n][1]:setSelected(true)
            self._firstIndex= 1

            self.m_resultMode[n] = {}
            --结算模式
            local resultlistener = function (sender,eventType)
                local tag = sender:getTag()
                for m=1 ,2 do
                    for k,v in pairs(self.m_resultMode[m]) do
                        if k == tag then
                            v:setSelected(true)
                        else
                            v:setSelected(false)
                        end
                    end
                end
                self._resultIndex = tag%2
            end

            for i=1,2 do
                local checkbox = self.pNode[n]:getChildByName("check_rule"..(i+4))
                if checkbox ~= nil then
                    self.m_resultMode[n][i] = checkbox
                    checkbox:setTag(i)
                    checkbox:addEventListener(resultlistener)
                end
            end
            self.m_resultMode[n][1]:setSelected(true)
            self._resultIndex = 1

    end



    -- 创建费用
    self.m_txtFee = self.m_spBg:getChildByName("Panel_normal"):getChildByName("txt_createfee")
    self.m_txtFee:setString("")


    -- 创建按钮
    btn = self.m_spBg:getChildByName("Panel_normal"):getChildByName("btn_createroom_1")
    -- btn:setTag(BTN_CREATE)
    -- btn:addTouchEventListener(btncallback)
    btn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            btn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(BTN_CREATE_1,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            btn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then
            if btn:getNumberOfRunningActions() >0 then
                btn:stopAllActions()
                self:onButtonClickedEvent(BTN_CREATE,ref)
            end
        end
    end)

    -- 代人开房
    btn = self.m_spBg:getChildByName("Panel_normal"):getChildByName("btn_createroom_2")
    btn:setTag(BTN_CREATE_1)
    btn:addTouchEventListener(btncallback)
    -- 必须参与
    if 1 == PriRoom:getInstance().m_tabRoomOption.cbIsJoinGame then
        btn:setEnabled(false)
        btn:setOpacity(200)
    end

    -- 金币房创建
    btn = self.m_spBg:getChildByName("Panel_gold"):getChildByName("btn_createroom_1")
    btn:setTag(BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)

    local btn = self.pNode[2]:getChildByName("btn_cell")
    btn:setTag(BTN_CELLSCORE)
    btn:addTouchEventListener(btncallback)

     local btn = self.pNode[2]:getChildByName("btn_enter")
    btn:setTag(BTN_ENTERCONDITION)
    btn:addTouchEventListener(btncallback)

    local btn = self.pNode[2]:getChildByName("btn_exit")
    btn:setTag(BTN_EXITCONDITION)
    btn:addTouchEventListener(btncallback)


    local btn = self.pNode[2]:getChildByName("check_rule_add")
    btn:setTag(CBT_SETTING)
    btn:addTouchEventListener(btncallback)
    btn:setSelected(true)


    self.m_txtCellScoreLimit = self.pNode[2]:getChildByName("txt_tip1")
    self.m_txtEnterLimit = self.pNode[2]:getChildByName("txt_tip2")
    self.m_txtExitLimit = self.pNode[2]:getChildByName("txt_tip3")

    self.m_cellScoreLimit =  self.pNode[2]:getChildByName("txt_tip1_0")

    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)

    dump({bPirGoldMode,bPirScoreMode},"=================")

    
    -- if not bPirScoreMode  or not bPirGoldMode then
    --     -- self.m_btnScoreType:setVisible(false)
    --     -- self.m_btnGoldType:setVisible(false)
    --     -- self.m_spBg:getChildByName("Panel_normal"):setVisible(bPirScoreMode)
    --     -- self.m_spBg:getChildByName("Panel_gold"):setVisible(bPirScoreMode)
    -- else 
    --     self:onGameModeSelectedEvent(self.m_bIsNormal and CBT_SCOREMODE or CBT_GOLDMODE)
    -- end

    -- if bPirGoldMode then
    --     self.m_bIsNormal = false
    -- else
    --     self.m_bIsNormal = true
    -- end

    self:onGameModeSelectedEvent(self.m_bIsNormal and CBT_SCOREMODE or CBT_GOLDMODE)


    -- 加载动画
    self:noActionTransitionIn(self)
end




--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    self.m_btnScoreType:setSelected(tag == CBT_SCOREMODE)
    self.m_btnGoldType:setSelected(tag == CBT_GOLDMODE)
    self.m_bIsNormal = (tag == CBT_SCOREMODE)

    self.m_btnScoreType:getChildByName("txt"):setColor(tag == CBT_SCOREMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    self.m_btnGoldType:getChildByName("txt"):setColor(tag == CBT_GOLDMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))

    self.m_spBg:getChildByName("Panel_normal"):setVisible(self.m_bIsNormal)
    self.m_spBg:getChildByName("Panel_gold"):setVisible(not self.m_bIsNormal)
    --self.pNode[1]:setPosition(cc.p(0,0))
    self.scrollView[1]:setInnerContainerSize(cc.size(1040, 640))
    self.scrollView[2]:setInnerContainerSize(cc.size(1040, 640))
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
    local this = self


    yPos = 51
    bgHeight = 0
    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}
    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    dump(PriRoom:getInstance().m_tabFeeConfigList,"================================111")
    -- 局数
    for i = 1, nConfig do
        yPos = dropDownMenuYPox[nConfig - i + 1] or 51
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local checkbx = self.m_imageDropDownBg1:getChildByName("check_cell_" .. i)
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

        local txtcount = checkbx:getChildByName("txt_cell_1")
        if nil ~= txtcount then
            -- 设置位置
            --txtcount:setPositionY(yPos)
            txtcount:setString(config.dwDrawCountLimit .. "局")
            --txtcount:setVisible(true)
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
        --self.m_imageDropDownBg2:setContentSize(self.m_imageDropDownBg2:getContentSize().width, bgHeight)
    end

    -- 免费判断
    if PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        self.m_spBg:getChildByName("Panel_normal"):getChildByName("txt_feetime"):setString(szfree)

        -- 划线
        local draw = cc.DrawNode:create()
        draw:drawSegment(cc.p(560, 75), cc.p(645, 75), 3, cc.c4f(0.18, 0.18, 0.18, 0.67))
        self.m_spBg:getChildByName("Panel_normal"):addChild(draw)

        -- 免费
        local szfile = "room/sparrowks_sp_createbtn_free.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            self.m_spBg:getChildByName("Panel_normal"):getChildByName("sp_create_tips"):setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
        end

        -- 字变灰
        self.m_txtFee:setTextColor(cc.c3b(45, 45, 45))
        self.m_txtFee:enableShadow(cc.c4b(0, 255, 0, 0), cc.size(0, -2))
        -- 钻石变灰
        szfile = "shop/itembuy_sp_diamond_gray.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            self.m_spBg:getChildByName("Panel_normal"):getChildByName("sp_diamond"):setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
        end
    end

        -- 激活按钮
    self.m_btnDropDown1:setEnabled(true)
 

            --入场，离场分数设置默认值
    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
    if bPirGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore
        self.m_nSelectScore[2] = self.m_cellScore

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
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE or meUser.cbUserStatus == yl.US_NULL or meUser.cbUserStatus == yl.US_PLAYING or meUser.cbUserStatus == yl.US_SIT)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            -- 创建登陆
            local buffer = ExternalFun.create_netdata(126)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            --print("===========111111111,"..self.m_nSelectScore[1])
            buffer:pushscore(self.m_bIsNormal == true and self.m_nSelectScore[1] or self.m_nSelectScore[2] )
            buffer:pushdword(self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(3)
            buffer:pushdword(0)

            -- 密码设置
            buffer:pushbyte(self.m_bIsNormal == true and self.m_nSelectPasswd or self._cmd_pri.define.tabPasswdMode.NO_PASSWD)
            -- 支付方式
            buffer:pushbyte(self.m_bIsNormal == true and self.m_nSelectPayMode or self._cmd_pri.define.tabPayMode.ONE_PAY)

            buffer:pushbyte(1)

            buffer:pushbyte(self._playerNum)
            buffer:pushbyte(self._gameModeIndex)
            buffer:pushbyte(0)
            buffer:pushbyte(0)
            buffer:pushbyte(self._firstIndex)
            buffer:pushbyte(0)
            buffer:pushbyte(self._outIndex)
            buffer:pushbyte(self._resultIndex)
            --dump({self.m_nPlayerNum,self._playindex,self._gunnum,self.m_nMaSelectIndex},"============================11111")
            for i = 1, 91 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig.cbGameMode or 0)

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
    local dwRoomID = roomDetailInfo.dwRoomID or 0
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0
    local sharetitle = string.format("【约战房间: %06d】", dwRoomID)
    local shareTxt = "局数:" .. turnCount .. "局，跑得快游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=240&a=1" .. "&p=" .. passwd
    return {title = sharetitle, content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("跑得快, 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
    -- -- 通知更新    
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
        eventListener.obj = yl.RY_CULB_PRIROOM_HIDE
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
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
    if BTN_HELP == tag then
        --self._scene:popHelpLayer2(7, 1)
    elseif BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_spBg)
    elseif BTN_CREATE == tag or BTN_CREATE_1 == tag then 
        if self.m_bLow and not PriRoom:getInstance().m_bLimitTimeFree and self.param.clubID == nil then
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
        PriRoom:getInstance().m_bCreateForOther = (BTN_CREATE_1 == tag)
        PriRoom:getInstance(). m_bPlayModel = 0

        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    elseif BTN_CREATEGOLD == tag then

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
    elseif MENU_DROPDOWN_1 == tag then
        self.m_maskDropDown[1]:setVisible(true)
        self.m_imageDropDownBg1:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureNormal("room/btn_up1.png")
    elseif MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown[1]:setVisible(false)
        self.m_imageDropDownBg1:setVisible(false)
        

        -- 更新箭头
        self.m_btnDropArrow1:loadTextureNormal("room/btn_down1.png")

    elseif BTN_CELLSCORE == tag or BTN_ENTERCONDITION == tag or BTN_EXITCONDITION == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, self._scene:getPopLevel())
        self._scene:addPopLayer(pl)
    elseif CBT_SETTING == tag then
        self.m_nOpenMode = not  self.m_nOpenMode
        sender:setSelected(self.m_nOpenMode  and true or false)
    end
end


--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)
    if BTN_CELLSCORE == tag then
        --设置了底分，入场和离场也会变化
        self.m_cellScore = scoere
        if self.m_cellScore > PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore
        elseif self.m_cellScore < PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore then
            self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        end
        self.m_nSelectScore[2] = self.m_cellScore

        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        self.m_exitScore = self.m_enterScore

        self.m_txtCellScoreLimit:setString(self.m_cellScore)
        self.m_txtEnterLimit:setString(self.m_enterScore)
        self.m_txtExitLimit:setString(self.m_exitScore)

    elseif BTN_ENTERCONDITION == tag then
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

    elseif BTN_EXITCONDITION == tag then
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
    
    self.m_nPasswdModeIdx = tag
    for k,v in pairs(self.m_tabPasswdModeBox) do
        if k ~= tag then
            v:setSelected(false)
        else
            v:setSelected(true)
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
    self:onButtonClickedEvent(MASK_DROPDOWN_1)
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
    mask:setTag(BTN_CLOSE)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 进入
    btn = spbg:getChildByName("btn_entergame")
    btn:setTag(BTN_ENTERGAME)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

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
    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读前两个规则
        buffer:readbyte()

        -- 游戏玩法
        local cbMode = buffer:readbyte()
        local szBaiDa = "经典通比（无庄家）"
        if 1 == cbMode then
            szBaiDa = "庄家比模式（霸王庄）"
        end

        -- 打枪
        local cbGunnum = buffer:readbyte()
        local szDongNan = "打枪加分+1"
        if 2 == cbGunnum then
            szDongNan = "打枪加分+2"
        end

        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(szBaiDa)
        -- 规则
        spbg:getChildByName("txt_guize"):setString(szDongNan)
    end

    -- 局数
    local ncount = self._param.dwDrawCountLimit or 0
    spbg:getChildByName("txt_jushu"):setString(ncount .. "局")

    self:scaleTransitionIn(spbg)
end

function PriRoomAAPayLayer:onButtonClickedEvent( tag,sender )
    if  tag == BTN_CLOSE then
        -- 断开
        PriRoom:getInstance():closeGameSocket()

        self:scaleTransitionOut(self.m_spBg)
    elseif tag == BTN_ENTERGAME then
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