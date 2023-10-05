--
-- Author: zhong
-- Date: 2016-12-17 14:07:02
--
-- 包含(创建界面、aa制付款提示界面)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

-- 斗地主私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")
local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local PrivateScoreInputLayer = appdf.req(appdf.PLAZA_VIEW_SRC .. "private.PrivateScoreInputLayer")    -- 输入界面

-- 斗地主AA制提示界面
local TransitionLayer = appdf.req(appdf.EXTERNAL_SRC .. "TransitionLayer")
local PriRoomAAPayLayer = class("PriRoomAAPayLayer", TransitionLayer)
PriRoomCreateLayer.PriRoomAAPayLayer = PriRoomAAPayLayer

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")

local TAG_START             = 100
local enumTable = 
{
    "BTN_CLOSE",            -- 关闭按钮
    "TAG_MASK",             -- 遮罩
    "BTN_HELP",             -- 帮助
    "BTN_CHARGE",           -- 充值
    "BTN_MYROOM",           -- 自己房间
    "BTN_CREATE",           -- 立即开局
    "BTN_CREATE_1",         -- 代人开房
    "BTN_ENTERGAME",        -- 进入游戏
    "CBT_ONE_PAY",          -- 一人支付
    "CBT_AA_PAY",           -- AA

    -- 密码配置
    "CBT_NEEDPASSWD",       -- 需要密码
    "CBT_NOPASSWD",         -- 不需要密码

    "CBT_SCOREMODE",                -- 积分模式
    "CBT_GOLDMODE",                 -- 金币模式
    "BTN_MANGZHU",                -- 盲注
    "BTN_QIANZHU",                    -- 前注
    "BTN_MAIRU",                     --  买入
    "BTN_CREATEGOLD",               -- 金币房卡创建
    "CBT_SETTING",                  -- 是否允许陌生人进入设置



}
local TAG_ENUM = ExternalFun.declarEnumWithTable(TAG_START, enumTable)
local CBT_RULEBEGIN = 200
local CBT_BEGIN = 400
local PLAYER_BEGIN = 500
local CBT_MANG  = 600

local dropDownMenuYPox = {51, 121, 191, 261, 331}
local dropDownBgHeight = {98, 168, 238, 308, 378}

 --p配置数字
local blinds = {1, 2, 5, 10, 20, 50}   --最小买入为大盲50倍，最大为100倍
--local times = {0.5, 1, 1.5, 2, 2.5, 3.0}
local ante = {0, 1, 2, 3, 4, 5}
local maxbuy = {10000, 20000, 50000, 100000, 0} 
local values = {2, 21, 40, 59, 78, 97}  
local valuesFor5 = {3, 27, 50, 74, 98}  

-- 创建界面
function PriRoomCreateLayer:ctor( scene,param,level )
    local this = self
    PriRoomCreateLayer.super.ctor(self, scene, param, level)
    
    self.m_nSelectMin = 0
    self.m_nMangIdx = 0
    self.m_nPlayerNum = 9

    self.m_nMinCellIdx = 1 --最小携带档位(默认大盲100倍) 
    self.m_nMaxCellIdex = 2 --最大携带档位(默认大盲300倍)
    self.m_nTotalScoreIdx = 7 --累计携带无上限
    self.m_nAnteIdx = 0

    self.m_tableIndex = {1, 1, 1, 1,1} --记录四个滑动条index
    self.m_seniorButton ={{},{},{},{},{}}
    self.m_tabTime ={}

    self.param = param

    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

   

    -- 底板
    local spbg = csbNode:getChildByName("sp_bg")
    self.m_spBg = spbg

    self.m_nodeNormal = spbg:getChildByName("node_normal")
    self.m_nodeGold = spbg:getChildByName("node_gold")

    self.m_bIsNormal = self.param.cbGameMode or false --true 为积分房卡，false为金币房卡

    self.m_mangZhuScore = 0
    self.m_qianZhuScore = 0
    self.m_maiRuScore = 0

    -- 支付选择
    self.m_nSelectPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    self.m_nPayModeIdx = TAG_ENUM.CBT_ONE_PAY
    local paymodelistener = function (sender,eventType)
        this:onPayModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPayModeBox = {}
    -- 一人付
    local checkbx = self.m_nodeNormal:getChildByName("check_consume_1")
    checkbx:setTag(TAG_ENUM.CBT_ONE_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.ONE_PAY
    checkbx:setSelected(true)
    self.m_tabPayModeBox[TAG_ENUM.CBT_ONE_PAY] = checkbx
    -- AA付
    checkbx = self.m_nodeNormal:getChildByName("check_consume_2")
    checkbx:setTag(TAG_ENUM.CBT_AA_PAY)
    checkbx:addEventListener(paymodelistener)
    checkbx.nPayMode = self._cmd_pri.define.tabPayMode.AA_PAY
    self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY] = checkbx
    
    -- 是否密码
    self.m_nSelectPasswd = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    self.m_nPasswdModeIdx = TAG_ENUM.CBT_NOPASSWD
    local passwdmodelistener = function (sender,eventType)
        this:onPasswdModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabPasswdModeBox = {}
    -- 需要密码
    checkbx = self.m_nodeNormal:getChildByName("check_pwd_0")
    checkbx:setTag(TAG_ENUM.CBT_NEEDPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.SET_PASSWD
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NEEDPASSWD] = checkbx

    -- 不需要密码
    checkbx = self.m_nodeNormal:getChildByName("check_pwd_1")
    checkbx:setTag(TAG_ENUM.CBT_NOPASSWD)
    checkbx:addEventListener(passwdmodelistener)
    checkbx.nPasswdMode = self._cmd_pri.define.tabPasswdMode.NO_PASSWD
    checkbx:setSelected(true)
    self.m_tabPasswdModeBox[TAG_ENUM.CBT_NOPASSWD] = checkbx

    --俱乐部支付判断
    if self.param.cbPayTypeMask ~= nil then
        if 2 == self.param.cbPayTypeMask then

        elseif 1 == self.param.cbPayTypeMask then  --群主支付
            self.m_tabPayModeBox[TAG_ENUM.CBT_AA_PAY]:setVisible(false)
            local txt_consume_2 = self.m_nodeNormal:getChildByName("txt_consume_2")
            txt_consume_2:setVisible(false)
            --一人支付文字修改
            local txt_consume_1 = self.m_nodeNormal:getChildByName("txt_consume_1")
            txt_consume_1:setString("圈主支付")
        end
    end

    
    -- 创建按钮
    self.m_createBtn = self.m_nodeNormal:getChildByName("btn_createroom")
    self.m_createBtn:addTouchEventListener(function (ref, tType)
        if tType == ccui.TouchEventType.began then
            self.m_createBtn:runAction(cc.Sequence:create(cc.DelayTime:create(3),cc.CallFunc:create(function ()
                    self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE_1,ref)
            end)))
        --elseif tType == ccui.TouchEventType.moved then
        elseif tType == ccui.TouchEventType.canceled then
            self.m_createBtn:stopAllActions()
        elseif tType == ccui.TouchEventType.ended then
            if self.m_createBtn:getNumberOfRunningActions() >0 then
                self.m_createBtn:stopAllActions()
                self:onButtonClickedEvent(TAG_ENUM.BTN_CREATE,ref)
            end
        end
    end)
