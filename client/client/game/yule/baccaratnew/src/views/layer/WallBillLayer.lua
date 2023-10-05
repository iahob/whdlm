--路单界面
local module_pre = "game.yule.baccaratnew.src";
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var;
local cmd = require(module_pre .. ".models.CMD_Game")

local WallBillLayer = class("WallBillLayer", cc.Layer)

local smallW = 29 --小单元格宽
local bigW = 58 --大单元格宽

function WallBillLayer:ctor(viewparent)
	--注册事件
	local function onLayoutEvent( event )
		if event == "exit" then
			self:onExit();
        elseif event == "enterTransitionFinish" then
        	self:onEnterTransitionFinish();
        end
	end
	self:registerScriptHandler(onLayoutEvent);

	self.m_parent = viewparent

	--加载csb资源
	local csbNode = ExternalFun.loadCSB("ludan/WallBill.csb", self);

	--关闭
	local btn_up = csbNode:getChildByName("btn_up");
	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:showLayer(false);
		end
	end	
	btn_up:addTouchEventListener(btnEvent);

	local btn_auto_open_1 = csbNode:getChildByName("btn_auto_open_1");
	local btn_auto_open_2 = csbNode:getChildByName("btn_auto_open_2");

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self.m_parent.isAutoOpenLudan = true;
			btn_auto_open_1:setVisible(false);
			btn_auto_open_2:setVisible(true);
		end
	end	
	btn_auto_open_1:addTouchEventListener(btnEvent);

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self.m_parent.isAutoOpenLudan = false;
			btn_auto_open_1:setVisible(true);
			btn_auto_open_2:setVisible(false);
		end
	end	
	btn_auto_open_2:addTouchEventListener(btnEvent);

	if self.m_parent.isAutoOpenLudan then
		btn_auto_open_1:setVisible(false);
		btn_auto_open_2:setVisible(true);
	else
		btn_auto_open_1:setVisible(true);
		btn_auto_open_2:setVisible(false);
	end
	
	--统计数据
    self.m_textXianSettle = csbNode:getChildByName("xian_settle");
    self.m_textZhuangSettle = csbNode:getChildByName("zhuang_settle");
    self.m_textPingSettle = csbNode:getChildByName("ping_settle");
    self.m_textXianDoubleCount = csbNode:getChildByName("xiandouble_count");
    self.m_textZhuangDoubleCount = csbNode:getChildByName("zhuangdouble_count");
    self.game_count = csbNode:getChildByName("game_count");
    
    --珠盘路、大路、大眼仔路、小路、曱甴路
    local scrollviewBase = csbNode:getChildByName("scrollviewBase");
    local scrollviewBig = csbNode:getChildByName("scrollviewBig");
    local scrollviewBigEye = csbNode:getChildByName("scrollviewBigEye");
    local scrollviewSmall = csbNode:getChildByName("scrollviewSmall");
    local scrollviewRoach = csbNode:getChildByName("scrollviewRoach");
    self.roadBaseContent = scrollviewBase;
    self.roadBigContent = scrollviewBig;
    self.roadBigEyeContent = scrollviewBigEye;
    self.roadSmallContent = scrollviewSmall;
    self.roadRoachContent = scrollviewRoach;

    self.node_next = csbNode:getChildByName("node_next");

    self.record = {};
    self.dataBig = {{}, {}, {}, {}, {}, {}};
    self.tempBiglist = {};
    
	self:reSet();
end

function WallBillLayer:onEnterTransitionFinish()
	self:registerTouch()
end

function WallBillLayer:onExit()
	local eventDispatcher = self:getEventDispatcher()
	eventDispatcher:removeEventListener(self.listener)
end

function WallBillLayer:registerTouch(  )
	
end

function WallBillLayer:showLayer( var )
	self:setVisible(var)
end

function WallBillLayer:reSet(  )
	
end

--初始化格子
function WallBillLayer:initCells(cnt, delta, sp, node)
	for i = 1, cnt do
    	local record = cc.Sprite:create("ludan/".. sp ..".png");
    	record:setAnchorPoint(0,0);
    	record:setPositionX((i-1)*delta);
		node:addChild(record);
		-- node:setContentSize(cc.size(self.m_atlasScore:getContentSize().width +120, 45))
    end
end


