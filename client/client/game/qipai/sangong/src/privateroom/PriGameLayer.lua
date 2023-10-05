local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local DismissLayer = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.privateroom.DismissLayer")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
local BTN_INFO = 106   

function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
   
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    local image_bg = csbNode:getChildByName("sp_gameInfo")

    --房号
    self.m_atlasRoomID = image_bg:getChildByName("txt_roomID")
    self.m_atlasRoomID:setString("000000")

    --局数
    self.m_atlasCount = image_bg:getChildByName("txt_round")
    self.m_atlasCount:setString("0/0")

    self.m_txtRule = image_bg:getChildByName("txt_rule")
    self.m_txtRule:setString("通用 玩法")


	local funCallback = function(ref)
		self:onButtonCallback(ref:getTag(), ref)
	end

    --背景
    image_bg:setTag(BTN_INFO)
    image_bg:addTouchEventListener(funCallback)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(funCallback)


    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, 10)

    if GlobalUserItem.bPrivateRoom then
        self.m_rootLayer:setVisible(true)
    end
end

function PriGameLayer:onButtonCallback( tag, sender )
    if BTN_DISMISS == tag then              -- 请求解散游戏
        -- local useritem = self._gameLayer:GetMeUserItem()

        -- if self._gameLayer.m_cbGameStatus == 0 and useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
            
        --     PriRoom:getInstance():getNetFrame():sendDissumeGame(self._gameLayer:GetMeTableID())

        --    -- self.m_rootLayer:removeAllChildren()
        --     --GlobalUserItem.bWaitQuit = false
        --    -- self._gameLayer:onExitRoom()
        -- else
        --     PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum())
        -- end
        PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum())
    elseif BTN_INVITE == tag then
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
                                local szRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0
                                
--                                local strPlayerCount = self._gameLayer.playerCount == 0 and "6人," or self._gameLayer.playerCount.."人,"
                                local strPlayerCount = ""

                                local shareTxt = "局数:" .. drawCount .. "局。"..strPlayerCount..cmd.GAME_NAME.."精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. szRoomID .. "&k="..cmd.KIND_ID.."&a=1" .. "&p=" .. passwd
                                print("获取分享信息===》", szRoomID,shareTxt,  url)
                                MultiPlatform:getInstance():shareToTarget(
                                    target, 
                                    sharecall, 
                                    "【约战房间: " .. szRoomID .. " 】", 
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
    self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    local strcount = ""..PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)

    if GlobalUserItem.bPrivateGoldRoom then
         self.m_atlasCount:setString("第" .. PriRoom:getInstance().m_tabPriData.dwPlayCount.. "局")
    end

    --玩法
    local rule = {"霸王庄", "轮流坐庄", "牌大坐庄", "明牌抢庄", "自由抢庄", "通比模式"}
    self.m_txtRule:setString(rule[self._gameLayer.cbRobBankerType])
    


    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    print("PriRoom:getInstance().m_tabPriData.dwPlayCount = ",PriRoom:getInstance().m_tabPriData.dwPlayCount)
    if PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
        self.m_btnInvite:setVisible(true)
    else
        self.m_btnInvite:setVisible(false)
    end 
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
    -- if GlobalUserItem.bPrivateGoldRoom and self._gameLayer.m_cbGameStatus == cmd.GS_TK_FREE then
    --     self.m_btnInvite:setVisible(true)
    --     return
    -- end
    if self._gameLayer.m_cbGameStatus > cmd.GAME_STATUS_FREE or self._gameLayer.m_bStartGame then --空闲场景
        self.m_btnInvite:setVisible(false)
        return
    end

    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