--dump(GlobalUserItem.roomlist , "GlobalUserItem.roomlist")   self.m_nSelectScore = v.lCellScore
    self.cellScoreNormal = 1
    self.cellScoreGold = 1
    for k,v in pairs(GlobalUserItem.roomlist) do
        if tonumber(v[1]) == GlobalUserItem.nCurGameKind then
            local listinfo = v[2]
            dump(listinfo , "listinfo")
            if type(listinfo) ~= "table" then
                break
            end
            local normalList = {}
            for k,v in pairs(listinfo) do
                if v.wServerType == yl.GAME_GENRE_PERSONAL and v.wKindID == 3 then
                    if v.dwSurportType==0 then
                        self.cellScoreNormal = v.lCellScore
                    elseif v.dwSurportType==1 then
                        self.cellScoreGold = v.lCellScore
                    end
                end
            end
            break
        end
    end 

    
     --最大最小携带
    self.m_buyMin = self.m_nodeNormal:getChildByName("txt_takeScoreMin")
    self.m_buyMax = self.m_nodeNormal:getChildByName("txt_takeScoreMax")

    
    for i=1, 6 do
        local txtTime = self.m_nodeNormal:getChildByName("txt_time"..i)
        if nil ~= txtTime then
            txtTime:setString("")
            self.m_seniorButton[1][i] = txtTime
            txtTime:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

            if i == self.m_tableIndex[1] then
                txtTime:setColor(cc.c3b(0xff, 0xea, 0x00))
            end
        end
 

        local txtBlinds= self.m_nodeNormal:getChildByName("txt_blinds"..i)
        local vSmall = ExternalFun.formatScoreText(blinds[i] *self.cellScoreNormal) 
        local vBig = ExternalFun.formatScoreText(blinds[i] *2 *self.cellScoreNormal)

        txtBlinds:setString("小:"..vSmall .. "\n" .. "大:"..vBig)--setFontSize  setPositionX
        print(txtBlinds:getPositionY() , "********")
        txtBlinds:setPositionY(322)
        --txtBlinds:setString(blinds[i] *self.cellScoreNormal .. "/" .. blinds[i] *2 *self.cellScoreNormal)
        self.m_seniorButton[2][i] = txtBlinds
        txtBlinds:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

        if i == self.m_tableIndex[2] then
            txtBlinds:setColor(cc.c3b(0xff, 0xea, 0x00))
        end

        local txtAnte = self.m_nodeNormal:getChildByName("txt_ante"..i)
        txtAnte:setString(""..ante[i])
        self.m_seniorButton[3][i] = txtAnte
        txtAnte:setColor(cc.c3b(0xcd, 0xc0, 0x7f))
        if i == self.m_tableIndex[3] then
            txtAnte:setColor(cc.c3b(0xff, 0xea, 0x00))
        end

        if i == 6 then
            local txtBuyMax = self.m_nodeNormal:getChildByName("txt_buymax"..i)
            txtBuyMax:setString("无限制")
            self.m_seniorButton[4][i]=txtBuyMax
            txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

            if i == self.m_tableIndex[4] then
                txtBuyMax:setColor(cc.c3b(0xff, 0xea, 0x00))
            end
        else
            local txtBuyMax = self.m_nodeNormal:getChildByName("txt_buymax"..i)
            txtBuyMax:setString(""..blinds[i] *self.cellScoreNormal *200 *blinds[1])
            self.m_seniorButton[4][i]=txtBuyMax
            txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

            if i == self.m_tableIndex[4] then
                txtBuyMax:setColor(cc.c3b(0xff, 0xea, 0x00))
            end
        end


    end

    local score = blinds[1] *self.cellScoreNormal *100
    local strScore = ""..score
    if score > 9999 then
        strScore = string.format("%d:", score/10000)
    end
    self.m_buyMin:setString(strScore)
    local score = blinds[1] *self.cellScoreNormal *200 
    local strScore = ""..score
    if score > 9999 then
        strScore = string.format("%d:", score/10000)
    end
    self.m_buyMax:setString(strScore)

    local slider_1 = self.m_nodeNormal:getChildByName("Slider_1") 
    local slider_2 = self.m_nodeNormal:getChildByName("Slider_2")
    local slider_3 = self.m_nodeNormal:getChildByName("Slider_3") 
    local slider_4 = self.m_nodeNormal:getChildByName("Slider_4") 
    slider_1:setPercent(3)
    slider_1:setTag(1)
    slider_2:setPercent(2)
    slider_2:setTag(2)
    slider_3:setPercent(2)
    slider_3:setTag(3)
    slider_4:setPercent(2)
    slider_4:setTag(4)

    local funcSlider = function( ref,eventType )
        local value = ref:getPercent()
        print("value=====",value)
        if value < 10  then
            self.m_tableIndex[ref:getTag()] = 1
        elseif value < 30 then
            self.m_tableIndex[ref:getTag()] = 2  
        elseif value < 50 then
            self.m_tableIndex[ref:getTag()] = 3  
        elseif value < 70 then
            self.m_tableIndex[ref:getTag()] = 4  
        elseif value < 90 then
            self.m_tableIndex[ref:getTag()] = 5  
        else
            self.m_tableIndex[ref:getTag()] = 6                   
        end

        ref:setPercent(values[self.m_tableIndex[ref:getTag()]]) 

        for k,v in pairs(self.m_seniorButton[ref:getTag()]) do
            if k == self.m_tableIndex[ref:getTag()] then
                v:setFontSize(24)
                v:setColor(cc.c3b(0xff, 0xea, 0x00))
            else
                v:setFontSize(20)
                v:setColor(cc.c3b(0xcd, 0xc0, 0x7f))
            end
        end

        if 2 == ref:getTag() then  --最大携带的数字随底注变换
           for i=1, 5 do  --最后一个不变
                local txtBuyMax = self.m_nodeNormal:getChildByName("txt_buymax"..i)
                local score = blinds[self.m_tableIndex[2]] *self.cellScoreNormal *200 *blinds[i]
                local strScore = ""..blinds[self.m_tableIndex[2]] *self.cellScoreNormal *200 *blinds[i]
                txtBuyMax:setString(strScore)
                self.m_seniorButton[4][i]=txtBuyMax
                txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

                if i == self.m_tableIndex[4] then
                    txtBuyMax:setColor(cc.c3b(0xff, 0xea, 0x00))
                end
           end

           for i=1, 6 do  --前注随小盲变化
                local txtAnte = self.m_nodeNormal:getChildByName("txt_ante"..i)
                local score = blinds[self.m_tableIndex[2]] *self.cellScoreNormal *ante[i]
                txtAnte:setString(score)
                self.m_seniorButton[3][i]=txtAnte
                txtAnte:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

                if i == self.m_tableIndex[3] then
                    txtAnte:setColor(cc.c3b(0xff, 0xea, 0x00))
                end
           end

            local score = blinds[self.m_tableIndex[2]] *self.cellScoreNormal *100
            local strScore = ""..score
            if score > 9999 then
                strScore = string.format("%d:", score/10000)
            end
            self.m_buyMin:setString(strScore)
            local score = blinds[self.m_tableIndex[2]] *self.cellScoreNormal *200 
            local strScore = ""..score
            if score > 9999 then
                strScore = string.format("%d:", score/10000)
            end
            self.m_buyMax:setString(strScore)--self.m_tableIndex[ref:getTag()]


        end
    end
    slider_1:addEventListener(function( ref,eventType )
        local value = ref:getPercent()
        print("value=====",value)
        if value < 13  then
            self.m_tableIndex[ref:getTag()] = 1
        elseif value < 38 then
            self.m_tableIndex[ref:getTag()] = 2  
        elseif value < 63 then
            self.m_tableIndex[ref:getTag()] = 3  
        elseif value < 88 then
            self.m_tableIndex[ref:getTag()] = 4  
        else
            self.m_tableIndex[ref:getTag()] = 5                   
        end
        ref:setPercent(valuesFor5[self.m_tableIndex[ref:getTag()]]) 

        for k,v in pairs(self.m_seniorButton[ref:getTag()]) do
            if k == self.m_tableIndex[ref:getTag()] then
                v:setScale(1.2)
                v:setColor(cc.c3b(0xff, 0xea, 0x00))
            else
                v:setScale(1)
                v:setColor(cc.c3b(0xcd, 0xc0, 0x7f))
            end
        end

        --时间的设置会导致费用变化
        if self.m_tableIndex[ref:getTag()] > #PriRoom:getInstance().m_tabFeeConfigList then
            self.m_tableIndex[ref:getTag()] = self.m_tableIndex[ref:getTag()] -1 
            ref:setPercent(values[self.m_tableIndex[ref:getTag()]]) 
        end
        -- 更新费用
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_tableIndex[ref:getTag()]]
        self.m_nSelectMin = self.m_tabSelectConfig.dwDrawTimeLimit
        self:updateCreateFee()
    
    end)
    slider_2:addEventListener(funcSlider)
    slider_3:addEventListener(funcSlider)
    slider_4:addEventListener(funcSlider)

    local btn = self.m_nodeGold:getChildByName("Edit_1")
    btn:setTag(TAG_ENUM.BTN_MANGZHU)
    btn:addTouchEventListener(btncallback)

     local btn = self.m_nodeGold:getChildByName("Edit_2")
    btn:setTag(TAG_ENUM.BTN_QIANZHU)
    btn:addTouchEventListener(btncallback)

    local btn = self.m_nodeGold:getChildByName("btn_createroom")
    btn:setTag(TAG_ENUM.BTN_CREATEGOLD)
    btn:addTouchEventListener(btncallback)

    self.m_buyMinGold = self.m_nodeGold:getChildByName("txt_takeScoreMin")
    self.m_buyMaxGold = self.m_nodeGold:getChildByName("txt_takeScoreMax")



     

    --是否允许陌生人加入
    self.m_nOpenMode = true
    local openmodelistener = function (sender,eventType)
        this:onOpenModeSelectedEvent(sender:getTag(),sender)
    end
    local checkbx = self.m_nodeGold:getChildByName("check_set")
    checkbx:setTag(TAG_ENUM.CBT_SETTING)
    checkbx:setSelected(true)
    checkbx:addEventListener(openmodelistener)

    self.m_txtMangZhuLimit = self.m_nodeGold:getChildByName("EditText_1")
    self.m_txtQianZhuLimit = self.m_nodeGold:getChildByName("EditText_2")

    self.m_txtMangZhuTips = self.m_nodeGold:getChildByName("MangZhu_tip")
    self.m_mangZhuScore = self.cellScoreGold
    self.m_txtMangZhuLimit:setString(self.m_mangZhuScore)
    self.m_txtMangZhuTips:setString("盲注范围:".. self.cellScoreGold .."-".. self.cellScoreGold*50 .. "(大盲:" .. self.cellScoreGold*2 .. ")")

    self.m_txtQianZhuTips = self.m_nodeGold:getChildByName("QianZhu_tip")
    self.m_qianZhuScore = 0
    self.m_txtQianZhuLimit:setString(self.m_qianZhuScore)
    self.m_txtQianZhuTips:setString("前注范围:".. 0 .."-".. self.m_mangZhuScore*5 )




    for i=1, 6 do

        if i == 6 then
            local txtBuyMax = self.m_nodeGold:getChildByName("txt_buymax"..i)
            txtBuyMax:setString("无限制")
            self.m_seniorButton[5][i]=txtBuyMax
            txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

            if i == self.m_tableIndex[5] then
                txtBuyMax:setColor(cc.c3b(0xff, 0xea, 0x00))
            end
        else
            local txtBuyMax = self.m_nodeGold:getChildByName("txt_buymax"..i)
            txtBuyMax:setString(""..self.m_mangZhuScore*2*100*blinds[i])
            self.m_seniorButton[5][i]=txtBuyMax
            txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

            if i == self.m_tableIndex[5] then
                txtBuyMax:setColor(cc.c3b(0xff, 0xea, 0x00))
            end
        end
    end

    local slider_5 = self.m_nodeGold:getChildByName("Slider_5") 
    slider_5:setPercent(3)
    slider_5:setTag(5)
    slider_5:addEventListener(funcSlider)

    
    local score = self.m_mangZhuScore*100
    local strScore = ""..self.m_mangZhuScore*100
    if score > 9999 then
        strScore = string.format("%d:", score/10000)
    end
    
    self.m_buyMinGold:setString(strScore)

    local score = self.m_mangZhuScore*200
    local strScore = ""..self.m_mangZhuScore*200
    if score > 9999 then
        strScore = string.format("%d:", score/10000)
    end
    self.m_buyMaxGold:setString(strScore)


