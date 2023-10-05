
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")
local ClubMemberSet = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubMemberSet")
local ClubShareLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubShareLayer")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

-- 用户中心(二级弹窗)
local ClubSpreadLayer = class("ClubSpreadLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_SHARE",            -- 分享
    "BTN_SAVE",             -- 保存
    "BTN_EXIT",             -- 退出大联盟
    "BTN_WX",               --微信
    "BTN_WXC",              --朋友圈
    "BTN_SURE",             --确定
    "BTN_COPY1",             --复制自己邀请码
    "BTN_COPY2",             --复制上级邀请码
    "BTN_COPY3",             --复制连接

    "BTN_CSHARE",        --复制分享
    "BTN_SAVE",
    "BTN_EDIT1",
    "BTN_EDIT2",
    "BTN_EDIT3",
    "BTN_EDIT4",

}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubSpreadLayer:ctor( scene, param, level ) --param 传房间信息
    local this = self

    ClubSpreadLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_share1.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.m_tipPanel = csbNode:getChildByName("Panel_6")

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    --spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    self.m_nCurrentClubInfo = GlobalUserItem.tabCurGroupInfo



    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("Button_share")
    btn:setTag(TAG_ENUM.BTN_CSHARE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    local btn = spbg:getChildByName("Image_ma")
     btn:setTouchEnabled(true)
     btn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            btn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(TAG_ENUM.BTN_SAVE,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            btn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then

        end
    end)

    local btn = spbg:getChildByName("myCode"):getChildByName("Button_copy")
    btn:setTag(TAG_ENUM.BTN_COPY1)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("upNode"):getChildByName("Button_copy")
    btn:setTag(TAG_ENUM.BTN_COPY2)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("url"):getChildByName("Image_14"):getChildByName("Button_copy")
    btn:setTag(TAG_ENUM.BTN_COPY3)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn = spbg:getChildByName("bind"):getChildByName("Button_bind")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_editImg = {}
    self.m_editTxt = {}
    self.m_editBox = {}

    --搜索输入框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    local pointEdit = {cc.p(303,550),cc.p(603,550),cc.p(895,550),cc.p(727,332)}
    self.m_editTxt[1] = spbg:getChildByName("Image_wx"):getChildByName("Text_1")
    self.m_editTxt[2] = spbg:getChildByName("Image_qq"):getChildByName("Text_1")
    self.m_editTxt[3] = spbg:getChildByName("Image_phone"):getChildByName("Text_1")
    self.m_editTxt[4] = spbg:getChildByName("bind"):getChildByName("txt")

    self.m_editImg[1] = spbg:getChildByName("Image_wx")
    self.m_editImg[2] = spbg:getChildByName("Image_qq")
    self.m_editImg[3] = spbg:getChildByName("Image_phone")
    self.m_editImg[4] = spbg:getChildByName("bind"):getChildByName("Image_15")

    self.m_txtTip ={"点击输入微信号","点击输入QQ号","点击输入手机号","点击输入邀请码"}

    for i=1,4 do
        local editbox = ccui.EditBox:create(cc.size(282, 52),"public/public_blank.png")
                :setPosition(pointEdit[i])
                --:setFontColor(cc.c3b(255, 237, 38))
                :setAnchorPoint(cc.p(0,0.5))  
                :setPlaceHolder("")
                :setPlaceholderFont("fonts/round_body.ttf", 25)
                :setFont("fonts/round_body.ttf", 25) 
                :setTag(i)

        mask:addChild(editbox)         
        self.m_editBox[i] = editbox
        self.m_editBox[i]:registerScriptEditBoxHandler(editHanlder)

        if i ~= 1  then
                editbox:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
                editbox:setMaxLength(11)
        else
                editbox:setMaxLength(15)
        end

        self.m_editTxt[i]:setString(self.m_txtTip[i])

        self.m_editImg[i]:setTag(TAG_ENUM.BTN_EDIT1 + i-1)
        self.m_editImg[i]:addTouchEventListener( touchFunC )
        --self.m_editImg:setTouchEnabled(false)
   end

   if self.m_myClubInfo.MemberInfo.szWX ~= "" then
        self.m_editTxt[1]:setString(self.m_myClubInfo.MemberInfo.szWX.."")
        self.m_editBox[1]:setText(self.m_myClubInfo.MemberInfo.szWX.."")
   end

    if self.m_myClubInfo.MemberInfo.szQQ ~= "" then
        self.m_editTxt[2]:setString(self.m_myClubInfo.MemberInfo.szQQ.."")
        self.m_editBox[2]:setText(self.m_myClubInfo.MemberInfo.szQQ.."")
   end

    if self.m_myClubInfo.MemberInfo.szPhone ~= "" then
        self.m_editTxt[3]:setString(self.m_myClubInfo.MemberInfo.szPhone.."")
        self.m_editBox[3]:setText(self.m_myClubInfo.MemberInfo.szPhone.."")
   end

    self.m_shareLink = spbg:getChildByName("url"):getChildByName("Image_14"):getChildByName("Panel_2"):getChildByName("txt")
    self.m_shareLink:setString("")

     self.upCode = self.m_spBg:getChildByName("upNode"):getChildByName("txt_ma")

    if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID  or  self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MANAGER  then

        local clubtxt = spbg:getChildByName("clubId")
        clubtxt:setVisible(true)
        clubtxt:getChildByName("id"):setString(ClubFrame:getInstance().m_nCurrentClub.."")

        spbg:getChildByName("url"):setPosition(426,298)


    elseif self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MEMBER then 

        local clubtxt = spbg:getChildByName("clubId")
        clubtxt:setVisible(true)
        clubtxt:getChildByName("id"):setString(ClubFrame:getInstance().m_nCurrentClub.."")

        spbg:getChildByName("url"):setPosition(425,239)

        spbg:getChildByName("bind"):setVisible(true)


    elseif self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_TEAMMEMBER  or self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then 
 
        spbg:getChildByName("upNode"):setVisible(true)
        spbg:getChildByName("url"):setPosition(425,177)

        spbg:getChildByName("url"):getChildByName("Image_14"):setPosition(130, -7)

        self:updateMyUpData()

        if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then

            spbg:getChildByName("myCode"):setVisible(true)
            spbg:getChildByName("Image_wx"):setPosition(91.5,567.5)
            spbg:getChildByName("Image_qq"):setPosition(91.5,503)
            spbg:getChildByName("Image_phone"):setPosition(483,567)

            

            self.m_editBox[1]:setPosition(351, 583)
            self.m_editBox[2]:setPosition(354, 518)
            self.m_editBox[3]:setPosition(737, 583)

            self.myCode = spbg:getChildByName("myCode"):getChildByName("code")

        end

    end


    ClubFrame:getInstance():setViewFrame(self)
    ClubFrame:getInstance():sendQueryCode(ClubFrame:getInstance().m_nCurrentClub)


    --print("url==="..self.m_shareLink:getString())


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
end

function ClubSpreadLayer:onExit()

    ClubSpreadLayer.super.onExit(self)
end

-- 按键监听
function ClubSpreadLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then

        self:removeFromParent()

    elseif TAG_ENUM.BTN_WX == tag or TAG_ENUM.BTN_WXC == tag then
        local index = TAG_ENUM.BTN_WX == tag and 0 or 1

        MultiPlatform:getInstance():shareToTarget(index, function(isok)

            end, self.m_param.title,self.m_param.content, self.m_szShareLink)
    elseif tag >= TAG_ENUM.BTN_EDIT1 and tag <= TAG_ENUM.BTN_EDIT4 then
        local index = tag -  TAG_ENUM.BTN_EDIT1 +1

        self.m_editBox[index]:touchDownAction(self.m_editBox[index], ccui.TouchEventType.ended)
        self.m_editImg[index]:setEnabled(false)

    elseif tag == TAG_ENUM.BTN_COPY1  then
        MultiPlatform:getInstance():copyToClipboard(self.myCode:getString())

    elseif tag == TAG_ENUM.BTN_COPY2  then
        MultiPlatform:getInstance():copyToClipboard(self.upCode:getString())

    elseif tag == TAG_ENUM.BTN_COPY3  then
        MultiPlatform:getInstance():copyToClipboard(self.m_shareLink:getString())

    elseif tag == TAG_ENUM.BTN_CSHARE  then
        --MultiPlatform:getInstance():copyToClipboard(self.m_shareLink:getString())
        local param ={}
        if self.m_myClubInfo.MemberInfo.nMemberRight >= cmd_Club.AUTHORITY_CAPTAIN then
            param.title = "【大联盟邀请码:"..self.m_myClubInfo.MemberInfo.lInvitationCode.."】"
        else
            param.title = "【大联盟ID:"..ClubFrame:getInstance().m_nCurrentClub.."】"
        end
        param.content = "我在大联盟游戏，等你来参加！"
        param.url = self.m_shareLink:getString()
        local pa = ClubShareLayer:create(self._scene, param, ClubShareLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pa)  

    elseif tag == TAG_ENUM.BTN_SAVE  then      
                local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
                local xRate = framesize.width / yl.WIDTH 
                local yRate = framesize.height / yl.HEIGHT

                local screenPos = self.m_spBg:convertToWorldSpace(cc.p(190, 290))
                local framePosX = (screenPos.x - 95) * xRate
                local framePosY = (screenPos.y - 95) * yRate
                local frameWidth = 190 * xRate
                local frameHeight = 190 * yRate
                local qrFrameRect = cc.rect(framePosX, framePosY, frameWidth, frameHeight)

                captureScreenWithArea(qrFrameRect, "qr_code.png", function(ok, savepath)
                    if ok then
                        if MultiPlatform:getInstance():saveImgToSystemGallery(savepath, "qr_code.png") then
                            
                            if self.m_tipPanel and tolua.cast(self.m_tipPanel, "cc.Node") then
                                showToast(self.m_tipPanel, "二维码图片已保存至系统相册", 1)
                            end
                        end
                    end
                end)
    elseif tag == TAG_ENUM.BTN_SURE  then  
                local code = self.m_editTxt[4]:getString()
                if string.len(code) ~= 8 then
                    showToast(self, "输入的邀请码不正确！", 2)
                end
                ClubFrame:getInstance():setViewFrame(self)
                ClubFrame:getInstance():sendBindInvitationCode(ClubFrame:getInstance().m_nCurrentClub, tonumber(code))
    end

end

function ClubSpreadLayer:onEditEvent(event, editbox)

      local index = editbox:getTag()
      local text = self.m_editTxt[index]
      local img = self.m_editImg[index]

        local src = editbox:getText() 

        if event == "return" then
            img:setEnabled(true)
            text:setString(src)
            if text:getContentSize().width == 0 then --没有输入
                text:setString(self.m_txtTip[index])
            else
                if index == 1 and self.m_myClubInfo.MemberInfo.szWX ~= src then
                    self.m_myClubInfo.MemberInfo.szWX = src
                    ClubFrame:getInstance():sendModifyInfo(ClubFrame:getInstance().m_nCurrentClub,2,src)
                elseif index == 2 and self.m_myClubInfo.MemberInfo.szQQ ~= src then
                    self.m_myClubInfo.MemberInfo.szWX = src
                    ClubFrame:getInstance():sendModifyInfo(ClubFrame:getInstance().m_nCurrentClub,1,src)
                elseif index == 3 and self.m_myClubInfo.MemberInfo.szPhone ~= src  then 
                    if string.len(src) ~= 11 then
                        showToast(self, "输入的电话号码不正确！", 2)
                        return
                    end
                    self.m_myClubInfo.MemberInfo.szPhone = src
                    ClubFrame:getInstance():sendModifyInfo(ClubFrame:getInstance().m_nCurrentClub,3,src)
                end
            end

            text:setVisible(true)
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))




        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= self.m_txtTip[index] then
                text:setString(src)
            else
                text:setString(self.m_txtTip[index])
            end

        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= self.m_txtTip[index] then
                text:setString(src)
            else
                text:setString(self.m_txtTip[index])
            end
            text:setVisible(false)
        end

