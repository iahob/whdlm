local GameViewLayer = class("GameViewLayer",function(scene)
		local gameViewLayer =  display.newLayer()
	return gameViewLayer
end)


local module_pre = "game.yule.linefruit.src"
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"

local cmd = appdf.req(appdf.GAME_SRC.."yule.linefruit.src.models.CMD_Game")
local QueryDialog   = require("app.views.layer.other.QueryDialog")
local GameRollList = appdf.req(module_pre .. ".views.layer.GameRollList")
local SettingLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local HelpLayer = appdf.req(module_pre .. ".views.layer.HelpLayer")
local RewardLayer = appdf.req(module_pre .. ".views.layer.RewardLayer")
local GoldAnim = appdf.req(module_pre .. ".views.layer.GoldAnim")

local BankLayer = appdf.req(module_pre..".views.layer.BankLayer")


local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")

GameViewLayer.RES_PATH = device.writablePath.. "game/yule/linefruit/res/"

local ButtonTag = 
{
	"Tag_Top" ,--顶部
	"Tag_Menu",
	"Tag_BT_Return",--返回按钮
	"Tag_BT_Bank",
    "Tag_BT_LINE_ADD",
    "Tag_BT_LINE_SUB",
    "Tag_BT_SCORE_ADD",
    "Tag_BT_SCORE_SUB",
    "Tag_BT_CHECK",
    "Tag_BT_MAX",
	"Tag_BT_Horn",--喇叭
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
	"Tag_BT_AUTO",--自动
	
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
	ExternalFun.playBackgroudAudio("bg.mp3")

    self.rollListListener = cc.EventListenerCustom:create("EVENT_NEXT",handler(self, self.callBackFun))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.rollListListener,self)
    self.nextNum = 1
    self.playing = false
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    if self.m_bankLayer then
        self.m_bankLayer:refreshBankRate(bankinfo.wRevenueTake,bankinfo.lBankOPButtonArray)
    end
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
        self.m_bankLayer:refreshBankScore()
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

    display.loadSpriteFrames(cmd.RES.."linefruit_game.plist",cmd.RES.."linefruit_game.png")
    display.loadSpriteFrames(cmd.RES.."animation/startbar.plist",cmd.RES.."animation/startbar.png")
end

function GameViewLayer:initData(  )

	self.m_gamedata = {}
	self.m_bTaskComplete = {}
	--self.m_plusindex = 1
	self.m_rolllist = {}

	self.isGongGao = false                                          --是否正在播放公告
    self.lineNum = cmd.MAX_LINE_COUNT
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
		elseif eventType == ccui.TouchEventType.canceled then
			ExternalFun.dismissTouchFilter()
		elseif eventType == ccui.TouchEventType.ended then
			ExternalFun.dismissTouchFilter()
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end
	--开始
	self.m_btnStartSet = csbNode:getChildByName("Button_start_set");
	self.m_btnStartBar = csbNode:getChildByName("Button_start_bar");
	self.m_btnStart = csbNode:getChildByName("Button_start");
	self.m_btnStart:setTag(TAG_ENUM.Tag_BT_Begin);
	self.m_btnStart:addTouchEventListener(btnEvent);
    self.sprite1 = csbNode:getChildByName("Sprite_1");
    self.sprite2 = csbNode:getChildByName("Sprite_2");
    self.m_btnStartNum = csbNode:getChildByName("Button_start"):getChildByName("Text_1")
	--自动
	self.m_btnAuto = csbNode:getChildByName("Button_auto");
	self.m_btnAuto:setTag(TAG_ENUM.Tag_BT_AUTO);
	self.m_btnAuto:addTouchEventListener(btnEvent);

    self.btnPosition = {}

	--设置
	self.Button_set = csbNode:getChildByName("Button_set");
	self.Button_set:setTag(TAG_ENUM.Tag_BT_Setting);
	self.Button_set:addTouchEventListener(btnEvent);
    self.btnPosition.Button_set = self.Button_set:getPositionY()

