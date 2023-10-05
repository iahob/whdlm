

--                            _ooOoo_  
--                           o8888888o  
--                           88" . "88  
--                           (| -_- |)  
--                            O\ = /O  
--                        ____/`---'\____  
--                      .   ' \\| |// `.  
--                      / \\||| : |||// \  
--                     / _||||| -:- |||||- \  
--                       | | \\\ - /// | |  
--                     | \_| ''\---/'' | |  
--                      \ .-\__ `-` ___/-. /  
--                   ___`. .' /--.--\ `. . __  
--                ."" '< `.___\_<|>_/___.' >'"".  
--               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
--                 \ \ `-. \_ __\ /__ _/ .-` / /  
--         ======`-.____`-.___\_____/___.-`____.-'======  
--                            `=---='  
--  
--         .............................................  
--                  --佛祖保佑             永无BUG 
local GameModel = appdf.req(appdf.CLIENT_SRC.."gamemodel.GameModel")

local GameLayer = class("GameLayer", GameModel)

local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")
local GameLogic = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.GameLogic")
local GameViewLayer = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.views.layer.GameViewLayer")
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function GameLayer:onExit()

    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local progress = self._gameView.m_TimeProgress[viewid]
        if progress then
            progress:stopAllActions()
        end
    end

    self:KillGameClock()
    self:dismissPopWait()
    
    GameLayer.super.onExit(self)
end

--退出桌子
function GameLayer:onExitTable()
    print("onExitTable()")
    if self.m_querydialog then
        return
    end
    self:KillGameClock()
    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        self:showPopWait()
        self:runAction(cc.Sequence:create(
            cc.CallFunc:create(
                function () 
                    self._gameFrame:StandUp(1)
                    print("11111111111111")
                end
                ),
            cc.DelayTime:create(10),
            cc.CallFunc:create(
                function ()
                    print("delay leave")
                    print("222222222222222222")
                    self:onExitRoom()
                end
                )
            )
        )
        return
    end

   self:onExitRoom()
end

--离开房间
function GameLayer:onExitRoom()
    self._scene:onKeyBack()
end
--系统消息
function GameLayer:onSystemMessage( wType,szString )
    if wType == 501 or wType == 515 or wType == 56 then
        self:KillGameClock()
        print("扎金花游戏币不足消息", szString)
        GlobalUserItem.bWaitQuit = true
        self:onSubNoticeAddScore(szString)
    end
end


function GameLayer:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = 5
    --print("椅子数目", nChairCount)
    local nChairID = self:GetMeChairID()
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        viewid = math.mod(chair + math.floor(nChairCount * 3/2) - nChairID, nChairCount) + 1
        --print("装换完的ID", viewid)
    end
    return viewid
end

function GameLayer:onGetSitUserNum()
    local userNum = 0
    for i=1,table.nums(self._gameView.nodePlayer) do
        if self._gameView.nodePlayer[i]:isVisible() then
            userNum = userNum + 1
        end
    end
    return userNum
    --return table.nums(self._gameView.nodePlayer)
end

function GameLayer:getUserInfoByChairID(chairId)
    return self.m_GameUserItem[chairId]--self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), chairId)
end

--获取gamekind
function GameLayer:getGameKind()
    return cmd.KIND_ID
end

-- 初始化界面
function GameLayer:ctor(frameEngine,scene)
    GameLayer.super.ctor(self,frameEngine,scene)
end

-- 创建场景
function GameLayer:CreateView()
    cc.FileUtils:getInstance():addSearchPath(device.writablePath..cmd.RES,true)
    return GameViewLayer:create(self)
        :addTo(self)
end

function GameLayer:getParentNode( )
    return self._scene
end
--设置私有房的层级
function GameLayer:addPrivateGameLayer( layer )
    if nil == layer then
        return
    end
        self._gameView:addChild(layer, 9)
end

------继承函数
function GameLayer:getVideoLayerConfig()
    return 
    {
        pos = cc.p(667, 50),
        wChairCount = self._gameFrame._wChairCount,
        maskOpacity = 50
    }
end

-- 初始化游戏数据
function GameLayer:OnInitGameEngine()

    GameLayer.super.OnInitGameEngine(self)

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户

    self.m_cbPlayStatus = {0,0,0,0,0}                     --游戏状态
    self.m_lTableScore = {0,0,0,0,0}                      --下注数目

    self.m_lMaxCellScore = 0                            --单元上限
    self.m_lCellScore = 0                               --单元下注

    self.m_lCurrentTimes = 0                            --当前倍数
    self.m_lUserMaxScore = 0                            --最大分数

    self.m_bLookCard  = {false,false,false,false,false}        --看牌动作

    self.m_giveUpUser = {false , false , false , false , false}

    self.m_myCardData = {}

    self.m_wLostUser = yl.INVALID_CHAIR                 --比牌失败
    self.m_wWinnerUser = yl.INVALID_CHAIR               --胜利用户

    self.m_lAllTableScore = 0

    self.m_bNoScore = false   --游戏币不足
    self.m_bStartGame = false

    self.m_isDelayGameEnd = false


    self.m_isMeEnd = false

    --for i=1 , 5 do
--self._gameView:ShowUserVoice(5, true)
--end

    self.isPriOver = false
    -- 提示
    self.m_szScoreMsg = ""
    
  
    self.nVoiceFollow = {0, 0, 0, 0, 0}

    self.bIsDismiss = false
    self.m_GameUserItem = {}
    self.playerCount = 0

    self.lInitScore = 0;

    self.m_bLookOn = false

    self.m_playingUser = {}

    
end

-- 重置游戏数据
function GameLayer:OnResetGameEngine()
    GameLayer.super.OnResetGameEngine(self)

    self._gameView:OnResetView()

    self.m_myCardData = {}

    self.m_isMeEnd = false
    self.bIsDismiss = false

    self.m_wCurrentUser = yl.INVALID_CHAIR              --当前用户
    self.m_wBankerUser = yl.INVALID_CHAIR               --庄家用户
    self.m_cbPlayStatus = {0,0,0,0,0}                     --游戏状态
    self.m_lTableScore = {0,0,0,0,0}                      --下注数目
    self.m_lMaxCellScore = 0                            --单元上限
    self.m_lCellScore = 0                               --单元下注
    self.m_lCurrentTimes = 0                            --当前倍数
    self.m_lUserMaxScore = 0                            --最大分数
    self.m_bLookCard  = {false,false,false,false,false}        --看牌动作
    self.m_wLostUser = yl.INVALID_CHAIR                 --比牌失败
    self.m_wWinnerUser = yl.INVALID_CHAIR               --胜利用户 
    self.m_lAllTableScore = 0
    self.cbTurnCount=0
    self.bShowHand=false
    self.m_isDelayGameEnd = false

    self.m_giveUpUser = {false , false , false , false , false}

   
end

function GameLayer:onPlayingUserInfo(userInfo)
    self.m_playingUser = {}
    for i=1,16 do
        self.m_playingUser[i] = userInfo.dwUserID[1][i]
    end
end

function GameLayer:onQueryExitGame()
    local bNoGame = false --没有参与游戏

    for i=1,#self.m_playingUser do
        if self.m_playingUser[i] == GlobalUserItem.tabAccountInfo.dwUserID  then
            bNoGame = false
            break
        elseif i == #self.m_playingUser then
            bNoGame = true
        end
    end

    if self.m_bLookOn  or bNoGame then
        self:onExitTable()
        return
    end

    if PriRoom and true == GlobalUserItem.bPrivateRoom and (false == GlobalUserItem.bPrivateGoldRoom or GlobalUserItem.dwCurRoomGroupID ~= 0) then  --金币房卡可以直接退出
        PriRoom:getInstance():queryQuitGame(self.m_cbGameStatus)
    else

        if self._queryDialog then
           return
        end
        local tip = self.m_cbGameStatus > 0 and "现在退出游戏会由系统托管代打哟，输了可不能怪他哦！" or "您要退出游戏么？"
       
        self._queryDialog = QueryDialog:create(tip, function(ok)
            if ok == true then
                --退出防作弊
                self._gameFrame:setEnterAntiCheatRoom(false)
                if  self.m_cbGameStatus > 0 then

                    self._gameFrame:onCloseSocket()
                    self:dismissPopWait()
                    self:stopAllActions()
                    self:KillGameClock()
                    self._scene:onKeyBack()
                    return
                end
                
                self:onExitTable()
            end
            self._queryDialog = nil
        end):setCanTouchOutside(false)
            :addTo(self)
    end
end

-- 设置计时器
function GameLayer:SetGameClock(chair,id,time)
    print("SetGameClock",chair,id,time)
    GameLayer.super.SetGameClock(self,chair,id,time)
    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local clock = self._gameView.m_clock[viewid]
        local progress = self._gameView.m_TimeProgress[viewid]
        if progress  and clock then
            clock:setVisible(true)
            clock:getChildByName("Clock_num"):setString(time)
            progress:setPercentage(100)
            
            progress:runAction(cc.Sequence:create(cc.ProgressTo:create(time, 0), cc.CallFunc:create(function()
                
                self:OnEventGameClockInfo(viewid, id)
            end)))
        end
    end
