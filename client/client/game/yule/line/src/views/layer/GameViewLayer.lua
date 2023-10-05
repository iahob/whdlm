local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
	return gameViewLayer
end)


local module_pre = "game.yule.line.src"
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"

local cmd = module_pre .. ".models.CMD_Game"
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local GameRollList = appdf.req(module_pre .. ".views.layer.GameRollList")
local GameRewardLayer = require(module_pre..".views.layer.GameRewardLayer") 
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")


local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")

GameViewLayer.RES_PATH              = device.writablePath.. "game/yule/line/res/"

local ButtonTag = 
{
	"Tag_Top" ,--顶部
	"Tag_Menu",
	"Tag_BT_Return",--返回按钮
	"Tag_BT_Bank",
	"Tag_BT_Horn",--喇叭
	"Tag_BT_Hosting",--托管
	"Tag_BT_Quit",--退出游戏
	"Tag_BT_Setting",--游戏设置
	"Tag_BT_SelfHead",--自己头像
	"Tag_BT_Goldplus",--金币加号
	"Tag_BT_Beanplus",--游戏豆加号
	"Tag_BT_Reward",--奖励表
	
	"Tag_BT_Begin",--开始
	"Tag_BT_Stop",--停止
	
	"Tag_Gold_Score",--金币
	"Tag_Bean_Score",--游戏豆
	
	"Tag_BT_Plus_Score",--增加下注
	"Tag_BT_Minus_Score",--减少下注

	"BT_CLOSEBANK",
	"BT_TAKESCORE",
	"BT_SAVESCORE",

	"Tag_Line_Layout",--线显示层
	"Tag_Egg_Layout",--砸蛋层
	"Tag_End_Layout",--结束分数显示层

	"Tag_REWARD_LAYER",
	"Tag_SET_LAYER",
	"Tag_BANK_LAYER",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(0, ButtonTag);

local zorders = 
{
	"Zorders_Pool_Score",--奖池
	"Zorders_Win_Score",--本局得分
	"Zorders_Plus_Score",--本局下注
	"Zorders_Line_Score",--单线下注
	

	"Zorders_Txt_Horn",--喇叭内容显示框
	"Zorders_Horn_Layer",--喇叭内容输入层
	
	"Zorders_Line_Layout",--线显示层
	"Zorders_Egg_Layout",--砸蛋层
	"Zorders_End_Layout",--结束分数显示层

	"Zorders_Reward_Layout",--奖励层
	"Zorders_Set_Layer",--设置层
	"Zorders_Bank_Layer",--银行层
}
local TAG_ZORDER = ExternalFun.declarEnumWithTable(1, zorders);

GameViewLayer.numpoint = {
	{x=10,y=483},   {x=10,y=303},   {x=10,y=123},
	{x=105,y=610}, {x=315,y=610}, {x=530,y=610}, 
	{x=678,y=613}, {x=677,y=6}
}

GameViewLayer.AnimPngNum          = 3                       --动画图片数量
GameViewLayer.AnimEgg             = "animegg"               --砸蛋动画
GameViewLayer.AnimEggEffect       = "animeggeffect"         --蛋特效
GameViewLayer.Anim3LianBG         = "anim3lianbg"           --三连动画背景
GameViewLayer.Anim3LianStar       = "anim3lianstar"         --三连星星动画
GameViewLayer.Anim9LianBG         = "anim9lianbg"           --九连动画背景
GameViewLayer.Anim9LianBGChange   = "anim9lianbgchange"     --九连动画背景切换
GameViewLayer.Anim9LianLight      = "anim9lianlight"        --九连扫光动画
GameViewLayer.AnimGold            = "animgold"              --金币动画
GameViewLayer.AnimToubi           = "animtoubi"             --投币动画

function GameViewLayer:getParentNode(  )
	return self._scene
end

function GameViewLayer:ctor(scene)
	--注册node事件
	ExternalFun.registerNodeEvent(self)
	self._scene = scene
	--添加路径
	self:addPath()
	--预加载资源
	self:loadTextures()
	 self:initData();
	--初始化csb界面
	self:initCsbRes();

	--播放背景音乐
	ExternalFun.playBackgroudAudio("BackSound.mp3")
end
--增加搜索路径
function GameViewLayer:addPath( )
	--搜索路径
	local gameList = self:getParentNode():getParentNode():getApp()._gameList;
	local gameInfo = {};
	for k,v in pairs(gameList) do 
		if tonumber(v._KindID) == tonumber(g_var(cmd).KIND_ID) then
			gameInfo = v;
			break;
		end
	end
	if nil ~= gameInfo._Module then
		self._searchPath = device.writablePath.."game/" .. gameInfo._Module .. "/res/";
		cc.FileUtils:getInstance():addSearchPath(self._searchPath);
	end
end

function GameViewLayer:readAnimation(file, key, num, time)
	local animation = cc.Animation:create()
	for i=0,num do
		local frameName = string.format(file.."%d.png", i)
		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(frameName) 
		animation:addSpriteFrame(frame)
	end
	animation:setDelayPerUnit(time)
	cc.AnimationCache:getInstance():addAnimation(animation, key)
end

function GameViewLayer:loadTextures()
	--读取图片plist
	for index=1,3 do
		cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("anim/anim_%d.plist",index-1))
		cc.Director:getInstance():getTextureCache():addImage(string.format("anim/anim_%d.png",index-1));
	end
	
	self:readAnimation("anim_egg_", self.AnimEgg, 16, 0.09);
	self:readAnimation("anim_egg_effect_", self.AnimEggEffect, 9, 0.045);
	self:readAnimation("anim_3lian_bg_", self.Anim3LianBG, 16, 0.06);
	self:readAnimation("anim_3lian_star_", self.Anim3LianStar, 14, 0.06);
	self:readAnimation("anim_9lian_bg_", self.Anim9LianBG, 11, 0.06);
	self:readAnimation("anim_9lian_bg_", self.Anim9LianBGChange, 11, 0.2);
	self:readAnimation("anim_9lian_light_", self.Anim9LianLight, 4, 0.1);
	self:readAnimation("anim_gold_", self.AnimGold, 9, 0.03);

end

function GameViewLayer:initData(  )

	self.m_gamedata = {}
	self.m_bTaskComplete = {}
	--self.m_plusindex = 1
	self.m_rolllist = {}

	self.isGongGao = false                                          --是否正在播放公告
end

