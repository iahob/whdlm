local GameLogic = GameLogic or {}

--转动定义
GameLogic.TURN_WIDTH_COUNT	=		3									--转动宽数量
GameLogic.TURN_HEIGHT_COUNT	=		3									--转动高数量

--纹理元素
GameLogic.TextureElement_A = 		0
GameLogic.TextureElement_B = 		1
GameLogic.TextureElement_C = 		2
GameLogic.TextureElement_D = 		3
GameLogic.TextureElement_E = 		4
GameLogic.TextureElement_F = 		5
GameLogic.TextureElement_G = 		6
GameLogic.TextureElement_H = 		7
GameLogic.TextureElement_I = 		8
GameLogic.TextureElement_Deform = 	9
GameLogic.TextureElement_BCD = 		10
GameLogic.TextureElement_Null = 	11
GameLogic.TextureElement_Any = 		12
GameLogic.TextureElement_Max = 		13


--元素定义
GameLogic.Element_A =				0
GameLogic.Element_B =				1
GameLogic.Element_C =				2
GameLogic.Element_D =				3
GameLogic.Element_E =				4
GameLogic.Element_F =				5
GameLogic.Element_G =				6
GameLogic.Element_H =				7
GameLogic.Element_I =				8
GameLogic.Element_Normal_Max =		9									-- 最大正常索引		
GameLogic.Element_Deform =			9									-- 变形元素
GameLogic.Element_Max =				10									-- 最大元素索引			
GameLogic.Element_Invalid =			-1									-- 无效元素	


--3连倍数
GameLogic.MultipleThree_AAA = 			0
GameLogic.MultipleThree_BBB =			1
GameLogic.MultipleThree_CCC =			2
GameLogic.MultipleThree_DDD =			3
GameLogic.MultipleThree_EEE =			4
GameLogic.MultipleThree_FFF =			5
GameLogic.MultipleThree_GGG =			6
GameLogic.MultipleThree_BCD =			7
GameLogic.MultipleThree_HHH =			8
GameLogic.MultipleThree_III =			9
GameLogic.MultipleThree_II =			10
GameLogic.MultipleThree_I =				11
GameLogic.MultipleThree_Max =			12									-- 3连倍数
GameLogic.MultipleThree_Invalid =		-1				-- 无效倍数	


--9连倍数

GameLogic.MultipleNine_A =				0
GameLogic.MultipleNine_B =				1
GameLogic.MultipleNine_C =				2
GameLogic.MultipleNine_D =				3
GameLogic.MultipleNine_E =				4
GameLogic.MultipleNine_F =				5
GameLogic.MultipleNine_G =				6
GameLogic.MultipleNine_H =				7
GameLogic.MultipleNine_I =				8
GameLogic.MultipleNine_Max =			9					-- 9连倍数
GameLogic.MultipleNine_Invalid =		-1					-- 无效倍数	

----------------------------------------------------------
--判断9连
function GameLogic:IsNineLine( nElementIndex )
	--获取第一个
	local nElementValue = nElementIndex[0][0]
	--无效元素
	if nElementValue < 0 or nElementValue > Element_Normal_Max then
		return false
	end
	--遍历元素
	for nWidthIndex=1,3 do
		for nHeightIndex=1,3 do
			if nElementIndex[nWidthIndex][nHeightIndex] ~= nElementValue then
				return false
			end
		end
	end
	return true
end

--3连值
function GameLogic:ThreeLineValue( nOne,nTwo,Three )
	--print("3连值 1", nOne,nTwo,Three )
	--位置2癞子变换
	if  nTwo == GameLogic.Element_Deform and nOne ~= GameLogic.Element_Deform then
		nTwo = nOne
	end
	--位置3癞子变换
	if  Three == GameLogic.Element_Deform and nTwo ~= GameLogic.Element_Deform  then
		Three = nTwo
	elseif Three == GameLogic.Element_Deform and nOne ~= GameLogic.Element_Deform then
		Three = nOne
	end
	--print("3连值 2", nOne,nTwo,Three )
	--MultipleThree_AAA 
	if nOne == GameLogic.Element_A and nTwo == GameLogic.Element_A and Three == GameLogic.Element_A then
		return GameLogic.MultipleThree_AAA
	end
	--MultipleThree_BBB 
	if nOne == GameLogic.Element_B and nTwo == GameLogic.Element_B and Three == GameLogic.Element_B then
		return GameLogic.MultipleThree_BBB;
	end
	--MultipleThree_CCC 
	if nOne == GameLogic.Element_C and nTwo == GameLogic.Element_C and Three == GameLogic.Element_C then
		return GameLogic.MultipleThree_CCC
	end
	--MultipleThree_DDD 
	if nOne == GameLogic.Element_D and nTwo == GameLogic.Element_D and Three == GameLogic.Element_D then
		return GameLogic.MultipleThree_DDD;
	end
	--MultipleThree_EEE 
	if nOne == GameLogic.Element_E and nTwo == GameLogic.Element_Eand and Three == GameLogic.Element_E then
		return GameLogic.MultipleThree_EEE;
	end
	--MultipleThree_FFF 
	if nOne == GameLogic.Element_F and nTwo == GameLogic.Element_F and Three == GameLogic.Element_F then
		return GameLogic.MultipleThree_FFF;
	end
	--MultipleThree_GGG 
	if nOne == GameLogic.Element_G and nTwo == GameLogic.Element_G and Three == GameLogic.Element_G then
		return GameLogic.MultipleThree_GGG;
	end
	--MultipleThree_BCD 
	if( ( nOne == GameLogic.Element_B or nOne == GameLogic.Element_C or nOne == GameLogic.Element_D )
		and ( nTwo == GameLogic.Element_B or nTwo == GameLogic.Element_C or nTwo == GameLogic.Element_D ) 
		and ( Three == GameLogic.Element_B or Three == GameLogic.Element_C or Three == GameLogic.Element_D ) ) then
		return GameLogic.MultipleThree_BCD;
	end
	--MultipleThree_HHH 
	if( nOne == GameLogic.Element_H and nTwo == GameLogic.Element_H and Three == GameLogic.Element_H ) then
		return GameLogic.MultipleThree_HHH;
	end
	-- MultipleThree_III 
	if( nOne == GameLogic.Element_I and nTwo == GameLogic.Element_I and Three == GameLogic.Element_I ) then
		return GameLogic.MultipleThree_III;
	end
	--MultipleThree_II 
	if( nOne == GameLogic.Element_I and nTwo == GameLogic.Element_I ) then
		return GameLogic.MultipleThree_II;
	end
	--MultipleThree_I 
	if( nOne == GameLogic.Element_I ) then
		return GameLogic.MultipleThree_I;
	end
	--无效倍数
	return GameLogic.MultipleThree_Invalid;

