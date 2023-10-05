--
-- Author: zhong
-- Date: 2017-10-10 10:37:02
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local BankInfoLayer = class("BankInfoLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

-- net
local BankFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BankFrame")
-- net

local TAG_START             = 100
local enumTable = 
{
    "CBT_SWITCH",           -- 切换按钮

    "BTN_ENABLEBANK",       -- 开通银行
    "EDIT_SETPASSWD",       -- 设置密码
    "EDIT_SUREPASSWD",      -- 确认密码

    "CBT_OPERATE_0",        -- 金额操作0
    "CBT_OPERATE_1",        -- 金额操作1
    "CBT_OPERATE_2",        -- 金额操作2
    "CBT_OPERATE_3",        -- 金额操作3
    "CBT_OPERATE_4",        -- 金额操作4
    "CBT_OPERATE_5",        -- 金额操作5

    "BTN_OPERATE_1",        -- 金额按钮1
    "BTN_OPERATE_2",        -- 金额按钮2
    "BTN_OPERATE_3",        -- 金额按钮3
    "BTN_OPERATE_4",        -- 金额按钮4
    "BTN_OPERATE_5",        -- 金额按钮5

    "BTN_SAVE",             -- 存款
    "BTN_TAKE",             -- 取款
    "BTN_SHOWPASSWD",       -- 显示密码
    "BTN_HIDEPASSWD",       -- 隐藏密码
    "EDIT_SAVETAKE_SCORE",  -- 存取金额
    "EDIT_TAKE_PASSWD",     -- 取款密码 
    "BTN_EDIT_PASSWORD",    -- 修改密码 

    "BTN_TRANSFER",         -- 赠送
    "EDIT_TRANSFER_ID",     -- 赠送ID
    "EDIT_TRANSFER_SCORE",  -- 赠送金额
    "EDIT_TRANSFER_PASSWD", -- 银行密码
    "EDIT_TRANSFER_REMARK", -- 赠送备注
    "BTN_REMONE",         -- 清除

    "BTN_SEND_EDIT",        -- 发送修改
    "EDIT_OLD_PASS",        -- 旧密码
    "EDIT_NEW_PASS",        -- 新密码
    "EDIT_NEWMORE_PASS",    -- 确认密码
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function BankInfoLayer:ctor(scene, param, level)
    local this = self
    ExternalFun.registerNodeEvent(this)
    BankInfoLayer.super.ctor( self, scene, param, level )

    -- 是否开启银行服务
    local enableBank = (GlobalUserItem.tabSystemConfig.EnjoinInsure == yl.ENABLE_VALUE)
    self.m_bEnableBank = enableBank
    
    --网络回调
    local  bankCallBack = function(result,message)
        this:onBankCallBack(result,message)
    end
    --网络处理
    self._bankFrame = BankFrame:create(self, bankCallBack)

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("usercenter/BankInfoLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self.m_csbNode = csbNode

    -- 提示开关
    local spflag = csbNode:getChildByName("sp_enableflag")
    if enableBank then
        if cc.FileUtils:getInstance():isFileExist("usercenter/ucenter_check_enablebank_1.png") then
            spflag:setSpriteFrame(cc.Sprite:create("usercenter/ucenter_check_enablebank_1.png"):getSpriteFrame())
        end
    else
        if cc.FileUtils:getInstance():isFileExist("usercenter/ucenter_check_enablebank_0.png") then
            spflag:setSpriteFrame(cc.Sprite:create("usercenter/ucenter_check_enablebank_0.png"):getSpriteFrame())
        end
    end

    -- 提示文字
    -- csbNode:getChildByName("ucenter_sp_bank_coinword"):setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))
    -- csbNode:getChildByName("shop_sp_coinicon"):setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))
    local bEnabled = enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled)
    csbNode:getChildByName("shop_sp_coinicon"):setVisible(bEnabled)
    csbNode:getChildByName("shop_sp_coinicon_0"):setVisible(bEnabled)
    csbNode:getChildByName("Text_15"):setVisible(bEnabled) --文字 银行
    csbNode:getChildByName("Text_14"):setVisible(bEnabled) --文字 携带
    

    -- 携带金币
    local tmpcoin = csbNode:getChildByName("tmp_coin")
    local clipcoin = ClipText:createClipText(tmpcoin:getContentSize(), ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore), nil, 26)
    clipcoin:setAnchorPoint(tmpcoin:getAnchorPoint())
    clipcoin:setPosition(tmpcoin:getPosition())
    clipcoin:setTextColor(cc.c3b(0xff,0xe5,0x89))
    csbNode:addChild(clipcoin)
    tmpcoin:removeFromParent()
    self.m_clipCoin = clipcoin
    self.m_clipCoin:setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))

    -- 银行金币
    tmpcoin = csbNode:getChildByName("tmp_insurecoin")
    clipcoin = ClipText:createClipText(tmpcoin:getContentSize(), "", nil, 26)
    clipcoin:setAnchorPoint(tmpcoin:getAnchorPoint())
    clipcoin:setPosition(tmpcoin:getPosition())
    clipcoin:setTextColor(cc.c3b(0xff,0xe5,0x89))
    csbNode:addChild(clipcoin)
    tmpcoin:removeFromParent()
    self.m_clipInsureCoin = clipcoin
    self.m_clipInsureCoin:setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))

    -- 
    local checkboxlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    -- 切换
    local cbtbox = csbNode:getChildByName("check_switchbank")
    cbtbox:addEventListener(checkboxlistener)
    cbtbox:setSelected(true)
    self.m_checkBox = cbtbox
    self.m_checkBox:setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled) )

    -- 底板
    self.m_spBankBg = csbNode:getChildByName("sp_bankbg")
    self.m_spBankBg:setVisible(enableBank and (1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))
    print("self.m_spBankBg:",self.m_spBankBg:isVisible())

    -- 开通界面
    self.m_enableLayer = csbNode:getChildByName("panel_enable")
    self.m_enableLayer:setVisible(enableBank and (0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled))
    self:initEnableLayer()

    -- 存取界面
    self.m_lOperateScore = 0
    self.m_saveLayer = csbNode:getChildByName("panel_banksave")
    self.m_saveLayer:setVisible(false)
    self:initSaveLayer()

    -- 赠送界面
    self.m_transferLayer = csbNode:getChildByName("panel_banktransfer")
    self.m_transferLayer:setVisible(false)
    self:initTransferLayer()

    --修改密码界面
    self.m_passwordEditLayer = csbNode:getChildByName("panel_bankeditpassword")
    self.m_passwordEditLayer:setVisible(false)
    self:initPasswordEditLayer()

    -- 是否编辑
    self.m_bBeginEdit = false
    self.m_bEditInput = false
    self.m_nEditCount = 0
