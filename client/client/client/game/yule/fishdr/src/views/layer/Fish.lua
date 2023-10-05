--
-- Author: chen
-- 
--鱼

local Fish = class("Fish",function(fishData,target)
	local fish =  display.newSprite()
	return fish
end)

local FISHTAG = 
{
	TAG_GUAN = 10
}

local fishCloneTag = 3000;

local module_pre = "game.yule.fishdr.src"			
local ExternalFun = require(appdf.EXTERNAL_SRC.."ExternalFun")
-- local cmd = module_pre..".models.CMD_LKGame"
local cmd = ExternalFun.req_var(module_pre ..".models.CMD_LKGame")
local g_var = ExternalFun.req_var
local scheduler = cc.Director:getInstance():getScheduler()

local shadow = {}
shadow.fsh = [[
       varying vec2 v_texCoord;
		void main(){
		float shadowMask = texture2D(CC_Texture0,v_texCoord).a;
		const float shadowOpacity = 0.3;
		gl_FragColor = vec4(0,0,0,shadowMask*shadowOpacity);
		}
    ]]
shadow.vsh =[[
        attribute vec4 a_position; 
		attribute vec2 a_texCoord; 
		 
		varying vec2 v_texCoord; 
		void main() 
		{
		gl_Position = CC_PMatrix * a_position; 
		v_texCoord = a_texCoord; 
		}
    ]]

