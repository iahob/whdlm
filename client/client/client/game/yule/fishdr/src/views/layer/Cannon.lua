--
-- Author: chen
--炮
local Cannon = class("Cannon", cc.Sprite)
local module_pre = "game.yule.fishdr.src"			
local ExternalFun = require(appdf.EXTERNAL_SRC.."ExternalFun")
-- local cmd = module_pre..".models.CMD_LKGame"
local cmd = ExternalFun.req_var(module_pre ..".models.CMD_LKGame")

local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local Bullet = require(module_pre..".views.layer.Bullet")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local g_var = ExternalFun.req_var
local scheduler = cc.Director:getInstance():getScheduler()
Cannon.Tag = 
{

	Tag_Award = 10,
	Tag_Light = 20,
	Tag_Type  = 30,
	Tag_lock  = 3000

}

local TagEnum = Cannon.Tag

function Cannon:ctor(viewParent)
	
	self.m_pos = 0    --炮台位置
	self.m_fort = nil
	self.m_nickName = nil
	self.m_score = nil
	self.m_multiple = nil
	self.m_isShoot = false
	self.m_canShoot = true
	self.m_autoShoot = false
	self.m_typeTime = 0
	self.orignalAngle = 0
	self.m_fishIndex = cmd.INT_MAX
	self.m_index  = 0 --子弹索引
	self.m_ChairID  = yl.INVALID_CHAIR
	self.m_autoShootSchedule = nil
	self.m_otherShootSchedule = nil
	self.m_typeSchedule = nil

	self.m_targetPoint = cc.p(0,0)
	self.m_cannonPoint = cc.p(0,0)
	self.m_firelist = {}

	self.m_nCurrentBulletScore = 1
	self.m_nMutipleIndex = 0

	self.m_Type = cmd.CannonType.Normal_Cannon
	self.parent = viewParent

	self._dataModel = self.parent._dataModel
	self.frameEngine = self.parent._gameFrame 

	self.m_laserPos = cc.p(0,0)

	self.m_laserConvertPos = cc.p(0,0)

	self.m_laserBeginConvertPos = cc.p(0,0)

--获取自己信息
	self.m_pUserItem = self.frameEngine:GetMeUserItem()
  	self.m_nTableID  = self.m_pUserItem.wTableID
  	self.m_nChairID  = self.m_pUserItem.wChairID	

--其他玩家信息
  	self.m_pOtherUserItem = nil

  	-- 炮台旋转角度
  	self.connonRotation = 0;
  	-- 当前炮台
  	self.curConnonIndex = 1;
 
 	self.m_goldList = {} -- 金币动画
 	self.m_goldIndex = 1 -- 金币动画
 	--金币横幅红绿切换
 	self.m_nBannerColor = 0

  	--注册事件
    ExternalFun.registerTouchEvent(self,false)
end


function Cannon:onExit( )

	self:unAutoSchedule()
	self:unTypeSchedule()
	self:unOtherSchedule()
end


function Cannon:setCannonMuitle(multiple)
	
	self.m_nMutipleIndex = multiple
end

function Cannon:initWithUser(userItem)
	
	self.m_ChairID = userItem.wChairID
	if self.m_ChairID ~= self.m_nChairID then
		self.m_pOtherUserItem = userItem
	end

	self:setContentSize(100,100)
	self:removeChildByTag(1000)
	-- self.m_fort = cc.Sprite:createWithSpriteFrameName("gun_1_1.png")
	-- self.m_fort:setTag(1000)
	-- self.m_fort:setPosition(50,50)
	-- self:addChild(self.m_fort,1)

	--炮台动画
	self:cannonSpinChange(1);

	self.m_pos = userItem.wChairID
	if self._dataModel.m_reversal then 
		self.m_pos = cmd.cannonMax - self.m_pos
	end

	if self.m_pos < cmd.connonLimit then
		self.m_fort:setRotation(180)
		self.connonRotation = 180;
	end
end

function Cannon:setFishIndex(index)
	self.m_fishIndex = index
end

function Cannon:setMultiple(multiple)

	self.m_nMutipleIndex = multiple
	local nMultipleValue = self._dataModel.m_secene.nMultipleValue[1][multiple+1]
	self.m_nCurrentBulletScore = nMultipleValue

	local nNum = 1
	if self.m_Type == cmd.CannonType.Special_Cannon then
		nNum = 2
	end

	-- local nBulletNum = math.floor(self.m_nMutipleIndex/2) + 1
	local nBulletNum = self.m_nMutipleIndex+1;

	if nBulletNum > 7  then
		nBulletNum = 7
	end

	-- local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(string.format("gun_%d_%d.png", nBulletNum,nNum))
	-- self.m_fort:setSpriteFrame(frame)

	--炮台动画
	if self.curConnonIndex ~= nBulletNum then
		self:cannonSpinChange(nBulletNum);
	end

	self.parent:updateMultiple(nMultipleValue,self.m_pos+1)

end

