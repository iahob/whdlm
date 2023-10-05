--
-- Author: zhong
-- Date: 2017-05-31 09:15:06
--
-- 包含(推广界面-SpreadLayer、绑定界面-BindAccountLayer)
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

-- 推广界面(二级弹窗)
local SpreadLayer = class("SpreadLayer", TransitionLayer)
-- 绑定界面(三级弹窗)
local BindAccountLayer = class("BindAccountLayer", TransitionLayer)
SpreadLayer.BindAccountLayer =BindAccountLayer

local SpreadSimpleLayer = class("SpreadSimpleLayer", TransitionLayer)
SpreadLayer.SpreadSimpleLayer =SpreadSimpleLayer

-- net
local ModifyFrame = appdf.req(appdf.CLIENT_SRC .. "plaza.models.ModifyFrame")                           -- 信息修改
-- net

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "IMAGE_EDIT",           -- 编辑
    "BTN_TIPS",             -- 提示
    "TAG_MASK_2",           -- 提示底板
    "BTN_COPY",             -- 复制链接
    "BTN_INVITE",           -- 邀请
    "BTN_BIND",             -- 绑定
    "BTN_SHARE_WECHAT",     -- 分享微信
    "BTN_SHARE_CYCLE",      -- 分享朋友圈
}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)


local this 
function SpreadSimpleLayer:ctor( scene, param, level )
    this = self
    --网络回调
    local modifyCallBack = function(result,message)
        self:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    SpreadSimpleLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("spread/SpreadLayer_simple.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
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

    -- 分享微信
    btn = spbg:getChildByName("btn_share1")
    btn:setTag(TAG_ENUM.BTN_SHARE_WECHAT)
    btn:addTouchEventListener( touchFunC )
    --btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    self.m_btnShareWX = btn

    -- 分享朋友圈
    btn = spbg:getChildByName("btn_share2")
    btn:setTag(TAG_ENUM.BTN_SHARE_CYCLE)
    btn:addTouchEventListener( touchFunC )
   --btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    self.m_btnShareCycle = btn

    -- 分享链接
    self.m_szShareLink = GlobalUserItem.tabSpreadInfo["sharelink"] or yl.HTTP_URL

    self:onShowShareRewards()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function SpreadSimpleLayer:onButtonClickedEvent(tag,sender)
    

    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_SHARE_WECHAT == tag then
        MultiPlatform:getInstance():shareToTarget(0, function(isok)
                --OK请求奖励
                self:onGetShareRewards()
            end, yl.SocialShare.title,yl.SocialShare.content, self.m_szShareLink)
    elseif TAG_ENUM.BTN_SHARE_CYCLE == tag then
        MultiPlatform:getInstance():shareToTarget(1, function(isok)
                --OK请求奖励
                self:onGetShareRewards()
            end, yl.SocialShare.title, yl.SocialShare.content, self.m_szShareLink)
    end
end

function SpreadSimpleLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function SpreadSimpleLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function SpreadSimpleLayer:onTransitionInFinish()
    -- 编辑框
end

function SpreadSimpleLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function SpreadSimpleLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function SpreadSimpleLayer:onExit()
    SpreadSimpleLayer.super.onExit(self)
end



function SpreadSimpleLayer:onShowShareRewards()
   --获取玩家信息
    local data, sign = GlobalUserItem:useridSign(GlobalUserItem.tabAccountInfo.dwUserID)
    local action = string.format("action=getsharereward&userid=%s&time=%s&sign=%s",GlobalUserItem.tabAccountInfo.dwUserID, data, sign)
    print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "获取玩家分享奖励", 7)
        if type(jstable) == "table" then
            local data = jstable["data"]
            self.m_tabDetailList = {}
            if type(data) == "table" then
                local list = data["list"]
                if self.m_spBg and list[1] then
                    
                    local diamNum = list[1]["TimeShareDiamond"]
                    local goldNum =  list[1]["TimeShareGold"]
                    local diamPX = self.m_spBg:getChildByName("Sprite_2"):getPositionX()
                    if goldNum >0 then
                        local txt = self.m_spBg:getChildByName("Sprite_2"):getChildByName("Text_3_0")
                        if diamNum >0  then
                            txt:setString("x"..goldNum..",")
                            diamPX = diamPX +32+txt:getContentSize().width+2
                        else
                            txt:setString("x"..goldNum..")")
                        end
                    else
                        self.m_spBg:getChildByName("Sprite_2"):setVisible(false)
                    end

                    if diamNum >0 then
                        local txt = self.m_spBg:getChildByName("Sprite_2_0"):getChildByName("Text_3_0")
                        
                            txt:setString("x"..diamNum..")")
                            
                        self.m_spBg:getChildByName("Sprite_2_0"):setPositionX(diamPX)
                    else
                        self.m_spBg:getChildByName("Sprite_2_0"):setVisible(false)
                    end

                    if diamNum == 0 and goldNum == 0 then
                        self.m_spBg:getChildByName("Text_3"):setVisible(false)
                    end
                end
            end
        end
    end)

