
--
-- 单游戏模式更新场景
local ForestDanceScene = class("ForestDanceScene", cc.load("mvc").ViewBase)
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local PopWait = appdf.req(appdf.BASE_SRC.."app.views.layer.other.PopWait")
function ForestDanceScene:onCreate()

    assert(self:getApp()._gameFrameEngine)
    self._gameFrame = self:getApp()._gameFrameEngine
    self._gameFrame:setViewFrame(self)
    self._gameFrame:setCallBack(function(code,result)
       self:onRoomCallBack(code,result)
    end)


    if PriRoom then
        PriRoom:getInstance():onEnterPlaza(self, self._gameFrame)
    end
    if MatchRoom then
        MatchRoom:getInstance():onEnterPlaza(self, self._gameFrame)
    end
end

function ForestDanceScene:onRoomCallBack(code,message)
    print("onRoomCallBack:"..code)
    if message then
        showToast(self,message,1)
    end
    if code == -1  then
        self:dismissPopWait()
    end
end
--显示等待
function ForestDanceScene:showPopWait(isTransparent)
    if not self._popWait then
        self._popWait = PopWait:create(isTransparent)
            :show(self,"请稍候！")
        self._popWait:setLocalZOrder(yl.MAX_INT)
    end
end

--关闭等待
function ForestDanceScene:dismissPopWait()
    if self._popWait then
        self._popWait:dismiss()
        self._popWait = nil
    end
end

function ForestDanceScene:getEnterGameInfo(  )
    return GlobalUserItem.m_tabEnterGame
end

function ForestDanceScene:onEnterRoom()
    --自定义房间界面处理登陆成功消息
    local entergame = self:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
        local customRoomFile = ""
        if cc.PLATFORM_OS_WINDOWS == targetPlatform then
            customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua" .. yl.WIN_SRC_SUFFIX
        else
            customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua"
        end
            print("customRoomFile is ....."..customRoomFile)
         
        if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
            if (appdf.req(customRoomFile):onEnterRoom(self._gameFrame)) then
                self:showPopWait()
                return
            else
              
            end
        end
    end    
end

function ForestDanceScene:onBackgroundCallBack(bEnter)
    if not bEnter then
        print("application did enter background")
        if nil ~= self._gameFrame and self._gameFrame:isSocketServer() and GlobalUserItem.bAutoConnect then         
            self._gameFrame:onCloseSocket()
        end

        --关闭好友服务器
        FriendMgr:getInstance():reSetAndDisconnect()

        self:dismissPopWait()
    else
        print("application will enter forceGround")
                   
        if self._gameFrame:isSocketServer() == false and GlobalUserItem.bAutoConnect then
            self._gameFrame:OnResetGameEngine()
            self:onStartGame()
        end
        
        --连接好友服务器
        FriendMgr:getInstance():reSetAndLogin()

        --查询财富
        if GlobalUserItem.bJftPay then
            --通知查询     
            local eventListener = cc.EventCustom:new(yl.RY_JFTPAY_NOTIFY)
            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
        end
    end
end

--启动游戏
function ForestDanceScene:onStartGame()
    local app = self:getApp()
    local entergame = self:getEnterGameInfo()
    assert(entergame)
    
    self:showPopWait()
    self._gameFrame:onInitData()
    self._gameFrame:setKindInfo(GlobalUserItem.nCurGameKind, entergame._KindVersion)
    self._gameFrame:setViewFrame(self)
    self._gameFrame:onCloseSocket()
    self._gameFrame:onLogonRoom()
end

function ForestDanceScene:onEnterTable()
    print("ForestDanceScene onEnterTable")
    local entergame = self:getEnterGameInfo()
    assert(entergame)

    local modulestr = entergame._KindName
    local gameScene = appdf.req(appdf.GAME_SRC.. modulestr .. "src.views.GameLayer")
    
    if gameScene and not self._gameLayer then
        self._gameLayer = gameScene:create(self._gameFrame,self)

        --返回键事件
        self._gameLayer:registerScriptKeypadHandler(function(event)
        if event == "backClicked" then
             if self._popWait == nil then 
                if self._gameLayer  and self._gameLayer.onKeyBack then
                    if self._gameLayer:onKeyBack() == true then
                        return
                    end
                end
                
                this:onKeyBack()
            end
        end
    end)

      self._gameLayer:setKeyboardEnabled(true)
      self:addChild(self._gameLayer)               
    end   

    self._gameFrame:setViewFrame(self._gameLayer)
end
--显示等待
function ForestDanceScene:showPopWait(isTransparent)
    if not self._popWait then
        self._popWait = PopWait:create(isTransparent)
            :show(self,"请稍候！")
        self._popWait:setLocalZOrder(yl.MAX_INT)
    end
end

--关闭等待
function ForestDanceScene:dismissPopWait()
    if self._popWait then
        self._popWait:dismiss()
        self._popWait = nil
    end
end

-- 进入场景而且过渡动画结束时候触发。
function ForestDanceScene:onEnterTransitionFinish()

    self:onStartGame()
    setbackgroundcallback(function (bEnter)
        if type(self.onBackgroundCallBack) == "function" then
            self:onBackgroundCallBack(bEnter)
        end
    end)
end

function ForestDanceScene:onExit()
   
end

function ForestDanceScene:onKeyBack()
    self._gameFrame:onCloseSocket()

    --进入游戏列表
    self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
    FriendMgr:getInstance():reSetAndLogin()
end

return ForestDanceScene