local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
    return gameViewLayer
end)
local module_pre = "game.yule.attlhp.src"

--external
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"

local cmd = appdf.req(module_pre..".models.CMD_Game")

local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"
local QueryDialog   = require("app.views.layer.other.QueryDialog")

local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local LuckyTimeLayer = appdf.req(module_pre .. ".views.layer.LuckyTimeLayer")
local RecordLayer = appdf.req(module_pre .. ".views.layer.RecordLayer")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local BankLayer = appdf.req(module_pre..".views.layer.BankLayer")
local RewardLayer = appdf.req(module_pre..".views.layer.RewardLayer")
local GameLogic = appdf.req(module_pre..".models.GameLogic")


GameViewLayer.TAG_START				= 100
local enumTable = 
{
	"BT_Exit",               -- 退出游戏
    "BT_Help",
    "BT_Set",
    "BT_Record",
	"BT_Menu",               -- 菜单
    "BT_Add",
    "BT_Sub",
    "BT_Clear",
    "BT_Switch",
    "BT_Rebet",
    "BT_Stop",
    "BT_BANK",
    "BT_DeFen",
    "BT_BiBei",
    "CardSprite",
	
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(GameViewLayer.TAG_START, enumTable);


GameViewLayer.FREE = 11
GameViewLayer.PLAY = 12
GameViewLayer.SWITCH = 13
GameViewLayer.LUCKY = 14

--中文长度计算(同步pc,中文长度为2)
local function ClipStringLen(szText, Len)
    if nil == Len or len == 0 then
        return szText
    end

    local i = 1
    while true do
        local cur = string.sub(szText,i,i)
        local byte = string.byte(cur)
        if byte == nil then
            break
        end
        if byte > 128 then
            i = i + 3
            Len = Len - 3
        elseif  byte ~= 32 then
            i = i + 1
            Len = Len - 2
        else
            Len = Len - 2
            i = i + 1
        end
        if Len <= 0 then
            break
        end
    end

    local cur = string.sub(szText, 1,i-1)
    if cur == szText then
        return szText
    else
        return cur .. "..."
    end
end

function GameViewLayer:ctor(scene)
	--注册node事件
	ExternalFun.registerNodeEvent(self)
	
	self._scene = scene

	self:loadRes();
	self:gameDataInit();

    self.betRecordIdx = 1 -- 上局下注筹码记录
    self.betIdx = 1 -- 当前显示筹码数

	--初始化csb界面
	self:initCsbRes();
	--初始化通用动作
	-- self:initAction();
end

function GameViewLayer:getParentNode(  )
	return self._scene
end

function GameViewLayer:getMeUserItem(  )
	if nil ~= GlobalUserItem.tabAccountInfo.dwUserID then
		return self:getDataMgr():getUidUserList()[GlobalUserItem.tabAccountInfo.dwUserID];
	end
	return nil;
end


function GameViewLayer:loadRes()
	--加载卡牌纹理
	-- cc.Director:getInstance():getTextureCache():addImage("game/card.png");

    -- display.loadSpriteFrames(cmd.RES.."game/game.plist",cmd.RES.."game/game.png")
end

---------------------------------------------------------------------------------------
--界面初始化
function GameViewLayer:initCsbRes(  )
	local rootLayer, csbNode = ExternalFun.loadRootCSB("game/GameLayer.csb", self);
	self.m_rootLayer = rootLayer

	self._csbNode = csbNode;

	-- 初始化ui
	self:initUi(csbNode);
    -- self:setNodePoints()
    self.betTxt = self._csbNode:getChildByName("betNum")--:setString(0)
    local node = self._csbNode:getChildByName("Node_bonus")
    self.bounsBet = node:getChildByName("txt_bonus")--:setString("0/80")
end


function GameViewLayer:gameDataInit()
    self.cardHeldStatus = {true,true,true,true,true}
    self.currStatus = GameViewLayer.FREE
end

function GameViewLayer:setBetNum(idx)
    local num = self._scene.lBetItem[idx]
    if not num then return end
    self.betIdx = idx
    self.betTxt:setString(num)
    self.bounsBet:setString(num.."/80")
    self:setNodePoints()
end

--重置本局数据
function GameViewLayer:ReSetGameData()
    self:gameDataInit()

    -- self.betTxt:setString(0)
    -- self.bounsBet:setString("0/80")
    self:setBetNum(1)

    self.currStatus = GameViewLayer.FREE
    self:setBtnStatus()

    for i = 1, 5 do
        local CardSprite = self._csbNode:getChildByName("Image_"..i)
        CardSprite:loadTexture("game/back.png")
        self.heldSprite[i]:setTexture("game/held-1.png")
    end
    
    if self.auto == true then
        self:setBetNum(self.betIdx)
        self._scene:sendPlaceJetton(self._scene.lBetItem[self.betIdx])
        self.betRecordIdx = self.betIdx
        self._scene:SendUserReady()
    end
end




function GameViewLayer:initSelfInfo()
    local headbg = self._csbNode:getChildByName("headbg")
    local myItem = self._scene:GetMeUserItem()
    local head = HeadSprite:createNormal( myItem ,90)
    head:addTo(headbg)
    head:setPosition(headbg:getContentSize().width/2,headbg:getContentSize().height/2)
    self._csbNode:getChildByName("name"):setString(ClipStringLen(myItem.szNickName, 15))
end
function GameViewLayer:updateScore( score )
    self.Myscore = math.floor(score/self._scene.lExchangeRadio)
    self._csbNode:getChildByName("myscore"):setString(self.Myscore)--score/self._scene.lExchangeRadio)