end

-- 关闭计时器
function GameLayer:KillGameClock(notView)
    local viewid = self:GetClockViewID()
    if viewid and viewid ~= yl.INVALID_CHAIR then
        local clock = self._gameView.m_clock[viewid]
        local progress = self._gameView.m_TimeProgress[viewid]
        if clock and progress then
            clock:setVisible(false)
            progress:stopAllActions()
        end
    end
    
    GameLayer.super.KillGameClock(self,notView)
end

-- 时钟处理
function GameLayer:OnEventGameClockInfo(chair,time,clockid)
     --房卡不托管
     
    local viewid = self:GetClockViewID()



    print("定时器每秒调用",chair,viewid,time,clockid)
    self._gameView:onUpdataClockTime(viewid , time)

    --勋章房
    if time <= 1 then
        -- print(GlobalUserItem.dwCurRoomGroupID, self.m_cbGameStatus, PriRoom:getInstance().m_tabPriData.dwPlayCount,"------------超时等待-----------------")
        if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 and self.m_cbGameStatus == cmd.GAME_STATUS_FREE then
            if PriRoom:getInstance().m_tabPriData.dwPlayCount == 0 then
                self:onExitTable()
            --else
                --准备
                --self._gameView:OnButtonClickedEvent(GameViewLayer.BT_READY,nil);
            end
        end
    end



    if GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        if time <= 0 then
            return true
        end
    end 
    if time < 5 then
        self:PlaySound(cmd.RES.."sound_res/GAME_WARN.wav")
    end
    if clockid == cmd.IDI_START_GAME then
        if time <= 0 then
            self._gameFrame:setEnterAntiCheatRoom(false)--退出防作弊
            self:onExitTable()
            print("退出防作弊")
            return true
        end
    elseif clockid == cmd.IDI_DISABLE then
        if time == 0 then
            return true
        end
    elseif clockid == cmd.IDI_USER_ADD_SCORE then
        if time == 0 then
            if self.m_wCurrentUser == self:GetMeChairID() then
                    --self:onGiveUp()
                return true
            end
        end
    elseif clockid == cmd.IDI_USER_COMPARE_CARD then
        if time == 0 then
            self._gameView:SetCompareCard(false)
            --self:onGiveUp()
            return true
        end
    end
end

--获得当前正在玩的玩家数量
function GameLayer:getPlayingNum()
    local num = 0
    for i = 1, cmd.GAME_PLAYER do
        if self.m_cbPlayStatus[i] == 1 then
            num = num + 1
        end
    end
    return num
end


--用户进入
function GameLayer:onEventUserEnter(tableid,chairid,useritem)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    if MyTable == tableid then
        local viewid = self:SwitchViewChairID(chairid)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)

            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end

    if self:onGetSitUserNum()==5 then
        self._gameView.btSit:setVisible(false);
    end
end

--用户状态
function GameLayer:onEventUserStatus(useritem,newstatus,oldstatus)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    local MyChair = self:GetMeChairID()

    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    --旧的清除
    if oldstatus and oldstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(oldstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, nil, useritem.cbUserStatus == yl.US_FREE)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, true)
            end
        end
    end

    --更新新状态
    if newstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(newstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end

    if self:onGetSitUserNum()==5 then
        self._gameView.btSit:setVisible(false);
    end
end



-- 场景信息
function GameLayer:onEventGameScene(cbGameStatus,dataBuffer)

    local myUserItem = self:GetMeUserItem()
    self.m_bLookOn = false
    if myUserItem.cbUserStatus == yl.US_LOOKON then
        self.m_bLookOn = true
    end
    print("self.m_bLookOn",self.m_bLookOn)
    
    --初始化已有玩家
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        local wViewChairId = self:SwitchViewChairID(i-1)
        self._gameView:OnUpdateUser(wViewChairId, userItem)
        self.m_GameUserItem[i -1] = userItem
        
        print("onEventGameScene viewid is==============> ", wViewChairId)

        if userItem and (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and wViewChairId==cmd.MY_VIEWID then
            self.lInitScore= userItem.lScore
        end
    end

    self.m_cbGameStatus = cbGameStatus
	

    self._gameView:updataUserInfo()
    -- 刷新房卡
    print("场景消息PriRoom GlobalUserItem.bPrivateRoom", PriRoom, GlobalUserItem.bPrivateRoom)
    if PriRoom and GlobalUserItem.bPrivateRoom then
        print("场景消息 self._gameView._priView self._gameView._priView.onRefreshInfo", self._gameView._priView, self._gameView._priView.onRefreshInfo)
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            print("场景消息 刷新房卡信息")
            self._gameView._priView:onRefreshInfo()
            if not GlobalUserItem.bPrivateGoldRoom then
                self._gameView:OnUpdateUserScore()
            end
        end
    else
        
        print("金币场 设置初始分",self.lInitScore)
    end

    if cbGameStatus == cmd.GAME_STATUS_FREE then                --空闲状态
        self:onFreeScene(dataBuffer)
    elseif cbGameStatus == cmd.GAME_STATUS_PLAY then            --游戏状态
        self:onGameScene(dataBuffer)
    end
    
    if GlobalUserItem.dwCurRoomGroupID ~= 0 and GlobalUserItem.bPrivateGoldRoom then 
        self.m_bClubRoom = true
    end

    self._gameView:showSitBtn(self.m_bLookOn)
    
    self:dismissPopWait()
end

--显示开始游戏按钮
function GameLayer:onEventShowBegin(dwWaitTime)
    print("显示开始按钮",dwWaitTime)
    self._gameView:showSitBtn(dwWaitTime~=0 and true or false,dwWaitTime)
end

--空闲消息
function GameLayer:onFreeScene(dataBuffer)

    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusFree, dataBuffer)
    dump(cmd_data, "CMD_S_StatusFree", 10)

    GlobalUserItem.bPrivateGoldRoom = cmd_data.cbPlayMode == 1 or false
    self.chipArray = cmd_data.nButtonValue[1]
    self._gameView:setChipNum(self.chipArray)
    self.maxTurnCount = cmd_data.cbMaxTurnCount
    self.playerCount = cmd_data.wGamePlayerCountRule
    self.m_lCellScore = cmd_data.lCellScore
    self.cbMinLookCardTurnCount=cmd_data.cbMinLookCardTurnCount
    self.cbMinCompareCardTurnCount=cmd_data.cbMinCompareCardTurnCount
    if cmd_data.lInitScore>0 then
        self.lInitScore=cmd_data.lInitScore
    end

    self._gameView:SetCellText(cmd_data.lCellScore)

   
    if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        self.m_lCellScore = 1
        self._gameView:SetCellScore(1)
    else
        self._gameView:SetCellScore(self.m_lCellScore)
    end
    

    if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom  or GlobalUserItem.dwCurRoomGroupID ~= 0 then
        -- 设置倒计时
        if not self.m_bLookOn then
            self:SetGameClock(self:GetMeChairID(),cmd.IDI_START_GAME,cmd.TIME_START_GAME)
        end
    end  
    local seq = cc.Sequence:create(
        cc.DelayTime:create(1.0),
        cc.CallFunc:create(
            function ()
               self:exitResultShowBtn()
            end
            )
        )
    seq:setTag(99)
    self:runAction(seq)

    if PriRoom and GlobalUserItem.bPrivateRoom then
        local ruleTable = {}
        ruleTable.cellScore = cmd_data.lCellScore
        ruleTable.maxtrun = cmd_data.cbMaxTurnCount
        ruleTable.minmeng = cmd_data.cbMinLookCardTurnCount
        ruleTable.compare = cmd_data.cbMinCompareCardTurnCount
        ruleTable.playnum = cmd_data.wGamePlayerCountRule
        ruleTable.score   = cmd_data.lInitScore
        self._gameView:updateRoomInfo(ruleTable)
    end
end

--游戏消息
function GameLayer:onGameScene(dataBuffer)
    local MyChair = self:GetMeChairID() + 1
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_StatusPlay, dataBuffer)
    dump(cmd_data, "GAME_STATUS_PLAY", 10)

    GlobalUserItem.bPrivateGoldRoom = cmd_data.cbPlayMode == 1 or false
    self.chipArray=cmd_data.nButtonValue[1]
    self._gameView:setChipNum(self.chipArray)
    if cmd_data.lInitScore>0 then
        self.lInitScore=cmd_data.lInitScore
    end
    self.cbMinLookCardTurnCount=cmd_data.cbMinLookCardTurnCount
    self.cbMinCompareCardTurnCount=cmd_data.cbMinCompareCardTurnCount
    self.cbCurTurnCount=cmd_data.cbCurTurnCount
    self.cbTurnCount=cmd_data.cbCurTurnCount+1
    self.maxTurnCount = cmd_data.cbMaxTurnCount
    self.m_lMaxCellScore = cmd_data.lMaxCellScore
    self.m_lCellScore = cmd_data.lCellScore
    self.m_lCurrentTimes = cmd_data.lCurrentTimes
    self.m_lUserMaxScore = cmd_data.lUserMaxScore
    self.m_wBankerUser = cmd_data.wBankerUser
    self.m_wCurrentUser = cmd_data.wCurrentUser
    self.playerCount = cmd_data.wGamePlayerCountRule
    self.bShowHand=cmd_data.bShowHand

    self._gameView:SetCellText(cmd_data.lCellScore)
    

    self._gameView:OnUpdataHuiHe(self.cbTurnCount,self.maxTurnCount)

    for i = 1, cmd.GAME_PLAYER do
        self.m_cbPlayStatus[i] = cmd_data.cbPlayStatus[1][i]
    end

    self.nowGameUser = {}
    for i = 1, cmd.GAME_PLAYER  do
        if not self.nowGameUser[i] then
            self.nowGameUser[i] = self.m_cbPlayStatus[i]
        end
    end

    if self.m_cbPlayStatus[MyChair]==1 then
        self.m_cbGameStatus = cmd.GAME_STATUS_PLAY
    else
        self.m_cbGameStatus = cmd.GAME_STATUS_FREE
    end

    for i = 1, cmd.GAME_PLAYER do
        self.m_bLookCard[i] = cmd_data.bMingZhu[1][i]
    end

    for i = 1, cmd.GAME_PLAYER do
        self.m_lTableScore[i]  = cmd_data.lTableScore[1][i]
    end

    local cardData = {}
    for i = 1, 3 do
        cardData[i] = cmd_data.cbHandCardData[1][i]
    end

    if PriRoom and GlobalUserItem.bPrivateRoom and  not GlobalUserItem.bPrivateGoldRoom then
        self.m_lCellScore = 1
        self._gameView:SetCellScore(1)
    else
        self._gameView:SetCellScore(self.m_lCellScore)
    end

    self._gameView:SetBanker(self:SwitchViewChairID(self.m_wBankerUser))

    for i = 1, cmd.GAME_PLAYER do
        --视图位置
        local viewid = self:SwitchViewChairID(i-1)
        --手牌显示
        if self.m_cbPlayStatus[i] == 1 then
            self._gameView.userCard[viewid].area:setVisible(true) 
            if i == MyChair  and self.m_bLookCard[MyChair] == true and not self.m_bLookOn then
                local savetype = GameLogic:getCardType(cardData)
                self._gameView:SetMyCardType( savetype)

                local cardIndex = {}
                for k = 1 , 3 do
                    cardIndex[k] = GameLogic:getCardColor(cardData[k])*13 + GameLogic:getCardValue(cardData[k])
                end
                self._gameView:SetUserCard(viewid,cardIndex)
            else
                self._gameView:SetUserCard(viewid,{0xff,0xff,0xff})
            end
        else
            if i == MyChair then
                self.m_isMeEnd = true
            end
            self._gameView.userCard[viewid].area:setVisible(false)
            self._gameView:SetUserCard(viewid)
        end
        --看牌显示
        if self.m_bLookCard[i] then
            self._gameView:SetUserOperate(viewid,1)
        end
        self._gameView:SetUserTableScore(viewid, self.m_lTableScore[i])
        self.m_lAllTableScore = self.m_lAllTableScore + self.m_lTableScore[i]

        self._gameView:PlayerJetton(viewid,self.m_lTableScore[i],true)

        --是否弃牌
        if self.m_cbPlayStatus[i] ~= 1 and self.m_lTableScore[i] > 0 then
            self._gameView.userCard[viewid].area:setVisible(true)
            self._gameView:SetUserOperate(viewid, 2)
            self._gameView:SetUserCard(viewid,{0,0,0})
            end
    end

    --总下注
    self._gameView:SetAllTableScore(self.m_lAllTableScore)

    local bCompareStatus = cmd_data.bCompareState
    --控件信息
    self._gameView.nodeButtomButton:setVisible(false)
    if not bCompareStatus then
        --控件信息
        if self:GetMeChairID() == self.m_wCurrentUser then
            self:UpdataControl()
        else
            self:UpdataBeforehandControl()
        end
            --设置时间
            if cmd_data.cbRemainingTime>0 then
                self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_ADD_SCORE, cmd_data.cbRemainingTime)
            end
    else

        if self:GetMeChairID() == self.m_wCurrentUser then
            --选择玩家比牌
            local compareStatus={false,false,false,false,false}
            for i = 1 ,cmd.GAME_PLAYER do
                if self.m_cbPlayStatus[i] == 1 and i ~= MyChair then
                    compareStatus[self:SwitchViewChairID(i-1)] = true
                end
            end
            self._gameView:SetCompareCard(true,compareStatus)
            --设置时间
            if cmd_data.cbRemainingTime>0 then
                self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_COMPARE_CARD, cmd_data.cbRemainingTime)
            end
        else
            self._gameView:SetCompareCard(false)
            --设置时间
            if cmd_data.cbRemainingTime>0 then
                self:SetGameClock(self.m_wCurrentUser, cmd.IDI_DISABLE, cmd_data.cbRemainingTime)
            end
        end
    end
        

    if PriRoom and GlobalUserItem.bPrivateRoom then
        local ruleTable = {}
        ruleTable.cellScore = cmd_data.lCellScore
        ruleTable.maxtrun = cmd_data.cbMaxTurnCount
        ruleTable.minmeng = cmd_data.cbMinLookCardTurnCount
        ruleTable.compare = cmd_data.cbMinCompareCardTurnCount
        ruleTable.playnum = cmd_data.wGamePlayerCountRule
        ruleTable.score   = cmd_data.lInitScore
        self._gameView:updateRoomInfo(ruleTable)
    end

    local seq = cc.Sequence:create(
        cc.DelayTime:create(1.0),
        cc.CallFunc:create(
            function ()
                if PriRoom and GlobalUserItem.bPrivateRoom  then
                    self._gameView.btChange:setVisible(false)
                    self._gameView.btReady:setVisible(false)
                    self._gameView.btMenuChange:setEnabled(false)
                else
                    if self.m_cbPlayStatus[MyChair]==0 and not self.m_bLookOn then 
                        self._gameView.btChange:setVisible(true)
                        self._gameView.btChange:setPositionX(0)
                        self._gameView.btMenuChange:setEnabled(true)
                    end
                end
            end
            )
        )
    seq:setTag(99)
    self:runAction(seq)
