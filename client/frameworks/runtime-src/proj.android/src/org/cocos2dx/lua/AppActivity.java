/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
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
package org.cocos2dx.lua;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import org.apache.http.util.EncodingUtils;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxLuaJavaBridge;
import org.cocos2dx.lib.Cocos2dxWebView;
import org.cocos2dx.utils.ConstDefine;
import org.cocos2dx.utils.MP3Recorder;
import org.cocos2dx.utils.ScreenShotListenManager;
import org.cocos2dx.utils.ScreenShotListenManager.OnScreenShotListener;
import org.cocos2dx.thirdparty.ThirdDefine;
import org.cocos2dx.thirdparty.ThirdParty;
import org.cocos2dx.thirdparty.ThirdDefine.ShareParam;
import org.cocos2dx.thirdparty.ThirdParty.PLATFORM;
import org.cocos2dx.utils.Utils;
import org.json.JSONException;
import org.json.JSONObject;

import com.zhonglongyule.fangka.R;
import com.google.zxing.BinaryBitmap;
import com.google.zxing.MultiFormatReader;
import com.google.zxing.NotFoundException;
import com.google.zxing.RGBLuminanceSource;
import com.google.zxing.Result;
import com.google.zxing.common.HybridBinarizer;
// import com.umeng.message.IUmengRegisterCallback;
// import com.umeng.message.PushAgent;
// import com.umeng.message.UTrack;

import android.R.integer;
import android.content.BroadcastReceiver;
import android.app.Activity;
import android.app.Dialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.provider.Settings;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.provider.ContactsContract;
import android.provider.MediaStore;
import android.util.Base64;
import android.telephony.PhoneStateListener;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.WebView;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.Toast;


public class AppActivity extends Cocos2dxActivity implements Cocos2dxWebView.LongClickCallBack{

	static AppActivity	instance;
	
    static String hostIPAdress = "0.0.0.0";
    private ArrayAdapter<String> adapter;
    private Handler m_hHandler = null;
    private PowerManager.WakeLock mWakeLock = null;
    //lua toast函数
    static final String g_LuaToastFun = "g_NativeToast";
    // lua截图监听
    static final String g_LuaTakeScreenShot = "g_takeScreenShot";
    //登陆监听
    private ThirdParty.OnLoginListener m_LoginListener = null;
    //分享监听
    private ThirdParty.OnShareListener m_ShareListener = null;
    //支付监听
    private ThirdParty.OnPayListener m_PayListener = null;
    //定位监听
    private ThirdParty.OnLocationListener m_LocationListener = null;
    // 删除授权监听
    private ThirdParty.OnDelAuthorListener m_DeleAuthorListener = null;
 //电量
    private BatteryChangedReceiver batteryChangedReceiver = null;  
    
    //蜂窝网络
    public PhoneStatListener __CellularListener = null;
    
    /** Lua函数引用 **/
	// 选择图片回调
	private int m_nPickImgCallFunC = -1;
	// 支付回调
	private int m_nThirdPayCallFunC = -1;
	// 登陆回调
	private int m_nThirdLoginFunC = -1;
	// 分享回调
	private int m_nShareFunC = -1;
	// 支付列表回调
	private int m_nPayListFunC = -1;
	// 定位回调
	private int m_nLocationFunC = -1;
	// 通讯录回调
	private int m_nContactFunC = -1;
	// 取消授权回调
	private int m_nDelAuthorizationFunC = -1;
	// 获取复制内容
	private int m_nGetCopyDataFunC = -1;
	private File qrFile;
//电池电量
	private static int m_nBatteryLevel = 100;
	
	//wifi信号强度
	private WifiInfo wifiInfo = null;       //获得的Wifi信息  
    private WifiManager wifiManager = null; //Wifi管理器  
    private static int m_nNetStatus = 3;//默认信号最强 1无信号 2一般 3强
    private static int m_nNetType = ConstDefine.NETWORKTYPE_WIFI;//默认wifi
	// 音频保存的路径
	private static MP3Recorder recorder = null;
	
	// 截屏监听
	private ScreenShotListenManager mScreenListenManager = null;
	
	// 启动参数(外链启动)
	private String m_szLaunchData = "";
	// 设备token
	private String m_szDeviceToken = "";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	Log.i("AppActivity", "onCreate");
        super.onCreate(savedInstanceState);     
        com.umeng.socialize.utils.Log.LOG = false;
        // 推送
     	//PushAgent.getInstance(this).onAppStart();
        
