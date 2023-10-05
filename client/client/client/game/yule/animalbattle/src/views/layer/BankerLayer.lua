
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.animalbattle.src"
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

local BankerLayer=class("BankerLayer",function ( ... )
    -- body
    return cc.Layer:create()
end)

function BankerLayer:ctor(scene)

    self._scene = scene
    self.csbNode=ExternalFun.loadCSB("BankerList/Layer.csb",self)

    self.csbNode = self.csbNode:getChildByName("bg")
    local view = self.csbNode:getChildByName("view")
    local back = self.csbNode:getChildByName("back")

    self.button_sqsz = self.csbNode:getChildByName("button_s_q_s_z")
    self.button_sqxz = self.csbNode:getChildByName("button_s_q_x_z")
    self.button_qxsq = self.csbNode:getChildByName("button_q_x_s_q")

    self.userList = cc.TableView:create(cc.size(view:getContentSize().width, view:getContentSize().height))
    self.userList:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL) 
    self.userList:setAnchorPoint(cc.p(0,0))
    self.userList:setPosition(cc.p(0,0))
    self.userList:setDelegate()
    self.userList:addTo(view)
    self.userList:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self.userList:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self.userList:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
    self.userList:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self.userList:reloadData()

    back:addClickEventListener(handler(self,self.onBack))

    self.button_sqsz:addClickEventListener(handler(self,self.onButtonSqsz))
    self.button_sqxz:addClickEventListener(handler(self,self.onButtonSqxz))
    self.button_qxsq:addClickEventListener(handler(self,self.onButtonQxsq))

    self.button_sqsz:setVisible(true)
    self.button_sqxz:setVisible(false)
    self.button_qxsq:setVisible(false)
    -- local useritem = nil
    
    for k,v in pairs(self._scene:getUserList()) do
        if v.wChairID == self._scene.m_wBankerUser then
            self:updateApplyBanker(v,self._scene.m_wBankerUser)
        end
    end

    for k,v in pairs(self._scene.m_nApplyBanker) do
        if v == self._scene:GetMeChairID() then 
            self.button_sqsz:setVisible(false)
            self.button_sqxz:setVisible(false)
            self.button_qxsq:setVisible(true)   
        end
    end

    print("self._scene.m_wBankerUser ",self._scene.m_wBankerUser )
    print("self._scene.bEnableSysBanker ",self._scene.bEnableSysBanker )
    -- if self._scene.bEnableSysBanker and wChairID == 65535 then -- 没有庄家
    --     self.button_sqsz:setVisible(true)
    --     self.button_sqxz:setVisible(false)
    -- end
    --[[--]]
end

function BankerLayer:updateApplyBanker( useritem ,wChairID)
    -- asset(false)
    if useritem == nil and wChairID == 65535 then -- 系统坐庄时
        self.button_sqsz:setVisible(true)
        self.button_sqxz:setVisible(false)
        self.button_qxsq:setVisible(false)
        for k,v in pairs(self._scene.m_nApplyBanker) do
            if v == self._scene:GetMeChairID() then 
                self.button_sqsz:setVisible(false)
                self.button_sqxz:setVisible(false)
                self.button_qxsq:setVisible(true)   
            end
        end
        self.userList:reloadData()
        return
    end


    if not self._scene.bEnableSysBanker and wChairID == 65535 then -- 没有庄家
        self.button_sqsz:setVisible(true)
        self.button_sqxz:setVisible(false)
        self.button_qxsq:setVisible(false)
        -- return
    end

    if useritem.dwUserID == self._scene:GetMeUserItem().dwUserID  then 
        self.button_sqsz:setVisible(false)
        self.button_sqxz:setVisible(true) 
        self.button_qxsq:setVisible(false)
        -- self:updateXzBtnRes("BankerList/image3.png","BankerList/image4.png",true)
    else
        self.button_sqsz:setVisible(true)
        self.button_sqxz:setVisible(false)
        self.button_qxsq:setVisible(false)
    end
    dump(self._scene.m_nApplyBanker , "等待庄家列表+"..self._scene:GetMeChairID())
    
    for k,v in pairs(self._scene.m_nApplyBanker) do
        if v == self._scene:GetMeChairID() then 
            self.button_sqsz:setVisible(false)
            self.button_sqxz:setVisible(false)
            self.button_qxsq:setVisible(true)   
        end
    end

    self.userList:reloadData()
    -- self:enable_NoteNum_Clear_ContinueBtn()
end

function BankerLayer:applyBankerReload( )
    self.userList:reloadData()
end

function BankerLayer:updateXzBtnRes(res,normal,isShow)

    self.button_sqsz:setVisible(not isShow)
    self.button_sqxz:setVisible(isShow)

    self.button_sqxz:loadTextures(res,0)
    self.button_sqxz:loadTextureNormal(normal,0)
    self.button_sqxz:loadTexturePressed(res,0)
end


function BankerLayer:onBack()
    self:removeFromParent()
end

function BankerLayer:onButtonSqsz()
    self._scene:sendApplyBanker(true)
end

function BankerLayer:onButtonSqxz()
    self._scene:sendApplyBanker(false)
end

function BankerLayer:onButtonQxsq()
    self._scene:sendApplyBanker(false)
end

function BankerLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()

    if nil == cell then
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local wChairID = self._scene.m_nApplyBanker[idx+1]
    if wChairID then
        for k,v in pairs(self._scene:getUserList()) do
            if wChairID == v.wChairID then 
                local csbNode = cc.CSLoader:createNode("BankerList/Node.csb")
                cell:addChild(csbNode)
                csbNode:setPosition(700/2+8,89/2)

                local head = csbNode:getChildByName("head")
                local av = PopupInfoHead:createClipHead( v, 95, "BankerList/image15.png" )
                head:addChild(av)

                local nameStr = string.EllipsisByConfig(v.szNickName,180,string.getConfig("fonts/FZHTJW.ttf" , 20))
                local name = csbNode:getChildByName("name")
                name:setString(nameStr)

                local font = csbNode:getChildByName("font")
                local gold = csbNode:getChildByName("gold")

                local lScore = v.lScore
                if v.lScore > 100000000 then 
                    lScore =  string.format("%.2f", lScore/100000000)
                    font:setTexture("BankerList/image9.png")
                elseif v.lScore > 10000 then 
                    lScore =  string.format("%.2f", lScore/10000)
                    font:setTexture("BankerList/image8.png")
                else
                    font:setVisible(false)
                end

                gold:setString(lScore)
                print("gold:getContentSize().width ",gold:getContentSize().width)
                print("gold:getPositionX() ",gold:getPositionX())
                font:setPositionX(gold:getPositionX()+gold:getContentSize().width)
            end
        end


        -- av:setPosition(head:getContentSize().width/2,head:getContentSize().height/2)


    end

    return cell
end

function BankerLayer:tableCellTouched(view, cell)

end

function BankerLayer.cellSizeForTable()
    return 700, 93
end

function BankerLayer:numberOfCellsInTableView()
        return #self._scene.m_nApplyBanker 
end
return BankerLayer