local HelpLayer = class("HelpLayer", cc.Layer)

function HelpLayer:ctor( )
    local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
    local csbNode = ExternalFun.loadCSB("LayerHelp.csb", self)
    self.csbNode = csbNode
    cc.funAddChildListener(csbNode, "Sprite_1/Button_1", function() self:onClickClose() end)
    cc.funAddChildListener(csbNode, "Panel_1", function() self:onClickClose() end)
    self:refresh(self.rule)
end

function HelpLayer:refresh(txt)
    local lab = cc.find(self.csbNode, "ScrollView_1/Text_1")
    lab:setString(txt)
    local sv = cc.find(self.csbNode, "ScrollView_1")
    local sizeLab = lab:getContentSize()
    local sizeInner = sv:getInnerContainerSize()
    lab:setPositionY(math.max(sizeInner.height, sizeLab.height))
    sv:setInnerContainerSize(cc.size(sizeInner.width, math.max(sizeInner.height, sizeLab.height)))
end

function HelpLayer:onClickClose()
    if self.csbNode and self.csbNode:getParent() then
        self.csbNode:removeFromParent()
    end
end

HelpLayer.rule = [[
牌型
一副牌54张
1、大王、小王可以当任点数。
2、无牛：没有任意三张牌能加起来成为10的倍数。
3、有牛：从牛1到牛9，任意三张牌相加是10的倍数，剩余2张牌相加不是10的倍数，
相加后取个位数，个位数是几（1-9），就是牛几（1-9）。
4、牛牛：任意三张牌相加是10的倍数，剩余2张牌相加也是10的倍数。
5、炸弹：即5张牌中有4张一样的牌，此时无需有牛。
6、五花牛：五花牛的花指的是手上的5张牌全为JQK的特殊牛牛牌型。
7、银牛：即五张牌中4张牌是10以上的特殊牛牛牌型

牌型比较
1、单张大小：从大到小为大王、小王、K、Q、J、10、9、8、7、6、5、4、3、2、
A。
2、花色大小：从大到小为黑、红、梅、方。
3、牌型大小：从大到小为炸弹、五花牛、银牛、牛牛、有牛、无牛。
4、庄闲同牌型时，挑出最大的一张牌进行大小比较，看两方最大牌一样，那么花色
进行比较。（特例：四炸时比较四张一样的那张牌）型时，挑出最大的一张

基本规则
1、分庄家和闲家进行游戏。每个闲家分别和庄家比牌。
2、玩家可以主动上庄，成为本回合的庄家。每回合结束后可重新上庄。
3、玩家可以在多个闲家位置上任意下注。

赔率
1.炸弹—10倍
2.五花牛—10倍
3.银牛—10倍
4.牛牛——10倍
5.牛9——9倍
6.牛8——8倍
7.牛7——7倍
8.牛6——6倍
9.牛5——5倍
10.牛4——4倍
11.牛3——3倍
12.牛2——2倍
13.牛1——1倍
14.无牛——1倍
]]

return HelpLayer