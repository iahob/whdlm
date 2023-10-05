
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local QueryDialog = appdf.req(appdf.PLAZA_VIEW_SRC .. "club.ClubQueryDialog")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")


-- 用户中心(二级弹窗)
local PopupInfoHead = appdf.req("client.src.external.PopupInfoHead")

local ClubNoticeLayer = class("ClubNoticeLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "IMG_EDIT1",             --
    "IMG_EDIT2",             --
    "BTN_SEND",             -- 发送或停止
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubNoticeLayer:ctor( scene, param, level )
    local this = self
    self.msgType = param._msgType

    ClubNoticeLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_notice.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    self.m_myClubInfo = GlobalUserItem.tabGroupUserInfo
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    spbg:setTouchEnabled(true)
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    local btn = spbg:getChildByName("btn_send")
    btn:setTag(TAG_ENUM.BTN_SEND)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    self.m_btnNotice = btn


    self.m_curRollMsg = {}

    if  bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) ~= 0 or bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0 then
    
        self.m_curRollMsg = ClubFrame:getInstance().m_tabClubRollMsg[1] or {}
        
    else
        self.m_curRollMsg = ClubFrame:getInstance().m_tabClubRollMsg[3] or {}

    end


    self.m_szClubNoticeMsg = ClubFrame:getInstance().m_szClubNoticeMsg


    --搜索输入框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    self.isClick = false
--设置公告

    local editbox = ccui.EditBox:create(cc.size(662, 278),"public/public_blank.png")
            :setPosition(361,481+80)
            :setFontColor(cc.c3b(171, 145, 210))
            :setAnchorPoint(cc.p(0,1))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 25)
            :setFont("fonts/round_body.ttf", 25) 
            --:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            
    mask:addChild(editbox)         
    self.m_editBox = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.tipString = self.msgType == 1 and "输入广播" or "请输入大联盟公告，发布后其他成员可查看"

    self.m_editText = cc.Label:createWithTTF("", "fonts/FZHTJW.TTF", 25, cc.size(645,0), cc.TEXT_ALIGNMENT_LEFT)--csbNode:getChildByName("txt_sign")
    self.m_editText:setString(self.tipString)
    self.m_editText:setPosition(72, 419)
    self.m_editText:setColor(cc.c3b(171, 145, 210))
    self.m_editText:setAnchorPoint(0, 1)
    self.m_editText:setLineBreakWithoutSpace(true)
    spbg:addChild(self.m_editText)

    if self.msgType == 1 then
            self.m_editBox:setMaxLength(32)
            if self.m_curRollMsg.szRollMsg and self.m_curRollMsg.szRollMsg  ~= "" then 
                self.m_editText:setString(self.m_curRollMsg.szRollMsg.."")
                self.m_editBox:setText(self.m_curRollMsg.szRollMsg.."")
            end

    else
            if self.m_szClubNoticeMsg and self.m_szClubNoticeMsg ~= "" then
                self.m_editText:setString(self.m_szClubNoticeMsg.."")
                self.m_editBox:setText(self.m_szClubNoticeMsg.."")
            end
            spbg:getChildByName("Sprite_3"):setTexture("club/gonggaoTitle.png")

            self.m_editBox:setMaxLength(120)
    end

    self.m_editImg = spbg:getChildByName("Image_3")
    self.m_editImg:setTag(TAG_ENUM.IMG_EDIT2)
    self.m_editImg:addTouchEventListener( touchFunC )
    self.m_editImg:setTouchEnabled(true)


    if self.msgType == 2 then
        spbg:getChildByName("txt_tip"):setVisible(false)
        self.m_btnNotice:setTitleText("发布")
        if bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) == 0 and  not ClubFrame:getInstance():isManagerAndRight(cmd_Club.MANAGER_RIGHT3) then  
            
            btn:setVisible(false)
            spbg:getChildByName("Image_3"):setContentSize(cc.size(662,378))

            self.m_editBox:setVisible(false)
            self.m_editImg:setTouchEnabled(false)

            if self.m_szClubNoticeMsg == "" then
                self.m_editBox:setVisible(false)
                self.m_editBox:setEnabled(false)
                self.tipString = "暂无公告"
                self.m_editText:setString("暂无公告")
            end
            
        end

    end

    if self.m_curRollMsg.cbFlag == 1 and self.msgType == 1 then
        self.m_btnNotice:setTitleText("停止")
    end


    --local lastText = cc.UserDefault:getInstance():getStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."notice","输入广播")
    --if lastText ~= "" and lastText ~= "输入广播" then    
        --self.m_editText:setString(lastText.."")
        --self.m_editBox:setText(lastText.."")
    --end

   -- self:onRefreshInfo(nil)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)
    self:onTransitionInFinish()



