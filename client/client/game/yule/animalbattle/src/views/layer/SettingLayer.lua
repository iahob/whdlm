local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local module_pre = "game.yule.animalbattle.src"
local cmd = appdf.req(module_pre .. ".models.CMD_Game")
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local SettingLayer = class("SettingLayer", TransitionLayer)
local CBT_MUSIC 	= 1
local CBT_SOUND   	= 2

function SettingLayer:ctor(parentNode)
	self._parentNode=parentNode
	SettingLayer.super.ctor(self)
 	self.csbNode=ExternalFun.loadCSB("SettingLayer.csb",self)
 	self.csbNode:setAnchorPoint(0.5,1)
 	self.csbNode:setPosition(yl.WIDTH/2,yl.HEIGHT+20)

    -- 滑动条事件
    local sliderFunC = function( sender, eventType )
        self:onSliderEvent( sender,eventType )
    end

 	self.effectAudio = appdf.getNodeByName(self.csbNode,"effect")
	self.bgAudio = appdf.getNodeByName(self.csbNode,"music")
	
	self.effectAudio:setTag(CBT_SOUND)
	self.bgAudio:setTag(CBT_MUSIC)
	self.effectAudio:addEventListener(sliderFunC)
	self.bgAudio:addEventListener(sliderFunC)

        print("GlobalUserItem.nMusic ",GlobalUserItem.nMusic)
        print("GlobalUserItem.nSound ",GlobalUserItem.nSound)

    if GlobalUserItem.nMusic < 5 then
        self.bgAudio:setPercent(5)
    elseif GlobalUserItem.nMusic > 95 then
        self.bgAudio:setPercent(95)
    else
         self.bgAudio:setPercent(GlobalUserItem.nMusic)
    end

    if GlobalUserItem.nSound < 5 then
        self.effectAudio:setPercent(5)
    elseif GlobalUserItem.nSound > 95 then
        self.effectAudio:setPercent(95)
    else
         self.effectAudio:setPercent(GlobalUserItem.nSound)
    end


	appdf.getNodeByName(self.csbNode,"Button_1"):addClickEventListener(function() 
        print("GlobalUserItem.nMusic ",GlobalUserItem.nMusic)
        print("GlobalUserItem.nSound ",GlobalUserItem.nSound)
        ExternalFun.playSoundEffect("guanbi.mp3")
        cc.UserDefault:getInstance():setBoolForKey("vocieable",GlobalUserItem.bVoiceAble)
        cc.UserDefault:getInstance():setBoolForKey("soundable",GlobalUserItem.bSoundAble)
        GlobalUserItem.setMusicVolume(GlobalUserItem.nMusic)
        GlobalUserItem.setEffectsVolume(GlobalUserItem.nSound)
        self:removeSelf() 
    end)

    -- self:registerEventListenr()

	-- ccui.Text:create("游戏版本"..cmd.VERSION_CLIENT,"fonts/round_body.ttf",26)
 --    					:addTo(self.csbNode)
 --    					:setPosition(cc.p(650,120))
 --                        :setTextColor(cc.c4b(140,230,100,255))
end


function SettingLayer:onSliderEvent( sender,eventType )
    local bSlience = true
    local bChange = false
    local tag = sender:getTag()
    local changePer = sender:getPercent()
    if changePer <= 5 then
        changePer = 0
        sender:setPercent(5) 
    elseif changePer > 95 then
        changePer = 100
        sender:setPercent(95) 
    end
    print("changePer" ,changePer)

    -- if event == ccui.SliderEventType.percentChanged then
        if CBT_MUSIC == tag then
            bChange = changePer ~= GlobalUserItem.nMusic
            print("changePer" ,changePer)
            GlobalUserItem.nMusic = changePer
        elseif CBT_SOUND == tag then
            print("changePer" ,changePer)
            bChange = changePer ~= GlobalUserItem.nSound
            GlobalUserItem.nSound = changePer
        end
    -- end

    if 0 ~= GlobalUserItem.nMusic or 0 ~= GlobalUserItem.nSound then
        bSlience = false
    end
    if bChange then
        if GlobalUserItem.nMusic == 0 then
            GlobalUserItem.setVoiceAble(false)
        else
            GlobalUserItem.bVoiceAble = true
            --背景音乐
            -- ExternalFun.playPlazzBackgroudAudio( )
        end

        if GlobalUserItem.nSound == 0 then
            GlobalUserItem.setSoundAble(false)
        else
            -- GlobalUserItem.bSoundAble = true
        end
        self:updateVolum()
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

return SettingLayer