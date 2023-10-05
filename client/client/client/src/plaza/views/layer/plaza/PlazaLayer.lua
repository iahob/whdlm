--
-- Author: zhong
-- Date: 2017-05-24 10:39:00
--
-- 大厅层
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PlazaLayer = class("PlazaLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local UIPageView = appdf.req(appdf.EXTERNAL_SRC .. "UIPageView")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- ui
local UserCenterLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.UserCenterLayer")           -- 用户中心
local PlazaNoticeLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaNoticeLayer")                 -- 大厅消息列表
local SettingLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "SettingLayer")                         -- 设置
local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                               -- 商城
local VideoMarkListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "video.VideoMarkListLayer")       -- 战绩
local SpreadLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "SpreadLayer")                           -- 推广
local HelpLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "HelpLayer")                               -- 帮助
local PrivateIdInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateIdInputLayer")   -- 约战ID
local RankListLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.plaza.RankListLayer")   -- 排行榜
local RewardLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "RewardLayer")                           -- 奖励
local PrivateRecordLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateRecordLayer")     -- 记录列表
local GameListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "GameListLayer")                       -- 游戏列表
local ServiceLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.logon.ServiceLayer")     -- 客服界面
local RoomListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "RoomLayer")                       -- 房间列表
local PlazaAdLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaAdLayer")                         -- 大厅广告
local PlazaIntroductionLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaIntroductionLayer")     -- 游戏玩法
local ClubLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMain")     -- 俱乐部
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")
local AgentLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "AgentLayer")     -- 代理界面

local SpineObj = appdf.req("client.src.external.SpineObj")

local DailyBonusLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "DailyBonusLayer")                    -- 每日签到
appdf.req(appdf.EXTERNAL_SRC .. "functions")

local uiActivity = appdf.req(appdf.PLAZA_VIEW_SRC .. "uiActivity")

