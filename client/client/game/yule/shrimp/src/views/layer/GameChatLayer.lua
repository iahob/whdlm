--
-- Author: zhong
-- Date: 2016-08-12 14:35:14
--
--通用聊天层

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local targetPlatform = cc.Application:getInstance():getTargetPlatform()

--聊天展现层
local GameChatLayer = class("GameChatLayer", cc.Layer)
--聊天内容
local ChatItem = class("ChatItem", cc.Node)
--表情面板
local BrowPanel = class("BrowPanel", cc.Node)

--关闭
local BT_CLOSE = 101
--发送聊天
local BT_SEND = 102
--文本聊天
local CBT_TEXT = 201
--表情聊天
local CBT_BROW = 202
--聊天记录
local CBT_RECORD = 203

--背景遮罩
local TAG_MASK = 204


local NUM_EMOJ = 18   --表情数目

local CHAT_ITEM_NAME = "game_chat_item"
local CHAT_BROW_PANEL = "game_brow_panel"

local GAMECHAT_LISTENER = "gamechat_listener"

--表情tableview宽度
local LEN_BROWVIEW_WIDTH = 480
--tableview宽度
local LEN_TABLEVIEW_WIDTH = 400
--聊天文本大小
local SIZE_CHAT_LABEL = 25
--聊天内容宽度
local LEN_CHAT_WIDTH = 580

--聊天记录
local chatRecord = {}
--文本聊天内容
local textChatConfig = {}

function GameChatLayer.getRecordConfig()
    return 
    {
        bBrow = false,
        strChat = "",
        nIdx = 0,
        szNick = ""
    }
end

function GameChatLayer.loadTextChat()
    local str = cc.FileUtils:getInstance():getStringFromFile("public/chat_text.json")
    local ok, datatable = pcall(function()
                   return cjson.decode(str)
                end)
    if true == ok and type(datatable) == "table" then
        for k,v in pairs(datatable) do
            local record = {}
            record.bBrow = false
            record.strChat = v
            textChatConfig[k] = record
        end
    else
        print("load text chat error!")
    end

    --[[cc.SpriteFrameCache:getInstance():addSpriteFrames("public/brow.plist")

    --缓存表情
    local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/brow.plist")
    local framesDict = dict["frames"]
    if nil ~= framesDict and type(framesDict) == "table" then
        for k,v in pairs(framesDict) do
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
            if nil ~= frame then
                frame:retain()
            end
        end
    end]]
end
GameChatLayer.loadTextChat()

-- 默认短语对比
function GameChatLayer.compareWithText(str)
    for k,v in pairs(textChatConfig) do
        if v.strChat == str then
            return (k - 1)
        end
    end
    return nil
end