end



function SpreadSimpleLayer:onGetShareRewards()
    --获取玩家信息
    local data, sign = GlobalUserItem:useridSign(GlobalUserItem.tabAccountInfo.dwUserID)
    local action = string.format("action=sharetimesreward&userid=%s&time=%s&sign=%s",GlobalUserItem.tabAccountInfo.dwUserID, data, sign)
    print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
        dump(jstable, "获取玩家战绩", 7)
        if type(jstable) == "table" then
            local data = jstable["data"]
            self.m_tabDetailList = {}
            if type(data) == "table" then
                local rs = data["rs"]
                if 1 == rs then
                    -- 奖励内容
                    local str = ""
                    local ldiamond = data["TimeShareDiamond"]
                    if ldiamond > 0 then
                        str = str..ldiamond.."钻石 "
                    end
                    local lgold = data["TimeShareGold"]
                    if lgold > 0 then
                        str = str..lgold.."金币 "
                    end
                    local tips = "恭喜您获得 " .. str .. "的分享奖励!"
                    showToast(self, tips, 2)

                    --更新金币
                    -- +钻石
                    local ldiamond = tonumber(data["TimeShareDiamond"]) or 0
                    GlobalUserItem.tabAccountInfo.lDiamond =  GlobalUserItem.tabAccountInfo.lDiamond + ldiamond
                    -- +金币
                    local ldiamond = tonumber(data["TimeShareGold"]) or 0
                    GlobalUserItem.tabAccountInfo.lUserScore = GlobalUserItem.tabAccountInfo.lUserScore + lgold

                    -- 通知更新        
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY) 
                    eventListener.obj = yl.RY_MSG_USERWEALTH
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                end
            end
        end
    end)
end



