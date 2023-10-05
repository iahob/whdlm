package org.cocos2dx.thirdparty;

import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.cocos2dx.thirdparty.ThirdDefine;
import org.cocos2dx.thirdparty.ThirdDefine.ShareParam;
import org.cocos2dx.thirdparty.alipay.PayResult;
import org.cocos2dx.thirdparty.alipay.ZhifubaoPay;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.amap.api.location.AMapLocation;
import com.amap.api.location.AMapLocationClient;
import com.amap.api.location.AMapLocationClientOption;
import com.amap.api.location.AMapLocationListener;
import com.amap.api.location.AMapLocationClientOption.AMapLocationMode;
import com.amap.api.maps2d.AMapUtils;
import com.amap.api.maps2d.model.LatLng;
import com.tencent.mm.sdk.constants.Build;
import com.tencent.mm.sdk.modelpay.PayReq;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
import com.umeng.socialize.PlatformConfig;
import com.umeng.socialize.ShareAction;
import com.umeng.socialize.UMAuthListener;
import com.umeng.socialize.UMShareAPI;
import com.umeng.socialize.UMShareListener;
import com.umeng.socialize.bean.SHARE_MEDIA;
import com.umeng.socialize.media.UMImage;
import com.umeng.socialize.media.UMWeb;

import sdk.pay.PayExceptionType;
import sdk.pay.PayMD5Util;
import sdk.pay.PayTypeModel;
import sdk.pay.PayUtil;
import sdk.pay.PayUtilCallBack;
import sdk.pay.TokenParam;

import com.zhonglongyule.fangka.R;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

public class ThirdParty implements PayUtilCallBack
{	
	public enum PLATFORM
	{
		INVALIDPLAT(-1),
		WECHAT(0),
		WECHAT_CIRCLE(1),
		ALIPAY(2),
		JFT(3),
		AMAP(4),
		IAP(5),
		SMS(6);
		
		private int nNum = -1;
		private PLATFORM(int n)
		{
			nNum = n;
		}
		public int toNumber()
		{
			return this.nNum;
		}
	}
	//登陆监听
	public static interface OnLoginListener
	{
		public void onLoginStart(PLATFORM plat, String msg);
		
		public void onLoginSuccess(PLATFORM plat, String msg);
		
		public void onLoginFail(PLATFORM plat, String msg);
		
		public void onLoginCancel(PLATFORM plat, String msg);
	}
	
	//分享监听
	public static interface OnShareListener
	{		
		public void onComplete(PLATFORM plat, int eCode, String msg);
		
		public void onError(PLATFORM plat, String msg);
		
		public void onCancel(PLATFORM plat);
	}
	
	//支付监听
	public static interface OnPayListener
	{
		public void onPaySuccess(PLATFORM plat, String msg);
		
		public void onPayFail(PLATFORM plat, String msg);
		
		public void onPayNotify(PLATFORM plat, String msg);
		
		public void onGetPayList(boolean bOk, String msg);
	}
	
	// 定位监听
	public static interface OnLocationListener
	{
		public void onLocationResult(boolean bSuccess, int errorCode, String backMsg);
	}
	
	// 授权删除监听
	public static interface OnDelAuthorListener
	{
		public void onDeleteResult(boolean bSuccess, int errorCode, String backMsg);
	}
	
	private static ThirdParty m_tInstance = new ThirdParty();
	private Activity m_Context = null;
	//友盟
    private UMShareAPI mShareAPI = null;
    //第三方平台列表
    private List<PLATFORM> m_ThridPlatList = null;
    //友盟第三方平台列表
    private Map<PLATFORM, SHARE_MEDIA> m_UMPartyList = null;
    //支付回调
    private OnPayListener m_OnPayListener = null;
    private PLATFORM m_enPayPlatform = PLATFORM.INVALIDPLAT;
    //支付宝
    private ZhifubaoPay m_AliPay = null;
    //骏付通
    private PayUtil m_PayUtil = null; 
	
    // 高德
    private AMapLocationClient locationClient = null;
	private AMapLocationClientOption locationOption = new AMapLocationClientOption();
	// 定位监听
    private AMapLocationListener locationListener = null;
    // 定位回调
    private OnLocationListener m_LocationListener = null;
	
