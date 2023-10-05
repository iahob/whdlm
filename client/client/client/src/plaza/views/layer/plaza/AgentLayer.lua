--代理界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- -- 推广界面(二级弹窗)
local AgentLayer = class("AgentLayer", TransitionLayer)
-- local AddAgentLayer = class("AddAgentLayer", TransitionLayer)

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            --关闭按钮
    "BTN_TYPE1",             --赚金说明
    "BTN_TYPE2",             --分享赚金
    "BTN_TYPE3",             --会员详情
    "BTN_TYPE4",             --我的奖励
    "BTN_WX1",
    "BTN_WX2",
    "BTN_SAVE",
    "BTN_COPY",
    "BTN_SHARE_WX",
    "BTN_SHARE_CL",
    "BTN_LINGJIANG",
    "BTN_ADD",
    "IMAGE_SAFE1",
    "IMAGE_SAFE2",
    "IMAGE_UPID",
    "IMAGE_TQ1",
    "IMAGE_TQ2",
    "IMAGE_TQ3",
    "IMAGE_XCXQ",
    "IMAGE_XCPLAYER",
    "IMAGE_ADDPLAYER1",
    "IMAGE_ADDPLAYER2",
    "IMAGE_ADDPLAYER3",  
    "BTN_SAFE",
    "BTN_BIND",
    "BTN_SHARE",
    "BTN_ZUAN",
    "BTN_GOLD",
    "BTN_TQ",
    "BTN_TQ_SEND",
    "BTN_TQXQ",
    "BTN_XQUP",
    "BTN_XQDOWN",
    "BTN_XQBACK",
    "BTN_CXXQ",
    "BTN_WANJIA",
    "BTN_DAILI",
    "BTN_PLAYERUP",
    "BTN_PLAYERDOWN",
    "BTN_ADDPLAYER",
    "BTN_CXPLAYER",
    "BTN_ADDSEND"
}

 local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)




