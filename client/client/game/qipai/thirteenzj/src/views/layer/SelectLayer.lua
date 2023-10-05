--
-- Author: Tang
-- Date: 2016-12-08 15:41:53
--
local SelectLayer = class("SelectLayer", function(scene,cbCardData)
     local cardLayer = display.newLayer()
  return cardLayer
end)
local module_pre = "game.qipai.thirteenzj.src"
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GameLogic = appdf.req(module_pre..".models.GameLogic")
local CardSprite = require(module_pre..".views.layer.CardSprite")
local cmd = appdf.req(module_pre..".models.cmd_game")
local Click_Area = 3
local Front = 1  --前墩
local Middle = 2 --中墩
local Tail = 3 --尾墩

local TAG = 
{
  SELECT_AREA = 0, --3
  CARD_TYPEBTN= 4

}



function SelectLayer:ctor(scene,cbCardData)
    self._scene = scene
    self._cardData = clone(cbCardData)   --扑克数据
    self._lastCard = clone(cbCardData)  --剩余手牌
    self._selectCard={} --正在选择的牌
    self._curSelectCard ={} --已经弹起选择的牌
    self._buttonIndex = 0
    self._cardList = {0,0,0,0,0,0,0,0,0,0,0,0,0} --左上角的牌
    --加载csb资源
    local csbNode = ExternalFun.loadCSB("game_res/SelectLayer.csb", self)
    csbNode:setTag(100)
    --csbNode:setPosition(yl.WIDTH/2, yl.HEIGHT/2)
    self._csbNode = csbNode
    self._cardBeganP = cc.p(165,83)
    self._cardSize =cc.size(102,144)

    self.bigParam = {}
    self.bigParam._width = self._cardSize.width
    self.bigParam._height=self._cardSize.height
    self.bigParam._file = "game_res/selectCard/Cards_big.png"


    self._curType = 1        --当前选择推荐类型
    self._lastType = 0        --上次选择的按钮
    self._cardSpace = 55          --扑克间隔
    self._specialDatas = {}     --特殊牌型组牌
    self._specialType = GameLogic.CT_EX_INVALID
    self._isSpecialType = false

    self._specialNode = nil
    self._selectedDatas = {}                  --所选扑克

    self._selectCardIndex = 0   --第一次选中扑克
    self._mobilePutCard = {}
    self._typeButton = {}
    self._cardButton = {}
    self._cancelButton = {}
    self._isFull ={false}
    self:initNode()
    
    --ExternalFun.registerNodeEvent(self)
    ExternalFun.registerTouchEvent(self, true)
    self:setTouchEnabled(true)
end

function SelectLayer:initNode(contentSize)



    if self._scene._scene._cbMaCardData > 0  then
        local handCard = CardSprite:createCard(self._scene._scene._cbMaCardData,self.bigParam)
        handCard:setScale(0.65)
        handCard:setPosition(164.5, 626.5)
        self._csbNode:addChild(handCard)
    else
        self._csbNode:getChildByName("maBg"):setVisible(false)
    end

    self:initTypeSelect()
    self:initEvent()
    self:updateHandPoker()
     
end


