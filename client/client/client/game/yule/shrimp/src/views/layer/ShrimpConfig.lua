

local ShrimpConfig = {}

ShrimpConfig.FISH_SHRIMP  			= 0				-- 鱼虾，
ShrimpConfig.FISH_CRAB  			= 1				-- 鱼蟹，
ShrimpConfig.FISH_LION_OR_CHICKEN  	= 2				-- 鱼狮子（鸡），
ShrimpConfig.FISH_TIGER  			= 3				-- 鱼老虎，
ShrimpConfig.FISH_GOURD  			= 4				-- 鱼葫芦，
ShrimpConfig.SHRIMP_CRAB  			= 5				-- 虾蟹，
ShrimpConfig.SHRIMP_LION_OR_CHICKEN = 6				-- 虾狮子（鸡），
ShrimpConfig.SHRIMP_TIGER  			= 7				-- 虾老虎，
ShrimpConfig.SHRIMP_GOURD  			= 8				-- 虾葫芦，
ShrimpConfig.CRAB_LION_OR_CHICKEN  	= 9				-- 蟹狮子（鸡），
ShrimpConfig.CRAB_TIGER  			= 10			-- 蟹老虎，
ShrimpConfig.CRAB_GOURD  			= 11			-- 蟹葫芦，
ShrimpConfig.LION_OR_CHICKEN_TIGER  = 12			-- 狮子（鸡）老虎，
ShrimpConfig.LION_OR_CHICKEN_GOURD  = 13			-- 狮子（鸡）葫芦，
ShrimpConfig.TIGER_GOURD  			= 14			-- 老虎 葫芦，
ShrimpConfig.ERROR					= 15			-- 错误

ShrimpConfig.ID_AR_FISH				= 0				-- 鱼
ShrimpConfig.ID_AR_XIA				= 1				-- 虾
ShrimpConfig.ID_AR_XIE			    = 2				-- 蟹
ShrimpConfig.ID_AR_LION				= 3				-- 狮（鸡）
ShrimpConfig.ID_AR_TIGER			= 4				-- 老虎
ShrimpConfig.ID_AR_HULU		        = 5				-- 葫芦

-- 押注区域,如果是串玩法则对应0~14个串（鱼虾，鱼蟹，鱼狮子（鸡），鱼老虎，鱼葫芦，
-- 虾蟹，虾狮子（鸡），虾老虎，虾葫芦，

function ShrimpConfig.getChuanIndex(id1 , id2 )
	-- body
	if id1 == 0  then -- 鱼XX
		return ShrimpConfig.getFishXX(id2)
	elseif id1 == 1 then -- 虾XX
		return ShrimpConfig.getShrimpXX(id2)
	elseif id1 == 2 then
		return ShrimpConfig.getCrabXX(id2)
	elseif id1 == 3 then
		return ShrimpConfig.getLionRoChickenXX(id2)
	elseif id1 == 4 then
		return ShrimpConfig.getTigerXX(id2)
	elseif id1 == 5 then
		return ShrimpConfig.getGourdXX(id2)
	end
end

-- 鱼XX
function ShrimpConfig.getFishXX(id)
	if id == 0 then 
		return ShrimpConfig.ERROR
	elseif id == 1 then --鱼虾
		return ShrimpConfig.FISH_SHRIMP
	elseif id == 2 then --鱼蟹
		return ShrimpConfig.FISH_CRAB
	elseif id == 3 then --鱼狮子（鸡）
		return ShrimpConfig.FISH_LION_OR_CHICKEN
	elseif id == 4 then --鱼老虎
		return ShrimpConfig.FISH_TIGER
	elseif id == 5 then --鱼葫芦
		return ShrimpConfig.FISH_GOURD
	end
	return ShrimpConfig.ERROR
end

--虾XX
function ShrimpConfig.getShrimpXX(id)
	if id == 0 then --虾鱼
		return ShrimpConfig.FISH_SHRIMP
	elseif id == 1 then --虾虾
		return ShrimpConfig.ERROR 
	elseif id == 2 then --虾蟹
		return ShrimpConfig.SHRIMP_CRAB
	elseif id == 3 then --虾狮子（鸡）
		return ShrimpConfig.SHRIMP_LION_OR_CHICKEN
	elseif id == 4 then --虾老虎
		return ShrimpConfig.SHRIMP_TIGER
	elseif id == 5 then --虾葫芦
		return ShrimpConfig.SHRIMP_GOURD
	end
	return ShrimpConfig.ERROR
end

--蟹XX
function ShrimpConfig.getCrabXX(id)
	if id == 0 then 	--蟹鱼
		return ShrimpConfig.FISH_CRAB
	elseif id == 1 then --蟹虾
		return ShrimpConfig.SHRIMP_CRAB 
	elseif id == 2 then --蟹蟹
		return ShrimpConfig.ERROR
	elseif id == 3 then --蟹狮子（鸡）
		return ShrimpConfig.CRAB_LION_OR_CHICKEN
	elseif id == 4 then --蟹老虎
		return ShrimpConfig.CRAB_TIGER
	elseif id == 5 then --蟹葫芦
		return ShrimpConfig.CRAB_GOURD
	end
	return ShrimpConfig.ERROR
end

