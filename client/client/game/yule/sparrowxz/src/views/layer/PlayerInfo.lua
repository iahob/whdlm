--
-- Author: David
-- Date: 2017-3-16
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.models.CMD_Game")

local popPosition = {}
popPosition[cmd.LEFT_VIEWID] = cc.p(210, 350)
popPosition[cmd.RIGHT_VIEWID] = cc.p(750, 350)
popPosition[cmd.MY_VIEWID] = cc.p(145, 175)
popPosition[cmd.TOP_VIEWID] = cc.p(750, 350)
local popAnchor = {} 
popAnchor[cmd.LEFT_VIEWID] = cc.p(0, 0)
popAnchor[cmd.RIGHT_VIEWID] = cc.p(1, 0)
popAnchor[cmd.MY_VIEWID] = cc.p(0, 0)
popAnchor[cmd.TOP_VIEWID] = cc.p(1, 1)

local posChatView = {cc.p(150, 90), cc.p(-150, 70), cc.p(150, 90), cc.p(-110, -130),}
local posText = {cc.p(150, 90), cc.p(-150, 75), cc.p(150, 90), cc.p(-110, -136),}

local PlayerInfo = class("PlayerInfo", cc.Node)

function PlayerInfo:ctor(userItem, viewId)
    ExternalFun.registerNodeEvent(self)
    self.m_nViewId = viewId
    self.m_userItem = userItem
    self.m_bNormalState = (self.m_userItem.cbUserStatus ~= yl.US_OFFLINE)

    self.originScore=self.m_userItem.lScore
    if GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore 
        then
         self.originScore = self.originScore - PriRoom:getInstance().m_tabPriData.lIniScore
    end

    self.m_clipScore = nil
    -- 加载csb资源
    self.csbNode = ExternalFun.loadCSB("game/nodeUser.csb",self)
   
    self.csbNode:getChildByName("sp_headCover"):setLocalZOrder(-1)

    -- 用户头像
    local head = PopupInfoHead:createNormal(userItem, 76)
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "game/popBg.png" --弹出框背景
    head:setPropInfo(param)
    head:enableInfoPop(true, popPosition[viewId], popAnchor[viewId])
    self.m_popHead = head

    --创建裁剪
    local strClip = "game/head_mask.png"
    clipSp = cc.Sprite:create(strClip)
    local clip = cc.ClippingNode:create()
    if nil ~= clipSp then
        --裁剪
        clip:setStencil(clipSp)
        clip:setAlphaThreshold(0.01)
        clip:addChild(head)
    end

    --获取头像NODE
    local nodeFace = self.csbNode:getChildByName("Node_face")
    nodeFace:addChild(clip)
     local selfSize = nodeFace:getContentSize()
    clip:setPosition(cc.p(selfSize.width * 0.5-6, selfSize.height * 0.5+4))
    
    -- 头像点击
    self.bthheadimage = self.csbNode:getChildByName("btn")
    self.bthheadimage:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
            if self.parent._scene.m_bLookOn then
                showToast(self.parent, "观战中不可使用该功能", 1)
                return
            end
            self.m_popHead:onTouchHead()
        end
    end)

    --金币框
    -- if viewId == cmd.MY_VIEWID then
        self.scoreBg = self.csbNode:getChildByName("Node_mycion")
        self.scoreBg:setVisible(true)
        self.csbNode:getChildByName("Node_othercoin"):setVisible(false)
    -- else
    --     self.scoreBg = self.csbNode:getChildByName("Node_othercoin")
    --     self.scoreBg:setVisible(true)
    --     self.csbNode:getChildByName("Node_mycion"):setVisible(false)
    -- end

        -- 昵称
    if nil == self.m_clipNick then            
        self.m_clipNick = ClipText:createClipText(cc.size(105, 20), self.m_userItem.szNickName,"fonts/round_body.ttf",20)
        self.m_clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        self.m_clipNick:setPosition(self.csbNode:getChildByName("Text_nickname"):getPosition())
        self:addChild(self.m_clipNick)
    else
        self.m_clipNick:setString(self.m_userItem.szNickName)
    end
    
    -- 金币
    self.labScore = self.scoreBg:getChildByName("text_coins")
    if nil ~= labScore then
        -- labScore:setString(self:formatScore(self.m_userItem.lScore))
        self:formatScore(self.labScore, self.m_userItem.lScore)
                -- 金币
        -- if nil == self.m_clipScore then
        --     self.m_clipScore = ClipText:createClipText(cc.size(110, 20), self.originScore .. "")
        --     if viewId == cmd.MY_VIEWID then
        --         self.m_clipScore:setAnchorPoint(cc.p(0, 0.5))
        --     else
        --         self.m_clipScore:setAnchorPoint(cc.p(0, 0.5))
        --     end
        --     self.m_clipScore:setTextColor(cc.YELLOW)
        --     self.m_clipScore:setPosition(labScore:getPosition())
        --     self.scoreBg:addChild(self.m_clipScore)
        -- else
        --     self.m_clipScore:setString(self.originScore .. "")
        -- end
    end

    -- 聊天气泡
    self.m_spChatBg = self.csbNode:getChildByName("voice_animBg")
    self.m_spChatBg:setVisible(false)

    -- 聊天内容
    if nil == self.m_labChat then
        self.m_labChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", 20, cc.size(150,0), cc.TEXT_ALIGNMENT_CENTER)        
        self.m_spChatBg:addChild(self.m_labChat)
    end

    -- 聊天动画
    local sc = cc.ScaleTo:create(0.1, 1.0, 1.0)
    local show = cc.Show:create()
    local spa = cc.Spawn:create(sc, show)
    self.m_actTip = cc.Sequence:create(spa, cc.DelayTime:create(2.0), cc.ScaleTo:create(0.1, 0.00001, 1.0), cc.Hide:create())
    self.m_actTip:retain()

    -- 语音动画
    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.1
    param.m_strName = cmd.VOICE_ANIMATION_KEY
    local animate = AnimationMgr.getAnimate(param)
    self.m_actVoiceAni = cc.RepeatForever:create(animate)
    self.m_actVoiceAni:retain()

        --语音动画
    if nil == self.m_spVoice then
        self.m_spVoice = display.newSprite() 
        self.m_spVoice:setAnchorPoint(cc.p(0.5, 0.5))
        
        self.m_spVoice:runAction(self.m_actVoiceAni)
        self.m_spVoice:addTo(self.m_spChatBg)
        self.m_spVoice:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5 +7)
    end

    self.m_spRoomerFlag =self.csbNode:getChildByName("icon_roomHolder")

    self:updateStatus()

