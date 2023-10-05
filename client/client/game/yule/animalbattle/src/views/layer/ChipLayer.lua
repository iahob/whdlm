


local ChipLayer = class("ChipLayer",function ()
	-- body
	return cc.Layer:create()
end)

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local cmd = appdf.req("game.yule.animalbattle.src.models.CMD_Game")


local start = {
    cc.p(137, 164)
}

-- local endPos = {cc.p()}

local res = { "button/chip_1.png", "button/chip_2.png", "button/chip_3.png", "button/chip_4.png", "button/chip_6.png" }

local bankerChairId = 16

function ChipLayer:ctor(scene)
	-- body
	self._scene = scene
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

function ChipLayer:createChipTxt( num )
	-- body
	local text = cc.Label:createWithSystemFont(self:formatScoreText(tonumber(num)), "Arial", 10, cc.size(0,0))--cc.LabelAtlas:create(num,"num/min_chip_num.png",12,16,string.byte("0"))
	text.num = num
	text:setAnchorPoint(0.5,0.5)
	-- text:setPosition(16, 16)

	return text
end

function ChipLayer:removeAllChip()
	-- body
	self:removeAllChildren()
end

function ChipLayer:createChip(num)
	-- body

	local chipArray = self._scene:getChipArray()
	for i=1,#chipArray[1] do
		if chipArray[1][i] == num then
			local chip = cc.Sprite:create(res[i])
			local txt = self:createChipTxt(num)
			txt:setPosition(chip:getContentSize().width/2,chip:getContentSize().height/2)
			chip:addChild(txt)
			chip.num = txt.num
			return chip
		end
	end
	return nil
end

function ChipLayer:createAllChip(num,index)
	-- body
	num = num or 0
	local chipArray = self._scene:getChipArray()
	local count = #chipArray[1]
	while(count<=0)
	do
		if num >= chipArray[1][count] and num > 0 then
			num = num - chipArray[1][count]
			local chip = self:createChip(chipArray[1][count])
			chip:setScale(1.1)
			self:addChild(chip)
			chip:setPosition(endPos[index])
			chip:setTag(index)
		else
			count = count - 1
		end
	end
end


function ChipLayer:flyChipAnimation(kind,start_pos,end_pos,num,isMe)
	-- body
	ExternalFun.playSoundEffect("chip.mp3")

	local chipArray = self:getChildren()
	local index = 0
	local myIndex = 0
	local otherChip = nil
	local myChip = nil
	for k,v in pairs(chipArray) do
		if v and tolua.cast(v,"cc.Node") then
			if v.kind == kind and false == v.isMe then 
				index = index + 1
				if not otherChip then otherChip = v end
			elseif true == v.isMe and v.kind == kind then 
				if not myChip then myChip = v end
				myIndex = myIndex + 1
			end
		end
	end

	if index >= 20 and otherChip then 
		otherChip:removeFromParent()
	end

	if myIndex >= 20 and myChip then 
		myChip:removeFromParent()
	end

	local chip = self:createChip(num)
	if chip then
		self:addChild(chip)
		chip:setPosition(start_pos)
		-- local move = cc.MoveTo:create(0.3,end_pos)
		local move = self:getMoveAction(start_pos,end_pos,0,0)
		chip:runAction(move)
		chip.isMe = isMe
		chip.kind = kind
	end

	print("kind === ",kind)
	
end

function ChipLayer:formatScoreText(score)

	-- print("score == ",score)

	local scorestr = ExternalFun.formatScore(score)
	if score < 10000 then
		return scorestr
	end

	if score < 100000000 then
		scorestr = string.format("%d万", score / 10000)
		return scorestr
	end
	scorestr = string.format("%d亿", score / 100000000)
	return scorestr
end

function ChipLayer:flyBankerWinChip(index)
	-- body
	local children = self:getChildren()
	for k,v in pairs(children) do
		if v and tolua.cast(v,"cc.Node") then
			if v and v.kind == index then
				local move = cc.MoveTo:create(0.5,cc.p(645,462))
				local func = cc.CallFunc:create(function () v:removeFromParent() end)
				v:runAction(cc.Sequence:create({move,func}))
			end 
		end
	end
end

local tab = {
	cmd.ID_YAN_ZI,
	cmd.ID_GE_ZI,
	cmd.ID_KONG_QUE,
	cmd.ID_LAO_YING,
	cmd.ID_SHI_ZI,
	cmd.ID_XIONG_MAO,
	cmd.ID_HOU_ZI,
	cmd.ID_TU_ZI,
	cmd.ID_SHA_YU,
	cmd.ID_FEI_QIN,
	cmd.ID_ZOU_SHOU,
	cmd.ID_SHA_YU
}