function GameViewLayer:onExit()
	--停止定时器
	for i=1,3 do
		if self.m_rolllist[i].m_scheduleUpdate then
			local scheduler = cc.Director:getInstance():getScheduler()
			scheduler:unscheduleScriptEntry(self.m_rolllist[i].m_scheduleUpdate)
			self.m_rolllist[i].m_scheduleUpdate = nil
		end
	end

	--播放大厅背景音乐
	ExternalFun.playPlazzBackgroudAudio()
	--重置搜索路径
	local oldPaths = cc.FileUtils:getInstance():getSearchPaths();
	local newPaths = {};
	for k,v in pairs(oldPaths) do
		if tostring(v) ~= tostring(self._searchPath) then
			table.insert(newPaths, v);
		end
	end
	cc.FileUtils:getInstance():setSearchPaths(newPaths);
end
---------------------------------------------------------------------------
--初始化界面
function GameViewLayer:initCsbRes(  )
	self.m_rootLayer,self._csbNode = ExternalFun.loadRootCSB(GameViewLayer.RES_PATH .."GameLayer.csb", self);
	--初始化按钮
	self:initUI(self._csbNode)
end
--初始化按钮
function GameViewLayer:initUI( csbNode )
	--按钮回调方法
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.began then
			ExternalFun.popupTouchFilter(1, false)
		elseif eventType == ccui.TouchEventType.canceled then
			ExternalFun.dismissTouchFilter()
		elseif eventType == ccui.TouchEventType.ended then
			ExternalFun.dismissTouchFilter()
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end
	--开始
	self.m_btnStart = csbNode:getChildByName("Button_start");
	self.m_btnStart:setTag(TAG_ENUM.Tag_BT_Begin);
	self.m_btnStart:addTouchEventListener(btnEvent);
	--停止
	self.m_btnStop = csbNode:getChildByName("Button_stop");
	self.m_btnStop:setTag(TAG_ENUM.Tag_BT_Stop);
	self.m_btnStop:addTouchEventListener(btnEvent);
	--设置
	local Button_set = csbNode:getChildByName("Button_set");
	Button_set:setTag(TAG_ENUM.Tag_BT_Setting);
	Button_set:addTouchEventListener(btnEvent);
	--退出
	local Button_quit = csbNode:getChildByName("Button_quit");
	Button_quit:setTag(TAG_ENUM.Tag_BT_Quit);
	Button_quit:addTouchEventListener(btnEvent);
	--银行按钮
	local Button_bank = csbNode:getChildByName("Button_bank");
	Button_bank:setTag(TAG_ENUM.Tag_BT_Bank);
	Button_bank:addTouchEventListener(btnEvent);
	--托管
	self.m_btnAuto = csbNode:getChildByName("Button_trusteeship");
	self.m_btnAuto:setTag(TAG_ENUM.Tag_BT_Hosting);
	self.m_btnAuto:addTouchEventListener(btnEvent);
	-------------------------
	--彩池
	self.m_spritePool = csbNode:getChildByName("im_pool_bg");
	self.m_textPoolScore = self.m_spritePool:getChildByName("Text_pot")
	--彩池动画
	local animationPool = cc.Animation:create()
	for i=0,1 do
		local str = string.format("game/im_pool_bg_%d.png",i)
		animationPool:addSpriteFrameWithFile(str)
	end
	animationPool:setDelayPerUnit(0.5)
	local animatePool = cc.Animate:create(animationPool)
	self.m_spritePool:runAction(cc.RepeatForever:create(animatePool))

	--本局得分
	local spriteWin = csbNode:getChildByName("im_win_score_bg");
	self.m_textWinScore = spriteWin:getChildByName("Text_winscore")
	self.m_textWinScore:setString(self:getScoreWithLength(0,12))
	--本局下注
	local plusScore = csbNode:getChildByName("im_plus_score_bg");
	--加注
	local buttonAdd = plusScore:getChildByName("Button_add")
	buttonAdd:setTag(TAG_ENUM.Tag_BT_Plus_Score)
	buttonAdd:addTouchEventListener(btnEvent);
	--减注
	local buttonReduce = plusScore:getChildByName("Button_reduce")
	buttonReduce:setTag(TAG_ENUM.Tag_BT_Minus_Score)
	buttonReduce:addTouchEventListener(btnEvent);
	--总押注
	self.m_textAllyafen = plusScore:getChildByName("Text_allin");
	--单线押注
	self.m_textOneyafen = plusScore:getChildByName("Text_onein");
	-------------------------
	--滚动层
	self.frameBg = csbNode:getChildByName("im_frame_bg")

	--初始化滚动层
	self:initRolllist()
	--投币动画
	self:readtoubianim();

	--初始化玩家信息
	self:initUserInfo(csbNode);
end

function GameViewLayer:initRolllist( )
	for i=1,3 do
		self.m_rolllist[i] = GameRollList:create(self._scene)
		self.m_rolllist[i]:setPosition(cc.p(50+208*(i-1),35))

		self.frameBg:addChild(self.m_rolllist[i])

	end
end

--初始化玩家信息
function GameViewLayer:initUserInfo( csbNode )
	--玩家头像
	local tmp = csbNode:getChildByName("Sprite_head")
	local head = PopupInfoHead:createNormal(self._scene:GetMeUserItem(), tmp:getContentSize().width-10)
	head:setPosition(tmp:getPosition())
	csbNode:addChild(head)
	head:enableInfoPop(true,cc.p(260,300),cc.p(0,1))

	--玩家游戏币
	local nodeCoin = csbNode:getChildByName("im_score_bg")
	self.m_textUserCoin = nodeCoin:getChildByName("Text_userscore")
	-- --豆
	-- local nodeBean = csbNode:getChildByName("im_bean_bg")
	-- nodeBean:setVisible(false);
	-- self.m_textUserBean = nodeBean:getChildByName("Text_userscore")

end

--游戏的奖励层初始化
function GameViewLayer:initReWardLayer()
	--奖励层
	if self.m_pReWardLayer == nil then
		self.m_pReWardLayer = GameRewardLayer:create(self._scene.m_gamedata)
		self._csbNode:addChild(self.m_pReWardLayer,TAG_ZORDER.Zorders_Reward_Layout)
	end

