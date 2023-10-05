//
//  ThirdParty.cpp
//  GloryProject
//
//  Created by zhong on 16/9/8.
//
//

#import "ThirdParty.h"

#import "AppController.h"
#import "RootViewController.h"

//友盟
#import "UMSocialWechatHandler.h"

//支付宝
//#import "DataSigner.h"
#import <AlipaySDK/AlipaySDK.h>
//#import "DataVerifier.h"
//#import "Order.h"
#import "APOrderInfo.h"
#import "APRSASigner.h"

//bugly
#import "BuglyAgent.h"
#import <Bugly/Bugly.h>

//竣付通
#import "JftPaySDK/jft_MD5.h"

#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

// 日志模块
#import "LogAsset.h"

#import <AssetsLibrary/AssetsLibrary.h>
@implementation ThirdParty
static ThirdParty* s_instance = nil;
+ (ThirdParty*) getInstance
{
    if (nil == s_instance)
    {
        s_instance = [ThirdParty alloc];
        [s_instance defaultInit];
    }
    return s_instance;
}

+ (void) destroy
{
    if (nil != s_instance)
    {
        /*[s_instance->_platformArray release];
        [s_instance->_umDict release];
        [s_instance->_umSharePlat release];
        [s_instance->_share release];
        [s_instance->_wechat release];
        [s_instance->_alipay release];
        [s_instance->_jftpay release];*/
        s_instance->_completionBlock = nil;
        
        /*[s_instance release];*/
    }
}
@synthesize shareDelegate = _shareDelegate;

#pragma mark UMSocial
-(void)didCloseUIViewController:(UMSViewControllerType)fromViewControllerType
{
    
}

-(void)didFinishGetUMSocialDataInViewController:(UMSocialResponseEntity *)response
{
    if (nil == _shareDelegate)
    {
        return;
    }
    
    if (response.responseCode == UMSResponseCodeCancel)
    {
        [_shareDelegate onCancel:INVALIDPLAT];
    }
    else if (response.responseCode == UMSResponseCodeSuccess)
    {
        [_shareDelegate onComplete:INVALIDPLAT backCode:response.responseCode backMsg:response.message];
    }
    else
    {
        [_shareDelegate onError:INVALIDPLAT backMsg:response.message];
    }
    [self setShareDelegate:nil];
}
#pragma mark -

#pragma mark -
#pragma mark WXApiDelegate

-(void) onReq:(BaseReq*)req
{
    
}
-(void) onResp:(BaseResp*)resp
{
    if([resp isKindOfClass:[PayResp class]])
    {
        //支付返回结果，实际支付结果需要去微信服务器端查询
        //NSString *strMsg,*strTitle = [NSString stringWithFormat:@"充值结果"];
        
        switch (resp.errCode)
        {
            case WXSuccess:
                NSLog(@"充值成功");
                [self onPayResult:TRUE msg:@""];
                break;
                
            default:
                NSLog(@"错误，retcode = %d, retstr = %@", resp.errCode,resp.errStr);
                std::string msg = cocos2d::StringUtils::format("wx 充值错误 retcode = %d, retstr = %s ", resp.errCode,[resp.errStr UTF8String]);
                // 写入日志
                LogAsset::getInstance()->logData(msg);
                [self onPayResult:FALSE msg:resp.errStr];
                break;
        }
    }
}
#pragma mark -

#pragma mark -
#pragma mark JftSDKPayDelegate
//获取支付列表成功
- (void)getPayTypeListSuccess:(NSArray *)list
{
    rapidjson::Document doc;
    doc.SetArray();
    rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();
    for (NSDictionary *dic : list)
    {
        NSString *typeID = [dic objectForKey:@"typeid"];
        doc.PushBack(rapidjson::Value([typeID UTF8String], alloc), alloc);
    }
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    if (nil != _payDelegate)
    {
        [_payDelegate onGetPayList:TRUE backMsg:[NSString stringWithCString:buffer.GetString() encoding:NSUTF8StringEncoding]];
    }
}

//获取支付列表失败
- (void)getPayTypeListFailure:(NSString *)message
{
    if (nil != _payDelegate)
    {
        [_payDelegate onGetPayList:FALSE backMsg:@"获取支付列表失败"];
    }
    [self setPayDelegate:nil];
}

//支付成功
- (void)jftPaySuccess
{
    //充值成功
    [self onPayResult:TRUE msg:@""];
}

//支付失败
- (void)jftPayFailure:(NSString *)message
{
    //充值失败
    [self onPayResult:FALSE msg:@""];
}

//支付结果
- (void)jftPayResult:(NSString *)result
{
    NSLog(@"pay result:%@",result);
}

- (void)openAppSuccessed{
    NSLog(@"竣付通 openAppSuccessed");
}

- (void)openAppFailer:(NSString *)failer{
    if (nil != _payDelegate)
    {
        [_payDelegate onPayNotify:@"" backMsg:failer];
    }
}
#pragma mark -

- (void) defaultInit
{
    //bugly
    [BuglyAgent class];
    [Bugly sdkVersion];
    
    _shareDelegate = nil;
    _payDelegate = nil;
    _payPlat = INVALIDPLAT;
    
    //配置第三方平台
    _platformArray = [NSMutableArray arrayWithCapacity:20];
    [_platformArray insertObject:WECHAT atIndex:0];
    [_platformArray insertObject:WECHAT_CIRCLE atIndex:1];
    [_platformArray insertObject:ALIPAY atIndex:2];
    [_platformArray insertObject:JFT atIndex:3];
    [_platformArray insertObject:AMAP atIndex:4];
    [_platformArray insertObject:IOSIAP atIndex:5];
    [_platformArray insertObject:SMS atIndex:6];
    [_platformArray retain];
    
    //配置友盟平台
    _umDict = [NSDictionary dictionaryWithObjectsAndKeys:
               UMShareToWechatSession, WECHAT,
               UMShareToWechatTimeline, WECHAT_CIRCLE,
               UMShareToAlipaySession, ALIPAY,
               UMShareToSms, SMS,
               nil];
    [_umDict retain];
    
    //友盟分享平台
    _umSharePlat = [NSArray arrayWithObjects:UMShareToWechatSession, UMShareToWechatTimeline, nil];
    [_umSharePlat retain];
    
    _share = [ShareConfig alloc];
    [_share retain];
    _wechat = [WeChatConfig alloc];
    [_wechat retain];
    _alipay = [AliPayConfig alloc];
    [_alipay retain];
    _jftpay = [JftPayConfig alloc];
    [_jftpay retain];
    
    _locationDelegate = nil;
    m_bConfigAmap = FALSE;
    _completionBlock = nil;
}

