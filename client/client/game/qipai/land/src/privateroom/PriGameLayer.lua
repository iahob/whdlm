--
-- Author: zhong
-- Date: 2016-12-29 11:13:57
--
-- 私人房游戏顶层
local module_pre = "game.qipai.land.src"
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local DismissLayer = appdf.req(appdf.GAME_SRC.."qipai.land.src.privateroom.DismissLayer")
local MarkLayer = appdf.req(appdf.GAME_SRC.."qipai.land.src.privateroom.MarkLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd = appdf.req(module_pre .. ".models.CMD_Game")


local TAG_START             = 100
local enumTable = 
{
    "BTN_ENTER",            -- 加入
    "BTN_INVITE",           -- 邀请
    "BTN_QUIT",             -- 退出
    "BTN_JUSHU",            -- 局数
    "BTN_SHARE",            -- 分享
    "BTN_BACK",            
    "BTN_MASK",          
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local MARK_LAYER_NAME = "__mark_layer_name__"
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.roomNode = csbNode:getChildByName ("Node_room")
    self.m_atlasRoomID = self.roomNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")
    self.roomX, self.roomY = self.roomNode:getPosition()


    -- 局数
    self.roundNode = csbNode:getChildByName("Node_round")
    self.m_atlasCount = self.roundNode:getChildByName("num_count")
    self.m_atlasCount:setString("0")
    self.m_atlasCount1 = self.roundNode:getChildByName("num_count1")
    self.m_atlasCount1:setString("/ 0")
    self.roundX, self.roundY = self.roundNode:getPosition()


    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 局数按钮
    self.btnRound = csbNode:getChildByName("btn_round")
    self.btnRound:setTag(TAG_ENUM.BTN_JUSHU)
    self.btnRound:addTouchEventListener(btncallback)

    --如果是金币房卡
    if GlobalUserItem.bPrivateGoldRoom and not self._gameLayer.m_bClubRoom then
         self.roundNode:setVisible(false)
         self.roomNode:setPosition(cc.p(self.roomX, self.roomY+12))
    else
         self.roundNode:setVisible(true)
         self.roomNode:setPosition(cc.p(self.roomX, self.roomY))
    end

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer)
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
                                        self:runAction(cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function ()
                                                showToast(self, "分享成功", 2)
                                        end)))
                                    end
                                end
                                local passwd = cmd_table.dwRoomDwd or 0
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local baseScore = "底分:"..self._gameLayer.m_nDiFen..","

                                local shareTxt = "局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit .. "局。"..baseScore.."斗地主游戏精彩刺激, 一起来玩吧! "
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=200&a=1" .. "&p=" .. passwd
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
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(2.0), cc.CallFunc:create(function ()
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
    elseif TAG_ENUM.BTN_JUSHU == tag then
        local mark = MarkLayer:create(self._gameLayer, self._gameLayer.m_nDiFen)
        mark:setName(MARK_LAYER_NAME)
        self:addChild(mark)
    elseif TAG_ENUM.BTN_BACK == tag or TAG_ENUM.BTN_MASK then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    self.m_atlasCount:setString(PriRoom:getInstance().m_tabPriData.dwPlayCount)
    self.m_atlasCount1:setString(" / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)

    --如果是金币房卡，替换纹理
    if GlobalUserItem.bPrivateGoldRoom and not self._gameLayer.m_bClubRoom then       
        self.roundNode:setVisible(false)
        self.roomNode:setPosition(cc.p(self.roomX, self.roomY+12))
    else
         self.roundNode:setVisible(true)
         self.roomNode:setPosition(cc.p(self.roomX, self.roomY))
    end

    --金币私有房不显示局数
    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    print("onRefreshInviteBtn-------------------------------------------------------->", self._gameLayer.m_cbGameStatus, PriRoom:getInstance():getChairCount())

    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
    --空闲场景
    if self._gameLayer.m_cbGameStatus ~= 0 then
        self.m_btnInvite:setVisible(false)
        return
    end
    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        
        if self._gameLayer:onGetSitUserNum() == 3 or self._gameLayer.m_cbGameStatus ~= 0 then
            self.m_btnInvite:setVisible(false)
            return
        end
    end
    self.m_btnInvite:setVisible(true)
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )
    local ZORDER = {}
    ZORDER.ZORDER_HEADBG = 2
    ZORDER.ZORDER_HEAD = 1

    self._gameLayer.bPriGameEnd=true
    self:removeChildByName(MARK_LAYER_NAME)
    self:removeChildByName("private_end_layer")
    local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID.."" or ""
    local baseScore = self._gameLayer.m_nDiFen.."" or ""
    local roundText = PriRoom:getInstance().m_tabPriData.dwPlayCount.."/"..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or ""

    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self.m_rootLayer)
    self.endCsbNode = csbNode
    self.roomInfo = csbNode:getChildByName("Node_roomInfo")

    self.roomInfo:getChildByName("Text_roomId"):setString(dwRoomID)
    self.roomInfo:getChildByName("Text_lBaseScore"):setString(baseScore)
    self.roomInfo:getChildByName("Text_roundNum"):setString(roundText)

    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")

    if self._gameLayer.bIsHaveShowEnd then
        csbNode:setVisible(true)
        self._gameLayer.bIsHaveShowEnd =false
    end

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    PriGameLayer.USERACHIEVEMENT = {
        {k = "userachievement", t = "table", d = cmd.USERACHIEVEMENT, l = {cmd.PLAYER_COUNT}},                          
    }

    local cmd_data = ExternalFun.read_netdata(PriGameLayer.USERACHIEVEMENT, dataBuffer)
    local image_bg = csbNode:getChildByName("Image_bg")
    local chairCount = PriRoom:getInstance():getChairCount()
    
    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()

    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    local winnerScore = {}

    for i = 1, chairCount do
        local endData = cmd_data.userachievement[1][i]
        local cellbg = csbNode:getChildByName("playerCell_" .. i)
        cellbg:getChildByName("headBg"):setZOrder(ZORDER.ZORDER_HEADBG)

        if nil ~= cellbg then
            cellbg:getChildByName("winner_flag"):setVisible(false)
            cellbg:setVisible(true)
            local cellsize = cellbg:getContentSize()            
            local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
            if nil ~= useritem then
                -- 头像
                local file = "privateroom/totalresult/bg_1.png"
                local head = HeadSprite:createNormal(useritem, 94)
                head:setPosition(cellsize.width/2, cellsize.height * 0.75+28.5)
                head:setZOrder(ZORDER.ZORDER_HEAD)
                head:enableHeadFrame(false)
                cellbg:addChild(head)

                local img = cc.Sprite:create("privateroom/totalresult/bg_1.png")
            else
                cellbg:setVisible(false)
            end

            -- 昵称
            local name =  string.EllipsisByConfig(endData.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28))
            cellbg:getChildByName("Text_Name"):setString(name)

            --ID
            cellbg:getChildByName("Text_num"):setString(endData.dwGameID.."")

            local score = scoreList[useritem.wChairID + 1] or 0
            table.insert(winnerScore, score)

            -- 成绩
            if score >= 0 then
                cellbg:getChildByName("AtlasLabel_totalScore"):setString("/"..math.abs(score))
                cellbg:getChildByName("AtlasLabel_totalScore_1"):setString("")
            else
                cellbg:getChildByName("AtlasLabel_totalScore_1"):setString("/"..math.abs(score))
                cellbg:getChildByName("AtlasLabel_totalScore"):setString("")
            end

            --炸弹等次数
            cellbg:getChildByName("AtlasLabel_bomb"):setString(endData.cbBombCount)
            cellbg:getChildByName("AtlasLabel_air"):setString(endData.cbPlaneCount)
            cellbg:getChildByName("AtlasLabel_rocket"):setString(endData.cbMissileCount)
            cellbg:getChildByName("AtlasLabel_spring"):setString(endData.cbChunTianCount)
            cellbg:getChildByName("AtlasLabel_reverSpring"):setString(endData.cbFanChunTianCount) 

            -- if i == chairCount then
            --     local maxScoreIndex = 1
            --     for j=2, chairCount do
            --         if winnerScore[maxScoreIndex] >= winnerScore[j] then
            --         else
            --             maxScoreIndex = j
            --         end
            --     end

            --     --防止玩家分数相等
            --     for i=1, chairCount do
            --         if winnerScore[maxScoreIndex] == winnerScore[i] then
            
            --         end
            --     end

            --     -- if maxScoreIndex == 0 then
            --     -- else
            --     --     cellbg:getChildByName("winner_flag"):setVisible(true)
            --     -- end
            -- end    
        end  
    end

    if bigWinner ~= yl.INVALID_CHAIR then 
        csbNode:getChildByName("playerCell_" .. (bigWinner+1)):getChildByName("winner_flag"):setVisible(true)
    end

    -- 分享按钮
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    btn = image_bg:getChildByName("btn_back")
    btn:setTag(TAG_ENUM.BTN_BACK)
    btn:addTouchEventListener(btncallback)

    btn = image_bg:getChildByName("btn_quit")
    btn:setTag(TAG_ENUM.BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = csbNode:getChildByName("mask")
    btn:setTag(TAG_ENUM.BTN_MASK)
    btn:addTouchEventListener(btncallback)

    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(3.0),
        cc.CallFunc:create(function()
            --csbNode:setVisible(true)
        end)))
end


function PriGameLayer:showEndLayer()
    if self.endCsbNode then
            self.endCsbNode:setVisible(true)
    end
end


function PriGameLayer:onExit()
    self._gameLayer.bPriGameEnd=false
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/land_game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/land_game.png")
end

return PriGameLayer