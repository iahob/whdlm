	local module_pre = "game.yule.sharkbattle.src"
	local cmd = appdf.req(module_pre .. ".models.CMD_Game")

	local ZhuanPanAni=class("ZhuanPanAni",cc.Layer)

	local aw,ah=90.2,52.6
	function ZhuanPanAni.rowcolToPos(row,col)
		local offx,offy=176,345
		local x=offx+aw*(col-1)+aw/2
		local y=offy+ah*(row-1)+ah/2
		return cc.p(x,y)
	end

	function ZhuanPanAni.rowcolToIndex(row,col) --设左上角为1，设任意一个为1都可以
		if row==7 then 
			return col
		elseif col==9 then
			return 9+7-row
		elseif row==1 then
			return 15+9-col
		else
			return 22+row
		end
	end

	--服务端index从0开始，从转换
	function ZhuanPanAni.indexToRowCol(index) --设左上角为1，左上角为索引最小值，顺时针索引变大
		if index<=9 then
			return 7,index
		elseif index<=15 then
			return 16-index,9
		elseif index<=23 then
			return 1,24-index
		else
			return index-22,1
		end
	end


	if ZhuanPanAni.tabZhuanpanPos==nil then

		ZhuanPanAni.tabZhuanpanPos={}

		for i=1,28 do
			ZhuanPanAni.tabZhuanpanPos[i]=ZhuanPanAni.rowcolToPos( ZhuanPanAni.indexToRowCol(i) )
		end
	end


	function ZhuanPanAni:ctor(par,begIndex,endIndex,duration,totalSec)  
		par:addChild(self)                    --服务端发来的转盘停止位置索引[0,27]
		self.par,self.begIndex,self.endIndex=par,begIndex+1+3,endIndex+1
		self.duration=duration
		local deltaT=cc.Director:getInstance():getAnimationInterval()
		duration=duration or 10
		self.frames=math.floor(duration/deltaT) --持续帧数		
		self.totalSec=totalSec
		self.followRects={}
		for i=1,5 do
			local rectSp
			if i==1 or i==5 then
				rectSp=cc.Sprite:create("darkrect1.png")
			else
				rectSp=cc.Sprite:create("darkrect2.png")
			end
			rectSp.m_index=i
			rectSp:setPosition(self.tabZhuanpanPos[rectSp.m_index])
			rectSp:addTo(self)
			table.insert(self.followRects,rectSp)
		end
		self.bright=cc.Sprite:create("brightrect.png")
		self.bright.m_index=self.endIndex
		self.bright:setPosition(self.tabZhuanpanPos[self.endIndex])
			:addTo(self)
			:setVisible(false)
	end

	function ZhuanPanAni.zhuanpanPosToKind(index) 
		local tab={								
		cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_EAGLE,cmd.ANIMAL_EAGLE,cmd.ANIMAL_EAGLE,
		cmd.ANIMAL_GOLD_SHARK, cmd.ANIMAL_LION, cmd.ANIMAL_LION, cmd.ANIMAL_LION, 
		cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_PANDA,cmd.ANIMAL_PANDA,cmd.ANIMAL_GOLD_SHARK,
		cmd.ANIMAL_MONKEY,cmd.ANIMAL_MONKEY,cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_RABBIT,
		cmd.ANIMAL_RABBIT,cmd.ANIMAL_RABBIT,cmd.ANIMAL_GOLD_SHARK,cmd.ANIMAL_SWALLOW,
		cmd.ANIMAL_SWALLOW,cmd.ANIMAL_SWALLOW,cmd.ANIMAL_SLIVER_SHARK,cmd.ANIMAL_PIGEON,
		cmd.ANIMAL_PIGEON,cmd.ANIMAL_GOLD_SHARK,cmd.ANIMAL_PEACOCK,cmd.ANIMAL_PEACOCK
		}              
		assert(#tab==28)  
		assert(index>=1 and index<=28)            
		return tab[index]
	end

	function ZhuanPanAni:moveAStep(node)
		node.m_index=node.m_index+1
		if node.m_index>28 then
			node.m_index=1
		end
		--print(node.m_index)
		node:setPosition(self.tabZhuanpanPos[node.m_index])
	end

	function ZhuanPanAni:animationForFirstOpening() --第一次进入为开奖状态，乱闪
		local bright=cc.Sprite:create("brightrect02.png")
		bright:addTo(self)
		bright:scheduleUpdate(function() bright:setPosition(self.tabZhuanpanPos[math.random(28)]) end)
	end

	function ZhuanPanAni:ZhuanPan(callback)
		self.callback=callback
		local begTime=os.time()
		self.startTime=begTime
		local endIndex=self.endIndex
		local begIndex=self.begIndex

		if self.frames==0 then
			return 
		end

		local par=self.par
		
		local perimeter=28 --总共28个格子
		 --表示每隔everyNFrame[speedKind]帧移动一次
		self.everyNFrame={9,8,7,6,5,4,3,2,1} --只需要设置这里以更改跑马灯动画
		
		--除最慢外，每种速度跑的steps数 varSpeedSteps=40/everyNFrame[speedKind]
		self.fastest=#self.everyNFrame --speedKind
		self.slowest=1			   --speedKind

		local t=self.frames/(2*#self.everyNFrame-1)
		self.varSpeedSteps={}
		for i=1,#self.everyNFrame-1 do
			self.varSpeedSteps[i]=math.max( 1, math.floor(t/self.everyNFrame[i]) )
		end

		--计算除最快速度外的总步数
		local steps=0
		for i=1,#self.everyNFrame-1 do
			steps=steps+2*(self.varSpeedSteps[i])
		end
		-- print("steps= ",steps)
		local fastestSteps=(perimeter+endIndex-(begIndex+steps)%perimeter)%perimeter
		if fastestSteps==0 then
			fastestSteps=perimeter
		end
		self.varSpeedSteps[#self.everyNFrame]=fastestSteps
		
		local function 	brightDelayRemoveSelf(t)
			print("t: ",t)
			self.bright:runAction(cc.Sequence:create(
			cc.Blink:create(2,2),
			cc.CallFunc:create(function()
				for i=1,#self.followRects do
					self.followRects[i]:setVisible(false)
				end
			end),
			cc.Blink:create(math.max(0,t-2),math.max(0,t-2)),
			cc.CallFunc:create(function() self.bright:removeSelf() end)
			))
		end
		self.brightDelayRemoveSelf=brightDelayRemoveSelf
		
		self.speedKind=self.slowest 
		self.frameN=0
		self.changeSpeed=1
		self.changeSpeedN=0 --每移动一次加1，当changeSpeedN>=varSpeedSteps[speedKind]，speedKind变化
		self.stepsRunned=0 -- for debug
		local function moveBrightRect(dt)

			self.frameN=self.frameN+1
			if self.frameN>=self.everyNFrame[self.speedKind] then
				self.frameN=0
				for k,circle in ipairs(self.followRects) do
					self:moveAStep(circle)
				end
				self.stepsRunned=self.stepsRunned+1
				self.changeSpeedN=self.changeSpeedN+1
				if self.changeSpeedN>=self.varSpeedSteps[self.speedKind] then
					self.changeSpeedN=0
					if self.speedKind==self.fastest then
						self.changeSpeed=-self.changeSpeed
					end
			
		
					self.speedKind=self.speedKind+self.changeSpeed
					if self.speedKind<=self.slowest-1 then

						self:unscheduleUpdate()
						self.bUnscheduleUpdate=true

						local usedSec=os.time()-begTime
						local restSec=math.max(0.1,self.totalSec-usedSec)

						print("restSec: ",restSec)
						print("usedSec",usedSec)
			
						self.bright:setVisible(true)
						self:runAction(
							cc.CallFunc:create(function() 
								brightDelayRemoveSelf(self.restSec)
								if self.callback then self.callback(math.max(0,self.restSec)) end
							end)
						)
					end
				end
			end
		end
		self.moveBrightRect=moveBrightRect

		self:scheduleUpdate(
			function()
				local t=os.time()
				if self.lastUpdateTime==nil then
					self.lastUpdateTime=t
					self.restSec=self.totalSec
					moveBrightRect()
					return
				end
				local elapsed=t-self.lastUpdateTime
				--print("elapsed: ",elapsed)
				if self.lastUpdateTime==nil or (t-self.startTime<self.duration and elapsed<=1) then 
					self.lastUpdateTime=t
					self.restSec=self.totalSec-(t-self.startTime)
					moveBrightRect()
					return
				end
				self.restSec=self.totalSec-(t-self.startTime)
				self.lastUpdateTime=t
				--print("elapsed: ",elapsed)
				--print("t-self.startTime: ",t-self.startTime)
				if t-self.startTime>=self.totalSec then --从后台返回来
					self:removeSelf()
				elseif t-self.startTime>=self.duration then
					self:unscheduleUpdate()
					self.bright:setVisible(true)
					self.bright:setPosition(self.tabZhuanpanPos[self.endIndex])
					
					self.restSec=self.totalSec-(t-self.startTime)

					self:runAction(
							cc.CallFunc:create(function() 
								self.brightDelayRemoveSelf(self.restSec)
								if self.callback then self.callback(self.restSec) end
							end))
				elseif t-self.startTime<self.duration then  
					self:resumeZhuanPan(elapsed)
				end
			end
			)
	end

	function ZhuanPanAni:resumeZhuanPan(timeElapsed) --timeElapsed表示在后台的时间
		self:stopAllActions()
		-- if self.bUnscheduleUpdate~=true then self.bright:stopAllActions() end
		local animationInterval=cc.Director:getInstance():getAnimationInterval()
		local passedFrames=timeElapsed/animationInterval
		for i=1,passedFrames do --在后台有passedFrames帧(=在后台的时间/animationInterval)没有调用moveBrightRect，这里一次性完成
			if self.bUnscheduleUpdate==true then
				return
			end
			self.moveBrightRect()
		end

		if self.bUnscheduleUpdate~=true then
			self:scheduleUpdate(self.moveBrightRect)
		end
	end

return ZhuanPanAni