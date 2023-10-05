
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local module_pre = "game.yule.paijiu.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")


local popPosition =  {cc.p(457, 102), cc.p(800, 259), cc.p(800, 414), cc.p(457, 410),cc.p(72,414),cc.p(72,259)}

--local popPosition =  {cc.p(457, 102), cc.p(1247, 259), cc.p(1247, 544), cc.p(457, 657),cc.p(72,544),cc.p(72,259)}

-- popPosition[cmd.MY_VIEWID] = cc.p(220, 214)
-- popPosition[cmd.RB_VIEWID] = cc.p(642, 214)
-- popPosition[cmd.RT_VIEWID] = cc.p(115, 60)
-- popPosition[cmd.TOP_VIEWID] = cc.p(220, 214)
-- popPosition[cmd.LT_VIEWID] = cc.p(642, 214)
-- popPosition[cmd.LB_VIEWID] = cc.p(115, 60)
local popAnchor = {} 
popAnchor[cmd.MY_VIEWID] = cc.p(0, 0.5)
popAnchor[cmd.RB_VIEWID] = cc.p(1, 0.5)
popAnchor[cmd.RT_VIEWID] = cc.p(1, 0.5)
popAnchor[cmd.TOP_VIEWID] = cc.p(0, 0.5)
popAnchor[cmd.LT_VIEWID] = cc.p(0, 0.5)
popAnchor[cmd.LB_VIEWID] = cc.p(0, 0.5)


-- local function formatPlayerScore(score)
--     if score<1e4 then return score
--     elseif score<1e5 then return string.format("%.2f万",score/1e4)
--     elseif score<1e6 then return string.format("%.1f万",score/1e4)
--     elseif score<1e7 then return string.format("%.0f万",score/1e4)
--     elseif score<1e8 then return string.format("%.0f万",score/1e4)

--     elseif score<1e9 then return string.format("%.2f亿",score/1e8)
--     elseif score<1e10 then return string.format("%.1f亿",score/1e8)
--     elseif score<1e11 then return string.format("%.0f亿",score/1e8)
--     elseif score<1e12 then return string.format("%.0f亿",score/1e8)
--     end
-- end

local function formatPlayerScore(score)
    if score<1e4 then return score
    elseif score<1e5 then score=math.floor(score/100) return (score/100).."万"
    elseif score<1e6 then score=math.floor(score/1000) return (score/10).."万"
    elseif score<1e7 then score=math.floor(score/10000) return (score).."万"
    elseif score<1e8 then score=math.floor(score/10000) return (score).."万"

    elseif score<1e9 then score=math.floor(score/1e6)  return (score/100).."亿"
    elseif score<1e10 then score=math.floor(score/1e7) return (score/10).."亿"
    elseif score<1e11 then score=math.floor(score/1e8) return (score).."亿"
    elseif score<1e12 then score=math.floor(score/1e8) return (score).."亿"
    end
end


local GameRoleItem = class("GameRoleItem", cc.Node)

