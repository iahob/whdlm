--
-- Author: David
-- Date: 2017-11-07 10:33:48
--
-- 大厅消息(二级弹窗)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PlazaIntroductionLayer = class("PlazaIntroductionLayer", TransitionLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")


local BTN_CLOSE = 101   -- 关闭按钮
local TAG_MASK = 102    -- 遮罩

function PlazaIntroductionLayer:ctor( scene, param, level )
    PlazaIntroductionLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("introduction/IntroductionLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("Image_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    self.curGame = param.kindId or 0

    self.m_tabList = {} --param
    self.m_tabBox = {}
    self.m_nSelectIdx = 1
    self.downLoadCount = 0


    --请求玩法列表
    local action = "action=GetGameIntroList&".. GlobalUserItem:urlUserIdSignParam()
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "desciption", 7)
        local msg = ""
        if type(jstable) == "table" and self.m_tabList then
            msg = jstable["msg"]
            local data = jstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    local ruleList = data["ruleList"]
                    if type(ruleList) == "table" then
                        for k,v in pairs(ruleList) do
                            local gameinfo = {}
                            gameinfo.KindID = v["KindID"]
                            gameinfo.KindName = v["KindName"]
                            gameinfo.LogID = v["LogID"]
                            gameinfo.RuleImg = v["RuleImg"]
                            gameinfo.Type = v["Type"]
                            if gameinfo.Type == 1  then
                                if self.curGame == v.KindID then
                                    table.insert(self.m_tabList, 1,gameinfo)
                                else
                                    for m,n in pairs(scene:getApp()._gameList) do
                                        if n and tonumber(n._KindID) == v.KindID  then
                                            table.insert(self.m_tabList, gameinfo)
                                            break
                                        end
                                    end
                                    
                                end
                            end
                        end
                    end
                end
            end

            
            self.downLoadCount = 0
            for k,v in pairs(self.m_tabList) do
                if v then
                    local imageName ="intro_game_"..v.KindID.."_"..v.LogID..".png"
                    local path = string.format("%sdownload/introduction/%s", device.writablePath, imageName)
                    if not cc.FileUtils:getInstance():isFileExist(path) then
                        self.downLoadCount = self.downLoadCount +1
                        downFileAsync(v.RuleImg, imageName, string.format("%sdownload/introduction", device.writablePath), function(...)
                                    if self.downLoadCallback then
                                        self:downLoadCallback(...) 
                                    end
                            end)
                    end
                end
            end
            if self._listView then
                self._listView:reloadData()
            end

        end
        --dump(self.m_tabList, "desciption", 7)
        --this:dismissPopWait()

    end)


    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end


function PlazaIntroductionLayer:downLoadCallback(main, sub)
    if main == appdf.DOWN_PRO_INFO then
    elseif main == appdf.DOWN_COMPELETED then
        self.downLoadCount = self.downLoadCount - 1

        print("下载完成",main, sub, self.downLoadCount)
        --下载完毕,刷新
        if self.downLoadCount <= 0 then
            self:updateImg()
        end
    else
        
    end
end

-- 按键监听
function PlazaIntroductionLayer:onButtonClickedEvent(tag,sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_MASK or tag == BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    end
end

function PlazaIntroductionLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end


function PlazaIntroductionLayer:updateImg()
    if not self._scrollView or #self.m_tabList == 0 then
        return 
    end
    self._scrollView:removeAllChildren()
    local viewSize = self._scrollView:getContentSize()

    
    local imageName = "intro_game_"..self.m_tabList[self.m_nSelectIdx].KindID.."_"..self.m_tabList[self.m_nSelectIdx].LogID..".png"
    local path = string.format("%sdownload/introduction/%s", device.writablePath, imageName)
    if cc.FileUtils:getInstance():isFileExist(path) then
        local img = cc.Sprite:create(path)
        if img then
            self._scrollView:addChild(img)
            img:setPosition(viewSize.width/2, img:getContentSize().height >= viewSize.height and img:getContentSize().height or viewSize.height)
            img:setAnchorPoint(cc.p(0.5,1))
            self._scrollView:setInnerContainerSize(cc.size(viewSize.width, img:getContentSize().height))
        end
    end

    for i=1,#self.m_tabList do
        local cell = self._listView:cellAtIndex(i-1)
        if cell then
            if self.m_nSelectIdx == i then
                cell:getChildByTag(1):setTexture("introduction/gameBtn2.png")
            else
                cell:getChildByTag(1):setTexture("introduction/gameBtn1.png")
            end
        end
    end

end


function PlazaIntroductionLayer:onTransitionInFinish()
   

    -- 规则详细
    local tmp = self.m_spBg:getChildByName("content2")
    -- 读取文本
    self._scrollView = ccui.ScrollView:create()
                          :setContentSize(tmp:getContentSize())
                          :setPosition(tmp:getPosition())
                          :setAnchorPoint(tmp:getAnchorPoint())
                          :setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
                          :setBounceEnabled(true)
                          :setScrollBarEnabled(false)
                          :addTo(self.m_spBg)
    tmp:removeFromParent()



    -- 规则列表
    local tmp = self.m_spBg:getChildByName("content1")
    self._listView = cc.TableView:create(tmp:getContentSize())
    self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
    self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)  
    self._listView:setPosition(tmp:getPosition())
    self._listView:setDelegate()
    self._listView:addTo(self.m_spBg)
    self._listView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    self._listView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    self._listView:registerScriptHandler(handler(self, self.scrollViewDidScroll), cc.SCROLLVIEW_SCRIPT_SCROLL)
    tmp:removeFromParent()
    self._listView:reloadData()
    self:updateImg()
end




function PlazaIntroductionLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PlazaIntroductionLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PlazaIntroductionLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end

function PlazaIntroductionLayer:tableCellTouched(view, cell)
    local index = cell:getIdx()
    local info = self.m_tabList[index +1]
    if nil == info then
        return 
    end

    self.m_nSelectIdx = index +1
    self:updateImg()
end

-- 子视图大小
function PlazaIntroductionLayer:cellSizeForTable(view, idx)
        return view:getViewSize().width, 100
end

function PlazaIntroductionLayer:tableCellAtIndex(view, idx)
    local cell = view:dequeueCell()
    if not cell then        
        cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
    end
    
    --获取游戏ID
    local info = self.m_tabList[idx +1]
    if nil == info then
        return cell
    end

    local imgBg = cc.Sprite:create("introduction/gameBtn"..(idx +1 == self.m_nSelectIdx and 2 or 1)..".png"):move(view:getViewSize().width/2, 50):addTo(cell)
    imgBg:setTag(1)
    local gameName = cc.Label:createWithTTF(info.KindName, "fonts/FZHTJW.TTF", 26)
    gameName:setColor(cc.c3b(206,171,228))
    gameName:setPosition(view:getViewSize().width/2, 50)
    gameName:addTo(cell)

    return cell
end

-- 子视图数目
function PlazaIntroductionLayer:numberOfCellsInTableView(view)
    return self.m_tabList and #self.m_tabList or 0
end

function PlazaIntroductionLayer:scrollViewDidScroll(view)

end

function PlazaIntroductionLayer:getEmptySpace( sz )

end

return PlazaIntroductionLayer