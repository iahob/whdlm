--
-- Author: zhong
-- Date: 2016-11-04 11:36:24
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local module_pre = "game.qipai.land.src"
local Define = appdf.req(module_pre .. ".models.Define")

local module_pre = "game.qipai.land.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local popPosition = {}
popPosition[cmd.LEFT_VIEWID] = cc.p(310, 510)
popPosition[cmd.RIGHT_VIEWID] = cc.p(1015, 510)
popPosition[cmd.MY_VIEWID] = cc.p(200, 200)

local popAnchor = {} 
popAnchor[cmd.LEFT_VIEWID] = cc.p(0, 0.5)
popAnchor[cmd.RIGHT_VIEWID] = cc.p(1, 0.5)
popAnchor[cmd.MY_VIEWID] = cc.p(0, 0.5)

local GameRoleItem = class("GameRoleItem", cc.Node)

function GameRoleItem:ctor(userItem, viewId, parent)
    self._gameView = parent
    ExternalFun.registerNodeEvent(self)
    self.m_nViewId = viewId
    self.m_userItem = userItem
    self.m_bNormalState = (self.m_userItem.cbUserStatus ~= yl.US_OFFLINE)

    -- 加载csb资源
    local csbNode = ExternalFun.loadCSB("game/GameRoleItem.csb",self)
    self.m_csbNode = csbNode

    -- 用户头像
    local head = PopupInfoHead:createNormal(userItem, 82)
    head:enableInfoPop(true, popPosition[viewId], popAnchor[viewId])
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "popInfo/popBg.png" --弹出框背景
    head:setPropInfo(param)
    head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB

    local anchorPointHead = {cc.p(0, 1), cc.p(0, 0), cc.p(1, 1),cc.p(1, 1)}
    local posHead = {cc.p(193, 300), cc.p(167, 57), cc.p(650, 300),cc.p(700,350)}
    head:enableInfoPop(true,posHead[viewId] , anchorPointHead[viewId])
    --head:enableHeadFrame(true, {_framefile = "land_headframe.png", _zorder = -1, _scaleRate = 0.75, _posPer = cc.p(0.5, 0.63)})
    self.m_popHead = head
    csbNode:addChild(head, -1)
    head:setVisible(false)

    --金币房卡场
    if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        csbNode:getChildByName("head_bg"):loadTexture("game/priRoombg.png")
    end

    --比赛场没有金币图标
    if MatchRoom and GlobalUserItem.bMatch then
        csbNode:getChildByName("head_bg"):loadTexture("game/priRoombg.png")
    end

    -- 游戏状态
    self.m_spGameFrame = csbNode:getChildByName("head_bg")
    self.m_spGameFrame:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
            -- self:createPlayerInfo()
            if  not self._gameView._scene.m_bLookOn then 
                head:onTouchHead()
            else
                showToast(self._gameView._scene, "观战中不可使用该功能!", 2)
            end
        end
    end)

if viewId == cmd.MY_VIEWID then
    self._gameView.playerNode:getChildByName("bg"):addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
            if  not self._gameView._scene.m_bLookOn then 
                head:onTouchHead()
            else
                showToast(self._gameView._scene, "观战中不可使用该功能!", 2)
            end
        end
    end)