local TAG_START             = 100
local enumTable =
{
    "BT_EXIT",              -- 退出
    "BT_MARK",              -- 战绩
    "BT_SPREAD",            -- 推广
    "BT_SET",               -- 设置
    "BT_SHOP",              -- 商店
    "BT_USERINFO",          -- 个人信息
    "IMAGE_UID",            -- 个人ID
    "BT_NOTICE",            -- 系统消息
    "BT_CREATEROOM",        -- 创建房间
    "BT_JOINROOM",          -- 加入房间
    "BT_BACKGAME",          -- 返回游戏
    "BT_BACKGAME_SELIST",   -- 返回游戏(选择列表)
    "BT_COPY",              -- 复制
    "BT_EXCHANGE",          -- 金币兑换
    "BT_SCORELIST",         -- 金币场
    "BT_SERVICE",           -- 客服
    "BT_AGENT",           -- 客服
    "BT_ADWARD",            -- 广告/活动
    "BT_INTRODUTION",       -- 玩法/介绍
    "BT_CLUB",              -- 俱乐部
    "BT_MATCH",              --比赛
    "BT_DAILY",             -- 客服
    "BT_REWARD",            --奖券
    "BT_MOREBTN",           --更多按钮
    "BT_RANK",              --更多按钮
    "BT_BG",                --背景
    "BT_GAMELIST",          --游戏列表

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local enumZorder =
{
    "ADSHOW_ZORDER",        -- 广告区
    "RANKLIST_ZORDER",      -- 排行榜
    "IDENTIFY_ZORDER",      -- 认证提示
    "SPREADTIP_ZORDER",     -- 推广提示
}
local TAG_ZORDER = ExternalFun.declarEnumWithTable(1, enumZorder)

local PLAZA_NOTICE_LAYER = "__plaza_notice_layer__"

function PlazaLayer:ctor( scene, param)
    local this = self
    PlazaLayer.super.ctor( self, scene, param)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/PlazaLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    local touchFunC = function(ref, tType)
        local tag = ref:getTag()
        --[[if tType == ccui.TouchEventType.began then
            if TAG_ENUM.BT_CREATEROOM == tag then
                self.m_nodeCreateBtn:setVisible(false)
                self.m_nodeCreateBtn:stopAllActions()
            elseif TAG_ENUM.BT_JOINROOM == tag then
                self.m_nodeJoinBtn:setVisible(false)
                self.m_nodeJoinBtn:stopAllActions()
            end
        elseif tType == ccui.TouchEventType.canceled then
            if TAG_ENUM.BT_CREATEROOM == tag then
                self.m_nodeCreateBtn:setVisible(true)
                self.m_nodeCreateBtn:stopAllActions()
                self.m_actCreateBtnAni:gotoFrameAndPlay(0,true)
                self.m_nodeCreateBtn:runAction(self.m_actCreateBtnAni)
            elseif TAG_ENUM.BT_JOINROOM == tag then
                self.m_nodeJoinBtn:setVisible(true)
                self.m_nodeJoinBtn:stopAllActions()
                self.m_actJoinBtnAni:gotoFrameAndPlay(0,true)
                self.m_nodeJoinBtn:runAction(self.m_actJoinBtnAni)
            end
        elseif tType == ccui.TouchEventType.moved then
            if TAG_ENUM.BT_CREATEROOM == tag then
                if ref:isHighlighted() then
                    self.m_nodeCreateBtn:setVisible(false)
                    self.m_nodeCreateBtn:stopAllActions()
                else
                    self.m_nodeCreateBtn:setVisible(true)
                    self.m_nodeCreateBtn:stopAllActions()
                    self.m_actCreateBtnAni:gotoFrameAndPlay(0,true)
                    self.m_nodeCreateBtn:runAction(self.m_actCreateBtnAni)
                end
            elseif TAG_ENUM.BT_JOINROOM == tag then
                if ref:isHighlighted() then
                    self.m_nodeJoinBtn:setVisible(false)
                    self.m_nodeJoinBtn:stopAllActions()
                else
                    self.m_nodeJoinBtn:setVisible(true)
                    self.m_nodeJoinBtn:stopAllActions()
                    self.m_actJoinBtnAni:gotoFrameAndPlay(0,true)
                    self.m_nodeJoinBtn:runAction(self.m_actJoinBtnAni)
                end
            end
        else]]if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(tag, ref)
        end
    end
    
    local mask = csbNode:getChildByName("mask")
    mask:setTag(TAG_ENUM.BT_BG)
    mask:addTouchEventListener( touchFunC )


    local bgImg = csbNode:getChildByName("bg_8")
    local obj = SpineObj:create("plaza/animate/plaza/skeleton.json", "plaza/animate/plaza/skeleton.atlas", true)
            obj:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
            bgImg:addChild(obj)

    self.nodeUp = csbNode:getChildByName("node_upInfo")
    self.nodeDown = csbNode:getChildByName("node_downInfo")

    -- 昵称
    local tmpnick = self.nodeUp:getChildByName("txt_nickname")
    local clipnick = ClipText:createClipText(tmpnick:getContentSize(), GlobalUserItem.tabAccountInfo.szNickName, nil, 28)
    clipnick:setAnchorPoint(tmpnick:getAnchorPoint())
    clipnick:setTextColor(cc.c3b(250, 225, 7))
    clipnick:setPosition(tmpnick:getPosition())
    self.nodeUp:addChild(clipnick)
    self.m_clipNickName = clipnick
    tmpnick:removeFromParent()

    -- gameid
    self.nodeUp:getChildByName("txt_gameid"):setString("ID:"..GlobalUserItem.tabAccountInfo.dwGameID)

    -- 钻石
    local diamondbg = self.nodeUp:getChildByName("plaza_image_diamondbg")
    local tmptxt = diamondbg:getChildByName("txt_diamond")
    local lScore = GlobalUserItem.tabAccountInfo.lDiamond
    local szscore = 0
    local pathStr = nil
    if lScore < 10000 then
        szscore = lScore
    elseif lScore >= 10000 and lScore < 100000000 then
        if lScore / 10000 == 10000 then
            szscore = szscore .."1.00"
            pathStr = "plaza/score_yi.png"
        else
            szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
            pathStr = "plaza/score_wan.png"
        end
    else
        szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
        pathStr = "plaza/score_yi.png"
    end

    tmptxt:setString(szscore.."")

    tmptxt:removeChildByName("_type_")
     if pathStr ~= nil then
        local scoreSize = tmptxt:getContentSize()
        local spWan = cc.Sprite:create(pathStr)
        spWan:setName("_type_")
        spWan:setAnchorPoint(cc.p(0,0.5))
        spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
        spWan:addTo(tmptxt)
    end

    self.m_clipDiamond = tmptxt


    -- 金币
    local scorebg = self.nodeUp:getChildByName("plaza_image_exchange")
    local tmptxt = scorebg:getChildByName("txt_score")
    local lScore = GlobalUserItem.tabAccountInfo.lUserScore

    local szscore = 0
    local pathStr = nil
    if lScore < 10000 then
        szscore = lScore
    elseif lScore >= 10000 and lScore < 100000000 then
        if lScore / 10000 == 10000 then
            szscore = szscore .."1.00"
            pathStr = "plaza/score_yi.png"
        else
            szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
            pathStr = "plaza/score_wan.png"
        end
    else
        szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
        pathStr = "plaza/score_yi.png"
    end

    tmptxt:setString(szscore.."")

    tmptxt:removeChildByName("_type_")
     if pathStr ~= nil then
        local scoreSize = tmptxt:getContentSize()
        local spWan = cc.Sprite:create(pathStr)
        spWan:setName("_type_")
        spWan:setAnchorPoint(cc.p(0,0.5))
        spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
        spWan:addTo(tmptxt)
    end

    self.m_clipScore = tmptxt


     -- 奖券
    local ticketbg = self.nodeUp:getChildByName("plaza_image_reward")
    local tmptxt = ticketbg:getChildByName("txt_diamond")
    local lScore = GlobalUserItem.tabAccountInfo.nAwardsTickets

    local szscore = 0
    local pathStr = nil
    if lScore < 10000 then
        szscore = lScore
    elseif lScore >= 10000 and lScore < 100000000 then
        if lScore / 10000 == 10000 then
            szscore = szscore .."1.00"
            pathStr = "plaza/score_yi.png"
        else
            szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
            pathStr = "plaza/score_wan.png"
        end
    else
        szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
        pathStr = "plaza/score_yi.png"
    end
    tmptxt:setString(szscore.."")

    tmptxt:removeChildByName("_type_")
     if pathStr ~= nil then
        local scoreSize = tmptxt:getContentSize()
        local spWan = cc.Sprite:create(pathStr)
        spWan:setName("_type_")
        spWan:setAnchorPoint(cc.p(0,0.5))
        spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
        spWan:addTo(tmptxt)
    end
    self.m_clipTicket = tmptxt

    -- 头像
    self.m_spHeadBg = self.nodeUp:getChildByName("plaza_sp_headbg")

    -- 退出
    local btn = self.nodeUp:getChildByName("btn_back")
    btn:setTag(TAG_ENUM.BT_EXIT)
    btn:addTouchEventListener(touchFunC)

    -- 个人信息
    local btn = self.nodeUp:getChildByName("btn_userinfo")
    btn:setTag(TAG_ENUM.BT_USERINFO)
    btn:addTouchEventListener(touchFunC)

    -- id复制
    btn = csbNode:getChildByName("plaza_image_idbg")
    btn:setTag(TAG_ENUM.IMAGE_UID)
    btn:addTouchEventListener(touchFunC)

    btn = csbNode:getChildByName("btn_copy")
    btn:setTag(TAG_ENUM.BT_COPY)
    btn:addTouchEventListener(touchFunC)

    -- 充值背景
    btn = self.nodeUp:getChildByName("plaza_image_diamondbg")
    btn:setTag(TAG_ENUM.BT_SHOP)
    btn:addTouchEventListener(touchFunC)

    -- 兑换背景
    btn = self.nodeUp:getChildByName("plaza_image_exchange")
    btn:setTag(TAG_ENUM.BT_EXCHANGE)
    btn:addTouchEventListener(touchFunC)

     -- 奖券
    btn = self.nodeUp:getChildByName("plaza_image_reward")
    btn:setTag(TAG_ENUM.BT_REWARD)
    btn:addTouchEventListener(touchFunC)

    -- 战绩按钮
    btn = self.nodeDown:getChildByName("btn_mark")
    btn:setTag(TAG_ENUM.BT_MARK)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    self.m_btnMark = btn



    -- 分享
    btn = self.nodeUp:getChildByName("btn_spread")
    btn:setTag(TAG_ENUM.BT_SPREAD)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    self.m_btnSpread = btn

    -- 广告/活动
    btn = self.nodeDown:getChildByName("btn_ad")
    btn:setTag(TAG_ENUM.BT_ADWARD)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    -- 排行榜
    btn = self.nodeDown:getChildByName("btn_rank")
    btn:setTag(TAG_ENUM.BT_RANK)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    if GlobalUserItem.tabSystemConfig.RankingListType == 0  or GlobalUserItem.tabSystemConfig.RankingListType >3 then
            btn:setVisible(false)
    end


     -- 更多按钮
    btn = self.nodeDown:getChildByName("btn_more")
    btn:setTag(TAG_ENUM.BT_MOREBTN)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    --更多按钮背景
    self.moreBTNBg = csbNode:getChildByName("img_morebtn")
    self.moreBTNBg:setVisible(false)
    self.moreBTNBg:setLocalZOrder(100)

    local mask = self.moreBTNBg:getChildByName("mask")
    mask:setTag(TAG_ENUM.BT_BG)
    mask:addTouchEventListener( touchFunC )

    -- 设置按钮
    btn = self.moreBTNBg:getChildByName("btn_set")
    btn:setTag(TAG_ENUM.BT_SET)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    -- 玩法
    btn = self.moreBTNBg:getChildByName("btn_wanfa")
    btn:setTag(TAG_ENUM.BT_INTRODUTION)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    -- 商城
    btn = self.nodeDown:getChildByName("btn_shop")
    btn:setTag(TAG_ENUM.BT_SHOP)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(false)
    self.m_btnShop = btn
    btn:setVisible(GlobalUserItem.tabSystemConfig.IsOpenMall == yl.ENABLE_VALUE)


    local obj = SpineObj:create("plaza/animate/shop/skeleton.json", "plaza/animate/shop/skeleton.atlas", true)
            obj:setPosition(btn:getPositionX() -25,btn:getPositionY() - 3)
            self.nodeDown:addChild(obj)




        --比赛
    btn = self.nodeDown:getChildByName("btn_match")
    btn:setTag(TAG_ENUM.BT_MATCH)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    --btn:setPositionX(1515)
    self.btnMatch = btn

    local objM = SpineObj:create("plaza/animate/match/skeleton.json", "plaza/animate/match/skeleton.atlas", true)
            objM:setPosition(btn:getPositionX(),btn:getPositionY() - 28)
            self.nodeDown:addChild(objM)

    --俱乐部
    local btn = self.nodeDown:getChildByName("btn_private")
    btn:setTag(TAG_ENUM.BT_CLUB)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    --btn:setPositionX(1515)
    self.btnClub = btn

    -- local objC = SpineObj:create("plaza/animate/club/skeleton.json", "plaza/animate/club/skeleton.atlas", true)
    --         objC:setPosition(btn:getPositionX(),btn:getPositionY() + 5)
    --         self.nodeDown:addChild(objC)

    for k,v in pairs(self._scene:getApp()._gameList) do
        if v then
            if tonumber(v._KindID) ==  200 then
                break
            elseif k == #self._scene:getApp()._gameList then
                self.btnMatch:setVisible(false)
                objM:removeFromParent()
                --self.btnClub:setPositionX(self.btnMatch:getPositionX())
               -- objC:setPositionX(self.btnMatch:getPositionX())

            end
        end
    end

    -- 客服
    btn = self.nodeUp:getChildByName("btn_server")
    btn:setTag(TAG_ENUM.BT_SERVICE)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)


        -- 推广
    btn = self.nodeUp:getChildByName("btn_tuiguang")
    btn:setTag(TAG_ENUM.BT_AGENT)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    btn = self.moreBTNBg:getChildByName("btn_daily")
    btn:setTag(TAG_ENUM.BT_DAILY)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)
    
    if GlobalUserItem.tabSystemConfig.IsOpenGameSignIn ~= yl.ENABLE_VALUE then
        btn:setVisible(false)
        self.moreBTNBg:setContentSize(cc.size(190,188))
    end


    btn = csbNode:getChildByName("btn_introduction")
    btn:setTag(TAG_ENUM.BT_INTRODUTION)
    btn:addTouchEventListener(touchFunC)
    btn:setPressedActionEnabled(true)

    

    local spWeChat = self.nodeUp:getChildByName("plaza_sp_wechaticon_2")
    spWeChat:setLocalZOrder(100)

        -- 系统消息
    btn = csbNode:getChildByName("btn_notice")
    btn:setTag(TAG_ENUM.BT_NOTICE)
    btn:addTouchEventListener(touchFunC)
    local noticeSize = btn:getContentSize()
    self.m_noticeArea = noticeSize


    -- 消息区域
    local stencil = display.newSprite()
        :setAnchorPoint(cc.p(0,0.5))
    stencil:setTextureRect(cc.rect(0, 0, noticeSize.width - 180, noticeSize.height))
    self._notifyClip = cc.ClippingNode:create(stencil)
        :setAnchorPoint(cc.p(0,0.5))
    self._notifyClip:setInverted(false)
    self._notifyClip:move(130,noticeSize.height * 0.5 + 3)
    self._notifyClip:addTo(btn)
    self._notifyText = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
                                :addTo(self._notifyClip)
                                --:setTextColor(cc.c4b(255,191,123,255))
                                :setAnchorPoint(cc.p(0,0.5))
                                :enableOutline(cc.c4b(79,48,35,255), 1)
    -- 消息内容
    self.m_tabSystemNotice = {}
    -- 播放索引
    self._sysIndex = 1
    -- 消息是否运行
    self.m_bNotifyRunning = false
    -- 跑马灯内容
    self.m_tabMarquee = {}
    -- 消息id
    self.m_nNotifyId = 0

    -- -- 加入房间
    -- btn = csbNode:getChildByName("btn_checkRoom")
    -- btn:addTouchEventListener(touchFunC)
    -- btn:setTag(TAG_ENUM.BT_JOINROOM)
    -- btn:setPressedActionEnabled(true)
    -- self.m_btnJoinGame = btn
    self.dwBackRoomID = 0
    -- 返回列表
    self.m_tabBackRoomList = {}


    -- 实名认证
    self.m_imageIdentifyBubble = csbNode:getChildByName("plaza_sp_identify")--csbNode:getChildByName("image_identifybubble")
    self.m_imageIdentifyBubble:setLocalZOrder(TAG_ZORDER.IDENTIFY_ZORDER)
    self.m_imageIdentifyBubble:setVisible(false)

    -- 免费提示
    self.m_imageSpreadBubble = self.nodeDown:getChildByName("plaza_sp_reddot")--csbNode:getChildByName("m_spIdentify")
    self.m_imageSpreadBubble:setLocalZOrder(TAG_ZORDER.SPREADTIP_ZORDER)
    self.m_imageSpreadBubble:setVisible(GlobalUserItem.bSpreadTipShow)

    -- 事件监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)




    --游戏列表
    self.m_spGameListPageView = csbNode:getChildByName("content")
    self.m_spGameListPageView:setTag(TAG_ENUM.BT_GAMELIST)
    self.m_spGameListPageView:addTouchEventListener( touchFunC )

    local param = {}
    param.list = self._scene:getApp()._gameList
    param.action = GameListLayer.ACTION_ENUM.ACTION_SCORELIST
    local gl = GameListLayer:create(self._scene, param, GameListLayer.SECOND_LEVEL +10)
    self.m_spGameListPageView:addChild(gl)
    self.m_gameListLayer = gl

    self.nodeUp:setPositionY(180)
    self.nodeDown:setPositionY(-180)

    self:refreshPlazaBG()
