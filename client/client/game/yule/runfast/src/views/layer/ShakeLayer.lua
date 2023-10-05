local ScreenShaker = class("ScreenShaker")

function ScreenShaker:ctor(target, time)
    self.init_x, self.init_y = 0, 0       --初始位置
    self.diff_x, self.diff_y = 0, 0       --偏移量
    self.diff_max = 8                     --最大偏移量
    self.interval = 0.01                  --震动频率
    self.totalTime = 0                    --震动时间
    self.time = 0                         --计时器

    self.target = target
    self.init_x, self.init_y = target:getPosition()
    self.totalTime = time
end

function ScreenShaker:shakeRun()
    self._timeShd = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function(ft)
                                                                                    self:shake(ft)
                                                                                end,
                                                                                self.interval, false)
end

function ScreenShaker:shake(ft)
    if self.time >= self.totalTime then
        self:stop()
        return
    end

    self.time = self.time + ft
    self.diff_x = math.random(-self.diff_max, self.diff_max)*math.random()
    self.diff_y = math.random(-self.diff_max, self.diff_max)*math.random()

    if not self.target then
        if self._timeShd then
            cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeShd)
        end
        return 
    end

    self.target:setPosition(cc.p(self.init_x + self.diff_x, self.init_y+self.diff_y))
end

function ScreenShaker:stop(ft)
    self.time = 0
    cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._timeShd)
    self.target:setPosition(cc.p(self.init_x, self.init_y))
    self._timeShd = nil
end

return ScreenShaker