-- 创建费用
    self.m_txtFee = self.m_nodeNormal:getChildByName("txt_feetime")
    self.m_txtFee:setString("")

    self:onGameModeSelectedEvent(self.m_bIsNormal and TAG_ENUM.CBT_SCOREMODE or TAG_ENUM.CBT_GOLDMODE)

    -- 注册事件监听
    self:registerEventListenr()
    -- 加载动画
    self:noActionTransitionIn(self)
end

--输入框确定时调用，用来设置数据
function PriRoomCreateLayer:setConfigString(tag, scoere)
    print("输入框回调",tag,scoere)
 
    if TAG_ENUM.BTN_MANGZHU == tag then
        if scoere<self.cellScoreGold then
            scoere = self.cellScoreGold
        elseif scoere>self.cellScoreGold*50 then 
            scoere = self.cellScoreGold*50
        end

        self.m_mangZhuScore = scoere
        self.m_txtMangZhuLimit:setString(self.m_mangZhuScore)
        self.m_txtMangZhuTips:setString("盲注范围:".. self.cellScoreGold .."-".. self.cellScoreGold*50 .. "(大盲:" .. self.m_mangZhuScore*2 .. ")")

        self.m_qianZhuScore = 0
        self.m_txtQianZhuLimit:setString(self.m_qianZhuScore)
        self.m_txtQianZhuTips:setString("前注范围:".. 0 .."-".. self.m_mangZhuScore*5 )

        for i=1, 6 do
            if i == 6 then
                local txtBuyMax = self.m_nodeGold:getChildByName("txt_buymax"..i)
                txtBuyMax:setString("无限制")
                txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))

                
            else
                local txtBuyMax = self.m_nodeGold:getChildByName("txt_buymax"..i)
                txtBuyMax:setString(""..self.m_mangZhuScore*2*100*blinds[i])
                txtBuyMax:setColor(cc.c3b(0xcd, 0xc0, 0x7f))
            end
        end
        local score = self.m_mangZhuScore*100
        local strScore = ""..self.m_mangZhuScore*100
        if score>9999 then
            strScore = string.format("%d:",score/10000)
        end
        self.m_buyMinGold:setString(strScore)

        local score = self.m_mangZhuScore*200
        local strScore = ""..self.m_mangZhuScore*200
        if score>9999 then
            strScore = string.format("%d:",score/10000)
        end
        self.m_buyMaxGold:setString(strScore)
 
    elseif TAG_ENUM.BTN_QIANZHU == tag then
        
        if scoere > self.m_mangZhuScore*5 then
            scoere = self.m_mangZhuScore*5
        end
        self.m_qianZhuScore = scoere
        self.m_txtQianZhuLimit:setString(scoere)

    elseif TAG_ENUM.BTN_MAIRU == tag then
        
        self.m_maiRuScore = scoere
        self.m_txtMaiRuLimit:setString(scoere)
    end