--    self.setLayer = csbNode:getChildByName("im_setting_bg");

    local Button_blank = csbNode:getChildByName("Button_blank");
	Button_blank:setTag(TAG_ENUM.Tag_BT_BLANK);
	Button_blank:addTouchEventListener(btnEvent);

    self.Button_music = csbNode:getChildByName("Button_music");
	self.Button_music:setTag(TAG_ENUM.Tag_BT_MUSIC);
	self.Button_music:addTouchEventListener(btnEvent);
    self.btnPosition.Button_music = self.Button_music:getPositionY()
    self.Button_music:setPosition(self.Button_set:getPositionX(),self.btnPosition.Button_set)

    self.Button_help = csbNode:getChildByName("Button_help");
	self.Button_help:setTag(TAG_ENUM.Tag_BT_HELP);
	self.Button_help:addTouchEventListener(btnEvent);
    self.btnPosition.Button_help = self.Button_help:getPositionY()
    self.Button_help:setPosition(self.Button_set:getPositionX(),self.btnPosition.Button_set)

    self.Button_bank = csbNode:getChildByName("Button_bank");
	self.Button_bank:setTag(TAG_ENUM.Tag_BT_Bank);
	self.Button_bank:addTouchEventListener(btnEvent);
    self.btnPosition.Button_bank = self.Button_bank:getPositionY()
    self.Button_bank:setPosition(self.Button_set:getPositionX(),self.btnPosition.Button_set)

    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    if not enableBank then
        self.Button_bank:setVisible(false)
    end

	--退出
	local Button_quit = csbNode:getChildByName("Button_quit");
	Button_quit:setTag(TAG_ENUM.Tag_BT_Quit);
	Button_quit:addTouchEventListener(btnEvent);

	--银行按钮
--	local Button_bank = csbNode:getChildByName("Button_bank");
--	Button_bank:setTag(TAG_ENUM.Tag_BT_Bank);
--	Button_bank:addTouchEventListener(btnEvent);
	-------------------------
	--本局得分
    self.totalScore = csbNode:getChildByName("im_totalwin_bg"):getChildByName("Text_userscore")
    
	--满线
    self.Button_check = csbNode:getChildByName("Button_check");
	self.Button_check:setTag(TAG_ENUM.Tag_BT_CHECK)
	self.Button_check:addTouchEventListener(btnEvent)

    --最大下注
    self.Button_max = csbNode:getChildByName("Button_max");
	self.Button_max:setTag(TAG_ENUM.Tag_BT_MAX)
	self.Button_max:addTouchEventListener(btnEvent)

    --线
    self.nodeLine = {}
    self.animLine = {}
    for i = 1, cmd.MAX_LINE_COUNT do
        self.nodeLine[i] = csbNode:getChildByName(string.format("Node_Line_%d",i))
        self.nodeLine[i]:setVisible(true)