end

function ClubNoticeLayer:onExit()
    ClubNoticeLayer.super.onExit(self)
end

-- 按键监听
function ClubNoticeLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        if self.m_bBeginEdit == true then
            self.m_bBeginEdit = false
            return
        end
        local str = self.m_editText:getString() ~= self.tipString and self.m_editText:getString() or ""
        if self.msgType == 2 and str ~= self.tipString and str ~= "" and str ~= self.m_szClubNoticeMsg  and (bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_OWNER) ~= 0 or  bit:_and(self.m_myClubInfo.MemberInfo.nMemberRight, cmd_Club.AUTHORITY_MANAGER) ~= 0) then 
                self.m_Query = QueryDialog:create("此次编辑并未保存,是否继续关闭界面？", function(ok)
                    if ok then
                        self:scaleTransitionOut(self.m_spBg)
                    end
                    self.m_Query = nil
                end, 26, QueryDialog.QUERY_SURE_CANCEL)
                :setCanTouchOutside(false)
                :addTo(self._scene)
        else
            self:scaleTransitionOut(self.m_spBg)
        end


        
    elseif tag == TAG_ENUM.IMG_EDIT1 then
        self.isClick = true
        self.m_editBox1:touchDownAction(self.m_editBox1, ccui.TouchEventType.ended)
        self.m_editImg1:setEnabled(false)
    elseif tag == TAG_ENUM.IMG_EDIT2 then
        self.isClick = true
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_editImg:setEnabled(false)

    elseif tag == TAG_ENUM.BTN_SEND then
        local str = self.m_editText:getString() ~= self.tipString and self.m_editText:getString() or ""

        if self.msgType == 1 then
                if self.m_curRollMsg.cbFlag then 
                    self.m_curRollMsg.cbFlag = (self.m_curRollMsg.cbFlag+1)%2
                else
                    self.m_curRollMsg.cbFlag = 1
                end
                
                ClubFrame:getInstance():sendRollMsg(ClubFrame:getInstance().m_nCurrentClub,self.m_curRollMsg.cbFlag,str)

                if self.m_curRollMsg.cbFlag == 1 then
                    self.m_btnNotice:setTitleText("停止")
                    showToast(self, "发送成功!", 2)
                else
                    self.m_btnNotice:setTitleText("发送")   
                    showToast(self, "停止成功!", 2)       
                end
        else
                self.m_szClubNoticeMsg = str
                showToast(self, "发布成功!", 2)
                ClubFrame:getInstance():sendNoticeMsg(ClubFrame:getInstance().m_nCurrentClub,str)

        end


    end
end

function ClubNoticeLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubNoticeLayer:onTransitionInFinish()


end

function ClubNoticeLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubNoticeLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubNoticeLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end







function ClubNoticeLayer:onEditEvent(event, editbox)

    if not self.isClick then 
        return
    end

    local src = editbox:getText() 
    
        if event == "return" then
            --self.m_bBeginEdit = false
            self.m_editImg:setEnabled(true)
            self.m_editText:setString(src)
            if self.m_editText:getContentSize().width == 0 then --没有输入
                self.m_editText:setString(self.tipString)
            end
            self.m_editText:setVisible(true)
            
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false

            end)))
            self.isClick =  false
            --cc.UserDefault:getInstance():setStringForKey(GlobalUserItem.tabAccountInfo.dwUserID.."notice", src)

        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= self.tipString then
                self.m_editText:setString(src)
            else
                self.m_editText:setString(self.tipString)
            end

        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= self.tipString then
                self.m_editText:setString(src)
            else
                self.m_editText:setString(self.tipString)
            end
            self.m_editText:setVisible(false)
        end

end




return ClubNoticeLayer