end

-- 游戏消息
function GameLayer:onEventGameMessage(sub,dataBuffer)

	if sub == cmd.SUB_S_GAME_START then 
        self:stopActionByTag(99)
		self:onSubGameStart(dataBuffer)
	elseif sub == cmd.SUB_S_ADD_SCORE then 
		self:onSubAddScore(dataBuffer)
	elseif sub == cmd.SUB_S_LOOK_CARD then 
		self:onSubLookCard(dataBuffer)
	elseif sub == cmd.SUB_S_COMPARE_CARD then 
		self:onSubCompareCard(dataBuffer)
	elseif sub == cmd.SUB_S_GIVE_UP then 
		self:onSubGiveUp(dataBuffer)
	elseif sub == cmd.SUB_S_PLAYER_EXIT then 
		self:onSubPlayerExit(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_END then 
        self:stopActionByTag(99)
        self.m_cbGameStatus = cmd.GAME_STATUS_FREE
        self:onSubGameEnd(dataBuffer)
    elseif sub == cmd.SUB_S_WAIT_COMPARE then 
        self:onSubWaitCompare(dataBuffer)
    elseif sub == cmd.SUB_S_RC_TREASURE_DEFICIENCY then
       --self:onSubNoticeAddScore("你的游戏币不足，无法继续游戏")
    elseif sub == cmd.SUB_S_CHEAT_CARD then
    elseif sub == cmd.SUB_S_ROOMCARD_RECORD then                     --游戏记录
         self:onSubGameRecord(dataBuffer)
    elseif sub== cmd.SUB_S_TURN_COUNT then
        self:onSubTurnCount(dataBuffer)
    elseif sub == cmd.SUB_S_GAME_STATUS then
        local gameStatus = dataBuffer:readbyte()
        if gameStatus == 1 then 
            self._gameView:onResetView()
            if PriRoom and GlobalUserItem.bPrivateRoom then
                if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
                    PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
                    self._gameView._priView:onRefreshInfo()
                end
            end
            if not self.m_bLookOn then 
                self._gameView:showSitBtn(false)
            end
        end
    else

    	print("unknow gamemessage sub is"..sub)
    end
end

function GameLayer:onSubTurnCount(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_TurnCount, dataBuffer)
    self.cbTurnCount=cmd_data.cbTurnCount + 1
    self._gameView:OnUpdataHuiHe(self.cbTurnCount , self.maxTurnCount)
    print("当前论述（单消息）",cmd_data.cbTurnCount)
end

--游戏开始
function GameLayer:onSubGameStart(dataBuffer)
    self.m_bStartGame = true
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameStart, dataBuffer)
    dump(cmd_data,"游戏开始咯！！")
    self:OnResetGameEngine()
    for i = 1, cmd.GAME_PLAYER do
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        local wViewChairId = self:SwitchViewChairID(i-1)
        
        if userItem and (not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom) and wViewChairId==cmd.MY_VIEWID then
            self.lInitScore= userItem.lScore
        end
    end

    if not self.m_bLookOn then 
        self._gameView:showSitBtn(false)
    end

    self.m_cbGameStatus = cmd.GAME_STATUS_PLAY
    self.m_isMeEnd = false
    self.m_lMaxCellScore = cmd_data.lMaxCellScore
    self.m_lCellScore = cmd_data.lCellScore
    self.m_lCurrentTimes = cmd_data.lCurrentTimes
    self.m_lUserMaxScore = cmd_data.lUserMaxScore
    print("游戏开始底注信息 MaxCellScore   CellScore", self.m_lMaxCellScore, self.m_lCellScore)
    self._gameView:SetCellText(cmd_data.lCellScore)
    self.m_wBankerUser = cmd_data.wBankerUser
    self.m_wCurrentUser = cmd_data.wCurrentUser

    self._gameView:OnUpdataHuiHe(1,self.maxTurnCount)

    self._gameView.btChange:stopAllActions()
    self._gameView.btChange:setVisible(false)
    self._gameView.btMenuChange:setEnabled(false)

    self._gameView:SetBanker(self:SwitchViewChairID(self.m_wBankerUser))

    --重置开始按钮
    self._gameView.btReady:setVisible(false);
    self._gameView.btStart:setVisible(false);
    self._gameView.m_GameEndView = nil

    
    if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        self.m_lCellScore = 1
        self._gameView:SetCellScore(1)
    else
        self._gameView:SetCellScore(self.m_lCellScore)
    end
   

   
    

    self.m_lAllTableScore = 0
    self._gameView:CleanAllJettons()
    for i = 1, cmd.GAME_PLAYER  do
        --跟新玩家
        local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
        local wViewChairId = self:SwitchViewChairID(i-1)
        self._gameView:OnUpdateUser(wViewChairId, userItem)
        self.m_GameUserItem[i -1] = userItem
        if wViewChairId == cmd.MY_VIEWID then
            self.lInitScore = userItem.lScore
            print("我的分",self.lInitScore)
        end
        print("onSubGameStart viewid is==============> ", wViewChairId)
        --读取游戏状态
        local data = cmd_data.cbPlayStatus[1][i]
        self.m_cbPlayStatus[i] = data
        if self.m_cbPlayStatus[i] == 1 then 
            self.m_lAllTableScore = self.m_lAllTableScore + cmd_data.lCellScore
            self.m_lTableScore[i] = cmd_data.lCellScore
            --用户下注
            self._gameView:SetUserTableScore(wViewChairId, cmd_data.lCellScore)
            --移动筹码
            self._gameView:PlayerJetton(wViewChairId,self.m_lTableScore[i])
        end
    end

    if not self.nowGameUser then
        self.nowGameUser = {}
    end
    for i = 1, cmd.GAME_PLAYER  do
        if not self.nowGameUser[i] then
            self.nowGameUser[i] = self.m_cbPlayStatus[i]
        end
    end

    --总计下注
    self._gameView:SetAllTableScore(self.m_lAllTableScore)

    --发牌
    self:PlaySound(cmd.RES.."sound_res/SEND_CARD_BEGIN.wav")
    local delayCount = 1
    for index = 1 , 3 do
        for i = 1, cmd.GAME_PLAYER do
            local chair = math.mod(self.m_wBankerUser + i - 1,cmd.GAME_PLAYER) 
            if self.m_cbPlayStatus[chair + 1] == 1 then
                self._gameView:SendCard(self:SwitchViewChairID(chair),index,delayCount*0.1)
                delayCount = delayCount + 1
            end
        end
    end
   
    self:SetGameClock(self.m_wCurrentUser,cmd.IDI_USER_ADD_SCORE,cmd.TIME_USER_ADD_SCORE)
