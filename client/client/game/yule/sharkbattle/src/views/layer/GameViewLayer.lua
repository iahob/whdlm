local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.sharkbattle.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local ZhuanPanAni=appdf.req(module_pre .. ".views.ZhuanPanAni")
local PlayerlistLayer= appdf.req(module_pre .. ".views.layer.PlayerlistLayer")
local MenuUnfoldLayer= appdf.req(module_pre .. ".views.layer.MenuUnfoldLayer")
local FishGen= appdf.req(module_pre .. ".views.FishGen")
local PopupInfoLayer = require(appdf.EXTERNAL_SRC .. "PopupInfoLayer")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")

local GameViewLayer = class("GameViewLayer",function(scene)
        return display.newLayer()
end)


local popZorder=1

local temptest=0
function GameViewLayer:ctor(scene)
	self._scene = scene
    --注册node事件
   	print(" GameViewLayer:ctor(scene) begin") 
  
    self.csbNode=ExternalFun.loadCSB("MainView.csb",self)
    self:initBetBtns()

    self.pond=appdf.getNodeByName(self.csbNode,"pond")
    self.pond:setLocalZOrder(-1)
    self.pond.m_tabFishes={}
    FishGen.createFishCrowd1(self.pond)
	FishGen.createFishCrowd2(self.pond)
    for i=1,6 do  --fish kind
		for j=1,7-i do
			FishGen.createSingleFish(i,self.pond)
		end
	end

    self.backBtn=appdf.getNodeByName(self.csbNode,"back")
    self.menuBtn=appdf.getNodeByName(self.csbNode,"menu")
    self.playerlistBtn=appdf.getNodeByName(self.csbNode,"friendlist")
    self.minusBtn=appdf.getNodeByName(self.csbNode,"minus")
    self.plusBtn=appdf.getNodeByName(self.csbNode,"plus")
    self.plusBtn.m_plusNote=true
    self.clearBtn=appdf.getNodeByName(self.csbNode,"clear")
    self.goonBtn=appdf.getNodeByName(self.csbNode,"stillbet")
    self.currentNoteText=appdf.getNodeByName(self.csbNode,"Text_3")
    self.countdownSp=appdf.getNodeByName(self.csbNode,"countdownsp")
    self.bg=appdf.getNodeByName(self.csbNode,"bg")

   
    local this=self
    self.backBtn:addClickEventListener(function() 
    	-- self._scene:onKeyBack() 
    	local MyChair =self._scene:GetMeChairID()+1
	    if self._scene.m_cbGameStatus == cmd.GAME_SCENE_BET or self._scene.m_cbGameStatus == cmd.GAME_SCENE_END then
	    	local betTotalNum = 0;
	    	for i=1,12 do
				betTotalNum = betTotalNum + self._scene.betNums[i];
			end
			if betTotalNum == 0 then
				self._scene.m_cbGameStatus = cmd.GAME_SCENE_FREE
			end
	    end
        self._scene:onQueryExitGame()
    end)
    local menuX,menuY=self.menuBtn:getPosition()
    local menuHeight=self.menuBtn:getContentSize().height
    self.menuBtn:loadTextureNormal("menuunfold.png")  
    self.menuBtn:addClickEventListener(function() 
    	self.menuBtn:loadTextureNormal("menu.png")
    	this:addChild(MenuUnfoldLayer:create(self,menuX,menuY,menuHeight,self.menuBtn),popZorder)  
    end)
    self.playerlistBtn:addClickEventListener(function() this:addChild(PlayerlistLayer:create(self._scene:getPlayerList()),popZorder) end)
    self.minusBtn:addClickEventListener(function(sender) if self.m_cbGameStatus==cmd.GAME_SCENE_BET then self._scene:OnMessageOperationSwitch(sender) end end)
    self.plusBtn:addClickEventListener(function(sender) if self.m_cbGameStatus==cmd.GAME_SCENE_BET then  self._scene:OnMessageOperationSwitch(sender) end end)
    self.clearBtn:addClickEventListener(function() if self.m_cbGameStatus==cmd.GAME_SCENE_BET then  self._scene:OnMessageOperationCancel() end end)
    self.goonBtn:addClickEventListener(function() if self.m_cbGameStatus==cmd.GAME_SCENE_BET then  self._scene:OnMessageOperationRenewal() end end)

    local function genLabelAtlas(file,w,h,bgName,bClock)
    	local labelAtlas=cc.LabelAtlas:create("",file,w,h,string.byte("0"))
    	local bg=appdf.getNodeByName(self.csbNode,bgName)
    	labelAtlas:addTo(bg)
		if true==bClock then 
			labelAtlas:setAnchorPoint(0.5,0.5)
			labelAtlas:setPosition(74,90) 
		else
			labelAtlas:setAnchorPoint(1,0.5)
			labelAtlas:setPosition(bg:getContentSize().width-15,bg:getContentSize().height/2+4)
		end
		return labelAtlas
    end

   	self.totalScoreAtlas=genLabelAtlas("scoreNumImgs.png",23,35,"totalscoreshadow")
    self.playShowWinAtlas=genLabelAtlas("scoreNumImgs.png",23,35,"scoreshadow")
    self.playShowPrizesAtlas=genLabelAtlas("prizeNumImgs.png",23,35,"colorgoldshadow")
    self.totalBetAtlas=genLabelAtlas("scoreNumImgs.png",23,35,"betscoreshadow")
    self.clockAtlas=genLabelAtlas("countNumImgs.png",50,73,"countdownsp",true)

	self:initData()

    ExternalFun.registerNodeEvent(self)
    AnimationMgr.loadAnimationFromFrame("%d.png",1,14,"latestRecordAni",2)
    
   --self:testPlayerlistLayer()
   --self:testRecord()
    print(" GameViewLayer:ctor(scene) end") 
  
end

function GameViewLayer:testRecord()
	for i=1,9 do
		self:AddTurnTableRecord(i)
	end
	self:AddTurnTableRecord(9)
	self:AddTurnTableRecord(1)
	self:AddTurnTableRecord(9)
	self:SetShowTurnTableRecord(true)
end

function GameViewLayer:testPlayerlistLayer()
	local playerlist={}
	for i=1,10 do 
		playerlist[i]={}
		playerlist[i].szNickName="玩家"..i.."号"
		playerlist[i].wFaceID=101
		playerlist[i].lScore=i*i*i*i+10000
	end
	self:addChild(PlayerlistLayer:create(playerlist),10000)
end

function GameViewLayer:initData()
    self.m_tabPlayerList={}
    self.turnTableRecords={}
    self.m_nAnimalMultiple={}

    --self.m_nCurrentNote = 0
    self.m_lPlayPrizes = 0 --玩家彩金
	self.m_lPlayShowWin = 0 --玩家显示得分
	self.m_lPlayShowPrizes = 0  --玩家显示彩金
	self.m_lGameScore=0  --玩家显示总分
	self.m_lPlayWin={}
	self.m_lPlayWin[1],self.m_lPlayWin[2]=0,0

	self:updatePlayShowWin()
    self:updatePlayShowPrizes(math.random(70000)) 
   	self:UpdateShowTotalScore(GlobalUserItem.tabAccountInfo.lUserScore)
   	self:UpdateShowTotalBet(0)
end

function GameViewLayer:reSetForNewGame()
	self:initData()
	self.bReset=true
end

function GameViewLayer:onExit()
	
	cc.Director:getInstance():getTextureCache():removeTextureForKey("bg.png")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("jiesuanlight.png")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("anibetresult.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    --播放大厅背景音乐
    ExternalFun.playPlazzBackgroudAudio()

end

function GameViewLayer:enableAllBtns(bEnable)
	for i=1,12 do
		self.betBtns[i]:setEnabled(bEnable)
	end
	self.minusBtn:setEnabled(bEnable)
    self.plusBtn:setEnabled(bEnable)
    self.clearBtn:setEnabled(bEnable)
    self.goonBtn:setEnabled(bEnable)
end

local function getNineDigits(number)
	if type(number)~="number" or number<1000000000 then 
		return number
	end
	return number%1000000000
end

function GameViewLayer:SetPlayShowWin(showWin)
	self.m_lPlayShowWin=showWin
end

function GameViewLayer:SetPlayShowPrizes(showPrizes)
	self.m_lPlayShowPrizes=showPrizes
end

function GameViewLayer:SetPlayShowGameScore(gameScore) 
	self.m_lGameScore=gameScore
end

function GameViewLayer:SetCurrentNote(note)
	local str
	if note>=10000 then
		str=(note/10000).."W"
	else 
		str=note..""
	end
	self.currentNoteText:setString(str)
end

function GameViewLayer:UpdateShowTotalScore(score)
	self.totalScoreAtlas:setString(getNineDigits(score))
end

function GameViewLayer:UpdateShowTotalBet(num)
	self.totalBetAtlas:setString(getNineDigits(num))
end

local function newDigitSp(filepath,digit,w,h) --digit单个数字0-9
	assert(digit)
	return cc.Sprite:create( filepath,cc.rect(w*digit,0,w,h) )
end


function GameViewLayer:SetGameStatus(gameStatus) --设置显示得分
	self.m_cbGameStatus=gameStatus
end

function GameViewLayer:updatePlayShowWin() --设置显示得分
	self.playShowWinAtlas:setString(getNineDigits(self.m_lPlayShowWin))
end

function GameViewLayer:updatePlayShowPrizes(prize) --设置显示彩金
	self.playShowPrizesAtlas:setString(getNineDigits(prize))
end


function GameViewLayer:OnUpdataClockView(clockViewChair,clockTime)
	for i=1,12 do
		if self.m_cbGameStatus~=cmd.GAME_SCENE_END then
			self.betBtns[i]:stopAllActions()
			self.betBtns[i]:setOpacity(255)
		end
	end

	if self.m_cbGameStatus==cmd.GAME_SCENE_END then
		self.countdownSp:setVisible(false)
		self:updatePlayShowPrizes(self.m_lPlayShowPrizes)
	else
		if self.jsLayer and not tolua.isnull(self.jsLayer) then
			self.jsLayer:removeSelf()
		end
		if self.zhuanPanAni and not tolua.isnull(self.zhuanPanAni) then
			self.zhuanPanAni:removeSelf()
		end

		self.countdownSp:setVisible(true)
		self:updatePlayShowPrizes(math.random(70000))
		if self.m_cbGameStatus==cmd.GAME_SCENE_FREE then
			self.countdownSp:setTexture("countdownfree.png")
		elseif self.m_cbGameStatus==cmd.GAME_SCENE_BET then
			self.countdownSp:setTexture("countdownbet.png")
			if clockTime<=3 then
				ExternalFun.playSoundEffect( "TIME_WARIMG.wav")
			end
		end
	end

	if clockTime~=nil and clockTime<10 then
		clockTime="0"..clockTime
	end
	self.clockAtlas:setString(clockTime)
end

function GameViewLayer:SetAnimalMultiple(tabAnimalMultiple) --倍率
	self.m_nAnimalMultiple=tabAnimalMultiple
	for i=1,#tabAnimalMultiple do
		--print("i="..i,tabAnimalMultiple[i])
		self.betRatioTexts[i]:setString( "x"..(tabAnimalMultiple[i] or 0) )
	end
end

function GameViewLayer:AddTurnTableRecord(betResultId)
	local len=#self.turnTableRecords
	if len >10*cmd.RECORD_COUNT_MAX then   --10可以换成任意大于1的数字
		for i=1,cmd.RECORD_COUNT_MAX-1 do	                    --删除old记录，只保留最近的cmd.RECORD_COUNT_MAX-1个
			self.turnTableRecords[i]=self.turnTableRecords[i+1+len-cmd.RECORD_COUNT_MAX]
		end
		for i=cmd.RECORD_COUNT_MAX,len do
			self.turnTableRecords[cmd.RECORD_COUNT_MAX]=nil
		end
	end
	table.insert(self.turnTableRecords,betResultId)
--	print("betResultId: ",betResultId)
--	print("self.turnTableRecords:",#self.turnTableRecords)
	assert(betResultId>=0 and betResultId<=9)
end

function GameViewLayer:GetGameStatus()
	return self.m_cbGameStatus
end

function GameViewLayer:SetPlayBet(playBet,kind) --一个参数tablePlayBet 或两个参数--playBet,animalIndex
	if kind then
		self.betNumTexts[kind]:setString(""..playBet) 
		self._scene.betNums[kind] = playBet;
	else
		for i=1,#playBet do --playBet为table
			self.betNumTexts[i]:setString(""..(playBet[i] or 0) )
			self._scene.betNums[i] = playBet[i];
		end
	end
end

function GameViewLayer:SetAllPlayBet(playBet,kind) --一个参数tablePlayBet 或两个参数--playBet,animalIndex
	if kind then
		self.allBetNumTexts[kind]:setString(""..playBet) 
	else
		for i=1,#playBet do 
			self.allBetNumTexts[i]:setString(""..(playBet[i] or 0) )
		end
	end
end

function GameViewLayer:GameOver(bTurnTwoTime, nTurnTableTarget, nPrizesMultiple,lPlayWin, lPlayPrizes,lGameScore)

	local totalSec=20-3
	local resultKind=ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[1]+1)
	print("resultKind: ",resultKind)
	if resultKind==cmd.ANIMAL_GOLD_SHARK then
		totalSec=totalSec+30

		local resultKind2=ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[2]+1)
		print("gold shark")
		print("nPrizesMultiple: ",nPrizesMultiple)
		-- assert(nPrizesMultiple>=6 and nPrizesMultiple<=99)
		print("bTurnTwoTime: ",bTurnTwoTime)
		assert(bTurnTwoTime==1)
		print("resultKind2: ", resultKind2)
		print("resultkind2 ratio: ",self.m_nAnimalMultiple[resultKind2+1])
		
	elseif resultKind== cmd.ANIMAL_SLIVER_SHARK then
		totalSec=totalSec+17

		local resultKind2=ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[2]+1)
		print("silver shark")
		print("nPrizesMultiple: ",nPrizesMultiple)
		assert(nPrizesMultiple==0)
		print("bTurnTwoTime: ",bTurnTwoTime)
		assert(bTurnTwoTime==1)
		print("resultKind2: ", resultKind2)
		print("resultkind2 ratio: ",self.m_nAnimalMultiple[resultKind2+1])
		
	end

	self.bTurnTwoTime=bTurnTwoTime
	
	local deltaT=2 --连续两次开奖动画间隔时间
	local dur=totalSec-5 --转盘时间
	if bTurnTwoTime==0 and (resultKind==cmd.ANIMAL_GOLD_SHARK or  resultKind==cmd.ANIMAL_SLIVER_SHARK) then
		dur=dur-5
	end
	local durations={dur,0}
	if bTurnTwoTime==1 then
		totalSec=(totalSec-deltaT)/2 
		durations[1]=totalSec-5
		durations[2]=totalSec-5
	end

	local function showAnims(self,i)
		if self.m_cbGameStatus~=cmd.GAME_SCENE_END then return end
		local resultKind=ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[i]+1)
		
		self:AddTurnTableRecord(resultKind)
		if i==2 then 
			print("durations[i]:",durations[i])
			print("totalSec:",totalSec)
		end
		local zhuanPanAni=ZhuanPanAni:create(self.bg,0,nTurnTableTarget[i],durations[i],totalSec)
		self.zhuanPanAni=zhuanPanAni

		local function callback(showtime)
			if self.m_cbGameStatus~=cmd.GAME_SCENE_END then return end
			self:SetShowTurnTableRecord(true)
			self:showBetResult(resultKind,showtime,nPrizesMultiple)
			local n=20
			local fadeAct=cc.FadeOut:create(math.max(0,showtime)/n) --cc.TintBy:create(math.max(0,showtime)/n,200,200,200)  --226,200,170)
			local repAct=cc.Repeat:create(
				cc.Sequence:create(fadeAct,fadeAct:reverse())
				--cc.Sequence:create(cc.FadeTo:create(math.max(0,showtime)/n,125),cc.FadeTo:create(math.max(0,showtime)/n,255))
				,math.floor(n/2)-2)
			self.betBtns[resultKind+1]:setEnabled(true)
			self.betBtns[resultKind+1]:runAction(repAct )
			if resultKind~=cmd.ANIMAL_SLIVER_SHARK and resultKind~=cmd.ANIMAL_GOLD_SHARK then
				self.betBtns[resultKind<=3 and 12 or 11]:setEnabled(true)
				self.betBtns[resultKind<=3 and 12 or 11]:runAction(repAct:clone())
			end
			self.m_lPlayPrizes = lPlayPrizes;
			self.m_lPlayWin[1]=lPlayWin[1]
			self.m_lPlayWin[2]=lPlayWin[2]
			if i==1 then
				self.m_lPlayShowWin = self.m_lPlayWin[1]
			else
				self.m_lPlayShowWin =self.m_lPlayWin[1]+ self.m_lPlayWin[2]
			end
			self.m_lGameScore=lGameScore
			self:updatePlayShowWin()
		end
		zhuanPanAni:ZhuanPan(callback)
	end
	
	showAnims(self,1)
	
	if bTurnTwoTime==1 then  
		self:runAction(
			cc.Sequence:create(cc.DelayTime:create(totalSec+deltaT),
				cc.CallFunc:create(function() showAnims(self,2) end))
			)
	end
	self.showAnims=showAnims

