--
-- Author: chen
-- 
--
local GameViewLayer = class("GameViewLayer", function(scene)
			local gameViewLayer = display.newLayer()
			return gameViewLayer

	end)

--Tag
GameViewLayer.VIEW_TAG = 
{
    tag_bg        = 200,
    tag_autoshoot = 210,
    tag_autolock = 211,
    tag_gameScore= 212,
    tag_gameMultiple = 213,
    tag_grounpTips = 214,
    tag_GoldCycle = 3000,
    tag_GoldCycleTxt = 4000,
    tag_Menu = 5000,
    tag_menu_1 = 301,
    tag_menu_2 = 302,
    tag_fishtype = 303,
    tag_bank = 304,
    tag_setting = 305,
    tag_exit = 306,
    tag_speed = 307,
    tag_change_fish = 308
}
local CHANGE_MULTIPLE_INTERVAL =  0.8
local  TAG = GameViewLayer.VIEW_TAG

local ExternalFun = require(appdf.EXTERNAL_SRC.."ExternalFun")
local g_var = ExternalFun.req_var
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local module_pre = "game.yule.fishdr.src"	
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local PRELOAD = require(module_pre..".views.layer.PreLoading") 
-- local cmd = module_pre .. ".models.CMD_LKGame"
local cmd = ExternalFun.req_var(module_pre ..".models.CMD_LKGame")

local BankLayer = appdf.req(module_pre .. ".views.layer.BankLayer")
local SetLayer = appdf.req(module_pre .. ".views.layer.SettingLayer")
local FishListLayer = appdf.req(module_pre .. ".views.layer.FishListLayer")

-- 金币条位置
local goldPosList = 
{
    cc.p(35, 640), cc.p(1300, 640), cc.p(35, 110), cc.p(1300, 110)
}

local tipColor = cc.c3b(255,228,0);

local scheduler = cc.Director:getInstance():getScheduler()
function GameViewLayer:ctor( scene )

    self._tag = 0
	self._scene = scene

	self:addSerchPaths()

    -----------------------------------
    self._gameFrame = scene._gameFrame
    self.m_pUserItem = self._gameFrame:GetMeUserItem()
    -----------------------------------------------------
   --预加载资源
    PRELOAD.loadTextures()

      --注册事件
    ExternalFun.registerTouchEvent(self,true)
    self.m_bCanChangeMultple = true

    self.m_leftMenuIsShow = false;
    self.m_rightMenuIsShow = false;
end


function GameViewLayer:onExit()

    PRELOAD.unloadTextures()
    PRELOAD.removeAllActions()
    PRELOAD.loadingBar = nil

    PRELOAD.resetData()

    self:StopLoading(true)

    --播放大厅背景音乐
    ExternalFun.playPlazzBackgroudAudio()

    --重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths();
    local newPaths = {};
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(self._searchPath) then
            table.insert(newPaths, v);
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths);

end

function GameViewLayer:StopLoading( bRemove )

    PRELOAD.StopAnim(bRemove)
end

function GameViewLayer:getDataMgr( )
    return self:getParentNode():getDataMgr()
end

function GameViewLayer:getParentNode( )
    return self._scene;
end

function GameViewLayer:addSerchPaths( )
   --搜索路径
    local gameList = self._scene._scene:getApp()._gameList;
    local gameInfo = {};
    for k,v in pairs(gameList) do
          if tonumber(v._KindID) == tonumber(cmd.KIND_ID) then
            gameInfo = v;
            break;
        end
    end

    if nil ~= gameInfo._KindName then
        self._searchPath = device.writablePath.."game/" .. gameInfo._Module .. "/res/";
        cc.FileUtils:getInstance():addSearchPath(self._searchPath);
    end


end

function GameViewLayer:changeMultipleSchedule( dt )

    local function  update( dt  )
        self:multipleUpdate()
    end

    if nil == self.multipleSchedule then
        self.multipleSchedule = scheduler:scheduleScriptFunc(update, dt, false)
    end
    
end

function GameViewLayer:unMultipleSchedule()
    self.m_bCanChangeMultple = true
    if nil ~= self.multipleSchedule then
        scheduler:unscheduleScriptEntry(self.multipleSchedule)
        self.multipleSchedule = nil
    end
end

function GameViewLayer:multipleUpdate()
    self:unMultipleSchedule()  
end

--界面初始化
function GameViewLayer:initCsbRes()
    local rootLayer, csbNode = ExternalFun.loadRootCSB("csb/GameViewLayer.csb", self);
    rootLayer:setLocalZOrder(8);
    self._csbNode = csbNode;

    -- 初始化ui
    self:initUiButton(csbNode);
end

-- 菜单显示
function GameViewLayer:onMenuAction(node_menu, isAleft, isLeftMenu)
    local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
    local xRate = framesize.width / yl.WIDTH 
    local posX = 0;
    if framesize.width == 2436 and framesize.height == 1125 then
        posX = 34*xRate
    end 
    if isAleft then
        local pos = cc.p(0, 0);
        if isLeftMenu then
            pos = cc.p(-110 + posX, 402);
        else
            pos = cc.p(1334, 402);
        end
        node_menu:runAction(cc.Sequence:create(
            cc.CallFunc:create( function()
                -- node_menu:setVisible(true);
            end),
             
            cc.MoveTo:create(0.2,pos)
        ));
    else
        local pos = cc.p(0, 0);
        if isLeftMenu then
            pos = cc.p(0 + posX, 402);
        else
            pos = cc.p(1444, 402);
        end
        node_menu:runAction(cc.Sequence:create(
            cc.MoveTo:create(0.2,pos),
            cc.CallFunc:create( function()
                -- node_menu:setVisible(false);
            end)
        ));
    end