--珠盘路
function WallBillLayer:roadBase()
	self.roadBaseContent:removeAllChildren();
	self:initCells(15, bigW, "cellBig", self.roadBaseContent);
    
    for i = 1, #self.record do
    	local rec = self.record[i]
		local str = "";
		if cmd.AREA_XIAN == rec.m_cbGameResult then
			str = "ludan/icon_xian.png";
		elseif cmd.AREA_ZHUANG == rec.m_cbGameResult then
			str = "ludan/icon_zhuang.png";
		elseif cmd.AREA_PING == rec.m_cbGameResult then
			str = "ludan/icon_ping.png";
		end
		
		local item = cc.Sprite:create(str);
    	item:setPosition(smallW + math.floor((i-1) / 6) * bigW, 325 - ((i-1) % 6) * bigW);
		self.roadBaseContent:addChild(item);
    end
end

--大路
function WallBillLayer:roadBig()
	self.roadBigContent:removeAllChildren();
	self:initCells(30, smallW, "cellMid", self.roadBigContent);
    self:setBigRoadData(self.record);
    
    local startPos = cc.p(15, 160);
    self:showWaybillItem(self.dataBig, self.bigXLen, self.roadBigContent, "ludan/cellMid.png", "ludan/circle_red_big.png", "ludan/circle_blue_big.png", 30, smallW, false, startPos);
end

--大眼仔路
function WallBillLayer:roadBigEye()
	self.roadBigEyeContent:removeAllChildren();
	self:initCells(30, smallW, "cellSmall", self.roadBigEyeContent);
    
    local startPos = cc.p(15, 81);
    local recordTemp = self:getWaybillList(2, 2, 1, 2, 1);
    self:showWaybill(true, recordTemp, self.roadBigEyeContent, "ludan/cellSmall.png", "ludan/circle_red_small.png", "ludan/circle_blue_small.png", 30, smallW, true, startPos);

end

--小路
function WallBillLayer:roadSmall()
	self.roadSmallContent:removeAllChildren();
	self:initCells(30, smallW, "cellSmall", self.roadSmallContent);
    
    local startPos = cc.p(15, 81);
    local recordTemp = self:getWaybillList(3, 2, 1, 3, 2);
    self:showWaybill(true, recordTemp, self.roadSmallContent, "ludan/cellSmall.png", "ludan/dot_red.png", "ludan/dot_blue.png", 30, smallW, true, startPos);

end

--曱甴路
function WallBillLayer:roadRoach()
	self.roadRoachContent:removeAllChildren();
	self:initCells(30, smallW, "cellSmall", self.roadRoachContent);
	
	local startPos = cc.p(15, 81);
    local recordTemp = self:getWaybillList(4, 2, 1, 4, 3);
    self:showWaybill(true, recordTemp, self.roadRoachContent, "ludan/cellSmall.png", "ludan/line_red.png", "ludan/line_blue.png", 30, smallW, true, startPos);

end

function WallBillLayer:refreshList()
	if nil == self.m_parent then
		return;
	end	
	local mgr = self.m_parent:getDataMgr();
	local vec = mgr:getRecords();
	-- dump(mgr:getRecords(), "--------mgr:getRecords()---------", 6)
	self.record = vec;
	
	self:roadBase();
	self:roadBig();
	self:roadBigEye();
	self:roadSmall();
	self:roadRoach();

	self:showNext();
end

----设置大路数据
function WallBillLayer:setBigRoadData(records)
	self.dataBig, self.bigXLen, self.dataBigReality, self.tempBiglist = self.m_parent:getDataMgr():setBigRoadData(records);
end

--获取路单数据、参照开始点、第一行比较列、其他行比较列、看大路
function WallBillLayer:getWaybillList(x0, y0, d0, d1, d2)
	return  self.m_parent:getDataMgr():getWaybillList(x0, y0, d0, d1, d2);
end

--是否显示、各路单记录、路单node、cell图、red、black、列数、记录宽
function WallBillLayer:showWaybill(bShow, recordTemp, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
	return self.m_parent:getDataMgr():showWaybill(bShow, recordTemp, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos);
end

function WallBillLayer:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
	self.m_parent:getDataMgr():showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos);
end

