--
-- Author: zhouweixiang
-- Date: 2017-02-25 16:16:10
--游戏比赛信息显示层

local MatchGameLayer = class("MatchGameLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local SpineObj = appdf.req("client.src.external.SpineObj")

local enumTable = {
    "Tag_BT_QIUT", --退出比赛
    "Tag_BT_SHARE", --分享
}
local BT_TAG = ExternalFun.declarEnumWithTable(100, enumTable)
-- 进入场景而且过渡动画结束时候触发。
function MatchGameLayer:onEnterTransitionFinish()
    self:playMatchBeginAnim()
end

-- 退出场景而且开始过渡动画时候触发。
function MatchGameLayer:onExitTransitionStart()
    
end

-- function MatchGameLayer:playMatchBeginAnim()
--     local sp=cc.Sprite:create("match/title_match.png"):addTo(self)
--     sp:setLocalZOrder(1)
--     sp:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
--     sp:setOpacity(255)
--     sp:runAction(cc.Sequence:create(cc.DelayTime:create(2) ,cc.FadeTo:create(0.5,255),cc.DelayTime:create(0.5),cc.FadeTo:create(0.5,100),cc.RemoveSelf:create()))
-- end

function MatchGameLayer:playMatchBeginAnim()

    local csbNode = ExternalFun.loadCSB("begin.csb", self )
   
    csbNode:setLocalZOrder(10)
     csbNode:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
    local act = ExternalFun.loadTimeLine("begin.csb")
  
    act:gotoFrameAndPlay(0,false)  --0,60,false
    act:setTimeSpeed(0.66)
    csbNode:runAction(act)
    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.RemoveSelf:create()))
end


function MatchGameLayer:ctor( scene )
	--注册node事件
    ExternalFun.registerNodeEvent(self)
    self._gameLayer = scene

    --房间数据
    self.m_matchRule = nil
    self.m_roomData = GlobalUserItem.GetRoomInfo(GlobalUserItem.nCurRoomIndex)
    if self.m_roomData then
        self.m_matchRule = MatchRoom:getInstance():getMatchRoomRuleByServerID(self.m_roomData.wServerID)
    end

end

