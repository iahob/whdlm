local GameEndView =  class("GameEndView",function(config)
         local gameEndView =  display.newLayer()
    return gameEndView
end)

local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local cmd = appdf.req(appdf.GAME_SRC.."qipai.sangong.src.models.CMD_Game")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

GameEndView.BT_CLOSE = 1
GameEndView.BT_GAMEHALL = 2
GameEndView.BT_GAMESHARE = 3
GameEndView.BT_GAME_CONTINUE = 4
GameEndView.BT_CHANGE_TABLE = 5


function GameEndView:ctor(config)
    
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self)
    local bg = csbNode:getChildByName("Image_1")
    self.m_spBg = bg

	--按钮回调
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
			self:OnButtonClickedEvent(ref:getTag(),ref)
        end
    end

    self.m_config = config

    self._endViewBg = display.newSprite("#"..cmd.GAME.."game_end_bg.png")
        :move(667,375)
        :addTo(self)

     ccui.Button:create(cmd.GAME.."bt_end_close_0.png",cmd.GAME.."bt_end_close_1.png",cmd.GAME.."bt_end_close_0.png",ccui.TextureResType.plistType)
         :setTag(GameEndView.BT_CLOSE)
         :move(1244,705)
         :addTo(self)
         :addTouchEventListener(btcallback)

    ccui.Button:create(cmd.GAME.."bt_gamehall_0.png",cmd.GAME.."bt_gamehall_1.png",cmd.GAME.."bt_gamehall_0.png",ccui.TextureResType.plistType)
         :setTag(GameEndView.BT_GAMEHALL)
         :move(420,85)
         :addTo(self)
         :addTouchEventListener(btcallback)

    ccui.Button:create(cmd.GAME.."bt_gameshare_0.png",cmd.GAME.."bt_gameshare_0.png",cmd.GAME.."bt_gameshare_1.png",ccui.TextureResType.plistType)
         :setTag(GameEndView.BT_GAMESHARE)
         :move(918,85)
         :addTo(self)
         :addTouchEventListener(btcallback)

    self.roomInfo = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
            :move(667,625)
            :setAnchorPoint(cc.p(0.5,0.5))
            :setColor(cc.c3b(255,238,217))
            :addTo(self)

    local startx = 217
    local starty = 400 - 305

    self.m_UserBg = {}
    self.m_UserHead = {}
    self.m_UserName = {}
    self.m_UserID = {}
    self.m_UserResult = {}
    self.m_UserScore = {} 
--    self.m_UserCard = {}
    self.m_UserOwner = {}
--    self.m_CardType = {}

    local ptBg = {cc.p(295,478),cc.p(667,478),cc.p(1033,478),cc.p(295,268),cc.p(667,268),cc.p(1033,268)}
    local ptCard = {cc.p(294,331),cc.p(465,331),cc.p(636,331),cc.p(809,331),cc.p(980,331),cc.p(980,331)}
    local ptType = {cc.p(324,331),cc.p(495,331),cc.p(666,331),cc.p(839,331),cc.p(1010,331),cc.p(1010,331)}
    for i = 1 , cmd.GAME_PLAYER do
        self.m_UserBg[i] = display.newSprite("#"..cmd.GAME.."game_end_user.png")
            :move(ptBg[i])
            :setVisible(false)
            :addTo(self)

        self.m_UserHead[i] = HeadSprite:createNormal({}, 80)
            :move(70,130)
            :addTo(self.m_UserBg[i])

        self.m_UserName[i] = cc.Label:createWithTTF("游戏玩家", "fonts/round_body.ttf", 26)
            :move(130,128)
            :setAnchorPoint(cc.p(0,0))
            :setColor(cc.c3b(140,57,57))
            :addTo(self.m_UserBg[i])

        self.m_UserID[i] = cc.Label:createWithTTF("ID:".."9999999", "fonts/round_body.ttf", 26)
            :move(130,93)
            :setAnchorPoint(cc.p(0,0))
            :setColor(cc.c3b(140,57,57))
            :addTo(self.m_UserBg[i])

        self.m_UserScore[i] = {}
        self.m_UserScore[i].frameWin = display.newSprite("#"..cmd.GAME.."end_win_num_bg.png")
				:setAnchorPoint(cc.p(0,0))
				:move(146,18 + 3)
				:setVisible(false)
				:addTo(self.m_UserBg[i])
        --赢数额
		self.m_UserScore[i].scoreWin = cc.LabelAtlas:create("666",cmd.RES.."end_win_num.png",27,37,string.byte("0"))
				:move(146 + 25,18)
				:setAnchorPoint(cc.p(0,0))
				:setVisible(false)
				:addTo(self.m_UserBg[i])
        self.m_UserScore[i].frameLost = display.newSprite("#"..cmd.GAME.."end_lost_num_bg.png")
				:setAnchorPoint(cc.p(0,0))
				:move(146 + 5,18 + 10)
				:setVisible(false)
				:addTo(self.m_UserBg[i])
        --输数额
		self.m_UserScore[i].scoreLost = cc.LabelAtlas:create("666",cmd.RES.."end_lost_num.png",27,37,string.byte("0"))
				:move(146 + 25,18)
				:setAnchorPoint(cc.p(0,0))
				:setVisible(false)
				:addTo(self.m_UserBg[i])

        self.m_UserResult[i] = display.newSprite("#"..cmd.GAME.."game_end_flagwin.png")
            :move(294,55)
            :setVisible(false)
            :addTo(self.m_UserBg[i])

        self.m_UserOwner[i] = display.newSprite("#"..cmd.GAME.."game_end_flagowner.png")
            :move(50,158)
            :setVisible(false)
            :addTo(self.m_UserBg[i])
    end
