//藏宝库IT社区（28xin.com）
#include "stdafx.h"
#include "WordFilter.h"

////////////////////////////////////////////////////////////////////////
//常量定义
#define LEN_KEYWORD					128							//最大长度

////////////////////////////////////////////////////////////////////////
//构造函数
CWordNode::CWordNode() : m_character(0)
{	
}

//构造函数
CWordNode::CWordNode(CHAR character) : m_character(character)
{	
}

//析构函数
CWordNode::~CWordNode()
{
	//设置变量
	m_strKeywords.Empty();

	//删除节点
	CHAR character=0;
	CWordNode * pWordNode=NULL;
	POSITION Position=m_TreeMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TreeMap.GetNextAssoc(Position,character,pWordNode);
		SafeDelete(pWordNode);
	}	
}

//查找节点
CWordNode* CWordNode::FindChild(CHAR character)
{
	//查找对象
	CWordNode * pWordNode=NULL;
	if (m_TreeMap.Lookup(character,pWordNode)==FALSE)
	{
		return NULL;
	}

	return pWordNode;    
}

//插入节点
CWordNode* CWordNode::InsertChild(CHAR character)
{
	//查找节点
    if (FindChild(character)) return NULL;

	//创建节点
	try
	{
		CWordNode * pWordNode=new CWordNode(character);
		if (pWordNode==NULL) throw TEXT("内存不足,对象创建失败！");

		//设置节点
		m_TreeMap[character]=pWordNode; 

		return pWordNode;
	}
	catch (...)
	{
		ASSERT (FALSE);			
	}    

    return NULL;
}

//查找关键词
bool CWordNode::FindKeyword(LPCSTR pszKeyword)
{
	//变量定义
	CHAR szKeyword[LEN_KEYWORD+1];
	_snprintf_s(szKeyword, CountArray(szKeyword), "%s|", pszKeyword);

	//查找字符串
	return m_strKeywords.Find(szKeyword)!=-1; 
}

//插入关键词
bool CWordNode::InsertKeyword(LPCSTR pszKeyword)
{
	//变量定义
	CHAR szKeyword[LEN_KEYWORD+1];
	_snprintf_s(szKeyword, CountArray(szKeyword), "%s|", pszKeyword);

	//查找关键词
	if (m_strKeywords.Find(szKeyword)!=-1) 
	{
		return false;
	}

	//插入关键词
	m_strKeywords.Append(szKeyword);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////

//构造函数
CWordTree::CWordTree() : m_nCount(0),m_pEmptyRoot(NULL)
{
}

//析构函数
CWordTree::~CWordTree()
{
	//释放节点
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);
}

//重置对象
VOID CWordTree::RebuildTree()
{
	//释放节点
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);

	//创建节点
	try
	{
		m_pEmptyRoot=new CWordNode();
	}
	catch (...)
	{
		ASSERT (FALSE);
		m_pEmptyRoot=NULL;
	}
}

//查找关键词
CWordNode* CWordTree::Find(LPCSTR pszKeyword)
{
	m_nCount=0;
	return Find(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//插入关键词
CWordNode* CWordTree::Insert(LPCSTR pszKeyword)
{
	//校验长度
	ASSERT (lstrlenA(pszKeyword)<=LEN_KEYWORD);
	if (lstrlenA(pszKeyword)>LEN_KEYWORD) return NULL;

	return Insert(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//跳过空格
VOID CWordTree::SkipSpace(LPCSTR & pszKeyword)
{
	while (*pszKeyword==32) 
	{
		m_nCount++;
		pszKeyword++;	
	}
}

//查找关键词
CWordNode* CWordTree::Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword)==0) return NULL;

	//跳过空格
	SkipSpace(pszSubKeyword);

	//查找首字符
	CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->FindChild(chFirst);
    if (pFirstNode==NULL) return NULL;

	//设置变量
    m_nCount++;

	//判断节点
	if (pFirstNode->m_TreeMap.IsEmpty()) 
    {
        return pFirstNode;
    }	

	//递归查找
    CWordNode* pFindNode=Find(pFirstNode,pszKeyword,pszSubKeyword+1);
	if (pFindNode==NULL)
	{
		//构造关键词
		CHAR szKeyword[LEN_KEYWORD]={};
		lstrcpynA(szKeyword,pszKeyword,pszSubKeyword-pszKeyword+2);

		//查找关键词
		if (pFirstNode->FindKeyword(szKeyword)==false)
		{
			//设置变量
			m_nCount-=1;

			return NULL;
		}

		return pFirstNode;
	}

	return pFindNode;
}

//插入关键词
CWordNode* CWordTree::Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword)==0) 
	{
		//插入关键词
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}

	//查找字符
    CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->FindChild(chFirst);

	//插入分支
    if (pFirstNode==NULL)
    {
        return InsertBranch(pParent,pszKeyword,pszSubKeyword);
    }

    return Insert(pFirstNode,pszKeyword,pszSubKeyword+1);
}

