--
-- Author: David
-- Date: 2017-4-11 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src.privateroom.DismissLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src.models.CMD_Game")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
local BTN_INFO = 106


function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)

    -- do return end
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("PrivateGameLayer.csb", self)--(cmd.RES_PATH.."privateroom/game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    --
    --local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = csbNode:getChildByName("num_count")
    self.m_atlasCount:setString("0:0")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- -- 解散按钮
    -- local btn = csbNode:getChildByName("bt_dismiss")
    -- btn:setTag(BTN_DISMISS)
    -- btn:addTouchEventListener(btncallback)

    -- -- 暂离按钮
    -- btn = csbNode:getChildByName("bt_zanli")
    -- btn:setTag(BTN_ZANLI)
    -- btn:addTouchEventListener(btncallback)

    -- --详情按钮
    -- local btn = csbNode:getChildByName("btn_info")
    -- btn:setTag(BTN_INFO)
    -- btn:addTouchEventListener(btncallback)

    local btn = csbNode:getChildByName("btn_info")
    btn:setTag(BTN_INFO)
    btn:addTouchEventListener(btncallback)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setPositionY(650)
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, self._gameLayer._gameView.ZORDER_RESULT)
end

function PriGameLayer:setGameInfoBtn()
    local btn = self.m_csbNode:getChildByName("btn_info")

    -- btn:loadTextureNormal("game/btn_roomInfo1.png")
    -- btn:loadTexturePressed("game/btn_roomInfo1.png")
    -- btn:loadTextureDisabled("game/btn_roomInfo1.png")

    self.m_atlasCount:setVisible(false)
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if BTN_DISMISS == tag then              -- 请求解散游戏
        -- local useritem = self._gameLayer:GetMeUserItem()

        -- if self._gameLayer.m_cbGameStatus == 0 and useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then

        --     PriRoom:getInstance():getNetFrame():sendDissumeGame(self._gameLayer:GetMeTableID())

        --    -- self.m_rootLayer:removeAllChildren()
        --     --GlobalUserItem.bWaitQuit = false
        --    -- self._gameLayer:onExitRoom()
        -- else
        --     PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum())
        -- end
        PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum())
    elseif BTN_INVITE == tag then
        print("获取分享信息===》",self.m_atlasRoomID:getString())
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID,
            function(cmd_table)
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()
                print("获取分享信息===》",scene)
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
                                        showToast(self, "分享成功", 2)
                                    end
                                end
                                local passwd = dwRoomDwd
                                local szRoomID = self.m_atlasRoomID:getString()
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0

                                local ruleText = self._gameLayer:grabRule(self._gameLayer)
                                local shareTxt = string.format("局数:%s。%s", drawCount, ruleText)

                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. szRoomID .. "&k=391&a=1" .. "&p=" .. passwd
                                print("获取分享信息===", szRoomID,shareTxt,url)
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
                    showToast(self, "分享成功", 2)
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
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- print("PriGameLayer:onRefreshInfo ignore =====", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)
    -- do return end
    print("PriGameLayer:onRefreshInfo=========================")
    -- 房间ID
    self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " : " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)

    if PriRoom:getInstance().m_tabPriData.dwDrawCountLimit>10000 then
        self.m_atlasCount:setString(PriRoom:getInstance().m_tabPriData.dwPlayCount)
    end
    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    -- print("PriGameLayer:onRefreshInviteBtn ignore =====")
    -- do return end
    -- print("PriGameLayer:onRefreshInviteBtn=========================")
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
    if self._gameLayer.m_cbGameStatus ~= 0 or PriRoom:getInstance().m_tabPriData.dwPlayCount ~= 0 then --空闲场景
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

function PriGameLayer:onClickShare()
    print("onClickShare")
    do return end
    PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
        local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享成功", 2)
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
end

function PriGameLayer:onClickQuit()
    print("onClickQuit")
    GlobalUserItem.bWaitQuit = false
    self._gameLayer:onExitRoom()
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table,dataBuffer )
    dump(cmd_table, "#onPriGameEnd==")

    --local function toCall()\
        self._gameLayer.isPriOver = true
        local dataList = {}

        local userRecord = self._gameLayer:getDetailScore()
        if not userRecord then
            print("PriGameLayer:onPriGameEnd userRecord is nil")
            userRecord = {}
        end

        for i=1,1000 - cmd_table.nSpecialInfoLen do
            dataBuffer:readbyte()
        end
        local bigWinner = dataBuffer:readword()

        local playerNum = self._gameLayer:getPlayerNum()
        local roomNum = self.m_atlasRoomID:getString()

        dataList.round = PriRoom:getInstance().m_tabPriData.dwPlayCount
        dataList.topRound = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
        dataList.roomNum = roomNum
        local mostScore = 0

        local isHasRecord = true
        for i=1,playerNum do
            local useritem = self._gameLayer:getUserInfoByChairID(i)
            local recordData = userRecord[i]

            if recordData and (self._gameLayer.m_cbPlayStatus[i] == 1 or  recordData.lAllScore ~= 0) then
                local playerData = {}
                playerData.nick = useritem.szNickName
                playerData.id = useritem.dwGameID
                playerData.score = recordData.lAllScore
                playerData.zimoCount = recordData.cbZiMo
                playerData.huCount = recordData.cbHuCount
                playerData.paoCount = recordData.cbDianPao
                playerData.angangCount = recordData.cbAnGang
                playerData.cbMingGang = recordData.cbMingGang
                playerData.useritem = clone(useritem)
                playerData.isMostWin = i-1 == bigWinner and true or false

                if playerData.score > mostScore then
                    mostScore = playerData.score
                end
                table.insert(dataList, playerData)
            end
        end
        
        -- for i=1,playerNum do
        --     local playerData = dataList[i]
        --     playerData.isMostWin = playerData.score >= mostScore
        -- end




        ----------游戏逻辑要停掉
        self._gameLayer._gameView:setBtnStartVisible(false)
        self._gameLayer:KillGameClock()
        ----------游戏逻辑要停掉

        local function callbackFinalReport(etype)
            if etype == "share" then
                self:onClickShare()
            elseif etype == "quit" then
                --需要规定时间关闭,总结算退出游戏,防止消息漏掉,卡房
                self._gameLayer._gameView.objFinalReport = nil
                self:onClickQuit()
            end
        end

        --设计的问题,导致结算的流程比较复杂
        local isCanShowFinalResult = self._gameLayer._gameView:isCanShowFinal()
        local uiFinalReport = require(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "views.uiFinalReport")
        local obj = uiFinalReport:ctor(self._gameLayer._gameView, callbackFinalReport)
        obj:hide()
        obj:refresh(dataList)
        self._gameLayer._gameView.objFinalReport = obj
        --self:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(function ( ... )
        if not self._gameLayer.m_bGameEnd then 
            obj:show()
        end
                    
        --end)))

end

function PriGameLayer:showPriEnd()
    self._gameLayer._gameView.objFinalReport:show()
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"`
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
end

return PriGameLayer