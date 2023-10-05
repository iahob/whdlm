--
-- Author: zhong
-- Date: 2017-05-31 18:10:25
--
-- 排行榜(非弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local RankListLayer = class("RankListLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local AnimationMgr = appdf.req(appdf.EXTERNAL_SRC .. "AnimationMgr")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local TAG_START             = 100
local enumTable = 
{
    "BT_SHOWRANK",          -- 显示排行
    "BT_HIDERANK",          -- 隐藏排行
    "BT_SCORERANK",         -- 财富榜
    "BT_BATTLERANK",        -- 战绩榜
    "BT_CONSUMERANK",       -- 消耗榜
    "RANK_NONE",            -- 无
    "RANK_SCORE",           -- 财富榜
    "RANK_BATTLE",          -- 战绩榜
    "RANK_CONSUME",         -- 消费榜

    "RANK_WINTIMES",         --胜局榜
    "BT_WINTIMES",          -- 胜局榜
    "BT_HELP",              -- 帮助说明
    "BT_MYRANK",             -- 我的排行
    "BT_NORMALRANK",         -- 简约排行
    "BT_CLOSE",         -- 关闭
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
RankListLayer.RANK_SCORE = TAG_ENUM.RANK_SCORE
RankListLayer.RANK_BATTLE = TAG_ENUM.RANK_BATTLE
RankListLayer.RANK_CONSUME = TAG_ENUM.RANK_CONSUME
RankListLayer.RANK_WINTIMES = TAG_ENUM.RANK_WINTIMES

function RankListLayer:ctor( scene,param,level )
    RankListLayer.super.ctor(self, scene, param, level)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/RankListLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self.m_rankShowListen = nil

    -- 激活的排行榜
    self.m_nValidRank = RankListLayer.RANK_SCORE
    -- 排行榜
     local sprank = csbNode:getChildByName("plaza_sp_rankbg")
    -- sprank:setPositionX(10)
    self.m_spRank = sprank
    
    -- 排行遮罩
    self.m_rankMask = csbNode:getChildByName("panel_mask")
    self.m_rankMask:setTag(TAG_ENUM.BT_CLOSE)
    self.m_rankMask:addTouchEventListener(touchFunC)
    --self.m_rankMask:setVisible(true)

    sprank:getChildByName("txt_tips"):setVisible(true)
    self.m_spRank:getChildByName("txt_tips"):setString("每日24点更新")
    -- 显示排行榜
    btn = sprank:getChildByName("btn_swithrank")
    btn:setTag(TAG_ENUM.BT_SHOWRANK)
    btn:addTouchEventListener(touchFunC)
    btn:setLocalZOrder(1)
    self.m_btnSwitchRank = btn

    -- 财富榜
    btn = sprank:getChildByName("tabs_bg_6"):getChildByName("btn_rank_1")
    btn:setTag(TAG_ENUM.BT_SCORERANK)
    btn:addTouchEventListener(touchFunC)
    btn:setLocalZOrder(1)
    btn.nRankType = RankListLayer.RANK_SCORE
    self.m_btnScoreRank = btn

    -- 胜局榜
    btn = sprank:getChildByName("tabs_bg_6"):getChildByName("btn_rank_2")
    btn:setTag(TAG_ENUM.BT_WINTIMES)
    btn:addTouchEventListener(touchFunC)
    btn:setLocalZOrder(1)
    btn.nRankType = RankListLayer.RANK_WINTIMES
    self.m_btnWinTimesRank = btn

    btn = sprank:getChildByName("btn_tips")
    btn:setTag(TAG_ENUM.BT_HELP)
    btn:addTouchEventListener(touchFunC)

    btn = sprank:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BT_CLOSE)
    btn:addTouchEventListener(touchFunC)

    self.m_rankBtnBg = sprank:getChildByName("tabs_bg_6")

    -- btn = sprank:getChildByName("btn_myRank")
    -- btn:setTag(TAG_ENUM.BT_MYRANK)
    -- btn:addTouchEventListener(touchFunC)

    self.listTypeTxt = sprank:getChildByName("Image_1"):getChildByName("Text_1_1")
    self.listTypeTxt:setString("金币")
    self.listTypeTxt:setPositionX(667)

    self.listRewardTxt = sprank:getChildByName("Image_1"):getChildByName("Text_1_0")
    self.listRewardTxt:setVisible(false)

    -- self.m_spScoreRankTitle = sprank:getChildByName("plaza_rank_1")
    -- self.m_spScoreRankTitle:setLocalZOrder(1)
    -- self.m_spScoreRankTitle:setVisible(false)
    -- 战绩榜
    -- btn = sprank:getChildByName("btn_rank_2")
    -- btn:setTag(TAG_ENUM.BT_BATTLERANK)
    -- btn:addTouchEventListener(touchFunC)
    -- btn:setLocalZOrder(1)
    -- btn:setEnabled(false)
    -- btn:setVisible(false)
    -- btn.nRankType = RankListLayer.RANK_BATTLE
    -- self.m_btnChargeRank = btn
    -- self.m_spChargeRankTitle = sprank:getChildByName("plaza_rank_2")
    -- self.m_spChargeRankTitle:setLocalZOrder(1)
    -- self.m_spChargeRankTitle:setVisible(false)
    -- -- 消耗榜
    -- btn = sprank:getChildByName("btn_rank_3")
    -- btn:setTag(TAG_ENUM.BT_CONSUMERANK)
    -- btn:addTouchEventListener(touchFunC)
    -- btn:setLocalZOrder(1)
    -- btn:setEnabled(false)
    -- btn:setVisible(false)
    -- btn.nRankType = RankListLayer.RANK_CONSUME
    -- self.m_btnConsumeRank = btn
    -- self.m_spConsumeRankTitle = sprank:getChildByName("plaza_rank_3")
    -- self.m_spConsumeRankTitle:setLocalZOrder(1)
    -- self.m_spConsumeRankTitle:setVisible(false)

    --我的排行信息
    self.myRankInfo = nil



    -- 列表
    local tmpcontent = sprank:getChildByName("content")
    local contentsize = tmpcontent:getContentSize()
    self.m_tabList = {}
    self.m_nListCount = 0
    self.m_nCellSize = 80
    self._listView = cc.TableView:create(contentsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listView:setPosition(tmpcontent:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(sprank)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)

    -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listView:getViewSize().width, self._listView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end
    self._listView:reloadData()
    tmpcontent:removeFromParent()


    --列表2
    self.normalList = sprank:getChildByName("Img_list")
    -- self.normalList:setTag(TAG_ENUM.BT_NORMALRANK)
    -- self.normalList:addTouchEventListener(touchFunC)

    self.normalList:setLocalZOrder(1000)
    local tmpcontent1 = self.normalList:getChildByName("content1")
    local contentsize = tmpcontent1:getContentSize()
    self._listNormalView = cc.TableView:create(contentsize)
    self._listNormalView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listNormalView:setPosition(cc.p(-3, 42))
    self._listNormalView:setDelegate()
    self._listNormalView:addTo(self.normalList)
    self._listNormalView:registerScriptHandler(handler(self, self.cellSizeForTableNormal), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listNormalView:registerScriptHandler(handler(self, self.tableCellAtIndexNormal), cc.TABLECELL_SIZE_AT_INDEX)
    self._listNormalView:registerScriptHandler(handler(self, self.numberOfCellsInTableViewNormal), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listNormalView:registerScriptHandler(handler(self, self.tableCellTouchedNormal), cc.TABLECELL_TOUCHED)

    -- 按钮
    self.m_itemNormalListFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listNormalView then
                return
            end

            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listNormalView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listNormalView:getViewSize().width, self._listNormalView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onNormalListItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end
    self._listNormalView:reloadData()
    tmpcontent1:removeFromParent()

    local btn = self.normalList:getChildByName("btn")
    btn:setTag(TAG_ENUM.BT_NORMALRANK)
    btn:addTouchEventListener(touchFunC)

    self.normalListTitle = self.normalList:getChildByName("img_title")

    self.Rank_tips = sprank:getChildByName("rank_tips")
    self.Rank_tips:setLocalZOrder(100)


    self.Panel_2 = sprank:getChildByName("Panel_2")
    self.Panel_2:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                self.Panel_2:setVisible(false)
                self.Rank_tips:setVisible(false)
        end
    end)

    self.myRankNode = sprank:getChildByName("plaza_sp_myRank")

    -- 加载
    self.m_panelMask = sprank:getChildByName("panel_mask")
    self.m_spWait = self.m_panelMask:getChildByName("sp_wait")
    self.m_panelMask:setLocalZOrder(self._listView:getLocalZOrder() + 1)
    self.m_panelMask:setVisible(false)

    self:adjustRank()


end

function RankListLayer:onItemButtonClickedEvent( tag, ref )
    if self.Rank_tips:isVisible() then
        self.Rank_tips:setVisible(false)
    else
        self:showRewardsInfo()
        self.Rank_tips:setVisible(true)
        self.Panel_2:setVisible(true)
    end
end

function RankListLayer:onNormalListItemButtonClickedEvent( tag, ref )
    self.normalList:setVisible(false)
    self.m_btnSwitchRank:setEnabled(false)
    self.m_btnSwitchRank:setVisible(true)
    self.m_btnSwitchRank:setTag(TAG_ENUM.BT_HIDERANK)
    self.m_btnSwitchRank:loadTextureDisabled("plaza/plaza_btn_hiderank_0.png")
    self.m_btnSwitchRank:loadTextureNormal("plaza/plaza_btn_hiderank_0.png")
    self.m_btnSwitchRank:loadTexturePressed("plaza/plaza_btn_hiderank_0.png")

    self.m_spRank:stopAllActions()
    if type(self.m_rankShowListen) == "function" then
        self.m_rankShowListen(true, function()
            -- 切换动画
            local m = cc.MoveTo:create(0.3, cc.p(self.m_spRank:getContentSize().width + 3, self.m_spRank:getPositionY()))
            local em = cc.EaseBackIn:create(m)
            local c = cc.CallFunc:create(function()
                self.m_rankMask:setVisible(true)
                self.m_btnSwitchRank:setEnabled(true)
                self:refreshList(self.m_nValidRank)
            end)
            local seq = cc.Sequence:create(em, cc.DelayTime:create(0.2), c)
            self.m_spRank:runAction(seq)
        end)
    end
end



function RankListLayer:setRankShowListener( listener )
    self.m_rankShowListen = listener
end

function RankListLayer:getNormalList()
    self:showWait()
    local param = string.format("action=getrankingdata&%s&typeid=%d", GlobalUserItem:urlUnLoginSignParam(), self.m_nValidRank == RankListLayer.RANK_SCORE and 1 or 2)
    self:doHttpRequest(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param)
end

function RankListLayer:showWait()
    self.m_panelMask:setVisible(true)
    self.m_spWait:setVisible(false)
    self.m_spWait:stopAllActions()
    self.m_spWait:runAction(cc.Sequence:create(
        cc.DelayTime:create(1.0),
        cc.CallFunc:create(function()
            self.m_spWait:setVisible(true)
            local param = AnimationMgr.getAnimationParam()
            param.m_fDelay = 0.01
            -- 动画
            param.m_strName = yl.COMMON_LOADING_ANI
            param.m_bRestore = true
            local animate = AnimationMgr.getAnimate(param)
            if nil ~= animate then
                local rep = cc.RepeatForever:create(animate)
                self.m_spWait:runAction(rep)
            end
        end)))
end

function RankListLayer:hideWait()
    self.m_spWait:stopAllActions()
    self.m_panelMask:setVisible(false)
end

function RankListLayer:onExit()
    RankListLayer.super.onExit(self)
end

function RankListLayer:onButtonClickedEvent(tag, ref)
    if TAG_ENUM.BT_SHOWRANK == tag then                             -- 显示排行榜
        self.m_btnSwitchRank:setEnabled(false)
        self.m_btnSwitchRank:setVisible(true) 
        self.m_btnSwitchRank:setTag(TAG_ENUM.BT_HIDERANK)
        self.m_btnSwitchRank:loadTextureDisabled("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTextureNormal("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTexturePressed("plaza/plaza_btn_hiderank_0.png")

        self.m_spRank:stopAllActions()
        if type(self.m_rankShowListen) == "function" then
            self.m_rankShowListen(true, function()
                -- 切换动画
                local m = cc.MoveTo:create(0.3, cc.p(self.m_spRank:getContentSize().width + 3, self.m_spRank:getPositionY()))
                local em = cc.EaseBackIn:create(m)
                local c = cc.CallFunc:create(function()
                    self.m_rankMask:setVisible(true)
                    self.m_btnSwitchRank:setEnabled(true)
                    self:refreshList(self.m_nValidRank)
                end)
                local seq = cc.Sequence:create(em, cc.DelayTime:create(0.2), c)
                self.m_spRank:runAction(seq)
            end)
        end
    elseif TAG_ENUM.BT_HIDERANK == tag then
        if self.Rank_tips:isVisible() then
            self.Rank_tips:setVisible(false)
            return
        end   

        -- 隐藏排行榜
        self.m_rankMask:setVisible(false)
        self.m_btnSwitchRank:setEnabled(false)
        self.m_btnSwitchRank:setVisible(false) 
        self.normalList:setVisible(true)
        self.m_btnSwitchRank:setTag(TAG_ENUM.BT_SHOWRANK)
        self.m_btnSwitchRank:loadTextureDisabled("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTextureNormal("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTexturePressed("plaza/plaza_btn_hiderank_0.png") 

        self.m_spRank:stopAllActions()
        -- 切换动画
        local m = cc.MoveTo:create(0.3, cc.p(13, self.m_spRank:getPositionY()))
        local em = cc.EaseBackOut:create(m)
        local c = cc.CallFunc:create(function()
            if type(self.m_rankShowListen) == "function" then
                self.m_rankShowListen(false, function()
                    self.m_btnSwitchRank:setEnabled(true) 
                end)
            end
        end)
        local seq = cc.Sequence:create(em, cc.DelayTime:create(0.2), c)
        self.m_spRank:runAction(seq)
    elseif TAG_ENUM.BT_SCORERANK == tag then                        -- 财富榜
        self.Rank_tips:setVisible(false)
        if self.m_nValidRank == RankListLayer.RANK_SCORE then
            return
        end
        --self:changeTitleState(true, false, false)
        self.listTypeTxt:setString("金币")
        self.listTypeTxt:setPositionX(667)
        self.listRewardTxt:setVisible(false)
        --按钮状态
        self.m_btnScoreRank:loadTextureDisabled("rank/check_day1.png")
        self.m_btnScoreRank:loadTextureNormal("rank/check_day1.png")
        self.m_btnScoreRank:loadTexturePressed("rank/check_day2.png")

        self.m_btnWinTimesRank:loadTextureDisabled("rank/check_week2.png")
        self.m_btnWinTimesRank:loadTextureNormal("rank/check_week2.png")
        self.m_btnWinTimesRank:loadTexturePressed("rank/check_week1.png")
        --self.m_spRank:getChildByName("txt_tips"):setVisible(false)
        self.m_spRank:getChildByName("txt_tips"):setString("每日24点更新")
        self.m_nValidRank = RankListLayer.RANK_SCORE
        self:animationRefresh()
    elseif TAG_ENUM.BT_BATTLERANK == tag then                       -- 战绩榜
        if self.m_spChargeRankTitle:isVisible() then
            return
        end
        self:changeTitleState(false, true, false)
        self.m_nValidRank = RankListLayer.RANK_BATTLE
        self:animationRefresh()
    elseif TAG_ENUM.BT_CONSUMERANK == tag then                      -- 消耗榜
        if self.m_spConsumeRankTitle:isVisible() then
            return
        end
        self:changeTitleState(false, false, true)
        self.m_nValidRank = RankListLayer.RANK_CONSUME
        self:animationRefresh()
    elseif TAG_ENUM.BT_WINTIMES == tag then 
        self.Rank_tips:setVisible(false)
        if self.m_nValidRank == RankListLayer.RANK_WINTIMES then
            return
        end
        self.listTypeTxt:setString("胜局")
        self.listTypeTxt:setPositionX(480)
        self.listRewardTxt:setVisible(true)
        self.m_btnWinTimesRank:loadTextureDisabled("rank/check_week1.png")
        self.m_btnWinTimesRank:loadTextureNormal("rank/check_week1.png")
        self.m_btnWinTimesRank:loadTexturePressed("rank/check_week2.png")

        self.m_btnScoreRank:loadTextureDisabled("rank/check_day2.png")
        self.m_btnScoreRank:loadTextureNormal("rank/check_day2.png")
        self.m_btnScoreRank:loadTexturePressed("rank/check_day1.png")
        --self.m_spRank:getChildByName("txt_tips"):setVisible(true)
        self.m_spRank:getChildByName("txt_tips"):setString("周六晚24点结算奖励")
        self.m_nValidRank = RankListLayer.RANK_WINTIMES
        self:animationRefresh()
    elseif TAG_ENUM.BT_MYRANK == tag then 
        self.Rank_tips:setVisible(false)
        --
        if nil ~= self.myRankInfo then
            local rank = self.myRankInfo.rank
        else
            showToast(self, "您未上榜！", 2)
        end
  
    elseif TAG_ENUM.BT_CLOSE == tag then 
        self:removeFromParent()
    elseif TAG_ENUM.BT_NORMALRANK == tag then 
        self.normalList:setVisible(false)

        self.m_btnSwitchRank:setEnabled(false)
        self.m_btnSwitchRank:setVisible(true) 
        self.m_btnSwitchRank:setTag(TAG_ENUM.BT_HIDERANK)
        self.m_btnSwitchRank:loadTextureDisabled("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTextureNormal("plaza/plaza_btn_hiderank_0.png")
        self.m_btnSwitchRank:loadTexturePressed("plaza/plaza_btn_hiderank_0.png")

        self.m_spRank:stopAllActions()
        if type(self.m_rankShowListen) == "function" then
            self.m_rankShowListen(true, function()
                -- 切换动画
                local m = cc.MoveTo:create(0.3, cc.p(self.m_spRank:getContentSize().width + 3, self.m_spRank:getPositionY()))
                local em = cc.EaseBackIn:create(m)
                local c = cc.CallFunc:create(function()
                    self.m_rankMask:setVisible(true)
                    self.m_btnSwitchRank:setEnabled(true)
                    self:refreshList(self.m_nValidRank)
                end)
                local seq = cc.Sequence:create(em, cc.DelayTime:create(0.2), c)
                self.m_spRank:runAction(seq)
            end)
        end
    end
end

function RankListLayer:changeTitleState( bShowScore, bShowBattle, bShowConsume)
    self.m_spConsumeRankTitle:setVisible(bShowConsume)
    self.m_spChargeRankTitle:setVisible(bShowBattle)
    self.m_spScoreRankTitle:setVisible(bShowScore)
end

function RankListLayer:animationRefresh()
    self.m_panelMask:setVisible(true)
    self.m_spWait:setVisible(true)
    self.m_spWait:stopAllActions()

    local param = AnimationMgr.getAnimationParam()
    param.m_fDelay = 0.005
    -- 火箭动画
    param.m_strName = yl.COMMON_LOADING_ANI
    param.m_bRestore = true
    local animate = AnimationMgr.getAnimate(param)
    if nil ~= animate then
        self.m_spWait:runAction(cc.Sequence:create(animate, cc.CallFunc:create(function()
            self:refreshList(self.m_nValidRank)
            self.m_panelMask:setVisible(false)
        end)))
    end
    
end

function RankListLayer:onHttpRequestFinish( jstable, jsdata, allparam )
    --dump(jstable, "desciption", 6)
    local fun = function( jsTab )
        local t = {}
        if type(jsTab) ~= "table" then
            return t
        end
        for k,v in pairs(jsTab) do
            local item = {}
            -- 昵称
            item["szNickName"] = v["NickName"] or ""
            -- 排行
            item["rank"] = v["RankNum"] or 100
            -- 钻石
            --item["lScore"] = v["Diamond"] or 0
            -- 用户id
            item["dwUserID"] = v["UserID"] or 0
            -- 战绩
            item["lScore"] = v["Gold"] or 0
            -- 头像
            item["wFaceID"] = v["FaceID"] or 0
            -- 自定义头像
            --item["dwCustomID"] = v["CustomFaceID"] or 0
            --GameID
            item["dwGameID"] = v["GameID"]

            item["szSign"] = v["UnderWrite"]

            item["winTimes"] = v["WinCount"] or 0

            item["cbGender"] = v["Gender"] or 0

            --IP          
            --item["szIpAddress"] = v["LastLogonAddress"]lScore
            -- 头像链接
            local faceurl = v["FaceUrl"] or ""
            if type(faceurl) == "string" and "" ~= faceurl then
                item["bThirdPartyLogin"] = true
                item["szThirdPartyUrl"] = faceurl
            end
            -- if item["dwUserID"] == GlobalUserItem.tabAccountInfo.dwUserID then --我在排行榜上
            --     if self.m_nValidRank == RankListLayer.RANK_WINTIMES then
            --         self.myRankInfo[2] = item
            --     elseif self.m_nValidRank == TAG_ENUM.RANK_SCORE then
            --         self.myRankInfo[1] = clone(item)
            --         dump(self.myRankInfo[1], "我已上榜，感觉良好！！！", 7)
            --     end
            -- end
            table.insert(t, item)
        end
        table.sort(t, function(a,b)
            return a.rank > b.rank
        end)
        return t
    end
    local msg = nil
    if type(jstable) == "table" then
        local data = jstable["data"]
        msg = jstable["msg"]
        if type(data) == "table" then
            logFunc(data, true)
            local valid = data["valid"]
            if valid then
                if false == data["IsRank"] then  
                    self.myRankInfo = nil
                end

                GlobalUserItem.m_tabCacheRankListData = {}
                -- 财富榜
                GlobalUserItem.m_tabCacheRankListData["WealthRank"] = fun(data["WealthRank"])
                -- -- 消耗榜
                -- GlobalUserItem.m_tabCacheRankListData["ConsumeRank"] = fun(data["ConsumeRank"])
                -- -- 战绩榜
                -- GlobalUserItem.m_tabCacheRankListData["ScoreRank"] = fun(data["ScoreRank"])

                -- 胜负榜
                GlobalUserItem.m_tabCacheRankListData["winRank"] = fun(data["GameRank"])
                self:refreshList( self.m_nValidRank )
            end
        end
    end
    self:hideWait()



    if type(msg) == "string" and "" ~= msg then
        showToast(self, msg, 1)
    end
end

function RankListLayer:refreshList( rankType )
    if  TAG_ENUM.RANK_SCORE == rankType and nil == GlobalUserItem.m_tabCacheRankListData then
        self:showWait()
        local param = string.format("action=getrankingdata&%s&typeid=%d", GlobalUserItem:urlUnLoginSignParam(), 1)
        self:doHttpRequest(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param)
    elseif TAG_ENUM.RANK_SCORE == rankType and 0 == #GlobalUserItem.m_tabCacheRankListData["WealthRank"] then
        self:showWait()
        local param = string.format("action=getrankingdata&%s&typeid=%d", GlobalUserItem:urlUnLoginSignParam(), 1)
        self:doHttpRequest(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param)
    elseif TAG_ENUM.RANK_WINTIMES == rankType and 0 == #GlobalUserItem.m_tabCacheRankListData["winRank"] then
        self:showWait()
        local param = string.format("action=getrankingdata&%s&typeid=%d", GlobalUserItem:urlUnLoginSignParam(), 2)
        self:doHttpRequest(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param)
    else
        self.m_nListCount = 0
        self.m_tabList = {}
        self.myRankInfo = nil 
        if nil == rankType or TAG_ENUM.RANK_NONE == rankType then
            print("RankListLayer:refreshList 无排行榜")
            return
        elseif TAG_ENUM.RANK_SCORE == rankType then
            if nil ~= GlobalUserItem.m_tabCacheRankListData["WealthRank"] then
                self.m_tabList = GlobalUserItem.m_tabCacheRankListData["WealthRank"]
                for i,v in ipairs(self.m_tabList) do
                    if v.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                        self.myRankInfo = v
                        break
                    end
                end
            end

            --刷新title
            self.normalListTitle:loadTexture("plaza/plaza_sp_wealthtitle.png")

            -- if nil ~= self.myRankInfo and nil ~= self.myRankInfo[1] then
            --     myRank = self.myRankInfo[1] 
            -- end
        elseif TAG_ENUM.RANK_BATTLE == rankType then
            if nil ~= GlobalUserItem.m_tabCacheRankListData["ScoreRank"] then
                self.m_tabList = GlobalUserItem.m_tabCacheRankListData["ScoreRank"]
            end
        elseif TAG_ENUM.RANK_CONSUME == rankType then
            if nil ~= GlobalUserItem.m_tabCacheRankListData["ConsumeRank"] then
                self.m_tabList = GlobalUserItem.m_tabCacheRankListData["ConsumeRank"]
            end
        elseif TAG_ENUM.RANK_WINTIMES == rankType then
            if nil ~= GlobalUserItem.m_tabCacheRankListData["winRank"] then
                self.m_tabList = GlobalUserItem.m_tabCacheRankListData["winRank"]
                for i,v in ipairs(self.m_tabList) do
                    if v.dwUserID == GlobalUserItem.tabAccountInfo.dwUserID then
                        self.myRankInfo = v
                        break
                    end
                end
            end

            --刷新title
            self.normalListTitle:loadTexture("plaza/plaza_sp_wintitle.png")
        end
        self.m_nListCount = #self.m_tabList
        self._listView:reloadData()
        self._listNormalView:reloadData()
        if self.m_nListCount == 0 then
            self._listView:setVisible(false)
        else
            self._listView:setVisible(true)
        end
        

        --刷新我的排行
        local tmp = self.myRankNode:getChildByName("sp_headbg")
        local head = PopupInfoHead:createNormal(GlobalUserItem.tabAccountInfo, tmp:getContentSize().width)
        head:setIsGamePop(false)
        head:setPosition(28, 27)
        tmp:addChild(head)

        --昵称
        local tmp = self.myRankNode:getChildByName("txt_name")
        local strNickname = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName, 130, 
                                string.getConfig("fonts/FZHTJW.TTF", 24))
        tmp:setString(strNickname)

        local rewards_diamond = self.myRankNode:getChildByName("plaza_sp_rewards_diamond")
        local txt_diamond = self.myRankNode:getChildByName("txt_Rewards_diamond")
        local rewards_gold = self.myRankNode:getChildByName("plaza_sp_rewards_gold")
        local txt_Rewards_gold = self.myRankNode:getChildByName("txt_Rewards_gold")

        if nil ~= self.myRankInfo then
                local txt_norank = self.myRankNode:getChildByName("txt_norank")
                txt_norank:setVisible(false)

                self.myRankNode:getChildByName("txt_rank"):setVisible(true)
                --金钱
                --self.myRankNode:getChildByName("plaza_sp_rewards"):setVisible(true)
                --self.myRankNode:getChildByName("txt_Rewards"):setVisible(true)
                self.myRankNode:getChildByName("txt_NoRewards"):setVisible(false)


                local sprank = self.myRankNode:getChildByName("rankImg")
                sprank:setVisible(false)
                if self.myRankInfo.rank > 0 and self.myRankInfo.rank < 4 then
                    local file = "rank/rank" .. self.myRankInfo.rank .. ".png"
                    
                    sprank:setTexture(file)
                    sprank:setVisible(true)
                    

                    local txtRank = self.myRankNode:getChildByName("txt_rank")
                    txtRank:setVisible(false)
                else
                    local txtRank = self.myRankNode:getChildByName("txt_rank")
                    txtRank:setVisible(true)
                    txtRank:setString(self.myRankInfo.rank .. "")
                end

                local scoreTxt = self.myRankNode:getChildByName("txt_wintimes")
                if TAG_ENUM.RANK_SCORE == rankType then
                    
                         
                            scoreTxt:setString(string.gsub(ExternalFun.numberThousands(self.myRankInfo.lScore), "[,]", ";").."")
                            scoreTxt:setVisible(true)


                            self.myRankNode:getChildByName("txt_NoRewards_0"):setVisible(false)

                else
                    scoreTxt:removeChildByName("_type_")
                    scoreTxt:setString("" .. self.myRankInfo.winTimes)
                end

                --奖励
                local rewardsGold, rewardsDiamond = self:getRewaeds(self.myRankInfo.rank)


                if rewardsGold >= 0 then
                    rewards_gold:setVisible(true)
                    txt_Rewards_gold:setVisible(true)

                    -- if rewardsDiamond == 0 then
                    --     rewards_diamond:setVisible(false)
                    --     txt_diamond:setVisible(false)

                    --     rewards_gold:setPositionX(375)
                    --     txt_Rewards_gold:setPositionX(375)
                    -- end

                    --设置金额
                    local str = "/"
                    if rewardsGold < 10000 then
                        str = str .. rewardsGold
                    elseif rewardsGold >= 10000 then
                        local szscore = string.gsub(string.format("%.2f", rewardsGold / 10000), "[.]", ":")
                        str = str .. szscore
                    end
                    print("奖励金币数量---------------------》", str)
                    txt_Rewards_gold:setString(str)

                    if rewardsGold >= 10000 then
                        local length = txt_Rewards_gold:getContentSize().width
                        local spWan = cc.Sprite:create("plaza/plaza_sp_rank_numW.png")
                        spWan:setPosition(cc.p(txt_Rewards_gold:getPositionX() + length + 10, txt_Rewards_gold:getPositionY()))   --+10为字的宽度
                        spWan:addTo(self.myRankNode)
                    end
                end

                if rewardsDiamond >= 0 then
                    rewards_diamond:setVisible(true)
                    txt_diamond:setVisible(true)

                    txt_diamond:setString("/" .. rewardsDiamond)

                    -- if rewardsGold == 0 then
                    --     rewards_gold:setVisible(false)
                    --     txt_Rewards_gold:setVisible(false)

                    --     rewards_diamond:setPositionX(375)
                    --     txt_diamond:setPositionX(375)
                    -- end
                end

                if TAG_ENUM.RANK_SCORE == rankType then
                    rewards_diamond:setVisible(false)
                    txt_diamond:setVisible(false)
                    rewards_gold:setVisible(false)
                    txt_Rewards_gold:setVisible(false)
                    scoreTxt:setPositionX(690)
                else

                    scoreTxt:setPositionX(488)
                end
        else
                rewards_diamond:setVisible(false)
                txt_diamond:setVisible(false)
                rewards_gold:setVisible(false)
                txt_Rewards_gold:setVisible(false)
                local txt_norank = self.myRankNode:getChildByName("txt_norank")
                txt_norank:setVisible(true)

                self.myRankNode:getChildByName("txt_rank"):setVisible(false)
                self.myRankNode:getChildByName("txt_NoRewards"):setVisible(true)
                self.myRankNode:getChildByName("rankImg"):setVisible(false)

                -- self.myRankNode:getChildByName("plaza_sp_rewards"):setVisible(false)
                -- self.myRankNode:getChildByName("txt_Rewards"):setVisible(false)
                --self.myRankNode:getChildByName("txt_NoRewards"):setVisible(true)

                 local scoreTxt = self.myRankNode:getChildByName("txt_wintimes")
                if TAG_ENUM.RANK_SCORE == rankType then
                    -- local szscore = ""
                    -- local path = nil
                    -- if GlobalUserItem.tabAccountInfo.lUserScore < 10000 then
                    --     szscore = GlobalUserItem.tabAccountInfo.lUserScore
                    -- elseif GlobalUserItem.tabAccountInfo.lUserScore >= 10000 and GlobalUserItem.tabAccountInfo.lUserScore < 100000000 then  
                    --     if GlobalUserItem.tabAccountInfo.lUserScore / 10000 == 10000 then
                    --         szscore = szscore .."1.00"
                    --         path = "plaza/plaza_sp_rank_numY.png"
                    --     else
                    --         szscore = string.gsub(string.format("%.2f", GlobalUserItem.tabAccountInfo.lUserScore / 10000), "[.]", ":")
                    --         path = "plaza/plaza_sp_rank_numW.png"
                    --     end
                    -- else
                    --     szscore = string.gsub(string.format("%.2f", GlobalUserItem.tabAccountInfo.lUserScore / 100000000), "[.]", ":")
                    --     path = "plaza/plaza_sp_rank_numY.png"
                    -- end
                    scoreTxt:setString(string.gsub(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore), "[,]", ";").."")
                    scoreTxt:setPositionX(690)
                    scoreTxt:setVisible(true)

                    scoreTxt:removeChildByName("_type_")
                    --  if path ~= nil then
                    --     local scoreSize = scoreTxt:getContentSize()
                    --     local spWan = cc.Sprite:create(path)
                    --     spWan:setName("_type_")
                    --     spWan:setAnchorPoint(cc.p(0,0.5))
                    --     spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2))   
                    --     spWan:addTo(scoreTxt)
                    -- end
                    self.myRankNode:getChildByName("txt_NoRewards_0"):setVisible(false)
                    self.myRankNode:getChildByName("txt_NoRewards"):setVisible(false)
               else
                    scoreTxt:removeChildByName("_type_")
                    scoreTxt:setVisible(false)
                    self.myRankNode:getChildByName("txt_NoRewards_0"):setVisible(true)
               end

        end
    end
end

-- 子视图大小
function RankListLayer:cellSizeForTable(view, idx)
    if view == self._listView then
        return 510, self.m_nCellSize
    elseif view == self._listNormalView then
        print("cellSizeForTable")
        return 878, 80
    end
end

function RankListLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    if view == self._listView then
        local iteminfo = self.m_tabList[idx + 1]
        if nil == iteminfo then
            return cell 
        end

        local csb = ExternalFun.loadCSB("plaza/RankItemNode.csb", cell)
        csb:setPosition(view:getViewSize().width * 0.5, self.m_nCellSize * 0.5)
        csb:setName("csbNode")

        -- 头像
        local tmp = csb:getChildByName("sp_headbg")
        --local head = HeadSprite:createClipHead(iteminfo, tmp:getContentSize().width)
        local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width)
        head:setIsGamePop(false)
        --dump(iteminfo, "最终玩家详情", 10)

        --if idx > 5 or self.m_nListCount < 7 then
        if true then
            head:enableInfoPop(true, cc.p(500, -140), cc.p(0.0, 1.0), cell)
            head:setPopCsbFile("plaza/Node_RankUserInfo.csb")  
        else
            head:enableInfoPop(true, cc.p(500, 60), cc.p(0.0, 0.0), cell)
            head:setPopCsbFile("plaza/Node_RankUserInfo_1.csb")  
        end
        --head:enableInfoPop(true, cc.p(0, -175), cc.p(0.5, 1.0), cell)
        
        csb:addChild(head)
        head:setPosition(tmp:getPosition())
        head:setName("head")
        tmp:removeFromParent()


        -- 排行
        local nrank = iteminfo.rank
        local sprank = csb:getChildByName("sp_rankIcon")
        sprank:setVisible(false)
        if nrank > 0 and nrank < 4 then
            local file = "rank/rank" .. nrank .. ".png"
            
            sprank:setTexture(file)
            sprank:setVisible(true)
            

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
        btn:addTouchEventListener(self.m_itemFunC)

        -- 昵称strNickname
        local tmp = csb:getChildByName("txt_nick") --cc.Label:createWithSystemFont(iteminfo.szNickName, "Arial", 20) -
        local strNickname = string.EllipsisByConfig(iteminfo.szNickName, 130, 
                                string.getConfig("fonts/FZHTJW.TTF", 24))
        tmp:setString(strNickname)

        local lScore = string.gsub(ExternalFun.numberThousands(iteminfo.lScore), "[,]", ";")--iteminfo.lScore 

        -- local path =nil
        -- if lScore < 10000 then
        -- elseif lScore >= 10000 and lScore < 100000000 then 
        --     if lScore / 10000 == 10000 then
        --         lScore = "1.00"
        --         path = "plaza/plaza_sp_rank_numY.png"
        --     else
        --         local szscore = string.gsub(string.format("%.2f", lScore / 10000), "[.]", ":")
        --         lScore = szscore..""
        --         path = "plaza/plaza_sp_rank_numW.png"
        --     end
        -- else
        --     local szscore = string.gsub(string.format("%.2f", lScore / 100000000), "[.]", ":")
        --     lScore = szscore..""
        --     path = "plaza/plaza_sp_rank_numY.png"
        -- end

        --胜局
        local txt_score = csb:getChildByName("txt_score")
        txt_score:removeChildByName("_type_")
        if self.m_nValidRank == TAG_ENUM.RANK_SCORE then
            txt_score:setString(""..lScore)
            btn:setVisible(false)
            txt_score:setPositionX(250)
        elseif self.m_nValidRank == TAG_ENUM.RANK_WINTIMES then
            txt_score:setString(""..iteminfo.winTimes)  
            btn:setVisible(true)
            txt_score:setPositionX(40)
        end

        -- if path ~= nil then
        --         local scoreSize = txt_score:getContentSize()
        --         local spWan = cc.Sprite:create(path)
        --         spWan:setName("_type_")
        --         spWan:setAnchorPoint(cc.p(0,0.5))
        --         spWan:setPosition(cc.p(scoreSize.width, scoreSize.height/2))   
        --         spWan:addTo(txt_score)
        --     end

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

                rewards_gold:setPositionX(260)
                txt_Rewards_gold:setPositionX(260)
            end

            --设置金额
            local str = "/"
            if rewardsGold < 10000 then
                str = str .. rewardsGold
            elseif rewardsGold >= 10000 then  
                local szscore = string.gsub(string.format("%.2f", rewardsGold / 10000), "[.]", ":")
                str = str .. szscore
            end
            txt_Rewards_gold:setString(str)

            if rewardsGold >= 10000 and  self.m_nValidRank == TAG_ENUM.RANK_WINTIMES then
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

                rewards_diamond:setPositionX(260)
                txt_diamond:setPositionX(260)
            end
        end

        if self.m_nValidRank == TAG_ENUM.RANK_SCORE then
            rewards_diamond:setVisible(false)
            txt_diamond:setVisible(false)
            rewards_gold:setVisible(false)
            txt_Rewards_gold:setVisible(false)
        elseif self.m_nValidRank == TAG_ENUM.RANK_WINTIMES then
            rewards_diamond:setVisible(true)
            txt_diamond:setVisible(true)
            rewards_gold:setVisible(true)
            txt_Rewards_gold:setVisible(true)
        end

        if rewardsDiamond == 0 and rewardsGold == 0 then
            rewards_diamond:setVisible(false)
            txt_diamond:setVisible(false)
            rewards_gold:setVisible(false)
            txt_Rewards_gold:setVisible(false)
        end

        --奖励
        -- local rewards = csb:getChildByName("txt_rewards")
        -- local rewardsGold, rewardsDiamond = self:getRewaeds(nrank)
        -- local str = "金币:"
        -- if rewardsGold < 10000 then
        --     str = str .. rewardsGold
        -- elseif rewardsGold >= 10000 and rewardsGold < 100000000 then
        --     local szscore = string.gsub(string.format("%.f", rewardsGold / 10000), "[.]", ".")
        --     str = str .. szscore .. "万"
        -- else
        --     local szscore = string.gsub(string.format("%.f", rewardsGold / 100000000), "[.]", ".")
        --     str = str .. szscore .. "亿"
        -- end
        -- if nil ~= rewardsDiamond and rewardsDiamond > 0 then
        --     str = str .. "\n钻石:" .. rewardsDiamond
        -- end
        -- rewards:setString(str)
    elseif view == self._listNormalView then
        local iteminfo = self.m_tabList[idx + 1]
        if nil == iteminfo then
            return cell 
        end

        local csb = ExternalFun.loadCSB("plaza/RankNormalItemNode.csb", cell)
        csb:setPosition(45, -40)
        csb:setName("csbNode")

        -- 头像
        local tmp = csb:getChildByName("sp_headbg")
        local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width)
        head:setIsGamePop(false)
  
        tmp:addChild(head)
        head:setPosition(cc.p(27, 27))    
        head:setName("head")

        local btn = csb:getChildByName("btn")
        btn:setPressedActionEnabled(false)
        btn:setSwallowTouches(false)
        btn:addTouchEventListener(self.m_itemNormalListFunC)
    end
    
    cell:setLocalZOrder(idx +1)

    return cell
end

function RankListLayer:tableCellTouched(view, cell)
    -- local  csbNode = cell:getChildByName("csbNode")
    -- if nil ~= csbNode then
    --     local head = csbNode:getChildByName("head")
    --     if nil ~= head then
    --         head:onTouchHead()
    --     end
    -- end
end


-- 子视图数目
function RankListLayer:numberOfCellsInTableView(view)
    return self.m_nListCount
end

function RankListLayer:cellSizeForTableNormal(view, idx)
    return 90, 65
end

function RankListLayer:tableCellAtIndexNormal(view, idx)
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

    local csb = ExternalFun.loadCSB("plaza/RankNormalItemNode.csb", cell)
    csb:setPosition(45, 30)
    csb:setName("csbNode")

    -- 头像
    local tmp = csb:getChildByName("sp_headbg")
    local head = PopupInfoHead:createNormal(iteminfo, tmp:getContentSize().width)
    head:setIsGamePop(false)

    tmp:addChild(head)
    head:setPosition(cc.p(27, 27))    
    head:setName("head")

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

    local btn = csb:getChildByName("btn")
    btn:setPressedActionEnabled(false)
    btn:setSwallowTouches(false)
    btn:addTouchEventListener(self.m_itemNormalListFunC)

    cell:setLocalZOrder(idx +1)

    return cell
end

-- 子视图数目
function RankListLayer:numberOfCellsInTableViewNormal(view)
    return self.m_nListCount
end

function RankListLayer:tableCellTouchedNormal(view, cell)

end

local posConfig = 
{
    {{226},},       -- 1
    {{120},{360}},      -- 2
    {{88.91},{226},{367.68}}      -- 3
}
-- 调整排行榜
function RankListLayer:adjustRank()
    local mask = GlobalUserItem.tabSystemConfig.RankingListType

    if mask ~= 3 then
        self.m_rankBtnBg:setVisible(false)    
        if  mask == 2 then
            self.m_nValidRank = RankListLayer.RANK_WINTIMES
            self.listTypeTxt:setString("胜局")
            self.listTypeTxt:setPositionX(480)
            self.listRewardTxt:setVisible(true)
        end 
        self.myRankNode:setPositionY(75+10)
        self._listView:setPositionY(157+10)
        self.m_spRank:getChildByName("Image_1"):setPositionY(474+10)
    else
        self.m_rankBtnBg:setVisible(true)
        return
    end


    -- if 0 == mask or mask > 7 then
    --     self.m_btnSwitchRank:setVisible(false)
    --     self.m_btnSwitchRank:setEnabled(false)
    --     return
    -- end
    --local idxList = {}
    -- 财富榜
    -- if 1 == bit:_and(mask, 1) then
    --     self.m_btnScoreRank:setVisible(true)
    --     table.insert(idxList, 1)
    -- end

    -- 消耗榜
    -- if 2 == bit:_and(mask, 2) then
    --     self.m_btnConsumeRank:setVisible(true)
    --     table.insert(idxList, 3)
    -- end

    -- -- 战绩榜
    -- if 4 == bit:_and(mask, 4) then
    --     self.m_btnChargeRank:setVisible(true)
    --     table.insert(idxList, 2)
    -- end
    -- local nCount = #idxList
    -- local tabPos = posConfig[nCount]
    -- if nil ~= tabPos then
        -- 第一个选中
        -- if nCount > 0 then
        --     local iDx = idxList[1]
        --     self.m_spRank:getChildByName("plaza_rank_" .. iDx):setVisible(true)
        --     self.m_nValidRank = self.m_spRank:getChildByName("btn_rank_" .. iDx).nRankType
        -- end

        -- for k,v in pairs(idxList) do
        --     local xPos = tabPos[k][1]
        --     -- 按钮
        --     local btn = self.m_spRank:getChildByName("btn_rank_" .. v)
        --     if nil ~= btn then
        --         btn:setPositionX(xPos)
        --     end
        --     -- 旗帜
        --     local flag = self.m_spRank:getChildByName("plaza_rank_" .. v)
        --     if nil ~= flag then
        --         flag:setPositionX(xPos)
        --     end
        -- end
    --end
end

--显示奖励信息
function RankListLayer:showRewardsInfo()
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
    dump(tabRewards, "showRewardsInfo", 7)

    --设置参数
    for i=1,#tabRewards do
        local info = tabRewards[i]
        local rewards = self.Rank_tips:getChildByName("des" .. i)
        print("showRewardsInfo--------------->", rewards)
        if rewards then
            local str = ""
            if info.Diamond > 0 then
                str = str .. info.Diamond .. "钻石、 "
            end
            if info.Gold < 10000 then
                str = str .. info.Gold .. "金币"
            elseif info.Gold >= 10000 and info.Gold < 100000000 then
                local szscore =string.gsub(string.format("%.2f", info.Gold / 10000), "[.]", ".")
                 str = str .. szscore .. "万金币"
            else
                local szscore = string.gsub(string.format("%.2f", info.Gold / 100000000), "[.]", ".")
                 str = str .. szscore .. "亿金币"
            end

            rewards:setString(str)
            rewards:setPositionX(160)
            rewards:setVisible(true)
        end
        local rank = self.Rank_tips:getChildByName("tet" .. i)
        rank:setVisible(true)
    end

end

--显示奖励信息
function RankListLayer:getRewaeds(rank)
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
    --dump(tabRewards, "getRewaeds", 7)
    if 0 == #tabRewards then
        return 0, 0
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

return RankListLayer