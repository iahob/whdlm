cc = cc or {}

--模拟参考Cocos Creator用法
function cc.find(parent, sign)
    if string.sub(sign, 1, 1) == "/" then
        error("invalid signature!")
    end

    if string.sub(sign, -1) ~= "/" then
        sign = sign .. "/"
    end

    local nextNode = parent
    for m in sign:gmatch("(.-)/") do
        nextNode = nextNode:getChildByName(m)
        if nextNode == nil then
            print("node not found", m)
            error("can not find " .. sign or type(sign))
            return
        end
    end
    return nextNode
end

function cc.performWithDelay(node, callback, delay)
    local action = cc.Sequence:create(
        cc.DelayTime:create(delay),
        cc.CallFunc:create(callback)
    )
    node:runAction(action)
    return action
end

function cc.funAddTouchEventListener(parent, sign, handle)
    local widget = cc.find(parent, sign)
    widget:addTouchEventListener(function(sender, eventType)
        handle(sender, eventType)
    end)
end

function cc.funAddClickEventListener(sender, handle)
    sender:addTouchEventListener(function(sender, eventType)
        if eventType == ccui.TouchEventType.ended then
            sender:setTouchEnabled(false)
            handle(sender)
            cc.performWithDelay(sender, function()
                sender:setTouchEnabled(true)
            end, 0.1)
        end
    end)
end

function cc.funAddChildListener(parent, sign, func)
    local widget = cc.find(parent, sign)
    if widget then
        cc.funAddClickEventListener(widget, func)
        return widget
    end
end

function string.clip(str, max_count)
    local _len = 0
    local _end = 0
    local _attach = ""
    local _str_len = string.len(str)
    if _str_len > max_count then
        _len=2
        _attach = ".."
    end

    for c in string.gmatch(str, "[%z\1-\127\192-\247][\128-\191]*") do
        local cur_len = 1
        local cur_bytes = 1
        local bytes = string.len(c)
        if bytes >= 2 then
            cur_len = 2
            cur_bytes = bytes
        end
        if _len+cur_len > max_count then break; end
        _len = _len + cur_len
        _end = _end + cur_bytes
    end
    return string.sub(str,1,_end) .. _attach
end