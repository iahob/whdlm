
--
-- 私人房游戏顶层
local DismissLayer = appdf.req(appdf.GAME_SRC.."yule.sparrowxz.src.privateroom.DismissLayer")
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
local BTN_INFO = 106

function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    local image_bg = csbNode 

    -- 房间ID
    self.m_atlasRoomID = image_bg:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")
    self.m_atlasRoomID:setVisible(false)

    -- 局数
    self.m_atlasCount = image_bg:getChildByName("num_count")
    self.m_atlasCount:setString("0 / 0")
    self.m_atlasCount:setVisible(false)

    image_bg:getChildByName("txt_room"):setVisible(false)

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- -- 解散按钮
    -- local btn = image_bg:getChildByName("bt_dismiss")
    -- btn:setTag(BTN_DISMISS)
    -- btn:addTouchEventListener(btncallback)

    -- -- 暂离按钮
    -- btn = image_bg:getChildByName("bt_zanli")
    -- btn:setTag(BTN_ZANLI)
    -- btn:addTouchEventListener(btncallback)

    --详情按钮
    local btn = csbNode:getChildByName("btn_info")
    btn:setTag(BTN_INFO)
    btn:addTouchEventListener(btncallback)
    self.btn_info=btn
    btn:setVisible(false)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)

    self.invitePosY=self.m_btnInvite:getPositionY()

    self._dismissLayer = DismissLayer:create(self):addTo(self._gameLayer._gameView, self._gameLayer._gameView.ZORDER_SETTING)

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
        PriRoom:getInstance():queryDismissRoom(self._gameLayer.m_cbGameStatus, self._gameLayer:onGetSitUserNum()) --参数无意义，可以去掉
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
                                local dwRoomID = PriRoom:getInstance().m_tabPriData.dwPersonalRoomID
                                local drawCount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit or 0

                                local configtxt = string.format("%d局 ", drawCount)
                                if self._gameLayer.bHuanSanZhang == true then
                                    configtxt = configtxt .. "换三张 "
                                end

                                if self._gameLayer.bHuJiaoZhuanYi == true then
                                    configtxt = configtxt .. "呼叫转移 "
                                end

                                if self._gameLayer.bZiMoAddTimes == true then
                                    configtxt = configtxt .. "自摸加倍 "
                                end

                                if self._gameLayer.bTianDiHu == true then
                                    configtxt = configtxt .. "天地胡 "
                                end

                                if self._gameLayer.bHaiDiLaoYue == true then
                                    configtxt = configtxt .. "海底捞月 "
                                end

                                if self._gameLayer.bMenQing == true then
                                    configtxt = configtxt .. "门清*2 "
                                end

                                local shareTxt = configtxt .. "。血战麻将精彩刺激, 一起来玩吧!"
                                print("kevin", shareTxt)

                                local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=302&a=1" .. "&p=" .. passwd
                                print("dwRoomID:",dwRoomID)
                                print("GlobalUserItem.tabAccountInfo.dwGameID:",GlobalUserItem.tabAccountInfo.dwGameID)
                              
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
        self._gameLayer._gameView:sendAskRCRecord()
    end
end


