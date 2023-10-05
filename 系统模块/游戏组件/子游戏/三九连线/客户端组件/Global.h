#pragma once

// ������
class CLightMove;

// �滭����
#define DN_TOP                      0x00000000
#define DN_LEFT                     0x00000000
#define DN_CENTER                   0x00000001
#define DN_RIGHT                    0x00000002
#define DN_VCENTER                  0x00000004
#define DN_BOTTOM                   0x00000008
	
// ȫ������
class CGlobal
{

	// ��Դ���
public:
	HMODULE								hModuleResource;			// DLL���

	// ��ԴͼƬ
public:
	dex::CD3DTexture					TextureElement[TextureElementType_Max][TextureElement_Max];

	// ��̬����
private:
	Static CGlobal						g_Global;					// ȫ�ֱ���

	// �ຯ��
private:
	// ���캯��
	CGlobal();
	// ��������
	~CGlobal();

public:
	// ���ض���
	inline Static CGlobal* _Object() { return &g_Global; }

	// ������
public:
	// ��������
	void CreateTexture( dex::CD3DDevice * pD3DDevice );
	// �ͷ�����
	void ReleaseTexture();
	// �滭Ԫ��
	void DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// �滭Ԫ��
	void DrawElement( dex::CD3DDevice * pD3DDevice, EnumTextureElement nElementIndex, int nPosX, int nPosY, int nDestWidth, int nDestHeight, dex::CD3DColor D3DColor, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// ����ת������
	EnumTextureElement MultipleThreeToTextureElement( EnumMultipleThree nEnumMultipleThree );
	// ����ת������
	EnumTextureElement MultipleNineToTextureElement( EnumMultipleNine nEnumMultipleNine );
	// ����ת������
	EnumTextureElement MultipleToTextureElement( int nMultipleIndex );
	// ����ת������
	int MultipleToElementCount( int nMultipleIndex );

	// ���溯��
public: 
	// �滭����
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, longlong lOutNum, int nXPos, int nYPos, uint uFormat = DN_LEFT|DN_TOP, byte cbAlpha = 255 );
	// �滭����
	CRect DrawNumber( dex::CD3DDevice * pD3DDevice , dex::CD3DTexture & ImageNumber, const tchar * pszImageNum, const tchar * pszOutNum, int nXPos, int nYPos, uint uFormat = DN_LEFT|DN_TOP, byte cbAlpha = 255 );
	// �滭���
	void DrawLightMove( dex::CD3DDevice * pD3DDevice, dex::CD3DTexture & ImageLight, CWHArray< CLightMove * > & ArrayLightMove, dex::EnumTextureEffectType nEffectType = dex::TextureEffect_Normal );
	// ��Ӷ���
	CString AppendComma( CString & StrNumber );
	// ��Ӷ���
	CString AppendComma( __int64 nNumber );
	// ��Ӷ���
	CString AppendComma( CString & StrNumber, int nMaxCount );
	// ��Ӷ���
	CString AppendComma( __int64 nNumber, int nMaxCount );

	// ��Ϣ����
public: 
	// ������Ϣ
	int64 MessageBox( const tchar * pszMessage, dex::CDirectUIWindow * pWindowParent = NULL );

};
