--
-- Author: Tang
-- Date: 2016-10-13 16:08:13
--
local Bank = class("Bank", function(module,scene)

	return display.newLayer()
end)
local Tag = 
{
	tag_btnClose   = 0,
	tag_btnTakeOut = 1
}
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.EXTERNAL_SRC .. "PopupInfoHead"
local ClipText = appdf.EXTERNAL_SRC .. "ClipText"
local g_var = ExternalFun.req_var
local game_cmd = appdf.HEADER_SRC .. "CMD_GameServer"

function Bank:ctor(module,scene)
	self._dataModule = module
	self._scene = scene
	self:initCsb()
	self:InitData()
end

function Bank:initCsb()

  local bg =  ccui.ImageView:create()
  bg:setContentSize(cc.size(yl.WIDTH, yl.HEIGHT))
  bg:setScale9Enabled(true)
  bg:setAnchorPoint(cc.p(0.5,0.5))
  bg:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  bg:setTouchEnabled(true)
  self:addChild(bg)

  bg:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
          self:runAction(cc.MoveTo:create(0.2,cc.p(0,-yl.HEIGHT)))
        end
  end)

  --加载CSB
  local csbnode = cc.CSLoader:createNode("game_res/Bank.csb");
  csbnode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
  self._rootNode = csbnode
  self:addChild(csbnode)

  
  --当前游戏币
  self.textCurrent = cc.Label:createWithTTF("0", "fonts/round_body.ttf", 20)
  self.textCurrent:setTextColor(cc.YELLOW)
  self.textCurrent:setAnchorPoint(cc.p(0.0,0.5))
  self.textCurrent:setPosition(cc.p(-145,108))
  csbnode:addChild(self.textCurrent)

--银行游戏币
  self.textBank = cc.Label:createWithTTF("0", "fonts/round_body.ttf", 20) 
  self.textBank:setTextColor(cc.YELLOW)
  self.textBank:setAnchorPoint(cc.p(0.0,0.5))
  self.textBank:setPosition(cc.p(163,108))
  csbnode:addChild(self.textBank)

  self:refreshScore()

  --取款金额
  local editbox = ccui.EditBox:create(cc.size(300, 42),"")
      :setPosition(cc.p(49,9))
      :setFontName("fonts/round_body.ttf")
      :setPlaceholderFontName("fonts/round_body.ttf")
      :setFontSize(24)
      :setPlaceholderFontSize(24)
      :setMaxLength(32)
      :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
      :setPlaceHolder("请输入取款金额")
  csbnode:addChild(editbox)
  editbox:updateNativeFrame()
  self.m_editNumber = editbox

  --取款密码
  editbox = ccui.EditBox:create(cc.size(300, 42),"")
      :setPosition(cc.p(49,-56))
      :setFontName("fonts/round_body.ttf")
      :setPlaceholderFontName("fonts/round_body.ttf")
      :setFontSize(24)
      :setPlaceholderFontSize(24)
      :setMaxLength(32)
      :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
      :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
      :setPlaceHolder("请输入取款密码")
  csbnode:addChild(editbox)
  editbox:updateNativeFrame()
  self.m_editPasswd = editbox

  --初始化按钮事件
  local btn = csbnode:getChildByName("btn_close")
  btn:setTag(Tag.tag_btnClose)
  btn:addTouchEventListener(handler(self, self.OnEvent))

  btn = csbnode:getChildByName("btn_takeOut")
  btn:setTag(Tag.tag_btnTakeOut)
  btn:addTouchEventListener(handler(self, self.OnEvent))
end

 --刷新游戏币
function Bank:refreshScore( )
    --携带游戏币
    local str = ExternalFun.numberThousands(GlobalUserItem.lUserScore)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    self.textCurrent:setString(str)

    --银行存款
    str = ExternalFun.numberThousands(GlobalUserItem.lUserInsure)
    if string.len(str) > 19 then
        str = string.sub(str, 1, 19)
    end
    
    self.textBank:setString(ExternalFun.numberThousands(GlobalUserItem.lUserInsure))
end

--申请取款
function Bank:sendTakeScore( lScore,szPassword )
  local cmddata = ExternalFun.create_netdata(g_var(game_cmd).CMD_GR_C_TakeScoreRequest)
  cmddata:setcmdinfo(g_var(game_cmd).MDM_GR_INSURE, g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
  cmddata:pushbyte(g_var(game_cmd).SUB_GR_TAKE_SCORE_REQUEST)
  cmddata:pushscore(lScore)
  cmddata:pushstring(md5(szPassword),yl.LEN_PASSWORD)

  self._scene:sendNetData(cmddata)
end

function Bank:onTakeScore( )
  --参数判断
  local szScore = string.gsub( self.m_editNumber:getText(),"([^0-9])","")
  local szPass =   self.m_editPasswd:getText()

  if #szScore < 1 then 
    showToast(cc.Director:getInstance():getRunningScene(),"请输入操作金额！",2)
    return
  end

  local lOperateScore = tonumber(szScore)
  if lOperateScore<1 then
    showToast(cc.Director:getInstance():getRunningScene(),"请输入正确金额！",2)
    return
  end

  if #szPass < 1 then 
    showToast(cc.Director:getInstance():getRunningScene(),"请输入银行密码！",2)
    return
  end
  if #szPass <6 then
    showToast(cc.Director:getInstance():getRunningScene(),"密码必须大于6个字符，请重新输入！",2)
    return
  end

  self._scene:showPopWait()
  self:sendTakeScore(lOperateScore,szPass)

  self.m_editNumber:setText("")
  self.m_editPasswd:setText("")

end

--银行操作成功
function Bank:onBankSuccess( )
  self._scene:dismissPopWait()

  local bank_success = self._scene.bank_success
  if nil == bank_success then
      return
  end
  GlobalUserItem.lUserScore = bank_success.lUserScore
  GlobalUserItem.lUserInsure = bank_success.lUserInsure

  self:refreshScore()
  showToast(cc.Director:getInstance():getRunningScene(), bank_success.szDescribrString, 2)
end

--银行操作失败
function Bank:onBankFailure( )
  self._scene:dismissPopWait()
  local bank_fail = self._scene.bank_fail
  if nil == bank_fail then
      return
  end

  showToast(cc.Director:getInstance():getRunningScene(), bank_fail.szDescribeString, 2)
end

function Bank:InitData()
 
end

function Bank:OnEvent(sender,eventType)
	local tag = sender:getTag()
	if eventType == ccui.TouchEventType.ended then
		if tag == Tag.tag_btnClose then
			self:runAction(cc.MoveTo:create(0.2,cc.p(0,-yl.HEIGHT)))
		elseif tag == Tag.tag_btnTakeOut then
      if 0 == GlobalUserItem.cbInsureEnabled then --判断是否已经开通银行
        showToast(cc.Director:getInstance():getRunningScene(), "请开通银行", 2)
        return
      end

		  self:onTakeScore()
		end
	end
end

function Bank:popLayer()
	self:runAction(cc.MoveTo:create(0.2,cc.p(0,0)))
end

-----------------------------------------------------------------------------


return Bank