- (NSString*) getPlatform:(int)nidx
{
    if(nidx > [_platformArray count])
    {
        return INVALIDPLAT;
    }
    return [_platformArray objectAtIndex:nidx];
}

- (ShareConfig*) getDefaultShareConfig
{
    return _share;
}

- (BOOL) handleOpenURL:(NSURL *)url
{
    BOOL bRes = [UMSocialSnsService handleOpenURL:url];
    if (FALSE == bRes)
    {
        NSString *pstr = [[NSString alloc] initWithFormat:@"%@://pay", _wechat.WeChatAppID];
        if ([[url absoluteString] rangeOfString:pstr].location == 0)
        {
            bRes = [WXApi handleOpenURL:url delegate:self];
        }
    }
    // 处理支付宝
    if ([url.host isEqualToString:@"safepay"])
    {
        //跳转支付宝钱包进行支付，处理支付结果
        [[AlipaySDK defaultService] processOrderWithPaymentResult:url standbyCallback:^(NSDictionary *resultDic)
         {
             NSLog(@"reslut = %@",resultDic);
             int resultvalue = [[resultDic valueForKey:@"resultStatus"] intValue];
             if (resultvalue == 9000)
             {
                 //充值成功
                 [self onPayResult:TRUE msg:@""];
             }
             else
             {
                 //充值失败
                 [self onPayResult:FALSE msg:@""];
                 // 写入日志
                 LogAsset::getInstance()->logData(cocos2d::StringUtils::format("Alipay 充值失败 错误代码 %d", resultvalue));
             }
         }];
        bRes = YES;
    }
    return bRes;
}

- (BOOL) openURL:(UIApplication *)application nsurl:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    // 微信
    // 支付为 wxappid://pay, 登陆为 wxappid://oauth, 分享为 wxappid://platformid=wechat
    
    // 支付宝
    // 支付为 scheme://safepay
    BOOL bRes = FALSE;
    NSString *pstr = [[NSString alloc] initWithFormat:@"%@://pay", _wechat.WeChatAppID];
    if ([[url absoluteString] rangeOfString:pstr].location == 0)
    {
        bRes = [WXApi handleOpenURL:url delegate:self];
    }
    /*else 暂时屏蔽竣付通 2017-10-30, 处理bRes结果冲突 oauth
    {
        bRes = [JfPay getApplication:application sourceApplication:sourceApplication openURL:url annotation:annotation];
    }*/
    
    // 处理支付宝
    if (FALSE == bRes && [url.host isEqualToString:@"safepay"])
    {
        //跳转支付宝钱包进行支付，处理支付结果
        [[AlipaySDK defaultService] processOrderWithPaymentResult:url standbyCallback:^(NSDictionary *resultDic)
        {
            NSLog(@"reslut = %@",resultDic);
            int resultvalue = [[resultDic valueForKey:@"resultStatus"] intValue];
            if (resultvalue == 9000)
            {
                //充值成功
                [self onPayResult:TRUE msg:@""];
            }
            else
            {
                //充值失败
                [self onPayResult:FALSE msg:@""];
                // 写入日志
                LogAsset::getInstance()->logData(cocos2d::StringUtils::format("Alipay 充值失败 错误代码 %d", resultvalue));
            }
        }];
        bRes = YES;
    }
    
    if (FALSE == bRes)
        bRes = [UMSocialSnsService handleOpenURL:url];
    return bRes;
}

- (void) didFinishLaunching:(UIApplication *)application options:(NSDictionary *)launchOptions;
{
    // 竣付通配置
    [JfPay application:application lanchOption:launchOptions];
    
    // 推送配置
    [UMessage startWithAppkey:UMENG_KEY launchOptions:launchOptions httpsEnable:YES];
    [UMessage registerForRemoteNotifications];
#if (COCOS2D_DEBUG > 0) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
    [UMessage setLogEnabled:YES];
#endif
    //iOS10必须加下面这段代码。
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    center.delegate=self;
    UNAuthorizationOptions types10 = UNAuthorizationOptionBadge | UNAuthorizationOptionAlert | UNAuthorizationOptionSound;
    [center requestAuthorizationWithOptions:types10
                          completionHandler:^(BOOL granted, NSError * _Nullable error) {
                              if (granted)
                              {
                                  //点击允许
                                  //这里可以添加一些自己的逻辑
                              }
                              else
                              {
                                  //点击不允许
                                  //这里可以添加一些自己的逻辑
                              }
                          }];
}

- (void) willEnterForegound:(UIApplication *)application
{
    [JfPay applicationWillEnterForeground:application];
}