-- 推广界面
function AgentLayer:ctor( scene, param, level )
 
    AgentLayer.super.ctor( self, scene, level )
    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("spread/AgentLayer.csb", self)
    self.m_rootLayer = rootLayer
    self.m_csbNode = csbNode
    self._scene = scene

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    local pagelistener = function (sender,eventType)
            self:onButtonClickedEvent(sender:getTag(),sender)
    end

    local btnClose = csbNode:getChildByName("btn_back")
    btnClose:setTag(TAG_ENUM.BTN_CLOSE)
    btnClose:addTouchEventListener( touchFunC )




    self.m_agentTypeBtn = {}
    local btn = csbNode:getChildByName("btn_select1")
    btn:setTag(TAG_ENUM.BTN_TYPE1)
    btn:addTouchEventListener(pagelistener)
    table.insert(self.m_agentTypeBtn,btn)
    
    btn = csbNode:getChildByName("btn_select2")
    btn:setTag(TAG_ENUM.BTN_TYPE2)
    btn:addTouchEventListener(pagelistener)
    table.insert(self.m_agentTypeBtn,btn)

    btn = csbNode:getChildByName("btn_select3")
    btn:setTag(TAG_ENUM.BTN_TYPE3)
    btn:addTouchEventListener(pagelistener)
    table.insert(self.m_agentTypeBtn,btn)

    btn = csbNode:getChildByName("btn_select4")
    btn:setTag(TAG_ENUM.BTN_TYPE4)
    btn:addTouchEventListener(pagelistener)
    table.insert(self.m_agentTypeBtn,btn)

    local id = csbNode:getChildByName("id")
    id:setString("ID:"..GlobalUserItem.tabAccountInfo.dwGameID)

    local time = csbNode:getChildByName("time1")
    time:setString(""..os.date("%x"))

    self.m_bAgent = false
    self.m_tichengPecrent = {}
    self.m_myAgentInfo ={}

    self.m_contentNode= {}

    for i =1,4 do
        self.m_contentNode[i] = csbNode:getChildByName("Panel_"..i)
    end

    -- 界面1
    -- local path = "ads/agentAd.png"
    --     if cc.FileUtils:getInstance():isFileExist(path) then
    --         self.m_contentNode[1]:getChildByName("Sprite_7"):setTexture(path)
    --     end



    --界面2-------------------------------------------------------

    local imgBg = csbNode:getChildByName("Image_12")

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 设置安全码1
    local editbox = ccui.EditBox:create(cc.size(302, 50),"public/public_blank.png")
            :setPosition(785,329)
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
            :setFont("fonts/FZHTJW.TTF", 1) 
            :setMaxLength(12)
            --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    imgBg:addChild(editbox)         
    self.m_safe1 = editbox
    self.m_safe1:registerScriptEditBoxHandler(editHanlder)

    self.m_safeTxt1= self.m_contentNode[2]:getChildByName("node2"):getChildByName("Text_25")
    self.m_safeTxt1:setString("")

    local image1 = self.m_contentNode[2]:getChildByName("node2"):getChildByName("Image_10")
    image1:setTag(TAG_ENUM.IMAGE_SAFE1)
    image1:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image1:setTouchEnabled(true)
    self.m_imageSafe1 = image1


    -- 设置安全码2
    local editbox = ccui.EditBox:create(cc.size(302, 50),"public/public_blank.png")
            :setPosition(793,258)
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
            :setFont("fonts/FZHTJW.TTF", 1) 
            :setMaxLength(12)
            --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    imgBg:addChild(editbox)         
    self.m_safe2 = editbox
    self.m_safe2:registerScriptEditBoxHandler(editHanlder)

    self.m_safeTxt2= self.m_contentNode[2]:getChildByName("node2"):getChildByName("Text_25_0")
    self.m_safeTxt2:setString("")

    local image2 = self.m_contentNode[2]:getChildByName("node2"):getChildByName("Image_10_0")
    image2:setTag(TAG_ENUM.IMAGE_SAFE2)
    image2:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image2:setTouchEnabled(true)
    self.m_imageSafe2 = image2

    local sureSafe= self.m_contentNode[2]:getChildByName("node2"):getChildByName("Button_9")
    sureSafe:setTag(TAG_ENUM.BTN_SAFE)
    sureSafe:addTouchEventListener( touchFunC )


    -- 上级ID
    local editbox = ccui.EditBox:create(cc.size(302, 50),"public/public_blank.png")
            :setPosition(745,57)
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
            :setFont("fonts/FZHTJW.TTF", 1) 
            :setMaxLength(6)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    imgBg:addChild(editbox)         
    self.m_upid = editbox
    self.m_upid:registerScriptEditBoxHandler(editHanlder)

    self.m_upidTxt= self.m_contentNode[2]:getChildByName("Text_22")
    self.m_upidTxt:setString("请输入玩家ID")

    local image3 = self.m_contentNode[2]:getChildByName("Image_9")
    image3:setTag(TAG_ENUM.IMAGE_UPID)
    image3:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image3:setTouchEnabled(true)
    self.m_imageUpid = image3

    local bindbtn= self.m_contentNode[2]:getChildByName("Button_7_0")
    bindbtn:setTag(TAG_ENUM.BTN_BIND)
    bindbtn:addTouchEventListener( touchFunC )

    self.m_szShareLink = GlobalUserItem.tabSpreadInfo["sharelink"] or yl.HTTP_URL

    local maBg = self.m_contentNode[2]:getChildByName("node1"):getChildByName("Image_6")
    local qr = QrNode:createQrNode(self.m_szShareLink, 180, 5, 1)
            qr:setPosition(maBg:getContentSize().width/2,maBg:getContentSize().height/2)
            maBg:addChild(qr)

    local lianjieTxt = self.m_contentNode[2]:getChildByName("node1"):getChildByName("Image_8"):getChildByName("Text_20")
    lianjieTxt:setString(self.m_szShareLink.."")

    local bindbtn= self.m_contentNode[2]:getChildByName("node1"):getChildByName("Button_7")
    bindbtn:setTag(TAG_ENUM.BTN_COPY)
    bindbtn:addTouchEventListener( touchFunC )
    bindbtn:setPressedActionEnabled(true)

    local bindbtn= self.m_contentNode[2]:getChildByName("node1"):getChildByName("Button_6")
    bindbtn:setTag(TAG_ENUM.BTN_SHARE)
    bindbtn:addTouchEventListener( touchFunC )
    bindbtn:setPressedActionEnabled(true)

    ---界面3
    self.m_curRewardType = 1 --1,钻石；2，金币
     local btn= self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Button_1")
    btn:setTag(TAG_ENUM.BTN_ZUAN)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Button_2")
    btn:setTag(TAG_ENUM.BTN_GOLD)
    btn:addTouchEventListener( touchFunC )

        local btn= self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Button_12")
    btn:setTag(TAG_ENUM.BTN_TQ)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Button_13")
    btn:setTag(TAG_ENUM.BTN_TQXQ)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Button_14")
    btn:setTag(TAG_ENUM.BTN_XQBACK)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Button_17")
    btn:setTag(TAG_ENUM.BTN_CXXQ)
    btn:addTouchEventListener( touchFunC )


    self.m_tqEdit ={}
    self.m_tqTxt ={}
    self.m_tqImg = {}
    local point ={cc.p(815,392),cc.p(815,295),cc.p(815,245)}
    for  i=1,3 do
        local editbox = ccui.EditBox:create(cc.size(135, 32),"public/public_blank.png")
                :setPosition(point[i])
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
                :setFont("fonts/FZHTJW.TTF", 1) 
                :setMaxLength(12)
                --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
                :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        imgBg:addChild(editbox)         
        self.m_tqEdit[i] = editbox
        self.m_tqEdit[i]:registerScriptEditBoxHandler(editHanlder)

        self.m_tqTxt[i]= csbNode:getChildByName("Panel_tq"):getChildByName("editNum"..i)
        self.m_tqTxt[i]:setString("")

        local image2 = csbNode:getChildByName("Panel_tq"):getChildByName("Image_id"..i)
        image2:setTag(TAG_ENUM.IMAGE_TQ1+i-1)
        image2:addTouchEventListener( touchFunC )
        --image:setLocalZOrder(2)
        image2:setTouchEnabled(true)
        self.m_tqImg[i] = image2
    end

    local Panel_tq = csbNode:getChildByName("Panel_tq")
        Panel_tq:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                   Panel_tq:setVisible(false)
        end
    end )

        local close = csbNode:getChildByName("Panel_tq"):getChildByName("Button_5")
        close:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                   Panel_tq:setVisible(false)
        end
    end )

    local btn= csbNode:getChildByName("Panel_tq"):getChildByName("Button_18")
    btn:setTag(TAG_ENUM.BTN_TQ_SEND)
    btn:addTouchEventListener( touchFunC )

    self.m_curPage = 1
    self.m_cxGameid = 0
    self.m_xqData = {}

    local btn= self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Button_15")
    btn:setTag(TAG_ENUM.BTN_XQUP)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Button_15_0")
    btn:setTag(TAG_ENUM.BTN_XQDOWN)
    btn:addTouchEventListener( touchFunC )    

    --查询
    local editbox = ccui.EditBox:create(cc.size(260, 50),"public/public_blank.png")
        :setPosition(cc.p(927,46))
        :setPlaceHolder("")
        :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
        :setFont("fonts/FZHTJW.TTF", 1) 
        :setMaxLength(6)
        --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    imgBg:addChild(editbox)         
    self.m_cxEdit = editbox
    self.m_cxEdit:registerScriptEditBoxHandler(editHanlder)

    self.m_cxTxt= self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Text_52")
    self.m_cxTxt:setString("请输入玩家ID")

    local image2 = self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Image_32")
    image2:setTag(TAG_ENUM.IMAGE_XCXQ)
    image2:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image2:setTouchEnabled(true)
    self.m_cxImg = image2

    --界面4
    self.m_curPlayerType = 1 --1,玩家；2，代理
    self.m_xqPlayerData = {}

     local btn= self.m_contentNode[4]:getChildByName("Button_1_0")
    btn:setTag(TAG_ENUM.BTN_WANJIA)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[4]:getChildByName("Button_2_0")
    btn:setTag(TAG_ENUM.BTN_DAILI)
    btn:addTouchEventListener( touchFunC )


    --查询
    local editbox = ccui.EditBox:create(cc.size(260, 50),"public/public_blank.png")
        :setPosition(cc.p(397,35))
        :setPlaceHolder("")
        :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
        :setFont("fonts/FZHTJW.TTF", 1) 
        :setMaxLength(6)
        --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    imgBg:addChild(editbox)         
    self.m_cx2Edit = editbox
    self.m_cx2Edit:registerScriptEditBoxHandler(editHanlder)

    self.m_cx2Txt= self.m_contentNode[4]:getChildByName("Text_80")
    self.m_cx2Txt:setString("请输入玩家ID")

    local image2 = self.m_contentNode[4]:getChildByName("Image_38")
    image2:setTag(TAG_ENUM.IMAGE_XCPLAYER)
    image2:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image2:setTouchEnabled(true)
    self.m_cx2Img = image2





        local btn= self.m_contentNode[4]:getChildByName("Button_up")
    btn:setTag(TAG_ENUM.BTN_PLAYERUP)
    btn:addTouchEventListener( touchFunC )

    local btn= self.m_contentNode[4]:getChildByName("Button_down")
    btn:setTag(TAG_ENUM.BTN_PLAYERDOWN)
    btn:addTouchEventListener( touchFunC ) 

    local btn= self.m_contentNode[4]:getChildByName("Button_24")
    btn:setTag(TAG_ENUM.BTN_ADDPLAYER)
    btn:addTouchEventListener( touchFunC ) 

    local btn= self.m_contentNode[4]:getChildByName("Button_23")
    btn:setTag(TAG_ENUM.BTN_CXPLAYER)
    btn:addTouchEventListener( touchFunC ) 

    local btn= csbNode:getChildByName("Panel_add"):getChildByName("Button_18")
    btn:setTag(TAG_ENUM.BTN_ADDSEND)
    btn:addTouchEventListener( touchFunC ) 

    self.m_playerEdit = {}
    self.m_playerTxt ={}
    self.m_playerImg = {}

    for i=1,3 do
        --添加代理
        local editbox = ccui.EditBox:create(cc.size(260, 50),"public/public_blank.png")
            :setPosition(cc.p(826,371))
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/FZHTJW.TTF", 1)
            :setFont("fonts/FZHTJW.TTF", 1) 
            --:setMaxLength(6)
            --:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            --:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)

            if i==1 then
                editbox:setMaxLength(6)
                editbox:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            elseif i==3 then
                editbox:setMaxLength(11)
                editbox:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            end

        imgBg:addChild(editbox)         
        self.m_playerEdit[i] = editbox
        self.m_playerEdit[i]:registerScriptEditBoxHandler(editHanlder)

        self.m_playerTxt[i]= csbNode:getChildByName("Panel_add"):getChildByName("id"..i)
        self.m_playerTxt[i]:setString("")

        local image2 = csbNode:getChildByName("Panel_add"):getChildByName("Image_id"..i)
        image2:setTag(TAG_ENUM.IMAGE_ADDPLAYER1+i-1)
        image2:addTouchEventListener( touchFunC )
        --image:setLocalZOrder(2)
        image2:setTouchEnabled(true)
        self.m_playerImg[i] = image2
    end


    local Panel_add = csbNode:getChildByName("Panel_add")
        Panel_add:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                   Panel_add:setVisible(false)
        end
    end )

    local close = csbNode:getChildByName("Panel_add"):getChildByName("Button_4")
        close:addTouchEventListener(function(ref, tType)
        if tType == ccui.TouchEventType.ended then
                   Panel_add:setVisible(false)
        end
    end )
    self._scene:showPopWait(nil,0,0.01,0)
    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(self)