--下局预测
function WallBillLayer:showNext()
	local red_next_1 = self.node_next:getChildByName("red_next_1");
	local red_next_2 = self.node_next:getChildByName("red_next_2");
	local red_next_3 = self.node_next:getChildByName("red_next_3");
	local black_next_1 = self.node_next:getChildByName("black_next_1");
	local black_next_2 = self.node_next:getChildByName("black_next_2");
	local black_next_3 = self.node_next:getChildByName("black_next_3");


	--下局庄
	local rec = {m_cbGameResult = cmd.AREA_MAX};
	rec.m_cbGameResult = cmd.AREA_ZHUANG;
	self.m_parent:getDataMgr():addGameRecord(rec);

	local mgr = self.m_parent:getDataMgr();
	local vec = mgr:getRecords();
	self:setBigRoadData(vec);

	--大眼仔路
	local recordTemp = self:getWaybillList(2, 2, 1, 2, 1);
    local lastDaEye = self:showWaybill(false, recordTemp);
    red_next_1:setTexture(lastDaEye == 1 and "ludan/circle_red_small.png" or "ludan/circle_blue_small.png");

    --小路
    local recordTemp = self:getWaybillList(3, 2, 1, 3, 2);
    local lastSmall = self:showWaybill(false, recordTemp);
    red_next_2:setTexture(lastSmall == 1 and "ludan/dot_red.png" or "ludan/dot_blue.png");

    --曱甴路
    local recordTemp = self:getWaybillList(4, 2, 1, 4, 3);
    local lastRoach = self:showWaybill(false, recordTemp);
    red_next_3:setTexture(lastRoach == 1 and "ludan/line_red.png" or "ludan/line_blue.png");

    self.m_parent:getDataMgr():subLastGameRecord();

	--下局闲
	local rec = {m_cbGameResult = cmd.AREA_MAX};
	rec.m_cbGameResult = cmd.AREA_XIAN;
	self.m_parent:getDataMgr():addGameRecord(rec);

	local mgr = self.m_parent:getDataMgr();
	local vec = mgr:getRecords();
	self:setBigRoadData(vec);

	--大眼仔路
	local recordTemp = self:getWaybillList(2, 2, 1, 2, 1);
    local lastDaEye = self:showWaybill(false, recordTemp);
    black_next_1:setTexture(lastDaEye == 1 and "ludan/circle_red_small.png" or "ludan/circle_blue_small.png");

    --小路
    local recordTemp = self:getWaybillList(3, 2, 1, 3, 2);
    local lastSmall = self:showWaybill(false, recordTemp);
    black_next_2:setTexture(lastSmall == 1 and "ludan/dot_red.png" or "ludan/dot_blue.png");

    --曱甴路
    local recordTemp = self:getWaybillList(4, 2, 1, 4, 3);
    local lastRoach = self:showWaybill(false, recordTemp);
    black_next_3:setTexture(lastRoach == 1 and "ludan/line_red.png" or "ludan/line_blue.png");

    self.m_parent:getDataMgr():subLastGameRecord();
end

--刷新局数
function WallBillLayer:refreshWallBillList()
	if nil == self.m_parent then
		return
	end
	local mgr = self.m_parent:getDataMgr()
	self:reSet()
	self:refreshList()

	--统计数据
	local vec = mgr:getRecords()
	local nTotal = #vec
	local nXian = 0
	local nZhuang = 0
	local nPing = 0
	local nXianDouble = 0
	local nZhuangDouble = 0
	local nXianTian = 0
	local nZhuangTian = 0
	for i = 1, nTotal do
		local rec = vec[i]
		if cmd.AREA_XIAN == rec.m_cbGameResult then
			nXian = nXian + 1
		elseif cmd.AREA_PING == rec.m_cbGameResult then
			nPing = nPing + 1
		elseif cmd.AREA_ZHUANG == rec.m_cbGameResult then
			nZhuang = nZhuang + 1
		end

		if rec.m_pServerRecord.bBankerTwoPair then
			nZhuangDouble = nZhuangDouble + 1
		end

		if rec.m_pServerRecord.bPlayerTwoPair then
			nXianDouble = nXianDouble + 1
		end

		--if cmd.AREA_XIAN_TIAN == rec.m_pServerRecord.cbKingWinner then
		if rec.m_pServerRecord.cbBankerCount < rec.m_pServerRecord.cbPlayerCount and rec.m_pServerRecord.cbPlayerCount >= 8 then
			nXianTian = nXianTian + 1
		end

		--if cmd.AREA_ZHUANG_TIAN == rec.m_pServerRecord.cbKingWinner then
		if rec.m_pServerRecord.cbPlayerCount < rec.m_pServerRecord.cbBankerCount and rec.m_pServerRecord.cbBankerCount >= 8 then
			nZhuangTian = nZhuangTian + 1
		end
	end

	local per = 0
	if 0 ~= nTotal then
		per = (nXian / nTotal) * 100
	end
	local str = string.format("%d", nXian)
	self.m_textXianSettle:setString(str)

	per = 0
	if 0 ~= nTotal then
		per = (nPing / nTotal) * 100
	end
	str = string.format("%d", nPing)
	self.m_textPingSettle:setString(str)

	per = 0
	if 0 ~= nTotal then
		per = (nZhuang / nTotal) * 100
	end
	str = string.format("%d", nZhuang)
	self.m_textZhuangSettle:setString(str)

	str = string.format("%d", nXianDouble)
	self.m_textXianDoubleCount:setString(str)

	str = string.format("%d", nZhuangDouble)
	self.m_textZhuangDoubleCount:setString(str)

	str = string.format("%d", nTotal)
	self.game_count:setString(str)

	self:showLayer(true)
