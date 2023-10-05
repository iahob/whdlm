function showToast(context,message,delaytime,color)
	if (context == nil) or  (message == nil) or (delaytime<1) then
		return
	end

	local msgtype = type(message)
	if msgtype == "userdata" or msgtype == "table" then
		return
	end

	if message == "" then
		return
	end
	local showMessage = message	
	
	local bg = context:getChildByName("toast_bg")
	local lab = nil
	if bg then
		bg:stopAllActions()
		lab = bg:getChildByName("toast_lab")
		if nil == lab then
			lab = cc.Label:createWithSystemFont(showMessage, "Arial", 24, cc.size(900,0))
			lab:setName("toast_lab")
			
			lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
			lab:addTo(bg)
		end
		bg:move(appdf.WIDTH * 0.5, appdf.HEIGHT *0.5)
		bg:runAction(cc.Sequence:create(
			cc.DelayTime:create(delaytime), 
			--cc.MoveTo:create(0.5, cc.p(appdf.WIDTH * 0.5, appdf.HEIGHT + 50)),
			cc.FadeOut:create(0.5),
			cc.RemoveSelf:create(true))
		)		
	else
		cc.Director:getInstance():getTextureCache():removeTextureForKey("public/public_pop_frame.png")
		bg = ccui.ImageView:create("public/public_pop_frame.png")
		bg:setAnchorPoint(cc.p(0.5, 0.5))
		bg:move(appdf.WIDTH * 0.5, appdf.HEIGHT *0.5)
		bg:addTo(context)
		bg:setName("toast_bg")
		bg:setScale9Enabled(true)
		bg:runAction(cc.Sequence:create( 
			cc.DelayTime:create(delaytime),
			--cc.MoveTo:create(0.5, cc.p(appdf.WIDTH * 0.5, appdf.HEIGHT + 50)), 
			cc.FadeOut:create(0.5),
			cc.RemoveSelf:create(true))
		)

		lab = cc.Label:createWithSystemFont(showMessage, "Arial", 24, cc.size(900,0))
		lab:setLineBreakWithoutSpace(true)
		lab:setName("toast_lab")
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		lab:addTo(bg)
	end

	if nil ~= lab and nil ~= bg then
		lab:setString(showMessage)
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		local labSize = lab:getContentSize()

		if labSize.height < 30 then
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
			bg:setContentSize(cc.size(appdf.WIDTH, 64))
		else
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
			bg:setContentSize(cc.size(appdf.WIDTH, 64 + labSize.height))		
		end
		lab:move(appdf.WIDTH * 0.5, bg:getContentSize().height * 0.5)
	end
end