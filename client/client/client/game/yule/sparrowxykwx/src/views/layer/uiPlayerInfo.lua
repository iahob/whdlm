local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowxykwx.src." .. "models.CMD_Game")

local uiPlayerInfo = class("uiPlayerInfo", cc.Node)

local popPosition = {}
popPosition[cmd.LEFT_VIEWID] = cc.p(210, 350)
popPosition[cmd.RIGHT_VIEWID] = cc.p(750, 350)
popPosition[cmd.MY_VIEWID] = cc.p(145, 175)
popPosition[cmd.TOP_VIEWID] = cc.p(600, 350)
local popAnchor = {} 
popAnchor[cmd.LEFT_VIEWID] = cc.p(0, 0)
popAnchor[cmd.RIGHT_VIEWID] = cc.p(1, 0)
popAnchor[cmd.MY_VIEWID] = cc.p(0, 0)
popAnchor[cmd.TOP_VIEWID] = cc.p(1, 1)

function uiPlayerInfo:ctor(userItem, viewId,scene)
    self:init(userItem, viewId,scene)
end

function uiPlayerInfo:init(userItem, viewId,scene)
    if self.isInited then
        error("Inited yet!")
        return
    end

    self.isInited = true
    self._scene = scene

    ExternalFun.registerNodeEvent(self)
    self.m_nViewId = viewId
    self.m_userItem = userItem
    self.m_bNormalState = (self.m_userItem.cbUserStatus ~= yl.US_OFFLINE)

    -- 加载csb资源
    if viewId == 2 then
        self.widget = ExternalFun.loadCSB("PlayerRight.csb", self)
    else
        self.widget = ExternalFun.loadCSB("PlayerLeft.csb", self)
    end
    self.m_clipScore = cc.find(self.widget, "Sprite_3/Text_3")
    self.m_clipNick = cc.find(self.widget, "Text_name")

    -- -- 用户头像
    -- local head = PopupInfoHead:createClipHead(userItem, 90, "faces/kwx_face_mask.png")
    -- self.m_popHead = head

    -- --获取头像NODE
    -- local nodeFace = self.widget:getChildByName("btn_face")
    -- nodeFace:addChild(head)
    -- local btnSize = nodeFace:getContentSize()
    -- head:setPosition(btnSize.width*0.5, btnSize.height*0.5)

    -- -- 头像点击
    -- cc.funAddChildListener(self.widget, "btn_face", function() self.m_popHead:onTouchHead() end)
    -- 用户头像
    local head = PopupInfoHead:createNormal(userItem, 80)
    local param = {}
    param.isProp = true     --显示道具互动，百人游戏不显示
    param.imgPath = "game/popInfo/popBg.png" --弹出框背景
    head:setPropInfo(param)
    head:setPopCsbFile("game/popInfo/GamePopInfo.csb") -- 自定义CSB
    head:enableInfoPop(true, popPosition[viewId], popAnchor[viewId])
    -- head:setPopCsbFile("game/GamePopInfo.csb")              
    --head:enableHeadFrame(true, {_framefile = "land_headframe.png", _zorder = -1, _scaleRate = 0.75, _posPer = cc.p(0.5, 0.63)})
    self.m_popHead = head
    --获取头像NODE
    local nodeFace = self.widget:getChildByName("Node_face")
    nodeFace:addChild(head)


    -- 头像点击
    local btn = self.widget:getChildByName("btn_face")
    btn:addTouchEventListener(function( ref, tType)
        if tType == ccui.TouchEventType.ended then
                if self._scene.m_bLookOn then
                        showToast(self, "观战中不可使用该功能", 1)
                        return
                end
            self.m_popHead:onTouchHead()
        end
    end)


    self.m_clipNick:setString( ExternalFun.clipText(self.m_userItem.szNickName,6,true) )


    -- 聊天气泡
    self.m_spChatBg = self.widget:getChildByName("sprite_bubble")
    self.m_spChatBg:setVisible(false)

    self.m_labChat = cc.find(self.m_spChatBg, "Text_1")
    self.m_labChat:setColor(cc.c3b(77, 77, 77));

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

    self:refresh(userItem)

    self:showBank(false)
    self:showRoomHolder(false)
end

function uiPlayerInfo:reSet()
    self.m_popHead:setVisible(true)
end

function uiPlayerInfo:showBank( isBanker )
    --庄家图标
    cc.find(self.widget, "Sprite_zhuang"):setVisible(isBanker)
end

--房主图标
function uiPlayerInfo:showRoomHolder( isRoomHolder )
    local spRoomOwner = cc.find(self.widget, "Sprite_fang")
    if GlobalUserItem.bPrivateGoldRoom then
        spRoomOwner:setVisible(false)
    else
        spRoomOwner:setVisible(isRoomHolder)
    end
end

function uiPlayerInfo:switeGameState( isBanker )
    error("Not implete yet!")
end