print("游戏开始self:UpdataControl()",self.m_wCurrentUser,self:GetMeChairID())
    if self.m_wCurrentUser == self:GetMeChairID() and not self.m_bLookOn then 
        self:UpdataControl()
    elseif not self.m_bLookOn and self.m_cbPlayStatus[self:GetMeChairID()+1]==1 then
        self:UpdataBeforehandControl()
    end
    self:PlaySound(cmd.RES.."sound_res/GAME_START.wav")

    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.onRefreshInfo then
            PriRoom:getInstance().m_tabPriData.dwPlayCount = PriRoom:getInstance().m_tabPriData.dwPlayCount + 1
            self._gameView._priView:onRefreshInfo()
            if not GlobalUserItem.bPrivateGoldRoom then
                self._gameView:OnUpdateUserScore()
                self.lInitScore = cmd_data.lUserMaxScore
                print("私人房设置初始分",self.lInitScore)
            end
        end
    end
end

--用户叫分
function GameLayer:onSubAddScore(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_AddScore, dataBuffer)
    dump(cmd_data,"CMD_S_AddScore:",6)
    self.bShowHand=cmd_data.bShowHand

    if self.bShowHand then
        self._gameView.m_befFollowIcon:setVisible(false)
        self._gameView.autoFollow = false
    end

    local MyChair = self:GetMeChairID()

   
    self.m_wCurrentUser = cmd_data.wCurrentUser
    local wAddScoreUser = cmd_data.wAddScoreUser
    local wCompareState = cmd_data.wCompareState
    local lAddScoreCount = cmd_data.lAddScoreCount
    local lCurrentTimes = cmd_data.lCurrentTimes

    local viewid = self:SwitchViewChairID(wAddScoreUser)
    if self.m_lCurrentTimes < lCurrentTimes then
        self._gameView:runAddTimesAnimate(viewid)
    end

    
    self:KillGameClock()
    

    if wAddScoreUser ~= MyChair then
        self._gameView:PlayerJetton(viewid, lAddScoreCount)
        self.m_lTableScore[wAddScoreUser+1] = self.m_lTableScore[wAddScoreUser+1] + lAddScoreCount
        self.m_lAllTableScore = self.m_lAllTableScore + lAddScoreCount
        self._gameView:SetUserTableScore(viewid, self.m_lTableScore[wAddScoreUser+1])
        self._gameView:SetAllTableScore(self.m_lAllTableScore)

        
    end

    if wCompareState == 0 then
        --声音
        if self.m_cbPlayStatus[wAddScoreUser + 1] == 1 then
            if self.m_lCurrentTimes == lCurrentTimes then
                local strFile = cmd.RES.."sound_res/"..string.format("FOLLOW_COIN_%d.wav", math.mod(self.nVoiceFollow[viewid], 3))
                self:PlaySound(strFile)
                self.nVoiceFollow[viewid] = self.nVoiceFollow[viewid] + 1
            else
                self:PlaySound(cmd.RES.."sound_res/RAISE_COIN.wav")
            end
        end
        --设置计时器
        self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_ADD_SCORE, cmd.TIME_USER_ADD_SCORE)
    end
    self.m_lCurrentTimes = lCurrentTimes

    if self.m_wCurrentUser == MyChair and self._gameView.autoFollow and lAddScoreCount==0 then
        self._gameView.autoFollow = not self._gameView.autoFollow
        self._gameView.m_befFollowIcon:setVisible(self.autoFollow)
    end

    --更新操作控件
    if wCompareState == 0 and self.m_wCurrentUser == MyChair and not self.m_bLookOn then
        self.isRunBFUpdata = false
        self:UpdataControl()
    elseif not self.m_bLookOn then
        self.isRunBFUpdata = true

        self:runAction(
        cc.Sequence:create(
        cc.DelayTime:create(0.5),
        cc.CallFunc:create(
            function ( )
                if self.isRunBFUpdata then
                    self:UpdataBeforehandControl() 
                end 
            end
            )
        ))
    end
end

--庄家信息
function GameLayer:onSubLookCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_LookCard, dataBuffer)
    local wLookCardUser = cmd_data.wLookCardUser
   
    local viewid = self:SwitchViewChairID(wLookCardUser)
    if not self.m_giveUpUser[viewid] then
        self._gameView:SetUserOperate(viewid,1)
    end
    if wLookCardUser == self:GetMeChairID() and not self.m_bLookOn then
        local cbCardData = {}
        for i = 1, 3 do
            cbCardData[i] = cmd_data.cbCardData[1][i]
        end

        dump(cbCardData , "1111")
        local savetype = GameLogic:getCardType(cbCardData)
        self._gameView:SetMyCardType( savetype)

        self.m_myCardData = cbCardData

        local cardIndex = {}
        for k = 1 , 3 do
            cardIndex[k] = GameLogic:getCardColor(cbCardData[k])*13 + GameLogic:getCardValue(cbCardData[k])
        end
        
        self._gameView:SetUserCard(viewid, cardIndex)



        if self._gameView.nodeButtomButton:isVisible() then
            self:UpdataControl()
        end

        
    end
end