end

function AgentLayer:updatePercentLayer()

   for k,v in pairs(self.m_tichengPecrent) do
        if v then
            local num = self.m_contentNode[1]:getChildByName("num"..v.AwardLevel.."_"..v.AwardType)
            if num then
                num:setString((v.AwardScale*100).."%")
            end
        end
   end
end

function AgentLayer:sendSafeNum()
    if self.m_safeTxt1:getString() == "" or self.m_safeTxt2:getString() == "" then
        showToast(self, "请输入安全码！", 2)
        return
    end 
    if self.m_safeTxt1:getString() ~= self.m_safeTxt2:getString() then
        showToast(self, "安全码设置不一致！", 2)
        return
    end

    local param = "action=setpassword&password="..md5(self.m_safeTxt1:getString()).."&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getreturnawardconfig===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_contentNode[2]:getChildByName("node2"):setVisible(false)
                    self.m_contentNode[2]:getChildByName("node1"):setVisible(true)
                    self.m_myAgentInfo["IsHasPassword"] = true
                end
                    showToast(self, msg, 2)
                
            end
        end
    end)

end


function AgentLayer:addUpAgent()
    if self.m_upidTxt:getString() == "请输入玩家ID"  or string.len(self.m_upidTxt:getString()) ~= 6 then
        showToast(self, "请输入正确的玩家ID！", 2)
        return
    end 
    local agentId = self.m_upidTxt:getString()

    local param = "action=bindagent&gameid="..self.m_upidTxt:getString().."&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "bindagent===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_contentNode[2]:getChildByName("Button_7_0"):setVisible(false)
                    self.m_contentNode[2]:getChildByName("Text_21"):setVisible(false)
                    self.m_imageUpid:setVisible(false)
                    self.m_upidTxt:setVisible(false)
                end
                    showToast(self, msg, 2)
                
            end
        end
    end)

