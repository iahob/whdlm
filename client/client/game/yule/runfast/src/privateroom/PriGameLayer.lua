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
local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.privateroom.DismissLayer")
local MarkLayer = appdf.req(appdf.GAME_SRC.."yule.runfast.src.privateroom.MarkLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local TAG_START             = 100
local enumTable = 
{
    "BTN_ENTER",            -- 加入
    "BTN_INVITE",           -- 邀请
    "BTN_QUIT",             -- 退出
    "BTN_JUSHU",            -- 局数
    "BTN_SHARE",            -- 分享
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local MARK_LAYER_NAME = "__mark_layer_name__"
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("privateroom/game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer
    self._csbNode = csbNode
    --
    self.priEndLayer = nil
   -- local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("AtlasLabel_roomNum")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = csbNode:getChildByName("AtlasLabel_playCount")
    self.m_atlasCount:setString("0/0")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer)
	
	-- local mark = MarkLayer:create(self._gameLayer)
	-- mark:setName(MARK_LAYER_NAME)
	-- self:addChild(mark)
	-- mark:setVisible(false)

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
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                                    end
                                end
                                local passwd = cmd_table.dwRoomDwd or 0
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local rule = "底分:"..self._gameLayer.lBaseScore..","..self._gameLayer.wPlayerCount.."人,"..self._gameLayer.cbUserMaxCardCount.."张,"..(self._gameLayer.bSpadeBanker and "黑桃三首出," or "赢家首出,")
                                        ..(self._gameLayer.bForceOutCard and "有大必出," or "非有大必出,")..(self._gameLayer.bEchelonConclude and "梯度结算," or "非梯度结算,")
                                local shareTxt = ""
                                if GlobalUserItem.bPrivateGoldRoom then
                                        shareTxt = rule.."跑得快游戏精彩刺激, 一起来玩吧! "
                                else
                                        shareTxt = "局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit .. "局。"..rule.."跑得快游戏精彩刺激, 一起来玩吧! "
                                end
                               
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=240&a=1" .. "&p=" .. passwd
                                --print("获取分享信息===》", dwRoomID,shareTxt,  url)
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
    elseif TAG_ENUM.BTN_QUIT == tag then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
		
        
		
    end
end

function PriGameLayer:showRoomRuleRecord()
    local mark = MarkLayer:create(self._gameLayer)
    mark:setName(MARK_LAYER_NAME)
    self:addChild(mark)
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    if PriRoom:getInstance().m_tabPriData and PriRoom:getInstance().m_tabPriData.dwPersonalRoomID then
        self.m_atlasRoomID:setString(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID.."")
    end

         -- 局数
    if PriRoom:getInstance().m_tabPriData and PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
        local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
        --self.m_txtRoom:setPosition(80, 23)
        self.m_atlasCount:setVisible(true)
        self.m_atlasCount:setString(strcount)
        if PriRoom:getInstance().m_tabPriData.dwDrawCountLimit>1000 then
            self._csbNode:getChildByName("Sprite_txtPlayCount"):setVisible(false)
            self.m_atlasCount:setString("")
        end
    end
    

   
    --self.m_atlasCount:setString(strcount)

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()

    self.m_btnInvite:setVisible(false)
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
	
    local chairCount = self._gameLayer.wPlayerCount or 3
    
    if self._gameLayer.m_cbGameStatus > 0 or self._gameLayer._gameView:getUserCount() == chairCount or GlobalUserItem.bVideo  then
        self.m_btnInvite:setVisible(false)
        return
    end
    -- 邀请按钮

    if self._gameLayer._gameView:getUserCount() < chairCount then
            self.m_btnInvite:setVisible(true)
    end  
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table , dataBuffer )
    self._gameLayer.isPriEnd=true
    self:removeChildByName(MARK_LAYER_NAME)
    self:removeChildByName("private_end_layer")

    local csbNode = ExternalFun.loadCSB("privateroom/game/PrivateGameEnd.csb", self.m_rootLayer)
    
    csbNode:setName("private_end_layer")
    csbNode:setVisible(false)

    
    

    self.priEndLayer = csbNode
    --大赢家显示
    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()
print("大赢家是：",bigWinner)
     -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    dump(scoreList , "玩家成绩")
    
    local count = self._gameLayer.m_userPriRecord.count
    local detail = clone(self._gameLayer.m_userPriRecord.detail)
    local bomboCount = clone(self._gameLayer.m_userPriRecord.bomboCount)
dump(self._gameLayer.m_userPriRecord , "流水详情")
    local winCount ={0,0,0}
    local maxScore ={0,0,0}

    for i=1,3 do
        for j=1,count do
            if detail[i][j] >=0 then
                winCount[i] = winCount[i] +1
            end
        end
    end

    for i=1,3 do
        for j=1,count do
            if detail[i][j] > maxScore[i] then
                maxScore[i] = detail[i][j]
            end
        end
    end

    if bigWinner==65535 then
        local max = 0
        for i=1,self._gameLayer.wPlayerCount do
            if scoreList[i]>max then
                max = scoreList[i]
                bigWinner = i-1
            end
            
        end
    end


    csbNode:getChildByName("fanghao"):setString("房间号:  "..(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000"))
   
    csbNode:getChildByName("jushu"):setString("局数:  "..count.."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)


    local endTime = csbNode:getChildByName("time")

    if not GlobalUserItem.bVideo then
        endTime:setString(""..cmd_table.sysEndTime.wYear.."/"..cmd_table.sysEndTime.wMonth.."/"..cmd_table.sysEndTime.wDay)
    else
        endTime:setVisible(false)
    end

  
    if self._gameLayer.wPlayerCount == 2 then
        csbNode:getChildByName("FileNode_3"):setVisible(false)
    end
    
dump(self._gameLayer.m_tabUserItem , "玩家信息")
    for i=1,self._gameLayer.wPlayerCount do
        local useritem = self._gameLayer.m_tabUserItem[i]
        if nil ~= useritem then

            local node = csbNode:getChildByName("FileNode_"..i)
            
            -- 头像
            local head = PopupInfoHead:createNormal(useritem, 90)
            head:enableInfoPop(false)
            node:getChildByName("Node_head"):addChild(head)

            -- 昵称
            local temp = node:getChildByName("Text_name")
            temp:setString(useritem.szNickName.."")

            local temp = node:getChildByName("Text_id")
            temp:setString("ID:"..useritem.dwGameID)

            node:getChildByName("Text_recoad"):setString(winCount[i].."胜"..(count-winCount[i]).."负")

            node:getChildByName("Text_maxScore"):setString(maxScore[i])
            node:getChildByName("Text_boom"):setString(bomboCount[i])
            --赢家ICON
            local winnerIcon = node:getChildByName("maxWin")
            if bigWinner == i-1 and scoreList[i] ~= 0 then
                winnerIcon:setVisible(true)
                local bg = node:getChildByName("Sprite_1")
                bg:setSpriteFrame( cc.Sprite:create("privateroom/game/img_winBg.png"):getSpriteFrame() )
            else 
                winnerIcon:setVisible(false)  
            end

            --分数
            local winScore = node:getChildByName("AtlasLabel_win")
            local loseScore = node:getChildByName("AtlasLabel_lose")

            local scoreNum = scoreList[i]
            if  scoreNum < 0 then
                scoreNum = math.abs(scoreNum)
                loseScore:setString("/"..scoreNum)
            else
                winScore:setString("/"..scoreNum)
            end


        end    
    end
    
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end


    -- 分享按钮
    btn = csbNode:getChildByName("Button_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:setVisible(true)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = csbNode:getChildByName("Button_exit")
    btn:setTag(TAG_ENUM.BTN_QUIT)
    btn:addTouchEventListener(btncallback)

     csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(3.5),
         cc.CallFunc:create(function()
             
            if not (self._gameLayer._gameView._resultLayer:isVisible()) then
                csbNode:setVisible(true)
            end
     end)))
 end

  

function PriGameLayer:onExit()
    self._gameLayer.bPriGameEnd=false
end

return PriGameLayer