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

local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.privateroom.DismissLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104

function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("privateroom/game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    self.m_csbNode = csbNode

  

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = csbNode:getChildByName("num_count")
    self.m_atlasCount:setString("0 / 0")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
   

   

   

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)

    self.m_btnInvite2 = csbNode:getChildByName("bt_invite2")
    self.m_btnInvite2:setTag(BTN_INVITE)
    self.m_btnInvite2:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, 25)
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
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                                    end
                                end
                                local passwd = dwRoomDwd
                                local szRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0
                                
                                local strPlayerCount = self._gameLayer.playerCount == 0 and "2-5人," or self._gameLayer.playerCount.."人,"

                                local shareTxt = "局数:" .. drawCount .. "局。"..strPlayerCount.."疯狂三张精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. szRoomID .. "&k=56&a=1" .. "&p=" .. passwd
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
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
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
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString("房间号："..string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    local strcount = "局数："..PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)
    if GlobalUserItem.bPrivateGoldRoom  and GlobalUserItem.dwCurRoomGroupID == 0 then
        self.m_atlasCount:setString("")
    end

    if PriRoom:getInstance().m_tabPriData.dwDrawCountLimit>1000 then
        self.m_atlasCount:setString("")
    end

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(false)
        return
    end
    if self._gameLayer.m_cbGameStatus ~= 0 then --空闲场景
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(true)
        return
    end

    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        print("邀请按钮,系统下发，坐下人数",chairCount, self._gameLayer:onGetSitUserNum())
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            self.m_btnInvite2:setVisible(false)
            return
        end
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount==0 then
        self.m_btnInvite:setVisible(true)
        self.m_btnInvite2:setVisible(false)
    else
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(true)
    end
end

