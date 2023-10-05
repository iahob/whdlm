//
//  LogAsset.h
//  GloryProject
//
//  Created by zhong on 16/8/19.
//
//

#ifndef ry_LogAsset_h
#define ry_LogAsset_h

#include <stdio.h>
#include "cocos2d.h"

int register_all_logasset();

// curl 通信方式
enum enCURL_METHOD
{
    kCURL_GET_METHOD,
    kCURL_POST_METHOD,
};

// 日志数据
struct tagLogData
{
    std::string strData = "";
    bool bExtraData = false;
};

// 分析数据
struct tagAnalysisData
{
    enCURL_METHOD enMethod;
    std::string szUrl;
    std::string szParam;
};

class LogAsset
{
private:
    LogAsset();
    ~LogAsset();
    
public:
    static LogAsset* getInstance();
    
    static void destroy();
    
    void init(const std::string &logPath, const bool &bEnable, const bool &bPrettyWrite = false);
    
    void logData(const std::string &str, const bool &bExtraData = false);
    
    void analysisDataByGet( const std::string &url );
    
    void analysisDataByPost( const std::string &url, const std::string &param );
private:
    /*
     * @brief 日志
     */
    void addToQueue(tagLogData* pData);
    
    void logRun();
    
    void backupFile(const std::string &filepath);
    
    /*
     * @brief 统计
     */
    void addToAnalysisQueue( tagAnalysisData* pData );
    
    void analysisRun();
private:
    std::string m_strFilePath;
    bool m_bEnable;
    bool m_bPrettyWrite;
    
    // 日志队列
    std::mutex m_logMutex;
    std::vector<tagLogData*> *m_vecQueue;
    
    // 统计队列
    std::mutex m_analysisMutex;
    std::vector<tagAnalysisData*> *m_vecAnalysisQueue;
};
#endif /* ry_LogAsset_h */