end

function PriRoomCreateLayer:onOpenModeSelectedEvent( tag, sender )
    self.m_nOpenMode = not self.m_nOpenMode
    sender:setSelected(self.m_nOpenMode)
end

--积分房，金币房选择
function PriRoomCreateLayer:onGameModeSelectedEvent( tag, sender )
    
    self.m_bIsNormal = (tag == TAG_ENUM.CBT_SCOREMODE)

    self.m_nodeGold:setVisible(not self.m_bIsNormal)
    self.m_nodeNormal:setVisible(self.m_bIsNormal)

    

end

function PriRoomCreateLayer:reFreshRule()
    local cellScore = {2,4,10,20,40,100,400,1000}
    local minMuitple = {50,100,200,300,400,500,1000,2000}
    local maxMuitple = {100,200,300,400,500,1000,2000,5000}
    local anteMuitple = {0.0,0.5,1.0,1.5,2.0,2.5}
    local minLimit = cellScore[self.m_nMaxCellIdex+1]*minMuitple[self.m_nMinCellIdx+1]
    local maxLimit = cellScore[self.m_nMaxCellIdex+1]*maxMuitple[self.m_nMaxCellIdex+1]
    local totalStr = "累计总带入无限制"
    if self.m_nTotalScoreIdx ~= 7 then 
        totalStr = "累计带入"..cellScore[self.m_nMaxCellIdex+1]*maxMuitple[self.m_nTotalScoreIdx+1]
    end
    local ante = cellScore[self.m_nMaxCellIdex+1]*anteMuitple[self.m_nAnteIdx+1]
    if ante == 0 then
        self.m_txtSelectRule:setString("最小带入"..minLimit.." | 最大带入"..maxLimit.." | "..totalStr)
    else
        self.m_txtSelectRule:setString("最小带入"..minLimit.." | 最大带入"..maxLimit.." | "..totalStr.." | ANTE底注"..ante)
    end

    self.m_minScore = minLimit

    local len = self.m_txtSelectRule:getContentSize().width
    self.m_txtSelectRule:setAnchorPoint(cc.p((len + 126)/2/len,0.5))
    self.m_txtSelectRule:setPositionX(605)
    self.m_senior:setPositionX(len-30)

    self.m_txtTakeScoreMin = self.m_spBg:getChildByName("pri_orginal_chip")
    self.m_txtTakeScoreMin:setString(""..minLimit)