end




function AgentLayer:getMyInfo()

    --返利比例
    local param = "action=getreturnawardconfig&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getreturnawardconfig===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_tichengPecrent = data["list"]
                    self:updatePercentLayer()
                else
                    showToast(self, msg, 2)
                end
            end
        end
    end)

    --个人信息
    local param = "action=getagentinfo&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getagentinfo===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_myAgentInfo = clone(data["info"])
                    self.m_myAgentInfo["DiamondSave"] =data["DiamondSave"] or 0
                    self.m_myAgentInfo["GoldSave"]=data["GoldSave"] or 0
                    self.m_bAgent = true
                    -- if tolua.cast(self._scene,"cc.Node")then
                        
                    -- end
                else
                    --showToast(self, msg, 2)
                    self.m_bAgent = false
                    self.m_myAgentInfo["ParentGameID"] = data["ParentGameID"]
                end
                self._scene:dismissPopWait()
            end
        end
    end)


end


function AgentLayer:onEditEvent(event, editbox)

    if editbox == self.m_safe1 then
        local src = editbox:getText() 
        if event == "return" then
            self.m_imageSafe1:setEnabled(true)
            self.m_safeTxt1:setString(src)
            if self.m_safeTxt1:getContentSize().width == 0 then --没有输入
                self.m_safeTxt1:setString("")
            end
        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "" then
                self.m_safeTxt1:setString(src)
            else
                self.m_safeTxt1:setString("")
            end

        elseif event == "began" then
            --self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "" then
                self.m_safeTxt1:setString(src)
            else
                self.m_safeTxt1:setString("")
            end
        end
    elseif editbox == self.m_safe2 then
        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_imageSafe2:setEnabled(true)

            self.m_safeTxt2:setString(src)
            --self.m_editSetName:setText("")
            if self.m_safeTxt2:getContentSize().width == 0 then --没有输入
                self.m_safeTxt2:setString("")
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "" then
                self.m_safeTxt2:setString(src)
            else
                self.m_safeTxt2:setString("")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "" then
                self.m_safeTxt2:setString(src)
            else
                self.m_safeTxt2:setString("")
            end
        end

    elseif editbox == self.m_upid then
        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_imageUpid:setEnabled(true)

            self.m_upidTxt:setString(src)
            --self.m_editSetName:setText("")
            if self.m_upidTxt:getContentSize().width == 0 then --没有输入
                self.m_upidTxt:setString("请输入玩家ID")
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_upidTxt:setString(src)
            else
                self.m_upidTxt:setString("请输入玩家ID")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_upidTxt:setString(src)
            else
                self.m_upidTxt:setString("请输入玩家ID")
            end
        end 
    elseif editbox == self.m_tqEdit[1] or editbox == self.m_tqEdit[2] or editbox == self.m_tqEdit[3] then
        local index = 1
        if editbox == self.m_tqEdit[1] then
            index = 1
        elseif editbox == self.m_tqEdit[2] then
            index = 2
        elseif editbox == self.m_tqEdit[3] then 
            index = 3
        end

        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_tqImg[index]:setEnabled(true)

            self.m_tqTxt[index]:setString(src)
            --self.m_editSetName:setText("")
            if self.m_tqTxt[index]:getContentSize().width == 0 then --没有输入
                self.m_tqTxt[index]:setString("")
            elseif string.len(src) == 6 and editbox == self.m_tqEdit[1] then

                    local param = "action=getnicknamebygameid&gameid=" ..src.."&".. GlobalUserItem:urlUserIdSignParam()
                    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
                            dump(jstable, "查询玩家", 7)
                            if type(jstable) == "table" then
                                local data = jstable["data"]
                                
                                if type(data) == "table" then
                                    if data["NickName"] then
                                        if self.m_csbNode and tolua.cast(self.m_csbNode,"cc.Node")then
                                            self.m_csbNode:getChildByName("Panel_tq"):getChildByName("name"):setString(data["NickName"])
                                        end
                                    else
                                        showToast(self, jstable["msg"], 2)
                                        if self.m_csbNode and tolua.cast(self.m_csbNode,"cc.Node")then
                                            self.m_csbNode:getChildByName("Panel_tq"):getChildByName("name"):setString("")
                                        end
                                    end
                                end
                            end
                        end)
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "" then
                self.m_tqTxt[index]:setString(src)
            else
                self.m_tqTxt[index]:setString("")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "" then
                self.m_tqTxt[index]:setString(src)
            else
                self.m_tqTxt[index]:setString("")
            end
        end
    elseif editbox == self.m_cxEdit then

        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_cxImg:setEnabled(true)

            self.m_cxTxt:setString(src)
            --self.m_editSetName:setText("")
            if self.m_cxTxt:getContentSize().width == 0 then --没有输入
                self.m_cxTxt:setString("请输入玩家ID")
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_cxTxt:setString(src)
            else
                self.m_cxTxt:setString("请输入玩家ID")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_cxTxt:setString(src)
            else
                self.m_cxTxt:setString("请输入玩家ID")
            end
        end 
    elseif editbox == self.m_cx2Edit then

        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_cx2Img:setEnabled(true)

            self.m_cx2Txt:setString(src)
            --self.m_editSetName:setText("")
            if self.m_cx2Txt:getContentSize().width == 0 then --没有输入
                self.m_cx2Txt:setString("请输入玩家ID")
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_cx2Txt:setString(src)
            else
                self.m_cx2Txt:setString("请输入玩家ID")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "请输入玩家ID" then
                self.m_cx2Txt:setString(src)
            else
                self.m_cx2Txt:setString("请输入玩家ID")
            end
        end 
    elseif editbox == self.m_playerEdit[1] or editbox == self.m_playerEdit[2] or editbox == self.m_playerEdit[3]  then
        local index = 1
        if editbox == self.m_playerEdit[1] then
            index = 1
        elseif editbox == self.m_playerEdit[2] then
            index = 2
        elseif editbox == self.m_playerEdit[3] then
            index = 3
        end

        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_playerImg[index]:setEnabled(true)

            self.m_playerTxt[index]:setString(src)
            --self.m_editSetName:setText("")
            if self.m_playerTxt[index]:getContentSize().width == 0 then --没有输入
                self.m_playerTxt[index]:setString("")
            elseif string.len(src) == 6 and editbox == self.m_playerEdit[1] then

                    local param = "action=getnicknamebygameid&gameid=" ..src.."&".. GlobalUserItem:urlUserIdSignParam()
                    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
                            dump(jstable, "查询玩家", 7)
                            if type(jstable) == "table" then
                                local data = jstable["data"]
                                
                                if type(data) == "table" then
                                    if data["NickName"] then
                                        if self.m_csbNode and tolua.cast(self.m_csbNode,"cc.Node")then
                                            self.m_csbNode:getChildByName("Panel_add"):getChildByName("name"):setString(data["NickName"])
                                        end
                                    else
                                        showToast(self, jstable["msg"], 2)
                                        if self.m_csbNode and tolua.cast(self.m_csbNode,"cc.Node")then
                                            self.m_csbNode:getChildByName("Panel_add"):getChildByName("name"):setString("")
                                        end
                                    end
                                end
                            end
                        end)
            end
        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "" then
                self.m_playerTxt[index]:setString(src)
            else
                self.m_playerTxt[index]:setString("")
            end
        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "" then
                self.m_playerTxt[index]:setString(src)
            else
                self.m_playerTxt[index]:setString("")
            end
        end

    end

