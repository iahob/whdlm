--
-- Author: zhong
-- Date: 2016-12-29 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.oxeight.src.privateroom.DismissLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")

    --局数按钮
    local detailBtn = csbNode:getChildByName("round_btn")
    detailBtn:addTouchEventListener(function( sender ,eventType)
        if eventType == ccui.TouchEventType.ended then
              self._gameLayer._gameView:showRoomInfo(true)
        end
    end)

    -- 局数
    self.m_atlasCount = detailBtn:getChildByName("num_game_count")
    self.m_atlasCount:setString("0/0")

    self.m_txtRoom = detailBtn:getChildByName("Text_1")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite"):setPositionY(500)
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, 1000)

end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if BTN_DISMISS == tag then              -- 请求解散游戏
        PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum())
    elseif BTN_INVITE == tag then
        print("GlobalUserItem.tabAccountInfo.dwGameID ====",GlobalUserItem.tabAccountInfo.dwGameID)
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()
                if nil == scene then
                    return
                end
                local dwRoomDwd = cmd_table.dwRoomDwd or 0

                local function _shareFun( dwRoomDwd )
                    if nil ~= scene.popTargetShare then
                        scene:popTargetShare(function(target)
                            if nil ~= target then
                                local function sharecall( isok )
                                    if type(isok) == "string" and isok == "true" then
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                                    end
                                end
                                local passwd = dwRoomDwd
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0

                                local cardType = {"经典模式","疯狂加倍"}
                                local sendType = {"发四等五","下注发牌"}
                                local bankerType = {"霸王庄","明牌抢庄","牛牛上庄","无牛下庄","自由抢庄","通比玩法"}
                                local config = self._gameLayer:getPrivateRoomConfig()
                                local chairCountstr = self._gameLayer.wPlayerCountRule == 0 and "2-8人" or string.format("%d人", self._gameLayer.wPlayerCountRule)
                                local configStr = string.format("%s,%d局,", chairCountstr,PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
                                configStr = configStr..cardType[config.cardType-22+1]..","..sendType[config.sendCardType-32+1]..","..bankerType[config.bankGameType-52+1]

                                local shareTxt = configStr .. "。新八人牛牛精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=57&a=1" .. "&p=" .. passwd

                                MultiPlatform:getInstance():shareToTarget(
                                    target, 
                                    sharecall, 
                                    string.format("【约战房间: %06d】", dwRoomID), 
                                    shareTxt, 
                                    url, 
                                    "")
                            end
                        end, 3)
                    end
                end
                
                if 0 ~= dwRoomDwd then
                    local query = QueryDialog:create("约战密码为 " .. dwRoomDwd .. " 是否邀请好友游戏?", function(ok)
                        if ok then
                            _shareFun(dwRoomDwd)
                        end
                    end)
                    :setCanTouchOutside(false)
                    :addTo(self)
                else
                    _shareFun( dwRoomDwd )
                end
            end
        )
    elseif BTN_SHARE == tag then
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                end
                GlobalUserItem.bAutoConnect = true
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            -- 截图分享
            local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
            local area = cc.rect(0, 0, framesize.width, framesize.height)
            local imagename = "grade_share.jpg"
            ExternalFun.popupTouchFilter(0, false)
            captureScreenWithArea(area, imagename, function(ok, savepath)
                ExternalFun.dismissTouchFilter()
                if ok then
                    if nil ~= target then
                        GlobalUserItem.bAutoConnect = false
                        MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的约战房战绩", "分享我的约战房战绩", url, savepath, "true")
                    end            
                end
            end)
        end, 3)
    elseif BTN_QUIT == tag then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
    elseif BTN_ZANLI == tag then
        PriRoom:getInstance():tempLeaveGame()
        self._gameLayer:onExitRoom()
    elseif BTN_INFO == tag then
        self._gameLayer._gameView:showRoomInfo(true)
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString(""..(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000"))

   -- dump(PriRoom:getInstance().m_tabPriData, "约战数据", 10)

    

    if GlobalUserItem.bPrivateGoldRoom and GlobalUserItem.dwCurRoomGroupID == 0 then
        self.m_atlasCount:setString("0/0")
        self.m_atlasCount:setVisible(false)
        self.m_txtRoom:setPosition(92, 30)
        self.m_txtRoom:setString("房间信息")
    else
        -- self.m_txtRoom:setPosition(35, 23)
        self.m_atlasCount:setVisible(true)
        self.m_txtRoom:setPosition(53, 30)
        self.m_txtRoom:setString("局数:")
    end
    -- 局数
    if PriRoom:getInstance().m_tabPriData.dwPlayCount and PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
        local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            self.m_atlasCount:setString(strcount)
    end

    self:onRefreshInviteBtn()


end

function PriGameLayer:onRefreshInviteBtn()
    print("刷新舒心",self._gameLayer.m_cbGameStatus)
   -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
    if self._gameLayer.m_cbGameStatus ~= 0 then --空闲场景
        self.m_btnInvite:setVisible(false)
        return
    end

    self.m_btnInvite:setVisible(false)
    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        if self._gameLayer:onGetSitUserNum() < chairCount and PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
            self.m_btnInvite:setVisible(true)
            return
        end
    end
    
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )

    self._gameLayer.isPriOver = true
    self:removeChildByName("private_end_layer")

    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self.m_rootLayer)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    local bgImg = csbNode:getChildByName("Image_1")
    local chairCount = self._gameLayer:getChairCount() or 2
    --bgImg:setContentSize(bgImg:getContentSize().width,bgImg:getContentSize().height - 70*(6-chairCount))


    local titleimg= bgImg:getChildByName("Sprite_1")
    --titleimg:setPositionY(588-70*(6-chairCount))

   print("===========================onPriGameEnd")

    local titleTxt = bgImg:getChildByName("Node_1")
    --titleTxt:setPositionY(520-70*(6-chairCount))


    ---房间ID
    local roomID = bgImg:getChildByName("Text_36")
    roomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID) or "000000" )
   
    --创建时间
    local createtime = bgImg:getChildByName("Text_38")
    createtime:setVisible(true)
    --print("时间")
    local time = os.date("%Y-%m-%d %H:%M",os.time())
    --print("time")
    createtime:setString(time)


    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()

    --房间类型
    local cardType = {"经典模式","疯狂加倍"}
    local sendType = {"发四等五","下注发牌"}
    local bankerType = {"霸王庄","倍数抢庄","牛牛上庄","无牛下庄","自由抢庄","通比玩法"}
    local atlas_config = bgImg:getChildByName("Text_37")
    local config = self._gameLayer:getPrivateRoomConfig()
    local configStr = string.format("2-8人,%d局,",PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
    if config.cardType ~= 0  and config.cardType < 24 then
        configStr = configStr..cardType[config.cardType-22+1]..","..sendType[config.sendCardType-32+1]..","..bankerType[config.bankGameType-52+1]
    end
    

    atlas_config:setString(configStr)


    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    self.m_scoreList = scoreList
    --获取最大成绩
    self.maxindex = 1
    self.maxNum = 0
    local maxscore = scoreList[1]
    for i=2,chairCount do
        if scoreList[i] > maxscore then
            maxscore = scoreList[i]
            self.maxindex = i
        end
    end



    for i = 1, chairCount do
        local useritem = self._gameLayer:getSaveUserInfos(i-1)

        if nil ~= useritem then
           
            if scoreList[i] == maxscore then
                self.maxNum =self.maxNum +1
            end
        end
    end 


    local num = self._gameLayer:onGetSitUserNum()
    local Panel_2 = bgImg:getChildByName("Panel_2")

    local realIndex  = 0;

    for i=1,8 do
        local useritem = self._gameLayer:getSaveUserInfos(i -1)

        if useritem and  (self.m_scoreList[i] ~= 0 or self._gameLayer.cbPlayStatus[i]==1) then 
                realIndex = realIndex +1
                local infoNode = Panel_2:getChildByName("FileNode_"..realIndex)
                infoNode:setVisible(true)
                --头像
                local head = PopupInfoHead:createClipHead(useritem, 99,"game/Sprite_head1.png")
                head:setScale(0.53)
                head:setAnchorPoint(cc.p(0.5,0.5))
                head:setPosition(cc.p(29, 0))
                infoNode:addChild(head)


                --昵称
                local nick =  ClipText:createClipText(cc.size(140, 26),useritem.szNickName,"fonts/round_body.ttf",24);
                nick:setAnchorPoint(cc.p(0, 0.5))
                nick:setPosition(cc.p(100, 0))
                nick:setColor(cc.c3b(255, 255, 255))
                infoNode:addChild(nick)

                 local allScore = infoNode:getChildByName("Text_1_3")
                    allScore:setString(self.m_scoreList[i])
                    if self.m_scoreList[i] >= 0 then
                        allScore:setString("+"..self.m_scoreList[i])
                    else
                        allScore:setColor(cc.c3b(205, 75, 101))
                    end
            
                -- if self.maxNum == 1 and self.maxindex == i then
                --     infoNode:getChildByName("Sprite_13"):setVisible(true)
                -- end
              
                    --房主标识
                local fangzhuicon = infoNode:getChildByName("Sprite_50")
                fangzhuicon:setVisible(false)
                if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                    fangzhuicon:setVisible(true)
                    fangzhuicon:setLocalZOrder(5)
                end

        end


    end

    --大赢家显示
    -- print(bigWinner, "----------------大赢家显示-------------------")
    if bigWinner ~= yl.INVALID_CHAIR then 
        local infoNode = Panel_2:getChildByName("FileNode_".. (bigWinner+1))
        infoNode:getChildByName("Sprite_13"):setVisible(true)
    end

    

    -- self._listView = cc.TableView:create(cc.size(Panel_2:getContentSize().width,Panel_2:getContentSize().height))
    -- self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    -- self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    -- self._listView:setAnchorPoint(0,0)
    -- self._listView:setPosition(0,0)
    -- self._listView:setDelegate()
    -- self._listView:addTo(Panel_2)
    -- self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    -- self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    -- self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    -- self._listView:reloadData()



    -- 分享按钮
    btn = bgImg:getChildByName("Button_2")
    btn:setTag(BTN_SHARE)
    btn:setVisible(true)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = bgImg:getChildByName("Button_1")
    --btn:setPositionY(573-73*(6-chairCount))
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0),
        cc.CallFunc:create(function()
            csbNode:setVisible(true)
        end)))
