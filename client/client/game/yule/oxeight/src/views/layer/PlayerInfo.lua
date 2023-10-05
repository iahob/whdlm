--
-- Author: David
-- Date: 2017-3-16
--
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")

local popPosition = {cc.p(153, 410), cc.p(123, 290), cc.p(286, 180), cc.p(820, 291),cc.p(800, 410),cc.p(811, 410),cc.p(478, 410),cc.p(362, 410)}

local popAnchor = {cc.p(0.0, 0.5), cc.p(0, 0.5),cc.p(0, 0),cc.p(1.0, 0.5),cc.p(1.0, 0.5),cc.p(1.0, 1.0),cc.p(0.5, 1.0),cc.p(0.0, 1.0)} 

local posChatView = {cc.p(185, 0), cc.p(185, 0), cc.p(60, 90), cc.p(-165, 0),cc.p(-165, 0),cc.p(135, -66),cc.p(115, -66),cc.p(135, -66)}
--local posText = {cc.p(150, 80), cc.p(-150, 75), cc.p(150, 90), cc.p(-110, -136),}

local PlayerInfo = class("PlayerInfo", cc.Node)

function PlayerInfo:ctor(userItem, viewId,scene)
    ExternalFun.registerNodeEvent(self)
    self.m_nViewId = viewId
    self.m_userItem = clone(userItem)
    self.m_bNormalState = (self.m_userItem.cbUserStatus ~= yl.US_OFFLINE)
    self._scene = scene
    self.m_clipScore = nil
    self.m_clipNick = nil
    self._nMultiple = 0

    -- 加载csb资源
    self.csbNode = ExternalFun.loadCSB("game/GameRoleItem.csb",self)

    -- 用户头像
    local head = PopupInfoHead:createClipHead(userItem, 84, "game/head.png")
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "game/popBg.png" --弹出框背景
    head:setPropInfo(param)
    dump(self._scene._scene.m_bLookOn,"====================8888888")
    head:enableInfoPop(true, popPosition[viewId], popAnchor[viewId])

    --head:enableHeadFrame(true, {_framefile = "land_headframe.png", _zorder = -1, _scaleRate = 0.75, _posPer = cc.p(0.5, 0.63)})
    self.m_popHead = head
    --获取头像NODE
    local nodeFace = self.csbNode:getChildByName("Node_face")
    nodeFace:addChild(head)

    -- 头像点击
    local btn = self.csbNode:getChildByName("btn")
    self.touchCount = 0
    btn:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then

            btn:runAction(cc.Sequence:create(cc.DelayTime:create(0.3), cc.CallFunc:create(function ( ... )
                if self.touchCount >= 2 then
                    scene:showSuperCtrl(userItem)
                else
                    if not self._scene._scene.m_bLookOn then 
                        self.m_popHead:onTouchHead()
                    end
                end
                btn:stopAllActions()
                self.touchCount = 0
            end)))
            self.touchCount = self.touchCount + 1 

        end
    end)

    --游戏币框
    self.csbNode:getChildByName("Text_coin"):setVisible(false)
    -- self.scoreBg = self.csbNode:getChildByName("Node_mycion")
    -- self.scoreBg:setVisible(true)

    
    -- if viewId == cmd.MY_VIEWID then

    -- else
    --     self.scoreBg = self.csbNode:getChildByName("Node_othercoin")
    --     self.scoreBg:setVisible(true)
    --     self.csbNode:getChildByName("Node_mycion"):setVisible(false)
    -- end

    -- 昵称
    if nil == self.m_clipNick then            
        self.m_clipNick = ClipText:createClipText(cc.size(110, 20), self.m_userItem.szNickName)
        self.m_clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        self.m_clipNick:setPosition(0, 52)
        self.csbNode:addChild(self.m_clipNick)
    else
        self.m_clipNick:setString(self.m_userItem.szNickName)
    end

    self.m_nodeInfo1 = self.csbNode:getChildByName("Node_playerInfo1")
    self.m_nodeInfo2 = self.csbNode:getChildByName("Node_playerInfo2")


    local spBanker = self.m_nodeInfo1:getChildByName("Sprite_banker")
    spBanker:setLocalZOrder(1)
    spBanker:setScale(0.3)

    
    -- 游戏币
    local labScore = self.csbNode:getChildByName("Text_coin")
    if nil ~= labScore then
        --labScore:setString(self.m_userItem.lScore)
                -- 游戏币
        if nil == self.m_clipScore then
            self.m_clipScore = ClipText:createClipText(cc.size(100, 20), self:getScoreString(self.m_userItem.lScore) .. "")
             --ClipText:createClipText(cc.size(100, 20), self.m_userItem.lScore .. "")
            self.m_clipScore:setAnchorPoint(cc.p(0.5, 0.5))
            self.m_clipScore:setTextColor(cc.c3b(192,155,124))
            self.m_clipScore:setPosition(labScore:getPosition())
            self.csbNode:addChild(self.m_clipScore)
        else
            self.m_clipScore:setString(self:getScoreString(self.m_userItem.lScore))
        end
    end

    local scale = 1.2
    if viewId == 3 then 
        self.m_clipScore:setPosition(cc.p(95,-16))
        self.m_clipNick:setPosition(cc.p(95,6))
        self.m_popHead:setScale(scale)
        self.csbNode:getChildByName("head_bg"):setScale(scale)
        self.csbNode:getChildByName("headMask"):setScale(scale)
        self.csbNode:getChildByName("head_flashbg"):setScale(scale)

    else
        self.m_clipScore:setPosition(cc.p(0,-79))
        self.m_clipNick:setPosition(cc.p(0,-57))
    end
    -- 聊天气泡
    self.m_spChatBg = nil 
    

    if viewId ==3 or viewId == 7 then
        self.m_spChatBg = self.csbNode:getChildByName("voice_animBg1")
    else

        self.m_spChatBg = self.csbNode:getChildByName("voice_animBg2")
    end
    self.m_spChatBg:setPosition(posChatView[viewId])
    self.m_spChatBg:setVisible(false)



    if viewId ==7 then
        self.m_spChatBg:setScaleY(-1)
    elseif viewId ==4 or viewId ==5   then
        self.m_spChatBg:setScaleX(-1)
    end


    -- 聊天内容
    if nil == self.m_labChat then
        self.m_labChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", 20, cc.size(170,0), cc.TEXT_ALIGNMENT_CENTER)
        self.m_labChat:setColor(cc.c3b(255,255,255)) 
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
        self.m_spVoice = display.newSprite("public/public_blank.png")
        self.m_spVoice:setAnchorPoint(cc.p(0.5, 0.5))
        self.m_spVoice:runAction(self.m_actVoiceAni)
        self.m_spVoice:setRotation(180)
        self.m_spVoice:addTo(self.m_spChatBg)
        self.m_spVoice:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5 +7)
    end

    local sprBanker = self.m_nodeInfo1:getChildByName("Sprite_banker")
    local sprhouseholder = self.m_nodeInfo1:getChildByName("Sprite_householder")
    local callInScoreBg = self.m_nodeInfo2:getChildByName("Sprite_coinBg")
    local textmulNum = self.m_nodeInfo2:getChildByName("Sprite_mulNum")  
    local spriteMul = self.m_nodeInfo2:getChildByName("Sprite_mul")  

    self._betEx = self.csbNode:getChildByName("tuizhu")

    self._betEx:setPosition(cc.p(0,85))
    if viewId == 7 then
        self._betEx:setPosition(cc.p(90,28))
    end

    self._bankerWait = self.csbNode:getChildByName("banker_waitIcon")
    self._spriteMul = spriteMul
    self._multipleNum = textmulNum

    self._callingIcon = self.csbNode:getChildByName("banker_calling_icon")
    self._callingIcon:setVisible(false)

    if viewId == 1 or viewId == 5 or viewId == 6 then
        self._callingIcon:setPosition(cc.p(115,45))
    end

    local point1 = {cc.p(155,42),cc.p(155,42),cc.p(546,117),cc.p(-150,45),cc.p(-150,45),cc.p(2,-231),cc.p(2,-231),cc.p(2,-231)}
    local point2 = {cc.p(88,60),cc.p(88,46),cc.p(-16,90),cc.p(-126,46),cc.p(-126,60),cc.p(-126,29),cc.p(-15,-210),cc.p(88,29)}
    local point3 = {cc.p(-36,20),cc.p(-46,103),cc.p(-36,21),cc.p(46,103),cc.p(77,68),cc.p(77,68),cc.p(-81,33),cc.p(-77,68)}
    local point4 = {cc.p(-77,68),cc.p(-64,37),cc.p(-64,-39),cc.p(46,103),cc.p(77,68),cc.p(77,68),cc.p(-81,33),cc.p(-77,68)}

    -- if viewId == 4 or viewId == 5 or viewId == 6 then  --右手边的位置
        -- sprhouseholder:setPosition(cc.p(64,-39))
        
    -- else 
        sprhouseholder:setPosition(cc.p(34,-18))
    -- end
    callInScoreBg:setPosition(point1[viewId])
    -- textmulNum:setPosition(point2[viewId])
    -- spriteMul:setPosition(cc.p(point2[viewId].x + 55,point2[viewId].y))
    spriteMul:setPosition(35,31)
    -- sprBanker:setPosition(point3[viewId])
    sprBanker:setPosition(-36,21)
    self:updateStatus()


    --self.m_spChatBg:setVisible(true)
    --self.m_spVoice:setVisible(true)
    --self:textChat("使得发达省份见风使舵金佛山敬佛打扫ifjoi多家")
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