- (void) configThirdParty:(NSString*)plat platConfig:(NSDictionary*)config
{
    BOOL bRes = FALSE;
    if ([plat isEqualToString:WECHAT])
    {
        [_wechat setWeChatAppID:[config objectForKey:@"AppID"]];
        bRes = (_wechat.WeChatAppID != nil);
        [_wechat setWeChatAppSecret:[config objectForKey:@"AppSecret"]];
        bRes = (_wechat.WeChatAppSecret != nil);
        [_wechat setWeChatPartnerID:[config objectForKey:@"PartnerID"]];
        bRes = (_wechat.WeChatPartnerID != nil);
        [_wechat setWeChatPayKey:[config objectForKey:@"PayKey"]];
        bRes = (_wechat.WeChatPayKey != nil);
        [_wechat setWeChatURL:[config objectForKey:@"URL"]];
        bRes = (_wechat.WeChatURL != nil);
        [_wechat setConfiged:bRes];
        
        if (TRUE == _wechat.Configed)
        {
            [UMSocialWechatHandler setWXAppId:_wechat.WeChatAppID appSecret:_wechat.WeChatAppSecret url:_wechat.WeChatURL];
        }
    }
    else if ([plat isEqualToString:ALIPAY])
    {
        [_alipay setAlipayAppID:[config objectForKey:@"AppID"]];
        bRes = (_alipay.AlipayAppID != nil);
        [_alipay setAlipayPartnerID:[config objectForKey:@"PartnerID"]];
        bRes = (_alipay.AlipayPartnerID != nil);
        [_alipay setAlipaySeller:[config objectForKey:@"SellerID"]];
        bRes = (_alipay.AlipaySeller != nil);
        [_alipay setAlipayPrivate:[config objectForKey:@"RsaKey"]];
        bRes = (_alipay.AlipayPrivate != nil);
        [_alipay setAlipayNotifyUrl:[config objectForKey:@"NotifyURL"]];
        bRes = (_alipay.AlipayNotifyUrl != nil);
        [_alipay setAliPaySchemes:[config objectForKey:@"AliSchemes"]];
        bRes = (_alipay.AliPaySchemes != nil);
        [_alipay setConfiged:bRes];
    }
    else if ([plat isEqualToString:JFT])
    {
        [_jftpay setJftPayKey:[config objectForKey:@"PayKey"]];
        bRes = (_jftpay.JftPayKey != nil);
        [_jftpay setJftPayPartnerID:[config objectForKey:@"PartnerID"]];
        bRes = (_jftpay.JftPayPartnerID != nil);
        [_jftpay setJftAppID:[config objectForKey:@"JftAppID"]];
        bRes = (_jftpay.JftAppID != nil);
        [_jftpay setJftPayAesKey:[config objectForKey:@"JftAesKey"]];
        bRes = (_jftpay.JftPayAesKey != nil);
        [_jftpay setJftPayAesVec:[config objectForKey:@"JftAesVec"]];
        bRes = (_jftpay.JftPayAesVec != nil);
        [_jftpay setConfiged:bRes];
        
        [JfPay setLogEnable:YES];
    }
    else if ([plat isEqualToString:AMAP])
    {
        NSString *k = [config objectForKey:@"AmapKeyIOS"];
        if (nil != k)
        {
            m_bConfigAmap = TRUE;
            [[AMapServices sharedServices] setEnableHTTPS:TRUE];
            [AMapServices sharedServices].apiKey = k;
            
            self.locationMgr = [[AMapLocationManager alloc] init];
            [self.locationMgr setDelegate:self];
            //设置期望定位精度
            [self.locationMgr setDesiredAccuracy:kCLLocationAccuracyHundredMeters];
            //设置不允许系统暂停定位
            //[self.locationMgr setPausesLocationUpdatesAutomatically:NO];
            //设置允许在后台定位
            //[self.locationMgr setAllowsBackgroundLocationUpdates:YES];
            //设置定位超时时间
            [self.locationMgr setLocationTimeout:6];
            //设置逆地理超时时间
            [self.locationMgr setReGeocodeTimeout:3];
            
            // 定位回调
            self.completionBlock = ^(CLLocation *location, AMapLocationReGeocode *regeocode, NSError *error)
            {
                rapidjson::Document doc;
                doc.SetObject();
                rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();
                rapidjson::Value("",alloc);
                
                if (error)
                {
                    doc.AddMember("berror", rapidjson::Value(true), alloc);
                    doc.AddMember("msg", rapidjson::Value([error.localizedDescription UTF8String], alloc), alloc);
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    doc.Accept(writer);
                    
                    NSLog(@"locError:{%ld - %@};", (long)error.code, error.localizedDescription);
                    // 写入日志
                    LogAsset::getInstance()->logData(cocos2d::StringUtils::format("定位失败 错误代码 %ld, 描述 %s", (long)error.code, [error.localizedDescription UTF8String]));
                    //如果为定位失败的error，则不进行annotation的添加
                    if (error.code == AMapLocationErrorLocateFailed)
                    {
                        if (nil != _locationDelegate)
                        {
                            // [_locationDelegate onLocationResult:FALSE backCode:(int)error.code backMsg:error.localizedDescription];
                            [_locationDelegate onLocationResult:TRUE backCode:(int)error.code backMsg:[NSString stringWithCString:buffer.GetString() encoding:NSUTF8StringEncoding]];
                        }
                        return;
                    }
                }
                
                //得到定位信息，添加annotation
                if (location)
                {
                    MAPointAnnotation *annotation = [[MAPointAnnotation alloc] init];
                    [annotation setCoordinate:location.coordinate];
                    
                    if (regeocode)
                    {
                        [annotation setTitle:[NSString stringWithFormat:@"%@", regeocode.formattedAddress]];
                        [annotation setSubtitle:[NSString stringWithFormat:@"%@-%@-%.2fm", regeocode.citycode, regeocode.adcode, location.horizontalAccuracy]];
                        doc.AddMember("address", rapidjson::Value([regeocode.formattedAddress UTF8String], alloc), alloc);
                    }
                    else
                    {
                        [annotation setTitle:[NSString stringWithFormat:@"lat:%f;lon:%f;", location.coordinate.latitude, location.coordinate.longitude]];
                        [annotation setSubtitle:[NSString stringWithFormat:@"accuracy:%.2fm", location.horizontalAccuracy]];
                    }
                    if (nil != _locationDelegate)
                    {
                        doc.AddMember("latitude", rapidjson::Value(location.coordinate.latitude), alloc);
                        doc.AddMember("longitude", rapidjson::Value(location.coordinate.longitude), alloc);
                        doc.AddMember("accuracy", rapidjson::Value(location.horizontalAccuracy), alloc);
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        doc.Accept(writer);
                        
                        [_locationDelegate onLocationResult:TRUE backCode:(int)error.code backMsg:[NSString stringWithCString:buffer.GetString() encoding:NSUTF8StringEncoding]];
                    }
                }
            };
        }
    }
}

