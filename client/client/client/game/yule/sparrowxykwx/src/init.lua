--游戏的一个内部环境
_G.env = _G.env or
{
    GAME_PATH = "sparrowxykwx",
}

local srcpath = string.format("%syule.%s.src.", appdf.GAME_SRC, env.GAME_PATH)
env.srcpath = srcpath

require(srcpath .. "utils.functions")
require(srcpath .. "utils.scheduler")
require(srcpath .. "utils.SchedulerPool")
env.uiBase = require(srcpath .. "views.uiBase")

function env.destroy()
    env.uiBase = nil
    env.GAME_PATH = nil
end