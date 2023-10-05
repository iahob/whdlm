--
-- Author: zhouweixiang
-- Date: 2017-02-14 19:11:49
--
--比赛房间列表
local MatchRoomListLayer = class("MatchRoomListLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cmd_match = appdf.req(appdf.CLIENT_SRC .. "plaza.models.match.header.CMD_Match")
local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")  
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText") 

local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--比赛定义
local DEFINE = cmd_match.define

local tag_back = 1		 --返回大厅
local tag_mineMatch = 2	 --我的比赛
local tag_signup = 3	 --报名比赛
local tag_cancell = 4    --取消比赛


local tag_exchange1 = 5
local tag_exchange2 = 9
local tag_progress = 6

local tag_historyLayer = 7

local tag_matchDetail = 8

-- 进入场景而且过渡动画结束时候触发。
function MatchRoomListLayer:onEnterTransitionFinish()
	 if self.m_ImmMatchTableView:isVisible() then self.m_ImmMatchTableView:reloadData() end
     if self.m_fixMatchTableView:isVisible() then self.m_fixMatchTableView:reloadData() end
end

-- 退出场景而且开始过渡动画时候触发。
function MatchRoomListLayer:onExitTransitionStart()
            if self._priTimeShd then
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._priTimeShd)
                self._priTimeShd = nil
            end
            self.m_ImmMatchTableView:release()
            self.m_fixMatchTableView:release()

end

function MatchRoomListLayer:ctor( scene )
	 FriendMgr:getInstance():sendMatchPlayerCount( )

	--注册node事件
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
    assert(self._scene)

    GlobalUserItem.bMatch = true
    self._curSignList = nil
    self._waitMatchEnterView = nil

    self._nWaitNumber = 0

    --GlobalUserItem.cbMatchType = -1

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("match/MatchRoomListLayer.csb", self)
    self._csbNode = csbNode

     csbNode:getChildByName("Image_4"):loadTexture("match/match_bg.png") --temp--

    --事件注册
    local touchFunc = function( sender,toucheEvent )
    	if toucheEvent == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)            
        end
    end

    --返回大厅
    local btn = csbNode:getChildByName("btn_back")
    btn:setTag(tag_back)
    btn:addTouchEventListener(touchFunc)

    btn = csbNode:getChildByName("btn_mineMatch")
    btn:setTag(tag_mineMatch)
    btn:addTouchEventListener(touchFunc)

      -- 充值背景
    btn = csbNode:getChildByName("plaza_image_diamondbg")
    btn:setTag(tag_exchange1)
    btn:addTouchEventListener(touchFunc)

    -- 兑换背景
    btn = csbNode:getChildByName("plaza_image_exchange")
    btn:setTag(tag_exchange2)
    btn:addTouchEventListener(touchFunc)

     -- 钻石
    local tmptxt = csbNode:getChildByName("txt_diamond")
    local lScore = GlobalUserItem.tabAccountInfo.lDiamond
    if lScore < 10000 then
        tmptxt:setString(lScore)
    elseif lScore >= 10000 and lScore < 100000000 then
        local szscore = string.format("%.2f", lScore / 10000)
        tmptxt:setString(szscore.."万")
    else
        local szscore = string.format("%.2f", lScore / 100000000)
        tmptxt:setString(szscore.."亿")
    end
    self.m_clipDiamond = tmptxt
  
    -- 钻石
    local tmptxt = csbNode:getChildByName("txt_score")
    local lScore = GlobalUserItem.tabAccountInfo.lUserScore
    if lScore < 10000 then
        tmptxt:setString(lScore)
    elseif lScore >= 10000 and lScore < 100000000 then
        local szscore = string.format("%.2f", lScore / 10000)
        tmptxt:setString(szscore.."万")
    else
        local szscore = string.format("%.2f", lScore / 100000000)
        tmptxt:setString(szscore.."亿")
    end
    self.m_clipScore = tmptxt



    self.m_tabMatchRoomList = MatchRoom:getInstance().m_tabMatchRoomList
    dump(self.m_tabMatchRoomList,"self.m_tabMatchRoomList",6)
  
    --房间列表
    self.roomListPanel = csbNode:getChildByName("Panel_table")

    --即时赛
    self.m_ImmMatchTableView=self:createMatchNodeTableView(MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE,self.roomListPanel)
    self.m_ImmMatchTableView:retain()

    --定时赛
    self.m_fixMatchTableView=self:createMatchNodeTableView( MatchRoom:getInstance().DEFINE.MATCH_TYPE_LOCKTIME,self.roomListPanel)
    self.m_fixMatchTableView:retain()

     local btn_immediate=csbNode:getChildByName("btn_immediate")
     local btn_fixtime=csbNode:getChildByName("btn_fixtime")

     --temp--
     btn_immediate:setVisible(true)
     btn_fixtime:setVisible(true)

     if self.m_ImmMatchTableView.number > 0 then
            btn_immediate:loadTextureNormal("match/match_btn_immediate2.png")
            btn_fixtime:loadTextureNormal("match/match_btn_fixtime1.png")

            self.m_ImmMatchTableView:addTo(self.roomListPanel)
    elseif self.m_fixMatchTableView.number > 0 then
             btn_immediate:loadTextureNormal("match/match_btn_immediate1.png")
             btn_fixtime:loadTextureNormal("match/match_btn_fixtime2.png")

             self.m_fixMatchTableView:addTo(self.roomListPanel)
      end


    btn_immediate:addClickEventListener(function() 
        self.roomListPanel:removeAllChildren()
        self.m_ImmMatchTableView:addTo(self.roomListPanel)
        self.m_ImmMatchTableView:setName("_roomList_")

        btn_immediate:loadTextureNormal("match/match_btn_immediate2.png")
        btn_fixtime:loadTextureNormal("match/match_btn_fixtime1.png")
    end)
    btn_fixtime:addClickEventListener(function()  
         
        self.roomListPanel:removeAllChildren()
        self.m_fixMatchTableView:addTo(self.roomListPanel)
        self.m_fixMatchTableView:setName("_roomList_")
         
        btn_immediate:loadTextureNormal("match/match_btn_immediate1.png")
        btn_fixtime:loadTextureNormal("match/match_btn_fixtime2.png")
    end)

   -- local localTime = os.date("*t",os.time())
        --dump(localTime,"================createMatchNodeTableView")

end


function MatchRoomListLayer:getTimeString(time)
    if type(time) ~= "number" then
        return "00/00"
    end
    local s = math.floor(time%60)
    local m = math.floor((time%3600 - s)/60)
    local h = math.floor((time%(3600*24) - m -s)/3600)
    if not m or not h then
        return "00/00"
    end

    local str1 = ""
    if s >= 0 then
        str1= s>=10 and "/"..s or "/0"..s
    end
    local str2 = ""
    if m>=0 then
        str2= m>=10 and ""..m or "0"..m
    end
    local str3 = ""
    if h>=0 then
        str3= h>=10 and ""..h or "0"..h
    end
    if h > 0 then
        return str3.."/"..str2
    else
        return str2..str1
    end

    return str3..str2
    
end