--自动射击
function Cannon:setAutoShoot( b )

	self.m_autoShoot = b

	if self.m_cannonPoint.x == 0 and self.m_cannonPoint.y == 0 then 
		self.m_cannonPoint = self:convertToWorldSpace(cc.p(self.m_fort:getPositionX(),self.m_fort:getPositionY()))
	end

	if self.m_Type >= cmd.CannonType.Laser_Cannon then
		return
	end

	if self.m_autoShoot then

		local time =  self._dataModel.m_secene.nBulletCoolingTime/1000
		if self.m_Type == cmd.CannonType.Special_Cannon then
			time = time / 2
		end

		self:autoSchedule(time)

	else
		self:unAutoSchedule()	
	end
end

function Cannon:typeTimeUpdate( dt )

	self.m_typeTime = self.m_typeTime - dt

	local tmp = self:getChildByTag(TagEnum.Tag_Type)
	if nil ~= tmp then
		local timeshow = tmp:getChildByTag(1)
		timeshow:setString(string.format("%d",self.m_typeTime))
	end
	--print("------------typeTimeUpdate  m_typeTime--------",self.m_typeTime)
	if self.m_typeTime <= 0 then
		self:removeTypeTag()
		self:unTypeSchedule()
		if	self.m_Type == cmd.CannonType.Laser_Cannon then
			--自己才会自动发
			if self.m_ChairID == self.m_nChairID then
				self:shootLaser()
			end
		else
			if	self.m_Type ~= cmd.CannonType.Laser_Cannon then
				self:setCannonType(cmd.CannonType.Normal_Cannon, 0)
			end
		end
		
	end
end


--自己开火
function Cannon:shoot( vec , isbegin )

	if not self.m_canShoot then
		self.m_isShoot = isbegin
		return
	end

	if self.m_cannonPoint.x == 0 and self.m_cannonPoint.y == 0 then
		self.m_cannonPoint = self:convertToWorldSpace(cc.p(self.m_fort:getPositionX(),self.m_fort:getPositionY()))
	end

	self.m_laserPos.x = vec.x
	self.m_laserPos.y = vec.y
	
	local angle = self._dataModel:getAngleByTwoPoint(vec, self.m_cannonPoint)

	self.m_targetPoint = vec

	if angle < 90  then 

		if not self._dataModel.m_autolock then
			self.m_fort:setRotation(angle)
			self.connonRotation = angle;
		end
	end

	if self.m_Type == cmd.CannonType.Laser_Shooting then
		return
	end

	if self.m_Type == cmd.CannonType.Laser_Cannon  then
		self:shootLaser()
		return
	end

	if self.m_autoShoot or self._dataModel.m_autolock then
		-- print("-------------------m_autoShoot return------------------")
		return
	end

	if not self.m_isShoot and isbegin then
		self.m_isShoot = true
		local time = self._dataModel.m_secene.nBulletCoolingTime/1000
		if self.m_Type == cmd.CannonType.Special_Canonn then
			time = time / 2 
		end

		self:autoUpdate(0)
		self:autoSchedule(time)
	end

	if not isbegin then
		self.m_isShoot = false
		self:unAutoSchedule()
	end

end

--其他玩家开火
function Cannon:othershoot( firedata )
	
	table.insert(self.m_firelist,firedata)
	self:setMultiple(self.m_nMutipleIndex)
	self.m_nCurrentBulletScore = firedata.nBulletScore
	local time = self._dataModel.m_secene.nBulletCoolingTime/1000
	if self.m_Type == cmd.CannonType.Special_Cannon then
		time = time/2
	end

	self:otherSchedule(time)
end

