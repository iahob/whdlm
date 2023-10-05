
local ActionMgr = {}

function ActionMgr:createOpActions(parent, opType, x, y, z)
    local opType = opType or 1
    local px,py = parent:getPosition()

    local opRes = {
        [0x08] = "game/yule/sparrowxykwx/res/ops/kwx_btn_peng.png",
        [0x10] = "game/yule/sparrowxykwx/res/ops/kwx_btn_gang.png",
        [0x20] = "game/yule/sparrowxykwx/res/ops/kwx_btn_ting.png",
        [0x40] = "game/yule/sparrowxykwx/res/ops/kwx_btn_hu.png",
    }

    local x = x or display.width*0.5
    local y = y or display.height*0.5
    print("x,y==", px, py, x, y)
    local path = opRes[opType]
    local density = 5
    for i=1,density do
        local sp = cc.Sprite:create(path)
        sp:setPosition(x, y-100+30)

        local rand = 0
        if i ~= 1 and i ~= density then
            rand = math.random(-30,-90)
            sp:setOpacity(255-255*(i*1)*0.1)
            sp:setRotation(rand)
        end
        sp:setVisible(false)

        sp:setScale(1)

        local moveDown = cc.MoveTo:create(0.2, cc.p(x, y-100) )
        local scaleLarger = cc.ScaleTo:create(0.2, 1.6)
        local spawn_1 = cc.Spawn:create(moveDown, scaleLarger)
        local ease_1 = cc.EaseIn:create(spawn_1, 1)

        local moveUp = cc.MoveTo:create(0.2, cc.p(x, y))
        local scaleSmall = cc.ScaleTo:create(0.2, 1)
        local spawn_2 = cc.Spawn:create(moveUp, scaleSmall)
        local ease_2 = cc.EaseOut:create(spawn_2, 1)

        local seq = cc.Sequence:create(cc.Show:create(), cc.DelayTime:create((i-1)*0.09), ease_1, ease_2, cc.RemoveSelf:create(true))
        sp:runAction(seq)
        parent:addChild(sp, z or 1)
    end
end

return ActionMgr
