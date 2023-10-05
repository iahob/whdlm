--
-- Author: zhong
-- Date: 2017-06-19 12:00:35
--
-- 包含(奖励推送、排行奖励 两个界面)

local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

-- 奖励推送(二级弹窗)
local RewardLayer = class("RewardLayer", TransitionLayer)
-- 排行奖励(二级弹窗)
local RankRewardLayer = class("RankRewardLayer", TransitionLayer)
RewardLayer.RankRewardLayer = RankRewardLayer

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭
    "TAG_MASK",             -- 遮罩
    "BTN_SURE",             -- 领取
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local tabPos = 
{
    {{0.5}},
    {{0.37}, {0.63}}
}
local tabRankType = {}
tabRankType[1] = "财富:"
tabRankType[2] = "消耗:"
tabRankType[4] = "战绩:"

-- 排行奖励
function RankRewardLayer:ctor( scene, param, level )
    RankRewardLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/RankRewardLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("btn_sure")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    if self._param.ntype == 2 then
        local spTitle = spbg:getChildByName("plaza_sp_rankrewards_title_12")
        if spTitle then
            titlefile = "plaza/plaza_sp_rankrewards_title1.png"
            if cc.FileUtils:getInstance():isFileExist(titlefile) then
                spTitle:setSpriteFrame(cc.Sprite:create(titlefile):getSpriteFrame())
            end
        end
    end

    -- 第一名
    -- 排名
    -- self.m_spRankBg = spbg:getChildByName("sp_rank")
    -- self.m_spRankBg:setVisible(false)
    -- self.m_spRankBg:setLocalZOrder(10)

    -- -- id
    -- self.m_txtId = spbg:getChildByName("txt_id")
    -- self.m_txtId:setString("")
    -- -- 排行值
    -- self.m_txtType = spbg:getChildByName("_txt_type")
    -- self.m_txtType:setString("")
    -- self.m_txtRankNum = spbg:getChildByName("txt_ranknum")
    -- self.m_txtRankNum:setString("")
    -- -- 奖励
    -- self.m_txtReward = spbg:getChildByName("txt_reward")
    -- self.m_txtReward:setString("")
    self.myRankNode = spbg:getChildByName("plaza_sp_myRank")

    -- 自己数据
    self.m_tabSelf = nil
    -- 列表
    self.m_tabList = {}
    self.m_nListCount = 0

    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function RankRewardLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_SURE then 

        if nil == self.m_tabSelf then
            self:scaleTransitionOut(self.m_spBg)
        end

        -- -- 领取奖励
        self:showPopWait()
        local dateid = self.m_tabSelf.DateID or 0
        local typeid = self.m_tabSelf.TypeID or 0
        local param = "action=receiverankingaward&" .. GlobalUserItem:urlUserIdSignParam() .. "&dateid=" .. dateid .. "&typeid=" .. typeid
        print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..param)
        local jsonStr = cjson.encode(action)
        LogAsset:getInstance():logData(jsonStr, true)
        appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
            dump(jstable, "jstable", 6)
            local msg = nil
            if type(jstable) == "table" then
                local data = jstable["data"]
                msg = jstable["msg"]
                if type(data) == "table" then
                    local valid = data["valid"]
                    if valid then
                        -- +钻石
                        local ldiamond = tonumber(data["diamond"]) or 0
                        GlobalUserItem.tabAccountInfo.lDiamond = ldiamond

                        -- +金币
                        local ldiamond = tonumber(data["score"]) or 0
                        GlobalUserItem.tabAccountInfo.lUserScore = ldiamond

                        -- 通知更新        
                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY) 
                        eventListener.obj = yl.RY_MSG_USERWEALTH
                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                    end
                end
            end
            if type(msg) == "string" and "" ~= msg then
                showToast(self._scene, msg, 3)
            end
            self:dismissPopWait()
            self:scaleTransitionOut(self.m_spBg)
        end)        
    end
end

function RankRewardLayer:animationRemove()
    --self:scaleTransitionOut(self.m_spBg)
end

function RankRewardLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function RankRewardLayer:onTransitionInFinish()
    local tabList = self._param.tablist or {}
    --dump(tabList, "RankRewardLayer:onTransitionInFinish", 7)
    local scrollList = {}
    -- 我的数据
    local myRank = nil
    
    for k, v in pairs(tabList) do
        -- v.dwCustomID = v.CustomFaceID
        -- v.wFaceID = v.SystemFaceID
        -- -- 头像链接
        -- local faceurl = v["FaceUrl"] or ""
        -- if type(faceurl) == "string" and "" ~= faceurl then
        --     v["bThirdPartyLogin"] = true
        --     v["szThirdPartyUrl"] = faceurl
        -- end
        table.insert(scrollList, v)
        if v.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
            self.m_tabSelf = v
            myRank = v
        end
    end
    if nil ~= myRank then
        -- 创建头像
        local head = HeadSprite:createNormal(myRank, 100)

                --刷新我的排行
        local tmp = self.myRankNode:getChildByName("sp_headbg")
        local head = PopupInfoHead:createNormal(myRank, tmp:getContentSize().width)
        head:setIsGamePop(false)
        head:setPosition(27, 27)
        tmp:addChild(head)

        --昵称
        local tmp = self.myRankNode:getChildByName("txt_name")
        local strNickname = string.EllipsisByConfig(myRank.szNickName, 130, 
                                string.getConfig("fonts/FZHTJW.TTF.ttf", 24))
        tmp:setString(strNickname)

        local txt_norank = self.myRankNode:getChildByName("txt_norank")
        txt_norank:setVisible(false)

        self.myRankNode:getChildByName("txt_rank"):setVisible(true)
        --金钱
        -- self.myRankNode:getChildByName("plaza_sp_rewards"):setVisible(true)
        -- self.myRankNode:getChildByName("txt_Rewards"):setVisible(true)
        self.myRankNode:getChildByName("txt_NoRewards"):setVisible(false)

        --名次
        local txt_rank = self.myRankNode:getChildByName("txt_rank")
        txt_rank:setString("" .. myRank.rank)

        if self._param.ntype == 1 then
            local txt_wintimes = self.myRankNode:getChildByName("txt_wintimes")
            txt_wintimes:setVisible(false)

            local txt_score = self.myRankNode:getChildByName("txt_score")

            if myRank.lRankValue < 10000 then
                txt_score:setString(myRank.lRankValue .. "")
            elseif myRank.lRankValue >= 10000 and myRank.lRankValue < 100000000 then
                local szscore = string.gsub(string.format("%.1f", myRank.lRankValue / 10000), "[.]", ".")
                txt_score:setString(szscore .. "万")
            else
                local szscore = string.gsub(string.format("%.1f", myRank.lRankValue / 100000000), "[.]", ".")
                txt_score:setString(szscore .. "亿")
            end
            txt_score:setVisible(true)

            local sp_rankgold = self.myRankNode:getChildByName("plaza_sp_rankgold")
            sp_rankgold:setVisible(true)

            local txt_wintimes = self.myRankNode:getChildByName("txt_wintimes")
            txt_wintimes:setVisible(false)
        elseif self._param.ntype == 2 then
            local txt_score = self.myRankNode:getChildByName("txt_score")
            txt_score:setVisible(false)

            local sp_rankgold = self.myRankNode:getChildByName("plaza_sp_rankgold")
            sp_rankgold:setVisible(false)

            local txt_wintimes = self.myRankNode:getChildByName("txt_wintimes")
            txt_wintimes:setString("本周胜场:" .. myRank.lRankValue)
            txt_wintimes:setVisible(true)
        end

        --奖励
        local rewardsGold, rewardsDiamond = self:getRewaeds(myRank.rank)
        local rewards_diamond = self.myRankNode:getChildByName("plaza_sp_rewards_diamond")
        local txt_diamond = self.myRankNode:getChildByName("txt_Rewards_diamond")
        local rewards_gold = self.myRankNode:getChildByName("plaza_sp_rewards_gold")
        local txt_Rewards_gold = self.myRankNode:getChildByName("txt_Rewards_gold")

        if rewardsGold > 0 then
            rewards_gold:setVisible(true)
            txt_Rewards_gold:setVisible(true)

            if rewardsDiamond == 0 then
                rewards_diamond:setVisible(false)
                txt_diamond:setVisible(false)

                rewards_gold:setPositionX(375)
                txt_Rewards_gold:setPositionX(375)
            end

            --设置金额
            local str = "/"
            if rewardsGold < 10000 then
                str = str .. rewardsGold
            elseif rewardsGold >= 10000 then
                local szscore = string.gsub(string.format("%.f", rewardsGold / 10000), "[.]", ".")
                str = str .. szscore
            end
            txt_Rewards_gold:setString(str)

            if rewardsGold >= 10000 then
                local length = txt_Rewards_gold:getContentSize().width
                local spWan = cc.Sprite:create("plaza/plaza_sp_rank_numW.png")
                spWan:setPosition(cc.p(txt_Rewards_gold:getPositionX() + length + 10, txt_Rewards_gold:getPositionY()))   --+10为字的宽度
                spWan:addTo(self.myRankNode)
            end
        end

        if rewardsDiamond > 0 then
            rewards_diamond:setVisible(true)
            txt_diamond:setVisible(true)

            txt_diamond:setString("/" .. rewardsDiamond)

            if rewardsGold == 0 then
                rewards_gold:setVisible(false)
                txt_Rewards_gold:setVisible(false)

                rewards_diamond:setPositionX(375)
                txt_diamond:setPositionX(375)
            end
        end

        --没有配置的情况
        if rewardsGold  == 0 and rewardsDiamond == 0 then
                rewards_diamond:setVisible(false)
                txt_diamond:setVisible(false)
                rewards_gold:setVisible(false)
                txt_Rewards_gold:setVisible(false)
        end
    else
              -- 创建头像
        local head = HeadSprite:createNormal(myRank, 100)

                --刷新我的排行
        local tmp = self.myRankNode:getChildByName("sp_headbg")
        local head = PopupInfoHead:createNormal(GlobalUserItem.tabAccountInfo, tmp:getContentSize().width)
        head:setIsGamePop(false)
        head:setPosition(27, 27)
        tmp:addChild(head)

        --昵称
        local tmp = self.myRankNode:getChildByName("txt_name")
        local strNickname = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName, 130, 
                                string.getConfig("fonts/FZHTJW.TTF.ttf", 24))
        tmp:setString(strNickname)

        --名次
        local txt_norank = self.myRankNode:getChildByName("txt_norank")
        txt_norank:setVisible(true)

        self.myRankNode:getChildByName("txt_rank"):setVisible(false)

        --金钱
        --self.myRankNode:getChildByName("plaza_sp_rewards"):setVisible(false)
        --self.myRankNode:getChildByName("txt_Rewards"):setVisible(false)
        self.myRankNode:getChildByName("txt_NoRewards"):setVisible(true)


        if self._param.ntype == 1 then
            local txt_wintimes = self.myRankNode:getChildByName("txt_wintimes")
            txt_wintimes:setVisible(false)

            local txt_score = self.myRankNode:getChildByName("txt_score")
            if GlobalUserItem.tabAccountInfo.lUserScore < 10000 then
                txt_score:setString(GlobalUserItem.tabAccountInfo.lUserScore .. "")
            elseif GlobalUserItem.tabAccountInfo.lUserScore >= 10000 and GlobalUserItem.tabAccountInfo.lUserScore < 100000000 then
                local szscore = string.gsub(string.format("%.f", GlobalUserItem.tabAccountInfo.lUserScore / 10000), "[.]", ",")
                txt_score:setString(szscore .. "万")
            else
                local szscore = string.gsub(string.format("%.f", GlobalUserItem.tabAccountInfo.lUserScore / 100000000), "[.]", ",")
                 txt_score:setString(szscore .. "亿")
            end
            --txt_score:setString(GlobalUserItem.tabAccountInfo.lUserScore .. "")
            txt_score:setVisible(true)

            local sp_rankgold = self.myRankNode:getChildByName("plaza_sp_rankgold")
            sp_rankgold:setVisible(true)
        elseif self._param.ntype == 2 then
            local txt_score = self.myRankNode:getChildByName("txt_score")
            txt_score:setVisible(false)

            local sp_rankgold = self.myRankNode:getChildByName("plaza_sp_rankgold")
            sp_rankgold:setVisible(false)

            local txt_wintimes = self.myRankNode:getChildByName("txt_wintimes")
            txt_wintimes:setString("未上榜")
            txt_wintimes:setVisible(true)
        end
    end
    table.sort( scrollList, function(a, b)
        return a.rank > b.rank
    end )
    self.m_nListCount = #scrollList
    self.m_tabList = scrollList

    -- 列表
    local tmp = self.m_spBg:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = 80
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_spBg)
    --self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()
    self._listView:reloadData()
