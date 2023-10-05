local LogonLayer = class("LogonLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- ui
local ServiceLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.logon.ServiceLayer")
local NoticeLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.logon.NoticeLayer")
local QueryExit = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")
local SpineObj = appdf.req("client.src.external.SpineObj")
-- ui

local BTN_NOTICE = 100 		-- 公告
local BTN_SERVICE = 101 	-- 客服
local BTN_WECHAT = 102 		-- 微信
local BTN_EXIT = 103        -- 退出
local DG_QUERYEXIT = 104    -- 退出提示

local BTN_COPY = 105

local tabTips = 
{
    "温馨提示:最强大的账号安全防护体系, 为您的账号安全保驾护航!",
    "温馨提示:最大的牌不是皇家同花顺, 而是手里的牌…",
    "温馨提示:断臂也比失去生命强. 牌局如此, 股市如此, 人生亦如此",
}
local POP_LAYER_NAME = "__pop_layer_name__"
function LogonLayer:ctor( scene )
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
	-- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("logon/LogonLayer.csb", self)
    self.m_rootLayer = rootLayer
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 公告按钮
    local btn = csbNode:getChildByName("btn_notice")
    btn:setTag(BTN_NOTICE)
    btn:addTouchEventListener( touchFunC )
    local noticeSize = btn:getContentSize()
    self.m_noticeArea = cc.size(640, noticeSize.height)
    btn:setVisible(false)

    --喇叭
    local sp = csbNode:getChildByName("logon_btn_notice_0_0_1")
    sp:setVisible(false)

    -- 公告区域
    local stencil  = display.newSprite()
        :setAnchorPoint(cc.p(0,0.5))
    stencil:setTextureRect(cc.rect(0, 0, 640, noticeSize.height))
    self._notifyClip = cc.ClippingNode:create(stencil)
        :setAnchorPoint(cc.p(0,0.5))
    self._notifyClip:setInverted(false)
    self._notifyClip:move(140,noticeSize.height * 0.5)
    self._notifyClip:addTo(btn)
    self._notifyText = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
                                :addTo(self._notifyClip)
                                :setTextColor(cc.c4b(255,255,255,255))
                                :setAnchorPoint(cc.p(0,0.5))
                                --:enableOutline(cc.c4b(79,48,35,255), 1)
    -- 公告内容
    self.m_tabSystemNotice = {}
    -- 播放索引
    self._sysIndex = 1
    -- 公告是否运行
    self.m_bNotifyRunning = false

    -- 客服按钮
    btn = csbNode:getChildByName("btn_service")
    btn:setTag(BTN_SERVICE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 微信登陆
    btn = csbNode:getChildByName("btn_wechat")
    btn:setTag(BTN_WECHAT)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    self.m_btnWeChat = btn


    self.m_loadingBg = csbNode:getChildByName("loadingBg")
    self.m_loadingBg:setVisible(false)



    -- 进度条背景
    self.m_spLoadingBar = self.m_loadingBg:getChildByName("logon_sp_bar")
    self.m_LoadingBarSize = self.m_spLoadingBar:getContentSize()

    self.m_loadingNum = self.m_loadingBg:getChildByName("AtlasLabel_1")
    


    -- 进度条
    local pTimer = cc.ProgressTimer:create(cc.Sprite:create("logon/logon_newl1.png"))
    pTimer:setType(cc.PROGRESS_TIMER_TYPE_BAR)
    pTimer:setMidpoint(cc.p(0.0,0.5))
    pTimer:setBarChangeRate(cc.p(1, 0))
    pTimer:setPercentage(0.0)
    pTimer:setPosition(self.m_spLoadingBar:getPosition())
    self.m_loadingBg:addChild(pTimer)
    self.m_loadingBar = pTimer
    -- 提示条
    -- local spThumb = cc.Sprite:create("logon/logon_sp_bar_thumb.png")
    -- pTimer:addChild(spThumb)
    -- spThumb:setAnchorPoint(cc.p(1, 0.5))
    -- spThumb:setPosition(0, -7)
    -- self.m_spThumb = spThumb
    -- self.m_spThumb:setVisible(false)

    -- local spThumb = SpineObj:create("loading.json", "loading.atlas", true)
    -- --spThumb:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
    -- pTimer:addChild(spThumb)
    -- spThumb:setAnchorPoint(cc.p(1, 0.5))
    -- spThumb:setPosition(0, 0)
    -- self.m_spThumb = spThumb
    -- self.m_spThumb:setVisible(false)
    local particle = cc.ParticleSystemQuad:create("logon/tuowei.plist")
    self.m_loadingBg:addChild(particle)
    particle:setAnchorPoint(cc.p(1, 0.5))
    particle:setPosition(28, 21)
    self.m_spThumb = particle
    self.m_spThumb:setVisible(false)


    self.m_actCreateBtnAni = ExternalFun.loadTimeLine( "logon/LogonLayer.csb")
    self.m_actCreateBtnAni:gotoFrameAndPlay(0,true)
    self:runAction(self.m_actCreateBtnAni)


    -- 温馨提示
    self.m_txtLoadingTips = self.m_loadingBg:getChildByName("txt_loadingtips")
    self.m_txtLoadingTips:setString("")

    -- 版本号
    local txtver = csbNode:getChildByName("txt_version")
    local verstr = scene:getApp():getVersionMgr():getResVersion() or "0"
    txtver:setString("v" .. appdf.BASE_C_VERSION .. "." .. verstr)


   --self:initCheckButton(csbNode)

    self:registerScriptKeypadHandler(function(event)
        if event == "backClicked" then
            self:onButtonClickedEvent(BTN_EXIT)
        end
    end)
    self:setKeyboardEnabled(true)
end

function LogonLayer:onExitTransitionStart()
    self:unregisterScriptKeypadHandler()
    self:setKeyboardEnabled(false)
end

function LogonLayer:onButtonClickedEvent(tag,ref)
	if tag == BTN_NOTICE then
		self:popNoticeLayer()
        --[[local msg = {}
        msg.button = "公告"
        msg.time = os.time()
        analysisFunc(cjson.encode(msg), "GET")]]
        --[[local token = MultiPlatform:getInstance():getDeviceToken()
        showToast(self, token, 3)]]
	elseif tag == BTN_SERVICE then
		local ser = ServiceLayer:create(self._scene)
        ser:setName(POP_LAYER_NAME)
		self._scene:addChild(ser)
        --[[local msg = {}
        msg.button = "客服"
        msg.time = os.time()
        analysisFunc(cjson.encode(msg), "POST")]]
	elseif tag == BTN_WECHAT then
		self.m_btnWeChat:setVisible(false)
        self._scene:thirdPartyLogin(yl.ThirdParty.WECHAT)
    elseif tag == BTN_EXIT then
        --退出按钮
        if self._scene:getChildByTag(DG_QUERYEXIT) then
            return
        end
        QueryExit:create("确认退出游戏平台吗？",function(ok)
            if ok == true then
                os.exit(0)
            end
        end, 30)
        :setCanTouchOutside(false)
        :setTag(DG_QUERYEXIT)
        :addTo(self._scene)
	end
end

function LogonLayer:autoLogon( bFirst )
    -- 判断是否授权
    if yl.AUTO_LOGIN and MultiPlatform:getInstance():isAuthorized(yl.ThirdParty.WECHAT) and bFirst then
        self.m_btnWeChat:setVisible(false)
        self._scene:thirdPartyLogin(yl.ThirdParty.WECHAT)
    else
        self.m_btnWeChat:setVisible(true)
    end
end

function LogonLayer:onLogon()
	self.m_loadingBg:setVisible(true)
    self.m_spThumb:setVisible(true)
    self.m_loadingBar:setVisible(true)
    self.m_loadingBar:stopAllActions()
    self.m_loadingBar:runAction(cc.ProgressTo:create(0.7, 60))
    self.m_spThumb:stopAllActions()
    self.m_spThumb:runAction(cc.MoveTo:create(0.7, cc.p(28+self.m_LoadingBarSize.width * 0.6 , 21)))
    if self.m_numSched then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_numSched)
        self.m_numSched =nil
    end

    self.m_timeNum = 0
        local function timeUpdate()
        if self.m_loadingNum then
            self.m_timeNum = self.m_timeNum + 0.71
            if self.m_timeNum >= 50 then
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_numSched)
                self.m_loadingNum:setString("50/")
                self.m_numSched = nil
            else
                self.m_loadingNum:setString(math.floor(self.m_timeNum).."/")
            end
        end
    end
    self.m_numSched = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 0.01, false)

    self.m_txtLoadingTips:stopAllActions()
    local call = cc.CallFunc:create(function()
        local randomidx = math.random(1, #tabTips)
        self.m_txtLoadingTips:setString(tabTips[randomidx])
    end)
     local seq = cc.Sequence:create( cc.DelayTime:create(0.1), call, cc.DelayTime:create(4.0) )
     self.m_txtLoadingTips:runAction(cc.RepeatForever:create(seq))

    --self:popNoticeLayer()
end

-- 登陆结束
function LogonLayer:onLogonFinish( callback )
    self.m_loadingBar:stopAllActions()
    self.m_loadingBar:runAction(cc.Sequence:create(cc.ProgressTo:create(0.7, 100), callback))
    self.m_spThumb:stopAllActions()
    self.m_spThumb:runAction(cc.MoveTo:create(0.7, cc.p(28+self.m_LoadingBarSize.width -20, self.m_spThumb:getPositionY())))

    if self.m_numSched then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_numSched)
        self.m_numSched =nil
    end

    self.m_timeNum = 50
        local function timeUpdate()
        if self.m_loadingNum then
            self.m_timeNum = self.m_timeNum + 1.1
            if self.m_timeNum >= 100 then
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.m_numSched)
                self.m_numSched = nil
                self.m_loadingNum:setString("100/")
            else
                self.m_loadingNum:setString(math.floor(self.m_timeNum).."/")
            end
        end
    end
    self.m_numSched = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 0.01, false)