--发射激光
function Cannon:shootLaser()
	self._dataModel:playEffect(cmd.Prop_armour_piercing)

	self.m_Type = cmd.CannonType.Laser_Shooting
	--print("---------------------shootLaser self.m_laserConvertPos------------",self.m_laserConvertPos.x,self.m_laserConvertPos.y)
	--print("---------------------shootLaser self.m_laserBeginConvertPos self.m_laserConvertPos------------",self.m_laserBeginConvertPos.x,self.m_laserBeginConvertPos.y)
	if self.m_laserBeginConvertPos.x ==0 and self.m_laserBeginConvertPos.y == 0 then
		self.m_laserBeginConvertPos.x = self.m_cannonPoint.x
		self.m_laserBeginConvertPos.y = self.m_cannonPoint.y
	end
	
	local angle = self._dataModel:getAngleByTwoPoint(self.m_laserConvertPos, self.m_laserBeginConvertPos)
	if  self.m_ChairID == self.m_nChairID then
		angle = self._dataModel:getAngleByTwoPoint(self.m_laserPos, self.m_cannonPoint)
	end
	
	--print("angle is ",angle)
	if self.m_pos < cmd.connonLimit then
		self.m_fort:setRotation(angle)
		self.connonRotation = angle;
	end

	--print("angle is ",angle)
	self.m_fort:setRotation(angle)
	self.connonRotation = angle;
	-- local anim = cc.Animate:create(cc.AnimationCache:getInstance():getAnimation("FortLightAnim"))
	-- self.m_fort:runAction(anim)
	self:removeChildByTag(TagEnum.Tag_Light)

	local node = cc.Node:create()
	node:setAnchorPoint(0.5,0.5)
	node:setContentSize(cc.size(10, 10))
	--local angle = self.m_fort:getRotation()
	local moveDir = cc.pForAngle(math.rad(90-angle))
	cc.pMul(moveDir,50)
	node:setPosition(cc.pAdd(self.m_cannonPoint,moveDir))
	self.parent:addChild(node)

	local light = cc.Sprite:createWithSpriteFrameName("light.png")
	light:setPosition(node:getContentSize().width/2,0)
	light:setScale(0.5,1.0)
	node:addChild(light, 10)

	local callFunc = cc.CallFunc:create(function ()
		light:removeFromParent()
	end)

	local action = cc.Sequence:create(cc.ScaleTo:create(1,1,1),cc.ScaleTo:create(1,0.5,1),callFunc)
	light:runAction(action)


	for i=1,4 do
		local fortLight = cc.Sprite:createWithSpriteFrameName(string.format("fortlight_%d.png", i-1))
		fortLight:setPosition(node:getContentSize().width/2,fortLight:getContentSize().height*0.6+(i-1)*fortLight:getContentSize().height*1.2-5*(i-1))
		fortLight:setScale(0.1,1.2)
		fortLight:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5,1.0,1.2),cc.ScaleTo:create(2,0,1.2)))
		node:addChild(fortLight)

	end
	node:setRotation(self.m_fort:getRotation())

	callFunc = cc.CallFunc:create(function()

		if nil ~= self.setCannonType then
			node:removeFromParent()

			self:setCannonType(cmd.CannonType.Normal_Cannon,0)
			self.m_Type = cmd.CannonType.Normal_Cannon

			if self.m_autoShoot --[[or self.m_isShoot]] then
				local time = self._dataModel.m_secene.nBulletCoolingTime/1000
				if self.m_Type == cmd.CannonType.Special_Cannon then
					time = time/2
				end

				self:autoUpdate(0)
				self:autoSchedule(time)

				elseif 0 ~= #self.m_firelist then
					local time = self._dataModel.m_secene.nBulletCoolingTime/1200
					if self.m_Type == cmd.CannonType.Special_Cannon then
						time = time/2
					end
					self:otherSchedule(time)
					
				end
		end
		
	end)

	node:runAction(cc.Sequence:create(cc.DelayTime:create(2.3),callFunc))

	if  self.m_ChairID == self.m_nChairID then 
		local tmp =  cc.p(self.m_cannonPoint.x,self.m_cannonPoint.y)
		
		local  beginPos = self._dataModel:convertCoordinateSystem(tmp, 0, self._dataModel.m_reversal)
		self.m_laserConvertPos	= self._dataModel:convertCoordinateSystem(cc.p(self.m_laserPos.x,self.m_laserPos.y), 0, self._dataModel.m_reversal)
		local unLossTime = currentTime() - self._dataModel.m_enterTime
		local cmddata = CCmd_Data:create(12)
		cmddata:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_LASER)
		cmddata:pushshort(beginPos.x)
		cmddata:pushshort(beginPos.y)
		cmddata:pushshort(self.m_laserConvertPos.x)
		cmddata:pushshort(self.m_laserConvertPos.y)
		cmddata:pushint(unLossTime)

		if not self.frameEngine:sendSocketData(cmddata) then
			self.frameEngine._callBack(-1,"发送激光消息失败")
		end
	end
end