-- shadow.fsh = [[
--         varying vec2 v_texCoord;
 
-- 		vec4 composite(vec4 over, vec4 under)
-- 		{
-- 		return over + (1 - over.a)*under;
-- 		}
-- 		void main(){
-- 		vec2 shadowOffset =vec2(-0.005, -0.005);
-- 		vec4 textureColor = texture2D(CC_Texture0,v_texCoord );
-- 		float shadowMask = texture2D(CC_Texture0,v_texCoord +shadowOffset ).a;
-- 		const float shadowOpacity = 0.5;
-- 		vec4 shadowColor = vec4(0,0,0,shadowMask*shadowOpacity);
-- 		gl_FragColor = composite(textureColor,shadowColor);
-- }

function Fish:ctor(fishData,target)
	self.m_bezierArry = {}
	self.fishCreateData = fishData
	self._scene = target
	self.m_schedule = nil

	self.m_data = fishData   		
	self.m_producttime = fishData.nProductTime
	self.m_ydtime = 0 			--鱼游动时间
	self.m_pathIndex = 1
	self.m_nQucikClickNum = 0
	self.m_fTouchInterval = 0
	self:setPosition(cc.p(-500,-500))
	self:setTag(cmd.Tag_Fish)
	self._dataModel = target._dataModel

	self:initWithType(self.fishCreateData,target)

	 --注册事件
    ExternalFun.registerTouchEvent(self,true)
end

function Fish:cleanEffect()
	if self.praticle then
		self.praticle:removeAllChildren();
        self.praticle:removeFromParent();
        self.praticle = nil;
    end

    local fishClone = self:getParent():getChildByTag(self.m_data.nFishKey);
    if fishClone then
    	fishClone:removeAllChildren();
    	fishClone:removeFromParent();
    end
end

function Fish:schedulerUpdate()
	print("fish update ")

	local function updateFish(dt)

		self.m_ydtime = self.m_ydtime + dt * 1000
		local bezier =  self.m_data.TBzierPoint[1] -- table
		local tbp =  bezier[self.m_pathIndex]

		while self.m_ydtime > tbp.Time  do
			self.m_ydtime = self.m_ydtime - tbp.Time
			self.m_pathIndex = self.m_pathIndex + 1
		end

		--鱼阴影
		local fishClone = self:getParent():getChildByTag(self.m_data.nFishKey);

		--切换场景清除所有鱼
		if self.m_data.nBezierCount <= self.m_pathIndex-1 or (self._dataModel._exchangeSceneing and self._dataModel._isCleanFish)then 
			if self.m_data.wHitChair == self._scene.m_nChairID  and self.m_data.nFishType == cmd.FishType.FishType_BaoXiang then
				self._dataModel.m_bBoxTime = false
			end
			-- self._dataModel.m_fishList[self.m_data.nFishKey] = nil
			-- self._dataModel.m_InViewTag[self.m_data.nFishKey] = nil

			-- self:unScheduleFish()
			-- self:removeAllChildren();
			-- self:removeFromParent()

			--渐变消失
			self._dataModel.m_fishList[self.m_data.nFishKey] = nil
			self._dataModel.m_InViewTag[self.m_data.nFishKey] = nil

			self:unScheduleFish()

	   		self:setOpacity(255)
	   		local callfunc = cc.CallFunc:create(function()
				-- self._dataModel.m_fishList[self.m_data.nFishKey] = nil
				-- self._dataModel.m_InViewTag[self.m_data.nFishKey] = nil

				-- self:unScheduleFish()

				self:cleanEffect();
				
				self:removeAllChildren();
				self:removeFromParent()
			end)
	   		self:runAction(cc.Sequence:create(cc.FadeTo:create(1.0,0), callfunc));

	   		--鱼阴影渐变消失
	   		if fishClone then
	   			fishClone:runAction(cc.FadeTo:create(1.0,0));
	   		end

			--print("******************fish removeFromParent********************")
			return
		end

		--路径百分比
		local percent = self.m_ydtime/tbp.Time

		local point = self:PointOnCubicBezier(self.m_pathIndex,percent)

		if self.m_data.fRotateAngle then
			local bzierpoint = bezier[1]
			local beginVec2 = cc.p(bzierpoint.BeginPoint.x,bzierpoint.BeginPoint.y)
			point = self:RotatePoint(beginVec2,self.m_data.fRotateAngle,point)
		end

		point = cc.p(point.x+self.m_data.PointOffSet.x,point.y+self.m_data.PointOffSet.y)
		local m_oldPoint = cc.p(self:getPositionX(),self:getPositionY())
		self:setConvertPoint(point)


		if cc.rectContainsPoint( cc.rect(0,0,yl.WIDTH, yl.HEIGHT), point ) then
			if self.m_data.nFishType ~= cmd.FishType.FishType_BaoXiang then
				self._dataModel._bFishInView = true
				self._dataModel.m_InViewTag[self.m_data.nFishKey] = 1
			end
		else
		    self._dataModel._bFishInView = false
		    self._dataModel.m_InViewTag[self.m_data.nFishKey] = nil
		end

		local nowPos = cc.p(self:getPositionX(),self:getPositionY())
		local angle = self._dataModel:getAngleByTwoPoint(nowPos,m_oldPoint)

		local fishAngle = angle;
		-- 元宝鱼方向
		if self.m_data.nFishType == cmd.FishType.FishType_YuanBao then
			self:setRotation(angle);
		else
			self:setRotation(180 + angle);

			fishAngle = 180 + angle;
		end

		-----------------------------------------------------------------
		local pos = cc.p(self:getPositionX(),self:getPositionY());

		if pos.x < m_oldPoint.x and not self._dataModel.m_reversal  then
			self:setFlippedX(true)

			if fishClone then
				fishClone:setFlippedX(true);
			end
		elseif pos.x < m_oldPoint.x and self._dataModel.m_reversal  then
			self:setFlippedX(false)

			if fishClone then
				fishClone:setFlippedX(false);
			end
		elseif pos.x > m_oldPoint.x and self._dataModel.m_reversal then
			self:setFlippedX(true)

			if fishClone then
				fishClone:setFlippedX(true);
			end
		--鱼阵鳄鱼特殊显示
		elseif pos.x == m_oldPoint.x and self._dataModel.m_reversal and self.m_data.nFishType == cmd.FishType.FishType_LiKui then
			self:setFlippedX(true)

			if fishClone then
				fishClone:setFlippedX(true);
			end
		else
			self:setFlippedX(false)

			if fishClone then
				fishClone:setFlippedX(false);
			end
		end

		-----------------------------------------------------------------
		local contentSize = self:getContentSize();
		local fishW = contentSize.width;
		local fishH = contentSize.height;

		local fishPos = cc.p( pos.x + fishW/2, pos.y - fishH/2);
	    if self.m_data.nFishType > 2 and self.m_data.nFishType <=11 then
	    	fishPos = cc.p( pos.x + fishW/3, pos.y - fishH/3);
	    elseif self.m_data.nFishType > 11 and self.m_data.nFishType <=20  then
	    	fishPos = cc.p( pos.x + fishW/4, pos.y - fishH/4);
	    elseif self.m_data.nFishType > 20 and self.m_data.nFishType <24  then
	    	fishPos = cc.p( pos.x + fishW/3, pos.y - fishH/3);
	    elseif self.m_data.nFishType == 24 then
	    	fishPos = cc.p( pos.x + fishW/3, pos.y - fishH/3);
	    end

		if self._dataModel.m_reversal then
			fishPos = cc.p( pos.x - fishW/2, pos.y + fishH/2);
		    if self.m_data.nFishType > 2 and self.m_data.nFishType <=11 then
		    	fishPos = cc.p( pos.x - fishW/3, pos.y + fishH/3);
		    elseif self.m_data.nFishType > 11 and self.m_data.nFishType <=20  then
		    	fishPos = cc.p( pos.x - fishW/4, pos.y + fishH/4);
		    elseif self.m_data.nFishType > 20 and self.m_data.nFishType <24  then
		    	fishPos = cc.p( pos.x - fishW/3, pos.y + fishH/3);
		    elseif self.m_data.nFishType == 24 then
		    	fishPos = cc.p( pos.x - fishW/3, pos.y + fishH/3);
		    end
		end

		if fishClone then
			if self.m_data.nFishType == cmd.FishType.FishType_BaoZhaFeiBiao or self.m_data.nFishType == cmd.FishType.FishType_ZhongYiTang 
		   	or self.m_data.nFishType == cmd.FishType.FishType_ShuiHuZhuan or self.m_data.nFishType == cmd.FishType.FishType_DaBaiSha 
		   	or self.m_data.nFishType == cmd.FishType.FishType_ShuangTouQiEn then
		   		
		   		fishPos = cc.p( pos.x + fishW/3, pos.y - fishH/3);
		   		if self._dataModel.m_reversal then
		   			fishPos = cc.p( pos.x - fishW/3, pos.y + fishH/3);
		   		end

		   		local fish = self:getChildByTag(100);
		   		if fish then
		   			fishAngle = fish:getRotation() + fishAngle;
		   			fishClone:setRotation(fishAngle);
		   		end
		   	else
		   		fishClone:setRotation(fishAngle);
		    end
			
			fishClone:setPosition(fishPos);
		end
		-- -- --世界坐标转换为本地坐标
		-- -- local pos = self.nodeHandCard:convertToNodeSpace(pos)
		-- -- -- 本地坐标转换为世界坐标系
		-- -- local point = spriteOne:convertToWorldSpaceAR(ccp(x,y))
		----------------------------------------------------------------
	end

	--定时器
	if nil == self.m_schedule then
		self.m_schedule = scheduler:scheduleScriptFunc(updateFish, 0, false)
	end
end

function Fish:unScheduleFish()
	if nil ~= self.m_schedule then
		scheduler:unscheduleScriptEntry(self.m_schedule)
		self.m_schedule = nil
	end
end

function Fish:onEnter()
	local time = currentTime()
	self.m_ydtime = time - self.m_producttime
	self:schedulerUpdate()
end

function Fish:onExit( )

	self._dataModel.m_InViewTag[self.m_data.nFishKey] = nil
	self:unScheduleFish()
end

function Fish:onTouchBegan(touch, event)
	print("fish touch began")

	local point = touch:getLocation()
	point = self:convertToNodeSpace(point)

	-- 缩小鱼的实际触摸范围
	local scale = 1.0;
	local x = 0;
	local y = 0;

	if self.m_data.nFishType >= cmd.FishType.FishType_LingDangYu and self.m_data.nFishType ~= cmd.FishType.FishType_LiKui then
		scale = 0.8;
		x = 20;
		y = 20;
	end

	local rect = self:getBoundingBox()
	-- rect = cc.rect(x,y,rect.width*scale,rect.height*scale)
	rect = cc.rect(x,y,rect.height*scale,rect.width*scale)  

	return cc.rectContainsPoint( rect, point )  

end

function Fish:onTouchEnded(touch, event )
	--切换成当前锁定目标
	self._dataModel.curSelectfishIndex = self.m_data.nFishKey;
	self._dataModel.m_fishIndex	= self.m_data.nFishKey
	print("fish touch ended")
end


function Fish:initWithType( param,target)
	self:initBezierConfig(param)
end

function Fish:initBezierConfig( param )
	
	if type(param) ~= "table" then
		print("传入参数类型有误, the param should be a type of table")
		return
	end

	if not param.bRepeatCreate then --按原路径返回
		
		local beziers =  param.TBzierPoint[1] -- table
		local tmp = {} 
		for i=1,#beziers-1 do
			tmp[i] = beziers[i]
		end

		for i=1,#tmp do
			local config = tmp[i]
			table.insert(beziers,#tmp+2,config)
		end
		tmp = {}
		self.m_data.TBzierPoint[1] = beziers

	end

	for i=1,param.nBezierCount do
		local bezier =  param.TBzierPoint[1] -- table
		local tbp =  bezier[i]
	
		local bconfig = 
		{
			dAx = 0,
			dBx = 0,
			dCx = 0,
			dAy = 0,
			dBy = 0,
			dCy = 0
		}
	
		bconfig.dCx = 3.0 * (tbp.KeyOne.x - tbp.BeginPoint.x)
		bconfig.dBx = 3.0 * (tbp.KeyTwo.x - tbp.KeyOne.x) - bconfig.dCx
		bconfig.dAx = tbp.EndPoint.x - tbp.BeginPoint.x - bconfig.dCx - bconfig.dBx

		bconfig.dCy = 3.0 * (tbp.KeyOne.y - tbp.BeginPoint.y)
		bconfig.dBy = 3.0 * (tbp.KeyTwo.y - tbp.KeyOne.y) - bconfig.dCy
		bconfig.dAy = tbp.EndPoint.y - tbp.BeginPoint.y - bconfig.dCy - bconfig.dBy
		table.insert(self.m_bezierArry, bconfig)
	end
end

function Fish:PointOnCubicBezier(pathIndex,t)

	local bconfig = 
		{
			dAx = 0,
			dBx = 0,
			dCx = 0,
			dAy = 0,
			dBy = 0,
			dCy = 0
		}

    local result = {}
	local tSquard = 0
	local tCubed = 0
	local param = self.m_data
	bconfig = self.m_bezierArry[pathIndex]

	local bezier =  param.TBzierPoint[1] -- table
	local tbp =  bezier[pathIndex]
	tSquard = t*t
	tCubed = tSquard*t
	result.x = (bconfig.dAx * tCubed) + (bconfig.dBx * tSquard) + (bconfig.dCx * t) + tbp.BeginPoint.x
	result.y = (bconfig.dAy * tCubed) + (bconfig.dBy * tSquard) + (bconfig.dCy * t) + tbp.BeginPoint.y

	return result
end

function Fish:RotatePoint(pcircle,dradian,ptsome)

	local tmp = {}
	ptsome.x = ptsome.x - pcircle.x
	ptsome.y = ptsome.y - pcircle.y

	tmp.x = ptsome.x*math.cos(dradian) - ptsome.y*math.sin(dradian) + pcircle.x
	tmp.y = ptsome.x * math.sin(dradian) + ptsome.y*math.cos(dradian) + pcircle.y

	return tmp
end

function Fish:initAnim(m_fishLayer)
	local namestr 
	local aniName
	if self.m_data.nFishType ~= cmd.FishType.FishType_YuanBao then
		namestr = string.format("%d_01.png", cmd.fishFrameMoveType[self.m_data.nFishType+1])
		aniName = string.format("animation_fish_move%d", self.m_data.nFishType+1)
	else
		namestr = "fishMove_ignot_1.png"
		aniName = "fish_ignot_move"
	end	

	if namestr then
		self:initWithSpriteFrameName(namestr)
	end
	
	
	local animation = cc.AnimationCache:getInstance():getAnimation(aniName)
	if nil ~= animation then
	   -- if self.m_data.nFishType == cmd.FishType.FishType_BaoZhaFeiBiao then
	   if self.m_data.nFishType == cmd.FishType.FishType_BaoZhaFeiBiao or self.m_data.nFishType == cmd.FishType.FishType_ZhongYiTang 
	   	or self.m_data.nFishType == cmd.FishType.FishType_ShuiHuZhuan or self.m_data.nFishType == cmd.FishType.FishType_DaBaiSha 
	   	or self.m_data.nFishType == cmd.FishType.FishType_ShuangTouQiEn then
	   	  	self:setOpacity(0)
	   	  	-- self:runAction(cc.FadeTo:create(0.5,255))

	   	  	-- local rotate = cc.RotateBy:create(8.5,360)
	   	  	local rotate = cc.RotateBy:create(6.0,360)
			local repeatation = cc.RepeatForever:create(rotate)
			local copySelf = cc.Sprite:createWithSpriteFrameName(namestr)
			if nil ~= copySelf then
				local contentSize = self:getContentSize()
				copySelf:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
				copySelf:setTag(100);

				if self.m_data.nFishType == cmd.FishType.FishType_ShuiHuZhuan then
					local rotate = cc.RotateBy:create(4.0,360)
					local repeatation = cc.RepeatForever:create(rotate)

					local fishCicle = cc.Sprite:createWithSpriteFrameName("fish_circle.png");
					fishCicle:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					self:addChild(fishCicle);
					fishCicle:runAction(repeatation)
				else
					-- copySelf:runAction(repeatation);
				end

				copySelf:runAction(repeatation)
				self:addChild(copySelf, 1)

				copySelf:runAction(cc.FadeTo:create(1.0,255))

				-- 鱼阴影
				local shader_program = cc.GLProgram:createWithByteArrays(shadow.vsh , shadow.fsh)
				local fishClone = cc.Sprite:createWithSpriteFrameName(namestr)
				fishClone:setGLProgram(shader_program);
				-- fishClone:setPosition(cc.p(-10, 20));
				-- fishClone:setAnchorPoint(0,0);
				fishClone:setTag(self.m_data.nFishKey);
				fishClone:setScale(0.6);
				m_fishLayer:addChild(fishClone, -1);

				fishClone:runAction(cc.RepeatForever:create(cc.RotateBy:create(6.0,360)));
				--渐变出现
		   		fishClone:setOpacity(0)
		   		fishClone:runAction(cc.FadeTo:create(1.0,255))
			end
	   else
	   		local action = cc.RepeatForever:create(cc.Animate:create(animation))
	   		self:runAction(action)
	   		--渐变出现
	   		self:setOpacity(0)
	   		self:runAction(cc.FadeTo:create(1.0,255))

	  		-- 鱼阴影
			local shader_program = cc.GLProgram:createWithByteArrays(shadow.vsh , shadow.fsh)
			local fishClone = cc.Sprite:createWithSpriteFrameName(namestr)
			fishClone:setGLProgram(shader_program);
			-- fishClone:setPosition(cc.p(-30, 20));
			-- fishClone:setAnchorPoint(0,0);
			fishClone:setTag(self.m_data.nFishKey);
			fishClone:setScale(0.6);
			local action2 = cc.RepeatForever:create(cc.Animate:create(animation));
			fishClone:runAction(action2);
			-- self:addChild(fishClone, -1);
			m_fishLayer:addChild(fishClone, -1);
			--渐变出现
	   		fishClone:setOpacity(0)
	   		fishClone:runAction(cc.FadeTo:create(1.0,255))

			
    		--金色鱼尾焰
    		if self.m_data.nFishType == cmd.FishType.FishType_DaJinSha or self.m_data.nFishType == cmd.FishType.FishType_JuXingHuangJinSha or self.m_data.nFishType == cmd.FishType.FishType_JinLong then
    			--尾焰效果
                self:showFishEffect()
    		end

	   		--金蟾显示
	   		if self.m_data.nFishType == cmd.FishType.FishType_JinLong then
	   			local rotate = cc.RotateBy:create(5.0,360)
				local repeatation = cc.RepeatForever:create(rotate)
				local copySelf = cc.Sprite:createWithSpriteFrameName("27_round.png")
				if nil ~= copySelf then
					local contentSize = self:getContentSize()
					copySelf:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					copySelf:setTag(100);
					copySelf:runAction(repeatation)
					self:addChild(copySelf, 1)
				end

				--阴影
				local copyClone = cc.Sprite:createWithSpriteFrameName("27_round.png")
				if nil ~= copyClone then
					copyClone:setGLProgram(shader_program);
					local contentSize = self:getContentSize()
					copyClone:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					fishClone:addChild(copyClone, 1)
					copyClone:runAction(cc.RepeatForever:create(cc.RotateBy:create(5.0,360)));
				end
	   		end
	   end
	end
end

--死亡处理
function Fish:deadDeal()
	self:setColor(cc.WHITE)
	self:stopAllActions()
	self:unScheduleFish()
	self:getPhysicsBody():setCategoryBitmask(8)

	local aniName 
	if self.m_data.nFishType ~= cmd.FishType.FishType_YuanBao then
		aniName = string.format("animation_fish_dead%d",self.m_data.nFishType+1)
	else
		aniName = "fish_ignot_dead"
	end

	local ani = cc.AnimationCache:getInstance():getAnimation(aniName)
	local parent = self:getParent()

	--爆炸飞镖
	if self.m_data.nFishType == cmd.FishType.FishType_BaoZhaFeiBiao then
		local nKnife = 18
		local angle = 20
		local radius = 1200
		for i=1,nKnife do
			local sKnife = cc.Sprite:create("game_res/knife.png")
			sKnife:setAnchorPoint(0,0.5)
			sKnife:setPosition(self:getPositionX(),self:getPositionY())
			local pos = cc.p(self:getPositionX(),self:getPositionY())
			local purPos = cc.p(0,0)
			purPos.x = pos.x + radius * self._dataModel.m_cosList[20*i]
			purPos.y = pos.y + radius * self._dataModel.m_sinList[20*i]
			purPos = self._dataModel:convertCoordinateSystem(purPos, 1, self._dataModel.m_reversal)
			local callfunc = cc.CallFunc:create(function()
				sKnife:removeFromParent()
			end)

			sKnife:runAction(cc.Sequence:create(cc.MoveTo:create(1.5,purPos),callfunc))
			parent:addChild(sKnife)

		
			 local angle = math.atan2((purPos.y - sKnife:getPositionY()), (purPos.x - sKnife:getPositionX())) / 3.14 * 180
			 if angle < 0 then
			 	angle = angle - (90 + angle)
			 	if self._dataModel.m_reversal then
			 		angle = angle + 180
			 	end
			 	sKnife:setRotation(angle + 180)
			 else
			 	angle = 90 - angle 
			 	if self._dataModel.m_reversal then
			 		angle = angle + 180
			 	end
			 	sKnife:setRotation(angle + 180)
			 end
		end
		--全屏鱼的位置出现飞镖爆炸
		local nBoom = 14 + math.random(5)
		local delayTime = 0
		for i=1,nBoom do
			delayTime = math.random(4) * 0.13

			local boomAnim = cc.AnimationCache:getInstance():getAnimation("BombDartsAnim")
			local bomb = cc.Sprite:createWithSpriteFrameName("boom_darts0.png")
			local pos = cc.p(0,0)
			pos.x = 100 + math.random(1234)
			pos.y = 100 + math.random(650)
			bomb:setPosition(pos.x,pos.y)

			local call = cc.CallFunc:create(function()
				bomb:removeFromParent()
			end)

			local delayAction = cc.DelayTime:create(delayTime)
			local action = cc.Sequence:create(delayAction,cc.Animate:create(boomAnim),call)
			bomb:runAction(action)
			parent:addChild(bomb,41)

		end
	end

	if self.m_data.nFishType == cmd.FishType.FishType_ShuiHuZhuan then
		--冰冻全屏动画
		self:bingdongSpin();

		-- local radius = 580
		-- local nBomb = 12

		-- local pos = cc.p(self:getPositionX(),self:getPositionY())
		-- for i=1,nBomb do
		-- 	local boomAnim = cc.AnimationCache:getInstance():getAnimation("BlueIceAnim")
		-- 	local bomb = cc.Sprite:createWithSpriteFrameName("blue00.png")
		-- 	bomb:setPosition(self:getPositionX(),self:getPositionY())
		-- 	bomb:runAction(cc.Animate:create(boomAnim))
		-- 	parent:addChild(bomb,40)
		-- 	if boomAnim then
		-- 		local purPos = cc.p(0,0)
		-- 		purPos.x = pos.x + radius * self._dataModel.m_cosList[30*i]
		-- 		purPos.y = pos.y + radius * self._dataModel.m_sinList[30*i]

		-- 		local move = cc.MoveTo:create(0.8,purPos)

		-- 		local call = cc.CallFunc:create(function()	
		-- 			bomb:removeFromParent()
		-- 		end)

		-- 		bomb:runAction(cc.Sequence:create(move,call))
		-- 	end
		-- end
	end

	if self.m_data.nFishType == cmd.FishType.FishType_ZhongYiTang then
		--爆炸鱼动画
		local pos = cc.p(self:getPositionX(),self:getPositionY());
		self:zhadanSpin(parent, pos);
	end

	if (self.m_data.nFishType >=  cmd.FishType.FishType_JianYu and self.m_data.nFishType <=  cmd.FishType.FishType_LiKui) or self.m_data.nFishType ==  cmd.FishType.FishType_BaoZhaFeiBiao then
		
		local radius = 360
		local nBomb = 1
		if self.m_data.nFishType >=  cmd.FishType.FishType_JianYu and self.m_data.nFishType <=  cmd.FishType.FishType_DaJinSha then
			nBomb = 1
		elseif self.m_data.nFishType >  cmd.FishType.FishType_DaJinSha and self.m_data.nFishType <=  cmd.FishType.FishType_LiKui then
			nBomb = 6
		elseif self.m_data.nFishType ==  cmd.FishType.FishType_BaoZhaFeiBiao then
			nBomb = 8
			radius = 580
			
		end

		local pos = cc.p(self:getPositionX(),self:getPositionY())

		for i=1,nBomb do
			local boomAnim = cc.AnimationCache:getInstance():getAnimation("BombAnim")
			local bomb = cc.Sprite:createWithSpriteFrameName("boom00.png")
			bomb:setPosition(pos.x,pos.y)
			bomb:runAction(cc.Animate:create(boomAnim))
			parent:addChild(bomb,40)

			if boomAnim then
				local action = nil
				if nBomb == 1 then
					action = cc.DelayTime:create(0.8)
				else
					local purPos = cc.p(0,0)
					purPos.x = pos.x + self._dataModel.m_cosList[360/nBomb*i]
					purPos.y = pos.y + self._dataModel.m_sinList[360/nBomb*i]
					purPos = self._dataModel:convertCoordinateSystem(purPos, 2, self._dataModel.m_reversal)
					action = cc.MoveTo:create(0.8,purPos)

				end
				local call = cc.CallFunc:create(function()
					bomb:removeFromParent()
				end)
				bomb:runAction(cc.Sequence:create(action,call))
			end
		end
	end
	self._dataModel.m_fishList[self.m_data.nFishKey] = nil
	
	if nil ~= ani then

		local times = 4
		if self.m_data.nFishType == cmd.FishType.FishType_YuanBao then
			times = 1

			local repeats = cc.Repeat:create(cc.Animate:create(ani),times)
			local call = cc.CallFunc:create(function()	
			   	self:unScheduleFish()

			   	self:cleanEffect();
			   	self:removeAllChildren();
	  	       	self:removeFromParent()
	   		end)
			local action = cc.Sequence:create(repeats,call)
			self:runAction(action)
		else
			local rotate1 = cc.RotateBy:create(0.05, 45);
			local rotate2 = cc.RotateBy:create(0.05, -45);
			local seq = cc.Sequence:create(rotate1, rotate2, rotate2, rotate1);

			local repeats = cc.Repeat:create(seq,times)
			local call = cc.CallFunc:create(function()	
			   	self:unScheduleFish()

			   	self:cleanEffect();
			   	self:removeAllChildren();
	  	       	self:removeFromParent()
	   		end)
			local action = cc.Sequence:create(repeats,call)
			self:runAction(action)

			--鱼阴影死亡动画
			local rotate3 = cc.RotateBy:create(0.05, 45);
			local rotate4 = cc.RotateBy:create(0.05, -45);
			local seq2 = cc.Sequence:create(rotate3, rotate4, rotate4, rotate3);
			local repeats2 = cc.Repeat:create(seq2,times)
			local fishClone = self:getParent():getChildByTag(self.m_data.nFishKey);
			fishClone:runAction(repeats2);
		end

	else
		self:unScheduleFish()

		self:cleanEffect();
		self:removeAllChildren();
		self:removeFromParent()
	end
end

--设置物理属性
function Fish:initPhysicsBody()
	local fishtype = self.fishCreateData.nFishType
	local body = self._dataModel:getBodyByType(fishtype)

	if body == nil then
		print("body is nil.......")
		return
	end

	self:setPhysicsBody(body)

--设置刚体属性
    self:getPhysicsBody():setCategoryBitmask(1)
    self:getPhysicsBody():setCollisionBitmask(0)
    self:getPhysicsBody():setContactTestBitmask(2)
    self:getPhysicsBody():setGravityEnable(false)
end

function Fish:initWithState(m_fishLayer)
	
	local fishstate = self.fishCreateData.nFishState
	if  fishstate ~= cmd.FishState.FishState_Normal then
		local contentsize = self:getContentSize()

		--阴影
		local fishClone = m_fishLayer:getChildByTag(self.m_data.nFishKey);
		local shader_program = cc.GLProgram:createWithByteArrays(shadow.vsh , shadow.fsh)

		if fishstate == cmd.FishState.FishState_King then
			local guan = cc.Sprite:createWithSpriteFrameName("fish_king.png")
			guan:setPosition(cc.p(contentsize.width/2,contentsize.height/2))
			self:addChild(guan, -1)
			guan:runAction(cc.RepeatForever:create(cc.RotateBy:create(8.5,360)))

			--阴影
		    if fishClone then
		    	local copyClone = cc.Sprite:createWithSpriteFrameName("fish_king.png")
				if nil ~= copyClone then
					copyClone:setGLProgram(shader_program);
					local contentSize = self:getContentSize()
					copyClone:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					fishClone:addChild(copyClone, -1)
					copyClone:runAction(cc.RepeatForever:create(cc.RotateBy:create(8.5,360)));
				end
		    end

		elseif fishstate == cmd.FishState.FishState_Killer then
			local guan = cc.Sprite:createWithSpriteFrameName("fish_bomb_2.png")
			guan:setPosition(cc.p(contentsize.width/2,contentsize.height/2))
			self:addChild(guan, -1)
			guan:runAction(cc.RepeatForever:create(cc.RotateBy:create(8.5,360)))

			--阴影
		    if fishClone then
		    	local copyClone = cc.Sprite:createWithSpriteFrameName("fish_bomb_2.png")
				if nil ~= copyClone then
					copyClone:setGLProgram(shader_program);
					local contentSize = self:getContentSize()
					copyClone:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					fishClone:addChild(copyClone, -1)
					copyClone:runAction(cc.RepeatForever:create(cc.RotateBy:create(8.5,360)));
				end
		    end

		elseif fishstate == cmd.FishState.FishState_Aquatic then
			local guan = cc.Sprite:createWithSpriteFrameName("35_01.png")
			local  anr  = guan:getAnchorPoint()
			guan:setPosition(cc.p(contentsize.width/2,contentsize.height/2))
			guan:setTag(100);
			self:addChild(guan, 3)
			local anim = cc.AnimationCache:getInstance():getAnimation("animation_fish_move26")
			local action1 = cc.Repeat:create(cc.Animate:create(anim), 999999)
			local action2 = cc.Repeat:create(cc.RotateBy:create(8.5,360), 999999)
			guan:runAction(action1)
			guan:runAction(action2)

			--阴影
		    if fishClone then
		    	local copyClone = cc.Sprite:createWithSpriteFrameName("35_01.png")
				if nil ~= copyClone then
					copyClone:setGLProgram(shader_program);
					local contentSize = self:getContentSize()
					copyClone:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
					fishClone:addChild(copyClone, 3)
					copyClone:runAction(cc.Repeat:create(cc.RotateBy:create(8.5,360), 999999));
				end
		    end
		end
	end
end

--转换坐标
function  Fish:setConvertPoint( point )
		
	 local WIN32_W = 1280
	 local WIN32_H = 800

	 local scalex = yl.WIDTH/WIN32_W
	 local scaley = yl.HEIGHT/WIN32_H

	 local pos = cc.p(point.x*scalex,(WIN32_H-point.y)*scaley) 
	 self:setPosition(pos)
end

--鱼停留
function Fish:Stay(time)
	self:unScheduleFish()
	local call = cc.CallFunc:create(function()	
		self:schedulerUpdate()
	end)
	local delay = cc.DelayTime:create(time/1000)

	self:runAction(cc.Sequence:create(delay,call))
end

--冰冻鱼动画
function Fish:bingdongSpin()
	local skeletonNode = sp.SkeletonAnimation:create("spine/bingdong/skeleton.json","spine/bingdong/skeleton.atlas",1)
    skeletonNode:setPosition(cc.p(667, 375))
    -- skeletonNode:setPosition(pos)
    skeletonNode:setAnimation(0, "animation", false);    --true是指循环播放walk动作
    -- self._scene._gameView:addChild(skeletonNode);
    local parent = self:getParent()
    parent:addChild(skeletonNode);

    skeletonNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0),cc.CallFunc:create(function()
		skeletonNode:removeAllChildren();
        skeletonNode:removeFromParent();
	end)));
