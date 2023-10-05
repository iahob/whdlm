--
-- Author: zhong
-- Date: 2017-05-25 17:08:11
--
-- 包含(商店界面-ShopLayer、 商店充值界面-ShopChargeLayer)

-- 商店界面(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ShopLayer = class("ShopLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

-- 商店充值(三级弹窗)
local ShopChargeLayer = class("ShopChargeLayer", TransitionLayer)
local LingQianPay = class("LingQianPay", TransitionLayer)
local ShopChargeOnlineLayer = class("ShopChargeOnlineLayer", TransitionLayer)

-- ui
local SpreadLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "SpreadLayer")             -- 绑定
local JFTPay = class("JFTPay", TransitionLayer)


local url = yl.HTTP_URL  .. "/shop/dist/#/?userid=" .. GlobalUserItem.tabAccountInfo.dwUserID

-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_WX",               -- 微信充值
    "BTN_ALIPAY",           -- 支付宝充值
    "BTN_JFPAY",            -- 竣付通
    "BTN_LQ",               -- 零钱
    "BTN_ONLINE",           -- 在线支付
   "BTN_JFT",  		    --竣付通
    "CBT_SWITCH",           -- 切换
    "BT_SHOP",              --商城
    "BT_EXCHANGE",          -- 金币兑换
    "BT_TICKET",            -- 奖券
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
-- 普通
local NORAML_PRODUCT = 0
-- 推荐
local RECOMMAND_PRODUCT = 1
-- 首充
local FIRSTPAY_PRODUCT = 2
local PAYTYPE = {}
PAYTYPE[TAG_ENUM.BTN_WX] =
{
    str = "wx",
    plat = yl.ThirdParty.WECHAT
}
PAYTYPE[TAG_ENUM.BTN_ALIPAY] =
{
    str = "zfb",
    plat = yl.ThirdParty.ALIPAY
}
PAYTYPE[TAG_ENUM.BTN_JFPAY] =
{
    str = "jft",
    plat = yl.ThirdParty.JFT
}
PAYTYPE[TAG_ENUM.BTN_LQ] =
{
    str = "lq",
    plat = yl.ThirdParty.LQ
}

PAYTYPE[TAG_ENUM.BTN_JFT] =
{
    str = "jft",
    plat = yl.ThirdParty.JFT
}


-- 金币兑换
ShopLayer.GOLD_EXCHANGE = 1
-- 钻石充值
ShopLayer.DIAMOND_CHARGE = 2

-- 兑换
ShopLayer.EXCHANGE =  3

local posConfig = 
{
    {{0.5},},       -- 1
    {{0.3},{0.7}},      -- 2
    {{0.20},{0.5},{0.8}},      -- 3
    {{0.20},{0.40},{0.60},{0.80}},      -- 4
}


--零钱支付页面
local LQ_RETURN = 3

function JFTPay:ctor(parent)
    self.m_parent = parent
    JFTPay.super.ctor( self, scene, param, level )

    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/JFTPay.csb", self)

    --背景
    local bg = csbNode:getChildByName("pay_bg")
    local bgsize = bg:getContentSize()


    --按钮回调
    local btcallback = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --返回按钮
    local btn = bg:getChildByName("btn_return")
    btn:setTag(LQ_RETURN)
    btn:addTouchEventListener(btcallback)

    --微信支付
    btn = bg:getChildByName("lq_pay1")
    btn:addTouchEventListener(btcallback)
    btn:setTag(1)

    --支付宝支付
    btn = bg:getChildByName("lq_pay2")
    btn:addTouchEventListener(btcallback)
    btn:setTag(2)
end

function JFTPay:onTouchBegan(touch, event)
    return self:isVisible()
end

function JFTPay:onExit()
 
end

function JFTPay:onButtonClickedEvent(tag, sender)
    print(tag)
    if tag == LQ_RETURN then
        --重新请求支付列表
        --self.m_parent:reloadBeanList()
        self.m_parent.m_nSelect = nil
        self.m_parent:setVisible(true)
        self:removeFromParent()
    else
        local plat = 0
        local str = ""
        if 1 == tag then
            self.m_parent.scene.m_nSubtype = "wx"
        elseif 2 == tag then
            self.m_parent.scene.m_nSubtype = "zfb"
        end

        print("JFTPay", self.m_parent.m_nSelect, type(self.m_parent._param) == "function", self.m_parent.scene.m_nSubtype)
        if nil ~= self.m_parent.m_nSelect and nil ~= PAYTYPE[self.m_parent.m_nSelect] and type(self.m_parent._param) == "function" then
            self.m_parent._param(PAYTYPE[self.m_parent.m_nSelect].plat, PAYTYPE[self.m_parent.m_nSelect].str)
        end
        print("******",PAYTYPE[self.m_parent.m_nSelect].plat, PAYTYPE[self.m_parent.m_nSelect].str)
        self.m_parent:removeFromParent()
        self:removeFromParent()
    end
end


function LingQianPay:ctor(parent)
    self.m_parent = parent
    LingQianPay.super.ctor( self, scene, param, level )

    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/LingQianPay.csb", self)
    --local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/ShopChargeLayer.csb", self)
    print("LingQianPay !!!!!!!!!!!!!!!!!!!", rootLayer, csbNode)

    --背景
    local bg = csbNode:getChildByName("pay_bg")
    local bgsize = bg:getContentSize()


    --按钮回调
    local btcallback = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --返回按钮
    local btn = bg:getChildByName("btn_return")
    btn:setTag(LQ_RETURN)
    btn:addTouchEventListener(btcallback)

    --微信支付
    btn = bg:getChildByName("lq_pay1")
    btn:addTouchEventListener(btcallback)
    btn:setTag(1)

    --支付宝支付
    btn = bg:getChildByName("lq_pay2")
    btn:addTouchEventListener(btcallback)
    btn:setTag(2)

end

function LingQianPay:onTouchBegan(touch, event)
    return self:isVisible()
end

function LingQianPay:onExit()
 
end

function LingQianPay:onButtonClickedEvent(tag, sender)
    print(tag)
    if tag == LQ_RETURN then
        --重新请求支付列表
        --self.m_parent:reloadBeanList()
        self.m_parent.m_nSelect = nil
        self.m_parent:setVisible(true)
        self:removeFromParent()
    else
        local plat = 0
        local str = ""
        if 1 == tag then
            self.m_parent.scene.m_nSubtype = "wx"
        elseif 2 == tag then
            self.m_parent.scene.m_nSubtype = "zfb"
        end

        print("LingQianPay", self.m_parent.m_nSelect, type(self.m_parent._param) == "function", self.m_parent.scene.m_nSubtype)
        if nil ~= self.m_parent.m_nSelect and nil ~= PAYTYPE[self.m_parent.m_nSelect] and type(self.m_parent._param) == "function" then
            self.m_parent._param(PAYTYPE[self.m_parent.m_nSelect].plat, PAYTYPE[self.m_parent.m_nSelect].str)
        end
        self.m_parent:removeFromParent()
        self:removeFromParent()
    end
end


function ShopChargeLayer:ctor( scene, param, level )
    ShopChargeLayer.super.ctor( self, scene, param, level )

    self.scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/ShopChargeLayer.csb", self)
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
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 微信充值
    btn = spbg:getChildByName("btn_chanel_2")
    btn:setTag(TAG_ENUM.BTN_WX)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    self.m_btnWxChanel = btn

    -- 支付宝充值
    btn = spbg:getChildByName("btn_chanel_1")
    btn:setTag(TAG_ENUM.BTN_ALIPAY)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    self.m_btnAlipayChanel = btn

    btn = spbg:getChildByName("btn_chanel_3")
    btn:setTag(TAG_ENUM.BTN_ONLINE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)
    
    --竣付通支付
    btn = spbg:getChildByName("btn_chanel_4")
    btn:setTag(TAG_ENUM.BTN_JFT)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setVisible(false)

    -- 支付渠道
    local chanelMask = GlobalUserItem.tabSystemConfig.PayChannel or 0
    local idxList = {}
    -- 开启微信
    if 1 == bit:_and(chanelMask, 1) then
        table.insert(idxList, 2)
    end

    -- 开启支付宝
    if 2 == bit:_and(chanelMask, 2) then
        table.insert(idxList, 1)
    end

    --开启零钱支付
    if 3 == bit:_and(chanelMask, 3) then
        table.insert(idxList, 3)
    end
    
    --开启俊付通
    if 4 == bit:_and(chanelMask, 4) then
         table.insert(idxList, 4)
     end

    local nCount = #idxList
    local tabPos = posConfig[nCount]
    if nil ~= tabPos then
        for k,v in pairs(idxList) do
            local xPos = tabPos[k][1]
            -- 按钮
            local btn = spbg:getChildByName("btn_chanel_" .. v)
            if nil ~= btn then
                btn:setVisible(true)
                btn:setPositionX(xPos * spbg:getContentSize().width)
            end
        end
    end

    self.m_nSelect = nil

    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function ShopChargeLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_WX == tag 
        or TAG_ENUM.BTN_ALIPAY == tag then

        -- local str = "无法支付"
        -- local plat = PAYTYPE[tag].plat
        -- if yl.ThirdParty.WECHAT == plat then
        --     str = "微信未安装,无法进行微信支付"
        -- elseif yl.ThirdParty.ALIPAY == plat then
        --     str = "支付宝未安装,无法进行支付宝支付"
        -- end
        -- --判断应用是否安装
        -- if false == MultiPlatform:getInstance():isPlatformInstalled(plat) and plat ~= yl.ThirdParty.JFT then
        --     showToast(self, str, 2, cc.c4b(250,0,0,255))
        --     return
        -- end

        -- if TAG_ENUM.BTN_WX == tag then
        --     self.scene.m_nSubtype = "wx"
        -- else
        --     self.scene.m_nSubtype = "zfb"
        -- end
        self.m_nSelect = tag
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_LQ == tag then
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        local lqPay = LingQianPay:create(self)
        if nil == lqPay then 
            print("fail!!!!!!!!!!!!!!!!!")
            return
        end
        self._scene:addChild(lqPay)
        self:setVisible(false)
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        self.m_nSelect = TAG_ENUM.BTN_LQ
    elseif TAG_ENUM.BTN_ONLINE == tag then
        -- 显示在线充值
        local sl = ShopChargeOnlineLayer:create(self._scene, {}, self:getLevel() + 1)
        self._scene:addPopLayer(sl)
    elseif TAG_ENUM.BTN_JFT==tag then
        local lqPay = JFTPay:create(self)
        if nil == lqPay then 
            print("fail!!!!!!!!!!!!!!!!!")
            return
        end
        self._scene:addChild(lqPay)
        self:setVisible(false)
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        self.m_nSelect = TAG_ENUM.BTN_JFT
    end
end

function ShopChargeLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ShopChargeLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ShopChargeLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ShopChargeLayer:onTransitionOutFinish()
    -- 
    if nil ~= self.m_nSelect and nil ~= PAYTYPE[self.m_nSelect] and type(self._param) == "function" then
        self._param(PAYTYPE[self.m_nSelect].plat, PAYTYPE[self.m_nSelect].str)
    end
    self:removeFromParent()
end


--在线充值界面
function ShopChargeOnlineLayer:ctor( scene, param, level )
    ShopChargeOnlineLayer.super.ctor( self, scene, param, level )

    self.scene = scene
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/ShopChargeOnlineLayer.csb", self)
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
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.m_tabList = {}

        -- 按钮
    self.m_itemFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30
                or nil == self._listView then
                return
            end
            -- widget add 到 tableview 会在非可见区域触发touch事件
            local toViewPoint = self._listView:convertToNodeSpace(endPos)
            local rec = cc.rect(0, 0, self._listView:getViewSize().width, self._listView:getViewSize().height)
            if true == cc.rectContainsPoint(rec, toViewPoint) then
                self:onItemButtonClickedEvent(ref:getTag(), ref)  
            end  
        end
    end

    -- 列表
    local tmp = spbg:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = tmpsize.width
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(spbg)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tmp:removeFromParent()

    self._listView:reloadData()



    self:onGetOnlineServerList()

    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function ShopChargeOnlineLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function ShopChargeOnlineLayer:onItemButtonClickedEvent( tag, ref )
    local serverInfo = ref.serverInfo
    if nil == serverInfo  then
        print("信息为空")
        return
    end
    MultiPlatform:getInstance():copyToClipboard(serverInfo.WeChat .. "")
    
    self:runAction(cc.Sequence:create(cc.DelayTime:create(2), cc.CallFunc:create(function()
        MultiPlatform:getInstance():openBrowser("weixin://")
    end)))
end

function ShopChargeOnlineLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ShopChargeOnlineLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ShopChargeOnlineLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function ShopChargeOnlineLayer:onTransitionOutFinish()
    -- 
    if nil ~= self.m_nSelect and nil ~= PAYTYPE[self.m_nSelect] and type(self._param) == "function" then
        self._param(PAYTYPE[self.m_nSelect].plat, PAYTYPE[self.m_nSelect].str)
    end
    self:removeFromParent()
end

function ShopChargeOnlineLayer:cellSizeForTable(view, idx)
    return self.m_nCellSize , 210
end

function ShopChargeOnlineLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    --取四个
    for i = 1, 4 do
        local serverInfo = self.m_tabList[idx *4 + i]
        if nil == serverInfo then
            return cell
        end

        local csb = ExternalFun.loadCSB("shop/ShopOnlineCell.csb", cell)
        csb:setPosition(self.m_nCellSize /4 * i - self.m_nCellSize /8 , 105)

        
        local bg = csb:getChildByName("bg")

        local btn = bg:getChildByName("btn_copy")
        btn:addTouchEventListener(self.m_itemFunC)
        btn.serverInfo = serverInfo

        --chanl
        local txtChanl = bg:getChildByName("txtChanl")
        if nil ~= txtChanl then
            txtChanl:setString(serverInfo.ConfigName)
        end
        --昵称
        local txtName = bg:getChildByName("txtName")
        if nil ~= txtName then
            local strNickname = string.EllipsisByConfig(serverInfo.NickName, 200, 
                                                 string.getConfig("fonts/FZHTJW.TTF", 20))
            txtName:setString(strNickname)
        end
    end
    return cell

end

-- 子视图数目
function ShopChargeOnlineLayer:numberOfCellsInTableView(view)
    if 0 < #self.m_tabList and #self.m_tabList < 4 then
        return 1
    end
    return math.ceil(#self.m_tabList/4)
end

function ShopChargeOnlineLayer:onGetOnlineServerList()
    local data, sign = GlobalUserItem:useridSign(GlobalUserItem.tabAccountInfo.dwUserID)
    local action = string.format("action=getonlinewechatlist&userid=%s&time=%s&sign=%s", GlobalUserItem.tabAccountInfo.dwUserID, data, sign)
    -- print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    -- local jsonStr = cjson.encode(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    -- LogAsset:getInstance():logData(jsonStr, true)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "onGetOnlineServerList", 7)
        if type(jstable) == "table" then
            local data = jstable["data"]
            self.m_tabList = {}
            local list = data["List"]
            if type(list) == "table" then
                for i=1,#list do
                    local info = list[i]
                    local serverInfo = {}
                    serverInfo.ConfigID = tonumber(info["ConfigID"])
                    serverInfo.ConfigName = info["ConfigName"]
                    serverInfo.WeChat = info["WeChat"]
                    serverInfo.NickName =  info["NickName"]
                    serverInfo.SortID =  tonumber(info["SortID"])
                    serverInfo.TagID =  tonumber(info["TagID"])
                    serverInfo.Nullity =  tonumber(info["Nullity"])
                    serverInfo.CollectDate = info["CollectDate"]

                    table.insert(self.m_tabList, serverInfo)
                    dump(self.m_tabList, "tableInfo", 6)
                end
            end
            --排序
            table.sort(self.m_tabList, function (a, b)
                return a.SortID < b.SortID
            end)

            self._listView:reloadData()
            --self:onRefreshInfo()
        end
    end)
end



function ShopLayer:ctor( scene, param, level )
    local this = self
    ShopLayer.super.ctor( self, scene, param, level )
    -- 当前动作
    self.m_nAction = self._param or ShopLayer.DIAMOND_CHARGE

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("shop/ShopLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    self:setName("_ShopLayer_")
    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 钻石
    local tmptxt = spbg:getChildByName("txt_count")
    local lScore = GlobalUserItem.tabAccountInfo.lDiamond
    if lScore < 10000 then
        tmptxt:setString(lScore)
    elseif lScore >= 10000 and lScore < 100000000 then
        local szscore = string.format("%.2f", lScore / 10000)
        tmptxt:setString(szscore.."万")
    else
        local szscore = string.format("%.2f", lScore / 100000000)
        tmptxt:setString(szscore.."亿")
    end
    self.m_clipDiamond = tmptxt

    -- local clipdiamond = ClipText:createClipText(tmptxt:getContentSize(), GlobalUserItem.tabAccountInfo.lDiamond .. "", nil, 26)
    -- clipdiamond:setTextColor(cc.c4b(255, 242, 109, 255))
    -- clipdiamond:setAnchorPoint(tmptxt:getAnchorPoint())
    -- clipdiamond:setPosition(tmptxt:getPosition())
    -- spbg:addChild(clipdiamond)
    -- self.m_clipDiamond = clipdiamond
    -- tmptxt:removeFromParent()

    -- 金币
    tmptxt = spbg:getChildByName("txt_count_2")
    local lScore = GlobalUserItem.tabAccountInfo.lUserScore
    if lScore < 10000 then
        tmptxt:setString(lScore)
    elseif lScore >= 10000 and lScore < 100000000 then
        local szscore = string.format("%.2f", lScore / 10000)
        tmptxt:setString(szscore.."万")
    else
        local szscore = string.format("%.2f", lScore / 100000000)
        tmptxt:setString(szscore.."亿")
    end
    self.m_clipCoin = tmptxt

    --奖券
    tmptxt = spbg:getChildByName("txt_count_3")
    local lScore = GlobalUserItem.tabAccountInfo.nAwardsTickets
    if lScore < 10000 then
        tmptxt:setString(lScore)
    elseif lScore >= 10000 and lScore < 100000000 then
        local szscore = string.format("%.2f", lScore / 10000)
        tmptxt:setString(szscore.."万")
    else
        local szscore = string.format("%.2f", lScore / 100000000)
        tmptxt:setString(szscore.."亿")
    end
    self.m_clipTicket = tmptxt

    

    -- local clipcoin = ClipText:createClipText(tmptxt:getContentSize(), GlobalUserItem.tabAccountInfo.lUserScore .. "", nil, 26)
    -- clipcoin:setTextColor(cc.c4b(255, 242, 109, 255))
    -- clipcoin:setAnchorPoint(tmptxt:getAnchorPoint())
    -- clipcoin:setPosition(tmptxt:getPosition())
    -- spbg:addChild(clipcoin)
    -- self.m_clipCoin = clipcoin
    -- tmptxt:removeFromParent()

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 钻石
    btn = spbg:getChildByName("shop_sp_diamondbg")
    btn:setTag(TAG_ENUM.BT_SHOP)
    btn:addTouchEventListener(touchFunC)

    -- 金币
    btn = spbg:getChildByName("shop_sp_diamondbg_0")
    btn:setTag(TAG_ENUM.BT_EXCHANGE)
    btn:addTouchEventListener(touchFunC)

     -- 奖券
    btn = spbg:getChildByName("shop_sp_ticket")
    btn:setTag(TAG_ENUM.BT_TICKET)
    btn:addTouchEventListener(touchFunC)
    btn:setEnabled(GlobalUserItem.tabSystemConfig.IsOpenRedemptionCenter == 0)

    -- 切换
    local checkboxlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    -- 切换
    -- local cbtbox = spbg:getChildByName("check_switch")
    -- cbtbox:addEventListener(checkboxlistener)
    -- cbtbox:setSelected(self.m_nAction == ShopLayer.GOLD_EXCHANGE)
    -- self.m_checkBox = cbtbox

    --宝石购买
    local cbtbox = spbg:getChildByName("check_diamond")
    cbtbox:addEventListener(checkboxlistener)
    cbtbox:setSelected(self.m_nAction == ShopLayer.DIAMOND_CHARGE)
    cbtbox:setTag(ShopLayer.DIAMOND_CHARGE)
    self.m_checkBox1 = cbtbox

    --金币购买
    local cbtbox = spbg:getChildByName("check_gold")
    cbtbox:addEventListener(checkboxlistener)
    cbtbox:setTag(ShopLayer.GOLD_EXCHANGE)
    cbtbox:setSelected(self.m_nAction == ShopLayer.GOLD_EXCHANGE)
    self.m_checkBox = cbtbox
    
     --兑换
    local cbtbox = spbg:getChildByName("check_exchange")
    cbtbox:addEventListener(checkboxlistener)
    cbtbox:setTag(ShopLayer.EXCHANGE)
    cbtbox:setSelected(self.m_nAction == ShopLayer.EXCHANGE)
    self.m_checkBoxExchange = cbtbox
    cbtbox:setVisible(GlobalUserItem.tabSystemConfig.IsOpenRedemptionCenter == 0)


    local m_actAni = ExternalFun.loadTimeLine("shop/ShopLayer.csb")
    m_actAni:gotoFrameAndPlay(0,true)
    self:runAction(m_actAni)


    self.m_tabDiamondList = {}
    self.m_tabGoldList = {}
    self.m_tabList = {}
    self.m_nListCount = 0
    -- 列表
    local tmp = spbg:getChildByName("content")
    local tmpsize = tmp:getContentSize()
    self.m_nCellSize = tmpsize.width / 4
    self._listView = cc.TableView:create(tmpsize)
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    --self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(tmp)
    --self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    --self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    --tmp:removeFromParent()
    self._listView:reloadData()
    self._shopNode = tmp

    --兑换界面
    local nodeExchange = spbg:getChildByName("content_exchange")
    nodeExchange:setVisible(false)

    --创建webview
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        self.m_webView = ccexp.WebView:create()
        nodeExchange:addChild(self.m_webView)
        self.m_webView:setPosition(cc.p(500, 300))
        self.m_webView:setContentSize(nodeExchange:getContentSize())
        self.m_webView:setScalesPageToFit(true)
        

        self.m_webView:loadURL(url)

        ExternalFun.visibleWebView(self.m_webView, false)
        self._scene:showPopWait()

        self.m_webView:setOnDidFailLoading(function ( sender, url )
            self._scene:dismissPopWait()
            print("open " .. url .. " fail")
        end)
        self.m_webView:setOnShouldStartLoading(function(sender, url)
            print("onWebViewShouldStartLoading, url is ", url)          
            return true
        end)
        self.m_webView:setOnDidFinishLoading(function(sender, url)
            print("setOnDidFinishLoading, url is ", url)
            self._scene:dismissPopWait()
            --ExternalFun.visibleWebView(self.m_webView, true)
        end)
    end

    self._exchangeNode = nodeExchange

    -- 注册事件监听
    --self:registerEventListenr()
	
	    -- 事件监听
    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)

    self._scene.m_nSubtype = ""


    self:onSelectedEvent(self.m_nAction)
    -- 加载动画
    self:scaleTransitionIn(spbg)
end

-- 按键监听
function ShopLayer:onButtonClickedEvent(tag,sender)
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:removeFromParent()
    elseif TAG_ENUM.BT_SHOP == tag or TAG_ENUM.BT_EXCHANGE == tag or TAG_ENUM.BT_TICKET == tag then
        self.m_checkBox:setSelected(TAG_ENUM.BT_EXCHANGE == tag)
        self.m_checkBox1:setSelected(TAG_ENUM.BT_SHOP == tag)

        self.m_checkBoxExchange:setSelected(TAG_ENUM.BT_TICKET == tag)

        self._shopNode:setVisible(tag == TAG_ENUM.BT_SHOP or tag == TAG_ENUM.BT_EXCHANGE)
        self._exchangeNode:setVisible(TAG_ENUM.BT_TICKET == tag)
        ExternalFun.visibleWebView(self.m_webView, TAG_ENUM.BT_TICKET == tag)

        -- local selColor=cc.c3b(0xff,0xe1,0xa3)
        -- local unSelColor=cc.c3b(0xa6,0xa1,0xac)
        -- self.m_checkBox:getChildByName("Text"):setColor(self.m_checkBox:isSelected() and selColor or unSelColor)
        -- self.m_checkBox1:getChildByName("Text"):setColor(self.m_checkBox1:isSelected() and selColor or unSelColor)

        print("选择", self.m_checkBox:isSelected(),  #self.m_tabGoldList)
        if TAG_ENUM.BT_EXCHANGE == tag then
            self.m_tabList = self.m_tabGoldList
            self.m_nListCount = #self.m_tabGoldList

            self._listView:reloadData()
        elseif TAG_ENUM.BT_SHOP == tag then
            self.m_tabList = self.m_tabDiamondList
            self.m_nListCount = #self.m_tabDiamondList

            self._listView:reloadData()
        else
            if nil ~= self.m_webView then
                self.m_webView:loadURL(url)
                self.m_webView:reload()
            end
        end  
    end
end

function ShopLayer:onSelectedEvent(tag, sender)
    self.m_checkBox:setSelected(tag == ShopLayer.GOLD_EXCHANGE)
    self.m_checkBox1:setSelected(tag == ShopLayer.DIAMOND_CHARGE)
    self.m_checkBoxExchange:setSelected(tag == ShopLayer.EXCHANGE)

    self._shopNode:setVisible(tag == ShopLayer.DIAMOND_CHARGE or tag == ShopLayer.GOLD_EXCHANGE)
    self._exchangeNode:setVisible(tag == ShopLayer.EXCHANGE)
    ExternalFun.visibleWebView(self.m_webView, tag == ShopLayer.EXCHANGE)


    -- local selColor=cc.c3b(0xff,0xe1,0xa3)
    -- local unSelColor=cc.c3b(0xa6,0xa1,0xac)
    -- self.m_checkBox:getChildByName("Text"):setColor(self.m_checkBox:isSelected() and selColor or unSelColor)
    -- self.m_checkBox1:getChildByName("Text"):setColor(self.m_checkBox1:isSelected() and selColor or unSelColor)

    print("选择", self.m_checkBox:isSelected(),  #self.m_tabGoldList)
    if self.m_checkBox:isSelected() then
        self.m_tabList = self.m_tabGoldList
        self.m_nListCount = #self.m_tabGoldList

        self._listView:reloadData()
    elseif self.m_checkBox1:isSelected() then
        self.m_tabList = self.m_tabDiamondList
        self.m_nListCount = #self.m_tabDiamondList

        self._listView:reloadData()
    else
        if nil ~= self.m_webView then
            self.m_webView:loadURL(url)
            self.m_webView:reload()
        end
    end
end

function ShopLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function ShopLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function ShopLayer:onTransitionInFinish()
    local this = self
    -- 查询信息
    self:showPopWait()
    local action = "action=GetPayProduct&typeid=0&" .. GlobalUserItem:urlUserIdSignParam()
    --logFunc(action, true)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", action, function(jstable, jsdata)
        dump(jstable, "jstable", 6)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    local isFirstPay = data["isFirst"] or false
                    local list = data["list"]
                    if type(list) == "table" then
                        for k,v in pairs(list) do
                            -- 类型 0普通  1 推荐  2 首充
                            local identify = v.PayIdentity
                            if 1 == v.ScoreType then
                                table.insert(this.m_tabDiamondList, v)
                            elseif 0 == v.ScoreType then
                                table.insert(this.m_tabGoldList, v)
                            end
                        end
                        -- 排序(tableview降序显示)
                        table.sort(this.m_tabDiamondList, function(a, b)
                            if a.IsFirstPay then
                                return a > b
                            else
                                return ExternalFun.sortRule(a.SortID, b.SortID)
                            end
                        end)
                        -- 排序(tableview降序显示)
                        table.sort(this.m_tabGoldList, function(a, b)
                            if a.IsFirstPay then
                                return a > b
                            else
                                return ExternalFun.sortRule(a.SortID, b.SortID)
                            end
                        end)
                        -- dump(self.m_tabDiamondList, "self.m_tabDiamondList", 6)
                    end
                    -- 兑换比例(钻石兑换金币)
                    -- local goldlist = data["goldList"]
                    -- dump(goldlist, "goldlist", 6)
                    -- if type(goldlist) == "table" then
                    --     for k,v in pairs(goldlist) do
                    --         table.insert(this.m_tabGoldList, v)
                    --     end
                    --     -- 排序(tableview降序显示)
                    --     table.sort(this.m_tabGoldList, function(a, b)
                    --         return ExternalFun.sortRule(a.SortID, b.SortID)
                    --     end)
                    --     -- dump(self.m_tabDiamondList, "self.m_tabDiamondList", 6)
                    -- end
                    print("获取商城商品数目金币,钻石", #this.m_tabGoldList, #this.m_tabDiamondList)
                    if this.m_checkBox:isSelected() then
                        this.m_tabList = this.m_tabGoldList
                        this.m_nListCount = #this.m_tabGoldList
                    else
                        this.m_tabList = this.m_tabDiamondList
                        this.m_nListCount = #this.m_tabDiamondList
                    end
                    this._listView:reloadData()
                end
            end
        end
        if type(msg) == "string" and "" ~= msg then
            showToast(this, msg, 3)
        end
        this:dismissPopWait()
    end)
end

-- function ShopLayer:onTransitionOutBegin()
--     self:sendHideEvent()
-- end

-- function ShopLayer:onTransitionOutFinish()
--     self:removeFromParent()
-- end

-- function ShopLayer:onOtherShowEvent()
--     if self:isVisible() then
--         self:setVisible(false)
--     end
-- end

-- function ShopLayer:onOtherHideEvent()
--     if not self:isVisible() then
--         self:setVisible(true) 
--     end
-- end

function ShopLayer:tableCellTouched(view, cell)

end

-- 子视图大小
function ShopLayer:cellSizeForTable(view, idx)
    return self.m_nCellSize, view:getViewSize().height/2
end

local tabImageType = 
{
    "shop/shop_sp_diamondlarge_1.png", 
    "shop/shop_sp_diamondlarge_2.png", 
    "shop/shop_sp_diamondlarge_3.png", 
    "shop/shop_sp_diamondlarge_4.png",
    "shop/shop_sp_diamondlarge_5.png", 
    "shop/shop_sp_diamondlarge_6.png", 
    "shop/shop_sp_diamondlarge_7.png", 
    "shop/shop_sp_diamondlarge_8.png"
}

local tabGoldImageType = 
{
    "shop/shop_sp_goldlarge_1.png", 
    "shop/shop_sp_goldlarge_2.png", 
    "shop/shop_sp_goldlarge_3.png", 
    "shop/shop_sp_goldlarge_4.png",
    "shop/shop_sp_goldlarge_5.png", 
    "shop/shop_sp_goldlarge_6.png", 
    "shop/shop_sp_goldlarge_7.png", 
    "shop/shop_sp_goldlarge_8.png"
}
function ShopLayer:tableCellAtIndex(view, idx)
    print("重新绘制cell")
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local this = self
    print("获取商城商品数目", #self.m_tabList)
    dump(self.m_tabList,"商城商品", 10)
    --一排摆4个
    for i=1, 4 do
        local iteminfo = self.m_tabList[idx *4 + i]
        if nil == iteminfo then
            return cell
        end
        local csb = ExternalFun.loadCSB("shop/ShopItemNode.csb", cell)
        local imagecell = csb:getChildByName("image_bg")
        local imagesize = imagecell:getContentSize()
        csb:setPosition(self.m_nCellSize * (i -0.5), view:getViewSize().height * 0.25)
        imagecell:setSwallowTouches(false)
        imagecell.info = iteminfo
        imagecell:addTouchEventListener(function(ref, tType)
            if tType == ccui.TouchEventType.ended then
                local beginPos = ref:getTouchBeganPosition()
                local endPos = ref:getTouchEndPosition()
                if math.abs(endPos.x - beginPos.x) > 30 
                    or math.abs(endPos.y - beginPos.y) > 30 then
                    return
                end

                -- widget add 到 tableview 会在非可见区域触发touch事件
                local toViewPoint = view:convertToNodeSpace(endPos)
                local rec = cc.rect(0, 0, view:getViewSize().width, view:getViewSize().height)
                if true == cc.rectContainsPoint(rec, toViewPoint) then
                    if false then-- this.m_checkBox:isSelected() then
                        this:showPopWait()
                        -- 兑换
                        local action = string.format("action=diamondexchgold&typeid=0&configid=%d&%s", iteminfo.ConfigID, GlobalUserItem.urlUserIdSignParam())
                        --logFunc(action, true)
                        appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
                            dump(jstable, "desciption", 7)
                            local msg = ""
                            if type(jstable) == "table" then
                                msg = jstable["msg"]
                                local data = jstable["data"]
                                if type(data) == "table" then
                                    local valid = data["valid"]
                                    if valid then
                                        -- + 钻石
                                        local diamond = tonumber(data["afterDiamond"])
                                        if nil ~= diamond then
                                            GlobalUserItem.tabAccountInfo.lDiamond = diamond
                                        end
                                        -- + 金币(身上金币)
                                        local score = tonumber(data["afterScore"])
                                        if nil ~= score then
                                            GlobalUserItem.tabAccountInfo.lUserScore = score
                                        end

                                        --更新界面金钱
                                        local lScore = GlobalUserItem.tabAccountInfo.lDiamond
                                        if lScore < 10000 then
                                            this.m_clipDiamond:setString(lScore)
                                        elseif lScore >= 10000 and lScore < 100000000 then
                                            local szscore = string.format("%.2f", lScore / 10000)
                                            this.m_clipDiamond:setString(szscore.."万")
                                        else
                                            local szscore = string.format("%.2f", lScore / 100000000)
                                            this.m_clipDiamond:setString(szscore.."亿")
                                        end
                                        --this.m_clipDiamond:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
                                        local lScore = GlobalUserItem.tabAccountInfo.lUserScore
                                        if lScore < 10000 then
                                            this.m_clipCoin:setString(lScore)
                                        elseif lScore >= 10000 and lScore < 100000000 then
                                            local szscore = string.format("%.2f", lScore / 10000)
                                            this.m_clipCoin:setString(szscore.."万")
                                        else
                                            local szscore = string.format("%.2f", lScore / 100000000)
                                            this.m_clipCoin:setString(szscore.."亿")
                                        end
                                        --this.m_clipCoin:setString(GlobalUserItem.tabAccountInfo.lUserScore .. "")

                                        -- 通知更新        
                                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                                        eventListener.obj = yl.RY_MSG_USERWEALTH
                                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                                    end
                                end
                            end
                            if type(msg) == "string" and "" ~= msg then
                                showToast(this, msg, 1)
                            end
                            this:dismissPopWait()
                        end)
                    else
                        -- 充值
                        -- 充值是否需要推广
                        -- if GlobalUserItem.tabSystemConfig.IsPayBindSpread == yl.ENABLE_VALUE then
                        --     -- 判断是否绑定推广员
                        --     local noSpreader = GlobalUserItem.tabPlusInfo.szSpreaderAccount == ""
                        --     if noSpreader then
                        --         local bl = SpreadLayer.BindAccountLayer:create(this._scene, {}, this:getLevel() + 1)
                        --         this._scene:addPopLayer(bl)
                        --         return
                        --     end
                        -- end
                        local chanelMask = GlobalUserItem.tabSystemConfig.PayChannel or 0
                        if 0 == chanelMask then
                            showToast(this, "充值服务暂未开启!", 2)
                            return
                        end
                        
                        -- 显示充值
                        local sl = ShopChargeLayer:create(this._scene, function(selePart, szType)
                            this:doThirdpay(selePart, szType, imagecell.info)
                        end, this:getLevel() + 1)
                        this._scene:addPopLayer(sl)
                    end
                end
            end
        end)
        -- 商品名
        local name = ""
        -- 价格
        local szprice = ""
        -- 标识
        local flag = csb:getChildByName("sp_flag")
        -- 图片
        local imagetype = iteminfo["ImageType"] or 0
        local imagefile = ""
        -- 背景图
        local bgimage = "shop/shop_itembg.png"
        -- 商品图
        local itemimage = "shop/shop_sp_diamondicon.png"
        -- 价格图
        local priceimage = "shop/shop_sp_rmbicon.png"

        flag:setVisible(false)
        if 1== iteminfo["PayIdentity"] or 2 == iteminfo["PayIdentity"] then
            flag:setVisible(true)
            self:showFirstChargeAnimate(iteminfo["PayIdentity"], flag)
        end

        if this.m_checkBox:isSelected() then
            dump(iteminfo, "@@@@@@@@", 10)
            -- 兑换
            name = iteminfo["Score"] or 0 --iteminfo["PayName"] or ""
            if name < 10000 then

            elseif name >= 10000 and name < 100000000 then
                local szscore = string.format("%.2f", name / 10000)
                name = szscore.."万"
            else
                local szscore = string.format("%.2f", name / 100000000)
                name = szscore.."亿"
            end

            szprice = iteminfo["PayPrice"] or 0
            
            
            imagefile = tabGoldImageType[imagetype]
            itemimage = "shop/shop_sp_coinicon.png"
            priceimage = "shop/shop_sp_diamondicon.png"
        else
            -- 充值
            name = iteminfo["Score"] or 0 --iteminfo["PayName"] or ""
            local price = iteminfo["PayPrice"] or 0
            szprice = string.format("%.2f", price) 
            imagefile = tabImageType[imagetype]
        end
        -- 名称
        -- local txtitemname = csb:getChildByName("txt_itemname")
        -- txtitemname:setString("" .. name)

        


        local goldDescribe=csb:getChildByName("goldDescribeNode")
        local diamondDescribe=csb:getChildByName("diamondDescribeNode")
        local parentBg = csb:getChildByName("Sprite_2")

        if this.m_checkBox:isSelected() then
            diamondDescribe:setVisible(false)
            goldDescribe:setVisible(true)
            goldDescribe:getChildByName("txt_describe"):setString("" .. name)
            parentBg:getChildByName("Text_2"):setString("额外赠送        金币")
        else
            diamondDescribe:setVisible(true)
            goldDescribe:setVisible(false)
            diamondDescribe:getChildByName("txt_describe"):setString("" .. name .. "钻石")
            parentBg:getChildByName("Text_2"):setString("额外赠送        钻石")
        end
        print("name",name)
        
        if iteminfo["PresentScore"] == 0 then
            parentBg:setVisible(false)
        else
            parentBg:getChildByName("zuan"):setString(iteminfo["PresentScore"].."")      
        end

        -- 价格
        local altasprice = csb:getChildByName("txt_itemprice")
        
        altasprice:setString("¥" .. szprice)

        -- 按鈕
        csb:getChildByName("btn_price"):setSwallowTouches(false)
        -- 图片
        if nil ~= imagefile and cc.FileUtils:getInstance():isFileExist(imagefile) then
            csb:getChildByName("sp_image"):setSpriteFrame(cc.Sprite:create(imagefile):getSpriteFrame())
        end
        -- 背景
        if nil ~= bgimage and cc.FileUtils:getInstance():isFileExist(bgimage) then
            imagecell:loadTexture(bgimage)
        end
        -- 商品图
        if nil ~= itemimage and cc.FileUtils:getInstance():isFileExist(itemimage) then
            local spitem = csb:getChildByName("itembuy_sp_item")
            spitem:setSpriteFrame(cc.Sprite:create(itemimage):getSpriteFrame())
            spitem:setPositionX(-4.6 )--- txtitemname:getContentSize().width * 0.5)
        end
        -- 
        local spprice = csb:getChildByName("sp_itemprice")
        -- 位置调整
        if this.m_checkBox:isSelected() then
            --altasprice:setPositionX(30)
            --spprice:setPositionX( - altasprice:getContentSize().width * 0.5)
        end
        if nil ~= priceimage and cc.FileUtils:getInstance():isFileExist(priceimage) then
            spprice:setSpriteFrame(cc.Sprite:create(priceimage):getSpriteFrame())
        end
        if not this.m_checkBox:isSelected() then
            spprice:setPosition(cc.p(50, -78.88))
            --altasprice:setPosition(cc.p(-65, -78.88))
        end

    end

    return cell
end

-- 子视图数目
function ShopLayer:numberOfCellsInTableView(view)
    if 0 < self.m_nListCount and self.m_nListCount < 4 then
        return 1
    end
    return math.ceil(self.m_nListCount/4)
end

function ShopLayer:scrollViewDidScroll(view)
end

function ShopLayer:doThirdpay( part,szType, info )
    local this = self
    if nil == info or nil == szType or nil == info.ConfigID then
        print("ShopLayer:doThirdpay 充值产品为空 begin")
        dump(info, "info", 6)
        print("szType ==> ", szType)
        print("ShopLayer:doThirdpay 充值产品为空 end")
        return
    end
    self:showPopWait()
    self:runAction(cc.Sequence:create(cc.DelayTime:create(20), cc.CallFunc:create(function()
        this:dismissPopWait()
    end)))
    local configid = info.ConfigID
     local action = nil
    if part == yl.ThirdParty.LQ then
        action = string.format("action=createpayorder&paytype=%s&configid=%d&subtype=%s&%s", szType, configid, self._scene.m_nSubtype, GlobalUserItem:urlUserIdSignParam())
        print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    
    elseif part==yl.ThirdParty.JFT then
         local targetPlatform = cc.Application:getInstance():getTargetPlatform()
         local platN=(targetPlatform==cc.PLATFORM_OS_ANDROID) and 3 or 2

        action = string.format("action=createpayorder&paytype=%s&configid=%d&subtype=%s&terminaltype=%d&%s", szType, configid, self._scene.m_nSubtype,platN,GlobalUserItem:urlUserIdSignParam())
        print("action:",action)
    else
        action = string.format("action=createpayorder&paytype=%s&configid=%d&%s", szType, configid, GlobalUserItem:urlUserIdSignParam())
    end
    --logFunc(action, true)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "jstable", 6)
        local msg = nil
        if type(jstable) == "table" then
            msg = jstable["msg"]
            local data = jstable["data"]
            if type(data) == "table" then
                if nil ~= data["valid"] and true == data["valid"] then
                    local payparam = {}
                    if part == yl.ThirdParty.WECHAT then --微信支付
                        --获取微信支付订单id
                        local paypackage = data["PayPackage"]
                        if type(paypackage) == "string" then
                            local ok, paypackagetable = pcall(function()
                                return cjson.decode(paypackage)
                            end)
                            if ok then
                                local payid = paypackagetable["prepayid"]
                                if nil == payid then
                                    showToast(this, "微信支付订单获取异常", 2)
                                    this:dismissPopWait()
                                    return 
                                end
                                payparam["info"] = paypackagetable
                            else
                                showToast(this, "微信支付订单获取异常", 2)
                                this:dismissPopWait()
                                return
                            end
                        end
                    elseif part == yl.ThirdParty.LQ then
                        --获取微信支付订单id
                        local paypackage = data["PayUrl"]
                        if type(paypackage) == "string" then
                            payparam["info"] = data["PayUrl"]
                            local jsonStr = cjson.encode(payparam)
                            LogAsset:getInstance():logData(jsonStr, true)
                            print("@@@@@@@@@@@@@@@@@@@@@@@@@", payparam["info"])

                            -- local webview = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.WebViewLayer"):create(self, payparam["info"])
                            -- local runScene = cc.Director:getInstance():getRunningScene()
                            -- if nil ~= runScene then
                            --     runScene:addChild(webview, yl.ZORDER.Z_AD_WEBVIEW)
                            -- end
                            MultiPlatform:getInstance():openBrowser(payparam["info"])
                            this:dismissPopWait()
                            return
                        end
                     elseif part==yl.ThirdParty.JFT then    

                        if type(data["PayUrl"]) ~= "string" or data["PayUrl"]=="" or type(data["Params"])~="string" then return end

                        self._scene.OrderID=data["OrderID"]

                        local plat= MultiPlatform:getInstance():getSupportPlatform()
                       
                   
                         GlobalUserItem.bShopPay = true
                         
                    
                         print(data["Params"])
                    

                        LogAsset:getInstance():logData(data["PayUrl"])
                        LogAsset:getInstance():logData(data["Params"])
                        if plat==cc.PLATFORM_OS_WINDOWS then return end
                         local webView=ccexp.WebView:create()
                         webView:addTo(self)
                         webView:setName("JFT_WEBVIEW")
                         webView:browsePost(data["PayUrl"],data["Params"])
                         webView:setScalesPageToFit(false) 
                        


                        webView:setContentSize(self.m_spBg:getContentSize().width*0.9,self.m_spBg:getContentSize().height)
                        local x,y=self.m_spBg:getPosition() 
                        webView:setPosition(cc.p(x-40,y))

                        function addCloseBtn( )
                             if self.webviewBg and not tolua.isnull(self.webviewBg) then
                                return
                             end
                            local webviewBg=cc.LayerColor:create(cc.c4b(20, 20, 20, 250),yl.WIDTH,yl.HEIGHT)
                            self.webviewBg=webviewBg
                            webviewBg:addTo(self)
                            webviewBg:setName("JFT_WEBVIEWBG")
                            local closeBtn=ccui.Button:create( )
                            closeBtn:addTo(webviewBg)
                            closeBtn:setContentSize(cc.size(self.m_spBg:getContentSize().width*0.1,yl.HEIGHT))
                            closeBtn:setPosition(self.m_spBg:getContentSize().width*0.95,yl.HEIGHT*0.5)
                            closeBtn:addClickEventListener(function() 
                                if webView and not tolua.isnull(webView) then
                                     webView:removeSelf() 
                                end
                                if webviewBg and not tolua.isnull(webviewBg) then
                                    webviewBg:removeSelf() 
                                end
                            end) --
                            closeBtn:ignoreContentAdaptWithSize(false)

                            cc.Label:createWithTTF("点击右侧关闭", "fonts/round_body.ttf", 36)
                                :setTextColor(cc.c4b(255,221,65,255))
                                :setAnchorPoint(cc.p(0.5,0.5))
                                :setDimensions(30, 750)
                                :setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
                                :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
                                :move(closeBtn:getPosition())
                                :addTo(webviewBg)
                        end

                         webView:setOnDidFailLoading(function ( sender, url )
                                addCloseBtn()
                                self:dismissPopWait()
                            end)

                        webView:setOnDidFinishLoading(function(sender, url)
                                addCloseBtn()
                                self:dismissPopWait()
                            end)
                       
                        return
                    end
                    --订单id
                    payparam["orderid"] = data["OrderID"]                       
                    --价格
                    payparam["price"] = info.PayPrice
                    --商品名
                    payparam["name"] = info.PayName or ""
                    local function payCallBack(param)
                        local runScene = cc.Director:getInstance():getRunningScene()
                        if type(param) == "string" and "true" == param then
                            GlobalUserItem.setTodayPay()
                            
                            showToast(runScene, "支付成功", 2)
                            -- 充值
                            local dia = info.Diamond or 0
                            -- 赠送
                            local sendScale = info.PresentScale or 0
                            GlobalUserItem.tabAccountInfo.lDiamond = GlobalUserItem.tabAccountInfo.lDiamond + dia + dia * sendScale

                            if nil ~= this or nil ~= this.m_clipDiamond then
                                this.m_clipDiamond:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
                            end
                            -- 通知更新        
                            local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                            eventListener.obj = yl.RY_MSG_USERWEALTH
                            cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                        else
                            showToast(runScene, "支付异常", 2)
                        end
                    end
                    this:showPopWait()
                    GlobalUserItem.bShopPay = true
                    MultiPlatform:getInstance():thirdPartyPay(part, payparam, payCallBack)
                    return
                end
            end
        end
        if type(msg) == "string" and msg ~= "" then
            showToast(this, msg, 2)
        end
        this:dismissPopWait()
    end)
