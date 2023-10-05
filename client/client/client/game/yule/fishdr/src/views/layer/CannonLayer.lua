--
-- Author: chen
-- 
--炮台
local CannonLayer = class("CannonLayer", cc.Layer)

local module_pre = "game.yule.fishdr.src"			
local ExternalFun = require(appdf.EXTERNAL_SRC.."ExternalFun")
-- local cmd = module_pre..".models.CMD_LKGame"
local cmd = ExternalFun.req_var(module_pre ..".models.CMD_LKGame")
local Cannon = module_pre..".views.layer.Cannon"
local g_var = ExternalFun.req_var

local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

CannonLayer.enum = 
{

	Tag_userNick =1, 	

	Tag_userScore=2,

	Tag_GameScore = 10,
	Tag_Buttom = 70 ,

	Tag_Cannon = 200,

}

local TAG =  CannonLayer.enum
function CannonLayer:ctor(viewParent)
	
	self.parent = viewParent
	self._dataModel = self.parent._dataModel

	self._gameFrame  = self.parent._gameFrame
	
	--自己信息
	self.m_pUserItem = self._gameFrame:GetMeUserItem()
    self.m_nTableID  = self.m_pUserItem.wTableID
    self.m_nChairID  = self.m_pUserItem.wChairID
    self.m_dwUserID  = self.m_pUserItem.dwUserID

    self.m_cannonList = {} --炮台列表

    self._userList    = {}

    self.rootNode = nil

    self.m_userScore = 0	--用户分数 

    self.longClickCount = 0;

-- --炮台位置转换坐标
--     self.m_pCannonPos1 = 
--     {
--     	cc.p(403,736),
-- 	    cc.p(878,736),
-- 	    cc.p(403,64),
-- 	    cc.p(878,64)
-- 	}

--炮台位置
    self.m_pCannonPos = 
    {
    	cc.p(420,690),
	    cc.p(915,690),
	    cc.p(420,60),
	    cc.p(915,60)
	}


--用户信息背景
	self.m_NickPos = cc.p(78,14)
	self.m_ScorePos = cc.p(88,45)

	self.myPos = 0			--视图位置


	self:init()

	 --注册事件
    ExternalFun.registerTouchEvent(self,false)
end

function CannonLayer:init()

	--加载csb资源
	local csbNode = ExternalFun.loadCSB("csb/CannonLayer.csb", self)
	-- csbNode:setZOrder(2);
    self.rootNode = csbNode
    self.node_cannon = self.rootNode:getChildByName("node_cannon");

	--初始化自己炮台
	local myCannon = g_var(Cannon):create(self)

	myCannon:initWithUser(self.m_pUserItem)
	myCannon:setPosition(self.m_pCannonPos[myCannon.m_pos + 1])
	self:removeChildByTag(TAG.Tag_Cannon + myCannon.m_pos + 1)
	myCannon:setTag(TAG.Tag_Cannon + myCannon.m_pos + 1)
	self.mypos = myCannon.m_pos + 1
	-- self:initCannon()
	self:addChild(myCannon)

    self:initCannon()
	--位置提示
	local tipsImage = ccui.ImageView:create("game_res/pos_tips.png")
	tipsImage:setAnchorPoint(cc.p(0.5,0.0))
	tipsImage:setPosition(cc.p(myCannon:getPositionX(),180))
	self:addChild(tipsImage)

	local arrow = ccui.ImageView:create("game_res/pos_arrow.png")
	arrow:setAnchorPoint(cc.p(0.5,1.0))
	arrow:setPosition(cc.p(tipsImage:getContentSize().width/2,3))
	tipsImage:addChild(arrow)


	--跳跃动画
	local jumpUP = cc.MoveTo:create(0.4,cc.p(myCannon:getPositionX(),210))
	local jumpDown =  cc.MoveTo:create(0.4,cc.p(myCannon:getPositionX(),180))
	tipsImage:runAction(cc.Repeat:create(cc.Sequence:create(jumpUP,jumpDown), 20))

	tipsImage:runAction(cc.Sequence:create(cc.DelayTime:create(5),cc.CallFunc:create(function (  )
		tipsImage:removeFromParent()
	end)))

	local pos = self.m_nChairID
	if self._dataModel.m_reversal then 
		pos = cmd.cannonMax - pos
	end

	self:showCannonByChair(pos+1)
	self:initUserInfo(pos+1,self.m_pUserItem)
	
	local cannonInfo ={d=self.m_dwUserID,c=pos+1,cid = self.m_nChairID}
	table.insert(self.m_cannonList,cannonInfo)

