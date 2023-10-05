


local ChipLayer = class("ChipLayer",function ()
	-- body
	return cc.Layer:create()
end)

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local Type = {
	-- chip 类型
	CHIP_TYPE_1 = 1,
	CHIP_TYPE_2 = 2,
	CHIP_TYPE_3 = 3,
	CHIP_TYPE_4 = 4,
	CHIP_TYPE_5 = 5,
}


local start = {cc.p(77,258),cc.p(77,390),cc.p(77,528),
			   cc.p(255,685),cc.p(366,685),cc.p(638,698),cc.p(947,685),cc.p(1070,685),
			   cc.p(1260,528),cc.p(1260,391),cc.p(1260,258),cc.p(43,43)}

local endPos = {{x=350,y=445},{x=682,y=445},{x=1012,y=445},{x=350,y=215},{x=682,y=215},{x=1012,y=215}}

local bankerChairId = 6
local fontName = 20

function ChipLayer:ctor(scene)
	-- body
	self._scene = scene
	self.config = {}
	self.nChipsNum = 0
	self.playerChip = {}

	for i=1,12 do
		local tab = {}
		for j=1,6 do
			tab[#tab+1] = 0
		end
		self.playerChip[#self.playerChip+1] = tab
	end
	-- self:setChipConfig()

	local function onNodeEvent(event)
        if event == "enter" then
            -- self:onEnter()
        elseif event == "exit" then
            self:onExit()
        end
    end
    self:registerScriptHandler(onNodeEvent)

end

function ChipLayer:onExit(  )
	-- body
	for k,v in pairs(self:getChildren()) do
		if v then 
			v:stopAllActions()
			v:removeFromParent()
		end
	end
	self:stopAllActions()
end

function ChipLayer:setChipConfig(config)
	-- body
	self.config = config
end


function ChipLayer:setDiceConfig(config)
	-- body
	self.diceConfig = config
end

function ChipLayer:getChipNumForType(num)
	-- body
	for k,v in pairs(self.config) do
		if v == num then
			return k 
		end
	end
	return nil
end

function ChipLayer:randomPos(aimsId)
	-- body
	aimsId = aimsId or 1
	local x = endPos[aimsId].x + math.random(-120, 120)
	local y = endPos[aimsId].y + math.random(-50, 90)
	return cc.p(x,y)
end


-- 最大公约数
function ChipLayer:GCD(a,b )
	-- body
	if a%b == 0 then 
		return b
	else
		return self:GCD(b,a%b)
	end
end

-- 最小公倍数
function ChipLayer:LCM(a,b)
	-- body
	return math.floor(a*b/self:GCD(a,b))
end

function ChipLayer:callbackWithArgs()
	local children = self:getChildren()
	for i=1,6 do
		local type1Num = 0
		local type2Num = 0
		local type3Num = 0
		local type4Num = 0
		local type5Num = 0
		for k, v in pairs(children) do
			-- print("v.isRun ",v.isRun)
			if v:getTag() == 1000 and v.aimsId == i and not v.isRun then
				type1Num = type1Num +1
			end
			if v:getTag() == 2000 and v.aimsId == i and not v.isRun then
				type2Num = type2Num +1
			end
			if v:getTag() == 3000 and v.aimsId == i and not v.isRun then
				type3Num = type3Num +1
			end
			if v:getTag() == 4000 and v.aimsId == i and not v.isRun then
				type4Num = type4Num +1
			end
			if v:getTag() == 5000 and v.aimsId == i and not v.isRun then
				type5Num = type5Num +1
			end
		end
		--不同档次到达指定额度，都会刷新(多加2个，防止操用户正在下注的筹码)
		local temp_lcm = self:LCM(self.config[1],self.config[2])
		local index = temp_lcm/self.config[1]
		local max_index = temp_lcm/self.config[2]
		-- print("刷新游戏币池 = "..max_index.."self.config[2]"..self.config[2])
		if type1Num >= index then
			self:changeAllJettons(1, index,i,max_index)
		end

		local temp_lcm = self:LCM(self.config[2],self.config[3])
		local index = temp_lcm/self.config[2]
		local max_index = temp_lcm/self.config[3]		
		if type2Num >= index  then
			self:changeAllJettons(2, index,i,max_index)
		end

		local temp_lcm = self:LCM(self.config[3],self.config[4])
		local index = temp_lcm/self.config[3]
		local max_index = temp_lcm/self.config[4]	
		if type3Num >= index then
			self:changeAllJettons(3, index,i,max_index)
		end
		
		local temp_lcm = self:LCM(self.config[4],self.config[5])
		local index = temp_lcm/self.config[4]
		local max_index = temp_lcm/self.config[5]
		if type4Num >= index then
			self:changeAllJettons(4, index,i,max_index)
		end
	end
end

function ChipLayer:playerJetton(wViewChairId,aimsId ,num, notani)
	if not num or num < 1 then
		return
	end
	notani = notani or false

	-- dump(self.playerChip)
	-- print("wViewChairId ",wViewChairId)
	-- print("aimsId       ",aimsId)
	if wViewChairId > 0 and wViewChairId <= 12 then
		self.playerChip[wViewChairId][aimsId] = 1
	end
	
	local strChip = ""
	local strScore = ""
	local ChipsType = 0

	-- dump(self.config,"playerJetton",12)
	-- print("num = ",num)
	for k,v in pairs(self.config) do
		if v == num then
			strChip = "chip/fly_chip_"..k..".png"
			chipscore = num
			strScore = num..""
			ChipsType = k
		end
	end

	local chip = cc.Sprite:create(strChip)
	self:addChild(chip)
	chip:setTag(1000 *ChipsType)
	chip.aimsId = aimsId
	
	local text =  ccui.Text:create()
	text:setFontName("fonts/round_body.ttf")
	text:setFontSize(fontName)
	text:setString(self._scene:formatScoreTextEx(num))
	 -- cc.LabelAtlas:create(self._scene:formatScoreTextEx(num),"chip/chip_num.png",16,21,string.byte("0"))
	chip:addChild(text)
	chip.text = num
	text:setName("text")
	-- text:setScale(0.6)
	text:setAnchorPoint(0.5,0.5)
	text:setPosition(29, 30)

	if notani == true then
		chip:setPosition(self:randomPos(aimsId))
		chip.isRun = false
		self:callbackWithArgs()
	else
		chip:setPosition(start[wViewChairId])

        local callFun = cc.CallFunc:create(function() 
        	chip.isRun = false
        	self:callbackWithArgs()
        end)
        chip.isRun = true
		chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.3, self:randomPos(aimsId)),callFun))
	end