end




-- 按键监听
function AgentLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self)
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        self.m_editBox:setVisible(false)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    elseif TAG_ENUM.BTN_TIPS == tag then
        self.m_layTips:setVisible(true)
    elseif TAG_ENUM.BTN_INVITE == tag then

    elseif TAG_ENUM.BTN_WX1 == tag then
        local szText = GlobalUserItem.tabServiceCache["Phone"]
        if type(szText) == "string" and szText ~= "" then
            MultiPlatform:getInstance():copyToClipboard(szText .. "")
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
                MultiPlatform:getInstance():openBrowser("weixin://")
            end)))
        end         
    elseif TAG_ENUM.BTN_WX2 == tag then
        local szText = GlobalUserItem.tabServiceCache["WeiXin"]
        if type(szText) == "string" and szText ~= "" then
            MultiPlatform:getInstance():copyToClipboard(szText .. "")
            self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
                MultiPlatform:getInstance():openBrowser("weixin://")
            end)))
        end 
    elseif TAG_ENUM.BTN_SHARE_WX == tag then 
           MultiPlatform:getInstance():shareToTarget(0, function(isok)
                
            end, yl.SocialShare.title,yl.SocialShare.content, self.m_szShareLink)
    elseif TAG_ENUM.BTN_SHARE_CL == tag then
            MultiPlatform:getInstance():shareToTarget(1, function(isok)
                
            end, yl.SocialShare.title,yl.SocialShare.content, self.m_szShareLink)

    elseif TAG_ENUM.BTN_COPY == tag then
        MultiPlatform:getInstance():copyToClipboard(self.m_szShareLink.."")
    elseif TAG_ENUM.BTN_SHARE == tag then
        
        -- local function sharecall( isok )
        --         if type(isok) == "string" and isok == "true" then
        --             self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
        --                                     cc.CallFunc:create(function()
        --                                         showToast(self, "分享成功", 2)
        --                                 end)))
        --         end
        --     end
        --     local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
        --     -- 截图分享
        --     local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
        --     local area = cc.rect(273, 127, 230, 230)
        --     local imagename = "grade_share.jpg"
        --     ExternalFun.popupTouchFilter(0, false)
        --     captureScreenWithArea(area, imagename, function(ok, savepath)
        --         ExternalFun.dismissTouchFilter()
        --         if ok then
        --             if nil ~= target then
        --                 MultiPlatform:getInstance():shareToTarget(0, sharecall, "", "", url, savepath, "true")
        --             end            
        --         end
        --     end)

        self._scene:popTargetShare(function(target, bMyFriend)
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0),
                                            cc.CallFunc:create(function()
                                                showToast(self, "分享成功", 2)
                                        end)))
                end
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            -- 截图分享
            local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
            local scalew, scaleh= framesize.width/1334,framesize.height/750
            local area = cc.rect(273*scalew, 127*scaleh, 230*scalew, 230*scaleh)--cc.rect(0, 0, framesize.width, framesize.height)
            local imagename = "grade_share.jpg"
            ExternalFun.popupTouchFilter(0, false)
            captureScreenWithArea(area, imagename, function(ok, savepath)
                ExternalFun.dismissTouchFilter()
                if ok then
                    if nil ~= target then
                        MultiPlatform:getInstance():shareToTarget(target, sharecall, "", "", url, savepath, "true")
                    end            
                end
            end)
        end, 3)

    elseif TAG_ENUM.BTN_ZUAN == tag then
        self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("img1"):setVisible(true)
        self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("img2"):setVisible(false)
        self.m_curRewardType = 1
        self:updateMyReward(1)
        

    elseif TAG_ENUM.BTN_GOLD == tag then   
        self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("img1"):setVisible(false)
        self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("img2"):setVisible(true) 
        self.m_curRewardType = 2
        self:updateMyReward(2)
    elseif TAG_ENUM.BTN_WANJIA == tag then  
        self.m_contentNode[4]:getChildByName("img1_0"):setVisible(true)
        self.m_contentNode[4]:getChildByName("img2_0"):setVisible(false) 
        self.m_curPlayerType = 1
        self.m_curPage = 1
        self.m_cxGameid = 0
        self:updateMyPlayer(0)
    elseif TAG_ENUM.BTN_DAILI == tag then  
        self.m_contentNode[4]:getChildByName("img1_0"):setVisible(false)
        self.m_contentNode[4]:getChildByName("img2_0"):setVisible(true) 
        self.m_curPlayerType = 2
        self.m_curPage = 1
        self.m_cxGameid = 0
        self:updateMyPlayer(0)

    elseif TAG_ENUM.BTN_TQ == tag then  
        self.m_csbNode:getChildByName("Panel_tq"):setVisible(true)
        self.m_csbNode:getChildByName("Panel_tq"):setEnabled(true)
        self:updateTiqu()
    elseif TAG_ENUM.BTN_TQ_SEND == tag then  
        self:sendReward()
    elseif TAG_ENUM.BTN_TQXQ == tag then  
        self.m_contentNode[3]:getChildByName("Node_1"):setVisible(false)
        self.m_contentNode[3]:getChildByName("Node_2"):setVisible(true)

        self.m_cxGameid = 0
        self.m_curPage = 1
        self:showXqList()
    elseif TAG_ENUM.BTN_XQUP == tag then  
        
        if self.m_curPage == 1 then
            return 
        end
        self:updateTqxq(-1)

    elseif TAG_ENUM.BTN_XQDOWN == tag then    

        self:updateTqxq(1)

    elseif TAG_ENUM.BTN_PLAYERUP == tag then  
        if self.m_curPage == 1 then
            return 
        end
        self:updatePlayerCell(-1)
    elseif TAG_ENUM.BTN_PLAYERDOWN == tag then  

        self:updateMyPlayer(1)
    elseif TAG_ENUM.BTN_ADDPLAYER == tag then

        self.m_csbNode:getChildByName("Panel_add"):setVisible(true)
        self.m_csbNode:getChildByName("Panel_add"):setEnabled(true)

    elseif TAG_ENUM.BTN_XQBACK == tag then 
        self.m_contentNode[3]:getChildByName("Node_1"):setVisible(true)
        self.m_contentNode[3]:getChildByName("Node_2"):setVisible(false)
    elseif TAG_ENUM.BTN_CXXQ == tag then 
        local gameId = self.m_cxTxt:getString()
        if string.len(gameId) ~= 6 then
            showToast(self, "请输入正确的玩家ID！", 2)
            return
        end
            self.m_curPage = 1
            self.m_cxGameid = gameId
            self:updateTqxq(0)
    elseif TAG_ENUM.BTN_CXPLAYER == tag then 
        local gameId = self.m_cx2Txt:getString()
        if string.len(gameId) ~= 6 then
            showToast(self, "请输入正确的玩家ID！", 2)
            return
        end
            self.m_curPage = 1
            self.m_cxGameid = gameId
            self:updateMyPlayer(0)

    elseif TAG_ENUM.BTN_ADDSEND == tag then 

            self:sendAddPlayer()    

    elseif TAG_ENUM.IMAGE_XCXQ == tag then 
            self.m_cxEdit:touchDownAction(self.m_cxEdit, ccui.TouchEventType.ended)
            self.m_cxImg:setEnabled(false)
    elseif TAG_ENUM.IMAGE_XCPLAYER == tag then 
            self.m_cx2Edit:touchDownAction(self.m_cx2Edit, ccui.TouchEventType.ended)
            self.m_cx2Img:setEnabled(false)    
    elseif TAG_ENUM.IMAGE_ADDPLAYER1 == tag  or TAG_ENUM.IMAGE_ADDPLAYER2 == tag or TAG_ENUM.IMAGE_ADDPLAYER3 == tag then 
            local index = tag - TAG_ENUM.IMAGE_ADDPLAYER1 +1

            self.m_playerEdit[index]:touchDownAction(self.m_playerEdit[index], ccui.TouchEventType.ended)
            self.m_playerImg[index]:setEnabled(false)  

    elseif TAG_ENUM.BTN_TYPE1 <= tag and TAG_ENUM.BTN_TYPE4 >= tag then

        if tag > TAG_ENUM.BTN_TYPE2 and not self.m_bAgent then
            showToast(self, "您还不是代理，请先成为代理！", 2)
            return 
        end
        for i=1,4 do
            self.m_agentTypeBtn[i]:getChildByName("Sprite_5"):setVisible(false)
            self.m_contentNode[i]:setVisible(false)
        end
        local curIndex = tag- TAG_ENUM.BTN_TYPE1 + 1
        self.m_agentTypeBtn[curIndex]:getChildByName("Sprite_5"):setVisible(true)
        self.m_contentNode[curIndex]:setVisible(true)
         if curIndex ==2 then
            if  self.m_myAgentInfo["IsHasPassword"] then
                self.m_contentNode[2]:getChildByName("node2"):setVisible(false)
                self.m_contentNode[2]:getChildByName("node1"):setVisible(true)
            else
                self.m_contentNode[2]:getChildByName("node2"):setVisible(true)
                self.m_contentNode[2]:getChildByName("node1"):setVisible(false)
            end

            if self.m_myAgentInfo["ParentGameID"] and string.len(self.m_myAgentInfo["ParentGameID"].."")==6 then
                self.m_contentNode[2]:getChildByName("Button_7_0"):setVisible(false)
                self.m_contentNode[2]:getChildByName("Text_21"):setVisible(false)
                self.m_imageUpid:setVisible(false)
                self.m_upidTxt:setVisible(false)
            end

        elseif curIndex ==3 then
            self.m_contentNode[3]:getChildByName("Node_1"):setVisible(true)
            self.m_contentNode[3]:getChildByName("Node_2"):setVisible(false)
            self:onButtonClickedEvent(TAG_ENUM.BTN_ZUAN)
        elseif curIndex ==4 then
            self:onButtonClickedEvent(TAG_ENUM.BTN_WANJIA)
         end
    elseif TAG_ENUM.BTN_SAVE == tag then
          

       elseif TAG_ENUM.BTN_LINGJIANG == tag then    
            self:getAgentReward()
       elseif TAG_ENUM.BTN_ADD == tag then   
            local addL = AddAgentLayer:create()
            self:addChild(addL)
        elseif TAG_ENUM.IMAGE_SAFE1 == tag then  
            self.m_safe1:touchDownAction(self.m_safe1, ccui.TouchEventType.ended)
            self.m_imageSafe1:setEnabled(false)
        elseif TAG_ENUM.IMAGE_SAFE2 == tag then  
            self.m_safe2:touchDownAction(self.m_safe2, ccui.TouchEventType.ended)
            self.m_imageSafe2:setEnabled(false)
        elseif TAG_ENUM.IMAGE_UPID == tag then  
            self.m_upid:touchDownAction(self.m_upid, ccui.TouchEventType.ended)
            self.m_imageUpid:setEnabled(false)   
        elseif TAG_ENUM.IMAGE_TQ1 <= tag and TAG_ENUM.IMAGE_TQ3 >= tag then  
            local index = tag - TAG_ENUM.IMAGE_TQ1 + 1
            self.m_tqEdit[index]:touchDownAction(self.m_tqEdit[index], ccui.TouchEventType.ended)
            self.m_tqImg[index]:setEnabled(false)

        elseif TAG_ENUM.BTN_SAFE == tag then 
            self:sendSafeNum()
        elseif TAG_ENUM.BTN_BIND == tag then 
            self:addUpAgent()
        end