-- netframe 网络
-- tabconfig 配置(csb路径、特殊资源)
function GameChatLayer:ctor(netframe, tabconfig)
    if nil == netframe then
        print("net frame can't be nil")
        return
    end
    self.m_netframe = netframe
    tabconfig = tabconfig or {}
    self.m_tabConfig = tabconfig

    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)

    csbfile = tabconfig.csbfile or "ui/GameChatLayer.csb"

    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB(csbfile, self)
    local chat_bg = csbNode:getChildByName("chat_bg")
    self.m_spChatBg = chat_bg


	--常用语聊天 区域 和位置 
	self.m_chat_common_region = chat_bg:getChildByName("chat_common_region")
	self.common_chat_bg  = self.m_chat_common_region:getChildByName("common_chat_bg")
	self.common_chat_bg:setVisible(false)
    self.m_common_region_szie = cc.size(self.m_chat_common_region:getContentSize().width,self.m_chat_common_region:getContentSize().height)


	--聊天记录 区域 和位置
	-- self.m_chat_recorde_region = chat_bg:getChildByName("chat_recorde_region")
 --    self.recorde_chat_bg  = self.m_chat_recorde_region:getChildByName("recorde_chat_bg")
	-- self.recorde_chat_bg:setVisible(false)
 --    self.m_recorde_region_szie = cc.size(self.m_chat_recorde_region:getContentSize().width,self.m_chat_recorde_region:getContentSize().height)

    --表情聊天
    self.m_emoj_region = chat_bg:getChildByName("emoj_region")
    self.m_emoj_bg  = self.m_emoj_region:getChildByName("emoj_bg")
    self.m_emoj_bg:setVisible(false)
    self.m_emoj_region:setVisible(false)
    self.m_emoj_region_szie = cc.size(self.m_emoj_region:getContentSize().width,self.m_emoj_region:getContentSize().height)
	
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    --关闭按钮
    -- local btn = chat_bg:getChildByName("btn_close")
    -- btn:setTag(BT_CLOSE)
    -- btn:addTouchEventListener(btncallback)

    -- self.edit_send_layer = chat_bg:getChildByName("edit_send_layer")
    -- --发送按钮
    -- btn = self.edit_send_layer:getChildByName("btn_send")
    -- btn:setTag(BT_SEND)
    -- btn:addTouchEventListener(btncallback)

    -- 遮罩
    local mask = csbNode:getChildByName("Panel_1")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( btncallback )


    local checkEvent = function (sender,eventType)
        self:onCheckBoxClickEvent(sender,eventType)
    end

    self.m_nSelect = CBT_TEXT
    --文本聊天
    local cbt = chat_bg:getChildByName("text_check")
    cbt:setTag(CBT_TEXT)
    cbt:addTouchEventListener(btncallback)
    self.m_checkText = cbt

  
    --表情
    cbt = chat_bg:getChildByName("emoj_check")
    cbt:setTag(CBT_BROW)
    cbt:addTouchEventListener(btncallback)
    self.m_checkEmoj = cbt   

    --选中提示
    self.m_spSelect = chat_bg:getChildByName("sp_select")
    self.m_spSelect:setPositionX(self.m_checkText:getPositionX())
    self.m_spSelect:setScaleX(1)

    --内容
    self.m_textTableView = nil
    self.m_browTableView = nil
    self.m_recordTableView = nil

        --创建表情icon
    self.touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local beginPos = ref:getTouchBeganPosition()
            local endPos = ref:getTouchEndPosition()
            if math.abs(endPos.x - beginPos.x) > 30 
                or math.abs(endPos.y - beginPos.y) > 30 then
                return
            end
            print("表情发送------------>", ref.index)
            self:sendBrowChat(ref.index)


            -- widget add 到 tableview 会在非可见区域触发touch事件
            -- local toViewPoint = self._listClubView:convertToNodeSpace(endPos)
            -- local rec = cc.rect(0, 0, self._listClubView:getViewSize().width, self._listClubView:getViewSize().height)
            -- if true == cc.rectContainsPoint(rec, toViewPoint) then
            --     this:onItemButtonClickedEvent(ref:getTag(), ref)  
            -- end  
        end
    end



    self:setVisible(false)

    --ios设置父节点缩放值，会导致editbox显示异常
    if targetPlatform == cc.PLATFORM_OS_ANDROID then
        --self.m_spChatBg:setScale(0.000001)
    end
    --动作
    local call = cc.CallFunc:create(function( )
        self:setVisible(true)
        if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
            self.m_recordTableView:reloadData()
            self:moveRecordToLastRow()
        end
    end)
    
    local sca = cc.ScaleTo:create(0.2, 1.0)
    -- local call1 = cc.CallFunc:create(function()     
    --     if targetPlatform ~= cc.PLATFORM_OS_ANDROID then            
    --         self.m_editChat:setVisible(true)
    --     end  
    --     self.m_editChat:setEnabled(true)    
    -- end)
    self.m_actShow = cc.Sequence:create(call, sca)
    ExternalFun.SAFE_RETAIN(self.m_actShow)

    -- local call2 = cc.CallFunc:create(function (  )      
    --     if targetPlatform ~= cc.PLATFORM_OS_ANDROID then
    --         self.m_editChat:setVisible(false)
    --     end
    --     self.m_editChat:setEnabled(false)
    -- end)
    local sca2 = cc.ScaleTo:create(0.2, 0.000001)
    local call3 = cc.CallFunc:create(function (  )
        self:setVisible(false)
    end)
    self.m_actHide = cc.Sequence:create(sca2, call3)
    ExternalFun.SAFE_RETAIN(self.m_actHide)

    self.m_listener = cc.EventListenerCustom:create(GAMECHAT_LISTENER,handler(self, function(target,event)
            if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
                self.m_recordTableView:reloadData()
                self:moveRecordToLastRow()
            end
        end))
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
end