-- 绑定界面
function BindAccountLayer:ctor( scene, param, level )
    --网络回调
    local modifyCallBack = function(result,message)
        self:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    BindAccountLayer.super.ctor( self, scene, param, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("spread/BindAccountLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
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
    btn:setTag(TAG_ENUM.BTN_BIND)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(false)
    btn:setPressedActionEnabled(true)
    self.m_btnSure = btn


    -- 绑定奖励
    local nre = GlobalUserItem.tabSystemConfig.BindSpreadPresent or 0
    spbg:getChildByName("txt_reward"):setString("x" .. nre)

    -- 编辑区
    self.m_editBox = nil
    self.m_bEditInput = false

    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function BindAccountLayer:onButtonClickedEvent(tag,sender)
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.BTN_BIND == tag then
        local txt = string.gsub(self.m_editBox:getText(), "[.]", "")
        if txt == "" then
            showToast(self, "推广员ID不能为空!", 1)
            return
        end
        txt = tonumber(txt)
        if nil == txt then
            showToast(self, "请输入合法的推广员ID!", 1)
            return
        end
        -- 自己
        if txt == GlobalUserItem.tabAccountInfo.dwGameID then
            showToast(self, "不能绑定自己!", 1)
            return
        end

        self:showPopWait()
        self._modifyFrame:onBindSpreader(txt)
    end
end

function BindAccountLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function BindAccountLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function BindAccountLayer:onTransitionInFinish()
    -- 编辑框
    local tmp = self.m_spBg:getChildByName("edit_bg")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 20, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceHolder("绑定推广员后将无法解绑")
            :setPlaceholderFont("fonts/round_body.ttf", 24)
            :setFont("fonts/round_body.ttf", 24)  
            --:setFontColor(cc.c4b(0,0,0,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
            :setPositionX(tmp:getPositionX() +10)
    self.m_spBg:addChild(editbox)         
    self.m_editBox = editbox
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)

    self.m_btnSure:setEnabled(true)
end

function BindAccountLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function BindAccountLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function BindAccountLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
    end
end

function BindAccountLayer:onModifyCallBack(result, tips)
    self:dismissPopWait()
    if type(tips) == "string" and "" ~= tips then
        showToast(self._scene, tips, 1)
    end
    self:scaleTransitionOut(self.m_spBg)
end

function BindAccountLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end
    BindAccountLayer.super.onExit(self)
end

-- 推广界面
function SpreadLayer:ctor( scene, param, level )
    --网络回调
    local modifyCallBack = function(result,message)
        self:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    SpreadLayer.super.ctor( self, scene, level )
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("spread/SpreadLayer.csb", self)
    self.m_rootLayer = rootLayer

    -- 按钮事件
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
    -- 提示1
    local bindSend = GlobalUserItem.tabSystemConfig.BindSpreadPresent or 0
    local szTips1 = "输入好友的游戏ID, 即可获得" .. bindSend .. "钻石奖励！"
    -- 提示2
    local effectround = GlobalUserItem.tabSystemConfig.EffectiveFriendGame or 0
    local szTips2 = "成功邀请的好友完成约战" .. effectround ..  "局后, 即成为有效好友, 可获得宝箱奖励!"

    -- 提示
    self.m_layTips = csbNode:getChildByName("panel_mask_2")
    self.m_layTips:setVisible(false)
    self.m_layTips:setTag(TAG_ENUM.TAG_MASK_2)
    self.m_layTips:addTouchEventListener( touchFunC )

    -- 提示内容
    local imagetips = self.m_layTips:getChildByName("image_tips")
    imagetips:getChildByName("txt_1"):setString(szTips1)
    imagetips:getChildByName("txt_2"):setString(szTips2)

    -- 关闭
    local tipsClose = imagetips:getChildByName("btn_close")
    tipsClose:setTag(TAG_ENUM.TAG_MASK_2)
    tipsClose:addTouchEventListener( touchFunC )

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_ENUM.TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    spbg:addTouchEventListener( touchFunC )
    self.m_spBg = spbg

    self.m_nValidSpreadCount = 0
    -- 推荐人数
    self.m_txtSpreadCount = spbg:getChildByName("txt_invite")
    self.m_txtSpreadCount:setString("0")
    -- 位置调整
    -- self.m_txtSpreadCountTip = spbg:getChildByName("spread_tips13")
    -- self.m_txtSpreadCountTip:setPositionX(self.m_txtSpreadCount:getPositionX() + self.m_txtSpreadCount:getContentSize().width/2 + 10)
    --下划线宽度设置
    -- self.m_txtSpreadCountLine = spbg:getChildByName("spread_line")
    -- self.m_txtSpreadCountLine:setScaleX(self.m_txtSpreadCount:getContentSize().width/72)


    -- 礼包
    self.m_tabImageGiftBox = {}
    -- 领取标识
    self.m_tabSpGotFlag = {}
    -- 领取箭头
    self.m_tabSpArrow = {}
    -- 邀请好友
    self.m_tabTxtInvite = {}
    -- 邀请背景
    self.m_tabSpInviteBg = {}
    local boxFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onBoxClickedEvent(ref:getTag(), ref)            
        end
    end
    for i = 1, 5 do
        -- 礼盒
        local box = spbg:getChildByName("image_giftbox_" .. i)
        box:setTag(i)
        box:addTouchEventListener( boxFunC )
        self.m_tabImageGiftBox[i] = box
        --self.m_tabImageGiftBox[i]:setVisible(false)
        self.m_tabImageGiftBox[i]:setEnabled(false)

        -- 领取标识
        local gotflag = spbg:getChildByName("sp_havegot_" .. i)
        gotflag:setVisible(false)
        self.m_tabSpGotFlag[i] = gotflag

        -- 领取箭头
        local arrow = spbg:getChildByName("sp_boxframe_" .. i)
        arrow:setVisible(false)
        self.m_tabSpArrow[i] = arrow

        -- 文本
        self.m_tabTxtInvite[i] = spbg:getChildByName("invite_" .. i)
        self.m_tabTxtInvite[i]:setVisible(false)

        -- 背景
        -- self.m_tabSpInviteBg[i] = spbg:getChildByName("sp_invitecount_bg_" .. i)
        -- self.m_tabSpInviteBg[i]:setVisible(false)
    end

    -- 关闭
    local btn = spbg:getChildByName("btn_close") 
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 提示
    btn = spbg:getChildByName("btn_tips")
    btn:setTag(TAG_ENUM.BTN_TIPS)
    btn:addTouchEventListener( touchFunC )
    self.m_btnTips = btn
    --btn:setPositionX(self.m_txtSpreadCountTip:getPositionX() + self.m_txtSpreadCountTip:getContentSize().width + 50)

    -- 邀请
    btn = spbg:getChildByName("btn_invite")
    btn:setTag(TAG_ENUM.BTN_INVITE)
    btn:addTouchEventListener( touchFunC )

    -- 复制
    btn = spbg:getChildByName("btn_copy")
    btn:setTag(TAG_ENUM.BTN_COPY)
    btn:addTouchEventListener( touchFunC )

    --我的邀请码
    local mycode = spbg:getChildByName("txt_myCode")
    mycode:setString(GlobalUserItem.tabAccountInfo.dwGameID)

    self.nodeNoBlind = spbg:getChildByName("Node_noBlind")

    -- 绑定奖励
    self.m_txtBindReward = self.nodeNoBlind:getChildByName("txt_bind")
    self.m_txtBindReward:setString("/" .. bindSend)

    -- 绑定
    btn = self.nodeNoBlind:getChildByName("btn_bind")
    btn:setTag(TAG_ENUM.BTN_BIND)
    btn:addTouchEventListener( touchFunC )
    self.m_btnBind = btn

    -- 是否编辑输入
    self.m_bEditInput = false
    -- 编辑内容
    self.m_txtEdit = self.nodeNoBlind:getChildByName("txt_agent")
    self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSpreaderAccount)
    -- 编辑按钮
    btn = self.nodeNoBlind:getChildByName("editbg")
    btn:setTag(TAG_ENUM.IMAGE_EDIT)
    btn:addTouchEventListener( touchFunC )
    self.m_imageEdit = btn
    self.m_imageEdit:setEnabled(true)

    self.nodeBlind = spbg:getChildByName("Node_blinded")
    self.m_txtMyCode = self.nodeBlind:getChildByName("txt_blindCode")
    self.m_txtMyCode:setString(GlobalUserItem.tabPlusInfo.szSpreaderAccount)

    local noSpreader = GlobalUserItem.tabPlusInfo.szSpreaderAccount == ""
    self.nodeNoBlind:setVisible(noSpreader)
    self.nodeBlind:setVisible(not noSpreader)

    --动画
    self.m_actCreateBtnAni = ExternalFun.loadTimeLine( "spread/SpreadLayer.csb")
    self.m_actCreateBtnAni:gotoFrameAndPlay(0,true)
    self:runAction(self.m_actCreateBtnAni)


    -- 分享链接
     self.m_szShareLink = GlobalUserItem.tabSpreadInfo["sharelink"] or yl.HTTP_URL
    -- 礼包组
    self.m_tabGiftBoxConfig = {}

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:scaleShakeTransitionIn(spbg)
end

-- 按键监听
function SpreadLayer:onButtonClickedEvent( tag,sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        self:scaleTransitionOut(self.m_spBg)
    elseif TAG_ENUM.IMAGE_EDIT == tag then
        --self.m_editBox:setVisible(false)
        self.m_editBox:touchDownAction(self.m_editBox, ccui.TouchEventType.ended)
        self.m_imageEdit:setEnabled(false)
    elseif TAG_ENUM.BTN_TIPS == tag then
        self.m_layTips:setVisible(true)
    elseif TAG_ENUM.BTN_INVITE == tag then
        self._scene:popTargetShare(function(target, bF2f)
            -- 面对面
            if bF2f then
                local onSaveFunc = function(ok, savepath)
                    if ok then
                        if MultiPlatform:getInstance():saveImgToSystemGallery(savepath, "qr_code.png") then
                            local runScene = cc.Director:getInstance():getRunningScene()
                            showToast(runScene, "您的推广码二维码图片已保存至系统相册", 1)
                        end
                    end
                end
                return self.m_szShareLink, onSaveFunc
            end
            if nil ~= target then
                MultiPlatform:getInstance():shareToTarget(target, function(isok)

                    end, "我的推广", "分享我的推广链接", self.m_szShareLink)
            end
        end, 7, self:getLevel() + 1)
    elseif TAG_ENUM.BTN_BIND == tag then
        local txt = string.gsub(self.m_editBox:getText(), "[.]", "")
        if txt == "" then
            showToast(self, "推广员ID不能为空!", 1)
            return
        end
        txt = tonumber(txt)
        if nil == txt then
            showToast(self, "请输入合法的推广员ID!", 1)
            return
        end
        -- 自己
        if txt == GlobalUserItem.tabAccountInfo.dwGameID then
            showToast(self, "不能绑定自己!", 1)
            return
        end

        self:showPopWait()
        self._modifyFrame:onBindSpreader(txt)
    elseif TAG_ENUM.BTN_COPY == tag then
        MultiPlatform:getInstance():copyToClipboard(self.m_szShareLink)
    elseif TAG_ENUM.TAG_MASK_2 == tag then
        self.m_layTips:setVisible(false)
    end
end

-- 礼盒监听
function SpreadLayer:onBoxClickedEvent( tag,sender )
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if self.m_bEditInput and cc.PLATFORM_OS_WINDOWS ~= targetPlatform then
        self.m_bEditInput = false
        -- editbox 隐藏键盘时屏蔽按钮
        return
    end
    local info = sender.config
    if type(info) ~= "table" then
        dump(sender.config, "desciption", 6)
        return
    end
    local configid = info.ConfigID
    if nil == configid then
        print("SpreadLayer:onBoxClickedEvent configid为nil")
        return
    end
    local needcount = info.SpreadNum or 0
    -- 未达到领取条件
    if self.m_nValidSpreadCount < needcount then
        showToast(self, "游戏推广好友未达到" .. needcount .. "人, 无法领取奖励!", 1)
        return
    end

    self:showPopWait()
    local param = "action=receivespreadaward&" .. GlobalUserItem:urlUserIdSignParam() .. "&configid=" .. configid
    appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx", "GET", param, function(jstable, jsdata)
        local msg = nil
        if type(jstable) == "table" then
            local data = jstable["data"]
            msg = jstable["msg"]
            if type(data) == "table" then
                local valid = data["valid"]
                if valid then
                    msg = ""
                    -- 钻石更新
                    local ldiamond = info.PresentDiamond
                    if type(ldiamond) == "number" and ldiamond ~= 0 then
                        GlobalUserItem.tabAccountInfo.lDiamond = GlobalUserItem.tabAccountInfo.lDiamond + ldiamond
                        msg = "恭喜获得 " .. ldiamond .. " 钻石奖励!"
                    end

                    -- 喇叭更新
                    local ntrumpet = info.PresentPropNum
                    if type(ntrumpet) == "number" and ntrumpet ~= 0 then
                        GlobalUserItem.nLargeTrumpetCount = GlobalUserItem.nLargeTrumpetCount + ntrumpet
                        msg = msg .. "恭喜获得 " .. ntrumpet .. " 个大喇叭奖励!"
                    end
                    --通知更新        
                    local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                    eventListener.obj = yl.RY_MSG_USERWEALTH
                    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

                    -- 更新礼包
                    for k,v in pairs(self.m_tabGiftBoxConfig) do
                        if v.ConfigID == configid then
                            v.Flag = true
                            break
                        end
                    end
                    self:refreshGiftBox()
                end
            end
        end
        if type(msg) == "string" and "" ~= msg then
            showToast(self, msg, 2)
        end
        self:dismissPopWait()
    end)
end

function SpreadLayer:onTransitionInFinish()
    --local effectround = GlobalUserItem.tabSystemConfig.EffectiveFriendGame or 0
    local placeholder = ""--"请输入推广员ID， 游戏" .. effectround .. "局后可领取奖励"

    -- 编辑框
    local tmp = self.m_imageEdit
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 100, tmp:getContentSize().height - 10),"public/public_blank.png")
            :setPosition(tmp:getPosition())
            :setPlaceholderFont("fonts/round_body.ttf", 1)
            :setFont("fonts/round_body.ttf", 1)  
            :setPlaceHolder(placeholder)
            --:setFontColor(cc.c4b(0,0,0,255))
            :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
            :setMaxLength(6)
    self.m_spBg:addChild(editbox)
    self.m_editBox = editbox
    self.m_editBox:setLocalZOrder(-10)
    self.m_editBox:setVisible(false)
    self.m_editBox:setOpacity(0)

    local noSpreader = GlobalUserItem.tabPlusInfo.szSpreaderAccount == ""
    if not noSpreader then
        editbox:setEnabled(false)
        editbox:setPlaceHolder(GlobalUserItem.tabPlusInfo.szSpreaderAccount)
        self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSpreaderAccount)
        self.m_btnBind:setOpacity(125)
        self.m_btnBind:setEnabled(false)
    else
        self.m_txtEdit:setString(placeholder)
    end
    self.nodeNoBlind:setVisible(noSpreader)
    self.nodeBlind:setVisible(not noSpreader)
    
    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_imageEdit:setEnabled(noSpreader)

    print("@@@@@@@@@@@@@@@推广信息@@@@@@@@@@@@@@@@@@",GlobalUserItem.tabSpreadInfo)
    dump(GlobalUserItem.tabSpreadInfo,"推广信息",6)
    if nil ~= GlobalUserItem.tabSpreadInfo then
        -- 分享链接
        self.m_szShareLink = GlobalUserItem.tabSpreadInfo["sharelink"] or yl.HTTP_URL
        print("@@@@@@@@@@@@@@@推广信息URL@@@@@@@@@@@@@@@@@@",self.m_szShareLink)

        -- 有效推广
        local ncount = GlobalUserItem.tabSpreadInfo["friendcount"] or 0
        self.m_txtSpreadCount:setString(ncount .. "")
        self.m_nValidSpreadCount = ncount

        -- 礼包
        local list = GlobalUserItem.tabSpreadInfo["list"]
        dump(list, "推广信息,礼包", 10)
        if type(list) == "table" and #list > 0 then
            self.m_tabGiftBoxConfig = list
            self:refreshGiftBox()
        end
    end

    if not GlobalUserItem.tabPlusInfo.bQueryInfo then
        -- 查询信息
        self:showPopWait()
        self._modifyFrame:onQueryUserInfo()
    end