end

-- --设置大路数据
-- function WallBillLayer:setBigRoadData()
-- 	--大路数据
-- 	self.tempBiglist = {};
-- 	local perType = -1;
-- 	local currX = 0;
-- 	local currY = 0;
	
-- 	--大路显示
-- 	self.dataBig = {{}, {}, {}, {}, {}, {}};
-- 	self.bigXLen = 1;
-- 	for i = 1, #self.record do
-- 		--换行
-- 		local turning = false;
-- 		local val = self.record[i].m_cbGameResult;
-- 		if cmd.AREA_PING ~= val then -- 不记录平
-- 			local perX = currX;
-- 	        local perY = currY;
-- 	        if perType == val then
-- 	        	if perY == 6 then  -- 行满时、长龙
-- 	        		currX = perX + 1;
-- 	        		currY = 6;
-- 	        		turning = true;
-- 	        	elseif not self.dataBig[perY + 1][perX] then  --下一行未被占据
-- 	        		if turning then
-- 	        			currX = perX + 1;
-- 	        			currY = perY;
-- 	        		else
-- 	        			currX = perX;
-- 	        			currY = perY + 1;
-- 	        		end
-- 	        	else
-- 	        		currX = perX + 1;
-- 	        		currY = perY;
-- 	        		turning = true;
-- 	        	end 
-- 	        else
-- 	        	turning = false;
-- 	        	local k = 1;
-- 	        	--第一行、换列
-- 	        	while self.dataBig[1][k] do
-- 	        		k = k + 1;
-- 	        	end
-- 	        	currX = k;
-- 	        	if currX < 1 then
-- 	        		currX = 1;
-- 	        	end
-- 	        	currY = 1;
-- 	        end

-- 	        local recordValue = 0;
-- 	        if val == cmd.AREA_XIAN then
-- 	        	recordValue = 2;
-- 	        elseif val == cmd.AREA_ZHUANG then
-- 	        	recordValue = 1;
-- 	        end

-- 	        self.dataBig[currY][currX] = recordValue;
-- 	        perType = val;
-- 	        self.bigXLen = math.max(self.bigXLen, currX);
-- 	        -- table.insert(self.tempBiglist, {recordValue, currX, currY});
-- 		end
-- 	end

-- 	local currRealityX = 0;
-- 	local currRealityY = 0;
-- 	--大路实际记录
-- 	self.dataBigReality = {{}, {}, {}, {}, {}, {}};
-- 	for i = 1, #self.record do
-- 		--换行
-- 		local turning = false;
-- 		local val = self.record[i].m_cbGameResult;
-- 		if cmd.AREA_PING ~= val then -- 不记录平
-- 	        local perRealityX = currRealityX;
-- 	        local perRealityY = currRealityY; 
-- 	        if perType == val then
-- 	        	currRealityX = perRealityX;
-- 	        	if currRealityX < 1 then
-- 	        		currRealityX = 1;
-- 	        	end
-- 	        	currRealityY = perRealityY + 1;
-- 	        else
-- 	        	-- turning = false;
-- 	        	local k = 1;
-- 	        	--第一行、换列
-- 	        	while self.dataBigReality[1][k] do
-- 	        		k = k + 1;
-- 	        	end
-- 	        	currRealityX = k;
-- 	        	if currRealityX < 1 then
-- 	        		currRealityX = 1;
-- 	        	end
-- 	        	currRealityY = 1;
-- 	        end

-- 	        local recordValue = 0;
-- 	        if val == cmd.AREA_XIAN then
-- 	        	recordValue = 2;
-- 	        elseif val == cmd.AREA_ZHUANG then
-- 	        	recordValue = 1;
-- 	        end