end

function GameViewLayer:setBounsInfo(bouns)
    local node = self._csbNode:getChildByName("Node_bonus")
    node:getChildByName("txt_5k"):setString(bouns.lBonus5K)
    node:getChildByName("txt_RS"):setString(bouns.lBonusRS)
    node:getChildByName("txt_sf"):setString(bouns.lBonusSF)
    node:getChildByName("txt_4k"):setString(bouns.lBonus4K)
end

function GameViewLayer:setNodePoints()
    local times = { {2,1,3,6,8,12,60,140,250,500},
                    {2,1,3,5,7,10,50,120,200,400},
                    {2,1,3,4,6,8,40,100,150,300},
                    }
    local Node_points = self._csbNode:getChildByName("Node_points")
    for i=1, 3 do
        local scoreNode = Node_points:getChildByName("Node_score"..i)
        for j = 1, 10 do
            scoreNode:getChildByName("txt_"..j):setString(self._scene.lBetItem[self.betIdx] * times[i][11-j])
        end
    end
end

--初始化按钮
function GameViewLayer:initUi(csbNode)
    local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self.heldSprite = {}
    for i = 1 , 5 do
        local CardSprite = self._csbNode:getChildByName("Image_"..i)
        CardSprite:setTag(TAG_ENUM.CardSprite+i)
        CardSprite:addTouchEventListener(btcallback)
        local held = self._csbNode:getChildByName("held_"..i)
        table.insert(self.heldSprite, held)
    end
    
    local Node_Button = csbNode:getChildByName("Node_Button")

    local btn = Node_Button:getChildByName("Button_stopAuto")
    btn:setTag(TAG_ENUM.BT_Stop)
    btn:addTouchEventListener(btcallback)
    self.btn_Stop = btn

    local btn = Node_Button:getChildByName("Button_start")
    self.btn_start = btn
    -- btn:setTag(TAG_ENUM.BT_Add)
    self.auto = false
    btn:addTouchEventListener(function(sender, eventType)
        if eventType == ccui.TouchEventType.began then
            if self.Myscore < self._scene.lBetItem[self.betIdx] then
                showToast(self, "金币不足",1)
                return
            end

            self._scene:sendPlaceJetton(self._scene.lBetItem[self.betIdx])
            self.betRecordIdx = self.betIdx
            btn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function() 
                self.auto = true
                self._scene:SendUserReady()
                self._scene:sendGameAuto(self.auto)
                self.btn_Stop:setVisible(true)
            end)))
        elseif tType == ccui.TouchEventType.canceled then
            btn:stopAllActions()
        elseif eventType == ccui.TouchEventType.ended then
            if btn:getNumberOfRunningActions() >0 then
                btn:stopAllActions()
                self._scene:SendUserReady()
                btn:setEnabled(false)
                self.btn_rebet:setEnabled(false)
            end
        end

    end)


    local btn = Node_Button:getChildByName("Button_sub")
    self.btn_sub = btn
    btn:setTag(TAG_ENUM.BT_Sub)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_add")
    self.btn_add = btn
    btn:setTag(TAG_ENUM.BT_Add)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_clear")
    self.btn_clear = btn
    btn:setTag(TAG_ENUM.BT_Clear)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_switch")
    self.btn_switch = btn
    btn:setTag(TAG_ENUM.BT_Switch)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_rebet")
    self.btn_rebet = btn
    btn:setTag(TAG_ENUM.BT_Rebet)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_record")
    btn:setTag(TAG_ENUM.BT_Record)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_bank")
    btn:setTag(TAG_ENUM.BT_BANK)
    btn:addTouchEventListener(btcallback)

    self.btn_defen = Node_Button:getChildByName("Button_defen")
    self.btn_defen:setTag(TAG_ENUM.BT_DeFen)
    self.btn_defen:addTouchEventListener(btcallback)
    self.btn_defen:setEnabled(false)

    self.btn_bibei = Node_Button:getChildByName("Button_bibei")
    self.btn_bibei:setTag(TAG_ENUM.BT_BiBei)
    self.btn_bibei:addTouchEventListener(btcallback)
    self.btn_bibei:setEnabled(false)

    local node_menu = Node_Button:getChildByName("Node_menu"):setVisible(false)
    self.node_menu = node_menu

    local btn = node_menu:getChildByName("Button_exit")
    btn:setTag(TAG_ENUM.BT_Exit)
    btn:addTouchEventListener(btcallback)

    local btn = node_menu:getChildByName("Button_help")
    btn:setTag(TAG_ENUM.BT_Help)
    btn:addTouchEventListener(btcallback)

    local btn = node_menu:getChildByName("Button_set")
    btn:setTag(TAG_ENUM.BT_Set)
    btn:addTouchEventListener(btcallback)

    local btn = Node_Button:getChildByName("Button_menu")
    btn:setTag(TAG_ENUM.BT_Menu)
    btn:addTouchEventListener(btcallback)

    self:setBtnStatus(  )