--        self.animLine[i] = cc.CSLoader:createNode(string.format("./animation/line_%d.csb", i))
--        self.animLine[i]:setVisible(false)
--        self.nodeLine[i]:addChild(self.animLine[i])
--        self.animLine[i]:setPosition(self.nodeLine[i]:getChildByName("Sprite_middle"):getPosition())
    end

    self.buttonBlank = csbNode:getChildByName("Button_blank")
	self.buttonBlank:addTouchEventListener(function() 
    end)

	--线押注
    self.Button_line_add = csbNode:getChildByName("Button_line_add")
	self.Button_line_add:setTag(TAG_ENUM.Tag_BT_LINE_ADD)
	self.Button_line_add:addTouchEventListener(btnEvent)
    self.Button_line_sub = csbNode:getChildByName("Button_line_sub")
	self.Button_line_sub:setTag(TAG_ENUM.Tag_BT_LINE_SUB)
	self.Button_line_sub:addTouchEventListener(btnEvent)
    self.artLineNum = csbNode:getChildByName("im_line_bg"):getChildByName("Art_score")
    self.artLineNum:setString(self.lineNum)

    --本局下注
    self.Button_score_add = csbNode:getChildByName("Button_score_add");
	self.Button_score_add:setTag(TAG_ENUM.Tag_BT_SCORE_ADD)
	self.Button_score_add:addTouchEventListener(btnEvent)
    self.Button_score_sub = csbNode:getChildByName("Button_score_sub");
	self.Button_score_sub:setTag(TAG_ENUM.Tag_BT_SCORE_SUB)
	self.Button_score_sub:addTouchEventListener(btnEvent)
    self.artScore = csbNode:getChildByName("im_cell_bg"):getChildByName("Art_score")
    self.artTotalScore = csbNode:getChildByName("im_totaladd_bg"):getChildByName("Text_userscore")
    
    local name = string.EllipsisByConfig(self._scene:GetMeUserItem().szNickName,140,string.getConfig("fonts/round_body.ttf" , 22))
    csbNode:getChildByName("im_name_bg"):getChildByName("Text_name"):setString(name)

	-------------------------
	--滚动层
	self.frameBg = csbNode:getChildByName("im_frame_bg")

	--初始化滚动层
	self:initRolllist()
	--投币动画
--	self:readtoubianim();

	--初始化玩家信息
	self:initUserInfo(csbNode);
    self.spriteBlank = csbNode:getChildByName("Sprite_blank")
    self.spriteFree = csbNode:getChildByName("Sprite_free")

    self:setScale(750/720)
--    self:showRewardLayer()
--    self:showGoldAnim()
end

function GameViewLayer:initRolllist( )
	for i=1,cmd.X_COUNT do
		self.m_rolllist[i] = GameRollList:create(self)
		self.m_rolllist[i]:setPosition(cc.p(316 + (i-1) * 141,167.5))
		self.frameBg:addChild(self.m_rolllist[i])
	end
end

function GameViewLayer:breath(sprite)
	local callfunc = cc.CallFunc:create(function()
		self:breath(sprite)
	end)
	sprite:runAction(cc.Sequence:create(cc.ScaleTo:create(0.4,1.2),cc.ScaleTo:create(0.4,1.0),callfunc))
end

--初始化玩家信息
function GameViewLayer:initUserInfo( csbNode )
	--玩家头像
--	local tmp = csbNode:getChildByName("Sprite_head")
--	local head = PopupInfoHead:createNormal(self._scene:GetMeUserItem(), tmp:getContentSize().width-10)
--	head:setPosition(tmp:getPosition())
--	csbNode:addChild(head)
--	head:enableInfoPop(true,cc.p(55,55),cc.p(0,0))

	--玩家游戏币
	local nodeCoin = csbNode:getChildByName("im_score_bg")
	self.m_textUserCoin = nodeCoin:getChildByName("Text_1")

	--彩金
	self.nodeLotteryScore = csbNode:getChildByName("Art_lotteryscore")
end