end



------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    --self.m_txtCardNum:setString(GlobalUserItem.tabAccountInfo.lDiamond .. "")
end

-- 刷新费用列表
function PriRoomCreateLayer:onRefreshFeeList()
    PriRoom:getInstance():dismissPopWait()
    local this = self

    local cbtlistener = function (sender,eventType)
        this:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}
    -- self.m_txtSelectRound = self.m_spBg:getChildByName("txt_selectTime")
    -- self.m_txtSelectRound:setString("")
    self.m_nSelectMin = 0
    local nConfig = #PriRoom:getInstance().m_tabFeeConfigList
    dump(PriRoom:getInstance().m_tabFeeConfigList, "the fee list", 6)
    -- 时间 
    for i = 1, nConfig do
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local txt_time = self.m_nodeNormal:getChildByName("txt_time" .. i)
        if nil ~= txt_time then
            -- 设置位置
            self.m_tabTime[i] = config.dwDrawTimeLimit
            txt_time:setString(config.dwDrawTimeLimit .. "分钟")
            txt_time:setVisible(true)
        end
    end


    -- 选择的玩法    
    self.m_nSelectIdx = 1
    self.m_tabSelectConfig = nil
    if nConfig > 0 then
        self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[1]
        --self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)
        --self.m_txtSelectRound:setString(string.format("%d分钟", self.m_tabSelectConfig.dwDrawTimeLimit))

        self.m_nSelectMin = self.m_tabSelectConfig.dwDrawTimeLimit
        -- 更新费用
        self:updateCreateFee()
    end

    dump(PriRoom:getInstance().m_tabFeeConfigList,"===========================111")
    -- 免费判断
    if PriRoom:getInstance().m_bLimitTimeFree then
        local wStart = PriRoom:getInstance().m_tabRoomOption.wBeginFeeTime or 0
        local wEnd = PriRoom:getInstance().m_tabRoomOption.wEndFeeTime or 0
        -- 免费时间
        local szfree = string.format("( 限时免费: %02d:00-%02d:00 )", wStart,wEnd)
        self.m_spBg:getChildByName("txt_feetime"):setString(szfree)

        -- 消耗隐藏
        self.m_spBg:getChildByName("txt_createfee_0"):setVisible(false)
        -- 费用隐藏
        self.m_txtFee:setVisible(false)
        -- 钻石隐藏
        self.m_spBg:getChildByName("sp_diamond"):setVisible(false)
    end




