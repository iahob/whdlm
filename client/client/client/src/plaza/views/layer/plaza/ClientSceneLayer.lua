--
-- Author: zhong
-- Date: 2017-06-13 14:54:53
--
-- 大厅场景层 管理 _sceneLayer
local ClientSceneLayer = class("ClientSceneLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local RoomListLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "RoomLayer")                       -- 房间列表
local ClubLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMain")     -- 俱乐部
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local ClubRoomLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubRoomLayer")     -- 俱乐部
-- ui
local PlazaLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "PlazaLayer")                      -- 大厅底层
-- ui

local VOICE_LAYER_NAME = "__voice_record_layer__"
function ClientSceneLayer:ctor( scene )
    self._scene = scene

    -- 当前层tag
    self.m_nCurrentTag = nil
    -- 层记录
    self._sceneRecord = {}
    -- 键盘监听
    self:registerScriptKeypadHandler(function(event)
        if event == "backClicked" then
            if nil == self._scene then
                return
            end
            if self._scene._popWait == nil then
                -- 移除一个弹窗
                local bHandle = self._scene:removePop()
                
                if not bHandle then
                    local cur_layer = self:getChildByTag(self._sceneRecord[#self._sceneRecord])
                    if cur_layer and cur_layer.onKeyBack then
                        if cur_layer:onKeyBack() == true then
                            return
                        end
                    end
                    self._scene:onKeyBack()
                end
            end
        end
    end)
end

-- 当前层tag
function ClientSceneLayer:getCurrentTag()
    return self.m_nCurrentTag
end

-- 当前层
function ClientSceneLayer:getCurrentLayer()
    return self:getChildByTag(self:getCurrentTag())
end

function ClientSceneLayer:onCleanPackage(name)
    if not name then
        return
    end
    for k ,v in pairs(package.loaded) do
        if type(k) == "string" then
            if string.find(k,name) ~= nil or string.find(k,name) ~= nil then
                print("package kill:"..k) 
                package.loaded[k] = nil
            end
        end
    end
end

-- 层切换
function ClientSceneLayer:onChangeShowMode(nTag, param)
    if nil == self._scene then
        return
    end
    local oldcount = collectgarbage("count")
    cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames() 
    collectgarbage("collect")
    local curcount = collectgarbage("count")
    print("**************collect:" .. (oldcount - curcount) .. "********************")

    local curtag = nil            --当前页面ID
    ExternalFun.popupTouchFilter()

    --当前页面
    if #self._sceneRecord > 0 then
        curtag = self._sceneRecord[#self._sceneRecord]
    end
    if not nTag then
        -- 返回登录
        if #self._sceneRecord < 1 or curtag == yl.SCENE_PLAZA then
            print("ClientSceneLayer:onChangeShowMode 回退至登陆")
            self._scene:exitPlaza()
            return
        end
        curtag = self._sceneRecord[#self._sceneRecord]
        self._sceneRecord[#self._sceneRecord] = nil
        --上一页面
        nTag = self._sceneRecord[#self._sceneRecord]
    else
        self._sceneRecord[#self._sceneRecord+1] = nTag --记录ID
    end

    self.m_nCurrentTag = self._sceneRecord[#self._sceneRecord]
    --当前页面(游戏界面)
    if curtag then
        local cur_layer = self:getChildByTag(curtag)
        if cur_layer then
            if curtag == yl.SCENE_GAME  then
                ExternalFun.playPlazzBackgroudAudio()
                -- 销毁回调
                PriRoom:getInstance():setViewFrame(nil)
                GameVideo:getInstance():setViewFrame(nil)
                if MatchRoom then
                    MatchRoom:getInstance():setViewFrame(nil)
                end
                -- 断开网络
                self._scene._gameFrame:onCloseSocket()
                self._scene._gameFrame:setScene(nil)
                self._scene._gameFrame:removeNetQuery()
            elseif curtag == yl.SCENE_MATCH then
                print("MatchRoomList 移除")
            elseif curtag == yl.SCENE_CLUB and nTag == yl.SCENE_CLUB then
                    self._sceneRecord[#self._sceneRecord] = nil
            elseif curtag == yl.SCENE_CLUB_ROOM and nTag == yl.SCENE_CLUB_ROOM then
                    self._sceneRecord[#self._sceneRecord] = nil
            end
            cur_layer:stopAllActions()
            cur_layer:removeFromParent()   
        end
    end

    -- 移除弹窗
    local popList = self._scene._scenePopLayer:getChildren()
    local popCount = #popList
    for k,v in pairs(popList) do
        if v:isVisible() then
            if v.animationRemove then
                v:animationRemove()
            else
                v:removeFromParent()
            end
        else
            v:removeFromParent()
        end
    end

    local dst = nil
    --目标页面
    if nTag == yl.SCENE_PLAZA then           -- 游戏列表
        -- 大厅底层
        dst = PlazaLayer:create(self._scene, {})
        -- 回放重置
        GameVideo:getInstance():exitGame()
        -- 约战重置
        PriRoom:getInstance():exitGame()
        --比赛重置
        if MatchRoom then
            MatchRoom:getInstance():exitGame()
        end
    elseif nTag == yl.SCENE_GAME then           -- 游戏界面
        local entergame = self._scene:getEnterGameInfo()
        if nil ~= entergame then
            local modulestr = entergame._KindName
            self:onCleanPackage(modulestr)

            local gameScene = appdf.req(appdf.GAME_SRC .. modulestr .. "src.views.GameLayer")
            if gameScene then
                dst = gameScene:create(self._scene._gameFrame,self._scene)                
            end
            if PriRoom and nil ~= dst and true == GlobalUserItem.bPrivateRoom then
                PriRoom:getInstance():enterGame(dst, self._scene)
            end

            --比赛
            if MatchRoom and nil ~= dst and GlobalUserItem.bMatch then
                MatchRoom:getInstance():enterGame(dst, self._scene)
            end
            -- 回放
            if GameVideo and nil ~= dst and true == GlobalUserItem.bVideo then
                GameVideo:getInstance():enterGame(dst, self._scene, param)
            end
            
        else
            print("游戏记录错误")
        end
    elseif nTag == yl.SCENE_ROOMLIST then           -- 游戏房间界面
        --是否有自定义房间列表
        local entergame = self._scene:getEnterGameInfo()
        if nil ~= entergame then
            local modulestr = string.gsub(entergame._KindName, "%.", "/")
            local targetPlatform = cc.Application:getInstance():getTargetPlatform()
            local customRoomFile = ""
            -- if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            --     customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua"
            -- else
            --     customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua"
            -- end
            -- if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
            --     dst = appdf.req(customRoomFile):create(self._scene, self._scene._gameFrame, param)
            -- end
            --旗舰使用统一房间界面
        end
        if nil == dst then
            dst = RoomListLayer:create(self._scene, param or {})
        end  
    elseif nTag == yl.SCENE_MATCH then
       --比赛
        if MatchRoom then
            dst = MatchRoom:getInstance():getTagLayer(MatchRoom.LAYTAG.MATCH_ROOMLIST) 

        end
    elseif nTag == yl.SCENE_CLUB then
        --先查找
        dst = self:getChildByName("ClubLayer")
        if nil ~= dst then
            dst:removeFromParent()
        end
        
        dst = ClubLayer:create(self._scene, {}, ClubLayer.SECOND_LEVEL)
        dst:setName("ClubLayer")

    elseif nTag == yl.SCENE_CLUB_ROOM then
        --先查找


        dst = self:getChildByName("ClubRoomLayer")
        if nil ~= dst then
            dst:removeFromParent()
        end
        dst = ClubRoomLayer:create(self._scene, {}, ClubRoomLayer.SECOND_LEVEL)
        dst:setName("ClubRoomLayer")

        if not ClubFrame:getInstance():isSocketServer()  then --特殊处理
            ClubFrame:getInstance():reSetAndLogin()
        end
        
    end

    if dst then
        self._scene.m_bEnableKeyBack = false
        dst:setTag(nTag)
        self:addChild(dst)
        self._scene.m_bEnableKeyBack = true
        ExternalFun.dismissTouchFilter()
    else
        print("dst is nil, change to ", nTag)
        self._scene:exitPlaza()
        return
    end
    if nTag == yl.SCENE_GAME then
        self._scene._gameFrame:setViewFrame(dst)
    end
    --游戏信息
    GlobalUserItem.bEnterGame = ( nTag == yl.SCENE_GAME )
    -- 切换结束
    self._scene:onChangeShowModeEnd(curtag, nTag)
end

-- 语音按钮
appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.game.VoiceRecorderKit")
function ClientSceneLayer:createVoiceBtn(pos, zorder, parent)
    parent = parent or self:getCurrentLayer()
    zorder = zorder or yl.ZORDER.Z_VOICE_BUTTON
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.began then
            self:startVoiceRecord()
        elseif tType == ccui.TouchEventType.ended 
            or tType == ccui.TouchEventType.canceled then
            self:stopVoiceRecord()
        end
    end
    pos = pos or cc.p(100, 100)
    local btn = ccui.Button:create("plaza/btn_voice_chat_0.png", "plaza/btn_voice_chat_1.png", "plaza/btn_voice_chat_0.png")
    btn:setPosition(pos)
    btn:setName(VOICE_BTN_NAME)
    btn:addTo(parent)
    btn:setLocalZOrder(zorder)
    btn:addTouchEventListener(btncallback)
end

function ClientSceneLayer:startVoiceRecord()
    --防作弊不聊天
    if GlobalUserItem.isAntiCheat() then
        local curLayer = self:getCurrentLayer()
        showToast(curLayer, "防作弊房间禁止聊天", 3)
        return
    end
    
    local lay = VoiceRecorderKit.createRecorderLayer(self._scene, self._scene._gameFrame)
    if nil ~= lay then
        lay:setName(VOICE_LAYER_NAME)
        local curLayer = self:getCurrentLayer()
        if nil ~= curLayer then
            curLayer:addChild(lay)
        end
    end
end

function ClientSceneLayer:stopVoiceRecord()
    local curLayer = self:getCurrentLayer()
    if nil ~= curLayer then
        local voiceLayer = curLayer:getChildByName(VOICE_LAYER_NAME)
        if nil ~= voiceLayer then
            voiceLayer:removeRecorde()
        end
    end
end

function ClientSceneLayer:cancelVoiceRecord()
    local curLayer = self:getCurrentLayer()
    if nil ~= curLayer then
        local voiceLayer = curLayer:getChildByName(VOICE_LAYER_NAME)
        if nil ~= voiceLayer then
            voiceLayer:cancelVoiceRecord()
        end
    end
end

-- 游戏喇叭
function ClientSceneLayer:onAddGameTrumpet( item )
    
end

function ClientSceneLayer:onGameTrumpet( msg )
    if nil ~= self.m_spGameTrumpetBg then
        local str = self.m_gameTrumpetList[1]
        table.remove(self.m_gameTrumpetList,1)      
        local text = self.m_spGameTrumpetBg.trumpetText                     
        if nil ~= text then
            text:setString(str)
            text:setPosition(cc.p(700, 0))
            text:stopAllActions()
            local tmpWidth = text:getContentSize().width
            text:runAction(cc.Sequence:create(cc.MoveTo:create(16 + (tmpWidth / 172),cc.p(0-text:getContentSize().width,0)),cc.CallFunc:create(function()
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

return ClientSceneLayer