-- 	        if self.dataBigReality[currRealityY] == nil then
-- 	        	self.dataBigReality[currRealityY] = {};
-- 	        end
-- 	        self.dataBigReality[currRealityY][currRealityX] = recordValue;
-- 	        perType = val;
	        
-- 	        table.insert(self.tempBiglist, {recordValue, currRealityX, currRealityY});
-- 		end
-- 	end 

-- 	-- dump(self.dataBig,"---------self.dataBig--------",6)
-- 	-- dump(self.tempBiglist,"---------self.tempBiglist--------",6)
-- 	-- dump(self.dataBigReality,"---------self.dataBigReality--------",6)
-- end

-- --获取路单数据、参照开始点、第一行比较列、其他行比较列、看大路
-- function WallBillLayer:getWaybillList(x0, y0, d0, d1, d2)
-- 	local recordTemp = {};
-- 	local bigX = x0;
-- 	local bigY = y0;

-- 	--记录为空
-- 	if self.dataBigReality[bigY][bigX] == nil then
-- 		bigX = bigX + 1;
-- 		bigY = 1;
-- 		if self.dataBigReality[bigY][bigX] == nil then
-- 			return recordTemp;
-- 		end
-- 	end

-- 	local len = #self.tempBiglist;
-- 	local startIndex = 1;
	
-- 	--查找起始元素
-- 	for i = 1, len do
-- 		if self.tempBiglist[i][2] == bigX and self.tempBiglist[i][3] == bigY then
-- 			startIndex = i;
-- 			break;
-- 		end
-- 	end
-- 	-- print(startIndex,d0,d1,"------startIndex--------")

-- 	local index = startIndex;
-- 	local dataBigYLen = #self.dataBigReality;
-- 	print(len,dataBigYLen,"------startIndex--------")
-- 	while index <= len do
-- 		local val = 0;
-- 		bigX = self.tempBiglist[index][2];
-- 		bigY = self.tempBiglist[index][3];
-- 		local delta = d2;
-- 		-- if d0 == 1 then
-- 		-- 	delta = 1;
-- 		-- end
-- 		if bigY == 1 then     -- 看齐整、1红2蓝
-- 			--是否齐整
-- 			local bOrder = false;
-- 			--个数
-- 			local red0 = 0;
-- 			local black0 = 0;
-- 			local red1 = 0;
-- 			local black1 = 0;
-- 			--整列相加
-- 			for k = 1, dataBigYLen do
-- 				if self.dataBigReality[k][bigX - d1] then
-- 					red0 = red0 +  (self.dataBigReality[k][bigX - d1] == 1 and 1 or 0);
-- 					black0 = black0 + (self.dataBigReality[k][bigX - d1] == 2 and 1 or 0);
-- 				end

-- 				if self.dataBigReality[k][bigX - d0] then
-- 					red1 = red1 + (self.dataBigReality[k][bigX - d0] == 1 and 1 or 0);
-- 					black1 = black1 + (self.dataBigReality[k][bigX - d0] == 2 and 1 or 0);
-- 				end
-- 			end
-- 			-- for k = 1, dataBigYLen do
-- 			-- 	if d0 == 0 then -- 当前列
-- 			-- 		if self.dataBigReality[k][bigX - d0] and k == 1 then --只看当前点之前的
-- 			-- 			red1 = red1 + (self.dataBigReality[k][bigX - d0] == 1 and 1 or 0);
-- 			-- 			black1 = black1 + (self.dataBigReality[k][bigX - d0] == 2 and 1 or 0);
-- 			-- 		end
-- 			-- 	else -- 其它列
-- 			-- 		if self.dataBigReality[k][bigX - d0] then
-- 			-- 			red1 = red1 + (self.dataBigReality[k][bigX - d0] == 1 and 1 or 0);
-- 			-- 			black1 = black1 + (self.dataBigReality[k][bigX - d0] == 2 and 1 or 0);
-- 			-- 		end
-- 			-- 	end
-- 			-- end
-- 			-- -- print(red0,black0,red1,black1,"-----------是否齐整-------------")
-- 			-- if red0 == red1 and black0 == black1 then
-- 			-- 	bOrder = true;
-- 			-- end
-- 			-- if red0 == black1 and black0 == red1 then
-- 			-- 	bOrder = true;
-- 			-- end