end
--按钮回调
function GameViewLayer:onButtonClickedEvent(tag,ref)
	if tag == TAG_ENUM.Tag_BT_Quit then             --退出
		self._scene.m_bIsLeave = true
		self:getParentNode():onQueryExitGame()
		ExternalFun.playClickEffect()
	elseif tag == TAG_ENUM.Tag_BT_Begin  then       --开始游戏
		print("游戏开始")
		self:getParentNode():onGameStart()
        ExternalFun.playSoundEffect("StartLogo.wav")
	elseif tag == TAG_ENUM.Tag_BT_Stop  then       --停止游戏
		print("游戏停止")
		self._scene.isAtOnceStop = true;

		self:getParentNode():onGameStop()
		ExternalFun.playClickEffect()
	elseif tag == TAG_ENUM.Tag_BT_Setting  then     --设置
		self:onSetLayer()
        ExternalFun.playSoundEffect("OpenWidnow.wav")
	elseif tag == TAG_ENUM.Tag_BT_Bank then
		--银行未开通
        if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            showToast(self, "初次使用，请先开通银行！", 1)
             return 
        end
		if nil == self._scene.m_gamestatus or g_var(cmd).GAME_SCENE_FREE ~= self._scene.m_gamestatus then
			print("m_gamestatus",self._scene.m_gamestatus)
			showToast(self,"游戏过程中不能进行银行操作",1)
			return
		end

		--房间规则
		local rule = self:getParentNode()._roomRule
		if rule == yl.GAME_GENRE_SCORE
		or rule == yl.GAME_GENRE_EDUCATE then 
			print("练习 or 积分房")
		end
		-- 当前游戏币场
		local rom = GlobalUserItem.GetRoomInfo()
		if nil ~= rom then
			if rom.wServerType ~= yl.GAME_GENRE_GOLD then
				showToast(self, "当前房间禁止操作银行!", 1)
				return
			end
		end

		if false == self:getParentNode():getFrame():OnGameAllowBankTake() then
			--showToast(self,"不允许银行取款操作操作",1)
			--return
		end
		if nil == self.m_bankLayer then
			self:createBankLayer()
		end
        ExternalFun.playSoundEffect("OpenWidnow.wav")
		self.m_bankLayer:setVisible(true)
		self:refreshScore()
	elseif tag == TAG_ENUM.Tag_BT_Plus_Score  then        --加注
		if self._scene.m_gamestatus == g_var(cmd).GAME_SCENE_FREE and not self._scene.m_bIsAuto then
			self:getParentNode():onAddScore()
			ExternalFun.playSoundEffect("shangfen.mp3")
		end

		-- self:getParentNode():onAddScore()
		-- ExternalFun.playSoundEffect("shangfen.mp3")
	elseif tag == TAG_ENUM.Tag_BT_Minus_Score  then        --减注
		if self._scene.m_gamestatus == g_var(cmd).GAME_SCENE_FREE and not self._scene.m_bIsAuto then
			self._scene:onSubScore()
			ExternalFun.playSoundEffect("shangfen1.mp3")
		end

		-- self._scene:onSubScore()
		-- ExternalFun.playSoundEffect("shangfen1.mp3")
	elseif tag == TAG_ENUM.Tag_BT_Hosting  then     --自动游戏
		print("自动游戏")
		self:getParentNode():onAutoStart()
		ExternalFun.playSoundEffect("ButtonDown.wav")
		--ExternalFun.playClickEffect()
	elseif tag == TAG_ENUM.BT_CLOSEBANK then
		if nil ~= self.m_bankLayer then
			self.m_bankLayer:setVisible(false)
		end
	elseif tag == TAG_ENUM.BT_TAKESCORE then
		self:onTakeScore()
	elseif tag == TAG_ENUM.BT_SAVESCORE then
		self:onSaveScore()
	else
		showToast(self,"功能尚未开放！",1)
	end
end

--停止转动
function GameViewLayer:onGameStop( bStopNew )
	print(".bStopNew",bStopNew)
	if bStopNew then
		self._scene.isAtOnceStop = false;
		
		for i=1,3 do
			self.m_rolllist[i]:setStatus(2)
		end
		self:gameEndScoreDeal()
		self._scene.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
		self:changeStartButton(false)

	else
		self.m_rolllist[1]:setStatus(1)
		local lastDeLayTime = self._scene.m_winScore > 0 and 3.8 or 3.0
		local pAction = cc.Sequence:create(
			cc.DelayTime:create(2),
			cc.CallFunc:create(function (  )
				print("self._scene.m_gamestatus ",self._scene.m_gamestatus)
				if self._scene.m_gamestatus == g_var(cmd).GAME_SCENE_SLOW then
					self.m_rolllist[2]:setStatus(1)
				end
			end),
			cc.DelayTime:create(2),
			cc.CallFunc:create(function (  )
				print("self.m_gamestatus 443 ",self.m_gamestatus)
				if self._scene.m_gamestatus == g_var(cmd).GAME_SCENE_SLOW then
					self.m_rolllist[3]:setStatus(1)
				end
			end),
			cc.DelayTime:create(lastDeLayTime),
			cc.CallFunc:create(function (  )
				self:changeStartButton(false)
				self:gameEndScoreDeal()
				--self._scene.m_bIsItemMove = false
			end)
			)
		self:runAction(pAction)
	end
end

--声音设置界面
function GameViewLayer:onSetLayer(  )
	local mgr = self._scene._scene:getApp():getVersionMgr()
	local verstr = mgr:getResVersion(g_var(cmd).KIND_ID) or "0"
	verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
	local set = SettingLayer:create(verstr)
	set.m_parent = self
	self._csbNode:addChild(set,TAG_ZORDER.Zorders_Set_Layer)
end

--更新自己的分数
function GameViewLayer:updateSelfScore()
	--local ptop = self:getChildByTag()
	-- auto ptop = this->getChildByTag(Tag_Top);
	-- auto pgoldscore = static_cast<Label *>(ptop->getChildByTag(Tag_Gold_Score));
	-- if (pgoldscore) {
	--     pgoldscore->setString(getScorewithComma(HallDataMgr::getInstance()->m_UserScore, ":"));
	--     if (pgoldscore->getContentSize().width > 150.f) {
	--         pgoldscore->setScale(150/pgoldscore->getContentSize().width);
	--     }

	-- }
	
	-- auto pbeanscore = static_cast<Label *>(ptop->getChildByTag(Tag_Bean_Score));
	-- if (pbeanscore) {
	--     pbeanscore->setString(getScorewithComma(HallDataMgr::getInstance()->m_Bean, ":"));
	--     if (pbeanscore->getContentSize().width > 150.f) {
	--         pbeanscore->setScale(150/pbeanscore->getContentSize().width);
	--     }
	-- }
end

--更新下注信息   
function GameViewLayer:updatePlusScore()
	--总共下注
	if self.m_textAllyafen then
		print("self:getParentNode().m_plusindex",self:getParentNode().m_plusindex)

		local allPlusScore = self:getParentNode().m_gamedata.nBetScore[1][self:getParentNode().m_plusindex]*g_var(cmd).LINE_INDEX_MAX
		allPlusScore = self:getScoreWithLength(allPlusScore,12)
		--print("allPlusScore",allPlusScore)
		self.m_textAllyafen:setString(allPlusScore)
	end
	--单线下注
	if self.m_textOneyafen then
		local onePlusScore = self:getParentNode().m_gamedata.nBetScore[1][self:getParentNode().m_plusindex]
		onePlusScore = self:getScoreWithLength(onePlusScore,8)
		--print("onePlusScore",onePlusScore)
		self.m_textOneyafen:setString(onePlusScore)
	end
	--赢的分数
	-- if self.m_textWinScore then
	-- 	--local onePlusScore = self:getParentNode().m_gamedata.nBetScore[1][self.m_plusindex]
	-- 	local winScore = self:getScoreWithLength(0,10)
	-- 	self.m_textWinScore:setString(winScore)
	-- end