end

function RankRewardLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function RankRewardLayer:onTransitionOutFinish()
    if type(self.m_removeCallBack) == "function" then
        self.m_removeCallBack()
    end
    self:removeFromParent()
end

-- 子视图大小
function RankRewardLayer:cellSizeForTable(view, idx)
    return view:getViewSize().width, self.m_nCellSize
end

function RankRewardLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local iteminfo = self.m_tabList[idx + 1]
    if nil == iteminfo then
        return cell
    end

       local csb = ExternalFun.loadCSB("plaza/RankItemNode_Rewards.csb", cell)
        csb:setPosition(view:getViewSize().width * 0.5, self.m_nCellSize * 0.5)
        csb:setName("csbNode")

        -- 头像
        local tmp = csb:getChildByName("sp_headbg")
        --local head = HeadSprite:createClipHead(iteminfo, tmp:getContentSize().width)
        local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width)
        head:setIsGamePop(false)
        --dump(iteminfo, "最终玩家详情", 10)

        csb:addChild(head)
        head:setPosition(tmp:getPosition())
        head:setName("head")
        tmp:removeFromParent()


        -- 排行
        local nrank = iteminfo.rank
        local sprank = csb:getChildByName("sp_rank")
        sprank:setVisible(false)
        if nrank > 0 and nrank < 4 then
            local file = "plaza/plaza_sp_rank_" .. nrank .. ".png"
            if cc.FileUtils:getInstance():isFileExist(file) then
                sprank:setSpriteFrame(cc.Sprite:create(file):getSpriteFrame())
                sprank:setVisible(true)
                sprank:setLocalZOrder(1000)
            end
            local sprankIcon = sprank:getChildByName("sp_rankIcon")
            local file = "plaza/plaza_sp_rank" .. nrank .. ".png"
            if cc.FileUtils:getInstance():isFileExist(file) then
                sprankIcon:setSpriteFrame(cc.Sprite:create(file):getSpriteFrame())
            end

            local txtRank = csb:getChildByName("txt_rank")
            txtRank:setVisible(false)
        else
            local txtRank = csb:getChildByName("txt_rank")
            txtRank:setVisible(true)
            txtRank:setString(nrank .. "")
        end

        local btn = csb:getChildByName("btn_rewards")
        btn:setPressedActionEnabled(false)
        btn:setSwallowTouches(false)
        btn:setVisible(false)
        --btn:addTouchEventListener(self.m_itemFunC)

        -- 昵称strNickname
        local tmp = csb:getChildByName("txt_nick") --cc.Label:createWithSystemFont(iteminfo.szNickName, "Arial", 20) -
        local strNickname = string.EllipsisByConfig(iteminfo.szNickName, 130, 
                                string.getConfig("fonts/FZHTJW.TTF.ttf", 24))
        tmp:setString(strNickname)


        -- 分数
        local atlas_score = csb:getChildByName("atlas_score")
        local lScore = iteminfo.lRankValue 

        if lScore < 10000 then
            atlas_score:setString(lScore)
        elseif lScore >= 10000 and lScore < 100000000 then
            local szscore = string.gsub(string.format("%.1f", lScore / 10000), "[.]", ".")
            atlas_score:setString(szscore.."万")
        else
            local szscore = string.gsub(string.format("%.1f", lScore / 100000000), "[.]", ".")
            atlas_score:setString(szscore.."亿")
        end

        --胜局
        local txtWinTimes = csb:getChildByName("txt_score")
        txtWinTimes:setString("本周胜场:" .. iteminfo.lRankValue)

        if self._param.ntype == 1  then

        elseif self._param.ntype == 2 then
            txtWinTimes:setVisible(true)
            atlas_score:setVisible(false)

            --icon
            local spIcon = csb:getChildByName("plaza_sp_rankgold")
            spIcon:setVisible(false)
        end

        --奖励
        local rewardsGold, rewardsDiamond = self:getRewaeds(nrank)
        local rewards_diamond = csb:getChildByName("plaza_sp_rewards_diamond")
        local txt_diamond = csb:getChildByName("txt_Rewards_diamond")
        local rewards_gold = csb:getChildByName("plaza_sp_rewards_gold")
        local txt_Rewards_gold = csb:getChildByName("txt_Rewards_gold")

        if rewardsGold > 0 then
            rewards_gold:setVisible(true)
            txt_Rewards_gold:setVisible(true)

            if rewardsDiamond == 0 then
                rewards_diamond:setVisible(false)
                txt_diamond:setVisible(false)

                rewards_gold:setPositionX(140)
                txt_Rewards_gold:setPositionX(140)
            end

            --设置金额
            local str = "/"
            if rewardsGold < 10000 then
                str = str .. rewardsGold
            elseif rewardsGold >= 10000 then
                local szscore = string.gsub(string.format("%.f", rewardsGold / 10000), "[.]", ".")
                str = str .. szscore
            end
            txt_Rewards_gold:setString(str)

            if rewardsGold >= 10000 then
                local length = txt_Rewards_gold:getContentSize().width
                local spWan = cc.Sprite:create("plaza/plaza_sp_rank_numW.png")
                print("plaza_sp_rank_numW---------->", txt_Rewards_gold:getPositionX() + length + 10, txt_Rewards_gold:getPositionY())
                spWan:setPosition(cc.p(txt_Rewards_gold:getPositionX() + length + 10, txt_Rewards_gold:getPositionY()))   --+10为字的宽度
                spWan:addTo(csb)
            end
        end

        if rewardsDiamond > 0 then
            rewards_diamond:setVisible(true)
            txt_diamond:setVisible(true)

            txt_diamond:setString("/" .. rewardsDiamond)

            if rewardsGold == 0 then
                rewards_gold:setVisible(false)
                txt_Rewards_gold:setVisible(false)

                rewards_diamond:setPositionX(140)
                txt_diamond:setPositionX(140)
            end
        end

    return cell
