
// �������
sampler2D _samSpirit0;
sampler2D _samSpirit1;

// �˻���ɫ
float4 _OverlayColor : COLOR;

// �����С
float2 _ReliefTexSize : TEXCOORD;	

// �߽���ɫ
float4 _BorderColor : COLOR; 

// ������ɫ
float4 _TopColor : COLOR; 
float4 _BottomColor : COLOR; 

// -----------------����----------------------
float4 PS_Shine ( float2 _Texture : TEXCOORD ) : COLOR
{
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0, _Texture );

	// �˻�����
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

// -----------------�Ҷ�----------------------
float4 PS_Gray ( float2 _Texture : TEXCOORD ) : COLOR
{
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0, _Texture );

	// �Ҷ�����
	float _Gray = 0.3 * _inColor.r + 0.59 * _inColor.g + 0.11* _inColor.b;

	// ����������
    float4 _outColor = float4(_Gray, _Gray, _Gray, _inColor.a);

	// �˻�����
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

// -----------------����----------------------
float4 PS_Relief ( float2 _Texture : TEXCOORD ) : COLOR
{
	// ��ȡ��Ϣ
	float2  _upLeftUV = float2(_Texture.x - 1.0/_ReliefTexSize.x , _Texture.y - 1.0/_ReliefTexSize.y);
	float4  _bkColor = float4(0.5 , 0.5 , 0.5 , 1.0);
	float4  _curColor   = tex2D( _samSpirit0, _Texture );
	float4  _upLeftColor =  tex2D( _samSpirit0, _upLeftUV );

	// ����õ���ɫ�Ĳ�
	float4  _delColor = _curColor - _upLeftColor;

	// ��Ҫ�������ɫ�Ĳ�����
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

// -----------------��߻滭----------------------
float4 PS_Border( float2 _Texture : TEXCOORD0 ) : COLOR 
{ 
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// �������
	float4 _outColor = _inColor;
	
	// ƫ�ƶ�
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);
     
	// ������
	if( _inColor.a == 0 )
	{
		// ��ȡ����ֵ��Ϣ
		float4 _Left = tex2D( _samSpirit0, _Texture.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( fX, fY ) ); 

		// ������ֵ
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// ���ñ�����ɫ
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

// -----------------����滭----------------------
float4 PS_Gradient( float2 _Texture : TEXCOORD ) : COLOR 
{ 
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// �������
	float4 _outColor = _inColor;

	// ������ɫ
	if( _inColor.a > 0 )
	{
		float _r = (_BottomColor.r - _TopColor.r)* ( _Texture.y / 1.0 ) + _TopColor.r;
		float _g = (_BottomColor.g - _TopColor.g)* ( _Texture.y / 1.0 ) + _TopColor.g;
		float _b = (_BottomColor.b - _TopColor.b)* ( _Texture.y / 1.0 ) + _TopColor.b;

		_outColor = float4( _r, _g, _b, _inColor.a ) * _OverlayColor;
	}

	// ������ɫ
	return _outColor;
}

technique TextureEffect_Gradient
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_Gradient();
    }
}

// -----------------������߻滭----------------------
float4 PS_GradientBorder( float2 _Texture : TEXCOORD ) : COLOR 
{
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// �������
	float4 _outColor = _inColor;

	// ƫ�ƶ�
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);

	// ������ɫ
	if( _inColor.a > 0 )
	{
		float _r = (_BottomColor.r - _TopColor.r)* ( _Texture.y / 1.0 ) + _TopColor.r;
		float _g = (_BottomColor.g - _TopColor.g)* ( _Texture.y / 1.0 ) + _TopColor.g;
		float _b = (_BottomColor.b - _TopColor.b)* ( _Texture.y / 1.0 ) + _TopColor.b;

		_outColor = float4( _r, _g, _b, _inColor.a ) * _OverlayColor;
	}
	// �����ɫ
	else
	{
		// ��ȡ����ֵ��Ϣ
		float4 _Left = tex2D( _samSpirit0, _Texture.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture.xy + float2( fX, fY ) ); 

		// ������ֵ
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// ���ñ�����ɫ
			_outColor = _BorderColor;
		}
	}

	// ������ɫ
	return _outColor;
}

technique TextureEffect_GradientBorder
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_GradientBorder();
    }
}

// -----------------����滭----------------------
float4 PS_Texture( float2 _Texture0 : TEXCOORD0, float2 _Texture1 : TEXCOORD1 ) : COLOR 
{
	// ��ȡ����
	float4 _inColor0 = tex2D( _samSpirit0 , _Texture0.xy);
	float4 _inColor1 = tex2D( _samSpirit1 , _Texture1.xy);

	// �������
	float4 _outColor = _inColor0 * _inColor1 * _OverlayColor;
	
	// ������ɫ
	return _outColor;
}

technique TextureEffect_Texture
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_Texture();
    }
}

// -----------------������߻滭----------------------
float4 PS_TextureBorder( float2 _Texture0 : TEXCOORD0, float2 _Texture1 : TEXCOORD1 ) : COLOR 
{
	// ��ȡ����
	float4 _inColor0 = tex2D( _samSpirit0 , _Texture0.xy);
	float4 _inColor1 = tex2D( _samSpirit1 , _Texture1.xy);

	// �������
	float4 _outColor = _inColor0;

	// ƫ�ƶ�
	float fX = (1.0 / _ReliefTexSize.x);
	float fY = (1.0 / _ReliefTexSize.y);

	// ������ɫ
	if( _inColor0.a > 0 )
	{
		_outColor = _inColor0 * _inColor1 * _OverlayColor;
	}
	// �����ɫ
	else
	{
		// ��ȡ����ֵ��Ϣ
		float4 _Left = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, 0 ) ); 
		float4 _Right = tex2D( _samSpirit0, _Texture0.xy + float2( fX, 0 ) ); 
		float4 _Top = tex2D( _samSpirit0, _Texture0.xy + float2( 0, -fY ) ); 
		float4 _Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( 0, fY ) ); 
             
		float4 _Left_Top = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, -fY ) ); 
		float4 _Right_Top = tex2D( _samSpirit0, _Texture0.xy + float2( fX, -fY ) ); 
		float4 _Left_Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( -fX, fY ) ); 
		float4 _Right_Bottom = tex2D( _samSpirit0, _Texture0.xy + float2( fX, fY ) ); 

		// ������ֵ
		if( _Left.a > 0 || _Right.a > 0 || _Top.a > 0 || _Bottom.a > 0 
		 || _Left_Top.a > 0 || _Right_Top.a > 0 || _Left_Bottom.a > 0 || _Right_Bottom.a > 0 )
		{
			// ���ñ�����ɫ
			_outColor = _BorderColor;
		}
	}

	// ������ɫ
	return _outColor;
}

technique TextureEffect_TextureBorder
{
    pass p0 
    {
    	PixelShader = compile ps_2_0 PS_TextureBorder();
    }
}


// -----------------���ǻ滭----------------------
float4 PS_Cover( float2 _Texture : TEXCOORD0 ) : COLOR 
{ 
	// ��ȡ����
	float4 _inColor = tex2D( _samSpirit0 , _Texture.xy);

	// �������
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


