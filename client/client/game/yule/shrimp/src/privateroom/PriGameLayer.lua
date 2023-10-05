--
-- Author: David
-- Date: 2017-8-22 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")

local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.privateroom.DismissLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")

local BetInfoLayer = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.views.layer.BetInfoLayer")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
local BTN_INFO = 106
local BTN_ROUND = 107

local HOMEOWNER = 0

function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    --local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("roomInfoPanel"):getChildByName("roomNum")
    self.m_atlasRoomID:setString("000000")

    self.m_room_sp = csbNode:getChildByName("roomInfoPanel"):getChildByName("roomSp")

    if GlobalUserItem.bPrivateGoldRoom then 
        self.m_room_sp:setPositionX(76.81)
        self.m_atlasRoomID:setPositionX(101.51)
    else
        self.m_room_sp:setPositionX(42.81-10)
        self.m_atlasRoomID:setPositionX(67.51-10)
    end
    -- 局数
    self.m_atlasCount = csbNode:getChildByName("roomInfoPanel"):getChildByName("number")
    self.m_atlasCount:setString("0 / 0")
    self.m_atlasCount:setVisible(false) 

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self.m_btnInvite:setPositionY(310)

    self.m_roomInfo = csbNode:getChildByName("btn_round")
    self.m_roomInfo:setTag(BTN_ROUND)
    self.m_roomInfo:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self.m_rootLayer, 10)
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if BTN_DISMISS == tag then              -- 请求解散游戏
        -- local useritem = self._gameLayer:GetMeUserItem()

        -- if self._gameLayer.gameStatus == 0 and useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
            
        --     PriRoom:getInstance():getNetFrame():sendDissumeGame(self._gameLayer:GetMeTableID())

        --    -- self.m_rootLayer:removeAllChildren()
        --     --GlobalUserItem.bWaitQuit = false
        --    -- self._gameLayer:onExitRoom()
        -- else
        --     PriRoom:getInstance():queryDismissRoom(self._gameLayer.gameStatus, self._gameLayer:onGetSitUserNum())
        -- end
        PriRoom:getInstance():queryDismissRoom(self._gameLayer.gameStatus, self._gameLayer:onGetSitUserNum())
    elseif BTN_INVITE == tag then
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()
                if nil == scene then
                    return
                end
                local dwRoomDwd = cmd_table.dwRoomDwd or 0

                local function _shareFun( dwRoomDwd )
                    if nil ~= scene.popTargetShare then
                        scene:popTargetShare(function(target)
                            if nil ~= target then
                                local function sharecall( isok )
                                    if type(isok) == "string" and isok == "true" then
                                        performWithDelay(self, function ( ... )
                                            showToast(self, "分享成功", 2)
                                        end, 2)
                                    end
                                end
                                local passwd = dwRoomDwd
                                local szRoomID = self.m_atlasRoomID:getString()
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0
                                
                                local strPlayerCount = (self._gameLayer.playerCount == 0 or self._gameLayer.playerCount == nil) and "2-11人," or self._gameLayer.playerCount.."人,"

                                local shareTxt = "局数:" .. drawCount .. "局。"..strPlayerCount.."鱼虾蟹精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. szRoomID .. "&k=6&a=1" .. "&p=" .. passwd
                                print("获取分享信息===》", szRoomID,shareTxt,  url)
                                MultiPlatform:getInstance():shareToTarget(
                                    target, 
                                    sharecall, 
                                    "【约战房间: " .. szRoomID .. " 】", 
                                    shareTxt, 
                                    url, 
                                    "")
                            end
                        end, 3)
                    end
                end
                
                if 0 ~= dwRoomDwd then
                    local query = QueryDialog:create("约战密码为 " .. dwRoomDwd .. " 是否邀请好友游戏?", function(ok)
                        if ok then
                            _shareFun(dwRoomDwd)
                        end
                    end)
                    :setCanTouchOutside(false)
                    :addTo(self)
                else
                    _shareFun( dwRoomDwd )
                end
            end
        )
    elseif BTN_SHARE == tag then
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    performWithDelay(self, function ()
                        showToast(self, "分享成功", 2)
                    end, 2)
                end
                GlobalUserItem.bAutoConnect = true
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            -- 截图分享
            local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
            local area = cc.rect(0, 0, framesize.width, framesize.height)
            local imagename = "grade_share.jpg"
            ExternalFun.popupTouchFilter(0, false)
            captureScreenWithArea(area, imagename, function(ok, savepath)
                ExternalFun.dismissTouchFilter()
                if ok then
                    if nil ~= target then
                        GlobalUserItem.bAutoConnect = false
                        MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的约战房战绩", "分享我的约战房战绩", url, savepath, "true")
                    end            
                end
            end)
        end, 3)
    elseif BTN_QUIT == tag then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
    elseif BTN_ZANLI == tag then
        PriRoom:getInstance():tempLeaveGame()
        self._gameLayer:onExitRoom()
    elseif BTN_INFO == tag then
        self._gameLayer._gameView:showRoomInfo(true)
    elseif BTN_ROUND == tag then 
        self._gameLayer:onSendGameGoldRecord()
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    if PriRoom:getInstance().m_tabPriData.dwDrawCountLimit < 100 then
        self.m_atlasCount:setVisible(true) 
        self.m_atlasCount:setString("第"..strcount.."局")
    end

    if GlobalUserItem.bPrivateGoldRoom then 
        self.m_room_sp:setPositionX(76.81)
        self.m_atlasRoomID:setPositionX(101.51)
    else
        self.m_room_sp:setPositionX(42.81-10)
        self.m_atlasRoomID:setPositionX(67.51-10)
    end


    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    -- 非房主、且密码房
    print("PriGameLayer:onRefreshInviteBtn ~~")
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end

    if self._gameLayer.gameStatus ~= cmd.GAME_STATUS_FREE  then --空闲场景
        self.m_btnInvite:setVisible(false)
        return
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount >= 1 then 
        self.m_btnInvite:setVisible(false)
        return
    end


    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        print("邀请按钮,系统下发，坐下人数",chairCount, self._gameLayer:onGetSitUserNum())
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            return
        end
    end
    self.m_btnInvite:setVisible(true)