end
--更新奖池信息
function GameViewLayer:updatePoolScore( )
	--奖池
	if  self.m_textPoolScore then  
		local poolScore = self:getParentNode().m_gamedata.nRoomLotteryPool
		poolScore = self:getScoreWithLength(poolScore,12)
		self.m_textPoolScore:setString(poolScore)
	end
end
--更新赢钱
function GameViewLayer:updateWinScore()
	if self.m_textWinScore then
		self.m_textWinScore:setString(self:getScoreWithLength(self._scene.m_winScore+self._scene.m_taskScore,12))
	end
end
--更新托管纹理
function GameViewLayer:updateAutoBtn(isAuto)
	local imageStr = isAuto == true and "game/bt_cancle_hosting.png" or "game/bt_hosting_0.png"
	self.m_btnAuto:loadTextureNormal(imageStr)
	
	self.m_btnStart:setEnabled(not isAuto)
	self.m_btnStop:setEnabled(not isAuto)
end

--在分数前加0
function GameViewLayer:getScoreWithLength( score,length )
	local len = length or 10
	local scorestr = tostring(score)
	local scorelength = string.len(scorestr)
	if scorelength < len then
		local tempStr = "%0".. len .. "d"
		scorestr = string.format(tempStr,scorestr)
	end
	return scorestr
end

--金币动画预加载
function GameViewLayer:readtoubianim(  )
	local animation = cc.Animation:create()
	for i=0,5 do
		local str = string.format("game/im_toubi_%d.png",i)
		animation:addSpriteFrameWithFile(str)
	end
	animation:setDelayPerUnit(0.08)
	cc.AnimationCache:getInstance():addAnimation(animation,self.AnimToubi)
end

--投币闪光动画
function GameViewLayer:showTouBiAni(  )
	local plight = cc.Sprite:create("game/im_toubi_light.png")
	plight:setPosition(cc.p(1145,232))
	plight:setAnchorPoint(0.5,0)
	self:addChild(plight,1)
	plight:setVisible(false)
	plight:runAction(cc.Sequence:create(
		cc.DelayTime:create(0.24),
		cc.CallFunc:create(function (  )
			plight:setVisible(true)
		end),
		cc.DelayTime:create(0.26),
		cc.CallFunc:create(function (  )
			plight:removeFromParent()
		end)
		))
	local ptoubigolde = cc.Sprite:create("game/im_toubi_0.png")
	ptoubigolde:setAnchorPoint(0.5,0)
	ptoubigolde:setPosition(cc.p(plight:getPositionX(),220))
	local panimate = cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.AnimToubi))
	ptoubigolde:runAction(cc.Sequence:create(
		panimate,
		cc.CallFunc:create(function (  )
			ptoubigolde:removeFromParent()
		end)
		))
	self:addChild(ptoubigolde)
end

--蛋动画
function GameViewLayer:showCrackEgg()
	local pegglayout = ccui.Layout:create()
	pegglayout:setTag(TAG_ENUM.Tag_Egg_Layout)
	pegglayout:setContentSize(cc.size(500, 420));
	pegglayout:setAnchorPoint(cc.p(0.5, 0.5));
	pegglayout:setPosition(cc.p(667, 375));
	self._csbNode:addChild(pegglayout,TAG_ZORDER.Zorders_Egg_Layout);
	--print("蛋动画1")
	local pegg = display.newSprite("#anim_egg_16.png");
	pegg:setPosition(pegg:getContentSize().width/2,pegg:getContentSize().height/2);
	pegg:runAction(cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.AnimEgg)));
	pegglayout:addChild(pegg);
	--print("蛋动画2")
	pegglayout:runAction(cc.Sequence:create(
		cc.DelayTime:create(1.17), 
		cc.CallFunc:create(function ()
			local peggeffect = display.newSprite("#anim_egg_effect_9.png");
			peggeffect:runAction(cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.AnimEggEffect)));
			peggeffect:setPosition(cc.p(pegg:getPositionX()-24, pegg:getPositionY()+30));
			pegglayout:addChild(peggeffect,1);
		end
	)))
	--print("蛋动画3")
	pegglayout:runAction(cc.Sequence:create(
		cc.DelayTime:create(1.43), 
		cc.CallFunc:create(function ()
			local popnum = #self._scene.m_egglist;
			local poplength = popnum*160+40;
			local ppoplayout = ccui.Layout:create();
			ppoplayout:setContentSize(cc.size(poplength, 160));
			ppoplayout:setAnchorPoint(0.5, 0);
			ppoplayout:setPosition(cc.p(pegglayout:getContentSize().width/2,65));
			if poplength > 550 then
				ppoplayout:setScale(550/poplength);
			end
			pegglayout:addChild(ppoplayout);
			
			for index=1,#self._scene.m_egglist do
				local pinteger = self._scene.m_egglist[index];
				local spriteindex = pinteger>8 and 8 or pinteger;
				local psprite = cc.Sprite:create(string.format("game/im_element_%d.png", spriteindex));
				psprite:setPosition(cc.p(20+160*(index-1), 80));
				ppoplayout:addChild(psprite);
				ppoplayout:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.MoveBy:create(0.63, cc.p(0, 5)), cc.MoveBy:create(0.63, cc.p(0, -5)))));
			end

		end
	)));
	-- print("蛋动画4")
	-- --4秒后移除
	pegglayout:runAction(cc.Sequence:create(
		cc.DelayTime:create(4), 
		cc.CallFunc:create(function (  )

			pegglayout:removeFromParent();     
		end
	)))
	
end
 
function GameViewLayer:gameBeginClearShow(  )
	  self._scene.m_winScore = 0;
	  self._scene.m_taskscore = 0;
	  self:updateWinScore();

	  --清空
  	local pLineLayout = self._csbNode:getChildByTag(TAG_ENUM.Tag_Line_Layout)
  	if pLineLayout then
	  	pLineLayout:stopAllActions()
	  	self._csbNode:removeChildByTag(TAG_ENUM.Tag_Line_Layout);
  	end

  	local pEggLayout = self._csbNode:getChildByTag(TAG_ENUM.Tag_Egg_Layout)
    if pEggLayout then
	  pEggLayout:stopAllActions()
	  self._csbNode:removeChildByTag(TAG_ENUM.Tag_Egg_Layout);
    end

    local pEndLayout = self._csbNode:getChildByTag(TAG_ENUM.Tag_End_Layout)
    if pEndLayout then
	  pEndLayout:stopAllActions()
	  self._csbNode:removeChildByTag(TAG_ENUM.Tag_End_Layout);
    end