end

--筹码转换
function ChipLayer:changeAllJettons(chipType, num,aimsId,max_index)
	-- body

	-- do return end
	local  pos = self:randomPos(aimsId)
	local numMove = 0
	local children = self:getChildren()

	for i=1,#children do
		local chip = children[i]
		if chip:getTag() == chipType * 1000 and chip.aimsId == aimsId then
			chip:setTag(0) -- 防止重复计算
			chip.isRun = true
			chip:runAction(cc.Sequence:create(cc.MoveTo:create(0.4, pos),
				cc.CallFunc:create(function()
					chip:removeFromParent()
			end)))
			numMove = numMove +1
			if numMove > num -1 then
				
				for j=1,max_index do

					local strChip = "chip/fly_chip_"..(chipType+1)..".png"
					local newChip = cc.Sprite:create(strChip)
					newChip:setTag(1000 *(chipType+1))
					local text = ccui.Text:create()
					text:setFontName("fonts/round_body.ttf")
					text:setFontSize(fontName)
					text:setString(self._scene:formatScoreTextEx(self.config[chipType+1]))
					-- cc.LabelAtlas:create(self._scene:formatScoreTextEx(self.config[chipType+1]),"chip/chip_num.png",16,21,string.byte("0"))
					newChip:addChild(text)
					newChip.text = self.config[chipType+1]
					text:setName("text")
					text:setAnchorPoint(0.5,0.5)
					text:setPosition(29, 30)
					-- text:setScale(0.6)
					newChip:setPosition( pos)
					if max_index >=2 and j ~= 1 then 
						newChip:setPosition( self:randomPos(aimsId))
					end
					newChip.aimsId = aimsId
					newChip.isRun = false
					self:addChild(newChip)

					if j == max_index then
						newChip:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function()
							self:callbackWithArgs()
						end)))
					end

				end

				-- newChip:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function()
				-- 	self:callbackWithArgs()
				-- end)))
				
				return
			end
		end
	end
end