function GameChatLayer:showGameChat( bShow )
    --防作弊不聊天
    if GlobalUserItem.isAntiCheat() then
        local runScene = cc.Director:getInstance():getRunningScene()
        showToast(runScene, "防作弊房间禁止聊天", 3)
        return
    end

    local ani = nil
    if bShow then
        ani = self.m_actShow
    else 
        ani = self.m_actHide
    end

    if nil ~= ani then
        self.m_spChatBg:stopAllActions()
        if targetPlatform == cc.PLATFORM_OS_ANDROID then
            self.m_spChatBg:runAction(ani)
        else
            self:setVisible(bShow)
            if self.m_nSelect == CBT_RECORD and nil ~= self.m_recordTableView then
                self.m_recordTableView:reloadData()
                self:moveRecordToLastRow()
            end
        end         
    end
end

function GameChatLayer:showTextChat(bShow)
    if bShow then
        if nil == self.m_textTableView then
            self.m_textTableView = self:getDataTableView(self.m_common_region_szie)
            self.m_chat_common_region:addChild(self.m_textTableView)
        end
        self.m_textTableView:reloadData()
    end

    if nil ~= self.m_textTableView then
        --self.m_textTableView:setVisible(bShow)
		self.m_chat_common_region:setVisible(bShow)
		self.m_emoj_region:setVisible(not bShow)
		--self.edit_send_layer:setVisible(bShow)
		--self.m_editChat:setVisible(bShow)
    end
end

function GameChatLayer:showRecord(bShow)
    if bShow then
        if nil == self.m_recordTableView then
            self.m_recordTableView = self:getDataTableView(self.m_recorde_region_szie)
            self.m_chat_recorde_region:addChild(self.m_recordTableView)
        end
        self.m_recordTableView:reloadData()
        self:moveRecordToLastRow()
    end

    if nil ~= self.m_recordTableView then
        --self.m_recordTableView:setVisible(bShow)
		self.m_chat_recorde_region:setVisible(bShow)
		self.m_chat_common_region:setVisible(not bShow)
		self.edit_send_layer:setVisible(not bShow)
		--self.m_editChat:setVisible(not bShow)
    end
end

function GameChatLayer:showEmoj(bShow)
    if bShow then
        if nil == self.m_browTableView then
            self.m_browTableView = self:getDataTableView(self.m_emoj_region_szie)
            self.m_emoj_region:addChild(self.m_browTableView)
        end
        self.m_browTableView:reloadData()
    end

    if nil ~= self.m_browTableView then
        --self.m_recordTableView:setVisible(bShow)
        self.m_emoj_region:setVisible(bShow)
        self.m_chat_common_region:setVisible(not bShow)
        --self.edit_send_layer:setVisible(not bShow)
        --self.m_editChat:setVisible(not bShow)
    end
end

function GameChatLayer:moveRecordToLastRow()    
    local container = self.m_recordTableView:getContainer()
    local needToLastRow = false
    if nil ~= container then
        needToLastRow = (container:getContentSize().height >= self.m_recordTableView:getViewSize().height)
        --self.m_recordTableView:setTouchEnabled(needToLastRow)
    end
    if needToLastRow then
        self.m_recordTableView:setContentOffset(cc.p(0,0),false)
    end
end

