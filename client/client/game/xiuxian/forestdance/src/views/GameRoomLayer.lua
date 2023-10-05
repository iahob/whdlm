--
-- Author: zhong
-- Date: 2016-10-12 15:22:32
--
local RoomLayerModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameRoomLayerModel")
local GameRoomLayer = class("GameRoomLayer", RoomLayerModel)

function GameRoomListLayer:ctor(scene, frameEngine, isQuickStart)
	GameRoomListLayer.super.ctor(self, scene, isQuickStart)
	self._frameEngine = frameEngine
end

function GameRoomListLayer:onEnterRoom( frameEngine )
	print("自定义房间进入")
	if nil ~= frameEngine and frameEngine:SitDown(yl.INVALID_TABLE,yl.INVALID_CHAIR) then
        return true
	end
end

--获取开始坐下默认坐下位置
function GameRoomListLayer.getDefaultSit()
	return yl.INVALID_TABLE,yl.INVALID_CHAIR
end
return GameRoomLayer