end

--界面初始化
function GameViewLayer:initUiButton(csbNode)
    --按钮列表
    local function btnEvent( sender, eventType )
        if eventType == ccui.TouchEventType.ended then
            self:ButtonEvent(sender,eventType);
        end
    end 

    local node_menu_1 = csbNode:getChildByName("node_menu_1");
    self.node_menu_1 = node_menu_1;

    local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
    local xRate = framesize.width / yl.WIDTH 

    if framesize.width == 2436 and framesize.height == 1125 then
        node_menu_1:setPositionX(-110 + 34*xRate)
    end

    local node_menu_2 = csbNode:getChildByName("node_menu_2");
    self.node_menu_2 = node_menu_2;

    -- 左菜单
    local btn_menu = node_menu_1:getChildByName("btn_menu");
    btn_menu:setTag(TAG.tag_menu_1);
    btn_menu:addTouchEventListener(btnEvent);
    self.btn_menu = btn_menu;

    -- 鱼种
    local btn_fish_type = node_menu_1:getChildByName("btn_fish_type");
    btn_fish_type:setTag(TAG.tag_fishtype);
    btn_fish_type:addTouchEventListener(btnEvent);

    -- 银行
    local btn_bank = node_menu_1:getChildByName("btn_bank");
    btn_bank:setTag(TAG.tag_bank);
    btn_bank:addTouchEventListener(btnEvent);

    -- 设置
    local btn_set = node_menu_1:getChildByName("btn_set");
    btn_set:setTag(TAG.tag_setting);
    btn_set:addTouchEventListener(btnEvent);

    -- 退出
    local btn_exit = node_menu_1:getChildByName("btn_exit");
    btn_exit:setTag(TAG.tag_exit);
    btn_exit:addTouchEventListener(btnEvent);

    -- 右菜单
    local btn_menu2 = node_menu_2:getChildByName("btn_menu");
    btn_menu2:setTag(TAG.tag_menu_2);
    btn_menu2:addTouchEventListener(btnEvent);
    self.btn_menu2 = btn_menu2;

    -- 锁定
    local btn_lock = node_menu_2:getChildByName("btn_lock");
    btn_lock:setTag(TAG.tag_autolock);
    btn_lock:addTouchEventListener(btnEvent);
    self.btn_lock = btn_lock;

    -- -- 加速
    -- local btn_speed = node_menu_2:getChildByName("btn_speed");
    -- btn_speed:setTag(TAG.tag_speed);
    -- btn_speed:addTouchEventListener(btnEvent);

    -- 切换锁定鱼
    local btn_change_fish = node_menu_2:getChildByName("btn_change_fish");
    btn_change_fish:setTag(TAG.tag_change_fish);
    btn_change_fish:addTouchEventListener(btnEvent);

    -- 自动
    local btn_auto = node_menu_2:getChildByName("btn_auto");
    btn_auto:setTag(TAG.tag_autoshoot);
    btn_auto:addTouchEventListener(btnEvent);
    self.btn_auto = btn_auto;
end

function GameViewLayer:initView(  )
    local bg =  ccui.ImageView:create("game_res/game_bg_0.png")
	bg:setAnchorPoint(cc.p(.5,.5))
    bg:setTag(TAG.tag_bg)
	bg:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
	self:addChild(bg)

    --初始化csb界面
    self:initCsbRes();

    local function callBack( sender, eventType)
        self:ButtonEvent(sender,eventType)
    end
	
    --水波效果
    local render = cc.RenderTexture:create(1334,750)
    render:beginWithClear(0,0,0,0)
    local water = cc.Sprite:createWithSpriteFrameName("water_0.png")
    water:setScale(2.5)
    water:setOpacity(120)
    water:setBlendFunc(gl.SRC_ALPHA,gl.ONE)
    water:visit()
    render:endToLua()
    water:addChild(render)
    render:setPosition(667,375) 
    water:setPosition(667,375)
    self:addChild(water, 6)

    local ani1 = cc.Animate:create(cc.AnimationCache:getInstance():getAnimation("WaterAnim"))
    local ani2 = ani1:reverse()

    local action = cc.RepeatForever:create(cc.Sequence:create(ani1,ani2))
    water:runAction(action)
end