- (void) configSocialShare:(NSDictionary *)config
{
    BOOL bConfiged = FALSE;
    [_share setShareTitle:[config objectForKey:@"title"]];
    bConfiged = (_share.ShareTitle != nil);
    [_share setShareContent:[config objectForKey:@"content"]];
    bConfiged = (_share.ShareContent != nil);
    [_share setShareUrl:[config objectForKey:@"url"]];
    bConfiged = (_share.ShareUrl != nil);
    [_share setConfiged:bConfiged];
    
    if (_share.Configed)
    {
        [UMSocialData setAppKey:UMENG_KEY];
        [UMSocialConfig setSupportedInterfaceOrientations:UIInterfaceOrientationMaskLandscape];
    }
}

- (void) thirdPartyLogin:(NSString*)plat delegate:(id<LoginDelegate>) delegate
{
    //判断友盟平台
    NSString *platstr = [_umDict objectForKey:plat];
    if (nil != platstr)
    {
        //判断是否配置
        if (FALSE == _wechat.Configed || FALSE == _share.Configed)
        {
            [delegate onLoginFail:platstr backMsg:@"did not config platform"];
            return;
        }
        
        //判断是否安装微信
        if (FALSE == [WXApi isWXAppInstalled])
        {
            [delegate onLoginFail:platstr backMsg:@"微信客户端未安装,无法进行授权登陆!"];
            return;
        }
        
        //是否授权(微信暂时需要重新授权)
        if ([UMSocialAccountManager isOauthAndTokenNotExpired:platstr])
        {
            [self getAuthorizedUserInfo:platstr delegate:delegate];
            return;
        }
        
        UMSocialSnsPlatform *snsplat = nil;
        //微信登陆
        if ([WECHAT isEqualToString:plat])
        {
            snsplat = [UMSocialSnsPlatformManager getSocialPlatformWithName:platstr];
        }
        
        //账户信息
        if (nil != snsplat)
        {
            AppController *pApp = (AppController*)[[UIApplication sharedApplication] delegate];
            [UMSocialConfig setSupportedInterfaceOrientations:UIInterfaceOrientationMaskAll];
            snsplat.loginClickHandler(pApp->viewController, [UMSocialControllerService defaultControllerService], YES, ^(UMSocialResponseEntity *response)
                                      {
                                          [self parseAuthorizeData:platstr response:response delegate:delegate];
                                      });
        }
    }
    else
    {
        [delegate onLoginSuccess:INVALIDPLAT backMsg:@""];
    }
}

- (void) getAuthorizedUserInfo:(NSString *)platstr delegate:(id<LoginDelegate>) delegate
{
    [[UMSocialDataService defaultDataService] requestSocialAccountWithCompletion:^(UMSocialResponseEntity *response)
     {
         //[self parseAuthorizeData:platstr response:response delegate:delegate];
         if (response.responseCode == UMSResponseCodeSuccess)
         {
             // 解析授权数据
             NSDictionary *accounts = [response.data objectForKey:@"accounts"];
             if (nil != accounts)
             {
                 NSDictionary *wxsession = [accounts objectForKey:@"wxsession"];
                 if (nil != wxsession)
                 {
                     // 根据token获取信息
                     NSString *URL= [NSString stringWithFormat:@"https://api.weixin.qq.com/sns/userinfo?access_token=%@&openid=%@&lang=zh_CN",[wxsession objectForKey:@"access_token"], [wxsession objectForKey:@"openid"]];
                     NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
                     [request setURL:[NSURL URLWithString:URL]];
                     [request setHTTPMethod:@"GET"];
                     NSHTTPURLResponse *urlResponse=nil;
                     NSError *errorr=nil;
                     NSData *receivedData = [NSURLConnection sendSynchronousRequest:request
                                                                  returningResponse:&urlResponse
                                                                              error:&errorr];
                     
                     NSString *results=[[[NSString alloc]initWithBytes:[receivedData bytes] length:[receivedData length] encoding:NSUTF8StringEncoding] autorelease];
                     NSData *data = [results dataUsingEncoding:NSUTF8StringEncoding];
                     NSError *error = nil;
                     id jsonObject = [NSJSONSerialization JSONObjectWithData:data
                                                                     options:NSJSONReadingAllowFragments
                                                                       error:&error];
                     if (FALSE == [jsonObject isKindOfClass:[NSDictionary class]])
                     {
                         // token获取失败， 删除授权
                         [[UMSocialDataService defaultDataService] requestUnOauthWithType:platstr completion:^(UMSocialResponseEntity *response) {
                             NSLog(@"res %@", response);
                             // 写入日志
                             LogAsset::getInstance()->logData(cocos2d::StringUtils::format("wx token获取异常 %s", [response.message UTF8String]));
                         }];
                         [delegate onLoginFail:platstr backMsg:@"token获取异常"];
                         return;
                     }
                     // 获取错误码
                     NSDictionary *info = (NSDictionary*)jsonObject;
                     NSString *errcode = [info objectForKey:@"errcode"];
                     if (nil != errcode)
                     {
                         // token获取失败， 删除授权
                         [[UMSocialDataService defaultDataService] requestUnOauthWithType:platstr completion:^(UMSocialResponseEntity *response) {
                             NSLog(@"res %@", response);
                             // 写入日志
                             std::string msg = cocos2d::StringUtils::format("wx token获取异常errorcode %s", [errcode UTF8String]);
                             LogAsset::getInstance()->logData(msg);
                         }];
                         [delegate onLoginFail:platstr backMsg:@"token获取异常"];
                         return;
                     }
                     
                     //添加同安卓一致信息
                     [wxsession setValue:[wxsession objectForKey:@"username"] forKey:@"screen_name"];
                     [wxsession setValue:[wxsession objectForKey:@"icon"] forKey:@"profile_image_url"];
                     [wxsession setValue:[info objectForKey:@"unionid"] forKey:@"unionid"];
                     int nSex = [[wxsession objectForKey:@"gender"] intValue];
                     if (1 == nSex)
                     {
                         [wxsession setValue:@"男" forKey:@"gender"];
                     }
                     
                     error = nil;
                     NSData *jsonData = [NSJSONSerialization dataWithJSONObject:wxsession
                                                                        options:NSJSONWritingPrettyPrinted
                                                                          error:&error];
                     if (nil != jsonData)
                     {
                         NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
                         //NSLog(@"json : %@",jsonString);
                         [delegate onLoginSuccess:platstr backMsg:jsonString];
                         [jsonString release];
                         return;
                     }
                 }
             }
         }
         std::string msg = cocos2d::StringUtils::format("wx授权失败 errorcode %d, msg %s", response.responseCode, [response.message UTF8String]);
         LogAsset::getInstance()->logData(msg);
         // 登陆失败删除授权
         [[UMSocialDataService defaultDataService] requestUnOauthWithType:platstr completion:^(UMSocialResponseEntity *response) {
             NSLog(@"res %@", response);
         }];
         [delegate onLoginFail:platstr backMsg:@""];
     }];
}