end

function PriRoomCreateLayer:onLoginPriRoomFinish()
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end
    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE 
        or meUser.cbUserStatus == yl.US_NULL 
        or meUser.cbUserStatus == yl.US_PLAYING
        or meUser.cbUserStatus == yl.US_SIT)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            -- 创建登陆
            local buffer = ExternalFun.create_netdata(126)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            buffer:pushscore(self.m_bIsNormal and self.cellScoreNormal or self.cellScoreGold )
            buffer:pushdword(--[[self.m_tabSelectConfig and self.m_tabSelectConfig.dwDrawCountLimit or]] 0)
            buffer:pushdword(self.m_tabSelectConfig and self.m_tabSelectConfig.dwDrawTimeLimit or 0)
            buffer:pushword(9)
            buffer:pushdword(0)
            -- 密码设置
            buffer:pushbyte(self.m_nSelectPasswd)
            -- 支付方式
            buffer:pushbyte(self.m_nSelectPayMode)
            
             --额外配置
            buffer:pushbyte(1)
            buffer:pushbyte(0) --人数
            buffer:pushbyte(self.m_nPlayerNum) --最大人数


            local score = blinds[self.m_tableIndex[2]] *(self.m_bIsNormal and self.cellScoreNormal or self.cellScoreGold) *200 *(self.m_tableIndex[4] ==6 and 999999 or  blinds[self.m_tableIndex[4]])
            local str  = tostring(score)
            local len = string.len(str)

            --
            local tablet={}
            for i=1,20 do
                    if i<= len then  
                        buffer:pushbyte(tonumber(string.sub(str,(len-i+1),(len-i+1))))
                        table.insert(tablet,tonumber(string.sub(str,(len-i+1),(len-i+1))))
                    else
                        buffer:pushbyte(0)
                        table.insert(tablet,0)
                    end
            end
            --dump(tablet,"=====================3")
            if self.m_bIsNormal then
                buffer:pushbyte(self.m_tableIndex[2] -1) --大盲默认 1/2
                buffer:pushbyte(self.m_tableIndex[4] -1) --累计带入
                buffer:pushbyte(self.m_tableIndex[3] -1) --ante
                buffer:pushbyte(self.m_nSelectMin)--时间

                
              
            else
                buffer:pushint(self.m_mangZhuScore) --发的是小盲，服务端×2
                buffer:pushint(self.m_qianZhuScore) --前注
                --buffer:pushbyte(self.m_nSelectMin)--时间
                 
            end


            dump(self.m_tableIndex,"======================1111")
            
            --游戏规则
            for i = 1, self.m_bIsNormal and 73 or 69 do
                buffer:pushbyte(0)
            end
            buffer:pushbyte(self.m_tabSelectConfig ~= nil and self.m_tabSelectConfig.cbGameMode or 0)
            buffer:pushscore(self.m_bIsNormal and 0 or self.m_mangZhuScore * 100)
            buffer:pushscore(self.m_bIsNormal and 0 or self.m_mangZhuScore * 100)
            print("----------------------------->", self.m_bIsNormal)
            if self.m_nOpenMode then
                buffer:pushbyte(1)
                print("fa fa fa 1111111111111")
            else
                buffer:pushbyte(0)
                print("fa fa fa 00000000000000")
            end
            if self.param.clubID ~= nil then
                buffer:pushdword(self.param.clubID)  --0为非俱乐部，俱乐部创建填俱乐部ID
            else
                buffer:pushdword(0)  --0为非俱乐部，俱乐部创建填俱乐部ID
            end
            buffer:pushdword(0)
            PriRoom:getInstance():getNetFrame():sendGameServerMsg(buffer)
            return true
        end        
    end
    return false
