--
-- Author: zhong
-- Date: 2016-12-17 14:07:02
--
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 三公私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

-- AA制提示界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local TAG_START             = 50
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_HELP",             -- 帮助
    "BTN_CHARGE",           -- 充值
    "BTN_MYROOM",           -- 自己房间
    "BTN_CREATE1",           -- 代人开房
    "BTN_CREATE2",           -- 立即开局

    "BTN_CREATE_1",           -- 代开房间
    "BTN_ENTERGAME",        -- 进入游戏
    "CBT_ONE_PAY",          -- 一人支付
    "CBT_AA_PAY",           -- AA

    "BTN_RULE1",            --庄家玩法
    "BTN_RULE2",
    "BTN_RULE3",
    "BTN_RULE4",
    "BTN_RULE5",
    "BTN_RULE6",

    "MENU_DROPDOWN_1",      -- 下拉菜单1

    "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式

    "BTN_CELLSCORE",                -- 底注
    "BTN_ENTERCONDITION",           -- 入场设定
    "BTN_EXITCONDITION",            -- 离场设定
    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置

}

local CBT_DIFEN =100
local CBT_MODE = 110
local CBT_FANBEI = 120
local CBT_Z_SCORE = 130
local CBT_JUSHU = 150
local CBT_X_SCORE = 160
local CBT_GAOJI = 170
local CBT_Z_MAX = 180
local CBT_TESHU = 190
local CBT_PLAYER_NUM = 200
local CBT_BANKER_MODE = 210 --定庄方式

local RuleBtnPosX = {703,486,923,267,1144}

local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local dropDownMenuYPox = {51, 121, 191, 261, 331}
local dropDownBgHeight = {98, 168, 238, 308, 378}
local this
-- 创建界面
function PriRoomCreateLayer:ctor( scene ,param)
    PriRoomCreateLayer.super.ctor(self, scene, param)
    -- 加载动画
    self:noActionTransitionIn(self)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode
    self.param = param

    local function btncallback(ref, tType)
        
        if ref:getTag() < 100 then
            if tType == ccui.TouchEventType.ended  then
                self:onButtonClickedEvent(ref:getTag(),ref)
            end
        else
            if tType == ccui.TouchEventType.ended  or tType == ccui.TouchEventType.began or tType == ccui.TouchEventType.canceled then
                self:onButtonClickedEvent(ref:getTag(),ref)
            end
        end
           
        
    end

        --是否允许陌生人加入
    self.m_nOpenMode = true

    print("self.param.cbGameMode",self.param.cbGameMode)
    --默认是金币房卡
    self.m_bIsNormal = self.param.cbGameMode --true 为积分房卡，false为金币房卡


    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    

    self.m_ScoreNode = spbg:getChildByName("Panel_2")
    self.m_GoldNode = spbg:getChildByName("Panel_3")
    --self.clipPanel:setClippingEnabled(true)
 
    
    this = self
            --模式切换按钮
    local gamemodelistener = function (sender,eventType)
        this:onGameModeSelectedEvent(sender:getTag(),sender)
    end

    self.m_btnScoreType = spbg:getChildByName("ckBox_score")
    self.m_btnScoreType:setTag(TAG_ENUM.CBT_SCOREMODE)
    self.m_btnScoreType:addEventListener(gamemodelistener)
    self.m_btnScoreType:setVisible(false)
    
    self.m_btnGoldType = spbg:getChildByName("ckBox_gold")
    self.m_btnGoldType:setTag(TAG_ENUM.CBT_GOLDMODE)
    self.m_btnGoldType:addEventListener(gamemodelistener)
    self.m_btnGoldType:setVisible(false)
    self.m_btnGoldType:getChildByName("Text_55"):setColor(cc.c4b(0xa5, 0x89, 0xbc, 0xff))

    self.m_nSelectScore = 0
    self.m_enterScore = 0
    self.m_exitScore = 0
    self.m_cellScore = 0

     self.curCellHeight = 0
     self.m_ruleBtn = {}
     self.m_ruleTxt = {}
     self.m_cellNode = {}
    for i=1, 4 do
        local txt = spbg:getChildByName("Text_"..i)
        self.m_ruleTxt[i] = txt

        local btn = txt:getChildByName("Button_rule"..i)
        btn:setTag(TAG_ENUM.BTN_RULE1 +(i-1))
        btn:addTouchEventListener(btncallback)

        self.m_ruleBtn[i] = btn 
    end

    -- self.m_line = {}
    -- for i=1,4 do
    --     local line = self.m_spBg:getChildByName("line_"..i)
    --     line:setVisible(false)
    --     self.m_line[i] = line
    -- end

