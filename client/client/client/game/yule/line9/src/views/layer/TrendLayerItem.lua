local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local TrendLayerItem = class("TrendLayerItem", cc.TableViewCell)

--构造
function TrendLayerItem:ctor()
    --加载csb资源
    self._csbNode = ExternalFun.loadCSB("TrendLayer/TrendLayerItem.csb", self)
end

return TrendLayerItem