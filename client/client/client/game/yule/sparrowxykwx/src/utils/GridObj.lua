--todo,模拟unity GridGroup
local GridObj = {}

function GridObj:meetCenter(fatherNode, offsetx, ...)
    local objs = {...}
    local _,obj = next(objs)
    local w = obj:getContentSize().width
    local count = #objs
    local totalWidth = count * w + (count-1) * offsetx

    local center = fatherNode:getContentSize().width * 0.5
    local startx = center - totalWidth * 0.5 + w * 0.5

    for i,obj in ipairs(objs) do
        obj:setPositionX( startx + (i-1) * (w+offsetx) )
    end
end

return GridObj