end

    -- if viewId == cmd.MY_VIEWID then
    --     self._gameView.playerNode:getChildByName("Text_name"):setString(userItem.szNickName)
    --     self._gameView.playerNode:getChildByName("Text_gold"):setString(userItem.szNickName)
    -- end

    -- 聊天气泡
    self.m_spChatBg = csbNode:getChildByName("chat_bg")
    self.m_spChatBg:setVisible(false)
    self.m_spChatBg:setScaleX(0.00001)


    -- 聊天内容
    self.m_labChat = nil

    -- 信息底板
    -- local infobg = csbNode:getChildByName("head_bg")
    --infobg:setVisible(false)
    -- self.m_spInfoBg = infobg

    -- 聊天表情
    self.m_spBrow = self.m_spChatBg:getChildByName("chat_face")

    self.m_clipNick = nil
    self.m_clipScore = nil

    -- 聊天动画
    local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
    local show = cc.Show:create()
    local spa = cc.Spawn:create(sc, show)
    self.m_actTip = cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())
    self.m_actTip:retain()

    -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = Define.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()

    -- 地主标识
    self.m_spLandFlag = csbNode:getChildByName("sp_landflag")
    self.m_spLandFlag:setVisible(false)

    -- 房主标识
    self.m_spRoomerFlag = csbNode:getChildByName("sp_roomerflag")
    self.m_spRoomerFlag:setVisible(false)

    -- if cmd.RIGHT_VIEWID == self.m_nViewId then
        -- local tmpLanderX = self.m_spLandFlag:getPositionX()
        -- self.m_spLandFlag:setPositionX(self.m_spRoomerFlag:getPositionX())
        -- self.m_spRoomerFlag:setPositionX(tmpLanderX)
    -- end

    if cmd.MY_VIEWID == self.m_nViewId then
        csbNode:getChildByName("head_bg"):setVisible(false)
        csbNode:getChildByName("Text_name"):setPosition(cc.p(20, -276))
        csbNode:getChildByName("Text_gold"):setPosition(cc.p(132, -276))
        csbNode:getChildByName("Text_gold"):setPosition(cc.p(132, -276))
    end

    local name =  string.EllipsisByConfig(self.m_userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
    --名字
    self.m_clipNick = csbNode:getChildByName("Text_name"):setString(name)

    local lScore = self.m_userItem.lScore or 0
    -- 积分场
    if true == GlobalUserItem.bVideo then
        lScore = ""
    elseif GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
        local iniscore = PriRoom:getInstance().m_tabPriData.lIniScore or 0
        lScore = lScore - iniscore
    end

    -- 游戏币
    self.m_clipScore = csbNode:getChildByName("Text_gold"):setString(lScore)
end

function GameRoleItem:createPlayerInfo()
    local csbNode = ExternalFun.loadCSB("game/GamePopInfo.csb",self._gameView)
    local bg = csbNode:getChildByName("bg_kuang")
    local sign = self.m_userItem.szSign or "此人很懒，没有签名"
    bg:setPosition(popPosition[self.m_nViewId])
    
    csbNode:getChildByName("mask"):addClickEventListener(function(sender) 
                                                            csbNode:removeFromParent()
                                                         end)
    bg:getChildByName("Text_name"):setString(string.EllipsisByConfig(self.m_userItem.szNickName, 180,string.getConfig("fonts/round_body.ttf" , 28) ))
    bg:getChildByName("Text_sign"):setString(sign)
end

function GameRoleItem:onExit()
    self.m_actTip:release()
    self.m_actTip = nil
    self.m_actVoiceAni:release()
    self.m_actVoiceAni = nil
end

function GameRoleItem:reSet()
    self.m_spLandFlag:setVisible(false)
    self.m_spRoomerFlag:setVisible(false)
end

function GameRoleItem:switeGameState( isBanker )
        local lScore = self.m_userItem.lScore or 0
        -- 积分场
        if true == GlobalUserItem.bVideo then
            lScore = ""
        elseif GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
            local iniscore = PriRoom:getInstance().m_tabPriData.lIniScore or 0
            lScore = lScore - iniscore
        end

        -- 游戏币
        self.m_clipScore:setString(lScore .. "")

    -- self.m_spLandFlag:setVisible(isBanker)

    self:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function()
        --self:updateStatus()
    end)))
end

function GameRoleItem:switeRoomerState( isRoomer )
    print("switeRoomerState", isRoomer, GlobalUserItem.bPrivateGoldRoom)
    self.m_spRoomerFlag:setVisible(isRoomer)
    self.m_spRoomerFlag:setVisible(false)
    if GlobalUserItem.bPrivateGoldRoom then
        self.m_spRoomerFlag:setVisible(false)
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
        -- self.m_spChatBg:setContentSize(170, labSize.height + 30)
        self.m_spChatBg:setContentSize(207, 103)
    else
        -- self.m_spChatBg:setContentSize(170, 40)
        self.m_spChatBg:setContentSize(207, 103)
    end
    self.m_labChat:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5)