end

function PlazaLayer:onEnterTransitionFinish()
    PlazaLayer.super.onEnterTransitionFinish(self)
    -- 创建头像
    local head = HeadSprite:createClipHead(GlobalUserItem.tabAccountInfo, 82, "plaza/plaza_sp_cliphead.png")
    if nil ~= head then
        head:setPosition(43, 43)
        self.m_spHeadBg:addChild(head)
        --self.m_spHeadBg:setLocalZOrder(1)
        self._head = head
    end

    self.nodeUp:runAction(cc.MoveTo:create(0.4, cc.p(0, 0)))
    self.nodeDown:runAction(cc.MoveTo:create(0.4, cc.p(0, 0)))

   
    -- self._scene:queryUserScoreInfo(function()
    --         -- 弹窗奖励
    --         if self._scene.popReward then
    --             self._scene:popReward()
    --         end
    --     end)
    -- 系统消息 20170804游戏中已经播放的喇叭消息不需要再次播放
    --self:onGetNotice(self._scene.m_vecNotice)
end

function PlazaLayer:onExit()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end
    ExternalFun.SAFE_RELEASE(self.m_actCreateBtnAni)
    ExternalFun.SAFE_RELEASE(self.m_actJoinBtnAni)
    PlazaLayer.super.onExit(self)