- (void) parseAuthorizeData:(NSString *)platstr response:(UMSocialResponseEntity *)response delegate:(id<LoginDelegate>) delegate
{
    if (response.responseCode == UMSResponseCodeSuccess)
    {
        //添加同安卓一致信息
        [response.thirdPlatformUserProfile setValue:[response.thirdPlatformUserProfile objectForKey:@"nickname"] forKey:@"screen_name"];
        [response.thirdPlatformUserProfile setValue:[response.thirdPlatformUserProfile objectForKey:@"headimgurl"] forKey:@"profile_image_url"];
        int nSex = [[response.thirdPlatformUserProfile objectForKey:@"sex"] intValue];
        if (1 == nSex)
        {
            [response.thirdPlatformUserProfile setValue:@"男" forKey:@"gender"];
        }
        
        NSError *error;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:response.thirdPlatformUserProfile
                                                           options:NSJSONWritingPrettyPrinted
                                                             error:&error];
        if (nil != jsonData)
        {
            NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            //NSLog(@"json : %@",jsonString);
            [delegate onLoginSuccess:platstr backMsg:jsonString];
            [jsonString release];
        }
        else
        {
            NSLog(@"to json error:%@", error);
            [delegate onLoginFail:platstr backMsg:@""];
        }
    }
    else if(response.responseCode == UMSResponseCodeCancel)
    {
        [delegate onLoginCancel:platstr backMsg:@""];
    }
    else
    {
        std::string msg = cocos2d::StringUtils::format("wx授权失败 errorcode %d, msg %s", response.responseCode, [response.message UTF8String]);
        LogAsset::getInstance()->logData(msg);
        [delegate onLoginFail:platstr backMsg:@""];
    }
}

- (void) openShare:(id<ShareDelegate>)delegate share:(tagShareParam)param
{
    [self setShareDelegate:delegate];
    AppController *pApp = (AppController*)[[UIApplication sharedApplication] delegate];
    
    if (TRUE == param.bImageOnly)
    {
        [UMSocialData defaultData].extConfig.wxMessageType = UMSocialWXMessageTypeImage;
    }
    else
    {
        [UMSocialData defaultData].extConfig.wxMessageType = UMSocialWXMessageTypeNone;
        [UMSocialWechatHandler setWXAppId:_wechat.WeChatAppID appSecret:_wechat.WeChatAppSecret url: param.sTargetURL];
    }
    [UMSocialData defaultData].extConfig.wechatSessionData.title = param.sTitle;
    
    [UMSocialSnsService presentSnsIconSheetView:pApp->viewController
                                         appKey:_share.AppKey
                                      shareText:param.sContent
                                     shareImage:[self getShareImage:param]
                                shareToSnsNames:[NSArray arrayWithArray:_umSharePlat]
                                       delegate:self];
}

- (void) targetShare:(id<ShareDelegate>)delegate share:(tagShareParam)param
{
    NSString *plat = [self getPlatform:param.nTarget];
    NSString *platstr = [_umDict objectForKey:plat];    
    [self setShareDelegate:delegate];
    AppController *pApp = (AppController*)[[UIApplication sharedApplication] delegate];
    
    if (TRUE == param.bImageOnly)
    {
        [UMSocialData defaultData].extConfig.wxMessageType = UMSocialWXMessageTypeImage;
    }
    else
    {
        [UMSocialData defaultData].extConfig.wxMessageType = UMSocialWXMessageTypeNone;
        [UMSocialWechatHandler setWXAppId:_wechat.WeChatAppID appSecret:_wechat.WeChatAppSecret url: param.sTargetURL];
    }
    [UMSocialData defaultData].extConfig.wechatSessionData.title = param.sTitle;
    
    [[UMSocialDataService defaultDataService] postSNSWithTypes:@[platstr]
                                                       content:param.sContent
                                                         image:[self getShareImage:param]
                                                      location:nil
                                                   urlResource:nil
                                           presentedController:pApp->viewController
                                                    completion:^(UMSocialResponseEntity *response)
     {
         if (response.responseCode == UMSResponseCodeCancel)
         {
             [_shareDelegate onCancel:INVALIDPLAT];
         }
         else if (response.responseCode == UMSResponseCodeSuccess)
         {
             [_shareDelegate onComplete:INVALIDPLAT backCode:response.responseCode backMsg:response.message];
         }
         else
         {
             [_shareDelegate onError:INVALIDPLAT backMsg:response.message];
         }
         [self setShareDelegate:nil];
     }];
}