end

function ShopLayer:onUserInfoChange( event  )

      local wvBg=self:getChildByName("JFT_WEBVIEWBG")
      local wv=self:getChildByName("JFT_WEBVIEW")
      if wvBg then wvBg:removeSelf() end
      if wv then wv:removeSelf() end 

    print("----------userinfo change notify------------")
	local this = self
    local msgWhat = event.obj
    
    if nil ~= msgWhat then
        if  msgWhat == yl.RY_MSG_USERRECHARGE then
			--请求
			local url = yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx"
			this:showPopWait()
			local param = "action=getuserwealth&" .. GlobalUserItem:urlUserIdSignParam()
			appdf.onHttpJsionTable(url ,"GET",param,function(jstable,jsdata)
				this:dismissPopWait()
				if type(jstable) == "table" then
					msg = jstable["msg"]
					local data = jstable["data"]
					if type(data) == "table" then
						local valid = data["valid"]
						if valid then
							-- + 钻石
							local diamond = tonumber(data["diamond"])
							if nil ~= diamond then
								GlobalUserItem.tabAccountInfo.lDiamond = diamond
							end
							--更新界面金钱
							this.m_clipDiamond:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
							-- 通知更新        
							local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
							eventListener.obj = yl.RY_MSG_USERWEALTH
							cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
						end
					end
				end
			end)
		end
    end