--制造子弹
function Cannon:productBullet( isSelf,fishIndex, netColor)

	-- 切换动作方法
	if self.m_fort then
		self.m_fort:setAnimation(0, "Attack", false);

		self.m_fort:stopAllActions();
	    self.m_fort:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function()
			self.m_fort:setAnimation(0, "Standby", true);
		end)))

		-- if self._dataModel.m_autoshoot or self._dataModel.m_autolock then
		-- 	self.m_fort:stopAllActions();
		--     self.m_fort:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),cc.CallFunc:create(function(	)
		-- 		self.m_fort:setAnimation(0, "Standby", true);  
		-- 	end)))
		-- else
		-- 	if self._dataModel.isExchageConnonState then
		-- 		self:runAction(cc.Sequence:create(cc.DelayTime:create(3.0),cc.CallFunc:create(function(	)
		-- 			self.m_fort:setAnimation(0, "Standby", true);   
		-- 		end)))
		-- 	end
		-- end
	end

	self.m_index = self.m_index + 1
	local angle = self.m_fort:getRotation()
	self:setFishIndex(self._dataModel.m_fishIndex)
	
	local bullet0 = Bullet:create(angle,self.m_ChairID,self.m_nCurrentBulletScore,self.m_nMutipleIndex,self.m_Type,self)

	-- 子弹增加
	if isSelf then
		self._dataModel.curBulletNum = self._dataModel.curBulletNum + 1;
		if self._dataModel.curBulletNum >= cmd.BULLET_LIMIT and self._dataModel._exchangeSceneing then
			-- showToast(self.parent.parent._gameView, "发射的子弹已达上限，上限数量" .. cmd.BULLET_LIMIT .. "发", 1)
			showToast(cc.Director:getInstance():getRunningScene(), "发射的子弹已达上限，上限数量" .. cmd.BULLET_LIMIT .. "发", 2)
		end
	end

	angle = math.rad(90-angle)
	local movedir = cc.pForAngle(angle)
	movedir = cc.p(movedir.x * 25 , movedir.y * 25)
	local offset = cc.p(25 * math.sin(angle),5 * math.cos(angle))
	-- local moveBy = cc.MoveBy:create(0.065,cc.p(-movedir.x*0.65,-movedir.y * 0.65))
	-- self.m_fort:runAction(cc.Sequence:create(moveBy,moveBy:reverse()))

	bullet0:setType(self.m_Type)
	bullet0:setIndex(self.m_index)
	bullet0:setIsSelf(isSelf)
	bullet0:setFishIndex(fishIndex)
	bullet0:initPhysicsBody()
	bullet0:setTag(cmd.Tag_Bullet)
	local pos = cc.p(self.m_cannonPoint.x+movedir.x,self.m_cannonPoint.y+movedir.y)
	pos = cc.p(pos.x , pos.y - offset.y/2)
	bullet0:setPosition(pos)
	self.parent.parent._gameView:addChild(bullet0,5) --self.parent.parent为GameLayer

	if isSelf then

		self.parent.parent:setSecondCount(60)
		local cmddata = CCmd_Data:create(16)

   		cmddata:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_FIRE)
    	cmddata:pushint(self._dataModel.m_secene.nMultipleIndex[1][self.m_ChairID+1])
  		cmddata:pushint(bullet0.m_fishIndex)
  		cmddata:pushint(self.m_index)

  		local pos = cc.p(movedir.x * 25 , movedir.y * 25)
  		pos = cc.p(self.m_cannonPoint.x + pos.x , self.m_cannonPoint.y + pos.y)
  		pos = self._dataModel:convertCoordinateSystem(pos, 0, self._dataModel.m_reversal)

  		self._dataModel:playEffect(cmd.Shell_8)

  		cmddata:pushshort(pos.x)
  		cmddata:pushshort(pos.y)

   		  --发送失败
   		if nil == self.frameEngine then
   			return
   		end
		if not self.frameEngine:sendSocketData(cmddata) then
			if nil ~= self.frameEngine then
   				self.frameEngine._callBack(-1,"发送开火息失败")
   			end	
		end
	end
end

function Cannon:fastDeal(  )
	self.m_canShoot = false
	self:runAction(cc.Sequence:create(cc.DelayTime:create(5.0),cc.CallFunc:create(function(	)
		self.m_canShoot = true
	end)))
end