function GameChatLayer:getDataTableView(size)
    --tableview
    --获取tableView位置，根据chat_area来计算

    local m_tableView = cc.TableView:create(size)
    m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    m_tableView:setPosition(cc.p(0,0))
	m_tableView:setAnchorPoint(cc.p(0.0, 0.0))
    m_tableView:setDelegate()
    m_tableView:registerScriptHandler(handler(self, self.cellSizeForTable), cc.TABLECELL_SIZE_FOR_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
    m_tableView:registerScriptHandler(handler(self, self.tableCellTouched), cc.TABLECELL_TOUCHED)
    m_tableView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
    return m_tableView
end

function GameChatLayer:onButtonClickedEvent( tag, sender )
    if BT_CLOSE == tag then
        self:showGameChat(false)
    elseif BT_SEND == tag then
        local chatstr = self.m_editChat:getText()
        chatstr = string.gsub(chatstr, " " , "")
        if ExternalFun.stringLen(chatstr) > 128 then
            showToast(self, "聊天内容过长", 2)
            return
        end
        --判断emoji
        if ExternalFun.isContainEmoji(chatstr) then
            showToast(self, "聊天内容包含非法字符,请重试", 2)
            return
        end

        --敏感词过滤  
        if true == ExternalFun.isContainBadWords(chatstr) then
            showToast(self, "聊天内容包含敏感词汇!", 2)
            return
        end

        if "" ~= chatstr then
            local valid, msg = self:sendTextChat(chatstr)
            if false == valid and type(msg) == "string" and "" ~= msg then
                showToast(self, msg, 2)
            else
                self.m_editChat:setText("")
            end
        end
    elseif TAG_MASK == tag then
        self:showGameChat(false)
    elseif CBT_TEXT == tag then
        self.m_nSelect = CBT_TEXT
        self.m_spSelect:setPositionX(self.m_checkText:getPositionX())
        self.m_spSelect:setScaleX(1)

        self:showTextChat(true)
        self:showEmoj(false)
    elseif CBT_BROW == tag then
        self.m_nSelect = CBT_BROW
        self.m_spSelect:setPositionX(self.m_checkEmoj:getPositionX())
        self.m_spSelect:setScaleX(-1)
        self:showTextChat(false)
        self:showEmoj(true)
    end
end

function GameChatLayer:onCheckBoxClickEvent(sender, eventType)
    if nil == sender then
        return
    end

    local tag = sender:getTag()
    if self.m_nSelect == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelect = tag
    self.m_editChat:setText("")

    local bShowText = false
    local bShowEmoj = false
    if CBT_TEXT == tag then
        bShowText = true
        bShowEmoj = false
        self.m_spSelect:setPositionX(116)
        self.m_spSelect:setScaleX(1)
    elseif CBT_BROW == tag then
        bShowText = false
        bShowEmoj = true
        self.m_spSelect:setPositionX(316)
        self.m_spSelect:setScaleX(-1)
    end
    self:showTextChat(bShowText)
    self:showEmoj(bShowEmoj)


	--self.m_chat_recorde_region:setVisible(bShow)
	--self.m_chat_common_region:setVisible(not bShow)
end

function GameChatLayer:onEnterTransitionFinish()
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end

    --编辑框tmp
    -- local tmp =  self.edit_send_layer:getChildByName("edit_frame")
    -- local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"public/public_blank.png")
    --     :setPosition(tmp:getPosition())
    --     :setFontColor(cc.c3b(0, 0, 0))
    --     :setFontName("fonts/round_body.ttf")
    --     :setPlaceHolder("请输入聊天信息")
    --     :setPlaceholderFont("fonts/round_body.ttf", 30)
    --     :setFontSize(30)
    --     :setMaxLength(32)
    --     :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)  
    -- self.edit_send_layer:addChild(editbox)
    -- self.m_editChat = editbox
    -- self.m_editChat:registerScriptEditBoxHandler(editHanlder)
    -- self.m_bEditChat = false

        --默认显示
    self:showTextChat(true)
end

function GameChatLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditChat = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditChat = string.len(src) ~= 0
    end
end

function GameChatLayer:onExit()
    chatRecord = {}

    ExternalFun.SAFE_RELEASE(self.m_actShow)
    self.m_actShow = nil
    ExternalFun.SAFE_RELEASE(self.m_actHide)
    self.m_actHide = nil

    if nil ~= self.m_listener then
        cc.Director:getInstance():getEventDispatcher():removeEventListener(self.m_listener)
    end
