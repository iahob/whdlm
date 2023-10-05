--[[
	询问对话框
		2016_04_27 C.P
	功能：确定/取消 对话框 与用户交互
]]
local QueryDialog = class("QueryDialog", function(msg,callback)
		local queryDialog = display.newLayer()
    return queryDialog
end)

--默认字体大小
QueryDialog.DEF_TEXT_SIZE 	= 24

--UI标识
QueryDialog.DG_QUERY_EXIT 	=  2 
QueryDialog.BT_CANCEL		=  0   
QueryDialog.BT_CONFIRM		=  1
QueryDialog.TAG_MASK 		= 3

-- 对话框类型
QueryDialog.QUERY_SURE 			= 1
QueryDialog.QUERY_SURE_CANCEL 	= 2

--窗外触碰
function QueryDialog:setCanTouchOutside(canTouchOutside)
	self._canTouchOutside = canTouchOutside
	return self
end

--msg 显示信息
--callback 交互回调
--txtsize 字体大小
function QueryDialog:ctor(msg, callback, txtsize, queryType,strC,strS)
	queryType = queryType or QueryDialog.QUERY_SURE_CANCEL
    txtsize = txtsize or 24
	self._callback = callback
	self._canTouchOutside = true

	-- 加载csb
	local csbNode = cc.CSLoader:createNode("club/QueryDialogClub.csb")
	self:addChild(csbNode)
	--按键监听
	local btcallback = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(QueryDialog.TAG_MASK)
    mask:addTouchEventListener( btcallback )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg
    --spbg:setPosition(appdf.WIDTH * 0.5, appdf.HEIGHT)
    spbg:setScale(0.05)

    -- 确定
    local btn = spbg:getChildByName("btn_sure")
    btn:setTag(QueryDialog.BT_CONFIRM)
    btn:addTouchEventListener(btcallback)
    btn:setPressedActionEnabled(true)
    self.m_btnSure = btn

    if strS then
    	self.m_btnSure:setTitleText (strS.."")
    end


    -- 取消
    btn = spbg:getChildByName("btn_cancel")
    btn:setTag(QueryDialog.BT_CANCEL)
    btn:addTouchEventListener(btcallback)
    btn:setPressedActionEnabled(true)
    self.m_btnCancel = btn

    if strC then
    	self.m_btnCancel:setTitleText(strC.."")
    end

    if QueryDialog.QUERY_SURE == queryType then
    	self.m_btnSure:setPositionX(spbg:getContentSize().width * 0.5)
    	self.m_btnCancel:setEnabled(false)
    	self.m_btnCancel:setVisible(false)
    end
 
 	-- 标题

 	-- 内容
    local txtcontent = spbg:getChildByName("txt_content")
 	txtcontent:setString(msg)
    txtcontent:setFontSize(txtsize)
    local labttf = cc.Label:createWithTTF(msg, "fonts/FZHTJW.TTF", txtsize, txtcontent:getContentSize(), cc.TEXT_ALIGNMENT_CENTER)
    labttf:setLineBreakWithoutSpace(true)
    labttf:setAnchorPoint(txtcontent:getAnchorPoint())
    labttf:setPosition(cc.p(273,150))
    spbg:addChild(labttf)
    txtcontent:removeFromParent()

	self._dismiss  = false
	spbg:stopAllActions()
	spbg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

--按键点击
function QueryDialog:onButtonClickedEvent(tag,ref)
	if tag == QueryDialog.TAG_MASK then
		if not self._canTouchOutside then
			return
		end
	end
	if self._dismiss == true then
		return
	end
	--取消显示
	self:dismiss()
	--通知回调
	if self._callback then
		self._callback(tag == QueryDialog.BT_CONFIRM)
	end
end

--取消消失
function QueryDialog:dismiss()
	self._dismiss = true
	self.m_spBg:stopAllActions()
	self.m_spBg:runAction(
		cc.Sequence:create(
			cc.ScaleTo:create(0.1, 0.5),
			cc.CallFunc:create(function()
					self:removeSelf()
				end)
			))	
end

return QueryDialog
