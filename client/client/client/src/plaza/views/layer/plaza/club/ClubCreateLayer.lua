
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")

-- 用户中心(二级弹窗)
local ClubCreateLayer = class("ClubCreateLayer", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "BTN_CREATE",           -- 创建


    "CBT_PAYHOLDER",         -- 群主支付
    "CBT_PAYAA",             -- AA支付

    "EDIT_NAME",            --名字
    "EDIT_DES",            --介绍

    "BTN_EDIT",            -- 编辑按钮

    "IMAGE_NAME_EDIT",           -- 名字编辑
    "IMAGE_DES_EDIT",            -- 描述编辑
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubCreateLayer:ctor( scene, param, level )
    local this = self

    ClubCreateLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_create.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self._scene = scene
    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("bg")
    spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --创建大联盟按钮
    local btn = spbg:getChildByName("btn_ok")
    btn:setTag(TAG_ENUM.BTN_CREATE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    --名字编辑背景
    local image = spbg:getChildByName("Image_1")
    image:setTag(TAG_ENUM.IMAGE_NAME_EDIT)
    image:addTouchEventListener( touchFunC )
    --image:setLocalZOrder(2)
    image:setTouchEnabled(true)
    self.m_imageNameEdit = image


      -- 编辑框

    -- 设置名字
        local editHanlder = function(event,editbox)
            self:onEditEvent(event,editbox)
        end
        
    local editbox = ccui.EditBox:create(cc.size(471, 72),"public/public_blank.png")
            :setPosition(450,393)
            :setFontColor(cc.c3b(255, 238, 145))
            :setAnchorPoint(cc.p(0,0.5))
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/FZHTJW.TTF", 25)
            :setFont("fonts/FZHTJW.TTF", 25) 
            :setTag(TAG_ENUM.EDIT_NAME)
            :setMaxLength(6)
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
    mask:addChild(editbox)         
    self.m_editSetName = editbox
    self.m_editSetName:registerScriptEditBoxHandler(editHanlder)
    

    self.txt_name = spbg:getChildByName("Text_5")
    self.txt_name:setString("请输入大联盟名字")

    local maxCreater = 10
    dump(ClubFrame:getInstance().m_tabClubCommon)
    for k,v in pairs(ClubFrame:getInstance().m_tabClubCommon) do
        if v and v.szOptionName =="MaxCreateGroupCount" then
            maxCreater = v.nOptionValue
        end
    end
    spbg:getChildByName("Text_1"):setString(maxCreater.."")

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(self.m_spBg)

end

function ClubCreateLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end 
    ClubCreateLayer.super.onExit(self)
end

-- 按键监听
function ClubCreateLayer:onButtonClickedEvent(tag,sender)
    print("onButtonClickedEvent---------------------->", tag)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        if self.m_bBeginEdit then
            self.m_bBeginEdit = false
            return
        end
        self:removeFromParent()
    elseif TAG_ENUM.BTN_CREATE == tag then --创建
        local content = self.txt_name:getString()
        print("string.len", string.len(content))
        if string.len(content) <1 or content == "请输入大联盟名字" then
            showToast(self, "大联盟名字不能为空", 3)
            return
        else
            --敏感词过滤  
            if true == ExternalFun.isContainBadWords(content) then
                showToast(self, "大联盟名字包含敏感词汇!", 3)
                return
            end 

        if ExternalFun.isContainSpecial(content) then
            showToast(self, "大联盟名字不能含有字符!", 3)
            return
        end 

            if string.len(content) >18 then
                showToast(self, "最多输入6个汉字或12个字符", 3)
                return
            end
        end


        local tabInfo = {}
        tabInfo.cbPayType = 1
        tabInfo.cbCurrencyKind = 0
        tabInfo.cbBattleMask = 0
        tabInfo.dwStationID = 0
        tabInfo.szGroupName = self.txt_name:getString()
        tabInfo.szGroupIntroduce = ""
        ClubFrame:getInstance().m_curQueryClub = 1

        ClubFrame:getInstance():setViewFrame(self)
        ClubFrame:getInstance():sendCreateGroup(tabInfo)
    elseif TAG_ENUM.IMAGE_NAME_EDIT == tag then --名字
        --self.m_editSetName:setVisible(true)

        self.m_editSetName:touchDownAction(self.m_editSetName, ccui.TouchEventType.ended)
        self.m_imageNameEdit:setEnabled(false)
    elseif TAG_ENUM.IMAGE_DES_EDIT == tag then --名字
        --self.m_editSetDes:setVisible(true)
        self.m_editSetDes:touchDownAction(self.m_editSetDes, ccui.TouchEventType.ended)
        self.m_imageDesEdit:setEnabled(false)

        print("!!!!!!!!!!!!!IMAGE_DES_EDIT !!!!!!!!!!!!")
    end
end


function ClubCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubCreateLayer:onTransitionInFinish()
    self:onRefreshInfo()
end

function ClubCreateLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubCreateLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ClubCreateLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubCreateLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end


function ClubCreateLayer:onEditEvent(event, editbox)


        local src = editbox:getText() 
        if event == "return" then
            print("onEditEvent", src)
            self.m_imageNameEdit:setEnabled(true)
            self.txt_name:setVisible(true)
            self.txt_name:setString(src)
            --self.m_editSetName:setText("")
            if self.txt_name:getContentSize().width == 0 then --没有输入
                self.txt_name:setString("请输入大联盟名字")
            end
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))

        elseif event == "changed" then
            if string.len(src) ~= 0 and src ~= "请输入大联盟名字" then
                self.txt_name:setString(src)
            else
                self.txt_name:setString("请输入大联盟名字")
            end

        elseif event == "began" then

            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "请输入大联盟名字" then
                self.txt_name:setString(src)
            else
                self.txt_name:setString("请输入大联盟名字")
            end
            self.txt_name:setVisible(false)

        end

end

function ClubCreateLayer:onSubCreateGroupSuccess()
    --创建成功
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, "大联盟创建成功！", 2)
    end
    self:scaleTransitionOut(self.m_spBg)

    
    -- 通知更新    
    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY )
    eventListener.obj = yl.RY_CULB_MEMBER_NOTIFY
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end


return ClubCreateLayer