end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table )

    self._gameLayer._gameView:removeChildByName("private_end_layer")

    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self._gameLayer._gameView)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")
    csbNode:setLocalZOrder(90)

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    print("IsDismiss ===================>", self._gameLayer.bIsDismiss)

    local image_bg = csbNode:getChildByName("Sprite_1")

    local chairCount = PriRoom:getInstance():getChairCount()
    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    --dump(scoreList, "scoreList", 6)

    local time = image_bg:getChildByName("time")
    if time then 
        time:setString(os.date("%Y/%m/%d  %H:%M:%S",os.time()))
    end

    --房间ID
    local labID = image_bg:getChildByName("roominfo")
    if nil ~= labID then
        labID:setString("房间ID:"..self.m_atlasRoomID:getString())
    end
    --时间
    local labTime = image_bg:getChildByName("text_time")
    if nil ~= labTime and not GlobalUserItem.bVideo then
        labTime:setString("时间:"..cmd_table.sysStartTime.wYear.."-"..cmd_table.sysStartTime.wMonth.."-"..cmd_table.sysStartTime.wDay.."("
                                ..cmd_table.sysStartTime.wHour..":"..cmd_table.sysStartTime.wMinute.."~"..cmd_table.sysEndTime.wHour..":"
                                ..cmd_table.sysEndTime.wMinute..")")
    end
    --dump(cmd_table, "总结算信息====================>", 10)
    local jsonStr = cjson.encode(cmd_table)
    LogAsset:getInstance():logData(jsonStr, true)

    chairCount = self._gameLayer.allCountData.m_userWinScore

    local listview = image_bg:getChildByName("ListView")
    listview:setScrollBarEnabled(false)
    local layer = ccui.Layout:create()
    listview:addChild(layer)
    local width = 1237
    if PriRoom:getInstance().m_tabPriData.dwPlayCount > 5 then 
        width = width + (PriRoom:getInstance().m_tabPriData.dwPlayCount - 5)*175
    end
    layer:setContentSize(cc.size(width,519))

    local index = 0
    for k,v in pairs(self._gameLayer.allCountData.m_userWinScore) do
        local useritem = self._gameLayer:getUserInfoByChairID(k - 1)
        if useritem then
            local image = ccui.ImageView:create("game/tiao.png")
            image:setAnchorPoint(cc.p(0, 0.5))
            image:setPosition(0, 496-(46*index))
            image:setScale9Enabled(true)
            image:setContentSize(cc.size(width,46))
            layer:addChild(image)
            index = index + 1

            if self._gameLayer.allCountData.cbBankerID == useritem.wChairID then 
                local sp = cc.Sprite:create("game/zhuangjia-biao.png")
                image:addChild(sp)
                sp:setPosition(22,23)
            end

            --Create Text_1
            local text = ccui.Text:create()
            text:setFontName("fonts/round_body.ttf")
            text:setFontSize(24)
            text:setString(useritem.szNickName)
            text:setAnchorPoint(0, 0.5)
            text:setPosition(43, 23)
            text:setTextColor({r = 78, g = 50, b = 50})
            image:addChild(text)

            --Create Text_1_0
            local text = ccui.Text:create()
            text:setFontName("fonts/round_body.ttf")
            text:setFontSize(24)
            text:setString([[总分:]])
            text:setAnchorPoint(0, 0.5)
            text:setPosition(204, 23)
            text:setTextColor({r = 78, g = 50, b = 50})
            image:addChild(text)

            local allScore = 0
            for i=1,#v do
                allScore = allScore + v[i]
            end
            --Create Text_1_0_0
            local text = ccui.Text:create()
            text:setFontName("fonts/round_body.ttf")
            text:setFontSize(30)
            text:setString(allScore)
            text:setAnchorPoint(0, 0.5)
            text:setPosition(264, 23)
            text:setTextColor({r = 15, g = 129, b = 0})
            image:addChild(text)

            if allScore < 0 then 
                text:setTextColor({r = 205, g = 58, b = 0})
            end

            for i=1,PriRoom:getInstance().m_tabPriData.dwPlayCount do
                local text = ccui.Text:create()
                text:setFontName("fonts/round_body.ttf")
                text:setFontSize(24)
                text:setString("第"..i.."局")
                text:setAnchorPoint(0, 0.5)
                text:setPosition(361+(i-1)*173.5, 23)
                text:setTextColor({r = 134, g = 55, b = 55})
                image:addChild(text)

                --Create Text_1_0_0_0
                local text = ccui.Text:create()
                text:setFontName("fonts/round_body.ttf")
                text:setFontSize(30)
                text:setString(v[i])
                text:setAnchorPoint(0, 0.5)
                text:setPosition(438+(i-1)*174.35, 23)
                text:setTextColor({r = 15, g = 129, b = 0})
                image:addChild(text)

                if v[i] < 0 then 
                    text:setTextColor({r = 205, g = 58, b = 0})
                end


                local btn = ccui.Button:create()
                btn:setScale9Enabled(true)
                btn:setContentSize(cc.size(120,46))
                image:addChild(btn)
                btn:setAnchorPoint(cc.p(0,0.5))
                btn:setPosition(361+(i-1)*173.5, 23)
                local index = i
                btn:addClickEventListener(function ()
                    -- self.gameCount = index
                    -- self.detailsUseritem = useritem
                    -- PriRoom:getInstance():setDetailsViewFrame(self)
                    -- PriRoom:getInstance()._priFrame:sendSingleVideo(self.m_atlasRoomID:getString(),self.gameCount)
                end)
            end


        end

    end

    local labID = image_bg:getChildByName("roominfo")
    if nil ~= labID then
        labID:setString("房间ID:"..self.m_atlasRoomID:getString().."   人数："..index)
    end

    -- 分享按钮
    local btn = image_bg:getChildByName("share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener(btncallback)



    -- 退出按钮
    btn = image_bg:getChildByName("exit")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)


    -- 退出按钮
    btn = image_bg:getChildByName("close")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    csbNode:setVisible(false)


    local count = 0
    for k,v in pairs(self._gameLayer.gameEndData) do
        if v then 
            count = count + 1
        end
    end

    if count <= 0 then 
        csbNode:setVisible(true)
    end

    self._gameLayer.isPriOver = true