--更新手牌
function SelectLayer:updateHandPoker()
  self._handPoker = {}
  --print("the count is "..#self._lastCard)
  --手牌数据排序
  self:SortCardsData()
  --更新按钮显示
  self:updateControl()

  for i=1,13 do
    self:removeChildByTag(i)
  end
  self._curSelectCard = {}
  

  self._cardBeganP.x = 275 + (13-#self._lastCard)*self._cardSpace/2

  for i=1,#self._lastCard do
    --print("pppppppppppppppppppppppppppppi="..i)
    local handCard = CardSprite:createCard(self._lastCard[i],self.bigParam)
    handCard._index = i
    handCard:setTag(i)
    handCard:setMaCard("game_res/ma_image.png")
    handCard:setAnchorPoint(cc.p(0,0))
    handCard:setPosition(cc.p(self._cardBeganP.x+(handCard._index-1)*self._cardSpace,self._cardBeganP.y))
    handCard:setOrignalPosY(self._cardBeganP.y)
    self:addChild(handCard)
    --handCard:runAction(cc.MoveTo:create(0.25,cc.p(self._cardBeganP.x+(handCard._index-1)*self._cardSpace,self._cardBeganP.y)))
    table.insert(self._handPoker, handCard)
    --table.insert(self._originalCardsData,self._cardData[i])
    if self._lastCard[i] == self._scene._scene:getMaCode() and self._scene._scene.m_bHaveMaCard then 
      handCard:showMaCard(true)
    end
  end
end


--手牌数据排序 
function SelectLayer:SortCardsData()
  GameLogic:SortCardList(self._lastCard,#self._lastCard,0)
end

--初始类型选择
function SelectLayer:initTypeSelect()
  local cardData = self._scene:getParentNode()._cbautoCard

      if cardData.cbSpecialCardType ~= GameLogic.CT_EX_INVALID  then
          self._specialType = cardData.cbSpecialCardType
          local specialCard = {}
          for i=1,3 do
                for j=1,5 do
                      if i ==1 then
                          if j<=3 then
                            table.insert(specialCard,cardData.cbSpecialData[i][j])
                          end
                      else
                          table.insert(specialCard,cardData.cbSpecialData[i][j])
                      end
                end
          end
          table.insert(self._mobilePutCard,1,specialCard)

          local button = self._csbNode:getChildByName("btn_cardtype1")
            button:setVisible(true)
            button:addTouchEventListener(function(ref, eventType)
                if eventType == ccui.TouchEventType.ended then
                    self:clickRecmdType(1)
                end
            end)

            button:getChildByName("type1"):setVisible(false)
            button:getChildByName("type3"):setVisible(false)
            button:getChildByName("Sprite_2"):setVisible(true)
            button:getChildByName("type2"):setString(""..GameLogic.CT_EX_NAME[self._specialType])


      end


   -- if not cardData.cbMobilePutCount then
   --        return
   -- end
   --    for i=1,cardData.cbMobilePutCount do
        
   --      self._mobilePutCard[i]=clone(cardData.cbMobilePutCard[i])
   --    end
  
     --  if cardData.cbSpecialCardType ~= GameLogic.CT_EX_INVALID  then
     --      self._specialType = cardData.cbSpecialCardType
     --      local specialCard = {}
     --      for i=1,3 do
     --            for j=1,5 do
     --                  if i ==1 then
     --                      if j<=3 then
     --                        table.insert(specialCard,cardData.cbSpecialData[i][j])
     --                      end
     --                  else
     --                      table.insert(specialCard,cardData.cbSpecialData[i][j])
     --                  end
     --            end
     --      end

     --      table.insert(self._mobilePutCard,1,specialCard)
     --      if self._mobilePutCard[4] then
     --          table.remove(self._mobilePutCard,4)
     --      end
     --  end
     -- --self._mobilePutCard[1] ={0x09,0x2c,0x2d,0x17,0x7,0x4,0x34,0x19,0x13,0x12,0x8,0x22,0x31}

     --  for i=1, #self._mobilePutCard do
     --        local button = self._csbNode:getChildByName("btn_cardtype"..i)
     --        button:setVisible(true)
     --        button:addTouchEventListener(function(ref, eventType)
     --            if eventType == ccui.TouchEventType.ended then
     --                self:clickRecmdType(i)
     --            end
     --        end)
            
     --        dump({self._specialType,GameLogic.CT_EX_INVALID},"========================6666")
     --       if i==1 and  self._specialType ~= GameLogic.CT_EX_INVALID then

                
     --              button:getChildByName("type1"):setVisible(false)
     --              button:getChildByName("type3"):setVisible(false)
     --              button:getChildByName("Sprite_2"):setVisible(true)
     --              --print("=================11,"..self._specialType)
     --              button:getChildByName("type2"):setString(""..GameLogic.CT_EX_NAME[self._specialType])
     --       else
     --            button:getChildByName("Sprite_2"):setVisible(false)

                
     --            local front ={}
     --            local middle = {}
     --            local tail = {}
     --            for m = 1,13 do
     --                  if m<=3 then
     --                      table.insert(front,self._mobilePutCard[i][m])
     --                  elseif m <=8 then
     --                      table.insert(middle,self._mobilePutCard[i][m])
     --                  else
     --                      table.insert(tail,self._mobilePutCard[i][m])
     --                  end
     --            end
     --            local index = self._specialType ~= GameLogic.CT_EX_INVALID and i-1 or i
     --            local fronttype = cardData.cbMobilePutCardType[index][1]  --GameLogic:GetCardTypeWithKing(front, 3,self._scene._scene.m_bHaveKing,self._scene._scene.m_nGameMode==1)
                
     --            local middletype = cardData.cbMobilePutCardType[index][2]--GameLogic:GetCardTypeWithKing(middle, 5,self._scene._scene.m_bHaveKing,self._scene._scene.m_nGameMode==1)
     --            local tailtype = cardData.cbMobilePutCardType[index][3]--GameLogic:GetCardTypeWithKing(tail, 5,self._scene._scene.m_bHaveKing,self._scene._scene.m_nGameMode==1)
     --            button:getChildByName("type1"):setString(""..GameLogic.CT_NAME[fronttype])
     --            button:getChildByName("type2"):setString(""..GameLogic.CT_NAME[middletype])
     --            button:getChildByName("type3"):setString(""..GameLogic.CT_NAME[tailtype])
     --       end 
     --  end
end

function SelectLayer:clickRecmdType(index)


      for i=1,13 do

            self._cardList[i] = self._mobilePutCard[index][i]

            self._cardButton[i]:removeChildByName("cardImg")
            local card = CardSprite:createCard(self._cardList[i],self.bigParam)
            card:setPosition(cc.p(0,0))
            card:setAnchorPoint(cc.p(0,0))
            card:setName("cardImg")
            if self._cardList[i] == self._scene._scene:getMaCode() and self._scene._scene.m_bHaveMaCard then 
              card:showMaCard(true)
            end
            self._cardButton[i]:addChild(card)
            if not self._cardList[i] or self._cardList[i] == 0 then
                showToast(self,"牌数据错误",5)
            end
      end

      if index==1 and self._specialType ~= GameLogic.CT_EX_INVALID then
          self._isSpecialType = true
      else
          self._isSpecialType = false
      end

      self._lastCard = {}
      self:updateHandPoker()
      self:updateCancelButton()

end
--判断按钮状态
function SelectLayer:updateControl()


  self._sortCardsTypeResult = {}
  self._sortCardsTypeRecord = {}
  if #self._lastCard > 0 then

     self._sortCardsTypeResult,self._sortCardsTypeRecord = GameLogic:sortAllCarsType(self._lastCard,#self._lastCard)
  end
 
  self._curType = 0
  for i=1,8 do
    local btn = self._typeButton[i]
    local enable =  false
    if #self._sortCardsTypeRecord > 0 then
      enable = self._sortCardsTypeRecord[i].bTag
    end
    btn:setEnabled(enable)
  end
end

--初始化事件
function SelectLayer:initEvent()

    self.sureBtn = self._csbNode:getChildByName("select_sure")
    self.sureBtn:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            self:selectCardFinish()
          end
        end)

    
    for i=1,8 do
        self._typeButton[i] = self._csbNode:getChildByName("btn_card"..i)
        self._typeButton[i]:addTouchEventListener(function(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
              self:clickByteButton(i)
          end
        end)
    end

    for i=1,13 do
          self._cardButton[i] = self._csbNode:getChildByName("card_layout"):getChildByName("cardBg"..i)
          self._cardButton[i]:addTouchEventListener(function(sender,eventType)
          if eventType == ccui.TouchEventType.ended then
                self:clickCardButton(i)
            end
          end)
    end

    for i=1,3 do

           self._cancelButton[i] = self._csbNode:getChildByName("cancel"..i)
           self._cancelButton[i]:addTouchEventListener(function(sender,eventType)
                if eventType == ccui.TouchEventType.ended then
                     local index = {{1,3},{4,8},{9,13}}
                          self._isSpecialType = false
                          local flag = false
                          for j=index[i][1],index[i][2] do

                                if self._cardList[j] ~= 0 then

                                    table.insert(self._lastCard,self._cardList[j])

                                    self._cardButton[j]:removeChildByName("cardImg")
                                    self._cardList[j] = 0
                                    flag = true
                                end

                          end

                          if flag then
                              sender:setVisible(false)
                              self:updateCancelButton()
                              self:updateHandPoker()
                          end
                          if self._buttonIndex ~= 0 then
                            if self._buttonIndex >=index[i][1] and self._buttonIndex <=index[i][2] then
                                self._buttonIndex = 0
                            else
                                local img = self._cardButton[self._buttonIndex]:getChildByName("cardImg")
                                if img then
                                  img:showSelectEffect(false)
                                  
                                end
                                self._buttonIndex = 0
                            end
                          end
                          
                end
            end)
    end

end


function SelectLayer:updateCancelButton()


      local okFlag = true

      for i=1,13 do

            
            if self._cardList[i] ~=  0 then
                  if i<=3 then
                        self._cancelButton[1]:setVisible(true)
                  elseif i<= 8 then
                        self._cancelButton[2]:setVisible(true)
                  else
                        self._cancelButton[3]:setVisible(true)
                  end
            else
                okFlag = false
            end
      end
      self.sureBtn:setVisible(okFlag)

end

function SelectLayer:clickCardButton(tag)


      --print(tag.."=================="..self._buttonIndex)
      if self._cardList[tag] ~= 0 then
          self:popCardsByData()
          if self._buttonIndex ~= 0 and self._buttonIndex ~= tag  then --交换并检查牌型大小
              local tempList = clone(self._cardList)
              local tempCard = tempList[tag]
              tempList[tag] = tempList[self._buttonIndex]
              tempList[self._buttonIndex] = tempCard

             -- if self:isValid(tempList) then
                  self._cardList = clone(tempList)
                  self:changeCardButton(self._buttonIndex,tag)
              --else
                  --self._cardButton[self._buttonIndex]:getChildByName("cardImg"):showSelectEffect(false)
              --end
              
              self._buttonIndex = 0
          else--选中
              self._buttonIndex = tag
              if self._cardButton[tag]:getChildByName("cardImg") then
                self._cardButton[tag]:getChildByName("cardImg"):showSelectEffect(true)
              end

          end

      else 

          if self._buttonIndex ~= 0 then --点击的无牌区域且上次有点选牌蹲的牌
              if self._cardButton[self._buttonIndex]:getChildByName("cardImg") then
                self._cardButton[self._buttonIndex]:getChildByName("cardImg"):showSelectEffect(false)
              end
              self._buttonIndex = 0
          elseif #self._curSelectCard > 0 then --将牌上移到牌蹲
                
                local cardTemp = {}
                local index =0
                if tag <=3 then
                    cardTemp={self._cardList[1],self._cardList[2],self._cardList[3]}
                    index = 0
                elseif tag <=8 then
                    cardTemp={self._cardList[4],self._cardList[5],self._cardList[6],self._cardList[7],self._cardList[8]}
                    index = 3
                else
                    cardTemp={self._cardList[9],self._cardList[10],self._cardList[11],self._cardList[12],self._cardList[13]}
                    index = 8
                end

                local num = 0
                for i=1,#cardTemp do
                    if cardTemp[i] == 0 then
                        num = num+1
                    end 
                end

                if num >= #self._curSelectCard then
                    local tempList = clone(self._cardList)

                     

                    if #self._curSelectCard == 1 then

                          tempList[tag] = self._lastCard[self._curSelectCard[1]]
                    else

                          for i=1,#self._curSelectCard do
                                for j=1,#cardTemp do

                                    if tempList[index +j] == 0 then

                                        tempList[index +j] = self._lastCard[self._curSelectCard[i]]
                                        break
                                     end
                               end
                          end
                      end

                      local isAllFull = false
                      if self:isTwoFull(tempList) then
                            local templast = {}--剩余手牌索引
                            for i=1,#self._lastCard do

                                  for j=1,#self._curSelectCard do
                                      if i == self._curSelectCard[j] then
                                            break
                                      elseif j == #self._curSelectCard then
                                            table.insert(templast,i)
                                      end
                                  end

                            end

                           for i=1,#templast do
                                
                                for j=1,13 do

                                      if tempList[j] == 0 then

                                            tempList[j] = self._lastCard[templast[i]]
                                          break
                                      end
                               end
                               
                           end
                           isAllFull = true
                      end

                      --if self:isValid(tempList) then
                            self._cardList = clone(tempList)
                            for i=1,13 do
                                  if self._cardList[i] ~= 0 then
                                      self._cardButton[i]:removeChildByName("cardImg")
                                      local card = CardSprite:createCard(self._cardList[i],self.bigParam)
                                     -- card:setScale(0.88)
                                      card:setPosition(cc.p(0,0))
                                      card:setAnchorPoint(cc.p(0,0))
                                      card:setName("cardImg")
                                      if self._cardList[i] == self._scene._scene:getMaCode() and self._scene._scene.m_bHaveMaCard then 
                                        card:showMaCard(true)
                                      end
                                      self._cardButton[i]:addChild(card)
                                  end
                            end

                            --table.sort(self._curSelectCard,)
                            table.sort(self._curSelectCard,function (a,b)
                                  return a>b
                                end)

                            if isAllFull then
                                self._lastCard = {}
                            else
                                for i=1,#self._curSelectCard do

                                      table.remove(self._lastCard,self._curSelectCard[i])

                                end
                            end

                            self:updateHandPoker()

                            self:updateCancelButton()

                      --end

                end

          end

      end
end





function SelectLayer:changeCardButton(index1,index2)

    self._cardButton[index1]:removeChildByName("cardImg")
    self._cardButton[index2]:removeChildByName("cardImg")

    self._isSpecialType = false

    local card1 = CardSprite:createCard(self._cardList[index1],self.bigParam)
    --card1:setScale(0.88)
    card1:setPosition(self._cardButton[index2]:getPosition())
    self._csbNode:getChildByName("card_layout"):addChild(card1, 10,10)
    card1:runAction(cc.Sequence:create(cc.MoveTo:create(0.1,cc.p(self._cardButton[index1]:getPosition())),cc.RemoveSelf:create()))

    local card2 = CardSprite:createCard(self._cardList[index2],self.bigParam)
    --card2:setScale(0.88)
    card2:setPosition(self._cardButton[index1]:getPosition())
    self._csbNode:getChildByName("card_layout"):addChild(card2, 10,10)
    card2:runAction(cc.Sequence:create(cc.MoveTo:create(0.1,cc.p(self._cardButton[index2]:getPosition())),cc.CallFunc:create(function ()
        
        
        
        local card = CardSprite:createCard(self._cardList[index1],self.bigParam)
        --card:setScale(0.88)
        card:setPosition(cc.p(0,0))
        card:setAnchorPoint(cc.p(0,0))
        card:setName("cardImg")
        if self._cardList[index1] == self._scene._scene:getMaCode() and self._scene._scene.m_bHaveMaCard then 
              card:showMaCard(true)
        end
        self._cardButton[index1]:addChild(card)

        local card = CardSprite:createCard(self._cardList[index2],self.bigParam)
        --card:setScale(0.88)
        card:setPosition(cc.p(0,0))
        card:setAnchorPoint(cc.p(0,0))
        card:setName("cardImg")
        if self._cardList[index2] == self._scene._scene:getMaCode() and self._scene._scene.m_bHaveMaCard then 
              card:showMaCard(true)
        end
        self._cardButton[index2]:addChild(card)

    end),cc.RemoveSelf:create()))


end



function SelectLayer:isTwoFull(cardList)
  local isFull = {true,true,true}
      for i=1,13 do

            if i<=3 then
                if cardList[i] == 0 then
                    isFull[1] = false
                end
            elseif i<=8 then
                if cardList[i] == 0 then
                    isFull[2] = false
                end
            else
                if cardList[i] == 0 then
                    isFull[3] = false
                end
            end

      end
      local num= 0
      for i=1,3 do
            if isFull[i] then
                num = num+1
            end
      end

      return num>= 2 and true or false
end


--判断移动后牌型是否符合要求
function SelectLayer:isValid(cardData)
  
  local data = {}
          data[1] = {cardData[1], cardData[2], cardData[3]}
          data[2] = {cardData[4], cardData[5], cardData[6], cardData[7], cardData[8]}
          data[3] = {cardData[9], cardData[10], cardData[11], cardData[12], cardData[13]}


  for i=1, 2 do
      for j=1,#data[i] do
          if data[i][j] == 0  then
                break
          elseif j==#data[i] then
                
                      for m= i+1,3 do

                          for n=1,#data[m] do
                                if data[m][n] == 0  then
                                  break
                                elseif n == #data[m] then
                                    if i==1 then
                                      --print("ttttttttttttttttttttt1")
                                        local compare = GameLogic:CompareCard(data[1], data[m], 3, 5,false,self._scene._scene.m_tonghuaFlag)
                                          if compare == GameLogic.enCRLess then
                                                  --print("ttttttttttttttttttttt2")
                                                if m==2 then 
                                                      showToast(cc.Director:getInstance():getRunningScene(),"牌型:头墩必须小于中墩,请重新选择",1.2)
                                                else
                                                      showToast(cc.Director:getInstance():getRunningScene(),"牌型:头墩必须小于尾墩,请重新选择",1.2)
                                                end
                                                return false
                                          end
                                    else
                                          local compare = GameLogic:CompareCard(data[2],data[3],5,5,true,self._scene._scene.m_tonghuaFlag)
                                          if compare == GameLogic.enCRLess then

                                                showToast(cc.Director:getInstance():getRunningScene(),"牌型:中墩必须小于尾墩,请重新选择",1.2)

                                                return false
                                          end
                                    end
                                end
                          end

                      end
                
          end
      end
  end
--[[
  local result = false
  local compareend = GameLogic:CompareCard(middle,tail,5,5, true)
  local comparefront = GameLogic:CompareCard(front, middle, 3, 5, false)
  if compareend == GameLogic.enCRGreater and comparefront == GameLogic.enCRGreater then
    result = true
  elseif compareend ~= GameLogic.enCRGreater then
    showToast(cc.Director:getInstance():getRunningScene(),"牌型:中墩必须小于尾墩,请重新选择",1.2)
  else
    showToast(cc.Director:getInstance():getRunningScene(),"牌型:头墩必须小于中墩,请重新选择",1.2)
  end]]
  return true
end

--选牌结束
function SelectLayer:selectCardFinish()
  if not self._isSpecialType and self:isValid(self._cardList) == false then
     return 
  end

  local sureBtn = self._csbNode:getChildByName("select_sure")
  sureBtn:setEnabled(false)
  local tempCard = {}
  tempCard[1] ={self._cardList[1],self._cardList[2],self._cardList[3]}
  tempCard[2] ={self._cardList[4],self._cardList[5],self._cardList[6],self._cardList[7],self._cardList[8]}
  tempCard[3] ={self._cardList[9],self._cardList[10],self._cardList[11],self._cardList[12],self._cardList[13]}
  for i=1,3 do
      GameLogic:SortCardList(tempCard[i],#tempCard[i],0)
  end

  for i=1,13 do

        if i<=3 then
            table.insert(self._selectedDatas,tempCard[1][i])
        elseif i<=8 then
            table.insert(self._selectedDatas,tempCard[2][i-3])
        else
            table.insert(self._selectedDatas,tempCard[3][i-8])
        end

  end
  if self._isSpecialType then
      self._selectedDatas = clone(self._mobilePutCard[1])
      self._scene:finishSelect(self._selectedDatas,self._specialType)
  else
      self._scene:finishSelect(self._selectedDatas,0)
  end

  
end

--选牌时间结束
function SelectLayer:logicTimeZero()

    self:clickRecmdType(1)
    self:selectCardFinish()
end

function SelectLayer:UpdataClockTime(clockID,time)
  local timeTxt = self._csbNode:getChildByName("timeBg"):getChildByName("timeNum")
  timeTxt:setString(""..time)
end

function SelectLayer:clickByteButton(tag)
    self._buttonIndex = 0
    if tag ~= self._lastType then
      self._lastType = tag
      self._curType = 0
    end

    self._curType =  self._curType + 1
    local record = self._sortCardsTypeRecord[tag]
    if self._curType > #record.list then
      self._curType = 1
    end

    local data = record.list[self._curType]
    local shootCount = self:popCardsByData(data)
end

function SelectLayer:popCardsByData(cardData)

     for i=1,13 do
        local card = self:getChildByTag(i)
        if card then
            card:setPositionY(self._cardBeganP.y)
        end
     end

      self._curSelectCard = {}

      if cardData then
          for i=1,#cardData do
              for j=#self._lastCard, 1,-1 do

                    if cardData[i] == self._lastCard[j] then

                      
                      local card = self:getChildByTag(j)

                      if #self._curSelectCard > 0 then --处理两张花色相同的牌的情况
                          local isSame = false
                           for n=1, #self._curSelectCard do
                                if self._curSelectCard[n] ==j then
                                   isSame = true
                                   break
                                end
                           end
                           if not isSame then
                                if card then
                                    card:setPositionY(self._cardBeganP.y + 20)
                                end

                                table.insert(self._curSelectCard,j)
                                break
                           end
                      else

                            if card then
                                card:setPositionY(self._cardBeganP.y + 20)
                            end

                            table.insert(self._curSelectCard,j)
                            break
                      end

                    end
              end
          end
      end
      
end


function SelectLayer:showSelectArea()
  if #self._selectCard  >0 then
    self:cleanSelectArea()
    --dump(self._selectCard,"showSelectArea==================")
    for i=1,#self._selectCard do

      local card = self:getChildByTag(self._selectCard[i])
      card:showSelectEffect(true)
      
    end

  end
end

function SelectLayer:cleanSelectArea()
    
    for i=1,13 do
        local card = self:getChildByTag(i)
        if card then
            card:showSelectEffect(false)
        end
    end
end



function SelectLayer:getIndexByPos(point)

      local index = nil
      if point.x < self._cardBeganP.x or point.x >self._cardBeganP.x+(#self._lastCard-1)*self._cardSpace + self._cardSize.width or 
        point.y <self._cardBeganP.y or point.y > self._cardBeganP.y+self._cardSize.height then 
          return nil
      end 

      index =  math.ceil((point.x - self._cardBeganP.x)/self._cardSpace)  > #self._lastCard and #self._lastCard or math.ceil((point.x - self._cardBeganP.x)/self._cardSpace)

      return index

end

--触摸事件处理
function SelectLayer:onTouchBegan(touch, event)
  if self.sureBtn:isVisible() then
    return false
  end
 
  local pos = touch:getLocation()
  --print("touch begin1!", pos.x, pos.y)

  local posIndex = self:getIndexByPos(pos)
  self._selectCard = {}
  if posIndex then
    self.beginPoint = pos
    self.mLastIndex = posIndex
    self.isBreakTouch = false
    table.insert(self._selectCard ,posIndex)
    
    --print("hhhhhhhhhhhhhhhhhhhhhhh,"..posIndex)
    self:showSelectArea()
  else
    self.beginPoint  = nil
    for i=1,#self._curSelectCard do
        local card = self:getChildByTag(self._curSelectCard[i])
        if card then
            card:setPositionY(self._cardBeganP.y)
        end
    end
    self._curSelectCard = {}
    self.isBreakTouch = true
    return true
  end
  
  
  return true
end

function SelectLayer:onTouchMoved(touch, event)
  if self.isBreakTouch then

    return true
  end
  local pos = touch:getLocation()
  local posIndex = self:getIndexByPos(pos)
  if posIndex and self.beginPoint and math.abs(posIndex - self.mLastIndex)<=1 then
    
    self.mLastIndex = posIndex
      if self._selectCard[#self._selectCard]~=posIndex then
        local breakFlag = 0
        for k,v in pairs(self._selectCard) do--往回拖动
          if v == posIndex then
            breakFlag = k
            break
          end
        end
        if breakFlag~= 0 then
          for i= #self._selectCard,1, -1 do
            if i>breakFlag then
              table.remove(self._selectCard,i)
            end
          end
        else
          table.insert(self._selectCard ,posIndex)
          --ExternalFun.playSoundEffect("cardSeclect.mp3")
        end

        self:showSelectArea()
      end
    
  else
    self.isBreakTouch = true
    self:endSelect()
    return true
  end

  return true
end

function SelectLayer:onTouchEnded(touch, event)

    if self.isBreakTouch then
      return 
    end
    local pos = touch:getLocation()
    self:endSelect()

end



function SelectLayer:endSelect()

    local removeT = {}
    local addT= {}

    for i=1,#self._selectCard do

      --dump(self._curSelectCard,"onTouchEnded=====================1")
      if #self._curSelectCard == 0 then

        local card =  self:getChildByTag(self._selectCard[i])
          card:setPositionY(card:getPositionY() + 20)
          table.insert(addT,clone(self._selectCard[i]))
      else
        --dump(self._selectCard,"onTouchEnded=====================2")
        for j=1,#self._curSelectCard do 
          if self._selectCard[i] == self._curSelectCard[j] then
            local card =  self:getChildByTag(self._selectCard[i])
            card:setPositionY(card:getPositionY() - 20)
            table.insert(removeT,j)
            break
          elseif j==#self._curSelectCard then
            local card =  self:getChildByTag( self._selectCard[i] )
            card:setPositionY(card:getPositionY() + 20)
            table.insert(addT,clone(self._selectCard[i]))
            
          end
        end
      end
    end

    if #self._selectCard> 0 then
        ExternalFun.playSoundEffect("cardSeclect.mp3")
    end

    table.sort(removeT,function (a,b)
      return a<b
    end)

    for i=#removeT,1,-1 do

      table.remove(self._curSelectCard,removeT[i])
    end

    for i=1,#addT do
      table.insert(self._curSelectCard,addT[i])
    end

    --判断选择的牌是否符合出牌条件
    self.mSelectCardData = {}
    for i=1,#self._curSelectCard do
      table.insert(self.mSelectCardData,self._lastCard[self._curSelectCard[i]])
    end
  
    self:cleanSelectArea()
end



return SelectLayer