function PlayerInfo:setBankerWaitStatus( isShow,isNormal,viewIndex,isXuan)
   self._bankerWait:setVisible(isShow)
   local point  ={cc.p(145,-60),cc.p(32,-86),cc.p(145,-60),cc.p(-32,-86),cc.p(-145,-60),cc.p(31,-86),cc.p(0,-138),cc.p(31,-86)}
   self._bankerWait:setPosition(point[viewIndex])
   
   
end


function PlayerInfo:showBetBtEx(visible)
    self._betEx:setVisible(visible)
end
--庄家
function PlayerInfo:showBank( isBanker)
    --庄家图标
    local bank = self.m_nodeInfo1:getChildByName("Sprite_banker")
        bank:setVisible(isBanker)
    -- if isBanker then
    --     bank:runAction(cc.Sequence:create(cc.ScaleTo:create(0.15,0.3),cc.ScaleTo:create(0.15,0.3)))
    -- end

    print("Visible(isBanker)",self.m_nodeInfo1:getChildByName("Sprite_banker"):isVisible())
end
--房主图标
function PlayerInfo:showRoomHolder( isRoomHolder )
    self.m_nodeInfo1:getChildByName("Sprite_householder"):setVisible(isRoomHolder)
    if self._scene._scene.cbPlayMode == 1 then
        self.m_nodeInfo1:getChildByName("Sprite_householder"):setVisible(false)
    end

