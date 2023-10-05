/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "platform/CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#include "UIWebViewImpl-ios.h"
#include "renderer/CCRenderer.h"
#include "base/CCDirector.h"
#include "platform/CCGLView.h"
#include "platform/ios/CCEAGLView-ios.h"
#include "platform/CCFileUtils.h"
#include "ui/UIWebView.h"


static std::string getFixedBaseUrl(const std::string& baseUrl)
{
    std::string fixedBaseUrl;
    if (baseUrl.empty() || baseUrl.c_str()[0] != '/') {
        fixedBaseUrl = [[[NSBundle mainBundle] resourcePath] UTF8String];
        fixedBaseUrl += "/";
        fixedBaseUrl += baseUrl;
    }
    else {
        fixedBaseUrl = baseUrl;
    }
    
    size_t pos = 0;
    while ((pos = fixedBaseUrl.find(" ")) != std::string::npos) {
        fixedBaseUrl.replace(pos, 1, "%20");
    }
    
    if (fixedBaseUrl.c_str()[fixedBaseUrl.length() - 1] != '/') {
        fixedBaseUrl += "/";
    }
    
    return fixedBaseUrl;
}

@interface UIWebViewWrapper : NSObject
{NSInteger _gesState;}
@property (nonatomic) std::function<bool(std::string url)> shouldStartLoading;
@property (nonatomic) std::function<void(std::string url)> didFinishLoading;
@property (nonatomic) std::function<void(std::string url)> didFailLoading;
@property (nonatomic) std::function<void(std::string url)> onJsCallback;

@property(nonatomic, readonly, getter=canGoBack) BOOL canGoBack;
@property(nonatomic, readonly, getter=canGoForward) BOOL canGoForward;
@property (nonatomic,copy) NSString *imgURL;


- (void)handleLongTouch;

+ (instancetype)webViewWrapper;

- (void)setVisible:(bool)visible;

- (void)setFrameWithX:(float)x y:(float)y width:(float)width height:(float)height;

- (void)setJavascriptInterfaceScheme:(const std::string &)scheme;

- (void)loadData:(const std::string &)data MIMEType:(const std::string &)MIMEType textEncodingName:(const std::string &)encodingName baseURL:(const std::string &)baseURL;

- (void)loadHTMLString:(const std::string &)string baseURL:(const std::string &)baseURL;

- (void)loadUrl:(const std::string &)urlString;

- (void)loadFile:(const std::string &)filePath;

- (void)browsePost:(const std::string &)url param:(const std::string &)param;

- (void)stopLoading;

- (void)reload;

- (void)evaluateJS:(const std::string &)js;

- (void)goBack;

- (void)goForward;

- (void)setScalesPageToFit:(const bool)scalesPageToFit;
@end


@interface UIWebViewWrapper () <UIWebViewDelegate>
@property(nonatomic, retain) UIWebView *uiWebView;
@property(nonatomic, copy) NSString *jsScheme;

@end

@implementation UIWebViewWrapper {

}

+ (instancetype)webViewWrapper {
    return [[[self alloc] init] autorelease];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.uiWebView = nil;
        self.shouldStartLoading = nullptr;
        self.didFinishLoading = nullptr;
        self.didFailLoading = nullptr;
    
    }
    return self;
}

- (void)dealloc {
    self.uiWebView.delegate = nil;
    [self.uiWebView removeFromSuperview];
    self.uiWebView = nil;
    self.jsScheme = nil;
    [super dealloc];
}

- (void)setupWebView {
    if (!self.uiWebView) {
        self.uiWebView = [[[UIWebView alloc] init] autorelease];
        self.uiWebView.delegate = self;
        self.uiWebView.opaque = NO;
        self.uiWebView.backgroundColor = [UIColor clearColor];
    }
    if (!self.uiWebView.superview) {
        auto view = cocos2d::Director::getInstance()->getOpenGLView();
        auto eaglview = (CCEAGLView *) view->getEAGLView();
        [eaglview addSubview:self.uiWebView];
    }
}

- (void)setVisible:(bool)visible {
    self.uiWebView.hidden = !visible;
}


- (void)setFrameWithX:(float)x y:(float)y width:(float)width height:(float)height {
    if (!self.uiWebView) {[self setupWebView];}
    CGRect newFrame = CGRectMake(x, y, width, height);
    if (!CGRectEqualToRect(self.uiWebView.frame, newFrame)) {
        self.uiWebView.frame = CGRectMake(x, y, width, height);
    }
}

