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
    self._csbNode = ExternalFun.loadCSB("popInfo/HeadInfoLayer.csb", self);
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

    local sp_bg = self._csbNode:getChildByName("bg_kuang");
    self.m_spBg = sp_bg;
    self.node_info = sp_bg:getChildByName("Node_1");
end

function RoleInfoLayer:freshRoleInfo(roleItem, pos)
    local headIcon = self.node_info:getChildByName("_head_bg");
   
    -- 创建头像
    local head = HeadSprite:createNormal(roleItem, headIcon:getContentSize().width);
    if nil ~= head then
        -- head:setPosition(role_icon:getPosition())
        head:setPosition(cc.p(headIcon:getContentSize().width/2,headIcon:getContentSize().height/2));
        headIcon:addChild(head, 1);
    end

    local roleName = self.node_info:getChildByName("nick_text");
    local name = string.EllipsisByConfig(roleItem.szNickName,180,string.getConfig("fonts/round_body.ttf" , 28));
    roleName:setString(name or "");

    local roleId = self.node_info:getChildByName("id_text");
    roleId:setString(roleItem.dwGameID or "");

    local score_text = self.node_info:getChildByName("score_text");
    local str = ExternalFun.formatScoreText(roleItem.lScore);
    score_text:setString(str or "");

    local signText = self.node_info:getChildByName("sign_text");
    if roleItem.szSign then
        signText:setString(roleItem.szSign);
    else
        signText:setString("此人很懒，没有签名");
    end
    
    --弹窗位置调整
    self.m_spBg:setPosition(pos);
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