end

--背景闪光
function PlayerInfo:showFlashBg( flag )
    local headFlashBg = self.csbNode:getChildByName("head_flashbg")
    if flag == 1 then
        --闪烁动画
        headFlashBg:runAction(cc.RepeatForever:create(cc.Blink:create(0.5,2)))
    elseif flag == 2 then

        headFlashBg:setVisible(true)
        headFlashBg:runAction(cc.Sequence:create(cc.DelayTime:create(0.12),cc.Hide:create()))

    elseif flag == 3 then
        headFlashBg:stopAllActions()
        headFlashBg:setVisible(false)
    elseif flag == 4 then
        headFlashBg:runAction(cc.Sequence:create(cc.Blink:create(0.8,2), cc.Show:create()))
    elseif flag == 5 then
        headFlashBg:stopAllActions()
        headFlashBg:setVisible(true)
    end
end

function PlayerInfo:setFlashBg(visible)
        self.csbNode:getChildByName("head_flashbg"):setVisible(visible)
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
    
    -- 游戏币
    -- self.scoreBg = self.csbNode:getChildByName("Node_mycion")
    -- local labScore = self.scoreBg:getChildByName("text_coins")
    if nil == self.m_clipScore then
        self.m_clipScore:setString(self:getScoreString(self.m_userItem.lScore))
    end

    --庄家图标
    self.m_nodeInfo1:getChildByName("Sprite_banker"):setVisible(isBanker)

    self:runAction(cc.Sequence:create(cc.DelayTime:create(1),cc.CallFunc:create(function()
        self:updateStatus()
    end)))
end