end

function GameChatLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function GameChatLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spChatBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) and false == self.m_bEditChat then
        self:showGameChat(false)
    end

    if true == self.m_bEditChat then
        self.m_bEditChat = false
    end
end

function GameChatLayer:isTouchBrowEnable()
    return (CBT_BROW == self.m_nSelect)
end

--发送文本聊
function GameChatLayer:sendTextChat(msg)
    if nil ~= self.m_netframe and nil ~= self.m_netframe.sendTextChat then
        self:setVisible(false)
        return self.m_netframe:sendTextChat(msg)
    end
    return false, ""
end

--发送表情聊天
function GameChatLayer:sendBrowChat(idx)
    if nil ~= self.m_netframe and nil ~= self.m_netframe.sendBrowChat then
        self:setVisible(false)
        return self.m_netframe:sendBrowChat(idx)
    end
    return false, ""
end

--添加聊天记录
function GameChatLayer.addChatRecord(rec)
    rec = rec or GameChatLayer.getRecordConfig()
    table.insert(chatRecord, 1, rec)

    --通知
    local eventListener = cc.EventCustom:new(GAMECHAT_LISTENER)
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function GameChatLayer.addChatRecordWith(cmdtable, bBrow)
    local rec = GameChatLayer.getRecordConfig()
    rec.bBrow = bBrow
    rec.szNick = cmdtable.szNick
    if bBrow then
        rec.nIdx = cmdtable.wItemIndex
    else
        rec.strChat = cmdtable.szChatString
    end

    GameChatLayer.addChatRecord(rec)
end

--tableview
function GameChatLayer:cellSizeForTable( view, idx )
    if CBT_TEXT == self.m_nSelect then
        local record = textChatConfig[idx + 1]
        if nil ~= record then
            return self.m_common_region_szie.width, 64--self:getTextChatSize(record.strChat)
        end
        return 0,0
    elseif CBT_BROW == self.m_nSelect then
        return self.m_emoj_region_szie.width, 130
    end
    return 0,0
end

function GameChatLayer:getTextChatSize(str)
    local tmp = cc.LabelTTF:create(str, "fonts/round_body.ttf", SIZE_CHAT_LABEL, cc.size(LEN_CHAT_WIDTH,0))
    local tmpsize = tmp:getContentSize()
    tmp:setString("网")
    return LEN_TABLEVIEW_WIDTH, tmpsize.height + 15, tmp:getContentSize().height
end

function GameChatLayer:numberOfCellsInTableView( view )
    if CBT_TEXT == self.m_nSelect then
        return #textChatConfig
    elseif CBT_BROW == self.m_nSelect then  
        return math.ceil(NUM_EMOJ /3)
    end
    return 0
end

function GameChatLayer:tableCellAtIndex(view, idx)  
    local cell = view:dequeueCell()
    local dataitem = nil
    if CBT_TEXT == self.m_nSelect or CBT_BROW == self.m_nSelect then
        if nil == cell then
            cell = cc.TableViewCell:new()
            dataitem = self:getDataItemAt(view,idx)
            if nil ~= dataitem then
                dataitem:setName(CHAT_ITEM_NAME)
                cell:addChild(dataitem)
            end
        else
            dataitem = cell:getChildByName(CHAT_ITEM_NAME)
        end
        if nil ~= dataitem then
            self:refreshDataItemAt(view, dataitem, idx)
        end
    -- elseif CBT_BROW == self.m_nSelect then  --表情依次绘制
    --     if nil == cell then
    --         cell = cc.TableViewCell:new()
    --         dataitem = self:getDataItemAt(view,idx)
    --         if nil ~= dataitem then
    --             dataitem:setName(CHAT_ITEM_NAME)
    --             cell:addChild(dataitem)
    --         end
    --     else
    --         dataitem = cell:getChildByName(CHAT_ITEM_NAME)
    --     end
    --     if nil ~= dataitem then
    --         self:refreshDataItemAt(view, dataitem, idx)
    --     end
    end

    cell:setTag(idx)
    return cell
end