end

function GameViewLayer:setBtnStatus()
    self.btn_start:setEnabled(self.currStatus == GameViewLayer.FREE)
    self.btn_switch:setEnabled(self.currStatus == GameViewLayer.SWITCH)
    self.btn_add:setEnabled(self.currStatus == GameViewLayer.FREE)
    self.btn_sub:setEnabled(self.currStatus == GameViewLayer.FREE)
    self.btn_rebet:setEnabled(self.currStatus == GameViewLayer.FREE)
    self.btn_defen:setEnabled(self.currStatus == GameViewLayer.LUCKY)
    self.btn_bibei:setEnabled(self.currStatus == GameViewLayer.LUCKY)
end

------------------------------------------------
function GameViewLayer:setHandCard(cardData)
    for i =1, 5 do
        local CardSprite = self._csbNode:getChildByName("Image_"..i)
        local value = GameLogic.GetCardValue( cardData[i] )
        local color = GameLogic.GetCardColor( cardData[i] )
        CardSprite:loadTexture("poker/"..color.."_"..value..".png")
    end
end

function GameViewLayer:sendCards( cardData )
    local cardData = cardData --self._scene.cbFirstCardArray
        local idx = 1
    local function turnCard()
        local CardSprite = self._csbNode:getChildByName("Image_"..idx)
        local value = GameLogic.GetCardValue( cardData[idx] )
        local color = GameLogic.GetCardColor( cardData[idx] )
        print("value,color",value,color)

        local action = cc.Sequence:create(cc.ScaleTo:create(0.1,0.01,1), cc.CallFunc:create(function ()
        CardSprite:loadTexture("poker/"..color.."_"..value..".png")
        end), cc.ScaleTo:create(0.1,1,1), cc.DelayTime:create(0.1), cc.CallFunc:create(function ()
            idx = idx + 1
            if idx <= 5 then
                turnCard()
            else
                self.currStatus = GameViewLayer.SWITCH
                self:setBtnStatus()
                if self.auto == true then
                    self._scene:sendSwitchCard()
                end
            end
        end))
        CardSprite:runAction(action)
    end
    turnCard()
