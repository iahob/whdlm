local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")

local TrustLayer = class("TrustLayer", cc.Layer)

TrustLayer.BOX_BANKER = 1

TrustLayer.BOX_BANKER_RATIO = 10

TrustLayer.BOX_BETS = 20
TrustLayer.BOX_BETS_CONFIG = 30

TrustLayer.BT_CLOSE = 200
TrustLayer.BT_RATIO = 101
TrustLayer.BT_BETS  = 102
TrustLayer.BT_TIPS  = 103

TrustLayer.BT_MSAKBANKER  = 105     --抢庄下拉后背景
TrustLayer.BT_MSAKBETS    = 106     --下注下拉后背景
TrustLayer.BT_TRUST       = 107     --下注下拉后背景

TrustLayer.TAG_MASK = 110
TrustLayer.TAG_MASKTIPS = 111

function TrustLayer:ctor(scene)
    --加载csb资源
     --注册触摸事件
    local csbNode = ExternalFun.loadCSB("trust/LayerTrust.csb", self)
    self.m_csbNode = csbNode
    self._scene = scene

    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:onBtnClick(sender:getTag(), sender)
        end
    end


    local mask = csbNode:getChildByName("mask")  
    mask:setTouchEnabled(true)
    mask:setTag(TrustLayer.TAG_MASK)
    mask:addTouchEventListener(btnEvent)


    local mask = csbNode:getChildByName("mask_tips")  
    mask:setTouchEnabled(true)
    mask:setTag(TrustLayer.TAG_MASKTIPS)
    mask:addTouchEventListener(btnEvent)
    self.tipsMask = mask

    --抢庄倍数
    local btn_zhuangRaito = csbNode:getChildByName("btn_zhuangRaito")
    btn_zhuangRaito:setTag(TrustLayer.BT_RATIO)
    btn_zhuangRaito:addTouchEventListener(btnEvent)
    self.btnBankerRatio = btn_zhuangRaito

    local btn = csbNode:getChildByName("btn_zhuangRaito1")
    btn:setTag(TrustLayer.BT_RATIO)
    btn:addTouchEventListener(btnEvent)
    self.btnBankerRatio1 = btn

    --加注配置
    local btn_betsInfo = csbNode:getChildByName("btn_betsInfo")
    btn_betsInfo:setTag(TrustLayer.BT_BETS)
    btn_betsInfo:addTouchEventListener(btnEvent)
    self.btnBetsInfo = btn_betsInfo

    local btn = csbNode:getChildByName("btn_betsInfo1")
    btn:setTag(TrustLayer.BT_BETS)
    btn:addTouchEventListener(btnEvent)
    self.btnBetsInfo1 = btn

    local btn = csbNode:getChildByName("btn_betstips")
    btn:setTag(TrustLayer.BT_TIPS)
    btn:addTouchEventListener(btnEvent)

    local btn = csbNode:getChildByName("btn_trust")
    btn:setTag(TrustLayer.BT_TRUST)
    btn:addTouchEventListener(btnEvent)

    --关闭按钮
    local btn = csbNode:getChildByName("btn_close")
    btn:setTag(TrustLayer.BT_CLOSE)
    btn:addTouchEventListener(btnEvent)



    local boxBankerListener = function (sender,eventType)
        self:onBoxBankerSelectedEvent(sender:getTag(),sender)
    end

    --复选框----抢庄
    self.tabBoxBanker = {}
    for i=1, 2 do
        local check = csbNode:getChildByName("checkZhuang".. i)
                        :setTag(TrustLayer.BOX_BANKER + i)
        check:addEventListener(boxBankerListener)
        self.tabBoxBanker[TrustLayer.BOX_BANKER + i] = check
        check:setSelected(false) 

        if i == 1 then 
            check:setSelected(true) 
            self.selectTabBanker = TrustLayer.BOX_BANKER + 1 
        end
    end

    --抢庄倍数
    local boxBankerRatioListener = function (sender,eventType)
        self:onBoxBankerRatioSelectedEvent(sender:getTag(),sender)
    end

    --复选框----抢庄倍数
    self.tabBoxBankerRatio = {}

    local mask1 = csbNode:getChildByName("mask1")
    mask1:setTag(TrustLayer.BT_MSAKBANKER)
    mask1:addTouchEventListener(btnEvent)
    self.mask1 = mask1

    local imgBg = mask1:getChildByName("imgBg")

    for i=1, 4 do
        local check = imgBg:getChildByName("ratio".. i)
                        :setTag(TrustLayer.BOX_BANKER_RATIO + i)
        check:addEventListener(boxBankerRatioListener)
        self.tabBoxBankerRatio[TrustLayer.BOX_BANKER_RATIO + i] = check

        if i == 1 then 
            check:setSelected(true) 
            self.selectTabBankerRatio = TrustLayer.BOX_BANKER_RATIO + 1 
        end
    end

    --显示字串
    self.bankerRatio = csbNode:getChildByName("txt_zhuangRatio")
    

    local boxBetsListener = function (sender,eventType)
        self:onBoxBetsSelectedEvent(sender:getTag(),sender)
    end

    --复选框----下注
    self.tabBoxBets = {}
    for i=1, 3 do
        local check = csbNode:getChildByName("checkBets".. i)
                        :setTag(TrustLayer.BOX_BETS + i)
        check:addEventListener(boxBetsListener)
        self.tabBoxBets[TrustLayer.BOX_BETS + i] = check

        if i == 1 then 
            check:setSelected(true) 
            self.selectTabBets = TrustLayer.BOX_BETS + 1 
        end
    end

    --下注配置
    local boxBetsConfigListener = function (sender,eventType)
        self:onBoxBetsConfigSelectedEvent(sender:getTag(),sender)
    end

    --复选框----下注配置
    self.tabBoxBetsConfig = {}
    local mask2 = csbNode:getChildByName("mask2")
    mask2:setTag(TrustLayer.BT_MSAKBETS)
    mask2:addTouchEventListener(btnEvent)
    self.mask2 = mask2

    local imgBg = mask2:getChildByName("imgBg")
    for i=1, 6 do
        local check = imgBg:getChildByName("ratio".. i)
                        :setTag(TrustLayer.BOX_BETS_CONFIG + i)
        check:addEventListener(boxBetsConfigListener)
        self.tabBoxBetsConfig[TrustLayer.BOX_BETS_CONFIG + i] = check

        if i == 1 then 
            check:setSelected(true) 
            self.selectTabBetsConfig = TrustLayer.BOX_BETS_CONFIG + 1 
        end
    end

    --显示字串
    self.betsConfig = csbNode:getChildByName("txtBetsInfo")

    self.spTips = csbNode:getChildByName("imgTipsBg")