--按钮回调
function GameViewLayer:onButtonClickedEvent(tag,ref)
	if tag == TAG_ENUM.Tag_BT_Quit then             --退出
		self._scene.m_bIsLeave = true
		self:getParentNode():onQueryExitGame()
		ExternalFun.playClickEffect()
	elseif tag == TAG_ENUM.Tag_BT_Begin  then       --开始游戏
		print("游戏开始")
        if self.playing then
            return
        end
        self.playing = true
        self.totalScoreNum = 0
        self:updateTotalScore()
        self:setAllButtonEnabled(false)
        for i = 1, cmd.MAX_LINE_COUNT do
            self.nodeLine[i]:setVisible(false)
        end
        self.spriteFree:setVisible(false)

        self._csbNode:getChildByName("game_bg"):runAction(cc.Sequence:create(
        cc.CallFunc:create(function() 
            if self._scene.nGiveAwayCount > 0 then
                self.m_btnStart:loadTextureNormal("game/bt_start_1.png")
                self.sprite2:getChildByName("Sprite_2"):setSpriteFrame("game/bt_start_1.png")
            elseif self._scene.nGiveAwayCount == 0 then
                self.m_btnStart:loadTextureNormal("game/bt_start_0.png")
                self.sprite2:getChildByName("Sprite_2"):setSpriteFrame("game/bt_start_0.png")
            end
        end),
        cc.DelayTime:create(0.2),
        cc.CallFunc:create(function() 
            self.m_btnStartSet:setSpriteFrame("animation/startbar_2.png")
	        self.m_btnStartBar:setVisible(false)
	        self.m_btnStart:setVisible(false)
            self.sprite1:setVisible(true)
        end),
        cc.DelayTime:create(0.1),
        cc.CallFunc:create(function() 
            self.sprite1:setVisible(false)
            self.m_btnStartSet:setSpriteFrame("animation/startbar_3.png")
	        self.m_btnStartBar:setVisible(false)
	        self.m_btnStart:setVisible(false)
            self.sprite2:setVisible(true)
        end),
        cc.DelayTime:create(0.2),
        cc.CallFunc:create(function()
            self.m_btnStartSet:setSpriteFrame("animation/startbar_1.png")
	        self.m_btnStartBar:setVisible(true)
	        self.m_btnStart:setVisible(true)
            self.m_btnStart:loadTextureNormal("game/bt_start_0.png")
            self.sprite2:setVisible(false)

            self:getParentNode():onGameStart()
            ExternalFun.playSoundEffect("bar_eff.wav")
--            self:showRewardLayer()
        end)
        ))
	elseif tag == TAG_ENUM.Tag_BT_AUTO  then       --自动
        if self.autoBegin then
            self.autoBegin = false
            self.m_btnAuto:loadTextureNormal(cmd.RES.."game/bt_auto_0.png")
        else
            self.autoBegin = true
            self.m_btnAuto:loadTextureNormal(cmd.RES.."game/bt_auto_1.png")
            if not self.playing then
                self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Begin)
            end
        end
        ExternalFun.playSoundEffect("StartLogo.wav")
	elseif tag == TAG_ENUM.Tag_BT_Setting  then     --设置
        if self.setOpen then
            self.Button_music:runAction(cc.MoveBy:create(0.2,cc.p(0,self.btnPosition.Button_set - self.btnPosition.Button_music)))
            self.Button_help:runAction(cc.MoveBy:create(0.2,cc.p(0,self.btnPosition.Button_set - self.btnPosition.Button_help)))
            self.Button_bank:runAction(cc.MoveBy:create(0.2,cc.p(0,self.btnPosition.Button_set - self.btnPosition.Button_bank)))
            self.Button_set:loadTextureNormal(cmd.RES.."game/bt_setting_0.png")
            self.setOpen = false
        else
            self.Button_music:runAction(cc.MoveBy:create(0.2,cc.p(0,-(self.btnPosition.Button_set - self.btnPosition.Button_music))))
            self.Button_help:runAction(cc.MoveBy:create(0.2,cc.p(0,-(self.btnPosition.Button_set - self.btnPosition.Button_help))))
            self.Button_bank:runAction(cc.MoveBy:create(0.2,cc.p(0,-(self.btnPosition.Button_set - self.btnPosition.Button_bank))))
            self.Button_set:loadTextureNormal(cmd.RES.."game/bt_setting_1.png")
            self.setOpen = true
        end
        ExternalFun.playSoundEffect("rule_click.wav")
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
		if nil == self._scene.m_gamestatus or cmd.GAME_SCENE_FREE ~= self._scene.m_gamestatus then
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
		
			self:createBankLayer()
		
        ExternalFun.playSoundEffect("OpenWidnow.wav")
		
	elseif tag == TAG_ENUM.Tag_BT_Plus_Score  then        --加注