end

function PlazaLayer:onButtonClickedEvent( tag, ref )
    if TAG_ENUM.BT_MOREBTN ~= tag then    
        self.moreBTNBg:setVisible(false)
    end
    if TAG_ENUM.BT_EXIT == tag then                                 -- 退出
        self._scene:exitPlaza()
    elseif TAG_ENUM.BT_MARK == tag then                             -- 战绩
        --showToast(self, "暂未开放！", 2)
        self:popMark()
    elseif TAG_ENUM.BT_SPREAD == tag then                           -- 推广
        self:popSpread()

    elseif TAG_ENUM.BT_SET == tag then                              -- 设置
        self:popSet()
    elseif TAG_ENUM.BT_RANK == tag then


        -- 排行榜
        self._rankListLayer = RankListLayer:create(self._scene)
            :addTo(self)
        self._rankListLayer:setRankShowListener(function(bShow, showFunc)
            self:stopAllActions()
            showFunc()
           
        end)
         self._rankListLayer:getNormalList()

    elseif TAG_ENUM.BT_SHOP == tag then                             -- 商店
        self:popShopLayer(ShopLayer.DIAMOND_CHARGE)
        --self._scene:popReward()
    elseif TAG_ENUM.BT_USERINFO == tag then                         -- 用户信息
        local ul = UserCenterLayer:create(self._scene, {}, UserCenterLayer.SECOND_LEVEL)
        self._scene:addPopLayer(ul)
    elseif TAG_ENUM.IMAGE_UID == tag or TAG_ENUM.BT_COPY == tag then                           -- 复制ID
        MultiPlatform:getInstance():copyToClipboard(GlobalUserItem.tabAccountInfo.dwGameID .. "")
    elseif TAG_ENUM.BT_NOTICE == tag then                           -- 公告
        self:popNotice()
    elseif TAG_ENUM.BT_JOINROOM == tag then                         -- 加入房间
        local pa = PrivateIdInputLayer:create(self._scene, {}, PrivateIdInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BT_CREATEROOM == tag then                       -- 创建房间
        self:popCreate()
    elseif TAG_ENUM.BT_BACKGAME == tag then                         -- 返回游戏
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onSearchRoom(self.dwBackRoomID)
    elseif TAG_ENUM.BT_BACKGAME_SELIST == tag then                  -- 返回游戏
        local pr = PrivateRecordLayer:create(
            self._scene,
            { from = PrivateRecordLayer.FROM_ENUM.FROM_BACKGAME, list = self.m_tabBackRoomList },
            PrivateRecordLayer.SECOND_LEVEL
        )
        self._scene:addPopLayer(pr)
    elseif TAG_ENUM.BT_SCORELIST == tag then                        -- 金币场
        -- 判断游戏数
        local count = #self._scene:getApp()._gameList
        if count > 1 then
            local param = {}
            param.list = self._scene:getApp()._gameList
            param.action = GameListLayer.ACTION_ENUM.ACTION_SCORELIST
            local gl = GameListLayer:create(self._scene, param, GameListLayer.SECOND_LEVEL +10)
            self._scene:addPopLayer(gl)
        else
            self:enterRoomList()
        end
    elseif TAG_ENUM.BT_SERVICE == tag then                          -- 客服
        self:popService()
    elseif TAG_ENUM.BT_AGENT == tag then
        local sl = AgentLayer:create(self._scene, {}, AgentLayer.SECOND_LEVEL)
        self._scene:addPopLayer(sl)
    elseif TAG_ENUM.BT_DAILY == tag then                          -- 客服
        local pa = DailyBonusLayer:create(self._scene, {}, DailyBonusLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)
    elseif TAG_ENUM.BT_EXCHANGE == tag then
        self:popShopLayer(ShopLayer.GOLD_EXCHANGE)
    elseif TAG_ENUM.BT_REWARD == tag then
        self:popShopLayer(ShopLayer.EXCHANGE)
    elseif TAG_ENUM.BT_ADWARD == tag then
        -- self:popAdward()
        self:popActivity()
    elseif TAG_ENUM.BT_INTRODUTION == tag then
        print("玩法介绍")
        self:popIntroduction()
    elseif TAG_ENUM.BT_CLUB == tag then
        -- local pa = ClubLayer:create(self._scene, {}, ClubLayer.SECOND_LEVEL)
        -- self._scene:addPopLayer(pa)
        self.btnClub:setEnabled(false)
        self.btnClub:runAction(cc.Sequence:create(cc.DelayTime:create(2.5), cc.CallFunc:create(function ()
            --self.btnClub:setEnabled(true)
        end)))

        self._scene:onChangeShowMode(yl.SCENE_CLUB)

    elseif TAG_ENUM.BT_MATCH == tag then
   
         if MatchRoom then
             self._scene:onChangeShowMode(yl.SCENE_MATCH)
         end
    elseif TAG_ENUM.BT_RANK == tag then
         if MatchRoom then
             self._scene:onChangeShowMode(yl.SCENE_MATCH)
         end
    elseif TAG_ENUM.BT_MOREBTN == tag then
        if self.moreBTNBg:isVisible() then
            self.moreBTNBg:setVisible(false)
        else
            self.moreBTNBg:setVisible(true)
        end
    elseif TAG_ENUM.BT_BG == tag or TAG_ENUM.BT_GAMELIST == tag then
        self.moreBTNBg:setVisible(false)
    end
end

function PlazaLayer:popActivity()
    local obj = uiActivity:ctor(self._scene, self)
    if obj then
        obj:setdelayTime(0)
        obj:refresh(GlobalUserItem.getAwardData())
    else
        print("uiActivity:ctor return nil parent had added?")
    end
end

-- 弹出消息
function PlazaLayer:popNotice()
    local pn = PlazaNoticeLayer:create(self._scene, self.m_tabSystemNotice, PlazaNoticeLayer.SECOND_LEVEL)
    pn:setName(PLAZA_NOTICE_LAYER)
    self._scene:addPopLayer(pn)
end

-- 弹出广告
function PlazaLayer:popAdward()
    local pl = PlazaAdLayer:create(self._scene, {}, PlazaAdLayer.SECOND_LEVEL)
    self._scene:addPopLayer(pl)
    GlobalUserItem.setTodayNoAdNotice( true )
end

-- 弹出战绩
function PlazaLayer:popMark()
    local ml = VideoMarkListLayer:create(self._scene, {}, VideoMarkListLayer.SECOND_LEVEL)
    self._scene:addPopLayer(ml)
end

-- 弹出设置
function PlazaLayer:popSet()
    local st = SettingLayer:create(self._scene, self, SettingLayer.SECOND_LEVEL)
    self._scene:addPopLayer(st)
end

-- 弹出客服
function PlazaLayer:popService()
    local ser = ServiceLayer:create(self._scene, {}, ServiceLayer.SECOND_LEVEL)
    self._scene:addPopLayer(ser)
end

-- 弹出商店
function PlazaLayer:popShopLayer( shopType )

    -- 是否开启
    if GlobalUserItem.tabSystemConfig.IsOpenMall == yl.ENABLE_VALUE then
        local sl = ShopLayer:create(self._scene, shopType, ShopLayer.SECOND_LEVEL)
        self._scene:addPopLayer(sl)
    else
        local hl = HelpLayer:create(self._scene, shopType, HelpLayer.SECOND_LEVEL)
        self._scene:addPopLayer(hl)
    end
end

-- 弹出推广
function PlazaLayer:popSpread()
    local sl = SpreadLayer.SpreadSimpleLayer:create(self._scene, {}, SpreadLayer.SECOND_LEVEL)
    self._scene:addPopLayer(sl)
    GlobalUserItem.bSpreadTipShow = false
    self.m_imageSpreadBubble:setVisible(GlobalUserItem.bSpreadTipShow)
end

-- 弹出创建
function PlazaLayer:popCreate()
    -- 判断游戏数
    local count = #self._scene:getApp()._gameList
    if count > 1 then
        local param = {}
        param.list = self._scene:getApp()._gameList
        param.action = GameListLayer.ACTION_ENUM.ACTION_PRIVATE
        local gl = GameListLayer:create(self._scene, param, GameListLayer.SECOND_LEVEL +10)
        self._scene:addPopLayer(gl)
    else
        if not PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.nCurGameKind) then
            showToast(self, "约战房未开启!", 1)
            return
        end
        local cl = PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_CREATEPRIROOME, self._scene, {})
        if nil ~= cl then
            self._scene:addPopLayer(cl)
            cl:setName(PriRoom.NAME_CREATE_LAYER)
        end
        PriRoom:getInstance().m_CurrentClubID = 0
    end
