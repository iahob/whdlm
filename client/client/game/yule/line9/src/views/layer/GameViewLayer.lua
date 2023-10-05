local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
	return gameViewLayer
end)


local module_pre = "game.yule.line9.src"
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"

local cmd = appdf.req(appdf.GAME_SRC.."yule.line9.src.models.CMD_Game")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local GameRollList = appdf.req(module_pre .. ".views.layer.GameRollList")
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local TrendLayer = appdf.req(module_pre .. ".views.layer.TrendLayer")
local RewardLayer = appdf.req(module_pre .. ".views.layer.RewardLayer")


local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")

GameViewLayer.RES_PATH = device.writablePath.. "game/yule/line9/res/"

local ButtonTag = 
{
	"Tag_Top" ,--顶部
	"Tag_Menu",
	"Tag_BT_Return",--返回按钮
	"Tag_BT_Bank",
    "Tag_BT_LINE",
    "Tag_BT_ADD",
    "Tag_BT_CHECK",
	"Tag_BT_Horn",--喇叭
	"Tag_BT_TREND",
	"Tag_BT_Setting",--游戏设置
	"Tag_BT_BLANK",
	"Tag_BT_MUSIC",
	"Tag_BT_HELP",
	"Tag_BT_Quit",--退出游戏
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

	"Tag_REWARD_LAYER",
	"Tag_SET_LAYER",
	"Tag_BANK_LAYER",
	"Tag_BT_LINE_LOOK",
	"Tag_BT_LINE_LOOK_1",
	"Tag_BT_LINE_LOOK_2",
	"Tag_BT_LINE_LOOK_3",
	"Tag_BT_LINE_LOOK_4",
	"Tag_BT_LINE_LOOK_5",
	"Tag_BT_LINE_LOOK_6",
	"Tag_BT_LINE_LOOK_7",
	"Tag_BT_LINE_LOOK_8",
	"Tag_BT_LINE_LOOK_9",
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

local BANK_BTN_NUM = {0,0,0,0,0}

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

    self.rollListListener = cc.EventListenerCustom:create("EVENT_NEXT",handler(self, self.callBackFun))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.rollListListener,self)
    self.nextNum = 1
end


function GameViewLayer:callBackFun()
    print("GameViewLayer:callBackFun")
    self.nextNum = self.nextNum + 1
    self:onGameStop(false)
end


--增加搜索路径
function GameViewLayer:addPath( )
	--搜索路径
	local gameList = self:getParentNode():getParentNode():getApp()._gameList;
	local gameInfo = {};
	for k,v in pairs(gameList) do 
		if tonumber(v._KindID) == tonumber(cmd.KIND_ID) then
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
--	for index=1,3 do
--		cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("anim/anim_%d.plist",index-1))
--		cc.Director:getInstance():getTextureCache():addImage(string.format("anim/anim_%d.png",index-1));
--	end

--	self:readAnimation("anim_egg_", self.AnimEgg, 16, 0.09);
--	self:readAnimation("anim_egg_effect_", self.AnimEggEffect, 9, 0.045);
--	self:readAnimation("anim_3lian_bg_", self.Anim3LianBG, 16, 0.06);
--	self:readAnimation("anim_3lian_star_", self.Anim3LianStar, 14, 0.06);
--	self:readAnimation("anim_9lian_bg_", self.Anim9LianBG, 11, 0.06);
--	self:readAnimation("anim_9lian_bg_", self.Anim9LianBGChange, 11, 0.2);
--	self:readAnimation("anim_9lian_light_", self.Anim9LianLight, 4, 0.1);
--	self:readAnimation("anim_gold_", self.AnimGold, 9, 0.03);

--    display.loadSpriteFrames(cmd.RES.."ninelineking_game.plist",cmd.RES.."ninelineking_game.png")
--    display.loadSpriteFrames(cmd.RES.."ninelineking_gameline.plist",cmd.RES.."ninelineking_gameline.png")
end

function GameViewLayer:initData(  )

	self.m_gamedata = {}
	self.m_bTaskComplete = {}
	--self.m_plusindex = 1
	self.m_rolllist = {}

	self.isGongGao = false                                          --是否正在播放公告
    self.lineNum = 9
    self.scoreIndex = 1
    self.totalScoreNum = 0
end

function GameViewLayer:onExit()
    cc.Director:getInstance():getEventDispatcher():removeEventListener(self.rollListListener)
    
	--停止定时器
	for i=1,cmd.X_COUNT do
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

