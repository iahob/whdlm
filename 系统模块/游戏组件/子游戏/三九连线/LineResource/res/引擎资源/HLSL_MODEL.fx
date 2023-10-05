/*

Dexter.2014

*/

float4x4 World         : 	WORLD;
float4x4 View          : 	VIEW;
float4x4 Projection    : 	PROJECTION;
float4x4 WorldViewProj : 	WORLDVIEWPROJ;
float4x4 WorldView     : 	WORLDVIEW;

// 纹理
texture Texture
<
	string UIName = "Texture";
>;
sampler MeshTextureSampler = sampler_state
{
    Texture = <Texture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// 环境光
float4 AmbientColor  
<
	string UIName = "AmbientColor";
> = float4( 0.4f, 0.4f, 0.4f, 1.0f ); 

// 透明信息
float Transparency
<
	string UIName = "Transparency";
	float UIMin = 0.0f;
	float UIMax = 1.0f;	
> = 1.f;

//---------------------------------------------------
// 灯光一
bool LightEnableOne
<  
	string UIName = "LightEnableOne"; 
> = true;

// 灯光方向
float3 LightDirOne : Direction 
<  
	string UIName = "LightDirOne"; 
	string Object = "TargetLight";
> = {0.0, 1.0, 0.0};
	
// 灯光颜色
float4 DiffuseColorOne
<
	string UIName = "DiffuseColorOne";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );  
	
float4 SpecularColorOne
<
	string UIName = "SpecularColorOne";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f ); 
	
int SpecularPowerOne
<
	string UIName = "SpecularPowerOne";
	int UIMin = 0;
	int UIMax = 50;	
> = 15;

//---------------------------------------------------
// 灯光二
bool LightEnableTwo
<  
	string UIName = "LightEnableTwo"; 
> = true;

// 灯光方向
float3 LightDirTwo : Direction 
<  
	string UIName = "LightDirTwo"; 
	string Object = "TargetLight";
> = {0.0, 1.0, 0.0};
	
// 灯光颜色
float4 DiffuseColorTwo
<
	string UIName = "DiffuseColorTwo";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );  
	
float4 SpecularColorTwo
<
	string UIName = "SpecularColorTwo";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f ); 
	
int SpecularPowerTwo
<
	string UIName = "SpecularPowerTwo";
	int UIMin = 0;
	int UIMax = 50;	
> = 15;

//---------------------------------------------------
// 灯光三
bool LightEnableThree
<  
	string UIName = "LightEnableThree"; 
> = true;

// 灯光方向
float3 LightDirThree : Direction 
<  
	string UIName = "LightDirThree"; 
	string Object = "TargetLight";
> = {0.0, 1.0, 0.0};
	
// 灯光颜色
float4 DiffuseColorThree
<
	string UIName = "DiffuseColorThree";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );  
	
float4 SpecularColorThree
<
	string UIName = "SpecularColorThree";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f ); 
	
int SpecularPowerThree
<
	string UIName = "SpecularPowerThree";
	int UIMin = 0;
	int UIMax = 50;	
> = 15;

//--------------------------------------------------------------------------------------
// 输入输出结构
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position   : POSITION;  
    float2 TextureUV  : TEXCOORD0;  
	float3 Normal     : NORMAL; 
};

struct VS_OUTPUT
{
 	float4 Position   : POSITION;
	float2 TextureUV  : TEXCOORD0; 
	float3 Normal     : TEXCOORD1; 
};

VS_OUTPUT VS_Model( VS_INPUT In )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Position  = mul(float4(In.Position,1), WorldViewProj);
	Out.TextureUV = In.TextureUV;
	Out.Normal = normalize( mul( In.Normal, World ) );

    return Out;
}

float4 DirectionalLight( float3 vLight, float3 Normal, float4 fDiffuseLight, float4 fSpecularLight, int nSpecularPower )
{
    // 漫反射光
    float4 fDiffuse = max( 0, dot( Normal, normalize( vLight )) ) * fDiffuseLight;
    // 高光
	float4 fSpecular = pow( max ( 0, dot( Normal, normalize( vLight ))) , (float)nSpecularPower / 4.f ) * fSpecularLight;
	// 返回颜色
	return fDiffuse + fSpecular;
}