- (id) getShareImage:(tagShareParam)param
{
    id image = nil;
    NSString *imageString = param.sMedia;
    if ([imageString hasPrefix:@"http://"] || [imageString hasPrefix:@"https://"])
    {
        [[UMSocialData defaultData].urlResource setResourceType:UMSocialUrlResourceTypeImage url:imageString];
    }
    else
    {
        if (FALSE == [param.sMedia isEqualToString:@""])
        {
            NSString *imageString = param.sMedia;
            if ([imageString.lowercaseString hasSuffix:@".gif"])
            {
                NSString *path = [[NSBundle mainBundle] pathForResource:[[imageString componentsSeparatedByString:@"."] objectAtIndex:0]
                                                                 ofType:@"gif"];
                image = [NSData dataWithContentsOfFile:path];
            }
            else if ([imageString rangeOfString:@"/"].length > 0)
            {
                image = [NSData dataWithContentsOfFile:imageString];
            }
            else
            {
                image = [UIImage imageNamed:imageString];
            }
            [UMSocialData defaultData].urlResource.resourceType = UMSocialUrlResourceTypeDefault;
        }
        else
        {
            image = [UIImage imageNamed:@"Icon-72.png"];
        }
    }
    return image;
}

- (void) thirdPartyPay:(NSString *)plat delegate:(id<PayDelegate>)delegate payparam:(NSDictionary *)payparam
{
    
    _payPlat = plat;
    [self setPayDelegate:delegate];
    
    if ([WECHAT isEqualToString:plat])
    {
        [self doWeChatPay:[payparam objectForKey:@"info"]];
    }
    else if ([ALIPAY isEqualToString:plat])
    {
        //解析支付参数
        BOOL bRes = FALSE;
        NSString *sOrderId = [payparam objectForKey:@"orderid"];
        bRes = (sOrderId != nil);
        NSString *sPrice = [payparam objectForKey:@"price"];
        bRes = (sPrice != nil);
        NSString *sProducName = [payparam objectForKey:@"name"];
        bRes = (sProducName != nil);
        if (FALSE == bRes)
        {
            [delegate onPayFail:plat backMsg:@"订单数据解析失败"];
            return;
        }
        tagPayParam param = {sOrderId, sProducName, [sPrice floatValue], 1};
        [self doAliPay:param];
    }
    else if ([JFT isEqualToString:plat])
    {
        [self doJftPay:payparam];
    }
    else if ([IOSIAP isEqualToString:plat])
    {
        [self doIapPay:payparam];
    }
}

- (void) getPayList:(NSString*)szparam delegate:(id<PayDelegate>)delegate
{
    if (FALSE == _jftpay.Configed)
    {
        [delegate onGetPayList:FALSE backMsg:@"竣付通配置异常"];
        return;
    }
    NSData *data = [szparam dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data
                                                    options:NSJSONReadingAllowFragments
                                                      error:&error];
    if (FALSE == [jsonObject isKindOfClass:[NSDictionary class]])
    {
        [delegate onGetPayList:FALSE backMsg:@"token数据解析失败"];
        return;
    }
    NSDictionary *tokenparam = (NSDictionary*)jsonObject;
    [self setPayDelegate:delegate];
    
    JFTTokenModel *tokenModel = [JFTTokenModel new];
    tokenModel.p1_user_code = _jftpay.JftPayPartnerID;
    tokenModel.userAppid = _jftpay.JftAppID;
    tokenModel.keyString = _jftpay.JftPayAesKey;
    tokenModel.ivString  = _jftpay.JftPayAesVec;
    tokenModel.p2_order = [tokenparam objectForKey:@"oid"];
    tokenModel.p3_money = [tokenparam objectForKey:@"mon"];
    tokenModel.p4_returnurl = [tokenparam objectForKey:@"rurl"];
    tokenModel.p5_notifyurl = [tokenparam objectForKey:@"nurl"];
    NSDateFormatter *formatter = [[NSDateFormatter alloc]init];
    [formatter setDateFormat:@"yyyyMMddHHmmss"];
    NSDate *p2_orderDate = [NSDate date];
    tokenModel.p6_ordertime = [formatter stringFromDate:p2_orderDate];
    NSString *p7_sign = [NSString stringWithFormat:@"%@&%@&%@&%@&%@&%@%@",tokenModel.p1_user_code,tokenModel.p2_order,tokenModel.p3_money,tokenModel.p4_returnurl,tokenModel.p5_notifyurl,tokenModel.p6_ordertime,_jftpay.JftPayKey];
    tokenModel.p7_sign = [jft_MD5 md532BitUpper:p7_sign];
    tokenModel.serviceType = @"JFT";
    //以下为可空参数
    tokenModel.parameterDic =@{
                               @"p8_signtype":@"",@"p10_paychannelnum":@"",@"p11_cardtype":@"",@"p12_channel":@"",@"p13_orderfailertime":@"",@"p14_customname":@"",@"p15_customcontacttype":@"",@"p16_customcontact":@"",@"p17_customip":@"",@"p18_product":@"",@"p19_productcat":@"",@"p20_productnum":@"",@"p21_pdesc":@"",@"p22_version":@"",@"p23_charset":@"",@"p24_remark":@"",@"p25_terminal":@"2",@"p26_iswappay":@"",@"P27_phonecharacter":@""};
    
    [JfPay getToken:tokenModel success:^(BOOL tokenSuccess) {
        [JfPay getPayTypeListkey:tokenModel.keyString iv:tokenModel.ivString appId:tokenModel.userAppid delegate:self];
        [tokenModel release];
    }failer:^(NSString *failerStr) {
        // 写入日志
        std::string msg = cocos2d::StringUtils::format("jft token获取失败 msg %s", [failerStr UTF8String]);
        LogAsset::getInstance()->logData(msg);
        NSLog(@"JfPay getToken failer %@",failerStr);
        [delegate onGetPayList:FALSE backMsg:failerStr];
        [tokenModel release];
    }];
}

- (BOOL) isPlatformInstalled:(NSString *)plat
{
    if ([WECHAT isEqualToString:plat])
    {
        return [WXApi isWXAppInstalled];
    }
    else if ([ALIPAY isEqualToString:plat])
    {
        NSURL *alipayUrl = [NSURL URLWithString:@"alipay:"];
        return [[UIApplication sharedApplication] canOpenURL:alipayUrl];
    }
    return FALSE;
}

- (void) requestLocation:(id<LocationDelegate>)delegate
{
    [self setLocationDelegate:delegate];
    if (FALSE == m_bConfigAmap)
    {
        [delegate onLocationResult:FALSE backCode:-1 backMsg:@"定位服务未配置!"];
        return;
    }
    [self.locationMgr requestLocationWithReGeocode:YES completionBlock:self.completionBlock];
}

