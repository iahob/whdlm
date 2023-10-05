-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104

function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    -- local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    -- self.m_rootLayer = rootLayer
end


function PriGameLayer:onButtonClickedEvent( tag, sender )
 
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
   
end

function PriGameLayer:onRefreshInviteBtn()
   
end

function PriGameLayer:updateReloadCardType()
    
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )--
    GlobalUserItem.bWaitQuit = true;

    local query = QueryDialog:create("游戏房间已解散！", function(ok)
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom();
    end)
    :setCanTouchOutside(false)
    :addTo(self)
end

function PriGameLayer:onLeaveGame()
    
end

function PriGameLayer:onExit()
  
end

return PriGameLayer