function GameChatLayer:tableCellTouched(view, cell)
    if CBT_TEXT == self.m_nSelect then
        local idx = cell:getIdx() + 1
        if nil ~= textChatConfig[idx] then
            self:sendTextChat(textChatConfig[idx].strChat)
        end  
    elseif CBT_BROW == self.m_nSelect then 
    end
end

local idxTable = {}
function GameChatLayer:getDataItemAt(view, idx)
    if CBT_TEXT == self.m_nSelect  then
        local chatitem = ChatItem:create(1, self.m_tabConfig)
        chatitem:setName(CHAT_ITEM_NAME)
        return chatitem
    elseif CBT_BROW == self.m_nSelect then
        local chatitem = ChatItem:create(2, self.m_tabConfig, self.touchFunC)
        chatitem:setName(CHAT_ITEM_NAME)
        return chatitem
    end
    return nil
end

function GameChatLayer:refreshDataItemAt(view, item, idx)   
    if nil == item then
        return 
    end

    if CBT_TEXT == self.m_nSelect then
        local record = textChatConfig[idx + 1]
        if nil ~= record then
            item:refreshTextItem(record.strChat)
        end         
    elseif CBT_BROW == self.m_nSelect then
        item:refreshEmojItem(idx)
    end

    local size = item:getContentSize()
    item:setPosition(size.width * 0.5, 0)
end

function GameChatLayer:loadEmojChat(idx)  
    cc.SpriteFrameCache:getInstance():addSpriteFrames(string.format("chat/emoj/%d.plist", idx))

    --缓存表情
    local dict = cc.FileUtils:getInstance():getValueMapFromFile(string.format("chat/emoj/%d.plist", idx))
    local framesDict = dict["frames"]
    if nil ~= framesDict and type(framesDict) == "table" then
        local framesNum = #framesDict
                --动画
        local animation = cc.Animation:create()
        local times = 0.15
        for i=1,framesNum do
            local strName = string.format("%d.png", i)
            local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strName)
            if spriteFrame then
                animation:addSpriteFrame(spriteFrame)
            else
                break
            end
            animation:setLoops(-1)
            animation:setDelayPerUnit(times)
        end

        local animate = cc.Animate:create(animation)
        local spriteFrame = cc.SpriteFrameCache:getInstance():getSpriteFrame("1.png")
        local spr = cc.Sprite:createWithSpriteFrame(spriteFrame)
        return spr, animate
    end
    return nil
end

function ChatItem:ctor( nType, tabconfig, callFun )
    if 1 == nType then --聊天
        local framename = tabconfig.szItemFrameName or "chat/public_chatLine.png"
        if cc.FileUtils:getInstance():isFileExist(framename) then
            local sp_line = cc.Sprite:create(framename)
            local frame = sp_line:getSpriteFrame()
            if nil ~= frame then
                self.m_spLine = sp_line --cc.Sprite:createWithSpriteFrame(frame)
                --self.m_spLine:setAnchorPoint(cc.p(0.5, 0.5))
                print("位置----------》", LEN_TABLEVIEW_WIDTH *0.5 - self.m_spLine:getContentSize().width * 0.5)
                --self.m_spLine:setPosition(LEN_TABLEVIEW_WIDTH *0.5 - self.m_spLine:getContentSize().width * 0.5, 2)
                self:addChild(self.m_spLine)
            end
        end

        --选中效果 
        local framename = tabconfig.szItemSelectFrameName or "caht/public_chat_txtSelect.png"
        if cc.FileUtils:getInstance():isFileExist(framename) then
            local sp_select = cc.Sprite:create(framename)
            local frame = sp_select:getSpriteFrame()
            if nil ~= frame then
                self.m_spSelect = cc.Sprite:createWithSpriteFrame(frame)
               -- self.m_spSelect:setAnchorPoint(cc.p(0.5, 0.5))
                print("位置----------》", LEN_TABLEVIEW_WIDTH *0.5 - self.m_spSelect:getContentSize().width * 0.5)
                self.m_spSelect:setPosition(0, self.m_spSelect:getContentSize().height * 0.5)
                self:addChild(self.m_spSelect)
                self.m_spSelect:setVisible(false)
            end
        end
    elseif 2 == nType then --表情
        self._callFun = callFun
    end

    self.m_labelChat = nil
    self.m_clipUserName = nil