------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
   -- self.m_atlasRoomID:setString(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000")
    self.m_atlasRoomID:setString(string.format("%06d", PriRoom:getInstance().m_tabPriData.dwPersonalRoomID))

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    self.m_atlasCount:setString(strcount)


    if GlobalUserItem.bPrivateGoldRoom then
       
      
        self.m_atlasCount:setVisible(false)
       
       self.btn_info:loadTextureNormal("game/btn_roomInfo1.png")
       self.btn_info:loadTexturePressed("game/btn_roomInfo1.png")
       if self.m_btnInvite:isVisible() then self.m_btnInvite:setPositionY(self.invitePosY+130) end
    else
         -- self.m_atlasCount:setVisible(true)
         self.btn_info:loadTextureNormal("game/btnInfo1.png")
         self.btn_info:loadTexturePressed("game/btnInfo2.png")
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
        print("self._gameLayer.m_cbGameStatus:",self._gameLayer.m_cbGameStatus)
        
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

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table, dataBuffer )

    self._gameLayer._gameView:removeChildByName("private_end_layer")

    self._gameLayer.bEnd=true

    local record=self._gameLayer:getRecord()

    local csbNode = cc.CSLoader:createNode("game/PrivateGameEndLayer.csb")
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer"):setLocalZOrder(99999999)
    csbNode:getChildByName("btn_close"):addClickEventListener(
        function() 
            csbNode:removeSelf() 
            GlobalUserItem.bWaitQuit = false
            self._gameLayer:onExitRoom()
        end
    )

    csbNode.onTouchBegan=function(csbNode,touch, event) return true end
    ExternalFun.registerTouchEvent(csbNode,true)

    self._gameLayer._gameView:addChild(csbNode)
   
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    csbNode:getChildByName("Text_roomid"):setString(PriRoom:getInstance().m_tabPriData.dwPersonalRoomID or "000000")
    csbNode:getChildByName("Text_date"):setString(os.date("%Y-%m-%d %X",os.time())) --用参数cmd_table(CMD_GR_PersonalTableEnd)里的时间

    local image_bg = csbNode:getChildByName("Image_bg")

    local chairCount = PriRoom:getInstance():getChairCount()
   
    for i=1,1000 - cmd_table.nSpecialInfoLen do
        dataBuffer:readbyte()
    end
    local bigWinner = dataBuffer:readword()

    --游戏记录
    for i = 1, chairCount do
        local useritem = self._gameLayer:getUserInfoByChairID(i)
        local head=PopupInfoHead:createNormal(useritem, 76)
        --dump(useritem, "useritem", 6)
      
        local cellbg = image_bg:getChildByName("infoNode_" .. i)

        if i-1==self._gameLayer.myChirID then
            cellbg:getChildByName("Image_bg"):loadTexture("game/selfinfobg.png",ccui.TextureResType.localType)
        end

        local headbg=cellbg:getChildByName("head")
        local sz=headbg:getContentSize()
        --创建裁剪
        local strClip = "game/head_mask.png"
        clipSp = cc.Sprite:create(strClip)
        local clip = cc.ClippingNode:create()
            --裁剪
        clip:setStencil(clipSp)
        clip:setAlphaThreshold(0.01)
        clip:addChild(head)
        clip:addTo(headbg)
        clip:setPosition(sz.width/2,sz.height/2)
        if nil ~= cellbg then
            cellbg:setVisible(true)
            local cellsize = cellbg:getContentSize()            

            if nil ~= useritem then
                cellbg:getChildByName("broomer"):setVisible(useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID)
                -- 昵称
                local textNickname = cellbg:getChildByName("Text_name")
                local strNickname = string.EllipsisByConfig(useritem.szNickName, 110, 
                                                            string.getConfig("fonts/round_body.ttf", 21))
                textNickname:setString(strNickname)
                --玩家ID
                local textUserId = cellbg:getChildByName("Text_ID")
                textUserId:setString(useritem.dwGameID)
                --刮风次数
                cellbg:getChildByName("Text_GuaFeng"):setString(record.guafeng[i])
                --下雨次数
                cellbg:getChildByName("Text_XiaYu"):setString(record.xiayu[i])
                --放炮次数
                cellbg:getChildByName("Text_FangPao"):setString(record.fangpao[i])
                --自摸个数
                cellbg:getChildByName("Text_ZiMo"):setString(record.zimo[i])

                local scorestr=record.allscore[i]
                if record.allscore[i]>=0 then
                    scorestr="+"..record.allscore[i]
                    cellbg:getChildByName("Text_Score"):setTextColor(cc.c4b(0xc0,0x4d,0x45,0xff))
                else
                    cellbg:getChildByName("Text_Score"):setTextColor(cc.c4b(0x58,0x6e,0xbe,0xff))
                end
                --总成绩
                cellbg:getChildByName("Text_Score"):setString(scorestr)

                local bWinner=true
                -- local equalN=0
                -- for j=1,chairCount do
                --     if record.allscore[i]<record.allscore[j] then
                --         cellbg:getChildByName("winnertag"):setVisible(false)
                --         bWinner=false
                --         break
                --     elseif record.allscore[i]==record.allscore[j] then
                --         equalN=equalN+1
                --     end
                -- end
                -- if equalN==chairCount then
                --     bWinner=false
                --     cellbg:getChildByName("winnertag"):setVisible(false)
                -- end

                -- 大赢家标志
                if bigWinner == i - 1 then
                    cellbg:getChildByName("winnertag"):setVisible(true)
                else
                    cellbg:getChildByName("winnertag"):setVisible(false)
                    bWinner=false
                end
                
                if bWinner then
                    textNickname:setPositionX(textNickname:getPositionX()+10)
                end


            else
                cellbg:setVisible(false)
            end
        end        
    end

    -- 分享按钮
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = image_bg:getChildByName("btn_quit")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(1.5),
        cc.CallFunc:create(function() 
            local resultLayer=self._gameLayer._gameView._resultLayer
            if resultLayer~=nil and not tolua.isnull(resultLayer) then
                resultLayer:removeSelf()
            end
        end),
        cc.DelayTime:create(0.1),
        cc.CallFunc:create(function()
            csbNode:setVisible(true)
        end)))
end

function PriGameLayer:onLeaveGame()
    --"房卡切换后台"`
    self._dismissLayer:onLeaveGame()
end

function PriGameLayer:onExit()
    self._dismissLayer:onExit()
end

return PriGameLayer