--        print("邀请按钮,系统下发，坐下人数",chairCount, self._gameLayer:onGetSitUserNum())
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            return
        end
    end
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table )
--    dump(cmd_table, "PriGameLayer:onPriGameEnd")
    self._gameLayer.isPriOver = true
    self:removeChildByName("private_end_layer")

    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self.m_rootLayer)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")

     local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonCallback(ref:getTag(),ref)
        end
    end

    local bgImg = csbNode:getChildByName("Image_1")
    local chairCount = 8

        -- 分享按钮
    local btn = bgImg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:setVisible(true)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = bgImg:getChildByName("btn_back")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    btn = bgImg:getChildByName("btn_close")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)


     --创建时间
    local startHour = cmd_table.sysStartTime.wHour>=10 and cmd_table.sysStartTime.wHour or "0"..cmd_table.sysStartTime.wHour
    local startMin = cmd_table.sysStartTime.wMinute>=10 and cmd_table.sysStartTime.wMinute or "0"..cmd_table.sysStartTime.wMinute

    local endHour = cmd_table.sysEndTime.wHour>=10 and cmd_table.sysEndTime.wHour or "0"..cmd_table.sysEndTime.wHour
    local endMin = cmd_table.sysEndTime.wMinute>=10 and cmd_table.sysEndTime.wMinute or "0"..cmd_table.sysEndTime.wMinute
    local roomDate = cmd_table.sysStartTime.wYear.."年"..cmd_table.sysStartTime.wMonth.."月"..cmd_table.sysStartTime.wDay.."日  "
                                ..startHour..":"..startMin.."~"..endHour..":"..endMin

    local txtTime = bgImg:getChildByName("time")
    txtTime:setString(roomDate)

    --房号和局数
    local txt_roomID = bgImg:getChildByName("roomid")
    txt_roomID:setString("房号："..PriRoom:getInstance().m_tabPriData.dwPersonalRoomID)

    local txt_round = bgImg:getChildByName("roundID")
    txt_round:setString("局数："..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)

    -- 玩家成绩
    local scoreList = {}
    --dump(scoreList, "scoreList", 6)

    local winnerGod = 0
    for i = 1, cmd.GAME_PLAYER do
        table.insert(scoreList,cmd_table.lScore[1][i])
        if cmd_table.lScore[1][i] > winnerGod then
            winnerGod = cmd_table.lScore[1][i] 
        end
    end

    --结算界面
    local realIndex  = 0;
    local Panel_2 = bgImg:getChildByName("Panel_2")

    for i=1,8 do
        local useritem = self._gameLayer:getSaveUserInfos(i -1)

        if useritem then --and  (scoreList[i] ~= 0 or self._gameLayer.cbPlayStatus[i]==1) then 
                realIndex = realIndex +1
                local infoNode = Panel_2:getChildByName("FileNode_"..realIndex)
                infoNode:setVisible(true)

                local cell = nil
                if scoreList[i] > 0 then
                    cell = cc.CSLoader:createNode("game/NodeInfo2.csb"):addTo(infoNode)
                    --显示大赢家
                    print("大赢家判断------》", scoreList[i], winnerGod)
                    if scoreList[i] == winnerGod then
                        local spBigWiner = cell:getChildByName("sp_bigWiner")
                        spBigWiner:setVisible(true)
                    end
                else
                    cell = cc.CSLoader:createNode("game/NodeInfo1.csb"):addTo(infoNode)
                end

                --头像
                local nodeHead = cell:getChildByName("nodeHead")
                local head = PopupInfoHead:createNormal(useritem, 80)
                -- head:setScale(0.53)
                head:setAnchorPoint(cc.p(0.5,0.5))
                -- head:setPosition(cc.p(29, 0))
                nodeHead:addChild(head)
 

                --昵称
                local txtNick = cell:getChildByName("txtNick")
                local nick = string.EllipsisByConfig(useritem.szNickName, 140, 
                                            string.getConfig("fonts/round_body.ttf", 22))
                txtNick:setString(nick)

                --id
                 local txtID = cell:getChildByName("txtID")
                 txtID:setString("id:"..useritem.dwGameID)

                local allScore = cell:getChildByName("txtScore") 
                allScore:setString(scoreList[i])              
                allScore:setString("/"..math.abs(scoreList[i]))
            
  
                    --房主标识
                -- local fangzhuicon = infoNode:getChildByName("Sprite_50")
                -- fangzhuicon:setVisible(false)
                -- if PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID == useritem.dwUserID then
                --     fangzhuicon:setVisible(true)
                --     fangzhuicon:setLocalZOrder(5)
                -- end
        end
    end

    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(2.0),
    cc.CallFunc:create(function()
        csbNode:setVisible(true)
    end)))
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
end

return PriGameLayer