end

--炸弹鱼动画
function Fish:zhadanSpin(parent, pos)
    skeletonNode = sp.SkeletonAnimation:create("spine/zhadan/skeleton.json","spine/zhadan/skeleton.atlas",1)
    skeletonNode:setPosition(pos)
    skeletonNode:setAnimation(0, "animation", false);    --true是指循环播放walk动作
	-- skeletonNode:registerSpineEventHandler(function (event)
 --  	end, sp.EventType.ANIMATION_END)
    parent:addChild(skeletonNode, 100)

    skeletonNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0),cc.CallFunc:create(function()
		skeletonNode:removeAllChildren();
        skeletonNode:removeFromParent();
	end)));
end

--尾焰效果
function Fish:showFishEffect()
	local contentSize = self:getContentSize()
	
    self.praticle = cc.ParticleSystemQuad:create("main/tuowei.plist");
    -- self.praticle:setScale(2.0)
    -- self.praticle:setPosition(cc.p(667,375))
    self.praticle:setRotation(-90)
    self.praticle:setPosition(cc.p(contentSize.width/2,contentSize.height/2))
    self.praticle:setAnchorPoint(0.5, 0.5);
    self.praticle:setPositionType(cc.POSITION_TYPE_FREE);
    -- self.praticle:setPositionType(cc.POSITION_TYPE_GROUPED);
    self:addChild(self.praticle,1);
end

return Fish