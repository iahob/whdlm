local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = require(appdf.EXTERNAL_SRC .. "PopupInfoHead")

local ClubDeskItem = class("ClubDeskItem", cc.Node)

ClubDeskItem.BT_MASK = 1

--构造
function ClubDeskItem:ctor(parent, roomInfo, idx)
    -- dump(roomInfo,"-----roomInfo-------",6)
    local csb = ExternalFun.loadCSB("club/game/game_145/club_Desk_Game.csb", self);
    -- csb:setPosition(0, 0)
    self._csbNode = csb;

    self.parent = parent;
    local node_desk_info = csb:getChildByName("node_desk_info");
    local title_name = node_desk_info:getChildByName("title_name");
    local player_num = node_desk_info:getChildByName("player_num");
    local banker_num = node_desk_info:getChildByName("banker_num");
    local min_bet_num = node_desk_info:getChildByName("min_bet_num");


    title_name:setString("玩法"..((idx*1 +1)>=10 and (idx*1 +1) or "0"..(idx*1 +1)));
    player_num:setString(roomInfo.wJoinGamePeopleCount);
    if roomInfo.wCurSitPlayer then
        player_num:setString(roomInfo.wCurSitPlayer .. "/" .. roomInfo.wJoinGamePeopleCount);
    end
    local bankerLimit = ExternalFun.bytesToInt(roomInfo.cbGameRule[1], 10, 17);
    banker_num:setString(bankerLimit);
    min_bet_num:setString(roomInfo.lCellScore);

    local node_ludan = csb:getChildByName("node_ludan");
    local iocn_ludan_1 = node_ludan:getChildByName("iocn_ludan_1");

    if roomInfo.roomType == 2 then
        --dump(self.m_sGameRoomList,"==")
        for k,v in pairs(self.parent.m_sGameConfigList) do
            if v and v.dwConfigID == roomInfo.dwConfigID then
                title_name:setString("玩法"..(k>= 10 and k or "0"..k))
            end
        end
    end

    local playeCount = roomInfo.wJoinGamePeopleCount

    for j=1,10 do
        local head = csb:getChildByName("head_"..j);
        head:setVisible(true);
    end

    if roomInfo.roomType == 2 then 
        -- dump(roomInfo.dwPlayerUserId[1],"===================")
        local index = 0;
        local bankerIndex = 0;
        for j=1,10 do --最大16个椅子
            if roomInfo.dwPlayerUserId[1][j] == roomInfo.dwBankerUserID then
                index = 1;
                bankerIndex = j;
            else
                index = j + 1;
                if index > 10 and bankerIndex > 0 then
                    index = bankerIndex;
                end
            end
            local head = csb:getChildByName("head_"..index)
            if roomInfo.dwPlayerUserId[1][j] ~= 0 and j<= 10 and head then
                local node =head:getChildByName("head")
                head:getChildByName("Sprite_4"):setVisible(false)
                local iteminfo = {}
                iteminfo.dwUserID = roomInfo.dwPlayerUserId[1][j]

                local data, sign = GlobalUserItem:useridSign(iteminfo.dwUserID)
                local action = string.format("action=GetUserInfo&userid=%s&time=%s&sign=%s", iteminfo.dwUserID, data, sign)
                print("url =====", yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx?"..action)
                local jsonStr = cjson.encode(action)
                LogAsset:getInstance():logData(jsonStr, true)
                appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/NewMoblieInterface.ashx","GET",action,function(jstable,jsdata)
                   -- dump(jstable, "获取玩家信息", 7)
                    if type(jstable) == "table" then
                        local data = jstable["data"]
                        local msg = jstable["msg"]
                        if type(data) == "table" then
                            local valid = data["valid"]
                            if valid then
                                iteminfo.szSign = data["UnderWrite"]
                                iteminfo.cbGender = tonumber(data["cbGender"])
                                iteminfo.UserID = data["UserID"]
                                iteminfo.szNickName = data["NickName"]
                                iteminfo.dwCustomID = data["CustomID"]
                                iteminfo.dwGameID = data["GameID"]

                                if node and tolua.cast(node,"cc.Node")then
                                    node:removeChildByName("_head_")
                                    local head = PopupInfoHead:createClipHead(iteminfo, 70,"club/Sprite_head1.png")
                                    head:enableInfoPop(false)
                                    -- head:setScale(0.51)
                                    node:addChild(head)
                                    head:setPosition(0,0)
                                    head:setName("_head_")
                                end
                            end
                        end
                    end
                end)
            else
                --if node and tolua.cast(node,"cc.Node")then
                    --node:setVisible(false)
                --end
            end
        end
    end

end

return ClubDeskItem