	public static ThirdParty getInstance()
	{
		return m_tInstance;
	}
	
	public static void destroy()
	{
		if (null != m_tInstance.locationClient)
		{
			m_tInstance.locationClient.onDestroy();
			m_tInstance.locationClient = null;
		}
		
		if (null != m_tInstance.m_PayUtil)
		{
			m_tInstance.m_PayUtil.destroy();
			m_tInstance.m_PayUtil = null;
		}
	}
	
	public void init(Activity context)
	{
		m_Context = context;
		mShareAPI = UMShareAPI.get(m_Context);
			
		//第三方平台
		m_ThridPlatList = new ArrayList<ThirdParty.PLATFORM>();
		m_ThridPlatList.add(0,ThirdParty.PLATFORM.WECHAT);
		m_ThridPlatList.add(1,ThirdParty.PLATFORM.WECHAT_CIRCLE);
		m_ThridPlatList.add(2,ThirdParty.PLATFORM.ALIPAY);
		m_ThridPlatList.add(3, ThirdParty.PLATFORM.JFT);
		m_ThridPlatList.add(4, ThirdParty.PLATFORM.AMAP);
		m_ThridPlatList.add(5, ThirdParty.PLATFORM.IAP);
		m_ThridPlatList.add(6, ThirdParty.PLATFORM.SMS);
		
		//添加友盟平台
		m_UMPartyList = new HashMap<ThirdParty.PLATFORM, SHARE_MEDIA>();
		m_UMPartyList.put(ThirdParty.PLATFORM.WECHAT, SHARE_MEDIA.WEIXIN);
		m_UMPartyList.put(ThirdParty.PLATFORM.WECHAT_CIRCLE, SHARE_MEDIA.WEIXIN_CIRCLE);
		m_UMPartyList.put(ThirdParty.PLATFORM.ALIPAY, SHARE_MEDIA.ALIPAY);
		m_UMPartyList.put(ThirdParty.PLATFORM.SMS, SHARE_MEDIA.SMS);
	}
	
	public PLATFORM getPlatform(final int nPart)
	{
		//判断友盟平台
		int len = m_ThridPlatList.size();
		if (nPart < 0 || nPart >= len) 
		{
			return ThirdParty.PLATFORM.INVALIDPLAT;
		}
		return m_ThridPlatList.get(nPart);
	}
	