function GameRoleItem:ctor(userItem, viewId)

    if cmd.globalData.wPlayerCount==4 then
        popAnchor[cmd.MY_VIEWID] = cc.p(0, 0.5)
        popAnchor[cmd.FOUR_RIGHT_VIEWID] = cc.p(1, 0.5)
        popAnchor[cmd.FOUR_TOP_VIEWID] = cc.p(0, 0.5)
        popAnchor[cmd.FOUR_LEFT_VIEWID] = cc.p(0, 0.5)

        popPosition =   {cc.p(600, 150), cc.p(800, 412), cc.p(670, 410),cc.p(72,412)}
    end

    ExternalFun.registerNodeEvent(self)
    self.m_nViewId = viewId
    self.m_userItem = userItem
    self.m_bNormalState = (self.m_userItem.cbUserStatus ~= yl.US_OFFLINE)

    -- 加载csb资源
    local csbNode = ExternalFun.loadCSB("game/GameRoleItem.csb",self)
    self.m_csbNode = csbNode

    -- 用户头像
    local head = PopupInfoHead:createNormal(userItem, 76)
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "game/popBg.png" --弹出框背景
    head:setPropInfo(param)
   -- head:setPopCsbFile("game/GamePopInfo.csb")
    head:enableInfoPop(true, popPosition[viewId], popAnchor[viewId])
   
    self.m_popHead = head
    csbNode:addChild(head)

    self.m_csbNode:getChildByName("banker"):setLocalZOrder(1)


    -- 游戏状态
    self.m_spGameFrame = csbNode:getChildByName("head_frame")
    self.m_spGameFrame:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
            if self.m_spGameFrame:isVisible() then
                self.m_popHead:onTouchHead()
            end
        end
    end)
    self.m_spGameFrame:setTouchEnabled(true)

    -- csbNode:getChildByName("name"):setString(userItem.szNickName)
    -- csbNode:getChildByName("score"):setString(userItem.lScore)

    -- 聊天气泡
    self.m_spChatBg = csbNode:getChildByName("chat_bg")
    self.m_spChatBg:setVisible(false)
    self.m_spChatBg:setScaleX(0.00001)

    -- 聊天表情
    self.m_spBrow = self.m_spChatBg:getChildByName("chat_face")
    -- 聊天内容
    self.m_labChat = nil

    -- -- 信息底板
    -- local infobg = csbNode:getChildByName("head_bg")
    -- infobg:setVisible(false)
    -- self.m_spInfoBg = infobg

    self.m_clipNick = ClipText:createClipText(cc.size(90, 20), self.m_userItem.szNickName)
    self.m_clipNick:setAnchorPoint(cc.p(0.5, 0.5))
    local nameNode=csbNode:getChildByName("name")
    self.m_clipNick:setPosition(nameNode:getPosition()):setAnchorPoint(nameNode:getAnchorPoint())
    nameNode:removeSelf()
    self.m_csbNode:addChild(self.m_clipNick)
    

    local scoreNode=csbNode:getChildByName("score")
    self.m_clipScore =scoreNode --ClipText:createClipText(cc.size(100, 20), "")
   -- self.m_clipScore:setAnchorPoint(cc.p(0.5, 0.5))
    --self.m_clipScore:setPosition(scoreNode:getPosition()):setAnchorPoint(scoreNode:getAnchorPoint())
   -- scoreNode:removeSelf()
    --self.m_csbNode:addChild(self.m_clipScore)

    -- 聊天动画
    local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
    local show = cc.Show:create()
    local spa = cc.Spawn:create(sc, show)
    self.m_actTip = cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())
    self.m_actTip:retain()

    --语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()

    -- 房主标识
    self.m_spRoomerFlag = csbNode:getChildByName("sp_roomerflag")
    self.m_spRoomerFlag:setVisible(false):setLocalZOrder(1)

    self:updateStatus()
    self:updateScore(self.m_userItem.lScore)
end

function GameRoleItem:onExit()
    self.m_actTip:release()
    self.m_actTip = nil
    self.m_actVoiceAni:release()
    self.m_actVoiceAni = nil
end

function GameRoleItem:reSet()
    -- self.m_spInfoBg:setVisible(false)
    -- self.m_spLandFlag:setVisible(false)
    -- self.m_spRoomerFlag:setVisible(false)
end

function GameRoleItem:hideBankerFlag()
    self.m_csbNode:getChildByName("banker"):setVisible(false)
end

function GameRoleItem:switeGameState( isBanker )
   
end

function GameRoleItem:setRoomerState( isRoomer )
    self.m_spRoomerFlag:setVisible(isRoomer)
    if GlobalUserItem.bPrivateGoldRoom then
        self.m_spRoomerFlag:setVisible(false)
    end
end

function GameRoleItem:setBankerState( bBanker )
    self.m_csbNode:getChildByName("banker"):setVisible(bBanker)
    if bBanker then
        local bRight=self:getParent():getDir(self.m_nViewId)
        if bRight then
            self.m_csbNode:getChildByName("banker"):setPositionX(67) 
        else
            self.m_csbNode:getChildByName("banker"):setPositionX(-67) 
        end
    end
end


function GameRoleItem:textChat( str )
    self.m_spBrow:setVisible(false)
     --移除表情
    self.m_csbNode:removeChildByName("EMOJ_CHAT")

    if nil == self.m_labChat then
        self.m_labChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)        
        self.m_spChatBg:addChild(self.m_labChat)
    else
        self.m_labChat:setString(str)
    end
    self:changeChatPos()
    self.m_labChat:setVisible(true)
    -- 尺寸调整
    local labSize = self.m_labChat:getContentSize()
    if labSize.height >= 40 then
        self.m_spChatBg:setContentSize(170, labSize.height + 30)
    else
        self.m_spChatBg:setContentSize(170, 40)
    end
    self.m_labChat:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5)
end

function GameRoleItem:changeChatPos()
    local bRight=self:getParent():getDir(self.m_nViewId)
    if bRight then
        self.m_spChatBg:setAnchorPoint(cc.p(1.0, 0.5))
        self.m_spChatBg:loadTexture("game/game_chat_1.png")
        self.m_spChatBg:setPosition(-81, 20)
    else
        self.m_spChatBg:setAnchorPoint(cc.p(0, 0.5))
        self.m_spChatBg:loadTexture("game/game_chat_0.png")
        self.m_spChatBg:setPosition(81, 20)
    end
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:runAction(self.m_actTip)
end



-- function GameRoleItem:browChat( idx )
--     if nil ~= self.m_labChat then
--         self.m_labChat:setVisible(false)
--     end
--     --self:changeChatPos()
--     self.m_spBrow:setVisible(false)
--     self.m_spChatBg:setContentSize(170, 40)
--     self.m_spChatBg:setVisible(false)