end

function ClubSpreadLayer:updateMyUpData()
    local curClubInfo = nil
    for k, v in pairs(ClubFrame:getInstance().m_tabGroupList) do
            if v.dwGroupID  == ClubFrame:getInstance().m_nCurrentClub then
                curClubInfo = clone(v)
            end
    end
    if curClubInfo then
        self.m_spBg:getChildByName("upNode"):getChildByName("txt_WX"):setString(curClubInfo.cmd_extra.szWX.."")
        self.m_spBg:getChildByName("upNode"):getChildByName("txt_QQ"):setString(curClubInfo.cmd_extra.szQQ.."")
        self.m_spBg:getChildByName("upNode"):getChildByName("txt_Phone"):setString(curClubInfo.cmd_extra.szPhone.."")
        self.m_spBg:getChildByName("upNode"):getChildByName("txt_ma"):setString(curClubInfo.cmd_extra.lInvitationCode.."")
    end


end


function ClubSpreadLayer:updateCode(lInvitationCode)

    local linkUrl = yl.GROUP_SHARE_URL.."?gr="..self.m_nCurrentClubInfo.GroupProperty.dwGroupID.."&code="..lInvitationCode.."&a=3&y=lua"
    self.m_shareLink:setString(linkUrl)
    --self.m_editTxt:setString(lInvitationCode.."")
    print("linkUrl==", linkUrl, yl.GROUP_SHARE_URL)

    self.m_spBg:removeChildByName("QrNodeImg")
    local qr = QrNode:createQrNode(linkUrl, 190, 5, 1)
             qr:setPosition(190,290)
             qr:setAnchorPoint(cc.p(0.5,0.5))
             qr:setName("QrNodeImg")
            self.m_spBg:addChild(qr)

    if self.m_nCurrentClubInfo.GroupProperty.dwCreaterID == GlobalUserItem.tabAccountInfo.dwUserID  or  self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MANAGER  then


    elseif self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_TEAMMEMBER then 


    elseif self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_MEMBER  or self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then 
 

        if self.m_myClubInfo.MemberInfo.nMemberRight == cmd_Club.AUTHORITY_CAPTAIN then

            self.myCode:setString(lInvitationCode.."")

        end

    end

end

function ClubSpreadLayer:updateBlind()

    local lInvitationCode = self.m_editTxt[4]:getString()
    local linkUrl = yl.GROUP_SHARE_URL.."?gr="..self.m_nCurrentClubInfo.GroupProperty.dwGroupID.."&code="..lInvitationCode.."&a=3&y=lua"
    self.m_shareLink:setString(linkUrl)
    self.m_spBg:getChildByName("clubId"):setVisible(false)
    self.m_spBg:getChildByName("upNode"):setVisible(true)
    self.m_spBg:getChildByName("url"):setPosition(425,177)
    self.m_spBg:getChildByName("url"):getChildByName("Image_14"):setPosition(130, -7)
    self.m_spBg:getChildByName("bind"):setVisible(false)


    self.m_spBg:removeChildByName("QrNodeImg")
    local qr = QrNode:createQrNode(linkUrl, 190, 5, 1)
             qr:setPosition(190,290)
             qr:setAnchorPoint(cc.p(0.5,0.5))
             qr:setName("QrNodeImg")
            self.m_spBg:addChild(qr)

    self:updateMyUpData()

end




function ClubSpreadLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubSpreadLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function ClubSpreadLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubSpreadLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubSpreadLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end



return ClubSpreadLayer