end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/Zhajinhua_game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/Zhajinhua_game.png")
end

function PriGameLayer:onSubSingleVideo(pdata)

    -- body


    -- local sub = pdata.readword()

    local data = ExternalFun.read_netdata(cmd.CMD_S_GAMERecord, pdata)
    dump( data, "战绩详情", 6)

    local tab = {}

    -- dump(data,"intBetInfo",12)

    local item = {}
    item.useritem = self.detailsUseritem
    
    item.lAllAreaInfo = data.lAllAreaInfo[item.useritem.wChairID+1]
    item.lAllBaoZiInfo = data.lAllBaoZiInfo[item.useritem.wChairID+1]
    item.lAllChuanInfo = data.lAllChuanInfo[item.useritem.wChairID+1]
    item.nuo_dan_info = data.nuo_dan_info[item.useritem.wChairID+1]
    item.nuo_chuan_info = data.nuo_chuan_info[item.useritem.wChairID+1]
    tab[#tab+1] = item


    -- 清空没有下注的玩家
    local betInfo = {}
    for k,v in pairs(tab) do
        local isNil = 0
        for m,l in pairs(v) do
            if m == "lAllAreaInfo" or m == "lAllBaoZiInfo" or m == "lAllChuanInfo" then 
                for w,n in pairs(l) do
                    if n ~= 0 then
                        isNil = isNil + 1
                    end
                end
            end

            if m == "nuo_dan_info" or m == "nuo_chuan_info" then
                for w,n in pairs(l) do
                    if n ~= 0 then
                        isNil = isNil + 1
                    end
                end
            end
        end
        print(" isNil ", isNil)
        if isNil > 0 then 
            betInfo[#betInfo+1] = v
        end
    end

    -- if #betInfo<=0 then 
    --     showToast(self._gameLayer, "暂无下注详情!", 1)
    --     return
    -- end

    local layer = BetInfoLayer:create(self._gameLayer,"ui/BetInfo_0.csb")
    self._gameLayer._gameView:addChild(layer,91)
    layer:setData(betInfo)
    layer:setName(NAME_BET_INIFO_LAYER)
    layer:setSice(data.cbSice)
    layer:setCount(self.gameCount)

    -- layer:set

end

return PriGameLayer