function MatchRoomListLayer:initLockTimeWaitLayer(Layer)
    local csbNode = Layer
    --比赛描述
    local list = self._curSignList
    local txt_matchDes = csbNode:getChildByName("txt_matchDes")
    txt_matchDes:setString(list.szMatchName)

    local txt_player = csbNode:getChildByName("Text_2")
    txt_player:setString(self._nWaitNumber.."")

    local allCount = list.cbMatchRule.cbTotalRounds+1
    local playerTable = {}
    table.insert(playerTable,list.cbMatchRule.wStartUserCount)

    for i=1, list.cbMatchRule.cbTotalRounds do
            table.insert(playerTable,list.cbMatchRule.hMatchRounds[1][i].wPromotion)
    end

    -- local progress=csbNode:getChildByName("LoadingBar_1")
    -- local pointx,pointy = progress:getPosition()
    -- local width = progress:getContentSize().width
    -- for i=1,allCount do
    --         local filepath=  i==1 and "match/match_wait_countbg1.png" or "match/match_wait_countbg2.png"
    --         local sprite = cc.Sprite:create(filepath):move(pointx+i*(width/(allCount+1)),pointy):addTo(Layer)
    --         ccui.Text:create(playerTable[i], "fonts/round_body.ttf", 30):move(sprite:getContentSize().width/2,sprite:getContentSize().height/2):addTo(sprite)
    -- end

    --   local progress = 0
    --  local progress = csbNode:getChildByName("LoadingBar_1")
    --  if list.cbMatchRule.wStartUserCount ~= 0 then
    --     fpercent = self._nWaitNumber/list.cbMatchRule.wStartUserCount*100 
    --     fpercent = fpercent >100 and 100 or fpercent
    -- end
    
    --  progress:setPercent(fpercent/(allCount +1))


   -- local curIcon = cc.Sprite:create("match/match_wait_cone.png"):move(pointx+(width/(allCount+1)),pointy + 45):addTo(Layer)

    self.LockTimetxt_time = csbNode:getChildByName("AtlasLabel_time")

    self.m_timeNum = 0--os.time({year=list.cbMatchRule.MatchStartTime.wYear, month = list.cbMatchRule.MatchStartTime.wMonth, day =list.cbMatchRule.MatchStartTime.wDay, hour =list.cbMatchRule.MatchStartTime.wHour, min =list.cbMatchRule.MatchStartTime.wMinute, sec = list.cbMatchRule.MatchStartTime.wSecond}) - os.time()
   --print(self.m_timeNum.."=========================222888")
        local function timeUpdate()
        if self.m_timeNum ~= 0  and self.LockTimetxt_time then
            --print(self.m_timeNum.."=========================111888")
            self.m_timeNum = self.m_timeNum -1 
            if self.m_timeNum <= 0 then
                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._priTimeShd)
                self._priTimeShd = nil      
                self.LockTimetxt_time:setString("00/00")
            else         

                self.LockTimetxt_time:setString(self:getTimeString(self.m_timeNum))
                --self.curUserTime = PriRoom:getInstance().m_tabPriData.dwDrawTimeLimit*60 - self.m_timeNum
            end
        end
      --self.m_timeTxt:setString(""..os.date("%H:%M:%S",self.m_timeNum))
    end
    self._priTimeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(timeUpdate, 1, false)

        --取消报名
    local function cancelSign( sender,toucheEvent)
        if toucheEvent == ccui.TouchEventType.ended then
            local touchFunc = function( sender,toucheEvent )
                if toucheEvent == ccui.TouchEventType.ended then
                    if sender:getTag() == 1 then
                        if self._cbMatchUserStatus == 2 then
                            self:showPopWait()
                            self:getMatchFrame():sendLeaveMatch()

                            self:runAction(cc.Sequence:create(
                                cc.DelayTime:create(10),
                                cc.CallFunc:create(function ()
                                        self:onSubMatchUStatus(DEFINE.MUS_NULL)
                                    end)))
                        end
                        sender:getParent():removeFromParent()
                    elseif sender:getTag() == 2 then
                        sender:getParent():removeFromParent()
                    end          
                end
            end
            local rootLayer, csbNode = ExternalFun.loadRootCSB("match/MatchQuery.csb", self)  

            --比赛名称
            local txt_match_name = csbNode:getChildByName("txt_2")
            txt_match_name:setString(list.szMatchName)

            local txt_tip = csbNode:getChildByName("txt_1_1"):setString("只需要再等待      分钟就可以开赛,")
            --等待人数
            local txt_wait_num = csbNode:getChildByName("txt_3")
            txt_wait_num:setString(""..math.floor(self.m_timeNum/60))

            --取消报名
            local btn = csbNode:getChildByName("btn_1")  
            btn:setTag(1)
            btn:addTouchEventListener(touchFunc)  

            btn = csbNode:getChildByName("btn_2")  
            btn:setTag(2)
            btn:addTouchEventListener(touchFunc)
        end
    end

    local tabRoundInfo=list.cbMatchRule.hMatchRounds[1]
    local btn_cancell_signup = csbNode:getChildByName("btn_cancell_signup")
    btn_cancell_signup:addTouchEventListener(cancelSign)

    local barSize=csbNode:getChildByName("Img_progress"):getContentSize()
    local bar=csbNode:getChildByName("Img_progress")

    local curRound=1

    local cellW,cellH=200,190
    local tvW=1200
    if list.cbMatchRule.cbTotalRounds+1<6 then
        cellW=tvW/(list.cbMatchRule.cbTotalRounds+1)
    end
    self.cellW=cellW
    self.barSize=barSize

     --self._nWaitNumber = 5
     local tvPos=cc.p(bar:getPositionX(),bar:getPositionY()-cellH/2)
     local per=(self._nWaitNumber or 1)/list.cbMatchRule.wStartUserCount
     per = per >= 1 and 1 or  per
     bar:setContentSize(cc.size(70*per,barSize.height))

    local function scrollViewDidScroll(view)
        local x=view:getContentOffset().x
        bar:setPositionX(tvPos.x+x)     --+cellW/2
    end 
  
    local function cellSizeForTable(view, idx)
        return cellW,cellH 
    end

    local function numberOfCellsInTableView(view)
        return list.cbMatchRule.cbTotalRounds+1
    end

    local function tableCellAtIndex(view, idx)
 
        local cell = view:dequeueCell()
        if not cell then        
            cell = cc.TableViewCell:new()
        else
            cell:removeAllChildren()
        end

        local filepath=  1+idx<=curRound and "match/match_wait_countbg1.png" or "match/match_wait_countbg2.png"
       
        local sp=cc.Sprite:create(filepath):addTo(cell)
        local hw,hh=sp:getContentSize().width/2,sp:getContentSize().height/2

        sp:setPosition(cc.p(cellW/2,cellH/2))
        local count=list.cbMatchRule.wStartUserCount
        if idx>0 then count=tabRoundInfo[idx].wPromotion or 1 end
        ccui.Text:create(count, "fonts/round_body.ttf", 24):addTo(cell):setPosition(cc.p(cellW/2,cellH/2))
        if 1+idx==curRound then
            cc.Sprite:create("match/match_wait_cone.png"):addTo(cell):setAnchorPoint(cc.p(0.5,0)):setPosition(cc.p(cellW/2,hh+cellH/2-10))
        end
        if idx==0 then
            posFX=sp:getPositionX()
        end

        return cell
    end

    local tableView = cc.TableView:create({width=tvW,height=cellH})
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
    tableView:setPosition(tvPos)  
    tableView:addTo(csbNode)
    tableView:setDelegate()
    tableView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tableView:registerScriptHandler(scrollViewDidScroll,cc.SCROLLVIEW_SCRIPT_SCROLL)
    tableView:reloadData()
    Layer.tableView = tableView
end