end

-- 弹出玩法
function PlazaLayer:popIntroduction()
    local ser = PlazaIntroductionLayer:create(self._scene, {}, PlazaNoticeLayer.SECOND_LEVEL)
    self._scene:addPopLayer(ser)
end

-- 进入房间列表
function PlazaLayer:enterRoomList()
    -- local sl = RoomListLayer:create(self._scene, {}, RoomListLayer.SECOND_LEVEL)
    -- self._scene:addPopLayer(sl)
    -- sl:setName(yl.PLAZA_ROOMLIST_LAYER)
    self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
end

-- 翻页监听
function PlazaLayer:onTouchPageListener( event )
    --dump(event, "onTouchPageListener - event:")

    if event.name == "pageChange" then
        -- 翻页后继续滚动
        self:showAdAutoScroll()
    elseif event.name == "clicked" then
        if nil ~= event.item and type(event.item.info) == "table" then
            local info = event.item.info
            local action = info["link"]
            print("PlazaLayer:onTouchPageListener ", action)
            if yl.AD_ACTION_TOSHOP == action then
                self:popShopLayer(ShopLayer.DIAMOND_CHARGE)
            elseif yl.AD_ACTION_TOSPREAD == action then
                self:popSpread()
            elseif yl.AD_ACTION_CREATEROOM == action then
                self:onButtonClickedEvent(TAG_ENUM.BT_CREATEROOM, self.m_btnCreateRoom)
            elseif yl.AD_ACTION_VIDEO == action then
                self:onButtonClickedEvent(TAG_ENUM.BT_MARK, self.m_btnMark)
            elseif yl.AD_ACTION_IDENTIFY == action and GlobalUserItem.bEnableIdentify then
                self._scene:popIdentify()
            end
        end
    end