function Cannon:setCannonType( cannontype,times)	

	if self.m_Type == cmd.CannonType.Special_Cannon and cannontype ~= cmd.CannonType.Special_Cannon then 
		if self.m_autoShoot or self.m_isShoot then
			self:unAutoSchedule()
			local time = self._dataModel.m_secene.nBulletCoolingTime/1000
			self:autoSchedule(time)
		end

		if 0 ~= #self.m_firelist then
			self:unOtherSchedule()
			local time = self._dataModel.m_secene.nBulletCoolingTime/1200
			self:otherSchedule(time)
		end
	end

	if cannontype == cmd.CannonType.Special_Cannon then

		-- local nBulletNum = math.floor(self.m_nMutipleIndex/2) + 1
		local nBulletNum = self.m_nMutipleIndex + 1;

		if nBulletNum > 7  then
			nBulletNum = 7;
		end

		-- local str = string.format("gun_%d_2.png", nBulletNum)
		-- self.m_fort:setSpriteFrame(cc.SpriteFrameCache:getInstance():getSpriteFrame(str))
		--炮台动画
		if self.curConnonIndex ~= nBulletNum then
			self:cannonSpinChange(nBulletNum);
		end

		self.m_Type = cmd.CannonType.Special_Cannon

		if self.m_autoShoot or self.m_isShoot then
			self:unAutoSchedule()
			local  time = self._dataModel.m_secene.nBulletCoolingTime/2000
			self:autoSchedule(time)
		end


		if #self.m_firelist > 0 then 
			self:unOtherSchedule()
			local  time = self._dataModel.m_secene.nBulletCoolingTime/2400
			self:otherSchedule(time)
		end

		local Type = cc.Sprite:create("game_res/im_icon_0.png")
		Type:setTag(TagEnum.Tag_Type)
		Type:setPosition(-16,-40)
		self:removeTypeTag()
		self:addChild(Type)

		local pos = nil
		if self.m_pos < cmd.connonLimit then
			pos = cc.p(110,-45)
		else
			pos = cc.p(110,150)
		end

		Type:setPosition(pos.x,pos.y)

		self.m_typeTime = times
		self:typeTimeSchedule(1.0)


		local timeShow = cc.LabelAtlas:create(string.format("%d", times),"game_res/lockNum.png",16,22,string.byte("0"))
		timeShow:setAnchorPoint(0.5,0.5)
		timeShow:setPosition(Type:getContentSize().width/2, 27)
		timeShow:setTag(1)
		Type:addChild(timeShow)

		Type:runAction(cc.RepeatForever:create(CirCleBy:create(1.0,cc.p(pos.x,pos.y),10)))


	elseif cannontype == cmd.CannonType.Laser_Cannon then
		--print("------------cmd.CannonType.Laser_Cannon self.m_ChairID self.m_nChairID --------",self.m_ChairID,self.m_nChairID)
		if self.m_Type == cmd.CannonType.Laser_Cannon  then
				self:unAutoSchedule()
				-- self.m_fort:setSpriteFrame("fort_light_0.png")
				self.m_typeTime = times
				self:typeTimeSchedule(1.0)
			return
		end

		self._dataModel:playEffect(cmd.Small_Begin)

		self.m_Type = cmd.CannonType.Laser_Cannon

		self:unAutoSchedule()

		--有激光时不显示锁定鱼
		self:removeLockTag();

		-- self.m_fort:setSpriteFrame("fort_light_0.png")
		-- if self.m_ChairID == self.m_nChairID then
		-- 	self:unAutoSchedule()
		-- 	self.m_typeTime = times
		-- 	self:typeTimeSchedule(1.0)
		-- end
		--------------------------------------------------------------
		local Type = cc.Sprite:create("game_res/im_icon_2.png")
		Type:setTag(TagEnum.Tag_Type)
		Type:setPosition(-16,-40)
		self:removeTypeTag()
		self:addChild(Type)

		local pos = nil
		if self.m_pos < cmd.connonLimit then
			pos = cc.p(110,-45)
		else
			pos = cc.p(110,150)
		end

		Type:setPosition(pos.x,pos.y)

		self.m_typeTime = times
		self:typeTimeSchedule(1.0)


		local timeShow = cc.LabelAtlas:create(string.format("%d", times),"game_res/lockNum.png",16,22,string.byte("0"))
		timeShow:setAnchorPoint(0.5,0.5)
		timeShow:setPosition(Type:getContentSize().width/2, 27)
		timeShow:setTag(1)
		Type:addChild(timeShow)

		Type:runAction(cc.RepeatForever:create(CirCleBy:create(1.0,cc.p(pos.x,pos.y),10)))
		----------------------------------------------------------
		local light = cc.Sprite:createWithSpriteFrameName("light_0.png")
		light:setTag(TagEnum.Tag_Light)

		if self.m_cannonPoint.x == 0 and self.m_cannonPoint.y == 0 then 
			self.m_cannonPoint = self:convertToWorldSpace(cc.p(self.m_fort:getPositionX(),self.m_fort:getPositionY()))
		end
		-- light:setPosition(self.m_fort:getPositionX(),self:getPositionY())
		light:setPosition(self.m_fort:getPositionX(),self.m_fort:getPositionY())
		self:addChild(light)

		local animate = cc.Animate:create(cc.AnimationCache:getInstance():getAnimation("LightAnim"))
		local action = cc.RepeatForever:create(cc.Sequence:create(animate,animate:reverse()))
		light:runAction(action)
	
	elseif cannontype== cmd.CannonType.Bignet_Cannon then
		
		self.m_Type = cmd.CannonType.Bignet_Cannon
	
		self:removeTypeTag()

		local Type = cc.Sprite:create("game_res/im_icon_1.png")
		Type:setTag(TagEnum.Tag_Type)
		Type:setPosition(-16,40)
		self:addChild(Type)
		if self.m_pos < cmd.connonLimit then
			Type:setPositionY(self:getContentSize().height - 30)
		end
		self.m_typeTime = times
		self:typeTimeSchedule(1.0)

	elseif cannontype == cmd.CannonType.Normal_Cannon then
		
		self.m_Type = cmd.CannonType.Normal_Cannon
		-- local nBulletNum = math.floor(self.m_nMutipleIndex/2) + 1
		local nBulletNum = self.m_nMutipleIndex + 1;

		if nBulletNum > 7  then
			nBulletNum = 7
		end

		--炮台动画
		if self.curConnonIndex ~= nBulletNum then
			self:cannonSpinChange(nBulletNum);
		end

		-- local str = string.format("gun_%d_1.png", nBulletNum)
		-- self.m_fort:setSpriteFrame(cc.SpriteFrameCache:getInstance():getSpriteFrame(str))
		self:removeTypeTag()
	end
end