function GameViewLayer:initConnnonAddOrLess()
    -- --获取自己炮台
    local function getCannonPos()
        
        local cannonPos = self._scene.m_nChairID
        if self._scene._dataModel.m_reversal then 
            cannonPos = cmd.cannonMax - cannonPos
        end
        return cannonPos
    end
    local cannon = self._scene.m_cannonLayer:getCannoByPos(getCannonPos() + 1)

    --加
    local switchAddBtn = ccui.Button:create("main/btn_add_1.png", "main/btn_add_2.png")
    -- switchAddBtn:setAnchorPoint(0.5,0.0)
    switchAddBtn:addTouchEventListener(function( sender , eventType )
        if eventType == ccui.TouchEventType.ended then
            if not self.m_bCanChangeMultple then
                -- return
            end

            --激光不能切换倍数
            local cannonPos = self._scene.m_nChairID
            if self._scene._dataModel.m_reversal then 
                cannonPos = cmd.cannonMax - cannonPos
            end
            local cannon = self._scene.m_cannonLayer:getCannoByPos(cannonPos + 1)
            --激光不能切换倍数
            if cmd.CannonType.Laser_Cannon == cannon.m_Type or cmd.CannonType.Laser_Shooting == cannon.m_Type then
                 return
            end
            --出现自己的宝箱不能切换倍数
            if self._scene._dataModel.m_bBoxTime then
                return
            end

            --加速不能切换倍数
            if cmd.CannonType.Special_Cannon == cannon.m_Type then
                showToast(cc.Director:getInstance():getRunningScene(), "在加速状态下不能切换炮台", 2)
                return
            end

            self._scene._dataModel:playEffect(cmd.SWITCHING_RUN)
            local index = self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1]
            index = index + 1
            
            local nNum = cmd.MULTIPLE_MAX_INDEX;

            if index >= nNum then
                index = 0;
            end
            -- index = math.mod(index,6)
           
            local cmddata = CCmd_Data:create(4)
            cmddata:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_MULTIPLE);
            cmddata:pushint(index)
            self._scene:sendNetData(cmddata) 
            self.m_bCanChangeMultple = false
            self:changeMultipleSchedule(CHANGE_MULTIPLE_INTERVAL)
            print("切换炮台.........index.22222.............", index)

            self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1] = index

            cannon:setMultiple(self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1])

            self:updateMultiple(self._scene._dataModel.m_secene.nMultipleValue[1][index+1]) 
        
        end
    end)
    switchAddBtn:setPosition(140, 20)
    cannon:addChild(switchAddBtn,10)

    --减
    local switchLessBtn = ccui.Button:create("main/btn_less_1.png", "main/btn_less_2.png")
    -- switchLessBtn:setAnchorPoint(0.5,0.0)
    switchLessBtn:addTouchEventListener(function( sender , eventType )
        if eventType == ccui.TouchEventType.ended then
           
            if not self.m_bCanChangeMultple then
                -- return
            end

            --激光不能切换倍数
            local cannonPos = self._scene.m_nChairID
            if self._scene._dataModel.m_reversal then 
                cannonPos = cmd.cannonMax - cannonPos
            end
            local cannon = self._scene.m_cannonLayer:getCannoByPos(cannonPos + 1)
            --激光不能切换倍数
            if cmd.CannonType.Laser_Cannon == cannon.m_Type or cmd.CannonType.Laser_Shooting == cannon.m_Type then
                 return
            end
            --出现自己的宝箱不能切换倍数
            if self._scene._dataModel.m_bBoxTime then
                return
            end

            --加速不能切换倍数
            if cmd.CannonType.Special_Cannon == cannon.m_Type then
                showToast(cc.Director:getInstance():getRunningScene(), "在加速状态下不能切换炮台", 2)
                return
            end

            self._scene._dataModel:playEffect(cmd.SWITCHING_RUN)
            local index = self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1]
            index = index - 1

            local nNum = cmd.MULTIPLE_MAX_INDEX
            if index < 0 then
                index = nNum - 1
            end
            -- index = math.mod(index,6)

            local cmddata = CCmd_Data:create(4)
            cmddata:setcmdinfo(game_cmd.MDM_GF_GAME, cmd.SUB_C_MULTIPLE);
            cmddata:pushint(index)
            self._scene:sendNetData(cmddata) 
            self.m_bCanChangeMultple = false
            self:changeMultipleSchedule(CHANGE_MULTIPLE_INTERVAL)
            print("切换炮台.........index..............", index)

            self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1] = index

            cannon:setMultiple(self._scene._dataModel.m_secene.nMultipleIndex[1][self._scene.m_nChairID+1])

            self:updateMultiple(self._scene._dataModel.m_secene.nMultipleValue[1][index+1]) 
        end
    end)
    switchLessBtn:setPosition(-40, 20)
    cannon:addChild(switchLessBtn,10)
end

function GameViewLayer:initUserInfo()
   
end

function GameViewLayer:updateUserScore( score )
    
    -- local _score  = self:getChildByTag(TAG.tag_gameScore)
    -- if nil ~=  _score then
    --     _score:setString(string.format("%d",score))
    -- end
end

function GameViewLayer:updateMultiple( multiple )
    -- local _Multiple = self:getChildByTag(TAG.tag_gameMultiple)
    -- if nil ~=  _Multiple then
    --     _Multiple:setString(string.format("%d:",multiple))
    -- end

end

function GameViewLayer:updteBackGround(param, isHide)


    local bg = self:getChildByTag(TAG.tag_bg)

    if bg  then
        local call = cc.CallFunc:create(function()
            bg:removeFromParent()
        end)

        bg:runAction(cc.Sequence:create(cc.FadeTo:create(2.5,0),call))

        local bgfile = string.format("game_res/game_bg_%d.png", param)
        local _bg = cc.Sprite:create(bgfile)
        _bg:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
        _bg:setOpacity(0)
        _bg:setTag(TAG.tag_bg)
        self:addChild(_bg)

        _bg:runAction(cc.FadeTo:create(5,255))
    end

    --断线重进不显示
    if isHide then
        return;
    end

    --鱼阵提示、
    local groupTips = ccui.ImageView:create("game_res/fish_grounp.png")
    groupTips:setPosition(cc.p(yl.WIDTH/2,yl.HEIGHT/2))
    groupTips:setTag(TAG.tag_grounpTips)

    self:addChild(groupTips,30)

    local callFunc = cc.CallFunc:create(function()
            groupTips:removeFromParent()
        end)

    groupTips:runAction(cc.Sequence:create(cc.DelayTime:create(5.0),callFunc))

       