end

--游戏结算分数处理
function GameViewLayer:gameEndScoreDeal( )

	self:updateSelfScore();
	self:updateWinScore();
	print("self._scene.m_winScore",self._scene.m_winScore)
	if self._scene.m_winScore > 0 then
		ExternalFun.playSoundEffect("GameWin.wav")
		self:showGameLine()
		if #self._scene.m_egglist ~= 0 then
			self:showCrackEgg()
		end
	else
		ExternalFun.playSoundEffect("GameFail.wav")
	end
	self._scene.m_gamestatus = g_var(cmd).GAME_SCENE_FREE
end

--显示中奖线
function GameViewLayer:showGameLine( )

	--先判断是否存在
	local pLastLineLayout = self._csbNode:getChildByTag(TAG_ENUM.Tag_Line_Layout)
	if pLastLineLayout then
		pLastLineLayout:stopAllActions()
		self._csbNode:removeChildByTag(TAG_ENUM.Tag_Line_Layout);
	end

	local lineLayout = ccui.Layout:create()
	lineLayout:setTag(TAG_ENUM.Tag_Line_Layout)
	lineLayout:setContentSize(cc.size(567,528))
	lineLayout:setAnchorPoint(1,1)
	lineLayout:setPosition(cc.p(567+164,320+210))
	self._csbNode:addChild(lineLayout,TAG_ZORDER.Zorders_Line_Layout)
	--遍历每条线
	for index=1,g_var(cmd).LINE_INDEX_MAX do
		-- dump(self._scene.m_lineMultiple)
		-- print(index)
		if self._scene.m_lineMultiple[index] then
			local lineStr1 = ""
			local anchorVec2
			local point
			if index < 4 then
				lineStr1 = "game/im_line_0.png"
				anchorVec2 = cc.p(0, 0.5)
				point = cc.p(self.numpoint[index].x-24, self.numpoint[index].y);
			elseif  index < 7   then
				lineStr1 = "game/im_line_1.png"
				anchorVec2 = cc.p(0, 1)
				point = cc.p(self.numpoint[index].x, self.numpoint[index].y+24);
			elseif  index == 7   then
				lineStr1 = "game/im_line_2.png"
				anchorVec2 = cc.p(1, 1)
				point = cc.p(self.numpoint[index].x+24, self.numpoint[index].y+25);
			else
				lineStr1 = "game/im_line_3.png"
				anchorVec2 = cc.p(1, 0)
				point = cc.p(self.numpoint[index].x+24, self.numpoint[index].y-20);             
			end

			-- local pline = cc.Sprite:create(lineStr1);
			-- pline:setAnchorPoint(anchorVec2);
			-- pline:setPosition(point);
			-- lineLayout:addChild(pline);
			
			local pline1 = cc.Sprite:create(lineStr1);
			pline1:setAnchorPoint(anchorVec2);
			pline1:setPosition(point);
			--pline1:setBlendFunc(gl.GL_SRC_ALPHA,gl.GL_ONE);
			pline1:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.FadeTo:create(0.8, 150), cc.FadeTo:create(0.8, 255))));
			lineLayout:addChild(pline1);
		end
	end

	local waittime = #self:getParentNode().m_egglist > 0 and 2 or 0.5
	lineLayout:runAction(cc.Sequence:create(
		cc.DelayTime:create(waittime),
		cc.CallFunc:create(function ()
			self:showGameEndScore()
		end)
		))

end

function GameViewLayer:showGameEndScore()
	self:refreshGameScore()
	ExternalFun.playSoundEffect("jiangli.wav")
	if self._scene.m_multipleIndex >= g_var(cmd).MultipleThree_Max and self._scene.m_multipleIndex ~= (g_var(cmd).MultipleThree_Max + g_var(cmd).MultipleNine_Max) then

		self:show9lian()
	else
		self:show3lian();
	end
end

