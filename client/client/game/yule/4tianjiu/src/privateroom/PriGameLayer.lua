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
local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.privateroom.DismissLayer")
local MarkLayer = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.privateroom.MarkLayer")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local cmd = appdf.req(appdf.GAME_SRC.."yule.4tianjiu.src.models.CMD_Game")

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
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    self:setName("PriGameLayer")
    --
    -- 房间ID
    self.m_atlasRoomID = csbNode:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = csbNode:getChildByName("num_count")
    self.m_atlasCount:setString("0/0")

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 局数按钮
    local btn = csbNode:getChildByName("btn_round")
    btn:setTag(TAG_ENUM.BTN_JUSHU)
    btn:addTouchEventListener(btncallback)
    self.btnRound = btn

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)

    self.m_btnInvite_0 = csbNode:getChildByName("bt_invite_0")
    self.m_btnInvite_0:setTag(TAG_ENUM.BTN_INVITE)
    self.m_btnInvite_0:addTouchEventListener(btncallback)

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView):setLocalZOrder(yl.MAX_INT)
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
                                local shareTxt = "局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit .. "局。牌九游戏精彩刺激, 一起来玩吧! "
                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=14&a=1" .. "&p=" .. passwd

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
    elseif TAG_ENUM.BTN_JUSHU == tag then
        local bExit=self._gameLayer._gameView:createZhanjiView()
        if not bExit then 
            self._gameLayer:sendZhanjiRequest(math.max(1,PriRoom:getInstance().m_tabPriData.dwPlayCount-32),math.max(1,PriRoom:getInstance().m_tabPriData.dwPlayCount)) 
        end
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString(string.format("%06d", (PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000")))

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. "/" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)

      self.btnRound:loadTextures("game/btnInfo0.png","game/btnInfo0.png")
      self.btnRound:getChildByName("Text_1"):setVisible(true)
    if GlobalUserItem.bPrivateGoldRoom then
        -- self.m_atlasCount:setVisible(false)
         -- self.btnRound:getChildByName("Text_1"):setString("");
        --  self.btnRound:getChildByName("Text_1"):setPositionX(self.btnRound:getContentSize().width/2)
        -- self.btnRound:getChildByName("Text_1"):setFontSize(26)
    else
        -- self.m_atlasCount:setVisible(true)
        -- self.btnRound:getChildByName("Text_1"):setString("局   数：");
        -- self.btnRound:getChildByName("Text_1"):setTextColor(cc.c3b(0x9d,0x96,0xcc))
        -- self.btnRound:getChildByName("Text_1"):setPositionX(31)
        -- self.btnRound:getChildByName("Text_1"):setFontSize(24)
    end

    if GlobalUserItem.bPrivateGoldRoom  and GlobalUserItem.dwCurRoomGroupID == 0 then
        self.btnRound:getChildByName("Text_1"):setString("");
        self.m_atlasCount:setString("")
    else
        self.btnRound:getChildByName("Text_1"):setString("局   数：");
    end

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    print("PriRoom:getInstance().m_tabPriData.dwPlayCount:",PriRoom:getInstance().m_tabPriData.dwPlayCount)
    if PriRoom:getInstance().m_tabPriData.dwPlayCount and PriRoom:getInstance().m_tabPriData.dwPlayCount>=1 then
        if nil ~= self._gameLayer.onGetSitUserNum then
            local chairCount = PriRoom:getInstance():getChairCount()
            if self._gameLayer:onGetSitUserNum() == chairCount then
                self.m_btnInvite:setVisible(false)
                self.m_btnInvite_0:setVisible(false)
                return
            end
        end
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return 
    end

    -- 非房主、且密码房
    if not PriRoom:getInstance().m_bIsMyRoomOwner and true == PriRoom:getInstance().m_tabPriData.bRoomPwd then
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return
    end
    --空闲场景
    if self._gameLayer.m_cbGameStatus ~= 0 then
        self.m_btnInvite:setVisible(false)
        print("self._gameLayer.m_cbGameStatus:",self._gameLayer.m_cbGameStatus)
        self.m_btnInvite_0:setVisible(true)
        return
    end
    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            self.m_btnInvite_0:setVisible(false)
            return
        end
        print("self._gameLayer:onGetSitUserNum()",self._gameLayer:onGetSitUserNum())
        print("chairCount",chairCount)
    end

    if PriRoom:getInstance().m_tabPriData.dwPlayCount and PriRoom:getInstance().m_tabPriData.dwPlayCount > 0 then 
        self.m_btnInvite:setVisible(false)
        self.m_btnInvite_0:setVisible(true)
        return
    end

    self.m_btnInvite:setVisible(not GlobalUserItem.bVideo)
    self.m_btnInvite_0:setVisible(false)
end

function PriGameLayer:onPriGameEnd(cmd_table, dataBuffer)
    local dataBuffer = dataBuffer;
    local datalen = dataBuffer:getlen()

    LogAsset:getInstance():logData("onPriGameEnd")
    self._gameLayer.bEnd=true
    self._gameLayer._gameView:removeChildByName("private_end_layer")
    self:onPriGameEndDelay(cmd_table, dataBuffer) 

    -- self:runAction(cc.Sequence:create(cc.DelayTime:create(2.0),
    --     cc.CallFunc:create(function()
    --         self:onPriGameEndDelay(cmd_table, dataBuffer)         
    --     end)))
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEndDelay(cmd_table, dataBuffer) 

    self._gameLayer.bPriGameEnd=true
    self:removeChildByName(MARK_LAYER_NAME)

    local csbNode =cc.CSLoader:createNode("game/PriGameEndLayer.csb")
    csbNode:addChild(cc.LayerColor:create(cc.c4b(30, 30, 30, 200), 1334, 750),-1)
    csbNode:setVisible(false):setLocalZOrder(99999999)
    csbNode:setName("private_end_layer")

    csbNode.onTouchBegan=function(csbNode,touch, event) return true end
    ExternalFun.registerTouchEvent(csbNode,true)

    self._gameLayer._gameView:addChild(csbNode)

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

   
    local winScoreColor=cc.c4b(0x62,0xb9,0x78,0xff)
    local loseScoreColor=cc.c4b(0xdf,0x51,0x51,0xff)
    local meInfoColor=cc.c4b(0xff,0xc4,0x5e,0xff)
    local otherInfoColor=cc.c4b(0xff,0xff,0xff,0xff)

    local chairCount = PriRoom:getInstance():getChairCount()
    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]

    --大赢家
    -- local datalen = dataBuffer:getlen()

    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte();
    end
    local bigWinner = dataBuffer:readword();
    -- print(bigWinner,"----------大赢家-------------")

    local curNum = 1
    for i = 1, chairCount do --chairid
        local cellbg = csbNode:getChildByName("FileNode_" .. i);
        local icon_win_player = cellbg:getChildByName("icon_win_player");
        icon_win_player:setVisible(false);

        cellbg:setVisible(true)
        -- if chairCount<4 then cellbg:setPositionY(cellbg:getPositionY()-30) end
        local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
        
        if nil ~= useritem and self._gameLayer.cbPlayStatus[i] == cmd.PS_SITTING then
            curNum = curNum + 1
            cellbg.userItem = useritem
            -- 头像
            local head = PopupInfoHead:createNormal(useritem, 50)
            local sz=cellbg:getChildByName("headbg"):getContentSize()
           head:addTo(cellbg:getChildByName("headbg")):setPosition(sz.width/2,sz.height/2)

           local infoColor= (i-1==self._gameLayer.myChairId) and meInfoColor or otherInfoColor
           local strNickname = string.EllipsisByConfig(useritem.szNickName, 100, string.getConfig("fonts/round_body.ttf", 20))
           cellbg:getChildByName("name"):setString(strNickname):setTextColor(infoColor)
           assert(self._gameLayer)
           assert(not tolua.isnull(self._gameLayer))
           local a,b,c=0,0,0
           local recordData = self._gameLayer.recordData;
           if recordData then
                a = recordData.cbWinCount[1][i];
                b = recordData.cbLoseCount[1][i];
                c = recordData.cbEqualCount[1][i];
           end
           cellbg:getChildByName("win"):setString(string.format("胜%d次",a)):setTextColor(infoColor)
           cellbg:getChildByName("lose"):setString(string.format("负%d次",b)):setTextColor(infoColor)
           cellbg:getChildByName("draw"):setString(string.format("平%d次",c)):setTextColor(infoColor)
            
            local score = scoreList[i] or 0
            -- 成绩
            local scoreColor
            local strScore = score .. ""
            if score >= 0 then strScore = "+" .. score end
            cellbg:getChildByName("score"):setString(strScore):setTextColor(score>=0 and winScoreColor or loseScoreColor )

            -- 大赢家标志
            if bigWinner == i - 1 then
                icon_win_player:setVisible(true);
            else
                icon_win_player:setVisible(false);
            end
        else
            cellbg:setVisible(false);
        end
    end

    -- 分享按钮
    local btn = csbNode:getChildByName("btn_share")
    btn:setTag(TAG_ENUM.BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = csbNode:getChildByName("btn_exit")
    btn:setTag(TAG_ENUM.BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    -- csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(3.0),
    --     cc.CallFunc:create(function()
    --         csbNode:setVisible(true)
    --     end)))
    csbNode:setVisible(true)
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._gameLayer.bPriGameEnd=false
    self._dismissLayer:onExit()
end

return PriGameLayer