- (NSString*) metersBetweenLocation:(NSDictionary *)loParam
{
    // 自己坐标
    double myLatitude = [[loParam objectForKey:@"myLatitude"] doubleValue];
    double myLongitude = [[loParam objectForKey:@"myLongitude"] doubleValue];
    
    // 对方坐标
    double otherLatitude = [[loParam objectForKey:@"otherLatitude"] doubleValue];
    double otherLongitude = [[loParam objectForKey:@"otherLongitude"] doubleValue];
    
    CLLocationCoordinate2D my2d = CLLocationCoordinate2DMake(myLatitude, myLongitude);
    CLLocationCoordinate2D o2d = CLLocationCoordinate2DMake(otherLatitude, otherLongitude);
    MAMapPoint mP = MAMapPointForCoordinate(my2d);
    MAMapPoint oP = MAMapPointForCoordinate(o2d);
    CLLocationDistance distance = MAMetersBetweenMapPoints(mP, oP);
    
    return [NSString stringWithFormat:@"%.2f", distance];
}

- (BOOL) isAuthorized:(NSString *)plat
{
    NSString *platstr = [_umDict objectForKey:plat];
    if (nil != platstr)
    {
        //是否授权(微信暂时需要重新授权)
        if ([UMSocialAccountManager isOauthAndTokenNotExpired:platstr])
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

- (void) deleteThirdPartyAuthorization:(NSString *)plat delegate:(id<DelAuthorDelegate>)delegate
{
    if (nil == delegate)
    {
        return;
    }
    NSString *platstr = [_umDict objectForKey:plat];
    if (nil != platstr)
    {
        // 删除授权
        [[UMSocialDataService defaultDataService] requestUnOauthWithType:platstr completion:^(UMSocialResponseEntity *response) {
            NSLog(@"res %@", response);
            if (response.responseCode == UMSResponseCodeSuccess)
            {
                [delegate onDeleteResult:true backCode:1 backMsg:@""];
            }
            else
            {
                [delegate onDeleteResult:false backCode:0 backMsg:@""];
            }
        }];
    }
    else
    {
        [delegate onDeleteResult:true backCode:2 backMsg:@""];
    }
}

#pragma mark -
- (void) doWeChatPay:(NSString*) infostr
{
    if (FALSE == _wechat.Configed)
    {
        [self onPayResult:FALSE msg:@"初始化失败"];
        return;
    }
    NSData *data = [infostr dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data
                                                    options:NSJSONReadingAllowFragments
                                                      error:&error];
    if (FALSE == [jsonObject isKindOfClass:[NSDictionary class]])
    {
        [self onPayResult:FALSE msg:@"订单数据解析失败"];
        return;
    }
    NSDictionary *info = (NSDictionary*)jsonObject;
    BOOL bRes = FALSE;
    NSString *appid = [info objectForKey:@"appid"];
    bRes = (appid != nil);
    NSString *partnerId = [info objectForKey:@"partnerid"];
    bRes = (partnerId != nil);
    NSString *prepayId = [info objectForKey:@"prepayid"];
    bRes = (prepayId != nil);
    NSString *packageValue = [info objectForKey:@"package"];
    bRes = (packageValue != nil);
    NSString *nonceStr = [info objectForKey:@"noncestr"];
    bRes = (nonceStr != nil);
    NSString *timeStamp = [info objectForKey:@"timestamp"];
    bRes = (timeStamp != nil);
    NSString *sign = [info objectForKey:@"sign"];
    bRes = (sign != nil);
    if (FALSE == bRes)
    {
        [self onPayResult:FALSE msg:@"订单数据解析失败"];
        return;
    }
    
    //调起微信支付
    PayReq* req = [[[PayReq alloc] init] autorelease];
    req.partnerId = partnerId;
    req.prepayId = prepayId;
    req.nonceStr = nonceStr;
    req.package = packageValue;
    req.timeStamp = [timeStamp intValue];
    req.sign = sign;
    [WXApi sendReq:req];
}

- (void) doAliPay:(tagPayParam) param
{
    if (FALSE == _alipay.Configed)
    {
        [self onPayResult:FALSE msg:@"初始化失败"];
        return;
    }
    // Order *porder = [[Order alloc] init];
    // porder.partner = _alipay.AlipayPartnerID;
    // porder.seller = _alipay.AlipaySeller;
    // porder.tradeNO = param.sOrderId;
    // porder.productName = param.sProductName;
    // porder.productDescription = param.sProductName;
    // porder.amount = [NSString stringWithFormat:@"%.2f", param.fPrice];
    // porder.notifyURL = _alipay.AlipayNotifyUrl;
    // porder.service = @"mobile.securitypay.pay";
    // porder.paymentType = @"1";
    // porder.inputCharset = @"utf-8";
    // porder.itBPay = @"30m";
    // porder.showUrl = @"m.alipay.com";
    
    // NSString *orderSpec = [porder description];
    // id<DataSigner> signer = CreateRSADataSigner(_alipay.AlipayPrivate);
    // NSString *signedString = [signer signString:orderSpec];
    // NSString *orderString = [NSString stringWithFormat:@"%@&sign=\"%@\"&sign_type=\"%@\"",orderSpec, signedString, @"RSA"];
    
    NSString *appID = _alipay.AlipayAppID;
    NSString *rsa2PrivateKey = @"";
    NSString *rsaPrivateKey = @"";
    rsa2PrivateKey = _alipay.AlipayPrivate;
     /*
     *生成订单信息及签名
     */
    //将商品信息赋予AlixPayOrder的成员变量
    APOrderInfo* order = [APOrderInfo new];
    
    // NOTE: app_id设置
    //order.app_id = _alipay.AlipayAppID;
    order.app_id = appID;
    
    // NOTE: 支付接口名称
    order.method = @"alipay.trade.app.pay";
    
    // NOTE: 参数编码格式
    order.charset = @"utf-8";
    
    // NOTE: 当前时间点
    NSDateFormatter* formatter = [NSDateFormatter new];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    order.timestamp = [formatter stringFromDate:[NSDate date]];
    
    // NOTE: 支付版本
    order.version = @"1.0";
    
    // NOTE: sign_type 根据商户设置的私钥来决定
    order.sign_type = (rsa2PrivateKey.length > 1)?@"RSA2":@"RSA";

    order.notify_url = _alipay.AlipayNotifyUrl;
    
    // NOTE: 商品数据
    order.biz_content = [APBizContent new];
    order.biz_content.body = @"";
    order.biz_content.subject = param.sProductName;
    order.biz_content.out_trade_no = param.sOrderId; //订单ID（由商家自行制定）
    order.biz_content.timeout_express = @"30m"; //超时时间设置
    order.biz_content.total_amount = [NSString stringWithFormat:@"%.2f", param.fPrice]; //商品价格
    //order.biz_content.product_code = @"QUICK_MSECURITY_PAY"; //超时时间设置
    
    //将商品信息拼接成字符串
    NSString *orderInfo = [order orderInfoEncoded:NO];
    NSString *orderInfoEncoded = [order orderInfoEncoded:YES];
    NSLog(@"orderSpec = %@",orderInfo);
    
    // NOTE: 获取私钥并将商户信息签名，外部商户的加签过程请务必放在服务端，防止公私钥数据泄露；
    //       需要遵循RSA签名规范，并将签名字符串base64编码和UrlEncode
    NSString *signedString = nil;
    APRSASigner* signer = [[APRSASigner alloc] initWithPrivateKey:((rsa2PrivateKey.length > 1)?rsa2PrivateKey:rsaPrivateKey)];
    if ((rsa2PrivateKey.length > 1)) {
        signedString = [signer signString:orderInfo withRSA2:YES];
    } else {
        signedString = [signer signString:orderInfo withRSA2:NO];
    }

    // NOTE: 如果加签成功，则继续执行支付
    if (signedString != nil) {
        //应用注册scheme,在AliSDKDemo-Info.plist定义URL types
        NSString *appScheme = _alipay.AliPaySchemes;
        //NSString *appScheme = @"alisdkdemo";
        // NOTE: 将签名成功字符串格式化为订单字符串,请严格按照该格式
        NSString *orderString = [NSString stringWithFormat:@"%@&sign=%@",
                                 orderInfoEncoded, signedString];
        
        // NOTE: 调用支付结果开始支付
        // [[AlipaySDK defaultService] payOrder:orderString fromScheme:appScheme callback:^(NSDictionary *resultDic) {
        //     NSLog(@"reslut = %@",resultDic);
        // }];

        // 调用支付
        [[AlipaySDK defaultService] payOrder:orderString
                                  fromScheme:appScheme
                                    callback:^(NSDictionary *resultDic)
        {
            NSLog(@"reslut = %@",resultDic);
            int resultvalue = [[resultDic valueForKey:@"resultStatus"] intValue];
            if (resultvalue == 9000)
            {
                //充值成功
                [self onPayResult:TRUE msg:@""];
            }
            else
            {
                // 写入日志
                LogAsset::getInstance()->logData(cocos2d::StringUtils::format("Alipay 充值失败 错误代码 %d", resultvalue));
                //充值失败
                [self onPayResult:FALSE msg:@""];
            }
        }];
    }

    
    // [porder release];
}

- (void) doJftPay:(NSDictionary *)payparam
{
    if (FALSE == _wechat.Configed)
    {
        [self onPayResult:FALSE msg:@"初始化失败"];
        return;
    }
    
    int nId = [[payparam valueForKey:@"paytype"] intValue];
    NSString *typeId = [NSString stringWithFormat:@"%d", nId];
    JFTPayModel *payModel =[JFTPayModel new];
    payModel.key = _jftpay.JftPayAesKey;
    payModel.iv = _jftpay.JftPayAesVec;
    payModel.appId = _jftpay.JftAppID;
    payModel.payTypeId = typeId;
    AppController * pApp = (AppController*)[[UIApplication sharedApplication] delegate];
    payModel.controler = (pApp->viewController);
    payModel.isReturn = YES;
    payModel.weichatAppid = _wechat.WeChatAppID;
    [JfPay payByJftPayModel:payModel delegate:self];
    [payModel release];
    
    //竣付通充值无回调
    [self onPayResult:TRUE msg:@""];
}

- (void) doIapPay:(NSDictionary *)payparam
{
    NSString *nsUrl = [payparam objectForKey:@"http_url"];
    if (nil == nsUrl)
    {
        [self onPayResult:FALSE msg:@"URL为空"];
        return;
    }
    std::string url = [nsUrl UTF8String];
    int dwudi = [[payparam objectForKey:@"uid"] intValue];
    NSString *nsProduct = [payparam objectForKey:@"productid"];
    if (nil == nsProduct)
    {
        [self onPayResult:FALSE msg:@"产品ID为空"];
        return;
    }
    std::string productid = [nsProduct UTF8String];
    float price = [[payparam objectForKey:@"price"] floatValue];
    m_iosiap.requestProducts(productid, [self,price](IOSProduct *product, int code)
                             {
                                 if (IAP_REQUEST_SUCCESS == code)
                                 {
                                     m_iosiap.requestPayment(1, price, [self](bool succeed, std::string &identifier, int quantity)
                                                             {
                                                                 if (false == succeed)
                                                                 {
                                                                     [self onPayResult:FALSE msg:@""];
                                                                 }
                                                                 else
                                                                 {
                                                                     [self onPayResult:TRUE msg:@""];
                                                                 }
                                                             });
                                 }
                             }, url, dwudi);
}

- (void) onPayResult:(BOOL)res msg:(NSString*)msg
{
    if (nil != _payDelegate)
    {
        if (TRUE == res)
        {
            [_payDelegate onPaySuccess:_payPlat backMsg:msg];
        }
        else
        {
            [_payDelegate onPayFail:_payPlat backMsg:msg];
        }
    }
    [self setPayDelegate:nil];
}
@end