--补给
function Cannon:ShowSupply( data )
	
	local pos = {}
	local box = cc.Sprite:createWithSpriteFrameName("fishDead_025_1.png")
	if self.m_pos < cmd.connonLimit then
		pos = cc.p(-30,20)
		
	else
		pos = cc.p(-40,self:getPositionY() - 30)
	end

	box:setPosition(pos.x, pos.y)
	self:addChild(box,20)

	local nSupplyType = data.nSupplyType

	local action = cc.Animate:create(cc.AnimationCache:getInstance():getAnimation("animation_fish_dead25"))

	local call = cc.CallFunc:create(function ()
		if nSupplyType ~= cmd.SupplyType.EST_NULL then
			local gold = cc.Sprite:create("game_res/im_box_gold.png")
			gold:setPosition(box:getContentSize().width/2,box:getContentSize().height/2)
			box:addChild(gold)

			local typeStr = string.format("game_res/im_supply_%d.png", nSupplyType)
			local title = cc.Sprite:create(typeStr)
			if nil ~= title  then
				title:setPosition(box:getContentSize().width/2,100)
				box:addChild(title)
			end
		end
	end)

	box:runAction(cc.Sequence:create(action,call))


	call = cc.CallFunc:create(function()
		box:removeFromParent()
	end)

	local delay = cc.DelayTime:create(4)
	box:runAction(cc.Sequence:create(delay,call))

	if nSupplyType == cmd.SupplyType.EST_Laser then
		self:setCannonType(cmd.CannonType.Laser_Cannon, data.lSupplyCount)
		if self.m_ChairID == self.m_nChairID then

			
			local ptPos = cc.p(0,0)
			ptPos.x = math.random(200) + 200
			ptPos.y = math.random(200) + 200
			local cmddata = CCmd_Data:create(4)
			cmddata:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_BEGIN_LASER)
			cmddata:pushword(ptPos.x)
			cmddata:pushword(ptPos.y)
			if not self.frameEngine:sendSocketData(cmddata) then
				self.frameEngine._callBack(-1,"发送准备激光消息失败")
			end
		end

	elseif nSupplyType == cmd.SupplyType.EST_Speed then
		self:setCannonType(cmd.CannonType.Special_Cannon, data.lSupplyCount)

	end
end


