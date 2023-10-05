local CompareView  = class("CompareView",function(config)
		local compareView =  display.newLayer(cc.c4b(0, 0, 0, 125))
    return compareView
end)
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")

function CompareView:onExit()
	print("CompareView onExit")
	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()	-- body
    self:setTouchEnabled(false)
end

function CompareView:ctor(config)
    display.loadSpriteFrames(cmd.RES.."sangong_gamecompare.plist",cmd.RES.."sangong_gamecompare.png")

	local this = self
	self.m_config = string.getConfig("fonts/round_body.ttf" , 20)
--	local function onNodeEvent(event)  
--       if "exit" == event then  
--            this:onExit()  
--        end  
--    end  

--    self:registerScriptHandler(onNodeEvent)  

	self:setTouchEnabled(false)
	self:registerScriptTouchHandler(function(eventType, x, y)
	 	return this:onTouch(eventType, x, y)
	end)

	display.newSprite("#"..cmd.GAME.."game_vs_bg.png")
		:move(667,400)
		:addTo(self)

	display.newSprite("#"..cmd.GAME.."vs.png")
		:move(657,430)
		:addTo(self)

	display.newSprite("#"..cmd.GAME.."game_vs_user.png")
		:move(218,435)
		:addTo(self)

	display.newSprite("#"..cmd.GAME.."game_vs_user.png")
		:move(1110,410)
		:addTo(self)

	self.m_FirstCard = {}
	self.m_SecondCard = {}

	for i = 1, 3 do
		self.m_FirstCard[i] = display.newSprite("#"..cmd.GAME.."card_back.png")
				:setAnchorPoint(cc.p(0,0.5))
                :setScale(0.9)
				:move(330 + 40*(i - 1), 440)
				:addTo(self)
		self.m_SecondCard[i] = display.newSprite("#"..cmd.GAME.."card_back.png")
				:move(830 + 40*(i - 1), 410)
                :setScale(0.9)
				:setAnchorPoint(cc.p(0,0.5))
				:addTo(self)
	end
	self.m_bFirstWin = false

	self.m_flagBall = {}

	self.m_flagBall[1] = display.newSprite("#"..cmd.GAME.."game_vs_boll.png")
		:move(406,415)
		:setVisible(false)
		:addTo(self)
	self.m_flagBall[2] = display.newSprite("#"..cmd.GAME.."game_vs_boll.png")
		:move(925 ,405)
		:setVisible(false)
		:addTo(self)

	self.m_flushAni = display.newSprite("#"..cmd.GAME.."game_ani_vs1.png")
		:setVisible(false)
		:move(667,400)
		:addTo(self)

	self.m_LoseFlag = display.newSprite("#"..cmd.GAME.."game_vs_lose.png")
		:setVisible(false)
		:addTo(self)

	self.m_UserInfo = {}
	self.m_UserInfo[1] = {}
	self.m_UserInfo[1].head = HeadSprite:createNormal({}, 80)
		:move(218,450)
		:addTo(self)
	self.m_UserInfo[1].name = cc.Label:createWithTTF("游戏玩家", "fonts/round_body.ttf", 20)
		:move(218,450-58)
		:addTo(self)
	self.m_UserInfo[2] = {}
	self.m_UserInfo[2].head = HeadSprite:createNormal({}, 80)
		:move(1110,425)
		:addTo(self)
	self.m_UserInfo[2].name = cc.Label:createWithTTF("游戏玩家", "fonts/round_body.ttf", 20)
		:move(1110,425-58)
		:addTo(self)

	self.m_AniCallBack = nil
end

function CompareView:onTouch(eventType, x, y)
    if eventType == "began" then
--        self:ShowCompare()
    end

    return true
end

function CompareView:StopCompareCard()
	self.m_flushAni:stopAllActions()
	self.m_flagBall[1]:stopAllActions()
	self.m_flagBall[2]:stopAllActions()

	self.m_flushAni:setVisible(false)
	self.m_flagBall[1]:setVisible(false)
	self.m_flagBall[2]:setVisible(false)

	self.m_LoseFlag:setVisible(false)
	self.m_LoseFlag:stopAllActions()

end