end

function GameViewLayer:setAutoShoot(b,target)
                 
    if b then
        target:loadTextureNormal("main/btn_auto_1.png")
        target:loadTexturePressed("main/btn_auto_2.png")

    else
        target:loadTextureNormal("main/btn_auto_2.png")
        target:loadTexturePressed("main/btn_auto_1.png")

         -- target:removeChildByTag(1)
    end
          
end

function GameViewLayer:setAutoLock(b,target)
          
    if b then

        target:loadTextureNormal("main/btn_lock_1.png")
        target:loadTexturePressed("main/btn_lock_2.png")

    else
        target:loadTextureNormal("main/btn_lock_2.png")
        target:loadTexturePressed("main/btn_lock_1.png")

         --取消自动射击
         self._scene._dataModel.m_fishIndex = cmd.INT_MAX

        --删除自动锁定图标
         local cannonPos = self._scene.m_nChairID
         if self._scene._dataModel.m_reversal then 
           cannonPos = cmd.cannonMax - cannonPos
         end

         local cannon = self._scene.m_cannonLayer:getCannoByPos(cannonPos + 1)
         cannon:removeLockTag()

    end              
end


--银行操作成功
function GameViewLayer:onBankSuccess( )
     self._scene:dismissPopWait()

    local bank_success = self._scene.bank_success
    if nil == bank_success then
        return
    end

    --local addScore = bank_success.lUserScore - GlobalUserItem.lUserScore
    GlobalUserItem.tabAccountInfo.lUserScore = bank_success.lUserScore 
    GlobalUserItem.tabAccountInfo.lUserInsure = bank_success.lUserInsure


    print("onBankSuccess addScore lUserScore",addScore,bank_success.lUserScore,GlobalUserItem.tabAccountInfo.lUserScore)

    self:refreshScore()


    showToast(cc.Director:getInstance():getRunningScene(), bank_success.szDescribrString, 2)
end

--银行操作失败
function GameViewLayer:onBankFailure( )

     self._scene:dismissPopWait()
    local bank_fail = self._scene.bank_fail
    if nil == bank_fail then
        return
    end

    showToast(cc.Director:getInstance():getRunningScene(), bank_fail.szDescribeString, 2)
end

--银行资料
function GameViewLayer:onGetBankInfo(bankinfo)
    bankinfo.wRevenueTake = bankinfo.wRevenueTake or 10;
    if self.bankLayer then
        self.bankLayer:refreshBankRate(bankinfo.wRevenueTake, bankinfo.lBankOPButtonArray[1])
    end
end

  --刷新游戏币
function GameViewLayer:refreshScore( )
     --携带游戏币
    local str = ExternalFun.numberThousands(self._scene._dataModel.m_secene.lPalyCurScore[1][self._scene.m_nChairID+1])
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end

    -- 银行
    if self.bankLayer ~= nil then
        local curScore = self._scene._dataModel.m_secene.lPalyCurScore[1][self._scene.m_nChairID+1];
        local bankScore = GlobalUserItem.tabAccountInfo.lUserInsure;
        self.bankLayer:refreshScore(curScore, bankScore);
    end
    
    --用户分数
    self:updateUserScore(self._scene._dataModel.m_secene.lPalyCurScore[1][self._scene.m_nChairID + 1])

    
    --获取炮台视图位置
    local cannonPos = self._scene.m_nChairID
    if self._scene._dataModel.m_reversal then 
      cannonPos = cmd.cannonMax - cannonPos
    end
    --更新炮台分数
    self._scene.m_cannonLayer:updateUserScore(self._scene._dataModel.m_secene.lPalyCurScore[1][self._scene.m_nChairID+1],cannonPos+1 )

end

