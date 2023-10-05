//
//  LuaObjectCBridge.m
//  GloryProject
//
//  Created by zhong on 16/8/31.
//
//
#import "Utils.h"

//iOS中获取网卡mac信息
#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#import "RealReachability.h"
using namespace cocos2d;

@implementation Utils
+(NSString*) loadKeyChainSavedUUID:(NSString*) keys
{
    id ret = nil;
    NSMutableDictionary *keychainQuery = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                          (id)kSecClassGenericPassword , (id)kSecClass,
                                          keys, (id)kSecAttrService,
                                          keys, (id)kSecAttrAccount,
                                          (id)kSecAttrAccessibleAfterFirstUnlock, (id)kSecAttrAccessible,
                                          nil];
    //Configure the search setting
    //Since in our simple case we are expecting only a single attribute to be returned (the password) we can set the attribute kSecReturnData to kCFBooleanTrue
    [keychainQuery setObject:(id)kCFBooleanTrue forKey:(id)kSecReturnData];
    [keychainQuery setObject:(id)kSecMatchLimitOne forKey:(id)kSecMatchLimit];
    CFDataRef keyData = NULL;
    if (SecItemCopyMatching((CFDictionaryRef)keychainQuery, (CFTypeRef *)&keyData) == noErr) {
        @try {
            ret = [NSKeyedUnarchiver unarchiveObjectWithData:(__bridge NSData *)keyData];
        } @catch (NSException *e) {
            NSLog(@"Unarchive of %@ failed: %@", keys, e);
        } @finally {
        }
    }
    if (keyData)
        CFRelease(keyData);
    return ret;
}

+(void) saveUUID:(NSString *)keys data:(id)data
{
    //Get search dictionary
    NSMutableDictionary *keychainQuery = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                          (id)kSecClassGenericPassword , (id)kSecClass,
                                          keys, (id)kSecAttrService,
                                          keys, (id)kSecAttrAccount,
                                          (id)kSecAttrAccessibleAfterFirstUnlock, (id)kSecAttrAccessible,
                                          nil];
    //Delete old item before add new item
    SecItemDelete((CFDictionaryRef)keychainQuery);
    //Add new object to search dictionary(Attention:the data format)
    [keychainQuery setObject:[NSKeyedArchiver archivedDataWithRootObject:data] forKey:(id)kSecValueData];
    //Add item to keychain with the search dictionary
    SecItemAdd((CFDictionaryRef)keychainQuery, NULL);
}

+(NSString*) getUUID
{
    NSString *packagename = [[NSBundle mainBundle] bundleIdentifier];
    NSString *uuid = [Utils loadKeyChainSavedUUID: packagename];
    if (nil == uuid || [uuid isEqualToString:@""])
    {
        // 生成uuid
        NSString *ivendor = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        uuid = [NSString stringWithFormat:@"%@%@",ivendor,packagename];
        
        // 存储
        [Utils saveUUID:packagename data:uuid];
    }
    return uuid;
}

+(NSString*) getHostAdress
{
    //return @"192.168.1.1";
    NSString *address = @"192.168.1.1";
    struct ifaddrs *interfaces = nil;
    struct ifaddrs *temp_addr = nil;
    int success = 0;
    success = getifaddrs(&interfaces);
    if (0 == success)
    {
        temp_addr = interfaces;
        while (nil != temp_addr)
        {
            if ( AF_INET == temp_addr->ifa_addr->sa_family )
            {
                if ([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"])
                {
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    freeifaddrs(interfaces);
    return address;
}

+(void) startNetworkDetetion
{
    [GLobalRealReachability startNotifier];
}

+(BOOL) isNetworkAvailable
{
    ReachabilityStatus status = [GLobalRealReachability currentReachabilityStatus];
    NSLog(@"Initial reachability status:%@",@(status));
    return status != RealStatusNotReachable;
}
@end
