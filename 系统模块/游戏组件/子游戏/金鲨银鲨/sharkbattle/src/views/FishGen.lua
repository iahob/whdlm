	local FishGen={}

	local Fish=class("Fish",function(texFile) return cc.Sprite:create(texFile) end)

	function Fish:ctor(texFile,w,h,picN,speed) --一幅图(非plist)包含多个小图，w,h为小图宽高
		self.w,self.h=w,h
		self.speed=speed or 25
		self.curPic=0
		self.picN=picN
		if startPos then
			self:setPosition(startPos.x,startPos.y)
		end
		self:setTextureRect(cc.rect(0,0,w,h))
		schedule(self,function() self:setTextureRect(cc.rect(self.curPic*w,0,w,h)) self.curPic=(self.curPic+1)%self.picN  end,0.1) 
	end

	local function startPosGen(o,r,rad) --圆心，半径，弧度
		rad=rad or  2*math.pi*math.random()
		return cc.p( o.x+r*math.cos(rad),o.y+r*math.sin(rad) )
	end

	local function disSquare(p1,p2)
		local dx=p1.x-p2.x
		local dy=p1.y-p2.y
		return dx*dx+dy*dy
	end

	local posLimit={l=140,r=1000,b=295,t=745} --left,right,bottom,top
	--相对于scene的，改为相对于fishpond

	local pondPos={l=264,r=897,b=396,t=663}
	local pondCenter=cc.p(579,523)

	--local pondSz={width=,height=}

	local function pathGen(pondO,pondR) 
		local startPos=startPosGen(pondO,pondR)
		if startPos.x<posLimit.l then
			startPos.x=posLimit.l
		elseif startPos.x>posLimit.r then
			startPos.x=posLimit.r
		end
		if startPos.y<posLimit.b then
			startPos.y=posLimit.b 
		elseif startPos.y>posLimit.t then
			startPos.y=posLimit.t
		end

		local targetPos=startPosGen(pondO,pondR)
		if startPos.x<=pondPos.l then
			targetPos.x=math.random(pondCenter.x,posLimit.r)
		elseif startPos.x>=pondPos.r then
			targetPos.x=math.random(posLimit.l,pondCenter.x)
		elseif startPos.y<=pondPos.b then
			targetPos.y=math.random(pondCenter.y,posLimit.t)
		elseif startPos.y>=pondPos.t then
			targetPos.y=math.random(posLimit.b,pondCenter.y)
		else 
			--assert(false)
			while true do
				targetPos=startPosGen(pondO,pondR)
				if disSquare(targetPos,startPos)>400 then --距离至少大于20
					break
				end
			end
		end

		--parent node is pond
		startPos.x= startPos.x-pondPos.l
		startPos.y= startPos.y-pondPos.b
		targetPos.x= targetPos.x-pondPos.l
		targetPos.y= targetPos.y-pondPos.b

		pointTable={}
		pointTable[1]=startPos
		pointTable[2]=targetPos
		--...
		pointTable[#pointTable+1]=pointTable[1]
		return pointTable
	end

	local function rot(p1,p2)
		assert(p1.x)
		assert(p2.x)
		local dx=p2.x-p1.x
		local dy=p2.y-p1.y
		local deg=math.deg( math.atan2(dy,dx) )
		return deg
	end

	function Fish:setRandomPath()
		self.path=pathGen(cc.p(580,530),610)
		self.startPos=self.path[1]
	end

	function Fish:setPath(path)
		self.path=path
		self.startPos=path[1]
	end

	function Fish:swim() --添加圆弧运动轨迹   贝塞尔
		local path=self.path
		self:setPosition(path[1])
		local rot1=90-rot(path[1],path[2])
		local rot2=90-rot(path[2],path[1])
		local t1=math.sqrt(disSquare(path[1],path[2]))/self.speed
		local setRotation1=cc.CallFunc:create(function() self:setRotation(rot1) end)
		local leaveAct=cc.MoveTo:create(t1,path[2])
		local setRotation2=cc.CallFunc:create(function() self:setRotation(rot2) end)
		local backAct=cc.MoveTo:create(t1,path[1])
		local seqAct=cc.Sequence:create(setRotation1,leaveAct,setRotation2,backAct)
		self:runAction(cc.RepeatForever:create(seqAct))
	end

	local fishTexs={ --小图宽高
	{71,71},
	{81,81},
	{79,79},
	{171,171},
	{219,219},
	{254,250}
	}

	local fishSpeed= --不同种类鱼的speed
	{60,80,70,50,50,50}


	function FishGen.createSingleFish(kind,par,speed)
		local fish=Fish:create("FISH_"..kind..".png",fishTexs[kind][1],fishTexs[kind][2],4,speed or fishSpeed[kind])
		fish:setRandomPath()
		par:addChild(fish)
		table.insert(par.m_tabFishes,fish)
		fish:swim()
	end

	function FishGen.createFishCrowd1(par)--5红
		local leader=Fish:create("FISH_1.png",fishTexs[1][1],fishTexs[1][2],4,20)
		par:addChild(leader)
		table.insert(par.m_tabFishes,leader)
		leader:setRandomPath()
		leader:swim()
		local offx,offy=20,30
		local followers={}
		for i=1,4 do
			followers[i]=Fish:create("FISH_1.png",fishTexs[1][1],fishTexs[1][2],4,leader.speed)
		end
		followers[1]:setPath( {cc.p(leader.path[1].x+offx,leader.path[1].y+offy), cc.p(leader.path[2].x+offx,leader.path[2].y+offy)} )
		followers[2]:setPath( {cc.p(leader.path[1].x+offx,leader.path[1].y-offy), cc.p(leader.path[2].x+offx,leader.path[2].y-offy)} )
		followers[3]:setPath( {cc.p(leader.path[1].x-offx,leader.path[1].y+offy), cc.p(leader.path[2].x-offx,leader.path[2].y+offy)} )
		followers[4]:setPath( {cc.p(leader.path[1].x-offx,leader.path[1].y-offy), cc.p(leader.path[2].x-offx,leader.path[2].y-offy)} )
		for i=1,4 do
			followers[i]:swim()
			par:addChild(followers[i])
			table.insert(par.m_tabFishes,followers[i])
		end
	end

	function FishGen.createFishCrowd2(par)--3黄
		local leader=Fish:create("FISH_2.png",fishTexs[2][1],fishTexs[2][2],4,30)
		par:addChild(leader)
		table.insert(par.m_tabFishes,leader)
		leader:setRandomPath()
		leader:swim()
		local offx=60
		local offy=0
		local followers={}
		for i=1,2 do
			followers[i]=Fish:create("FISH_2.png",fishTexs[2][1],fishTexs[2][2],4,leader.speed)
		end
		followers[1]:setPath( {cc.p(leader.path[1].x+offx,leader.path[1].y+offy), cc.p(leader.path[2].x+offx,leader.path[2].y+offy)} )
		followers[2]:setPath( {cc.p(leader.path[1].x-offx,leader.path[1].y-offy), cc.p(leader.path[2].x-offx,leader.path[2].y-offy)} )
		
		for i=1,2 do
			followers[i]:swim()
			par:addChild(followers[i])
			table.insert(par.m_tabFishes,followers[i])
		end
	end

return FishGen