end

--显示奖励信息
function RankRewardLayer:getRewaeds(rank)
    if nil == rank then
        return 0, 0
    end
       -- 根据奖励类型
    local tabRewards = {}
    if self.m_nValidRank == TAG_ENUM.RANK_SCORE then
        tabRewards = GlobalUserItem.tabWeathRewardConfigList
    elseif self.m_nValidRank == TAG_ENUM.RANK_WINTIMES then
        tabRewards = GlobalUserItem.tabWinTimesRewardConfigList
    end
    --排序
    table.sort(tabRewards, function (a, b)
        -- body
        return a.rankType < b.rankType
    end)

    if rank == 1 and tabRewards[1] then
        return tabRewards[1].Gold, tabRewards[1].Diamond
    elseif rank == 2 and tabRewards[2] then
        return tabRewards[2].Gold, tabRewards[2].Diamond
    elseif rank == 3 and tabRewards[3] then
        return tabRewards[3].Gold, tabRewards[3].Diamond
    elseif rank > 3 and rank < 11 and tabRewards[4] then
        return tabRewards[4].Gold, tabRewards[4].Diamond
    elseif rank >= 11 and rank < 21 and tabRewards[5] then
        return tabRewards[5].Gold, tabRewards[5].Diamond
    elseif rank >= 21 and rank < 51 and tabRewards[6] then
        return tabRewards[6].Gold, tabRewards[6].Diamond
    elseif rank >= 51 and rank <= 100 and tabRewards[7] then
        return tabRewards[7].Gold, tabRewards[7].Diamond
    end

    return 0,0