end

--横一3连
function GameLogic:IsThreeLineWidth1( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_WIDTH_1_1],nElementIndex[LINE_WIDTH_1_2], nElementIndex[LINE_WIDTH_1_3])
end

--横二3连
function GameLogic:IsThreeLineWidth2( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_WIDTH_2_1],nElementIndex[LINE_WIDTH_2_2], nElementIndex[LINE_WIDTH_2_3])
end

--横三3连
function GameLogic:IsThreeLineWidth3( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_WIDTH_3_1],nElementIndex[LINE_WIDTH_3_2], nElementIndex[LINE_WIDTH_3_3])
end

--竖四3连
function GameLogic:IsThreeLineWidth4( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_HEIGHT_4_1],nElementIndex[LINE_HEIGHT_4_2], nElementIndex[LINE_HEIGHT_4_3])
end

--竖五3连
function GameLogic:IsThreeLineWidth5( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_HEIGHT_5_1],nElementIndex[LINE_HEIGHT_5_2], nElementIndex[LINE_HEIGHT_5_3])
end

--竖六3连
function GameLogic:ISThreeLineHeight6( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_HEIGHT_6_1],nElementIndex[LINE_HEIGHT_6_2], nElementIndex[LINE_HEIGHT_6_3])
end

--斜七3连
function GameLogic:ISThreeLineOblique7( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_OBLIQUE_7_1],nElementIndex[LINE_OBLIQUE_7_2], nElementIndex[LINE_OBLIQUE_7_3])
end

--斜八3连
function GameLogic:ISThreeLineOblique8( nElementIndex )
	return ThreeLineValue(nElementIndex[LINE_OBLIQUE_8_1],nElementIndex[LINE_OBLIQUE_8_2], nElementIndex[LINE_OBLIQUE_8_3])
end

--癞子数量
function GameLogic:DeformCount( nElementIndex )
	--癞子数量
	local nDeformCount = 0;

   	--遍历元素
   	for nWidthIndex=1,GameLogic.TURN_WIDTH_COUNT do
   		for nHeightIndex=1,GameLogic.TURN_HEIGHT_COUNT do
            if nElementIndex[nWidthIndex][nHeightIndex] ~= GameLogic.Element_Deform then
            
                nDeformCount = nDeformCount + 1
            end
   		end
   	end
    return nDeformCount;
end

--3连癞子
function GameLogic:ThreeLineDeform( nLineElement ) 

    -- 临时保存
    local nTempOne = nLineElement[1];
    local nTempTwo = nLineElement[2];
    local nTempThree = nLineElement[3];
    --print("3连癞子 1",nDeformOne, nDeformTwo,  nDeformThree )
    --位置2癞子变换
    if  nLineElement[2] == GameLogic.Element_Deform and nLineElement[1] ~= GameLogic.Element_Deform then
    	nLineElement[2] = nLineElement[1]
	end
    
    --位置3癞子变换
    if  nLineElement[3] == GameLogic.Element_Deform and nLineElement[2] ~= GameLogic.Element_Deform then
        nLineElement[3] = nLineElement[2];
    elseif  nLineElement[3] == GameLogic.Element_Deform and nLineElement[1] ~= GameLogic.Element_Deform then
        nLineElement[3] = nLineElement[1];
    end
    --print("3连癞子 2",nDeformOne, nDeformTwo,  nDeformThree )
    -- print("nTempOne ~= nDeformOne",nTempOne ~= nDeformOne)
    -- print("nTempTwo ~= nDeformTwo",nTempTwo ~= nDeformTwo)
    -- print("nTempThree ~= nDeformThree",nTempThree ~= nDeformThree)
    --print("GameLogic:ThreeLineValue(nDeformOne, nDeformTwo, nDeformThree)",GameLogic:ThreeLineValue(nDeformOne, nDeformTwo, nDeformThree))
    -- print("GameLogic.MultipleThree_Invalid",GameLogic.MultipleThree_Invalid)
    local result = (nTempOne ~= nLineElement[1] or nTempTwo ~= nLineElement[2] or nTempThree ~= nLineElement[3]) and (GameLogic:ThreeLineValue(nLineElement[1], nLineElement[2], nLineElement[3]) ~= GameLogic.MultipleThree_Invalid);
    local tempResult = GameLogic:ThreeLineValue(nLineElement[1], nLineElement[2], nLineElement[3])
    -- print("tempResult",tempResult)
    -- print("结果,",result)
    return result
end


-----------------------------------------------------------------------------------

--拷贝表
function GameLogic:copyTab(st)
    local tab = {}
    for k, v in pairs(st) do
        if type(v) ~= "table" then
            tab[k] = v
        else
            tab[k] = self:copyTab(v)
        end
    end
    return tab
 end


return GameLogic