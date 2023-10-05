--
-- Author: zhong
-- Date: 2017-06-08 11:18:40
--
-- 定位标识
local UIPageViewIndicator = class("UIPageViewIndicator", cc.Node)
function UIPageViewIndicator:ctor(normalfile, selectfile)
    self.m_szNoramlFile = normalfile
    self.m_szSelectFile = selectfile

    -- 间隔
    self.m_fSpace = 10
    -- 标记符
    self.m_tabIndexNode = {}
end

function UIPageViewIndicator:refreshIndicator( size )
    if #self.m_tabIndexNode < size then
        repeat
            local indexnode = cc.Sprite:create(self.m_szNoramlFile)
            self:addChild(indexnode)
            table.insert(self.m_tabIndexNode, indexnode)
        until #self.m_tabIndexNode == size
    end
    if #self.m_tabIndexNode > size then
        repeat
            self.m_tabIndexNode[1]:removeFromParent()
            table.remove(self.m_tabIndexNode, 1)
        until #self.m_tabIndexNode == size
    end
    self:rearrange()
end

function UIPageViewIndicator:indicate( idx )
    if idx < 0 or idx > #self.m_tabIndexNode then
        return
    end
    for k,v in pairs(self.m_tabIndexNode) do
        if k == idx then
            v:setSpriteFrame(cc.Sprite:create(self.m_szSelectFile):getSpriteFrame())
        else
            v:setSpriteFrame(cc.Sprite:create(self.m_szNoramlFile):getSpriteFrame())
        end
    end
end

function UIPageViewIndicator:rearrange()
    local total = #self.m_tabIndexNode
    if total == 0 then
        return
    end
    
    local idxsize = self.m_tabIndexNode[1]:getContentSize()
    local totalsize = total * idxsize.width + self.m_fSpace * (total - 1)
    local posValue = -( totalsize * 0.5) + idxsize.width * 0.5
    for k,v in pairs(self.m_tabIndexNode) do
        -- toto 未区分方向
        local pos = cc.p(posValue, idxsize.height * 0.5)
        v:setPosition(pos)
        posValue = posValue + idxsize.width + self.m_fSpace
    end
end

return UIPageViewIndicator