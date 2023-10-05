--
-- Author: zhong
-- Date: 2016-07-05 11:29:56
--
local GameFrame = class("GameFrame")

local module_pre = "game.yule.baccaratnew.src";
local cmd = require(module_pre .. ".models.CMD_Game")
local bjlDefine = require(module_pre .. ".models.bjlGameDefine")
local GameLogic = require(module_pre .. ".models.GameLogic")

------
--tips
-- lua的table下标是从1开始，所以在处理椅子号的时候 +1
------

function GameFrame:ctor()
    --以座椅号管理
    self.m_tableChairUserList = {}
    --以uid管理
    self.m_tableUidUserList = {}
    --编号管理
    self.m_tableList = {}
    --上庄用户
    self.m_tableApplyList = {}
    --申请上庄数量
    self.m_llApplyCount = 0

    --游戏记录
    self.m_vecRecord = {}
    --路单
    self.m_vecWallBill = {}

    --游戏成绩
    self.m_llTotalScore =  0
    --命中率
    self.m_fGameRate = 0.00
    --游戏总局数
    self.m_nTotalRound = 0
    --赢局数量
    self.m_nTotalWinRound = 0

    --游戏结果
    self.m_tabGameResult = {}
    self.m_bJoin = false
    self.m_tabBetArea = {}
    self.m_tabGameEndCmd = {}
end

------
--游戏玩家管理

--初始化用户列表
function GameFrame:initUserList( userList )
    --以座椅号管理
    self.m_tableChairUserList = {}
    --以uid管理
    self.m_tableUidUserList = {}
    self.m_tableList = {}

    for k,v in pairs(userList) do
        self.m_tableChairUserList[v.wChairID + 1] = v;
        self.m_tableUidUserList[v.dwUserID] = v;
        table.insert(self.m_tableList, v)
    end
end