end

function SpreadLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function SpreadLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function SpreadLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true) 
    end
end

function SpreadLayer:onExit()
    if nil ~= self._modifyFrame then
        self._modifyFrame:onCloseSocket()
        self._modifyFrame = nil
    end
    SpreadLayer.super.onExit(self)
end

function SpreadLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditInput = string.len(src) ~= 0
        --self.m_txtEdit:setVisible(false)
        --self.m_editBox:setVisible(false)
    elseif event == "changed" then
        self.m_bEditInput = string.len(src) ~= 0
        self.m_txtEdit:setString(src)
    elseif event == "return" then
        self.m_txtEdit:setVisible(true)
        self.m_txtEdit:setString(src)
        self.m_imageEdit:setEnabled(true)
    end
end

function SpreadLayer:onModifyCallBack(result, tips)
    self:dismissPopWait()
    if type(tips) == "string" and "" ~= tips then
        showToast(self, tips, 2)
    end

    if result == self._modifyFrame.INPUT_SPREADER or result == self._modifyFrame.QUERY_USERINFO then
        local noSpreader = GlobalUserItem.tabPlusInfo.szSpreaderAccount == ""
        self.m_imageEdit:setEnabled(noSpreader)
        if not noSpreader then
            self.m_editBox:setPlaceHolder(GlobalUserItem.tabPlusInfo.szSpreaderAccount)
            self.m_txtEdit:setString(GlobalUserItem.tabPlusInfo.szSpreaderAccount)

            self.m_btnBind:setOpacity(125)
            self.m_btnBind:setEnabled(false)

            self.m_txtMyCode = self.nodeBlind:getChildByName("txt_blindCode")
            self.m_txtMyCode:setString(GlobalUserItem.tabPlusInfo.szSpreaderAccount)
            self.nodeNoBlind:setVisible(noSpreader)
            self.nodeBlind:setVisible(not noSpreader)
        end
    end
