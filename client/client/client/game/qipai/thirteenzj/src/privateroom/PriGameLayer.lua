--
-- Author: zhong
-- Date: 2016-12-29 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local DismissLayer = appdf.req(appdf.GAME_SRC.."qipai.thirteenzj.src.privateroom.DismissLayer")
local MarkLayer = appdf.req(appdf.GAME_SRC.."qipai.thirteenzj.src.privateroom.MarkLayer")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.thirteenzj.src.models.cmd_game")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local BTN_JUSHU = 101
local BTN_SHARE = 102
local BTN_INVITE = 103
local BTN_QUIT  = 104
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    self._gameLayer.m_bPriEnd = false
    self._gameLayer.m_bCloseGameEnd = false
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("AtlasLabel_RoomID")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = csbNode:getChildByName("AtlasLabel_count")
    self.m_atlasCount:setString("0 / 0")


    

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 局数按钮
    local btn = csbNode:getChildByName("bt_info")
    btn:setTag(BTN_JUSHU)
    btn:addTouchEventListener(btncallback)
    -- btn:setPositionY(555)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self.m_btnInvite:setPositionY(250)


    self.m_btnInvite_0 = csbNode:getChildByName("bt_invite_0")
    self.m_btnInvite_0:setTag(BTN_INVITE)
    self.m_btnInvite_0:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer)
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if BTN_INVITE == tag then
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
               
                local dwRoomDwd = cmd_table.dwRoomDwd or 0
   
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()

                local function _shareFun( dwRoomDwd )
                    if nil ~= scene and nil ~= scene.popTargetShare then
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
                                local passwd = cmd_table.dwRoomDwd or 0
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID

                                local shareTxt = "局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit .. "局。十三水游戏精彩刺激, 一起来玩吧! "
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=51&a=1" .. "&p=" .. passwd

                                MultiPlatform:getInstance():shareToTarget(
                                    target, 
                                    sharecall, 
                                    string.format("【约战房间: %06d】", dwRoomID), 
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
            bMyFriend = bMyFriend or false
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
        GlobalUserItem.bAutoConnect = true
        self._gameLayer:onExitRoom()
    elseif BTN_JUSHU == tag then
        if self._gameLayer.m_bLookOn then
          showToast(self, "观战中不可使用该功能", 1)
          return
        end
        local mark = MarkLayer:create(self._gameLayer, self._gameLayer.m_nDiFen)
        self:addChild(mark)
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    dump(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, "约战数据onRefreshInfo", 10)
    if PriRoom:getInstance().m_tabPriData.dwPersonalRoomID ~= nil then
        self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))
    else
        return
    end
    
    if GlobalUserItem.bPrivateGoldRoom then
        self.m_csbNode:getChildByName("Sprite_6"):setVisible(false)
        self.m_csbNode:getChildByName("AtlasLabel_count"):setVisible(false)
        --self.m_csbNode:getChildByName("bt_info"):setVisible(true)
    else
        self.m_csbNode:getChildByName("Sprite_6"):setVisible(true)
        self.m_csbNode:getChildByName("AtlasLabel_count"):setVisible(true)
        --self.m_csbNode:getChildByName("bt_info"):setVisible(false)
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount == nil then
        return
    end

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return
    end
    --空闲场景
    if self._gameLayer.m_cbGameStatus ~= 0 then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return
    end
    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            self.m_btnInvite_0:setVisible(true)
            return
        end
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount > 0 then 
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return
    end

    self.m_btnInvite:setVisible(true)
    self.m_btnInvite_0:setVisible(false)
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )
    self._gameLayer.m_bPriEnd = true
    GlobalUserItem.bAutoConnect = false
    self:removeChildByName("private_end_layer")
    if self._gameLayer._selectLayer then
        self._gameLayer._selectLayer:removeFromParent()
    end
   
    local csbNode = ExternalFun.loadCSB("game/PrivateGameEnd.csb", self)
    self._endLayer = csbNode
    -- if not self._gameLayer._bIsEndLayer  then
    --     csbNode:setVisible(false)
    -- end
    csbNode:setName("private_end_layer")
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    ---房间ID
    local roomID = csbNode:getChildByName("fanghao")
    roomID:setString("房号:"..(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000"))
    --创建时间
    local endTime = csbNode:getChildByName("endTime")

    if not GlobalUserItem.bVideo then
        endTime:setString(""..cmd_table.sysEndTime.wYear.."年"..cmd_table.sysEndTime.wMonth.."月"..cmd_table.sysEndTime.wDay.."日\n"
                                ..cmd_table.sysEndTime.wHour..":"..(cmd_table.sysEndTime.wMinute<10 and "0"..cmd_table.sysEndTime.wMinute or cmd_table.sysEndTime.wMinute)..":"..(cmd_table.sysEndTime.wSecond <10 and "0"..cmd_table.sysEndTime.wSecond or cmd_table.sysEndTime.wSecond))
    else
        endTime:setVisible(false)
    end

    local chairCount = self._gameLayer:onGetSitUserNum()

    local showFlag = true
    local scoreList = cmd_table.lScore[1]
    --获取最大成绩
    local maxscore = scoreList[1]
    for i=2,cmd.GAME_PLAYER do
        if scoreList[i] > maxscore then
            maxscore = scoreList[i]
            showFlag = true
        elseif  scoreList[i] == maxscore then
            showFlag = false
        end
    end


    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()

    local curNum = 0
    for i = 1, cmd.GAME_PLAYER do
        local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
        local bIsGame = false
        if nil ~= useritem then
            for j=1,#self._gameLayer.m_playingUser do
                if useritem.dwUserID == self._gameLayer.m_playingUser[j] then 
                    bIsGame = true
                    break
                end
            end
        end

        if nil ~= useritem and (bIsGame or self._gameLayer.m_bLookOn) then
            curNum = curNum+1
            local itembg = csbNode:getChildByName("FileNode_"..curNum)
            itembg:setVisible(true)
            --头像
            local head = PopupInfoHead:createNormal(useritem, 89)
            head:setAnchorPoint(cc.p(0.5,0.5))
            head:setPosition(cc.p(45, 45))
            itembg:getChildByName("headKuang"):addChild(head)

            --昵称
            local tempnick = itembg:getChildByName("name")
            local nick =  ClipText:createClipText(cc.size(140, 25),useritem.szNickName,"fonts/round_body.ttf",23);
            nick:setAnchorPoint(cc.p(0, 0.5))
            nick:setPosition(cc.p(tempnick:getPosition()))
            nick:setColor(tempnick:getColor())
            itembg:addChild(nick)
            tempnick:removeFromParent()

            --玩家ID
            local gameID = itembg:getChildByName("id")
            gameID:setString(""..useritem.dwGameID)

            local winTimes = itembg:getChildByName("winNum")
            winTimes:setString(""..self._gameLayer.m_recordList.cbWinCount[1][i])
            
            local loseTimes = itembg:getChildByName("loseNum")
            loseTimes:setString(""..self._gameLayer.m_recordList.cbLoseCount[1][i])
            
            local dogfallTimes = itembg:getChildByName("pinNum")
            dogfallTimes:setString(""..self._gameLayer.m_recordList.cbEqualCount[1][i])


            local scoreTxt = itembg:getChildByName("score")
            if scoreList[i] < 0  then
                scoreTxt:setString(""..scoreList[i])
            else
                scoreTxt:setString("+"..scoreList[i])
            end

            -- if showFlag and scoreList[i] == maxscore then
            if bigWinner == i - 1 then
                itembg:getChildByName("Sprite_4"):setVisible(true)
                -- itembg:getChildByName("Sprite_1"):setTexture("")

            end
            
            --房主标识
            local fangzhuicon = itembg:getChildByName("Sprite_3")
            if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                fangzhuicon:setVisible(true)
            end
        end
    end 




    -- 分享按钮
    btn = csbNode:getChildByName("bt_share")
    btn:setTag(BTN_SHARE)
    btn:setVisible(true)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = csbNode:getChildByName("bt_qiut")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    
end

function PriGameLayer:showPrivateEndLayer()
    if self._gameLayer.m_bPriEnd then
        self:getChildByName("private_end_layer"):setVisible(true)
    end
end

function PriGameLayer:onExit()
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/land_game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/land_game.png")
end

return PriGameLayer