end

function PlayerInfo:onExit()
    self.m_actTip:release()
    self.m_actTip = nil
    self.m_actVoiceAni:release()
    self.m_actVoiceAni = nil
end

function PlayerInfo:reSet()
    self.m_popHead:setVisible(true)
end

function PlayerInfo:setShowCallCardKind(bShow)
    self.csbNode:getChildByName("callcard"):setVisible(bShow)
end

function PlayerInfo:showBank( isBanker )
    --庄家图标
    local spbanker = self.csbNode:getChildByName("Sprite_banker")
    spbanker:setVisible(isBanker)

    if cmd.RIGHT_VIEWID == self.m_nViewId or cmd.TOP_VIEWID == self.m_nViewId then
        spbanker:setPosition(-55, 35)
    else
        spbanker:setPosition(45, 35)
    end
end

function PlayerInfo:switeRoomerState( isRoomer )
    print("switeRoomerState", isRoomer, GlobalUserItem.bPrivateGoldRoom)
    self.m_spRoomerFlag:setVisible(isRoomer)
    if GlobalUserItem.bPrivateGoldRoom then
        self.m_spRoomerFlag:setVisible(false)
    end
end


function PlayerInfo:showRoomHolder( isRoomHolder )
    --房主图标
    local sproomHolder = self.csbNode:getChildByName("icon_roomHolder")
    sproomHolder:setVisible(isRoomHolder and not GlobalUserItem.bPrivateGoldRoom)

    if cmd.RIGHT_VIEWID == self.m_nViewId or cmd.TOP_VIEWID == self.m_nViewId then
        sproomHolder:setPosition(-55, -5)
    else
        sproomHolder:setPosition(55, -5)
    end
end

function  PlayerInfo:showCallCard(kind)
    print("kind:",kind)
    local filepath={"label_wan.png","label_tiao.png","label_tong.png"}
    local sp=self.csbNode:getChildByName("callcard")
    sp:setVisible(true)
    sp:setTexture("game/"..filepath[kind])
end

--回放--
function  PlayerInfo:setShowCallCard(kind)
    if not kind then
        self.csbNode:getChildByName("callcard"):setVisible(false)
    else
        self:showCallCard(kind)
    end
end

function PlayerInfo:switeGameState( isBanker )
    self.m_popHead:setVisible(false) 
    -- 昵称
    if nil == self.m_clipNick then            
        self.m_clipNick = ClipText:createClipText(cc.size(90, 20), self.m_userItem.szNickName)
        self.m_clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        self.m_clipNick:setPosition(0, 70)
        self:addChild(self.m_clipNick)
    else
        self.m_clipNick:setString(self.m_userItem.szNickName)
    end
    
    -- 金币
    self.scoreBg = self.csbNode:getChildByName("Node_mycion")
    local labScore = self.scoreBg:getChildByName("text_coins")
    if nil ~= labScore then
        labScore:setString(self.m_userItem.lScore)
    end

    --庄家图标
    self.csbNode:getChildByName("Sprite_banker"):setVisible(isBanker)


    self:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function()
        self:updateStatus()
    end)))
end

