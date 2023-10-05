package org.cocos2dx.lib;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.util.concurrent.CountDownLatch;

import org.apache.http.util.EncodingUtils;

import com.google.zxing.*;
import com.google.zxing.common.HybridBinarizer;

class ShouldStartLoadingWorker implements Runnable {
    private CountDownLatch mLatch;
    private boolean[] mResult;
    private final int mViewTag;
    private final String mUrlString;

    ShouldStartLoadingWorker(CountDownLatch latch, boolean[] result, int viewTag, String urlString) {
        this.mLatch = latch;
        this.mResult = result;
        this.mViewTag = viewTag;
        this.mUrlString = urlString;
    }

    @Override
    public void run() {
        this.mResult[0] = Cocos2dxWebViewHelper._shouldStartLoading(mViewTag, mUrlString);
        this.mLatch.countDown(); // notify that result is ready
    }
}

public class Cocos2dxWebView extends WebView implements android.view.View.OnLongClickListener{
    private static final String TAG = Cocos2dxWebViewHelper.class.getSimpleName();
    private LongClickCallBack mCallBack;
    public interface LongClickCallBack{
        /**用于传递图片地址*/
        void onLongClickCallBack(String imgUrl);
    }
    
    private int mViewTag;
    private String mJSScheme;

    public Cocos2dxWebView(Context context) {
        this(context, -1);
    }

    @SuppressLint("SetJavaScriptEnabled")
    public Cocos2dxWebView(Context context, int viewTag) {
        super(context);
        this.mViewTag = viewTag;
        this.mJSScheme = "";

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);

        this.getSettings().setSupportZoom(false);

        this.getSettings().setJavaScriptEnabled(true);
        this.getSettings().setDomStorageEnabled(true);

        //设置背景颜色
        this.setBackgroundColor(0);

        // `searchBoxJavaBridge_` has big security risk. http://jvn.jp/en/jp/JVN53768697
        try {
            Method method = this.getClass().getMethod("removeJavascriptInterface", new Class[]{String.class});
            method.invoke(this, "searchBoxJavaBridge_");
        } catch (Exception e) {
            Log.d(TAG, "This API level do not support `removeJavascriptInterface`");
        }

        this.setWebViewClient(new Cocos2dxWebViewClient());
        this.setWebChromeClient(new WebChromeClient());
        