end

function GameRoleItem:browChat( idx )
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end

    self.m_spBrow:setVisible(false)
    self.m_spChatBg:setContentSize(207, 103)
    self.m_spChatBg:setVisible(false)


    --self:changeChatPos()

    print("browChat--------------------->", idx)
    self.m_csbNode:removeChildByName("EMOJ_CHAT")
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("public/emoj/%d.plist", idx))
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
    if cmd.MY_VIEWID == self.m_nViewId then
        spr:setPosition(cc.p(20,-100))
    end
    self.m_csbNode:addChild(spr)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.m_csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        self.m_csbNode:removeChildByName("EMOJ_CHAT")
    end)))
end

-- function GameRoleItem:browChat( idx )
--     if nil ~= self.m_labChat then
--         self.m_labChat:setVisible(false)
--     end
--     --self:changeChatPos()
--     self.m_spBrow:setVisible(false)
--     -- self.m_spChatBg:setContentSize(170, 40)
--     self.m_spChatBg:setContentSize(207, 103)
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

function GameRoleItem:onUserVoiceStart()
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self.m_spBrow:stopAllActions()
    self:changeChatPos()  
    -- self.m_spChatBg:setContentSize(170, 40) 
    self.m_spChatBg:setContentSize(207, 103) 
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:setVisible(true)
    self.m_spChatBg:setScale(1.0)
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("land_blank.png")
    if nil ~= frame then
        if cmd.RIGHT_VIEWID == self.m_nViewId then 
            self.m_spBrow:setRotation(0)
        else
            self.m_spBrow:setRotation(180)
        end        
        self.m_spBrow:setVisible(true)
        self.m_spBrow:setSpriteFrame(frame)
        self.m_spBrow:runAction(self.m_actVoiceAni)
    end
end

function GameRoleItem:onUserVoiceEnded()
    self.m_spBrow:setRotation(0)
    self.m_spBrow:stopAllActions()
    self.m_spBrow:setVisible(false)
    self.m_spChatBg:setVisible(false)
    self.m_spChatBg:setScaleX(0.00001)
end

function GameRoleItem:changeChatPos()
    if cmd.RIGHT_VIEWID == self.m_nViewId then
        self.m_spChatBg:setAnchorPoint(cc.p(1.0, 0.5))
        self.m_spChatBg:loadTexture("game_chat_2.png", UI_TEX_TYPE_PLIST)
        self.m_spChatBg:setPosition(-37, -80)
    elseif cmd.LEFT_VIEWID == self.m_nViewId then
        self.m_spChatBg:setAnchorPoint(cc.p(0, 0.5))
        self.m_spChatBg:loadTexture("game_chat_1.png", UI_TEX_TYPE_PLIST)
        self.m_spChatBg:setPosition(37, -80)
    else
        self.m_spChatBg:setAnchorPoint(cc.p(0, 0.5))
        self.m_spChatBg:loadTexture("game_chat_0.png", UI_TEX_TYPE_PLIST)
        self.m_spChatBg:setPosition(-70, -195)
    end
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:runAction(self.m_actTip)
end

function GameRoleItem:updateStatus()
    if self.m_userItem.cbUserStatus == yl.US_OFFLINE then
        self.m_bNormalState = false
        if GlobalUserItem.bPrivateGoldRoom then
            return
        end
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

    -- 昵称
    local name =  string.EllipsisByConfig(self.m_userItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
    if nil ~= self.m_clipNick then
        self.m_clipNick:setString(name)
    end
    self:updateScore(self.m_userItem.lScore)
end

-- 更新分数
function GameRoleItem:updateScore( lScore )
    lScore = lScore or 0
    -- 积分场
    if true == GlobalUserItem.bVideo then
        lScore = ""
    elseif GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
        local iniscore = PriRoom:getInstance().m_tabPriData.lIniScore or 0
        lScore = lScore - iniscore
    end
    -- 游戏币
    if nil ~= self.m_clipScore then
        self.m_clipScore:setString(lScore .. "")
    end
end

return GameRoleItem