end

-- 登陆失败
function LogonLayer:onLogonFail()
    -- 隐藏进度
    self.m_loadingBg:setVisible(false)
    self.m_loadingBar:stopAllActions()
    self.m_loadingBar:setPercentage(0)
     self.m_spThumb:setVisible(false)
     self.m_spThumb:stopAllActions()
     self.m_spThumb:setPositionX(10)

    -- 显示按钮
    self.m_btnWeChat:setVisible(true)

    -- 停止提示
    self.m_txtLoadingTips:stopAllActions()
    self.m_txtLoadingTips:setString("")

    -- 移除弹窗
    local pop = self._scene:getChildByName(POP_LAYER_NAME)
    if nil ~= pop then
        if pop.animationRemove then
            pop:animationRemove()
        else
            pop:removeFromParent()
        end
    end
end

function LogonLayer:onGetNotice( tabNotice, bFirstLogin )
    self.m_tabSystemNotice = tabNotice
    self:onChangeNotify(self.m_tabSystemNotice[self._sysIndex])
    --[[if bFirstLogin then
        self:stopAllActions()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(function()
                self:popNoticeLayer()
            end),
            cc.DelayTime:create(0.5),
            cc.CallFunc:create(function()
                self:autoLogon()
            end)))
    end]]
    self:autoLogon(bFirstLogin)