function PriGameLayer:updateReloadCardType()
    local csbNode = self._gameLayer._gameView:getChildByName("private_end_layer")
    if not csbNode then
        return
    end

    local image_bg = csbNode:getChildByName("Image_bg")
    local chairCount = PriRoom:getInstance():getChairCount()
    for i = 1, chairCount do
        local cellbg = image_bg:getChildByName("Node_info" .. i)
        local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
        if cellbg and useritem then
            for j=3 , 6 do
                local typeText = cellbg:getChildByName("CardType_"..j)
                if self._gameLayer.m_cardType[i][j] then
                    typeText:setString(self._gameLayer.m_cardType[i][j])
                else
                    typeText:setString(0)
                end
                    
            end
        end
    end
    
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )--
     self._gameLayer._gameView:removeChildByName("private_end_layer")

    print("onPriGameEnd ===================================>")


    local csbNode = ExternalFun.loadCSB("privateroom/game/PrivateGameEndLayer.csb", self._gameLayer._gameView)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")
    csbNode:setLocalZOrder(90)
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    print("IsDismiss ===================>", self._gameLayer.bIsDismiss)
    if self._gameLayer.bIsDismiss then
        csbNode:setVisible(true)
    end

    self._gameLayer.isPriOver = true
    local image_bg = csbNode:getChildByName("Image_bg")
    local ownerPos = {195.52 , 428.68, 658.07 , 886.2 ,1117.3}

    local chairCount = PriRoom:getInstance():getChairCount()
    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    --dump(scoreList, "scoreList", 6)

    --房间ID
    local labID = image_bg:getChildByName("text_roomID")
    if nil ~= labID then
        labID:setString(self.m_atlasRoomID:getString())
    end
    --时间
    local labTime = image_bg:getChildByName("text_time")
    if nil ~= labTime and not GlobalUserItem.bVideo then
        labTime:setString("创建时间:"..cmd_table.sysStartTime.wYear.."."..cmd_table.sysStartTime.wMonth.."."..cmd_table.sysStartTime.wDay.." "..cmd_table.sysStartTime.wHour..":"..cmd_table.sysStartTime.wMinute)
    end
    --dump(cmd_table, "总结算信息====================>", 10)
    local jsonStr = cjson.encode(cmd_table)
    LogAsset:getInstance():logData(jsonStr, true)
    local maxScore = 0
    local maxScoreIndex = 0
    local typeCard = {"","","顺子×","金花×","顺金×","豹子×"}
    dump(self._gameLayer.m_cardType , "牌型")
    local curNum = 1
    local tabShowCell = {}
    for i = 1, chairCount do
        local cellbg = image_bg:getChildByName("Node_info" .. curNum)
        print("结算时获取背景 Node_info"..i)
        if nil ~= cellbg then
            print("结算时获取背景成功")
            cellbg:setVisible(true)
            local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
            
            dump(self._gameLayer.nowGameUser, "self._gameLayer.nowGameUser", 6)
            if nil ~= useritem  then 
                curNum = curNum + 1
                print("结算时获取到用户信息")
                cellbg.userItem = useritem
                table.insert(tabShowCell , cellbg)
                if useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID and GlobalUserItem.dwCurRoomGroupID == 0 then
                    local owner = image_bg:getChildByName("Sprite_roomHolder")
                    owner:setVisible(true)
                    owner:setPositionX(ownerPos[i])
                end
                -- 头像
                local head = PopupInfoHead:createNormal(useritem, 72)
                head:enableInfoPop(true)
                local nodeFace = cellbg:getChildByName("Node_face")
                if nil ~= nodeFace then
                    nodeFace:addChild(head)
                    nodeFace:setLocalZOrder(1)
                    nodeFace:getChildByName("face_bg"):setLocalZOrder(2)
                end
            
                -- 昵称
                local nickText = cellbg:getChildByName("Text_name") --CSB上字串
                local nick = ClipText:createClipText(cc.size(150, 30), useritem.szNickName, nil, 30)
                cellbg:addChild(nick)
                nick:setAnchorPoint(cc.p(0.5, 0.5))
                if nil ~= nickText then
                    nickText:setVisible(false)
                    nick:setPosition(nickText:getPosition())
                else
                    nick:setPosition(cc.p(0, 50))
                end

                for j=3 , 6 do
                    local typeText = cellbg:getChildByName("CardType_"..j)
                    if self._gameLayer.m_cardType[i][j] then
                        typeText:setString(self._gameLayer.m_cardType[i][j])
                    else
                        typeText:setString(0)
                    end
                    
                end

                local score = scoreList[useritem.wChairID + 1] or 0
                if score>maxScore then
                    maxScore = score
                    maxScoreIndex = i
                end
                -- 成绩
                local strscore = score .. ""
                if score > 0 then
                    strscore = "+" .. score
                else
                    strscore = score
                end

                local cpscore = cellbg:getChildByName("Text_score")
                if cpscore then
                    cpscore:setString(strscore)
                    if score > 0 then
                        cpscore:setColor(cc.c3b(255, 246, 123))
                    else
                        cpscore:setColor(cc.c3b(152, 177, 252))
                    end
                end
                
            else
                cellbg:setVisible(false)
            end
        end        
    end

    local pos = {177.91 , 411.4, 641.52 , 869.95 ,1100}
    local win = image_bg:getChildByName("Sprite_winPlayer")
    -- if maxScoreIndex~=0 then
    --     win:setVisible(true)
    --     win:setPositionX(pos[maxScoreIndex])
    -- else
    --     win:setVisible(false)
    -- end

    --大赢家显示
    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()
    for i = 1, #tabShowCell do
        local cellbg = tabShowCell[i]
        if cellbg.userItem and cellbg.userItem.wChairID==bigWinner then
            win:setVisible(true)
            win:setPositionX(pos[i])
        end
    end


    

   


    -- 分享按钮
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = image_bg:getChildByName("btn_quit")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    if self._gameLayer._gameView.bIsVideo then
        csbNode:setVisible(true)
    else
        csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(7.0),
         cc.CallFunc:create(function()
             csbNode:setVisible(true)
         end)))
    end

    -- 
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
  self._dismissLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/Zhajinhua_game.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("game/Zhajinhua_game.png")
end

return PriGameLayer