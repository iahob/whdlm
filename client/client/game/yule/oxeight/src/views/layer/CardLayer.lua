local CardLayer = class("CardLayer", function(scene)
	local cardLayer = display.newLayer()
	return cardLayer
end)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC.."ExternalFun")
local cmd = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.models.CMD_Game")


function CardLayer:ctor(scene)
	print("CardLayer=====================================>")
	self._scene = scene
	local winSize = cc.Director:getInstance():getWinSize()
	local black = cc.c4b(0,0,0,0.5)
	local rect={cc.p(0,0),cc.p(winSize.width,0),cc.p(winSize.width,winSize.height),cc.p(0,winSize.height)}
	local greyBg=cc.DrawNode:create()
	greyBg:setAnchorPoint(cc.p(0,0))
	greyBg:setPosition(cc.p(0, 0))
	greyBg:drawPolygon(rect, 4, black, 0, black)
	self:addChild(greyBg)

    
	local index = self._scene._scene:GetMeChairID() +1

	self.myCardData = self._scene._scene.cbCardData[index]
	self.touchFlag ={false,false,false,false,false}


	self.selectImg = 0
	self.beginPoint =cc.p(0,0)
	self.isTouch = false
	self.imgPoint = cc.p(490,0)

	self:showCard()
	
	    local closeButton = ccui.Button:create(cmd.RES_PATH.."common/common_close.png",cmd.RES_PATH.."common/common_close.png")
    		 closeButton:setPosition(1260, 680)
    		 closeButton:addTo(self)
    		 closeButton:addTouchEventListener(function( sender , eventType )
		    if eventType == ccui.TouchEventType.ended then
		    	self._scene.CuoLayer = nil
		    	for i=1,5 do
				self.cardSprite[i]:stopAllActions()
			end
		    	self:stopAllActions()
		    	self:removeFromParent()
		        	
		    end
    		end)

	ExternalFun.registerTouchEvent(self, true)
	
end

function CardLayer:showCard()

	self.cardSprite = {}
	self.cardSize ={}
	for i=1,5 do
		local ardValue = yl.POKER_VALUE[self.myCardData[i]] 
    		local cardColor = yl.CARD_COLOR[self.myCardData[i]]  
    		local card = cc.Sprite:create(cmd.RES_PATH.."game/bigCard/card_"..cardColor.."_"..ardValue..".png")
    			:setAnchorPoint(cc.p(0,0))
    			:setPosition(490, 0)
    			:setRotation(1+2*(i-1))
    			:addTo(self)
    		cc.Sprite:create(cmd.RES_PATH.."game/bigCard/cardKuang.png"):move(0,0):setAnchorPoint(cc.p(0,0)):addTo(card)
    		self.cardSize = card:getContentSize()

    		table.insert(self.cardSprite,card)
	end
	dump(self.cardSize,"====================")
end




function CardLayer:isPointInImg(pos,index)
	local cardPoint = {}
	cardPoint.x,cardPoint.y = self.cardSprite[index]:getPosition()

	local x= pos.x - cardPoint.x--相对坐标
	local y = pos.y - cardPoint.y

	--dump(self.cardSize,"====================")

	--y = k*x +b
	local pai = 3.1415926/180
	local k1 = math.tan((90-1-(index-1)*2)*pai)
	local b1= -k1*(self.cardSize.width/math.cos((1+(index-1)*2)*pai))

	local k2 = -math.tan((1+(index-1)*2)*pai)

	local b2= -k2*(self.cardSize.height/math.cos((90 -1- (index-1)*2)*pai))

	print(k1..","..b1..","..k2..","..b2.."=============="..index..","..(math.tan(45*pai))..","..(math.cos(60*pai)))
	if x> y/k1 and x < (y- b1)/k1 and y> k2*x and y<k2*x + b2 then
		return true
	end

	return false
end

function CardLayer:isMoveEnd()
	local num =0
	for i=1,5 do
		if self.touchFlag[i] then
			num = num +1
		end
	end

	if num >= 4 then
		return true 
	else
		return false 
	end
end


--触摸事件处理
function CardLayer:onTouchBegan(touch, event)
	
	local pos = touch:getLocation()

	self.selectImg = 0
	self.beginPoint =cc.p(0,0)
	self.isTouch = false

	dump(pos,"pos=================")

	for i=5,1,-1 do
		if self:isPointInImg(pos,i) then
			self.selectImg = i
			self.beginPoint =pos
			self.isTouch = true
			self.imgPoint.x,self.imgPoint.y = self.cardSprite[i]:getPosition()

			print("======================hahah,"..i)
			break
		end
	end
	
	return true
end

function CardLayer:onTouchMoved(touch, event)
	if not self.isTouch then
		return 
	end

	local pos = touch:getLocation()
	
	if self.selectImg ~= 0 then
		local selectImg = self.cardSprite[self.selectImg]
		selectImg:setPosition(cc.p( self.imgPoint.x + pos.x -self.beginPoint.x ,self.imgPoint.y + pos.y -self.beginPoint.y))
	end

	return true
end

function CardLayer:onTouchEnded(touch, event)
	if not self.isTouch then
		return 
	end

	local pos = touch:getLocation()
	
	if self.selectImg ~= 0 then
		self.touchFlag[self.selectImg] = true


		if self:isMoveEnd() then

			for i=1,5 do
				self.cardSprite[i]:runAction(cc.Sequence:create(cc.MoveTo:create(0.35,cc.p(490,0)),cc.DelayTime:create(0.1),
					cc.MoveTo:create(0.35,cc.p(490,-600)),cc.CallFunc:create(function ()
						self._scene:cuoActionEnd(false)
					end)))			
			end
			
		end
	end
	
end


return CardLayer