end

function GameViewLayer:showBtnAnim()
    print("donghua ***************")
    self.btn_defen:setEnabled(true)
    self.btn_bibei:setEnabled(true)

    self.btn_defen:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.ScaleTo:create(1,1.2),cc.ScaleTo:create(1,1))))
    self.btn_bibei:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.ScaleTo:create(1,1.2),cc.ScaleTo:create(1,1))))
end

function GameViewLayer:stopBtnAnim()
    self.btn_defen:setEnabled(false)
    self.btn_bibei:setEnabled(false)

    self.btn_defen:stopAllActions()
    self.btn_bibei:stopAllActions()
    self.btn_defen:setScale(1)
    self.btn_bibei:setScale(1)
end

function GameViewLayer:switchCards( bswitch, bGuess)
    local cardData = self._scene.cbSecondCardArray
    dump(cardData,"cardData")
    local idx = 0
    local function turnCard()
        idx = idx + 1
        if self.auto == false and not bswitch[idx] then
            if idx < 5 then
                return turnCard()
            else
                if self.auto then
                    self._scene:sendGameEnd(self._scene.lBetItem[self.betIdx])
                else
                    if bGuess then
                        self:showBtnAnim()
                    else
                        self._scene:sendGameEnd(self._scene.lBetItem[self.betIdx])
                    end
                end
                return
            end
        end

        local CardSprite = self._csbNode:getChildByName("Image_"..idx)
        local value = GameLogic.GetCardValue( cardData[idx] )
        local color = GameLogic.GetCardColor( cardData[idx] )

        local action = cc.Sequence:create(
            cc.CallFunc:create(function ()
                CardSprite:loadTexture("poker/hongse.png")
            end), 
            cc.ScaleTo:create(0.1,0.01,1), 
            cc.CallFunc:create(function ()
                CardSprite:loadTexture("poker/"..color.."_"..value..".png")
            end), 
            cc.ScaleTo:create(0.1,1,1), cc.DelayTime:create(0.1), 
            cc.CallFunc:create(function ()
                if idx < 5 then
                    turnCard()
                else

                    if self.auto then
                        self._scene:sendGameEnd(self._scene.lBetItem[self.betIdx])
                    else
                        if bGuess then

                            self:showBtnAnim()
                        else
                            self._scene:sendGameEnd(self._scene.lBetItem[self.betIdx])
                        end
                    end
                end
            end)
            )
        CardSprite:runAction(action)
    end
    turnCard()
end