function PlayerInfo:textChat( str )
    print("玩家头像上聊天信息", self.m_spChatBg, str)
    self.m_spChatBg:setVisible(true)
    self.m_spVoice:setVisible(false)

    self.m_labChat:setString(str)

    self.csbNode:removeChildByName("EMOJ_CHAT")

    --self:changeChatPos()
    self.m_labChat:setVisible(true)
    -- 尺寸调整
    local labSize = self.m_labChat:getContentSize()
    if labSize.height >= 40 then
        self.m_spChatBg:setContentSize(200, labSize.height + 54)
    else
        self.m_spChatBg:setContentSize(200, 70)
    end

    self.m_labChat:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5 +7)

    --self.m_labChat:setScale(1)

    if self.m_nViewId ==7 then
        self.m_labChat:setScaleY(-1)
    elseif self.m_nViewId ==4 or self.m_nViewId ==5 then
        self.m_labChat:setScaleX(-1)
    end


    self.m_spChatBg:runAction(cc.Sequence:create(cc.DelayTime:create(3.0), cc.Hide:create()))
end

function PlayerInfo:browChat( idx )
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    --self:changeChatPos()

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
        print("removeFromParent!!!!!!!!!!!!!!!!!")
        
       self.csbNode:removeChildByName("EMOJ_CHAT")
    end)))


end

function PlayerInfo:onUserVoiceStart()
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    --self:changeChatPos()  
    self.m_spChatBg:setContentSize(200, 70) 
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:setVisible(true)
    self.m_spVoice:setPosition(105, 39)
    if nil ~= self.m_spVoice then
        self.m_spVoice:setVisible(true)
    end
end

function PlayerInfo:onUserVoiceEnded()
    self.m_spChatBg:setVisible(false)
    if nil ~= self.m_spVoice then
        self.m_spVoice:setVisible(false)
    end
end

function PlayerInfo:changeChatPos()
    print("聊天气泡位置", posChatView[self.m_nViewId].x, posChatView[self.m_nViewId].y)
    self.m_spChatBg:setPosition(posChatView[self.m_nViewId])
    --self.m_spChatBg:setScale(1)

    --if posChatView[self.m_nViewId].x< 0 then
        --self.m_spChatBg:setScaleX(-1)
    --end

    --if posChatView[self.m_nViewId].y< 0 then
        --self.m_spChatBg:setScaleY(-1)
    --end

end

function PlayerInfo:updateStatus()
    if self.m_userItem.cbUserStatus == yl.US_OFFLINE and self._scene._scene.cbPlayMode == 0 then
        self.m_bNormalState = false
        if nil ~= convertToGraySprite then
            -- 灰度图
            if nil ~= self.m_popHead and nil ~= self.m_popHead.m_head and nil ~= self.m_popHead.m_head.m_spRender then
                convertToGraySprite(self.m_popHead.m_head.m_spRender)
                self:setHeadStatus(cmd.US_Off)
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
                    -- self:setHeadStatus(cmd.US_Normal)
                end
            end
        end
    end

    if nil ~= self.m_clipScore then
        self.m_clipScore:setString(self:getScoreString(self.m_userItem.lScore))
        --判断积分房
        if PriRoom and GlobalUserItem.bPrivateRoom then
            if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
                if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
                        print("lScore is ============",self.m_userItem.lScore)
                        print("lIniScore is ==========",PriRoom:getInstance().m_tabPriData.lIniScore)
                    self.m_clipScore:setString(self.m_userItem.lScore - PriRoom:getInstance().m_tabPriData.lIniScore)
                end
            else
                self.m_clipScore:setString("0")
            end
        end
        if GlobalUserItem.bVideo and PriRoom:getInstance().m_tabPriData.lIniScore then
            self.m_clipScore:setString(self.m_userItem.lScore - PriRoom:getInstance().m_tabPriData.lIniScore)
        end
    end

    if nil ~= self.m_clipNick then
        self.m_clipNick:setString(self.m_userItem.szNickName)
    end
   -- self.m_popHead:updateHead(self.m_userItem)

end