end

function PlazaLayer:onUserInfoChange( event  )
    print("----------userinfo change notify------------")

    local msgWhat = event.obj

    if nil ~= msgWhat then
        if msgWhat == yl.RY_MSG_USERHEAD then
            --更新头像
            if nil ~= self._head then
                self._head:updateHead(GlobalUserItem)
            end
        elseif msgWhat == yl.RY_MSG_USERWEALTH or msgWhat == yl.RY_CULB_DIAMOND_NOTIFY then
            --更新财富
            local lScore = GlobalUserItem.tabAccountInfo.lUserScore
            local szscore = 0
            local pathStr = nil
            if lScore < 10000 then
                szscore = lScore
            elseif lScore >= 10000 and lScore < 100000000 then
                if lScore / 10000 == 10000 then
                    szscore = szscore .."1.00"
                    pathStr = "plaza/score_yi.png"
                else
                    szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
                    pathStr = "plaza/score_wan.png"
                end
            else
                szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
                pathStr = "plaza/score_yi.png"
            end
            self.m_clipScore:setString(szscore.."")
            self.m_clipScore:removeChildByName("_type_")
            if pathStr ~= nil then
                local scoreSize = self.m_clipScore:getContentSize()
                local spWan = cc.Sprite:create(pathStr)
                spWan:setName("_type_")
                spWan:setAnchorPoint(cc.p(0,0.5))
                spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
                spWan:addTo(self.m_clipScore)
            end

            local pathStr = nil
            local lDiamond = GlobalUserItem.tabAccountInfo.lDiamond
            local szscore = 0
            if lDiamond < 10000 then
                szscore = lDiamond
            elseif lDiamond >= 10000 and lDiamond < 100000000 then
                if lDiamond / 10000 == 10000 then
                    szscore = szscore .."1.00"
                    pathStr = "plaza/score_yi.png"
                else
                    szscore = string.gsub(string.format("%.2f", lDiamond / 10000), "[.]", ":")
                    pathStr = "plaza/score_wan.png"
                end
            else
                szscore = string.gsub(string.format("%.2f", lDiamond / 100000000), "[.]", ":")
                pathStr = "plaza/score_yi.png"
            end
            self.m_clipDiamond:setString(szscore.."")
            self.m_clipDiamond:removeChildByName("_type_")
            if pathStr ~= nil then
                local scoreSize = self.m_clipDiamond:getContentSize()
                local spWan = cc.Sprite:create(pathStr)
                spWan:setName("_type_")
                spWan:setAnchorPoint(cc.p(0,0.5))
                spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
                spWan:addTo(self.m_clipDiamond)
            end

            -- 奖券
            local ticketbg = self.nodeUp:getChildByName("plaza_image_reward")
            local tmptxt = ticketbg:getChildByName("txt_diamond")
            local lScore = GlobalUserItem.tabAccountInfo.nAwardsTickets

            local szscore = 0
            local pathStr = nil
            if lScore < 10000 then
                szscore = lScore
            elseif lScore >= 10000 and lScore < 100000000 then
                if lScore / 10000 == 10000 then
                    szscore = szscore .."1.00"
                    pathStr = "plaza/score_yi.png"
                else
                    szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
                    pathStr = "plaza/score_wan.png"
                end
            else
                szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
                pathStr = "plaza/score_yi.png"
            end
            tmptxt:setString(szscore.."")

            tmptxt:removeChildByName("_type_")
             if pathStr ~= nil then
                local scoreSize = tmptxt:getContentSize()
                local spWan = cc.Sprite:create(pathStr)
                spWan:setName("_type_")
                spWan:setAnchorPoint(cc.p(0,0.5))
                spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2 +2))   
                spWan:addTo(tmptxt)
            end
            self.m_clipTicket = tmptxt

        elseif msgWhat == yl.RY_MSG_USERIDENTIFY then
            local bIdentify = event.value
            self.m_imageIdentifyBubble:setVisible(not GlobalUserItem.isRealAuth())
        end
    end