--		self:getParentNode():onAddScore()
--		ExternalFun.playSoundEffect("shangfen.mp3")
	elseif tag == TAG_ENUM.Tag_BT_Minus_Score  then        --减注
--		self._scene:onSubScore()
--		ExternalFun.playSoundEffect("shangfen1.mp3")
	elseif tag == TAG_ENUM.BT_CLOSEBANK then
		
	elseif tag == TAG_ENUM.BT_TAKESCORE then
		self:onTakeScore()
	elseif tag == TAG_ENUM.BT_SAVESCORE then
		self:onSaveScore()
    elseif tag == TAG_ENUM.Tag_BT_LINE_ADD then
        self:onAddLine()
        ExternalFun.playSoundEffect("add_line.wav")
    elseif tag == TAG_ENUM.Tag_BT_LINE_SUB then
        self:onSubLine()
        ExternalFun.playSoundEffect("del_line.wav")
    elseif tag == TAG_ENUM.Tag_BT_SCORE_ADD then
        self:onAddScore()
        ExternalFun.playSoundEffect("add_bet.wav")
    elseif tag == TAG_ENUM.Tag_BT_SCORE_SUB then
        self:onSubScore()
        ExternalFun.playSoundEffect("del_bet.wav")
    elseif tag == TAG_ENUM.Tag_BT_CHECK then
        self:onLineCheck()
        ExternalFun.playSoundEffect("click.wav")
    elseif tag == TAG_ENUM.Tag_BT_MAX then
        self:onMax()
        ExternalFun.playSoundEffect("ButtonDown.wav")
	else
		showToast(self,"功能尚未开放！",1)
	end
end

--满线
function GameViewLayer:onLineCheck()
    for i = 1, cmd.MAX_LINE_COUNT do
        self.nodeLine[i]:setVisible(true)
    end
    self.lineNum = cmd.MAX_LINE_COUNT
    self.artLineNum:setString(self.lineNum)
    local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    self.artTotalScore:setString(score * self.lineNum)
--    self.buttonBlank:setVisible(true)
end

function GameViewLayer:setAllButtonEnabled(flag)
    self.Button_line_add:setEnabled(flag)
    self.Button_line_sub:setEnabled(flag)
    self.Button_score_add:setEnabled(flag)
    self.Button_score_sub:setEnabled(flag)
    self.Button_check:setEnabled(flag)
    self.Button_max:setEnabled(flag)
--    self.m_btnStart:setEnabled(flag)
end

--最大下注
function GameViewLayer:onMax()
    for i = 1, cmd.MAX_LINE_COUNT do
        self.nodeLine[i]:setVisible(true)
    end
    self.lineNum = cmd.MAX_LINE_COUNT
    self.artLineNum:setString(self.lineNum)
    self.scoreIndex = self._scene.m_gamedata.cbBetCount
    local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    self.artScore:setString(score)
    self.artTotalScore:setString(score * self.lineNum)
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

function GameViewLayer:onSubScore()
    if self.scoreIndex <= 1 then
        self.scoreIndex = self._scene.m_gamedata.cbBetCount
    else
        self.scoreIndex = self.scoreIndex - 1
    end
    self:updatePlusScore()
end

--赠送次数
function GameViewLayer:onGiveAwayCount(nGiveAwayCount)
    self._scene.nGiveAwayCount = nGiveAwayCount
    if nGiveAwayCount > 0 then
        self.m_btnStart:loadTextureNormal("game/bt_start_1.png")
        self.m_btnStartNum:setString(string.format("%d", nGiveAwayCount))
        self.m_btnStartNum:setVisible(true)
    else
        self.m_btnStartNum:setVisible(false)
    end
end

--更新下注信息  
function GameViewLayer:updatePlusScore()
    if self._scene.m_gamedata.lCellScore and self._scene.m_gamedata.lBetScore[1] and #self._scene.m_gamedata.lBetScore[1] > 0 then
        local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
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

