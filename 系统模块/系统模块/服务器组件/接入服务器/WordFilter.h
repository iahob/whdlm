#ifndef WORD_FILTER_HEAD_FILE
#define WORD_FILTER_HEAD_FILE

//引入文件
#include <AfxTempl.h>

////////////////////////////////////////////////////////////////////////////////
//类型声明
class CWordNode;
class CWordTree;
class CWordFilter;

//类型定义	
typedef CMap<CHAR,CHAR,CWordNode *,CWordNode *>			CTreeMap;	

////////////////////////////////////////////////////////////////////////////////
//关键词节点
class CWordNode
{
	friend class CWordTree;

	//变量定义
protected:
	CHAR								m_character;			//字符对象
    CTreeMap							m_TreeMap;				//关键词表
	CStringA							m_strKeywords;			//关键词组	

	//函数定义
public:
	//构造函数
	CWordNode();
	//构造函数
    CWordNode(CHAR character);
	//析构函数
	virtual ~CWordNode();

	//函数定义
public:	
	//查找节点
    CWordNode* FindChild(CHAR character);
	//插入节点
    CWordNode* InsertChild(CHAR character);	
	//获取字符
    CHAR GetCharacter() { return m_character; }

	//关键词管理
public:
	//查找关键词
	bool FindKeyword(LPCSTR pszKeyword);
	//插入关键词
	bool InsertKeyword(LPCSTR pszKeyword);	
};

////////////////////////////////////////////////////////////////////////////////
//关键词树
class CWordTree
{
	//友类声明
	friend class CWordFilter;

	//变量定义
public:
	int								m_nCount;						//匹配数量

	//变量定义
private:	
	CWordNode *						m_pEmptyRoot;					//根部节点

	//函数定义
public:
	//构造函数
	CWordTree();	
	//析构函数
	virtual ~CWordTree();

	//功能函数
public:    	
	//查找关键词
    CWordNode* Find(LPCSTR pszKeyword);    
	//插入关键词
    CWordNode* Insert(LPCSTR pszKeyword);		

	//辅助函数
protected:
	//重置对象
	VOID RebuildTree();
	//跳过空格
	VOID SkipSpace(LPCSTR & pszKeyword);

	//内部函数
private:   
	//查找关键词
	CWordNode* Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);
	//插入关键词
    CWordNode* Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);	
	//插入关键词
    CWordNode* InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword);    
};

////////////////////////////////////////////////////////////////////////////////
//关键词过滤
class CWordFilter
{
	//变量定义
protected:
	CWordTree							m_WordTree;					//关键词树	
	CHAR								m_szFilter[512];			//过滤缓冲

	//函数定义
public:
	//构造函数
	CWordFilter();
	//析构函数
	virtual ~CWordFilter();

	//功能函数
public:
	//重置对象
	VOID ResetFilter();
	//插入关键词
	bool InsertKeyword(LPCSTR pszKeyWord);	
	//插入关键词
	bool InsertKeyword(LPCWSTR pszKeyWord);

	//过滤函数
public:		
	//过滤函数
	bool FilterWords(LPCSTR pszKeyword);
	//过滤函数
	bool FilterWords(LPCWSTR pszKeyword);
	//过滤函数
	bool FilterWords(CHAR szKeyWord[], WORD wBufferSize);
	//过滤函数
	bool FilterWords(WCHAR szKeyWord[], WORD wBufferSize);
};

////////////////////////////////////////////////////////////////////////////////

#endif