function PlayerInfo:textChat( str )
    print("玩家头像上聊天信息", self.m_spChatBg, str)
    self.m_spChatBg:setVisible(true)
    self.m_spVoice:setVisible(false)

    self.m_labChat:setString(str)

    self:changeChatPos()
    self.m_labChat:setVisible(true)
    -- 尺寸调整
    local labSize = self.m_labChat:getContentSize()
    if labSize.height >= 40 then
        self.m_spChatBg:setContentSize(170, labSize.height + 54)
    else
        self.m_spChatBg:setContentSize(170, 74)
    end

    self.m_labChat:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5 +7)

    if cmd.TOP_VIEWID == self.m_nViewId then --在顶部旋转了，子串不旋转
        self.m_labChat:setScale(-1)
    elseif cmd.RIGHT_VIEWID == self.m_nViewId then
        self.m_labChat:setScaleX(-1)
    else
        self.m_labChat:setScale(1)
    end

    self.m_spChatBg:runAction(cc.Sequence:create(cc.DelayTime:create(3.0), cc.Hide:create()))
end

function PlayerInfo:browChat( idx )
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self:changeChatPos()

    self.m_spChatBg:setVisible(false)

    print("browChat--------------------->", idx)
    self.csbNode:removeChildByName("EMOJ_CHAT")
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
    self.csbNode:addChild(spr)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        self.csbNode:removeChildByName("EMOJ_CHAT")
    end)))
end

function PlayerInfo:updateScore(score)
    -- self.m_clipScore:setString(score .. "")
    dump(self.m_userItem,"===============m_clipScore====================")
    self:formatScore(self.labScore, self.m_userItem.lScore)  
end

function PlayerInfo:onUserVoiceStart()
    print("onUserVoiceStart")
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self:changeChatPos()  
    self.m_spChatBg:setContentSize(170, 80) 
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:setVisible(true)
    self.m_spChatBg:setScale(1.0)
    if nil ~= self.m_labChat then
        self.m_spVoice:setVisible(true)
    end
end

function PlayerInfo:onUserVoiceEnded()
    print("onUserVoiceEnded")
    self.m_spChatBg:setVisible(false)
    if nil ~= self.m_labChat then
        self.m_spVoice:setVisible(false)
    end
end

function PlayerInfo:changeChatPos()
    print("聊天气泡位置", posChatView[self.m_nViewId].x, posChatView[self.m_nViewId].y)
    self.m_spChatBg:setPosition(posChatView[self.m_nViewId])

    if cmd.RIGHT_VIEWID == self.m_nViewId then
        self.m_spChatBg:setScaleX(-1)
    elseif cmd.TOP_VIEWID == self.m_nViewId then
        self.m_spChatBg:setScale(-1)
    else
        self.m_spChatBg:setScale(1)
    end
end

function PlayerInfo:updateStatus(bSetGray)
    if self.m_userItem.cbUserStatus == yl.US_OFFLINE  or bSetGray then
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
    local score=self.m_userItem.lScore
    if GlobalUserItem.bPrivateRoom and PriRoom and 1 == PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore 
         then
         score = score - PriRoom:getInstance().m_tabPriData.lIniScore
    end
    -- self.m_clipScore:setString(self:formatScore(score))
    -- self:formatScore(self.labScore, self.m_userItem.lScore)
    self:formatScore(self.labScore, score)

    --更新换桌头像
    self.m_popHead:updateHead(self.m_userItem)

    --更新换桌昵称
    self.m_clipNick:setString(self.m_userItem.szNickName)
end

function PlayerInfo:formatScore(lab, score)
    if lab == nil then return end
    local spUnit = lab:getChildByName("num_wan")
    local score = tonumber(score)
    local scorestr = tostring(score)

    local function setText(str, file)
        lab:setString(str)
        spUnit:setVisible(true)
        spUnit:setTexture(file)
        spUnit:setPositionX(lab:getContentSize().width+10)
    end

    if score < 10000 then
        lab:setString(scorestr)
        spUnit:setVisible(false)
        return 
    end

    if score < 100000 then
        scorestr = string.format("%.2f万", score / 10000)
        setText(scorestr, "game/num_wan.png")
        return
    end
    if score < 1000000 then
        scorestr = string.format("%.1f万", score / 10000)
        setText(scorestr, "game/num_wan.png")
        return
    end
    if score < 100000000 then
        scorestr = string.format("%d万", math.floor(score / 10000))
        setText(scorestr, "game/num_wan.png")
        return
    end

    if score < 1000000000 then
        scorestr = string.format("%.2f亿", score / 100000000)
        setText(scorestr, "game/num_yi.png")
        return
    end
    if score < 10000000000 then
        scorestr = string.format("%.1f亿", score / 100000000)
        setText(scorestr, "game/num_yi.png")
        return
    end
    if score < 100000000000 then
        scorestr = string.format("%d亿", math.floor(score / 100000000))
        setText(scorestr, "game/num_yi.png")
        return
    end
end

return PlayerInfo