	public PLATFORM getPlatformFrom(SHARE_MEDIA mdia)
	{
		//判断友盟平台
		Set<PLATFORM> ptSet= m_UMPartyList.keySet();
		for (PLATFORM pt: ptSet)
		{
			if (m_UMPartyList.get(pt) == mdia)
			{
				return pt;
			}
		}
		return PLATFORM.INVALIDPLAT;
	}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (null != mShareAPI)
		{
			mShareAPI.onActivityResult(requestCode, resultCode, data);
		}
	}
	
	public void onPayResult(boolean bOk, String msg)
	{
		if (null != m_OnPayListener)
		{
			if (bOk)
			{
				m_OnPayListener.onPaySuccess(m_enPayPlatform, msg);
			}
			else 
			{
				m_OnPayListener.onPayFail(m_enPayPlatform, msg);
			}
		}
		m_OnPayListener = null;
	}
	
	public void onPayNotify(String msg)
	{
		if (null != m_OnPayListener)
		{
			m_OnPayListener.onPayNotify(m_enPayPlatform, msg);
		}
	}
	
	
	
	@Override
	public void onGetTokenResult() {
		if (null != m_PayUtil){
			m_PayUtil.getPayType();
		}
	}

	@Override
	public void onPayDataResult() {
		if (null == m_OnPayListener || null == m_PayUtil){
			Log.e("JFT", "监听异常");
			return;
		}
		
		List<PayTypeModel> payTypeModelList = m_PayUtil.getPayTypeModels();
		if (payTypeModelList.size() > 0) {
			Log.v("获取通道成功", "竣付通支付");
			JSONArray jArray = new JSONArray();
			ArrayList<PayTypeModel> arrayList  =  (ArrayList<PayTypeModel>) payTypeModelList;
			for (int idx = 0; idx < arrayList.size(); ++idx)
			{
				jArray.put(arrayList.get(idx).getTypeid());
			}
			String jsonStr = jArray.toString();
			m_OnPayListener.onGetPayList(true, jsonStr);
        } 
		else {
            Log.v("未获取到支付通道", "竣付通支付");
            m_OnPayListener.onGetPayList(false, "未获取到支付通道");
        }
	}

	@Override
	public void onPayException(int exceptionType, String exceptionContent) {
		if (null == m_OnPayListener){
			Log.e("JFT", "Listener null");
			return;
		}
		
		if (null != exceptionContent) {
			m_OnPayListener.onPayNotify(PLATFORM.JFT, exceptionContent);
		} else {
			PayExceptionType type = PayExceptionType.values()[exceptionType];
			String str;
			switch (type) {
				case DATA_EXCEPTION: {
					str = m_Context.getString(R.string.data_exception);
					break;
				}
				case ENCRYPT_EXCEPTION: {
					str = m_Context.getString(R.string.encrypt_exception);
					break;
				}
				case GET_PAY_METHOD_FAILED: {
					str = m_Context.getString(R.string.get_pay_method_failed);
					break;
				}
				case DECRYPT_EXCEPTION: {
					str = m_Context.getString(R.string.decrypt_exception);
					break;
				}
				case RETURN_ERROR_DATA: {
					str = m_Context.getString(R.string.return_error_data);
					break;
				}
				case PAY_SYSTEM_ID_EMPTY: {
					str = m_Context.getString(R.string.pay_system_id_empty);
					break;
				}
				case SERVER_CONNECTION_EXCEPTION: {
					str = m_Context.getString(R.string.server_connection_exception);
					break;
				}
				case GET_PAY_STATUS_FAILED: {
					str = m_Context.getString(R.string.get_pay_status_failed);
					break;
				}
				case INVALID_TOKEN: {
					str = m_Context.getString(R.string.invalid_token);
					break;
				}
				case GET_TOKEN_FAILURE: {
					str = m_Context.getString(R.string.get_token_failed);
					break;
				}
				case GET_SERVERS_FAILURE: {
					str = m_Context.getString(R.string.get_servers_failure);
					break;
				}
				case NET_WORK_NOT_AVAILABLE: {
					str = m_Context.getString(R.string.net_work_not_available);
					break;
				}
				default: {
					str = "";
					break;
				}
			}
			m_OnPayListener.onPayNotify(PLATFORM.JFT, str);
		}
	}

	@Override
	public void onPayStatus(int arg0) {
	}

	public void configThirdParty(PLATFORM plat, String configstr)
	{
		switch (plat) 
		{
		case WECHAT:
			doConfigWeChat(configstr);
			break;
		case ALIPAY:
			doConfigAlipay(configstr);
			break;
		case JFT:
			doConfigJFT(configstr);
			break;
		case AMAP:
			doConfigAMAP(configstr);
			break;
		default:
			break;
		}
	}
	
	public void configSocialShare()
	{
		if (null == mShareAPI)
		{
			return;
		}
	}
	
	public void thirdPartyLogin(PLATFORM plat, OnLoginListener listener)
	{
		//判断友盟
		if (m_UMPartyList.containsKey(plat))
		{
			SHARE_MEDIA mdia = m_UMPartyList.get(plat);			
			if (mdia == SHARE_MEDIA.WEIXIN)
			{
				doWeChatLogin(listener);
			}
		}
	}
	
	public void deleteThirdPartyAuthorization(PLATFORM plat, final OnDelAuthorListener listener)
	{
		if (m_UMPartyList.containsKey(plat))
		{
			SHARE_MEDIA mdia = m_UMPartyList.get(plat);	
			
			mShareAPI.deleteOauth(m_Context, mdia, new UMAuthListener()
			{
				@Override
				public void onCancel(SHARE_MEDIA arg0, int arg1) 
				{
					listener.onDeleteResult(false, 2, "");
				}

				@Override
				public void onComplete(SHARE_MEDIA arg0, int arg1,
						Map<String, String> arg2) 
				{
					listener.onDeleteResult(true, 1, "");
				}

				@Override
				public void onError(SHARE_MEDIA arg0, int arg1, Throwable arg2) 
				{
					listener.onDeleteResult(false, 0, "");
				}

				@Override
				public void onStart(SHARE_MEDIA arg0) {
					
				}
			});
		}
		else 
		{
			listener.onDeleteResult(true, 2, "");
		}
	}
	
	public void openShare(final OnShareListener listener, ShareParam param)
	{
		if (null == m_Context)
		{
			listener.onError(m_enPayPlatform, "init error");
			return;
		}
		final SHARE_MEDIA[] displaylist = new SHARE_MEDIA[]
                {
                    SHARE_MEDIA.WEIXIN, SHARE_MEDIA.WEIXIN_CIRCLE
                };
		ShareAction sAct = newShareAction(param, false);
		sAct.setDisplayList(displaylist).setCallback(newShareListener(listener)).open();
	}
	
	public void targetShare(final OnShareListener listener, ShareParam param)
	{
		ThirdParty.PLATFORM plat = ThirdParty.getInstance().getPlatform(param.nTarget);
		if (m_UMPartyList.containsKey(plat))
		{
			UMImage img = UMAsset.getUmImage(m_Context, param.sMedia);		
			if (null == img)
			{
				img = new UMImage(m_Context, R.drawable.icon);
			}
			SHARE_MEDIA mdia = m_UMPartyList.get(plat);	
			ShareAction sAct = newShareAction(param, mdia == SHARE_MEDIA.WEIXIN_CIRCLE);
			sAct.setPlatform(mdia).setCallback(newShareListener(listener)).share();
		}
		else 
		{
			listener.onError(plat, "do not support target");
		}
	}
	
	public void thirdPartyPay(PLATFORM plat, String payparam, final OnPayListener listener)
	{
		m_enPayPlatform = plat;
		//解析支付参数
		try 
		{
			JSONObject jObject = new JSONObject(payparam);			
			m_OnPayListener = listener;	
			
			//判断平台
			switch (plat) 
			{
			case WECHAT:
				JSONObject infoObject = jObject.getJSONObject("info");
				doWeChatPay(infoObject);
				break;
			case ALIPAY:
			{
				ThirdDefine.PayParam param = new ThirdDefine.PayParam();
				param.sOrderId = jObject.getString("orderid");
				param.fPrice = (float)jObject.getDouble("price");
				param.sProductName = jObject.getString("name");
				doAliPay(param);
			}				
				break;
			case JFT:
			{
				int nPayType = (int)jObject.getInt("paytype");
				doJtfPay(nPayType);
			}				
				break;
			default:
				break;
			}
			
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
			listener.onPayFail(m_enPayPlatform, "订单数据解析失败");
		}
	}
	
	public void getPayList(String sztokenparam, final OnPayListener listener)
	{
		if (null == listener)
		{
			return;
		}		
		if (false == ThirdDefine.bConfigJFT)
		{
			listener.onGetPayList(false, "竣付通配置异常");
			return;
		}
		m_OnPayListener = listener;
		if (null != m_PayUtil)
		{
			m_PayUtil.destroy();
		}
		
		try 
		{
			JSONObject jObject = new JSONObject(sztokenparam);
			String mon = jObject.getString("mon");
	    	String rurl = jObject.getString("rurl");
	    	String nurl = jObject.getString("nurl");
	    	String oid = jObject.getString("oid");
	    	TokenParam tokenParam = new TokenParam();
	    	Date date = new Date();
			DateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
			tokenParam.setP1_usercode(ThirdDefine.JFTPartnerID);
			tokenParam.setP2_order(oid);
			tokenParam.setP3_money(mon);
			tokenParam.setP4_returnurl(rurl);
			tokenParam.setP5_notifyurl(nurl);
			tokenParam.setP6_ordertime(dateFormat.format(date));
			tokenParam.setP14_customname("user");// user name
			tokenParam.setP25_terminal("3");
			tokenParam.setP7_sign(PayMD5Util.getMD5(
					ThirdDefine.JFTPartnerID + "&" + oid + "&"
							+ mon + "&" + rurl
							+ "&" + nurl + "&" + dateFormat.format(date)
							+ ThirdDefine.JFTPayKey).toUpperCase());
			
			m_PayUtil = new PayUtil(m_Context, ThirdDefine.JFTAppID, ThirdDefine.JFTKey, ThirdDefine.JFTVector, "jft",this, false);
	    	m_PayUtil.getToken(tokenParam);
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
			onPayResult(false, "竣付通数据解析异常");
		}
	}
	
	public boolean isPlatformInstalled(PLATFORM plat)
	{
		String packageName = "";
		if (plat == ThirdParty.PLATFORM.WECHAT)
		{
			packageName = "com.tencent.mm";
		}
		else if (plat == ThirdParty.PLATFORM.ALIPAY) 
		{
			packageName = "com.eg.android.AlipayGphone";
		}
		else 
		{
			return false;
		}
		android.content.pm.ApplicationInfo info = null;
        try 
        {
            info = m_Context.getPackageManager().getApplicationInfo(packageName, 0);
            return info != null;
        } 
        catch (NameNotFoundException e) 
        {
            return false;
        }
	}
	
	// 请求单次定位
	public void requestLocation(OnLocationListener listener)
	{
		m_LocationListener = listener;
		if (null != locationClient)
		{
			locationClient.onDestroy();
			locationClient = null;
		}
		// 初始化client
		locationClient = new AMapLocationClient(m_Context.getApplicationContext());
		if (null != locationClient && null != locationListener && null != locationOption)
		{
			// 设置定位参数
			locationClient.setLocationOption(locationOption);
			// 设置定位监听
			locationClient.setLocationListener(locationListener);
			// 定位请求
			locationClient.startLocation();
		}
		else
		{
			listener.onLocationResult(false, -1, "定位服务初始化失败!");
		}
	}
	
	// 停止定位
	public void stopLocation()
	{
		locationClient.stopLocation();
	}
	
	// 距离计算
	public String metersBetweenLocation(String loParam)
	{
		String msg = "0";
		try 
		{
			JSONObject jObject = new JSONObject(loParam);
			double myLatitude = jObject.getDouble("myLatitude");
			double myLongitude = jObject.getDouble("myLongitude");
			
			double otherLatitude = jObject.getDouble("otherLatitude");
			double otherLongitude = jObject.getDouble("otherLongitude");
			
			LatLng my2d = new LatLng(myLatitude, myLongitude);
			LatLng or2d = new LatLng(otherLatitude, otherLongitude);
			msg = String.valueOf(AMapUtils.calculateLineDistance(my2d, or2d));
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
		return msg;
	}
	
	// 是否授权
	public boolean isAuthorized( PLATFORM plat )
	{
		if (m_UMPartyList.containsKey(plat))
		{
			SHARE_MEDIA mdia = m_UMPartyList.get(plat);	
			return UMShareAPI.get(m_Context).isAuthorize(m_Context, mdia);
		}
		return false;
	}
	
	private void doConfigWeChat(String configstr)
	{
		try 
		{
			JSONObject jObject = new JSONObject(configstr);
			ThirdDefine.WeixinAppID = jObject.getString("AppID");
	    	ThirdDefine.WeixinAppSecret = jObject.getString("AppSecret");
	    	ThirdDefine.WeixinPartnerid = jObject.getString("PartnerID");
	    	ThirdDefine.WeixinPayKey = jObject.getString("PayKey");
	    	ThirdDefine.bConfigWeChat = true;
			
	    	PlatformConfig.setWeixin(ThirdDefine.WeixinAppID, ThirdDefine.WeixinAppSecret);
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
	}
	
	private void doConfigAlipay(String configstr)
	{
		try 
		{
			JSONObject jObject = new JSONObject(configstr);
	    	
	    	ThirdDefine.ZFBAPP = jObject.getString("AppID");
	    	ThirdDefine.ZFBPARTNER = jObject.getString("PartnerID");
	    	ThirdDefine.ZFBSELLER = jObject.getString("SellerID");
	    	ThirdDefine.ZFBNOTIFY_URL = jObject.getString("NotifyURL");
	    	ThirdDefine.ZFBRSA_PRIVATE = jObject.getString("RsaKey");
	    	ThirdDefine.bConfigAlipay = true;
	    	
	    	PlatformConfig.setAlipay(ThirdDefine.ZFBPARTNER);
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
	}
	
	private void doConfigJFT(String configstr)
	{
		try 
		{
			JSONObject jObject = new JSONObject(configstr);
	    	
			String appid = jObject.getString("JftAppID");
	    	String key = jObject.getString("JftAesKey");
	    	String vec = jObject.getString("JftAesVec");
	    	String partId = jObject.getString("PartnerID");
	    	String payKey = jObject.getString("PayKey");

			ThirdDefine.JFTKey = key;
			ThirdDefine.JFTVector = vec;
			ThirdDefine.JFTAppID = appid;	
			ThirdDefine.JFTPartnerID = partId;
			ThirdDefine.JFTPayKey = payKey;
	        ThirdDefine.bConfigJFT = true;
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}		
	}
	
	private void doConfigAMAP(String configstr) 
	{
		// 设置定位参数
		locationOption = new AMapLocationClientOption();
		//设置返回地址信息，默认为true
		locationOption.setNeedAddress(true);
		locationOption.setLocationMode(AMapLocationMode.Hight_Accuracy);//可选，设置定位模式，可选的模式有高精度、仅设备、仅网络。默认为高精度模式
		locationOption.setOnceLocation(true);//可选，设置是否单次定位。默认是false
				
		// 定位监听
		locationListener = new AMapLocationListener() 
		{
			@Override
			public void onLocationChanged(AMapLocation loc) 
			{
				boolean bRes = false;
				int errorCode = AMapLocation.ERROR_CODE_UNKNOWN;
				String backMsg = "";
				if (null != loc) 
				{
					//解析定位结果
					int errorCode1 = loc.getErrorCode();
					if (0 == loc.getErrorCode())
					{
						JSONObject jObject = new JSONObject();
						try 
						{
							bRes = true;
							jObject.put("berror", false);
							jObject.put("latitude", loc.getLatitude());
							jObject.put("longitude", loc.getLongitude());
							jObject.put("accuracy", loc.getAccuracy());
							jObject.put("address", loc.getAddress());
							jObject.put("country", loc.getCountry());
							jObject.put("province", loc.getProvince());
							jObject.put("city", loc.getCity());
							jObject.put("district", loc.getDistrict());
							jObject.put("street", loc.getStreet());
							jObject.put("streetNum", loc.getStreetNum());
							backMsg = jObject.toString();
						} 
						catch (JSONException e) 
						{
							backMsg = "定位数据解析异常!" + loc.getErrorInfo();
							e.printStackTrace();
						}
					}
					else 
					{
						JSONObject jObject = new JSONObject();
						try 
						{
							bRes = true;
							jObject.put("berror", true);
							jObject.put("msg", errorCode1 + ",定位失败! " + loc.getErrorInfo());
							backMsg = jObject.toString();
						} 
						catch (JSONException e) 
						{
							backMsg = "定位数据解析异常!" + loc.getErrorInfo();
							e.printStackTrace();
						}
						locationClient.stopLocation();
					}
				} 
				else 
				{
					backMsg = "定位数据异常!";
				}
				
				if ( null != m_LocationListener ) 
				{
					m_LocationListener.onLocationResult(bRes, errorCode, backMsg);
				}
				locationClient.stopLocation();
			}
		};
	}
	
	private void doWeChatLogin(final OnLoginListener listener)
	{
		if (false == mShareAPI.isInstall(m_Context, SHARE_MEDIA.WEIXIN))
		{
			listener.onLoginFail(PLATFORM.WECHAT, "微信客户端未安装,无法授权登陆");
			return;
		}
		
		if (null == m_Context || false == ThirdDefine.bConfigWeChat)
		{
			listener.onLoginFail(PLATFORM.WECHAT, "");
			return;
		}
		
		mShareAPI.doOauthVerify(m_Context, SHARE_MEDIA.WEIXIN, new UMAuthListener() 
		{
			@Override
			public void onCancel(SHARE_MEDIA arg0, int arg1) 
			{
				listener.onLoginCancel(PLATFORM.WECHAT, "");
			}

			@Override
			public void onComplete(SHARE_MEDIA arg0, int arg1,
					Map<String, String> arg2) 
			{
				//parseAuthorData(listener, PLATFORM.WECHAT, arg1, arg2);
				getPlatFormInfo(SHARE_MEDIA.WEIXIN, listener);
			}

			@Override
			public void onError(SHARE_MEDIA arg0, int arg1, Throwable arg2) 
			{
				listener.onLoginFail(PLATFORM.WECHAT, "");
			}

			@Override
			public void onStart(SHARE_MEDIA arg0) {
				// TODO Auto-generated method stub
				
			}
		});
	}
	
	private void doWeChatPay(final JSONObject info)
	{
		if (null == m_Context || false == ThirdDefine.bConfigWeChat)
		{
			onPayResult(false, "初始化失败");
			return;
		}
		
		IWXAPI msgApi = WXAPIFactory.createWXAPI(m_Context, ThirdDefine.WeixinAppID);
		msgApi.registerApp(ThirdDefine.WeixinAppID);
		if (msgApi.getWXAppSupportAPI() >= Build.PAY_SUPPORTED_SDK_INT)
		{			
    		try 
    		{
    			PayReq request = new PayReq();
        		request.appId = info.getString("appid");
        		request.partnerId = info.getString("partnerid");
				request.prepayId= info.getString("prepayid");
				request.packageValue = info.getString("package");
	    		request.nonceStr= info.getString("noncestr");
	    		request.timeStamp= info.getString("timestamp");
	    		request.sign=  info.getString("sign");
	    		msgApi.sendReq(request);
			} 
    		catch (JSONException e) 
			{
				e.printStackTrace();
				onPayResult(false, "订单数据解析异常");
			}    		
		}
		else
		{
			onPayResult(false, "未安装微信或微信版本过低");
		}
	}
	
	private void doAliPay(ThirdDefine.PayParam param)
	{
		if(null == m_Context || false == ThirdDefine.bConfigAlipay)
		{
			onPayResult(true, "初始化失败");
			return;
		}
		if (null == m_AliPay)
		{
			m_AliPay = new ZhifubaoPay(new Handler(){

				@Override
				public void handleMessage(Message msg) 
				{
					if (msg.what == ThirdDefine.ZFB_Pay)
					{
						// 新支付宝支付
						PayResult payResult = new PayResult((Map<String, String>) msg.obj);
						/**
						 对于支付结果，请商户依赖服务端的异步通知结果。同步通知结果，仅作为支付结束的通知。
						 */
						String resultInfo = payResult.getResult();// 同步返回需要验证的信息
						String resultStatus = payResult.getResultStatus();
						// 判断resultStatus 为9000则代表支付成功
						if (TextUtils.equals(resultStatus, "9000")) {
							// 该笔订单是否真实支付成功，需要依赖服务端的异步通知。
							// Toast.makeText(PayDemoActivity.this, "支付成功", Toast.LENGTH_SHORT).show();
							// onPayResult(true, payResult.getResult());
							onPayResult(true, "支付成功");
						} else {
							// Log.i("msp", "-------------支付失败----------------");
							// 该笔订单真实的支付结果，需要依赖服务端的异步通知。
							// Toast.makeText(PayDemoActivity.this, "支付失败", Toast.LENGTH_SHORT).show();
							onPayResult(false, "支付失败");
						}
					}
				}
				
			}, m_Context);
		}
		m_AliPay.setOrderNo(param.sOrderId);
		m_AliPay.pay(param.fPrice, param.sProductName);
	}
	
	private void doJtfPay(int nPayType)
	{
		if (null != m_PayUtil)
		{
			m_PayUtil.getPayParam(nPayType);
		}	
		onPayResult(true, "");
	}
	
	private void getPlatFormInfo(final SHARE_MEDIA mdia, final OnLoginListener listener)
	{
		final PLATFORM plat = getPlatformFrom(mdia);
		mShareAPI.getPlatformInfo(m_Context, mdia, new UMAuthListener() 
		{
			
			@Override
			public void onError(SHARE_MEDIA arg0, int arg1, Throwable arg2) 
			{
				listener.onLoginFail(plat, arg2.getMessage());
			}
			
			@Override
			public void onComplete(SHARE_MEDIA arg0, int arg1, Map<String, String> arg2) 
			{
				parseAuthorData(listener, plat, arg1, arg2);				
			}
			
			@Override
			public void onCancel(SHARE_MEDIA arg0, int arg1) 
			{
				listener.onLoginFail(plat, ""+arg1);
			}

			@Override
			public void onStart(SHARE_MEDIA arg0) {
				// TODO Auto-generated method stub
				
			}
		});		
	}
	
	private void parseAuthorData(final OnLoginListener listener, PLATFORM plat, int arg1, Map<String, String> arg2)
	{	    	
    	if(/*arg1 == 0 && */arg2 != null)
		{
    		//登陆成功                            	
        	JSONObject jObject = new JSONObject(arg2);
        	try 
        	{
    			jObject.put("valid", true);
    			jObject.put("um_code", arg1);
    			listener.onLoginSuccess(plat, jObject.toString());
    		} 
        	catch (JSONException e) 
    		{
    			listener.onLoginFail(plat, "");
    			e.printStackTrace();
    		} 
        }
		else
		{
			JSONObject jObject = new JSONObject();
			try 
			{
				jObject.put("valid", false);
				jObject.put("errorcode", arg1);
				listener.onLoginFail(plat, jObject.toString());
			} 
			catch (JSONException e) 
			{
				listener.onLoginFail(plat, "登陆发生错误："+arg1);
				e.printStackTrace();
			}				
        }
	}
	
	/*
	 * 
	 */
	private ShareAction newShareAction( ShareParam param, boolean bCycleShare )
	{
		UMImage img = UMAsset.getUmImage(m_Context, param.sMedia);		
		if (null == img)
		{
			img = new UMImage(m_Context, R.drawable.icon);
		}
		
		ShareAction sAct = new ShareAction(m_Context);
		if ("" != param.sContent && false == param.bImageOnly)
		{
			sAct.withText(param.sContent);
		}
		if ("" != param.sTitle && false == param.bImageOnly)
		{
			sAct.withSubject(param.sTitle);
		}
		if ("" != param.sTargetURL && false == param.bImageOnly)
		{
			UMWeb web = new UMWeb(param.sTargetURL);
			web.setDescription(param.sContent);
			// 朋友圈分享特殊处理
	        if (bCycleShare)
	        {
	        	web.setTitle(param.sContent);
	        }
	        else 
	        {
	        	web.setTitle(param.sTitle);
			}
	        web.setThumb(img);
	        sAct.withMedia(web);
		}
		if (true == param.bImageOnly)
		{
			sAct.withMedia(img);
		}
		return sAct;
	}
	
	private UMShareListener newShareListener(final OnShareListener listener)
	{
		return new UMShareListener() 
		{
			@Override
			public void onResult(SHARE_MEDIA arg0) 
			{
				PLATFORM pt = getPlatformFrom(arg0);
				if (pt != PLATFORM.INVALIDPLAT)
				{
					listener.onComplete(pt, 200, "");
				}
				else 
				{
					listener.onError(pt, "invalid platform " + pt.toString());
				}
			}

			@Override
			public void onError(SHARE_MEDIA arg0, Throwable arg1) 
			{
				PLATFORM pt = getPlatformFrom(arg0);
				listener.onError(pt, "invalid platform " + arg1.getMessage());
			}

			@Override
			public void onCancel(SHARE_MEDIA arg0) 
			{
				PLATFORM pt = getPlatformFrom(arg0);
				listener.onCancel(pt);
			}

			@Override
			public void onStart(SHARE_MEDIA arg0) {
				// TODO Auto-generated method stub
				
			}			
		};
	}
}
