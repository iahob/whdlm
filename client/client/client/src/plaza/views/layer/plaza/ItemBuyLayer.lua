--
-- Author: zhong
-- Date: 2017-05-24 20:35:00
--
-- 物品购买(三级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ItemBuyLayer = class("ItemBuyLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- net
local ShopDetailFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ShopDetailFrame")
-- net

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_BUY",              -- 购买
    "BTN_SUB",              -- 减
    "BTN_ADD",              -- 加
    "IMAGE_EDIT",           -- 编辑
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function ItemBuyLayer:ctor( scene, param, level )
    --网络回调
    local shopDetailCallBack = function(result,message)
        return self:onShopDetailCallBack(result,message)
    end
    --网络处理
    self._shopDetailFrame = ShopDetailFrame:create(self,shopDetailCallBack)

    ItemBuyLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/ItemBuyLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )


    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

        -- 光
    --self.m_spLight = spbg:getChildByName("sp_light")

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 购买
    btn = spbg:getChildByName("btn_buy")
    btn:setTag(TAG_ENUM.BTN_BUY)
    btn:addTouchEventListener( touchFunC )
    self.m_btnBuy = btn
    self.m_btnBuy:setEnabled(false)

    -- 减
    btn = spbg:getChildByName("btn_sub")
    btn:setTag(TAG_ENUM.BTN_SUB)
    btn:addTouchEventListener( touchFunC )

    -- 加
    btn = spbg:getChildByName("btn_add")
    btn:setTag(TAG_ENUM.BTN_ADD)
    btn:addTouchEventListener( touchFunC )

    -- 消耗
    self.m_txtConsume = spbg:getChildByName("txt_count")
    self.m_txtConsume:setString("0")
    -- 是否编辑输入
    self.m_bEditInput = false

    -- 购买数量
    self.m_txtCount = spbg:getChildByName("txt_buycount")
    self.m_txtCount:setString("0")


    -- 编辑按钮
    btn = spbg:getChildByName("shop_image_editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    btn:setTouchEnabled(false)
    self.m_imageEdit = btn

    -- 钻石不足提示
    self.m_txtTips = spbg:getChildByName("txt_tips")
    self.m_txtTips:setVisible(false)

    --剩余钻石数目
    self.m_txtLeft = spbg:getChildByName("txt_left_num")
    self.m_txtLeft:setString(GlobalUserItem.nLargeTrumpetCount)

    self._propertyInfo = nil
    --查找道具信息
    for i=1, #GlobalUserItem.tabPropertyItem do
        if 306 == GlobalUserItem.tabPropertyItem[i].dwPropID and 0 == GlobalUserItem.tabPropertyItem[i].bExchangeType then
            self._propertyInfo = GlobalUserItem.tabPropertyItem[i]
            break
        end
    end
    dump(GlobalUserItem.tabPropertyItem, "onSubRoomListInfo", 7)

    -- 购买数量
    self.m_nInputCount = 0
    self.m_nInputCount = 1--self.m_nInputCount + GlobalUserItem.tabSystemConfig.DiamondBuyPropCount
    if nil ~= self._propertyInfo then
        self:onUpdateNum()
    end
    

    -- 加载动画
    self:scaleShakeTransitionIn(self.m_spBg)

    if nil == self._propertyInfo then
        self:scaleTransitionOut(self.m_spBg)
    end
end

-- 按键监听
function ItemBuyLayer:onButtonClickedEvent(tag,sender)
    --[[local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end]]
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SUB == tag then
        if self.m_nInputCount > 1 then
            self.m_nInputCount = self.m_nInputCount - 1 --GlobalUserItem.tabSystemConfig.DiamondBuyPropCount
            --self.m_editBox:setText( self.m_nInputCount .. "" )
            self:onUpdateNum()
        end
    elseif TAG_ENUM.BTN_ADD == tag then
        if self.m_nInputCount < 9998 then
            self.m_nInputCount = self.m_nInputCount + 1 --GlobalUserItem.tabSystemConfig.DiamondBuyPropCount
            --self.m_editBox:setText( self.m_nInputCount .. "" )
            self:onUpdateNum()
        end
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        --self.m_editBox:setVisible(true)
        --self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    elseif TAG_ENUM.BTN_BUY == tag then
        if type(self.m_nInputCount) ~= "number" or self.m_nInputCount <= 0 then
            showToast(self, "请输入合法的购买数量", 1)
            return
        end
        self:showPopWait()
        self._shopDetailFrame:onPropertyBuy(self.m_nInputCount, yl.LARGE_TRUMPET, 0)
    end
end

function ItemBuyLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ItemBuyLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ItemBuyLayer:onTransitionInFinish()
    -- 编辑框
    --[[local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 60, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入购买数量")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setFontColor(cc.c4b(247,252,117,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setMaxLength(4)
    self.m_spBg:addChild(editbox)
    self.m_editBox = editbox
    self.m_editBox:setVisible(false)      
    
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_imageEdit:setTouchEnabled(true)]]
    self.m_btnBuy:setEnabled(true)
end

function ItemBuyLayer:onTransitionOutBegin()
    --self.m_spLight:setVisible(false)
    self:sendHideEvent()
end

function ItemBuyLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function ItemBuyLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtCount:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "return" then
        local sztxt = editbox:getText()
        local ndst = tonumber(sztxt)
        if "number" == type(ndst) then
            self.m_nInputCount = ndst
        end
        if "" == sztxt then
            self.m_nInputCount = 0
        end
        editbox:setVisible(false)
        self.m_imageEdit:setEnabled(true)
        self:onUpdateNum()
    end
end

function ItemBuyLayer:onExit()
    if nil ~= self._shopDetailFrame then
        self._shopDetailFrame:onCloseSocket()
        self._shopDetailFrame = nil
    end
    ItemBuyLayer.super.onExit(self)
end

-- 更新购买数量
function ItemBuyLayer:onUpdateNum()
    if nil == self._propertyInfo then
        showToast(self, "找不到道具信息！", 2)
        return
    end
    self.m_txtCount:setVisible(true)
    if 0 == self.m_nInputCount then
        self.m_txtCount:setString("0")
    else
        --self.m_txtCount:setString(string.formatNumberThousands( self.m_nInputCount, true, ""))
        self.m_txtCount:setString(self.m_nInputCount)
    end    


    local nConsume = self.m_nInputCount * self._propertyInfo.dwExchangeRatio --/ GlobalUserItem.tabSystemConfig.DiamondBuyPropCount
    local enableBuy = nConsume <= GlobalUserItem.tabAccountInfo.lDiamond
    self.m_txtTips:setVisible(not enableBuy)
    self.m_btnBuy:setEnabled(enableBuy)
    if enableBuy then
        self.m_btnBuy:setOpacity(255)
    else
        self.m_btnBuy:setOpacity(200)
    end
    self.m_txtConsume:setString(nConsume .. "")

    --刷新大喇叭数量
    self.m_txtLeft:setString(GlobalUserItem.nLargeTrumpetCount)
end

-- 购买回调
function ItemBuyLayer:onShopDetailCallBack(result,message)
    local runScene = cc.Director:getInstance():getRunningScene()
    if nil ~= runScene then
        showToast(runScene, message, 2)
    end

    self:dismissPopWait()

    -- 刷新数量
    self.m_nInputCount = 1
    self:onUpdateNum()

    self:runAction(cc.Sequence:create(cc.DelayTime:create(1.5), cc.CallFunc:create(function ()
        self:scaleTransitionOutAction(self.m_spBg)
    end)))
    
end

return ItemBuyLayer