--by hhp,临时写的,用来应付创房ui
local SelectGroupObj = {}
SelectGroupObj.__index = SelectGroupObj

--模拟unity的选择组概念
--增加选择组列表,默认选择某一个
function SelectGroupObj:addCheckBox(which_select_default, callbackHit, ...)
    local selectGroupList = {...}
    setmetatable(selectGroupList, self)

    for idx,obj in ipairs(selectGroupList) do
        -- self:setChildClickAble(obj, selectGroupList, callbackHit)
        obj:setTag(idx)
        obj:setSelected(idx == which_select_default)
        obj:addEventListener(function(sender, etype)
            local tag = sender:getTag()
            local isSelectSome = false

            for _,chbox in ipairs(selectGroupList) do
                if chbox:getTag() == tag then
                    if not chbox:isSelected() then
                        chbox:setSelected(true)
                    end
                    isSelectSome = true
                else
                    if chbox:isSelected() then
                        chbox:setSelected(false)
                    end
                end

                chbox:setSelected(tag == chbox:getTag())
            end

            if isSelectSome and type(callbackHit) == "function" then
                callbackHit(tag)
            end
        end)
    end

    return selectGroupList
end

function SelectGroupObj:notify(tag)
    for _,chbox in ipairs(self) do
        if chbox:getTag() == tag then
            if not chbox:isSelected() then
                chbox:setSelected(true)
            end
        else
            if chbox:isSelected() then
                chbox:setSelected(false)
            end
        end
    end
end

function SelectGroupObj:setChildClickAble(chbox, group, callbackHit)
    local children = chbox:getChildren()
    for k,v in pairs(children) do
        v:setTouchEnabled(true)
        v:addTouchEventListener(function(sender, event)
            if event == 2 then
                local tag = sender:getParent():getTag()
                group:notify(tag)
                if type(callbackHit) == "function" then
                    callbackHit(tag)
                end
            end
        end)
    end
end

--获取选择组的结果
function SelectGroupObj:getSelectIdx()
    for k,v in pairs(self) do
        if v.isSelected and v:isSelected() then
            return v:getTag()
        end
    end
    -- assert(false, "选择组没有选中任何抉择!")
    print("选择组没有选中任何选项")
    return nil
end

return SelectGroupObj