end



function AgentLayer:myAgentUpdate()

    if not self.m_myAgentInfo  then
        return
    end

    if self.m_myAgentInfo["AgentID"] ~= 0 then
        local node = self.m_contentNode[4]:getChildByName("Node_2"):setVisible(true)
        if not self.m_myAgentReward  then
            return 
        end

        node:getChildByName("data1"):setString(self.m_myAgentReward["BelowUser"].."")
        node:getChildByName("data2"):setString(self.m_myAgentReward["BelowAgent"].."")
        node:getChildByName("data3"):setString(NumberHundred(self.m_myAgentReward["LastDayAward"]))
        node:getChildByName("data4"):setString(NumberHundred(self.m_myAgentReward["TotalAward"]))
        node:getChildByName("data5"):setString(NumberHundred(self.m_myAgentReward["CanReceiveGold"]))
    else
        self.m_contentNode[4]:getChildByName("Node_1"):setVisible(true)
    end
end

function AgentLayer:onTransitionInFinish()

    self:getMyInfo()



   --  --获取代理数据
   --  self:onButtonClickedEvent(TAG_ENUM.BTN_TYPE1)
end


function AgentLayer:updateMyReward(index)
        --个人信息
    local param = "action=getawardinfo&" .. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getawardinfo===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_myRewardInfo = clone(data["info"])

                    self:showMyReward(index)
                else
                    showToast(self, msg, 2)
                end
            end
        end
    end)