--狮子（鸡）XX
function ShrimpConfig.getLionRoChickenXX(id)
	if id == 0 then 	--狮子（鸡）鱼
		return ShrimpConfig.FISH_LION_OR_CHICKEN
	elseif id == 1 then --狮子（鸡）虾
		return ShrimpConfig.SHRIMP_LION_OR_CHICKEN
	elseif id == 2 then --狮子（鸡）蟹
		return ShrimpConfig.CRAB_LION_OR_CHICKEN
	elseif id == 3 then --狮子（鸡）狮子（鸡）
		return ShrimpConfig.ERROR
	elseif id == 4 then --狮子（鸡）老虎
		return ShrimpConfig.LION_OR_CHICKEN_TIGER
	elseif id == 5 then --狮子（鸡）葫芦
		return ShrimpConfig.LION_OR_CHICKEN_GOURD
	end
	return ShrimpConfig.ERROR
end

--老虎XX
function ShrimpConfig.getTigerXX(id)
	if id == 0 then 	--老虎鱼
		return ShrimpConfig.FISH_TIGER
	elseif id == 1 then --老虎虾
		return ShrimpConfig.SHRIMP_TIGER
	elseif id == 2 then --老虎蟹
		return ShrimpConfig.CRAB_TIGER
	elseif id == 3 then --老虎狮子（鸡）
		return ShrimpConfig.LION_OR_CHICKEN_TIGER
	elseif id == 4 then --老虎老虎
		return ShrimpConfig.ERROR
	elseif id == 5 then --老虎葫芦
		return ShrimpConfig.TIGER_GOURD
	end
	return ShrimpConfig.ERROR
end

--葫芦XX
function ShrimpConfig.getGourdXX(id)
	if id == 0 then 	--葫芦鱼
		return ShrimpConfig.FISH_GOURD
	elseif id == 1 then --葫芦虾
		return ShrimpConfig.SHRIMP_GOURD
	elseif id == 2 then --葫芦蟹
		return ShrimpConfig.CRAB_GOURD
	elseif id == 3 then --葫芦狮子（鸡）
		return ShrimpConfig.LION_OR_CHICKEN_GOURD
	elseif id == 4 then --葫芦老虎
		return ShrimpConfig.TIGER_GOURD
	elseif id == 5 then --葫芦葫芦
		return ShrimpConfig.ERROR
	end
	return ShrimpConfig.ERROR
end


function ShrimpConfig.geiIndexForChuan(chuan)

	if ShrimpConfig.FISH_SHRIMP == chuan then 
		-- 鱼虾，
		return ShrimpConfig.ID_AR_FISH, ShrimpConfig.ID_AR_XIA
	elseif ShrimpConfig.FISH_CRAB == chuan then	
		-- 鱼蟹，
		return ShrimpConfig.ID_AR_FISH, ShrimpConfig.ID_AR_XIE
	elseif ShrimpConfig.FISH_LION_OR_CHICKEN == chuan then	
		-- 鱼狮子（鸡），
		return ShrimpConfig.ID_AR_FISH, ShrimpConfig.ID_AR_LION
	elseif ShrimpConfig.FISH_TIGER == chuan then 
		-- 鱼老虎，
		return ShrimpConfig.ID_AR_FISH, ShrimpConfig.ID_AR_TIGER
	elseif ShrimpConfig.FISH_GOURD == chuan then 
		-- 鱼葫芦，
		return ShrimpConfig.ID_AR_FISH, ShrimpConfig.ID_AR_HULU
	elseif ShrimpConfig.SHRIMP_CRAB == chuan then 
		-- 虾蟹，
		return ShrimpConfig.ID_AR_XIA, ShrimpConfig.ID_AR_XIE
	elseif ShrimpConfig.SHRIMP_LION_OR_CHICKEN == chuan then 
		-- 虾狮子（鸡），
		return ShrimpConfig.ID_AR_XIA, ShrimpConfig.ID_AR_LION
	elseif ShrimpConfig.SHRIMP_TIGER == chuan then 
		-- 虾老虎，
		return ShrimpConfig.ID_AR_XIA, ShrimpConfig.ID_AR_TIGER
	elseif ShrimpConfig.SHRIMP_GOURD == chuan then 
		-- 虾葫芦，
		return ShrimpConfig.ID_AR_XIA, ShrimpConfig.ID_AR_HULU
	elseif ShrimpConfig.CRAB_LION_OR_CHICKEN == chuan then 
		-- 蟹狮子（鸡），
		return ShrimpConfig.ID_AR_XIE, ShrimpConfig.ID_AR_LION
	elseif ShrimpConfig.CRAB_TIGER == chuan then 
		-- 蟹老虎，
		return ShrimpConfig.ID_AR_XIE, ShrimpConfig.ID_AR_TIGER
	elseif ShrimpConfig.CRAB_GOURD == chuan then 
		-- 蟹葫芦，
		return ShrimpConfig.ID_AR_XIE, ShrimpConfig.ID_AR_HULU
	elseif ShrimpConfig.LION_OR_CHICKEN_TIGER == chuan then 
		-- 狮子（鸡）老虎，
		return ShrimpConfig.ID_AR_LION, ShrimpConfig.ID_AR_TIGER
	elseif ShrimpConfig.LION_OR_CHICKEN_GOURD == chuan then 
		-- 狮子（鸡）葫芦，
		return ShrimpConfig.ID_AR_LION, ShrimpConfig.ID_AR_HULU
	elseif ShrimpConfig.TIGER_GOURD == chuan then 
		-- 老虎 葫芦，
		return ShrimpConfig.ID_AR_TIGER, ShrimpConfig.ID_AR_HULU
	elseif ShrimpConfig.ERROR == chuan then 
		-- 错误
		return nil , nil
	end
	return nil , nil
end


return ShrimpConfig