end

function TrustLayer:initLayer(param)
    self.param = param

    if param.cbRobBankerType ~= cmd.EM_OP_BANKER_MPQZ then
        self.btnBankerRatio:setEnabled(false)
        self.btnBankerRatio1:setEnabled(false)

        self.btnBetsInfo:setEnabled(false)
        self.btnBetsInfo1:setEnabled(false)


        local check = self.m_csbNode:getChildByName("checkBets".. 3)
        check:setEnabled(false)
    end

    for i=1, 2 do
        local check = self.m_csbNode:getChildByName("checkZhuang".. i)
        if param.cbRobBankerType ~= cmd.EM_OP_BANKER_MPQZ and param.cbRobBankerType ~= cmd.EM_OP_BANKER_ZYQZ  then
            check:setEnabled(false)
        end
    end

    --统计个数
    local btnNum = 4
    local imgBg = self.mask1:getChildByName("imgBg")
    for i=1, 4 do
        local check = imgBg:getChildByName("ratio".. i)

        local txt = check:getChildByName("txt")
        txt:setString("抢" .. param.cbRobBankerRatio[i] .. "倍")

        if 0 == param.cbRobBankerRatio[i] then
            check:setVisible(false)
            btnNum = btnNum -1
        end
    end
    self.bankerRatio:setString("抢" .. param.cbRobBankerRatio[1] .. "倍")

    --设置背景大小
    local height = btnNum * 50
    imgBg:setContentSize(cc.size(144, height))
    --设置按钮坐标
    for i=1, 4 do
        local check = imgBg:getChildByName("ratio".. i)
        check:setPositionY(height + 25 - 50 *i)
    end
end

