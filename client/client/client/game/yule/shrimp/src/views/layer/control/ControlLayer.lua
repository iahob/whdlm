--
-- Author: tom
-- Date: 2017-02-27 17:26:42
--
local ControlLayer = class("ControlLayer", function(scene)
	local ControlLayer = display.newLayer()
	return ControlLayer
end)

local cmd = appdf.req(appdf.GAME_SRC.."yule.shrimp.src.models.CMD_Game")
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local description_txt = {
	"虎","芦","鸡/狮","虾","鱼","蟹"
}

local dice_num = {4,5,3,1,0,2}


function ControlLayer:onInitData()
end

function ControlLayer:onResetData()
end

function ControlLayer.checkBoxCall( boxtab,func )
    -- body
    if type(boxtab) == "table" then
        for i=1,#boxtab do
            boxtab[i]:setSelected(false)
            boxtab[i]:onEvent(function(event)
                for k,v in pairs(boxtab) do v:setSelected(false) end
                event.target:setSelected(true)
                if func then func(i) end
            end)
        end
        boxtab[1]:setSelected(true)
        if func then func(1) end
    end
end


function ControlLayer:ctor(scene)
	self._scene = scene
	self:onInitData()

	self.m_csbnode = cc.CSLoader:createNode("control/control.csb");
	self:addChild(self.m_csbnode)

	-- self.colorLayer = cc.LayerColor:create(cc.c4b(0, 0, 0, 125))
	-- 	:setContentSize(display.width, display.height)
	-- 	:addTo(self)
	-- self.colorLayer:setTouchEnabled(true)
	-- self.colorLayer:registerScriptTouchHandler(function(eventType, x, y)
	-- 	return this:onTouch(eventType, x, y)
	-- end)

	self.dice_control = self.m_csbnode:getChildByName("dice_control")
	self.banker_control = self.m_csbnode:getChildByName("banker_control")
	self.user_query = self.m_csbnode:getChildByName("user_query")
	self.inventory_control = self.m_csbnode:getChildByName("inventory_control")
	self.close = self.m_csbnode:getChildByName("close")
	self.text_query = self.user_query:getChildByName("text_query")
	self.text_query:setString("")

	self:initDiceControl()
	self:initUserQuery()
	self:initBankerControl()
	self:initInventoryControl()


 --    local tab = {}
 --    tab.cbReqType = 1
 --    tab.lStorageStart = 1
 --    tab.lStorageDeduct = 1
 --    tab.lStorageCurrent = 1
 --    tab.lStorageMax1 = 1
 --    tab.lStorageMul1 = 1
 --    tab.lStorageMax2 = 1
 --    tab.lStorageMul2 = 1

	-- self:updataInventory(tab)
	self.close:addClickEventListener(handler(self,self.hideLayer))

end