function MatchRoomListLayer:initImmediateWaitLayer(Layer)
	local csbNode = Layer

	--比赛描述
	local list = self._curSignList
	local txt_matchDes = csbNode:getChildByName("txt_matchDes")
	txt_matchDes:setString(list.szMatchName)

	--报名进度
    local progress = cc.ProgressTimer:create(display.newSprite("match/sign_up_progress0.png"))
             :move(csbNode:getChildByName("sign_up_progress1_3"):getPosition())
             :setName("Progress")
             :setPercentage(50)
             :addTo(csbNode)

    local tabRoundInfo=list.cbMatchRule.hMatchRounds[1]

    --满人数目
    local txt_full_count = csbNode:getChildByName("txt_full_count")
    txt_full_count:setString("/"..list.cbMatchRule.wStartUserCount)

    local txt_promotion_count = csbNode:getChildByName("txt_promotion_count")
    txt_promotion_count:setString(tabRoundInfo[1].wPromotion)

    --报名进度
    local fpercent = 0
    if list.cbMatchRule.wStartUserCount ~= 0 then
    	fpercent = self._nWaitNumber/list.cbMatchRule.wStartUserCount*100
    	progress:setPercentage(fpercent)
    end
    
    --已报名人数
    local panel = csbNode:getChildByName("Panel_1")
    panel:setLocalZOrder(1)
    local atlasLabel_number = panel:getChildByName("AtlasLabel_number") 
    atlasLabel_number:setString(""..self._nWaitNumber)

    --loadingBar
    -- local bar = csbNode:getChildByName("LoadingBar") 
    -- bar:setPercent(0)

    -- self:runAction(cc.Sequence:create(cc.DelayTime:create(0.0),cc.CallFunc:create(function()
    -- 	bar:setPercent(fpercent)
    -- end)))

    local function createBlinkDots(csbNode,pos,dt,n,bReverse)
		dt =dt or 0.5
		local offX=bReverse and -20 or 20
		n= n or 5
		for i=1,n do
			local dot=cc.Sprite:create("match/match_wait_dot2.png"):addTo(csbNode)
			dot:setPosition(cc.p(pos.x+i*offX,pos.y))
			dot:setVisible(false)
			
			local d1,d2=i*dt,(n-i)*dt --d1隐藏时间 d2显示时间
			local seq=cc.Sequence:create(cc.DelayTime:create(d1),cc.Show:create(),cc.DelayTime:create(d2),cc.Hide:create())
			dot:runAction(cc.RepeatForever:create(seq))

		end
	end

	createBlinkDots(csbNode,cc.p(csbNode:getChildByName("dot"):getPosition()))

    --取消报名
    local function cancelSign( sender,toucheEvent)
    	if toucheEvent == ccui.TouchEventType.ended then
    		local touchFunc = function( sender,toucheEvent )
		    	if toucheEvent == ccui.TouchEventType.ended then
		            if sender:getTag() == 1 then
		            	if self._cbMatchUserStatus == 2 then
				    	self:showPopWait()
					self:getMatchFrame():sendLeaveMatch()
                                                     self:runAction(cc.Sequence:create(
                                                        cc.DelayTime:create(10),
                                                        cc.CallFunc:create(function ()
                                                                self:onSubMatchUStatus(DEFINE.MUS_NULL)
                                                            end)))
    				end
    				sender:getParent():removeFromParent()
		            elseif sender:getTag() == 2 then
		            	sender:getParent():removeFromParent()
		            end          
		        end
		    end
        	local rootLayer, csbNode = ExternalFun.loadRootCSB("match/MatchQuery.csb", self)  

        	--比赛名称
        	local txt_match_name = csbNode:getChildByName("txt_2")
        	txt_match_name:setString(list.szMatchName)

        	--等待人数
        	local txt_wait_num = csbNode:getChildByName("txt_3")
        	local n=list.cbMatchRule.wStartUserCount-self._nWaitNumber
        	txt_wait_num:setString(n>0 and n or 2)

            --取消报名
            local btn = csbNode:getChildByName("btn_1")  
            btn:setTag(1)
            btn:addTouchEventListener(touchFunc)  

            btn = csbNode:getChildByName("btn_2")  
            btn:setTag(2)
            btn:addTouchEventListener(touchFunc)
        end
    end

    local btn_quit = csbNode:getChildByName("btn_quit")
    local btn_cancell_signup = csbNode:getChildByName("btn_cancell_signup")
    btn_cancell_signup:addTouchEventListener(cancelSign)
    btn_quit:addTouchEventListener(cancelSign)

    local barSize=csbNode:getChildByName("Img_progress"):getContentSize()
    local bar=csbNode:getChildByName("Img_progress")

    local curRound=1

    local cellW,cellH=200,190
    local tvW=1200
    if list.cbMatchRule.cbTotalRounds+1<6 then
    	cellW=tvW/(list.cbMatchRule.cbTotalRounds+1)
    end
    self.cellW=cellW
    self.barSize=barSize

     local tvPos=cc.p(bar:getPositionX(),bar:getPositionY()-cellH/2)
     local per=(self._nWaitNumber or 1)/list.cbMatchRule.wStartUserCount
    bar:setContentSize(cc.size((cellW-117)*0.5*per,barSize.height))

    local function scrollViewDidScroll(view)
		local x=view:getContentOffset().x
		bar:setPositionX(tvPos.x+x)		--+cellW/2
	end 
  
    local function cellSizeForTable(view, idx)
        return cellW,cellH 
    end

    local function numberOfCellsInTableView(view)
        return list.cbMatchRule.cbTotalRounds+1
    end

    local function tableCellAtIndex(view, idx)
 
        local cell = view:dequeueCell()
        if not cell then        
            cell = cc.TableViewCell:new()
        else
            cell:removeAllChildren()
        end

        local filepath=  1+idx<=curRound and "match/match_wait_countbg1.png" or "match/match_wait_countbg2.png"
       
        local sp=cc.Sprite:create(filepath):addTo(cell)
        local hw,hh=sp:getContentSize().width/2,sp:getContentSize().height/2

        sp:setPosition(cc.p(cellW/2,cellH/2))
        local count=list.cbMatchRule.wStartUserCount
        if idx>0 then count=tabRoundInfo[idx].wPromotion or 1 end
        ccui.Text:create(count, "fonts/round_body.ttf", 24):addTo(cell):setPosition(cc.p(cellW/2,cellH/2))
        if 1+idx==curRound then
            cc.Sprite:create("match/match_wait_cone.png"):addTo(cell):setAnchorPoint(cc.p(0.5,0)):setPosition(cc.p(cellW/2,hh+cellH/2-10))
        end
        if idx==0 then
        	posFX=sp:getPositionX()
        end

        return cell
    end

    local tableView = cc.TableView:create({width=tvW,height=cellH})
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
    tableView:setPosition(tvPos)  
    tableView:addTo(csbNode)
    tableView:setDelegate()
    tableView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tableView:registerScriptHandler(scrollViewDidScroll,cc.SCROLLVIEW_SCRIPT_SCROLL)
    tableView:reloadData()
    
    Layer.tableView = tableView

end

function MatchRoomListLayer:onBackgroundCallBack(bEnter) --
	if bEnter then

		

                    if self._waitMatchEnterView and not tolua.isnull(self._waitMatchEnterView)  then
                        MatchRoom:getInstance():setLoginServerID(MatchRoom:getInstance().dwServerID)

                        print("GlobalUserItem.nCurGameKind:",GlobalUserItem.nCurGameKind)
                        --报名
                        GlobalUserItem.bMatch = true
                        self._scene:updateCurrentGame()
                        --self._curSignList = ref._curSignList
                         --  GlobalUserItem.cbMatchType = ref.matchType

                        MatchRoom:getInstance():showPopWait()
                        self:runAction(cc.Sequence:create(cc.DelayTime:create(13),cc.CallFunc:create(function() MatchRoom:getInstance():dismissPopWait() end)))

                        --连接游戏服
                        
                        local entergame = self._scene:getEnterGameInfo()
                        if entergame then
                            self:getGameFrame():setKindInfo(GlobalUserItem.nCurGameKind, entergame._KindVersion)
                            self:getGameFrame():setViewFrame(self._scene)
                            
                            self:getGameFrame():onCloseSocket()
                            
                            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.1),cc.CallFunc:create(function() self:getGameFrame():onLogonRoom() end)))
                            MatchRoom:getInstance().bSignup = true
                        end
                     end


	else
		
		MatchRoom:getInstance().bSignup = false
		self._cbMatchUserStatus = DEFINE.MUS_NULL
	end