function GameViewLayer:showResult(score)
    local csbNode = ExternalFun.loadCSB("game/ResultLayer.csb",self._csbNode)
    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(5),cc.CallFunc:create(function()
        csbNode:removeFromParent() 
        self:ReSetGameData()
        if self.luckLayer and tolua.cast(self.luckLayer, "cc.Node") then
            self.luckLayer:removeFromParent()
            self.luckLayer = nil
        end
    end)))

    local btn = csbNode:getChildByName("btn_close")
    btn:addTouchEventListener(function(sender, EventType)
        if EventType == ccui.TouchEventType.ended then
            csbNode:removeFromParent()
            self:ReSetGameData()
            if self.luckLayer and tolua.cast(self.luckLayer, "cc.Node") then
                self.luckLayer:removeFromParent()
                self.luckLayer = nil
            end
        end
    end)

    local lScore = csbNode:getChildByName("score_lose"):setVisible(false)
    local wScore = csbNode:getChildByName("score_win"):setVisible(false)
    local spLose = csbNode:getChildByName("image_lose"):setVisible(false)
    local spWin = csbNode:getChildByName("image_win"):setVisible(false)
    local title = csbNode:getChildByName("title")
    local sp = cc.Sprite:create("game/result/lose_wan.png")
    sp:setVisible(false)
    sp:setAnchorPoint(0,0.5)
    sp:addTo(csbNode,2,2)
    local fileName = "game/result/"
    if score > 0 then
        title:setTexture("game/result/win.png")
        wScore:setString("/"..score)
        wScore:setVisible(true)
        spWin:setVisible(true)
        if math.abs(score) >= 100000000 then
            wScore:setString("/"..math.abs(score/100000000))
            fileName = fileName .. "win_yi.png"
            sp:setTexture(fileName)
            sp:setVisible(true)
        elseif math.abs(score) >= 10000 then
            wScore:setString("/"..math.abs(score/10000))
            fileName = fileName .. "win_wan.png"
            sp:setTexture(fileName)
            sp:setVisible(true)
        end
        sp:setPosition(wScore:getPositionX() + wScore:getContentSize().width ,wScore:getPositionY())
    else
        title:setTexture("game/result/lose.png")
        lScore:setString("/"..math.abs(score))
        lScore:setVisible(true)
        spLose:setVisible(true)

        if math.abs(score) >= 100000000 then
            lScore:setString("/"..math.abs(score/100000000))
            fileName = fileName .. "lose_yi.png"
            sp:setTexture(fileName)
            sp:setVisible(true)
        elseif math.abs(score) >= 10000 then
            lScore:setString("/"..math.abs(score/10000))
            fileName = fileName .. "lose_wan.png"
            sp:setTexture(fileName)
            sp:setVisible(true)
        end
        sp:setPosition(lScore:getPositionX() + lScore:getContentSize().width ,lScore:getPositionY())
    end


end

function GameViewLayer:showLuckyTime()
    self.luckLayer = LuckyTimeLayer:create(self._scene, self._scene.lBetItem[self.betIdx])
    self._csbNode:addChild(self.luckLayer)
end

function GameViewLayer:GuessResult(CardData)
    if tolua.cast(self.luckLayer, "cc.Node")  then
        self.luckLayer:getGuessResult(CardData)
    end
end

-- 倒计时
function GameViewLayer:onUpdateTime()
    if self.wait_time_lab then
        self.wait_time_lab:setString(self.waitCurTime);
    end

    self.waitCurTime = self.waitCurTime - 1;

    if self.waitCurTime < 0 then
        self.waitStationLayer:setVisible(false);
        if self._timeLessUpdate then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeLessUpdate);
            self._timeLessUpdate = nil;
        end
    end
end

-----------------------------------------------------------------------------------------------
function GameViewLayer:onResetView()
    self:stopAllActions();
end

function GameViewLayer:onExit()
    self:onResetView();
end