function GameViewLayer:ButtonEvent( sender , eventType)
    
    if eventType == ccui.TouchEventType.ended then

        local function getCannonPos()
             --获取自己炮台
          local cannonPos = self._scene.m_nChairID
          if self._scene._dataModel.m_reversal then 
             cannonPos = cmd.cannonMax - cannonPos
          end
          return cannonPos
        end

        local tag = sender:getTag()

        if tag == TAG.tag_autoshoot then --自动射击
            self._scene._dataModel.m_autoshoot = not self._scene._dataModel.m_autoshoot

            if self._scene._dataModel.m_autoshoot then
                self._scene._dataModel.m_autolock = false
            end

            self:setAutoShoot(self._scene._dataModel.m_autoshoot,sender)
            -- local lock = self:getChildByTag(TAG.tag_autolock)
            local lock = self.btn_lock;
            self:setAutoLock(self._scene._dataModel.m_autolock,lock)

            local isauto = false

            if self._scene._dataModel.m_autoshoot or self._scene._dataModel.m_autolock then
                isauto =  true
            end
             
            local cannon = self._scene.m_cannonLayer:getCannoByPos(getCannonPos() + 1)
            cannon:setAutoShoot(isauto)

            if self._scene._dataModel.m_autoshoot then
                cannon:removeLockTag()
            end
                
        elseif tag == TAG.tag_autolock then --自动锁定
            self._scene._dataModel.m_autolock = not self._scene._dataModel.m_autolock
            if self._scene._dataModel.m_autolock then
                self._scene._dataModel.m_autoshoot = false
            end
          
            -- local auto = self:getChildByTag(TAG.tag_autoshoot)
            local auto = self.btn_auto;
            self:setAutoShoot(self._scene._dataModel.m_autoshoot,auto)
            self:setAutoLock(self._scene._dataModel.m_autolock,sender) 

            local isauto = false

            if self._scene._dataModel.m_autoshoot or self._scene._dataModel.m_autolock then
                isauto =  true
            end
         
            local cannon = self._scene.m_cannonLayer:getCannoByPos(getCannonPos() + 1)
            cannon:setAutoShoot(isauto)

            if self._scene._dataModel.m_autoshoot then
                cannon:removeLockTag()
            end
        elseif tag == TAG.tag_menu_1 then --菜单1
            if self.m_leftMenuIsShow then
                self:onMenuAction(self.node_menu_1,true, true)
                self.m_leftMenuIsShow = false;
                self.btn_menu:loadTextures("main/btn_left_1.png", "main/btn_left_2.png");
            else
                self:onMenuAction(self.node_menu_1,false, true)
                self.m_leftMenuIsShow = true;
                self.btn_menu:loadTextures("main/btn_right_1.png", "main/btn_right_2.png");
            end 
        elseif tag == TAG.tag_menu_2 then --菜单2
            if self.m_rightMenuIsShow then
                self:onMenuAction(self.node_menu_2,false, false)
                self.m_rightMenuIsShow = false;
                self.btn_menu2:loadTextures("main/btn_left_1.png", "main/btn_left_2.png");
            else
                self:onMenuAction(self.node_menu_2,true, false)
                self.m_rightMenuIsShow = true;
                self.btn_menu2:loadTextures("main/btn_right_1.png", "main/btn_right_2.png");
            end 
        elseif tag == TAG.tag_fishtype then --鱼种
            self:onFishListLayer();
        elseif tag == TAG.tag_bank then --银行
            --银行
            if 0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
                showToast(self, "初次使用，请先开通银行！", 1)
                return 
            end

            self:onBankLayer();
        elseif tag == TAG.tag_setting then --设置
            self:onSetLayer();
            -- self._scene._gameFrame:QueryChangeDesk();
        elseif tag == TAG.tag_exit then --退出
            self._scene:onQueryExitGame();
        elseif tag == TAG.tag_speed then --加速
        elseif tag == TAG.tag_change_fish then --切换锁定鱼

            if self._scene._dataModel.m_autolock then
                -- self._scene._dataModel.m_fishSelectCurMax = self._scene._dataModel.m_fishSelectCurMax + 1; --第几大鱼
                -- self._scene._dataModel.m_fishIndex = self._scene._dataModel:selectMaxFish()

                local typeList = self._scene._dataModel:curFishTypeList();
                local isF = true;

                if self._scene._dataModel.curSelectFishType then
                    for i = 1, #typeList do
                        if typeList[i] < self._scene._dataModel.curSelectFishType and isF then
                            self._scene._dataModel.curSelectFishType = typeList[i];
                            self._scene._dataModel.curSelectfishIndex = 2147483647;
                            isF = false;
                        end
                    end
                end
                
                if isF and #typeList > 0 then -- 
                    self._scene._dataModel.curSelectFishType = typeList[1];
                    self._scene._dataModel.curSelectfishIndex = 2147483647;
                end
            end
        elseif tag == TAG.tag_Menu then --菜单
        end
    end
end

-- 银行
function GameViewLayer:onBankLayer()
    local bankLayer = BankLayer:create();
    bankLayer.m_parent = self;
    self:addChild(bankLayer,200);
    self.bankLayer = bankLayer;
    --银行消息请求
    self:getParentNode():sendRequestBankInfo();

    self:refreshScore();
end

-- 设置
function GameViewLayer:onSetLayer()
    local setLayer = SetLayer:create();
    setLayer.m_parent = self;
    self:addChild(setLayer,200);
end

-- 鱼表
function GameViewLayer:onFishListLayer()
    local fishListLayer = FishListLayer:create();
    fishListLayer.m_parent = self;
    self:addChild(fishListLayer,200);
end

function GameViewLayer:Showtips( tips )

    if tips == "" then
        return;
    end

    local lb =  cc.Label:createWithTTF(tips, "fonts/round_body.ttf", 20)
    local bg = ccui.ImageView:create("game_res/clew_box.png")
    lb:setTextColor(tipColor)
    bg:setScale9Enabled(true)
    bg:setContentSize(cc.size(lb:getContentSize().width + 60  , 40))
    bg:setScale(0.1)
    lb:setPosition(bg:getContentSize().width/2, 20)
    bg:addChild(lb)

    self:ShowTipsForBg(bg)

end

function GameViewLayer:ShowCoin( score,wChairID,pos,fishtype ,nMultipleCount)

  --print("score.."..score.."wChairID.."..wChairID.."fishtype.."..fishtype)

  --------------------------------------
  local nMyNum = self.m_pUserItem.wChairID >= cmd.connonLimit
  local playerNum = wChairID >= cmd.connonLimit
  ---------------------------------------

  self._scene._dataModel:playEffect(cmd.Coinfly)

  local silverNum = {2,2,3,4,4}
  local goldNum = {1,1,1,2,2,3,3,4,5,6,8,16,16,16,18,18,18}
  
  -- local nMyNum = self._scene.m_nChairID/cmd.connonLimit
  -- local playerNum = wChairID/cmd.connonLimit

  local cannonPos = wChairID