end

function MatchRoomListLayer:getMatchServerCount()
	--dump(self.m_tabMatchRoomList,"00000000000",6)
	print("#self.m_tabMatchRoomList",#self.m_tabMatchRoomList)
	return #self.m_tabMatchRoomList
end


function MatchRoomListLayer:getSpecificMatchServerCount(kind)  
	kind=kind or  MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE
	local n=0
	for k,v in pairs(self.m_tabMatchRoomList) do
		local iteminfo =v[1]
		local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(iteminfo.wServerID)
		if matchinfo.cbMatchType == kind then
			n=n+1
		end
	end
	return n
end



function MatchRoomListLayer:showMatchDetails(iteminfo)

	if nil==iteminfo then return end
	local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(iteminfo.wServerID)
	if nil==matchinfo then return end
	--dump(iteminfo,"iteminfo:",6)

	local layer=self:getChildByTag(tag_matchDetail)
	if layer then
		layer:removeFromParent()
	end

	local csbNode=ExternalFun.loadCSB("match/MatchDetails.csb",self)
	csbNode:setTag(tag_matchDetail)
	local mask=csbNode:getChildByName("Image_mask")
	mask:setTouchEnabled(true)
	mask:setSwallowTouches(true)

	local node_desc=csbNode:getChildByName("Node_desc")
	local node_rule=csbNode:getChildByName("Node_rule")
	local btn_desc=csbNode:getChildByName("btn_desc")
           local btn_rule=csbNode:getChildByName("btn_rule")

    btn_desc:loadTextureNormal("match/match_info_btn2.png") 

    btn_desc:addClickEventListener(function() 
        node_desc:setVisible(true) 
        node_rule:setVisible(false) 
        btn_desc:loadTextureNormal("match/match_info_btn2.png") 
        btn_rule:loadTextureNormal("match/match_info_btn3.png") 
    end)

	btn_rule:addClickEventListener(function() 
        node_desc:setVisible(false) 
        node_rule:setVisible(true) 
        btn_rule:loadTextureNormal("match/match_info_btn4.png") 
        btn_desc:loadTextureNormal("match/match_info_btn1.png") 
    end)
	csbNode:getChildByName("Button_3"):addClickEventListener(function() csbNode:removeSelf() end)

	node_desc:setVisible(true)
	node_rule:setVisible(false)

	node_desc:getChildByName("Text_name"):setString(matchinfo.szMatchName)
	--node_desc:getChildByName("Text_num"):setString(string.format("%d/%d人",(matchinfo.cbMatchRule.joinUserCount or 0) ,matchinfo.cbMatchRule.wStartUserCount))
	local reward=0
           local reward1 = 0
	for i=1,3 do
		if matchinfo.MatchRewardInfo[1][i].wRankID==1 then
			reward=matchinfo.MatchRewardInfo[1][i].lRewardGold
                                reward1=matchinfo.MatchRewardInfo[1][i].lRewardIngot
		end
	end
	node_desc:getChildByName("Text_num"):setString(reward.."金币,"..reward1.."钻石")
	local ruleStr= matchinfo.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE and "即时赛" or "定时赛"
	node_desc:getChildByName("Text_rule"):setString(ruleStr)
	node_desc:getChildByName("Text_fee"):setString(matchinfo.lSignupFee..(matchinfo.cbFeeType == 0 and "金币" or "钻石"))

          if matchinfo.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE then
                node_desc:getChildByName("Text_6"):setString("人满即开")
          else
            local hour = matchinfo.cbMatchRule.MatchStartTime.wHour < 10 and ("0"..matchinfo.cbMatchRule.MatchStartTime.wHour) or matchinfo.cbMatchRule.MatchStartTime.wHour
            local min = matchinfo.cbMatchRule.MatchStartTime.wMinute <10 and ("0"..matchinfo.cbMatchRule.MatchStartTime.wMinute) or matchinfo.cbMatchRule.MatchStartTime.wMinute
                   node_desc:getChildByName("Text_6"):setString(hour..":"..min)
          end
    -- node_rule:getChildByName("")
    -- node_rule:getChildByName("")
    -- node_rule:getChildByName("")
    -- node_rule:getChildByName("")

        node_rule:getChildByName("Text_initscore"):setString(""..matchinfo.cbMatchRule.lInitalScore)
        node_rule:getChildByName("Text_rule"):setString("使用每轮淘汰制度，每轮比赛结束后，剩下一定数量的玩家晋级到下一轮比赛，直至角逐冠军")
	--报名按钮
			--事件注册
    local touchFunc = function( sender,toucheEvent )
    	if toucheEvent == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(sender:getTag(), sender)  
            csbNode:removeSelf()          
        end
    end
	local btn_signup = csbNode:getChildByName("Button_join")
	btn_signup:setTag(tag_signup)
	btn_signup.kind = iteminfo.wKindID
	print("iteminfo.wKindID:",iteminfo.wKindID)
	btn_signup.nCurRoomIndex = iteminfo._nRoomIndex
	btn_signup._curSignList = matchinfo
            btn_signup.wServerID=iteminfo.wServerID
           btn_signup.matchType = matchinfo.cbMatchType

	btn_signup:addTouchEventListener(touchFunc)


	local contentBg=csbNode:getChildByName("content")
	local cellSize=contentBg:getContentSize()

	local tabMatchReward={}-- tabMatchReward[i]={rankRange={3,6},reward=matchinfo.MatchRewardInfo[i]}
	for i=1,matchinfo.wRewardCount do
		local matchRewardInfo=matchinfo.MatchRewardInfo[1][i]
		if matchRewardInfo then
			local t={}
			if matchRewardInfo.wRankID<=3 then
				t.rankRange={matchRewardInfo.wRankID}
				t.reward=matchRewardInfo
				table.insert(tabMatchReward,t)
			else
				--tobeupdated
			end
		end
	end


	local function cellSizeForTable(view, idx)
	    return cellSize.width,cellSize.height/5
	end

	local function numberOfCellsInTableView(view)
   		return #tabMatchReward
	end

	local function tableCellAtIndex(view, idx)
		local cell = view:dequeueCell()
	    if not cell then        
	        cell = cc.TableViewCell:new()
	    else
	        cell:removeAllChildren()
	    end

	    local node=ExternalFun.loadCSB("match/MatchDetailNode.csb",cell)

	    local matchReward=tabMatchReward[idx+1]
	    if matchReward.rankRange[1]<=3 then
	    	local sps={"match/match_firstplace.png","match/match_secondplace.png","match/match_thirdplace.png"}
	    	node:getChildByName("sp_rank"):setTexture(sps[matchReward.rankRange[1]])
	    	node:getChildByName("txt_rank"):setVisible(false)
	    else
	    	node:getChildByName("sp_rank"):setVisible(false)
	    	local str=""
	    	if matchReward.rankRange[2] then
	    		str=string.format("%d-%d名",matchReward.rankRange[1],matchReward.rankRange[2])
	    	else
	    		str=string.format("第%d名",matchReward.rankRange[1])
	    	end
	    	node:getChildByName("txt_rank"):setString(str)
	    end

	    node:getChildByName("txt_reward"):setString("x"..matchReward.reward.lRewardGold)

	    return cell
	end

	local tableView = cc.TableView:create(contentBg:getContentSize())
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(contentBg:getPosition())
    tableView:setDelegate()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(csbNode)
    tableView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tableView:reloadData()
    contentBg:removeSelf()
end

function MatchRoomListLayer:createMatchNodeTableView(kind,panel) 
	kind=kind or MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE

	local specificMatchRoomList={}

	for i=1,#self.m_tabMatchRoomList do
		local iteminfo = self.m_tabMatchRoomList[i][1]
		local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(iteminfo.wServerID)
                      --print("iteminfo.wServerID: ",iteminfo.wServerID)
                      dump(matchinfo, "match info============================="..i, 5)
     
		if matchinfo and matchinfo.cbMatchType == kind then
                                if matchinfo.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE then
                                            table.insert(specificMatchRoomList,self.m_tabMatchRoomList[i])
                                else
                                            -- local localTime = os.date("*t",os.time())
                                            -- dump(localTime,"================createMatchNodeTableView")
                                            local time = os.time({year=matchinfo.cbMatchRule.MatchStartTime.wYear, month = matchinfo.cbMatchRule.MatchStartTime.wMonth, day =matchinfo.cbMatchRule.MatchStartTime.wDay, hour =matchinfo.cbMatchRule.MatchStartTime.wHour, min =matchinfo.cbMatchRule.MatchStartTime.wMinute, sec = matchinfo.cbMatchRule.MatchStartTime.wSecond}) - os.time()
			            
                                       
                                            if time >= 0 then
                                                table.insert(specificMatchRoomList,self.m_tabMatchRoomList[i])
                                            end

                                end
		end
	end

--子视图大小
	local function cellSizeForTable(view, idx)
	  	return 1100 , 160
	end

--子视图数目
	local function numberOfCellsInTableView(view)
		return #specificMatchRoomList --self:getSpecificMatchServerCount(kind)
	end

	local function tableCellTouched(view, cell)

	end

	--获取子视图
	local function tableCellAtIndex(view, idx)
		local iteminfo = specificMatchRoomList[idx+1][1]
		-- dump(iteminfo, "info ", 6)

		local cell = view:dequeueCell()
		local wLv = (iteminfo == nil and 0 or iteminfo.wServerLevel)
		if cell == nil then
			cell = cc.TableViewCell:new()
		end
		cell:removeAllChildren()

		if not iteminfo then
			return cell
		end

		local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(iteminfo.wServerID)
		--dump(matchinfo, "match info", 6)
		

		
		if yl.GAME_GENRE_MATCH == iteminfo.wServerType then
			local matchType = kind == MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE or false

			local matchNode = cc.CSLoader:createNode(matchType == true and "match/MatchNode.csb" or "match/MatchNode1.csb");
			matchNode:setAnchorPoint(0.5,0.5)
		          matchNode:setPosition(550, 80)
		          cell:addChild(matchNode)

		          local mask= matchNode:getChildByName("Image_bg")
		          mask:setTouchEnabled(true)
		          mask:setSwallowTouches(false)
		          mask:addTouchEventListener(
		    	function(ref,toucheEvent) 
			    	if toucheEvent == ccui.TouchEventType.ended then
			    		local beginPos = ref:getTouchBeganPosition()
                        	                	local endPos = ref:getTouchEndPosition()
                        	                	if math.abs(endPos.x - beginPos.x) > 30 or math.abs(endPos.y - beginPos.y) > 30 then
                        	                    	          return
                        	                	end
		    			self:showMatchDetails(iteminfo)  
		    		end
		    	end)

			local matchTypeStr =""
			--比赛信息
			if matchType then --即时赛
				local tainer_bg = matchNode:getChildByName("tainer_bg")
				matchTypeStr = matchinfo.szMatchName

				--满人
				local fullCount = tainer_bg:getChildByName("txt_match_playCount")
				local fullStr = "满"..matchinfo.cbMatchRule.wStartUserCount.."人开赛"
				fullCount:setString(fullStr)

				--等待人数
				local atlasLabel_Player = tainer_bg:getChildByName("AtlasLabel_Player")
				atlasLabel_Player:setString(matchinfo.cbMatchRule.wStartUserCount-(matchinfo.cbMatchRule.joinUserCount or 0))
				matchinfo._waitCountAtlas = atlasLabel_Player

				--比赛名称
				local txt_match_name = tainer_bg:getChildByName("txt_match_name")
				txt_match_name:setString(matchTypeStr)

				--比赛类型
				local txt_match_game = tainer_bg:getChildByName("txt_match_game")
				txt_match_game:setString(iteminfo.szServerName)

				local rewardGold=0
                                                   local rewardDiamond=0
				for i=1,3 do
					if matchinfo.MatchRewardInfo[1][i].wRankID==1 then
						rewardGold=matchinfo.MatchRewardInfo[1][i].lRewardGold
                                                                rewardDiamond = matchinfo.MatchRewardInfo[1][i].lRewardIngot
					end
				end

                                                   
                                                   

                                                   local goldTxt =  tainer_bg:getChildByName("Text_reward")
                                                   local diamondTxt = tainer_bg:getChildByName("Text_reward_0")
                                                    if rewardGold >0 then
                                
                                                        goldTxt:setString("x"..rewardGold)
                                                    else
                                                        goldTxt:setVisible(false)
                                                        tainer_bg:getChildByName("Sprite_10"):setVisible(false)
                                                        diamondTxt:setPositionY(75)
                                                        tainer_bg:getChildByName("Sprite_11"):setPositionY(75)
                                                    end

                                                    if rewardDiamond >0 then
                                                        diamondTxt:setString("x"..rewardDiamond)
                                                    else
                                                        diamondTxt:setVisible(false)
                                                        tainer_bg:getChildByName("Sprite_11"):setVisible(false)
                                                        goldTxt:setPositionY(75)
                                                        tainer_bg:getChildByName("Sprite_10"):setPositionY(75)
                                                    end


				matchinfo._txtJoinNum=tainer_bg:getChildByName("Text_joinnum")
				tainer_bg:getChildByName("Text_joinnum"):setString((matchinfo.cbMatchRule.joinUserCount or 0) .."/"..matchinfo.cbMatchRule.wStartUserCount)

				--报名按钮
				--事件注册
			    local touchFunc = function( sender,toucheEvent )
			    	if toucheEvent == ccui.TouchEventType.ended then
			            self:onButtonClickedEvent(sender:getTag(), sender)            
			        end
			    end
				local btn_signup = tainer_bg:getChildByName("btn_signup")
				btn_signup:setTag(tag_signup)
				btn_signup.kind = iteminfo.wKindID
                                           btn_signup.matchType = kind
				btn_signup.nCurRoomIndex = iteminfo._nRoomIndex
				btn_signup._curSignList = matchinfo
				btn_signup.wServerID=iteminfo.wServerID
				 btn_signup:addTouchEventListener(touchFunc)
				--btn_signup:setSwallowTouches(false)

				--报名费用
				local txt_consum = btn_signup:getChildByName("txt_consum")
				
				if matchinfo.lSignupFee<=0 then
					btn_signup:getChildByName("image_consum"):setVisible(false)
					txt_consum:setString("免费")
					txt_consum:setPositionX(txt_consum:getPositionX()-15)
				else
					txt_consum:setString(matchinfo.lSignupFee)
				end

                                            local imgIcon = btn_signup:getChildByName("image_consum")
                                            if matchinfo.cbFeeType == 0 then
                                                    imgIcon:setTexture("shop/coin.png")
                                            else
                                                    imgIcon:setTexture("shop/diamond.png")
                                            end

			elseif kind == MatchRoom:getInstance().DEFINE.MATCH_TYPE_LOCKTIME then

				--local hourstr = string.format("%02d:%02d", matchinfo.cbMatchRule.MatchStartTime.wHour, matchinfo.cbMatchRule.MatchStartTime.wMinute)
				--matchTypeStr = matchinfo.szMatchName.."定时赛"
                                            local tainer_bg = matchNode:getChildByName("tainer_bg")
                                            matchTypeStr = matchinfo.szMatchName

                                            
                                            tainer_bg:getChildByName("txt_match_name"):setString("["..iteminfo.szServerName.."]")
                                            tainer_bg:getChildByName("Text_2"):setString(matchTypeStr.."")

                                            local rewardGold=0
                                                                      local rewardDiamond=0
                                            for i=1,3 do
                                                if matchinfo.MatchRewardInfo[1][i].wRankID==1 then
                                                    rewardGold=matchinfo.MatchRewardInfo[1][i].lRewardGold
                                                                                            rewardDiamond = matchinfo.MatchRewardInfo[1][i].lRewardIngot
                                                end
                                            end

                                           local goldTxt =  tainer_bg:getChildByName("Text_reward")
                                           local diamondTxt = tainer_bg:getChildByName("Text_reward_0")
                                            if rewardGold >0 then
                        
                                                goldTxt:setString("x"..rewardGold)
                                            else
                                                goldTxt:setVisible(false)
                                                tainer_bg:getChildByName("Sprite_1"):setVisible(false)
                                                diamondTxt:setPosition(400, 41)
                                                tainer_bg:getChildByName("Sprite_4"):setPosition(386,44)
                                            end

                                            if rewardDiamond >0 then
                                                diamondTxt:setString("x"..rewardDiamond)
                                            else
                                                diamondTxt:setVisible(false)
                                                tainer_bg:getChildByName("Sprite_4"):setVisible(false)
                                            end

                                            matchinfo._txtJoinNum=tainer_bg:getChildByName("Text_joinnum")

                                            tainer_bg:getChildByName("Text_joinnum"):setString((matchinfo.cbMatchRule.joinUserCount or 0).."")
                                            local min = matchinfo.cbMatchRule.MatchStartTime.wMinute <10 and ("0"..matchinfo.cbMatchRule.MatchStartTime.wMinute) or matchinfo.cbMatchRule.MatchStartTime.wMinute
                                            tainer_bg:getChildByName("Text_time"):setString((matchinfo.cbMatchRule.MatchStartTime.wHour < 10 and ("0"..matchinfo.cbMatchRule.MatchStartTime.wHour) or matchinfo.cbMatchRule.MatchStartTime.wHour) .."    "..min)


                                            --报名按钮
                                            --事件注册
                                            local touchFunc = function( sender,toucheEvent )
                                                if toucheEvent == ccui.TouchEventType.ended then
                                                    self:onButtonClickedEvent(sender:getTag(), sender)            
                                                end
                                            end
                                            local btn_signup = tainer_bg:getChildByName("btn_signup")
                                            btn_signup:setTag(tag_signup)
                                            btn_signup.kind = iteminfo.wKindID
                                            btn_signup.matchType = kind
                                            btn_signup.nCurRoomIndex = iteminfo._nRoomIndex
                                            btn_signup._curSignList = matchinfo
                                            btn_signup.wServerID=iteminfo.wServerID
                                             btn_signup:addTouchEventListener(touchFunc)
                                            --btn_signup:setSwallowTouches(false)

                                            --报名费用
                                            local txt_consum = btn_signup:getChildByName("txt_consum")

                                            
                                            if matchinfo.lSignupFee<=0 then
                                                btn_signup:getChildByName("image_consum"):setVisible(false)
                                                txt_consum:setString("免费")
                                                txt_consum:setPositionX(txt_consum:getPositionX()-15)
                                            else
                                                txt_consum:setString(matchinfo.lSignupFee)
                                            end

                                            local imgIcon = btn_signup:getChildByName("image_consum")
                                            if matchinfo.cbFeeType == 0 then
                                                    imgIcon:setTexture("shop/coin.png")
                                            else
                                                    imgIcon:setTexture("shop/diamond.png")
                                            end


			end

		else
			print("比赛房间数据出错")

		end

		return cell
	end

	local _listView = cc.TableView:create(cc.size(panel:getContentSize().width, panel:getContentSize().height +20))
	 _listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
	 _listView:setAnchorPoint(0,0)  
	 _listView:setPosition(cc.p(0, 0))
	 _listView:setDelegate()
	 --_listView:addTo(panel)
	 _listView:registerScriptHandler(tableCellTouched, cc.TABLECELL_TOUCHED)
	 _listView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	 _listView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
	 _listView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
            _listView.number = #specificMatchRoomList

	 return _listView
end
--------------------------------------------------------------------------------

function MatchRoomListLayer:onSubMatchFee(dataBuffer)
	
end

--比赛状态
function MatchRoomListLayer:onSubMatchUStatus(status)
	print("比赛报名状态-------",status)
	local cbMatchUserStatus = status
	self._cbMatchUserStatus = cbMatchUserStatus
	if cbMatchUserStatus == DEFINE.MUS_SIGNUPED then --报名状态
		--进入等待页面
                      if self._waitMatchEnterView then
                            self._waitMatchEnterView:removeFromParent()
                            self._waitMatchEnterView = nil
                            if self._priTimeShd then
                                cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._priTimeShd)
                                self._priTimeShd = nil 
                            end
                     end
                     self._scene.m_bEnableKeyBack = false
		--if not self._waitMatchEnterView then
                    if GlobalUserItem.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_IMMEDIATE then --即时赛
			local rootLayer, csbNode = ExternalFun.loadRootCSB("match/Wait_Layer.csb", self)
			self._waitMatchEnterView = csbNode

			self:initImmediateWaitLayer(self._waitMatchEnterView)
                    else
                                local rootLayer, csbNode = ExternalFun.loadRootCSB("match/Wait_Layer1.csb", self)
                                self._waitMatchEnterView = csbNode
                                self:initLockTimeWaitLayer(self._waitMatchEnterView)
                    end
		--end

	elseif cbMatchUserStatus == DEFINE.MUS_NULL then
		self:getGameFrame():onCloseSocket()
                     self:stopAllActions()
		if self._waitMatchEnterView then
                                MatchRoom:getInstance().bSignup = false
			self._waitMatchEnterView:removeFromParent()
			self._waitMatchEnterView = nil
                                if self._priTimeShd then
                                    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._priTimeShd)
                                    self._priTimeShd = nil 
                                end
		end
                      self._scene.m_bEnableKeyBack = true
	end