end

function AgentLayer:showMyReward(index)

    if not self.m_myRewardInfo then
        return 
    end
    local curData ={}
    if index ==1 then
        curData = clone(self.m_myRewardInfo["Diamond"])
    else
        curData = clone(self.m_myRewardInfo["Gold"])
    end
    self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Text_36"):setString(curData["TotalAward"].."")
    self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Text_36_0"):setString(curData["MonthAward"].."")
    self.m_contentNode[3]:getChildByName("Node_1"):getChildByName("Text_36_1"):setString((index ==1 and self.m_myAgentInfo["DiamondAward"] or self.m_myAgentInfo["GoldAward"]).."")


end

function AgentLayer:updateTiqu()
    local layer = self.m_csbNode:getChildByName("Panel_tq") 
    for i=1,3 do
        self.m_tqEdit[i]:setText("")
        self.m_tqTxt[i]:setString("")
    end
            

    if self.m_curRewardType ==1 then
        layer:getChildByName("num1"):setString(self.m_myAgentInfo["DiamondAward"].."")
        layer:getChildByName("Text_60"):setString("可提钻石：")
        layer:getChildByName("Text_60_2"):setString("提取游戏币：")
        layer:getChildByName("Text_60_3_0"):setString("注：提取钻石需要预留"..self.m_myAgentInfo["DiamondSave"].."钻石")
    else
        layer:getChildByName("num1"):setString(self.m_myAgentInfo["GoldAward"].."")
        layer:getChildByName("Text_60"):setString("可提游戏币：")
        layer:getChildByName("Text_60_2"):setString("提取游戏币：")
        layer:getChildByName("Text_60_3_0"):setString("注：提取游戏币需要预留"..self.m_myAgentInfo["GoldSave"].."游戏币")
    end
    