end

function LogonLayer:onChangeNotify( msg )
    self._notifyText:stopAllActions()
    if not msg or not msg.NoticeTitle or #msg.NoticeTitle == 0 then
        self._notifyText:setString("")
        self.m_bNotifyRunning = false
        self._tipIndex = 1
        return
    end
    self.m_bNotifyRunning = true
    local msgcolor = msg.color or cc.c4b(255,191,123,255)
    self._notifyText:setVisible(false)
    self._notifyText:setString(msg.NoticeTitle)
    self._notifyText:setTextColor(msgcolor)
    
    local tmpWidth = self._notifyText:getContentSize().width
    self._notifyText:runAction(
            cc.Sequence:create(
                cc.CallFunc:create( function()
                    self._notifyText:move(self.m_noticeArea.width, 0)
                    self._notifyText:setVisible(true)
                end),
                cc.MoveTo:create(16 + (tmpWidth / self.m_noticeArea.width),cc.p(0 - tmpWidth,0)),
                cc.CallFunc:create( function()
                    local tipsSize = 0
                    local tips = {}
                    local index = 1
                    -- 系统公告
                    local tmp = self._sysIndex + 1
                    if tmp > #self.m_tabSystemNotice then
                        tmp = 1
                    end
                    self._sysIndex = tmp
                    self:onChangeNotify(self.m_tabSystemNotice[self._sysIndex])             
                end)
            )
    )
end

function LogonLayer:popNoticeLayer()
    if #self.m_tabSystemNotice > 0 then
        local no = NoticeLayer:create(self._scene, self.m_tabSystemNotice)
        no:setName(POP_LAYER_NAME)
        self._scene:addChild(no)
    end
end

--------测试用
function LogonLayer:initCheckButton(csbNode)

        local cbtlistener = function (sender,eventType)
            self:onSelectedEvent(sender:getTag(),sender)
        end
    self.m_CheckBox = {}
    self.index = CCUserDefault:getInstance():getIntegerForKey("LogonCount", 1)

    for i = 1, 4 do

            local checkbx = ccui.CheckBox:create()
            checkbx:move(1300,330 - i*70)
            checkbx:loadTextures("logon/anniu2.png",
                                  "logon/anniu1.png",
                                  "logon/anniu1.png",
                                  "logon/anniu2.png",
                                  "logon/anniu2.png")
            checkbx:addTo(self.m_rootLayer)
        if nil ~= checkbx then
            --checkbx:setVisible(false)
            checkbx:setTag(i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_CheckBox[i] = checkbx
        end
  
     end
     self.m_CheckBox[self.index]:setSelected(true)
end


function LogonLayer:onSelectedEvent(tag,sender)

        self.m_CheckBox[self.index]:setSelected(false)
        self.index = tag
        self.m_CheckBox[tag]:setSelected(true)

        CCUserDefault:getInstance():setIntegerForKey("LogonCount",tag)
        CCUserDefault:getInstance():flush()

end


return LogonLayer