--            self.artScore:getChildByName("Sprite_wan"):setPosition(string.len(score) * 24 + 10, 1)
--            self.artScore:setPosition(930, 33)
--            self.artScore:getChildByName("Sprite_wan"):setVisible(true)
--        else
--            self.artScore:setPosition(950, 33)
--            self.artScore:getChildByName("Sprite_wan"):setVisible(false)
--        end
        self.artScore:setString(score)
        self.artTotalScore:setString(score * self.lineNum)
    end
end

--线押注
function GameViewLayer:onAddLine()
    self.lineNum = tonumber(self.artLineNum:getString())
    if self.lineNum >= cmd.MAX_LINE_COUNT then
        self.lineNum = 1
        for i = 1, cmd.MAX_LINE_COUNT do
            self.nodeLine[i]:setVisible(false)
        end
        self.nodeLine[1]:setVisible(true)
    else
        self.lineNum = self.lineNum + 1
        self.nodeLine[self.lineNum]:setVisible(true) 
    end
    self.artLineNum:setString(self.lineNum)
    local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    self.artTotalScore:setString(score * self.lineNum)
end

function GameViewLayer:onSubLine()
    self.lineNum = tonumber(self.artLineNum:getString())
    if self.lineNum <= 1 then
        self.lineNum = cmd.MAX_LINE_COUNT
        for i = 1, cmd.MAX_LINE_COUNT do
            self.nodeLine[i]:setVisible(true)
        end
    else
        self.nodeLine[self.lineNum]:setVisible(false) 
        self.lineNum = self.lineNum - 1
    end
    self.artLineNum:setString(self.lineNum)
    local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    self.artTotalScore:setString(score * self.lineNum)
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
--		self:changeStartButton(false)
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
	self._csbNode:addChild(help,TAG_ZORDER.Zorders_Set_Layer + 1)
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
--    for i = 1, cmd.MAX_LINE_COUNT do
--        self.nodeLine[i]:getChildByName("Sprite_left"):setSpriteFrame(string.format("game/im_%d_0.png", i))
--        self.nodeLine[i]:getChildByName("Sprite_right"):setSpriteFrame(string.format("game/im_%d_0.png", i))
--        self.nodeLine[i]:getChildByName("Sprite_middle"):setVisible(false)
--        self.animLine[i]:setVisible(false)
--    end
    if self.rewardLayer then
        self.rewardLayer:showLayer(false)
    end
    self._scene.m_winScore = 0;
    self._scene.m_taskScore = 0;
    self:updateWinScore();
    self.totalScoreNum = 0
    self:updateTotalScore()
end

--游戏总分
function GameViewLayer:updateTotalScore()
    local node = self.totalScore

    -- self.totalScoreNum = self.totalScoreNum + self._scene.m_winScore

    if self._scene.m_taskScore > 0 then
    	self.totalScoreNum = self._scene.m_taskScore;
    else
    	self.totalScoreNum = self._scene.m_winScore;
    end

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
    end
    if self._scene.m_scheduleUpdate ~= nil then
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._scene.m_scheduleUpdate)
        self._scene.m_scheduleUpdate = nil
    end
end

function GameViewLayer:gameEnd()
    self:updateTotalScore()
    self:updateSelfScore()
    self:updateWinScore()
    self:refreshGameScore()
    self._scene.m_gamestatus = cmd.GAME_SCENE_FREE
    print("self._scene.m_winScore", self._scene.m_winScore)
    --        for i=1,cmd.X_COUNT do
    --            local s = self.m_rolllist[i].m_RollLayout:getChildren()
    --            self:breath(s[1])
    --        end
    if self._scene.m_winScore > 0 then
        ExternalFun.playSoundEffect("GameWin.wav")
    else
        ExternalFun.playSoundEffect("GameFail.wav")
    end