end

-- 子视图数目
function RankRewardLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

-- 奖励推送
function RewardLayer:ctor( scene, param, level )
    RewardLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/RewardLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 领取按钮
    local btn = spbg:getChildByName("btn_sure")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --title动画
    -- local spAction = spbg:getChildByName("sp_action")
    -- local animation = cc.Animation:create()
    -- local times = 0.5
    -- for m = 1, 9 do
    --     local strPath = "plaza/"..string.format("plaza_sp_rewardstitaction%d.png", m)
    --     print("===========>", strPath)
    --     local spriteFrame = cc.Sprite:create(strPath):getSpriteFrame()
    --     if spriteFrame then
    --         animation:addSpriteFrame(spriteFrame)
    --     else
    --         break
    --     end
    --     animation:setLoops(-1)
    --     animation:setDelayPerUnit(times)
    -- end
    -- local animate = cc.Animate:create(animation)
    -- spAction:runAction(animate)
    local m_actAni = ExternalFun.loadTimeLine( "plaza/RewardLayer.csb")
    m_actAni:gotoFrameAndPlay(0,true)
    csbNode:runAction(m_actAni)

    -- 光动画
    --spbg:getChildByName("sp_rewardlight"):runAction(cc.RepeatForever:create(cc.RotateBy:create(20 , 360)))

    local sztips = self._param.sztips or ""
    -- 提示文本
    spbg:getChildByName("txt_tips"):setString(sztips) 

    -- 钻石奖励
    spbg:getChildByName("sp_diamond"):setVisible(false)

    -- 喇叭奖励
    spbg:getChildByName("sp_trumpet"):setVisible(false)

    local bgwidth = spbg:getContentSize().width
    local list = {}
    -- 奖励钻石
    local diamond = self._param.ldiamond or 0
    if 0 ~= diamond then
        table.insert(list, "sp_diamond")
    end
    -- 奖励喇叭
    local trumpet = self._param.npropertyid or 0
    if 0 ~= trumpet then
        table.insert(list, "sp_trumpet")
    end
    local ncount = #list
    if ncount > 0 then
        local pos = tabPos[ncount]
        for k,v in pairs(list) do
            local xPos = pos[k][1] * bgwidth
            local sp = spbg:getChildByName(v)
            if nil ~= sp then
                sp:setVisible(true)
                sp:setPositionX(xPos)
            end
        end
    end

    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function RewardLayer:onButtonClickedEvent(tag,sender)
    if TAG_ENUM.BTN_SURE == tag then
        -- 判断领取
        if yl.REWARD_LIST.SPREAD_REWARD == self._param.ntype then
            self:onGetSpreadReward()
        elseif yl.REWARD_LIST.REGISTER_REWARD == self._param.ntype then
            self:onGetRegisterReward()
        else
            self:scaleTransitionOut(self.m_spBg)
        end
    end
