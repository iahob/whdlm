local sp_EventType =
{
    ANIMATION_START = 0,
    ANIMATION_INTERRUPT = 1,
    ANIMATION_END = 2,
    ANIMATION_COMPLETE = 3,
    ANIMATION_DISPOSE = 4,
    ANIMATION_EVENT = 5,
}

local SpineObj = {}

function SpineObj:create(jsonFile, atlasFile, isloop)
    local skeletonNode = sp.SkeletonAnimation:create(jsonFile, atlasFile)
    skeletonNode:setAnimation(0, "animation", isloop and true)

    skeletonNode:registerSpineEventHandler(function (event)
            -- print(string.format("[spine] %d start:", event.trackIndex))
    end, sp_EventType.ANIMATION_START)

    skeletonNode:registerSpineEventHandler(function (event)
            -- print(string.format("[spine] %d end:", event.trackIndex))
    end, sp_EventType.ANIMATION_END)

    skeletonNode:registerSpineEventHandler(function (event)
            -- print(string.format("[spine] %d complete:", event.trackIndex))
            if self._completeCallback then self._completeCallback() end

    end, sp_EventType.ANIMATION_COMPLETE)

    skeletonNode:registerSpineEventHandler(function (event)
        --[[
        print(string.format("[spine] %d event: %s, %d, %f, %s",
                              event.trackIndex,
                              event.eventData.name,
                              event.eventData.intValue,
                              event.eventData.floatValue,
                              event.eventData.stringValue))
]]
        if self._eventCallback then self._eventCallback() end

    end, sp_EventType.ANIMATION_EVENT)

    local peer = tolua.getpeer(skeletonNode)
    if not peer then
        peer = {}
        tolua.setpeer(skeletonNode, peer)
    end
    self.__index = self
    setmetatable(peer, self)
    return skeletonNode
end

function SpineObj:setCompleteCallback(completeCallback)
    assert(type(completeCallback)=="function", "Invalid completeCallback!!!")
    self._completeCallback = completeCallback
end

function SpineObj:setEventCallback(eventCallback)
    assert(type(eventCallback)=="function", "Invalid eventCallback!!!")
    self._eventCallback = eventCallback
end

function SpineObj:destroy()
    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.01), cc.RemoveSelf:create(true)))
end

return SpineObj