end	

function CannonLayer:initCannon()

	local mypos = self.m_nChairID

	if self._dataModel.m_reversal then 
		mypos = cmd.cannonMax - mypos
	end

	for i=1,cmd.GAME_PLAYER do
		if i~= mypos+1 then
			self:HiddenCannonByChair(i)
		end
	end
end

function CannonLayer:initUserInfo(viewpos,userItem)

	local infoBG = self.rootNode:getChildByName(string.format("node_player_%d", viewpos))

	if infoBG == nil then
		return
	end

	local role_cannon = infoBG:getChildByName("role_cannon");
	role_cannon:setVisible(false);
	

	local role_icon = infoBG:getChildByName("role_icon1");
	if not userItem then
		return;
	end

	role_icon:removeAllChildren();
	local head = HeadSprite:createNormal(userItem, 100);
    if nil ~= head then
        head:setPosition(cc.p(role_icon:getContentSize().width/2,role_icon:getContentSize().height/2));
        role_icon:addChild(head, 1);
    end

	local role_name = infoBG:getChildByName("role_name");
	local name = string.EllipsisByConfig(userItem.szNickName,200,string.getConfig("fonts/round_body.ttf" , 26));
	role_name:setString(name);

	--用户分数
	if 0 ==  self._dataModel.m_secene.lPalyCurScore[1][userItem.wChairID+1] then
		self._dataModel.m_secene.lPalyCurScore[1][userItem.wChairID+1] = userItem.lScore
	end

	local role_score = infoBG:getChildByName("role_score");
	-- role_score:setString(string.format("%d", self._dataModel.m_secene.lPalyCurScore[1][userItem.wChairID+1]));
	role_score:setString(self._dataModel.m_secene.lPalyCurScore[1][userItem.wChairID+1]);
	role_score:setTag(TAG.Tag_userScore);

	if viewpos<cmd.GAME_PLAYER/2+1 then
		-- role_name:setRotation(180);
		-- role_score:setRotation(180);
	end
end

function CannonLayer:updateMultiple( mutiple,cannonPos )
	-- local infoBG = self.rootNode:getChildByName(string.format("node_player_%d", cannonPos))
	-- if infoBG == nil then
	-- 	return
	-- end

	-- local multiple_bg = infoBG:getChildByName("multiple_bg");
	-- local multipe_num = multiple_bg:getChildByName("multipe_num");
	-- multipe_num:setString(string.format("%d", mutiple));

	local cannon = self:getCannon(cannonPos);
	if not cannon then
		return;
	end
	local gunPlatformButtom = cannon:getChildByTag(TAG.Tag_Buttom+cannonPos);
	if not gunPlatformButtom then
		return;
	end
	local labelMutiple = gunPlatformButtom:getChildByTag(500);
	if nil ~= labelMutiple then
		labelMutiple:setString(string.format("%d", mutiple));
	end

	-- local gunPlatformButtom = self:getChildByTag(TAG.Tag_Buttom+cannonPos);
	-- local labelMutiple = gunPlatformButtom:getChildByTag(500);
	-- if nil ~= labelMutiple then
	-- 	labelMutiple:setString(string.format("%d", mutiple));
	-- end
	
end

function CannonLayer:updateUserScore( score,cannonpos )
	
	local infoBG = self.rootNode:getChildByName(string.format("node_player_%d", cannonpos))
	if infoBG == nil then
		return
	end
	local scoreLB = infoBG:getChildByTag(TAG.Tag_userScore)
	
	if score >= 0 and nil ~= scoreLB then
		-- scoreLB:setString(string.format("%d", 2634125049))
		scoreLB:setString(score);
	end

	local mypos = self.m_nChairID
	if self._dataModel.m_reversal then 
		mypos = cmd.cannonMax - mypos
	end

	if mypos == cannonpos - 1 then
		self.parent._gameView:updateUserScore(score)
	end
end


function CannonLayer:HiddenCannonByChair( chair )
	print("隐藏隐藏.........."..chair)

	local infoBG = self.rootNode:getChildByName(string.format("node_player_%d", chair));
	infoBG:setVisible(false);

	--倍数
	local cannon = self:getCannon(chair);
	if not cannon then
		return;
	end
	cannon:removeChildByTag(TAG.Tag_Buttom + chair);

	-- self:removeChildByTag(TAG.Tag_Buttom + chair);
end