end

function RewardLayer:animationRemove()
    --self:scaleTransitionOut(self.m_spBg)
end

function RewardLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function RewardLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function RewardLayer:onTransitionOutFinish()
    if type(self.m_removeCallBack) == "function" then
        self.m_removeCallBack()
    end
    self:removeFromParent()
end

function RewardLayer:onGetSpreadReward()
    self:showPopWait()
    local configid = self._param.nconfigid or 0
    local param = "action=receivespreadaward&" .. GlobalUserItem:urlUserIdSignParam() .. "&configid=" .. configid
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "jstable", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    -- +钻石
                    local ldiamond = tonumber(self._param.ldiamond) or 0
                    GlobalUserItem.tabAccountInfo.lDiamond = GlobalUserItem.tabAccountInfo.lDiamond + ldiamond

                    -- +喇叭
                    local ntrumpet = self._param.npropertycount or 0
                    GlobalUserItem.nLargeTrumpetCount = GlobalUserItem.nLargeTrumpetCount + ntrumpet

                    -- 通知更新        
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                    eventListener.obj = yl.RY_MSG_USERWEALTH
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                end
            end
        end
        if type(msg) == "string" and "" ~= msg then
            showToast(self._scene, msg, 3)
        end
        self:dismissPopWait()
        self:scaleTransitionOut(self.m_spBg)
    end)
end

function RewardLayer:onGetRegisterReward()
    self:showPopWait()
    local param = "action=receiveregistergrant&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "jstable", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    -- + 钻石
                    local diamond = tonumber(data["diamond"])
                    if nil ~= diamond then
                        GlobalUserItem.tabAccountInfo.lDiamond = diamond
                    end
                    -- + 金币(身上金币)
                    local score = tonumber(data["score"])
                    if nil ~= score then
                        GlobalUserItem.tabAccountInfo.lUserScore = score
                    end
                    -- 通知更新        
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                    eventListener.obj = yl.RY_MSG_USERWEALTH
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                end
            end
        end
        if type(msg) == "string" and "" ~= msg then
            showToast(self._scene, msg, 3)
        end
        self:dismissPopWait()
        self:scaleTransitionOut(self.m_spBg)
    end)
end

return RewardLayer