- (void)browsePost:(const std::string &)url param:(const std::string &)param {
    //why not visible
    if (!self.uiWebView) {[self setupWebView];}
    NSLog(@"ios-mm browsePost");
    NSLog(@(url.c_str()));
    NSLog(@(param.c_str()));
    NSLog(@"ios-mm browsePost end");
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@(url.c_str())]];
    NSString *post = [NSString stringWithFormat:@(param.c_str())];
    NSData *postData = [post dataUsingEncoding:NSUTF8StringEncoding];
    [request setHTTPMethod:@"POST"];
    [request setValue:[NSString stringWithFormat:@"%d", [postData length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:postData];
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [self.uiWebView loadRequest:request];
    
    
//    NSMutableString *s = [NSMutableString stringWithCapacity:0];
//    [s appendString: @"<html><body onload=\"document.forms[0].submit()\">"
//     "<form method=\"post\" action=\"http://someplace.com/\">"
//     "<input type=\"hidden\" name=\"param1\">"];
//    [s appendString: Param1Value]; //It's your variable
//    [s appendString: @"</input></form></body></html>"];
}

- (void)setJavascriptInterfaceScheme:(const std::string &)scheme {
    self.jsScheme = @(scheme.c_str());
}

- (void)loadData:(const std::string &)data MIMEType:(const std::string &)MIMEType textEncodingName:(const std::string &)encodingName baseURL:(const std::string &)baseURL {
    [self.uiWebView loadData:[NSData dataWithBytes:data.c_str() length:data.length()]
                    MIMEType:@(MIMEType.c_str())
            textEncodingName:@(encodingName.c_str())
                     baseURL:[NSURL URLWithString:@(getFixedBaseUrl(baseURL).c_str())]];
}

- (void)loadHTMLString:(const std::string &)string baseURL:(const std::string &)baseURL {
    if (!self.uiWebView) {[self setupWebView];}
    [self.uiWebView loadHTMLString:@(string.c_str()) baseURL:[NSURL URLWithString:@(getFixedBaseUrl(baseURL).c_str())]];
}

- (void)loadUrl:(const std::string &)urlString {
    if (!self.uiWebView) {[self setupWebView];}
    NSURL *url = [NSURL URLWithString:@(urlString.c_str())];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [self.uiWebView loadRequest:request];
}

- (void)loadFile:(const std::string &)filePath {
    if (!self.uiWebView) {[self setupWebView];}
    NSURL *url = [NSURL fileURLWithPath:@(filePath.c_str())];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [self.uiWebView loadRequest:request];
}

- (void)stopLoading {
    [self.uiWebView stopLoading];
}

- (void)reload {
    [self.uiWebView reload];
}

- (BOOL)canGoForward {
    return self.uiWebView.canGoForward;
}

- (BOOL)canGoBack {
    return self.uiWebView.canGoBack;
}

- (void)goBack {
    [self.uiWebView goBack];
}

- (void)goForward {
    [self.uiWebView goForward];
}

- (void)evaluateJS:(const std::string &)js {
    if (!self.uiWebView) {[self setupWebView];}
    [self.uiWebView stringByEvaluatingJavaScriptFromString:@(js.c_str())];
}

- (void)setScalesPageToFit:(const bool)scalesPageToFit {
    if (!self.uiWebView) {[self setupWebView];}
    self.uiWebView.scalesPageToFit = scalesPageToFit;
}

#pragma mark - UIWebViewDelegate
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
    
    NSString *url = [[request URL] absoluteString];
    NSLog(@"shouldStartLoadWithRequest url:%@",url);
    if ([[[request URL] scheme] isEqualToString:self.jsScheme]) {
        self.onJsCallback([url UTF8String]);
        return NO;
    }
    
    NSArray *components = [url componentsSeparatedByString:@":"];
    if ([components count] > 1 && [(NSString *)[components objectAtIndex:0]
                                   isEqualToString:@"myweb"]) {
        if([(NSString *)[components objectAtIndex:1] isEqualToString:@"touch"])
        {
            //NSLog(@"you are touching!");
            if ([(NSString *)[components objectAtIndex:2] isEqualToString:@"start"])
            {
                NSLog(@" UIGestureRecognizerStateBegan");
                _gesState = UIGestureRecognizerStateBegan;
                
                float ptX  =[[components objectAtIndex:3]floatValue];
                float ptY =[[components objectAtIndex:4]floatValue];
                
                NSString *js = [NSString stringWithFormat:@"document.elementFromPoint(%f, %f).tagName", ptX, ptY];
                NSString * tagName = [webView stringByEvaluatingJavaScriptFromString:js];
                NSLog(@"tagName:%@",tagName);
                if ([tagName isEqualToString:@"IMG"]) {
                     self.imgURL = [NSString stringWithFormat:@"document.elementFromPoint(%f, %f).src", ptX, ptY];
                }
                NSLog(@"self.imgURL:%@",self.imgURL);
                if (self.imgURL) {
                    NSLog(@"performSelector:selector(handleLongTouch)");
                    [self performSelector:@selector(handleLongTouch) withObject:nil afterDelay:1.0];
                }
            }
            else if ([(NSString *)[components objectAtIndex:2] isEqualToString:@"move"])
            {
                 NSLog(@" UIGestureRecognizerStateChanged");
                _gesState = UIGestureRecognizerStateChanged;
                
            }
        }
        else if ([(NSString*)[components objectAtIndex:2]isEqualToString:@"end"]) {
             NSLog(@" UIGestureRecognizerStateEnded");
            _gesState = UIGestureRecognizerStateEnded;
            
        }
    }// ]end
    
    if ([url hasPrefix:@"alipays://"] )
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
    }
    
    if (self.shouldStartLoading && url) {
        return self.shouldStartLoading([url UTF8String]);
    }
    return YES;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView {
    static NSString* const kTouchJavaScriptString=
    @"document.ontouchstart=function(event){\
    x=event.targetTouches[0].clientX;\
    y=event.targetTouches[0].clientY;\
    document.location=\"myweb:touch:start:\"+x+\":\"+y;};\
    document.ontouchmove=function(event){\
    x=event.targetTouches[0].clientX;\
    y=event.targetTouches[0].clientY;\
    document.location=\"myweb:touch:move:\"+x+\":\"+y;};\
    document.ontouchcancel=function(event){\
    document.location=\"myweb:touch:cancel\";};\
    document.ontouchend=function(event){\
    document.location=\"myweb:touch:end\";};";
    [webView stringByEvaluatingJavaScriptFromString:kTouchJavaScriptString];//注入js方法
     NSLog(@"webViewDidFinishLoad");
    if (self.didFinishLoading) {
       
        NSString *url = [[webView.request URL] absoluteString];
        self.didFinishLoading([url UTF8String]);
    }
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error {
    NSLog(@"didFailLoadWithError");
    if (self.didFailLoading) {
        NSString *url = error.userInfo[NSURLErrorFailingURLStringErrorKey];
        NSLog(@"didfailloading: %@",url);
        if (url) {
            self.didFailLoading([url UTF8String]);
        }
    }
}

- (UIViewController *)viewController {
    for (UIView* next = [self superview]; next; next = next.superview) {
        UIResponder *nextResponder = [next nextResponder];
        if ([nextResponder isKindOfClass:[UIViewController class]]) {
            return (UIViewController *)nextResponder;
        }
    }
    return nil;
}


- (void)handleLongTouch {
    NSLog(@"handleLongTouch");
    if (self.imgURL && _gesState == UIGestureRecognizerStateBegan) {
        
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
        
        NSString *urlToSave = [self.uiWebView stringByEvaluatingJavaScriptFromString:self.imgURL];
        NSLog(@"urlToSave:%@",urlToSave);
        NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString:urlToSave]];
        if(NULL==data ) {NSLog(@"urltosavedata nil"); return;}
        UIImage* image = [UIImage imageWithData:data];
        if(NULL==image) {NSLog(@"Image nil"); return;}
        NSDictionary *options = [[NSDictionary alloc] initWithObjectsAndKeys:
                                 @"CIDetectorAccuracy", @"CIDetectorAccuracyHigh",nil];
        CIDetector *detector = nil;
        if ([[UIDevice currentDevice].systemVersion floatValue] >= 8.0)
            detector = [CIDetector detectorOfType:CIDetectorTypeQRCode
                                          context:nil
                                          options:options];
        NSArray *features = [detector featuresInImage:[CIImage imageWithCGImage:image.CGImage]] ;
            
        // CIQRCodeFeature *feature = [features objectAtIndex:0];
        // NSString *scannedResult = feature.messageString;
            
        if (features.count <1)
            return;
        
        // 保存图片到手机
        UIAlertAction *saveImage = [UIAlertAction actionWithTitle:@"保存图中二维码" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            
            UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
        }];

        [alertController addAction:saveImage];
        
     
        UIAlertAction *cancell = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        
                }];
        
        [alertController addAction:cancell];
        
        UIViewController * vc=nil;
        for (UIView* next = [self.uiWebView superview]; next; next = next.superview) {
            UIResponder *nextResponder = [next nextResponder];
            if ([nextResponder isKindOfClass:[UIViewController class]]) {
                vc= (UIViewController *)nextResponder;
                break;
            }
        }
        
       [vc presentViewController:alertController animated:YES completion:nil];
    }
}

