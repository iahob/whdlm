local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.animalbattle.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local ZhuanPanAni=appdf.req(module_pre .. ".views.ZhuanPanAni")
local MenuLayer=appdf.req(module_pre..".views.layer.MenuLayer")
local PlayerlistLayer=appdf.req(module_pre .. ".views.layer.PlayerlistLayer")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local UserListLayer = appdf.req(module_pre..".views.layer.UserListLayer")
local ChipLayer = appdf.req(module_pre..".views.layer.ChipLayer")
local SpineObj = appdf.req("client.src.external.SpineObj")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local BankerLayer = appdf.req(module_pre..".views.layer.BankerLayer")


local DEBUG=1
if DEBUG==1 then
	dbg_assert=assert
	dbg_print=print
else
	dbg_assert=function() end
	dbg_print=function() end
end

local GameViewLayer = class("GameViewLayer",function(scene)
        return display.newLayer()
end)

function GameViewLayer:ctor(scene)
	self._scene = scene

	self.m_tabPlayerList={}
	self.turnTableRecords={}

	self.noteNumBtns={}
	self.noteNumEffs={}
	self.betBtns={}

	self.csbNode=ExternalFun.loadCSB("MainScene.csb",self)

    self.m_ChipLayer = ChipLayer:create(self._scene)
    self:addChild(self.m_ChipLayer)

    local totalBetLayer = cc.Layer:create()
    self:addChild(totalBetLayer)

	self.animal_node = self.csbNode:getChildByName("aniPanel")
	for i=1,28 do
		local sp = self.animal_node:getChildByName("animal_"..i)
		local x , y = sp:getPosition()
		ZhuanPanAni.tabZhuanpanPos[i] = cc.p(x,y)--self:convertToWorldSpace(cc.p(x,y))
	end

	for i=1,5 do --5个赌注大小按钮
    	self.noteNumBtns[i]=appdf.getNodeByName(self.csbNode,"betnum"..i)
    	self.noteNumBtns[i].m_noteNum=math.pow(10,i+1)
    	self.noteNumBtns[i]:addClickEventListener(function(sender) 
    		ExternalFun.playSoundEffect("dianji.mp3")
    		self._scene:OnNoteSwitch(sender,i) 
    	end)

    	self.noteNumEffs[i]=appdf.getNodeByName(self.csbNode,"noteNumEffs_"..i)
    	self.noteNumEffs[i]:setVisible(false)
    end
    self.noteNumEffs[1]:setVisible(true)

    self.betBtnPoses={}
    self.lotteryEffects = {}
    self.totalBetTxt = {}
    self.myTotalBetTxt = {}

    self.bet_node = self.csbNode:getChildByName("betPanel")

    for i=1,11 do --11个动物下注按钮 --betBtns[i]对应
    	local btn= self.bet_node:getChildByName("Button_"..(i))
		btn.m_kind=i
		self.betBtns[i]=btn
		btn:addClickEventListener(function(sender) self._scene:OnPlaceJetton(sender) end)
		self.betBtnPoses[i]=cc.p( btn:getPosition() )

		print("i == ",i)
    	local sp = self.bet_node:getChildByName("sp_"..(i))
    	sp.m_kind=i
    	sp:setVisible(false)
    	self.lotteryEffects[i] = sp

    	local x,y = btn:getPosition()
    	local txt = ccui.Text:create("0","fonts/round_body.ttf",23)
		totalBetLayer:addChild(txt)
		txt:setAnchorPoint(0,1)
		txt:setPosition(cc.p(x - btn:getContentSize().width/2+5,y+btn:getContentSize().height/2-5))
		txt:setTextColor(cc.c4b(255,255,255,255))
		txt.m_kind=i
		txt:setVisible(false)
		self.totalBetTxt[i] = txt

    	local x,y = btn:getPosition()
    	local myTxt = ccui.Text:create("0","fonts/round_body.ttf",23)
		totalBetLayer:addChild(myTxt)
		myTxt:setAnchorPoint(0,1)
		myTxt:setPosition(cc.p(x - btn:getContentSize().width/2+5,y+btn:getContentSize().height/2-25))
		myTxt:setTextColor(cc.c4b(255,211,56,255))
		myTxt.m_kind=i
		-- txt:setString(123654)
		myTxt:setVisible(false)
		self.myTotalBetTxt[i] = myTxt
    end

    self.backBtn=appdf.getNodeByName(self.csbNode,"backbtn")
    self.setBtn=appdf.getNodeByName(self.csbNode,"setbtn")
    self.clearBtn=appdf.getNodeByName(self.csbNode,"clearbtn")
    self.continueBtn=appdf.getNodeByName(self.csbNode,"continuebtn")

    self.backBtn:addClickEventListener(function() 
    	ExternalFun.playSoundEffect("dianji.mp3")
    	self._scene:onKeyBack() 
    end)

    local x,y=self.setBtn:getPosition()
    self.setBtn:addClickEventListener(function() 
    	ExternalFun.playSoundEffect("dianji.mp3")
    	self:addChild(MenuLayer:create(self,x,y),100)
    end)
    self.clearBtn:addClickEventListener(function() 
    	ExternalFun.playSoundEffect("dianji.mp3")
    	self._scene:OnCleanJetton() 
    end)
    self.continueBtn:addClickEventListener(function() 
    	ExternalFun.playSoundEffect("dianji.mp3")
    	self._scene:OnLastPlaceJetton() 
    	self.continueBtn:setEnabled(false)
    end)

    self.timeTextImg=ccui.ImageView:create()

    local countBg=appdf.getNodeByName(self.csbNode,"countbg")
    self.timeTextImg:setPosition(100,30)
    countBg:addChild(self.timeTextImg)

    self.recordbg=appdf.getNodeByName(self.csbNode,"recordbg")
 
    self.storagebg=appdf.getNodeByName(self.csbNode,"storagebg")



    self.recordView=nil
	self:initTableRecord()
    self:initZhuanpan()

    self.storageLabelAtlas=cc.LabelAtlas:create("", "storage.png", 28, 38, string.byte("0")) 
    			:setAnchorPoint(0,0.5)
    			:move(613,612)
    			:addTo(self)
    			:setScaleX(0.91)


	local banker = self.csbNode:getChildByName("banker")
	self.totalscore = banker:getChildByName("totalscore")
	-- self.totalscore:setString("0")

	self.ZhuanPanView = self.csbNode:getChildByName("panPanel")



	local asset = self.csbNode:getChildByName("asset")
	self.my_name = asset:getChildByName("my_name")
	self.my_gold = asset:getChildByName("my_gold")
	local sp_head = asset:getChildByName("sp_head")

	local userItem = self._scene:GetMeUserItem()
	self.my_gold:setString(""..userItem.lScore)
	self.my_name:setString(""..userItem.szNickName)

    local av = PopupInfoHead:createClipHead( useritem, 92, "head_sp.png" )
    sp_head:addChild(av)
    av:setPosition(sp_head:getContentSize().width/2,sp_head:getContentSize().height/2)

    self.btn_other = self.csbNode:getChildByName("btn_other")
    self.btn_other:addClickEventListener(handler(self,self.onBtnOther))

    local num_other = self.csbNode:getChildByName("num_other")
    local nunCount = function()
    	local index = 0
	    for k,v in pairs(self._scene:getUserList()) do
	    	if v then
	    		index = index+1
	    	end
	    	if index > 99 then 
	    		break
	    	end
	    end
	    num_other:setString("."..index.."/")
    end
   	schedule(num_other, function() nunCount() end, 1)
	nunCount()

    self.tips_sp = self.csbNode:getChildByName("tips_sp")
    self.tips_time = self.csbNode:getChildByName("tips_time")
    self.tips_time_eff = self.csbNode:getChildByName("tips_time_eff")
    self.tips_time_eff:setVisible(false)

    local onBanker = self.csbNode:getChildByName("banker")
    self.onBankerSzBtn = onBanker:getChildByName("banker_sz")
    self.onBankerSzBtn:addClickEventListener(function() self:onSendBanker(true) end)

    self.onBankerXzBtn = onBanker:getChildByName("banker_xz")
    self.onBankerXzBtn:addClickEventListener(function() self:onSendBanker(false) end)

    self.onBankerSzBtn:setVisible(true)
	self.onBankerXzBtn:setVisible(false)

    self.bankerName = onBanker:getChildByName("name")
    self.bankerGold = onBanker:getChildByName("AtlasLabel_1")
    self.bankerCount = onBanker:getChildByName("Text_1")
    self.bankerCount:setString("0")

	self:initTotalScore(0)
	self:updateCurrentScore(0)


	local wifi_tab = {}
	local node_platform = self.csbNode:getChildByName("node_platform")
	wifi_tab[1] = node_platform:getChildByName("Sprite_1")
	wifi_tab[2] = node_platform:getChildByName("Sprite_2")
	wifi_tab[3] = node_platform:getChildByName("Sprite_3")
	local time_txt = node_platform:getChildByName("time_txt")
	local loadingBar = node_platform:getChildByName("LoadingBar_1")

	local updaPlatform = function ()
		-- body
		local state = MultiPlatform:getInstance():getNetworkStrength()
          	for i=1, 3 do
                if i <= state then
                    wifi_tab[i]:setVisible(true)
                else
                    wifi_tab[i]:setVisible(false)
                end
          	end
		time_txt:setString(os.date("%H:%M",os.time()))

		local bCount1 , bCount2 = math.modf(MultiPlatform:getInstance():getBatteryLevel())
		loadingBar:setPercent(bCount1)

	end
	updaPlatform()
	schedule(node_platform,updaPlatform,1)
	--self.bAllowOpeningAni=false

   --self.storageLabelAtlas:setVisible(false)
   --	self:testPlayerlistLayer()
   --self:testRecord()
   -- self.testShowJieSuan=nil
   -- if self.testShowJieSuan==1 then
  	-- 	self:showJieSuanView(8,10,155)
   -- end

   --注册node事件
   ExternalFun.registerNodeEvent(self)