function CannonLayer:showCannonByChair( chair )

	local infoBG = self.rootNode:getChildByName(string.format("node_player_%d", chair))

	if infoBG == nil then
		return
	end

	infoBG:setVisible(true) --玩家信息

	local multiple_bg = infoBG:getChildByName("multiple_bg");
	multiple_bg:setVisible(false);

	local multipe_num = multiple_bg:getChildByName("multipe_num");

	--倍数显示
	local cannon = self:getCannon(chair);
	if not cannon then
		return;
	end

	local gunPlatformButtom = cc.Sprite:create("main/multiple_bottom.png");
	if chair > 2 then
		gunPlatformButtom:setPosition(50, 25);
	else
		gunPlatformButtom:setPosition(50, 75);
	end
	
	gunPlatformButtom:setTag(TAG.Tag_Buttom+chair);
	cannon:removeChildByTag(TAG.Tag_Buttom+chair);
	cannon:addChild(gunPlatformButtom,5);

	-- local gunPlatformButtom = cc.Sprite:create("main/multiple_bottom.png");
	-- gunPlatformButtom:setPosition(multiple_bg:getPositionX(), multiple_bg:getPositionY());
	-- gunPlatformButtom:setTag(TAG.Tag_Buttom+chair);
	-- self:removeChildByTag(TAG.Tag_Buttom+chair);
	-- self:addChild(gunPlatformButtom);
	-- gunPlatformButtom:set
	
	--倍数
	local labelMutiple = cc.LabelAtlas:create("1","main/score_font.png",18,24,string.byte("0"));
	labelMutiple:setTag(500);
	labelMutiple:setAnchorPoint(0.5,0.5);
	labelMutiple:setPosition(gunPlatformButtom:getContentSize().width/2,gunPlatformButtom:getContentSize().height/2);
	gunPlatformButtom:removeChildByTag(500);
	gunPlatformButtom:addChild(labelMutiple,1);
end

function CannonLayer:getCannon(pos)
	local cannon = self:getChildByTag(pos + TAG.Tag_Cannon)
	return cannon 
end


function CannonLayer:getCannoByPos( pos )

	local cannon = self:getChildByTag(TAG.Tag_Cannon + pos)
	return  cannon

end

function CannonLayer:getChairByCannon( viewid )
	local chairID = yl.INVALID_CHAIR
	if self._dataModel.m_reversal then 
		chairID = math.abs(6-viewid)
	else	
	   chairID = viewid - 1 
	end
	return chairID
end


function CannonLayer:getUserIDByCannon(viewid)

	local userid = 0
	if #self.m_cannonList > 0 then
		for i=1,#self.m_cannonList do
			local cannonInfo = self.m_cannonList[i]
			if cannonInfo.c == viewid then
				userid = cannonInfo.d
				break
			end
		end
 	end
	
	 return userid
end

function CannonLayer:onEnter( )
	
end


function CannonLayer:onEnterTransitionFinish(  )

  
end

function CannonLayer:onExit( )

	self.m_cannonList = nil
end

function CannonLayer:onTouchBegan(touch, event)

	if self._dataModel._exchangeSceneing  then 	--切换场景中不能发炮
		showToast(self, "潮水来临，请稍后发炮", 2)
		return false
	end

	local cannon = self:getCannon(self.mypos)

	if nil ~= cannon then
		local pos = touch:getLocation()

		cannon:shoot(cc.p(pos.x,pos.y), true)

		self.parent:setSecondCount(60)
		
	end

	-- -- 炮台状态
	-- self._dataModel.isExchageConnonState = false;
	-- if self.longClick then
 --        -- 注销时钟
 --        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.longClick);
 --        self.longClick = nil;
 --    end

	return true
end

function CannonLayer:onTouchMoved(touch, event)
	
	local cannon = self:getCannon(self.mypos)

	if nil ~= cannon then
		local pos = touch:getLocation()

		cannon:shoot(cc.p(pos.x,pos.y), true)
		self.parent:setSecondCount(60)

	end

end

function CannonLayer:onTouchEnded(touch, event )
	
	local cannon = self:getCannon(self.mypos)

	if nil ~= cannon then
		local pos = touch:getLocation()

		cannon:shoot(cc.p(pos.x,pos.y), false)
		self.parent:setSecondCount(60)
	end

	-- -- -- 炮台状态
	-- self.longClickCount = 1;
	-- self._dataModel.isExchageConnonState = false;
	-- self.longClick = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
	-- 	if self.longClickCount >= 2 then
	-- 		self._dataModel.isExchageConnonState = true;

	-- 		if self.longClick then
	-- 	        -- 注销时钟
	-- 	        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.longClick);
	-- 	        self.longClick = nil;
	-- 	    end
	-- 	end
 --        self.longClickCount = self.longClickCount + 1;
 --    end, 1, false)