end

-- 隐藏自己
function PlazaLayer:onHide()
    -- todo
    self:setVisible(false)
    --self.m_PageView:activeTouch(false)
end

-- 显示自己
function PlazaLayer:onShow()
    -- todo
    self:setVisible(true)
    --self.m_PageView:activeTouch(true)
end

-- 获取公告
function PlazaLayer:onGetNotice( tabNotice )
    self.m_tabSystemNotice = clone(tabNotice)
    self.m_tabMarquee = clone(self.m_tabSystemNotice)
    self:onChangeNotify(self.m_tabMarquee[self._sysIndex])
end

-- 添加公告
function PlazaLayer:onAddNotice( msg )
    if type(msg) ~= "table" then
        dump(msg, "PlazaLayer:onAddNotice 参数非法", 6)
        return
    end
    table.insert( self.m_tabSystemNotice, msg )
    table.insert( self.m_tabMarquee, msg )
    if not self.m_bNotifyRunning then
        self:onChangeNotify(self.m_tabMarquee[self._sysIndex])
    end
    local pn = self._scene._scenePopLayer:getChildByName(PLAZA_NOTICE_LAYER)
    if nil ~= pn then
        pn:refreshNoticeList(self.m_tabSystemNotice)
    end

    -- 房间列表
    local rl = self._scene._scenePopLayer:getChildByName(yl.PLAZA_ROOMLIST_LAYER)
    if nil ~= rl then
        rl:onAddNotice( msg )
    end
end

-- 循环公告
function PlazaLayer:onChangeNotify( msg )
    self._notifyText:stopAllActions()
    if not msg or not msg.title or #msg.title == 0 then
        self._notifyText:setString("")
        self.m_bNotifyRunning = false
        self._tipIndex = 1
        return
    end
    local showMessage = msg.title
    if not msg.showtitle then
        showMessage = msg.marqueetitle
    end
    if not showMessage or #showMessage == 0 then
        self._notifyText:setString("")
        self.m_bNotifyRunning = false
        self._tipIndex = 1
        return
    end
    self.m_bNotifyRunning = true
    local msgcolor = msg.color or cc.c4b(255,191,123,255)
    self._notifyText:setVisible(false)
    self._notifyText:setString(showMessage)
    --self._notifyText:setTextColor(msgcolor)

    if true == msg.autoremove then
        msg.showcount = msg.showcount or 0
        msg.showcount = msg.showcount - 1
        if msg.showcount <= 0 then
            self:removeNoticeById(msg.id)
        end
    end

    local tmpWidth = self._notifyText:getContentSize().width
    self._notifyText:runAction(
        cc.Sequence:create(
            cc.CallFunc:create( function()
                self._notifyText:move(self.m_noticeArea.width, 0)
                self._notifyText:setVisible(true)
            end),
            cc.MoveTo:create(16 + (tmpWidth / self.m_noticeArea.width) * 8,cc.p(0 - tmpWidth,0)),
            cc.CallFunc:create( function()
                local tipsSize = 0
                local tips = {}
                local index = 1
                -- 系统公告
                local tmp = self._sysIndex + 1
                if tmp > #self.m_tabMarquee then
                    tmp = 1
                end
                self._sysIndex = tmp
                self:onChangeNotify(self.m_tabMarquee[self._sysIndex])
            end)
        )
    )
end

-- 移除公告
function PlazaLayer:removeNoticeById(id)
    if nil == id then
        return
    end
    local idx = nil
    for k,v in pairs(self.m_tabMarquee) do
        if nil ~= v.id and v.id == id then
            idx = k
            break
        end
    end
    if nil ~= idx then
        table.remove(self.m_tabMarquee, idx)
    end
end

-- 广告滚动
function PlazaLayer:showAdAutoScroll()
    --[[if not self.m_PageView.m_bIsAutoScroll then
        self.m_PageView:autoGoToPage()
    end]]
end

-- 弹出回放ID输入
function PlazaLayer:popVideoCodeEdit( szVideoCode )
    local vd = VideoMarkListLayer.VideoCodeEditLayer:create( self._scene, szVideoCode, VideoMarkListLayer.VideoCodeEditLayer.SECOND_LEVEL)
    self._scene:addPopLayer(vd)
end

-- 弹出房间ID输入
function PlazaLayer:popPrivateIdEdit( szRoomId )
    local vd = PrivateIdInputLayer.PrivateIdEditLayer:create( self._scene, szRoomId, PrivateIdInputLayer.PrivateIdEditLayer.SECOND_LEVEL)
    self._scene:addPopLayer(vd)
end

-- 界面动画
function PlazaLayer:plazaAnimation()
    --[[-- 创建动画(创建转圈-加入转圈-创建转圈 ...)
    self:createRoomAni()
    -- 推广动画
    self:spreadAni()
    -- 商店动画
    self:shopAni()]]
end

-- 创建房间动画
function PlazaLayer:createRoomAni()
    -- 创建按钮
    self.m_nodeCreateBtn:stopAllActions()
    self.m_actCreateBtnAni:gotoFrameAndPlay(0,true)
    self.m_nodeCreateBtn:runAction(self.m_actCreateBtnAni)

    -- 加入按钮
    self.m_nodeJoinBtn:stopAllActions()
    self.m_actJoinBtnAni:gotoFrameAndPlay(0,true)
    self.m_nodeJoinBtn:runAction(self.m_actJoinBtnAni)