end

function GameEndView:setRoomInfo(roomInfo,winnerGod)
    self.winnerGod = winnerGod
    self.roomInfo:setString(roomInfo)
end

function GameEndView:OnButtonClickedEvent(tag,ref)
    if tag == GameEndView.BT_GAME_CONTINUE then
        if self:getParent()._scene.m_bNoScore then
            local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
            local msg = self:getParent()._scene.m_szScoreMsg or "你的游戏币不足，无法继续游戏"
            local query = QueryDialog:create(msg, function(ok)
                if ok == true then
                    self:getParent()._scene:onExitTable()
                end
            query = nil
            end, 32, QueryDialog.QUERY_SURE):setCanTouchOutside(false)
            :addTo(self:getParent()._scene)
        else
            self:getParent()._scene:onStartGame(true)
        end
    elseif tag == GameEndView.BT_CHANGE_TABLE then
        self:getParent()._scene:onChangeDesk()
    elseif tag == GameEndView.BT_GAMESHARE then
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
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
    elseif tag == GameEndView.BT_CLOSE or tag == GameEndView.BT_GAMEHALL then
        self:hideLayer()
        GlobalUserItem.bWaitQuit = false
        self:getParent()._scene:onExitTable()
    end
end

function GameEndView:ReSetData()
    self.userList = {}
    for i = 1 , cmd.GAME_PLAYER do
        self.m_UserBg[i]:setVisible(false)
        self.m_UserHead[i]:setVisible(false)
        self.m_UserName[i]:setVisible(false)
        self.m_UserID[i]:setVisible(false)
        self.m_UserScore[i].frameWin:setVisible(false)
        self.m_UserScore[i].scoreWin:setVisible(false)
        self.m_UserScore[i].frameLost:setVisible(false)
        self.m_UserScore[i].scoreLost:setVisible(false)
        self.m_UserResult[i]:setVisible(false)
        self.m_UserOwner[i]:setVisible(false)
--        for j = 1 , 3 do
--            self.m_UserCard[i][j]:setVisible(false)
--        end
--        self.m_CardType[i]:setVisible(false)
    end
end

function GameEndView:SetUserScore(viewId, score, userItem)
    table.insert(self.userList,viewId)
    local i = #self.userList
    self.m_UserBg[i]:setVisible(true)
    if score < 0 then
        self.m_UserScore[i].frameLost:setVisible(true)
        self.m_UserScore[i].scoreLost:setVisible(true)
        self.m_UserScore[i].scoreLost:setString(math.abs(score))
    else
        self.m_UserScore[i].frameWin:setVisible(true)
        self.m_UserScore[i].scoreWin:setVisible(true)
        self.m_UserScore[i].scoreWin:setString(score)
        if score == self.winnerGod then
            self.m_UserResult[i]:setVisible(true)
        end
    end
    self:SetUserInfo(i,userItem)
end

function GameEndView:SetUserInfo(viewid,useritem)
    self.m_UserHead[viewid]:setVisible(true)
    self.m_UserHead[viewid]:updateHead(useritem)
    self.m_UserName[viewid]:setVisible(true)
    self.m_UserID[viewid]:setVisible(true)
    if useritem and useritem.szNickName then
        self.m_UserName[viewid]:setString(string.EllipsisByConfig(useritem.szNickName,180, self.m_config))
    else
        self.m_UserName[viewid]:setString("游戏玩家")
    end

    if useritem and useritem.dwGameID then
        self.m_UserID[viewid]:setString(string.EllipsisByConfig("ID:"..useritem.dwGameID,180, self.m_config))
    else
        self.m_UserID[viewid]:setString("ID:".."88888888")
    end
    if useritem and useritem.dwUserID == PriRoom:getInstance().m_tabPriData.dwTableOwnerUserID then
		self.m_UserOwner[viewid]:setVisible(true)
	end
end

function GameEndView:SetUserCard(viewid, cardData, cardtype, isbreak)

end

function GameEndView:GetMyBoundingBox()
    return self._endViewBg:getBoundingBox()
end

function GameEndView:showLayer()
    self:runAction(cc.Sequence:create(cc.DelayTime:create(3.0),
    cc.CallFunc:create(function()
        self.colorLayer:setTouchEnabled(true)
        self:setVisible(true)
    end)))
end

function GameEndView:hideLayer()
	self.colorLayer:setTouchEnabled(false)
	self:setVisible(false)
end

function GameEndView:onTouch(eventType, x, y)
	if eventType == "began" then
		return true
	end

	local pos = cc.p(x, y)
    local rectLayerBg = self._endViewBg:getBoundingBox()
    if not cc.rectContainsPoint(rectLayerBg, pos) then
    end

    return true
end

return GameEndView