--
-- Author: zhong
-- Date: 2017-10-10 09:36:15
--
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local IdentifyLayer = class("IdentifyLayer", TransitionLayer)

-- net
local ModifyFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ModifyFrame")                           -- 信息修改
-- net

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_SURE",             -- 确定
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)

-- 身份证验证
--验证身份证信息
--只支持18位身份证的验证

--[[
#身份证18位编码规则：dddddd yyyymmdd xxx y   
#dddddd：地区码   
#yyyymmdd: 出生年月日   
#xxx:顺序类编码，无法确定，奇数为男，偶数为女   
#y: 校验码，该位数值可通过前17位计算获得  
#<p />  
#18位号码加权因子为(从右到左) Wi = [ 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2,1 ]  
#验证位 Y = [ 1, 0, 10, 9, 8, 7, 6, 5, 4, 3, 2 ]   
#校验位计算公式：Y_P = mod( ∑(Ai×Wi),11 )   
#i为身份证号码从右往左数的 2...18 位; Y_P为脚丫校验码所在校验码数组位置  
参考代码:
      https://github.com/yujinqiu/idlint
]]
local string_len = string.len
local tonumber = tonumber

-- // wi =2(n-1)(mod 11) 
local wi = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2, 1 }; 
-- // verify digit 
local vi= { '1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2' }; 

local function isBirthDate(date)
    local year = tonumber(date:sub(1,4))
    local month = tonumber(date:sub(5,6))
    local day = tonumber(date:sub(7,8))
    if year < 1900 or year > 2100 or month >12 or month < 1 then
        return false
    end
    -- //月份天数表
    local month_days = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    local bLeapYear = (year % 4 == 0 and year % 100 ~= 0) or (year % 400 == 0)
    if bLeapYear  then
        month_days[2] = 29;
    end

    if day > month_days[month] or day < 1 then
        return false
    end

    return true
end

local function isAllNumberOrWithXInEnd( str )
    local ret = str:match("%d+X?") 
    return ret == str 
end

local function checkSum(idcard)
    -- copy from http://stackoverflow.com/questions/829063/how-to-iterate-individual-characters-in-lua-string
    local nums = {}
    local _idcard = idcard:sub(1,17)
    for ch in _idcard:gmatch"." do
        table.insert(nums,tonumber(ch))
    end
    local sum = 0
    for i,k in ipairs(nums) do
        sum = sum + k * wi[i]
    end

    return vi [sum % 11+1] == idcard:sub(18,18 )
end

local err_success = 0
local err_length = 1
local err_province = 2
local err_birth_date = 3
local err_code_sum = 4
local err_unknow_charactor = 5

local function verifyIDCard(idcard)
    if string.len(idcard) ~= 18 then
        return err_length
    end

    if not isAllNumberOrWithXInEnd(idcard) then
        return err_unknow_charactor
    end
    -- //第1-2位为省级行政区划代码，[11, 65] (第一位华北区1，东北区2，华东区3，中南区4，西南区5，西北区6)
    local nProvince = tonumber(idcard:sub(1, 2))
    if( nProvince < 11 or nProvince > 65 ) then
        return err_province
    end

    -- //第3-4为为地级行政区划代码，第5-6位为县级行政区划代码因为经常有调整，这块就不做校验

    -- //第7-10位为出生年份；//第11-12位为出生月份 //第13-14为出生日期
    if not isBirthDate(idcard:sub(7,14)) then
        return err_birth_date
    end

    if not checkSum(idcard) then
        return err_code_sum
    end

    return err_success
end

function IdentifyLayer:ctor( scene, param, level )
    local this = self
    --网络回调
    local modifyCallBack = function(result,message)
        this:onModifyCallBack(result,message)
    end
    self.m_param = param
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    IdentifyLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("usercenter/IdentifyLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 确定
    btn = spbg:getChildByName("btn_sure")
    btn:setTag(TAG_ENUM.BTN_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)
    btn:setEnabled(false)
    self.m_btnSure = btn

    -- 是否编辑输入
    self.m_bEditInput = false
    -- 用户姓名输入
    self.m_spNameEdit = spbg:getChildByName("sp_edit_name")
    -- 身份证号输入
    self.m_spIdCardEdit = spbg:getChildByName("sp_edit_idcard")

    -- 奖励
    local nre = GlobalUserItem.tabSystemConfig.RealNameAuthentPresent or 0
    spbg:getChildByName("txt_reword"):setString("x" .. nre)

    -- 加载动画
    self:scaleTransitionIn(spbg)
end

function IdentifyLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end
    IdentifyLayer.super.onExit(self)
end

function IdentifyLayer:onButtonClickedEvent(tag, ref)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_SURE then
        -- 获取姓名
        local szName = self.m_editNameBox:getText()
        if ExternalFun.stringLen(szName) < 1 then
            showToast(self, "姓名不能为空", 2)
        end
        -- 获取身份证
        local szPassportID = self.m_editIdCardBox:getText()
        local s = verifyIDCard(szPassportID)
        if s ~= err_success then
            showToast(self, "请输入正确的身份证号码!", 2)
            return
        end
        self:showPopWait()
        self._modifyFrame:onRealAuth(szName, szPassportID)
    end
end

function IdentifyLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function IdentifyLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function IdentifyLayer:onTransitionInFinish()
    local this = self
    local editHanlder = function(event,editbox)
        this:onEditEvent(event,editbox)
    end
    
    -- 编辑框
    local tmp = self.m_spNameEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width*tmp:getScaleX() - 60, tmp:getContentSize().height*tmp:getScaleY()- 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("    请输入真实姓名")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 24)
            :setFontColor(cc.c4b(0x74,0x63,0x89,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            --:setMaxLength(8)
            :setPositionX(tmp:getPositionX() + 30)
    self.m_spBg:addChild(editbox)
    self.m_editNameBox = editbox 
    editbox:registerScriptEditBoxHandler(editHanlder)   

    tmp = self.m_spIdCardEdit
    editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width*tmp:getScaleX() - 60, tmp:getContentSize().height*tmp:getScaleY() - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("    请输入身份证号")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 24)
            :setFontColor(cc.c4b(0x74,0x63,0x89,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
            :setPositionX(tmp:getPositionX() + 30)
            --:setMaxLength(18)
    self.m_spBg:addChild(editbox)
    self.m_editIdCardBox = editbox
    editbox:registerScriptEditBoxHandler(editHanlder)
    
    self.m_btnSure:setEnabled(true)
end

function IdentifyLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    end
end

function IdentifyLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function IdentifyLayer:onTransitionOutFinish()
    if type(self.m_removeCallBack) == "function" then
        self.m_removeCallBack()
    end
    self:removeFromParent()
end

function IdentifyLayer:onModifyCallBack(result, tips)
    self:dismissPopWait()
    if type(tips) == "string" and "" ~= tips then
        showToast(self._scene, tips, 2)
    end

    if type(tips) == "table" then
        local bSuccessed = tips.bSuccessed
        local sztips = tips.szNotifyContent
        if type(sztips) == "string" and "" ~= sztips then
            showToast(self._scene, sztips, 2)
        end

        -- 通知       
        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
        eventListener.obj = yl.RY_MSG_USERIDENTIFY
        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
        if self.m_param.parent and self.m_param.parent.updateIdentify then
            self.m_param.parent:updateIdentify()
        end
    end

    
    self:scaleTransitionOut(self.m_spBg)
end

return IdentifyLayer