---------------------------------------------------------------------------------
function GameViewLayer:onButtonClickedEvent(tag,ref)
    ExternalFun.playClickEffect();
    if tag == TAG_ENUM.BT_Exit then
        self._scene:onQueryExitGame()
    elseif tag == TAG_ENUM.BT_Help then
        self._csbNode:addChild(HelpLayer:create())
        self.node_menu:setVisible(false)
    elseif tag == TAG_ENUM.BT_Set then
        local setting = SettingLayer:create(self)
        self._csbNode:addChild(setting)
        self.node_menu:setVisible(false)
    elseif tag == TAG_ENUM.BT_Menu then
        self.node_menu:setVisible(not self.node_menu:isVisible())
        -- local l = { 0x01,0x1a, 0x3b,0x1c,0x1d}

        -- dump(GameLogic:GetCardType(l),"ceshi")
    elseif tag == TAG_ENUM.BT_Add then
        print("self.betIdx",self.betIdx)
        -- if self.betIdx >= 10 or self.Myscore < self._scene.lBetItem[self.betIdx + 1] then
        --     self.btn_add:setEnabled(false)
        --     return
        -- else
        --     self.btn_add:setEnabled(true)
        -- end
        if self.betIdx < 10 then
            self.betIdx = self.betIdx + 1
        else
            self.betIdx = 1
        end

        self:setBetNum(self.betIdx)

    elseif tag == TAG_ENUM.BT_Sub then
        print("self.Myscore",self.Myscore)
        if self.betIdx > 1 then
            self.betIdx = self.betIdx - 1
        else
            for k,v in pairs(self._scene.lBetItem) do
                if self.Myscore >= v then
                    self.betIdx = k
                end
            end
            -- self.betIdx = 10
        end
        if self.Myscore >= self._scene.lBetItem[self.betIdx] then
            self.btn_add:setEnabled(true)
        end
        self:setBetNum(self.betIdx)

    elseif tag == TAG_ENUM.BT_Clear then
        self:setBetNum(1)

    elseif tag == TAG_ENUM.BT_Record then
        self._csbNode:addChild(RecordLayer:create(self._scene.recordInfo))

    elseif tag == TAG_ENUM.BT_Stop then
        self.auto = false
        self.btn_Stop:setVisible(false)

    elseif tag == TAG_ENUM.BT_Rebet then
        if self.Myscore < self._scene.lBetItem[self.betIdx] then
            showToast(self, "金币不足",1)
            return
        end
        self:setBetNum(self.betRecordIdx)
        self._scene:sendPlaceJetton(self._scene.lBetItem[self.betRecordIdx])
        self._scene:SendUserReady()
        self.btn_rebet:setEnabled(false)

    elseif tag == TAG_ENUM.BT_Switch then
        self.btn_switch:setEnabled(false)
        self._scene:sendSwitchCard()

    elseif tag >= TAG_ENUM.CardSprite +1 and tag <= TAG_ENUM.CardSprite +5 then
        if self.currStatus ~= GameViewLayer.SWITCH then return end
        if self.cardHeldStatus[tag - TAG_ENUM.CardSprite] then
            self.heldSprite[tag - TAG_ENUM.CardSprite]:setTexture("game/held-2.png")
            self.cardHeldStatus[tag - TAG_ENUM.CardSprite] = false
        else
            self.heldSprite[tag - TAG_ENUM.CardSprite]:setTexture("game/held-1.png")
            self.cardHeldStatus[tag - TAG_ENUM.CardSprite] = true
        end
    elseif tag == TAG_ENUM.BT_BANK then
        --银行
        if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
             return 
        end

        self._scene:sendRequestBankInfo()

    elseif tag == TAG_ENUM.BT_BiBei then
        self._scene:sendLuckyTime()
        self:stopBtnAnim()

    elseif tag == TAG_ENUM.BT_DeFen then
        self._scene:sendGameEnd(self._scene.lBetItem[self.betIdx])
        self:stopBtnAnim()
    else
        showToast(self,"功能尚未开放！",1)
    end
end


-- -- 银行
-- function GameViewLayer:onBankLayer()
--     self.m_bankLayer = bank;
-- 	bank.m_parent = self;
-- 	self.node_layer:addChild(bank,10);

-- end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    GlobalUserItem.tabAccountInfo.lUserScore  = bankinfo.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bankinfo.lUserInsure

    local bank = BankLayer:create(self);
    self._csbNode:addChild(bank)
    self.m_bankLayer = bank

    if bank then
        bank:refreshBankRate(bankinfo.wRevenueTake)
    end
end

--移除银行
function GameViewLayer:removeBankLayer()
    if self.m_bankLayer then
        self.m_bankLayer:removeFromParent()
        self.m_bankLayer = nil
    end
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
    local bank_success = self:getParentNode().bank_success
    if nil == bank_success then
        return
    end
    
    GlobalUserItem.tabAccountInfo.lUserScore= bank_success.lUserScore
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
        self.m_bankLayer:refreshBankScore()
    end
    showToast(self, bank_success.szDescribrString, 2)
end

--银行操作失败
function GameViewLayer:onBankFailure( )
    local bank_fail = self:getParentNode().bank_fail
    if nil == bank_fail then
        return
    end
    print("银行操作失败",bank_fail.szDescribeString)
    showToast(self, bank_fail.szDescribeString, 2)
end


function GameViewLayer:canUseBank()

    if GameViewLayer.FREE ~= self.currStatus then
        showToast(self,"已下注不可进行银行操作",1)
        return false
    end
    -- if self:isMeChair(self.m_wBankerUser) and self.m_cbGameStatus ~= cmd.GAME_SCENE_FREE then
    --     showToast(self,"非空闲时间段不可进行银行操作",1)
    --     return false
    -- end
    return true 
end

return GameViewLayer