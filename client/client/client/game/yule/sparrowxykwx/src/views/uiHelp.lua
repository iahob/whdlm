local uiHelp = env.uiBase:ctor{
    zorder = 1000,
}

local str_test =
[[
一、  基础规则
1.  2倍起胡、可以点炮胡、自摸、抢杠胡；
2.  只能碰或杠不能吃；
3.  分全频道和半频道；
4.  其他玩家亮倒后，自己非亮倒情况下不能打出别人胡牌；
二、  计分规则
1.  放冲炮只有放炮的玩家扣分，如果玩家自摸，则其他两家玩家均扣分
2.  放杠时只有放杠的玩家扣分，当时计分
3.  放杠仅放杠玩家出，暗杠、蓄杠其他两家玩家均扣分，当时计分
4.  杠上杠: 如果A杠后打出的牌被别人杠或者自己继续杠，那么第二杠要
翻倍，如果杠完之后又被人杠或者自己杠，就再翻一倍，以此类推。
5.  如果荒庄，首亮的玩家需赔其他人1分，荒庄不荒杠，杠牌分数正常结
算；
三、  番型
1.  屁胡×1番即为普通胡牌牌型(此胡牌型不为同色牌)
2.  碰碰胡×2番由四个刻子(杠)和一对组成的胡牌牌
3.  清一色×4番由同一门花色牌组成的胡牌牌型
4.  七对×4番手中有七个对子的胡牌牌型，碰出的牌不算
5.  豪华七对×8番有四个相同的牌当做两个对子使用
6.  超豪华七对×32番有两个四个相同的牌当做四个对子
7.  超超豪华七对×128番有三组四个相同的牌当做四个对子使用
8.  卡5星×2番，卡5饼、5条胡牌
9.  小三元×4番和牌时，如果牌面上有中、发、白中的任意2种(每种牌3张，
    碰杠都算)，并且牌面中还有其余1对
10. 大三元×8番和牌时，如果牌面上有中、发、白每种三张(碰杠都算)称之
为大三元
11. 明四归×2番(全频道)碰牌后胡牌时，手上有第4张牌。
12. 暗四归×4番(全频道)胡牌时手上有4张未杠的相同牌。
13. 手抓一×4番胡牌时自己手上只有一张牌
14. 杠上炮×2番杠后打出的牌被别人胡，杠钱照算
15. 杠开×2番，杠来自己要胡的牌胡牌，杠牌分数另算
16. 明牌×2番，即听牌后，可以随时选择把手中的牌摊开示众（让其他二位
玩家都看到手中的牌)来打，(如果胡牌或自己放冲或者别的玩家自摸都按双
番分数结算，如果明牌后，当局没有玩家胡牌,则明牌的玩家付给其它二个
玩家各底注的分数)
17. 抢杠×2番胡别人自抓开明杠的牌，按照抢杠人胡牌的牌型X2付分
18. 蓄杠×1番(其它二个玩家均扣分)
19. 放杠×2番(放杠的玩家扣分)
20. 暗杠×2番(其它二个玩家均扣分)
21. 海底炮×2番(全频道)，每局最后一张牌打出去后其他玩家可以胡牌，屁
胡也可以胡
22. 海底捞×2番(全频道)，每局最后一张牌自摸胡牌
]]


function uiHelp:ctor(...)
    self:init(...)
    self:registerNodeEvent()
    return self
end

function uiHelp:init(parent)
    local csbNode = cc.CSLoader:createNode("HelpLayer.csb")
    parent:addChild(csbNode, self.zorder, 1)
    self.widget = csbNode

    cc.funAddChildListener(csbNode, "Button_quit", function() self:onClickQuit() end)
    self:refresh(str_test)
end

function uiHelp:refresh(str)
    local text = cc.find(self.widget, "ScrollView_1/Text_1")
    text:setString(str)
    local scrollview = cc.find(self.widget, "ScrollView_1")
    local size = scrollview:getInnerContainerSize()
    local txtSize = text:getContentSize()
    scrollview:setInnerContainerSize(cc.size(size.width, txtSize.height))
    text:setPosition(0, txtSize.height)
end

function uiHelp:onClickQuit()
    self:destroy()
end

function uiHelp:onExit()
end

return uiHelp