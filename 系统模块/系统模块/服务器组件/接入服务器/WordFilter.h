#ifndef WORD_FILTER_HEAD_FILE
#define WORD_FILTER_HEAD_FILE

//�����ļ�
#include <AfxTempl.h>

////////////////////////////////////////////////////////////////////////////////
//��������
class CWordNode;
class CWordTree;
class CWordFilter;

//���Ͷ���	
typedef CMap<CHAR,CHAR,CWordNode *,CWordNode *>			CTreeMap;	

////////////////////////////////////////////////////////////////////////////////
//�ؼ��ʽڵ�
class CWordNode
{
	friend class CWordTree;

	//��������
protected:
	CHAR								m_character;			//�ַ�����
    CTreeMap							m_TreeMap;				//�ؼ��ʱ�
	CStringA							m_strKeywords;			//�ؼ�����	

	//��������
public:
	//���캯��
	CWordNode();
	//���캯��
    CWordNode(CHAR character);
	//��������
	virtual ~CWordNode();

	//��������
public:	
	//���ҽڵ�
    CWordNode* FindChild(CHAR character);
	//����ڵ�
    CWordNode* InsertChild(CHAR character);	
	//��ȡ�ַ�
    CHAR GetCharacter() { return m_character; }

	//�ؼ��ʹ���
public:
	//���ҹؼ���
	bool FindKeyword(LPCSTR pszKeyword);
	//����ؼ���
	bool InsertKeyword(LPCSTR pszKeyword);	
};

////////////////////////////////////////////////////////////////////////////////
//�ؼ�����
class CWordTree
{
	//��������
	friend class CWordFilter;

	//��������
public:
	int								m_nCount;						//ƥ������

	//��������
private:	
	CWordNode *						m_pEmptyRoot;					//�����ڵ�

	//��������
public:
	//���캯��
	CWordTree();	
	//��������
	virtual ~CWordTree();

	//���ܺ���
public:    	
	//���ҹؼ���
    CWordNode* Find(LPCSTR pszKeyword);    
	//����ؼ���
    CWordNode* Insert(LPCSTR pszKeyword);		

	//��������
protected:
	//���ö���
	VOID RebuildTree();
	//�����ո�
	VOID SkipSpace(LPCSTR & pszKeyword);

	//�ڲ�����
private:   
	//���ҹؼ���
	CWordNode* Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);
	//����ؼ���
    CWordNode* Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);	
	//����ؼ���
    CWordNode* InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);    
};

////////////////////////////////////////////////////////////////////////////////
//�ؼ��ʹ���
class CWordFilter
{
	//��������
protected:
	CWordTree							m_WordTree;					//�ؼ�����	
	CHAR								m_szFilter[512];			//���˻���

	//��������
public:
	//���캯��
	CWordFilter();
	//��������
	virtual ~CWordFilter();

	//���ܺ���
public:
	//���ö���
	VOID ResetFilter();
	//����ؼ���
	bool InsertKeyword(LPCSTR pszKeyWord);	
	//����ؼ���
	bool InsertKeyword(LPCWSTR pszKeyWord);

	//���˺���
public:		
	//���˺���
	bool FilterWords(LPCSTR pszKeyword);
	//���˺���
	bool FilterWords(LPCWSTR pszKeyword);
	//���˺���
	bool FilterWords(CHAR szKeyWord[], WORD wBufferSize);
	//���˺���
	bool FilterWords(WCHAR szKeyWord[], WORD wBufferSize);
};

////////////////////////////////////////////////////////////////////////////////

#endif