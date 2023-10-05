--
-- Author: zhong
-- Date: 2017-08-04 15:22:17
--
-- 多目录下载
local MultiUpdater = class("MultiUpdater")

-- url:下载list地址 
-- wirtepath:保存路径
-- curfile:当前list路径
function MultiUpdater:ctor(newfileurl,savepath,curfile,downurl)
    self._newfileurl = newfileurl
    self._savepath = savepath
    self._curfile = curfile
    self._downUrl = downurl
    self._app = nil
    self._updateGame = nil
    print("curfile:"..curfile)
end

--开始更新 
--app MyApp
--listener回调目标
--listener:updateProgress(sub,msg)
--listener:updateResult(result,msg)
function MultiUpdater:upDateClient( app, listener, gameinfo )
    --监听
    self._app = app
    self._listener = listener
    self._updateGame = gameinfo
    self._downList={}           --下载列表

    local result = false
    local msg = nil

    while(not result)
    do
        --创建文件夹
        if not createDirectory(self._savepath) then
            msg = "创建文件夹失败！"
            break
        end

        --获取当前文件列表
        local szCurList = cc.FileUtils:getInstance():getStringFromFile(self._curfile)
        local oldlist = {}
        if szCurList ~= nil and #szCurList > 0 then
            local ok, curMD5List = pcall(function()
                return cjson.decode(szCurList)
            end)
            if ok then
                oldlist = curMD5List["listdata"]
            end
        end
        appdf.onHttpJsionTable(self._newfileurl,"GET","",function(jsondata,response)
                --记录新的list
                local fileUtil = cc.FileUtils:getInstance()
                self._response = response
                if jsondata then
                    local newlist = jsondata["listdata"]
                    if nil == newlist and nil ~= self._listener and nil ~= self._listener.updateResult then
                        if true == isDebug() then
                            self._listener:updateResult(self._updateGame, false, self._newfileurl .." 获取服务器列表失败！md5文件列表为空!")
                        else
                            self._listener:updateResult(self._updateGame, false,"获取服务器列表失败！")
                        end
                        return
                    end
                    --删除判断
                    for k,v in pairs(oldlist) do
                        local oldpath = v["path"]
                        local oldname = v["name"]
                        if  oldpath then
                            local bdel = true
                            for newk,newv in pairs(newlist) do
                                if oldpath == newv["path"] and oldname == newv["name"] then
                                    bdel = false
                                    break
                                end
                            end
                            --删除文件
                            if bdel == true then
                                local delpath = self._savepath..oldpath..oldname
                                print("removefile:".. delpath)
                                fileUtil:removeFile(delpath)
                            end
                        end
                    end
                    --下载判断
                    for k ,v in pairs(newlist) do
                        local newpath = v["path"]
                        if newpath then
                            local needupdate = true
                            local newname = v["name"]
                            local newmd5 = v["md5"]
                            for oldk,oldv in pairs(oldlist) do
                                local oldpath = oldv["path"]
                                local oldname = oldv["name"]
                                local oldmd5 = oldv["md5"]
                                
                                if oldpath == newpath and newname == oldname then 
                                    if newmd5 == oldmd5 then
                                        needupdate = false
                                    end
                                    break
                                end
                            end
                            --保存到下载列队
                            if needupdate == true then
                                print(newname.."-needupdate")
                                -- 先移除
                                local delpath = self._savepath..newpath..newname
                                if fileUtil:isFileExist(delpath) then
                                    fileUtil:removeFile(delpath)
                                end
                                
                                table.insert(self._downList , {newpath,newname})
                            end
                        end
                        
                    end
                    print("update_count:"..#self._downList)
                    --开始下载
                    if #self._downList > 0 then
                        self._retryCount = 3
                        self._downIndex = 1
                        self:UpdateFile()
                    else
                        cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
                        if nil ~= self._listener and nil ~= self._listener.updateResult then
                            self._listener:updateResult(self._updateGame, true,"")
                        elseif nil ~= self._app and nil ~= self._updateGame then
                            local updateKind = self._updateGame._KindID
                            for k,v in pairs(self._app._gameList) do
                                if v._KindID == updateKind then
                                    self._app:getVersionMgr():setResVersion(v._ServerResVersion, v._KindID)
                                    v._Active = true
                                    break
                                end
                            end
                            -- 清理下载管理
                            GlobalUserItem.tabUpdater[updateKind] = nil
                        end
                    end
                else
                    if nil ~= self._listener and nil ~= self._listener.updateResult then
                        if true == isDebug() then
                            self._listener:updateResult(self._updateGame, false, self._newfileurl .." 更新列表为空!")
                        else
                            self._listener:updateResult(self._updateGame, false,"获取服务器列表失败！")
                        end
                    end
                end
            end)

        result = true
    end
    if not result and nil ~= self._listener and nil ~= self._listener.updateResult then
        self._listener:updateResult(self._updateGame, false, msg)
    end
end


--开始更新 
--app MyApp
--listener回调目标
--listener:updateProgress(sub,msg)
--listener:updateResult(result,msg)
function MultiUpdater:upDownClient( app, listener, gameinfo )
    --监听
    self._app = app
    self._listener = listener
    self._updateGame = gameinfo
    self._downList={}           --下载列表

    downFileAsync(self._downUrl, self._curfile, self._savepath, function(main, sub)
            --下载回调
            if main == appdf.DOWN_PRO_INFO then --进度信息
                print("更新进度 "..self._updateGame._Module..",%"..sub)
                if nil ~= self._listener and nil ~= self._listener.updateResult then
                    self._listener:updateProgress(self._updateGame, sub,self._downUrl, sub)
                end
            elseif main == appdf.DOWN_COMPELETED then --下载完毕
                
                GlobalUserItem.tabUpdater[self._updateGame._KindID] = nil

                local zipfile = self._savepath .. self._curfile

                --解压
                unZipAsync(zipfile, self._newfileurl, function(result)
                        --删除压缩文件
                        os.remove(zipfile)
                        print("解压完毕 "..zipfile)
                        local updateKind = self._updateGame._KindID
                            for k,v in pairs(self._app._gameList) do
                                if v._KindID == updateKind then
                                    self._app:getVersionMgr():setResVersion(v._ServerResVersion, v._KindID)
                                    v._Active = true
                                    break
                                end
                            end
                            -- 清理下载管理
                            GlobalUserItem.tabUpdater[updateKind] = nil

                        if nil ~= self._listener and nil ~= self._listener.updateResult then
                            self._listener:updateResult(self._updateGame, true,"")
                        end
                end)
            else
                self._listener:updateResult(self._updateGame, false,"下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息

                showToast(nil, "下载失败，错误码：" .. main .. ", " .. sub, 2)
            end
    end)

end


--下载
function MultiUpdater:UpdateFile()
    if not self._downIndex or not self._downList and ( nil ~= self._listener and nil ~= self._listener.updateResult )  then 
        self._listener:updateResult(self._updateGame, false,"下载信息损坏！")
        return
    end 
    --列表完成
    if self._downIndex == (#self._downList + 1) then
        --更新本地MD5
        cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
        --通知完成
        if nil ~= self._listener and nil ~= self._listener.updateResult then
            self._listener:updateResult(self._updateGame, true,"")
        elseif nil ~= self._app and nil ~= self._updateGame then
            local updateKind = self._updateGame._KindID
            for k,v in pairs(self._app._gameList) do
                if v._KindID == updateKind then
                    self._app:getVersionMgr():setResVersion(v._ServerResVersion, v._KindID)
                    v._Active = true
                    print("MultiUpdater update over ", v._GameName)
                    break
                end
            end
            -- 清理下载管理
            GlobalUserItem.tabUpdater[updateKind] = nil
        end
        return 
    end
    --下载参数
    local fileinfo = self._downList[self._downIndex]
    local url 
    local dstpath

    url = self._downUrl..fileinfo[1]..fileinfo[2]
    dstpath = self._savepath..fileinfo[1]

    --print("from ==> " .. url .. " download " .. fileinfo[2])
    --调用C++下载
    downFileAsync(url,fileinfo[2],dstpath,function(main,sub)
        --下载回调
        if main == appdf.DOWN_PRO_INFO then --进度信息
            if nil ~= self._listener and nil ~= self._listener.updateProgress then
                self._listener:updateProgress(self._updateGame, sub,fileinfo[2], (self._downIndex / (#self._downList + 1)) * 100)
            end
        elseif main == appdf.DOWN_COMPELETED then --下载完毕
            self._retryCount = 3
            self._downIndex = self._downIndex +1
            self:UpdateFile()
        else
            if sub == 28 and self._retryCount > 0 then
                self._retryCount = self._retryCount - 1
                self:UpdateFile()
            else
                if nil ~= self._listener and nil ~= self._listener.updateResult then
                    if true == isDebug() then
                        self._listener:updateResult(self._updateGame, false,"文件 [" .. fileinfo[2] .. "] 下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息
                    else
                        self._listener:updateResult(self._updateGame, false,"下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息
                    end
                end
            end
        end
    end)
end

return MultiUpdater