        if(nativeIsLandScape()) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        } else {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
        }
        // 隐藏导航
        hideNavigationBar();
        // 监听
        this.getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
			@Override
			public void onSystemUiVisibilityChange(int arg0) {
				hideNavigationBar();
			}
		});
        
        hostIPAdress = getHostIpAddress();
		instance = this;
		// 常亮
		PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag"); 
		
        ThirdParty.getInstance().init(AppActivity.this);
        initHandler();
        initLoginListener();
        initShareListener();
        initPayListener();
        initLocationListener();
        initDelAuthorListener();
     	
  //电池电量
        batteryChangedReceiver=new BatteryChangedReceiver();
        
        //注册电量监听
        registBatter();
     	// 截屏监听
     	ScreenShotListenManager.getInstance().init(this);
     	ScreenShotListenManager.getInstance().setListener(new OnScreenShotListener() {
			@Override
			public void onShot(String imagePath) {
				toLuaGlobalFunC(g_LuaTakeScreenShot, imagePath);
			}
		});
     	ScreenShotListenManager.getInstance().startListen();
     	
     	getURLData();
     	
     	// m_szDeviceToken = PushAgent.getInstance(this).getRegistrationId();
     	// Log.i("Umeng Token", m_szDeviceToken);
     	// PushAgent.getInstance(this).addAlias(Utils.getUUID(this), "__FOXUC_UMSG_ALIAS__", new UTrack.ICallBack() {
     	//     @Override
     	//     public void onMessage(boolean isSuccess, String message) {
     	//     	Log.i("addAlias", "isSuccess:" + isSuccess + "," + message);
        if (null == __CellularListener) {
			// 开始监听
			 __CellularListener = new PhoneStatListener();
			// // 监听信号强度
			 TelephonyManager telephonyManager = (TelephonyManager) instance
			 		.getSystemService(Context.TELEPHONY_SERVICE);
			  //telephonyManager.listen(__CellularListener,
			  //		PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);

		}
 		// 获得WifiManager  
        wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);  
        
        // 使用定时器,每隔5秒获得一次信号强度值  
        Timer timer = new Timer();  
        timer.scheduleAtFixedRate(new TimerTask() {  
            @Override  
            public void run() 
            {  
            	//获取网络类型
            	if(getNetWorkType(instance)==ConstDefine.NETWORKTYPE_WIFI)
            	{
            		wifiInfo = wifiManager.getConnectionInfo();  
                    //获得信号强度值  
                    int level = wifiInfo.getRssi();  
                    //根据获得的信号强度发送信息  
                    if (level <= 0 && level >= -50) {  
                        Message msg = new Message();  
                        msg.what = ConstDefine.MSG_WIFI_STATUS_STRONG;  
                        m_hHandler.sendMessage(msg);  
                    } else if (level < -50 && level >= -70) {  
                        Message msg = new Message();  
                        msg.what = ConstDefine.MSG_WIFI_STATUS_SOSO;  
                        m_hHandler.sendMessage(msg);  
                    } else if (level < -70 && level >= -80) {  
                        Message msg = new Message();  
                        msg.what = ConstDefine.MSG_WIFI_STATUS_SOSO;  
                        m_hHandler.sendMessage(msg);  
                    } else if (level < -80 && level >= -100) {  
                        Message msg = new Message();  
                        msg.what = ConstDefine.MSG_WIFI_STATUS_WEAK;  
                        m_hHandler.sendMessage(msg);  
                    } else {  
                        Message msg = new Message();  
                        msg.what = ConstDefine.MSG_WIFI_STATUS_WEAK;  
                        m_hHandler.sendMessage(msg);  
                    }  
      
            	}
      
            }  
  
        }, 1000, 5000);  
     	//     }
     	// });
    }
 /*****************************************************/   
  //注册电量监听的Broadcastreceiver  
    public void registBatter()
    {  
        IntentFilter intentFilter=getFilter();  
        registerReceiver(batteryChangedReceiver,intentFilter);  
    }  
    //取消注册电量监听的Broadcastreceiver  
    public void unRegistBatter()
    {  
        unregisterReceiver(batteryChangedReceiver);  
    }  
    
    ///获取IntentFilter对象  
    private IntentFilter getFilter() 
    {  
        IntentFilter filter = new IntentFilter();  
        filter.addAction(Intent.ACTION_BATTERY_CHANGED);    
        return filter;  
    }  
    
    public class BatteryChangedReceiver extends BroadcastReceiver {  
        @Override  
        public void onReceive(Context context, Intent intent) 
        {  
            // TODO Auto-generated method stub  
            final String action = intent.getAction();  
            if (action.equalsIgnoreCase(Intent.ACTION_BATTERY_CHANGED))
            {  
                // 电池当前的电量, 它介于0和 EXTRA_SCALE之间  
                int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);  
                //AppActivity activity = (AppActivity)context;
                m_nBatteryLevel = level;
    
     
            }  
       } 
   }
    
  /********************************************************************************/  
    
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (RESULT_OK == resultCode) 
		{
			switch (requestCode) 
			{
				case ConstDefine.RES_PICKIMG_END:
				{
					photoClip((Uri)data.getData());
				}
					break;
				case ConstDefine.RES_CLIPEIMG_END:
				{
					photoClipEnd(data.getExtras());
				}
					break;
				case ConstDefine.RES_PICKIMG_END_NOCLIP:
				{
					photoPickEnd((Uri)data.getData());
				}
					break;
				case ConstDefine.RES_PICKCONTACK_END:
				{
					contactPickEnd((Uri)data.getData());
				}
					break;
				default:
					break;
			}
		}
		super.onActivityResult(requestCode, resultCode, data);
		ThirdParty.getInstance().onActivityResult(requestCode, resultCode, data);
	}   
    
	@Override
	protected void onDestroy() 
	{
		if(isAuthorized(ThirdParty.PLATFORM.WECHAT.ordinal()))
		{
			ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(0);
			ThirdParty.getInstance().deleteThirdPartyAuthorization(plat, m_DeleAuthorListener);
		}


		ThirdParty.destroy();
		unRegistBatter();
		ScreenShotListenManager.getInstance().stopListen();
		super.onDestroy();
	}

	@Override
	protected void onResume() {
		super.onResume();
		getURLData();
		mWakeLock.acquire(); 
		setIntent(null);
	}

	@Override
	protected void onPause() {
		mWakeLock.release();
		super.onPause();
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		setIntent(intent);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		Log.i("AppActivity", "have focus ==> " + hasFocus);
		if (hasFocus) {
			hideNavigationBar();
		}
	}

	private boolean isNetworkConnected() {
        return Utils.isNetworkConnected(this); 
    } 
     
    public String getHostIpAddress() 
    {
       return Utils.getHostIpAddress(this);
    }
    
    public static String getLocalIpAddress() {
        return hostIPAdress;
    }
    
    public void sendMessage(int what)
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = what;
    	
    	m_hHandler.sendMessage(msgMessage);
    }
    
    public void sendMessageWithObj(int what, Object obj)
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = what;
    	msgMessage.obj = obj;
    	
    	m_hHandler.sendMessage(msgMessage);
    }
    
    public void sendMessageWith(Message msg)
    {
    	m_hHandler.sendMessage(msg);
    }
    
    private void getURLData()
    {
    	// 外链启动数据
     	Intent itent = getIntent();
     	if (null != itent)
     	{
     		String action = itent.getAction();
         	if (Intent.ACTION_VIEW.equals(action)) {
         		Uri uriData = itent.getData();
         		if (uriData != null){
         			String urlParam = uriData.getQuery();
         			Log.i("getURLData ==> ", urlParam);
         			m_szLaunchData = urlParam;
         		}
         	}
     	}
    }
    
    private void initHandler()
    {
    	m_hHandler = new Handler()
    	{
			@Override
			public void handleMessage(Message msg) 
			{
				switch (msg.what) 
				{
					case ConstDefine.MSG_START_PICKIMG:
					{
						Intent intent = new Intent(Intent.ACTION_PICK, null);
				        intent.setDataAndType(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "image/*");  
		                startActivityForResult(intent, ConstDefine.RES_PICKIMG_END);
					}
						break;
					case ConstDefine.MSG_PICKIMG_END:
					{
						final String path = (String)msg.obj;
						toLuaFunC(instance.m_nPickImgCallFunC, path);
					}
						break;
					case ConstDefine.MSG_START_PICKIMG_NOCLIP:
					{
						Intent intent = new Intent(Intent.ACTION_PICK, null);
				        intent.setDataAndType(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "image/*");  
		                startActivityForResult(intent, ConstDefine.RES_PICKIMG_END_NOCLIP);
					}
						break;
					case ConstDefine.MSG_CONFIG_PARTY:
					{
						String configMsg = (String)msg.obj;
						if(null != configMsg && "" != configMsg)
						{
							ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(msg.arg1);
							if (plat != ThirdParty.PLATFORM.INVALIDPLAT)
							{
								ThirdParty.getInstance().configThirdParty(plat, configMsg);
							}
						}
						else 
						{
							toLuaToast("配置信息异常");							
						}						
					}
						break;
					case ConstDefine.MSG_SHARE_CONFIG:
					{
						ThirdParty.getInstance().configSocialShare();
					}
						break;
					case ConstDefine.MSG_THIRD_PAY:
					{
						String payparam = (String)msg.obj;						
						ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(msg.arg1);
						if (plat != ThirdParty.PLATFORM.INVALIDPLAT)
						{
							ThirdParty.getInstance().thirdPartyPay(plat, payparam, m_PayListener);
						}
					}
						break;
					case ConstDefine.MSG_THIRD_LOGIN:
					{
						ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(msg.arg1);
						if (plat != ThirdParty.PLATFORM.INVALIDPLAT)
						{
							ThirdParty.getInstance().thirdPartyLogin(plat, m_LoginListener);
						}						
					}
						break;
					case ConstDefine.MSG_SOCIAL_SHARE:
					{						
						ShareParam param = new ShareParam();
						param.sTitle = ThirdDefine.ShareTitle;
						param.sContent = ThirdDefine.ShareContent;
						param.sTargetURL = ThirdDefine.ShareURL; 
						param.sMedia = "";
						ThirdParty.getInstance().openShare(m_ShareListener, param);
					}
						break;
					case ConstDefine.MSG_SOCIAL_CUSCHARE:
					{
						ShareParam param = (ShareParam)msg.obj;
						ThirdParty.getInstance().openShare(m_ShareListener, param);
					}
						break;
					case ConstDefine.MSG_SOCIAL_TARGETSHARE:
					{
						ShareParam param = (ShareParam)msg.obj;
						ThirdParty.getInstance().targetShare(m_ShareListener, param);
					}
						break;
					case ConstDefine.MSG_JFT_PAYLIST:
					{
						String token = (String)msg.obj;
						ThirdParty.getInstance().getPayList(token, m_PayListener);
					}
						break;
					case ConstDefine.MSG_LOCATION_REQ:
					{
						ThirdParty.getInstance().requestLocation(m_LocationListener);
					}
						break;
					case ConstDefine.MSG_CONTACT_REQ:
					{
						Intent intent = new Intent(Intent.ACTION_PICK, ContactsContract.Contacts.CONTENT_URI);
		                startActivityForResult(intent, ConstDefine.RES_PICKCONTACK_END);
					}
						break;
					case ConstDefine.MSG_OPEN_BROWSER:
					{
				        String url = (String)msg.obj;
				        if (url != "")
				        {

					        	Intent intent = new Intent();
						        intent.setAction("android.intent.action.VIEW");
						        Uri content_url = Uri.parse(url);
						        intent.setData(content_url);
						        startActivity(intent);
					
				        }
					}
						break;
					case ConstDefine.MSG_OPEN_BROWSER_POST:
					{
						String url = (String)msg.obj;
				        if (url != "")
				        {
				        	String[] sArray=url.split(";");
				        	Log.d("sArray[0]",sArray[0]);
				        	Log.d("sArray[1]",sArray[1]);
				    		WebView webview = new WebView(instance);
				    		FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
				                    FrameLayout.LayoutParams.WRAP_CONTENT);
				            layoutParams.leftMargin = 300;
				            layoutParams.topMargin = 100;
				            layoutParams.width = 1300;
				            layoutParams.height = 760;
				            layoutParams.gravity = Gravity.TOP | Gravity.LEFT;
				    		webview.setLayoutParams(layoutParams);
							setContentView(webview);
							byte[] post = EncodingUtils.getBytes(sArray[1], "BASE64");
							webview.postUrl(sArray[0], post);
				        }
					}
						break;
					case ConstDefine.MSG_COPY_CLIPBOARD:
					{
						String str = (String)msg.obj;
						ClipboardManager clipBoard = (ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);
						clipBoard.setPrimaryClip(ClipData.newPlainText("txt copy", str));
						if (false == str.equals(""))
						{
							toLuaToast("复制成功!");
						}
					}
						break;
					case ConstDefine.MSG_DEL_AUTHOR:
					{
						ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(msg.arg1);
				    	ThirdParty.getInstance().deleteThirdPartyAuthorization(plat, m_DeleAuthorListener);
					}
						break;
					case ConstDefine.MSG_GET_COPYDATA:
					{
						// 复制内容
				    	String szClipText = "";
				    	ClipboardManager clipBoard = (ClipboardManager)instance.getSystemService(Context.CLIPBOARD_SERVICE);
				    	if (clipBoard.hasPrimaryClip()){
				    		// 获取数据对象
				        	ClipData clip = clipBoard.getPrimaryClip();
				        	if (null != clip){
				        		ClipData.Item item = clip.getItemAt(0);
				        		if (null != item.getText()){
				        			szClipText = item.getText().toString();
				        		}
				        	}
				    	}
				    	toLuaFunC(instance.m_nGetCopyDataFunC, szClipText);
				    	m_nGetCopyDataFunC = -1;
					}
					case ConstDefine.MSG_NATIVE_TOAST:
					{
						String str = (String)msg.obj;
						if (null != str)
						{
							toLuaGlobalFunC(g_LuaToastFun, str);
						}
					}
						break;
					case ConstDefine.MSG_SYSTEM_CALL:
					{
						String szTel = (String)msg.obj;
						if (null != szTel && szTel.trim().length() < 0)
						{
							return;
						}
						Intent intent = new Intent(Intent.ACTION_DIAL, Uri.parse("tel:" + szTel.trim()));
				        startActivity(intent);
					}
						break; 
					case ConstDefine.MSG_RECOGNIZE_QRCODE:
					{
						
						String qrcode=(String)msg.obj;
						Log.w("558",qrcode);
						if(qrcode != null)
						{
							showDialog();
							adapter.add("保存图中二维码");
							adapter.notifyDataSetChanged();
						}
						
					}
						break;
					case ConstDefine.MSG_WIFI_STATUS_STRONG:
					{
						m_nNetStatus = 3;
					}
						break;
					case ConstDefine.MSG_WIFI_STATUS_SOSO:
					{
						m_nNetStatus = 2;
					}
						break;
					case ConstDefine.MSG_WIFI_STATUS_WEAK:
					{
						m_nNetStatus = 1;
					}
						break;
					default:
						break;
				}
			}    		
    	};
    }
    
    //图片裁剪
    private void photoClip(Uri uri)
    {
    	Log.v("photo", "clip start");
    	Intent intent = new Intent("com.android.camera.action.CROP");
        intent.setDataAndType(uri, "image/*");
        intent.putExtra("crop", "true");
        intent.putExtra("aspectX", 1);
        intent.putExtra("aspectY", 1);
        intent.putExtra("outputX", 96);
        intent.putExtra("outputY", 96);
        intent.putExtra("return-data", true);
        startActivityForResult(intent, ConstDefine.RES_CLIPEIMG_END);
    }
    
    private void photoClipEnd(Bundle extras)
    {
    	Log.v("photo", "clip end");
    	if (null != extras)
    	{
    		Bitmap mBitmap = extras.getParcelable("data");
            try 
        	{
            	String imgName = "/@ci_" + this.getPackageName() + ".png";
            	String savePath = this.getFilesDir().getPath();
        		String path = savePath + imgName;
        		
    			File myCaptureFile = new File(savePath, imgName);
    			BufferedOutputStream bos = new BufferedOutputStream(
                                                 new FileOutputStream(myCaptureFile));
    			mBitmap.compress(Bitmap.CompressFormat.PNG, 100, bos);
    			bos.flush();
    			bos.close();
    			
    			sendMessageWithObj(ConstDefine.MSG_PICKIMG_END, path);
    		} 
        	catch (Exception e) 
        	{
    			e.printStackTrace();
    			Log.e("Head", "保存头像错误");
    		}
    	}
    }
    
    //图片选择
    private void photoPickEnd(Uri uri)
    {
    	String[] proj = {MediaStore.Images.Media.DATA};
        Cursor cursor = managedQuery(uri, proj, null, null, null); 
        int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        cursor.moveToFirst();
        String path = cursor.getString(column_index);        
    	Log.i("path", path);
    	
		toLuaFunC(m_nPickImgCallFunC, path);
		m_nPickImgCallFunC = -1;
    }
    
    // 通讯录选择
    private void contactPickEnd(Uri uri)
    {
    	String phoneNick = "";
    	String phoneNum = "";
    	//得到ContentResolver对象
    	ContentResolver cr = getContentResolver();
    	//取得电话本中开始一项的光标
    	Cursor cursor=cr.query(uri,null,null,null,null);
    	if(cursor!=null)
    	{
	    	cursor.moveToFirst();
	    	//取得联系人姓名
	    	int nameFieldColumnIndex = cursor.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME);
	    	phoneNick = cursor.getString(nameFieldColumnIndex);
	    	//取得电话号码
	    	String ContactId = cursor.getString(cursor.getColumnIndex(ContactsContract.Contacts._ID));
	    	Cursor phone = cr.query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, null,
	    	ContactsContract.CommonDataKinds.Phone.CONTACT_ID + "=" + ContactId, null, null);
	    	try 
	    	{
	    		if(phone != null && phone.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER) > 0)
		    	{
		    		phone.moveToFirst();
		    		phoneNum = phone.getString(phone.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER));
		    	}
			} 
	    	catch (Exception e) 
			{
				e.printStackTrace();
			}	    	
	    	phone.close();
	    	cursor.close();
    	}
    	JSONObject backJson = new JSONObject();
    	String backMsg = "";
    	try 
		{
			backJson.put("contactName", phoneNick);
			backJson.put("contactNumber", phoneNum);
			backMsg = backJson.toString();
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
    	Log.i("Contact", backMsg);
    	toLuaFunC(m_nContactFunC, backMsg);
    	m_nContactFunC = -1;
    }
    
    private void initLoginListener()
    {
    	m_LoginListener = new ThirdParty.OnLoginListener() 
    	{		
    		@Override
			public void onLoginStart(PLATFORM plat, String msg) 
			{
				toLuaToast("登陆开始" + msg);
			}
    		
    		@Override
			public void onLoginCancel(PLATFORM plat, String msg) 
			{
    			toLuaToast("登陆取消 ==> " + msg);
    			toLuaFunC(m_nThirdLoginFunC, "");
    			m_nThirdLoginFunC = -1;
			}
    		
			@Override
			public void onLoginSuccess(PLATFORM plat, String msg) 
			{
				//toLuaToast("登陆成功");
				toLuaFunC(m_nThirdLoginFunC, msg);
				m_nThirdLoginFunC = -1;
			}
			
			@Override
			public void onLoginFail(PLATFORM plat, String msg) 
			{
				toLuaToast("登陆失败 ==> " + msg);
				toLuaFunC(m_nThirdLoginFunC, "");
				m_nThirdLoginFunC = -1;
			}
		};
    }
    
    private void initShareListener()
    {
    	m_ShareListener = new ThirdParty.OnShareListener() 
    	{				
			@Override
			public void onComplete(PLATFORM plat, int eCode,String msg) 
			{
				toLuaFunC(m_nShareFunC, "true");
				m_nShareFunC = -1;
			}

			@Override
			public void onError(PLATFORM plat, String msg) 
			{
				toLuaToast("分享错误 ==> " + msg);
				toLuaFunC(m_nShareFunC, "false");
				m_nShareFunC = -1;
			}

			@Override
			public void onCancel(PLATFORM plat) 
			{
				toLuaToast("分享取消 ==> " + plat);
				toLuaFunC(m_nShareFunC, "false");
				m_nShareFunC = -1;
			}
		};
    }
    
    private void initPayListener()
    {
    	m_PayListener = new ThirdParty.OnPayListener() 
    	{
			
			@Override
			public void onPaySuccess(PLATFORM plat, String msg) 
			{
				if ("" != msg)
				{
					toLuaToast("支付成功");
				}				
				toLuaFunC(m_nThirdPayCallFunC, "true");
				m_nThirdLoginFunC = -1;
			}
			
			@Override
			public void onPayFail(PLATFORM plat, String msg) 
			{
				toLuaToast("支付失败 ==> " + msg);
				toLuaFunC(m_nThirdPayCallFunC, "false");
				m_nThirdLoginFunC = -1;
			}

			@Override
			public void onPayNotify(PLATFORM plat, String msg) 
			{
				toLuaToast(msg);
			}

			@Override
			public void onGetPayList(boolean bOk, String msg) 
			{
				String str = msg;
				if (false == bOk)
				{
					str = "";
					toLuaToast(msg);
				}
				toLuaFunC(m_nPayListFunC, str);
				m_nPayListFunC = -1;				
			}
		};
    }
    
    private void initLocationListener()
    {		
		/**
		 * 定位监听
		 */
    	m_LocationListener = new ThirdParty.OnLocationListener() 
    	{			
			@Override
			public void onLocationResult(boolean bSuccess, int errorCode, String backMsg) 
			{
				String msg = backMsg;
				if (false == bSuccess)
				{
					msg = "";
					toLuaToast(errorCode + ";" + backMsg);
				}
				toLuaFunC(m_nLocationFunC, msg);
				m_nLocationFunC = -1;
			}
		};
    }
    
    private void initDelAuthorListener()
    {
    	m_DeleAuthorListener = new ThirdParty.OnDelAuthorListener() {
			
			@Override
			public void onDeleteResult(boolean bSuccess, int errorCode, String backMsg) {
				String str = "false";
				if (bSuccess) {
					str = "true";
				}
				toLuaFunC(m_nDelAuthorizationFunC, str);
				m_nDelAuthorizationFunC = -1;	
			}
		};
    }
    
    // 隐藏虚拟导航
    private void hideNavigationBar(){
    	int uiFlags = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
    			| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
    			| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
    			| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
    			| View.SYSTEM_UI_FLAG_FULLSCREEN; // hide status bar
    	if( android.os.Build.VERSION.SDK_INT >= 19 ){ 
    		uiFlags |= 0x00001000;
    	}else {
    		uiFlags |= View.SYSTEM_UI_FLAG_LOW_PROFILE;
		}
    	this.getWindow().getDecorView().setSystemUiVisibility(uiFlags);
    }
    
    //Java to C++
    private static native boolean nativeIsLandScape();
    private static native boolean nativeIsDebug();
    public void toLuaFunC(final int funC, final String msg)
    {
    	if (-1 != funC && null != instance)
		{
    		instance.runOnGLThread(new Runnable() 
			{				
				@Override
				public void run() 
				{
					Cocos2dxLuaJavaBridge.callLuaFunctionWithString(funC,
							msg);
	                Cocos2dxLuaJavaBridge.releaseLuaFunction(funC);
				}
			});
		}
    }
    
    public void toLuaGlobalFunC(final String funName, final String msg)
    {
    	instance.runOnGLThread(new Runnable() 
		{				
			@Override
			public void run() 
			{
				Cocos2dxLuaJavaBridge.callLuaGlobalFunctionWithString(funName, msg);
			}
		});
    }
    
    public static native void nativeLogData(String msg);
    
    private void toLuaToast( String szMsg )
    {
    	// 延迟执行(尝试避免可能出现的黑条)    	
    	Message msgMessage = Message.obtain();
		msgMessage.what = ConstDefine.MSG_NATIVE_TOAST;
		msgMessage.obj = (Object)szMsg;
		m_hHandler.sendMessageDelayed(msgMessage, 500);
    }
    
    //Lua/C++ to Java
    //////////////////////////////////////////////////////////////////////////////////////
	/** UUID **/
	public static String getUUID() 
	{
		return Utils.getUUID(instance);
	}
	
	/** ipadress **/
	public static String getHostAdress()
	{
		return Utils.getHostIpAddress(instance);
	}
	
	public static String getSDCardDocPath()
	{
		Log.i("tag", Utils.getSDCardDocPath(instance));
		return Utils.getSDCardDocPath(instance);
	}
	
	public static int getBatteryLevel()
	{
		return m_nBatteryLevel;
	}
	
	public static int getWIFIStatus()
	{
		
		return m_nNetStatus;
	}

	public static int getNetType()
	{
		
		return m_nNetType;
	}

	//选取头像
	public static void pickImg(final int luaFunc, final boolean needChip)
	{
		instance.m_nPickImgCallFunC = luaFunc;
		if (needChip)
		{
			instance.sendMessage(ConstDefine.MSG_START_PICKIMG);
		}
		else 
		{
			instance.sendMessage(ConstDefine.MSG_START_PICKIMG_NOCLIP);
		}		
	}
    
    //分享配置
    public static void socialShareConfig(String title, String content, String Url)
    {
    	//默认分享icon
    	ThirdDefine.ShareTitle = title;
    	ThirdDefine.ShareContent = content;
    	ThirdDefine.ShareURL = Url;
    	instance.sendMessage(ConstDefine.MSG_SHARE_CONFIG);
    }
    
    //第三方平台配置
    public static void thirdPartyConfig(final int thridparty, final String configstr)
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_CONFIG_PARTY;
    	msgMessage.arg1 = thridparty;    	
    	msgMessage.obj = configstr;
    	
    	instance.sendMessageWith(msgMessage);
    }
    
    //第三方支付
    public static void thirdPartyPay(final int thridparty, final String payparam, final int luaFunc)
    {    	
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_THIRD_PAY;
    	msgMessage.arg1 = thridparty;    	
    	msgMessage.obj = payparam;
    	
    	instance.m_nThirdPayCallFunC = luaFunc;    	
    	instance.sendMessageWith(msgMessage);
    }
    
    //第三方登录
    public static void thirdLogin(final int thridparty,final int luaFunc)
	{
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_THIRD_LOGIN;
    	msgMessage.arg1 = thridparty;
    	
    	instance.m_nThirdLoginFunC = luaFunc;    	
    	instance.sendMessageWith(msgMessage);
	}
    
    //分享
    public static void startShare(final int luaFunc)
    {
    	instance.m_nShareFunC = luaFunc;
    	instance.sendMessage(ConstDefine.MSG_SOCIAL_SHARE);
    }
    
    //自定义分享
    public static void customShare(String title, String content, String url, String mediaPath, String imageOnly,final int luaFunc)
    {    	
    	ThirdDefine.ShareParam param = new ThirdDefine.ShareParam();
    	param.sTitle = title;
    	param.sContent = content;
    	param.sTargetURL = url;
    	param.sMedia = mediaPath;
    	if (imageOnly.equals("true"))
    	{
    		param.bImageOnly = true;
    	}
    	
    	instance.m_nShareFunC = luaFunc;
    	instance.sendMessageWithObj(ConstDefine.MSG_SOCIAL_CUSCHARE, param);
    }
    
    // 分享到指定平台
    public static void shareToTarget(final int target, String title, String content, String url, String mediaPath, String imageOnly,final int luaFunc)
    {
    	ThirdDefine.ShareParam param = new ThirdDefine.ShareParam();
    	param.nTarget = target;
    	param.sTitle = title;
    	param.sContent = content;
    	param.sTargetURL = url;
    	param.sMedia = mediaPath;
    	if (imageOnly.equals("true"))
    	{
    		param.bImageOnly = true;
    	}
    	
    	instance.m_nShareFunC = luaFunc;
    	instance.sendMessageWithObj(ConstDefine.MSG_SOCIAL_TARGETSHARE, param);
    }
    
    //install apk
    public static void installClient(String apkPath)
    {    	
    	if(!"".equals(apkPath))
    	{
    		File apkFile = new File(apkPath);
    		if (null != apkFile && apkFile.exists()) 
    		{
    			Intent installIntent = new Intent(Intent.ACTION_VIEW);
    			installIntent.setDataAndType(Uri.fromFile(apkFile), "application/vnd.android.package-archive");
    			instance.startActivity(installIntent);
			}
    	}
    }
    
    //获取竣付通支付列表
    public static void getPayList(String token, int luaFunc)
    {
    	instance.m_nPayListFunC = luaFunc;
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_JFT_PAYLIST;
    	msgMessage.obj = token;
    	instance.sendMessageWith(msgMessage);
    }
    
    //获取第三方应用是否安装
    public static boolean isPlatformInstalled(final int thridparty)
    {
    	ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(thridparty);
    	return ThirdParty.getInstance().isPlatformInstalled(plat);
    }
    
    //保存图片到系统相册
    public static boolean saveImgToSystemGallery(final String path, final String filename)
    {
    	boolean bRes = false;
    	// 文件插入系统图库
    	try 
    	{
            MediaStore.Images.Media.insertImage(instance.getContentResolver(), path, filename, null);
            // 最后通知图库更新
        	instance.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + path)));
        	bRes = true;
        } 
    	catch (FileNotFoundException e) 
        {
            e.printStackTrace();
        }
    	return bRes;
    }
    
    // 判断是否有录音权限
    public static boolean isHaveRecordPermission()
    {
    	PackageManager pManager = instance.getPackageManager();
    	Log.i("Permission", "pstate ==> " + pManager.checkPermission("android.permission.RECORD_AUDIO", instance.getPackageName()));
    	return PackageManager.PERMISSION_GRANTED == pManager.checkPermission("android.permission.RECORD_AUDIO", instance.getPackageName());
    }
    
    // 调用录音服务(安卓会提示权限)
    public static void testRecordService()
    {
    	int minBufferSize = AudioRecord.getMinBufferSize(44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
    	AudioRecord aRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                minBufferSize * 2);
    	try {
        	Log.i("AppActivity testRecordService", "start test");
        	aRecord.startRecording();
    	}catch (IllegalStateException e) {
    		Log.i("AppActivity testRecordService", "illegal stop");
    		aRecord.release();
    		aRecord = null;
    	}finally {  
    		Log.i("AppActivity testRecordService", "stop test");
    		aRecord.stop();
    		aRecord.release();
    		aRecord = null;
    	}
    }
    
    public static void startRecord(String fileName)
    {
    	if(recorder == null)
    	{
    		recorder = new MP3Recorder(fileName, 44100);
    		recorder.init();
    	}
    		
    	recorder.start(instance);
    }    
    
    public static void stopRecord()
    {
    	if(recorder != null)
    	{
    		recorder.stop();
    	}	
    }
    
    public static void cancelRecord()
    {
    	if(recorder != null)
    	{
    		recorder.cancel();
    	}
    }
    
    // 请求单次定位
    public static void requestLocation(int luaFunc)
    {
    	instance.m_nLocationFunC = luaFunc;
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_LOCATION_REQ;
    	instance.sendMessageWith(msgMessage);
    }
    
    // 计算距离
    public static String metersBetweenLocation(String loParam)
    {
    	return ThirdParty.getInstance().metersBetweenLocation(loParam);
    }
    
    // 请求通讯录
    public static void requestContact(int luaFunc)
    {
    	instance.m_nContactFunC = luaFunc;
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_CONTACT_REQ;
    	instance.sendMessageWith(msgMessage);
    }
    
    // 启动浏览器
    public static void openBrowser( String url )
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_OPEN_BROWSER;
    	msgMessage.obj = url;
    	instance.sendMessageWith(msgMessage);
    }
    
    // 启动浏览器
    public static void browsePost( String url ,String param)
    {	
    		
    		Log.d("browsePost",url);
    		int a=1/0;
    		Message msgMessage = Message.obtain();
    		msgMessage.what = ConstDefine.MSG_OPEN_BROWSER_POST;
    		msgMessage.obj = url+";"+param;
    		instance.sendMessageWith(msgMessage);
    		
    		
    	// Message msgMessage = Message.obtain();
    	// msgMessage.what = ConstDefine.MSG_OPEN_BROWSER;
    	// msgMessage.obj = url;
    	// instance.sendMessageWith(msgMessage);
    }
    
    // 复制到剪贴板
    public static boolean copyToClipboard( String msg )
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_COPY_CLIPBOARD;
    	msgMessage.obj = msg;
    	instance.sendMessageWith(msgMessage);
    	return true;
    }
    
    //是否授权
    public static boolean isAuthorized(final int thridparty)
    {
    	ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(thridparty);
    	return ThirdParty.getInstance().isAuthorized(plat);
    }
    
    // 取消授权
    public static void deleteThirdPartyAuthorization(final int thridparty, int luaFunc)
    {
    	instance.m_nDelAuthorizationFunC = luaFunc;
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_DEL_AUTHOR;
    	msgMessage.arg1 = thridparty;
    	instance.sendMessageWith(msgMessage);
    }
    
    // 网络是否可用
    public static boolean isNetworkAvailable()
    {
    	return Utils.isNetworkAvailable(instance);
    }
    
    // 跳转设置
    public static void goToSetting()
    {
    	instance.startActivity(new Intent(Settings.ACTION_SETTINGS));
    }
    
    // 获取启动参数
    public static String getLaunchData()
    {
    	String szData = instance.m_szLaunchData;
    	instance.m_szLaunchData = "";
    	return szData;
    }
    
    // 获取剪切板数据
    public static void getCopyBoardData(int luaFunc)
    {
    	instance.m_nGetCopyDataFunC = luaFunc;
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_GET_COPYDATA;
    	instance.sendMessageWith(msgMessage);
    }
    
    // 获取包名
    public static String getClientPackageName()
    {
    	try {
    		return instance.getPackageName();
 		} catch (Exception e) {
 		}
    	return "";
    }
    
    // 获取设备token
    public static String getDeviceToken()
    {
    	return instance.m_szDeviceToken;
    }
    
    // 拨号
    public static void systemCall( String szTel )
    {
    	Message msgMessage = Message.obtain();
    	msgMessage.what = ConstDefine.MSG_SYSTEM_CALL;
    	msgMessage.obj = szTel;
    	instance.sendMessageWith(msgMessage);
    }
 	public static int getNetWorkType(Context context) {
        int mNetWorkType = -1;
        ConnectivityManager manager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = manager.getActiveNetworkInfo();
        if (networkInfo != null && networkInfo.isConnected()) {
            String type = networkInfo.getTypeName();
            if (type.equalsIgnoreCase("WIFI")) {
                mNetWorkType = ConstDefine.NETWORKTYPE_WIFI;
            } else if (type.equalsIgnoreCase("MOBILE")) {
            	mNetWorkType =  ConstDefine.NETWORKTYPE_4G;
            }
        } else {
            mNetWorkType = ConstDefine.NETWORKTYPE_NONE;//没有网络
        }
        m_nNetType = mNetWorkType;
        return mNetWorkType;
    }
    

    private class PhoneStatListener extends PhoneStateListener 
    {
        //获取信号强度

        @Override
        public void onSignalStrengthsChanged(SignalStrength signalStrength)
        {
            super.onSignalStrengthsChanged(signalStrength);
            //获取网络信号强度
   
            String signalInfo = signalStrength.toString();  
            String[] params = signalInfo.split(" ");  

            
            if(getNetWorkType(instance) ==  ConstDefine.NETWORKTYPE_4G)
            {  
            //4G网络 最佳范围   >-90dBm 越大越好  
                int Itedbm = Integer.parseInt(params[9]);  
                if(Itedbm > -90)
                {
                    Message msg = new Message();  
                    msg.what = ConstDefine.MSG_WIFI_STATUS_STRONG;  
                    m_hHandler.sendMessage(msg); 

                }else if(Itedbm<=-90 && Itedbm>-120)
                {
                	 Message msg = new Message();  
                     msg.what = ConstDefine.MSG_WIFI_STATUS_SOSO;  
                     m_hHandler.sendMessage(msg); 
                	
                }else
                {
                	 Message msg = new Message();  
                     msg.what = ConstDefine.MSG_WIFI_STATUS_WEAK;  
                     m_hHandler.sendMessage(msg); 
                	
                }
                
            }
        }
    }
    public static Bitmap getBitmap(String sUrl){  
        try {  
        	int index=sUrl.indexOf(',');
        	String s=sUrl.substring(index+1);
        	byte[] imageAsBytes = Base64.decode(s.getBytes(), Base64.DEFAULT);
        	Bitmap bitmap = BitmapFactory.decodeByteArray(imageAsBytes, 0, imageAsBytes.length);
        	return bitmap;
        	      
            // Null config and displays OK for some files, 
            // Fails decodeByteArray for the file matching the one used as a resource and some others
        	
        	
            // URL url = new URL(sUrl);  
            // HttpURLConnection conn = (HttpURLConnection)url.openConnection();  
            // conn.setConnectTimeout(5000);  
            // conn.setRequestMethod("GET");  
            // if(conn.getResponseCode() == 200){  
            //     InputStream inputStream = conn.getInputStream();  
            //     Bitmap bitmap = BitmapFactory.decodeStream(inputStream);  
            //     return bitmap;  
            // }  
        } catch (Exception e) {  
            e.printStackTrace();  
        }  
        return null;  
    }  
    
    public  String handleQRCodeFormBitmap(Bitmap bitmap)
    {
    	if (bitmap == null)
        {
            return null;
        }
    	saveMyBitmap(bitmap,"code");
        int width = bitmap.getWidth(), height = bitmap.getHeight();
        int[] pixels = new int[width * height];
        bitmap.getPixels(pixels, 0, width, 0, 0, width, height);
        bitmap.recycle();
        bitmap = null;
        RGBLuminanceSource source = new RGBLuminanceSource(width, height, pixels);
        BinaryBitmap bBitmap = new BinaryBitmap(new HybridBinarizer(source));
        MultiFormatReader reader = new MultiFormatReader();
        try
        {
            Result result = reader.decode(bBitmap);
            String content=result.getText();
            return content;
        }
        catch (NotFoundException e)
        {
            return null;
        }
    	
    }
    
    public  String decodeImage(String imgUrl)
    {
    	return handleQRCodeFormBitmap(getBitmap(imgUrl));
    }

	@Override
	public void onLongClickCallBack(final String imgUrl) {
		Log.d("app onLongClickCallBack",imgUrl);
		// TODO Auto-generated method stub
		 new Thread(){
	            public void run() {
	                String content=decodeImage(imgUrl);
	                Log.w("1303",content);
	                sendMessageWithObj(ConstDefine.MSG_RECOGNIZE_QRCODE,(Object)content);
	            };
	        }.start();

	       
	}
	
	public abstract class CustomDialog extends Dialog {

	    private Context context;
	    /**
	     * 构造器
	     * @param context 上下文
	     * @param layoutId 资源文件id
	     */
	    public CustomDialog(Context context, int layoutId) {
	        super(context, R.style.CustomDialog);
	        this.context = context;
	        createDialog(layoutId);
	    }

	    /**
	     * 设置dialog
	     * @param layoutId
	     */
	    public  void createDialog(int layoutId){
	        setContentView(layoutId);
	        Window window = getWindow();
	        WindowManager.LayoutParams params = window.getAttributes();
	        params.width = WindowManager.LayoutParams.WRAP_CONTENT;
	        params.height = WindowManager.LayoutParams.WRAP_CONTENT;
	        params.gravity = Gravity.CENTER;
	        window.setAttributes(params);
	        initViews();
	        if(!(context instanceof Activity)){
	            getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
	        }
	    }

	    public void closeDialog(){
	        dismiss();
	    }
	    /**
	     * 用于初始化相应的控件
	     */
	    public abstract void initViews();

	}
	
	public  void saveMyBitmap(Bitmap mBitmap,String bitName)  {
		qrFile= new File( Environment.getExternalStorageDirectory()+"/"+bitName + ".jpg");
		FileOutputStream fOut = null;
		try {
			fOut = new FileOutputStream(qrFile);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		mBitmap.compress(Bitmap.CompressFormat.JPEG, 100, fOut);
		try {
			fOut.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
		try {
			fOut.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		Log.i("saveMyBitmap","saveMyBitmap");
		//return file;
	}
	/**
	 * 先保存到本地再广播到图库
	 * */
	public  void saveImageToGallery(Context context) {
		Log.i("saveImageToGallery","saveImageToGallery");
		// 其次把文件插入到系统图库
		try {
			MediaStore.Images.Media.insertImage(context.getContentResolver(), qrFile.getAbsolutePath(), "code", null);
			// 最后通知图库更新
			context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://"
					+ qrFile)));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	
	
	private void showDialog() {

        adapter = new ArrayAdapter<String>(this,R.layout.item_dialog);  
       // adapter.add("保存二维码");
//        adapter.add("发送给朋友");  
//        adapter.add("保存到手机");  
//        adapter.add("收藏");
        final AppActivity cont=(AppActivity) this;
        CustomDialog mCustomDialog = new CustomDialog(this, R.layout.custom_dialog) {
        	
            @Override
            public void initViews() {
                // 初始CustomDialog化控件
                ListView mListView = (ListView) findViewById(R.id.lv_dialog);
                mListView.setAdapter(adapter); 
                mListView.setOnItemClickListener(new OnItemClickListener() {

                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        // 点击事件
                        switch (position) {
                        case 0:
                        	cont.saveImageToGallery(cont);
                            closeDialog();
                            break;
                       
                        }

                    }
                });
            }
        };
        mCustomDialog.show();
    }

}