end

function GameViewLayer:testPlayerlistLayer()
	local playerlist={}
	for i=1,3 do 
		playerlist[i]={}
		playerlist[i].szNickName="玩家"..i.."号"
		playerlist[i].wFaceID=100+i
		playerlist[i].lScore=i*i*i*i+10000
	end
	self:addChild(PlayerlistLayer:create(playerlist),10000)
end

function GameViewLayer:testRecord()
	-- self.turnTableRecords={0,1,2,3,4,5,6,7,8,9}--,10,11,0,1,2,3,4,5,6,7,8,9,10,11,5,6,7}
	local i=0
	while i<69 do
		i=i+1
		self:AddTurnTableRecord(i%12)
		self:updateShowTurnTableRecord()
	end
end

function GameViewLayer:getPlayerList()
	--return self.m_tabPlayerList
	return self._scene:getPlayerList()
end

function GameViewLayer:AddTurnTableRecord(betResultId)

	local len=#self.turnTableRecords
	if len >= cmd.RECORD_COUNT_MAX then   --10可以换成任意大于1的数字
		for i=1,cmd.RECORD_COUNT_MAX-1 do	                    --删除old记录，只保留最近的cmd.RECORD_COUNT_MAX-1个
			self.turnTableRecords[i]=self.turnTableRecords[i+1]
		end
		-- for i=cmd.RECORD_COUNT_MAX,len do
		self.turnTableRecords[cmd.RECORD_COUNT_MAX]=betResultId
		-- end
	else
		table.insert(self.turnTableRecords,betResultId)
	end
	-- table.insert(self.turnTableRecords,betResultId)
end

function GameViewLayer:initTableRecord()

	local tableView=cc.TableView:create(cc.size(108,500))
	tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(cc.p(0,7))
    tableView:setDelegate()
    
    appdf.getNodeByName(self.csbNode,"recordbg"):addChild(tableView)

    tableView:registerScriptHandler(handler(self,self.numberOfCellsInTableView),cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    tableView:registerScriptHandler(handler(self,self.cellSizeForTable),cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(handler(self,self.tableCellAtIndex),cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(handler(self,self.scrollViewDidScroll),cc.SCROLLVIEW_SCRIPT_SCROLL)
    self.recordView=tableView
    tableView:reloadData()
    if #self.turnTableRecords>12 then
    	tableView:setContentOffset(cc.p(0,0))
    end
end

function GameViewLayer:scrollViewDidScroll()

end

function GameViewLayer:numberOfCellsInTableView()
	return #self.turnTableRecords
end

function GameViewLayer:cellSizeForTable()
	local cellW =108
	local cellH = 52
    return cellW,cellH
end

function GameViewLayer:tableCellAtIndex(view,idx)
	local cellW =108
	local cellH = 52

    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    -- self.turnTableRecords = {0,1,2,3,4,5,6,7,8,9,10,11}

	local betResId=self.turnTableRecords[#self.turnTableRecords-idx]
	local posIndex={4,0,1,2,6,7,5,3,10,8,9,11}--第i种动物在记录合集图中的位置序号
	local w,h=68,52

	if idx == 0 then  
		-- 最后个为最新的
		cc.Sprite:create("animal_table_box.png")
		:setAnchorPoint(cc.p(0.5,0.5))
		:setPosition(cellW/2,cellH/2)
		:addTo(cell)	
	end


	cc.Sprite:create("records/records_"..betResId..".png")
		:setAnchorPoint(cc.p(0.5,0.5))
		:setPosition(cellW/2,cellH/2)
		:addTo(cell)
		-- :setScale(0.6)
	

	return cell
end

function GameViewLayer:updateShowTurnTableRecord()
    self.recordView:reloadData()
    if #self.turnTableRecords>10 then
    	self.recordView:setContentOffset(cc.p(0,0))
    end
end

function GameViewLayer:initZhuanpan()
	-- for i=1,28 do
	-- 	local kind=ZhuanPanAni.zhuanpanPosToKind(i)
	-- 	local kindToIndex={8,4,7,3,6,5,2,0,1,11,10,9} --在动物合集图上的位置索引
	-- 	local circle=cc.Sprite:create("di.png")
	-- 	circle:setPosition(ZhuanPanAni.tabZhuanpanPos[i])
	-- 	local ani=cc.Sprite:create("aniatlas2.png",cc.rect(kindToIndex[kind+1]*84,0,84,75))
	-- 	ani:setPosition(circle:getContentSize().width/2,circle:getContentSize().height/2)
	-- 	self:addChild(circle)
	-- 	circle:addChild(ani)
	-- end
end

function GameViewLayer:brightRectBlink(showtime,pos)
	local sp = self.lotteryEffects[pos+1]
	sp:setVisible(true)
	sp.m_bVisible=true
	sp:runAction(cc.Sequence:create(
			cc.Blink:create(showtime,math.ceil(showtime)),
			cc.CallFunc:create(function() 
				sp:setVisible(false) 
				sp.m_bVisible=false
				if self.zhuanPanAni and not tolua.isnull(self.zhuanPanAni) then
					self.zhuanPanAni:removeSelf()
				end  
			end)
		)
	)
end

function  GameViewLayer:showJieSuanView(resultKind,showtime,shaYuAddMulti,curGameScore,lOtherUserScore)
	print("resultKind: ",resultKind)
	if resultKind<0 or resultKind>11 or showtime<=0 then return end
	if resultKind~=  cmd.JS_YIN_SHA then 
  		ExternalFun.playSoundEffect( "ANIMAL_SOUND_"..(resultKind)..".mp3")
  	end


	self.jsLayer=display.newLayer(cc.c4b(60,60,67,100))
	self.jsLayer:addTo(self)
	self.jsLayer:setVisible(false)

	if resultKind==cmd.JS_TONG_SHA or resultKind==cmd.JS_TONG_PEI then
		local scaleAct=cc.ScaleBy:create(showtime/3,3)
		local rotateAct=cc.RotateBy:create(showtime/6,20)

		local sp=cc.Sprite:create("js"..resultKind..".png")
		:addTo(self.jsLayer)
		:setPosition(display.center)
		
		if resultKind==cmd.JS_TONG_SHA then
			sp:runAction(cc.Sequence:create(rotateAct,rotateAct:reverse(),scaleAct,scaleAct:reverse() ))
		end

		self.jsLayer:runAction(
			cc.Sequence:create(
				cc.DelayTime:create(showtime),
				cc.CallFunc:create(function() 
					self.jsLayer:removeSelf() 
					self.jsLayer = nil
					self:flyGameEndGold(resultKind,curGameScore,lOtherUserScore)
				end)
			)
		)
	else
		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5),cc.CallFunc:create(function ()
			self.jsLayer:setVisible(true)
			local animation = {"yanzi","gezi","kongque","ying","shizi","xiongmao","houzi","tuzi","shayu","tongpei","tongsha","jinsha"}
			local skeletonNode = sp.SkeletonAnimation:create("skeleton/get/skeleton.json", "skeleton/get/skeleton.atlas")
			skeletonNode:setAnimation(0, "animation",false)
			skeletonNode:setPosition(667,375)
			self.jsLayer:addChild(skeletonNode)
			skeletonNode:setSkin(animation[resultKind+1])
			skeletonNode:registerSpineEventHandler(function (event)
				self.jsLayer:runAction(cc.Sequence:create(cc.DelayTime:create(0.01),cc.CallFunc:create(function()
					self.jsLayer:removeSelf() 
					self.jsLayer = nil
					self:flyGameEndGold(resultKind,curGameScore,lOtherUserScore)  

					if resultKind==cmd.JS_JIN_SHA or resultKind==cmd.JS_YIN_SHA then
						self:brightRectBlink(showtime,10) -- 鲨鱼
					else
						self:brightRectBlink(showtime,resultKind)
						if resultKind<=3 then 
							self:brightRectBlink(showtime,8) -- 飞禽
						else
							self:brightRectBlink(showtime,9) -- 走兽
						end
					end

				end)))
		    end, 2)
		end)))


	end
end

function GameViewLayer:enableBetBtns(bEnable) 
	for i=1,11 do
		self.betBtns[i]:setEnabled(bEnable)
	end
end

function GameViewLayer:enableAllBtns(bEnable)
	for i=1,5 do
		self.noteNumBtns[i]:setEnabled(bEnable)
	end
	for i=1,11 do
		self.betBtns[i]:setEnabled(bEnable)
	end
	self.clearBtn:setEnabled(bEnable)
	self.continueBtn:setEnabled(bEnable)


	if self.bankerUserId == self._scene:GetMeUserItem().dwUserID then 
		for i=1,5 do
			self.noteNumBtns[i]:setEnabled(false)
		end
		self.clearBtn:setEnabled(false)
		self.continueBtn:setEnabled(false)
	end
end

function GameViewLayer:enable_NoteNum_Clear_ContinueBtn(bEnable)
	for i=1,5 do
		self.noteNumBtns[i]:setEnabled(bEnable)
	end
	self.clearBtn:setEnabled(bEnable)
	self.continueBtn:setEnabled(bEnable)

	if self.bankerUserId == self._scene:GetMeUserItem().dwUserID then 
		for i=1,5 do
			self.noteNumBtns[i]:setEnabled(false)
		end
		self.clearBtn:setEnabled(false)
		self.continueBtn:setEnabled(false)
	end

end

function GameViewLayer:disableNoteNumBtns(startIndex)
	for i=startIndex,5 do
		self.noteNumBtns[i]:setEnabled(false)
	end

	if startIndex <= 1 then 
		for k,v in pairs(self.noteNumEffs) do
			v:setVisible(false)
		end
		self._scene.m_nCurrentNote = nil
	end

	if self.bankerUserId == self._scene:GetMeUserItem().dwUserID then 
		for i=1,5 do
			self.noteNumBtns[i]:setEnabled(false)
		end
		self.clearBtn:setEnabled(false)
		self.continueBtn:setEnabled(false)
	end
end

function GameViewLayer:enableNoteNumBtns(endIndex)
	for i=1,endIndex do
		self.noteNumBtns[i]:setEnabled(true)
	end

	if self.bankerUserId == self._scene:GetMeUserItem().dwUserID then 
		for i=1,5 do
			self.noteNumBtns[i]:setEnabled(false)
		end
		self.clearBtn:setEnabled(false)
		self.continueBtn:setEnabled(false)
	end

end

local leftAligned=0
local centerAligned=1
local rightAligned=2

local storageDigitKind=1
local countDigitKind=2
local totalbetDigitKind=3
local mybetDigitKind=4
local totalscoreDigitKind=5
local assetDigitKind=6
local curscoreDigitKind=7
local digitSpriteConfig={ --数字图片配置  --彩金池靠右对齐，其余居中对齐  --dis表示数字间间距
	{name="storagenum",filepath="storage.png",w=50,h=40 ,dis=5,align=leftAligned}, --彩金池 --pos为中心距离
	{name="countnum",filepath="countnum.png",w=31,h=40,dis=5,align=centerAligned}, --倒计时 --以下pos为最右距离
	{name="totalbetnum",filepath="allbetnum.png",w=12,h=16,dis=0,align=centerAligned},  --总下注
	{name="mybetnum",filepath="mebetnum.png",w=12,h=16,dis=0,align=centerAligned},  --自己下注
	{name="scorenum",filepath="score.png",w=17,h=20,dis=0,align=centerAligned},   --总得分 ,可能为负数
	{name="assetnum",filepath="score.png",w=17,h=20,dis=0,align=centerAligned},	--玩家资产
	{name="curscorenum",filepath="score.png",w=17,h=20,dis=0,align=centerAligned},
}

function GameViewLayer:updateNumberPic(kind,bg,pos,number)--左对齐则pos为左边界位置，中心对齐则pos为中心位置
	dbg_assert(bg)
	dbg_assert(not tolua.isnull(bg))
	-- print("updateNumberPic kind: ",kind)

	local numbersNode=bg:getChildByName(digitSpriteConfig[kind].name)
	if numbersNode and not tolua.isnull(numbersNode) then
		numbersNode:removeSelf()
	end

	if nil==number then return end
	
	local function getDigits(number)
		dbg_assert(number)
		local sign=number>=0 and 1 or -1
		if number<0 then number=-number end
		local digits={}
		if number==0 then
			digits[1]=0
			return digits
		end
		while number~=0 do
			local residue=number%10
			number=math.floor(number/10)
			table.insert(digits,residue)
		end
		if sign<0 then table.insert(digits,'-') end  --else table.insert(digits,'+') 
		return digits
	end

	local function newDigitSp(filepath,digit,w,h) --digit单个数字0-9
		if digit=='+' then 
			digit=10 
		elseif digit=='-' then 
			digit=11 
		end
		return cc.Sprite:create( filepath,cc.rect(w*digit,0,w,h) )
	end

	local digits=getDigits(number) --将number的每个位上数字存入table
	dbg_assert(#digits>0)

	local dsc=digitSpriteConfig[kind]
	local node=cc.Node:create() --对于一个size为0的node，setAnchorPoint(,)会对其子节点的显示有影响吗?
	node:addTo(bg):setName(digitSpriteConfig[kind].name)
	
	-- print("number: ",number)
	for i=1,#digits do
		print(i,digits[i])
		local sp= newDigitSp(dsc.filepath,digits[i],dsc.w,dsc.h)
		-- print("digits[i]: ",digits[i])
		-- print("number: ",number)
		sp:addTo(node)
		sp:setAnchorPoint(0,0)
		sp:setPosition( (dsc.dis+dsc.w)*(#digits-i),0 )
	end

	local totalWidth= (#digits) * (dsc.dis+dsc.w)
	if dsc.align==centerAligned then
		node:setPosition(pos.x-totalWidth/2,pos.y)
	elseif dsc.align==rightAligned then
		node:setPosition( pos.x-totalWidth,pos.y)
	elseif dsc.align==leftAligned then
		node:setPosition(pos)
	end
end

function GameViewLayer:updateTotalBets(tabBets)

	for i=1,cmd.AREA_COUNT-1 do
		self:updateTotalBet(i,tabBets)
	end
end

function GameViewLayer:updateGamePlayerBets(tabBets,isMe)

	-- print(debug.traceback())
	dump(tabBets,"tabBets",12)
	for i=1,cmd.AREA_COUNT-1 do
		if tabBets[i]==0 then
			-- self:updateMyBet(i,nil)
		else
			local num = tabBets[i]
			local chipArray = self._scene:getChipArray()
			local count = #chipArray[1]
			while(count > 0)
			do
				if num >= chipArray[1][count] and num > 0 then
					num = num - chipArray[1][count]
					self:updateGamePlayerBet(i,chipArray[1][count],isMe)
				else
					count = count - 1
				end
			end
		end
	end
end

function GameViewLayer:updateTotalBet(kind,tabBets)
	local allbetnum = 0
	for k,v in pairs(self.totalBetTxt) do
		if v.m_kind == kind then 
			if tabBets[kind] <= 0 then 
				v:setVisible(false)
			else
				v:setVisible(true)
			end
			v:setString(tabBets[kind])
		end 
	end

	for k,v in pairs(tabBets) do
		allbetnum = allbetnum + v
	end

	self.totalscore:setString(allbetnum)

end

function GameViewLayer:updateMyBets(tabBets)

	for i=1,cmd.AREA_COUNT-1 do
		self:updateMyTotalBet(i,tabBets)
	end


end

function GameViewLayer:updateMyTotalBet(kind,tabBets)
	local allbetnum = 0
	for k,v in pairs(self.myTotalBetTxt) do
		if v.m_kind == kind then 
			if tabBets[kind] <= 0 then 
				v:setVisible(false)
			else
				v:setVisible(true)
			end
			v:setString(tabBets[kind])
		end 
	end
end


function GameViewLayer:updateGamePlayerBet(kind,num,isMe)
	-- print(debug.traceback())

	local pos = cc.p(62,62)
	if not isMe then
		pos = cc.p(34,439)
	end

	-- if isMe then
	-- 	self:updateMyTotalBet(kind,num)
	-- end

	if isMe and not self._scene.m_myIsBet then 
		self._scene.m_myIsBet = true
	end

	if not isMe and not self._scene.m_otherIsBet then 
		self._scene.m_otherIsBet = true
	end

	local num = num
	local chipArray = self._scene:getChipArray()
	local count = #chipArray[1]
	while(count > 0)
	do
		if num >= chipArray[1][count] and num > 0 then
			local bg=self.betBtns[kind]
			local x , y = bg:getPosition()
			x = x + math.random(-(bg:getContentSize().width/2-20), bg:getContentSize().width/2-20)
			y = y + math.random(-(bg:getContentSize().height/2-20), bg:getContentSize().height/2-20) 

			num = num - chipArray[1][count]
			self.m_ChipLayer:flyChipAnimation(kind,pos,cc.p(x,y),chipArray[1][count],isMe)
		else
			count = count - 1
		end
	end

	
end

function GameViewLayer:flyGameEndGold(resultKind,curGameScore,lOtherUserScore)
	print("flyGameEndGold~~~~~~~~~~~~~~~~~~~")
	self:updateBankerMoney()
	self.m_ChipLayer:flyGameEndGold(resultKind,curGameScore,lOtherUserScore)
end

function GameViewLayer:updateBankerMoney()
	-- if self._scene.bEnableSysBanker then -- 没有庄家
	-- 	
	-- end
	for k,v in pairs(self._scene:getUserList()) do
		if v.dwUserID == self.bankerUserId then
			self.bankerGold:setString(v.lScore)
			break
		end
	end

end

function GameViewLayer:removeChip()
	-- body
	self.m_ChipLayer:removeAllChip()
end

-- function GameViewLayer:updateOtherBet( kind,num )
-- 	local bg=self.betBtns[kind]
-- 	local x , y = bg:getPosition()
-- 	x = x + math.random(-(bg:getContentSize().width/2-20), bg:getContentSize().width/2-20)
-- 	y = y + math.random(-(bg:getContentSize().height/2-20), bg:getContentSize().height/2-20) 
-- 	self.m_ChipLayer:flyChipAnimation(cc.p(62,62),cc.p(x,y),num,true)
-- end

function GameViewLayer:playBackgroundMusic()


end

function GameViewLayer:updateCurrentScore(score)
	-- self:updateNumberPic( curscoreDigitKind,appdf.getNodeByName(self.csbNode,"curscorebg"),cc.p(185,17),score )
end

function GameViewLayer:initTotalScore()
	self.totalscore:setString("0")
end

function GameViewLayer:updateAsset(assetNum)
	-- self:updateNumberPic( assetDigitKind,appdf.getNodeByName(self.csbNode,"asset"),cc.p(185,17),assetNum )
	if assetNum then 
		self.my_gold:setString(""..assetNum)
	end
end

function GameViewLayer:updateCountDown(cbGameStatus,clockTime)

	local function updataColock( clockTime )

		if clockTime == 3 then 
			if cbGameStatus == cmd.GS_PLACE_JETTON then
	            local spine = SpineObj:create("skeleton/stopBet/skeleton.json", "skeleton/stopBet/skeleton.atlas", false)
				spine:setAnimation(0, "animation", false)
				spine:registerSpineEventHandler(function (event) spine:destroy() end, 2)
				spine:setPosition(667,375)
				spine:setName("spine_stop")
				self:addChild(spine)
             end
		end

		if clockTime <= 3 and cbGameStatus == cmd.GS_PLACE_JETTON  then 
			ExternalFun.playSoundEffect("daojishi.mp3")
		end



		self.tips_time:setString(string.format("%02d",clockTime))

		local actionTo1 = cc.ScaleTo:create(0.2,1.1)
		local actionTo2 = cc.ScaleTo:create(0.2,1.0)
		self.tips_time:runAction(cc.Sequence:create({actionTo1,actionTo2}))

		self.tips_time_eff:setVisible(true)
		self.tips_time_eff:setString(string.format("%02d",clockTime))
		self.tips_time_eff:stopAllActions()
		self.tips_time_eff:setScale(1)
		self.tips_time_eff:setOpacity(255)
		local actionTo3 = cc.ScaleTo:create(0.8,1.5)
		local actionTo4 = cc.FadeOut:create(0.8)
		self.tips_time_eff:runAction(cc.Sequence:create({cc.Spawn:create({actionTo3,actionTo4}),cc.CallFunc:create(function ()
			self.tips_time_eff:setVisible(false)
		end)}))
	end

	updataColock(clockTime)

	self.tips_sp:setVisible(true)
	self.tips_sp:stopAllActions()

	if cbGameStatus == cmd.GS_PLACE_JETTON then
		self.tips_sp:setTexture("tips_xz.png")
	elseif cbGameStatus == cmd.GS_GAME_END then 
		self.tips_sp:setTexture("tips_kj.png")
	else
		-- self.tips_sp:setVisible(false)
		self.tips_sp:setTexture("tips_kx.png")
	end

	schedule(self.tips_sp, function ()
		clockTime = clockTime - 1
		if clockTime > 0 then
			updataColock(clockTime)
		end 
	end, 1)

end

function GameViewLayer:updateStorage(num) --彩金池
	--self:updateNumberPic( storageDigitKind,appdf.getNodeByName(self.csbNode,"storagebg"),cc.p(100,10),num)
    self.storageLabelAtlas:setString(num)
end

function GameViewLayer:enableBtns(bEnable)
	for i=1,5 do
		self.noteNumBtns[i]:setEnabled(bEnable)
	end
	self.clearBtn:setEnabled(bEnable)
	self.continueBtn:setEnabled(bEnable)
end

function GameViewLayer:SetGameStatus(gameStatus) --设置游戏状态
	self.gameStatus=gameStatus
	if gameStatus == cmd.GAME_STATUS_FREE and self._bankLayer and not tolua.isnull(self._bankLayer) and (self._scene.m_nBankerUserID == self._scene:GetMeUserItem().wChairID) then
        showToast(self._bankLayer,"现在是空闲时间，可以进行银行操作",1)
    end
end

function GameViewLayer:updateTimeTextImg() --休闲时间、下注时间、开奖时间 图片

	-- if self.gameStatus==cmd.GAME_STATUS_FREE then
	-- 	self.timeTextImg:loadTexture("xiuxianshijian.png",0)   --LOCAL = 0,PLIST = 1
	-- elseif self.gameStatus==cmd.GS_PLACE_JETTON then
	-- 	self.timeTextImg:loadTexture("xiazhushijian.png",0)
	-- elseif self.gameStatus==cmd.GS_GAME_END	then
	-- 	self.timeTextImg:loadTexture("kaijiangshijian.png",0)
	-- else 
	-- 	dbg_assert(false)
	-- end
end

function GameViewLayer:removeFirstOpeningAni( )
	if self.firstOpeningAni and not tolua.isnull(self.firstOpeningAni) then
		self.firstOpeningAni:removeSelf()
		self.firstOpeningAni=nil
	end
end

local function printn(n,...)
	local i=0
	while i<n do
		i=i+1
		print(...)
	end
end

function GameViewLayer:GameOver( nTurnTableTarget, curGameScore,cumulativeScore ,shaYuAddMulti,lOtherUserScore) --转盘结束后更新记录 self.tabRecords
 	self.nTurnTableTarget=nTurnTableTarget
 	self.resultKind1= ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[1])
 	self.shaYuAddMulti=shaYuAddMulti
 --bTurnTwoTime为1时，开奖时间为30秒	                   
 	local totalSec=20-3
	self.bTurnTwoTime=0
	if nTurnTableTarget[2]>=1 and nTurnTableTarget[2]<=28  then self.bTurnTwoTime=1 end
	local deltaT=2 --连续两次开奖动画间隔时间
	local dur=12 --转盘时间
	local durations={dur,0}
	if self.bTurnTwoTime==1 then
		totalSec=(30-2-deltaT)/2  --=13
		durations[1]=dur-2
		durations[2]=dur-2
	end

	local function showAnims(self,i)
		if self.gameStatus~=cmd.GS_GAME_END then return end
		self:AddTurnTableRecord(ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[i]))

		local zhuanPanAni=ZhuanPanAni:create(self.ZhuanPanView,1,nTurnTableTarget[i],durations[i],totalSec)
		self.zhuanPanAni=zhuanPanAni
		local resultKind=ZhuanPanAni.zhuanpanPosToKind(nTurnTableTarget[i])
		local function callback(resttime)
			if self.gameStatus~=cmd.GS_GAME_END then return end
			self:showJieSuanView(resultKind,resttime,shaYuAddMulti,curGameScore,lOtherUserScore)
			self:updateShowTurnTableRecord()
			self:updateCurrentScore(curGameScore)
			-- dump(self._scene:GetMeUserItem())
			self:updateAsset(self._scene:GetMeUserItem().lScore+curGameScore)
		end
		zhuanPanAni:ZhuanPan(callback)
	end
		
	-- 做下延迟等待停止下注动画放完
	self:runAction(cc.Sequence:create(cc.DelayTime:create(1.6),cc.CallFunc:create(function ()
		showAnims(self,1)
	end)))
	
	
	-- if bTurnTwoTime==1 then  --放到updataClockView了，后台切换
	-- 	self:runAction(
	-- 		cc.Sequence:create(cc.DelayTime:create(totalSec+deltaT),
	-- 			cc.CallFunc:create(function() showAnims(self,2) end))
	-- 		)
	-- end
	self.showAnims=showAnims
end

function GameViewLayer:onExit()
	
	for i=8,11 do
		cc.Director:getInstance():getTextureCache():removeTextureForKey("js"..i..".png")
	end
	cc.Director:getInstance():getTextureCache():removeTextureForKey("background.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
    cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()

    --播放大厅背景音乐
    ExternalFun.playPlazzBackgroudAudio()

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

function GameViewLayer:updateChipArray(lChipArray)
	-- body
	-- dump(lChipArray,"lChipArray",12)
	local function formatScore( score )
		local scorestr = ExternalFun.formatScore(score)
		if score < 10000 then
			return scorestr,0
		end

		if score < 100000000 then
			scorestr = string.format("%d", score / 10000)
			return scorestr,1
		end

		scorestr = string.format("%d", score / 100000000)
		return scorestr,2
	end

	for i=1,#self.noteNumBtns do
		local scale = 0.8
		local btn = self.noteNumBtns[i]
		local scorestr,fontStr = formatScore(lChipArray[1][i])
		local layer = ccui.Layout:create()
		layer:setAnchorPoint(0.5,0.5)
		btn:addChild(layer)
		btn.m_noteNum = lChipArray[1][i]

		local txt = ccui.TextAtlas:create(scorestr,"button/chip_num.png",18,27,"0")
		layer:addChild(txt)
		txt:setScale(scale)
		if fontStr == 1 then 
			local sp = cc.Sprite:create("button/chip_wan.png")
			layer:addChild(sp)
			sp:setScale(scale)
			layer:setContentSize(txt:getContentSize().width*scale+sp:getContentSize().width*scale,28)
			txt:setPosition(txt:getContentSize().width*scale/2,14)
			sp:setPosition(txt:getContentSize().width*scale+sp:getContentSize().width*scale/2,14)
			
			layer:setPosition(50,58)

		elseif fontStr == 2 then
			local sp = cc.Sprite:create("button/chip_yi.png")
			layer:addChild(sp)
			sp:setScale(scale)
			layer:setContentSize(txt:getContentSize().width*scale+sp:getContentSize().width*scale,28)
			txt:setPosition(txt:getContentSize().width*scale/2,14)
			sp:setPosition(txt:getContentSize().width*scale+sp:getContentSize().width*scale/2,14)

			layer:setPosition(50,58)
		elseif fontStr == 0 then
			layer:setContentSize(txt:getContentSize().width,28)
			txt:setPosition(txt:getContentSize().width/2,14)
			layer:setPosition(47,58)	
		end

		-- 
		-- local txt = btn:getChildByName("txt")
		-- if txt then 

		-- 	txt:setString(formatScore(lChipArray[1][i]))
		-- end
	end
end


--玩家列表
function GameViewLayer:showUserList()
    local userList = self._scene:getUserList()
    -- dump(userList,"dddddddddddddddddddddddddddddddddddddddddd",12)
    if next(userList) == nil then  
    	showToast(self, "目前没有玩家", 1) 
    	return
    end
    local tab = {}
    for k,v in pairs(userList) do
    	if v then
    		tab[#tab+1] = v
    	end
    end
    --显示玩家列表信息
    -- if self.userListLayer then
    --   	self.userListLayer:hideLayer()
    -- end
    UserListLayer:create(tab):addTo(self._scene)
    -- self.userListLayer:set

end

function GameViewLayer:onBtnOther()
	-- body
	self:showUserList()
end

function GameViewLayer:OnUpdataClockView(clockViewChair,clockTime)

	-- local t=os.time()

	-- self.cbTimeLive=clockTime
	-- -- self:updateCountDown(clockTime)
	-- if self.gameStatus==cmd.GS_PLACE_JETTON and clockTime<5 then
	-- 	ExternalFun.playSoundEffect( "TIME_WARIMG.wav")
	-- end

	-- if (self.gameStatus~=cmd.GS_GAME_END or (clockTime>0 and clockTime<=3)) then
	-- 	if self.jsLayer and not tolua.isnull(self.jsLayer) then
	-- 		if self.testShowJieSuan~=1 then self.jsLayer:removeSelf() print("self.jsLayer:removeSelf()") end
	-- 	end

	-- 	if self.zhuanPanAni and not tolua.isnull(self.zhuanPanAni) then
	-- 		self.zhuanPanAni:removeSelf()
	-- 		--printn(100000,"clockTime: ",clockTime,"  gameStatus: ",self.gameStatus)
	-- 	end 
	-- 	for i=1,#self.lotteryEffects do
	-- 		if true==self.lotteryEffects[i].m_bVisible then
	-- 			--printn(100,i.." clockTime: ",clockTime,"  gameStatus: ",self.gameStatus)
	-- 			self.lotteryEffects[i]:stopAllActions()
	-- 			self.lotteryEffects[i]:setVisible(false)
	-- 			self.lotteryEffects[i].m_bVisible=false
	-- 		end
	-- 	end
	-- end

	-- local dt
	-- dt=self.lastupdataT==nil and 0 or t-self.lastupdataT
	-- if dt<=1 
	--   or (self._lastStatus==cmd.GS_PLACE_JETTON and self.gameStatus==cmd.GS_PLACE_JETTON and dt<self._lastTimeLive) 
	--   or (self._lastStatus==cmd.GS_GAME_END and self.gameStatus==cmd.GS_GAME_END and dt<self._lastTimeLive)
	-- then
	-- 	--donothing
	-- else
	-- 	print("dt: ",dt)
	-- 	self._scene:clearBets()
	-- end

	-- if self._lastStatus==cmd.GS_GAME_END  and dt>=self._lastTimeLive then --and self._lastTimeLive>0
	-- 	self:updateCurrentScore(0)
	-- end

	-- if self.bTurnTwoTime==1 and self._lastStatus==cmd.GS_GAME_END and self.gameStatus==cmd.GS_GAME_END and self._lastTimeLive-dt>3 and self._lastTimeLive-dt<13 then
	-- 	self.bTurnTwoTime=0
	-- 	self.showAnims(self,2)
	-- end

	-- self.lastupdataT=t
	-- self._lastStatus=self.gameStatus
	-- self._lastTimeLive=clockTime
end

function GameViewLayer:onSendBanker(pBool)
	-- body
	-- self._scene:sendApplyBanker(pBool)

	BankerLayer:create(self._scene):addTo(self._scene):setName("BankerLayer")

end


function GameViewLayer:updateApplyBanker( useritem ,wChairID)

	local layer = self._scene:getChildByName("BankerLayer")
	if useritem == nil and wChairID == 65535 then -- 系统坐庄时
		if layer then layer:updateApplyBanker( useritem ,wChairID) end
		return
	-- elseif useritem and wChairID == 65535 then
	-- 	if layer then layer:updateApplyBanker( useritem ,wChairID) end
	-- 	return
	end

	if not self._scene.bEnableSysBanker and wChairID == 65535 then -- 没有庄家
		self.bankerName:setString("")
		self.bankerGold:setString("")

	    self.onBankerSzBtn:setVisible(true)
		self.onBankerXzBtn:setVisible(false)
	end


	self.bankerName:setString(useritem.szNickName)
	self.bankerGold:setString(useritem.lScore)
	self.bankerUserId = useritem.dwUserID
	if useritem.dwUserID == self._scene:GetMeUserItem().dwUserID  then 
    	self.onBankerSzBtn:setVisible(false)
		self.onBankerXzBtn:setVisible(true)	
		self:updateXzBtnRes("banker/x_banker_1.png","banker/x_banker_2.png",true)
	else
	    self.onBankerSzBtn:setVisible(true)
		self.onBankerXzBtn:setVisible(false)
	end

	for k,v in pairs(self._scene.m_nApplyBanker) do
		if v == self._scene:GetMeChairID() then 
			self:updateXzBtnRes("banker/q_banker_1.png","banker/q_banker_2.png",true)
		end
	end

	self:enable_NoteNum_Clear_ContinueBtn()

	local layer = self._scene:getChildByName("BankerLayer")
	if layer then 
		layer:updateApplyBanker( useritem ,wChairID) 
	end

end

function GameViewLayer:updateXzBtnRes(res,normal,isShow)
    self.onBankerSzBtn:setVisible(not isShow)
	self.onBankerXzBtn:setVisible(isShow)

	self.onBankerXzBtn:loadTextures(res,0)
	self.onBankerXzBtn:loadTextureNormal(normal,0)
	self.onBankerXzBtn:loadTexturePressed(res,0)
end


function GameViewLayer:updateBankerCount()
	-- body
	self.bankerCount:setString(self._scene:getBankerCount())
	local layer = self._scene:getChildByName("BankerLayer")
	if layer then 
		layer:applyBankerReload()
	end
	
end

--断线重连
function GameViewLayer:updateGameBroken(cbGameStatus)
	self:removeGameWait()

	if self.jsLayer and not tolua.isnull(self.jsLayer) then
		 self.jsLayer:removeSelf()
	end

	if self.zhuanPanAni and not tolua.isnull(self.zhuanPanAni) then
		self.zhuanPanAni:removeSelf()
	end 

	for i=1,#self.lotteryEffects do
		if true==self.lotteryEffects[i].m_bVisible then
			self.lotteryEffects[i]:stopAllActions()
			self.lotteryEffects[i]:setVisible(false)
			self.lotteryEffects[i].m_bVisible=false
		end
	end

	if cbGameStatus == cmd.GS_GAME_END then 
		self.waitGameSpine = sp.SkeletonAnimation:create("skeleton/wait/skeleton.json", "skeleton/wait/skeleton.atlas")
		self.waitGameSpine:setAnimation(0, "animation",true)
		self.waitGameSpine:setPosition(667,375)
		self:addChild(self.waitGameSpine)
	end

	self:removeChip()
	self:removeChildByName("spine_stop")
end

function GameViewLayer:removeGameWait( )
	-- body
	if self.waitGameSpine and not tolua.isnull(self.waitGameSpine) then
		self.waitGameSpine:removeSelf()
	end 
end

function GameViewLayer:canUseBank()
	if self._scene.m_myIsBet == true then
        showToast(self._bankLayer, "已下注不可进行银行操作", 1)
        return false
	end
	if self.bankerUserId == self._scene:GetMeUserItem().dwUserID and self.gameStatus ~= cmd.GAME_STATUS_FREE then
        showToast(self._bankLayer, "非空闲时间段不可进行银行操作", 1)
        return false
	end
	return true
end

return GameViewLayer