-- 			if (red0+black0) == (red1+black1) then
-- 				bOrder = true;
-- 			end
-- 			val = bOrder and 1 or 2;
-- 		elseif bigY == 2 then  -- 看有无
-- 			val = self.dataBigReality[bigY][bigX - delta] ~= nil and 1 or 2;
-- 		-- elseif bigY < 6 then   -- 看直落
-- 		-- 	val = (self.dataBigReality[bigY - 1][bigX - delta] == nil and self.dataBigReality[bigY + 1][bigX - delta] == nil) and 1 or 2;
-- 		else                   -- 看直落
-- 			-- val = (self.dataBigReality[bigY - 1][bigX - delta] == nil) and 1 or 2;
-- 			val = ((self.dataBigReality[bigY - 1][bigX - delta] == nil and self.dataBigReality[bigY][bigX - delta] == nil) or 
-- 			      (self.dataBigReality[bigY - 1][bigX - delta] and self.dataBigReality[bigY][bigX - delta])) and 1 or 2;
-- 		end
-- 		table.insert(recordTemp, val);
-- 		index = index + 1;
-- 	end
-- 	return recordTemp;
-- end

-- --显示路单数据
-- --是否显示、各路单记录、路单node、cell图、red、black、列数、记录宽
-- function WallBillLayer:showWaybill(bShow, recordTemp, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
-- 	local lastVal = -1;
-- 	local perType = -1;
-- 	local currX = 0;
-- 	local currY = 0;
-- 	local bigXLen = 1;
	

-- 	local showData = {{}, {}, {}, {}, {}, {}};
-- 	local turning = false;
-- 	for i = 1, #recordTemp do
-- 		local val = recordTemp[i];
-- 		local perX = currX;
--         local perY = currY; 
--         if perType == val then -- 连续
--         	if perY == 6 then  -- 行满时、长龙
--         		currX = perX + 1;
--         		currY = 6;
--         		turning = true;
--         	elseif not showData[perY + 1][perX] then  --下一行未被占据
--         		if turning then
--         			currX = perX + 1;
--         			currY = perY;
--         		else
--         			currX = perX;
--         			currY = perY + 1;
--         		end
--         	else
--         		currX = perX + 1;
--         		currY = perY;
--         		turning = true;
--         	end 
--         else
--         	turning = false;
--         	local k = 1;
--         	--第一行、换列
--         	while showData[1][k] do
--         		k = k + 1;
--         	end
--         	currX = k;
--         	if currX < 1 then
--         		currX = 1;
--         	end
--         	currY = 1;
--         end

--         showData[currY][currX] = val;
--         lastVal = val;
--         perType = val;

--         bigXLen = math.max(bigXLen, currX);
-- 	end 
-- 	if not bShow then
-- 		return lastVal;
-- 	end

-- 	self:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos);

-- 	return lastVal;
-- end

-- function WallBillLayer:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
-- 	--显示路单
-- 	local maxX = 0;
-- 	-- dump(showData,"----------显示路单------------")
-- 	for i = 1 , #showData do
-- 		for j = 1 , bigXLen do
-- 			local val = showData[i][j];
-- 			if val then
-- 				--增加单元格
-- 				if j > maxX and j >= cellCnt then
-- 					local record = cc.Sprite:create(spCellBg);
-- 			    	record:setAnchorPoint(0,0);
-- 			    	record:setPositionX(j * cellWd);
-- 					node:addChild(record);
-- 					node:setInnerContainerSize(cc.size(node:getInnerContainerSize().width + cellWd, node:getInnerContainerSize().height));
-- 					maxX = j;
-- 				end
-- 				--路单
-- 				local item = cc.Sprite:create(1 == val and spRed or spBlack);
-- 				if small then
-- 					item:setPosition(startPos.x + (j-1) * smallW,startPos.y - ((i-1) / 2) * smallW);
-- 				else
-- 					item:setPosition(startPos.x + (j-1) * smallW,startPos.y - (i-1) * smallW);
-- 				end
-- 				node:addChild(item);

-- 				--闪烁
-- 				-- print(i,j,#showData,#showData[1],"--------------闪烁-------------")
-- 				-- if i == #showData and j == #showData[1] then
-- 				-- 	item:runAction(cc.Sequence:create(
-- 				-- 		cc.Blink:create(5, 5), 
-- 				-- 		cc.CallFunc:create( function()
-- 				--             item:setVisible(true);
-- 				--             item:setOpacity(255);
-- 				--         end)
-- 				-- 	));
-- 				-- end
-- 			end
-- 		end
-- 	end
-- end
return WallBillLayer