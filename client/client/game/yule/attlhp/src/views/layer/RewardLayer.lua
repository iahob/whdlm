local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local RewardLayer = class("RewardLayer", cc.Layer)
-- local cmd = appdf.req(appdf.GAME_SRC.."yule.fairyworld.src.models.CMD_Game")

RewardLayer.BT_CLOSE = 1

--构造
function RewardLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true);
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("result/EndResultLayer.csb", self);
    self._csbNode:setPosition(0,0);

    --背景
    local sp_bg = self._csbNode:getChildByName("Panel_1");
    self.m_spBg = sp_bg;

    self.get_score = self._csbNode:getChildByName("get_score");
    -- 分数
    self.get_score:setString("0");
    
    
end

--播放效果
function RewardLayer:playGoldEfeect()
    --效果
    self:showGoldAnimation();
    self:showCoinBgAction();
    self:onGoldChangeEffect();

    --展示完自动移除
    -- self:removeLayer();
end

function RewardLayer:onButtonClickedEvent(tag, sender)
    if RewardLayer.BT_BEGIN == tag then
       
    end
end

-- 金币动画
function RewardLayer:showGoldAnimation()
    local node_gold = self._csbNode:getChildByName("node_gold");
    node_gold:removeAllChildren();

    -- 金币喷射
    local bigwingold = cc.ParticleSystemQuad:create("result/combigwingold.plist");
    bigwingold:setScale(0.6);
    bigwingold:setPosition(cc.p(815,435));
    bigwingold:setPositionType(cc.POSITION_TYPE_GROUPED);
    node_gold:addChild(bigwingold,12);

    -- bigwingold:runAction(cc.Sequence:create(
    --     cc.DelayTime:create(1.0),
    --     cc.CallFunc:create( function()
    --         bigwingold:setPositionType(cc.POSITION_TYPE_FREE);
    --     end ),
    --     cc.MoveTo:create(2.5,cc.p(226,160))
        
    -- ));
    -- -- 小奖
    -- local praticle = cc.ParticleSystemQuad:create("result/GOLDbigwin2.plist")
    -- praticle:setScale(2.0);
    -- praticle:setPosition(cc.p(900,375))
    -- praticle:setPositionType(cc.POSITION_TYPE_GROUPED)
    -- node_gold:addChild(praticle,3)
    -- praticle:setDuration(5);

    -- local praticle = cc.ParticleSystemQuad:create("result/GOLDbigwin.plist")
    -- -- local praticle = cc.ParticleSystemQuad:create("effect/GOLDbigwin2.plist")
    -- praticle:setScale(2.0);
    -- praticle:setPosition(cc.p(200,375))
    -- praticle:setPositionType(cc.POSITION_TYPE_GROUPED)
    -- node_gold:addChild(praticle,3)
    -- praticle:setDuration(5);
end

-- 金币背景
function RewardLayer:showCoinBgAction()
    local effect_1 = self._csbNode:getChildByName("effect_1");
    local effect_2 = self._csbNode:getChildByName("effect_2");
    local effect_3 = self._csbNode:getChildByName("effect_3");
    local bg = self._csbNode:getChildByName("bg");
    local get_score = self._csbNode:getChildByName("get_score");

    local rotate = cc.RotateBy:create(5.0,360);
    effect_1:runAction(cc.RepeatForever:create(rotate));

    -- self._csbNode:runAction(cc.RepeatForever:create(cc.Blink:create(1.0,10)))

    local moveTime = 0.05;
    bg:runAction(cc.RepeatForever:create(cc.Sequence:create(
        cc.MoveBy:create(moveTime,cc.p(5,5)),
        cc.MoveBy:create(moveTime,cc.p(-5,5)),
        cc.MoveBy:create(moveTime,cc.p(5,-5)),
        cc.MoveBy:create(moveTime,cc.p(-5,-5))
    )));
    get_score:runAction(cc.RepeatForever:create(cc.Sequence:create(
        cc.MoveBy:create(moveTime,cc.p(5,5)),
        cc.MoveBy:create(moveTime,cc.p(-5,5)),
        cc.MoveBy:create(moveTime,cc.p(5,-5)),
        cc.MoveBy:create(moveTime,cc.p(-5,-5))
    )));