--    local go = true
--    for i = 1, #self._scene.cbItemType do
--        if self._scene.cbItemType[i] > 0 then
--            go = false
--            break
--        end
--    end
--    go = false
--    if go then
--        if self.autoBegin then
--            self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Begin)
--            return
--        end
--    end
    
    local a = 0
    local b = 0
    local score = self._scene.m_gamedata.lCellScore * self._scene.m_gamedata.lBetScore[1][self.scoreIndex]
    score = math.floor(self.totalScoreNum/score)
    if self.autoBegin then
        a = 1
        b = 1.5
        local go = true
        if score > 0 then
            b = 3
        else
            b = 0.5
        end
    else
        a = 0.35
        b = 0.35
        --不同倍数时间
        if score < 50 and score > 0 then
            b = 1.0
        elseif score < 100 and score >= 50 then
            b = 2.0
        elseif score >= 100 then
            b = 3.0
        end
    end

    local rabbit = false
    self:runAction(cc.Sequence:create(
    cc.DelayTime:create(0.2),
    cc.CallFunc:create( function()
        if self._scene.rabbitStartInfo then
            self.spriteBlank:setVisible(true)
            self.buttonBlank:setVisible(true)
        end
        if score > 0 then
            self:showGoldAnim(score)
        end
    end ),
    cc.DelayTime:create(a),
    cc.CallFunc:create( function()
        if self._scene.beginLock then
            --                    if table.nums(self.nodeCountry) > 0 then
            --                        self:showRewardLayer()
            --                    end
        end
        if self._scene.rabbitStartInfo then
            self:showRewardLayer(self._scene.rabbitStartInfo)
            rabbit = true
            self.spriteBlank:setVisible(false)
            self.buttonBlank:setVisible(false)
            self.autoBegin = false
            self.m_btnAuto:loadTextureNormal(cmd.RES.."game/bt_auto_0.png")
        end
    end ),
    cc.DelayTime:create(b),
    cc.CallFunc:create( function()
        self.playing = false
        if self._scene.nGiveAwayCount > 0 then
            self:initButton()
            self:onGiveAwayCount(self._scene.nGiveAwayCount)
            self:setAllButtonEnabled(false)
        else
            self:setAllButtonEnabled(true)
        end
        if self.autoBegin and not rabbit then
            self:onButtonClickedEvent(TAG_ENUM.Tag_BT_Begin)
        end
    end )
    ))
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
--            self:showGameLine(i)
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
        if self._scene.m_lineMultiple[i] > 0 then
            coins = coins + self._scene.m_lineMultiple[i]
        end
    end

    self._scene.m_lUserCoins = self._scene.m_lUserCoins + coins
    self._scene.m_winScore = coins

    local rabbit = false
    self.sendEnd = false
    --线
    for i = 1,cmd.MAX_LINE_COUNT do
        if self._scene.cbItemType[i] ~= yl.INVALID_BYTE then
            self:showGameLine(i)
        end
        if self._scene.cbItemType[i] == 8 and not rabbit then
            rabbit = true
            self._scene:sendMessageRabbitStart()
        end
        if self._scene.cbItemType[i] == 9 then
            self.spriteFree:setVisible(true)
        end
    end
    if not rabbit then
        self.sendEnd = true
        self._scene:sendMessageGameStop()
        self:gameEnd()
    end
end

--显示中奖线
function GameViewLayer:showGameLine(i)
--    self.nodeLine[i]:getChildByName("Sprite_left"):setSpriteFrame(string.format("game/im_%d_1.png", i))
--    self.nodeLine[i]:getChildByName("Sprite_right"):setSpriteFrame(string.format("game/im_%d_1.png", i))
    self.nodeLine[i]:setVisible(true)

--    local animTimeline = cc.CSLoader:createTimeline(string.format("./animation/line_%d.csb", i))
--    local animinfo = animTimeline:getAnimationInfo("play")
--    animTimeline:gotoFrameAndPlay(animinfo.startIndex, animinfo.endIndex, false)
--    self.animLine[i]:setVisible(true)
--    self.animLine[i]:runAction(animTimeline)
end