end

-- 推广动画
function PlazaLayer:spreadAni()
    local call1 = cc.CallFunc:create(function()
        self.m_btnSpread:loadTextureNormal("plaza/plaza_btn_spread_0.png")
    end)
    local call9 = cc.CallFunc:create(function()
        self:spreadAni()
    end)
    local animRate = 0.11
    local acts = {}
    table.insert(acts, call1)
    for i = 1, 10 do
        local call = cc.CallFunc:create(function()
            local sz = "plaza/plaza_btn_spread_" .. i .. ".png"
            self.m_btnSpread:loadTextureNormal(sz)
        end)
        table.insert(acts, call)
        table.insert(acts, cc.DelayTime:create(animRate))
    end
    table.insert(acts, cc.DelayTime:create(1))
    table.insert(acts, call9)
    self.m_btnSpread:stopAllActions()
    self.m_btnSpread:runAction(cc.Sequence:create(acts))
end

-- 商城动画
function PlazaLayer:shopAni()
    if GlobalUserItem.tabSystemConfig.IsOpenMall ~= yl.ENABLE_VALUE then
        return
    end
    local call1 = cc.CallFunc:create(function()
        self.m_btnShop:loadTextureNormal("plaza/plaza_btn_shop_0.png")
    end)
    local call9 = cc.CallFunc:create(function()
        self:shopAni()
    end)
    local animRate = 0.15
    local acts = {}
    table.insert(acts, call1)
    for i = 1, 8 do
        local call = cc.CallFunc:create(function()
            local sz = "plaza/plaza_btn_shop_" .. i .. ".png"
            self.m_btnShop:loadTextureNormal(sz)
        end)
        table.insert(acts, call)
        table.insert(acts, cc.DelayTime:create(animRate))
    end
    table.insert(acts, call1)
    table.insert(acts, cc.DelayTime:create(0.5))
    table.insert(acts, call9)
    self.m_btnShop:stopAllActions()
    self.m_btnShop:runAction(cc.Sequence:create(acts))
end

-- 刷新大厅
function PlazaLayer:refreshPlaza()
    -- 实名认证气泡
    if GlobalUserItem.bEnableIdentify then
        self.m_imageIdentifyBubble:setVisible(not GlobalUserItem.isRealAuth())
    end
end

-- 约战创建列表
function PlazaLayer:onGetCreateList()
    --[[local bBackGame = false
    local nBtnTag = TAG_ENUM.BT_BACKGAME
    self.m_tabBackRoomList = {}

    -- 创建列表
    local list = PriRoom:getInstance().m_tabCreateRecord
    local ncount = #list

    -- 暂离记录
    local joinedGame = PriRoom:getInstance().m_tabJoinGameRecord[GlobalUserItem.nCurGameKind]
    if nil ~= joinedGame and nil ~= joinedGame.roomid then
        local playingRoomList = {}
        for k,v in pairs(list) do
            if v.cbIsDisssumRoom ~= PriRoom.ROOM_STATE.END_STATE then
                self.dwBackRoomID = v.dwPersonalRoomID
                table.insert(playingRoomList, v)
            end
        end
        local count = #playingRoomList
        if 0 == count then
            -- 非创建数量为0, 且暂离roomid不为空, 自己参与
            self.dwBackRoomID = joinedGame.roomid
            bBackGame = true
        else
            if 1 < count then
                nBtnTag = TAG_ENUM.BT_BACKGAME_SELIST
            end

            self.m_tabBackRoomList = playingRoomList
            bBackGame = (#playingRoomList ~= 0)
        end
    elseif 1 == PriRoom:getInstance().cbIsJoinGame then
        -- 必须参与
        for k,v in pairs(list) do
            if v.cbIsDisssumRoom ~= PriRoom.ROOM_STATE.END_STATE then
                self.dwBackRoomID = v.dwPersonalRoomID
                bBackGame = true
                break
            end
        end
    end
    local sznormal = "plaza/plaza_btn_joinroom_0.png"
    local szpress = "plaza/plaza_btn_joinroom_select.png"

    -- 是否返回游戏
    if bBackGame then
        sznormal = "plaza/plaza_btn_backgame_0.png"
        szpress = "plaza/plaza_btn_backgame_1.png"
        -- 隐藏麻将
        self.m_nodeJoinBtn:setVisible(false)
        self.m_nodeJoinBtn:stopAllActions()
    else
        -- 加入游戏
        nBtnTag = TAG_ENUM.BT_JOINROOM
        self.m_nodeJoinBtn:setVisible(true)
        self.m_nodeJoinBtn:stopAllActions()
        self.m_actJoinBtnAni:gotoFrameAndPlay(0,true)
        self.m_nodeJoinBtn:runAction(self.m_actJoinBtnAni)
    end
    self.m_btnJoinGame:loadTextureDisabled(sznormal)
    self.m_btnJoinGame:loadTextureNormal(sznormal)
    self.m_btnJoinGame:loadTexturePressed(szpress)
    self.m_btnJoinGame:setPressedActionEnabled(true)
    self.m_btnJoinGame:setTag(nBtnTag)]]
end

-- 刷新大厅
function PlazaLayer:refreshPlazaBG()
    -- 实名认证气泡
    self.BgList = 
    {
        "bg1.png",
        "bg2.png",
        "bg3.png",
        "bg4.png",
    }
    local index = cc.UserDefault:getInstance():getIntegerForKey("option_imgBG_Index", 1)

    local imgBg = self.m_csbNode:getChildByName("bg_8")
    --显示背景替换
    imgBg:loadTexture("public/" .. self.BgList[index])
    imgBg:setCapInsets(cc.rect(1, 1, 1, 1))
end


return PlazaLayer