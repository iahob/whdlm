local GameModel = class("GameModel", function(frameEngine,scene)
        local gameLayer =  display.newLayer()
    return gameLayer
end)
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")

--[[
    此类负责处理游戏服务器与客户端交互
    游戏数据保存放于此 于 onInitData 中初始化
    网络消息放于此
    计时器处理放于此
]]

-- local cmd = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.models.CMD_Game")
-- local GameLogic = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.models.GameLogic")
-- local GameViewLayer = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.views.layer.GameViewLayer")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 初始化界面
function GameModel:ctor(frameEngine,scene)
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
	self._gameFrame = frameEngine

    --设置搜索路径
    self._gameKind = self:getGameKind()
    self._searchPath = ""
    self:setSearchPath()

    self._gameView = self:CreateView()
    self:OnInitGameEngine()
    self.m_bOnGame = false
    self.m_cbGameStatus = -1
    GlobalUserItem.bAutoConnect = true

    -- 喇叭列表
    self.m_gameTrumpetList = {}
    -- 喇叭背景
    self.m_spGameTrumpetBg = nil

    -- 录音监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_VOICE_NOTIFY,handler(self, self.onVoiceNotify))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
    -- 错误监听
    self.m_errorListener = cc.EventListenerCustom:create("__lua_runerror_notify__",handler(self, self.onErrorListener))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_errorListener, self)
end

function GameModel:onExit()

end

function GameModel:onEnterTransitionFinish()

end

function GameModel:onCleanup()
    print("GameModel:onCleanup")
    GlobalUserItem.bAutoConnect = true
    self:KillGameClock()
    self:reSetSearchPath()
    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
        self.m_listener = nil
    end
    if nil ~= self.m_errorListener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_errorListener)
        self.m_errorListener = nil
    end
end

--获取gamekind
function GameModel:getGameKind()
    return nil
end

-- 创建场景
function GameModel:CreateView()
    -- body
end

function GameModel:setSearchPath()
    if nil == self._gameKind then
        return
    end

    local entergame = self._scene:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        self._searchPath = device.writablePath.."game/" .. modulestr .. "/res/"
        cc.FileUtils:getInstance():addSearchPath(self._searchPath)
    end
end

function GameModel:reSetSearchPath()
    --重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
    local newPaths = {}
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(self._searchPath) then
            table.insert(newPaths, v)
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths)
end

-- 
function GameModel:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
    if nil ~= self._gameView.addToRootLayer then
        self._gameView:addToRootLayer(layer, self:priGameLayerZorder())
    else
        self._gameView:addChild(layer, self:priGameLayerZorder())
    end
end

-- 房卡信息层zorder
function GameModel:priGameLayerZorder()
    if nil ~= self._gameView and nil ~= self._gameView.priGameLayerZorder then
        return self._gameView:priGameLayerZorder()
    end
    return yl.MAX_INT
end

-- 回放层配置
--[[
config = 
{
    pos, -- 回放按钮位置
}
]]
function GameModel:getVideoLayerConfig()
    print("GameModel: getVideoLayerConfig return default config")
    return nil
end

-- 添加回放控制层
function GameModel:addVideoControlLayer( layer )
    if nil == layer then
        return
    end
    if nil ~= self._gameView.addToRootLayer then
        self._gameView:addToRootLayer(layer, yl.MAX_INT)
    else
        self._gameView:addChild(layer, yl.MAX_INT)
    end
end

--显示等待
function GameModel:showPopWait()
    if self._scene and self._scene.showPopWait then
        self._scene:showPopWait()
    end
end

--关闭等待
function GameModel:dismissPopWait()
    if self._scene and self._scene.dismissPopWait then
        self._scene:dismissPopWait()
    end
end

--初始化游戏数据
function GameModel:OnInitGameEngine()

    self._ClockFun = nil
    self._ClockID = yl.INVALID_ITEM
    self._ClockTime = 0
    self._ClockChair = yl.INVALID_CHAIR
    self._ClockViewChair = yl.INVALID_CHAIR