--    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile(cmd.RES.."ninelineking_game.plist")
--	cc.Director:getInstance():getTextureCache():removeTextureForKey(cmd.RES.."ninelineking_game.png")
--    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile(cmd.RES.."ninelineking_gameline.plist")
--	cc.Director:getInstance():getTextureCache():removeTextureForKey(cmd.RES.."ninelineking_gameline.png")

	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

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

            if sender:getTag() == TAG_ENUM.Tag_BT_Begin then
                self.longClickCount = 1
                self.autoBegin = false
                self.longClick = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
                if self.longClickCount == 2 then
                    self.autoBegin = true
                    self:onButtonClickedEvent(sender:getTag(), sender)
                end
                self.longClickCount = self.longClickCount + 1
                end, 1, false)
            end
		elseif eventType == ccui.TouchEventType.canceled then
			if self.longClick then
                -- 注销时钟
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.longClick)
                self.longClick = nil
            end
			ExternalFun.dismissTouchFilter()
		elseif eventType == ccui.TouchEventType.ended then
            if self.longClick then
                -- 注销时钟
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self.longClick)
                self.longClick = nil
                ExternalFun.dismissTouchFilter()
                if not self.autoBegin then
                    self:onButtonClickedEvent(sender:getTag(), sender)
                end
                return
            end
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

    self.setLayer = csbNode:getChildByName("im_setting_bg");

    local Button_blank = csbNode:getChildByName("im_setting_bg"):getChildByName("Button_blank");
	Button_blank:setTag(TAG_ENUM.Tag_BT_BLANK);
	Button_blank:addTouchEventListener(btnEvent);

    local Button_music = csbNode:getChildByName("im_setting_bg"):getChildByName("Button_music");
	Button_music:setTag(TAG_ENUM.Tag_BT_MUSIC);
	Button_music:addTouchEventListener(btnEvent);

    local Button_help = csbNode:getChildByName("im_setting_bg"):getChildByName("Button_help");
	Button_help:setTag(TAG_ENUM.Tag_BT_HELP);
	Button_help:addTouchEventListener(btnEvent);

	--退出
	local Button_quit = csbNode:getChildByName("im_setting_bg"):getChildByName("Button_quit");
	Button_quit:setTag(TAG_ENUM.Tag_BT_Quit);
	Button_quit:addTouchEventListener(btnEvent);

    --中奖按钮
	local Button_trend = csbNode:getChildByName("Button_trend");
	Button_trend:setTag(TAG_ENUM.Tag_BT_TREND);
	Button_trend:addTouchEventListener(btnEvent);

	--银行按钮
	local Button_bank = csbNode:getChildByName("Button_bank");
	Button_bank:setTag(TAG_ENUM.Tag_BT_Bank);
	Button_bank:addTouchEventListener(btnEvent);
	-------------------------
	--本局得分
    self.totalScore = csbNode:getChildByName("im_totalwin_bg"):getChildByName("Text_userscore")
    
	--满线
    local Button_check = csbNode:getChildByName("Button_check");
	Button_check:setTag(TAG_ENUM.Tag_BT_CHECK)
	Button_check:addTouchEventListener(btnEvent)
	self.Button_check = Button_check;

    --线
    self.nodeLine = {}
    self.animLine = {}
    for i = 1, cmd.MAX_LINE_COUNT do
        self.nodeLine[i] = csbNode:getChildByName(string.format("Node_Line_%d",i))

        self.animLine[i] = cc.CSLoader:createNode(string.format("./animation/line_%d.csb", i))
        self.animLine[i]:setVisible(false)
        self.nodeLine[i]:addChild(self.animLine[i])
        self.animLine[i]:setPosition(self.nodeLine[i]:getChildByName("Sprite_middle"):getPosition())
    end

    --line btn
    for i = 1, cmd.MAX_LINE_COUNT do
    	local line_btn_left = self.nodeLine[i]:getChildByName("line_btn_left");
		line_btn_left:setTag(TAG_ENUM.Tag_BT_LINE_LOOK + i);
		line_btn_left:addTouchEventListener(btnEvent);

		local line_btn_right = self.nodeLine[i]:getChildByName("line_btn_right");
		line_btn_right:setTag(TAG_ENUM.Tag_BT_LINE_LOOK + i);
		line_btn_right:addTouchEventListener(btnEvent);
    end

    self.buttonBlank = csbNode:getChildByName("Button_blank")
	self.buttonBlank:addTouchEventListener(function() 
        for i = 1, #self.nodeLine do
            self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(false)
            self.animLine[i]:setVisible(false)
        end
        self.buttonBlank:setVisible(false)
    end)

	--线押注
    self.Button_line = csbNode:getChildByName("Button_line")
	self.Button_line:setTag(TAG_ENUM.Tag_BT_LINE)
	self.Button_line:addTouchEventListener(btnEvent)
    self.artLineNum = self.Button_line:getChildByName("Art_line_num")
    self.artLineNum:setString(self.lineNum)

    --本局下注
    self.Button_add = csbNode:getChildByName("Button_add");
	self.Button_add:setTag(TAG_ENUM.Tag_BT_ADD)
	self.Button_add:addTouchEventListener(btnEvent)
    self.artScore = csbNode:getChildByName("Art_score")

    csbNode:getChildByName("Text_name"):setString(self._scene:GetMeUserItem().szNickName)

	-------------------------
	--滚动层
	self.frameBg = csbNode:getChildByName("im_frame_bg")

    --彩池
    self.bonuspool = csbNode:getChildByName("anim_bonuspool")
    local animNode = cc.CSLoader:createNode("./animation/bonuspool.csb")
    csbNode:addChild(animNode)
    animNode:setPosition(self.bonuspool:getPosition())
    local animTimeline = cc.CSLoader:createTimeline("./animation/bonuspool.csb")
    local animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,true)
    self.bonuspool:setVisible(false)
    animNode:runAction(animTimeline)

    --托管提示
    local sprite = csbNode:getChildByName("Button_start"):getChildByName("Sprite_1")
    animNode = cc.CSLoader:createNode("./animation/autotips.csb")
    csbNode:getChildByName("Button_start"):addChild(animNode)
    animNode:setPosition(sprite:getPosition())
    animTimeline = cc.CSLoader:createTimeline("./animation/autotips.csb")
    animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,true)
    sprite:setVisible(false)
    animNode:runAction(animTimeline)

    --托管提示2
    self.autotips2 = cc.CSLoader:createNode("./animation/autotips2.csb")
    self.m_btnStop:addChild(self.autotips2,100)
    self.autotips2:setPosition(175/2,134/2)
    animTimeline = cc.CSLoader:createTimeline("./animation/autotips2.csb")
    animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex,animinfo.endIndex,animinfo.startIndex,true)
    self.autotips2:runAction(animTimeline)


	--初始化滚动层
	self:initRolllist()
	--投币动画
