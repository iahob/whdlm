--
-- Author: zhong
-- Date: 2017-05-09 09:06:00
--
local video_struct = {}

-- 时间定义
video_struct.SYSTEMTIME = 
{
    {t = "word", k = "wYear"},
    {t = "word", k = "wMonth"},
    {t = "word", k = "wDayOfWeek"},
    {t = "word", k = "wDay"},
    {t = "word", k = "wHour"},
    {t = "word", k = "wMinute"},
    {t = "word", k = "wSecond"},
    {t = "word", k = "wMilliseconds"},
}

return video_struct