
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClubFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.club.models.ClubFrame")
local cmd_Club = appdf.req(appdf.CLIENT_SRC .. "plaza.models.club.header.CMD_Club")

-- 用户中心(二级弹窗)


local ClubScorePickUp = class("ClubScorePickUp", TransitionLayer)



local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "IMAGE_BG",             -- 背景图
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_SERCH",             -- 搜索
    "BTN_BACK",               -- 回退
    "BTN_DELETE",             -- 删除
    "BTN_ARROW",               --箭头
    "BTN_EDIT",
    "BTN_SURE",
    "BTN_ALL"
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

function ClubScorePickUp:ctor( scene, param, level )
    local this = self
    
    ClubScorePickUp.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("club/nclub_tiqu.csb", self)
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
    spbg:setTouchEnabled(true)
    --spbg:setTag(TAG_ENUM.IMAGE_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg


    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    --数字键盘

    for i=1,10 do
        --数字键盘
        local btn  = spbg:getChildByName("Button_" .. (i-1))
        btn:setTag(i -1)
        btn:addTouchEventListener( touchFunC )
        btn:setPressedActionEnabled(true)
    end



    local btn  = spbg:getChildByName("Button_b")
    btn:setTag(TAG_ENUM.BTN_BACK)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn  = spbg:getChildByName("Button_x")
    btn:setTag(TAG_ENUM.BTN_DELETE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


    local btn  = spbg:getChildByName("Button_10")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    local btn  = spbg:getChildByName("Button_11")
    btn:setTag(TAG_ENUM.BTN_ALL)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)


        --搜索输入框
    local editHanlder = function(event,editbox)
        self:onEditEvent(event, editbox)
    end

    -- 设置介绍

    local editbox = ccui.EditBox:create(cc.size(282, 62),"public/public_blank.png")
            :setPosition(485,558)
            :setFontColor(cc.c3b(220, 205, 245))
            :setAnchorPoint(cc.p(0,0.5))  
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 23)
            :setFont("fonts/round_body.ttf", 23) 
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(11)
    mask:addChild(editbox)         
    self.m_editBox = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.m_editTxt = spbg:getChildByName("Text_3")
    self.m_editTxt:setString("请输入需要提取的积分")


    self.m_editImg = spbg:getChildByName("Image_1")
    self.m_editImg:setTag(TAG_ENUM.BTN_EDIT)
    self.m_editImg:addTouchEventListener( touchFunC )
    self.m_editImg:setTouchEnabled(true)


    self.m_todayLast = param.todayScore

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    --self:scaleTransitionIn(spbg)


end

function ClubScorePickUp:onExit()

    ClubScorePickUp.super.onExit(self)
end



-- 按键监听
function ClubScorePickUp:onButtonClickedEvent(tag,sender)
    print("onButtonClickedEvent---------------------------------->", tag)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        if self.m_bBeginEdit == true then
            self.m_bBeginEdit = false
            return 
        end
        self:removeFromParent()


    elseif tag >= 0 and tag <= 10 then
        local str = self.m_editTxt:getString()
        if str == "请输入需要提取的积分" then
            str = tag..""
        else
            str = str..tag..""
        end
        self.m_editTxt:setString(str.."")
        self.m_editBox:setText(str.."")
    elseif tag == TAG_ENUM.BTN_BACK then

        local str = self.m_editTxt:getString()
        if str =="请输入需要提取的积分"  or string.len(str) == 1 then 
            self.m_editTxt:setString("请输入需要提取的积分")
            self.m_editBox:setText("")
            return
        end
        str = string.sub(str,1,string.len(str)-1)

        self.m_editTxt:setString(str)
        self.m_editBox:setText(str)

    elseif tag == TAG_ENUM.BTN_DELETE then

        self.m_editTxt:setString("请输入需要提取的积分")
        self.m_editBox:setText("")

    elseif tag == TAG_ENUM.BTN_SURE then
        local str = self.m_editTxt:getString()
        if str == "请输入需要提取的积分" then
            showToast(self, "请输入需要提取的积分", 2)
            return
        end

        if not tonumber(str) or tonumber(str) == 0 then
            showToast(self, "输入有误，请重新输入", 2)
            return
        end

         if tonumber(str) > self.m_todayLast then
            showToast(self, "输入的数量高于今日可提取积分", 2)
            return
        end       
        ClubFrame:getInstance():setViewFrame(self._scene)
        ClubFrame:getInstance():sendPickUpScore(ClubFrame:getInstance().m_nCurrentClub,tonumber(str))
        self:removeFromParent()
    elseif tag == TAG_ENUM.BTN_ALL then
        if self.m_todayLast == 0 then
            showToast(self, "今日没有可提取积分！", 2)
            return
        end
        self.m_editTxt:setString(self.m_todayLast.."")
        self.m_editBox:setText(self.m_todayLast.."")

    elseif tag == TAG_ENUM.BTN_EDIT  then
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_editImg:setEnabled(false)

    end
end

function ClubScorePickUp:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ClubScorePickUp:onTransitionInFinish()
    
end

function ClubScorePickUp:onTransitionOutFinish()
    self:removeFromParent()
end

function ClubScorePickUp:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function ClubScorePickUp:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function ClubScorePickUp:onRemoveJionLayer()
    self:removeFromParent()
end



function ClubScorePickUp:onEditEvent(event, editbox)

        local src = editbox:getText() 
        if event == "return" then
            self.m_editImg:setEnabled(true)
            self.m_editTxt:setString(src)
            if self.m_editTxt:getContentSize().width == 0 then --没有输入
                self.m_editTxt:setString("请输入需要提取的积分")
            end

            self.m_editTxt:setVisible(true)
            self:runAction(cc.Sequence:create(cc.DelayTime:create(0.3),cc.CallFunc:create(function ( ... )
                self.m_bBeginEdit = false
            end)))

        elseif event == "changed" then       
            if string.len(src) ~= 0 and src ~= "请输入需要提取的积分" then
                self.m_editTxt:setString(src)
            else
                self.m_editTxt:setString("请输入需要提取的积分")
            end

        elseif event == "began" then
            self.m_bBeginEdit = true
            if string.len(src) ~= 0 and src ~= "请输入需要提取的积分" then
                self.m_editTxt:setString(src)
            else
                self.m_editTxt:setString("请输入需要提取的积分")
            end
            self.m_editTxt:setVisible(false)
        end

end





return ClubScorePickUp