function GameViewLayer:showRewardLayer()
    print("GameViewLayer:showRewardLayer")
    if self.rewardLayer then
        self.rewardLayer:removeFromParent()
    end
    self.rewardLayer = RewardLayer:create(self._scene.rabbitStartInfo)
    self.rewardLayer.m_parent = self
	self._csbNode:addChild(self.rewardLayer,TAG_ZORDER.Zorders_Set_Layer)
end

function GameViewLayer:showGoldAnim(score)
    if self.goldAnim then
        self.goldAnim:removeFromParent()
    end
    self.goldAnim = GoldAnim:create(score)
    self.goldAnim.m_parent = self
    self.goldAnim:setGold(self.totalScoreNum)
	self._csbNode:addChild(self.goldAnim,TAG_ZORDER.Zorders_Set_Layer)
end

--改变开始按钮
function GameViewLayer:changeStartButton(isStart)
--    local t = isStart and 1 or 0
--    if self.autoBegin then
--        isStart = true
--    end
--	local seq = cc.Sequence:create(
--        cc.CallFunc:create(function (  )
--            if isStart then
--                self.m_btnStart:setEnabled(false)
--            else
--                self.m_btnStart:setEnabled(true)
--            end
--		end),
--		cc.DelayTime:create(t),
--		cc.CallFunc:create(function (  )
--			self.m_btnStart:setVisible(not isStart)
----			self.m_btnStop:setVisible(isStart)
--		end))
--	--self:stopAllActions()
--	self:runAction(seq)
end

function GameViewLayer:getGameData(  )
	return self._scene.m_gamedata
end

----------------------

----------------------
--银行节点
function GameViewLayer:createBankLayer()
	

	self.m_bankLayer = BankLayer:create(self):addTo(self)
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
        self.m_bankLayer:refreshBankScore()
    end
    self._scene.m_lUserCoins = GlobalUserItem.tabAccountInfo.lUserScore
    self:refreshGameScore()

    
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

--移除银行
function GameViewLayer:removeBankerLayer()
    if self.m_bankLayer then
        self.m_bankLayer:removeFromParent()
        self.m_bankLayer = nil
    end
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
	bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10
    if self.m_bankLayer then
        self.m_bankLayer:refreshBankRate(bankinfo.wRevenueTake,bankinfo.lBankOPButtonArray)
    end
end




function GameViewLayer:refreshGameScore(  )
	--金币
--	local str = ExternalFun.numberThousands(self._scene.m_lUserCoins)
--	if string.len(str) > 19 then
--		str = string.sub(str, 1, 19)
--	end
--	str = string.gsub(str,",","/")

    local score = self._scene.m_lUserCoins
    GlobalUserItem.tabAccountInfo.lUserScore = self._scene.m_lUserCoins
    local node = self.m_textUserCoin
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
--        node:setPosition(115, 20)
--        node:getChildByName("Sprite_wan"):setVisible(true)
--    else
--        node:setPosition(118, 20)
--        node:getChildByName("Sprite_wan"):setVisible(false)
--    end
    node:setString(score)
    
    if nil ~= self.m_bankLayer and true == self.m_bankLayer:isVisible() then
        self.m_bankLayer:refreshBankScore()
    end
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
        self.Button_line_add:setEnabled(false)
        self.Button_score_add:setEnabled(false)
        self.Button_line_sub:setEnabled(false)
        self.Button_score_sub:setEnabled(false)

        self.m_btnStart:loadTextureNormal("game/bt_start_1.png")
        self.sprite2:getChildByName("Sprite_2"):setSpriteFrame("game/bt_start_1.png")
    else
        self.Button_line_add:setEnabled(true)
        self.Button_score_add:setEnabled(true)
        self.Button_line_sub:setEnabled(true)
        self.Button_score_sub:setEnabled(true)
    end
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