end

function AgentLayer:updateTqxq(page)
    local gameid= self.m_cxGameid ~= 0  and "&query="..gameId or ""

    local param = "action=getrecord&type="..((self.m_curRewardType ==1 ) and "pay" or "revenue").."&pagesize=8&pageindex="..(self.m_curPage+page)..gameid.."&".. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getawardinfo===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_curPage = data["pageCount"]
                    self.m_xqData = clone(data["record"])
                    self:updateCell()
                else
                    showToast(self, msg, 2)
                end
            end
        end
    end)

end

function AgentLayer:updateMyPlayer(page)

    local playerNum = self.m_contentNode[4]:getChildByName("Text_83")
    playerNum:setString((self.m_curPlayerType == 1 and self.m_myAgentInfo["BelowAgentsUser"] or self.m_myAgentInfo["BelowAgentsUser"]).."" )
    
    local gameid= self.m_cxGameid ~= 0 and "&query="..self.m_cxGameid or ""

    local param = "action=getbelowlist&type="..((self.m_curPlayerType ==1 ) and "user" or "agent").."&pagesize=8&pageindex="..(self.m_curPage +page)..gameid.."&".. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "getawardinfo===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_curPage = data["pageCount"]
                    self.m_xqPlayerData = clone(data["list"])
                    self:updatePlayerCell()
                else
                    showToast(self, msg, 2)
                end
            end
        end
    end)

end

function AgentLayer:updatePlayerCell()
        local node = self.m_contentNode[4]:getChildByName("Node_3")
        node:removeAllChildren()
        for i=1,8 do
            local data = self.m_xqPlayerData[i]
             if data then
                local csb = ExternalFun.loadCSB("spread/yonghuNode.csb", node)
                csb:setPosition(0,48*(8-i))

                csb:getChildByName("id"):setString(data["GameID"].."")
                csb:getChildByName("time"):setString(data["RegisterDate"].."")
                csb:getChildByName("name"):setString(data["NickName"].."")
                csb:getChildByName("shuishou"):setString(data["TotalGold"].."")
                csb:getChildByName("shuishou_0"):setString(data["Diamond"].."")
            end
        end
end

function AgentLayer:sendAddPlayer()

    local gameId = self.m_playerTxt[1]:getString()
    local realName = self.m_playerTxt[2]:getString()
    local phoneNum = self.m_playerTxt[3]:getString()

    if string.len(gameId) ~= 6 then
        showToast(self, "请输入正确的玩家ID！", 2)
        return
    end 

    if string.len(realName) == 0 or realName == "" then
        showToast(self, "请输入玩家姓名！", 2)
        return
    end 

    if string.len(phoneNum) ~= 11 then
        showToast(self, "请输入正确的电话号码！", 2)
        return
    end 


    local param = "action=addagent&gameid="..gameId.."&compellation="..realName.."&phone="..phoneNum.."&".. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "bindagent===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_csbNode:getChildByName("Panel_add"):setVisible(false)
                end
                    showToast(self, msg, 2)
                
            end
        end
    end)

end


function AgentLayer:updateCell()
        local node = self.m_contentNode[3]:getChildByName("Node_2"):getChildByName("Node_5")
        node:removeAllChildren()
        for i=1,8 do
            local data = self.m_xqData[i]
             if data then
                local csb = ExternalFun.loadCSB("spread/tichengNode.csb", node)
                csb:setPosition(0,46*(8-i))

                csb:getChildByName("time"):setString(data["CollectDate"].."")
                csb:getChildByName("id"):setString(data["GameID"].."")
                csb:getChildByName("name"):setString(data["NickName"].."")
                csb:getChildByName("shuishou"):setString(data["AwardScale"].."")
                csb:getChildByName("ticheng"):setString(data["Award"].."")
            end
        end

end

function AgentLayer:showXqList()
        local list = self.m_contentNode[3]:getChildByName("Node_2")
        list:getChildByName("Text_41"):setString(self.m_curRewardType ==1 and "钻石提成" or "游戏币提成")

        list:getChildByName("Text_46"):setString(self.m_curRewardType ==1 and self.m_myRewardInfo["Diamond"]["TotalReceive"].."" or self.m_myRewardInfo["Gold"]["TotalReceive"])
        list:getChildByName("Text_43"):setString(self.m_curRewardType ==1 and self.m_myAgentInfo["DiamondAward"].."" or self.m_myAgentInfo["GoldAward"])

        self:updateTqxq(0)


end


function AgentLayer:sendReward()
    
    if self.m_tqTxt[1]:getString() == ""  or self.m_tqTxt[2]:getString() == ""  or self.m_tqTxt[3]:getString() == "" then
        showToast(self, "请输入正确的提取信息！", 2)
        return
    end 

 
    local gameId = self.m_tqTxt[1]:getString()
    local num = self.m_tqTxt[2]:getString()
    local safe = md5(self.m_tqTxt[3]:getString())


    local param = "action=presentscore&gameid="..gameId.."&password=" ..safe.."&count="..num.."&type="..self.m_curRewardType.."&".. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        dump(jstable, "bindagent===========", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    self.m_csbNode:getChildByName("Panel_tq"):setVisible(false)
                end
                    showToast(self, msg, 2)
                
            end
        end
    end)

end


function AgentLayer:onTransitionOutFinish()
    self:removeFromParent()
end

-- function AgentLayer:onOtherShowEvent()
--     if self:isVisible() then
--         self:setVisible(false)
--     end
-- end

-- function AgentLayer:onOtherHideEvent()
--     if not self:isVisible() then
--         self:setVisible(true) 
--     end
-- end

function AgentLayer:onExit()
 
    AgentLayer.super.onExit(self)
end





return AgentLayer