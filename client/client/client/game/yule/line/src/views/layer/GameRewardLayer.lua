local GameRewardLayer = class("GameRewardLayer",function (  )
    return ccui.Layout:create()
end)

local cmd = "game.yule.line.src.models.CMD_Game"
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local ButtonTag = 
{
    "Tag_BT_Reward",       
    "Tag_IM_Arrow",
    "Tag_Reward_List",--奖励表
    "Tag_BT_BG",        --背景层
    "Tag_Task_Num",     --任务完成数
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(0, ButtonTag);

GameRewardLayer.m_bpop = false --是否出现


function GameRewardLayer:ctor(data)
    self:setRewardData(data)
	self:initUI()
end

function GameRewardLayer:initUI(  )
	--注册触摸事件
    ExternalFun.registerTouchEvent(self, true)

    self:setTouchEnabled(true)
    self:setSwallowTouches(false)
    --回调方法
    local cbtlistener = function (sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:OnButtonClickedEvent(sender:getTag(),sender)
        end
    end

    self.colorLayer = cc.LayerColor:create(cc.c4b(0,0,0,100))
     self.colorLayer:setVisible(false)
    self:addChild( self.colorLayer)

    local viwesize = {width = appdf.WIDTH,height = appdf.HEIGHT}

    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("gamere/GameRewardLayer.csb", self)

    print("加载csb资源")

    local nodeReward = self._csbNode:getChildByName("Panel_reward")
    self.m_bgView =  nodeReward 
    self.m_spBg = nodeReward:getChildByName("im_bg")

    local pReward = nodeReward:getChildByName("Sprite_reward")

    local btnReward = nodeReward:getChildByName("Button_reward")
    btnReward:setTag(TAG_ENUM.Tag_BT_Reward)
    btnReward:addTouchEventListener(cbtlistener)


    --奖励表动画
    local animationReward = cc.Animation:create()
    for i=0,1 do
        local str = string.format("gamere/im_reward_%d.png",i)
        animationReward:addSpriteFrameWithFile(str)
    end
    animationReward:setDelayPerUnit(0.5)
    local animateReward = cc.Animate:create(animationReward)
    pReward:runAction(cc.RepeatForever:create(animateReward))
    --箭头
    self.m_arrow = nodeReward:getChildByName("sprite_arrow")
    self.m_arrow:setTag(TAG_ENUM.Tag_IM_Arrow)

    --三连
    local node_sanlian = nodeReward:getChildByName("im_list_frame_sanlian")
    for i=1,12 do
        local node = node_sanlian:getChildByName(string.format("FileNode_%d",i))
        print("node,i",node,i)
        self:setSanLian(node,i)

    end
    --九连
    local node_jiulian = nodeReward:getChildByName("im_list_frame_jiulian")
    for i=1,9 do
        local node = node_jiulian:getChildByName(string.format("FileNode_%d",i))
        self:setJiuLian(node,i)
    end
    --任务
    self.node_renwu = nodeReward:getChildByName("im_list_frame_rewu")
    self:setRewardTask()
end

function GameRewardLayer:setSanLian( node ,index)
    if index <= 10 then
        for i=1,3 do
            local sp = node:getChildByName(string.format("Sprite_%d",i))
            local frame = cc.SpriteFrame:create(string.format("gamere/im_element_%d.png",index-1),cc.rect(0,0,40,40))
            sp:setSpriteFrame(frame)
        end
    elseif index == 11 then
        local sp1 = node:getChildByName("Sprite_1")
        local frame1 = cc.SpriteFrame:create("gamere/im_element_9.png",cc.rect(0,0,40,40))
        sp1:setSpriteFrame(frame1)
        local sp2 = node:getChildByName("Sprite_2")
        local frame2 = cc.SpriteFrame:create("gamere/im_element_9.png",cc.rect(0,0,40,40))
        sp2:setSpriteFrame(frame2)
        local sp3 = node:getChildByName("Sprite_3")
        local frame3 = cc.SpriteFrame:create("gamere/im_element_10.png",cc.rect(0,0,40,40))
        sp3:setSpriteFrame(frame3)
    elseif index == 12 then
        local sp1 = node:getChildByName("Sprite_1")
        local frame1 = cc.SpriteFrame:create("gamere/im_element_9.png",cc.rect(0,0,40,40))
        sp1:setSpriteFrame(frame1)
        local sp2 = node:getChildByName("Sprite_2")
        local frame2 = cc.SpriteFrame:create("gamere/im_element_10.png",cc.rect(0,0,40,40))
        sp2:setSpriteFrame(frame2)
        local sp3 = node:getChildByName("Sprite_3")
        local frame3 = cc.SpriteFrame:create("gamere/im_element_10.png",cc.rect(0,0,40,40))
        sp3:setSpriteFrame(frame3)
    end
    local mupleNum = node:getChildByName("text_num")
    mupleNum:setString(self.m_rewardData.nThreeLineMultiple[1][index])
end

function GameRewardLayer:setJiuLian( node ,index)
    if index <=7 then
        local sp = node:getChildByName("sprite_1")
        local frame = cc.SpriteFrame:create(string.format("gamere/im_element_%d.png",index-1),cc.rect(0,0,40,40))
        sp:setSpriteFrame(frame)
    else
        local sp = node:getChildByName("sprite_1")
        local frame = cc.SpriteFrame:create(string.format("gamere/im_element_%d.png",index),cc.rect(0,0,40,40))
        sp:setSpriteFrame(frame)
    end
    local mupleNum = node:getChildByName("text_num")
    mupleNum:setString(self.m_rewardData.nNineLineMultiple[1][index])
end

--任务
function GameRewardLayer:setRewardTask()
    local node = self.node_renwu:getChildByName("FileNode")
    for i=1,3 do
        local frame = node:getChildByName(string.format("sprite_frame_%d",i))
        for j=1,3 do
            local sprite = frame:getChildByName(string.format("sprite_%d",j))
            local index = self.m_rewardData.nTaskTarget[1][i]
            local frame = cc.SpriteFrame:create(string.format("gamere/im_element_%d.png",index),cc.rect(0,0,40,40))
            sprite:setSpriteFrame(frame)
        end
        local text_num = frame:getChildByName("text_num")
        local str = self.m_rewardData.nTaskProgress[1][i] .."/" ..self.m_rewardData.nTaskCount[1][i]
        text_num:setString(str)
    end

    local text_leiji = self.m_bgView:getChildByName("text_leiji")
    text_leiji:setString(self.m_rewardData.nTaskCount[1][1])
    local text_ewai = self.m_bgView:getChildByName("text_ewai")
    text_ewai:setString(self.m_rewardData.nTaskScore[1][1])

end

function GameRewardLayer:updateRewardList( )
    local node = self.node_renwu:getChildByName("FileNode")
    for i=1,3 do
        local frame = node:getChildByName(string.format("sprite_frame_%d",i))
        local text_num = frame:getChildByName("text_num")
        local str = self.m_rewardData.nTaskProgress[1][i] .."/" ..self.m_rewardData.nTaskCount[1][i]
        text_num:setString(str)
    end
end

function GameRewardLayer:setRewardData( data )
    self.m_rewardData = data
end


--按钮回调方法
function GameRewardLayer:OnButtonClickedEvent( tag, sender )
    if TAG_ENUM.Tag_BT_Reward == tag then    --
        ExternalFun.playSoundEffect("OpenWidnow.wav")
        if self.m_bpop then
            self.m_bpop = false
             self.colorLayer:setVisible(false)
             self:setSwallowTouches(false)
            self.m_bgView:runAction(cc.MoveTo:create(0.3,cc.p(0,0))) 

            local frame = cc.SpriteFrame:create("gamere/bt_arrow_0.png",cc.rect(0,0,33,49))
            self.m_arrow:setSpriteFrame(frame) 
        else
            self.m_bpop = true
            self.colorLayer:setVisible(true)
             self:setSwallowTouches(true)
            self.m_bgView:runAction(cc.MoveTo:create(0.3,cc.p(944,0)))
            local frame = cc.SpriteFrame:create("gamere/bt_arrow_1.png",cc.rect(0,0,33,49))
            self.m_arrow:setSpriteFrame(frame) 
        end
    end
end

--触摸回调
function GameRewardLayer:onTouchBegan(touch, event)
    return self.colorLayer:isVisible()
end

function GameRewardLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self.m_bpop = false
        self.colorLayer:setVisible(false)
        self:setSwallowTouches(false)
        self.m_bgView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))  
        local frame = cc.SpriteFrame:create("gamere/bt_arrow_0.png",cc.rect(0,0,33,49))
        self.m_arrow:setSpriteFrame(frame) 
        ExternalFun.playSoundEffect("OpenWidnow.wav")
    end

end
return GameRewardLayer