//插入关键词
CWordNode* CWordTree::InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword)==0)
	{
		//插入关键词
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}
	
	//查找字符
	CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->InsertChild(chFirst);

	//插入分支
    if (pFirstNode!=NULL && lstrlenA(pszSubKeyword)>0)
    {
        return InsertBranch(pFirstNode,pszKeyword,pszSubKeyword+1);
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////

//构造函数
CWordFilter::CWordFilter()
{
	ResetFilter();
}

//析构函数
CWordFilter::~CWordFilter()
{
}

//重置数据
VOID CWordFilter::ResetFilter()
{	
	//设置变量
	m_szFilter[0]=0;

	//构建词树
	m_WordTree.RebuildTree();
}

//过滤函数
bool CWordFilter::FilterWords(LPCSTR pszKeyword)
{
	//参数校验
	if (pszKeyword==NULL) return false;

	//变量定义	
	int nLength=lstrlenA(pszKeyword);

	//查找关键字
	for (int i=0; i<nLength; i++)
    {
        LPCSTR pSubstring=pszKeyword+i;
        if (m_WordTree.Find(pSubstring)!=NULL)
        {
			return true;
        }
    }
    	    
	return false;
}

//过滤函数
bool CWordFilter::FilterWords(LPCWSTR pszKeyword)
{
	//字符转换
	CW2A pszAnsKeyword(pszKeyword);

	return FilterWords(pszAnsKeyword);
}

//过滤函数
bool CWordFilter::FilterWords(CHAR szKeyWord[], WORD wBufferSize)
{
	//参数校验
	if (szKeyWord[0]==0) return false;

	//变量定义	
	LPCSTR pszKeyword=szKeyWord;
	
	//变量定义
	int i=0;
	int nDstLen=0;
	int nSrcLen=lstrlenA(pszKeyword);		

	while (i<nSrcLen)
    {
        LPCSTR pSubstring=pszKeyword+i;
        if (m_WordTree.Find(pSubstring)!=NULL)
        {			
			i+=m_WordTree.m_nCount;
			*(szKeyWord+nDstLen++)=TEXT('*');			
        }
		else
		{
			i++;
			*(szKeyWord+nDstLen++)=*pSubstring;			
		}	
    }	

	//设置结束符	
	szKeyWord[nDstLen]=0;

	return true;
}

//过滤函数
bool CWordFilter::FilterWords(WCHAR szKeyWord[], WORD wBufferSize)
{
	//参数校验
	if (szKeyWord[0]==0) return false;

	//变量定义
	CW2A pszKeyword(szKeyWord);
	int nLength=strlen(pszKeyword)+1;

	//判断缓冲
	if (nLength>=sizeof(m_szFilter)) return false;

	//拷贝字符串
	lstrcpynA(m_szFilter,pszKeyword,nLength);

	//过滤字符
	FilterWords(m_szFilter,nLength);

	//拷贝字符串
	lstrcpynW(szKeyWord,CA2W(m_szFilter),nLength);	

	return true;
}

//插入关键词
bool CWordFilter::InsertKeyword(LPCSTR pszKeyWord)
{
	return m_WordTree.Insert(pszKeyWord)!=NULL;
}

//插入关键词
bool CWordFilter::InsertKeyword(LPCWSTR pszKeyWord)
{
	return m_WordTree.Insert(CW2A(pszKeyWord))!=NULL;
}

////////////////////////////////////////////////////////////////////////////////////