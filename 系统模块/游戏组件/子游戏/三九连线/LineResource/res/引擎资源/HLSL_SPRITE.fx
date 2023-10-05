
// 纹理变量
sampler2D _samSpirit0;
sampler2D _samSpirit1;

// 乘积颜色
float4 _OverlayColor : COLOR;

// 纹理大小
float2 _ReliefTexSize : TEXCOORD;	

// 边界颜色
float4 _BorderColor : COLOR; 

// 渐变颜色
float4 _TopColor : COLOR; 
float4 _BottomColor : COLOR; 

// -----------------发亮----------------------
float4 PS_Shine ( float2 _Texture : TEXCOORD ) : COLOR
{
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0, _Texture );

	// 乘积像素
	float4 _outColor = _inColor * _OverlayColor;

    return _outColor;
}

technique TextureEffect_Shine
{
    pass p0 
    {	
	    SrcBlend = SRCALPHA;
    	DestBlend = ONE;
    	PixelShader = compile ps_2_0 PS_Shine();
    }
}

// -----------------灰度----------------------
float4 PS_Gray ( float2 _Texture : TEXCOORD ) : COLOR
{
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0, _Texture );

	// 灰度像素
	float _Gray = 0.3 * _inColor.r + 0.59 * _inColor.g + 0.11* _inColor.b;

	// 设置新像素
    float4 _outColor = float4(_Gray, _Gray, _Gray, _inColor.a);

	// 乘积像素
	_outColor = _outColor * _OverlayColor;
	
    return _outColor;
}

technique TextureEffect_Gray
{
    pass p0 
    {	
    	PixelShader = compile ps_2_0 PS_Gray();
    }
}

// -----------------浮雕----------------------
float4 PS_Relief ( float2 _Texture : TEXCOORD ) : COLOR
{
	// 获取信息
	float2  _upLeftUV = float2(_Texture.x - 1.0/_ReliefTexSize.x , _Texture.y - 1.0/_ReliefTexSize.y);
	float4  _bkColor = float4(0.5 , 0.5 , 0.5 , 1.0);
	float4  _curColor   = tex2D( _samSpirit0, _Texture );
	float4  _upLeftColor =  tex2D( _samSpirit0, _upLeftUV );

	// 相减得到颜色的差
	float4  _delColor = _curColor - _upLeftColor;

	// 需要把这个颜色的差设置
	float  _Gray = 0.3 * _delColor.r + 0.59 * _delColor.g + 0.11* _delColor.b;
	float4 _outColor =  float4(_Gray, _Gray, _Gray, 0.0) + _bkColor;
	return _outColor;
}

technique TextureEffect_Relief
{
    pass p0 
    {	
    	PixelShader = compile ps_2_0 PS_Relief();
    }
}

// -----------------描边绘画----------------------
float4 PS_Border( float2 _Texture : TEXCOORD0 ) : COLOR 
{ 
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// 输出像素
	float4 _outColor = _inColor;
	
	// 偏移度
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);
     
	// 空像素
	if( _inColor.a == 0 )
	{
		// 获取附近值信息
		float4 _Left = tex2D( _samSpirit0, _Texture.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( fX, fY ) ); 

		// 附近有值
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// 设置背景颜色
			_outColor = _BorderColor;
		}
	}
	else
	{     
		_outColor = _inColor * _OverlayColor;
	}

	return _outColor;
}

technique TextureEffect_Border
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_Border();
    }
}

// -----------------渐变绘画----------------------
float4 PS_Gradient( float2 _Texture : TEXCOORD ) : COLOR 
{ 
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// 输出像素
	float4 _outColor = _inColor;

	// 渐变颜色
	if( _inColor.a > 0 )
	{
		float _r = (_BottomColor.r - _TopColor.r)* ( _Texture.y / 1.0 ) + _TopColor.r;
		float _g = (_BottomColor.g - _TopColor.g)* ( _Texture.y / 1.0 ) + _TopColor.g;
		float _b = (_BottomColor.b - _TopColor.b)* ( _Texture.y / 1.0 ) + _TopColor.b;

		_outColor = float4( _r, _g, _b, _inColor.a ) * _OverlayColor;
	}

	// 返回颜色
	return _outColor;
}

technique TextureEffect_Gradient
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_Gradient();
    }
}

// -----------------渐变描边绘画----------------------
float4 PS_GradientBorder( float2 _Texture : TEXCOORD ) : COLOR 
{
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// 输出像素
	float4 _outColor = _inColor;

	// 偏移度
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);

	// 渐变颜色
	if( _inColor.a > 0 )
	{
		float _r = (_BottomColor.r - _TopColor.r)* ( _Texture.y / 1.0 ) + _TopColor.r;
		float _g = (_BottomColor.g - _TopColor.g)* ( _Texture.y / 1.0 ) + _TopColor.g;
		float _b = (_BottomColor.b - _TopColor.b)* ( _Texture.y / 1.0 ) + _TopColor.b;

		_outColor = float4( _r, _g, _b, _inColor.a ) * _OverlayColor;
	}
	// 描边颜色
	else
	{
		// 获取附近值信息
		float4 _Left = tex2D( _samSpirit0, _Texture.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( fX, fY ) ); 

		// 附近有值
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// 设置背景颜色
			_outColor = _BorderColor;
		}
	}

	// 返回颜色
	return _outColor;
}

technique TextureEffect_GradientBorder
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_GradientBorder();
    }
}

// -----------------纹理绘画----------------------
float4 PS_Texture( float2 _Texture0 : TEXCOORD0, float2 _Texture1 : TEXCOORD1 ) : COLOR 
{
	// 获取像素
	float4 _inColor0 = tex2D( _samSpirit0 , _Texture0.xy);
	float4 _inColor1 = tex2D( _samSpirit1 , _Texture1.xy);

	// 输出像素
	float4 _outColor = _inColor0 * _inColor1 * _OverlayColor;
	
	// 返回颜色
	return _outColor;
}

technique TextureEffect_Texture
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_Texture();
    }
}

// -----------------纹理描边绘画----------------------
float4 PS_TextureBorder( float2 _Texture0 : TEXCOORD0, float2 _Texture1 : TEXCOORD1 ) : COLOR 
{
	// 获取像素
	float4 _inColor0 = tex2D( _samSpirit0 , _Texture0.xy);
	float4 _inColor1 = tex2D( _samSpirit1 , _Texture1.xy);

	// 输出像素
	float4 _outColor = _inColor0;

	// 偏移度
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);

	// 纹理颜色
	if( _inColor0.a > 0 )
	{
		_outColor = _inColor0 * _inColor1 * _OverlayColor;
	}
	// 描边颜色
	else
	{
		// 获取附近值信息
		float4 _Left = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture0.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture0.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture0.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( fX, fY ) ); 

		// 附近有值
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// 设置背景颜色
			_outColor = _BorderColor;
		}
	}

	// 返回颜色
	return _outColor;
}

technique TextureEffect_TextureBorder
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_TextureBorder();
    }
}


// -----------------覆盖绘画----------------------
float4 PS_Cover( float2 _Texture : TEXCOORD0 ) : COLOR 
{ 
	// 获取像素
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// 输出像素
	float4 _outColor = _inColor; // * _OverlayColor;
	
	return _outColor;
}

technique TextureEffect_Cover
{
    pass p0 
    {
		AlphaBlendEnable = false;
    	PixelShader = compile ps_2_0 PS_Cover();
    }
}