--三连
function GameViewLayer:show3lian()
	local pbg = display.newSprite("#anim_3lian_bg_16.png")
	if  not pbg then
		print("show3lian return")
		return
	end
	ExternalFun.playSoundEffect("GoldMove.wav")
	--结束层
	local pEndLayout = ccui.Layout:create()
	pEndLayout:setTag(TAG_ENUM.Tag_End_Layout)
	pEndLayout:setContentSize(cc.size(550, 400))
	pEndLayout:setAnchorPoint(cc.p(0.5, 0.5))
	pEndLayout:setPosition(cc.p(appdf.WIDTH/2, appdf.HEIGHT/2))
	self._csbNode:addChild(pEndLayout,TAG_ZORDER.Zorders_End_Layout)
	
	pbg:setPosition(cc.p(pEndLayout:getContentSize().width/2,pEndLayout:getContentSize().width/2-70))
	pbg:runAction(cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.Anim3LianBG)))
	pEndLayout:addChild(pbg,2)
	--结束层
	pEndLayout:runAction(cc.Sequence:create(cc.DelayTime:create(0.15), cc.CallFunc:create(function()
			local pstar = display.newSprite("#anim_3lian_star_14.png")
			pstar:runAction(cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.Anim3LianStar)))
			pstar:setPosition(cc.p(pbg:getPositionX(), pbg:getPositionY()-200))
			pEndLayout:addChild(pstar,1)

			local plight = cc.Sprite:create("anim/im_3lian_light.png");
			plight:setPosition(cc.p(pEndLayout:getContentSize().width/2,pEndLayout:getContentSize().height/2));
			pEndLayout:addChild(plight);
			plight:setScale(0.5);
			plight:runAction(cc.ScaleTo:create(0.13, 1));
			plight:runAction(cc.RepeatForever:create(cc.RotateBy:create(4, 90)));
			--显示三连图片类型
			local elementtype = {0, 0, 0};
			local typeindex = self._scene.m_multipleIndex;
			if (self._scene.m_multipleIndex == 7) then
				typeindex = g_var(cmd).MultipleThree_II;
			elseif(self._scene.m_multipleIndex < 7) then
				typeindex = self._scene.m_multipleIndex;
			else
				typeindex = self._scene.m_multipleIndex-1;
			end
			for index=1,3 do
				elementtype[index] = typeindex;
			end
			if self._scene.m_multipleIndex == g_var(cmd).MultipleThree_II then
				elementtype[1] = 8;
				elementtype[2] = 8;
				elementtype[3] = 11;
			elseif self._scene.m_multipleIndex == g_var(cmd).MultipleThree_I then
				elementtype[1] = 8;
				elementtype[2] = 11;
				elementtype[3] = 11;
			end
			--元素
			local pelementLayout = ccui.Layout:create();
			pelementLayout:setContentSize(cc.size(225, 80));
			pelementLayout:setAnchorPoint(cc.p(0.5, 0.5));
			pelementLayout:setPosition(cc.p(266, 250));
			pEndLayout:addChild(pelementLayout,2);
			pelementLayout:setScale(0.5);
			pelementLayout:runAction(cc.ScaleTo:create(0.13, 1));

			local elementscale = {0.8, 1, 0.8};
			local elementposx = {10, 125, 240};
			for index=1,3 do
				local pelement = cc.Sprite:create(string.format("game/im_element_%d.png", elementtype[index]));
				pelement:setScale(elementscale[index]);
				pelement:setPosition(cc.p(elementposx[index], 90));
				pelementLayout:addChild(pelement,index%3);
			end
		end)))
	
	--游戏赢取分数
	local pscoreString = ExternalFun.formatScore(self._scene.m_winScore+self._scene.m_taskScore);

	local pscoreLayout = ccui.Layout:create();
	local scoreLength = 57*string.len(tostring(pscoreString));

	pscoreLayout:setContentSize(cc.size(scoreLength, 71));
	pscoreLayout:setAnchorPoint(cc.p(0.5, 0.5));
	pscoreLayout:setPosition(cc.p(pbg:getContentSize().width/2-25, 190));
	pbg:addChild(pscoreLayout);
	local targetscale = 1;
	if scoreLength >280 then
		targetscale = 280/scoreLength;
	end
	pscoreLayout:setScale(0.3);
	pscoreLayout:runAction(cc.ScaleTo:create(0.15, targetscale));
	
	local numstring = math.ceil(string.len(tostring(pscoreString)));
	local numwidth = {57, 57, 47, 40};
	local numheight = {67, 71, 62, 49};
	print("numstring,pscoreString",numstring,pscoreString)
	for pos=1, numstring do
		for index=1,4 do
			local psting = string.sub(pscoreString,pos,pos)
			-- print(string.format("anim/num_end_score_%d.png",index))
			local plabel = cc.LabelAtlas:create(psting,string.format("anim/num_end_score_%d.png",index),numwidth[index],numheight[index],string.byte("0"))
				:move((pos)*60, 25)
				:setAnchorPoint(cc.p(0.5,0.5))
				:addTo(pscoreLayout)
				if index == 4 then
					plabel:setPosition(cc.p((pos)*60, 27));
				end
		end
	end
	--任务奖励分数
	if self._scene.m_taskScore ~= 0 then
		local ptaskString = string.format("%d", self._scene.m_taskScore);
		local ptaskLayout = ccui.Layout:create();
		local tasklength = 32*string.len(ptaskString) + 145;
		ptaskLayout:setContentSize(cc.size(tasklength, 50));
		ptaskLayout:setAnchorPoint(cc.p(0.5, 0.5));
		ptaskLayout:setPosition(cc.p(pbg:getContentSize().width/2, 104));
		pbg:addChild(ptaskLayout);
		local taskScale = 1;
		if (tasklength >280) then
			taskScale = 280/tasklength;
		end
		ptaskLayout:setScale(0.3);
		ptaskLayout:runAction(cc.ScaleTo:create(0.15, taskScale));
		
		local ptaskTxt = ccui.ImageView:create("game/txt_task.png");
		ptaskTxt:setPosition(cc.p(72, 25));
		ptaskLayout:addChild(ptaskTxt);
		
		local plabel = cc.LabelAtlas:create(ptaskString,"game/num_task.png",32,46,string.byte("0"))
			:move(150, 25)
			:setAnchorPoint(cc.p(0,0.5))
			:addTo(ptaskLayout)
	end
	
	--金币动画
	local pGoldAction = cc.CallFunc:create(function ()
			-- local pgold0 = GameGoldMove::create("anim_gold_9.png");
			-- pgold0->initData(Vec2(pEndLayout->getContentSize().width/2, pEndLayout->getContentSize().height/2));
			-- pEndLayout->addChild(pgold0);
			
			-- local pgold1 = GameGoldMove::create("anim_gold_9.png");
			-- pgold1->initData(Vec2(pEndLayout->getContentSize().width/2, pEndLayout->getContentSize().height/2));
			-- pEndLayout->addChild(pgold1, 2);
		end);
	--pEndLayout:runAction(cc.Repeat::create(Sequence::createWithTwoActions(pgoldaction, DelayTime::create(0.024f)), 35));
	
	--5秒后移除
	pEndLayout:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function (  )
		pEndLayout:removeFromParent();
	end)));
