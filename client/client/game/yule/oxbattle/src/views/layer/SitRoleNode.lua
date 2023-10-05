--
-- Author: zhong
-- Date: 2016-07-14 17:42:14
--
--坐下玩家
local SitRoleNode = class("SitRoleNode", cc.Node)

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

local SpineObj = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.models.SpineObj")
local ActionMgr = appdf.req(appdf.GAME_SRC.."yule.oxbattle.src.views.layer.ActionMgr")

function SitRoleNode:ctor( viewParent, index )
	self.m_parent = viewParent
	--加载csb资源
	local csbNode = ExternalFun.loadCSB("SitRoleNode.csb", self)
	self.m_csbNode = csbNode

	--背景特效
	self.m_spEffect = csbNode:getChildByName("sitdown_effect")
	self.m_spEffect:setVisible(false)

	--信息背景
	local infoBg = csbNode:getChildByName("player_info_1")
	infoBg:setLocalZOrder(1)

	--头像
	local tmp = csbNode:getChildByName("head_bg")
	self.m_headSize = tmp:getContentSize().width

	--游戏币
	self.m_textScore = csbNode:getChildByName("score_text")
	self.m_textScore:setLocalZOrder(1)

	-- self.m_csbNode:getChildByName("score_gold"):setLocalZOrder(1)

	--名字
	tmp = csbNode:getChildByName("name_text")
	self.m_clipNick = ClipText:createClipText(tmp:getContentSize(),"")
	self.m_clipNick:setPosition(tmp:getPosition())
	csbNode:addChild(self.m_clipNick)
	self.m_clipNick:setLocalZOrder(1)
	self.m_clipNick:setAnchorPoint(cc.p(0.5,0.5))
	tmp:removeFromParent()

	--庄家标示
	self.m_spBanker = csbNode:getChildByName("sp_banker")
	self.m_spBanker:setVisible(false)
	self.m_spBanker:setLocalZOrder(2)

	--分数
	self.m_atlasScore = csbNode:getChildByName("altas_score")
	self.m_atlasScore:setString("")
	self.m_atlasScore:setLocalZOrder(2)

	self.spHeadMask = csbNode:getChildByName("head_bg")

	self.m_nIndex = index
	self.m_spHead = nil

	--下注分数
	self.m_lJettonScore = {0,0,0,0}

	--飞行动画
	local moveBy = cc.MoveBy:create(1.0, cc.p(0, 50))
	local fadeout = cc.FadeOut:create(0.5)
	local call = cc.CallFunc:create(function( )
		self.m_atlasScore:setPositionY(-40)
	end)
	self.m_actShowScore = cc.Sequence:create(moveBy, fadeout, call)
	ExternalFun.SAFE_RETAIN(self.m_actShowScore)
end

function SitRoleNode:onSitDown( useritem, bAni, isBanker )
	print("SitRoleNode:onSitDown=================================================================", useritem)
	if nil == useritem then
		return
	end
	isBanker = isBanker or false

	self:setVisible(true)
	self.m_wChair = useritem.wChairID
	self.m_szNickName = useritem.szNickName
	self.m_lJettonScore = {0,0,0,0}
	--坐下特效
	-- if bAni then
	-- 	local act = cc.Repeat:create(cc.Blink:create(1.0,1),5)
	-- 	self.m_spEffect:stopAllActions()
	-- 	self.m_spEffect:runAction(act)
	-- end

	--头像
	self.spHeadMask:removeAllChildren()

	local head = PopupInfoHead:createClipHead(useritem, 94, "game_res_v2_plist/img_face_mask.png")
    local param = {}
    param.isProp = false     --显示道具互动，百人游戏不显示
    param.imgPath = "popInfo/popBg.png" --弹出框背景
    head:setPropInfo(param)
    head:setPopCsbFile("popInfo/GamePopInfo.csb") -- 自定义CSB
	if head then
		head:setPosition(44, 46)
		-- head:enableInfoPop(true)
		self.spHeadMask:addChild(head)
		-- local size = cc.Director:getInstance():getWinSize()
		-- local pos = cc.p(size.width * 0.11, size.height * 0.33)
		-- local anchor = cc.p(self.m_nIndex / 8, 1.0)
		-- if self.m_nIndex > 3 then
		-- 	pos = cc.p(size.width * 0.595, size.height * 0.33)
		-- end

		-- if (self.m_nIndex == 1) or (self.m_nIndex == 8) then
		-- 	anchor = cc.p(self.m_nIndex / 8, 0)
		-- end

		head:enableInfoPop(true, pos, anchor)
	end

	self.m_spHead = head

	--昵称
	self.m_clipNick:setString(useritem.szNickName)

	--游戏币
	local str = ExternalFun.formatScoreText(useritem.lScore)
	self.m_textScore:setString(str)

	if useritem.lScore > 0 then
		-- self.m_csbNode:getChildByName("score_gold"):setVisible(true)
	else
		-- self.m_csbNode:getChildByName("score_gold"):setVisible(false)
	end

	--庄家
	self.m_spBanker:setVisible(isBanker)
end