function ChipLayer:flyGameEndGold(resultKind,winScore,lOtherUserScore)
	-- body
	-- print("tab[resultKind+1]  ========================================================================   ",type(tab[resultKind+1]))
	-- print("tab[resultKind+1]  ========================================================================   ",tab[resultKind+1])
	local flyTime = 0
	local children = self:getChildren()
	for k,v in pairs(children) do
		if v and tolua.cast(v,"cc.Node") then

			if resultKind==cmd.JS_TONG_SHA then 
				local move = cc.MoveTo:create(0.35,cc.p(645,462))
				local func = cc.CallFunc:create(function () v:removeFromParent() end)
				v:runAction(cc.Sequence:create({move,func}))
				flyTime = 0.35
				ExternalFun.playSoundEffect("goldjiesuan.mp3")
			elseif resultKind==cmd.JS_TONG_PEI then		
			-- 	local x,y = v:getPosition()
			-- 	self:flyChipAnimation(v.kind,cc.p(645,462),cc.p(x,y),v.num,v.isMe)
			-- 	flyTime = 0.35
			elseif resultKind==cmd.JS_JIN_SHA or resultKind==cmd.JS_YIN_SHA then
				if v.kind ~= cmd.ID_SHA_YU then 
					local move = cc.MoveTo:create(0.35,cc.p(645,462))
					local func = cc.CallFunc:create(function () v:removeFromParent() end)
					v:runAction(cc.Sequence:create({move,func}))
					flyTime = 0.35
				end
				ExternalFun.playSoundEffect("goldjiesuan.mp3")
			else

				local index = cmd.ID_ZOU_SHOU
				if resultKind<=3 then 
					index = cmd.ID_FEI_QIN -- 飞禽
				else
					index = cmd.ID_ZOU_SHOU -- 走兽
				end
				
				if v.kind ~= tab[resultKind+1] and v.kind ~= index then
					local move = cc.MoveTo:create(0.35,cc.p(645,462))
					local func = cc.CallFunc:create(function () v:removeFromParent() end)
					v:runAction(cc.Sequence:create({move,func}))
					flyTime = 0.35
				end
				-- ExternalFun.playSoundEffect("goldjiesuan.mp3")
			end
		end
	end

	self:runAction(cc.Sequence:create({cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
		-- body
		local children = self:getChildren()
		for k,v in pairs(children) do
			if v and tolua.cast(v,"cc.Node") then 
				local x,y = v:getPosition()
				self:flyChipAnimation(v.kind,cc.p(645,462),cc.p(x,y),v.num,v.isMe)
				flyTime = 0.35
			end
		end
		-- ExternalFun.playSoundEffect("goldjiesuan.mp3")
	end),cc.DelayTime:create(flyTime+0.5),cc.CallFunc:create(function ()
		-- body
		flyTime = 0
		local children = self:getChildren()
		for k,v in pairs(children) do
			if v and tolua.cast(v,"cc.Node") then
				local move = nil
				if v.isMe then 

					move = cc.MoveTo:create(0.35,cc.p(62,62))
				else

					move = cc.MoveTo:create(0.35,cc.p(34,439))
				end
				flyTime = 0.35
				local func = cc.CallFunc:create(function () v:removeFromParent() end)
				v:runAction(cc.Sequence:create({move,func}))
			end
		end
		-- ExternalFun.playSoundEffect("goldjiesuan.mp3")
	end),cc.DelayTime:create(flyTime+0.5),cc.CallFunc:create(function ()
		-- body

		if self._scene.m_myIsBet then 
			winScore = winScore or 0
			local lasStr = "win.png"
			if winScore > 0 then 
				lasStr = "win.png"
				winScore = "/"..winScore
				ExternalFun.playSoundEffect("goldwin.mp3")
			elseif winScore < 0 then
				lasStr = "lose.png"
				winScore = "/"..math.abs(winScore)
				ExternalFun.playSoundEffect("golslose.mp3")
			else

			end
			
			local las = cc.LabelAtlas:create(winScore,lasStr,40,58,string.byte("."))
			las:setPosition(143,122)
			las:setAnchorPoint(0.5,0.5)
			local move = cc.MoveTo:create(0.35,cc.p(143,182))
			local func = cc.CallFunc:create(function ()
				-- body
				las:removeFromParent()
			end)
			self._scene:addChild(las)
			las:runAction(cc.Sequence:create(move,cc.DelayTime:create(2),func))

		end

		if self._scene.m_otherIsBet then 
			lOtherUserScore = lOtherUserScore or 0
			local lasStr = "win.png"
			if lOtherUserScore > 0 then 
				lasStr = "win.png"
				lOtherUserScore = "/"..lOtherUserScore
			elseif lOtherUserScore < 0 then
				lasStr = "lose.png"
				lOtherUserScore = "/"..math.abs(lOtherUserScore)
			else
				-- return 
			end

			local las = cc.LabelAtlas:create(lOtherUserScore,lasStr,40,58,string.byte("."))
			las:setPosition(33,440)
			las:setAnchorPoint(0,0.5)
			local move = cc.MoveTo:create(0.35,cc.p(33,480))

			local func = cc.CallFunc:create(function ()
				-- body
				las:removeFromParent()
			end)
			self._scene:addChild(las)
			las:runAction(cc.Sequence:create(move,cc.DelayTime:create(2),func))
		end

	end)}))





end

--获取移动动画
--inorout,0表示加速飞出,1表示加速飞入
--isreverse,0表示不反转,1表示反转
function ChipLayer:getMoveAction(beginpos, endpos, inorout, isreverse)
    local offsety = (endpos.y - beginpos.y)*0.7
    local controlpos = cc.p(beginpos.x, beginpos.y+offsety)
    if isreverse == 1 then
        offsety = (beginpos.y - endpos.y)*0.7
        controlpos = cc.p(endpos.x, endpos.y+offsety)
    end

    local bezier = {
        controlpos,
        endpos,
        endpos
    }

    local beaction = cc.BezierTo:create(0.42, bezier)
    if inorout == 0 then
        return cc.EaseOut:create(beaction, 1)
    else
        return cc.EaseIn:create(beaction, 1)
    end
end

return ChipLayer