end

function BankInfoLayer:initEnableLayer()
    local this = self
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    -- 开通银行
    local btn = self.m_enableLayer:getChildByName("btn_enable")
    btn:setTag(TAG_ENUM.BTN_ENABLEBANK)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true) 
end

function BankInfoLayer:initSaveLayer()
    local this = self
    -- 存取金额
    self.m_tabOperate = {}
    self.m_nOperateIdx = TAG_ENUM.CBT_OPERATE_0
    self.m_nSelectOpearte = 0
    -- 
    --[[local checkboxlistener = function (sender,eventType)
        this:onOperateSelectEvent(sender:getTag(),sender)
    end]]
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    local cbtbox = self.m_saveLayer:getChildByName("opearte_1")
    cbtbox:setTag(TAG_ENUM.BTN_OPERATE_1)
    cbtbox:addTouchEventListener( touchFunC )
    --cbtbox:setPressedActionEnabled(true)
    cbtbox.nOperateScore = 10000

    cbtbox = self.m_saveLayer:getChildByName("opearte_2")
    cbtbox:setTag(TAG_ENUM.BTN_OPERATE_2)
    cbtbox:addTouchEventListener( touchFunC )
    --cbtbox:setPressedActionEnabled(true)
    cbtbox.nOperateScore = 100000

    cbtbox = self.m_saveLayer:getChildByName("opearte_3")
    cbtbox:setTag(TAG_ENUM.BTN_OPERATE_3)
    cbtbox:addTouchEventListener( touchFunC )
    --cbtbox:setPressedActionEnabled(true)
    cbtbox.nOperateScore = 1000000

    cbtbox = self.m_saveLayer:getChildByName("opearte_4")
    cbtbox:setTag(TAG_ENUM.BTN_OPERATE_4)
    cbtbox:addTouchEventListener( touchFunC )
    --cbtbox:setPressedActionEnabled(true)
    cbtbox.nOperateScore = 10000000

    cbtbox = self.m_saveLayer:getChildByName("opearte_5")
    cbtbox:setTag(TAG_ENUM.BTN_OPERATE_5)
    cbtbox:addTouchEventListener( touchFunC )
    --cbtbox:setPressedActionEnabled(true)
    cbtbox.nOperateScore = 20000000

    self.m_tabOpearteText = {}
    self.m_saveLayer:getChildByName("txt_operate_1"):setString("1W")
    self.m_tabOpearteText[TAG_ENUM.CBT_OPERATE_1] = self.m_saveLayer:getChildByName("txt_operate_1")
    self.m_saveLayer:getChildByName("txt_operate_2"):setString("10W")
    self.m_tabOpearteText[TAG_ENUM.CBT_OPERATE_2] = self.m_saveLayer:getChildByName("txt_operate_2")
    self.m_saveLayer:getChildByName("txt_operate_3"):setString("100W")
    self.m_tabOpearteText[TAG_ENUM.CBT_OPERATE_3] = self.m_saveLayer:getChildByName("txt_operate_3")
    self.m_saveLayer:getChildByName("txt_operate_4"):setString("1000W")
    self.m_tabOpearteText[TAG_ENUM.CBT_OPERATE_4] = self.m_saveLayer:getChildByName("txt_operate_4")
    self.m_saveLayer:getChildByName("txt_operate_5"):setString("2000W")
    self.m_tabOpearteText[TAG_ENUM.CBT_OPERATE_5] = self.m_saveLayer:getChildByName("txt_operate_5")

    -- 提示
    self.m_txtTakeTips = self.m_saveLayer:getChildByName("txt_bank_tips")

    -- 存款
    local btn = self.m_saveLayer:getChildByName("btn_save")
    btn:setTag(TAG_ENUM.BTN_SAVE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 取款
    btn = self.m_saveLayer:getChildByName("btn_take")
    btn:setTag(TAG_ENUM.BTN_TAKE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 显示密码
    btn = self.m_saveLayer:getChildByName("btn_showpasswd")
    btn:setTag(TAG_ENUM.BTN_SHOWPASSWD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    self.m_btnShowPasswd = btn

    --修改密码
    btn = self.m_saveLayer:getChildByName("btn_editPassword")
    btn:setTag(TAG_ENUM.BTN_EDIT_PASSWORD)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    --self.m_btnShowPasswd = btn


    --清除按钮
    local btnRemove = self.m_saveLayer:getChildByName("btn_remove")
    btnRemove:setTag(TAG_ENUM.BTN_REMONE)
    btnRemove:addTouchEventListener( touchFunC )
    btnRemove:setPressedActionEnabled(true)
end

function BankInfoLayer:onTouchImageBg()
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
end

function BankInfoLayer:onOperateSelectEvent( tag, sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        sender:setSelected(self.m_nOperateIdx == tag)
        if nil ~= self.m_tabOpearteText[tag] then
            local fontsize = (self.m_nOperateIdx == tag) and 26 or 24
            self.m_tabOpearteText[tag]:setFontSize(fontsize)
        end
        
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if self.m_nOperateIdx == tag then
        sender:setSelected(true)
        if nil ~= self.m_tabOpearteText[tag] then
            local fontsize = (self.m_nOperateIdx == tag) and 26 or 24
            self.m_tabOpearteText[tag]:setFontSize(fontsize)
        end
        return
    end
    self.m_nOperateIdx = tag
    for k,v in pairs(self.m_tabOperate) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= self.m_tabOpearteText[tag] then
        self.m_tabOpearteText[tag]:setFontSize(26)
    end
    for k,v in pairs(self.m_tabOpearteText) do
        if k ~= tag then
            v:setFontSize(20)
        end
    end

    if nil ~= sender.nOperateScore then
        self.m_nSelectOpearte = sender.nOperateScore
    end
    self.m_editSave:setText(self.m_nSelectOpearte .. "")
end

function BankInfoLayer:initTransferLayer()
    local this = self
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 赠送
    local btn = self.m_transferLayer:getChildByName("btn_transfer")
    btn:setTag(TAG_ENUM.BTN_TRANSFER)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 金额大写
    self.m_txtNumber = self.m_transferLayer:getChildByName("txt_operate_number")
    self.m_txtNumber:setString("")

    -- 提示
    self.m_txtTransferTips = self.m_transferLayer:getChildByName("txt_bank_tips")
end
function BankInfoLayer:initPasswordEditLayer()
    local this = self
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 赠送
    local btn = self.m_passwordEditLayer:getChildByName("btn_edit")
    btn:setTag(TAG_ENUM.BTN_SEND_EDIT)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
end


function BankInfoLayer:onEnterTransitionFinish()
    if self.m_bEnableBank then
        if 1 == GlobalUserItem.tabAccountInfo.cbInsureEnabled then
            self:showPopWait()
            self._bankFrame:onGetBankInfo()
        end
    end

    -- 编辑框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 设置密码
    local tmp = self.m_enableLayer:getChildByName("sp_passwd_edit")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入您的密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30) 
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setTag(TAG_ENUM.EDIT_SETPASSWD)
            :setMaxLength(6)
    self.m_enableLayer:addChild(editbox)         
    self.m_editSetPasswd = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 确认密码
    tmp = self.m_enableLayer:getChildByName("sp_ensurepasswd_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入您的密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setTag(TAG_ENUM.EDIT_SUREPASSWD)
            :setMaxLength(6)
    self.m_enableLayer:addChild(editbox)         
    self.m_editSurePasswd = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 存取
    tmp = self.m_saveLayer:getChildByName("sp_number_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width -70, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入存取金额")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
            :setTag(TAG_ENUM.EDIT_SAVETAKE_SCORE)
            :setPositionX(tmp:getPosition() -35)
    self.m_saveLayer:addChild(editbox)         
    self.m_editSave = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)
    -- 密码
    tmp = self.m_saveLayer:getChildByName("sp_passwd_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 70, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入银行密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_TAKE_PASSWD)
            :setPositionX(tmp:getPosition() -35)
    self.m_saveLayer:addChild(editbox)         
    self.m_editSavePasswd = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 赠送ID
    tmp = self.m_transferLayer:getChildByName("sp_id_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入赠送ID")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setTag(TAG_ENUM.EDIT_TRANSFER_ID)
    self.m_transferLayer:addChild(editbox)         
    self.m_editTransferID = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 赠送金额
    tmp = self.m_transferLayer:getChildByName("sp_number_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入赠送金额")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
            :setTag(TAG_ENUM.EDIT_TRANSFER_SCORE)
    self.m_transferLayer:addChild(editbox)         
    self.m_editTransferScore = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 银行密码
    tmp = self.m_transferLayer:getChildByName("sp_passwd_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入银行密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_TRANSFER_PASSWD)
    self.m_transferLayer:addChild(editbox)         
    self.m_editTransferPasswd = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    -- 备注
    tmp = self.m_transferLayer:getChildByName("sp_remark_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_TRANSFER_REMARK)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_transferLayer:addChild(editbox)         
    self.m_editTransferRemark = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --修改密码
    --旧密码
    tmp = self.m_passwordEditLayer:getChildByName("sp_old_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入旧密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_OLD_PASS)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_passwordEditLayer:addChild(editbox)         
    self.m_editOldPassEdit = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --新密码
    tmp = self.m_passwordEditLayer:getChildByName("sp_new_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入新密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_NEW_PASS)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_passwordEditLayer:addChild(editbox)         
    self.m_editNewPassEdit = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    --确认新密码
    tmp = self.m_passwordEditLayer:getChildByName("sp_newmore_edit")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请再次输入新密码")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)
            :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setTag(TAG_ENUM.EDIT_NEWMORE_PASS)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_passwordEditLayer:addChild(editbox)         
    self.m_editNewMorePassEdit = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)
end

function BankInfoLayer:onExit()
    if nil ~= self._bankFrame then
        self._bankFrame:onCloseSocket()
        self._bankFrame = nil
    end
end

function BankInfoLayer:onSelectedEvent(tag, sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        sender:setSelected(not sender:isSelected())
        return
    end
    self:onRefreshInfo()
end

function BankInfoLayer:onButtonClickedEvent(tag, sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end

    if TAG_ENUM.BTN_ENABLEBANK == tag then
        self:onEnableBank()
    elseif TAG_ENUM.BTN_SAVE == tag then
        self:onSaveScore()
    elseif TAG_ENUM.BTN_TAKE == tag then
        self:onTakeScore()
    elseif TAG_ENUM.BTN_SHOWPASSWD == tag then
        sender:loadTextureDisabled("usercenter/ucenter_btn_showpasswd.png")
        sender:loadTextureNormal("usercenter/ucenter_btn_showpasswd.png")
        sender:loadTexturePressed("usercenter/ucenter_btn_showpasswd.png")
        sender:setTag(TAG_ENUM.BTN_HIDEPASSWD)

        local txt = self.m_editSavePasswd:getText()
        self.m_editSavePasswd:removeFromParent()
        self.m_editSavePasswd = nil
        -- 编辑框
        local editHanlder = function(event,editbox)
            self:onEditEvent(event,editbox)
        end
        -- 密码
        local tmp = self.m_saveLayer:getChildByName("sp_passwd_edit")
        local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 70, tmp:getContentSize().height),"public/public_blank.png")
                :setPosition(tmp:getPosition())
                :setPlaceHolder("请输入银行密码")
                :setPlaceholderFont("fonts/round_body.ttf", 30)
                :setFont("fonts/round_body.ttf", 30)
                :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
                :setTag(TAG_ENUM.EDIT_TAKE_PASSWD)
                :setText(txt)
                :setPositionX(tmp:getPosition() -35)
        self.m_saveLayer:addChild(editbox)         
        self.m_editSavePasswd = editbox
        editbox:registerScriptEditBoxHandler(editHanlder)
    elseif TAG_ENUM.BTN_HIDEPASSWD == tag then
        sender:loadTextureDisabled("usercenter/ucenter_btn_hidepasswd.png")
        sender:loadTextureNormal("usercenter/ucenter_btn_hidepasswd.png")
        sender:loadTexturePressed("usercenter/ucenter_btn_hidepasswd.png")
        sender:setTag(TAG_ENUM.BTN_SHOWPASSWD)

        local txt = self.m_editSavePasswd:getText()
        self.m_editSavePasswd:removeFromParent()
        self.m_editSavePasswd = nil
        -- 编辑框
        local editHanlder = function(event,editbox)
            self:onEditEvent(event,editbox)
        end
        -- 密码
        local tmp = self.m_saveLayer:getChildByName("sp_passwd_edit")
        local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 70, tmp:getContentSize().height),"public/public_blank.png")
                :setPosition(tmp:getPosition())
                :setPlaceHolder("请输入银行密码")
                :setPlaceholderFont("fonts/round_body.ttf", 30)
                :setFont("fonts/round_body.ttf", 30)
                :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
                :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
                :setTag(TAG_ENUM.EDIT_TAKE_PASSWD)
                :setText(txt)
                :setPositionX(tmp:getPosition() -35)
        self.m_saveLayer:addChild(editbox)         
        self.m_editSavePasswd = editbox
        editbox:registerScriptEditBoxHandler(editHanlder)
    elseif TAG_ENUM.BTN_TRANSFER == tag then
        self:onTransferScore()
    elseif TAG_ENUM.BTN_OPERATE_1 == tag 
        or TAG_ENUM.BTN_OPERATE_2 == tag 
        or TAG_ENUM.BTN_OPERATE_3 == tag 
        or TAG_ENUM.BTN_OPERATE_4 == tag 
        or TAG_ENUM.BTN_OPERATE_5 == tag 
        then
        local opeScore = sender.nOperateScore or 0
        self.m_lOperateScore = self.m_lOperateScore + opeScore

        -- 更新金币
        self.m_editSave:setText(self.m_lOperateScore .. "")
    elseif TAG_ENUM.BTN_REMONE == tag then --清除
        print("sadasdfasfdsfgsasdaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
        self.m_editSave:setText("")
        self.m_lOperateScore = 0

        
    elseif TAG_ENUM.BTN_SEND_EDIT == tag then --发型修改密码
        --判断合法
        --参数判断
        local szNewPass =  self.m_editNewPassEdit:getText()
        local szNewMorePass =  self.m_editNewMorePassEdit:getText()
        local szOldPass =  self.m_editOldPassEdit:getText()

        if #szOldPass == 0 or #szNewPass == 0 or #szOldPass == 0 then
            showToast(self,"密码不能为空",2)
            return
        end

        if #szNewPass < 6 or #szOldPass < 6 then
            showToast(self,"密码必须为6个纯数字，请重新输入！",2)
            return
        end

        if szOldPass == szNewPass or szOldPass == szNewMorePass then
            showToast(self,"新旧密码不能一样，请确认后重新输入",2)
            return
        end

        if szNewPass ~= szNewMorePass then
            showToast(self,"两次输入的密码不一致，请确认后重新输入",2)
            return
        end



        self.m_editNewPassEdit:setText("")
        self.m_editNewMorePassEdit:setText("")
        self.m_editOldPassEdit:setText("")

        self._bankFrame:onEditPassWord(szNewPass, szOldPass)

        self.m_saveLayer:setVisible(true)
        self.m_transferLayer:setVisible(false)
        self.m_passwordEditLayer:setVisible(false)
    elseif TAG_ENUM.BTN_EDIT_PASSWORD == tag then --发型修改密码
        self.m_saveLayer:setVisible(false)
        self.m_transferLayer:setVisible(false)
        self.m_passwordEditLayer:setVisible(true)
    end
end

function BankInfoLayer:onEditEvent(event, editbox)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    local src = editbox:getText()
    local tag = editbox:getTag()
    if event == "began" then
        self.m_bBeginEdit = true
        self.m_nEditCount = 0
        if targetPlatform ~= cc.PLATFORM_OS_ANDROID then
            self.m_bEditInput = true
        end
    elseif event == "changed" then
        self.m_nEditCount = self.m_nEditCount + 1
        if self.m_bBeginEdit then
            if targetPlatform == cc.PLATFORM_OS_ANDROID then
                self.m_bEditInput = (self.m_nEditCount > 1)
            end
            
            if TAG_ENUM.EDIT_SAVETAKE_SCORE == tag then
                -- 重置存取金额选择
                --[[self.m_nOperateIdx = TAG_ENUM.CBT_OPERATE_0
                for k,v in pairs(self.m_tabOperate) do
                    v:setSelected(false)
                end
                for k,v in pairs(self.m_tabOpearteText) do
                    v:setFontSize(20)
                end]]
            elseif TAG_ENUM.EDIT_TRANSFER_SCORE == tag then
                -- 赠送大写
                local ndst = tonumber(src)
                if type(ndst) == "number" and ndst < 9999999999999 then
                    local strNum = string.EllipsisByConfig(ExternalFun.numberTransiform(src) , 240, 
                                                string.getConfig("fonts/round_body.ttf", 20))
                    --self.m_txtNumber:setString(strNum)
                    self.m_txtNumber:setString("(" .. strNum .. ")")
                else
                    self.m_txtNumber:setString("")
                end
            end
        end
    elseif event == "return" then
        self.m_bBeginEdit = false
        self.m_nEditCount = 0
        if TAG_ENUM.EDIT_SAVETAKE_SCORE == tag then
            local ndst = tonumber(src)
            if nil ~= ndst then
                self.m_lOperateScore = ndst
            end
        end
    end
end

function BankInfoLayer:onRefreshInfo()
    if not self.m_bEnableBank or (0 == GlobalUserItem.tabAccountInfo.cbInsureEnabled) then
        print("BankInfoLayer:onRefreshInfo 未开启银行服务或银行未开通")
        return
    end
    if self.m_checkBox:isSelected() then
        -- 存取
        self.m_saveLayer:setVisible(true)
        self.m_transferLayer:setVisible(false)
        self.m_passwordEditLayer:setVisible(false)

       -- self.m_spBankBg:setFlippedY(true)
    else
        -- 赠送
        self.m_saveLayer:setVisible(false)
        self.m_transferLayer:setVisible(true)
        self.m_passwordEditLayer:setVisible(false)

        self.m_spBankBg:setFlippedY(false)
    end
    self:clean()
end

function BankInfoLayer:onBankCallBack(result, message)
    self:dismissPopWait()
    if message ~= nil and message ~= "" then
        showToast(self,message,2)
    end

    -- 银行开通
    if result == BankFrame.OP_ENABLE_BANK or result == BankFrame.OP_ENABLE_BANK_GAME then
        if GlobalUserItem.tabAccountInfo.cbInsureEnabled == 1 then
            showToast(self,"银行开通成功！",2)
            self.m_enableLayer:setVisible(false) 
            
            -- 提示文字
            --self.m_csbNode:getChildByName("ucenter_sp_bank_coinword"):setVisible(true)
            self.m_csbNode:getChildByName("shop_sp_coinicon"):setVisible(true)
            self.m_csbNode:getChildByName("shop_sp_coinicon_0"):setVisible(true)
            self.m_csbNode:getChildByName("Text_15"):setVisible(true) --文字 银行
            self.m_csbNode:getChildByName("Text_14"):setVisible(true) --文字 携带
            self.m_clipCoin:setVisible(true)
            self.m_clipInsureCoin:setVisible(true)

            -- 请求银行信息
            self:showPopWait()
            if result == BankFrame.OP_ENABLE_BANK then
                self._bankFrame:sendGetBankInfo()
            elseif result == BankFrame.OP_ENABLE_BANK_GAME then
                self._bankFrame:onGetBankInfo()
            end
        end
    end

    -- 银行信息
    if result == self._bankFrame.OP_GET_BANKINFO then
        -- 允许转账
        local enableTransfer = (0 == message.cbEnjoinTransfer)
        if false == enableTransfer then
            local enable = message.dwUserRight or 0
            enableTransfer = (bit:_and(enable, yl.UR_OPEN_TRANSFER) ~= 0)
        end
        -- 显示信息
        self.m_checkBox:setVisible(enableTransfer)
        self.m_spBankBg:setVisible(enableTransfer)

        self.m_tabBankConfigInfo = message
        -- 取款收费比例
        local str = string.format("银行取款将收取%d‰的手续费", message.wRevenueTake)
        self.m_txtTakeTips:setString(str)

        -- 转账收费比例
        str = string.format("提示:金币赠送需扣除%d‰的手续费。", message.wRevenueTransfer)
        self.m_txtTransferTips:setString(str)

        self.m_clipInsureCoin:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))
        self:onRefreshInfo()
    end

    -- 银行成功
    if result == 1 then
        -- 更新财富
        self.m_clipCoin:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserScore))
        self.m_clipInsureCoin:setString(ExternalFun.numberThousands(GlobalUserItem.tabAccountInfo.lUserInsure))

        self:clean()
        if self._bankFrame._oprateCode == BankFrame.OP_SEND_SCORE then
            -- 转账凭证
            --self:showCerLayer(self._bankFrame._tabTarget)
        end
    end
end

-- 开通
function BankInfoLayer:onEnableBank()
    --参数判断
    local szPass = self.m_editSetPasswd:getText()
    local szPassConfirm = self.m_editSurePasswd:getText()

    if #szPass < 1 then 
        showToast(self,"请输入银行密码！",2)
        return
    end
    if #szPass < 6 then
        showToast(self,"密码必须等于6个字符，请重新输入！",2)
        return
    end

    if #szPassConfirm < 1 then 
        showToast(self,"请在确认栏输入银行密码！",2)
        return
    end
    if #szPassConfirm < 6 then
        showToast(self,"确认栏密码必须等于6个字符，请重新输入！",2)
        return
    end

    if szPass ~= szPassConfirm then
        showToast(self,"设置栏和确认栏的密码不相同，请重新输入！",2)
        return
    end

    -- 与帐号不同
    if string.lower(szPass) == string.lower(GlobalUserItem.tabAccountInfo.szAccount) then
        showToast(self,"密码不能与帐号相同，请重新输入！",2)
        return
    end

    --[[-- 银行不同登陆
    if string.lower(szPass) == string.lower(GlobalUserItem.tabAccountInfo.szPassword) then
        showToast(self, "银行密码不能与登录密码一致!", 2)
        return
    end]]
    
    --[[-- 首位为字母
    if 1 ~= string.find(szPass, "%a") then
        showToast(self,"密码首位必须为字母，请重新输入！",2)
        return
    end]]

    self:showPopWait()
    self._bankFrame:onEnableBank(szPass)
end

-- 存款
function BankInfoLayer:onSaveScore()
    --参数判断
    local szScore = string.gsub(self.m_editSave:getText(),"([^0-9])","")   
    szScore = string.gsub(szScore, "[.]", "")
    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end
    local lOperateScore = tonumber(szScore)
    if nil == lOperateScore or lOperateScore < 1 then
        showToast(self,"请输入正确金额！",2)
        return
    end
    if lOperateScore > GlobalUserItem.tabAccountInfo.lUserScore then
        showToast(self,"您所携带游戏币的数目余额不足,请重新输入游戏币数量!",2)
        return
    end

    self:showPopWait()
    self._bankFrame:onSaveScore(lOperateScore)
end

-- 取款
function BankInfoLayer:onTakeScore()
    --参数判断
    local szScore =  string.gsub(self.m_editSave:getText(),"([^0-9])","")
    szScore = string.gsub(szScore, "[.]", "")
    local szPass = self.m_editSavePasswd:getText()
    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end
    local lOperateScore = tonumber(szScore)
    if nil == lOperateScore or lOperateScore < 1 then
        showToast(self,"请输入正确金额！",2)
        return
    end
    if lOperateScore > GlobalUserItem.tabAccountInfo.lUserInsure then
        showToast(self,"您银行游戏币的数目余额不足,请重新输入游戏币数量！",2)
        return
    end
    if #szPass < 1 then 
        showToast(self,"请输入银行密码！",2)
        return
    end
    if #szPass <6 then
        showToast(self,"密码必须大于6个字符，请重新输入！",2)
        return
    end

    self:showPopWait()
    self._bankFrame:onTakeScore(lOperateScore,szPass)
end

-- 转账
function BankInfoLayer:onTransferScore()
    --参数判断
    local szScore =  string.gsub(self.m_editTransferScore:getText(),"([^0-9])","")
    local szPass = self.m_editTransferPasswd:getText()
    local szTarget = self.m_editTransferID:getText()
    local szRemark = self.m_editTransferRemark:getText()
    local byID = 1

    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end
    local lOperateScore = tonumber(szScore)
    if nil == lOperateScore or lOperateScore < 1 then
        showToast(self,"请输入正确金额！",2)
        return
    end
    if #szPass < 1 then 
        showToast(self,"请输入钱包密码！",2)
        return
    end
    if #szPass < 6 then
        showToast(self,"密码必须大于6个字符，请重新输入！",2)
        return
    end
    if #szTarget < 1 then 
        showToast(self,"请输入赠送用户ID！",2)
        return
    end

    self:showPopWait()
    self._bankFrame:onTransferScore(lOperateScore,szTarget,szPass,byID,szRemark)
end

function BankInfoLayer:clean()
    -- 重置编辑区域
    self.m_editSave:setText("")
    self.m_editSavePasswd:setText("")
    self.m_editTransferID:setText("")
    self.m_editTransferScore:setText("")
    self.m_editTransferPasswd:setText("")
    self.m_editTransferRemark:setText("")

    -- 重置选择
    -- self.m_nOperateIdx = TAG_ENUM.CBT_OPERATE_0
    -- for k,v in pairs(self.m_tabOperate) do
    --     v:setSelected(false)
    -- end
    self.m_lOperateScore = 0

    -- 转账金额大写
    self.m_txtNumber:setString("")

    -- 重置密码显示
    self.m_btnShowPasswd:loadTextureDisabled("usercenter/ucenter_btn_hidepasswd.png")
    self.m_btnShowPasswd:loadTextureNormal("usercenter/ucenter_btn_hidepasswd.png")
    self.m_btnShowPasswd:loadTexturePressed("usercenter/ucenter_btn_hidepasswd.png")
    self.m_btnShowPasswd:setTag(TAG_ENUM.BTN_SHOWPASSWD)
end

return BankInfoLayer