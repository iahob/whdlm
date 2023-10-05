local scheduler = appdf.req(appdf.EXTERNAL_SRC .. "scheduler")
local SchedulerPool = class("SchedulerPool")

function SchedulerPool:ctor()
    self.pool_ = {}
    self.id_ = 0
end

function SchedulerPool:clearAll()
    for k, v in pairs(self.pool_) do
        scheduler.unscheduleGlobal(v)
    end
    self.pool_ = {}
end

function SchedulerPool:clear(id)
    if id and self.pool_[id] then
        scheduler.unscheduleGlobal(self.pool_[id])
        self.pool_[id] = nil
    end
end

function SchedulerPool:delayCall(callback, delay, ...)
    self.id_ = self.id_ + 1
    local id = self.id_
    local args = {...}
    local handle = scheduler.performWithDelayGlobal(function()
        self.pool_[id] = nil
        if callback then
            callback(self, unpack(args))
        end
    end, delay)
    self.pool_[id] = handle
    return id
end

function SchedulerPool:loopCall(callback, interval, ...)
    self.id_ = self.id_ + 1
    local id = self.id_
    local args = {...}
    local handle = scheduler.scheduleGlobal(function()
        if callback then
            if not callback(self, id, unpack(args)) then
                if(self.pool_[id])then
                    scheduler.unscheduleGlobal(self.pool_[id])
                    self.pool_[id] = nil
                end
            end
        end
    end, interval)
    self.pool_[id] = handle
    return id
end


return SchedulerPool