-- 庄家赢
function ChipLayer:bankerWinFlyChip()
	-- body
	local time = 0
	local cbSice = clone(self._scene.gameEndData.cbSice[1])
	for i=1,#self.diceConfig do
		if cbSice[1] ~= self.diceConfig[i] and cbSice[2] ~= self.diceConfig[i] and cbSice[3] ~= self.diceConfig[i] then
			local children = self:getChildren()
			for j=1,#children do
				local chip = children[j]
				if chip and chip.aimsId == i then
					time = 0.3
					local move = cc.MoveTo:create(time,start[bankerChairId])
					local callFun = cc.CallFunc:create(function ()
						chip:removeFromParent()
					end)
					chip:runAction(cc.Sequence:create({move,callFun}))
				end
			end
		end
	end

	time = time + 0.3
	self:runAction(cc.Sequence:create({cc.DelayTime:create(time),cc.CallFunc:create(function()
		self:bankerLoseFlyChip()
	end)}))
end

-- 庄家输
function ChipLayer:bankerLoseFlyChip()
	dump( self._scene.gameEndData,"ChipLayer",12)
	if not self._scene.gameEndData.cbSice then 
		self._scene.gameView:showResultLayer()
		return 
	end

	local cbSice = clone(self._scene.gameEndData.cbSice[1])
	local time = 0
	for i=1,#self.diceConfig do
		if cbSice[1] == self.diceConfig[i] or cbSice[2] == self.diceConfig[i] or cbSice[3] == self.diceConfig[i] then
			-- 对应位置的筹码数量
			local allNum = 0
			for k,v in pairs(self:getChildren()) do
				if v.aimsId == i then
					allNum = allNum + v.text
					-- allNum = allNum + tonumber(v:getChildByName("text"):getString())
				end
			end

			if allNum >= 1000 then 
				allNum = 1000
			end

			--  已最小压注筹码创建
			local chipNum = allNum/self.config[1]
			for j=1,chipNum do
				local chip = cc.Sprite:create("chip/fly_chip_"..Type.CHIP_TYPE_1..".png")
				chip:setTag(1000 *Type.CHIP_TYPE_1)
				local text = ccui.Text:create()
				text:setFontName("fonts/round_body.ttf")
				text:setFontSize(fontName)
				text:setString(self._scene:formatScoreTextEx(self.config[1]))
				 -- cc.LabelAtlas:create(self._scene:formatScoreTextEx(self.config[1]),"chip/chip_num.png",16,21,string.byte("0"))
				chip:addChild(text)
				chip.text = self.config[1]
				text:setName("text")
				text:setAnchorPoint(0.5,0.5)
				text:setPosition(29, 30)
				-- text:setScale(0.6)
				chip:setPosition(start[bankerChairId]) -- 庄家位置
				chip.aimsId = i
				self:addChild(chip)
				time = 0.3
				local move = cc.MoveTo:create(time,self:randomPos(i))
				chip:runAction(move)
			end
		end
	end
	time = time + 0.3
	local flyTime = 0
	local callFun = cc.CallFunc:create(function ()
		-- 提取下注的玩家座位号
		local playerChair = {}
		for k,v in pairs(self.playerChip) do
			local index = 0
			for j,m in pairs(v) do
				if m > 0 then
					index = index + 1
				end
			end

			if index > 0 then
				playerChair[#playerChair+1] = k
			end
		end

		dump(playerChair,"playerChair",12)
		
		local children = self:getChildren()
		local num = (#children)/(#playerChair)
		-- print("#children = ",#children)
		-- print("#playerChair = ",#playerChair)
		-- print("cccccccccccccccccccccccc "..num)
		for k,v in pairs(playerChair) do
			for i=1,num do
				local chip = children[1]
				if chip then
					table.remove(children,1)
					flyTime = 0.3
					local move = cc.MoveTo:create(flyTime,start[v])
					local removeChip = cc.CallFunc:create(function ()
						chip:removeFromParent()
						chip = nil
					end)
					chip:runAction(cc.Sequence:create({move,removeChip}))
				end
			end
		end
		-- 保险起见清楚剩下的
		for k,v in pairs(children) do
			if v then v:removeFromParent() end
		end
	end)
	self:runAction(cc.Sequence:create({cc.DelayTime:create(time),callFun}))
	time = time + 0.3
	self:runAction(cc.Sequence:create({cc.DelayTime:create(time),cc.CallFunc:create(function ()
		-- body
		-- print("金币动画完成！！！！！！！！！！！！！！！！")
		self._scene.gameView:showResultLayer()
	end)}))
end

function ChipLayer:removeAllChip()
	-- body
	self:removeAllChildren()

	self.playerChip = {}
	for i=1,12 do
		local tab = {}
		for j=1,6 do
			tab[#tab+1] = 0
		end
		self.playerChip[#self.playerChip+1] = tab
	end
end

return ChipLayer