--自己开火
function Cannon:autoUpdate(dt)

	if not self.m_canShoot or self.m_Type == cmd.CannonType.Laser_Cannon  then
		--print("-------------- self.m_canShoot m_Type  return----------",tostring(self.m_canShoot),tostring(self.m_Type))
		return
	end

	if self._dataModel._exchangeSceneing  then 	--切换场景中不能发炮
		self:removeLockTag();
		return 
	end

	if 0 == table.nums(self._dataModel.m_InViewTag) then 
		--print("autoUpdate the view is not fish")
		return
	end

	if self._dataModel.curBulletNum >= cmd.BULLET_LIMIT and self.m_ChairID == self.m_nChairID and self._dataModel._exchangeSceneing then
		return;
	end

	self:setFishIndex(self._dataModel.m_fishIndex)

	local mutiple = self._dataModel.m_secene.nMultipleIndex[1][self.m_nChairID+1]
	local score = self._dataModel.m_secene.nMultipleValue[1][mutiple+1]

	score = self._dataModel.m_secene.lPalyCurScore[1][self.m_nChairID+1] - score
	
	if score < 0 then
		self:unAutoSchedule()
		self.m_autoShoot = false

		if nil == self._queryDialog then
      		local this = self
	    	self._queryDialog = QueryDialog:create("游戏币不足,请充值", function(ok)
	        this._queryDialog = nil
	    	end):setCanTouchOutside(false)
	        :addTo(cc.Director:getInstance():getRunningScene()) 
    	end

    	if self._dataModel.m_autoshoot then
    		self._dataModel.m_autoshoot = false;

    		if self.parent.parent._gameView then
    			local auto = self.parent.parent._gameView.btn_auto;
            	self.parent.parent._gameView:setAutoShoot(self._dataModel.m_autoshoot,auto);
    		end
    	end	

    	if self._dataModel.m_autolock then
    		self._dataModel.m_autolock = false;

    		if self.parent.parent._gameView then
    			local lock = self.parent.parent._gameView.btn_lock;
    			self.parent.parent._gameView:setAutoLock(self._dataModel.m_autolock,lock);
    		end
    	end
    	return
	end

	if self.m_autoShoot  and  self._dataModel.m_autolock then
		
		if self.m_fishIndex == cmd.INT_MAX then
			print("-------------- self.m_fishIndex == cmd.INT_MAX  return----------")
			 self:removeLockTag()
			 return
		end

		local fish = self._dataModel.m_fishList[self.m_fishIndex]

		-----------------------------------------------------------------------------
		--是否手动选择锁定鱼
		local isAutoSelect = false;
		if self._dataModel.curSelectfishIndex ~= 2147483647 and self._dataModel.curSelectfishIndex == self.m_fishIndex then
			local selectFish = self._dataModel.m_fishList[self._dataModel.curSelectfishIndex];
			local rect = cc.rect(0,0,yl.WIDTH,yl.HEIGHT)
	        if selectFish and false == selectFish.m_bRemove then
	        	--手动选中鱼类型
	        	self._dataModel.curSelectFishType = selectFish.m_data.nFishType;

	          	local pos = cc.p(selectFish:getPositionX(),selectFish:getPositionY()) 
	            --不在屏幕中
	          	if  not cc.rectContainsPoint(rect,pos) then
	              	isAutoSelect = true;
	              	self._dataModel.curSelectfishIndex = 2147483647;
	          	end
	        end
	    else
	    	isAutoSelect = true;
	        self._dataModel.curSelectfishIndex = 2147483647;
		end
		
		--自动锁定时
		if self._dataModel.m_autolock and isAutoSelect then

			local unCreateTime = 0;
			local isSelectFish = false;
			for k,v in pairs(self._dataModel.m_fishList) do
				if v ~= nil and v.m_data ~= nil then
					if self._dataModel.curSelectFishType == v.m_data.nFishType and (unCreateTime ==0 or unCreateTime > v.m_data.unCreateTime) then   -- and isFirst
						self.m_fishIndex = v.m_data.nFishKey;
						self._dataModel.m_fishIndex = v.m_data.nFishKey;

						unCreateTime = v.m_data.unCreateTime;

						fish = v;

						isSelectFish = true;
					end
				end
			end

			-----------------------------------------------------------------------
			local rect = cc.rect(0,0,yl.WIDTH,yl.HEIGHT)
            if isSelectFish and fish then
              	local pos = cc.p(fish:getPositionX(),fish:getPositionY()) 
                --不在屏幕中
              	if  not cc.rectContainsPoint(rect,pos) then
                  	isSelectFish = false;
              	end
            end
            --------------------------------------------------------------

			if not isSelectFish and self._dataModel.curSelectFishType then
				local typeList = self._dataModel:curFishTypeList();
				local isF = true;
				for i = 1, #typeList do
					if typeList[i] < self._dataModel.curSelectFishType and isF then
						self._dataModel.curSelectFishType = typeList[i];
						isF = false;
					end
				end

				if isF then
					if #typeList > 0 then
						self._dataModel.curSelectFishType = typeList[1];
					end
				end

				return;
			end

		end
		------------------------------------------------------------------------------
		if fish == nil then
			self:removeLockTag()
			print("-------------- fish == nil  return----------")
		    return
		end

		local fishData = fish.m_data
		-- local frameName = string.format("%d_%d.png", fishData.nFishType+1,fishData.nFishState+1)
		local frameName = string.format("fish_type_%d_1.png", fishData.nFishType+1)
		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(frameName)

		if nil ~= frame then
			local sp = self:getChildByTag(TagEnum.Tag_lock)
			if nil == sp then
				local myNum = self.m_nChairID/3
				local playerNum = self.m_ChairID/3
				local pos = cc.p(-40,145)
				if myNum ~= playerNum then
					pos = cc.p(-30,-30)
				end
				sp = cc.Sprite:createWithSpriteFrame(frame)
				self:removeLockTag()
				sp:setTag(TagEnum.Tag_lock)
				sp:setPosition(pos.x,pos.y)
				self:addChild(sp)

				sp:runAction(cc.RepeatForever:create(CirCleBy:create(1.0,cc.p(pos.x,pos.y),10)))
			else
				sp:setSpriteFrame(frame)
			end
		else
			self:removeLockTag()					
		end
		-- if nil ~= frame then
		-- 	if fishData.nFishType > cmd.FishType.FishType_HeTun then
		-- 		local sp = self:getChildByTag(TagEnum.Tag_lock)
		-- 		if nil == sp then
		-- 			local myNum = self.m_nChairID/cmd.connonLimit
		-- 			local playerNum = self.m_ChairID/cmd.connonLimit
		-- 			local pos = cc.p(-40,145)
		-- 			if myNum ~= playerNum then
		-- 				pos = cc.p(-30,-30)
		-- 			end
		-- 			sp = cc.Sprite:createWithSpriteFrame(frame)
		-- 			self:removeLockTag()
		-- 			sp:setTag(TagEnum.Tag_lock)
		-- 			sp:setPosition(pos.x,pos.y)
		-- 			self:addChild(sp)

		-- 			sp:runAction(cc.RepeatForever:create(CirCleBy:create(1.0,cc.p(pos.x,pos.y),10)))
		-- 		else
		-- 			sp:setSpriteFrame(frame)
		-- 		end
		-- 	else
		-- 		self:removeLockTag();

		-- 		local typeList = self._dataModel:curFishTypeList();
		-- 		if #typeList > 0 then
		-- 			self._dataModel.curSelectFishType = typeList[1];
		-- 		end
				
		-- 		return;
		-- 	end

			
		-- else
		-- 	self:removeLockTag()					
		-- end

		local pos = cc.p(fish:getPositionX(),fish:getPositionY())
		if self._dataModel.m_reversal then
			pos = cc.p(yl.WIDTH-pos.x,yl.HEIGHT-pos.y)
		end

		local angle = self._dataModel:getAngleByTwoPoint(pos, self.m_cannonPoint)
		self.m_fort:setRotation(angle)
		self.connonRotation = angle;

		self:productBullet(true,self.m_fishIndex,self._dataModel:getNetColor(1))
	else
		self:productBullet(true,cmd.INT_MAX,self._dataModel:getNetColor(1))

	end

	self:updateScore(score)

	self._dataModel.m_secene.lPalyCurScore[1][self.m_nChairID+1] = score

	self._dataModel.m_secene.lBulletConsume[1][self.m_nChairID+1] = self._dataModel.m_secene.lBulletConsume[1][self.m_nChairID+1] + self._dataModel.m_secene.nMultipleValue[1][self.m_nMutipleIndex+1]

end


function Cannon:autoSchedule(dt)

	local function updateAuto(dt)
		self:autoUpdate(dt)
	end

	if nil == self.m_autoShootSchedule then
		self.m_autoShootSchedule = scheduler:scheduleScriptFunc(updateAuto, dt, false)
	end

end