function MatchGameLayer:onUpdateMatchInfo( cmdtable )

    self.m_matchInfo=cmdtable

    -- LogAsset:getInstance():logData("onUpdateMatchInfo")
    -- LogAsset:getInstance():logData(cjson.encode(cmdtable))
    --ccui.Text:create("onUpdateMatchInfo "..cmdtable.cbCurRounds):setPosition(cc.p(600,300)):runAction(cc.Sequence:create(cc.MoveBy:create(3,cc.p(0,300)),cc.RemoveSelf:create()))
  
    if nil==self._gameLayer then
        return
    end
    if self._gameLayer.updateMatchInfo then
        self._gameLayer.updateMatchInfo(cmdtable)
        return
    end

    if nil==self._gameLayer._matchInfoLayer then
        self._gameLayer._matchInfoLayer=ExternalFun.loadCSB("match/MatchInfoLayer.csb",self._gameLayer._gameView.m_rootLayer or self._gameLayer)
    end
    local bg=self._gameLayer._matchInfoLayer:getChildByName("bg")
    --local infoStr1=string.format("%d/%d人（前%d晋级）",cmdtable.cbCurMatchRank,cmdtable.cbStartUserCount,cmdtable.cbPromotion)
     local serverID = MatchRoom:getInstance():getLoginServerID()
     local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(serverID)
     if nil==matchinfo then LogAsset:getInstance():logData("MatchGameLayer matchinfo nil "..serverID) return end
     local tabRoundInfo=matchinfo.cbMatchRule.hMatchRounds[1]
     local curRound = cmdtable.cbCurRounds
     local startUserCount = cmdtable.cbStartUserCount
     local count=curRound-1> 0 and tabRoundInfo[curRound-1].wPromotion or startUserCount

    local infoStr1=string.format("%d/%d人（前%d晋级）",cmdtable.cbCurMatchRank,count,cmdtable.cbPromotion)
    local infoStr2=string.format("%d/%d",cmdtable.cbCurPlayCount,cmdtable.cbTotalPlayCount)
    bg:getChildByName("Text_info1"):setString(infoStr1)
    bg:getChildByName("Text_info2"):setString(infoStr2)

    local sp=cc.Sprite:create("match/match_qualified_info.png"):addTo(self)
    sp:setLocalZOrder(2)
    sp:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
    -- LogAsset:getInstance():logData("cmdtable.cbPromotion=="..(cmdtable.cbPromotion or "Nil"))
    --  LogAsset:getInstance():logData("cmdtable.cbCurRounds=="..(cmdtable.cbCurRounds or "Nil"))
    local la=cc.LabelAtlas:create(cmdtable.cbPromotion, "match/match_qualified_atlas.png", 50, 66, string.byte("0")):addTo(sp)
    la:setPosition(cc.p(130,0))
    sp:setOpacity(0)
    la:setOpacity(0)
   -- debug cc.LabelAtlas:create(cmdtable.cbCurRounds, "match/match_qualified_atlas.png", 50, 66, string.byte("0")):addTo(self):setPosition(cc.p(600,200)):runAction(cc.Sequence:create(cc.DelayTime:create(5),cc.RemoveSelf:create()))
    local dt=cmdtable.cbCurRounds==1 and 3 or 0
    sp:runAction(cc.Sequence:create(cc.DelayTime:create(dt),cc.FadeTo:create(0.01,100),cc.FadeTo:create(0.5,255),cc.DelayTime:create(0.5),cc.FadeTo:create(0.5,100),cc.RemoveSelf:create()))
    la:runAction(cc.Sequence:create(cc.DelayTime:create(dt),cc.FadeTo:create(0.01,100),cc.FadeTo:create(0.5,255),cc.DelayTime:create(0.5),cc.FadeTo:create(0.5,100)))
end

function MatchGameLayer:onSubMatchPromotion(curRank)
    if nil==self.m_matchInfo then print("nil info") return end
    local cmdtable=self.m_matchInfo
    self:showPromotionLayer(cmdtable.cbStartUserCount,cmdtable.cbCurRounds,cmdtable.cbTotalRounds,curRank) 
end