float4 PS_Model( VS_OUTPUT In ) : COLOR 
{
	// 反射颜色
	float4 fColor = AmbientColor;
	
	// 灯光启动
	if( LightEnableOne )
	{
		fColor += DirectionalLight(LightDirOne, In.Normal, DiffuseColorOne, SpecularColorOne, SpecularPowerOne);
	}
	if( LightEnableTwo )
	{
		fColor += DirectionalLight(LightDirTwo, In.Normal, DiffuseColorTwo, SpecularColorTwo, SpecularPowerTwo);
	}
	if( LightEnableThree )
	{
		fColor += DirectionalLight(LightDirThree, In.Normal, DiffuseColorThree, SpecularColorThree, SpecularPowerThree);
	}
	// 透明度
	fColor.a = Transparency;
	
    return  tex2D( MeshTextureSampler, In.TextureUV ) * fColor; 
} 


technique Technique0
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Model();
		PixelShader = compile ps_2_0 PS_Model();
	}
}




















//=========================================================
// Desc: 效果源代码
//=========================================================


//---------------------------------------------------------
// 全局变量
//---------------------------------------------------------
float4 lightDir = {0.0f, 0.0f, -1.0f, 1.0f};
float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; 
float4 MaterialAmbient = {0.1f, 0.1f, 0.1f, 1.0f};
float4 MaterialDiffuse = {0.8f, 0.8f, 0.8f, 1.0f};

static const int MAX_MATRICES = 50;
float4x3    mWorldMatrixArray[MAX_MATRICES];  //骨骼调色板矩阵 
float4x4    mProj;
int         CurNumBones = 2;    //当前骨骼数量

//----------------------------------------------------------
// 顶点渲染器输入和输出结构
//----------------------------------------------------------
struct VS_INPUT_SKIN
{
    float4  Pos           : POSITION;
    float4  BlendWeights  : BLENDWEIGHT;
    float4  BlendIndices  : BLENDINDICES;
    float3  Normal        : NORMAL;
    float3  Tex0          : TEXCOORD0;
};

struct VS_OUTPUT_SKIN
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR;
    float2  Tex0    : TEXCOORD0;
};


//---------------------------------------------------------
// 子函数, 计算光照系数
//---------------------------------------------------------
float3 Diffuse(float3 Normal)
{
    float CosTheta;
    CosTheta = max(0.0f, dot(Normal, lightDir.xyz));
    return (CosTheta);
}

//---------------------------------------------------------
// 子函数, 完成顶点处理
//---------------------------------------------------------
VS_OUTPUT_SKIN VShade(VS_INPUT_SKIN i, uniform int NumBones)
{
    VS_OUTPUT_SKIN   o;
    float3      Pos = 0.0f;
    float3      Normal = 0.0f;
    float       sumWeight  = 0.0f;
    float       LastWeight = 0.0f;
     
    //为Geforce3类型的显卡所做的补偿
    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);
    
    //将骨骼权重向量和骨骼索引向量变换至数组
    float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;

    //计算前NumBones-1个骨骼对于该顶点位置及法向量的影响
    for (int iBone = 0; iBone < NumBones-1; iBone++)
    {
        sumWeight = sumWeight + BlendWeightsArray[iBone];
        
        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - sumWeight; 

    //计算最后一个骨骼对于顶点位置及法向量的影响
    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);
    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); 
    
    o.Pos = mul(float4(Pos.xyz, 1.0f), mProj);
    Normal = normalize(Normal);

    //计算光照
    o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;
    o.Diffuse.w = 1.0f;

    //输出纹理坐标
    o.Tex0  = i.Tex0.xy;

    return o;
}

float4 PS_Model1() : COLOR 
{

    return  float4(1.0f, 1.0f, 1.0f, 1.0f); 
} 


//---------------------------------------------------------
// 顶点渲染器
//---------------------------------------------------------
VertexShader vsArray[4] = {
                            compile vs_1_1 VShade(1), 
                            compile vs_1_1 VShade(2),
                            compile vs_1_1 VShade(3),
                            compile vs_1_1 VShade(4)
                          };


//---------------------------------------------------------
// 技术与通道
//---------------------------------------------------------
technique Technique1
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
		PixelShader = compile ps_1_1 PS_Model1();
    }
}