end

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local dwRoomID = roomDetailInfo.dwRoomID or 0
    local turnCount = roomDetailInfo.dwPlayTurnCount or 0
    local passwd = roomDetailInfo.dwRoomDwd or 0
    local baseScore = "底分:"..cc.UserDefault:getInstance():getIntegerForKey("land_baseScore",0)..","

    local shareTxt = "局数:" .. turnCount .. "局。"..baseScore.."德州扑克游戏精彩刺激, 一起来玩吧!"
    local url = yl.PRIVATE_SHARE_URL .. "?g=" .. GlobalUserItem.tabAccountInfo.dwGameID .. "&r=" .. dwRoomID .. "&k=200&a=1" .. "&p=" .. passwd
    print("获取分享信息===》", dwRoomID,shareTxt,  url)
    return {title = string.format("【约战房间: %06d】", dwRoomID), content = shareTxt, friendContent = "", url = url}
end

function PriRoomCreateLayer:getCopyShareMsg(roomDetailInfo)
    local dwRoomID = roomDetailInfo.dwRoomID or ""
    return {content = string.format("德州扑克, 房号[%06d],您的好友喊你打牌了!", dwRoomID)}
end

function PriRoomCreateLayer:onExit()
    --cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    --cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

function PriRoomCreateLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PriRoomCreateLayer:onTransitionOutFinish()
    self:removeFromParent()
end

function PriRoomCreateLayer:onOtherShowEvent()
    if self:isVisible() then
        self:setVisible(false)
    end
end

function PriRoomCreateLayer:onOtherHideEvent()
    if not self:isVisible() then
        self:setVisible(true)
    end
end
------
-- 继承/覆盖
------

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if TAG_ENUM.BTN_HELP == tag then
        --self._scene:popHelpLayer2(200, 1)
    elseif TAG_ENUM.BTN_CREATE == tag
    or TAG_ENUM.BTN_CREATE_1 == tag then 
        if self.m_bLow then
            local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
            local query = QueryDialog:create("您的钻石数量不足，是否前往商城充值！", function(ok)
                if ok == true then
                    self._scene:popShopLayer()           
                end
                query = nil
            end):setCanTouchOutside(false)
                :addTo(self._scene)
            return
        end
        if nil == self.m_tabSelectConfig or table.nums(self.m_tabSelectConfig) == 0 then
            showToast(self, "未选择玩法配置!", 2)
            return
        end

        -- if self.m_tabSelectConfig.lIniScore < self.m_minScore then
        --     showToast(self, "房间配置积分低于最小买入分，请重新配置！", 2)
        --     return
        -- end

        PriRoom:getInstance():setViewFrame(self)
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = (TAG_ENUM.BTN_CREATE_1 == tag)
        -- 创建房间
        PriRoom:getInstance().m_bPlayModel = 0

        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    elseif TAG_ENUM.MENU_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_1 == tag then
        self.m_maskDropDown1:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow1:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow1:loadTextureNormal("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_up_1.png")
    elseif TAG_ENUM.MASK_DROPDOWN_2 == tag then
        self.m_maskDropDown2:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow2:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow2:loadTextureNormal("room/land_pribtn_arrow_down_1.png")
    elseif TAG_ENUM.MENU_DROPDOWN_3 == tag then
        self.m_maskDropDown3:setVisible(true)
        -- 更新箭头
        self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_up_0.png")
        self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_up_1.png")    
    elseif TAG_ENUM.MASK_DROPDOWN_3 == tag then
        self.m_maskDropDown3:setVisible(false)
        -- 更新箭头
        self.m_btnDropArrow3:loadTextureDisabled("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_down_0.png")
        self.m_btnDropArrow3:loadTextureNormal("room/land_pribtn_arrow_down_1.png")  
    elseif TAG_ENUM.BTN_MANGZHU == tag or TAG_ENUM.BTN_QIANZHU == tag or TAG_ENUM.BTN_MAIRU == tag then
        --创建输入框
        local param = {}
        param.tag = tag
        local pl = PrivateScoreInputLayer:create(self, param, PrivateScoreInputLayer.SECOND_LEVEL)
        self._scene:addPopLayer(pl)
    elseif TAG_ENUM.BTN_CREATEGOLD == tag then
        if GlobalUserItem.tabAccountInfo.lUserScore < self.m_mangZhuScore then 
            self.m_Query = QueryDialog:create("您的金币数量不足，是否前往商城充值？", function(ok)
                if ok then
                    self._scene:popShopLayer() 
                end
                self.m_Query = nil
            end, nil, QueryDialog.QUERY_SURE_CANCEL)
            :setCanTouchOutside(false)
            :addTo(self._scene)
        end
        -- 是否代开
        PriRoom:getInstance().m_bCreateForOther = false
        --私有房模式
        PriRoom:getInstance(). m_bPlayModel = 1
        -- 创建房间
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    end  
end