function TrustLayer:onBtnClick( tag, sender )
    print("TrustLayer:nBtnClick------**************------>", tag)
    if TrustLayer.BT_CLOSE == tag or TrustLayer.TAG_MASK == tag then
        ExternalFun.playClickEffect()
        self:hideLayer()
        dump(self._scene.trustConfig, "托管配置------》", 7)
    elseif TrustLayer.BT_RATIO == tag then
        if self.mask1:isVisible() then
            self.mask1:setVisible(false)

            -- 更新箭头
            self.btnBankerRatio1:loadTextureDisabled("trust/btn_table2.png")
            self.btnBankerRatio1:loadTextureNormal("trust/btn_table1.png")
            self.btnBankerRatio1:loadTexturePressed("trust/btn_table2.png")
        else
            self.mask1:setVisible(true)

            -- 更新箭头
            self.btnBankerRatio1:loadTextureDisabled("trust/btn_table1.png")
            self.btnBankerRatio1:loadTextureNormal("trust/btn_table2.png")
            self.btnBankerRatio1:loadTexturePressed("trust/btn_table1.png")
        end
    elseif TrustLayer.BT_BETS == tag then
        if self.mask2:isVisible() then
            self.mask2:setVisible(false)

            -- 更新箭头
            self.btnBetsInfo1:loadTextureDisabled("trust/btn_table2.png")
            self.btnBetsInfo1:loadTextureNormal("trust/btn_table1.png")
            self.btnBetsInfo1:loadTexturePressed("trust/btn_table2.png")
        else
            self.mask2:setVisible(true)

            -- 更新箭头
            self.btnBetsInfo1:loadTextureDisabled("trust/btn_table1.png")
            self.btnBetsInfo1:loadTextureNormal("trust/btn_table2.png")
            self.btnBetsInfo1:loadTexturePressed("trust/btn_table1.png")
        end
  
    elseif TrustLayer.BT_MSAKBANKER  == tag then
        self.mask1:setVisible(false)

           -- 更新箭头
        self.btnBankerRatio1:loadTextureDisabled("trust/btn_table2.png")
        self.btnBankerRatio1:loadTextureNormal("trust/btn_table1.png")
        self.btnBankerRatio1:loadTexturePressed("trust/btn_table2.png")
    elseif TrustLayer.BT_MSAKBETS == tag then
        self.mask2:setVisible(false)

        -- 更新箭头
        self.btnBetsInfo1:loadTextureDisabled("trust/btn_table2.png")
        self.btnBetsInfo1:loadTextureNormal("trust/btn_table1.png")
        self.btnBetsInfo1:loadTexturePressed("trust/btn_table2.png")
    elseif TrustLayer.BT_TIPS  == tag then
        if self.spTips:isVisible() then
            self.spTips:setVisible(false)
            self.tipsMask:setVisible(false)
        else
            self.spTips:setVisible(true)
            self.tipsMask:setVisible(true)
        end
    elseif TrustLayer.BT_TRUST  == tag then
       self:hideLayer()
    
        self._scene.trust = true
        self._scene._gameView:ShowTrust(true)
    elseif TrustLayer.TAG_MASKTIPS  == tag then
        self.spTips:setVisible(false)
        self.tipsMask:setVisible(false)
    end
end

--抢庄
function TrustLayer:onBoxBankerSelectedEvent( tag, sender )
    print("onBoxBankerSelectedEvent---------->", tag, self.selectTab)
    if self.selectTabBanker == tag then
        sender:setSelected(true)
        return
    end
    sender:setSelected(true)
    self.selectTabBanker = tag

    for k,v in pairs(self.tabBoxBanker) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    --更新
    self._scene.trustConfig.BankerType = self.selectTabBanker - TrustLayer.BOX_BANKER
end

--抢庄倍数
function TrustLayer:onBoxBankerRatioSelectedEvent( tag, sender )
    print("onBoxBankerSelectedEvent---------->", tag, self.selectTab)
    if self.selectTabBankerRatio == tag then
        sender:setSelected(true)
        return
    end
    sender:setSelected(true)
    self.selectTabBankerRatio = tag

    for k,v in pairs(self.tabBoxBankerRatio) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    --更新
    self._scene.trustConfig.BankerRatio = self.param.cbRobBankerRatio[self.selectTabBankerRatio - TrustLayer.BOX_BANKER_RATIO]
    self.bankerRatio:setString("抢" .. self._scene.trustConfig.BankerRatio .. "倍")
end

--下注
function TrustLayer:onBoxBetsSelectedEvent( tag, sender )
    print("onBoxBankerSelectedEvent---------->", tag, self.selectTab)
    if self.selectTabBets == tag then
        sender:setSelected(true)
        return
    end
    sender:setSelected(true)
    self.selectTabBets = tag

    for k,v in pairs(self.tabBoxBets) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    --更新对应的玩法
    self._scene.trustConfig.BetsType = self.selectTabBets - TrustLayer.BOX_BETS
  
end

--下注配置
function TrustLayer:onBoxBetsConfigSelectedEvent( tag, sender )
    print("onBoxBankerSelectedEvent---------->", tag, self.selectTab)
    if self.selectTabBetsConfig == tag then
        sender:setSelected(true)
        return
    end
    sender:setSelected(true)
    self.selectTabBetsConfig = tag

    for k,v in pairs(self.tabBoxBetsConfig) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    
    --更新对应的玩法
    self._scene.trustConfig.BetsConfig = self.selectTabBetsConfig - TrustLayer.BOX_BETS_CONFIG
  
    local betsTimesTips = {"公七以上最大金额下注", "公八以上最大金额下注","公九以上最大金额下注",
                        "三公以上最大金额下注", "炸弹以上最大金额下注","豹九最大金额下注"}
    self.betsConfig:setString(betsTimesTips[self.selectTabBetsConfig - TrustLayer.BOX_BETS_CONFIG])
end


function TrustLayer:showLayer()
    self:setVisible(true)
end

function TrustLayer:hideLayer()
    self:setVisible(false)
end

return TrustLayer