--增加用户
function GameFrame:addUser( userItem )
    if nil == userItem then
        return;
    end

    self.m_tableChairUserList[userItem.wChairID + 1] = userItem;
    self.m_tableUidUserList[userItem.dwUserID] = userItem;
    
    local user = self:isUserInList(userItem)
    if nil == user then
        table.insert(self.m_tableList, userItem)
    else
        user = userItem
    end 

    print("after add:" .. #self.m_tableList)
end

function GameFrame:updateUser( useritem )
    if nil == useritem then
        return
    end

    local user = self:isUserInList(useritem)
    if nil == user then
        table.insert(self.m_tableList, useritem)
    else
        user = useritem
    end

    self.m_tableChairUserList[useritem.wChairID + 1] = user;
    self.m_tableUidUserList[useritem.dwUserID] = user;
end

function GameFrame:isUserInList( useritem )
    local user = nil
    for k,v in pairs(self.m_tableList) do
        if v.dwUserID == useritem.dwUserID then
            user = useritem
            break
        end
    end
    return user
end

--移除用户
function GameFrame:removeUser( useritem )
    if nil == useritem then
        return
    end

    local deleteidx = nil
    for k,v in pairs(self.m_tableList) do
        local item = v
        if v.dwUserID == useritem.dwUserID then
            deleteidx = k
            break
        end
    end

    if nil ~= deleteidx then
        table.remove(self.m_tableList,deleteidx)
    end

    table.remove(self.m_tableChairUserList,useritem.wChairID + 1)
    table.remove(self.m_tableUidUserList,useritem.dwUserID)

    print("after remove:" .. #self.m_tableList)
end

function GameFrame:removeAllUser(  )
    --以座椅号管理
    self.m_tableChairUserList = {}
    --以uid管理
    self.m_tableUidUserList = {}

    self.m_tableList = {}
    --上庄用户
    self.m_tableApplyList = {}
end

function GameFrame:getChairUserList(  )
    return self.m_tableChairUserList;
end

function GameFrame:getUidUserList(  )
    return self.m_tableUidUserList;
end

function GameFrame:getUserList( )
    return self.m_tableList
end

function GameFrame:sortList(  )
    --排序
    local function sortFun( a,b )
        return a.m_llIdx > b.m_llIdx
    end
    table.sort(self.m_tableApplyList, sortFun)
end

------
--庄家管理

--添加庄家申请用户
function GameFrame:addApplyUser( wchair,bRob )
    local useritem = self.m_tableChairUserList[wchair + 1]
    if nil == useritem then
        return
    end
    bRob = bRob or false

    local info = bjlDefine.getEmptyApplyInfo()
    
    info.m_userItem = useritem
    info.m_bCurrent = useritem.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID
    if bRob then
        --超级抢庄排最前
        info.m_llIdx = -1
    else
        self.m_llApplyCount = self.m_llApplyCount + 1
        if self.m_llApplyCount > yl.MAX_INT then
            self.m_llApplyCount = 0
        end
        info.m_llIdx = self.m_llApplyCount
    end    
    info.m_bRob = bRob

    local user = self:getApplyUser(wchair)
    if nil ~= user then
        user = info
    else
        table.insert(self.m_tableApplyList, info)
    end    

    self:sortList()
end

function GameFrame:getApplyUser( wchair )
    local user = nil
    for k,v in pairs(self.m_tableApplyList) do
        if v.m_userItem.wChairID == wchair then
            user = v
            break
        end
    end
    return user
end

--超级抢庄用户
--rob[抢庄用户] cur[当前用户]
function GameFrame:updateSupperRobBanker( rob,cur )
    if rob ~= cur then
        self:updateSupperRobState(cur, false)
    end
    if nil == self:updateSupperRobState(rob, true) then
        --添加
        self:addApplyUser(rob, true)
    end
    self:sortList()
end

--移除庄家申请用户
function GameFrame:removeApplyUser( wchair )
    local removeIdx = nil
    for k,v in pairs(self.m_tableApplyList) do
        if v.m_userItem.wChairID == wchair then
            removeIdx = k
            break
        end
    end

    if nil ~= removeIdx then
        table.remove(self.m_tableApplyList,removeIdx)
    end

    self:sortList()
end

--更新超级抢庄用户状态
function GameFrame:updateSupperRobState( wchair, state )
    local user = nil
    for k,v in pairs(self.m_tableApplyList) do
        if v.m_userItem.wChairID == wchair then
             --超级抢庄排最前
            v.m_llIdx = -1
            v.m_bRob = state
            user = v
            break
        end
    end
    return user
end

--获取申请庄家列表
function GameFrame:getApplyBankerUserList(  )
    return self.m_tableApplyList
end

------
--路单系统
--减少游戏记录
function GameFrame:subFirstGameRecord()
    table.remove(self.m_vecRecord,1)
end

function GameFrame:subLastGameRecord()
    table.remove(self.m_vecRecord)
end

--增加游戏记录
function GameFrame:addGameRecord( rec )
    table.insert(self.m_vecRecord, rec)
end

--获取游戏记录
function GameFrame:getRecords(  )
    return self.m_vecRecord
end

--获取路单
function GameFrame:getWallBills(  )
    return self.m_vecWallBill
end

--清理记录
function GameFrame:clearRecord(  )
    self.m_vecRecord = {}
    self.m_vecWallBill = {}
end

--在连赢的情况下，从指定索引开始，根据给定输赢结果寻找最近的上一局符合连赢条件的记录
function GameFrame:seekWinList( endIdx, cbResult )
    if cmd.AREA_PING == cbResult then
        return true
    end

    for i = endIdx, 1, -1 do
        local rec = self.m_vecRecord[i]
        --找到相同的即连赢
        if cbResult == rec.m_cbGameResult then
            return true
        end

        --找到一个不相同的就是非连赢
        if (cbResult ~= rec.m_cbGameResult) and (cmd.AREA_PING ~= rec.m_cbGameResult) then
            return false
        end
    end
    return false
end

--找到最后一个连赢记录下标(idx bFound)
function GameFrame:lastWinListIdx( nBegin )
    local idx = 1
    local bFound = false
    for i = nBegin, #self.m_vecWallBill do
        local bill = self.m_vecWallBill[i]
        if (bill.m_bWinList) and (false == bill.m_bJumpIdx) then
            idx = i
            bFound = true
            break
        end
    end
    return idx,bFound
end

--路单计算
--设置大路数据
function GameFrame:setBigRoadData(records)
    --大路数据
    self.tempBiglist = {};
    local perType = -1;
    local currX = 0;
    local currY = 0;
    self.record = records;
    --大路显示
    self.dataBig = {{}, {}, {}, {}, {}, {}};
    self.bigXLen = 1;
    for i = 1, #self.record do
        --换行
        local turning = false;
        local val = self.record[i].m_cbGameResult;
        if cmd.AREA_PING ~= val then -- 不记录平
            local perX = currX;
            local perY = currY;
            if perType == val then
                if perY == 6 then  -- 行满时、长龙
                    currX = perX + 1;
                    currY = 6;
                    turning = true;
                elseif not self.dataBig[perY + 1][perX] then  --下一行未被占据
                    if turning then
                        currX = perX + 1;
                        currY = perY;
                    else
                        currX = perX;
                        currY = perY + 1;
                    end
                else
                    currX = perX + 1;
                    currY = perY;
                    turning = true;
                end 
            else
                turning = false;
                local k = 1;
                --第一行、换列
                while self.dataBig[1][k] do
                    k = k + 1;
                end
                currX = k;
                if currX < 1 then
                    currX = 1;
                end
                currY = 1;
            end

            local recordValue = 0;
            if val == cmd.AREA_XIAN then
                recordValue = 2;
            elseif val == cmd.AREA_ZHUANG then
                recordValue = 1;
            end

            self.dataBig[currY][currX] = recordValue;
            perType = val;
            self.bigXLen = math.max(self.bigXLen, currX);
            -- table.insert(self.tempBiglist, {recordValue, currX, currY});
        end
    end

    local currRealityX = 0;
    local currRealityY = 0;
    --大路实际记录
    self.dataBigReality = {{}, {}, {}, {}, {}, {}};
    for i = 1, #self.record do
        --换行
        local turning = false;
        local val = self.record[i].m_cbGameResult;
        if cmd.AREA_PING ~= val then -- 不记录平
            local perRealityX = currRealityX;
            local perRealityY = currRealityY; 
            if perType == val then
                currRealityX = perRealityX;
                if currRealityX < 1 then
                    currRealityX = 1;
                end
                currRealityY = perRealityY + 1;
            else
                -- turning = false;
                local k = 1;
                --第一行、换列
                while self.dataBigReality[1][k] do
                    k = k + 1;
                end
                currRealityX = k;
                if currRealityX < 1 then
                    currRealityX = 1;
                end
                currRealityY = 1;
            end

            local recordValue = 0;
            if val == cmd.AREA_XIAN then
                recordValue = 2;
            elseif val == cmd.AREA_ZHUANG then
                recordValue = 1;
            end

            if self.dataBigReality[currRealityY] == nil then
                self.dataBigReality[currRealityY] = {};
            end
            self.dataBigReality[currRealityY][currRealityX] = recordValue;
            perType = val;
            
            table.insert(self.tempBiglist, {recordValue, currRealityX, currRealityY});
        end
    end
    return self.dataBig, self.bigXLen, self.dataBigReality, self.tempBiglist;
end

--获取路单数据、参照开始点、第一行比较列、其他行比较列、看大路
function GameFrame:getWaybillList(x0, y0, d0, d1, d2)
    local recordTemp = {};
    local bigX = x0;
    local bigY = y0;

    --记录为空
    if self.dataBigReality[bigY][bigX] == nil then
        bigX = bigX + 1;
        bigY = 1;
        if self.dataBigReality[bigY][bigX] == nil then
            return recordTemp;
        end
    end

    local len = #self.tempBiglist;
    local startIndex = 1;
    
    --查找起始元素
    for i = 1, len do
        if self.tempBiglist[i][2] == bigX and self.tempBiglist[i][3] == bigY then
            startIndex = i;
            break;
        end
    end

    local index = startIndex;
    local dataBigYLen = #self.dataBigReality;
    while index <= len do
        local val = 0;
        bigX = self.tempBiglist[index][2];
        bigY = self.tempBiglist[index][3];
        local delta = d2;
        if bigY == 1 then     -- 看齐整、1红2蓝
            --是否齐整
            local bOrder = false;
            --个数
            local red0 = 0;
            local black0 = 0;
            local red1 = 0;
            local black1 = 0;
            --整列相加
            for k = 1, dataBigYLen do
                if self.dataBigReality[k][bigX - d1] then
                    red0 = red0 +  (self.dataBigReality[k][bigX - d1] == 1 and 1 or 0);
                    black0 = black0 + (self.dataBigReality[k][bigX - d1] == 2 and 1 or 0);
                end

                if self.dataBigReality[k][bigX - d0] then
                    red1 = red1 + (self.dataBigReality[k][bigX - d0] == 1 and 1 or 0);
                    black1 = black1 + (self.dataBigReality[k][bigX - d0] == 2 and 1 or 0);
                end
            end
            if (red0+black0) == (red1+black1) then
                bOrder = true;
            end
            val = bOrder and 1 or 2;
        elseif bigY == 2 then  -- 看有无
            val = self.dataBigReality[bigY][bigX - delta] ~= nil and 1 or 2;
        else                   -- 看直落
            val = ((self.dataBigReality[bigY - 1][bigX - delta] == nil and self.dataBigReality[bigY][bigX - delta] == nil) or 
                  (self.dataBigReality[bigY - 1][bigX - delta] and self.dataBigReality[bigY][bigX - delta])) and 1 or 2;
        end
        table.insert(recordTemp, val);
        index = index + 1;
    end
    return recordTemp;
end

--显示路单数据
--是否显示、各路单记录、路单node、cell图、red、black、列数、记录宽
function GameFrame:showWaybill(bShow, recordTemp, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
    local lastVal = -1;
    local perType = -1;
    local currX = 0;
    local currY = 0;
    local bigXLen = 1;

    local showData = {{}, {}, {}, {}, {}, {}};
    local turning = false;
    for i = 1, #recordTemp do
        local val = recordTemp[i];
        local perX = currX;
        local perY = currY; 
        if perType == val then -- 连续
            if perY == 6 then  -- 行满时、长龙
                currX = perX + 1;
                currY = 6;
                turning = true;
            elseif not showData[perY + 1][perX] then  --下一行未被占据
                if turning then
                    currX = perX + 1;
                    currY = perY;
                else
                    currX = perX;
                    currY = perY + 1;
                end
            else
                currX = perX + 1;
                currY = perY;
                turning = true;
            end 
        else
            turning = false;
            local k = 1;
            --第一行、换列
            while showData[1][k] do
                k = k + 1;
            end
            currX = k;
            if currX < 1 then
                currX = 1;
            end
            currY = 1;
        end

        showData[currY][currX] = val;
        lastVal = val;
        perType = val;

        bigXLen = math.max(bigXLen, currX);
    end 
    if not bShow then
        return lastVal;
    end

    self:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos);
    return lastVal;
end

function GameFrame:showWaybillItem(showData, bigXLen, node, spCellBg, spRed, spBlack, cellCnt, cellWd, small, startPos)
    --显示路单
    local maxX = 0;
    local smallW = 29 --小单元格宽
    for i = 1 , #showData do
        for j = 1 , bigXLen do
            local val = showData[i][j];
            if val then
                --增加单元格
                if j > maxX and j >= cellCnt then
                    local record = cc.Sprite:create(spCellBg);
                    record:setAnchorPoint(0,0);
                    record:setPositionX(j * cellWd);
                    node:addChild(record);
                    node:setInnerContainerSize(cc.size(node:getInnerContainerSize().width + cellWd, node:getInnerContainerSize().height));
                    maxX = j;
                end
                --路单
                local item = cc.Sprite:create(1 == val and spRed or spBlack);
                if small then
                    item:setPosition(startPos.x + (j-1) * smallW,startPos.y - ((i-1) / 2) * smallW);
                else
                    item:setPosition(startPos.x + (j-1) * smallW,startPos.y - (i-1) * smallW);
                end
                node:addChild(item);

                --闪烁
                -- print(i,j,#showData,#showData[1],"--------------闪烁-------------")
                -- if i == #showData and j == #showData[1] then
                --  item:runAction(cc.Sequence:create(
                --      cc.Blink:create(5, 5), 
                --      cc.CallFunc:create( function()
                --             item:setVisible(true);
                --             item:setOpacity(255);
                --         end)
                --  ));
                -- end
            end
        end
    end
end

------
--计算胜率
function GameFrame:calcuteRata( nWin, nLost )
    self.m_nTotalRound = self.m_nTotalRound + nWin + nLost;
    self.m_nTotalWinRound = self.m_nTotalWinRound + nWin;
    
    self.m_fGameRate = self.m_nTotalWinRound / self.m_nTotalRound;
end
------

--计算下注标签
function GameFrame.calcuteJetton( llScore, bAllJetton, betNums)
    local vec = {};

    if bAllJetton then
        --已屏蔽全部下注
    else
        local tmpScore = llScore;
        
        --计算五百万下注数量
        local nCount = math.floor(tmpScore / betNums[6]);
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[6];
            info.m_cbCount = nCount;
            info.m_cbIdx = 6;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[6];
        end
        
        --计算一百万下注数量
        nCount = math.floor(tmpScore / betNums[5]);
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[5];
            info.m_cbCount = nCount;
            info.m_cbIdx = 5;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[5];
        end
        
        --计算十万下注数量
        nCount = math.floor(tmpScore / betNums[4]);
        --print("count:" .. nCount .. "; score:" .. tmpScore )
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[4];
            info.m_cbCount = nCount;
            info.m_cbIdx = 4;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[4];
        end
        
        --计算一万下注数量
        nCount = math.floor(tmpScore / betNums[3]);
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[3];
            info.m_cbCount = nCount;
            info.m_cbIdx = 3;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[3];
        end
        
        --计算一千下注数量
        nCount = math.floor(tmpScore / betNums[2]);
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[2];
            info.m_cbCount = nCount;
            info.m_cbIdx = 2;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[2];
        end
        
        --计算一百下注数量
        nCount = math.floor(tmpScore / betNums[1]);
        if (nCount > 0) then
            local info = {m_cbIdx = 0, m_cbCount = 0, m_llScore = 0};
            info.m_llScore = betNums[1];
            info.m_cbCount = nCount;
            info.m_cbIdx = 1;
            table.insert(vec,info);
            
            tmpScore = tmpScore - nCount * betNums[1];
        end

    end

    return vec;
end
return GameFrame;