end


function GameViewLayer:initBetBtns() --12个Button
	local kindToPos={4,5,6,7,3,2,1,0 }   -- 第i（加1）种动物Button在一排中的位置序号
	self.betBtns={} --bet按钮
	self.betRatioTexts={} --显示倍率
	self.betNumTexts={} --显示下注大小
	self.allBetNumTexts={} --显示总下注大小
	local width,height=90,90 --button size
	local offX=80 --各Button的左边界的x左标
	local offY=height/2+30
	local dis=25  --两button间距
	local boardWidth,boardHeight=100,66
	for i=1,8 do --i表示动物种类
		local btn=ccui.Button:create("bet"..i..".png","bet"..i.."down.png","bet"..i.."disabled.png") --string.format("bet%ddisabled",i)
		btn:setContentSize(width,height)
		btn:setPosition(width/2+offX+kindToPos[i]*(dis+width),offY)
		
		--btn.mTag=i
		btn.m_kind=i-1
		btn:addClickEventListener(function(sender) 
			if self.m_cbGameStatus==cmd.GAME_SCENE_BET then
			 	self._scene:OnMessagePlayBet(sender) 
			end
		 end)

		local ratBoard=cc.Sprite:create("betratio.png")
		ratBoard:setPosition(btn:getPositionX(),183)
		ratBoard:setContentSize(boardWidth,boardHeight)
		local betRatio=ccui.Text:create("","fonts/round_body.ttf",26)
			:addTo(ratBoard)
			:setPosition(boardWidth/2,67)
			:setColor(cc.c3b(18,234,248)) --hex 12eaf8

		local betNum=ccui.Text:create("","fonts/round_body.ttf",22)
			:addTo(ratBoard)
			:setPosition(boardWidth/2,41)
			:setColor(cc.c3b(203,231,255)) --hex cbe7ff

		local allBetNum=ccui.Text:create("","fonts/round_body.ttf",22)
			:addTo(ratBoard)
			:setPosition(boardWidth/2,18)
			:setColor(cc.c3b(203,231,255))

		self:addChild(btn)
		self:addChild(ratBoard)
		self.betBtns[i]=btn
		self.betRatioTexts[i]=betRatio
		self.betNumTexts[i]=betNum
		self.allBetNumTexts[i]=allBetNum

		-- local tintAct=cc.FadeOut:create(0.3) --cc.TintBy:create(0.2,226,200,170)
		-- self.betBtns[i]:runAction( cc.RepeatForever:create(
		-- 	cc.Sequence:create(
		-- 		tintAct,tintAct:reverse()
		-- 		))
		-- )
	end
	for i=9,12 do --第i-1种动物按钮
		local btn=appdf.getNodeByName(self.csbNode,"betBtn"..i)
		btn.m_kind=i-1
		self.betBtns[i]=btn
		self.betRatioTexts[i]=btn:getChildByName("Text_1")
		self.betNumTexts[i]=btn:getChildByName("Text_2")
		self.allBetNumTexts[i]=btn:getChildByName("Text_3")
		local ratioOriginY=self.betRatioTexts[i]:getPositionY()
		local betNumOriginY= self.betNumTexts[i]:getPositionY()
		local allBetNumOriginY= self.allBetNumTexts[i]:getPositionY()
		self.betBtns[i]:addTouchEventListener(function(sender, type)
			local offy=5
        	if type== ccui.TouchEventType.began then
        		self.betRatioTexts[i]:setPositionY(ratioOriginY-offy)
        		self.betNumTexts[i]:setPositionY(betNumOriginY-offy)
        		self.allBetNumTexts[i]:setPositionY(allBetNumOriginY-offy)
         	else 
         		if (type==ccui.TouchEventType.moved and false==self.betBtns[i]:isHighlighted()) or
         		   type==ccui.TouchEventType.ended
         		then
         			self.betRatioTexts[i]:setPositionY(ratioOriginY)
        			self.betNumTexts[i]:setPositionY(betNumOriginY)
        			self.allBetNumTexts[i]:setPositionY(allBetNumOriginY)
        		end

        		if type==ccui.TouchEventType.ended and self.m_cbGameStatus==cmd.GAME_SCENE_BET then
					self._scene:OnMessagePlayBet(sender)
				end
         	end
		end)
	end