end

--显示首充动画
function ShopLayer:showFirstChargeAnimate(type, sprFlag)
    if 2 == type then --每日首充
        --设置文理
        local szfile = "shop/shop_sp_firstflag.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            sprFlag:setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
            sprFlag:stopAllActions()
        end

        --动画
        local animation = cc.Animation:create()
        local times = 0.2
        for i=1, 13 do
            local strName = string.format("shop/shop_sp_firstflag%d.png", i)
             print("--------------------->", strName)
            local spriteFrame = cc.Sprite:create(strName):getSpriteFrame()
            if spriteFrame then
                animation:addSpriteFrame(spriteFrame)
            else
                break
            end
            animation:setLoops(-1)
            animation:setDelayPerUnit(times)
        end

        local animate = cc.Animate:create(animation)
        sprFlag:runAction(animate)
        --sprFlag:runAction(cc.RepeatForever:create(animate))
        -- sprFlag:runAction(cc.Spawn:create(animate, cc.Sequence:create(cc.DelayTime:create(2.5), cc.CallFunc:create(function()
        --     spr:removeFromParent()
        -- end))))
    elseif 1 == type then  --首充
        --设置文理
        local szfile = "shop/shop_sp_first1.png"
        if cc.FileUtils:getInstance():isFileExist(szfile) then
            sprFlag:setSpriteFrame(cc.Sprite:create(szfile):getSpriteFrame())
            sprFlag:stopAllActions()
        end

        --动画
        local animation = cc.Animation:create()
        local times = 0.2
        for i=1, 32 do
            local strName = string.format("shop/shop_sp_first%d.png", i)
             print("--------------------->", strName)
            local spriteFrame = cc.Sprite:create(strName):getSpriteFrame()
            if spriteFrame then
                animation:addSpriteFrame(spriteFrame)
            else
                break
            end
            animation:setLoops(-1)
            animation:setDelayPerUnit(times)
        end

        local animate = cc.Animate:create(animation)
        sprFlag:runAction(animate)
    end
end


return ShopLayer