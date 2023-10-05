--
-- Author: zhong
-- Date: 2017-03-15 16:05:02
--
-- 非md5列表资源更新(网站广告图等)
local ResUpdateScene = class("ResUpdateScene", cc.load("mvc").ViewBase)
local QueryDialog = appdf.req("base.src.app.views.layer.other.QueryDialog")
local SpineObj = appdf.req("client.src.external.SpineObj")

-- 初始化界面
function ResUpdateScene:onCreate()
    print("ResUpdateScene:onCreate")
    local this = self
    -- 更新组
    self._groupIndex = 1
    self._downGroup = self:getApp()._updateGroup
    self._downIndex = 1
    self._downList = {}

    --背景
    local newbasepath = cc.FileUtils:getInstance():getWritablePath() .. "/baseupdate/"
    local bgfile = newbasepath .. "base/res/background.png" 
    local sp = cc.Sprite:create(bgfile)
    if nil == sp then
        sp = cc.Sprite:create("background.png")
    end
    if nil ~= sp then
        sp:setPosition(appdf.WIDTH/2,appdf.HEIGHT/2)
        self:addChild(sp)
    end

    --标签
    local tipfile = newbasepath .. "base/res/logo_name_00.png"
    if false == cc.FileUtils:getInstance():isFileExist(tipfile) then
		tipfile = "logo_name_00.png"
	end
	sp = cc.Sprite:create(tipfile)
	if nil == sp then
		sp = cc.Sprite:create("logo_name_00.png")
	end
	if nil ~= sp then
		sp:setPosition(appdf.WIDTH/2, 450)
		self:addChild(sp)
		sp:runAction(cc.RepeatForever:create(cc.Sequence:create(cc.FadeTo:create(2,255),cc.FadeTo:create(2,128))))
	end
    
	--slogan
	local sloganfile = newbasepath .. "base/res/logo_text_00.png"
	if false == cc.FileUtils:getInstance():isFileExist(sloganfile) then
		sloganfile = "logo_text_00.png"
	end
	sp = cc.Sprite:create(sloganfile)
	if nil == sp then
		sp = cc.Sprite:create("logo_text_00.png")
	end
	if nil ~= sp then
		sp = cc.Sprite:create(sloganfile)
		sp:setPosition(appdf.WIDTH/2, 300)
		self:addChild(sp)
	end

    --提示文本
    self._txtTips = cc.Label:createWithTTF("", "fonts/round_body.ttf", 24)
        :setTextColor(cc.c4b(0,250,0,255))
        :setAnchorPoint(cc.p(1,0))
        :enableOutline(cc.c4b(0,0,0,255), 1)
        :move(appdf.WIDTH,0)
        :addTo(self)

	self.m_progressLayer = display.newLayer(cc.c4b(0, 0, 0, 0))
	self:addChild(self.m_progressLayer)
	self.m_progressLayer:setVisible(false)


            local loading_bg = cc.Sprite:create("loding_bg.png"):move(appdf.WIDTH/2, 105):addTo(self.m_progressLayer)
	--总进度
	local total_bg = cc.Sprite:create("logon_newl2.png"):move(263, 21):addTo(loading_bg)
	
	self.m_totalBar = ccui.LoadingBar:create()
	self.m_totalBar:loadTexture("logon_newl1.png")	
	loading_bg:addChild(self.m_totalBar)
	self.m_totalBar:setPosition(263, 21)
	

            self.m_loadingNum = cc.LabelAtlas:create("0", "loding_num.png", 20, 20, string.byte("/"))
            self.m_loadingNum:setAnchorPoint(cc.p(0.5,0.5))
            self.m_loadingNum:setPosition(550,21)
            loading_bg:addChild(self.m_loadingNum)

	-- self.m_totalThumb = cc.Sprite:create("thumb_0.png")
	-- self.m_totalBar:addChild(self.m_totalThumb)
	-- self.m_totalThumb:setAnchorPoint(cc.p(1, 0.5))
        --    self.m_totalThumb:setPosition(0, 3)

        self.m_totalThumb = cc.ParticleSystemQuad:create("tuowei.plist")
        loading_bg:addChild(self.m_totalThumb)
        self.m_totalThumb:setAnchorPoint(cc.p(1, 0.5))
        self.m_totalThumb:setPosition(25,21)
        --self.m_totalThumb:setPosition(0, self.m_totalBar:getContentSize().height * 0.5)
        self:updateBar(self.m_totalBar, self.m_totalThumb, 0)

 --    --单文件进度
 --    local file_bg = cc.Sprite:create("wait_frame_0.png")
 --    self.m_spFileBg = file_bg
 --    self.m_progressLayer:addChild(file_bg)
 --    file_bg:setPosition(appdf.WIDTH/2, 120)
 --    self.m_fileBar = ccui.LoadingBar:create()
 --    self.m_fileBar:loadTexture("wait_frame_2.png")
 --    self.m_fileBar:setPercent(0)
 --    self.m_progressLayer:addChild(self.m_fileBar)
 --    self.m_fileBar:setPosition(appdf.WIDTH/2, 120)
 --    self._fileTips = cc.Label:createWithTTF("", "fonts/round_body.ttf", 20)
 --        --:setTextColor(cc.c4b(0,250,0,255))
 --        :setName("text_tip")
 --        :enableOutline(cc.c4b(0,0,0,255), 1)
 --        :move(self.m_fileBar:getContentSize().width * 0.5, self.m_fileBar:getContentSize().height * 0.5)
 --        :addTo(self.m_fileBar)
 -- --    self.m_fileThumb = cc.Sprite:create("thumb_0.png")
 -- --    self.m_fileBar:addChild(self.m_fileThumb)
	-- -- self.m_fileThumb:setAnchorPoint(cc.p(1, 0.5))
 -- --    self.m_fileThumb:setPosition(0, 3)

 --    self.m_fileThumb = SpineObj:create("loading.json", "loading.atlas", true)
 --    --spThumb:setPosition(yl.WIDTH/2,yl.HEIGHT/2)
 --    self.m_fileBar:addChild(self.m_fileThumb)
 --    self.m_fileThumb:setAnchorPoint(cc.p(1, 0.5))
 --    self.m_fileThumb:setPosition(0, 0)

	-- --self.m_fileThumb:setPosition(0, self.m_fileBar:getContentSize().height * 0.5)
	-- self:updateBar(self.m_fileBar, self.m_fileThumb, 0)