function SitRoleNode:getChair(  )
	return self.m_wChair
end

function SitRoleNode:getNickName( )
	return self.m_szNickName
end

--获取坐下位置
function SitRoleNode:getIndex( )
	return self.m_nIndex
end

--下注分数设置
function SitRoleNode:addJettonScore(lScore, cbArea)
	if cbArea > 4 then
		return
	end
	self.m_lJettonScore[cbArea] = self.m_lJettonScore[cbArea] + lScore
end

--清空下注分数
function SitRoleNode:clearJettonScore()
	self.m_lJettonScore = {0,0,0,0}
end

function SitRoleNode:getJettonScoreWithArea(cbArea)
	return self.m_lJettonScore[cbArea]
end

function SitRoleNode:getJettonScore()
	return self.m_lJettonScore
end

--是否庄家
function SitRoleNode:updateBanker( isBanker )
	--庄家
	self.m_spBanker:setVisible(isBanker)
end

--游戏币动画、更新自己游戏币
function SitRoleNode:updateScore( lScore )
	if nil == lScore then
		return
	end
	--游戏币
	local str = ExternalFun.formatScoreText(lScore)
	self.m_textScore:setString(str)
end

function SitRoleNode:gameEndScoreChange( useritem, changescore )
	self:updateScore(useritem.lScore + changescore)

	if 0 == changescore then
		return
	end

	self.m_atlasScore:setOpacity(255)

	local str = "." .. ExternalFun.numberThousands(changescore)
	if string.len(str) > 10 then
		str = string.sub(str,1,10) .. "///"
	end
	if changescore > 0 then
		self.m_atlasScore:setProperty(str, "game_res/num_score_add.png", 21, 30, ".")
	elseif changescore < 0 then
		self.m_atlasScore:setProperty(str, "game_res/num_score_plus.png", 21, 30, ".")
	end

	self.m_atlasScore:stopAllActions()
	self.m_atlasScore:runAction(self.m_actShowScore)
end

function SitRoleNode:reSet(  )
	self.m_textScore:setString("")
	self.m_wChair = nil
end

--分数的实现没用bmfont,需要自己拼凑,设置位置
function SitRoleNode:createFloatNode(parent, score)
    local node = cc.CSLoader:createNode("Node_float_score.csb"):addTo(parent)
    node:setPositionY(30)

    local node_positive = cc.find(node, "Node_positive")
    local node_negative = cc.find(node, "Node_negative")
    node_positive:setVisible(score>=0)
    node_negative:setVisible(score<0)

    local prefix = "+"
    local node_score = node_positive --		+.0123456789
    local oldscore = score
    if oldscore < 0 then
    	node_score = node_negative --		-.0123456789
    	prefix = "-"
    end

    score = math.abs(oldscore)

    local subfix = ""
    local scorestr = ""
	local str = ExternalFun.formatScore(score)
	if score < 10000 then
		scorestr = tostring(score)
	elseif score < 100000000 then
		subfix = "万"
		scorestr = string.format("%.2f", score / 10000)
	else
		subfix = "亿"
		scorestr = string.format("%.2f", score / 100000000)
	end

	scorestr = string.gsub(scorestr, "%.", "/")
	prefix = string.gsub(prefix, "+", ".")
	prefix = string.gsub(prefix, "-", ".")

    local lab = cc.find(node_score, "al")
    local wan = cc.find(node_score, "wan")
    local img_bg = cc.find(node_score, "img_bg")
    lab:setString(prefix .. scorestr)
    if subfix == "" then
     	wan:setVisible(false)
    elseif subfix == "亿" then
    	if oldscore > 0 then
     		wan:setTexture("game_res_v2_plist/img_wan_positive.png")
    	else
     		wan:setTexture("game_res_v2_plist/img_wan_negative.png")
    	end
 	end
 	local w1 = lab:getContentSize().width
 	local w2 = wan:getContentSize().width
 	node_score:setContentSize(cc.size(w1+w2,100))
 	lab:setPosition(0, 0)
    wan:setPosition(w1, 0)
    node_score:setPosition(-(w1+w2)*0.5, 0)
 	img_bg:setPosition((w1+w2)*0.5, 0)

    return node
end


function SitRoleNode:addWinEffect(score)
	-- print("SitRoleNode:addWinEffect======================================================================", score)
    local obj = SpineObj:create("animate/sp_face_final/skeleton.json", "animate/sp_face_final/skeleton.atlas", false)
    obj:setCompleteCallback(function() obj:destroy() end)
    self:addChild(obj)

    local node = self:createFloatNode(self, score)
    ActionMgr:floatScoreAction(node, 30)
end

function SitRoleNode:addLoseEffect(score)
	-- print("SitRoleNode:addLoseEffect======================================================================", score)
    local obj = SpineObj:create("animate/sp_face_final/skeleton.json", "animate/sp_face_final/skeleton.atlas", false)
    obj:setCompleteCallback(function() obj:destroy() end)
    self:addChild(obj)

    local node = self:createFloatNode(self, score)
    ActionMgr:floatScoreAction(node, 30)
end

return SitRoleNode