--	self:readtoubianim();

	--初始化玩家信息
	self:initUserInfo(csbNode);

    --初始化趋势界面
    self.trendLayer = TrendLayer:create()
	self.trendLayer.m_parent = self
    self.trendLayer:showLayer(false)
	self._csbNode:addChild(self.trendLayer,TAG_ZORDER.Zorders_Set_Layer)
end

function GameViewLayer:initRolllist( )
	for i=1,cmd.X_COUNT do
		self.m_rolllist[i] = GameRollList:create(self)
		self.m_rolllist[i]:setPosition(cc.p(142 + (i-1) * 210,147))
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
	head:enableInfoPop(true,cc.p(55,55),cc.p(0,0))

	--玩家游戏币
	local nodeCoin = csbNode:getChildByName("im_score_bg")
	self.m_textUserCoin = nodeCoin:getChildByName("Text_userscore")

	--彩金
	self.nodeLotteryScore = csbNode:getChildByName("Art_lotteryscore")
end

--按钮回调
function GameViewLayer:onButtonClickedEvent(tag,ref)
	if tag == TAG_ENUM.Tag_BT_Quit then             --退出
        self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Setting)
		self._scene.m_bIsLeave = true
		self:getParentNode():onQueryExitGame()
		ExternalFun.playClickEffect()
	elseif tag == TAG_ENUM.Tag_BT_Begin  then       --开始游戏
		print("游戏开始")
        if not self.autoBegin then
            self.m_btnStart:loadTextures("game/bt_start_0.png", "game/bt_start_1.png","game/bt_start_3.png", ccui.TextureResType.plistType)
            self.m_btnStop:loadTextures("game/bt_stop_0.png", "game/bt_stop_1.png", "game/bt_stop_0.png", ccui.TextureResType.plistType)
        end
		self:getParentNode():onGameStart()
        ExternalFun.playSoundEffect("StartLogo.wav")
	elseif tag == TAG_ENUM.Tag_BT_Stop  then       --停止游戏
		print("游戏停止")
        if self.autoBegin then
            self.m_btnStart:loadTextures("game/bt_start_0.png", "game/bt_start_1.png","game/bt_start_3.png", ccui.TextureResType.plistType)
            self.m_btnStop:loadTextures("game/bt_stop_0.png", "game/bt_stop_1.png", "game/bt_stop_0.png", ccui.TextureResType.plistType)
            self.autotips2:setVisible(false)
            self.autoBegin = false
            -- self.Button_line:setEnabled(true)
            -- self.Button_add:setEnabled(true)
            -- self.Button_check:setEnabled(true)
            self:setButtonEnable(true);
            self:initButton()
--            self._scene.m_gamestatus = cmd.GAME_SCENE_FREE
            if self._scene.m_scheduleUpdate ~= nil then
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._scene.m_scheduleUpdate)
                self._scene.m_scheduleUpdate = nil
            end
--            self._scene.m_waittime = 0
            if self._scene.m_gamestatus == cmd.GAME_SCENE_FREE then
                self:changeStartButton(false)
            end
            self._scene:sendMessageGameStop()
        else
            self.m_btnStart:loadTextures("game/bt_start_0.png", "game/bt_start_1.png","game/bt_start_3.png", ccui.TextureResType.plistType)
            self.m_btnStop:loadTextures("game/bt_stop_0.png", "game/bt_stop_1.png", "game/bt_stop_0.png", ccui.TextureResType.plistType)
		    self:getParentNode():onGameStop()
		    ExternalFun.playClickEffect()
        end
	elseif tag == TAG_ENUM.Tag_BT_Setting  then     --设置
        if self.setLayer:isVisible() then
            self.setLayer:setVisible(false)
        else
            self.setLayer:setVisible(true)
            ExternalFun.playSoundEffect("OpenWidnow.wav")
        end
    elseif tag == TAG_ENUM.Tag_BT_BLANK then
        self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Setting)
    elseif tag == TAG_ENUM.Tag_BT_MUSIC then
        self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Setting)
        self:onSettingLayer()
    elseif tag == TAG_ENUM.Tag_BT_HELP then
        self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Setting)
        self:onHelpLayer()
	elseif tag == TAG_ENUM.Tag_BT_Bank then
		--银行未开通
		if 0 == GlobalUserItem.cbInsureEnabled then
			showToast(self,"初次使用，请先开通银行！",1)
			return
		end
		-- if nil == self._scene.m_gamestatus or cmd.GAME_SCENE_FREE ~= self._scene.m_gamestatus then
		-- 	print("m_gamestatus",self._scene.m_gamestatus)
		-- 	showToast(self,"游戏过程中不能进行银行操作",1)
		-- 	return
		-- end

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
		self:getParentNode():sendRequestBankInfo()
		self:refreshScore()
	elseif tag == TAG_ENUM.Tag_BT_Plus_Score  then        --加注
--		self:getParentNode():onAddScore()
--		ExternalFun.playSoundEffect("shangfen.mp3")
	elseif tag == TAG_ENUM.Tag_BT_Minus_Score  then        --减注