end


-- 子视图大小
function PriGameLayer:cellSizeForTable(view, idx)
    return 950, 100
end

function PriGameLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    local useritem = self._gameLayer.m_tabUserItem[idx+1] --self._gameLayer:getUserInfoByChairID(idx)
    if nil == useritem then
        print("idx is =============",idx)
        error("message",0)
        return cell 
    end

    local csb = ExternalFun.loadCSB("game/infoNode.csb", cell)
    csb:setPosition(506, 45)

    --头像
    local head = PopupInfoHead:createClipHead(useritem, 95,"game/Sprite_head1.png")
    head:setScale(0.55)
    head:setAnchorPoint(cc.p(0.5,0.5))
    head:setPosition(cc.p(-415.5, 0))
    csb:addChild(head)


    --昵称
    local nick =  ClipText:createClipText(cc.size(140, 26),useritem.szNickName,"fonts/round_body.ttf",24);
    nick:setAnchorPoint(cc.p(0, 0.5))
    nick:setPosition(cc.p(-354, 0))
    nick:setColor(cc.c3b(255, 255, 255))
    csb:addChild(nick)

     local allScore = csb:getChildByName("Text_1_3")
        allScore:setString(self.m_scoreList[idx+1])
        if self.m_scoreList[idx+1] >= 0 then
            allScore:setString("+"..self.m_scoreList[idx+1])
        else
            allScore:setColor(cc.c3b(205, 75, 101))
        end
    
    if self.maxNum == 1 and self.maxindex == idx +1 then
        csb:getChildByName("Sprite_13"):setVisible(true)
    end

    local gameRecord = self._gameLayer:getDetailScore()
    --dump(gameRecord)
     local winnum = csb:getChildByName("Text_1_0")
            winnum:setString(gameRecord.wincount[idx+1].."次")
            local losenum = csb:getChildByName("Text_1_2")
            losenum:setString(gameRecord.losecount[idx+1].."次")
            
            --房主标识
            local fangzhuicon = csb:getChildByName("Sprite_50")
            fangzhuicon:setVisible(false)
            if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                fangzhuicon:setVisible(true)
                fangzhuicon:setLocalZOrder(5)
            end


    return cell
end

-- 子视图数目
function PriGameLayer:numberOfCellsInTableView(view)
    local num = self._gameLayer:onGetSitUserNum()
    print(num.."=================numberOfCellsInTableView")
    return num
end



function PriGameLayer:onLeaveGame()
    --"房卡切换后台"
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
end

return PriGameLayer