function Cannon:unAutoSchedule()
	if nil ~= self.m_autoShootSchedule then
		scheduler:unscheduleScriptEntry(self.m_autoShootSchedule)
		self.m_autoShootSchedule = nil
	end
end

--其他玩家开火
function Cannon:otherUpdate(dt)
	
	if 0 == #self.m_firelist then
		self:unOtherSchedule()
		self.m_isShoot = false
		return
	end

	--[[if not self._dataModel._bFishInView then
		return
	end]]

	--print("the num is ............................."..table.nums(self._dataModel.m_InViewTag))
	if 0 == table.nums(self._dataModel.m_InViewTag) then 
		--print("the view is not fish")
		return
	end

	local fire = self.m_firelist[1]
	table.remove(self.m_firelist,1)

	local pos = cc.p(fire.ptPos.x,fire.ptPos.y)
	pos = self._dataModel:convertCoordinateSystem(pos, 1, self._dataModel.m_reversal)

	if self.m_cannonPoint.x == 0 and self.m_cannonPoint.y == 0 then 
		self.m_cannonPoint = self:convertToWorldSpace(cc.p(self.m_fort:getPositionX(),self.m_fort:getPositionY()))
	end

	local angle = self._dataModel:getAngleByTwoPoint(pos, self.m_cannonPoint)
	self.m_fort:setRotation(angle)
	self.connonRotation = angle;

	self:productBullet(false, fire.nTrackFishIndex, cc.WHITE)	
	
	--更新分数
	self._dataModel.m_secene.lPalyCurScore[1][self.m_pOtherUserItem.wChairID+1] = self._dataModel.m_secene.lPalyCurScore[1][self.m_pOtherUserItem.wChairID+1] - fire.nBulletScore
	--print("==============lPalyCurScore ===============",self.m_pOtherUserItem.wChairID,self._dataModel.m_secene.lPalyCurScore[1][self.m_pOtherUserItem.wChairID+1])
	self:updateScore(self._dataModel.m_secene.lPalyCurScore[1][self.m_pOtherUserItem.wChairID+1])
end

function Cannon:updateScore(score)
	self.parent:updateUserScore(score,self.m_pos+1)	
end


function Cannon:otherSchedule(dt)

	local function updateOther(dt)
		self:otherUpdate(dt)
	end

	if nil == self.m_otherShootSchedule then
		self.m_otherShootSchedule = scheduler:scheduleScriptFunc(updateOther, dt, false)
	end

end

function Cannon:unOtherSchedule()
	if nil ~= self.m_otherShootSchedule then
		scheduler:unscheduleScriptEntry(self.m_otherShootSchedule)
		self.m_otherShootSchedule = nil
	end
end

function Cannon:typeTimeSchedule( dt )

	local function  update( dt  )
		self:typeTimeUpdate(dt)
	end

	if nil == self.m_typeSchedule then
		self.m_typeSchedule = scheduler:scheduleScriptFunc(update, dt, false)
	end
	
end

function Cannon:unTypeSchedule()
	if nil ~= self.m_typeSchedule then

		self:removeChildByTag(TagEnum.Tag_Light)
		scheduler:unscheduleScriptEntry(self.m_typeSchedule)
		self.m_typeSchedule = nil
	end
end


function Cannon:removeLockTag()

	self:removeChildByTag(TagEnum.Tag_lock)
end


function Cannon:removeTypeTag()
	

	self:removeChildByTag(TagEnum.Tag_Type)

end

-- 炮台骨骼动画
function Cannon:cannonSpinChange(index)
	if self.m_fort then
		self.m_fort = nil;
		self:removeChildByTag(1000);
	end
	self.curConnonIndex = index;
	self.m_fort = sp.SkeletonAnimation:create("spine/" .. "0" .. index .. "/skeleton.json","spine/" .. "0" .. index .. "/skeleton.atlas",1);  -- 1是缩放比例
    self.m_fort:setPosition(cc.p(50, 50))
    self.m_fort:setAnchorPoint(cc.p(0.5,0.5));
    self.m_fort:setScale(0.45);
    self.m_fort:setLocalZOrder(1);
    self.m_fort:setAnimation(0, "Standby", true);    --true是指循环播放walk动作
    self.m_fort:setMix("Standby", "Attack", 0.5); 
    self.m_fort:setTag(1000);
    self:addChild(self.m_fort);

    self.m_fort:setRotation(self.connonRotation);

    -- 切换动作方法
    -- self.m_fort:setAnimation(0, "Attack", true);    --true是指循环播放walk动作
end

function Cannon:moveAllBannerAndGolds(pNode)
	local moveX = 70
	local moveY = 0
			
	local listCount = #self.m_goldList
	if listCount >=1 then
		local tNode = self.m_goldList[1]
		if tNode.index == pNode.index then
			table.remove(self.m_goldList,1)
			tNode:removeFromParent()
		end

		-- for i = 1,#self.m_goldList do
		-- 	local node = self.m_goldList[i]
		-- 	node:runAction(cc.Sequence:create(cc.MoveBy:create(0.35,cc.p(moveX,moveY)),cc.CallFunc:create(function()
		-- 		self:moveAllBannerAndGolds(node)
		-- 	end)))
		-- end
	end
end

return Cannon