--     -- local str = string.format("e(%d).png", idx)
--     -- local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
--     -- if nil ~= frame then
--     --     self.m_spBrow:setSpriteFrame(frame)
--     -- end
--     print("browChat--------------------->", idx)
--     self.m_csbNode:removeChildByName("EMOJ_CHAT")
--     cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))
--     --缓存表情
--     --动画
--     local animation = cc.Animation:create()
--     local times = 0.2
--     for i=1, 12 do
--         local strName = string.format("emoj%d_%d.png",idx, i)
--          print("browChatSprName--------------------->", strName)
--         local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
--         if spriteFrame then
--             animation:addSpriteFrame(spriteFrame)
--         else
--             break
--         end
--         animation:setLoops(-1)
--         animation:setDelayPerUnit(times)
--     end

--     local animate = cc.Animate:create(animation)
--     local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("emoj%d_1.png", idx))
--     local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
--     self.m_csbNode:addChild(spr)
--     spr:setName("EMOJ_CHAT")
--     spr:runAction(cc.Spawn:create(animate, cc.Sequence:create(cc.DelayTime:create(2.5), cc.CallFunc:create(function()
--         print("removeFromParent!!!!!!!!!!!!!!!!!")
--         spr:removeFromParent()
--     end))))
-- end

function GameRoleItem:browChat( idx )
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end

    self.m_spChatBg:setVisible(false)

    print("browChat--------------------->", idx)
    self.m_csbNode:removeChildByName("EMOJ_CHAT")
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))

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
    self.m_csbNode:addChild(spr)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.m_csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        print("removeFromParent!!!!!!!!!!!!!!!!!")
        if spr and not tolua.isnull(spr) then
            spr:removeFromParent()
        end
    end)))
end


function GameRoleItem:onUserVoiceStart()
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self.m_spBrow:stopAllActions()
    self:changeChatPos()  
    self.m_spChatBg:setContentSize(170, 40) 
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:setVisible(true)
    self.m_spChatBg:setScale(1.0)
   -- local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("land_blank.png")
   -- if nil ~= frame then
        local bRight=self:getParent():getDir(self.m_nViewId)
        if bRight then 
            self.m_spBrow:setRotation(0)
        else
            self.m_spBrow:setRotation(180)
        end        
        self.m_spBrow:setVisible(true)
      --  self.m_spBrow:setSpriteFrame(frame)
        self.m_spBrow:runAction(self.m_actVoiceAni)
    --end
end

function GameRoleItem:onUserVoiceEnded()
    self.m_spBrow:setRotation(0)
    self.m_spBrow:stopAllActions()
    self.m_spBrow:setVisible(false)
    self.m_spChatBg:setVisible(false)
    self.m_spChatBg:setScaleX(0.00001)
end


function GameRoleItem:updateStatus()
    if self.m_userItem.cbUserStatus == yl.US_OFFLINE then

        if GlobalUserItem.bPrivateGoldRoom then
            return
        end

        self.m_bNormalState = false
        if nil ~= convertToGraySprite then
            -- 灰度图
            if nil ~= self.m_popHead and nil ~= self.m_popHead.m_head and nil ~= self.m_popHead.m_head.m_spRender then
                convertToGraySprite(self.m_popHead.m_head.m_spRender)
            end
        end        
    else
        if not self.m_bNormalState then
            self.m_bNormalState = true
            -- 普通图
            if nil ~= convertToNormalSprite then
                -- 灰度图
                if nil ~= self.m_popHead and nil ~= self.m_popHead.m_head and nil ~= self.m_popHead.m_head.m_spRender then
                    convertToNormalSprite(self.m_popHead.m_head.m_spRender)
                end
            end
        end
    end
end

function GameRoleItem:updateRole( userItem )
    self.m_userItem = userItem
    self:updateStatus()

    -- 昵称
    if nil ~= self.m_clipNick then
        self.m_clipNick:setString(self.m_userItem.szNickName)
    end

    self:updateScore(self.m_userItem.lScore)

    -- 头像
    self.m_popHead:updateHead(userItem)
end

-- 更新分数
function GameRoleItem:updateScore( lScore )
    lScore = lScore or 0
    -- 积分场
    print("GlobalUserItem.bVideo:",GlobalUserItem.bVideo)
    print("GlobalUserItem.bPrivateRoom:",GlobalUserItem.bPrivateRoom)
    print("PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore:",PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore)
    
    if GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
        local iniscore = PriRoom:getInstance().m_tabPriData.lIniScore or 0
        lScore = lScore - iniscore
    end

    if  GlobalUserItem.bVideo then
        lScore = ""
    else
        lScore=formatPlayerScore(lScore)
    end

    -- 游戏币
    if nil ~= self.m_clipScore then
        self.m_clipScore:setString(lScore .. "")
    end
end

return GameRoleItem