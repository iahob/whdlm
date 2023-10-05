--
-- Author: zhong
-- Date: 2017-10-10 09:34:51
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local UserInfoLayer = class("UserInfoLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

-- ui
local IdentifyLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ucenter.IdentifyLayer")                        -- 认证
local ShopLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "ShopLayer")                                        -- 商城
-- ui

-- net
local ModifyFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ModifyFrame")                           -- 信息修改
-- net

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_IDENTIFY",         -- 认证
    "BTN_AGENT",            -- 代理
    "BTN_COPY",             -- 复制
    "BTN_CHARGE",           -- 充值
    "IMAGE_EDIT",           -- 编辑
    "BTN_SURE",             -- 确定
    "BTN_EXCHANGE",         -- 兑换
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function UserInfoLayer:ctor(scene, param, level)
    local this = self
    ExternalFun.registerNodeEvent(this)
    UserInfoLayer.super.ctor( self, scene, param, level )
    
    --网络回调
    local modifyCallBack = function(result,message)
        this:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("usercenter/UserInfoLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self.m_csbNode = csbNode

    -- 头像
    self.m_spHeadBg = csbNode:getChildByName("ucenter_sp_head")
    self.m_spHeadBg:setLocalZOrder(1000)

    -- 昵称
    local tmptxt = csbNode:getChildByName("txt_nick")
    local cliptxt = ClipText:createClipText(tmptxt:getContentSize(), GlobalUserItem.tabAccountInfo.szNickName, nil, 26)
    cliptxt:setAnchorPoint(tmptxt:getAnchorPoint())
    cliptxt:setTextColor(cc.c3b(0xdf, 0xff, 0xff))
    cliptxt:setPosition(tmptxt:getPosition())
    csbNode:addChild(cliptxt)
    tmptxt:removeFromParent()

    -- gameid
    csbNode:getChildByName("txt_gameid"):setString("ID:"..GlobalUserItem.tabAccountInfo.dwGameID .. "")

    -- --位置
    -- csbNode:getChildByName("txt_loction"):setString(GlobalUserItem.szLocation .. "")

    -- --IP
    -- csbNode:getChildByName("txt_ip"):setString(GlobalUserItem.szIpAdress .. "")

    self.m_redDot=csbNode:getChildByName("reddot")

    -- 钻石
    local tmpdiamond = csbNode:getChildByName("txt_diamond")
    local clipdiamond = ClipText:createClipText(tmpdiamond:getContentSize(), ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lDiamond), nil, 28)
    clipdiamond:setAnchorPoint(tmpdiamond:getAnchorPoint())
    clipdiamond:setPosition(tmpdiamond:getPosition())
    clipdiamond:setTextColor(cc.c3b(0xff,0xe5,0x89))
    csbNode:addChild(clipdiamond)
    tmpdiamond:removeFromParent()
    self.m_clipDiamond = clipdiamond

    -- 金币
    local tmpcoin = csbNode:getChildByName("txt_coin")
    local clipcoin = ClipText:createClipText(tmpdiamond:getContentSize(), ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore), nil, 28)
    clipcoin:setAnchorPoint(tmpcoin:getAnchorPoint())
    clipcoin:setPosition(tmpcoin:getPosition())
    clipcoin:setTextColor(cc.c3b(0xff,0xe5,0x89))
    csbNode:addChild(clipcoin)
    tmpcoin:removeFromParent()
    self.m_clipCoin = clipcoin

    -- 认证
    btn = csbNode:getChildByName("btn_identify")
    btn:setTag(TAG_ENUM.BTN_IDENTIFY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(GlobalUserItem.bEnableIdentify)
    
    self.m_btnRealAuth = btn
    self:updateIdentify()

     -- 代理
    btn = csbNode:getChildByName("btn_agent")
    btn:setTag(TAG_ENUM.BTN_AGENT)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    if not self.m_btnRealAuth:isVisible() then
        btn:setPositionY(213)
    end

    -- if GlobalUserItem.tabAccountInfo.cbIsAgent > 0 then
    --     btn:setVisible(true)
    -- end
    

    -- 复制
    btn = csbNode:getChildByName("btn_copy")
    btn:setTag(TAG_ENUM.BTN_COPY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 充值
    btn = csbNode:getChildByName("btn_charge")
    btn:setTag(TAG_ENUM.BTN_CHARGE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 兑换
    btn = csbNode:getChildByName("btn_exchange")
    btn:setTag(TAG_ENUM.BTN_EXCHANGE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_bBeginEdit = false
    -- 是否编辑输入
    self.m_bEditInput = false
    -- 编辑内容
    self.m_txtEdit = cc.Label:createWithTTF("", "fonts/FZHTJW.TTF", 24, cc.size(440,0), cc.TEXT_ALIGNMENT_LEFT)--csbNode:getChildByName("txt_sign")
    self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSign)
    self.m_txtEdit:setPosition(628, 358)
    self.m_txtEdit:setAnchorPoint(0.0, 1.0)
    self.m_txtEdit:setLineBreakWithoutSpace(true)
    csbNode:addChild(self.m_txtEdit)

    --性别
    self.m_spSex = csbNode:getChildByName("img_sex")
    if GlobalUserItem.tabAccountInfo.cbGender == 0 then
        self.m_spSex:loadTexture("plaza/rank_userFemale.png")
    end

    --编辑提示
    self.editTips = csbNode:getChildByName("txt_signTips")
    self.editTips:setString("最多输入32个字符")

    -- 编辑按钮
    btn = csbNode:getChildByName("editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    btn:setTouchEnabled(false)
    self.m_imageEdit = btn
end

function UserInfoLayer:onTouchImageBg()
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
end


function UserInfoLayer:updateIdentify()
    if GlobalUserItem.bEnableIdentify then
        if GlobalUserItem.isRealAuth() then
            -- 判断是否认证
            self.m_btnRealAuth:setEnabled(false)
            self.m_btnRealAuth:setTitleText("已认证")
            self.m_redDot:setVisible(false)
        else
            self.m_redDot:setVisible(true)
        end
    else
        self.m_redDot:setVisible(false)
    end
end

function UserInfoLayer:onEnterTransitionFinish()
    -- 编辑框
    local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 20), "public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setText(GlobalUserItem.tabPlusInfo.szSign)
            :setPlaceHolder("此人很懒，什么都没留下")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 24)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE) 
            --:setFontColor(cc.c4b(0,0,0,255))
            :setMaxLength(32)
    print("GlobalUserItem.tabPlusInfo.szSign",GlobalUserItem.tabPlusInfo.szSign)
    self.m_csbNode:addChild(editbox)
    self.m_editBox = editbox
    self.m_editBox:setVisible(false)      
    
    local this = self
    local editHanlder = function(event,editbox)
        this:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_imageEdit:setTouchEnabled(true)

    -- 创建头像
    local head = HeadSprite:createClipHead(GlobalUserItem.tabAccountInfo, 221, "plaza/plaza_sp_cliphead.png")
    if nil ~= head then
        head:setPosition(self.m_spHeadBg:getPosition())
        self.m_csbNode:addChild(head)
    end

    -- 查询信息
    if not GlobalUserItem.tabPlusInfo.bQueryInfo then
        -- 查询信息
        self:showPopWait()
        self._modifyFrame:onQueryUserInfo()
    end
end

function UserInfoLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end
end

function UserInfoLayer:onButtonClickedEvent(tag,sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if TAG_ENUM.BTN_IDENTIFY == tag then
        local il = IdentifyLayer:create(self._scene, {parent = self}, IdentifyLayer.SECOND_LEVEL)
        self._scene:addPopLayer(il)
    elseif TAG_ENUM.BTN_COPY == tag then
        MultiPlatform:getInstance():copyToClipboard(GlobalUserItem.tabAccountInfo.dwGameID .. "")
    elseif TAG_ENUM.BTN_CHARGE == tag then
        self._scene:popShopLayer(ShopLayer.DIAMOND_CHARGE)
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        self.m_editBox:setVisible(true)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    elseif TAG_ENUM.BTN_EXCHANGE == tag then
        self._scene:popShopLayer(ShopLayer.GOLD_EXCHANGE)
    elseif TAG_ENUM.BTN_AGENT == tag then
        --我的代理
        MultiPlatform:getInstance():openBrowser(yl.HTTP_URL .. "/Card/Index.aspx?g=" .. GlobalUserItem.tabAccountInfo.dwGameID)
    end
end

function UserInfoLayer:onRefreshInfo()
    -- 更新财富
    self.m_clipDiamond:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lDiamond))
    self.m_clipCoin:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore))

    -- 刷新认证
    if GlobalUserItem.bEnableIdentify then
        if GlobalUserItem.isRealAuth() then
            -- 判断是否认证
            self.m_btnRealAuth:setEnabled(false)
            self.m_btnRealAuth:setTitleText("已认证")
            self.m_redDot:setVisible(false)
        else
            self.m_redDot:setVisible(true)
        end
    else
        self.m_redDot:setVisible(false)
    end