--		self._scene:onSubScore()
--		ExternalFun.playSoundEffect("shangfen1.mp3")
	elseif tag == TAG_ENUM.Tag_BT_TREND  then
        self:onTrendLayer()
		ExternalFun.playSoundEffect("OpenWidnow.wav")
	elseif tag == TAG_ENUM.BT_CLOSEBANK then
		if nil ~= self.m_bankLayer then
			self.m_bankLayer:setVisible(false)
		end
	elseif tag == TAG_ENUM.BT_TAKESCORE then
		self:onTakeScore()
	elseif tag == TAG_ENUM.BT_SAVESCORE then
		self:onSaveScore()
    elseif tag == TAG_ENUM.Tag_BT_LINE then
        self:onAddLine()
        ExternalFun.playSoundEffect("ButtonDown.wav")
    elseif tag == TAG_ENUM.Tag_BT_ADD then
        self:onAddScore()
        ExternalFun.playSoundEffect("shangfen.mp3")
    elseif tag == TAG_ENUM.Tag_BT_CHECK then
        self:onLineCheck()
    elseif tag > TAG_ENUM.Tag_BT_LINE_LOOK and tag <= TAG_ENUM.Tag_BT_LINE_LOOK+9 then
        self:onLineLookByIndex(tag - TAG_ENUM.Tag_BT_LINE_LOOK);
	else
		showToast(self,"功能尚未开放！",1)
	end
end

--满线
function GameViewLayer:onLineCheck()
    for i = 1, #self.nodeLine do
        self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(true)
    end
    self.buttonBlank:setVisible(true)
end

--固定线束显示
function GameViewLayer:onLineLookByIndex(index)
	local line = self.nodeLine[index]:getChildByName("Sprite_middle");
	line:setVisible(true);

	--显示2s隐藏
	line:runAction(cc.Sequence:create(
        cc.DelayTime:create(2.0),
        cc.CallFunc:create(function() 
            line:setVisible(false);
        end)
    ))
end

--下注 
function GameViewLayer:onAddScore()
    if self.scoreIndex >= self._scene.m_gamedata.cbBetCount then
        self.scoreIndex = 1
    else
        self.scoreIndex = self.scoreIndex + 1
    end
    self:updatePlusScore()
end

--赠送次数
function GameViewLayer:onGiveAwayCount(nGiveAwayCount)
    self._scene.nGiveAwayCount = nGiveAwayCount
    if nGiveAwayCount > 0 then
        self._csbNode:getChildByName("im_totalwin_bg"):getChildByName("Sprite_bar"):getChildByName("Text_1"):setString(string.format("x %d", nGiveAwayCount))
        self._csbNode:getChildByName("im_totalwin_bg"):getChildByName("Sprite_bar"):setVisible(true)
    else
        self._csbNode:getChildByName("im_totalwin_bg"):getChildByName("Sprite_bar"):setVisible(false)
    end
end

--更新下注信息  
function GameViewLayer:updatePlusScore()
    if self._scene.m_gamedata.lCellScore and self._scene.m_gamedata.lBetScore[1] and #self._scene.m_gamedata.lBetScore[1] > 0 then
        local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
        if score >= 10000 then
            if score >= 1000000 then
                score = math.floor(score / 10000)
            else
                local a,b = math.modf(score / 10000)
                if b < 0.1 then
                    score = a
                else
                    score = string.format("%.1f",score / 10000)
                end
            end
             
            self.artScore:getChildByName("Sprite_wan"):setPosition(string.len(score) * 24 + 10, 1)
            self.artScore:setPosition(930, 33)
            self.artScore:getChildByName("Sprite_wan"):setVisible(true)
        else
            self.artScore:setPosition(950, 33)
            self.artScore:getChildByName("Sprite_wan"):setVisible(false)
        end
        self.artScore:setString(score)
    end
end

--线押注
function GameViewLayer:onAddLine()
    self.lineNum = tonumber(self.artLineNum:getString())
    if self.lineNum >= cmd.MAX_LINE_COUNT then
        self.lineNum = 1
    else
        self.lineNum = self.lineNum + 1
    end
    self.artLineNum:setString(self.lineNum)
end

--停止转动
function GameViewLayer:onGameStop( bStopNew )
	print(".bStopNew",bStopNew)
	if bStopNew then
		for i=1,cmd.X_COUNT do
			self.m_rolllist[i]:setStatus(2)
		end
		self._scene.m_gamestatus = cmd.GAME_SCENE_FREE
		self:gameEndScoreDeal()
	else
        self._scene.slow = true
        if self.nextNum == 1 then
            local pAction = cc.Sequence:create(
            cc.DelayTime:create(0.3),
			cc.CallFunc:create(function (  )
				print("self._scene.m_gamestatus ",self._scene.m_gamestatus)
				if self._scene.m_gamestatus == cmd.GAME_SCENE_SLOW then
					self.m_rolllist[1]:setStatus(1)
				end
			end))
		    self:runAction(pAction)
        elseif self.nextNum == 2 then
            if self._scene.m_gamestatus == cmd.GAME_SCENE_SLOW then
				self.m_rolllist[2]:setStatus(1)
			end
        elseif self.nextNum == 3 then
            if self._scene.m_gamestatus == cmd.GAME_SCENE_SLOW then
				self.m_rolllist[3]:setStatus(1)
			end
        elseif self.nextNum == 4 then
            if self._scene.m_gamestatus == cmd.GAME_SCENE_SLOW then
				self.m_rolllist[4]:setStatus(1)
			end
        elseif self.nextNum == 5 then
            if self._scene.m_gamestatus == cmd.GAME_SCENE_SLOW then
			    self.m_rolllist[5]:setStatus(1,true)
                self._scene:sendMessageGameStop()
			end
        end
	end
