--
-- Author: David
-- Date: 2017-4-11 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.privateroom.DismissLayer")
local cmd = appdf.req(appdf.GAME_SRC.."yule.sparrowgdex.src.models.CMD_Game")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
local BTN_INFO = 106


function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self)--(cmd.RES_PATH.."privateroom/game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    --
    --local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")
    self.m_atlasRoomID:setVisible(false)

    -- 局数
    csbNode:getChildByName("txt_room"):setVisible(false)
    self.m_atlasCount = csbNode:getChildByName("num_count")
    self.m_atlasCount:setString("0 / 0")
    self.m_atlasCount:setVisible(false)


    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- -- 解散按钮
    -- local btn = csbNode:getChildByName("bt_dismiss")
    -- btn:setTag(BTN_DISMISS)
    -- btn:addTouchEventListener(btncallback)

    -- -- 暂离按钮
    -- btn = csbNode:getChildByName("bt_zanli")
    -- btn:setTag(BTN_ZANLI)
    -- btn:addTouchEventListener(btncallback)

    --详情按钮
    local btn = csbNode:getChildByName("btn_info")
    btn:setTag(BTN_INFO)
    btn:addTouchEventListener(btncallback)
    btn:setVisible(false)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setPositionY(140)
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, self._gameLayer._gameView.ZORDER_RESULT+1)
    self.thisEndData = nil
end

function PriGameLayer:setGameInfoBtn()
    local btn = self.m_csbNode:getChildByName("btn_info")

    btn:loadTextureNormal("game/btn_roomInfo1.png")
    btn:loadTexturePressed("game/btn_roomInfo1.png")
    btn:loadTextureDisabled("game/btn_roomInfo1.png")

    self.m_atlasCount:setVisible(false)
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
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
        print("获取分享信息===》",self.m_atlasRoomID:getString())
        PriRoom:getInstance():queryRoomPasswd(
            PriRoom:getInstance().m_tabPriData.dwPersonalRoomID, 
            function(cmd_table)
                -- 第三方分享
                local scene = PriRoom:getInstance():getPlazaScene()
                print("获取分享信息===》",scene)
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
                                local szRoomID = self.m_atlasRoomID:getString()
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0

                                local strMa = self._gameLayer.cbMaCount == 0 and "无马," or self._gameLayer.cbMaCount.."马,"
                                local mode = {"无鬼,", "白板做鬼,", "翻鬼,", "翻双鬼,",}
                                local strMagicMode = mode[self._gameLayer.cbMagicMode +1]
                                local strQiangGangHu = self._gameLayer.bQiangGangHu and "可抢杠胡," or ""
                                local strHuQiDui = self._gameLayer.bHuQiDui and "可胡七对," or ""
                                local strHaveZiCard = self._gameLayer.bHaveZiCard and "带风," or "不带风,"
                                local strNoMagicDouble = self._gameLayer.bNoMagicDouble and "无鬼翻倍," or ""

                                local shareTxt = "局数:" .. drawCount .. "局。"..strMa..strMagicMode..strQiangGangHu..strHuQiDui..strHaveZiCard..strNoMagicDouble.."广东麻将精彩刺激, 一起来玩吧!"
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. szRoomID .. "&k=391&a=1" .. "&p=" .. passwd
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
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)
    if GlobalUserItem.bPrivateGoldRoom  and GlobalUserItem.dwCurRoomGroupID == 0 then
        self.m_atlasCount:setString("")
    end

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        return
    end
    if self._gameLayer.m_cbGameStatus ~= 0 then --空闲场景
        self.m_btnInvite:setVisible(false)
        return
    end

    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        print("邀请按钮,系统下发，坐下人数",chairCount, self._gameLayer:onGetSitUserNum())
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            return
        end
    end
    self.m_btnInvite:setVisible(true)
end