--出牌信息
function GameLayer:onSubCompareCard(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_CompareCard, dataBuffer)
    dump(cmd_data , "比牌结果")
    self.bShowHand=cmd_data.bShowHand
    self.m_wCurrentUser = cmd_data.wCurrentUser
    local wCompareUser = {}
    for i = 1, 2 do
        wCompareUser[i] = cmd_data.wCompareUser[1][i]
    end
    self.m_wLostUser = cmd_data.wLostUser
    self.m_wWinnerUser = wCompareUser[1] + wCompareUser[2] - self.m_wLostUser

    self.m_cbPlayStatus[self.m_wLostUser+1] = 0

    self._gameView:SetCompareCard(false)

    self:KillGameClock()
    local this = self
    local firstuser =  self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), wCompareUser[1])
    local seconduser =  self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), wCompareUser[2])
    self.m_isDelayGameEnd = true
    self._gameView:CompareCard(firstuser,seconduser,nil,nil,wCompareUser[1] == self.m_wWinnerUser, function()
         this:OnFlushCardFinish()
         self.m_isDelayGameEnd = false
        end)
    -- self._gameView:CompareCard(self:SwitchViewChairID(self.m_wWinnerUser), self:SwitchViewChairID(self.m_wLostUser), function()
    --     this:OnFlushCardFinish()
    -- end)

    self:PlaySound(cmd.RES.."sound_res/COMPARE_CARD.wav")
end

function GameLayer:OnFlushCardFinish()
    
    local nodeCard = self._gameView.userCard[self:SwitchViewChairID(self.m_wLostUser)]
    

    self._gameView:SetUserOperate(self:SwitchViewChairID(self.m_wLostUser) , 3)

    local myChair = self:GetMeChairID()

    if self.m_wCurrentUser == myChair and self._gameView.autoFollow  then
        self._gameView.autoFollow = not self._gameView.autoFollow
        self._gameView.m_befFollowIcon:setVisible(self.autoFollow)
    end

    
    
    local count = self:getPlayingNum()
    if count > 1 then  
        if self.m_wCurrentUser == self:GetMeChairID() then
            self:UpdataControl()
        end
        self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_ADD_SCORE, cmd.TIME_USER_ADD_SCORE)
    else
        if self.m_cbPlayStatus[myChair+1] == 1 or  myChair == self.m_wLostUser then
            local sendBuffer = CCmd_Data:create(0)
            self:SendData(cmd.SUB_C_FINISH_FLASH, sendBuffer)
        end
    end

    if myChair == self.m_wWinnerUser then
        self:PlaySound(cmd.RES.."sound_res/COMPARE_WIN.wav")
    elseif myChair == self.m_wLostUser then
        self:PlaySound(cmd.RES.."sound_res/COMPARE_LOSE.wav")
        self._gameView.nodeBeforehandButton:setVisible(false)
        self.m_isMeEnd = true
    end
end

--游戏结算
function GameLayer:onSubGiveUp(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GiveUp, dataBuffer)

    local wGiveUpUser = cmd_data.wGiveUpUser
    local viewid = self:SwitchViewChairID(wGiveUpUser)
    self._gameView:SetUserOperate(viewid,2)

    self.m_cbPlayStatus[wGiveUpUser+1] = 0

    self.m_giveUpUser[viewid] = true

    if self._gameView.bCompareChoose then
        local compareStatus={false,false,false,false,false}
        for i = 1 ,cmd.GAME_PLAYER do
            if self.m_cbPlayStatus[i] == 1 and i ~= self:GetMeChairID()+1 then
                compareStatus[self:SwitchViewChairID(i-1)] = true
            end
        end
        self._gameView:SetCompareCard(true,compareStatus)
    end

    

    --超时服务器自动放弃
    if wGiveUpUser == self:GetMeChairID() then
        self:KillGameClock()
        self._gameView:SetCompareCard(false, nil)
        self._gameView.m_ChipNode:setVisible(false)
        self._gameView.nodeButtomButton:setVisible(false)
        self._gameView.nodeBeforehandButton:setVisible(false)

        self.m_isMeEnd = true
        self.m_cbGameStatus = cmd.GAME_STATUS_FREE
    end

    self:PlaySound(cmd.RES.."sound_res/GIVE_UP.wav")
end

--设置基数
function GameLayer:onSubPlayerExit(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_PlayerExit, dataBuffer)
    local wPlayerID = cmd_data.wPlayerID
    local wViewChairId = self:SwitchViewChairID(wPlayerID)
    self.m_cbPlayStatus[wPlayerID + 1] = 0
    self._gameView.nodePlayer[wViewChairId]:setVisible(false)
end

--设置基数
function GameLayer:onSubGameEnd(dataBuffer)
    
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_GameEnd, dataBuffer)
    dump(cmd_data , "结束消息")
    self:KillGameClock()
    self.m_bStartGame = false

   

    self._gameView:OnUpdataHuiHe(self.cbTurnCount , self.maxTurnCount)

    local MyChair = self:GetMeChairID() + 1

     --清理界面
    self._gameView:SetCompareCard(false)
    self._gameView.m_ChipNode:setVisible(false)

    self._gameView.nodeButtomButton:setVisible(false)
    self._gameView.nodeBeforehandButton:setVisible(false)
    self.isRunBFUpdata = false
    
    



    local endShow = false

    

    local lGameTax = cmd_data.lGameTax

    local winner 
    local lGameScore = {}
    self.bIsDismiss = true
    for i = 1, cmd.GAME_PLAYER do
        lGameScore[i] = cmd_data.lGameScore[1][i]
        if lGameScore[i] > 0 then
            winner = i
        else
            winner = 0
        end

        if lGameScore[i] ~= 0 then
            self.bIsDismiss = false
        end
    end

    --用户扑克
    local cbCardData = {}
    for i = 1, cmd.GAME_PLAYER do
        cbCardData[i] = {}
        for j = 1, 3 do
            cbCardData[i][j] = cmd_data.cbCardData[i][j]
        end
    end

    --比牌用户
    local wCompareUser = {}
    for i = 1, cmd.GAME_PLAYER  do
        wCompareUser[i] = {}
        for j = 1, 4 do
            wCompareUser[i][j] = cmd_data.wCompareUser[i][j]
        end
    end

    local wEndState = cmd_data.wEndState

    local bDelayOverGame = cmd_data.bDelayOverGame

   

    local savetype = {}

    self._gameView:createGameEndView()

    --移动筹码

    for i = 1, cmd.GAME_PLAYER do
        local viewid = self:SwitchViewChairID(i-1)
        if lGameScore[i] ~= 0 then
            if lGameScore[i] > 0 then
                self._gameView:SetUserTableScore(viewid,"+"..lGameScore[i])
                self._gameView:WinTheChip(viewid)
                if viewid == cmd.MY_VIEWID then
                    print("播放胜利音乐")
                    self:PlaySound(cmd.RES.."sound_res/GAME_WIN.wav")
                else
                    self:PlaySound(cmd.RES.."sound_res/GAME_LOSE.wav")
                end
            else
                self._gameView:SetUserTableScore(viewid,lGameScore[i])
            end

            endShow = true
            self._gameView.m_GameEndView:SetUserScore(viewid, lGameScore[i])
            self._gameView.m_GameEndView:SetUserCard(viewid,nil,nil,lGameScore[i]<0)
            local userItem = self._gameFrame:getTableUserItem(self._gameFrame:GetTableID(), i-1)
            self._gameView.m_GameEndView:SetUserInfo(viewid,userItem)
            savetype[i] = GameLogic:getCardType(cbCardData[i])

            print("savetype["..i.."]"..savetype[i])
        else
            savetype[i] = 0
            self._gameView:SetUserTableScore(viewid)
        end
    end
    
    for i = 1 , cmd.GAME_PLAYER - 1 do
        local wUserID = wCompareUser[MyChair][i]
        if wUserID and wUserID ~= yl.INVALID_CHAIR then
            local viewid = self:SwitchViewChairID(wUserID)
            local cardIndex = {}
            for k = 1 , 3 do
                cardIndex[k] = GameLogic:getCardColor(cbCardData[wUserID+1][k])*13 + GameLogic:getCardValue(cbCardData[wUserID+1][k])
            end
            self._gameView:SetUserCard(viewid, cardIndex)
            self._gameView:SetMyCardType( savetype[wUserID+1])
            self._gameView.m_GameEndView:SetUserCard(viewid,cardIndex,savetype[wUserID+1])
        end
    end
    --比过牌或看过牌，才能显示自己的牌
    if wCompareUser[MyChair][1] ~= yl.INVALID_CHAIR or self.m_bLookCard[MyChair] == true then
        local cardIndex = {}
        for k = 1 , 3 do
             cardIndex[k] = GameLogic:getCardColor(cbCardData[MyChair][k])*13 + GameLogic:getCardValue(cbCardData[MyChair][k])
        end
        self._gameView:SetUserCard(cmd.MY_VIEWID, cardIndex)
        self._gameView:SetMyCardType( savetype[MyChair])
        self._gameView.m_GameEndView:SetUserCard(cmd.MY_VIEWID,cardIndex,savetype[MyChair])
    end

    if #self.m_myCardData == 3 then
        local endType = GameLogic:getCardType(self.m_myCardData)
        local cardIndex = {}
        for k = 1 , 3 do
             cardIndex[k] = GameLogic:getCardColor(self.m_myCardData[k])*13 + GameLogic:getCardValue(self.m_myCardData[k])
        end
        self._gameView.m_GameEndView:SetUserCard(cmd.MY_VIEWID,cardIndex,endType)
    end

    if wEndState == 1 then
        if self.m_cbPlayStatus[MyChair] == 1 then
            for i =1 , cmd.GAME_PLAYER do
                if self.m_cbPlayStatus[i] == 1 then
                    local cardIndex = {}
                    for k = 1 , 3 do
                        cardIndex[k] = GameLogic:getCardColor(cbCardData[i][k])*13 + GameLogic:getCardValue(cbCardData[i][k])
                    end
                    local viewid = self:SwitchViewChairID(i-1)
                    self._gameView:SetUserCard(viewid, cardIndex)
                    self._gameView:SetMyCardType( savetype[i])
                    self._gameView.m_GameEndView:SetUserCard(viewid,cardIndex,savetype[i])
                end
            end
        end
    end

    self._gameView.m_GameEndView:setUserItemPosX()
    print("endShow =========================>", endShow)
    local delay = 0.1
    if self.m_isDelayGameEnd then
        delay = 3.2
    end

        self:runAction(
        cc.Sequence:create(
        cc.DelayTime:create(delay),
        cc.CallFunc:create(
            function ()
                if lGameScore[MyChair]~=0 then
                    if self._gameView.m_GameEndView ~= nil then
                        self._gameView.m_GameEndView:setVisible(true)
                    end
                    
                    self.bIsDismiss = false
                else
                    if self._gameView.m_GameEndView ~= nil then
                        self._gameView.m_GameEndView:removeFromParent()
                    end
                    self:exitResultShowBtn()
                    self.bIsDismiss = true
                end
            end
            )
        ))


    --勋章房
    if PriRoom and GlobalUserItem.dwCurRoomGroupID ~= 0 and self.m_cbGameStatus == cmd.GAME_STATUS_FREE then
        self:runAction(
        cc.Sequence:create(
        cc.DelayTime:create(delay + 3.0),
        cc.CallFunc:create(
            function ()
                if self._gameView.m_GameEndView ~= nil then
                    self._gameView.m_GameEndView:removeFromParent()
                    self._gameView.m_GameEndView = nil
                end
            end
            )
        ))
    end

    if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
        -- 设置倒计时
        self:SetGameClock(self:GetMeChairID(),cmd.IDI_START_GAME,cmd.TIME_START_GAME)
    end 
    

    self.m_cbPlayStatus = {0, 0, 0, 0, 0}
    self.nVoiceFollow = {0, 0, 0, 0, 0}
    self:PlaySound(cmd.RES.."sound_res/GAME_END.wav")

    if 0 ~=  winner then
        if lGameScore[winner]/self.m_lCellScore > 100 then
            self:PlaySound(cmd.RES.."sound_res/GET_MORECOIN.wav")
        else
            self:PlaySound(cmd.RES.."sound_res/GET_COIN.wav")
        end
    end

