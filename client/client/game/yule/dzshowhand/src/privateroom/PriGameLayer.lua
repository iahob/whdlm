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
local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.dzshowhand.src.privateroom.DismissLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd = appdf.req("game.yule.dzshowhand.src.models.CMD_Game")

local TAG_START             = 100
local enumTable = 
{
    "BTN_ENTER",            -- 加入
    "BTN_INVITE",           -- 邀请
    "BTN_QUIT",             -- 退出
    "BTN_JUSHU",            -- 局数
    "BTN_SHARE",            -- 分享
    "BTN_CLOSEINFO",        --关闭房间详情
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local MARK_LAYER_NAME = "__mark_layer_name__"
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

        -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("Text_3")
    self.m_atlasRoomID:setString("000000")

    self.priRoomInfo = csbNode:getChildByName("priRoomInfo")

    --局数按钮
    local detailBtn = csbNode:getChildByName("Button_1")

    detailBtn:addTouchEventListener(function( sender ,eventType)
        if eventType == ccui.TouchEventType.ended then
              self.priRoomInfo:setVisible(true)
        end
    end)



    -- 局数
    self.m_atlasCount = csbNode:getChildByName("Text_1")
    self.m_atlasCount:setString("0/0")

    self.m_priTimeTxt = csbNode:getChildByName("Text_4")


    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end


   
    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)

    self.m_btnInvite2 = csbNode:getChildByName("bt_invite2")
    self.m_btnInvite2:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite2:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer)

    
    --遮罩
    local Panel_mask = self.priRoomInfo:getChildByName("Panel_2")
    Panel_mask:addTouchEventListener(function(sender,eventType)
      if eventType == ccui.TouchEventType.ended then
          self.priRoomInfo:setVisible(false)
      end
    end)
	
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if TAG_ENUM.BTN_INVITE == tag then
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
                                        showToast(self, "分享成功", 2)
                                    end
                                end
                                local passwd = cmd_table.dwRoomDwd or 0
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local baseScore = "底分:"..self._gameLayer.m_lCellScore..","

                                local shareTxt = "时间:" .. PriRoom:getInstance().m_tabPriData.dwDrawTimeLimit .. "分钟。"..baseScore.."德州扑克游戏精彩刺激, 一起来玩吧! "
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=3&a=1" .. "&p=" .. passwd
                                print("获取分享信息===》", dwRoomID,shareTxt,  url)
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
    elseif TAG_ENUM.BTN_SHARE == tag then
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target)
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享成功", 2)
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
    elseif TAG_ENUM.BTN_QUIT == tag then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
    elseif TAG_ENUM.BTN_JUSHU == tag then
		
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
     dump(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, "约战数据onRefreshInfo", 10)

        -- 房间ID
    self.m_atlasRoomID:setString(""..(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000"))


    -- 局数
    if PriRoom:getInstance().m_tabPriData.dwPlayCount and PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
        local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            self.m_atlasCount:setString(strcount)
    end

    
    if PriRoom:getInstance().m_tabPriData.dwPlayCount == nil then
        return
    end

    self:onRefreshInviteBtn()
    
end

function PriGameLayer:initPriRoomInfo()
    local y = 550
    local nameTable = {"time" , "mangZhu" , "qianZhu" , "buy" , "min" , "max"}
    local infopPrameter = {self._gameLayer.m_cbPriTime , 
                            self._gameLayer.m_lCellScore ,
                            self._gameLayer.m_lAnteScore , 
                            self._gameLayer.m_lAllBetScore,
                            self._gameLayer.m_lMinBetScore , 
                            self._gameLayer.m_lMaxBetScore}
    if infopPrameter[1]<=0 then
        infopPrameter[1] = nil
    end

    for i=1 , 6 do
        if infopPrameter[i] then
            local title = self.priRoomInfo:getChildByName("title_"..nameTable[i])
            local info = self.priRoomInfo:getChildByName("info_"..nameTable[i])
            title:setVisible(true)
            info:setVisible(true)
            title:setPositionY(y)
            info:setPositionY(y)
            y = y - 70
            if i==1 then
                info:setString(infopPrameter[i] .. "分钟")
            elseif i==2 then
                info:setString(infopPrameter[i]/2 .. "/"..infopPrameter[i])
            else
                info:setString(infopPrameter[i])
            end
        end
    end


end

--if PriRoom:getInstance().m_tabPriData.dwPlayCount ==0 and self._gameLayer.m_cbGameStatus == 0 and #self._gameLayer.m_tabUserItem ~= 9 then

function PriGameLayer:onRefreshInviteBtn()
     -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(false)
        return
    end
    if self._gameLayer.m_cbGameStatus ~= 0 then --空闲场景
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(true)
        return
    end

    -- 邀请按钮
    if #self._gameLayer.m_tabUserItem == 9 then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(false)
        return
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount==0 then
        self.m_btnInvite:setVisible(true)
        self.m_btnInvite2:setVisible(false)
    else
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite2:setVisible(true)
    end

end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table,dataBuffer )

    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    self.bigWinner = dataBuffer:readword()
    print("self.bigWinner=====",self.bigWinner)

    dump(self._gameLayer._gameReord, "房卡总结算", 6)
    dump(self._gameLayer.m_tabUserItem,"====================1")
    self._gameLayer.bPriGameEnd = true
    self:runAction(cc.Sequence:create(cc.DelayTime:create(2),cc.CallFunc:create(function ( ... )
        self:removeChildByName("private_end_layer")

            local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self.m_rootLayer)
            csbNode:setVisible(false)
            csbNode:setName("private_end_layer")

            

            local data = self._gameLayer._gameReord
            if not data then
                return
            end
            


            local allBuy = 0
            for i=1,cmd.GAME_PLAYER do
                    allBuy = allBuy + data.lTakeScore[1][i]
            end


            csbNode:getChildByName("Text_jushu"):setString((isNil and 0 or data.nCount) .."")
            csbNode:getChildByName("Text_mairu"):setString(allBuy.."")

            local Panel_2 = csbNode:getChildByName("Panel_2")
            self._listView = cc.TableView:create(cc.size(Panel_2:getContentSize().width,Panel_2:getContentSize().height))
            self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
            self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
            self._listView:setAnchorPoint(0,0)
            self._listView:setPosition(0,0)
            self._listView:setDelegate()
            self._listView:addTo(Panel_2)
            self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
            self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
            self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
            self._listView:reloadData()

            local function btncallback(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    self:onButtonClickedEvent(ref:getTag(),ref)
                end
            end

            -- 分享按钮
            btn = csbNode:getChildByName("btn_share")
            btn:setTag(TAG_ENUM.BTN_SHARE)
            btn:setVisible(true)
            btn:addTouchEventListener(btncallback)

            -- 退出按钮
            btn = csbNode:getChildByName("btn_quit")
            btn:setTag(TAG_ENUM.BTN_QUIT)
            btn:addTouchEventListener(btncallback)

            csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                cc.CallFunc:create(function()
                    csbNode:setVisible(true)
            end)))
    end)))
    
 end