--获取炮台
  if self._scene._dataModel.m_reversal then 
     cannonPos = cmd.cannonMax - cannonPos
   end

   local cannon = self._scene.m_cannonLayer:getCannoByPos(cannonPos + 1)

   if nil == cannon then
      return
   end

   local anim = nil
   local coinNum = 1
   local frameName = nil
   local distant = 50


  if fishtype < 5 then
    anim = cc.AnimationCache:getInstance():getAnimation("SilverAnim")
    frameName = "silver_coin_0.png"
    coinNum = silverNum[fishtype+1]
  elseif fishtype>=5 and fishtype<17 then
    anim = cc.AnimationCache:getInstance():getAnimation("GoldAnim")
    frameName = "gold_coin_0.png"

    coinNum = goldNum[fishtype+1]

  elseif fishtype == cmd.FishType.FishType_YuanBao then
    anim = cc.AnimationCache:getInstance():getAnimation("FishIgnotCoin")
    frameName = "ignot_coin_0.png"
    coinNum = 1
  end

  local posX = {}
  local initX = -105
  posX[1] = initX

  for i=2,10 do
    posX[i] = initX-(i-1)*39
  end

  local node = cc.Node:create()
  node:setAnchorPoint(0.5,0.5)
  node:setContentSize(cc.size(distant*5 , distant*4))
  
  if coinNum > 5 then
    node:setContentSize(cc.size(distant*5 , distant*2+40))
  end

  node:setPosition(pos.x, pos.y)
  self._scene.m_cannonLayer:addChild(node,1)

  if nil ~= anim then
      local action = cc.RepeatForever:create(cc.Animate:create(anim))
     
      if coinNum > 10 then
        coinNum = 10
      end

     local num = cc.LabelAtlas:create(string.format("%d", score),"game_res/num_game_gold.png",37,34,string.byte("0"))
     num:setAnchorPoint(0.5,0.5)
     num:setPosition(node:getContentSize().width/2, node:getContentSize().height-140)
     node:addChild(num)
     local call = cc.CallFunc:create(function()
       num:removeFromParent()
     end)

     num:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),call))

     local secondNum = coinNum
     if coinNum > 5 then
        secondNum = coinNum/2 
     end

     local node1 = cc.Node:create()
     node1:setContentSize(cc.size(distant*secondNum, distant))
     node1:setAnchorPoint(0.5,0.5)
     node1:setPosition(node:getContentSize().width/2, distant/2)
     node:addChild(node1)

     for i=1,secondNum do
       local coin = cc.Sprite:createWithSpriteFrameName(frameName)
       coin:runAction(action:clone())
       coin:setPosition(distant/2+(i-1)*distant, distant/2)
       node1:addChild(coin)
     end

     if coinNum > 5 then
       local firstNum = coinNum - secondNum
       local node2 = cc.Node:create()
       node2:setContentSize(cc.size(distant*firstNum, distant))
       node2:setAnchorPoint(0.5,0.5)
       node2:setPosition(node:getContentSize().width/2, distant*3/2)
       node:addChild(node2)

     end
  end

  local cannonPos = cc.p(cannon:getPositionX(),cannon:getPositionY())
  local call = cc.CallFunc:create(function()
    node:removeFromParent()
  end)

  node:runAction(cc.Sequence:create(cc.MoveBy:create(1.0,cc.p(0,40)),cc.MoveTo:create(0.5,cannonPos),call))

  local angle = 70.0
  local time = 0.12
  local moveY = 30.0

  if fishtype >= cmd.FishType.FishType_JianYu and fishtype <= cmd.FishType.FishType_LiKui then
    self._scene._dataModel:playEffect(cmd.bingo)
    local goldCycle = self:getChildByTag(TAG.tag_GoldCycle + wChairID )
    if nil == goldCycle then
        goldCycle = cc.Sprite:create("game_res/goldCircle.png")
        goldCycle:setTag(TAG.tag_GoldCycle + wChairID)

        goldCycle:setPosition(pos.x, pos.y)
        self:addChild(goldCycle,6)
        local call = cc.CallFunc:create(function( )
           goldCycle:removeFromParent()
        end)
        goldCycle:runAction(cc.Sequence:create(cc.RotateBy:create(time*18,360*1.3),call))
    end


    local goldTxt = self:getChildByTag(TAG.tag_GoldCycleTxt + wChairID)
    if goldTxt == nil then

      goldTxt = cc.LabelAtlas:create(string.format("%d", score),"game_res/mutipleNum.png",14,17,string.byte("0"))
      goldTxt:setAnchorPoint(0.5,0.5)

      goldTxt:setPosition(pos.x, pos.y)          
      self:addChild(goldTxt,6)

      local action = cc.Sequence:create(cc.RotateTo:create(time*2,angle),cc.RotateTo:create(time*4,-angle),cc.RotateTo:create(time*2,0))
      local call = cc.CallFunc:create(function()  
          goldTxt:removeFromParent()
      end)

      goldTxt:runAction(cc.Sequence:create(action,call))

    end

  end
  ---------------------------------------------------------------------------------
    if fishtype ~= cmd.FishType_BaoXiang then
        local cannonPos = wChairID;
        --获取炮台
        if self._scene._dataModel.m_reversal then 
            cannonPos = cmd.cannonMax - cannonPos;
        end
        local goldPos = goldPosList[cannonPos+1];

        -- local bannerText = cc.LabelAtlas:create(string.format("%d", score),"game_res/mutipleNum.png",14,17,string.byte("0"))

        local scoreStr = score;
        if score <= 0 then
            return;
        end
        if score >= 10000 then
            scoreStr = math.floor(score/10000) .. "W";
        end

        local bannerText =  cc.Label:createWithTTF(scoreStr, "fonts/round_body.ttf", 20)
        bannerText:setTextColor(cc.YELLOW)

        bannerText:setTag(7000)
        bannerText:setAnchorPoint(cc.p(0.5,0.5))

        if cannon.m_nBannerColor == 0 then
            cannon.m_nBannerColor = 1
        else
            cannon.m_nBannerColor = 0
        end
        local spGoldBanner = cc.Sprite:create("game_res/gold_banner.png")
        local goldBanner = ccui.Scale9Sprite:create("game_res/gold_banner.png")
        goldBanner:updateWithSprite(spGoldBanner,cc.rect(0,18*cannon.m_nBannerColor,75, 18),false, cc.rect(5, 5, 65, 8))
        goldBanner:setContentSize(cc.size(bannerText:getContentSize().width + 10, bannerText:getContentSize().height))

        local oneGoldHeight = 7 --一个金币高度
        local goldsNums =  score / cmd.GOLD_SCORE--一万分一个金币
        if goldsNums <= 0 then
            goldsNums = 1
        end 
        
        -- 金币分档
        goldsNums = 1;
        if nMultipleCount >= 2 and nMultipleCount <= 5 then
            goldsNums = 1;
        elseif nMultipleCount >=6 and nMultipleCount <=10 then
            goldsNums = 2;
        elseif nMultipleCount >=12 and nMultipleCount <=20 then
            goldsNums = 3;
        elseif nMultipleCount >=25 and nMultipleCount <=40 then
            goldsNums = 4;
        elseif nMultipleCount >=150 and nMultipleCount <=1000 then
            goldsNums = 5;
        end

        local totalGoldHeight = oneGoldHeight * goldsNums
        if totalGoldHeight >= 35 then
            totalGoldHeight = 35
        end
        --totalGoldHeight = 399
        local golds = cc.Sprite:create("game_res/golds.png",cc.rect(0,0,38,totalGoldHeight))
        goldBanner:addChild(golds, 19)

        local posArray = {}
        local initPos = 0;
        posArray[1] = initPos
        local moveX = 0
        local moveY = totalGoldHeight-- + 16
        local tMoveX = 70
        local tMoveY = 0
        for i = 1, 5 do
            if i == 5 then
                posArray[i] = 140 - i * 70;
            else
                posArray[i] = 70 - i * 70
            end
            
        end

        local posOff = posArray[#cannon.m_goldList + 1];
        if cannonPos+1 == 1 or cannonPos+1 == 3 then
            posOff = - posOff;
            tMoveX = -70;
        else

        end

        if nMyNum == playerNum then
            goldBanner:setPosition(cc.p(goldPos.x + posOff, goldPos.y + 10))
            golds:setPosition(cc.p(goldBanner:getContentSize().width / 2, - totalGoldHeight / 2))
        else
            moveY = -totalGoldHeight
            goldBanner:setPosition(cc.p(goldPos.x + posOff, goldPos.y - 10))
            golds:setPosition(cc.p(goldBanner:getContentSize().width / 2, totalGoldHeight / 2 + 20))
        end



        local seAction = cc.Sequence:create(cc.MoveBy:create(0.2,cc.p(moveX,moveY)));

        local index = #cannon.m_goldList

        if index >= 4 then
            local node = cannon.m_goldList[4];
            local text = node:getChildByTag(7000);
            if nil ~= text and nil~=text.lScore then
                text.lScore = text.lScore + score;
                text:setString(string.format("%d", text.lScore));
            end

            --先移除第4个,移动前3个,再显示第4个
            if #cannon.m_goldList  and (#cannon.m_goldList == 4) then
                cannon.m_goldList[1]:removeAllChildren();
                cannon.m_goldList[1]:removeFromParent();
                table.remove(cannon.m_goldList,1);
            end

            for i = 1, #cannon.m_goldList do
                local node = cannon.m_goldList[i];
                node:runAction(cc.Sequence:create(cc.MoveBy:create(0.2,cc.p(tMoveX,tMoveY)), cc.CallFunc:create(function()
                end)))
            end

            self:addChild(goldBanner, 19)
            bannerText:setPosition(cc.p(goldBanner:getContentSize().width / 2, goldBanner:getContentSize().height / 2))
            goldBanner:addChild(bannerText)
            table.insert(cannon.m_goldList,goldBanner)
            goldBanner:runAction(cc.Sequence:create(seAction, cc.DelayTime:create(1.0),cc.CallFunc:create(function()
                cannon.m_goldList[1]:removeAllChildren();
                cannon.m_goldList[1]:removeFromParent();
                table.remove(cannon.m_goldList,1);

                for i = 1, #cannon.m_goldList do
                    local node = cannon.m_goldList[i];
                    node:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(tMoveX,tMoveY)), cc.CallFunc:create(function()
                    end)))
                end
            end)))

        else
            self:addChild(goldBanner, 19)
            bannerText:setPosition(cc.p(goldBanner:getContentSize().width / 2, goldBanner:getContentSize().height / 2))
            goldBanner:addChild(bannerText)
            table.insert(cannon.m_goldList,goldBanner)
            goldBanner:runAction(cc.Sequence:create(seAction, cc.DelayTime:create(1.0), cc.CallFunc:create(function()
                cannon.m_goldList[1]:removeAllChildren();
                cannon.m_goldList[1]:removeFromParent();
                table.remove(cannon.m_goldList,1);

                for i = 1, #cannon.m_goldList do
                    local node = cannon.m_goldList[i];
                    node:runAction(cc.Sequence:create(cc.MoveBy:create(0.1,cc.p(tMoveX,tMoveY)), cc.CallFunc:create(function()
                    end)))
                end
            end)))

        end

    end