end

--重置框架
function GameModel:OnResetGameEngine()
    self:KillGameClock()
    self.m_bOnGame = false
end

--退出询问
function GameModel:onQueryExitGame()
    print(GlobalUserItem.dwCurRoomGroupID.."=====onQueryExitGame=====")
    if (PriRoom and true == GlobalUserItem.bPrivateRoom and false == GlobalUserItem.bPrivateGoldRoom) or GlobalUserItem.dwCurRoomGroupID ~= 0 then  --金币房卡可以直接退出
        PriRoom:getInstance():queryQuitGame(self.m_cbGameStatus)
    else

        if self._queryDialog then
           return
        end
        local tip = self.m_cbGameStatus > 0 and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
       
        self._queryDialog = QueryDialog:create(tip, function(ok)
            if ok == true then
                --退出防作弊
                self._gameFrame:setEnterAntiCheatRoom(false)
                if  self.m_cbGameStatus > 0 then

                    self._gameFrame:onCloseSocket()
                    self:dismissPopWait()
                    self:stopAllActions()
                    self:KillGameClock()
                    self._scene:onKeyBack()
                    return
                end
                
                self:onExitTable()
            end
            self._queryDialog = nil
        end):setCanTouchOutside(false)
            :addTo(self)
    end
end

-- 退出桌子
function GameModel:onExitTable()
    self:stopAllActions()
    self:KillGameClock()

    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        
            local wait = self._gameFrame:StandUp(1)

            if wait then
                self:showPopWait()
                return
            end
        
    end
    self:dismissPopWait()
    self._scene:onKeyBack()
end

function GameModel:onExitRoom()
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
end

-- 返回键处理
function GameModel:onKeyBack()
    -- 回放屏蔽
    if GlobalUserItem.bVideo then
        return true
    end
    -- 解散申请屏蔽
    if GlobalUserItem.bPrivateRoom and nil ~= self._gameView._priView and nil ~= self._gameView._priView._dismissLayer then
        if self._gameView._priView._dismissLayer:isVisible() then
            return true
        end
    end
    self:onQueryExitGame()
    return true
end

-- 获取自己椅子
function GameModel:GetMeChairID()
    return self._gameFrame:GetChairID()
end

-- 获取自己桌子
function GameModel:GetMeTableID()
   return self._gameFrame:GetTableID()
end

-- 获取自己
function GameModel:GetMeUserItem()
    return self._gameFrame:GetMeUserItem()
end

-- 椅子号转视图位置,注意椅子号从0~nChairCount-1,返回的视图位置从1~nChairCount
function GameModel:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = self._gameFrame:GetChairCount()
    local nChairID = self:GetMeChairID()
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        viewid = math.mod(chair + math.floor(nChairCount * 3/2) - nChairID, nChairCount) + 1
    end
    return viewid
end

-- 是否合法视图id
function GameModel:IsValidViewID( viewId )
    local nChairCount = self._gameFrame:GetChairCount()
    if nil == nChairCount then
        return false
    end
    return (viewId > 0) and (viewId <= nChairCount)
end

-- 设置计时器
function GameModel:SetGameClock(chair,id,time)
    if not self._ClockFun then
        local this = self
        self._ClockFun = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
                this:OnClockUpdata()
            end, 1, false)
    end
    self._ClockChair = chair
    self._ClockID = id
    self._ClockTime = time or 0
    self._ClockViewChair = self:SwitchViewChairID(chair)
    self:OnUpdataClockView()
end

function GameModel:GetClockViewID()
    return self._ClockViewChair
end

-- 关闭计时器
function GameModel:KillGameClock(notView)
    print("KillGameClock")
    self._ClockID = yl.INVALID_ITEM
    self._ClockTime = 0
    self._ClockChair = yl.INVALID_CHAIR
    self._ClockViewChair = yl.INVALID_CHAIR
    if self._ClockFun then
        --注销时钟
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._ClockFun) 
        self._ClockFun = nil
    end
    if not notView then
        self:OnUpdataClockView()
    end