function uiPlayerInfo:textChat( str )
    print("玩家头像上聊天信息", self.m_spChatBg, str)
    self.m_spChatBg:setVisible(true)
    self.m_spVoice:setVisible(false)

    self.m_labChat:setString(str)

    self:changeChatPos()
    self.m_labChat:setVisible(true)
    -- 尺寸调整
    local labSize = self.m_labChat:getContentSize()
    if labSize.height >= 40 then
        self.m_spChatBg:setContentSize(200, labSize.height + 30)
    else
        self.m_spChatBg:setContentSize(200, 64)
    end

    -- self.m_labChat:setPosition(self.m_spChatBg:getContentSize().width * 0.5, self.m_spChatBg:getContentSize().height * 0.5 +7)

    if cmd.TOP_VIEWID == self.m_nViewId then --在顶部旋转了，子串不旋转
        -- self.m_labChat:setScaleX(-1)  
    elseif cmd.RIGHT_VIEWID == self.m_nViewId then
        -- self.m_labChat:setScaleX(-1)
    else
        self.m_labChat:setScale(1)
    end

    self.m_spChatBg:runAction(cc.Sequence:create(cc.DelayTime:create(3.0), cc.Hide:create()))
end

function uiPlayerInfo:browChat( idx )
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self:changeChatPos()

    print("browChat--------------------->", idx)
    self.widget:removeChildByName("EMOJ_CHAT")
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
    spr:setPosition(cc.p(71, 112))
    self.widget:addChild(spr)
    spr:setName("EMOJ_CHAT")
    spr:runAction(cc.RepeatForever:create(cc.Animate:create(animation)))
    self.widget:runAction(cc.Sequence:create(cc.DelayTime:create(2.0) ,cc.CallFunc:create(function()
        self.widget:removeChildByName("EMOJ_CHAT")
    end)))
end

function uiPlayerInfo:onUserVoiceStart()
    if nil ~= self.m_labChat then
        self.m_labChat:setVisible(false)
    end
    self:changeChatPos()  
    self.m_spChatBg:setContentSize(170, 74) 
    self.m_spChatBg:stopAllActions()
    self.m_spChatBg:setVisible(true)
    if nil ~= self.m_spVoice then
        self.m_spVoice:setVisible(true)
    end
end

function uiPlayerInfo:onUserVoiceEnded()
    self.m_spChatBg:setVisible(false)
    if nil ~= self.m_spVoice then
        self.m_spVoice:setVisible(false)
    end
end

function uiPlayerInfo:changeChatPos()
    -- print("聊天气泡位置", posChatView[self.m_nViewId].x, posChatView[self.m_nViewId].y, self.m_nViewId)
    -- self.m_spChatBg:setPosition(posChatView[self.m_nViewId])
    if cmd.RIGHT_VIEWID == self.m_nViewId then
        -- self.m_spChatBg:setScaleX(-1)
    elseif cmd.TOP_VIEWID == self.m_nViewId then
        -- self.m_spChatBg:setScaleX(-1)
    else
        self.m_spChatBg:setScale(1)
    end
end

function uiPlayerInfo:setIsReady(isReady)
    cc.find(self.widget, "sp_status"):setVisible(isReady)
end

function uiPlayerInfo:refresh(data)
    if not self.isInited then return end

    --在线情况,只负责刷新,不负责数据处理
    if data.cbUserStatus == yl.US_OFFLINE and not GlobalUserItem.bPrivateGoldRoom then
        --待优化,这种多重访问的方式耦合太重
        convertToGraySprite(self.m_popHead.m_head.m_spRender)
    else
        convertToNormalSprite(self.m_popHead.m_head.m_spRender)
    end

    self.m_userItem = data

    --名字
    local clipName = string.clip(data.szNickName, 16)
    self.m_clipNick:setString(ExternalFun.clipText(data.szNickName,6,true))
    --score ExternalFun.clipText(data.szNickName,12,true)
    self.m_clipScore:setString(self:getScoreString(self.m_userItem.lScore))
    -- print("uiPlayerInfo:refresh=====", GlobalUserItem.bPrivateRoom, PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore)

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore then
            if  PriRoom:getInstance().m_tabPriData.cbIsGoldOrGameScore == 1 then
                self.m_clipScore:setString(self.m_userItem.lScore - PriRoom:getInstance().m_tabPriData.lIniScore)
            end
        else
            self.m_clipScore:setString("0")
        end
    end

    if  GlobalUserItem.bVideo then
        self.m_clipScore:setString("0")
    end

    -- if GlobalUserItem.bVideo and PriRoom:getInstance().m_tabPriData.lIniScore then
    --     self.m_clipScore:setString(self.m_userItem.lScore - PriRoom:getInstance().m_tabPriData.lIniScore)
    -- end
end

function uiPlayerInfo:getScoreString( score )
    local strScore = ""
    if score < 100000 then
        strScore = strScore..score
    elseif score < 100000000 then
        -- print("分数转换1", score/10000)
        strScore = strScore..string.format("%.2f", score/10000).."万"
    else
        -- print("分数转换2", score/100000000)
        strScore = strScore..string.format("%.2f", score/100000000).."亿"
    end
    -- print("分数转换", score, strScore)
    return strScore
end

--这里有点搞笑,没有统一的数据处理流程,只能在ui刷新的地方取最保险
function uiPlayerInfo:getUserItem()
    return self.m_userItem
end

function uiPlayerInfo:onExit()
    self.m_actTip:release()
    self.m_actTip = nil
    self.m_actVoiceAni:release()
    self.m_actVoiceAni = nil

    self.isInited = false
end

return uiPlayerInfo