end



function GameLayer:exitResultShowBtn()
    self._gameView.m_GameEndView = nil

    if not self.m_bLookOn and not self.m_bStartGame then
        self._gameView.btReady:setVisible(true)
        self._gameView.btChange:setVisible(true)
        self._gameView.btMenuChange:setEnabled(true)
    end
    if PriRoom and GlobalUserItem.bPrivateRoom  then
        self._gameView.btChange:setVisible(false)
        self._gameView.btReady:setPositionX(0)
        self._gameView.btMenuChange:setEnabled(false)
    else
        self._gameView.btChange:setPositionX(150)
        self._gameView.btReady:setPositionX(-150)
    end
end


--等待比牌
function GameLayer:onSubWaitCompare(dataBuffer)
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_WaitCompare, dataBuffer)
    local wCompareUser = cmd_data.wCompareUser
    assert(wCompareUser == self.m_wCurrentUser , "onSubWaitCompare assert wCompareUser ~= m_wCurrentUser")
    --if self.m_wCurrentUser ~= self:GetMeChairID() then
        self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_COMPARE_CARD, cmd.TIME_USER_COMPARE_CARD)
    --end
end



function GameLayer:onSubNoticeAddScore(szString)
    --设置准备按钮不可见
    self.m_bNoScore = true

    if self.m_bStartGame  or GlobalUserItem.dwCurRoomGroupID ~= 0 then
        local msg = szString or "你的游戏币不足，无法继续游戏"
        self.m_querydialog = QueryDialog:create(msg,function()
            if not self.m_bClubRoom then 
                    self:onExitTable()
            end 
        end,nil,1)
        self.m_querydialog:setCanTouchOutside(false)
        self.m_querydialog:addTo(self)
    else
        self.m_bNoScore = true
        self.m_szScoreMsg = szString
    end
end


--发送准备
function GameLayer:onStartGame(bReady)
    self:OnResetGameEngine()
    if bReady == true then

        self:SendUserReady()
        self.m_bStartGame = true
        
    end
end

--自动比牌
function GameLayer:onAutoCompareCard()

    local MyChair = self:GetMeChairID() + 1

    for i = 1 , cmd.GAME_PLAYER - 1 do
        local chair = MyChair - i
        if chair < 1 then
            chair = chair + cmd.GAME_PLAYER
        end
        if self.m_cbPlayStatus[chair] == 1 then
            --发送比牌消息
            local dataBuffer = CCmd_Data:create(2)
            dataBuffer:pushword(chair - 1)
            self:SendData(cmd.SUB_C_COMPARE_CARD,dataBuffer)
            break
        end
    end
end

--比牌操作
function GameLayer:onCompareCard()
    local MyChair = self:GetMeChairID()
    if not MyChair or MyChair ~= self.m_wCurrentUser then
        return
    end
    MyChair = MyChair + 1
    self._gameView.nodeButtomButton:setVisible(false)

    local playerCount = self:getPlayingNum() 

    if playerCount < 2 then
        return
    end
     
    self:KillGameClock()

    local score = self.m_lCurrentTimes*(self.m_bLookCard[MyChair] == true and 4 or 2)

   -- if not GlobalUserItem.bPrivateRoom then
        score=score*self.m_lCellScore
    --end

    print("onCompareCard score:"..score)
    self.m_lTableScore[MyChair] = self.m_lTableScore[MyChair] + score
    self.m_lAllTableScore = self.m_lAllTableScore + score
    self._gameView:PlayerJetton(cmd.MY_VIEWID, score)
    self._gameView:SetUserTableScore(cmd.MY_VIEWID, self.m_lTableScore[MyChair])
    self._gameView:SetAllTableScore(self.m_lAllTableScore)

    
    self:onSendAddScore(score, true)--发送下注消息

    local bAutoCompare = (self:getPlayingNum() == 2)
    if not bAutoCompare then
        bAutoCompare =((self.m_wBankerUser+1) == MyChair and (self.m_lTableScore[MyChair]-score) == self.m_lCellScore) 
    end

    if bAutoCompare then
        self:onAutoCompareCard()
    else
        local compareStatus={false,false,false,false,false}
        for i = 1 ,cmd.GAME_PLAYER do
            if self.m_cbPlayStatus[i] == 1 and i ~= MyChair then
                compareStatus[self:SwitchViewChairID(i-1)] = true
            end
        end
        self._gameView:SetCompareCard(true,compareStatus)
       
        --发送等待比牌消息
        local compareBuffer = CCmd_Data:create(0)
        self:SendData(cmd.SUB_C_WAIT_COMPARE,compareBuffer)
        self:SetGameClock(self.m_wCurrentUser, cmd.IDI_USER_COMPARE_CARD, cmd.TIME_USER_COMPARE_CARD)
    end
end

function  GameLayer:OnCompareChoose(index)
    if not index or index == yl.INVALID_CHAIR then
        print("OnCompareChoose error index")
        return
    end
    local MyChair = self:GetMeChairID()
    if self.m_wCurrentUser ~= MyChair then
        print("OnCompareChoose not m_wCurrentUser")
        return
    end
    MyChair = MyChair+1
    for i = 1 ,cmd.GAME_PLAYER do
        if i ~= MyChair and self.m_cbPlayStatus[i] == 1 and index == self:SwitchViewChairID(i-1) then
            self._gameView:SetCompareCard(false)
            self:KillGameClock()
            --发送比牌消息
            local dataBuffer = CCmd_Data:create(2)
            dataBuffer:pushword(i - 1)
            self:SendData(cmd.SUB_C_COMPARE_CARD,dataBuffer)
            break
        end
    end

end


function GameLayer:getGameRule(buffer)
        --if true then
        --    return nil
        --end
        dump(buffer, "玩法", 5)
        if nil == buffer then
            return
        end

        local strTable ={}
        --游戏模式(玩法)
        local bipai = buffer[6]
        local menpai = buffer[7]
        local xiazhu = buffer[8]

        local szMode = "最小比牌轮数:"..bipai
        table.insert(strTable,szMode)

        szMode = "最小闷牌轮数:"..menpai
        table.insert(strTable,szMode)

        szMode = "最大下注轮数:"..xiazhu
        table.insert(strTable,szMode)

        
    return strTable
end

--放弃操作
function GameLayer:onGiveUp()
    --删除计时器
    self:KillGameClock()
    --隐藏操作按钮
    self._gameView.nodeButtomButton:setVisible(false)
    self._gameView.nodeBeforehandButton:setVisible(false)
    --发送数据
    local dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_GIVE_UP,dataBuffer)