end

--声音设置界面
function GameViewLayer:onSettingLayer(  )
	local mgr = self._scene._scene:getApp():getVersionMgr()
	local verstr = mgr:getResVersion(cmd.KIND_ID) or "0"
	verstr = "游戏版本:" .. appdf.BASE_C_VERSION .. "." .. verstr
	local set = SettingLayer:create(verstr)
	set.m_parent = self
	self._csbNode:addChild(set,TAG_ZORDER.Zorders_Set_Layer)
end

--规则界面
function GameViewLayer:onHelpLayer(  )
	local help = HelpLayer:create()
	help.m_parent = self
	self._csbNode:addChild(help,TAG_ZORDER.Zorders_Set_Layer)
end

--中奖界面
function GameViewLayer:onTrendLayer(  )
    self.trendLayer:showLayer(true)
end

--更新自己的分数
function GameViewLayer:updateSelfScore()
end

--更新奖池信息
function GameViewLayer:updatePoolScore()
    local score = self._scene.m_gamedata.lLotteryScore
    if score then
--        if score >= 10000 then
--            if score >= 1000000 then
--                score = math.floor(score / 10000)
--            else
--                local a,b = math.modf(score / 10000)
--                if b < 0.1 then
--                    score = a
--                else
--                    score = string.format("%.1f",score / 10000)
--                end
--            end
--            self.nodeLotteryScore:getChildByName("Sprite_wan"):setPosition(string.len(score) * 24 + 10, 1)
--            self.nodeLotteryScore:getChildByName("Sprite_wan"):setVisible(true)
--        else
--            self.nodeLotteryScore:getChildByName("Sprite_wan"):setVisible(false)
--        end
        self.nodeLotteryScore:setString(score)
    end
end

--更新赢钱
function GameViewLayer:updateWinScore()
end

--更新托管纹理
function GameViewLayer:updateAutoBtn(isAuto)
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

function GameViewLayer:gameBeginClearShow()
    self.nextNum = 1
    for i = 1, cmd.MAX_LINE_COUNT do
        self.nodeLine[i]:getChildByName("Sprite_left"):setSpriteFrame(string.format("game/im_%d_0.png", i))
        self.nodeLine[i]:getChildByName("Sprite_right"):setSpriteFrame(string.format("game/im_%d_0.png", i))
        self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(false)
        self.animLine[i]:setVisible(false)
    end
    if self.rewardLayer then
        self.rewardLayer:showLayer(false)
    end
    self._scene.m_winScore = 0;
    self._scene.m_taskscore = 0;
    self:updateWinScore();
end

--游戏总分
function GameViewLayer:updateTotalScore()
    local node = self.totalScore
    self.totalScoreNum = self.totalScoreNum + self._scene.m_winScore
    print("GameViewLayer:updateTotalScore",self.totalScoreNum)
    local score = self.totalScoreNum
--    if score >= 10000 then
--        if score >= 1000000 then
--            score = math.floor(score / 10000)
--        else
--            local a,b = math.modf(score / 10000)
--            if b < 0.1 then
--                score = a
--            else
--                score = string.format("%.1f",score / 10000)
--            end
--        end
--        node:getChildByName("Sprite_wan"):setPosition(string.len(score) * 19, 1)
--        node:getChildByName("Sprite_wan"):setVisible(true)
--    else
--        node:getChildByName("Sprite_wan"):setVisible(false)
--    end
    node:setString(score)
end

--游戏结算分数处理
function GameViewLayer:gameEndScoreDeal()
    if self._scene.m_lineMultiple then
        self:onCalculate()
        self:updateTotalScore()
        self:updateSelfScore()
        self:updateWinScore()
        self:refreshGameScore()
        self._scene.m_gamestatus = cmd.GAME_SCENE_FREE
        print("self._scene.m_winScore", self._scene.m_winScore)
        if self._scene.nGiveAwayCount > 0 then
            self:initButton()
            self:onGiveAwayCount(self._scene.nGiveAwayCount)
        end
        if self._scene.m_winScore > 0 then
            ExternalFun.playSoundEffect("GameWin.wav")
        else
            ExternalFun.playSoundEffect("GameFail.wav")
        end
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(function() 
            end),
            cc.DelayTime:create(1),
            cc.CallFunc:create(function() 
                if self._scene.beginLock then
                    if table.nums(self.nodeCountry) > 0 then
                        self:showRewardLayer()
                    end
                end
            end),
            cc.DelayTime:create(2),
            cc.CallFunc:create(function()
                if self.autoBegin then
                    self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Begin)
                end
                for i = 1, cmd.MAX_LINE_COUNT do
        			self.nodeLine[i]:getChildByName("Sprite_left"):setSpriteFrame(string.format("game/im_%d_0.png", i))
        			self.nodeLine[i]:getChildByName("Sprite_right"):setSpriteFrame(string.format("game/im_%d_0.png", i))
        			self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(false)
        			self.animLine[i]:setVisible(false)
    			end
            end)
        ))
    end
    if self._scene.m_scheduleUpdate ~= nil then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._scene.m_scheduleUpdate)
        self._scene.m_scheduleUpdate = nil
    end
end