end

--用户进入
function CannonLayer:onEventUserEnter( wTableID,wChairID,useritem )
    print("add user " .. useritem.wChairID .. "; nick " .. useritem.szNickName)
    -- print("---------------------33333333333333444444444444444-----------------------")
    -- dump(useritem,"-----------------------------",6)

    if wChairID > cmd.cannonMax or wTableID ~= self.m_nTableID or wChairID == self.m_nChairID then
    	return
    end

    local pos = wChairID
    if self._dataModel.m_reversal then 
    	pos = cmd.cannonMax -pos 
    end

    if pos + 1 == self.m_pos then  --过滤自己
 		return
 	end

    -- self:showCannonByChair(pos + 1)
 	self:removeChildByTag(TAG.Tag_Cannon + pos + 1)

 	if #self.m_cannonList > 0 then
 		for i=1,#self.m_cannonList do
 			local cannonInfo = self.m_cannonList[i]
 			if cannonInfo.d == useritem.dwUserID then
 				table.remove(self.m_cannonList,i)
 				break
 			end
 		end
 	end

 	if #self._userList > 0 then
 		for i=1,#self._userList do
 			local Item = self._userList[i]
 			if Item.dwUserID == useritem.dwUserID then
 				table.remove(self._userList,i)
 				break
 			end
 		end
 	end
 	
    local Cannon = g_var(Cannon):create(self)
	Cannon:initWithUser(useritem)
	Cannon:setPosition(self.m_pCannonPos[Cannon.m_pos + 1])
	Cannon:setTag(TAG.Tag_Cannon + Cannon.m_pos + 1)
	self:addChild(Cannon)
	self:initUserInfo(pos + 1,useritem)

	self:showCannonByChair(pos + 1)

	local cannonInfo ={d=useritem.dwUserID,c=pos+1,cid = useritem.wChairID}
	table.insert(self.m_cannonList,cannonInfo)

	table.insert(self._userList, useritem)
end

--用户状态
function CannonLayer:onEventUserStatus(useritem,newstatus,oldstatus)

  		if oldstatus.wTableID ~= self.m_nTableID  then
  			print("不是本桌用户....")
  			return
		end

        if newstatus.cbUserStatus == yl.US_FREE or  newstatus.cbUserStatus == yl.US_NULL then
                 
          	    if #self.m_cannonList > 0 then
          	    	for i=1,#self.m_cannonList do

	          	    	local cannonInfo = self.m_cannonList[i]
	          	    	if cannonInfo.d == useritem.dwUserID then
	          	    		print("用户离开"..cannonInfo.c)

	          	    		self:HiddenCannonByChair(cannonInfo.c)
	          	    		local  chair  = self:getChairByCannon(cannonInfo.c)
	          	    		self._dataModel.m_secene.lPalyCurScore[1][cannonInfo.cid+1] = 0 
	          	    		self._dataModel.m_secene.nMultipleIndex[1][cannonInfo.cid+1] = 0
	          	    		table.remove(self.m_cannonList,i)

		          	    	if #self._userList > 0 then
						 		for i=1,#self._userList do
						 			local Item = self._userList[i]
						 			if Item.dwUserID == useritem.dwUserID then
						 				table.remove(self._userList,i)
						 				break
						 			end
						 		end
						 	end

	          	    	    local cannon = self:getChildByTag(TAG.Tag_Cannon + cannonInfo.c)
				          	if nil ~= cannon then
				          		--金币叠加移除
				          		for k,v in pairs(cannon.m_goldList) do
				          			if nil ~=  cannon.m_goldList[k] then
				          				cannon.m_goldList[k]:removeAllChildren();
				          				cannon.m_goldList[k]:removeFromParent();
				          			end
				          		end

				          		cannon:removeChildByTag(1000)
					          	cannon:removeTypeTag()
				          	    cannon:removeLockTag()
				          	    cannon:removeFromParent()
				          	end

	          	    	 
	          	    		break
	          	    	end
          	   		 end
          	    end 
        else

        		self._gameFrame:QueryUserInfo( self.m_nTableID,useritem.wChairID)
        end

end

return CannonLayer