end


function GameViewLayer:playLoopAnimation( pNode, param)
    local animation = cc.AnimationCache:getInstance():getAnimation(param.m_strName)
    if nil == animation then
        return
    end

    -- 设置参数
    if param.m_bResetParam then
        animation:setDelayPerUnit(param.m_fDelay)
        animation:setRestoreOriginalFrame(param.m_bRestore)
    end

    local act = cc.Animate:create(animation)
    if nil == act then
        return
    end

    pNode:runAction(cc.RepeatForever:create(act))
end

function GameViewLayer:SetShowTurnTableRecord(bShow)
	if not bShow then
		if nil==self.recordView then return end
		self.recordView:setVisible(false)
		return 
	end

	local recordTable=self.turnTableRecords 
	
	print("-----SetShowTurnTableRecord--recordTable----------")
		for i=1,#recordTable do
			print("i=",i,recordTable[i])
		end

	local function numberOfCellsInTableView()
		return cmd.RECORD_COUNT_MAX
	end

	local function cellSizeForTable(view,idx) 
    	return 50,50
	end

	local param=AnimationMgr.getAnimationParam()
    param.m_strName= "latestRecordAni"
    param.m_fDelay= 0.2
    param.m_bRestore=false

	local function tableCellAtIndex(view, idx) --idx从0开始
		--local betResId=recordTable[#recordTable-idx] ----设定recordTable排在后面的（即tableView最顶部的）为最近的的历史记录
		local betResId=self.turnTableRecords[#self.turnTableRecords-idx]
		-- if betResId==nil then print("idx: ",idx) assert(false) end
		--recordTable保存bet结果ID
		print("-----tableCellAtIndex--recordTable----------")
		for i=1,#recordTable do
			print("i=",i,recordTable[i])
		end
		print("-----tableCellAtIndex---self.turnTableRecords----------")
		for i=1,#self.turnTableRecords do
			print("i=",i,self.turnTableRecords[i])
		end
		--assert(recordTable[#recordTable-idx]==self.turnTableRecords[#recordTable-idx])
		if self.bReset==true then
			print("idx=",idx,"betResId=",betResId)
			print("#recordTable:",#recordTable)
		end

    	local cell=view:dequeueCell()

    	local w,h=51,41 --
    	local posIndex={4,5,6,7,3,2,1,0,8,11}--第i种动物在记录合集图中的位置序号
    	local sp
    	if nil==cell then
    		cell=cc.TableViewCell:create()
    	end

    	cell:removeAllChildren()
    	if betResId~=nil then
    		local recordBg=display.newSprite("recordAniBg.png")
    			:setAnchorPoint(cc.p(0,0))
        		:setPosition(cc.p(0, 0))
        		:addTo(cell)
  
    		sp=cc.Sprite:create("betresrecord.png",cc.rect(posIndex[1+betResId]*w,0,w,h))
        	sp:setAnchorPoint(cc.p(0,0))
        	sp:setPosition(0,5)
        	sp:addTo(cell)

        	local sz=recordBg:getContentSize()
        	if idx==0  then
	    		local pNode=cc.Sprite:create()
	    					:addTo(recordBg)
	    					:setPosition(sz.width/2,sz.height/2)
	    		self:playLoopAnimation( pNode, param)
	    	end
    	end

    	return cell
	end

	if self.recordView and not tolua.isnull(self.recordView) then 
		print("----------self.recordView:reloadData()---------")

		print("------------recordTable----------")
		for i=1,#recordTable do
			print("i=",i,recordTable[i])
		end
		print("------------self.turnTableRecords----------")
		for i=1,#self.turnTableRecords do
			print("i=",i,self.turnTableRecords[i])
		end
		self.recordView:reloadData()
		
		--if self.bReset==true then assert(false) end
		return 
	end
	local tableView=cc.TableView:create(cc.size(50,300))
	tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(90,340))
    tableView:setDelegate()
    tableView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
    self:addChild(tableView)
    tableView:registerScriptHandler(numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(cellSizeForTable,cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex,cc.TABLECELL_SIZE_AT_INDEX)
    self.recordView=tableView
    tableView:reloadData()

end


function GameViewLayer:testShowRecords(recordTable)
	self:showRecords({1,2,3,4,5,6,7,8,9,10,11,12})
end

function GameViewLayer:showBetResult(resultKind,showtime,nAnimalMultiple)
		if resultKind~=  cmd.ANIMAL_SLIVER_SHARK then --银鲨鱼缺音效文件
	    	ExternalFun.playSoundEffect( "ANIMAL_SOUND_"..(resultKind)..".wav")
	    end

		local jsLayer=display.newLayer()
		self.jsLayer=jsLayer
		self.jsLayer:setName("jsLayer")

		jsLayer.onTouchBegan=function(self,touch,event) return true end
		ExternalFun.registerTouchEvent(jsLayer,true)
		self:addChild(jsLayer)
		
		display.newSprite("mask.png")
			:addTo(jsLayer)
			:setPosition(display.center)

		local sp
		if resultKind>=cmd.ANIMAL_SLIVER_SHARK then
			sp=display.newSprite("betresult"..resultKind..".png")
		else
			local w=300
			local h=176
			local posIndex={4,5,6,7,3,2,1,0}   --第i种动物在动物合集图中的位置索引
			local x=posIndex[1+resultKind]*w
			sp=cc.Sprite:create("anibetresult.png",cc.rect(x,0,w,h))  --从图中截取一个rect
	  		
	  		display.newSprite("jiesuanlight.png")
	  			:addTo(sp)
	  			:setScale(0.9)
	  			:setPosition(w/2,h/2)
	  	end
	  	sp:addTo(jsLayer)
	  	sp:setPosition(590,533)
	 
	  	
	  	local aniTextSp=display.newSprite("js"..resultKind..".png"):setAnchorPoint(0.5,0.5)
	  	local multiplyImg=display.newSprite("multiply.png"):setAnchorPoint(0,0.5)
	  	local ratio=self.m_nAnimalMultiple[resultKind+1]
	  	print("ratio: ",ratio)
	  	local ratioImg=cc.LabelAtlas:create(ratio,"jsdigits.png",58,68,string.byte("0"))
	  	ratioImg:setAnchorPoint(0,0.5)

	  	aniTextSp:addTo(jsLayer)
	  		:setPosition(500,410)
	  	local x=aniTextSp:getContentSize().width
	  	local y=aniTextSp:getContentSize().height/2
	  	multiplyImg:addTo(aniTextSp):setPosition(x,y)
	  	x=x+multiplyImg:getContentSize().width
	  	ratioImg:addTo(aniTextSp):setPosition(x,y)
	  	if resultKind>=cmd.ANIMAL_SLIVER_SHARK and nAnimalMultiple>0 then
	  		local plus=display.newSprite("plussign.png"):setAnchorPoint(0,0.5)
	  		local multipleImg=cc.LabelAtlas:create(nAnimalMultiple,"jsdigits.png",58,68,string.byte("0"))
	  		multipleImg:setAnchorPoint(0,0.5)
	  		local equal=display.newSprite("equalsign.png"):setAnchorPoint(0,0.5)
	  		local totalRatioImg=cc.LabelAtlas:create(ratio+nAnimalMultiple,"jsdigits.png",58,68,string.byte("0"))
	  		totalRatioImg:setAnchorPoint(0,0.5)

	  		x=x+ratioImg:getContentSize().width
	  		plus:addTo(aniTextSp):setPosition(x,y)
	  		x=x+plus:getContentSize().width
	  		print("multipleImg:getContentSize().width: ",multipleImg:getContentSize().width)
	  		multipleImg:addTo(aniTextSp):setPosition(x,y)
	  		x=x+multipleImg:getContentSize().width
	  		equal:addTo(aniTextSp):setPosition(x,y)
	  		x=x+equal:getContentSize().width
	  		totalRatioImg:addTo(aniTextSp):setPosition(x,y)
	  		aniTextSp:setPosition(370,410)
	  	end

	  	jsLayer:runAction(cc.Sequence:create(cc.DelayTime:create(showtime),cc.CallFunc:create(function() jsLayer:removeSelf() end) ) )
end

function GameViewLayer:OnUpdateUser(viewId, userItem, bLeave)
    local myViewId=self._scene:SwitchViewChairID(self._scene:GetMeChairID()) 
    if viewId==myViewId then
    	return 
    end
	if bLeave then
		self.m_tabPlayerList[viewId]=nil
		print(viewId.." leave")
	else
		if userItem then
			print("viewId", viewId)
			self.m_tabPlayerList[viewId]=userItem
		end
	end
end


--银行操作成功
function GameViewLayer:onBankSuccess( )
	if self._bankLayer and not tolua.isnull(self._bankLayer) then
		self._bankLayer:onBankSuccess()
	end
end

function GameViewLayer:onBankFailure( )
	if self._bankLayer and not tolua.isnull(self._bankLayer) then
		self._bankLayer:onBankFailure()
	end
end

function GameViewLayer:onGetBankInfo(bankinfo)
	if self._bankLayer and not tolua.isnull(self._bankLayer) then
		self._bankLayer:onGetBankInfo(bankinfo)
	end
end


return GameViewLayer