end

--等待人数
function MatchRoomListLayer:onSubMatchNum(cmdtable)
	-- --刷新人数
	-- if self._waitMatchEnterView then
	-- 	 --已报名人数
	-- 	local csbNode = self._waitMatchEnterView
	--     local panel = csbNode:getChildByName("Panel_1")
	--     local atlasLabel_number = panel:getChildByName("AtlasLabel_number") 
	--     atlasLabel_number:setString(""..self._nWaitNumber)
	-- end
           if cmdtable and cmdtable.dwSeconds then
                self.m_timeNum = cmdtable.dwSeconds
           end
end

--刷新人数
function MatchRoomListLayer:onSubMatchSignUpCount(dataBuffer)

	local wServerID = dataBuffer.wServerID
	local matchInfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(wServerID)
	if nil==matchInfo then
		
		return
	end
	print("matchInfo.cbMatchRule.wStartUserCount,dataBuffer.dwMatchCount",matchInfo.cbMatchRule.wStartUserCount,dataBuffer.dwMatchCount)
	local _waitCountAtlas = matchInfo._waitCountAtlas
	local __txtJoinNum=matchInfo._txtJoinNum
	
	matchInfo.cbMatchRule.joinUserCount= dataBuffer.dwMatchCount
	if  _waitCountAtlas and not tolua.isnull(_waitCountAtlas) then  
		_waitCountAtlas:setString(""..(matchInfo.cbMatchRule.wStartUserCount-dataBuffer.dwMatchCount))
	end
	if  __txtJoinNum and not tolua.isnull(__txtJoinNum) then  
                     if  matchInfo.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_LOCKTIME  then
                        __txtJoinNum:setString(dataBuffer.dwMatchCount.."")
                     else
		  __txtJoinNum:setString(dataBuffer.dwMatchCount.."/"..matchInfo.cbMatchRule.wStartUserCount)
                     end
	end

	--LogAsset:getInstance():logData("matchInfo.cbMatchRule.wStartUserCount,dataBuffer.dwMatchCount"..matchInfo.cbMatchRule.wStartUserCount.."  "..dataBuffer.dwMatchCount)
	self._nWaitNumber = dataBuffer.dwMatchCount --已报名人数


	if self._waitMatchEnterView and not tolua.isnull(self._waitMatchEnterView) and  self._curSignList==matchInfo then

                --print(GlobalUserItem.cbMatchType.."====================15555")
                if matchInfo.cbMatchType == MatchRoom:getInstance().DEFINE.MATCH_TYPE_LOCKTIME then
                         local txt_player = self._waitMatchEnterView:getChildByName("Text_2")
                         if txt_player then
                            txt_player:setString(dataBuffer.dwMatchCount.."")
                         end


                    local fpercent = 0
                    if matchInfo.cbMatchRule.wStartUserCount ~= 0 then
                        fpercent = self._nWaitNumber/matchInfo.cbMatchRule.wStartUserCount*100
                         fpercent = fpercent >= 100 and 100 or  fpercent
                    end
                        
                        --  progress:setPercent(fpercent/(matchInfo.cbMatchRule.cbTotalRounds +2))

                         self._waitMatchEnterView:getChildByName("Img_progress"):setContentSize(cc.size(0.7*fpercent,self.barSize.height))
                else
		local progress = self._waitMatchEnterView:getChildByName("Progress") 
           
            	    --报名进度
            	    local fpercent = 0
            	    if matchInfo.cbMatchRule.wStartUserCount ~= 0 then
            	    	fpercent = self._nWaitNumber/matchInfo.cbMatchRule.wStartUserCount*100
            	    	progress:setPercentage(fpercent)
            	    end
            	    
            	    --loadingBar
            	    local bar = self._waitMatchEnterView:getChildByName("LoadingBar") 
            	    bar:setPercent(fpercent)
                    print("fpercent:",fpercent)

                    self._waitMatchEnterView:getChildByName("Img_progress"):setContentSize(cc.size((self.cellW-117)*0.5*fpercent/100,self.barSize.height))
            	    local temp=self._waitMatchEnterView:getChildByName("Img_progress")
                   -- if self._curSignList==matchInfo then
            	    self._waitMatchEnterView:getChildByName("Panel_1"):getChildByName("AtlasLabel_number"):setString(dataBuffer.dwMatchCount)
	   -- end
                end
	end