function CompareView:CompareCard(userCard,firstuser,seconduser,firstcard,secondcard,bfirstwin,callback)
    self:setTouchEnabled(true)
	self.m_AniCallBack = callback
	for i = 1 , 3  do
        local card = 0
        local cardData = userCard[firstuser.wChairID+1]
        if cardData[i] <= 0x0D then
             card = cardData[i]
        elseif cardData[i] > 0x0D and cardData[i] <= 0x1D then
             card = cardData[i] - 3
        elseif cardData[i] > 0x1D and cardData[i] <= 0x2D then 
             card = cardData[i] - 6
        elseif cardData[i] > 0x2D and cardData[i] <= 0x3D then 
             card = cardData[i] - 9
        end
		local strCard = string.format(cmd.GAME.."card_player_%02d.png",card)
		self.m_FirstCard[i]:setSpriteFrame(strCard)
	end
    for i = 1 , 3  do
        local card = 0
        local cardData = userCard[seconduser.wChairID+1]
        if cardData[i] <= 0x0D then
             card = cardData[i]
        elseif cardData[i] > 0x0D and cardData[i] <= 0x1D then
             card = cardData[i] - 3
        elseif cardData[i] > 0x1D and cardData[i] <= 0x2D then 
             card = cardData[i] - 6
        elseif cardData[i] > 0x2D and cardData[i] <= 0x3D then 
             card = cardData[i] - 9
        end
		local strCard = string.format(cmd.GAME.."card_player_%02d.png",card)
		self.m_SecondCard[i]:setSpriteFrame(strCard)
	end
	self.m_bFirstWin = bfirstwin

	self.m_UserInfo[1].head:updateHead(firstuser)
	self.m_UserInfo[2].head:updateHead(seconduser)

	local nickname 
	if firstuser and firstuser.szNickName then
		nickname = firstuser.szNickName
	else
		nickname = "游戏玩家"
	end

	self.m_UserInfo[1].name:setString(string.EllipsisByConfig(nickname,70, self.m_config))

	if seconduser and seconduser.szNickName then
		nickname = seconduser.szNickName
	else
		nickname = "游戏玩家"
	end

	self.m_UserInfo[2].name:setString(string.EllipsisByConfig(nickname,70, self.m_config))

	self:setVisible(true)

	local this = self
	self.m_flushAni:stopAllActions()
	self.m_flagBall[1]:stopAllActions()
	self.m_flagBall[2]:stopAllActions()

	self.m_flushAni:setVisible(true)
	self.m_flagBall[1]:setVisible(true)
	self.m_flagBall[2]:setVisible(true)

    --闪电动画
	local animation = cc.Animation:create()
	local i = 1
	while true do
		local strVS = string.format(cmd.GAME.."game_ani_vs%d.png",i)
		i = i + 1
		local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strVS)
		if spriteFrame then
			animation:addSpriteFrame(spriteFrame)
		else
			break
		end
	end

	animation:setLoops(2)
	animation:setDelayPerUnit(0.1)
	local animate = cc.Animate:create(animation)
	local animateVS = cc.Spawn:create(animate, cc.CallFunc:create(function()
		AudioEngine.playEffect(cmd.RES.."sound_res/COMPARE_ING.wav")
		end))

	self.m_flagBall[1]:runAction(cc.RepeatForever:create(cc.Sequence:create(
		cc.ScaleTo:create(0.3,0.7),
		cc.ScaleTo:create(0.3,1)
		)))
	self.m_flagBall[2]:runAction(cc.RepeatForever:create(cc.Sequence:create(
		cc.ScaleTo:create(0.3,0.7),
		cc.ScaleTo:create(0.3,1)
		)))
	self.m_flushAni:runAction(cc.Sequence:create(
		animateVS,
		cc.CallFunc:create(
			function()
				this:FlushEnd()
			end)
		))

end

function CompareView:FlushEnd()
	local this = self
	self.m_flushAni:stopAllActions()
	self.m_flagBall[1]:stopAllActions()
	self.m_flagBall[2]:stopAllActions()

	self.m_flushAni:setVisible(false)
	self.m_flagBall[1]:setVisible(false)
	self.m_flagBall[2]:setVisible(false)

	if self.m_bFirstWin then
		self.m_LoseFlag:move(925 ,395)
	else
		self.m_LoseFlag:move(406,420)
	end
	self.m_LoseFlag:setVisible(true)
	self.m_LoseFlag:setScale(3.0)
	self.m_LoseFlag:setOpacity(0)

	self.m_LoseFlag:runAction(cc.Sequence:create(
		cc.Spawn:create(
			cc.ScaleTo:create(0.3,1.0),
			cc.FadeTo:create(0.3,255)),
		cc.CallFunc:create(
			function()
				for i = 1 , 3  do
					if not this.m_bFirstWin then 
--						this.m_FirstCard[i]:setSpriteFrame("card_break.png")
					else
--						this.m_SecondCard[i]:setSpriteFrame("card_break.png")
					end
				end
			end),
		cc.DelayTime:create(0.5),
		cc.CallFunc:create(
			function()
				this.m_AniCallBack()
                self:setTouchEnabled(false)
			end
			)
	))
end


return CompareView