end
--九连
function GameViewLayer:show9lian( )
	local pbg = display.newSprite("#anim_9lian_bg_11.png")
	if  not pbg then
		print("show9lian return")
		return
	end

	ExternalFun.playSoundEffect("GoldBigMove.mp3")
	--结束层
	local pEndLayout = ccui.Layout:create()
	pEndLayout:setTag(TAG_ENUM.Tag_End_Layout)
	pEndLayout:setContentSize(cc.size(550, 400))
	pEndLayout:setAnchorPoint(cc.p(0.5, 0.5))
	pEndLayout:setPosition(cc.p(appdf.WIDTH/2, appdf.HEIGHT/2))
	self._csbNode:addChild(pEndLayout,TAG_ZORDER.Zorders_End_Layout)
	
	pbg:setPosition(cc.p(pEndLayout:getContentSize().width/2,pEndLayout:getContentSize().width/2))
	pbg:runAction(cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.Anim9LianBG)))
	pbg:runAction(cc.Sequence:create(
		cc.DelayTime:create(0.75),
		cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.Anim9LianBGChange))
		))
	pEndLayout:addChild(pbg,1)

	--元素
	local pelement = cc.Sprite:create(string.format("game/im_element_%d.png", self._scene.m_multipleIndex-g_var(cmd).MultipleThree_Max));
	pelement:setPosition(cc.p(pbg:getContentSize().width/2, pbg:getContentSize().height/2));
	pbg:addChild(pelement);
	pbg:setScale(0.3);
	pbg:runAction(cc.ScaleTo:create(0.5, 1));

	--背景转动光
	local plight = cc.Sprite:create("anim/im_9lian_light.png");
	plight:setPosition(cc.p(pEndLayout:getContentSize().width/2,pEndLayout:getContentSize().height/2));
	pEndLayout:addChild(plight);
	plight:setScale(0.5);
	plight:runAction(cc.ScaleTo:create(0.13, 1));
	plight:runAction(cc.RepeatForever:create(cc.RotateBy:create(4, 90)));

	local bgIndex = math.ceil(math.random(2)%2)
	local pscoreBg = cc.Sprite:create(string.format("anim/im_9lian_bg_%d.png", bgIndex))
	pscoreBg:setAnchorPoint(0.5,1)
	pscoreBg:setPosition(cc.p(pEndLayout:getContentSize().width/2, pscoreBg:getContentSize().height))
	pscoreBg:setScale(0.3)
	pEndLayout:addChild(pscoreBg,1)
	pscoreBg:runAction(cc.ScaleTo:create(0.5, 1));

	pscoreBg:runAction(cc.Sequence:create(
		cc.DelayTime:create(0.72), 
		cc.CallFunc:create(function()
			local ptxtLight = display.newSprite("#anim_9lian_light_4.png")
			ptxtLight:runAction(cc.Sequence:create(
				cc.Animate:create(cc.AnimationCache:getInstance():getAnimation(self.Anim3LianStar)),
				cc.CallFunc:create(function (  )
					ptxtLight:removeFromParent()
				end)))
			pscoreBg:addChild(ptxtLight);
			ptxtLight:setAnchorPoint(cc.p(0.5, 1));
			ptxtLight:setPosition(cc.p(pscoreBg:getContentSize().width/2, pscoreBg:getContentSize().height-100));
		end)))

	--游戏赢取分数
	local pscoreString = ExternalFun.formatScore(self._scene.m_winScore+self._scene.m_taskScore);
	local pscoreLabel = cc.LabelAtlas:create(pscoreString,"anim/num_9lian_score.png",37,50,string.byte("0"))
		:setAnchorPoint(cc.p(0.5,1))
		:addTo(pscoreBg)
	pscoreLabel:setPositionX(pscoreBg:getContentSize().width/2)
	pscoreLabel:setPositionY(bgIndex >0 and 90 or 105)

	--任务奖励分数
	if self._scene.m_taskScore ~= 0 then
		local ptaskString = string.format("%d", self._scene.m_taskScore);
		local ptaskLayout = ccui.Layout:create();
		local tasklength = 32*string.len(ptaskString) + 145;
		ptaskLayout:setContentSize(cc.size(tasklength, 50));
		ptaskLayout:setAnchorPoint(cc.p(0.5, 0.5));
		ptaskLayout:setPosition(cc.p(pbg:getContentSize().width/2, 104));
		pbg:addChild(ptaskLayout,2);
		local taskScale = 1;
		if (tasklength >280) then
			taskScale = 280/tasklength;
		end
		ptaskLayout:setScale(0.3);
		ptaskLayout:runAction(cc.ScaleTo:create(0.15, taskScale));
		
		local ptaskTxt = ccui.ImageView:create("game/txt_task.png");
		ptaskTxt:setPosition(cc.p(72, 25));
		ptaskLayout:addChild(ptaskTxt);
		
		local plabel = cc.LabelAtlas:create(ptaskString,"game/num_task.png",32,46,string.byte("0"))
			:move(150, 25)
			:setAnchorPoint(cc.p(0,0.5))
			:addTo(ptaskLayout)
	end
	
	--金币动画
	local pGoldAction = cc.CallFunc:create(function ()
			-- local pgold0 = GameGoldMove::create("anim_gold_9.png");
			-- pgold0->initData(Vec2(pEndLayout->getContentSize().width/2, pEndLayout->getContentSize().height/2));
			-- pEndLayout->addChild(pgold0);
			
			-- local pgold1 = GameGoldMove::create("anim_gold_9.png");
			-- pgold1->initData(Vec2(pEndLayout->getContentSize().width/2, pEndLayout->getContentSize().height/2));
			-- pEndLayout->addChild(pgold1, 2);
		end);
	--pEndLayout:runAction(cc.Repeat::create(Sequence::createWithTwoActions(pgoldaction, DelayTime::create(0.024f)), 35));
	
	--5秒后移除
	pEndLayout:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function (  )
		pEndLayout:removeFromParent();
	end)));
end

--改变开始按钮
function GameViewLayer:changeStartButton(isStart)
	local seq = cc.Sequence:create(
		cc.DelayTime:create(0.1),
		cc.CallFunc:create(function (  )
			self.m_btnStart:setVisible(not isStart)
			self.m_btnStop:setVisible(isStart)
		end))
	--self:stopAllActions()
	self:runAction(seq)
end

function GameViewLayer:getGameData(  )
	return self._scene.m_gamedata
end

----------------------

----------------------
--银行节点
function GameViewLayer:createBankLayer()
	self.m_bankLayer = ccui.Layout:create()

	ExternalFun.registerTouchEvent(self.m_bankLayer, true)
    self.m_bankLayer:setTouchEnabled(true)
    self.m_bankLayer:setSwallowTouches(true)

	local colorLayer = cc.LayerColor:create(cc.c4b(0,0,0,100))
	self.m_bankLayer:addChild(colorLayer)
	self._csbNode:addChild(self.m_bankLayer,TAG_ZORDER.Zorders_Bank_Layer)
	self.m_bankLayer:setTag(TAG_ENUM.Tag_BANK_LAYER)

	--加载csb资源
	local csbNode = ExternalFun.loadCSB("bank/BankLayer.csb", self.m_bankLayer)
	csbNode:setPosition(667,375)
	local sp_bg = csbNode:getChildByName("Sprite_Bg")

	------
	--按钮事件
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end 
	--关闭按钮
	local btn = csbNode:getChildByName("Button_close")
	btn:setTag(TAG_ENUM.BT_CLOSEBANK)
	btn:addTouchEventListener(btnEvent)

	--取款按钮
	btn = csbNode:getChildByName("Button_out")
	btn:setTag(TAG_ENUM.BT_TAKESCORE)
	btn:addTouchEventListener(btnEvent)

	--取款按钮
	btn = csbNode:getChildByName("Button_save")
	btn:setTag(TAG_ENUM.BT_SAVESCORE)
	btn:addTouchEventListener(btnEvent)
	------

	------
	--编辑框
	--取款金额
	local tmp = csbNode:getChildByName("Sprite_outScoreText")
	local editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sprite_frame1.png")
		:setPosition(tmp:getPosition())
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入取款金额")
		:setFontColor(cc.c3b(255,255,255))
		:setPlaceholderFontColor(cc.c3b(255,255,255))
	csbNode:addChild(editbox)
	self.m_bankLayer.m_editNumber = editbox
	tmp:removeFromParent()

	--取款密码
	tmp = csbNode:getChildByName("Sprite_bankPWText")
	editbox = ccui.EditBox:create(tmp:getContentSize(),"bank/sprite_frame1.png")
		:setPosition(tmp:getPosition())
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入取款密码")
		:setFontColor(cc.c3b(255,255,255))
		:setPlaceholderFontColor(cc.c3b(255,255,255))
	csbNode:addChild(editbox)
	self.m_bankLayer.m_editPasswd = editbox
	tmp:removeFromParent()
	------

	--当前游戏币
	self.m_bankLayer.m_textCurrent = csbNode:getChildByName("Text_curScore")

	--银行游戏币
	self.m_bankLayer.m_textBank = csbNode:getChildByName("Text_bankScore")

	--取款费率
	self.m_bankLayer.m_textTips = csbNode:getChildByName("Text_tip")
	self:getParentNode():sendRequestBankInfo()