function PriGameLayer:onShowGameEnd(  )
    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self._gameLayer._gameView)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")
    csbNode:setLocalZOrder(90)
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self._endLayer = csbNode
    if not self._gameLayer.bIsShowOver  then 
        csbNode:setVisible(true)
    end
    local image_bg = csbNode:getChildByName("Image_bg")

    local chairCount = PriRoom:getInstance():getChairCount()

    print("总结算信息====================>", 10)
    -- 玩家成绩
    local scoreList = self.thisEndData.lScore[1]

    --房间ID
    local labID = image_bg:getChildByName("text_roomID")
    if nil ~= labID then
        labID:setString("房间ID:"..self.m_atlasRoomID:getString())
    end
    --时间
    local labTime = image_bg:getChildByName("text_time")
    if nil ~= labTime and not GlobalUserItem.bVideo then
        local startHour = self.thisEndData.sysStartTime.wHour>=10 and self.thisEndData.sysStartTime.wHour or "0"..self.thisEndData.sysStartTime.wHour
        local startMin = self.thisEndData.sysStartTime.wMinute>=10 and self.thisEndData.sysStartTime.wMinute or "0"..self.thisEndData.sysStartTime.wMinute

        local endHour = self.thisEndData.sysEndTime.wHour>=10 and self.thisEndData.sysEndTime.wHour or "0"..self.thisEndData.sysEndTime.wHour
        local endMin = self.thisEndData.sysEndTime.wMinute>=10 and self.thisEndData.sysEndTime.wMinute or "0"..self.thisEndData.sysEndTime.wMinute
        labTime:setString("时间:"..self.thisEndData.sysStartTime.wYear.."-"..self.thisEndData.sysStartTime.wMonth.."-"..self.thisEndData.sysStartTime.wDay.."("
                                ..startHour..":"..startMin.."~"..endHour..":"
                                ..endMin..")")
    end


    

    -- 分享按钮
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = image_bg:getChildByName("btn_quit")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    
             --csbNode:setVisible(true)
             --游戏记录
    local tabUserRecord = self._gameLayer:getDetailScore()
    for i = 1, chairCount do
        local useritem = self._gameLayer:getUserInfoByChairID(i)
        --dump(useritem, "useritem", 6)

        --头像
        local nodeFace = csbNode:getChildByName("Node_face" .. i)

        print("get user face node ================>", nodeFace, "Node_face" .. i)
        if nil ~= nodeFace then
            -- 头像
            local head = PopupInfoHead:createNormal(useritem, 95)
            head:enableInfoPop(false)
            nodeFace:addChild(head)
        end

        local cellbg = image_bg:getChildByName("infoNode_" .. i)
        if nil ~= cellbg then
            cellbg:setVisible(true)

            if self.bigWinner+1 == i then
                cellbg:getChildByName("maxWin"):setVisible(true)
            end

            local cellsize = cellbg:getContentSize()            

            if nil ~= useritem then
                -- 昵称
                local textNickname = cellbg:getChildByName("Text_name")
                local strNickname = string.EllipsisByConfig(useritem.szNickName, 190, 
                                                            string.getConfig("fonts/round_body.ttf", 21))
                textNickname:setString(strNickname)
                --玩家ID
                local textUserId = cellbg:getChildByName("Text_ID")
                textUserId:setString(useritem.dwGameID)

                --胡牌次数
                local textHuNum = cellbg:getChildByName("Text_Hu")
                if nil ~= tabUserRecord[i] then
                    textHuNum:setString(tabUserRecord[i].cbHuCount)
                else
                    textHuNum:setString(0)
                end
                --公杠次数
                local text_MingGang = cellbg:getChildByName("Text_MingGang")
                if nil ~= tabUserRecord[i] then
                    text_MingGang:setString(tabUserRecord[i].cbMingGang)
                else
                    text_MingGang:setString(0)
                end
                
                --暗杠次数
                local textAnGang = cellbg:getChildByName("Text_Angang")
                if nil ~= tabUserRecord[i] then
                    textAnGang:setString(tabUserRecord[i].cbAnGang)
                else
                    textAnGang:setString(0)
                end

                --中码个数
                local textMaNum = cellbg:getChildByName("Text_Ma")
                if nil ~= tabUserRecord[i] then
                    textMaNum:setString(tabUserRecord[i].cbMaCount)
                else
                    textMaNum:setString(0)
                end

                --总成绩
                local textTotalScore = cellbg:getChildByName("Text_Score")
                textTotalScore:setString(scoreList[i])
            else
                cellbg:setVisible(false)
            end
        end        
    end
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table , dataBuffer)
    self._gameLayer.bIsPriEnd = true

    --self._gameLayer:onSendQueryRecord()
    self._gameLayer._gameView:removeChildByName("private_end_layer")

    self.thisEndData = cmd_table
    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    self.bigWinner = dataBuffer:readword()

    --if self._gameLayer._gameView.bIsVideo  or self._gameLayer.openAllResult then
        self:onShowGameEnd()
    --end
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"`
    self._dismissLayer:onLeaveGame()
end


function PriGameLayer:showEndLayer()
    if self._endLayer then 
            self._endLayer:setVisible(true)
    end
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
end

return PriGameLayer