end

--换位操作
function GameLayer:onChangeDesk()
    self._gameFrame:QueryChangeDesk()
end

--看牌操作
function GameLayer:onLookCard()
    local MyChair = self:GetMeChairID()
    if not MyChair or MyChair == yl.INVALID_CHAIR then
        return
    end

    if self.m_wCurrentUser ~= MyChair then
        --return
    end
    print("发看牌")
    self.m_bLookCard[MyChair + 1] = true
    self._gameView.btLookCard:setColor(cc.c3b(158, 112, 8))
    self._gameView.btLookCard:setEnabled(false)

    self._gameView.btLookCard_bef:setColor(cc.c3b(158, 112, 8))
    self._gameView.btLookCard_bef:setEnabled(false)


    --发送消息
    local dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_LOOK_CARD,dataBuffer)
end

--下注操作
function GameLayer:addScore(index,bFollow)
    local MyChair = self:GetMeChairID()
    if self.m_wCurrentUser ~= MyChair then
        return
    end

    MyChair = MyChair + 1

    --self:KillGameClock()
    --清理界面
    self._gameView.m_ChipNode:setVisible(false)
    self._gameView.nodeButtomButton:setVisible(false)

    --下注金额
    local scoreIndex = (not index and 0 or index)
    local score 
    print("xiazhuchushi",self.m_lCurrentTimes)
    if bFollow then
        score = self.m_lCurrentTimes
    else
        score = scoreIndex
    end

    if not GlobalUserItem.bPrivateRoom or GlobalUserItem.bPrivateGoldRoom then
        score=score*self.m_lCellScore
    end

    --看牌加倍
    if self.m_bLookCard[MyChair] == true then
        score = score*2
    end

    
    self.m_lTableScore[MyChair] = self.m_lTableScore[MyChair] + score
    self.m_lAllTableScore = self.m_lAllTableScore + score
    self._gameView:PlayerJetton(cmd.MY_VIEWID, score)
    self._gameView:SetUserTableScore(cmd.MY_VIEWID, self.m_lTableScore[MyChair])
    self._gameView:SetAllTableScore(self.m_lAllTableScore)

print("下注多少",score)
    --发送数据
    self:onSendAddScore(score, false)
end

function GameLayer:betAll()
 
    self._gameView.nodeButtomButton:setVisible(false)
 
    local MyChair = self:GetMeChairID() +1
    print("self.lInitScore",self.lInitScore)
    dump(self.m_lTableScore,"self.m_lTableScore"..MyChair)
    local score=math.max(0,self.lInitScore-self.m_lTableScore[MyChair])

    self.m_lTableScore[MyChair] = self.m_lTableScore[MyChair] + score
   
    self.m_lAllTableScore = self.m_lAllTableScore + score
print("全压了111",score)
    self._gameView:PlayerJetton(cmd.MY_VIEWID, score)
 print("全压了222",score)
    self._gameView:SetUserTableScore(cmd.MY_VIEWID, self.m_lTableScore[MyChair])
  
    self._gameView:SetAllTableScore(self.m_lAllTableScore)
   

    --发送数据
    self:onSendAddScore(score, false)
end

--发送加注消息
function GameLayer:onSendAddScore(score, bCompareCard)
    local  dataBuffer = CCmd_Data:create(10)
    print("最终全压的值",score)
    dataBuffer:pushscore(score)
    dataBuffer:pushword(bCompareCard and 1 or 0)
    
   
    self:SendData(cmd.SUB_C_ADD_SCORE, dataBuffer)
end

--发送请求流水
function GameLayer:onSendQueryRecord()
    local  dataBuffer = CCmd_Data:create(0)
    self:SendData(cmd.SUB_C_REQUEST_RCRecord, dataBuffer)
end

--更新预操按钮控制
function GameLayer:UpdataBeforehandControl()
    local MyChair = self:GetMeChairID() 
    if not MyChair or MyChair == yl.INVALID_CHAIR or self.m_isMeEnd or GlobalUserItem.bVideo or self.m_bLookOn or self.m_cbPlayStatus[self:GetMeChairID()+1]==0 then
        print("UpdataControl myChair is"..(not MyChair and "nil" or "INVALID_CHAIR"))
        self._gameView.nodeBeforehandButton:setVisible(false)
        self._gameView.nodeButtomButton:setVisible(false)
        return
    end
    MyChair = MyChair + 1
    self._gameView.nodeBeforehandButton:setVisible(true)
    self._gameView.m_ChipNode:setVisible(false)


        

    --看牌按钮
    if not self.m_bLookCard[MyChair] then
        self._gameView.btLookCard_bef:setColor(cc.c3b(255, 255, 255))
        self._gameView.btLookCard_bef:setEnabled(true)
    else        
        self._gameView.btLookCard_bef:setColor(cc.c3b(158, 112, 8))
        self._gameView.btLookCard_bef:setEnabled(false)
    end


    self._gameView.btGiveUp_bef:setColor(cc.c3b(255, 255, 255))
    self._gameView.btGiveUp_bef:setEnabled(true)

print(" self.cbTurnCount",self.cbTurnCount," self.lInitScore",self.lInitScore," self.m_lTableScore[MyChair]",self.m_lTableScore[MyChair])

    if GlobalUserItem.bPrivateRoom and nil==self.cbTurnCount or self.cbTurnCount<= self.cbMinLookCardTurnCount then
        self._gameView.btLookCard_bef:setEnabled(false)
        self._gameView.btLookCard_bef:setColor(cc.c3b(158, 112, 8))
    end

end

--更新按钮控制
function GameLayer:UpdataControl()
    local MyChair = self:GetMeChairID() 
    if not MyChair or MyChair == yl.INVALID_CHAIR or GlobalUserItem.bVideo or  self.m_bLookOn then
        print("UpdataControl myChair is"..(not MyChair and "nil" or "INVALID_CHAIR"))
        self._gameView.nodeBeforehandButton:setVisible(false)
        self._gameView.nodeButtomButton:setVisible(false)
        return
    end
    MyChair = MyChair + 1



    
    self._gameView.m_ChipNode:setVisible(false)

    

    if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
        self._gameView.followBetText:setString(self.m_lCurrentTimes*(self.m_bLookCard[MyChair] and 2 or 1))
    else
        self._gameView.followBetText:setString(self.m_lCellScore*self.m_lCurrentTimes*(self.m_bLookCard[MyChair] and 2 or 1))
    end
        

    --看牌按钮
    if not self.m_bLookCard[MyChair] then
        self._gameView.btLookCard:setColor(cc.c3b(255, 255, 255))
        self._gameView.btLookCard:setEnabled(true)
    else        
        self._gameView.btLookCard:setColor(cc.c3b(158, 112, 8))
        self._gameView.btLookCard:setEnabled(false)
    end

    self._gameView.btGiveUp:setColor(cc.c3b(255, 255, 255))
    self._gameView.btGiveUp:setEnabled(true)

  

    self._gameView.btBetAll:setColor( cc.c3b(255, 255, 255))
    self._gameView.btBetAll:setEnabled(true)

    local userCount = self:getPlayingNum()
    print("更新按钮控制时参数 times cellscore MaxCellScore", self.m_lCurrentTimes, self.m_lCellScore, self.m_lMaxCellScore)
    local lTempTime =(self.m_bLookCard[MyChair] == true and 6 or 5)
    if self.lInitScore - self.m_lTableScore[MyChair] >= 
        self.m_lCellScore*(lTempTime + self.m_lCurrentTimes) + (userCount - 1)*self.m_lCurrentTimes*self.m_lCellScore*4 then
        --查找上家
        local perUser
        for i = 1 , cmd.GAME_PLAYER - 1 do
            local tmpchair = MyChair - i
            if tmpchair < 1 then
                tmpchair = tmpchair + cmd.GAME_PLAYER
            end
            if self.m_cbPlayStatus[tmpchair] == 1 then
                perUser = tmpchair
                break
            end
        end
        --跟注按钮
        print("跟注按钮的判断",perUser ,self.m_lTableScore[perUser] , self._gameView.cellTextScore)
        if perUser and (self.m_lTableScore[perUser] == self._gameView.cellTextScore) then
            self._gameView.btFollow:setEnabled(false)
        else
            self._gameView.btFollow:setEnabled(true)
        end

        print("self.m_lCurrentTimes,self.m_lMaxCellScore",self.m_lCurrentTimes,self.m_lMaxCellScore)


        --加注按钮
        if((self.m_lCurrentTimes*self.m_lCellScore) < self.m_lMaxCellScore) then
            self._gameView.btAddScore:setColor((self.m_lCurrentTimes<self.chipArray[4]) and cc.c3b(255, 255, 255) or cc.c3b(158, 112, 8))
            self._gameView.btAddScore:setEnabled(self.m_lCurrentTimes<self.chipArray[4])

            for i=1,4 do
                 self._gameView.btChip[i]:setEnabled(self.m_lCurrentTimes< self.chipArray[i])
            end

        else
            print("exceeded self.m_lMaxCellScore")
            self._gameView.btAddScore:setColor(cc.c3b(158, 112, 8))
            self._gameView.btAddScore:setEnabled(false)
            
            for i=1,4 do
                 self._gameView.btChip[i]:setEnabled(false)
            end

        end  

        --比牌按钮
        print("比牌判断============================>", self.m_lCellScor, self.m_lTableScore[MyChair])
        if self.m_wBankerUser == MyChair - 1 or self.m_lTableScore[MyChair] >= 2*self.m_lCellScore then
            self._gameView.btCompare:setColor(cc.c3b(255, 255, 255))
            self._gameView.btCompare:setEnabled(true)
        else
            self._gameView.btCompare:setColor(cc.c3b(158, 112, 8))
            self._gameView.btCompare:setEnabled(false)     
        end  

    else

        print("不能再下注了！！！")
        local ratio=(GlobalUserItem.bPrivateRoom and 1 or self.m_lCellScore)*(self.m_bLookCard[MyChair] and 2 or 1)
        local leftBet=self.lInitScore-self.m_lTableScore[MyChair]
        print("leftBet,self.m_lCurrentTimes:",leftBet,self.m_lCurrentTimes)


        if leftBet<self.m_lCurrentTimes*ratio*2  then
            self._gameView.btCompare:setColor(cc.c3b(158, 112, 8))
            self._gameView.btCompare:setEnabled(false)
        else
            self._gameView.btCompare:setColor(cc.c3b(255, 255, 255))
            self._gameView.btCompare:setEnabled(true)
        end

        -- if leftBet<self.chipArray[1]*ratio then
        --     self._gameView.btAddScore:setColor(cc.c3b(158, 112, 8))
        --     self._gameView.btAddScore:setEnabled(false)
        -- else
        --     self._gameView.btAddScore:setColor(cc.c3b(255, 255, 255))
        --     self._gameView.btAddScore:setEnabled(true)
        -- end

        self._gameView.btAddScore:setColor((self.m_lCurrentTimes < self.chipArray[4]) and cc.c3b(255, 255, 255) or cc.c3b(158, 112, 8))
        self._gameView.btAddScore:setEnabled(self.m_lCurrentTimes <self.chipArray[4])

        if leftBet<self.m_lCurrentTimes*ratio then
            self._gameView.btFollow:setEnabled(false)
        else
            self._gameView.btFollow:setEnabled(true)
        end

    end

    if self.bShowHand then
        self._gameView.btFollow:setEnabled(false)
        self._gameView.btAddScore:setEnabled(false)
        self._gameView.btCompare:setEnabled(false) 

        self._gameView.btAddScore:setColor(cc.c3b(158, 112, 8))
        self._gameView.btCompare:setColor(cc.c3b(158, 112, 8))

        
        if PriRoom and GlobalUserItem.bPrivateRoom and not GlobalUserItem.bPrivateGoldRoom then
            self._gameView:SetCellScore(2)
        else
            self._gameView:SetCellScore(self.m_lCellScore*2)
        end
    end

  