-- 子视图大小
function PriGameLayer:cellSizeForTable(view, idx)
    return 780, 50
end

function PriGameLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local useritem = nil
    local curIndex = 0
    local curChair = -1

    for i= 1 ,cmd.GAME_PLAYER  do

            useritem =  self._gameLayer.m_tabUserItem[i]
            if useritem  then
                curIndex = curIndex +1
                if curIndex == idx +1 then
                    curChair = i
                    break
                end

            end

    end

    --local useritem =  self._gameLayer:getSitUserInfoByChairID(idx)
    if nil == useritem or curChair == -1 then
        
        return cell 
    end

    local csb = ExternalFun.loadCSB("game/endCellNode.csb", cell)
    csb:setPosition(0, 25)

    --头像
    local head = PopupInfoHead:createNormal(useritem, 40)
    head:setAnchorPoint(cc.p(0.5,0.5))
    head:setPosition(cc.p(88,0))
    csb:addChild(head)


    --用户昵称
    

    local clipNick = ClipText:createClipText(cc.size(100, 20), useritem.szNickName, "" , 25)
        clipNick:setAnchorPoint(cc.p(0.5, 0.5))
        clipNick:setPosition(cc.p(270,0))
        csb:addChild(clipNick)

    --携带
    local data = self._gameLayer._gameReord

    local take = csb:getChildByName("Text_2")
    take:setString(data.lTakeScore[1][curChair].."")
    dump(useritem)
    print(curChair,data.lTakeScore[1][curChair])
    --盈利
    local str = ""
    local color = nil
    if data.lAllScore[1][curChair] >= 0 then
        str = "+"..data.lAllScore[1][curChair]
        color = cc.c3b(100, 211, 97)
    else
        str = ""..data.lAllScore[1][curChair]  
        color = cc.c3b(219, 70, 70)
    end
    local win =  csb:getChildByName("Text_3")
    win:setString(str)
    win:setTextColor(color)
    win:enableOutline(color, 1)

    if self.bigWinner and self.bigWinner == curChair -1 then
        csb:getChildByName("winner"):setVisible(true)
    end
    return cell
end

-- 子视图数目
function PriGameLayer:numberOfCellsInTableView(view)
   local num = 0
   for k,v in pairs(self._gameLayer.m_tabUserItem) do
        if v then
            num = num+1
        end
   end
    --print("numberOfCellsInTableView======================,"..num)
    return num 
end

function PriGameLayer:onExit()
    self._gameLayer.bPriGameEnd=false
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/land_game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/land_game.png")
end

return PriGameLayer