--------------------------------------金币房卡--------------------------------------------------


------------------------------------积分房卡-----------------------------------------
    self.m_checkTable = {}
    self.downBg = {}
    self.downBgPlayer = {}
    self.scrollView = {}
    self.panel = {}
    local node = {self.m_ScoreNode,self.m_GoldNode}
    self.panel[1] = self.m_ScoreNode:getChildByName("Panel_1")
    self.panel[2] = self.m_GoldNode:getChildByName("Panel_1")
    for n = 1, 2 do
        
        local viewSize = self.m_ScoreNode:getContentSize()
        self.scrollView[n] = ccui.ScrollView:create()
                              :setContentSize(cc.size(1030,430))
                              :setPosition(cc.p(20,0))
                              :setAnchorPoint(cc.p(0,0))
                              :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
                              :setBounceEnabled(true)
                              :setScrollBarEnabled(false)
                              :addTo(node[n])
            self.panel[n]:retain()
            self.panel[n]:removeFromParent()
            self.panel[n]:setPosition(cc.p(0,70))
            self.panel[n]:setAnchorPoint(cc.p(0,0))
            self.panel[n]:addTo(self.scrollView[n])
            self.panel[n]:setName("cellPanel")
            self.panel[n]:release()
            self.scrollView[n]:setTouchEnabled(false)

            local function scrollView1DidScroll()
                print("scrollView1DidScroll")
            end

            self.scrollView[n]:setInnerContainerSize(cc.size(viewSize.width, viewSize.height + 70))
            self.scrollView[n]:registerScriptHandler(scrollView1DidScroll)
            self.scrollView[n]:addTouchEventListener(function (ref, tType)
                --print("================"..tType)
                -- if tType ==0 then
                    -- self.m_spBg:getChildByName("Slider_1"):setVisible(true)
                    -- self.m_spBg:getChildByName("Sprite_4"):setVisible(true)
                    -- local point = self.panel[n]:convertToWorldSpace(cc.p(0,0))
                    
                    -- if self.curCellHeight >0 then
                    --     self.slider:setPercent(100*(point.y - 140)/self.curCellHeight)
                    -- else
                    --     self.slider:setPercent(0)
                    -- end

                   
                    
                    --dump(point,"===================1111")
                -- elseif tType ==1 then
                --     local point = self.panel[n]:convertToWorldSpace(cc.p(0,0))
                --     if self.curCellHeight >0 then
                --         self.slider:setPercent(100*(point.y - 140)/self.curCellHeight)
                --     else
                --         self.slider:setPercent(0)
                --     end
                    --dump(point,"===================222")

                -- elseif tType ==2 then
                    -- self.m_spBg:getChildByName("Slider_1"):setVisible(false)
                    -- self.m_spBg:getChildByName("Sprite_4"):setVisible(false)
                -- elseif tType ==3 then
                    -- self.m_spBg:getChildByName("Slider_1"):setVisible(false)
                    -- self.m_spBg:getChildByName("Sprite_4"):setVisible(false)
                -- end

                --print(self.curCellHeight.."=================="..point.y)
            end)

        self.m_cellNode[n] = {}

        for i=1, 7 do
            self.m_cellNode[n][i] = self.panel[n]:getChildByName("cell"..i)
            self.m_cellNode[n][i]:setAnchorPoint(cc.p(0,1))
        end

        --积分房才有推注
        local btn_help = self.m_cellNode[n][4]:getChildByName("btn_help")
        local txtTouchBg = self.m_cellNode[n][4]:getChildByName("Panel_4")
        local txtPanel = txtTouchBg:getChildByName("Panel_3")
        btn_help:addTouchEventListener(function(ref, eventType)
            if eventType == ccui.TouchEventType.began then
            elseif eventType == ccui.TouchEventType.ended then 
                txtTouchBg:setVisible(not txtTouchBg:isVisible())
                -- or eventType == ccui.TouchEventType.canceled then
                -- txtPanel:setVisible(false)
            end
        end)
        btn_help:setScale(1.5)
        txtTouchBg:addTouchEventListener(function(ref, eventType)
            if eventType == ccui.TouchEventType.began then
            elseif eventType == ccui.TouchEventType.ended then 
                txtTouchBg:setVisible(false)
            end
        end)
    --=================================================================
        self.m_checkTable[n] = {}
        local checkbx = nil

        self._joinMode = 1
        local  checkbx = self.panel[n]:getChildByName("cell6"):getChildByName("checkzhu1")
        checkbx:setTouchEnabled(true)
        checkbx:setSelected(true)
        checkbx:addTouchEventListener(function(sender, type)
            if type == ccui.TouchEventType.ended then
                print("checkbx:isSelected() ========",checkbx:isSelected())
                checkbx:setSelected( checkbx:isSelected())
                if checkbx:isSelected() then
                    self._joinMode = 1
                else
                    self._joinMode = 0
                end
                print("_joinMode ========",self._joinMode)
            end
        end)

        --是否需要密码
        self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
        local  checkbx = self.panel[n]:getChildByName("cell6"):getChildByName("checkzhu2")
        checkbx:setTouchEnabled(true)
        checkbx:setSelected(true)
        checkbx:addTouchEventListener(function(sender, type)
            if type == ccui.TouchEventType.ended then
                checkbx:setSelected( checkbx:isSelected())
                if checkbx:isSelected() then
                    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
                else
                    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
                end
                print("_joinMode ========",self._joinMode)
            end
        end)

        --翻倍规则
        for i=1, 3 do
            local checkbx = self.panel[n]:getChildByName("cell2"):getChildByName("checkbei"..i)
            checkbx:setTag(CBT_FANBEI + i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(btncallback)
            self.m_checkTable[n][CBT_FANBEI+i] = checkbx
        end

        self.m_fanbei_index = CBT_FANBEI+1
        self.m_checkTable[n][self.m_fanbei_index]:setSelected(true)

        --人数
        for i=1, 2 do
            local checkbx = self.panel[n]:getChildByName("cell1"):getChildByName("check"..i)
            print("kanakan看看",i, CBT_PLAYER_NUM + i)
            checkbx:setTag(CBT_PLAYER_NUM + i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(btncallback)
            self.m_checkTable[n][CBT_PLAYER_NUM+i] = checkbx
        end

        self.playerNum_index = CBT_PLAYER_NUM+1
        self.m_checkTable[n][self.playerNum_index]:setSelected(true)

        --抢庄倍数
        for i=1, 3 do
            local checkbx = self.panel[n]:getChildByName("cell3"):getChildByName("checkbei"..i)
            checkbx:setTag(CBT_Z_SCORE + i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(btncallback)
            self.m_checkTable[n][CBT_Z_SCORE+i] = checkbx
        end

        self.m_zscore_index = CBT_Z_SCORE+1
        self.m_checkTable[n][self.m_zscore_index]:setSelected(true)

        --推注
  
        for i=1, 2 do
            local checkbx = self.panel[n]:getChildByName("cell4"):getChildByName("checkzhu"..i)
            checkbx:setTag(CBT_X_SCORE + i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(btncallback)
            self.m_checkTable[n][CBT_X_SCORE+i] = checkbx
            if i==1 then
                checkbx:getChildByName("Text_7"):setString("无")
            elseif i==2 then
                checkbx:getChildByName("Text_7"):setString("翻倍")
            end
        end

        self.m_xscore_index = CBT_X_SCORE+1
        self.m_checkTable[n][self.m_xscore_index]:setSelected(true)

        --上庄方式
        for i=1, 3 do
            local checkbx = self.panel[n]:getChildByName("cell7"):getChildByName("checkbei"..i)
            checkbx:setTag(CBT_BANKER_MODE + i)
            checkbx:setTouchEnabled(true)
            checkbx:setSelected(false)
            checkbx:addTouchEventListener(btncallback)
            self.m_checkTable[n][CBT_BANKER_MODE + i] = checkbx
        end
        self.m_bankerMode_index = CBT_BANKER_MODE + 1
        self.m_checkTable[n][self.m_bankerMode_index]:setSelected(true)

        --支付
        if n==1 then --积分
            -- 支付选择
            self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
            self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
            local paymodelistener = function(sender,eventType)
                self:onPayModeSelectedEvent(sender:getTag(),sender)
            end
            self.m_tabPayModeBox = {}
            -- 一人付
            checkbx = self.panel[n]:getChildByName("cell5"):getChildByName("checkfee1")
            checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
            checkbx:addTouchEventListener(paymodelistener)
            checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
            checkbx:setSelected(true)
            self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
            -- AA付
            checkbx = self.panel[n]:getChildByName("cell5"):getChildByName("checkfee2")
            checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
            checkbx:addTouchEventListener(paymodelistener)
            checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
            self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx

            --俱乐部支付判断
            if self.param.cbPayTypeMask ~= nil then
                if 2 == self.param.cbPayTypeMask then

                elseif 1 == self.param.cbPayTypeMask then  --群主支付
                    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY]:setVisible(false)
                    --一人支付文字修改
                    local txt_consume_1 = self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY]:getChildByName("Text_7")
                    txt_consume_1:setString("圈主支付")
                end
            end
            
            -- 是否密码
            -- self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
        else
            local btn = self.panel[n]:getChildByName("cell5"):getChildByName("btn_difen")
            btn:setTag(TAG_ENUM.BTN_CELLSCORE)
            btn:addTouchEventListener(btncallback)

             local btn = self.panel[n]:getChildByName("cell5"):getChildByName("btn_ruchang")
            btn:setTag(TAG_ENUM.BTN_ENTERCONDITION)
            btn:addTouchEventListener(btncallback)

            local btn = self.panel[n]:getChildByName("cell5"):getChildByName("btn_lichang")
            btn:setTag(TAG_ENUM.BTN_EXITCONDITION)
            btn:addTouchEventListener(btncallback)

            local btn = self.m_GoldNode:getChildByName("bt_createRoom3")
            btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
            btn:addTouchEventListener(btncallback)
            btn:setPositionX(465)

            self.m_txtCellScoreLimit = self.panel[n]:getChildByName("cell5"):getChildByName("txt_difen")
            self.m_txtCellScoreRange = self.panel[n]:getChildByName("cell5"):getChildByName("txt_difen_range")
            self.m_txtEnterLimit = self.panel[n]:getChildByName("cell5"):getChildByName("txt_ruchang")
            self.m_txtExitLimit = self.panel[n]:getChildByName("cell5"):getChildByName("txt_lichang")

        end
    end

    self.m_createBtn = spbg:getChildByName("Panel_2"):getChildByName("bt_createRoom1")
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
    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleTransitionIn(self)

    self.m_ruleCell = {}
    self.m_ruleCell[1] = {  {1, 2, 3, 4, 5, 6},
                            {1, 2, 4, 5, 6},
                            {1, 2, 4, 7, 5, 6},  
                            {1, 2, 5, 6}}

    self.m_ruleCell[2] = {  {5, 1, 2, 3, 6},
                            {5, 1, 2, 6},
                            {5, 1, 2, 7, 6, },  
                            {5, 1, 2, 6}}

    self.m_cellHeigh ={80, 80, 80, 80, 160, 80, 80}
    self.m_rule = 1
    self:updateRule(self.m_rule)
    self.isSlide = false


    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    local bPirScoreMode = PriRoom:getInstance():isCurrentGameOpenPriScore(GlobalUserItem.nCurGameKind)
    --dump({bPirGoldMode,bPirScoreMode},"=================")

    -- if not bPirScoreMode  or not bPirGoldMode then
    --         self.m_btnScoreType:setVisible(false)       
    --         self.m_btnGoldType:setVisible(false)  
    -- else
    --        self.m_btnScoreType:setVisible(true)       
    --         self.m_btnGoldType:setVisible(true)  
    -- end


    -- if bPirGoldMode then
    --     self.m_bIsNormal = false
    -- else
    --     self.m_bIsNormal = true
    -- end

    dump({self.m_bIsNormal},"==================")
    self.m_ScoreNode:setVisible(self.m_bIsNormal)
    self.m_GoldNode:setVisible(not self.m_bIsNormal)
    -- self:onGameModeSelectedEvent(bPirGoldMode and TAG_ENUM.CBT_GOLDMODE or TAG_ENUM.CBT_SCOREMODE)
end




function PriRoomCreateLayer:SlideEvent(event)
  if event.name == "ON_PERCENTAGE_CHANGED" then
    local percent = event.target:getPercent()
    
        if self.isSlide then
            --self.panel:setPositionY(0+50*percent/100)
        end
  end
end



function PriRoomCreateLayer:updateRule(tag)
    print(" PriRoomCreateLayer:updateRule tag",tag)
        
        self.m_ruleTxt[self.m_rule]:setColor(cc.c3b(255, 255, 255))
        self.m_ruleTxt[self.m_rule]:getChildByName("selectBg_2"):setVisible(false)

        self.m_ruleTxt[tag]:setColor(cc.c3b(245, 208, 96))
        self.m_ruleTxt[tag]:getChildByName("selectBg_2"):setVisible(true)

        self.m_rule = tag 
       
        local opType = {4, 5, 0 ,6}
        self.m_lOpBankerType = opType[tag]

        self:updateCell(self.m_rule)
end

function PriRoomCreateLayer:updateCell(tag,index)
    for n = 1, 2 do
        self.isSlide = true
            
        self.scrollView[n]:setTouchEnabled(true)

        for i=1, 7 do
            self.m_cellNode[n][i]:setVisible(false)
        end
        local pointY= 0
        local height = 0
        for i=1,#self.m_ruleCell[n][tag] do
            local cellIndex = self.m_ruleCell[n][tag][i]
            self.m_cellNode[n][cellIndex]:setVisible(true)
            self.m_cellNode[n][cellIndex]:setPosition(0,460 - pointY)
            pointY = pointY + self.m_cellHeigh[cellIndex]
            height = height +self.m_cellHeigh[cellIndex]
        end
        

        local tempH = height - 460 >0 and height - 460 or 0
        self.curCellHeight = tempH

        self.scrollView[n]:getChildByName("cellPanel"):setPositionY(tempH)
        self.scrollView[n]:setInnerContainerSize(cc.size(930, 460 + tempH))

        self.m_cellNode[n][4]:getChildByName("Panel_4"):setVisible(false)
    end
        
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    self.m_btnScoreType:setSelected(tag == TAG_ENUM.CBT_SCOREMODE)
    self.m_btnGoldType:setSelected(tag == TAG_ENUM.CBT_GOLDMODE)
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_btnScoreType:getChildByName("Text_55"):setColor(tag == TAG_ENUM.CBT_SCOREMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))
    self.m_btnGoldType:getChildByName("Text_55"):setColor(tag == TAG_ENUM.CBT_GOLDMODE and cc.c4b(0xff, 0xff, 0xff, 0xff) or cc.c4b(0xa5, 0x89, 0xbc, 0xff))

    self.m_ScoreNode:setVisible(self.m_bIsNormal)
    self.m_GoldNode:setVisible(not self.m_bIsNormal)
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

 
    local cbtlistener = function (sender,eventType)
            local tag =sender:getTag()

            if eventType == ccui.TouchEventType.began or eventType == ccui.TouchEventType.ended or eventType == ccui.TouchEventType.canceled  then
        
                     if self.m_nSelectIdx == tag then
                            --sender:setSelected(true)
                           -- return
                    end

                    self.m_checkTable[1][self.m_nSelectIdx]:setSelected(false)

                    self.m_nSelectIdx = tag
                    self.m_checkTable[1][self.m_nSelectIdx]:setSelected(true)

                    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[tag - CBT_JUSHU]

                    if nil == self.m_tabSelectConfig then
                        return
                    end

                    -- 更新费用
                    self:updateCreateFee()
            end
    end

    local panel = self.scrollView[1]:getChildByName("cellPanel")
    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    -- 局数
    for i = 1, nConfig do
        
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local checkbx = panel:getChildByName("cell5"):getChildByName("checkju"..i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTouchEnabled(true)
            checkbx:setTag(CBT_JUSHU + i)
            checkbx:addTouchEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_checkTable[1][CBT_JUSHU + i] = checkbx

            local txtcount = checkbx:getChildByName("Text_7")
            txtcount:setString(config.dwDrawCountLimit .. "局")
        end
    end
    -- 选择的玩法    
    self.m_nSelectIdx = CBT_JUSHU + 1
    self.m_tabSelectConfig = nil
    if nConfig > 0 then
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_JUSHU]
        self.m_checkTable[1][self.m_nSelectIdx]:setSelected(true)

        -- 创建费用
        self.m_txtFee = self.m_ScoreNode:getChildByName("feeNum")
        self.m_txtFee:setString("")

        -- 更新费用
        self:updateCreateFee()
    end
    -- 免费判断
    if PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        panel:getChildByName("cell2"):getChildByName("freeTime"):setString(szfree)

        panel:getChildByName("Sprite_5"):setVisible(false)
        panel:getChildByName("Text_1"):setVisible(false)
        panel:getChildByName("feeNum"):setVisible(false)

    end
end

function PriRoomCreateLayer:onRefreshRule()
      local ruleT = 
    {
        { k = "lOperateBankerRatio",t = "score",l = {3}},   --最大抢庄倍数
        { k = "lMaxScore",t = "score",l = {5}},             --封顶分数（通比模式）

    }
    print("================="..PriRoom:getInstance().m_tabRule:getlen())
    local cmd_table1 = ExternalFun.read_netdata(ruleT, PriRoom:getInstance().m_tabRule) --积分房配置

    local lastLen = 1024 - PriRoom:getInstance().m_tabRule:getcurlen() + self._cmd_pri.define.LEN_PERSONAL_OTHER

    ExternalFun.read_netdata({{ k = "emptyType",t = "byte",l = {lastLen}}}, PriRoom:getInstance().m_tabRule)--去掉中间多余字节

    local cmd_table2 = ExternalFun.read_netdata(ruleT, PriRoom:getInstance().m_tabRule)--金币房配置

    local cmd_table = self.m_bIsNormal and clone(cmd_table1) or clone(cmd_table2)
  
    self.ruleData = cmd_table
    dump(cmd_table)
  
    for n=1 ,2 do

        local  panel = self.scrollView[n]:getChildByName("cellPanel")

        --抢庄倍数 
        for i = 1, 3 do
            local checkbx = panel:getChildByName("cell3"):getChildByName("checkbei" .. i)
            if nil ~= checkbx then
                local txt = checkbx:getChildByName("Text_7")
                txt:setString(cmd_table.lOperateBankerRatio[1][i].."倍")
            end
        end  
    end
    --self:updateCell(self.m_rule,self.m_mode_index-CBT_MODE)
    self:updateRule(self.m_rule)


            --入场，离场分数设置默认值
    local bPirGoldMode = PriRoom:getInstance():isCurrentGameOpenPriGold(GlobalUserItem.nCurGameKind)
    dump(PriRoom:getInstance().m_tabGoldRoomOption, "金币房卡配置", 6)
    if bPirGoldMode and nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
        self.m_cellScore = PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore
        self.m_enterScore = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        -- local cell1 = self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        -- local cell2 = self.m_cellScore *self.ruleData.cbMinBetTimes*self.ruleData.cbMaxCardTypeTimes*5*4
        -- self.m_enterScore = cell1 > cell2  and cell1 or  cell2--self.m_cellScore *PriRoom:getInstance().m_tabGoldRoomOption.lMinEnterScore
        
        self.m_exitScore = self.m_enterScore
        self.m_nSelectScore = self.m_cellScore

        if nil ~= PriRoom:getInstance().m_tabGoldRoomOption then
            self.m_txtCellScoreLimit:setString(self.m_cellScore)
            self.m_txtEnterLimit:setString(self.m_enterScore)
            self.m_txtExitLimit:setString(self.m_exitScore)
            self.m_txtCellScoreRange:setString("底分范围："..PriRoom:getInstance().m_tabGoldRoomOption.lMinCellScore.."-"..PriRoom:getInstance().m_tabGoldRoomOption.lMaxCellScore)
        end
    end
    --self.m_ruleBtn[i]
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
            -- 创建登陆
            local buffer = ExternalFun.create_netdata(126) --CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            if self.m_bIsNormal then
                buffer:pushscore(0)
            else
                buffer:pushscore(self.m_nSelectScore)
            end
            buffer:pushdword(self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(4)
            buffer:pushdword(0)

            if not self.m_bIsNormal then
                self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
            end
            -- 密码设置
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)

            -- 额外规则
            buffer:pushbyte(1)
            if self.m_bIsNormal then
                buffer:pushbyte(self.playerNum_index - CBT_PLAYER_NUM  == 1 and 6 or 8)    --游戏人数
                buffer:pushbyte(self.m_xscore_index - CBT_X_SCORE == 1 and 0 or 1)  --闲家推注
            else
                buffer:pushbyte(8)
                buffer:pushbyte(0)
            end

            if 0 == self.m_lOpBankerType then
                self.m_lOpBankerType = self.m_bankerMode_index - CBT_BANKER_MODE
            end
            buffer:pushbyte(self.m_lOpBankerType)  --庄家玩法

            buffer:pushbyte(self.m_fanbei_index - CBT_FANBEI -1)  --倍数规则
            
            buffer:pushscore(self.ruleData.lOperateBankerRatio[1][self.m_zscore_index - CBT_Z_SCORE])  --最大抢庄倍数

            buffer:pushscore(0)  --封顶分数

            print("BankerModeconfig_sangong", self.m_lOpBankerType)
            cc.UserDefault:getInstance():setIntegerForKey("PlayerModeconfig_sangong", self.playerNum_index - CBT_PLAYER_NUM  == 1 and 6 or 8)
            cc.UserDefault:getInstance():setIntegerForKey("BankerModeconfig_sangong", self.m_lOpBankerType)
        
            for i = 1, 79 do
                buffer:pushbyte(0)
            end

            buffer:pushbyte(self.m_tabSelectConfig.cbGameMode)

            buffer:pushscore(self.m_enterScore)
            buffer:pushscore(self.m_exitScore)

            -- if self.m_nOpenMode then --是否允许陌生人
            --     buffer:pushbyte(1)
            -- else
            --     buffer:pushbyte(0)
            -- end
            print("否允许陌生人self._joinMode>>>>>>>>>>>>",self._joinMode)
            buffer:pushbyte(self._joinMode)
            if self.param.clubID ~= nil then
                buffer:pushdword(self.param.clubID)  --0为非俱乐部，俱乐部创建填俱乐部ID
            else
                buffer:pushdword(0)  --0为非俱乐部，俱乐部创建填俱乐部ID
            end
            --buffer:pushdword(0)  --0为非俱乐部，俱乐部创建填俱乐部ID

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


    local bankerType = {"霸王庄","轮流坐庄","牌大坐庄","明牌抢庄","自由抢庄","通比玩法"}

    local PlayerMode = cc.UserDefault:getInstance():getIntegerForKey("PlayerModeconfig_sangong", 8)
    local chairCountstr = string.format("%d人", PlayerMode)
    local configStr = string.format("%s,%d局,", chairCountstr,turnCount)
    configStr = configStr..bankerType[cc.UserDefault:getInstance():getIntegerForKey("bankerType", 1)]
    local shareTxt = configStr .. "。三公精彩刺激, 一起来玩吧!"

    local url = yl.PRIVATE_SHARE_URL ..  "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=33&a=1" .. "&p=" .. passwd
    
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("三公, 房号[ %06d],您的好友喊你打牌了!", dwRoomID)}
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
--print(tag,"=======================>1")
    local index = self.m_bIsNormal and 1 or 2

    if TAG_ENUM.TAG_MASK == tag or TAG_ENUM.BTN_CLOSE == tag then
        self:scaleTransitionOut(self.m_spBg)
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


    elseif TAG_ENUM.BTN_CREATE1 == tag or TAG_ENUM.BTN_CREATE2 == tag then 

        self.m_createBtn:setEnabled(false)
        self:runAction(cc.Sequence:create(
            cc.DelayTime:create(1),
            cc.CallFunc:create(function()
            self.m_createBtn:setEnabled(true)
        end
        )))
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
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE1 == tag)
        PriRoom:getInstance(). m_bPlayModel = 0
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()

    elseif tag >= TAG_ENUM.BTN_RULE1 and tag <=TAG_ENUM.BTN_RULE6  then 
            print(tag - TAG_ENUM.BTN_RULE1 +1,"=======================>2")
            self:updateRule(tag - TAG_ENUM.BTN_RULE1 +1)


    elseif  tag >= CBT_FANBEI and tag<(CBT_FANBEI+10) then 
        local last = self.m_fanbei_index
        self.m_fanbei_index = tag
        for n=1, 2 do
            self.m_checkTable[n][last]:setSelected(false)
            self.m_checkTable[n][self.m_fanbei_index]:setSelected(true)
        end

    elseif  tag >= CBT_PLAYER_NUM + 1 and tag<(CBT_PLAYER_NUM+8) then 
        local last = self.playerNum_index
        print("last",last)
        self.playerNum_index = tag
        for n=1, 2 do
            self.m_checkTable[n][last]:setSelected(false)
            self.m_checkTable[n][self.playerNum_index]:setSelected(true)
        end
    elseif  tag >= CBT_Z_SCORE and tag<(CBT_Z_SCORE+10) then 
        local last = self.m_zscore_index
        self.m_zscore_index = tag
        for n=1,2 do
            self.m_checkTable[n][last]:setSelected(false)
            self.m_checkTable[n][self.m_zscore_index]:setSelected(true)
        end
    elseif  tag >= CBT_X_SCORE and tag<(CBT_X_SCORE+10) then 
        local last = self.m_xscore_index
        self.m_xscore_index = tag
        for n=1,2 do
            self.m_checkTable[n][last]:setSelected(false)
            self.m_checkTable[n][self.m_xscore_index]:setSelected(true)
        end
    elseif  tag >= CBT_BANKER_MODE and tag<(CBT_BANKER_MODE+10) then 
        local last = self.m_bankerMode_index
        self.m_bankerMode_index = tag
        for n=1,2 do
            self.m_checkTable[n][last]:setSelected(false)     
            self.m_checkTable[n][self.m_bankerMode_index]:setSelected(true)
        end
    elseif TAG_ENUM.BTN_CELLSCORE == tag or TAG_ENUM.BTN_ENTERCONDITION == tag or TAG_ENUM.BTN_EXITCONDITION == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, PrivateScoreInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pl)

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

        sender:setSelected(not sender:isSelected())
        
        if self.m_nSelectPasswd == self._cmd_pri.define.tabPasswdMode.SET_PASSWD then
                self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD 
        else
                self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
        end
  
   --checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
   -- checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