end

function UserInfoLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bBeginEdit = true
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtEdit:setVisible(false)

        self.editTips:setVisible(true)
    elseif event == "changed" then

        if self.m_bBeginEdit then
            self.m_bEditInput = string.len(src) ~= 0
        end
    elseif event == "return" then
        self.m_bBeginEdit = false
        local szSign = string.gsub(editbox:getText(), " ", "")
        self.m_imageEdit:setEnabled(true)
        editbox:setVisible(false)
        self.m_txtEdit:setVisible(true)
        self.m_txtEdit:setString(szSign)

        self.editTips:setVisible(false)

        --判断长度
        if ExternalFun.stringLen(szSign) < 1 then
            showToast(self, "个性签名不能为空", 2)
            self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSign)
            return
        end

        --判断emoji
        if ExternalFun.isContainEmoji(szSign) then
            showToast(self, "个性签名中包含非法字符,请重试", 2)
            self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSign)
            return
        end

        --判断是否有非法字符
        if true == ExternalFun.isContainBadWords(szSign) then
            showToast(self, "个性签名中包含敏感字符,请重试", 2)
            self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSign)
            return
        end

        -- 判断一致
        if szSign == GlobalUserItem.tabPlusInfo.szSign then
            return
        end

        self:showPopWait()
        self._modifyFrame:onModifyUserInfo(GlobalUserItem.tabAccountInfo.cbGender, GlobalUserItem.tabAccountInfo.szNickName,szSign)
    end
end

function UserInfoLayer:onModifyCallBack(result, tips)
    self:dismissPopWait()
    if type(tips) == "string" and "" ~= tips then
        showToast(self, tips, 2)
    end
end

return UserInfoLayer