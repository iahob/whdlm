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
int         CurNumBones = 2;    //当前骨骼数量
float4x4    mProj;

//----------------------------------------------------------
// 顶点渲染器输入和输出结构
//----------------------------------------------------------
struct VS_INPUT
{
    float4  Pos           : POSITION;
    float4  BlendWeights  : BLENDWEIGHT;
    float4  BlendIndices  : BLENDINDICES;
    float3  Normal        : NORMAL;
    float3  Tex0          : TEXCOORD0;
};

struct VS_OUTPUT
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
VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
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
	
	o.Pos = float4(0.0, 0.0, 0.0, 1.0);

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
technique t0
{
    pass p0
    {
        VertexShader = (vsArray[CurNumBones]);
		//PixelShader = compile ps_2_0 PS_Model1();
    }
}