end


--------------------------------------------------------------------------------

--显示等待
function MatchRoomListLayer:showPopWait()
	if self._scene then
		self._scene:showPopWait()
	end
end

--关闭等待
function MatchRoomListLayer:dismissPopWait()
	if self._scene then
		self._scene:dismissPopWait()
	end
end

function MatchRoomListLayer:onButtonClickedEvent( tag, ref )
	self.name="MatchRoomListLayer"
    if tag_back == tag then 
         -- 大厅界面
        GlobalUserItem.bMatch = false -- 退出设置为false
        self._scene:onChangeShowMode(yl.SCENE_PLAZA)
    elseif tag_mineMatch == tag then
    	--我的比赛
    	local csbNode = ExternalFun.loadCSB("match/Mine_MatchLayer.csb", self)
    	csbNode:setTag(tag_historyLayer)
    	print("csbNode:getParent().name:",csbNode:getParent().name)

    	local btn_close = csbNode:getChildByName("btn_close")
    	btn_close:addTouchEventListener(function(sender,TouchEventType)
    		if TouchEventType == ccui.TouchEventType.ended then
           		csbNode:removeFromParent()      
        	end
    	end)

    	FriendMgr:getInstance():sendMyMatchHistory()

    elseif tag_signup == tag then

   
    	if self._cbMatchUserStatus == 2 then --?
    		 
    		MatchRoom:getInstance():showPopWait()
			self:getMatchFrame():sendLeaveMatch()
    		return
    	end

        local nKindID = ref.kind
        local scene = self._scene
        local app = scene:getApp()
        local game = scene:getGameInfo(nKindID)
       
        local version = tonumber(app:getVersionMgr():getResVersion(nKindID))

        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
      
        if (not version or game._ServerResVersion > version ) and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then

            self:checkGameUpdate(nKindID) 
           
            return
        end

    	MatchRoom:getInstance().dwServerID= ref.wServerID
    	MatchRoom:getInstance():setLoginServerID(ref.wServerID)

    	print("GlobalUserItem.nCurGameKind:",GlobalUserItem.nCurGameKind)
    	--报名
    	GlobalUserItem.bMatch = true
    	GlobalUserItem.nCurGameKind = ref.kind
    	GlobalUserItem.nCurRoomIndex = ref.nCurRoomIndex
    	self._scene:updateCurrentGame()
    	self._curSignList = ref._curSignList
           GlobalUserItem.cbMatchType = ref.matchType

    	MatchRoom:getInstance():showPopWait()
    	self:runAction(cc.Sequence:create(cc.DelayTime:create(13),cc.CallFunc:create(function() MatchRoom:getInstance():dismissPopWait() end)))

    	--连接游戏服
    	
    	local entergame = self._scene:getEnterGameInfo()
		self:getGameFrame():setKindInfo(GlobalUserItem.nCurGameKind, entergame._KindVersion)
		self:getGameFrame():setViewFrame(self._scene)
		
		self:getGameFrame():onCloseSocket()
		
		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.1),cc.CallFunc:create(function() self:getGameFrame():onLogonRoom() end)))
		MatchRoom:getInstance().bSignup = true
	elseif tag_cancell == tag then
		GlobalUserItem.bMatch = false
		--取消报名
		MatchRoom:getInstance():showPopWait()
		self:getMatchFrame():sendLeaveMatch()
	elseif tag_exchange1 == tag  or tag_exchange2 == tag then
		-- 是否开启
	    if GlobalUserItem.tabSystemConfig.IsOpenMall == yl.ENABLE_VALUE then
	        local sl = ShopLayer:create(self._scene, tag_exchange2 == tag and ShopLayer.DIAMOND_CHARGE or ShopLayer.GOLD_EXCHANGE, ShopLayer.SECOND_LEVEL)
	        self._scene:addPopLayer(sl)
	    else
	        local hl = HelpLayer:create(self._scene, shopType, HelpLayer.SECOND_LEVEL)
	        self._scene:addPopLayer(hl)
	    end	
    end