end

function ChatItem:refreshTextItem(str,sendusernick)
    if nil == self.m_labelChat then
        self.m_labelChat = cc.LabelTTF:create(str, "fonts/FZHTJW.TTF", SIZE_CHAT_LABEL, cc.size(LEN_TABLEVIEW_WIDTH -40,0), cc.TEXT_ALIGNMENT_LEFT)
        --self.m_labelChat:setPosition(2, self.m_spSelect:getContentSize().height * 0.5)

        self:addChild(self.m_labelChat)
    else
        self.m_labelChat:setString(str)
    end

    self.m_labelChat:setColor(cc.c3b(78,50,50))
    local height = 64
    local labSize = self.m_labelChat:getContentSize()
    if labSize.height + 15 > height then
        height = labSize.height + 15
    end
    self:setContentSize(cc.size(LEN_TABLEVIEW_WIDTH, height))
    self.m_labelChat:setPosition(10, self:getContentSize().height * 0.5)
    if nil ~= self.m_spSelect then
        --self.m_spSelect:setPosition(LEN_TABLEVIEW_WIDTH *0.5 - self.m_spSelect:getContentSize().width * 0.5, self:getContentSize().height * 0.5)
    end
end

function ChatItem:refreshRecordItem(str,sendusernick)
    if nil == self.m_labelChat then
        self.m_labelChat = cc.LabelTTF:create(str, "fonts/round_body.ttf", 20, cc.size(LEN_CHAT_WIDTH,0), cc.TEXT_ALIGNMENT_LEFT)
        self.m_labelChat:setAnchorPoint(cc.p(0.5, 0))
        self.m_labelChat:setPositionY(5)

        self:addChild(self.m_labelChat)
    else
        self.m_labelChat:setString(str)
    end
    self.m_labelChat:setColor(cc.c3b(78,50,50))
    local labSize = self.m_labelChat:getContentSize()
    self:addSendUser(labSize.height + 10, sendusernick)
    if nil ~= sendusernick and type(sendusernick) == "string" then
        labSize.height = labSize.height + self.m_clipUserName:getContentSize().height + 5
    end
    self:setContentSize(cc.size(tabconfig.width, labSize.height + 15))
end

function ChatItem:addSendUser(posHeight,sendusernick)
    if nil ~= sendusernick and type(sendusernick) == "string" then
        if nil == self.m_clipUserName then
            self.m_clipUserName = ClipText:createClipText(cc.size(200,20), sendusernick .. ":", "fonts/round_body.ttf", 20)
            self.m_clipUserName:setTextColor(cc.c3b(36, 236, 255))
            self.m_clipUserName:setAnchorPoint(cc.p(0, 0.5))
            self.m_clipUserName:setPositionX(-LEN_CHAT_WIDTH * 0.5)
            self:addChild(self.m_clipUserName)
        else
            self.m_clipUserName:setString(sendusernick .. ":")
        end

        self.m_clipUserName:setPositionY(posHeight + self.m_clipUserName:getContentSize().height * 0.5)     
    else
        if nil ~= self.m_clipUserName then
            self.m_clipUserName:setVisible(false)
        end
    end
end

function ChatItem:refreshEmojItem(idx)
    self:removeAllChildren()
    for i=1,3 do
        local index = idx *3 +i
        if index > NUM_EMOJ then
            break
        end
        local filename = string.format("chat/emoj/emoj_%d.png", index)
        print("表情创建------------>", filename, idx)
        if cc.FileUtils:getInstance():isFileExist(filename) then
            local btn = ccui.Button:create(filename, filename, filename)
            btn.index = index
            btn:addTouchEventListener( self._callFun )
            btn:setPressedActionEnabled(false)
            btn:setSwallowTouches(false)
            btn:setName(index .. "_icon")
            self:addChild(btn)
            btn:setPosition(i * 130 -60 , 65)
        end
    end
end

return GameChatLayer