--显示玩家下注分数
function PlayerInfo:showCallInScore(score,isVisible,isAni)
    -- if self.m_nViewId == 3 then
        -- print("******显示玩家下注分数",self.m_nViewId,isVisible, debug.traceback())
    -- end
    local coinBg = self._scene._csbNode:getChildByName("Node_betScore"):getChildByName("coinBg_"..self.m_nViewId)
    coinBg:setVisible(false)
    local num =  coinBg:getChildByName("Text_callIn")
    num:setString(self:getScoreString(score))

    local callInScoreBg = self.m_nodeInfo2:getChildByName("Sprite_coinBg")
    callInScoreBg:setVisible(false)
    local callInScore =  callInScoreBg:getChildByName("Text_callIn")
    callInScore:setString(self:getScoreString(score))
    if isVisible  then
            if isAni then
                for i=1,2 do
                    local gold = cc.Sprite:create("game/im_fly_gold_1.png"):move(0,0):addTo(self)
                    gold:runAction(cc.Sequence:create(cc.DelayTime:create(0.1+0.05*i),
                        cc.Spawn:create(
                            cc.MoveTo:create(0.3,cc.p(callInScoreBg:getPositionX() - 48.5,callInScoreBg:getPositionY() - 0.4)),
                            cc.ScaleTo:create(0.3,0.6)
                        ),
                        -- cc.DelayTime:create(0.2),
                        cc.RemoveSelf:create(),
                        cc.CallFunc:create(function ( ... )
                            if i == 2 then
                                coinBg:setVisible(true)
                            end
                        end)
                    ))
                end
            else
                coinBg:setVisible(true)
            end
             self:hiddenMultiple()
    end

end

function PlayerInfo:hiddenMultiple()
    -- print("hiddenMultiple",debug.traceback())
    self._spriteMul:setVisible(false)
    self._multipleNum:setVisible(false)
end

function PlayerInfo:setMultipleBankerPos(bBankerPos)
    
    local point1 = {cc.p(155,42),cc.p(155,42),cc.p(506,117),cc.p(-150,45),cc.p(-150,45),cc.p(2,-231),cc.p(2,-231),cc.p(2,-231)}
    if not bBankerPos then
        self._multipleNum:setPosition(point1[self.m_nViewId])
        self._multipleNum:setTexture("game/"..string.format("multip_%d.png", self._nMultiple))
    else
        self._multipleNum:setPosition(cc.p(36,31))
        self._multipleNum:setTexture("game/"..string.format("Sprite_mul%d.png", self._nMultiple))
    end
end

function PlayerInfo:setMultiple( multiple )
    self._nMultiple = multiple

    self:setCallingBankerStatus(false)
    -- if 0 ~= multiple then
       -- self._spriteMul:setVisible(true)
    -- end
    self._multipleNum:setVisible(true)
    print("multiple is =============",multiple)
    local multipleFrame = cc.Sprite:create("game/"..string.format("Sprite_mul%d.png", multiple))
    self._multipleNum:setSpriteFrame(multipleFrame:getSpriteFrame())
    --self._multipleNum:runAction(action)
    self._multipleNum:runAction(cc.Sequence:create(cc.ScaleTo:create(0.15,1.3),cc.ScaleTo:create(0.15,1)))
end

function PlayerInfo:getMultiple()
    return self._nMultiple
end

function PlayerInfo:setCallingBankerStatus(isCalling)
    if isCalling == true then
       self:hiddenMultiple()
    end
   
    self._callingIcon:setVisible(isCalling)

end
--转换分数
function PlayerInfo:getScoreString( score )
    if type(score) ~= "number" then
        return ""
    end
    local strScore = ""
    if score < 100000 then
        strScore = strScore..score
    elseif score < 100000000 then
        --print("分数转换1", score/10000)
        strScore = strScore..string.format("%.2f", score/10000).."万"
    else
        --print("分数转换2", score/100000000)
        strScore = strScore..string.format("%.2f", score/100000000).."亿"
    end
    --print("分数转换", score, strScore)
    return strScore
end

function PlayerInfo:getHeadStatus(status)
    return self.headStatus or cmd.US_Normal
end

function PlayerInfo:setHeadStatus(status)
    -- print(status,self.m_nViewId,debug.traceback())
    local headMask = self.csbNode:getChildByName("headMask")
    local sp = headMask:getChildByName("head_status")
    self.headStatus = status
    headMask:setVisible(false)
    if status == cmd.US_Normal then
        headMask:setVisible(false)
    elseif status == cmd.US_Auto then
        headMask:setVisible(true)
        sp:setTexture("game/head_auto.png")
    elseif status == cmd.US_CuoPai then
        headMask:setVisible(true)
        sp:setTexture("game/head_cuo.png")
    elseif status == cmd.US_Bet then
        headMask:setVisible(true)
        sp:setTexture("game/head_bet.png")
    elseif status == cmd.US_Off then
        headMask:setVisible(true)
        sp:setTexture("game/heat_off.png")
    elseif status == cmd.US_CuoWan then
        headMask:setVisible(true)
        sp:setTexture("game/head_complete.png")
    end
end

return PlayerInfo