--
-- Author: David
-- Date: 2017-3-16
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")

local popPosition = {cc.p(177, 258-100), cc.p(177, 390-100), cc.p(177, 527-120),
                     cc.p(8, 400),cc.p(366-217, 400),cc.p(450, 400),cc.p(946-217, 400),cc.p(1069-237, 400),
                     cc.p(1259-550, 527-120),cc.p(1259-550, 390-120),cc.p(1259-550, 258-120),cc.p(50, 50)
                    }
local popAnchor = {
                    cc.p(0, 0), cc.p(0, 0),cc.p(0, 0),
                    cc.p(0.5, 1.0),cc.p(0.5, 1.0),cc.p(0.5, 1.0),cc.p(0.5, 1.0),cc.p(0.5, 1.0),
                    cc.p(1.0, 0.5),cc.p(1.0, 0.5),cc.p(0, 0)
                } 
local posChatView = {cc.p(60, -120), cc.p(60, 120), cc.p(60, 120), cc.p(-60, 120),cc.p(60, -120),cc.p(60, -120)}
local posText = {cc.p(150, 80), cc.p(-150, 75), cc.p(150, 90), cc.p(-110, -136),}
-- local HeadSprite = require(appdf.EXTERNAL_SRC .. "HeadSprite")

local PlayerInfo = class("PlayerInfo", cc.Node)

function PlayerInfo:ctor(useritem, viewId)


    -- 加载csb资源
    -- self.csbNode = ExternalFun.loadCSB("ui/GameRoleItem.csb",self)
    local rootLayer, csbNode = ExternalFun.loadRootCSB("ui/GameRoleItem.csb", self)
    self.csbNode = csbNode
    self.useritem = clone(useritem)

    local panel_1 = csbNode:getChildByName("Node_1")
    local panel_2 = csbNode:getChildByName("Node_2")
    local panel_3 = csbNode:getChildByName("Node_3")
    panel_1:setVisible(false)
    panel_2:setVisible(false)
    panel_3:setVisible(false)
    self.panel = panel_1
    if viewId == 6 then
        self.panel = panel_2
    elseif viewId == 12 then
        self.panel = panel_3
    end
    self.panel:setVisible(true)

    self.avBg = self.panel:getChildByName("av")
    self.name = self.panel:getChildByName("name")
    self.money = self.panel:getChildByName("money")

    self.name:setString(self.useritem.szNickName)
    local headUseritem = clone(self.useritem)
    
    if GlobalUserItem.bPrivateRoom then 
        local score = self.useritem.lScore - PriRoom:getInstance().m_tabPriData.lIniScore
        self.money:setString(score)
        headUseritem.lScore = score
    else
        local score = ExternalFun.formatScoreText(self.useritem.lScore)
        self.money:setString(score)
    end
    


    self.ready = cc.Sprite:create("public/ready.png")
    self:addChild(self.ready)
    self.ready:setVisible(false)
    -- self.ready:setPosition(600,0)

    if viewId <= 4 and viewId > 0 then 
        self.ready:setPosition(110,0)
    elseif viewId >= 5 and viewId <= 8 then
        self.ready:setPosition(40,-35)
    elseif viewId >= 8 then
        self.ready:setPosition(-45,40)
    end
    self.viewId = viewId
    self:createHead(headUseritem)


    self.qipao1 = csbNode:getChildByName("Image_1")
    self.qipao2 = csbNode:getChildByName("Image_2")

    self.qipao1:setVisible(false)
    self.qipao2:setVisible(false)
    
    self.qipao = self.qipao2
    if viewId > 8 then 
        self.qipao = self.qipao1 
    end
end

function PlayerInfo:createHead(useritem)
    local clippingfile = "avatar/avatar_down.png"
    local bgPath = "avatar/avatar_down.png"
    local size = 90
    if self.viewId == 6 then 
        clippingfile = "avatar/banker_size.png" 
        bgPath = "avatar/banker_gold.png"
        size = 92
    end

    if self.viewId == 12 then 
        clippingfile = "avatar/head_kuang.png" 
        bgPath = "avatar/head_kuang.png"
        size = 90
    end

    local av = PopupInfoHead:createClipHead( useritem, size, clippingfile )
    self.avBg:addChild(av)
    av:enableInfoPop(true, popPosition[self.viewId], popAnchor[self.viewId])
    -- av:setPopCsbFile("ui/GamePopInfo.csb")

    local param = {}
    param.isProp = true
    av:setPropInfo(param)


    local btn = self.csbNode:getChildByName("Button")
    btn:addClickEventListener(function ()
        -- if self.viewId == 12 then return end
        av:onTouchHead()
    end)

    -- av:setPosition(cc.p(40,40))
    -- av:setName("head")
end

function PlayerInfo:showBanker(pShow)
    if self.viewId ~= 6 then 
        return
    end

    local sp = self:getChildByName("banker_icon")
    if sp then sp:removeFromParent() end

    if pShow  then 
     sp = cc.Sprite:create("avatar/banker_icon.png")
     self:addChild(sp)
     sp:setPosition(15.42,77.35)
     sp:setName("banker_icon")
    end
end

function PlayerInfo:onExit()

end

function PlayerInfo:reSet()
    self.m_popHead:setVisible(true)
end

function PlayerInfo:setBankerWaitStatus( isShow,isNormal,viewIndex)

end
--庄家
function PlayerInfo:showBank( isBanker)
    --庄家图标

end
--房主图标
function PlayerInfo:showRoomHolder( isRoomHolder )

end


function PlayerInfo:browChat( idx )
    -- if nil ~= self.m_labChat then
    --     self.m_labChat:setVisible(false)
    -- end
    -- --self:changeChatPos()

    -- self.m_spChatBg:setVisible(false)

    print("browChat--------------------->", idx)
    self.csbNode:removeChildByName("EMOJ_CHAT")
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("chat/emoj/%d.plist", idx))
    --缓存表情
    --动画


    --加载动画
    local frames = {}
    local actionTime = 0.2
    for i=1,32 do
        local strName = string.format("emoj%d_%d.png",idx, i)
        print("browChatSprName--------------------->", strName)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
        if spriteFrame then
            table.insert(frames, spriteFrame)
        else
            break
        end
    end
    local  animation = cc.Animation:createWithSpriteFrames(frames,actionTime)    
    local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", idx))
    local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
    self.csbNode:addChild(spr)
    spr:setName("EMOJ_CHAT")

    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        print("removeFromParent!!!!!!!!!!!!!!!!!")
        
       self.csbNode:removeChildByName("EMOJ_CHAT")
    end)))


end



return PlayerInfo