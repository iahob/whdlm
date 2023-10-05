--
-- Author: zhong
-- Date: 2017-05-24 16:26:58
--
-- 设置界面(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local SettingLayer = class("SettingLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

-- ui
local ServiceLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.logon.ServiceLayer")
local HelpLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "HelpLayer")
-- ui

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "CBT_SLIENCE",          -- 一键静音
    "SLIDER_BG",            -- 背景滑动
    "SLIDER_EFFECT",        -- 音效滑动
    "BTN_LOCK",             -- 锁定
    "BTN_UNLOCK",           -- 解锁
    "BTN_SWITCH",           -- 切换账号
    "BTN_HELP",             -- 帮助界面
    "BTN_SERVICE",          -- 客服界面
    "BTN_SLIENCE_OPEN",     -- 开启静音
    "BTN_SLIENCE_CLOSE",    -- 关闭静音
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
function SettingLayer:ctor( scene, param, level )
    SettingLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("option/SettingLayer.csb", self)
    self.m_rootLayer = rootLayer

    --大厅界面
    self.PlazaLayer = param

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 滑动条事件
    local sliderFunC = function( sender, eventType )
        self:onSliderEvent( sender,eventType )
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    -- 头像
    self.m_spHeadBg = spbg:getChildByName("optin_sp_headbg")
    self.m_spHeadBg:setVisible(false)

    -- 昵称
    -- local tmptxt = spbg:getChildByName("txt_nick")
    -- local cliptxt = ClipText:createClipText(tmptxt:getContentSize(), GlobalUserItem.tabAccountInfo.szNickName, nil, 26)
    -- --cliptxt:setTextColor(cc.c4b(140, 87, 30))
    -- cliptxt:setAnchorPoint(tmptxt:getAnchorPoint())
    -- cliptxt:setPosition(tmptxt:getPosition())
    -- spbg:addChild(cliptxt)
    -- tmptxt:removeFromParent()

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 音乐滑动
    local slider_bg = spbg:getChildByName("slider_bg")
    slider_bg:setTag(TAG_ENUM.SLIDER_BG)
    slider_bg:addEventListener(sliderFunC)

    self.m_sliderBg = slider_bg
    if GlobalUserItem.nMusic < 5 then
        self.m_sliderBg:setPercent(5)
    elseif GlobalUserItem.nMusic > 95 then
        self.m_sliderBg:setPercent(95)
    else
         self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
    end
    local bMusic = slider_bg:getPercent() == 5



    -- 音效滑动
    local slider_effect = spbg:getChildByName("slider_effect")
    slider_effect:setTag(TAG_ENUM.SLIDER_EFFECT)
    slider_effect:addEventListener(sliderFunC)

    self.m_sliderEffect = slider_effect 
    if GlobalUserItem.nSound < 5 then
        self.m_sliderEffect:setPercent(5)
    elseif GlobalUserItem.nSound > 95 then
        self.m_sliderEffect:setPercent(95)
    else
         self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
    end
    local bEffect = slider_effect:getPercent() == 5

    -- 静音
    btn = spbg:getChildByName("btn_slience")
    btn:addTouchEventListener( touchFunC )
    self.m_btnSlience = btn
    self:updateSlience(bMusic and bEffect)

    -- 切换账号
    btn = spbg:getChildByName("btn_switch")
    btn:setTag(TAG_ENUM.BTN_SWITCH)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 帮助
    -- btn = spbg:getChildByName("btn_help")
    -- btn:setTag(TAG_ENUM.BTN_HELP)
    -- btn:addTouchEventListener( touchFunC )
    -- btn:setPressedActionEnabled(true)

    -- -- 联系客服
    -- btn = spbg:getChildByName("btn_service")
    -- btn:setTag(TAG_ENUM.BTN_SERVICE)
    -- btn:addTouchEventListener( touchFunC )
    -- btn:setPressedActionEnabled(true)
    self.BgList = 
    {
        "bg1.png",
        "bg2.png",
        "bg3.png",
        "bg4.png",
    }

    self.TitleStr =
    {
        "夏日倾情",
        "浪漫之夜",
        "海滨都市",
        "风光之旅",
    }


    self.SelectBg = cc.UserDefault:getInstance():getIntegerForKey("option_imgBG_Index", 1)

    self.SelectBgImg = spbg:getChildByName("img_bg_big")
    --显示背景替换
    self.SelectBgImg:loadTexture("public/" .. self.BgList[self.SelectBg])
    self.SelectBgImg:setCapInsets(cc.rect(1, 1, 1, 1))


    local tmp = spbg:getChildByName("content")
    self.m_fCount = tmp:getContentSize().height / 3
    self._listView = cc.TableView:create(tmp:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_HORIZONTAL)
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(spbg)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    tmp:removeFromParent()


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

    self._listView:reloadData()

    -- 版本号
    local txtver = spbg:getChildByName("txt_version")
    local verstr = self._scene:getApp():getVersionMgr():getResVersion() or "0"
    txtver:setString("v" .. appdf.BASE_C_VERSION .. "." .. verstr)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function SettingLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_UNLOCK == tag 
        or TAG_ENUM.BTN_LOCK == tag  then

    elseif TAG_ENUM.BTN_SWITCH == tag then
        self._scene:exitPlaza()
    elseif TAG_ENUM.BTN_HELP == tag then
        local hl = HelpLayer:create(self._scene, {}, self:getLevel() + 1)
        self._scene:addPopLayer(hl)
    elseif TAG_ENUM.BTN_SERVICE == tag then
        local ser = ServiceLayer:create(self._scene, {}, self:getLevel() + 1)
        self._scene:addPopLayer(ser)
    elseif TAG_ENUM.BTN_SLIENCE_OPEN == tag then
        -- 关闭静音
        self:updateSlience(false)
        GlobalUserItem.setVoiceAble(true)
        GlobalUserItem.setSoundAble(true)

        if GlobalUserItem.nMusic < 5 then
            self.m_sliderBg:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.m_sliderBg:setPercent(95)
        else
             self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
        end

        if GlobalUserItem.nSound < 5 then
            self.m_sliderEffect:setPercent(5)
        elseif GlobalUserItem.nSound > 95 then
            self.m_sliderEffect:setPercent(95)
        else
             self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
        end
       
        
        self:updateVolum()

        GlobalUserItem.setVoiceAble(true)
        GlobalUserItem.setSoundAble(true)
    elseif TAG_ENUM.BTN_SLIENCE_CLOSE == tag then
        -- 开启静音
        self:updateSlience(true)
        GlobalUserItem.setVoiceAble(false)
        GlobalUserItem.setSoundAble(false)

        if GlobalUserItem.nMusic < 5 then
            self.m_sliderBg:setPercent(5)
        elseif GlobalUserItem.nMusic > 95 then
            self.m_sliderBg:setPercent(95)
        else
             self.m_sliderBg:setPercent(GlobalUserItem.nMusic)
        end

        if GlobalUserItem.nSound < 5 then
            self.m_sliderEffect:setPercent(5)
        elseif GlobalUserItem.nSound > 95 then
            self.m_sliderEffect:setPercent(95)
        else
             self.m_sliderEffect:setPercent(GlobalUserItem.nSound)
        end
        self:updateVolum()

        GlobalUserItem.setVoiceAble(false)
        GlobalUserItem.setSoundAble(false)
    end
end

-- 滑动条
function SettingLayer:onSliderEvent( sender, event )
    local bSlience = true
    local bChange1 = false
    local bChange2 = false
    local tag = sender:getTag()
    local changePer = sender:getPercent()
    if changePer <= 5 then
        changePer = 0
        sender:setPercent(5) 
    elseif changePer > 95 then
        changePer = 100
        sender:setPercent(95) 
    end

    if event == ccui.SliderEventType.percentChanged then
        if TAG_ENUM.SLIDER_BG == tag then
            bChange1 = changePer ~= GlobalUserItem.nMusic
            GlobalUserItem.nMusic = changePer
        elseif TAG_ENUM.SLIDER_EFFECT == tag then
            bChange2 = changePer ~= GlobalUserItem.nSound
            GlobalUserItem.nSound = changePer
        end
    end

    if 0 ~= GlobalUserItem.nMusic or 0 ~= GlobalUserItem.nSound then
        bSlience = false
    end
    if bChange1 then
        
        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false)
        else
            GlobalUserItem.bVoiceAble = true
            --背景音乐
            ExternalFun.playPlazzBackgroudAudio( )
        end
    end

    if bChange2 then
        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false)
        else
            GlobalUserItem.bSoundAble = true
        end
    end
    
    if bChange1 or bChange2 then

        self:updateSlience(bSlience)
        self:updateVolum()
    end
    if GlobalUserItem.nSound == 0 and GlobalUserItem.nMusic == 0 then
            self:updateSlience(true)
    end

end

function SettingLayer:onExit()
    cc.UserDefault:getInstance():setBoolForKey("vocieable",GlobalUserItem.bVoiceAble)
    cc.UserDefault:getInstance():setBoolForKey("soundable",GlobalUserItem.bSoundAble)
    GlobalUserItem.setMusicVolume(GlobalUserItem.nMusic)
    GlobalUserItem.setEffectsVolume(GlobalUserItem.nSound)
end

function SettingLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function SettingLayer:onTransitionInFinish()
    -- 创建头像
    -- local head = HeadSprite:createNormal(GlobalUserItem.tabAccountInfo, self.m_spHeadBg:getContentSize().width - 10)
    -- if nil ~= head then
    --     head:setPosition(self.m_spHeadBg:getPosition())
    --     self.m_spBg:addChild(head)
    -- end
end

function SettingLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function SettingLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function SettingLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

-- 更新音量
function SettingLayer:updateVolum()
    local tmp = GlobalUserItem.nMusic 
    if tmp >100 then
        tmp = 100
    elseif tmp < 0 then
        tmp = 0
    end
    AudioEngine.setMusicVolume(tmp/100.0) 

    tmp = GlobalUserItem.nSound 
    if tmp >100 then
        tmp = 100
    elseif tmp < 0 then
        tmp = 0
    end
    AudioEngine.setEffectsVolume(tmp/100.0) 
end

-- 更新开关
function SettingLayer:updateSlience( bSlience )
    local normal = "option/option_switch_close_0.png"
    local disable = "option/option_switch_close_0.png"
    local press = "option/option_switch_close_0.png"
    -- 静音
    if bSlience then
        -- 已开启静音, 关闭
        normal = "option/option_switch_open_0.png"
        disable = "option/option_switch_open_0.png"
        press = "option/option_switch_open_0.png"
        self.m_btnSlience:setTag(TAG_ENUM.BTN_SLIENCE_OPEN)
    else
        -- 已关闭静音, 开启 
        self.m_btnSlience:setTag(TAG_ENUM.BTN_SLIENCE_CLOSE)
    end
    self.m_btnSlience:loadTextureDisabled(disable)
    self.m_btnSlience:loadTextureNormal(normal)
    self.m_btnSlience:loadTexturePressed(press) 
end

function SettingLayer:cellSizeForTable(view, idx)
    return 140 , 100
end

function SettingLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end

    local imageName = self.BgList[idx + 1]
    if nil == imageName then
        return cell
    end
    print("tableCellAtIndex---------------->", imageName)

    local csb = ExternalFun.loadCSB("option/node_setBg.csb", cell)
    csb:setPosition(70, 50)
    csb:setName("csbNode")

    local btn = csb:getChildByName("btn")
    btn:setPressedActionEnabled(true)
    btn:setSwallowTouches(false)
    btn:addTouchEventListener(self.m_itemFunC)
    btn:setTag(idx + 1)
    btn.cell = cell

    --获取图片
    local bg = csb:getChildByName("imgBg")
    bg:loadTexture("public/" .. imageName)
    bg:setCapInsets(cc.rect(1, 1, 1, 1))
    -- bg:addTouchEventListener(self.m_itemFunC)
    -- bg:setTag(idx + 1)

    local box = csb:getChildByName("imgBox")
    if self.SelectBg == idx + 1 then
        box:loadTexture("option/option_sp_bgBox.png")
    else
        box:loadTexture("option/option_sp_bgBoxSelect.png")
    end
 
    --名字
    local txtName = csb:getChildByName("txt")
    if nil ~= txtName then
        txtName:setString(self.TitleStr[idx + 1])
    end
    

    return cell
end

-- 子视图数目
function SettingLayer:numberOfCellsInTableView(view)
    return #self.BgList
end

function SettingLayer:onItemButtonClickedEvent( tag, ref )
    --替换大厅背景
    self.SelectBg = tag
    for i=1,#self.BgList do
        cell = self._listView:cellAtIndex(i -1)
        if cell then
                local csb = cell:getChildByName("csbNode")
                local box = csb:getChildByName("imgBox")
                if self.SelectBg == i then
                    box:loadTexture("option/option_sp_bgBox.png")
                else
                    box:loadTexture("option/option_sp_bgBoxSelect.png")
                end   
        end
    end
    --显示背景替换
    self.SelectBgImg:loadTexture("public/" .. self.BgList[tag])
    self.SelectBgImg:setCapInsets(cc.rect(1, 1, 1, 1))

    --记录本地文件
    cc.UserDefault:getInstance():setIntegerForKey("option_imgBG_Index", self.SelectBg)

    --大厅刷新
    self.PlazaLayer:refreshPlazaBG()
end


return SettingLayer