end

function GameViewLayer:ShowAwardTip(data)

  local labelList = {}

  local tipStr  = ""
  local tipStr1 = ""
  local tipStr2 = ""

  if data.nFishMultiple >= 50 then
    if data.nScoreType == cmd.SupplyType.EST_Cold then
       tipStr = "捕中了"..cmd.FishTypeName[data.nFishType+1]..",获得"
    elseif data.nScoreType == cmd.SupplyType.EST_Laser then
      
       tipStr = "使用激光,获得"
    end

  tipStr1 = string.format("%d倍 %d分数",data.nFishMultiple,data.lFishScore)
  if data.nFishMultiple > 500 then
     tipStr2 = "超神了!!!"
  elseif data.nFishMultiple == 19 then
       tipStr2 = "运气爆表!!!"   
  else
      tipStr2 = "实力超群!!!"     
  end

  local name = data.szPlayName
  local tableStr = nil
  if data.wTableID == self._scene.m_nTableID  then 
    tableStr = "本桌玩家"

  else
       tableStr = string.format("第%d桌玩家",data.wTableID+1)

  end

  local lb1 =  cc.Label:createWithTTF(tableStr, "fonts/round_body.ttf", 20)
  lb1:setTextColor(tipColor)
  lb1:setAnchorPoint(0,0.5)
  table.insert(labelList, lb1)
 

  local lb2 =  cc.Label:createWithTTF(name, "fonts/round_body.ttf", 20)
  lb2:setTextColor(tipColor)
  lb2:setAnchorPoint(0,0.5)
  table.insert(labelList, lb2)

  local lb3 =  cc.Label:createWithTTF(tipStr, "fonts/round_body.ttf", 20)
  lb3:setTextColor(tipColor)
  lb3:setAnchorPoint(0,0.5)
  table.insert(labelList, lb3)

  local lb4 =  cc.Label:createWithTTF(tipStr1, "fonts/round_body.ttf", 20)
  lb4:setTextColor(tipColor)
  lb4:setAnchorPoint(0,0.5)
  table.insert(labelList, lb4)

  local lb5 =  cc.Label:createWithTTF(tipStr2, "fonts/round_body.ttf", 20)
  lb5:setTextColor(tipColor)
  lb5:setAnchorPoint(0,0.5)
  table.insert(labelList, lb5)

  else

    local lb1 =  cc.Label:createWithTTF("恭喜你捕中了补给箱,获得", "fonts/round_body.ttf", 20)
    lb1:setTextColor(tipColor)
    lb1:setAnchorPoint(0,0.5)

    local lb1 =  cc.Label:createWithTTF(string.format("%d倍 %d分数 !", data.nFishMultiple,data.lFishScore), "fonts/round_body.ttf", 20)
    lb1:setTextColor(tipColor)
    lb1:setAnchorPoint(0,0.5)

    table.insert(labelList, lb1)
    table.insert(labelList, lb2)

  end



  local length = 60
  for i=1,#labelList do
    local lb = labelList[i]
    lb:setPosition(length - 30 , 20)
    length =  length + lb:getContentSize().width + 5 
  end

    -- local isHide = true;
    -- --内容不能为空
    -- for i=1,#labelList do
    --     local lbStr = labelList[i]:getString();
    --     if lbStr ~= "" then
    --         isHide = false;
    --     end
    -- end
    -- if isHide then
    --     showToast(self, "111111111111111111!", 2)
    --     error("")
    --     return;
    -- end

   local bg = ccui.ImageView:create("game_res/clew_box.png")
    bg:setScale9Enabled(true)
  
    bg:setContentSize(length,40)
    bg:setScale(0.1)

    for i=1,#labelList do
      local lb = labelList[i]
      bg:addChild(lb)
    end

    self:ShowTipsForBg(bg)
    labelList = {}