end

-- 进入场景而且过渡动画结束时候触发。
function ResUpdateScene:onEnterTransitionFinish()
    self._groupIndex = 1
    self:goUpdate()
end

function ResUpdateScene:downloadConfigfile( updateinfo )
    local remotefile = updateinfo.remoteconfig
    local localfile = updateinfo.localconfig
    local savepath = updateinfo.downpath
    local downurl = updateinfo.downurl
    if nil == remotefile or "" == removetefile or nil == downurl or "" == downurl then
        self:updateResult(false,"获取服务器列表失败！")
        return
    end
    --创建文件夹
    if not createDirectory(savepath) then
        self:updateResult(false,"创建文件夹失败！")
        return
    end
    --获取当前文件列表
    local szCurList = cc.FileUtils:getInstance():getStringFromFile(localfile)
    local oldlist = {}
    if szCurList ~= nil and #szCurList > 0 then
        local ok, curMD5List = pcall(function()
            return cjson.decode(szCurList)
        end)
        if ok then
            oldlist = curMD5List["listdata"]
        end
    end
    local this = self

    print("downloadConfigfile--------------------->", remotefile)
    appdf.onHttpJsionTable(remotefile,"GET","",function(jsondata,response)
        --记录新的list
        local fileUtil = cc.FileUtils:getInstance()
        this._response = response
        local updateinfo = this._downGroup[this._groupIndex]
        if nil ~= updateinfo then
            this._downGroup[this._groupIndex]["config"] = response
        end
        if jsondata then
            local newlist = jsondata["listdata"]
            if nil == newlist and nil ~= this.updateResult then
                if true == isDebug() then
                    this:updateResult(false, downurl .." 获取服务器列表失败！md5文件列表为空!")
                else
                    this:updateResult(false,"获取服务器列表失败！")
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
                        local delpath = savepath..oldpath..oldname
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
                        local delpath = savepath..newpath..newname
                        if fileUtil:isFileExist(delpath) then
                            fileUtil:removeFile(delpath)
                        end
                        
                        local url = string.format("%s%s%s", downurl, newpath, newname)
                        local savepath = savepath .. newpath
                        table.insert(this._downList, {savepath = savepath, name = newname, downloadurl = url})
                    end
                end
                
            end
            print("update_count:" .. #this._downList)
            --开始下载
            if #this._downList > 0 then
                this._retryCount = 3
                this._downIndex = 1
                this:UpdateFile()
            else
                cc.FileUtils:getInstance():writeStringToFile(this._response,localfile)
                -- 更新版本号
                if updateinfo.updateversion then
                    this:getApp()._version:setResVersion(updateinfo.remoteversion, updateinfo.updatekindid)
                end
                this:updateResult(true,"")
            end
        else
            if true == isDebug() then
                this:updateResult(false, downurl .." 获取服务器列表失败！md5文件列表为空!")
            else
                this:updateResult(false,"获取服务器列表失败！")
            end
        end
    end)
end

function ResUpdateScene:goUpdate()
    self.m_progressLayer:setVisible(true)
    self:updateGroup()
end

function ResUpdateScene:updateGroup()
    if not self._groupIndex or not self._downList then 
        self:updateResult(false,"下载信息损坏！")
        return
    end
    local updateinfo = self._downGroup[self._groupIndex]
    if not updateinfo then
        self:updateResult(false,"下载信息损坏！")
        return
    end
    print("更新组 ==> ", self._groupIndex)
    local ismd5 = updateinfo["ismd5"]
    if not ismd5 then
        self._retryCount = 3
        self._downIndex = 1
        self._downList = updateinfo["list"]
        self:UpdateFile()
    else
        -- MD5对比
        self:downloadConfigfile( updateinfo )
    end
end

function ResUpdateScene:groupDownloadFinish()
    if self._groupIndex == (#self._downGroup + 1) then
        -- 通知完成
        self:updateResult(true,"")
    else
        self:updateGroup()
    end
end

function ResUpdateScene:UpdateFile()
    if not self._downIndex or not self._downList  then 
        self:updateResult(false,"下载信息损坏！")
        return
    end 
    local this = self
    --列表完成
    if self._downIndex == (#self._downList + 1) then
        print("ResUpdateScene 更新配置信息")
        -- 写入配置
        local groupInfo = self._downGroup[self._groupIndex]
        if nil ~= groupInfo then
            local config = groupInfo["config"] or ""
            local configpath = groupInfo["localconfig"] or ""
            cc.FileUtils:getInstance():writeStringToFile(config, configpath)
            if groupInfo.updateversion then
                self:getApp()._version:setResVersion(groupInfo.remoteversion, groupInfo.updatekindid)
            end
        end

        self._groupIndex = self._groupIndex + 1
        --通知完成
        self:groupDownloadFinish()
        return 
    end
    --下载参数
    local fileinfo = self._downList[self._downIndex]
    local url 
    local dstpath

    url = fileinfo["downloadurl"]
    dstpath = fileinfo["savepath"]

    print("from ==> " .. url .. " download " .. fileinfo["name"])
    --调用C++下载
    downFileAsync(url,fileinfo["name"],dstpath,function(main,sub)
        --下载回调
        if main == appdf.DOWN_PRO_INFO then --进度信息
            this:updateProgress(sub,fileinfo["name"], (this._downIndex / (#this._downList + 1)) * 100)
        elseif main == appdf.DOWN_COMPELETED then --下载完毕
            this._retryCount = 3
            this._downIndex = this._downIndex + 1
            this:UpdateFile()
        else
            if sub == 28 and this._retryCount > 0 then
                this._retryCount = this._retryCount - 1
                this:UpdateFile()
            else
                if true == isDebug() then
                    this:updateResult(false,"文件 [" .. fileinfo["name"] .. "] 下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息
                else
                    this:updateResult(false,"下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息
                end
            end
        end
    end)
end

--更新进度
function ResUpdateScene:updateProgress(sub, msg, mainpersent)
    --self:updateBar(self.m_fileBar, self.m_fileThumb, sub)
    self:updateBar(self.m_totalBar, self.m_totalThumb, math.floor(mainpersent))
end

--更新结果
function ResUpdateScene:updateResult(result,msg)
    if nil ~= self.m_spDownloadCycle then
        self.m_spDownloadCycle:stopAllActions()
        self.m_spDownloadCycle:setVisible(false)
    end
    
    if result == true then
        self._txtTips:setString("OK")
        self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
        FriendMgr:getInstance():reSetAndLogin()
    else
        self.m_progressLayer:setVisible(false)
        --self:updateBar(self.m_fileBar, self.m_fileThumb, 0)
        self:updateBar(self.m_totalBar, self.m_totalThumb, 0)

        --重试询问
        self._txtTips:setString("")
        QueryDialog:create(msg .. "\n是否重试？",function(bReTry)
                if bReTry == true then
                    self:goUpdate()
                else
                    os.exit(0)
                end
            end)
            :setCanTouchOutside(false)
            :addTo(self)     
    end
end

function ResUpdateScene:updateBar(bar, thumb, percent)
    if nil == bar or nil == thumb then
        return
    end
    local text_tip = bar:getChildByName("text_tip")
    if nil ~= text_tip then
        local str = string.format("%d%%", percent)
        text_tip:setString(str)
    end
    self.m_loadingNum:setString(percent.."/")
    bar:setPercent(percent)
    local size = self.m_totalBar:getVirtualRendererSize()
    thumb:setPositionX(25+size.width* percent / 100)
    --thumb:setPositionX(155)
end

return ResUpdateScene