        this.setOnTouchListener(new View.OnTouchListener() { 
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                Log.i("onTouch ","onTouch ");
                switch (event.getAction()) { 
                    case MotionEvent.ACTION_DOWN:  
                        Log.i("onTouch ACTION_DOWN","onTouch ACTION_DOWN");
                        final HitTestResult htr = getHitTestResult();//获取所点击的内容
                        if (htr.getType() == WebView.HitTestResult.IMAGE_TYPE) {//判断被点击的类型为图片
                            Log.i("长按图片","长按图片");
                            Cocos2dxActivity activity = (Cocos2dxActivity)getContext();
                            mCallBack=(LongClickCallBack)activity;
                            String s=htr.getExtra();
                            Log.i("getExtra",s);
                            if(s.startsWith("data:image/png;base64") || s.startsWith("data:image/gif;base64"))//仅处理竣付通
                                mCallBack.onLongClickCallBack(s);
                        }
                        return false;        
                }
                return false;                  
            }
            });
    }
    
    // public static Bitmap getBitmap(String sUrl){  
    //     try {  
    //         URL url = new URL(sUrl);  
    //         HttpURLConnection conn = (HttpURLConnection)url.openConnection();  
    //         conn.setConnectTimeout(5000);  
    //         conn.setRequestMethod("GET");  
    //         if(conn.getResponseCode() == 200){  
    //             InputStream inputStream = conn.getInputStream();  
    //             Bitmap bitmap = BitmapFactory.decodeStream(inputStream);  
    //             return bitmap;  
    //         }  
    //     } catch (Exception e) {  
    //         e.printStackTrace();  
    //     }  
    //     return null;  
    // }  
    
    // public static String handleQRCodeFormBitmap(Bitmap bitmap)
    // {
    // 	if (bitmap == null)
    //     {
    //         Log.e(TAG, "uri is not a bitmap" );
    //         return null;
    //     }
    //     int width = bitmap.getWidth(), height = bitmap.getHeight();
    //     int[] pixels = new int[width * height];
    //     bitmap.getPixels(pixels, 0, width, 0, 0, width, height);
    //     bitmap.recycle();
    //     bitmap = null;
    //     RGBLuminanceSource source = new RGBLuminanceSource(width, height, pixels);
    //     BinaryBitmap bBitmap = new BinaryBitmap(new HybridBinarizer(source));
    //     MultiFormatReader reader = new MultiFormatReader();
    //     try
    //     {
    //         Result result = reader.decode(bBitmap);
    //         String content=result.getText();
    //         return content;
    //     }
    //     catch (NotFoundException e)
    //     {
    //         Log.e(TAG, "decode exception", e);
    //         return null;
    //     }
    	
    // }

    public void setJavascriptInterfaceScheme(String scheme) {
        this.mJSScheme = scheme != null ? scheme : "";
    }

    public void setScalesPageToFit(boolean scalesPageToFit) {
        this.getSettings().setSupportZoom(scalesPageToFit);
    }
    



    class Cocos2dxWebViewClient extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, final String urlString) {
            Cocos2dxActivity activity = (Cocos2dxActivity)getContext();

            if(urlString.startsWith("http:") || urlString.startsWith("https:")) 
            {   
                // view.loadUrl(urlString);    
                // return false;    
            }else
            {  
                Intent intent = new Intent(Intent.ACTION_VIEW);
                intent.setData(Uri.parse(urlString));
                activity.startActivity(intent);  
                return true;    
            }  


            try {
                URI uri = URI.create(urlString);
                if (uri != null && uri.getScheme().equals(mJSScheme)) {
                    activity.runOnGLThread(new Runnable() {
                        @Override
                        public void run() {
                            Cocos2dxWebViewHelper._onJsCallback(mViewTag, urlString);
                        }
                    });
                    return true;
                }
            } catch (Exception e) {
                Log.d(TAG, "Failed to create URI from url");
            }

            boolean[] result = new boolean[] { true };
            CountDownLatch latch = new CountDownLatch(1);

            // run worker on cocos thread
            activity.runOnGLThread(new ShouldStartLoadingWorker(latch, result, mViewTag, urlString));

            // wait for result from cocos thread
            try {
                latch.await();
            } catch (InterruptedException ex) {
                Log.d(TAG, "'shouldOverrideUrlLoading' failed");
            }

            return result[0];
        }

        @Override
        public void onPageFinished(WebView view, final String url) {
            super.onPageFinished(view, url);
            Cocos2dxActivity activity = (Cocos2dxActivity)getContext();
            activity.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    Cocos2dxWebViewHelper._didFinishLoading(mViewTag, url);
                }
            });
        }

        @Override
        public void onReceivedError(WebView view, int errorCode, String description, final String failingUrl) {
            super.onReceivedError(view, errorCode, description, failingUrl);
            Cocos2dxActivity activity = (Cocos2dxActivity)getContext();
            activity.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    Cocos2dxWebViewHelper._didFailLoading(mViewTag, failingUrl);
                }
            });
        }
    }

    public void setWebViewRect(int left, int top, int maxWidth, int maxHeight) {
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        layoutParams.leftMargin = left;
        layoutParams.topMargin = top;
        layoutParams.width = maxWidth;
        layoutParams.height = maxHeight;
        layoutParams.gravity = Gravity.TOP | Gravity.LEFT;
        this.setLayoutParams(layoutParams);
    }
    
    public void browsePost(String url,String param) {
    	byte[] post = EncodingUtils.getBytes(param, "BASE64");
        Log.d("postUrl",url);
		this.postUrl(url, post);
        //assert(false);
    }


	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch (keyCode) {
        	case KeyEvent.KEYCODE_BACK:
        		if (canGoBack()){
        			goBack();
        		}
        		return true;
        	default:
        		return super.onKeyDown(keyCode, event);
		}
	}

	@Override
	public boolean onLongClick(View v) {
		 // 长按事件监听（注意：需要实现LongClickCallBack接口并传入对象）
//    	Log.i("长按","长按");
//        final HitTestResult htr = getHitTestResult();//获取所点击的内容
//        if (htr.getType() == WebView.HitTestResult.IMAGE_TYPE) {//判断被点击的类型为图片
//        	Log.i("长按图片","长按图片");
//        	Cocos2dxActivity activity = (Cocos2dxActivity)getContext();
//        	mCallBack=(LongClickCallBack)activity;
//            mCallBack.onLongClickCallBack(htr.getExtra());
//        }
        return false;
	}
}