end

--金币增加效果
function RewardLayer:onGoldChangeEffect()
    -- 金币增加效果
    local startScore = 0;
    local endScore = self.m_parent.m_PlayerWinScore;

    showScore = startScore;
    residue = endScore - startScore;

    local index = 0;
    self.goldSchedulerUpdate1 = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
        if residue > 100000000 then
            showScore = showScore + 1 + 10 + 100 + 1000 + 10000 + 100000 + 10000000 + 100000000;
            residue = residue - 1 - 10 - 100 - 1000 - 10000 - 100000 - 10000000 - 100000000;
        elseif residue > 10000000 then
            showScore = showScore + 1 + 10 + 100 + 1000 + 10000 + 100000 + 10000000;
            residue = residue - 1 - 10 - 100 - 1000 - 10000 - 100000 - 10000000;
        elseif residue > 1000000 then
            showScore = showScore + 1 + 10 + 100 + 1000 + 10000 + 100000 + 1000000;
            residue = residue - 1 - 10 - 100 - 1000 - 10000 - 100000 - 1000000;
        elseif residue > 100000 then
            showScore = showScore + 1 + 10 + 100 + 1000 + 10000 + 100000;
            residue = residue - 1 - 10 - 100 - 1000 - 10000 - 100000;
        elseif residue > 10000 then
            showScore = showScore + 1 + 10 + 100 + 1000 + 10000;
            residue = residue - 1 - 10 - 100 - 1000 - 10000;
        elseif residue > 1000 then
            showScore = showScore + 1 + 10 + 100 + 1000;
            residue = residue - 1 - 10 - 100 - 1000;
        elseif residue > 100 then
            showScore = showScore + 1 + 10 + 100;
            residue = residue - 1 - 10 - 100;
        elseif residue > 10 then
            showScore = showScore + 1 + 10;
            residue = residue - 1 - 10;
        elseif residue >= 0 then
            showScore = showScore + 1;
            residue = residue - 1;
        end

        if self.get_score then
            -- 分数
            self.get_score:setString(showScore);
            -- self.m_parent:showRewardFresh(showScore);
        end

        index = index + 1;
        if showScore >= endScore or index == 9 then

            -- 注销金币定时器
            if self.goldSchedulerUpdate1 then
                -- error("",0)
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.goldSchedulerUpdate1)
                self.goldSchedulerUpdate1 = nil;
            end
            
            self.get_score:setString(endScore);
            -- self.m_parent:showRewardFresh(endScore);
        end

    end, 0.1, false)
end

function RewardLayer:showLayer( var )
    self:setVisible(var)
end

function RewardLayer:removeLayer()
    local delayTime = cc.DelayTime:create(0.1);
    local fadeout = cc.FadeOut:create(0.1);
    local call = cc.CallFunc:create(function( )
        self.m_parent.rewardLayer = nil;
        self:removeFromParent();
    end)
    self._csbNode:runAction(cc.Sequence:create(delayTime,fadeout, call));
end

--触摸回调
function RewardLayer:onTouchBegan(touch, event)
    local pos = touch:getLocation();
    local m_spBg = self.m_spBg;
    pos = m_spBg:convertToNodeSpace(pos);
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height);
    if false == cc.rectContainsPoint(rec, pos) then
        -- local fadeout = cc.FadeOut:create(0.5);
        -- local call = cc.CallFunc:create(function( )
        --     self.m_parent.rewardLayer = nil;
        --     self:removeFromParent();
        -- end)
        -- self._csbNode:runAction(cc.Sequence:create(fadeout, call));
    end
end

function RewardLayer:onTouchEnded(touch, event)

end

function RewardLayer:onExit()
    if self.goldSchedulerUpdate1 then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.goldSchedulerUpdate1)
        self.goldSchedulerUpdate1 = nil;
    end
end

return RewardLayer