function ControlLayer:initDiceControl()
	-- body
	local panel_1 = self.dice_control:getChildByName("Panel_1")
	local panel_2 = self.dice_control:getChildByName("Panel_2")
	local panel_3 = self.dice_control:getChildByName("Panel_3")
	local description = self.dice_control:getChildByName("description")
	-- local textField = self.dice_control:getChildByName("TextField")
	local btn_set = self.dice_control:getChildByName("btn_set")
	local btn_cancel = self.dice_control:getChildByName("btn_cancel")
	local box_index_1 = 1
	local box_index_2 = 1
	local box_index_3 = 1

    local tmp = self.dice_control:getChildByName("Sprite_5")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请搜索桌子号")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(2)
    tmp:addChild(editbox)       
	-- editbox:registerScriptEditBoxHandler(function(event, editbox)
	--     if event == "began" then

	--     elseif event == "changed" then

	--     elseif event == "return" then

	--     end
	-- end)

	local CheckBox_1 = {}
	for i=1,6 do
		CheckBox_1[#CheckBox_1+1] = panel_1:getChildByName("CheckBox_"..i)
	end

	local CheckBox_2 = {}
	for i=1,6 do
		CheckBox_2[#CheckBox_2+1] = panel_2:getChildByName("CheckBox_"..i)
	end

	local CheckBox_3 = {}
	for i=1,6 do
		CheckBox_3[#CheckBox_3+1] = panel_3:getChildByName("CheckBox_"..i)
	end

	local updata_description = function ( index_1,index_2,index_3 )
		description:setString(description_txt[index_1]..","..description_txt[index_2]..","..description_txt[index_3])
	end

	ControlLayer.checkBoxCall(CheckBox_1,function(index)
		box_index_1 = index 
		updata_description(box_index_1,box_index_2,box_index_3)
	end)
	ControlLayer.checkBoxCall(CheckBox_2,function(index)
		box_index_2 = index 
		updata_description(box_index_1,box_index_2,box_index_3)
	end)
	ControlLayer.checkBoxCall(CheckBox_3,function(index)
		box_index_3 = index 
		updata_description(box_index_1,box_index_2,box_index_3)
	end)

	btn_set:addClickEventListener(function()
		local table_id = editbox:getText()
		if table_id == "" then 
			showToast(self._scene,"请输入正确的桌子号",1)
			return
		end
		print("editbox:getText() = ",editbox:getText())
		local sice = {[1] = dice_num[box_index_1],[2] = dice_num[box_index_2],[3] = dice_num[box_index_3]}
		self._scene:sendContro(tonumber(table_id),1,2,0,sice,1)
	end)

	btn_cancel:addClickEventListener(function()
		-- body
		local table_id = editbox:getText()
		if table_id == "" then 
			showToast(self._scene,"请输入正确的桌子号",1)
			return
		end

		local sice = {1,1,1}

		self._scene:sendContro(tonumber(table_id),0,2,0,sice,0,1)
	end)

end

function ControlLayer:initUserQuery()
	-- body
    local tmp = self.user_query:getChildByName("sp_2")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入用户ID")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    
   	local query = self.user_query:getChildByName("btn_set")
	query:addClickEventListener(function ()
		-- body
		if editbox:getText() == "" then 
			showToast(self._scene,"请输入用户ID",1)  
			return 
		end

		self._scene:sendUserQuery(editbox:getText())
	end)
end

function ControlLayer:initInventoryControl()


   	local tmp = self.inventory_control:getChildByName("sp_1")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageStart = editbox

   	local tmp = self.inventory_control:getChildByName("sp_5")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageDeduct = editbox

   	local tmp = self.inventory_control:getChildByName("sp_4")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageCurrent = editbox

    tmp = self.inventory_control:getChildByName("sp_2")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageMax1 = editbox

    tmp = self.inventory_control:getChildByName("sp_6")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageMul1 = editbox

   	tmp = self.inventory_control:getChildByName("sp_3")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageMax2 = editbox

    tmp = self.inventory_control:getChildByName("sp_7")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请输入...")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(10)
    tmp:addChild(editbox)
    self.lStorageMul2 = editbox


    local btn_updata = self.inventory_control:getChildByName("btn_set")
    local btn_save = self.inventory_control:getChildByName("btn_set_0")

    btn_updata:addClickEventListener(function ()
	    local tab = {}
	    tab.cbReqType = 1
	    tab.lStorageStart = 1
	    tab.lStorageDeduct = 1
	    tab.lStorageCurrent = 1
	    tab.lStorageMax1 = 1
	    tab.lStorageMul1 = 1
	    tab.lStorageMax2 = 1
	    tab.lStorageMul2 = 1

		self._scene:sendUpDataInventory(tab)
    end)

    btn_save:addClickEventListener(function ()
    	-- body
    	local tab = {}
    	tab.cbReqType = 2
    	tab.lStorageStart = self.lStorageStart:getText()
		tab.lStorageDeduct = self.lStorageDeduct:getText()
		tab.lStorageCurrent = self.lStorageCurrent:getText()
		tab.lStorageMax1 = self.lStorageMax1:getText()
		tab.lStorageMul1 = self.lStorageMul1:getText()
		tab.lStorageMax2 = self.lStorageMax2:getText()
		tab.lStorageMul2 = self.lStorageMul2:getText()

		if tab.lStorageStart == "" then 
			showToast(self._scene,"请输入库存起始值",1)
			return 
		end
		
		if tab.lStorageDeduct == "" then 
			showToast(self._scene,"请输入库存衰减值",1)
			return 
		end
		
		if tab.lStorageCurrent == "" then 
			showToast(self._scene,"请输入当前库存值",1)
			return 
		end
		
		if tab.lStorageMax1 == "" then 
			showToast(self._scene,"请输入库存上限值1",1)
			return 
		end
		
		if tab.lStorageMul1 == "" then 
			showToast(self._scene,"请输入玩家赢分概率",1)
			return 
		end
		
		if tab.lStorageMax2 == "" then 
			showToast(self._scene,"请输入库存上限值2",1)
			return 
		end
		
		if tab.lStorageMul2 == "" then 
			showToast(self._scene,"请输入玩家赢分概率",1)
			return 
		end

		self._scene:sendUpDataInventory(tab)
    end)

end

function ControlLayer:updataInventory( data )
	-- body
	self.lStorageStart:setText(data.lStorageStart)
	self.lStorageDeduct:setText(data.lStorageDeduct)
	self.lStorageCurrent:setText(data.lStorageCurrent)
	self.lStorageMax1:setText(data.lStorageMax1)
	self.lStorageMul1:setText(data.lStorageMul1)
	self.lStorageMax2:setText(data.lStorageMax2)
	self.lStorageMul2:setText(data.lStorageMul2)
end

function ControlLayer:initBankerControl()
	-- body

	local checkBox = {}
	local box_index = 1
	checkBox[#checkBox+1] = self.banker_control:getChildByName("win"):getChildByName("CheckBox")
	checkBox[#checkBox+1] = self.banker_control:getChildByName("lose"):getChildByName("CheckBox")
	ControlLayer.checkBoxCall(checkBox,function(index)
		box_index = index == 1 and 1 or 0 
	end)

    tmp = self.banker_control:getChildByName("Sprite_5_0")
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(2)
    tmp:addChild(editbox)  
    editbox:setText(1)


    tmp = self.banker_control:getChildByName("Sprite_5")
    table_editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width, tmp:getContentSize().height),"public/public_blank.png")
		    :setAnchorPoint(cc.p(0, 0.5))
            :setPosition(0,tmp:getContentSize().height/2)
            :setPlaceHolder("请搜索桌子号")
            :setPlaceholderFont("fonts/round_body.ttf", 20)
            :setFont("fonts/round_body.ttf", 30)
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(2)
    tmp:addChild(table_editbox) 

	local btn_set = self.banker_control:getChildByName("btn_set")
	local btn_cancel = self.banker_control:getChildByName("btn_cancel")

	btn_set:addClickEventListener(function()
		local table_id = table_editbox:getText()
		if table_id == "" then 
			showToast(self._scene,"请输入正确的桌子号",1)
			return
		end

		local count_id = editbox:getText()
		if count_id == "" then 
			showToast(self._scene,"请输入正确的局数",1)
			return
		end


		local sice = {1,1,1}
		self._scene:sendContro(tonumber(table_id),1,1,box_index,sice,count_id)
	end)

	btn_cancel:addClickEventListener(function()
		-- body
		local table_id = table_editbox:getText()
		if table_id == "" then 
			showToast(self._scene,"请输入正确的桌子号",1)
			return
		end
		local count_id = editbox:getText()
		if count_id == "" then 
			showToast(self._scene,"请输入正确的局数",1)
			return
		end


		local sice = {1,1,1}
		self._scene:sendContro(tonumber(table_id),0,1,box_index,sice,count_id)
	end)
end

function ControlLayer:showTextQuery( data )
	-- body
	if data.bFind then 
		local str = "昵  称 : "..data.userinfo.szNickName .. "\n用户id : "..data.userinfo.dwGameID..
					"\n座位号 : "..data.userinfo.wChairID.."\n桌子号 : "..data.userinfo.wTableID

		self.text_query:setString(str)
	else
		self.text_query:setString("未查找到用户")
	end
end

function ControlLayer:onTouch(eventType, x, y)
	-- if eventType == "began" then
	-- 	return true
	-- end

	-- local pos = cc.p(x, y)
 --    local rectLayerBg = self.sp_layerBg:getBoundingBox()
 --    if not cc.rectContainsPoint(rectLayerBg, pos) then
 --    	self:hideLayer()
 --    end

 --    return true
end

function ControlLayer:showLayer()
	-- self.colorLayer:setTouchEnabled(true)
end

function ControlLayer:hideLayer()
	-- self.colorLayer:setTouchEnabled(false)
	-- self:onResetData()
	self:removeFromParent()
end


return ControlLayer