end


function GameViewLayer:ShowTipsForBg( bg )

  local infoCount = #self._scene.m_infoList
  local sublist = {}

  while infoCount >= 1 do

    local node = self._scene.m_infoList[1]
    table.remove(self._scene.m_infoList,1)
    node:removeFromParent()

    for i=1,#self._scene.m_infoList do
      local bg = self._scene.m_infoList[i]
      bg:runAction(cc.MoveBy:create(0.2,cc.p(0,60)))
    end

    infoCount = #self._scene.m_infoList
  end

  bg:setPosition(yl.WIDTH/2, yl.HEIGHT-120-60*infoCount)
  self:addChild(bg,30)
  table.insert(self._scene.m_infoList, bg)

  local call = cc.CallFunc:create(function()
    bg:removeFromParent()
    for i=1,#self._scene.m_infoList do
      local _bg = self._scene.m_infoList[i]
      if bg == _bg then
        table.remove(self._scene.m_infoList,i)
        break
      end
    end

    if #self._scene.m_infoList > 0 then
      for i=1,#self._scene.m_infoList do

       local _bg = self._scene.m_infoList[i]
          _bg:runAction(cc.MoveBy:create(0.2,cc.p(0,60)))

       end
    end

  end)

  bg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.17,1.0),cc.DelayTime:create(5),cc.ScaleTo:create(0.17,0.1,1.0),call)) 
end

return GameViewLayer