function MatchGameLayer:showPromotionLayer(startUserCount,curRound,nRound,curRank)

    local serverID = MatchRoom:getInstance():getLoginServerID()
    local matchinfo = MatchRoom:getInstance():getMatchRoomRuleByServerID(serverID)
    print("serverID",serverID)
    dump(matchinfo)
   
    local rule=matchinfo.cbMatchRule

    local tabRoundInfo=rule.hMatchRounds[1]

    local csbNode=ExternalFun.loadCSB("match/PromotionLayer.csb",self)
    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(5),cc.RemoveSelf:create()))
    local bg=csbNode:getChildByName("bg")
    bg:getChildByName("promotion_num"):setString(curRank)--tabRoundInfo[curRound].wPromotion)
    bg:getChildByName("promotion_num"):setLocalZOrder(2)
    local totalnum= tabRoundInfo[curRound].wPromotion or startUserCount
    bg:getChildByName("total_num"):setString("/"..totalnum)
    bg:getChildByName("total_num"):setLocalZOrder(2)

    local cellSize = 740/rule.cbTotalRounds
    for i=1,rule.cbTotalRounds -1 do
            local zhuzi = cc.Sprite:create("match/zhuzi"..(i<=curRound and 1 or 2)..".png"):move(290+cellSize*i,116):addTo(bg)
            if i ==curRound  then
                local curImg = cc.Sprite:create("match/curImg.png"):move(290+cellSize*i,177):addTo(bg)
            end
            local curPlayer = cc.Sprite:create("match/scoreBg.png"):move(290+cellSize*i,47.5):addTo(bg)
            local txtround = cc.Label:createWithTTF( tabRoundInfo[i].wPromotion.."","fonts/FZHTJW.TTF",20)
            txtround:setPosition(cc.p(40, 14))
            curPlayer:addChild(txtround)
            txtround:setColor(cc.c4b(227,193,85,255))
    end
    local bar=bg:getChildByName("LoadingBar_1")
    bar:setPercent(8+(curRound*cellSize/8.8 <86 and curRound*cellSize/8.8 or 86))

    local obj = SpineObj:create("plaza/animate/jinji/skeleton.json", "plaza/animate/jinji/skeleton.atlas", false)
            obj:setPosition(677,400)
            bg:addChild(obj)

    -- local barSize=bg:getChildByName("Img_progress"):getContentSize()
    -- local bar=bg:getChildByName("Img_progress")

    -- local cellW,cellH=200,190
    -- local tvW=1200
    -- if rule.cbTotalRounds+1<6 then
    --     cellW=tvW/(rule.cbTotalRounds+1)
    -- end

    -- local tvPos=cc.p(bar:getPositionX(),bar:getPositionY()-cellH/2)
    -- bar:setContentSize(cc.size(cellW*curRound,barSize.height))

    -- local function scrollViewDidScroll(view)
    --     local x=view:getContentOffset().x
    --     bar:setPositionX(tvPos.x+cellW/2+x)     
    -- end 
  
    -- local function cellSizeForTable(view, idx)
    --     return cellW,cellH 
    -- end

    -- local function numberOfCellsInTableView(view)
    --     return rule.cbTotalRounds+1
    -- end

    -- local function tableCellAtIndex(view, idx)
 
    --     local cell = view:dequeueCell()
    --     if not cell then        
    --         cell = cc.TableViewCell:new()
    --     else
    --         cell:removeAllChildren()
    --     end

    --     local filepath=  idx<=curRound and "match/match_promotion_countbg1.png" or "match/match_promotion_countbg2.png" -- 1+idx<=curRound
       
    --     local sp=cc.Sprite:create(filepath):addTo(cell)
    --     local hw,hh=sp:getContentSize().width/2,sp:getContentSize().height/2

    --     sp:setPosition(cc.p(cellW/2,cellH/2))
    --     local count=rule.wStartUserCount
    --     if idx>0 then count=tabRoundInfo[idx].wPromotion or 1 end
    --     ccui.Text:create(count, "fonts/round_body.ttf", 24):addTo(cell):setPosition(cc.p(cellW/2,cellH/2))
    --     if idx==curRound then --1+idx==curRound
    --         cc.Sprite:create("match/match_wait_cone.png"):addTo(cell):setAnchorPoint(cc.p(0.5,0)):setPosition(cc.p(cellW/2,hh+cellH/2-10))
    --     end

    --     return cell
    -- end

    -- local tableView = cc.TableView:create({width=tvW,height=cellH})
    -- tableView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
    -- tableView:setPosition(tvPos)  
    -- tableView:addTo(csbNode)
    -- tableView:setDelegate()
    -- tableView:registerScriptHandler(cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    -- tableView:registerScriptHandler(tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
    -- tableView:registerScriptHandler(numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    -- tableView:registerScriptHandler(scrollViewDidScroll,cc.SCROLLVIEW_SCRIPT_SCROLL)
    -- tableView:reloadData()

end

--显示提示
function MatchGameLayer:onSubMatchTip( cmdtable )
    if nil==cmdtable.szString then return end
    self:removeTipNode()
    self.m_tipNode =  ExternalFun.loadCSB("match/MatchTips.csb", self)
    self.m_tipNode:setVisible(false)

    --tips 内容
    local txt_tips = self.m_tipNode:getChildByName("Text_1")
    txt_tips:setString(cmdtable.szString)
    -- LogAsset:getInstance():logData( "onSubMatchTip: "..cmdtable.szString)
    self:runAction(cc.Sequence:create(cc.DelayTime:create(5.0),cc.CallFunc:create(function ()
        if  self.m_tipNode then
          if self._gameLayer and self._gameLayer.m_cbGameStatus == 0 then
               self._gameLayer._gameView:reSetForNewGame()
           end

           self.m_tipNode:setVisible(true)
        end
    end)))

end

--移除提示
function MatchGameLayer:removeTipNode( )
	if nil ~= self.m_tipNode then
		self.m_tipNode:removeFromParent()
		self.m_tipNode = nil
	end
end


function MatchGameLayer:onSubMatchResult( cmdtable )

        if self._gameLayer and self._gameLayer.m_cbGameStatus == 0 then
            self._gameLayer._gameView:reSetForNewGame()
        end

        self:removeTipNode()
	local resultNode = ExternalFun.loadCSB("match/MatchGameResult.csb", self)
	local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    dump(cmdtable,"onSubMatchResult:",5)

    resultNode:setVisible(false)

    local t=cmdtable.MatchEndTime 
    if t then
        resultNode:getChildByName("txt_time"):setString(string.format("%d月%d日 %02d:%02d:%02d",t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond))
    else
        resultNode:getChildByName("txt_time"):setString(os.date("%m月%d日 %H:%M:%S",os.time()))
    end

    --比赛场次
    local txt_matchName = resultNode:getChildByName("txt_1")

    if cmdtable.szMatchName then
       txt_matchName:setString("恭喜您在["..cmdtable.szMatchName.."]中荣获")
    elseif conditions then
       txt_matchName:setString("恭喜您在本次比赛中荣获")   
    end

    local txt_rank = resultNode:getChildByName("txt_2")
    txt_rank:setString("第"..cmdtable.wMatchRank.."名")

    local txt_reward = resultNode:getChildByName("txt_3")

     --LogAsset:getInstance():logData(cjson.encode(cmdtable))
    if (cmdtable.lGold and cmdtable.lGold>0) or  (cmdtable.lIngot and cmdtable.lIngot > 0 ) or (cmdtable.dwTicket and cmdtable.dwTicket > 0 ) then
        -- LogAsset:getInstance():logData(type(cmdtable.lGold))
        --  LogAsset:getInstance():logData("cmdtable.lGold "..cmdtable.lGold)
        local str = ""
        if cmdtable.lGold > 0 then
            str = str..cmdtable.lGold.."金币"
        end

        if cmdtable.lIngot > 0 then
            if cmdtable.lGold > 0 then
                str = str..","
            end
            
            str = str..cmdtable.lIngot.."钻石"
        end

        if cmdtable.dwTicket > 0 then
            if str ~= "" then
                str = str..","
            end
            
            str = str..cmdtable.dwTicket.."奖券"
        end

        txt_reward:setString("奖励["..str.."],已发放到您的账户")
        txt_reward:setVisible(true)
    else
       txt_reward:setVisible(false)    
    end

    --返回
    local btn = resultNode:getChildByName("btn_1")
    btn:setTag(BT_TAG.Tag_BT_QIUT)
    btn:addTouchEventListener(btncallback)

    local shareBtn = resultNode:getChildByName("btn_2") 
    --btn:setTag(BT_TAG.Tag_BT_SHARE)
    --btn:addTouchEventListener(btncallback)

    self:runAction(cc.Sequence:create(cc.DelayTime:create(6.0),cc.CallFunc:create(function ()
        resultNode:setVisible(true)
    end)))
    shareBtn:addClickEventListener(function() 
            self._gameLayer._scene:popTargetShare(function(target)
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
            render:beginWithClear(0, 0, 0, 0)
            resultNode:visit()
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

end

function MatchGameLayer:onButtonClickedEvent(tag, ref)
	if tag == BT_TAG.Tag_BT_QIUT then
		GlobalUserItem.bWaitQuit = false
		self._gameLayer._scene:onChangeShowMode(yl.SCENE_PLAZA)
        GlobalUserItem.bMatch = false
	elseif tag == BT_TAG.Tag_BT_SHARE then
       
	end
end

return MatchGameLayer