@end



namespace cocos2d {
namespace experimental {
    namespace ui{

WebViewImpl::WebViewImpl(WebView *webView)
        : _uiWebViewWrapper([UIWebViewWrapper webViewWrapper]),
        _webView(webView) {
    [_uiWebViewWrapper retain];
            
    _uiWebViewWrapper.shouldStartLoading = [this](std::string url) {
        if (this->_webView->_onShouldStartLoading) {
            return this->_webView->_onShouldStartLoading(this->_webView, url);
        }
        return true;
    };
    _uiWebViewWrapper.didFinishLoading = [this](std::string url) {
        if (this->_webView->_onDidFinishLoading) {
            this->_webView->_onDidFinishLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper.didFailLoading = [this](std::string url) {
        if (this->_webView->_onDidFailLoading) {
            this->_webView->_onDidFailLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper.onJsCallback = [this](std::string url) {
        if (this->_webView->_onJSCallback) {
            this->_webView->_onJSCallback(this->_webView, url);
        }
    };
}

WebViewImpl::~WebViewImpl(){
    [_uiWebViewWrapper release];
    _uiWebViewWrapper = nullptr;
}

void WebViewImpl::setJavascriptInterfaceScheme(const std::string &scheme) {
    [_uiWebViewWrapper setJavascriptInterfaceScheme:scheme];
}

void WebViewImpl::loadData(const Data &data,
                           const std::string &MIMEType,
                           const std::string &encoding,
                           const std::string &baseURL) {
    
    std::string dataString(reinterpret_cast<char *>(data.getBytes()), static_cast<unsigned int>(data.getSize()));
    [_uiWebViewWrapper loadData:dataString MIMEType:MIMEType textEncodingName:encoding baseURL:baseURL];
}

void WebViewImpl::loadHTMLString(const std::string &string, const std::string &baseURL) {
    [_uiWebViewWrapper loadHTMLString:string baseURL:baseURL];
}

void WebViewImpl::loadURL(const std::string &url) {
    [_uiWebViewWrapper loadUrl:url];
}

void WebViewImpl::loadFile(const std::string &fileName) {
    auto fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);
    [_uiWebViewWrapper loadFile:fullPath];
}

void WebViewImpl::browsePost(const std::string &url, const std::string &param) {
    [_uiWebViewWrapper browsePost:url param:param];
}

void WebViewImpl::stopLoading() {
    [_uiWebViewWrapper stopLoading];
}

void WebViewImpl::reload() {
    [_uiWebViewWrapper reload];
}

bool WebViewImpl::canGoBack() {
    return _uiWebViewWrapper.canGoBack;
}

bool WebViewImpl::canGoForward() {
    return _uiWebViewWrapper.canGoForward;
}

void WebViewImpl::goBack() {
    [_uiWebViewWrapper goBack];
}

void WebViewImpl::goForward() {
    [_uiWebViewWrapper goForward];
}

void WebViewImpl::evaluateJS(const std::string &js) {
    [_uiWebViewWrapper evaluateJS:js];
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit) {
    [_uiWebViewWrapper setScalesPageToFit:scalesPageToFit];
}

void WebViewImpl::draw(cocos2d::Renderer *renderer, cocos2d::Mat4 const &transform, uint32_t flags) {
    if (flags & cocos2d::Node::FLAGS_TRANSFORM_DIRTY) {
        
        auto direcrot = cocos2d::Director::getInstance();
        auto glView = direcrot->getOpenGLView();
        auto frameSize = glView->getFrameSize();
        
        auto scaleFactor = [static_cast<CCEAGLView *>(glView->getEAGLView()) contentScaleFactor];

        auto winSize = direcrot->getWinSize();

        auto leftBottom = this->_webView->convertToWorldSpace(cocos2d::Vec2::ZERO);
        auto rightTop = this->_webView->convertToWorldSpace(cocos2d::Vec2(this->_webView->getContentSize().width, this->_webView->getContentSize().height));

        auto x = (frameSize.width / 2 + (leftBottom.x - winSize.width / 2) * glView->getScaleX()) / scaleFactor;
        auto y = (frameSize.height / 2 - (rightTop.y - winSize.height / 2) * glView->getScaleY()) / scaleFactor;
        auto width = (rightTop.x - leftBottom.x) * glView->getScaleX() / scaleFactor;
        auto height = (rightTop.y - leftBottom.y) * glView->getScaleY() / scaleFactor;

        [_uiWebViewWrapper setFrameWithX:x
                                      y:y
                                  width:width
                                 height:height];
    }
}

void WebViewImpl::setVisible(bool visible){
    [_uiWebViewWrapper setVisible:visible];
}
        
    } // namespace ui
} // namespace experimental
} //namespace cocos2d

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS
