--
-- Author: David
-- Date: 2018-07-17 
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local DailyBonusLayer = class("DailyBonusLayer", TransitionLayer)

-- net
local DailyBounsFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.DailyBounsFrame")                           -- 签到

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_FREEPRIZE",        -- 免费抽奖
    "TAG_BG",               -- 背景
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

local posGift = {cc.p(134, 482), cc.p(284, 482), cc.p(434, 482), cc.p(584, 482), cc.p(584, 310), cc.p(434, 310), cc.p(284, 310),  cc.p(134, 310),}
-- 
function DailyBonusLayer:ctor( scene, param, level )
    DailyBonusLayer.super.ctor( self, scene, param, level )
    
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/DailyBonusLayer.csb", self)
    self.m_rootLayer = rootLayer

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

        --网络回调
    local dailyCallBack = function(result, message, param)
        if nil ~= self.onDailyCallBack then
            self:onDailyCallBack(result, message, param)
        end
    end
    --网络处理
    self._dailyFrame = DailyBounsFrame:create(self, dailyCallBack)


    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:setTag(TAG_ENUM.TAG_BG)
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg


    -- 提示
    self.m_layTips = spbg:getChildByName("tipsbg")
    self.m_layTips:setVisible(false)


    -- 提示内容
    local txtDays = self.m_layTips:getChildByName("txtDays")



 
    -- 礼包
    self.m_tabImageGiftBox = {}
    -- 领取标识
    self.m_tabSpGotFlag = {}
    -- 天数
    self.m_tabTxtDay = {}
    --可领取标识
    self.m_tabSpCanGotFlag = {}

    self.m_spPrizeDayBg = spbg:getChildByName("prizedaybg")
    --设置文字颜色



    local boxFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onBoxClickedEvent(ref:getTag(), ref)            
        end
    end
    for i = 1, 6 do
        -- 礼盒
        local box = spbg:getChildByName("btn_prize" .. i)
        box:setTag(i)
        box:addTouchEventListener( boxFunC )
        self.m_tabImageGiftBox[i] = box
        self.m_tabImageGiftBox[i]:setEnabled(false)

        -- 领取标识
        local gotflag = spbg:getChildByName("boxget" .. i)
        gotflag:setVisible(false)
        self.m_tabSpGotFlag[i] = gotflag

        local cangotflag = spbg:getChildByName("boxcanget" .. i)
        cangotflag:setVisible(false)
        self.m_tabSpCanGotFlag[i] = cangotflag

        -- 文本
        self.m_tabTxtDay[i] = self.m_spPrizeDayBg:getChildByName("txt" .. i)
        self.m_tabTxtDay[i]:setVisible(false)
    end

    -- 关闭
    local btn = spbg:getChildByName("btn_close") 
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 免费领取
    btn = spbg:getChildByName("btn_freePrize")
    btn:setTag(TAG_ENUM.BTN_FREEPRIZE)
    btn:addTouchEventListener( touchFunC )
    self.m_btnFree = btn

 
    --日历
    local dateBg = spbg:getChildByName("datebg")
    --获取系统时间
    local week = os.date("%w");
    local dataWeek = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六",}
    local txtWeek = dateBg:getChildByName("txt_week")
    txtWeek:setString(dataWeek[week +1])

    local day = os.date("%d");
    local txtDay = dateBg:getChildByName("txt_day")
    txtDay:setString(day .. "")

    local data = os.date("%Y-%m-%d");
    local txtDate = dateBg:getChildByName("txt_date")
    txtDate:setString(data .. "")


    self.index = 1
    self.count = 0
    self.time = 0.5
    self.isVoice = true

    --请求签到配置信息
    self._dailyFrame:onGetDailyInfo()

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)

end