print(" self.cbTurnCount",self.cbTurnCount," self.lInitScore",self.lInitScore," self.m_lTableScore[MyChair]",self.m_lTableScore[MyChair])
    if nil==self.cbTurnCount or self.cbTurnCount<=3 or self.lInitScore-self.m_lTableScore[MyChair]<=0  then
        self._gameView.btBetAll:setEnabled(false)
        self._gameView.btBetAll:setColor(cc.c3b(158, 112, 8))
    end

    if GlobalUserItem.bPrivateRoom and nil==self.cbTurnCount or self.cbTurnCount<= self.cbMinLookCardTurnCount then
        self._gameView.btLookCard:setEnabled(false)
        self._gameView.btLookCard:setColor(cc.c3b(158, 112, 8))
    end
    if GlobalUserItem.bPrivateRoom and nil==self.cbTurnCount or self.cbTurnCount<=self.cbMinCompareCardTurnCount then
        self._gameView.btCompare:setEnabled(false)
        self._gameView.btCompare:setColor(cc.c3b(158, 112, 8))
    end

    if not self._gameView.btFollow:isEnabled() and not self._gameView.btAddScore:isEnabled() then
        self._gameView.btBetAll:setColor( cc.c3b(255, 255, 255))
        self._gameView.btBetAll:setEnabled(true)
    end


    if self._gameView.autoFollow then
        if self._gameView.btFollow:isEnabled() then
            self:addScore(0,true)
        elseif self._gameView.btBetAll:isEnabled() then
            self:betAll()
        end
        self._gameView.nodeButtomButton:setVisible(false)
        self._gameView.nodeBeforehandButton:setVisible(true)
    else
        self._gameView.nodeButtomButton:setVisible(true)
        self._gameView.nodeBeforehandButton:setVisible(false)
    end
    

    

end

function GameLayer:onUserChat(chatinfo,sendchair)
    if chatinfo and sendchair then
        local viewid = self:SwitchViewChairID(sendchair)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserChat(viewid, chatinfo.szChatString)
        end
    end
end

function GameLayer:onUserExpression(expression,sendchair)
print("GameLayer:onUserExpression",expression,sendchair)
    if expression and sendchair then
        local viewid = self:SwitchViewChairID(sendchair)
        print("2222",viewid, expression.wItemIndex)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:ShowUserExpression(viewid, expression.wItemIndex)
        end
    end
end

-- 语音播放开始
function GameLayer:onUserVoiceStart( useritem, filepath )
    local viewid = self:SwitchViewChairID( useritem.wChairID )
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, true)
    end
end

-- 语音播放结束
function GameLayer:onUserVoiceEnded( useritem, filepath )
    local viewid = self:SwitchViewChairID(useritem.wChairID)
    if viewid and viewid ~= yl.INVALID_CHAIR then
        self._gameView:ShowUserVoice(viewid, false)
    end
end

-- 互动道具消息
function GameLayer:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
    self._gameView:onEventUserPropAction(dwUserID,dwRecvUserID,dwPropID)
end


-------------------------------------------------------新大厅增加函数--------------------------------------------
--游戏记录（房卡）
function GameLayer:onSubGameRecord(dataBuffer)
    print("游戏记录")
      print("dataBuffer:len",dataBuffer:getlen())
    local cmd_data = ExternalFun.read_netdata(cmd.CMD_S_RoomCardRecord, dataBuffer)

    dump(cmd_data, "CMD_S_Record")
    --全部成绩

    self.m_userRecord = {}
    local scoreList = {}
    local nInningsCount = cmd_data.nCount
    for j = 1, nInningsCount do
        local DetailScore = {}
        for i=1, cmd.GAME_PLAYER do
            if i-1==self:GetMeChairID() or not GlobalUserItem.bPrivateGoldRoom then
                DetailScore[i] = cmd_data.lDetailScore[i][j]
            end
        end
        table.insert(scoreList, DetailScore)
    end

    self.m_cardType = {}
    for i=1 , cmd.GAME_PLAYER do
        local caraType = {}
        for j=1 , 7 do
            caraType[j] = cmd_data.cbCardTypeCount[i][j]
        end
        table.insert(self.m_cardType, caraType)
    end
    self._gameView:updataScoreList(scoreList)


    if PriRoom and GlobalUserItem.bPrivateRoom then
        if nil ~= self._gameView._priView and nil ~= self._gameView._priView.updateReloadCardType then
            self._gameView._priView:updateReloadCardType()
        end
    end
end

-- 用户截屏
function GameLayer:onTakeScreenShot(imagepath)
    if type(imagepath) == "string" then
        local exit = cc.FileUtils:getInstance():isFileExist(imagepath)
        print(exit)
        if exit then
            local shareLayer = cc.CSLoader:createNode(cmd.RES.."share/shareLayer.csb"):addTo(self)
            local TAG_WXSHARE = 101
            local TAG_CYCLESHARE = 102
            local MASK_PANEL = 103
            -- 按钮事件
            local touchFunC = function(ref, tType)
                if tType == ccui.TouchEventType.ended then
                    local tag = ref:getTag()
                    local target = nil
                    if TAG_WXSHARE == tag then
                        target = yl.ThirdParty.WECHAT
                    elseif TAG_CYCLESHARE == tag then
                        target = yl.ThirdParty.WECHAT_CIRCLE
                    elseif MASK_PANEL == tag then
                        
                    end
                    if nil ~= target then
                        MultiPlatform:getInstance():shareToTarget(
                            target, 
                            function(isok)

                            end,
                            "截图分享", 
                            "分享我的游戏截图", 
                            yl.HTTP_URL, 
                            imagepath, 
                            "true"
                        )
                    end
                    shareLayer:removeFromParent()
                end
            end

            local bg = shareLayer:getChildByName("bg")
            -- 微信按钮
            local btn = bg:getChildByName("Button_friend")
            btn:setTag(TAG_WXSHARE)
            btn:addTouchEventListener( touchFunC )
            -- 朋友圈按钮
            btn = bg:getChildByName("Button_friendGroup")
            btn:setTag(TAG_CYCLESHARE)
            btn:addTouchEventListener( touchFunC )
            -- 屏蔽层
            local panel = shareLayer:getChildByName("Panel_2")
            panel:setTag(MASK_PANEL)
            panel:addTouchEventListener( touchFunC )
        else
            print("no image")
        end
    end
end


return GameLayer