function GameViewLayer:onCalculate()
    self.nodeCountry = {}
    self.nodeCountryNum = {[0] = {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}
    local lineInfo = {}
    local fun = function(t,i) 
        local temp = clone(t)
--        table.sort(temp,function(a,b) return a > b end)
--        dump(temp,"temp")
        if temp[1] == temp[2] and temp[1] == temp[3] then
            self.nodeCountry[i] = temp[1]
        elseif temp[2] == temp[3] and temp[2] == temp[4] then
            self.nodeCountry[i] = temp[2]
        elseif temp[3] == temp[4] and temp[3] == temp[5] then
            self.nodeCountry[i] = temp[3]
        end
    end
    for i = 1, cmd.MAX_LINE_COUNT do
        if self._scene.m_lineMultiple[i] > 0 then
            self:showGameLine(i)
            if i == 1 then
                local temp = clone(self._scene.cbItemInfo[2])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 2 then
                local temp = clone(self._scene.cbItemInfo[1])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 3 then
                local temp = clone(self._scene.cbItemInfo[3])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 4 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[1][1])
                table.insert(temp,self._scene.cbItemInfo[2][2])
                table.insert(temp,self._scene.cbItemInfo[3][3])
                table.insert(temp,self._scene.cbItemInfo[2][4])
                table.insert(temp,self._scene.cbItemInfo[1][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 5 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[3][1])
                table.insert(temp,self._scene.cbItemInfo[2][2])
                table.insert(temp,self._scene.cbItemInfo[1][3])
                table.insert(temp,self._scene.cbItemInfo[2][4])
                table.insert(temp,self._scene.cbItemInfo[3][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 6 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[2][1])
                table.insert(temp,self._scene.cbItemInfo[1][2])
                table.insert(temp,self._scene.cbItemInfo[1][3])
                table.insert(temp,self._scene.cbItemInfo[1][4])
                table.insert(temp,self._scene.cbItemInfo[2][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 7 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[2][1])
                table.insert(temp,self._scene.cbItemInfo[3][2])
                table.insert(temp,self._scene.cbItemInfo[3][3])
                table.insert(temp,self._scene.cbItemInfo[3][4])
                table.insert(temp,self._scene.cbItemInfo[2][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 8 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[1][1])
                table.insert(temp,self._scene.cbItemInfo[1][2])
                table.insert(temp,self._scene.cbItemInfo[2][3])
                table.insert(temp,self._scene.cbItemInfo[3][4])
                table.insert(temp,self._scene.cbItemInfo[3][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            elseif i == 9 then
                local temp = {}
                table.insert(temp,self._scene.cbItemInfo[3][1])
                table.insert(temp,self._scene.cbItemInfo[3][2])
                table.insert(temp,self._scene.cbItemInfo[2][3])
                table.insert(temp,self._scene.cbItemInfo[1][4])
                table.insert(temp,self._scene.cbItemInfo[1][5])
                lineInfo[i] = clone(temp)
                fun(temp,i)
            end
        end
    end
--    dump(self.nodeCountry,"self.nodeCountry")
--    dump(lineInfo,"lineInfo")
	local flag = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}
    for i = 1, cmd.MAX_LINE_COUNT do
        if self.nodeCountry[i] then
        	local flagTemp = {}
        	if i == 1 then
        		for j = 1 , 5 do
        			flagTemp[j] = 2
        		end
        	elseif i == 2 then
        		for j = 1 , 5 do
        			flagTemp[j] = 1
        		end
        	elseif i == 3 then
        		for j = 1 , 5 do
        			flagTemp[j] = 3
        		end
        	elseif i == 4 then
                flagTemp[1] = 1
                flagTemp[2] = 2
                flagTemp[3] = 3
                flagTemp[4] = 2
                flagTemp[5] = 1
            elseif i == 5 then
            	flagTemp[1] = 3
                flagTemp[2] = 2
                flagTemp[3] = 1
                flagTemp[4] = 2
                flagTemp[5] = 3
            elseif i == 6 then
            	flagTemp[1] = 2
                flagTemp[2] = 1
                flagTemp[3] = 1
                flagTemp[4] = 1
                flagTemp[5] = 2
            elseif i == 7 then
            	flagTemp[1] = 2
                flagTemp[2] = 3
                flagTemp[3] = 3
                flagTemp[4] = 3
                flagTemp[5] = 2
            elseif i == 8 then
            	flagTemp[1] = 1
                flagTemp[2] = 1
                flagTemp[3] = 2
                flagTemp[4] = 3
                flagTemp[5] = 3
            elseif i == 9 then
            	flagTemp[1] = 3
                flagTemp[2] = 3
                flagTemp[3] = 2
                flagTemp[4] = 1
                flagTemp[5] = 1
        	end

            if lineInfo[i][1] == lineInfo[i][2] and lineInfo[i][1] == lineInfo[i][3] and lineInfo[i][1] == lineInfo[i][4] and lineInfo[i][1] == lineInfo[i][5] then
                for j = 1 , 5 do
                    if lineInfo[i][j] == self.nodeCountry[i] and flag[flagTemp[j]][j] == 0 then
                    	flag[flagTemp[j]][j] = 1
						self.nodeCountryNum[self.nodeCountry[i]][1] = self.nodeCountryNum[self.nodeCountry[i]][1] + 1
                    end
                end
            elseif lineInfo[i][1] == lineInfo[i][2] and lineInfo[i][1] == lineInfo[i][3] and lineInfo[i][1] == lineInfo[i][4] then
                for j = 1 , 4 do
                    if lineInfo[i][j] == self.nodeCountry[i] and flag[flagTemp[j]][j] == 0 then
                    	flag[flagTemp[j]][j] = 1
                        self.nodeCountryNum[self.nodeCountry[i]][1] = self.nodeCountryNum[self.nodeCountry[i]][1] + 1
                    end
                end
            elseif lineInfo[i][1] == lineInfo[i][2] and lineInfo[i][1] == lineInfo[i][3] then
                for j = 1 , 3 do
                    if lineInfo[i][j] == self.nodeCountry[i] and flag[flagTemp[j]][j] == 0 then
                    	flag[flagTemp[j]][j] = 1
                        self.nodeCountryNum[self.nodeCountry[i]][1] = self.nodeCountryNum[self.nodeCountry[i]][1] + 1
                    end
                end
            elseif lineInfo[i][2] == lineInfo[i][3] and lineInfo[i][2] == lineInfo[i][4] and lineInfo[i][2] == lineInfo[i][5] then
                for j = 2 , 5 do
                    if lineInfo[i][j] == self.nodeCountry[i] and flag[flagTemp[j]][j] == 0 then
                    	flag[flagTemp[j]][j] = 1
                        self.nodeCountryNum[self.nodeCountry[i]][1] = self.nodeCountryNum[self.nodeCountry[i]][1] + 1
                    end
                end
            elseif lineInfo[i][3] == lineInfo[i][4] and lineInfo[i][3] == lineInfo[i][5] then
                for j = 3 , 5 do
                    if lineInfo[i][j] == self.nodeCountry[i] and flag[flagTemp[j]][j] == 0 then
                    	flag[flagTemp[j]][j] = 1
                        self.nodeCountryNum[self.nodeCountry[i]][1] = self.nodeCountryNum[self.nodeCountry[i]][1] + 1
                    end
                end
            end
        end
    end
--    dump(self.nodeCountryNum,"self.nodeCountryNum")

    --分数
    local coins = 0
    for i = 1,cmd.MAX_LINE_COUNT do
        if self._scene.m_lineMultiple[i] > 0 and self.nodeCountry[i] ~= cmd.CT_CHINA then
            coins = coins + self._scene.m_lineMultiple[i]
        end
    end
    self._scene.m_lUserCoins = self._scene.m_lUserCoins + coins
    self._scene.m_winScore = coins
end

--显示中奖线
function GameViewLayer:showGameLine(i)
    self.nodeLine[i]:getChildByName("Sprite_left"):setSpriteFrame(string.format("game/im_%d_1.png", i))
    self.nodeLine[i]:getChildByName("Sprite_right"):setSpriteFrame(string.format("game/im_%d_1.png", i))
--    self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(true)

    local animTimeline = cc.CSLoader:createTimeline(string.format("./animation/line_%d.csb", i))
    local animinfo = animTimeline:getAnimationInfo("play")
    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false)
    self.animLine[i]:setVisible(true)
    self.animLine[i]:runAction(animTimeline)
end

function GameViewLayer:showRewardLayer()
    print("GameViewLayer:showRewardLayer")
    if self.rewardLayer then
        self.rewardLayer:removeFromParent()
    end
    self.rewardLayer = RewardLayer:create()
    self.rewardLayer.m_parent = self
    self.rewardLayer:updateInfo(self.nodeCountry,self.nodeCountryNum,self._scene.m_winScore,self._scene.nGiveAwayCount)
	self._csbNode:addChild(self.rewardLayer,TAG_ZORDER.Zorders_Set_Layer)
end

--改变开始按钮
function GameViewLayer:changeStartButton(isStart)
    local t = isStart and 0 or 0
    if self.autoBegin then
        isStart = true
    end
	local seq = cc.Sequence:create(
        cc.CallFunc:create(function (  )
            if isStart then
                self.m_btnStart:setEnabled(false)
            else
                self.m_btnStart:setEnabled(true)
            end
		end),
		cc.DelayTime:create(t),
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
	-- csbNode:setPosition(667,375)
	-- local sp_bg = csbNode:getChildByName("Sprite_Bg")

	------
	--按钮事件
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end 
	--关闭按钮
	local btn = csbNode:getChildByName("Button_exit")
	btn:setTag(TAG_ENUM.BT_CLOSEBANK)
	btn:addTouchEventListener(btnEvent)

	--取款按钮
	btn = csbNode:getChildByName("Button_take")
	btn:setTag(TAG_ENUM.BT_TAKESCORE)
	btn:addTouchEventListener(btnEvent)

	--取款按钮
	btn = csbNode:getChildByName("Button_in")
	btn:setTag(TAG_ENUM.BT_SAVESCORE)
	btn:addTouchEventListener(btnEvent)
	------

	------
	--编辑框
	--取款金额
	local tmp = csbNode:getChildByName("input_1")
	local editbox = ccui.EditBox:create(cc.size(300,76),"")
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
	-- tmp:removeFromParent()

	--取款密码
	tmp = csbNode:getChildByName("input_2")
	editbox = ccui.EditBox:create(cc.size(300,76),"")
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
	-- tmp:removeFromParent()
	------

	--当前游戏币
	self.m_bankLayer.m_textCurrent = csbNode:getChildByName("cur_score")

	--银行游戏币
	self.m_bankLayer.m_textBank = csbNode:getChildByName("bank_score")

	--取款费率
	self.m_bankLayer.m_textTips = csbNode:getChildByName("Text_1")

    self.bankBtns = {}
    for i = 1, 5 do 
        local btn = csbNode:getChildByName("Button_"..i)
        btn:setTag(i)
        if i == 5 then
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_bankLayer.m_editNumber:setText("")
            end)
        else
            if i < 4 then
                self:addLabelAtlas(btn,BANK_BTN_NUM[i])
                table.insert(self.bankBtns,btn)
            end
            if i == 4 then
            	btn:setTag(5)
            end
            btn:addTouchEventListener(function(sender) 
                ExternalFun.playClickEffect() 
                self.m_bankLayer.m_editNumber:setText(BANK_BTN_NUM[sender:getTag()])
            end)
        end
    end
end

function GameViewLayer:addLabelAtlas(parent, str)
    parent:removeAllChildren()
    local lab = cc.LabelAtlas:create("","bank/numb.png",22, 33, string.byte("."))
    parent:addChild(lab)
    lab:setAnchorPoint(0.5,0.5)
    lab:setPosition(parent:getContentSize().width/2,parent:getContentSize().height/2  )
    local num = str
    lab:setString(num)
    if num >= 100000000 then
        num = num / 100000000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/yi.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
    elseif num >= 10000 then
        num = num / 10000
        lab:setString(num)
        local sp = cc.Sprite:create("bank/wan.png")
        parent:addChild(sp)
        sp:setAnchorPoint(0, 0.5)
        lab:setPositionX(parent:getContentSize().width/2-14)
        sp:setPosition(lab:getPositionX() + lab:getContentSize().width/2 , parent:getContentSize().height/2)
    end
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
	
	GlobalUserItem.tabAccountInfo.lUserScore= bank_success.lUserScore
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
	BANK_BTN_NUM = bankinfo.lBankOPButtonArray[1]
	if nil ~= self.m_bankLayer then
		local str = "温馨提示:取款将扣除" .. bankinfo.wRevenueTake .. "‰的手续费"
		self.m_bankLayer.m_textTips:setString(str)
		self:refreshScore(bankinfo)
        for i = 1, 3 do
            self:addLabelAtlas(self.bankBtns[i], BANK_BTN_NUM[i])
        end
	end
end

--刷新游戏币
function GameViewLayer:refreshScore( userinfo )
	local userinfo =  GlobalUserItem

	--携带游戏币
	local str = ExternalFun.numberThousands(userinfo.tabAccountInfo.lUserScore)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	-- str = string.gsub(str,",",".")
	self.m_bankLayer.m_textCurrent:setString(userinfo.tabAccountInfo.lUserScore)
	--银行存款
	str = ExternalFun.numberThousands(userinfo.tabAccountInfo.lUserInsure)
	if string.len(str) > 19 then
		str = string.sub(str, 1, 19)
	end
	str = string.gsub(str,",","/")
	self.m_bankLayer.m_textBank:setString(userinfo.tabAccountInfo.lUserInsure)
	--self.m_bankLayer.m_textBank:setString(ExternalFun.numberThousands(userinfo.lUserInsure))
	self._scene.m_lUserCoins = userinfo.tabAccountInfo.lUserScore
	self:refreshGameScore()

	self.m_bankLayer.m_editNumber:setText("")
	self.m_bankLayer.m_editPasswd:setText("")

end


function GameViewLayer:refreshGameScore(  )
	--金币
--	local str = ExternalFun.numberThousands(self._scene.m_lUserCoins)
--	if string.len(str) > 19 then
--		str = string.sub(str, 1, 19)
--	end
--	str = string.gsub(str,",","/")

    local score = self._scene.m_lUserCoins
    local node = self.m_textUserCoin
    if score >= 10000 then
        if score >= 1000000 then
            score = math.floor(score / 10000)
        else
            local a,b = math.modf(score / 10000)
            if b < 0.1 then
                score = a
            else
                score = string.format("%.1f",score / 10000)
            end
        end
        node:getChildByName("Sprite_wan"):setPosition(string.len(score) * 19, 1)
        node:setPosition(115, 20)
        node:getChildByName("Sprite_wan"):setVisible(true)
    else
        node:setPosition(118, 20)
        node:getChildByName("Sprite_wan"):setVisible(false)
    end
    node:setString(score)
    
	--彩金
--	str = ExternalFun.numberThousands(self._scene.m_lUserBeans)
--	if string.len(str) > 19 then
--		str = string.sub(str, 1, 19)
--	end
--	str = string.gsub(str,",","/")
    self:updatePoolScore()
end

function GameViewLayer:initButton()
    if self._scene.nGiveAwayCount > 0 then
        self.lineNum = self._scene.m_gamedata.cbLineCount
        self.scoreIndex = self._scene.m_gamedata.cbBetIndex + 1
        self.artLineNum:setString(self.lineNum)
        self:updatePlusScore()
        -- self.Button_line:setEnabled(false)
        -- self.Button_add:setEnabled(false)
        -- self.Button_check:setEnabled(false)
        self:setButtonEnable(false);
    else
        -- self.Button_line:setEnabled(true)
        -- self.Button_add:setEnabled(true)
        -- self.Button_check:setEnabled(true)
        self:setButtonEnable(true);
    end
end

function GameViewLayer:setButtonEnable(isEable)
	self.Button_line:setEnabled(isEable);
    self.Button_add:setEnabled(isEable);
    self.Button_check:setEnabled(isEable);
end

--公告
function GameViewLayer:addGongGao(_cmd_table)
	table.insert(self._scene.tGonggao,{cmd_table =_cmd_table})

	if not self.isGongGao then
		self:showGongGao(clone(self._scene.tGonggao[1].cmd_table))
	end
end

function GameViewLayer:showGongGao(cmd_table,ggtype)
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