end  

function MatchRoomListLayer:onLoginMatchRoomFinish()
	if MatchRoom:getInstance().bSignup then
	 	self:getMatchFrame():sendJoinMatch()
	end
end  

function MatchRoomListLayer:getMatchFrame()
	return MatchRoom:getInstance()._matchFrame
end

function MatchRoomListLayer:getGameFrame()
	return MatchRoom:getInstance()._matchFrame._gameFrame
end

function MatchRoomListLayer:showMatchHistoryDetails(cmdtable)

	local function formatTimeStr(t,kind) --match_struct.SYSTEMTIME
		return string.format("%d月%d日 %02d:%02d:%02d",t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond)
	end

	 local csb = ExternalFun.loadCSB("match/MatchGameResult.csb", self)
	 csb:getChildByName("btn_close"):setVisible(true)
	 csb:getChildByName("btn_close"):addClickEventListener(function() csb:removeSelf() end)
	 csb:getChildByName("txt_time"):setString(formatTimeStr(cmdtable.MatchStartTime))
	 local retBtn=csb:getChildByName("btn_1")
	 local shareBtn=csb:getChildByName("btn_2")
	 shareBtn:setVisible(true)
	 retBtn:setVisible(false)
     shareBtn:setPositionX(0.5*(shareBtn:getPositionX()+retBtn:getPositionX()))
	 shareBtn:addClickEventListener(function() 
	 		self._scene:popTargetShare(function(target)
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享成功", 2)
                end
                GlobalUserItem.bAutoConnect = true
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL

            ExternalFun.popupTouchFilter(0, false)

            local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
        
            local imagename = "match_share.png"
            local render = cc.RenderTexture:create(yl.WIDTH,yl.HEIGHT ,cc.TEXTURE2_D_PIXEL_FORMAT_RGB_A8888)
		    render:retain()
		    render:beginWithClear(0, 0, 0, 0)
		    csb:visit()
		    render:endToLua() 
		    local ok=render:saveToFile(imagename)
		    local savepath=cc.FileUtils:getInstance():getWritablePath().."/"..imagename
		    local function saveCallBack( )
                ExternalFun.dismissTouchFilter()
               
                if not ok or nil == target then return end
                GlobalUserItem.bAutoConnect = false
                MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的比赛成绩", "分享我的比赛成绩", url, savepath, "true")
            end
            
		    self:runAction(cc.Sequence:create(cc.DelayTime:create(0.1),cc.CallFunc:create(saveCallBack )))
		    
	 	end,3)
	 end)

	 local str1=string.format("恭喜您在本次[%s]比赛中荣获",cmdtable.szMatchName)
	 local str2=string.format("第%d名",cmdtable.wRandID)
	 local str3
	 
	 if cmdtable.lRewardGold<=0 and cmdtable.lRewardDiamond<=0 then
	 	str3=""
	 elseif cmdtable.lRewardGold>0 and cmdtable.lRewardDiamond>0 then
	 	str3=string.format("奖励[%d金币][%d钻石],已发放到您的账户",cmdtable.lRewardGold,cmdtable.lRewardDiamond)
	 else
	 	local s=cmdtable.lRewardGold>0 and cmdtable.lRewardGold.."金币" or cmdtable.lRewardDiamond.."钻石"
	 	str3=string.format("奖励[%s],已发放到您的账户",s)
	 end
	 
	 csb:getChildByName("txt_1"):setString(str1)
	 csb:getChildByName("txt_2"):setString(str2)
	 csb:getChildByName("txt_3"):setString(str3)