end

--计时器更新
function GameModel:OnClockUpdata()
    if  self._ClockID ~= yl.INVALID_ITEM and self._ClockTime then
        self._ClockTime = self._ClockTime - 1

        local result = self:OnEventGameClockInfo(self._ClockChair,self._ClockTime,self._ClockID)
        if self._ClockTime and (result == true   or self._ClockTime < 1) then
            if self.KillGameClock then
                self:KillGameClock()
            end
        end
    end
    if self.OnUpdataClockView then 
        self:OnUpdataClockView()
    end
end

--更新计时器显示
function GameModel:OnUpdataClockView()
    if self._gameView and self._gameView.OnUpdataClockView then
        self._gameView:OnUpdataClockView(self._ClockViewChair,self._ClockTime)
    end
end

-- 计时器响应
function GameModel:OnEventGameClockInfo(chair,time,clockid)
    -- body
end

--用户状态
function GameModel:onEventUserStatus(useritem,newstatus,oldstatus)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    local MyChair = self:GetMeChairID()

    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    --旧的清除
    if oldstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(oldstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, nil, useritem.cbUserStatus == yl.US_FREE)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, true)
            end
        end
    end

    --更新新状态
    if newstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(newstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end
end

--用户积分
function GameModel:onEventUserScore(useritem)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end 

    if  MyTable == useritem.wTableID then
        local viewid = self:SwitchViewChairID(useritem.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
        end
    end 
end

--用户进入
function GameModel:onEventUserEnter(tableid,chairid,useritem)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    if MyTable == tableid then
        local viewid = self:SwitchViewChairID(chairid)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)

            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end
end

--发送准备
function GameModel:SendUserReady(dataBuffer)
    self._gameFrame:SendUserReady(dataBuffer)
end

--发送数据
function GameModel:SendData(sub,dataBuffer)
    if self._gameFrame then
        dataBuffer:setcmdinfo(game_cmd.MDM_GF_GAME, sub)
        return self._gameFrame:sendSocketData(dataBuffer)   
    end

    return false
end

--是否观看
function GameModel:IsLookonMode()
    
end

--播放音效
function GameModel:PlaySound(path)
    if GlobalUserItem.bSoundAble == true then
        AudioEngine.playEffect(path)
    end
end

-- 场景消息
function GameModel:onEventGameScene(cbGameStatus,dataBuffer)

end

-- 游戏消息
function GameModel:onEventGameMessage(sub,dataBuffer)
    -- body
end

-- 私人房解散响应
-- useritem 用户数据
-- cmd_table CMD_GR_RequestReply(回复数据包)
-- 返回是否自定义处理
function GameModel:onCancellApply(useritem, cmd_table)
    print("base onCancellApply")
    return false
end

-- 私人房解散结果
-- 返回是否自定义处理
function GameModel:onCancelResult( cmd_table )
    print("base onCancelResult")
    return false
end

-- 桌子坐下人数
function GameModel:onGetSitUserNum()
    print("base get sit user number")
    return 0
end

-- 根据chairid获取玩家信息
function GameModel:getUserInfoByChairID( chairid )
    
end

-- 框架通知准备
function GameModel:onGetNoticeReady()
    print("GameModel onGetNoticeReady")
end

-- 喇叭背景
function GameModel:addTrumpetBackground( )

end
-- 游戏喇叭
function GameModel:onAddGameTrumpet(item)
    item = item or {}
    table.insert(self.m_gameTrumpetList, clone(item))

    if nil == self.m_spGameTrumpetBg then
        local trumpetBg = cc.Sprite:create("plaza/plaza_btn_notice_0.png")
        if nil ~= trumpetBg then 
            self.m_spGameTrumpetBg = trumpetBg
            trumpetBg:setScaleX(0.0001)
            self:addChild(trumpetBg)
            local bgsize = self.m_spGameTrumpetBg:getContentSize()    
            trumpetBg:setPosition(appdf.WIDTH * 0.5, appdf.HEIGHT - bgsize.height)

            -- 消息区域
            local stencil = display.newSprite()
                :setAnchorPoint(cc.p(0,0.5))
            stencil:setTextureRect(cc.rect(0, 0, bgsize.width - 50, bgsize.height))
            local clip = cc.ClippingNode:create(stencil)
                :setAnchorPoint(cc.p(0,0.5))
            clip:setInverted(false)
            clip:move(25,bgsize.height * 0.5 + 2)
            clip:addTo(trumpetBg)
            notifyText = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
                                        :addTo(clip)
                                        :setTextColor(cc.c4b(255,191,123,255))
                                        :setAnchorPoint(cc.p(0,0.5))
                                        :enableOutline(cc.c4b(79,48,35,255), 1)
            self.m_spGameTrumpetBg.trumpetText = notifyText
            trumpetBg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5,1.0), cc.CallFunc:create(function()
                self:onGameTrumpet()
            end)))
        end
    end