function PriRoomCreateLayer:onPayModeSelectedEvent( tag, sender )
    if self.m_nPayModeIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nPayModeIdx = tag
    for k,v in pairs(self.m_tabPayModeBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= sender.nPayMode then
        self.m_nSelectPayMode = sender.nPayMode
    end
    -- 更新费用
    self:updateCreateFee()
end

function PriRoomCreateLayer:onPasswdModeSelectedEvent( tag, sender )
    if self.m_nPasswdModeIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nPasswdModeIdx = tag
    for k,v in pairs(self.m_tabPasswdModeBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    if nil ~= sender.nPasswdMode then
        self.m_nSelectPasswd = sender.nPasswdMode
    end
end

function PriRoomCreateLayer:onSelectedEvent(tag, sender)

    self:onButtonClickedEvent(TAG_ENUM.MASK_DROPDOWN_2)
    if self.m_nSelectIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectIdx = tag
    for k,v in pairs(self.m_tabCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[tag - CBT_BEGIN]
    self.m_nSelectMin = self.m_tabSelectConfig.dwDrawTimeLimit
    if nil == self.m_tabSelectConfig then
        return
    end
    self.m_txtSelectRound:setString(string.format("%d分钟", self.m_tabSelectConfig.dwDrawTimeLimit))

    -- 更新费用
    self:updateCreateFee()
end

function PriRoomCreateLayer:updateCreateFee()
    self.m_bLow = false
    if GlobalUserItem.tabAccountInfo.lDiamond < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end
    local fee = 0
    if self.m_nSelectPayMode == self._cmd_pri.define.tabPayMode.AA_PAY then
        -- AA付
        fee = self.m_tabSelectConfig.wAAPayFee or 0
    else
        -- 一人付
        fee = self.m_tabSelectConfig.lFeeScore or 0
    end
    self.m_txtFee:setString( "x" .. fee )
end

-- AA制界面
function PriRoomAAPayLayer:ctor( scene, param, level )
    PriRoomAAPayLayer.super.ctor( self, scene, param, level )
dump(param , "111121221")
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomAAPayLayer.csb", self)
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
    self.m_spBg = spbg

    -- 关闭
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(TAG_ENUM.BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 进入
    btn = spbg:getChildByName("btn_entergame")
    btn:setTag(TAG_ENUM.BTN_ENTERGAME)
    btn:addTouchEventListener( touchFunC )
    btn:setPressedActionEnabled(true)

    -- 房间id
    local roomid = self._param.dwRoomId or 0
    spbg:getChildByName("txt_roomid"):setString(string.format("%06d", roomid))

    -- 消耗钻石
    local consume = self._param.lDiamondFee or 0
    spbg:getChildByName("txt_consume"):setString(consume .. "")

    -- 局数
   
    
    local buffer = self._param.buffer
    if nil ~= buffer and nil ~= buffer.readbyte then
        -- 读底分
        --local lscore = buffer:readbyte()
        -- 玩法
        --spbg:getChildByName("txt_wanfa"):setString(lscore .. "")
        for i=1 , 26 do
            if i==26 then
                spbg:getChildByName("txt_jushu"):setString(buffer:readbyte() .. "分钟")
            end
            print(i , "***",buffer:readbyte())
        end
    end
    self:scaleTransitionIn(spbg)
end

function PriRoomAAPayLayer:onButtonClickedEvent( tag,sender )
    if tag == TAG_ENUM.TAG_MASK or tag == TAG_ENUM.BTN_CLOSE then
        -- 断开
        PriRoom:getInstance():closeGameSocket()

        self:scaleTransitionOut(self.m_spBg)
    elseif tag == TAG_ENUM.BTN_ENTERGAME then
        -- 判断是否密码, 且非房主
        if self._param.bRoomPwd and GlobalUserItem.tabAccountInfo.dwUserID ~= self._param.dwRommerID then
            PriRoom:getInstance():passwdInput()
        else
            PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_SEARCHROOM
            PriRoom:getInstance():showPopWait()
            PriRoom:getInstance():getNetFrame():sendEnterPrivateGame()
        end
    end
end

function PriRoomAAPayLayer:animationRemove()
    self:scaleTransitionOut(self.m_spBg)
end

function PriRoomAAPayLayer:onTransitionInBegin()
    self:sendShowEvent()
end

function PriRoomAAPayLayer:onTransitionOutBegin()
    self:sendHideEvent()
end

function PriRoomAAPayLayer:onTransitionOutFinish()
    self:removeFromParent()
end

return PriRoomCreateLayer