end

function MatchRoomListLayer:onSubMyMatchHistoryResult(tabRecord)
	local historyLayer=self:getChildByTag(tag_historyLayer)
	if nil==historyLayer then
		return
	end

	local contentBg=historyLayer:getChildByName("contentbg")
	local cellSize=cc.size(contentBg:getContentSize().width,contentBg:getContentSize().height/3)

	local tempTabRecord={}
	for i=1,#tabRecord do
		if  tabRecord[i].bResultCode then
			table.insert(tempTabRecord,tabRecord[i])
		end
	end
	tabRecord=tempTabRecord

	local function cellSizeForTable(view, idx)
	    return cellSize.width,cellSize.height
	end

	local function numberOfCellsInTableView(view)
   		return math.ceil(#tabRecord/2)
	end

	local function tableCellAtIndex(view, idx)
	    local cell = view:dequeueCell()
	    if not cell then        
	        cell = cc.TableViewCell:new()
	    else
	        cell:removeAllChildren()
	    end

	    for i=1, 2 do
	    	local record=tabRecord[idx*2+i]
	        if nil == record then
	            return cell
	        end
	        local csb = ExternalFun.loadCSB("match/MatchHistoryNode.csb", cell)
	        local bg = csb:getChildByName("bg")
	        bg:setTouchEnabled(true)
	        bg:setSwallowTouches(false)
	        bg:addClickEventListener(function(ref) 
	        	local beginPos = ref:getTouchBeganPosition()
                local endPos = ref:getTouchEndPosition()
                if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30 then
                    return
                end
	        	self:showMatchHistoryDetails(record) 
	        end)
	        csb:setPositionX(cellSize.width*(i==1 and 0.25 or 0.75))
	        local matchName=bg:getChildByName("Text_matchname")
	        local matchNameBg=bg:getChildByName("matchnamebg")
	        local gameInfo= self._scene:getGameInfo(record.wKindID)
	        matchName:setString(gameInfo._GameName)
	        local w=matchName:getContentSize().width
	        if w>matchNameBg:getContentSize().width-7 then
	        	matchNameBg:setContentSize(cc.size(w+7,matchNameBg:getContentSize().height))
	        	matchName:setPositionX(matchNameBg:getPositionX())
	        	matchName:setAnchorPoint(cc.p(1,0.5))
	        end
	        bg:getChildByName("Text_describe"):setString(record.szMatchName) --record.szDescribeString)
	        local t=record.MatchStartTime
	        local timeStr=string.format("%02d.%02d %d:%02d",t.wMonth,t.wDay,t.wHour,t.wMinute)
	        bg:getChildByName("Text_time"):setString(timeStr)
	        if record.wRandID<=3 then
	        	local sps={"match/match_firstplace.png","match/match_secondplace.png","match/match_thirdplace.png"}
	        	bg:getChildByName("sp_rank"):setVisible(true)
	        	bg:getChildByName("Text_rank"):setVisible(false)
	        	bg:getChildByName("sp_rank"):setTexture(sps[record.wRandID])
	        else
	        	bg:getChildByName("sp_rank"):setVisible(false)
	        	bg:getChildByName("Text_rank"):setVisible(true)
	        	bg:getChildByName("Text_rank"):setString(string.format("第%d名",record.wRandID))
	        end
	    end
	    return cell
	end

    local tableView = cc.TableView:create(contentBg:getContentSize())
    tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    tableView:setPosition(contentBg:getPosition())
    tableView:setDelegate()
    tableView:setVerticalFillOrder(0)
    tableView:addTo(historyLayer)
    tableView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    tableView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    tableView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tableView:reloadData()
end

function MatchRoomListLayer:checkGameUpdate(nKindID)
    -- print("检测资源更新")
    -- local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    -- local scene = self._scene
    -- local app = scene:getApp()
    -- local game = scene:getGameInfo(nKindID)
    -- local version = tonumber(app:getVersionMgr():getResVersion(nKindID))
    -- dump(game, "Jack GAME", 6)
    -- print("Version.........",version)
    -- if not version or game._ServerResVersion > version then
        if false == self._scene:singleGameUpdate(nKindID, function()
               print("资源更新成功回调")
               
            end) then
            if nil ~= game then
                local gamename = game._GameName or ""
                local msg = string.format("请更新 %s 游戏资源!", gamename)
                showToast(self._scene, msg, 2)
            else
                showToast(self._scene, "请更新游戏资源!", 2)
            end
            return false
        end
    --     return true
    -- end
    -- return true
end


return MatchRoomListLayer