function DailyBonusLayer:showGiftAction(beginIndex, endIndex, tips)
    --获取光圈
    local spSelect = self.m_spBg:getChildByName("prizeSelect")
    spSelect:setVisible(true)

    local tabIndex = {1, 2, 3, 4, 8, 7, 6, 5}  --索引转化，ID转化为对应索引
    local RealEndIndex = tabIndex[endIndex]

    self.index = beginIndex
    if self.index > 8 then
        self.index = 1
        self.count = self.count +1
    end

    if self.count < 1 then
        self.time = self:getAddTime(self.time)
    elseif self.count > 3 and self.index > endIndex then
        self.time = self:getReduceTime(self.time)
    end
    
    local delay = cc.DelayTime:create(self.time)
    local call = cc.CallFunc:create(function()
        spSelect:setPosition(posGift[self.index])
            if self.isVoice then
                AudioEngine.playEffect("sound/startspin.mp3",false)
                self.isVoice = false
                self:runAction(cc.Sequence:create(cc.DelayTime:create(0.15),cc.CallFunc:create(function ( ... )
                    self.isVoice = true
                end)))
            end
            

        if self.count > 4  and self.index == RealEndIndex then
            self:showGetGift()
            if nil ~= tips then
                showToast(self, "恭喜获得".. tips, 2)
            end
            return
        end

        self:showGiftAction(self.index +1, endIndex, tips)
    end)

    self:runAction(cc.Sequence:create(delay,call))
end

function DailyBonusLayer:getReduceTime(timeCur)
    local a1 = timeCur
    local  time  = timeCur + 0.06
    return time
end

function DailyBonusLayer:getAddTime(timeCur)
    local a1 = timeCur
    local  time  = timeCur - 0.06
    return time
end

function DailyBonusLayer:showGetGift()
    --获取光圈
    self.index = 1
    self.count = 0
    self.time = 0.5

    self:stopAllActions()
    local spSelect = self.m_spBg:getChildByName("prizeSelect")
    spSelect:runAction(cc.Sequence:create(cc.Blink:create(3, 6), cc.Hide:create(), cc.CallFunc:create(function ( )
        spSelect:setPosition(posGift[1])
    end)))
end

-- 按键监听
function DailyBonusLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_FREEPRIZE == tag then
        --self:showGiftAction(1, 7)
        self.m_btnFree:setEnabled(false)
        self._dailyFrame:onGetPrizeDaily()
        --self._dailyFrame:onGetCumulativeRewards(1)
    elseif TAG_ENUM.TAG_BG == tag then
        --self.m_layTips:setVisible(false)
        local prizeDesBg = self.m_spBg:getChildByName("prizeDes")
        prizeDesBg:setVisible(false)
    end
end



-- 礼盒监听
function DailyBonusLayer:onBoxClickedEvent( tag,sender )

    local info = sender.info
    if type(info) ~= "table" then
        dump(sender.config, "desciption", 6)
        return
    end

    if sender.isGet or info.nNeedDay > GlobalUserItem.tabDailyInfo.wSeriesDate  then
        local txtContent = cc.Label:createWithSystemFont(info.szPackInfo, "Arial", 24, cc.size(264,0), cc.TEXT_ALIGNMENT_LEFT)
        txtContent:setLineBreakWithoutSpace(true)
        txtContent:setColor(cc.c3b(0xFF, 0xE4, 0x00))
        txtContent:setAnchorPoint(cc.p(0.5, 0))
        local adjHeight = txtContent:getContentSize().height +30

        local txtModel = cc.Label:createWithSystemFont(info.szPackInfo, "Arial", 24, cc.size(0,0), cc.TEXT_ALIGNMENT_LEFT)
        local adjWidth = txtModel:getContentSize().width + 30

        print("onBoxClickedEvent", adjHeight, adjWidth)

        local prizeDesBg = self.m_spBg:getChildByName("prizeDes")
        prizeDesBg:setVisible(true)
        prizeDesBg:removeChildByName("TxtDes")

        if adjWidth > 286 then
            adjWidth = 286
        end
        prizeDesBg:setContentSize(cc.size(adjWidth, adjHeight))

        local txt = prizeDesBg:getChildByName("txtDes")
        txt:setVisible(false)
        prizeDesBg:addChild(txtContent)
        txtContent:setName("TxtDes")
        txtContent:setPosition(txt:getPosition())

        prizeDesBg:setPositionX(sender:getPositionX() - 55)
    else
        --发送领奖
        self._dailyFrame:onGetCumulativeRewards(info.nPackageID)
    end

    -- local configid = info.ConfigID
    -- if nil == configid then
    --     print("DailyBonusLayer:onBoxClickedEvent configid为nil")
    --     return
    -- end
    --local needcount = info.SpreadNum or 0
    -- 未达到领取条件
    -- if self.m_nValidSpreadCount < needcount then
    --     showToast(self, "游戏推广好友未达到" .. needcount .. "人, 无法领取奖励!", 1)
    --     return
    -- end
end

function DailyBonusLayer:onTransitionInFinish()

end

function DailyBonusLayer:onTransitionOutFinish()
    if type(self.m_removeCallBack) == "function" then
        self.m_removeCallBack()
    end
    self:removeFromParent()
end

function DailyBonusLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function DailyBonusLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function DailyBonusLayer:onExit()
    if nil ~= self._dailyFrame then
        self._dailyFrame:onCloseSocket()
        self._dailyFrame = nil
    end
    DailyBonusLayer.super.onExit(self)
end

function DailyBonusLayer:refreshGiftBox()
    local fileUtil = cc.FileUtils:getInstance()
    --每日签到礼包
    --dump(GlobalUserItem.tabDailyRewards)
    for i=1, (#GlobalUserItem.tabDailyRewards > 8 and 8 or #GlobalUserItem.tabDailyRewards ) do
        local prize = self.m_spBg:getChildByName("prizebg" .. i)
        local prizeInfo = GlobalUserItem.tabDailyRewards[i]
        if prize then
            local txtDes = prize:getChildByName("txt")
            txtDes:setString(prizeInfo.szPackageName)
        end
        --签到礼包图片设置
        local this = self
        if prizeInfo.szImgUrl and prizeInfo.szImgUrl ~= "" then
            local imageName = ExternalFun.getFileName(prizeInfo.szImgUrl)
            print("refreshGiftBox", prizeInfo.szImgUrl, imageName)
            local path = string.format("%s/download/DailyBouns/%s", device.writablePath, imageName)
            if not fileUtil:isFileExist(path) then
                --downFileAsync(yl.IMAGE_URL .. url, imageName, string.format("%sclient/res/DailyBouns", device.writablePath), function(...)self:downLoadCallback(...) end)
                --调用C++下载
                downFileAsync(yl.IMAGE_URL .. prizeInfo.szImgUrl, imageName, string.format("%s/download/DailyBouns", device.writablePath), function(main,sub)
                    --下载回调
                    if main == appdf.DOWN_PRO_INFO then --进度信息

                    elseif main == appdf.DOWN_COMPELETED then --下载完毕
                        local sprGift = cc.Sprite:create(path)
                        if sprGift and tolua.cast(prize,"cc.Node") then
                            local spIcon = prize:getChildByName("sp")
                            spIcon:setSpriteFrame(sprGift:getSpriteFrame())
                        end
                    else
                        this._scene:dismissPopWait()
                        -- 下载失败
                        if nil ~= this._scene then
                            showToast(this._scene, "下载失败, 请重试!", 2)
                        end
                        --失败信息
                        print("下载失败 main, sub ", main, sub)
                    end
                end)
            else
                local sprGift = cc.Sprite:create(path)
                if sprGift then
                    local spIcon = prize:getChildByName("sp")
                    spIcon:setSpriteFrame(sprGift:getSpriteFrame())
                end
            end
        elseif prizeInfo.dwGoodsCount > 1 then
             local path = "client/res/shop/Goods/10000.png"
             if fileUtil:isFileExist(path) then
                local sprGift = cc.Sprite:create(path)
                if sprGift then
                    local spIcon = prize:getChildByName("sp")
                    spIcon:setSpriteFrame(sprGift:getSpriteFrame())
                end
            end
        end

    end

       -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onBoxClickedEvent(ref:getTag(), ref)            
        end
    end

    --累计签到礼包
    local loadBarValue = {16, 32, 49, 67, 83, 100}
    local loadBarValueNotPoint = {6, 23, 40, 57, 75, 91}  --没有到达指定点的进度
    local maxPer = 0
    local nextDays = 0  --下一个可领的天数
    local lastDays = 0  --上一个可领的天数
    for i=1, #GlobalUserItem.tabCumulativeRewards do
        local btn = self.m_tabImageGiftBox[i]
        local box =  self.m_tabSpGotFlag[i]
        local prizeInfo = GlobalUserItem.tabCumulativeRewards[i]
        btn.info = prizeInfo
        btn:setEnabled(true)
        for j=1, 16 do
            if GlobalUserItem.tabDailyInfo.nGotPackageID[1][j] == prizeInfo.nNeedDay then
                print("refreshGiftBox ------->", GlobalUserItem.tabDailyInfo.nGotPackageID[1][j])
                box:setVisible(true)
                btn.isGet = true
                self.m_tabSpCanGotFlag[i]:setVisible(false)
                break
            end
        end


        if 0 == GlobalUserItem.tabDailyInfo.nGotPackageID[1][i] and prizeInfo.nNeedDay <= GlobalUserItem.tabDailyInfo.wSeriesDate then
            if nil ~= self.m_tabSpCanGotFlag[i] then
                self.m_tabSpCanGotFlag[i]:setVisible(true)
            end
        end
        if prizeInfo.nNeedDay <= GlobalUserItem.tabDailyInfo.wSeriesDate then
            maxPer = i
            lastDays = prizeInfo.nNeedDay
        end
        if 0 == nextDays and prizeInfo.nNeedDay >= GlobalUserItem.tabDailyInfo.wSeriesDate then
            nextDays = prizeInfo.nNeedDay
        end

        --天数
        local txt = self.m_tabTxtDay[i]
        txt:setVisible(true)
        txt:setString(prizeInfo.nNeedDay .. "")
        print("refreshGiftBox ------->", prizeInfo.nNeedDay, lastDays, nextDays)
    end

    --签到天数
    if GlobalUserItem.tabDailyInfo.wSeriesDate > 0 then
        self.m_layTips:setVisible(true)
        local txtDays = self.m_layTips:getChildByName("txtDays")
        txtDays:setString(GlobalUserItem.tabDailyInfo.wSeriesDate .. "")
    else
        self.m_layTips:setVisible(false)
    end

    --设置进度
    local loadingBar = self.m_spPrizeDayBg:getChildByName("loadingBar")
    loadingBar:setPercent(0)
    if GlobalUserItem.tabDailyInfo.wSeriesDate > 0 then
        if nextDays == currentDays then
            loadingBar:setPercent(loadBarValue[maxPer])
        else --说明 先确认已经达到领取的进度，当前达到和下一个达到之前的进度按两者之间（当前天数 - 上一个达到的天数）/两者之间间隔的总天数 算出比率
            if maxPer > 0 then
                loadingBar:setPercent(loadBarValue[maxPer] + (GlobalUserItem.tabDailyInfo.wSeriesDate - lastDays)/(nextDays -1 - lastDays) * (loadBarValueNotPoint[maxPer +1] - loadBarValue[maxPer]))
            else
                loadingBar:setPercent((GlobalUserItem.tabDailyInfo.wSeriesDate - lastDays)/(nextDays -1 - lastDays) * loadBarValueNotPoint[1])
            end
        end
        
        for i=1, 6 do
            local txt = self.m_spPrizeDayBg:getChildByName("txt" .. i)
            txt:setVisible(true)
            if i <= maxPer then
                
                txt:setColor(cc.c3b(0, 96, 255))
            else
                txt:setColor(cc.c3b(148, 209, 253))
            end
        end
    else
        for i=1, 6 do
            local txt = self.m_spPrizeDayBg:getChildByName("txt" .. i)
            txt:setVisible(true)
            txt:setColor(cc.c3b(148, 209, 253))
        end
    end

    --判断是够可以免费签到
    if GlobalUserItem.tabDailyInfo.bTodayChecked then
        self.m_btnFree:setEnabled(false)
    end
end

function DailyBonusLayer:onDailyCallBack(result, tips, param)
    print("onDailyCallBack", result, tips)
    self._scene:dismissPopWait()
    if 1 == result then
        --获取信息成功设置礼包
        self:refreshGiftBox()
    elseif 2 == result then
        dump(param, "param", 7)
        if param ~= nil and param.nPackageID ~= nil then
            print("onDailyCallBack", param.nPackageID)
             for i=1, #GlobalUserItem.tabDailyRewards do
                print("onDailyCallBack", GlobalUserItem.tabDailyRewards[i].nPackageID)
                if GlobalUserItem.tabDailyRewards[i].nPackageID == param.nPackageID then
                    self:showGiftAction(1, i, tips)
                    break
                end
             end
        end
        self:refreshGiftBox()
    elseif 3 == result then
        showToast(self, "恭喜获得" .. tips, 2)
        self:refreshGiftBox()
    elseif type(tips) == "string" and "" ~= tips then
        showToast(self, tips, 2)
    end
end

return DailyBonusLayer