end

function SpreadLayer:refreshGiftBox()
    for k,v in pairs(self.m_tabGiftBoxConfig) do
        -- 礼盒
        local imagebox = self.m_tabImageGiftBox[k]
        if nil ~= imagebox then
            imagebox.config = v
            imagebox:setVisible(true)
            imagebox:setEnabled(true)
        end

        -- 邀请
        local szinvite = self.m_tabTxtInvite[k]
        local num = v.SpreadNum or 0
        if nil ~= szinvite then
            szinvite:setString(num .. "位好友")
            szinvite:setVisible(true)
        end

        -- 邀请背景
        -- local spinvite = self.m_tabSpInviteBg[k]
        -- if nil ~= spinvite then
        --     spinvite:setVisible(true)
        -- end

        -- 箭头
        local arrow = self.m_tabSpArrow[k]
        if nil ~= arrow then
            --arrow:setVisible(true)
            -- if cc.FileUtils:getInstance():isFileExist("spread/spread_sp_arrow.png") and k ~= 5 then
            --     arrow:setSpriteFrame(cc.Sprite:create("spread/spread_sp_arrow.png"):getSpriteFrame())
            -- end
        end
        if self.m_nValidSpreadCount >= num then
            -- 已领取
            local gotflag = self.m_tabSpGotFlag[k]
            if nil ~= gotflag then
                gotflag:setVisible(v.Flag)
            end

            if nil ~= imagebox then
                if v.Flag then
                    imagebox:setEnabled(false)
                    imagebox:setOpacity(200)
                    imagebox:stopAllActions()
                else
                    imagebox:stopAllActions()
                    imagebox:runAction(cc.RepeatForever:create(
                        cc.Sequence:create(
                            cc.ScaleTo:create(0.1, 1.3), 
                            cc.DelayTime:create(0.1), 
                            cc.ScaleTo:create(0.2, 1.0),
                            cc.DelayTime:create(1.2)
                            )
                        )
                    )
                end
            end
            
            -- 可领取
            if not v.Flag  and k <= 5 and nil ~= arrow then
                -- 框
                arrow:setVisible(true)
                --arrow:setSpriteFrame(cc.Sprite:create("spread/spread_sp_boxframe.png"):getSpriteFrame())
            end
        end
    end
end

return SpreadLayer