end

--取款
function GameViewLayer:onTakeScore()
	--参数判断
	local szScore = string.gsub(self.m_bankLayer.m_editNumber:getText(),"([^0-9])","")
	local szPass = self.m_bankLayer.m_editPasswd:getText()

	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end

	local lOperateScore = tonumber(szScore)
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",1)
		return
	end

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",1)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",1)
		return
	end

	self:showPopWait()  
	self:getParentNode():sendTakeScore(szScore,szPass)
end

function GameViewLayer:onSaveScore(  )

	--参数判断
	local szScore = string.gsub(self.m_bankLayer.m_editNumber:getText(),"([^0-9])","")
	local szPass = self.m_bankLayer.m_editPasswd:getText()
	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end
	
	local lOperateScore = tonumber(szScore)
	
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

    if lOperateScore > GlobalUserItem.tabAccountInfo.lUserScore then
        showToast(self,"您所携带游戏币的数目余额不足,请重新输入游戏币数量!",2)
        return
    end

	self:showPopWait()

	self._scene:onSaveScore(lOperateScore)
end

--银行操作成功
function GameViewLayer:onBankSuccess( )
	local bank_success = self:getParentNode().bank_success
	if nil == bank_success then
		return
	end
	GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore
	GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure

	if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
		self:refreshScore()
	end
	showToast(self, bank_success.szDescribrString, 2)
end

--银行操作失败
function GameViewLayer:onBankFailure( )
	local bank_fail = self:getParentNode().bank_fail
	if nil == bank_fail then
		return
	end
	print("银行操作失败",bank_fail.szDescribeString)
	showToast(self, bank_fail.szDescribeString, 2)
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
	bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
	if nil ~= self.m_bankLayer then
		local str = "温馨提示:取款将扣除" .. bankinfo.wRevenueTake .. "%的手续费"
		self.m_bankLayer.m_textTips:setString(str)
		self:refreshScore(bankinfo)
	end
end

--刷新游戏币
function GameViewLayer:refreshScore( userinfo )
	local userinfo = userinfo and userinfo or GlobalUserItem.tabAccountInfo
	-- dump(userinfo, "--------------刷新游戏币----------------",6)
	--携带游戏币
	local str = ExternalFun.numberThousands(userinfo.lUserScore)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	str = string.gsub(str,",","/")
	self.m_bankLayer.m_textCurrent:setString(str)
	--银行存款

	str = ExternalFun.numberThousands(userinfo.lUserInsure)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	str = string.gsub(str,",","/")
	self.m_bankLayer.m_textBank:setString(str)
	--self.m_bankLayer.m_textBank:setString(ExternalFun.numberThousands(userinfo.tabAccountInfo.lUserInsure))
	self._scene.m_lUserCoins = userinfo.lUserScore
	self:refreshGameScore()

	self.m_bankLayer.m_editNumber:setText("")
	self.m_bankLayer.m_editPasswd:setText("")

end


function GameViewLayer:refreshGameScore(  )
	--金币
	local str = ExternalFun.numberThousands(self._scene.m_lUserCoins)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	str = string.gsub(str,",","/")
	self.m_textUserCoin:setString(str)

	-- --豆子
	-- str = ExternalFun.numberThousands(self._scene.m_lUserBeans)
	-- if string.len(str) > 19 then
	-- 	str = string.sub(str, 1, 19)
	-- end
	-- str = string.gsub(str,",","/")
	-- self.m_textUserBean:setString(str)

end


--公告
function GameViewLayer:addGongGao(_cmd_table)
	table.insert(self._scene.tGonggao,{cmd_table =_cmd_table})

	if not self.isGongGao then
		self:showGongGao(clone(self._scene.tGonggao[1].cmd_table))
	end
end

function GameViewLayer:showGongGao(cmd_table,ggtype)
  
    local  layout1 = self._csbNode:getChildByName("im_horn_frame")
    ---layout:setSize()
    --local  txt = layout1:getChildByName("Text_1")
    --txt:setVisible(false)

    local layout = ccui.Layout:create()
            layout:setContentSize(cc.size(660, 39))
            layout:setPosition(180,635)
            layout:setClippingEnabled(true)

    self._csbNode:addChild(layout)

    if self.isGongGao then
        return
    end
   
    self.isGongGao = true

    local typeStr = ""
    if cmd_table.nMultipleIndex >= 12 then
    	typeStr = "获得九连全盘大奖"
    else
    	typeStr = "获得三连单线大奖"
    end
    --cmd_table.wPlayName
    local strT = {}
    local colorT = {}

    strT = {cmd_table.szPlayName,typeStr,"奖金 ",cmd_table.nAwardScore}
    colorT = {cc.c3b(255,217,84),cc.c3b(255,217,84),cc.c3b(255,217,84),cc.c3b(255,217,84)}

    local txt1 = cc.Label:createWithSystemFont("恭喜","fonts/round_body.ttf",20)
                        :move(800,20)
                        :setColor(cc.c3b(255,217,84))
                        :setAnchorPoint(cc.p(0,0.5))
                        :addTo(layout)
    local lenth = txt1:getContentSize().width

    for i=1,#strT do
       --print("addGongGao11111,"..cmd_table.nGemIndex)
        --print("addGongGao,i="..i..",#strT = "..#strT..","..ggtype)p
          local txt2 = cc.Label:createWithSystemFont((strT[i]..""),"fonts/round_body.ttf",20)
                        :move(lenth,0)
                        :setColor(colorT[i])
                        :setAnchorPoint(cc.p(0,0))
                        :addTo(txt1)
            lenth = lenth + txt2:getContentSize().width
    end  
    txt1:runAction(cc.Sequence:create(cc.Repeat:create(cc.Sequence:create(cc.MoveBy:create(10,cc.p(-660 - lenth,0)),cc.CallFunc:create(function ()
    table.remove(self._scene.tGonggao,1)

      txt1:setPosition(800,20)
    end)),10),cc.RemoveSelf:create(),cc.CallFunc:create(function ()
      print("addGongGao============>")
      self.isGongGao = false

      if #self._scene.tGonggao >=1 then
      	self:showGongGao(clone(self._scene.tGonggao[1].cmd_table))
      end
    end)))

end

function GameViewLayer:showPopWait( )
	self:getParentNode():showPopWait()
end

function GameViewLayer:dismissPopWait( )
	self:getParentNode():dismissPopWait()
end

function GameViewLayer:addToRootLayer( node , zorder)
	if nil == node then
		return
	end

	self.m_rootLayer:addChild(node)
	node:setLocalZOrder(zorder)
end

return GameViewLayer