end

function GameModel:onGameTrumpet( )
    if nil ~= self.m_spGameTrumpetBg then
        local item = self.m_gameTrumpetList[1]
        local szmarqueetitle = nil
        if nil ~= item then
            szmarqueetitle = item.marqueetitle
            if true == item.autoremove then
                local count = item.showcount or 0
                item.showcount = count - 1
                if item.showcount <= 0 then
                    local idx = nil
                    for k,v in pairs(self.m_gameTrumpetList) do
                        if nil ~= v.id and v.id == item.id then
                            idx = k
                            break
                        end
                    end
                    if nil ~= idx then
                        table.remove(self.m_gameTrumpetList, idx)
                    end
                end
            end
        end
         
        local text = self.m_spGameTrumpetBg.trumpetText   
        local bgsize = self.m_spGameTrumpetBg:getContentSize()                  
        if nil ~= text and nil ~= szmarqueetitle then
            text:setString(szmarqueetitle)
            text:setPosition(cc.p(bgsize.width, 0))
            text:stopAllActions()
            local tmpWidth = text:getContentSize().width
            text:runAction(cc.Sequence:create(cc.MoveTo:create(16 + (tmpWidth / bgsize.width),cc.p(0-tmpWidth,0)),cc.CallFunc:create(function()
                if 0 ~= #self.m_gameTrumpetList then
                    self:onGameTrumpet()
                else
                    self.m_spGameTrumpetBg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5, 0.0001, 1.0), cc.CallFunc:create(function()
                        self.m_spGameTrumpetBg:removeFromParent()
                        self.m_spGameTrumpetBg = nil
                    end)))
                end                                                     
            end)))
        end
    end
end

function GameModel:onReQueryFailure(code, msg)
    self:dismissPopWait()
    if nil ~= msg and type(msg) == "string" then
        showToast(self._scene,msg,2)
    end

    self:onExitTable()
end

-- 系统截屏
-- @param[imagepath] 图片路径
function GameModel:onTakeScreenShot( imagepath )
    print("base onTakeScreenShot")
end

function GameModel:onVoiceNotify( event )
    print("GameModel:onVoiceNotify ==> ", event.obj)
    local msgWhat = event.obj
    if nil ~= msgWhat then
        if msgWhat == yl.RY_MSG_VOICE_START then
            self:onUserVoiceStart(event.useritem, event.spath)
        elseif msgWhat == yl.RY_MSG_VOICE_END then
            self:onUserVoiceEnded(event.useritem, event.spath)
        end
    end 
end

function GameModel:onUserVoiceStart(useritem, spath)
    print("GameModel:onUserVoiceStart")
end

function GameModel:onUserVoiceEnded(useritem, spath)
    print("GameModel:onUserVoiceEnded")
end

function GameModel:onErrorListener( event )
    dump(event, "GameModel:onErrorListener", 6)
    -- 触发断网重连
    if self._gameFrame:isSocketServer() then
        self._gameFrame:OnResetGameEngine()
        self._scene:onStartGame()
    end
end

return GameModel