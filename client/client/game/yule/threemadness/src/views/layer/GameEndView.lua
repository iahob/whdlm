local GameEndView =  class("GameEndView",function()
        local gameEndView =  display.newLayer(cc.c4b(0, 0, 0, 125))
    return gameEndView
end)

local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local cmd = appdf.req(appdf.GAME_SRC.."yule.threemadness.src.models.CMD_Game")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

GameEndView.BT_EXIT = 1
GameEndView.BT_GAME_CONTINUE = 2
GameEndView.BT_CHANGE_TABLE = 3
GameEndView.TAG_MASK = 4


function GameEndView:ctor(scene)
	local this = self
    self._scene = scene
	--按钮回调
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
			this:OnButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local rootLayer, csbNode = ExternalFun.loadRootCSB("result/ResultLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode

    self.m_UserHead = {}

    csbNode:getChildByName("Panel_1")
        :setTag(GameEndView.TAG_MASK)
        :addTouchEventListener( btcallback )

   

    local btnChangeChair = csbNode:getChildByName("Button_change")
    btnChangeChair:setTag(GameEndView.BT_CHANGE_TABLE)
    btnChangeChair:addTouchEventListener(btcallback)


    local btnReady = csbNode:getChildByName("Button_ready")
    btnReady:setTag(GameEndView.BT_GAME_CONTINUE)
    btnReady:addTouchEventListener(btcallback)
    btnReady:setEnabled( not self._scene._scene.m_bLookOn )

    local btnExit = csbNode:getChildByName("Button_exit")
    btnExit:setTag(GameEndView.BT_EXIT)
    btnExit:addTouchEventListener(btcallback)
    if PriRoom and GlobalUserItem.bPrivateRoom and GlobalUserItem.bPrivateGoldRoom then--退出 准备 
        btnChangeChair:setVisible(false)


        btnExit:setPositionX(466.90)
        btnReady:setPositionX(867.10)
    elseif PriRoom and GlobalUserItem.bPrivateRoom then --准备
        btnChangeChair:setVisible(false)
        btnExit:setVisible(false)

        btnReady:setPositionX(667)
    else--退出 准备 换桌

    end

    if PriRoom and GlobalUserItem.bPrivateRoom then
        btnExit:setEnabled(PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
    else
        btnExit:setEnabled(true)
    end



    
 
end

function GameEndView:OnButtonClickedEvent(tag,ref)
    
    if tag == GameEndView.BT_GAME_CONTINUE then
        -- if  self:getParent()._scene.m_bNoScore then
        --     local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
        --     local msg = self._scene._scene.m_szScoreMsg or "你的游戏币不足，无法继续游戏"
        --     local query = QueryDialog:create(msg, function(ok)
        --         if ok == true then
        --             self:getParent()._scene:onExitTable()
        --         end
        --     query = nil
        --     end, 32, QueryDialog.QUERY_SURE):setCanTouchOutside(false)
        --     :addTo(self:getParent()._scene)
        -- else
            --self:getParent()._scene:onStartGame(true)
            if PriRoom and GlobalUserItem.bPrivateRoom then
                local layerEnd = self:getParent():getChildByName("private_end_layer") 
                if layerEnd then
                    layerEnd:setVisible(true)
                elseif PriRoom:getInstance().m_tabPriData.dwPlayCount < PriRoom:getInstance().m_tabPriData.dwDrawCountLimit then
                    self:getParent()._scene:onStartGame(true)
                end
            else
                self:getParent()._scene:onStartGame(true)
            end

            self:getParent()._scene:exitResultShowBtn()
            self:removeFromParent()
        -- end
    elseif tag == GameEndView.BT_CHANGE_TABLE then
        self:getParent()._scene:onChangeDesk()
        self:removeFromParent()
    elseif tag == GameEndView.BT_EXIT then
        self:getParent()._scene:exitResultShowBtn()
        self:getParent()._scene:onQueryExitGame()
        self:removeFromParent()
    elseif tag == GameEndView.TAG_MASK then
        --self:getParent()._scene:exitResultShowBtn()
        --self:removeFromParent()
    end
    
    
end



function GameEndView:SetUserScore(viewid , score)
    local userNode = self.m_csbNode:getChildByName("Result_bg"):getChildByName("Node_user"..viewid)
    local winScore = userNode:getChildByName("Win_score")
    local loseScore = userNode:getChildByName("Lose_score")

    local scoreTxt = nil
    if score>0 then
        winScore:setVisible(true)
        loseScore:setVisible(false)
        scoreTxt = winScore:getChildByName("Score") 
    else
        winScore:setVisible(false)
        loseScore:setVisible(true)
        scoreTxt = loseScore
    end
    scoreTxt:setString("/".. math.abs(score) )

    if viewid == cmd.MY_VIEWID then
        if score>=0 then
            self.m_csbNode:getChildByName("Result_bg"):loadTexture(cmd.RES.."result/winBg.png")
        else
            self.m_csbNode:getChildByName("Result_bg"):loadTexture(cmd.RES.."result/loseBg.png")
        end
    end



  
end

function GameEndView:SetUserInfo(viewid,useritem)
    local userNode = self.m_csbNode:getChildByName("Result_bg"):getChildByName("Node_user"..viewid)
    userNode:setVisible(true)
    HeadSprite:createNormal(useritem, 72)
            :move(cc.p(0,203.16))
            :setLocalZOrder(1)
            :addTo(userNode)
    userNode:getChildByName("Head_bg"):setLocalZOrder(2)

    local name = userNode:getChildByName("NickName")
    if useritem and useritem.szNickName then
        name:setString(string.EllipsisByConfig(useritem.szNickName,125, self.m_config))
    else
        name:setString("游戏玩家")
    end
        
    userNode:getChildByName("Sprite_banker"):setVisible(self._scene._scene:SwitchViewChairID(self._scene._scene.m_wBankerUser) == viewid)
    userNode:getChildByName("Sprite_banker"):setLocalZOrder(2)
    
end

GameEndView.RES_CARD_TYPE = {"card_type_0.png","card_type_1.png","card_type_2.png","card_type_3.png","card_type_4.png","card_type_5.png"}

function GameEndView:SetUserCard(viewid,cardData,cardtype,isbreak)

    local userNode = self.m_csbNode:getChildByName("Result_bg"):getChildByName("Node_user"..viewid)
    local cardNode = userNode:getChildByName("Card_node")
    for i = 1, 3 do
        local spCard = cardNode:getChildByName("Card_"..i)
        if not cardData or not cardData[i] or cardData[i] == 0 or cardData[i] == 0xff  then
            spCard:setSpriteFrame(not isbreak and "card_back.png" or"card_break.png")
        else
            local strCard = string.format("card_player_%02d.png",cardData[i])
            spCard:setSpriteFrame(strCard)
        end
        spCard:setVisible(true)
    end

    local cardTypeTxt = cardNode:getChildByName("Card_type")
    if cardtype and cardtype >= 1 and cardtype <= 6 then 
        local sprite = display.newSprite("game_res/"..GameEndView.RES_CARD_TYPE[cardtype])
        cardTypeTxt:setSpriteFrame(sprite:getSpriteFrame())
        cardTypeTxt:setVisible(true)
    else
        cardTypeTxt:setVisible(false)
    end

end

function GameEndView:setUserItemPosX()
    local bg = self.m_csbNode:getChildByName("Result_bg")--:getChildByName("Node_user"..viewid)
    local beginPosX = 93.9
    for i=1 , 5 do
        local userNode = bg:getChildByName("Node_user"..i)
        if userNode and userNode:isVisible() then
            userNode:setPositionX(beginPosX)
            beginPosX = beginPosX + 187.8
        end
    end
end


function GameEndView:GetMyBoundingBox()
    return self._endViewBg:getBoundingBox()
end

return GameEndView