end


function PriRoomCreateLayer:updateCreateFee()
    self.m_bLow = false
    
    local fee = 0
    if self.m_nSelectPayMode == self._cmd_pri.define.tabPayMode.AA_PAY then
        -- AA付
        fee = self.m_tabSelectConfig.wAAPayFee or 0
    else
        -- 一人付
        fee = self.m_tabSelectConfig.lFeeScore or 0
    end

    if GlobalUserItem.tabAccountInfo.lDiamond < fee then
        self.m_bLow = true
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
    local roomid = self._param.dwRoomId or 0
    spbg:getChildByName("txt_roomid"):setString(string.format("%06d", roomid))
    -- spbg:getChildByName("Text_1"):setString("12131221321321")

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString("x"..consume)

    -- 玩法
    local wanfa = spbg:getChildByName("txt_wanfa")

    -- 规则
    local guize = spbg:getChildByName("txt_guize")
    local buffer = self._param.buffer
    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读前两个规则
        buffer:readbyte()
        buffer:readbyte()
        buffer:readbyte()

        --抢庄模式(玩法)
        local gameMode = buffer:readbyte()
        local bankerType = {"霸王庄","轮流坐庄","牌大坐庄","明牌抢庄","自由抢庄","通比玩法"}
        print("庄家模式 ", gameMode)
        local szMode = bankerType[gameMode]

        local betMode = buffer:readbyte()
        local betType = {"不翻倍","通用翻倍","几点几倍"}
        print("翻倍模式 ", betMode)
        local szMode = betType[betMode + 1]
 
        -- 玩法
        spbg:getChildByName("txt_wanfa"):setString(szMode)
        -- 规则
        spbg:getChildByName("txt_guize"):setString("")
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