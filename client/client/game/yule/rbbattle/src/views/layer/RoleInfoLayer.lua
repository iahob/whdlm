local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")

local RoleInfoLayer = class("RoleInfoLayer", cc.Layer)

RoleInfoLayer.BT_MASK = 1

--构造
function RoleInfoLayer:ctor()
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("headinfo/HeadInfoLayer.csb", self);
    self._csbNode:setPosition(0,0);

    --回调方法
    local function btnEvent( sender, eventType )
      if eventType == ccui.TouchEventType.ended then
          self:onButtonClickedEvent(sender:getTag(), sender)
      end
    end

    --遮罩
    local btn = self._csbNode:getChildByName("Panel_1");
    btn:setTag(RoleInfoLayer.BT_MASK);
    btn:addTouchEventListener(btnEvent);
    -- self.panel:setTouchEnabled(false);

    local sp_bg = self._csbNode:getChildByName("headBg");
    self.m_spBg = sp_bg;
end

function RoleInfoLayer:freshRoleInfo(  )
    local headIcon = self._csbNode:getChildByName("headIcon")
    -- local head = PopupInfoHead:createClipHead(self.m_parent._scene:GetMeUserItem(), 150)
    -- head:setPosition(headIcon:getPosition())
    -- self._csbNode:addChild(head)
    -- 创建头像
    local head = HeadSprite:createNormal(GlobalUserItem.tabAccountInfo, headIcon:getContentSize().width);
    if nil ~= head then
        -- head:setPosition(role_icon:getPosition())
        head:setPosition(cc.p(headIcon:getContentSize().width/2,headIcon:getContentSize().height/2));
        headIcon:addChild(head, 1);
    end

    local roleSexIcon = self._csbNode:getChildByName("roleSexIcon");
    if GlobalUserItem.tabAccountInfo.cbGender == 0 then
        roleSexIcon:setTexture("headinfo/icon_woman.png");
    else
        roleSexIcon:setTexture("headinfo/icon_man.png");
    end

    local roleName = self._csbNode:getChildByName("roleName");
    local name = string.EllipsisByConfig(GlobalUserItem.tabAccountInfo.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28));
    roleName:setString(name or "");

    local roleId = self._csbNode:getChildByName("roleId");
    roleId:setString(GlobalUserItem.tabAccountInfo.dwGameID or "");
    
    local roleGoldNum = self._csbNode:getChildByName("roleGoldNum");
    local userScore = self.m_parent:formatScoreText(GlobalUserItem.tabAccountInfo.lUserScore);
    roleGoldNum:setString(userScore);

    local signText = self._csbNode:getChildByName("signText");
    signText:setString("个性签名：" .. GlobalUserItem.tabPlusInfo.szSign);
end

function RoleInfoLayer:freshDeskRoleInfo(roleItem)
    local headIcon = self._csbNode:getChildByName("headIcon");
   
    
    -- 创建头像
    local head = HeadSprite:createNormal(roleItem, headIcon:getContentSize().width);
    if nil ~= head then
        -- head:setPosition(role_icon:getPosition())
        head:setPosition(cc.p(headIcon:getContentSize().width/2,headIcon:getContentSize().height/2));
        headIcon:addChild(head, 1);
    end

    local roleSexIcon = self._csbNode:getChildByName("roleSexIcon");
    if GlobalUserItem.tabAccountInfo.cbGender == 0 then
        roleSexIcon:setTexture("headinfo/icon_woman.png");
    else
        roleSexIcon:setTexture("headinfo/icon_man.png");
    end

    local roleName = self._csbNode:getChildByName("roleName");
    local name = string.EllipsisByConfig(roleItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28));
    roleName:setString(name or "");

    local roleId = self._csbNode:getChildByName("roleId");
    roleId:setString(roleItem.dwGameID or "");
    
    local roleGoldNum = self._csbNode:getChildByName("roleGoldNum");
    local userScore = self.m_parent:formatScoreText(roleItem.lScore);
    roleGoldNum:setString(userScore);

    local signText = self._csbNode:getChildByName("signText");
    if roleItem.szSign then
        signText:setString("个性签名：" .. roleItem.szSign);
    else
        signText:setString("个性签名：此人很懒，没有签名");
    end
    -- signText:setString("");
end

function RoleInfoLayer:showLayer( var )
    self:setVisible(var);
end

--按钮回调方法
function RoleInfoLayer:onButtonClickedEvent(tag, sender)
    if RoleInfoLayer.BT_MASK == tag then
        self:removeFromParent();
    end
end

--触摸回调
function RoleInfoLayer:onTouchBegan(touch, event)
   local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        local fadeout = cc.FadeOut:create(0.2)
        local call = cc.CallFunc:create(function( )
            self:removeFromParent();
            -- self.m_parent.roleInfoLayer = nil;
        end)
        self:runAction(cc.Sequence:create(fadeout, call));
    end
end

function RoleInfoLayer:onTouchEnded(touch, event)

end

return RoleInfoLayer