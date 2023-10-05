//�ر���IT������28xin.com��
#include "stdafx.h"
#include "WordFilter.h"

////////////////////////////////////////////////////////////////////////
//��������
#define LEN_KEYWORD					128							//��󳤶�

////////////////////////////////////////////////////////////////////////
//���캯��
CWordNode::CWordNode() : m_character(0)
{	
}

//���캯��
CWordNode::CWordNode(CHAR character) : m_character(character)
{	
}

//��������
CWordNode::~CWordNode()
{
	//���ñ���
	m_strKeywords.Empty();

	//ɾ���ڵ�
	CHAR character=0;
	CWordNode * pWordNode=NULL;
	POSITION Position=m_TreeMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TreeMap.GetNextAssoc(Position,character,pWordNode);
		SafeDelete(pWordNode);
	}	
}

//���ҽڵ�
CWordNode* CWordNode::FindChild(CHAR character)
{
	//���Ҷ���
	CWordNode * pWordNode=NULL;
	if (m_TreeMap.Lookup(character,pWordNode)==FALSE)
	{
		return NULL;
	}

	return pWordNode;    
}

//����ڵ�
CWordNode* CWordNode::InsertChild(CHAR character)
{
	//���ҽڵ�
    if (FindChild(character)) return NULL;

	//�����ڵ�
	try
	{
		CWordNode * pWordNode=new CWordNode(character);
		if (pWordNode==NULL) throw TEXT("�ڴ治��,���󴴽�ʧ�ܣ�");

		//���ýڵ�
		m_TreeMap[character]=pWordNode; 

		return pWordNode;
	}
	catch (...)
	{
		ASSERT (FALSE);			
	}    

    return NULL;
}

//���ҹؼ���
bool CWordNode::FindKeyword(LPCSTR pszKeyword)
{
	//��������
	CHAR szKeyword[LEN_KEYWORD+1];
	_snprintf_s(szKeyword, CountArray(szKeyword), "%s|", pszKeyword);

	//�����ַ���
	return m_strKeywords.Find(szKeyword)!=-1; 
}

//����ؼ���
bool CWordNode::InsertKeyword(LPCSTR pszKeyword)
{
	//��������
	CHAR szKeyword[LEN_KEYWORD+1];
	_snprintf_s(szKeyword, CountArray(szKeyword), "%s|", pszKeyword);

	//���ҹؼ���
	if (m_strKeywords.Find(szKeyword)!=-1) 
	{
		return false;
	}

	//����ؼ���
	m_strKeywords.Append(szKeyword);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////

//���캯��
CWordTree::CWordTree() : m_nCount(0),m_pEmptyRoot(NULL)
{
}

//��������
CWordTree::~CWordTree()
{
	//�ͷŽڵ�
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);
}

//���ö���
VOID CWordTree::RebuildTree()
{
	//�ͷŽڵ�
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);

	//�����ڵ�
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

//���ҹؼ���
CWordNode* CWordTree::Find(LPCSTR pszKeyword)
{
	m_nCount=0;
	return Find(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//����ؼ���
CWordNode* CWordTree::Insert(LPCSTR pszKeyword)
{
	//У�鳤��
	ASSERT (lstrlenA(pszKeyword)<=LEN_KEYWORD);
	if (lstrlenA(pszKeyword)>LEN_KEYWORD) return NULL;

	return Insert(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//�����ո�
VOID CWordTree::SkipSpace(LPCSTR & pszKeyword)
{
	while (*pszKeyword==32) 
	{
		m_nCount++;
		pszKeyword++;	
	}
}

//���ҹؼ���
CWordNode* CWordTree::Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword)==0) return NULL;

	//�����ո�
	SkipSpace(pszSubKeyword);

	//�������ַ�
	CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->FindChild(chFirst);
    if (pFirstNode==NULL) return NULL;

	//���ñ���
    m_nCount++;

	//�жϽڵ�
	if (pFirstNode->m_TreeMap.IsEmpty()) 
    {
        return pFirstNode;
    }	

	//�ݹ����
    CWordNode* pFindNode=Find(pFirstNode,pszKeyword,pszSubKeyword+1);
	if (pFindNode==NULL)
	{
		//����ؼ���
		CHAR szKeyword[LEN_KEYWORD]={};
		lstrcpynA(szKeyword,pszKeyword,pszSubKeyword-pszKeyword+2);

		//���ҹؼ���
		if (pFirstNode->FindKeyword(szKeyword)==false)
		{
			//���ñ���
			m_nCount-=1;

			return NULL;
		}

		return pFirstNode;
	}

	return pFindNode;
}

//����ؼ���
CWordNode* CWordTree::Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword)==0) 
	{
		//����ؼ���
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}

	//�����ַ�
    CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->FindChild(chFirst);

	//�����֧
    if (pFirstNode==NULL)
    {
        return InsertBranch(pParent,pszKeyword,pszSubKeyword);
    }

    return Insert(pFirstNode,pszKeyword,pszSubKeyword+1);
}

//����ؼ���
CWordNode* CWordTree::InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword)==0)
	{
		//����ؼ���
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}
	
	//�����ַ�
	CHAR chFirst=pszSubKeyword[0];
    CWordNode* pFirstNode=pParent->InsertChild(chFirst);

	//�����֧
    if (pFirstNode!=NULL && lstrlenA(pszSubKeyword)>0)
    {
        return InsertBranch(pFirstNode,pszKeyword,pszSubKeyword+1);
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////

//���캯��
CWordFilter::CWordFilter()
{
	ResetFilter();
}

//��������
CWordFilter::~CWordFilter()
{
}

//��������
VOID CWordFilter::ResetFilter()
{	
	//���ñ���
	m_szFilter[0]=0;

	//��������
	m_WordTree.RebuildTree();
}

//���˺���
bool CWordFilter::FilterWords(LPCSTR pszKeyword)
{
	//����У��
	if (pszKeyword==NULL) return false;

	//��������	
	int nLength=lstrlenA(pszKeyword);

	//���ҹؼ���
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

//���˺���
bool CWordFilter::FilterWords(LPCWSTR pszKeyword)
{
	//�ַ�ת��
	CW2A pszAnsKeyword(pszKeyword);

	return FilterWords(pszAnsKeyword);
}

//���˺���
bool CWordFilter::FilterWords(CHAR szKeyWord[], WORD wBufferSize)
{
	//����У��
	if (szKeyWord[0]==0) return false;

	//��������	
	LPCSTR pszKeyword=szKeyWord;
	
	//��������
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

	//���ý�����	
	szKeyWord[nDstLen]=0;

	return true;
}

//���˺���
bool CWordFilter::FilterWords(WCHAR szKeyWord[], WORD wBufferSize)
{
	//����У��
	if (szKeyWord[0]==0) return false;

	//��������
	CW2A pszKeyword(szKeyWord);
	int nLength=strlen(pszKeyword)+1;

	//�жϻ���
	if (nLength>=sizeof(m_szFilter)) return false;

	//�����ַ���
	lstrcpynA(m_szFilter,pszKeyword,nLength);

	//�����ַ�
	FilterWords(m_szFilter,nLength);

	//�����ַ���
	lstrcpynW(szKeyWord,CA2W(m_szFilter),nLength);	

	return true;
}

//����ؼ���
bool CWordFilter::InsertKeyword(LPCSTR pszKeyWord)
{
	return m_WordTree.Insert(pszKeyWord)!=NULL;
}

//����ؼ���
bool CWordFilter::InsertKeyword(LPCWSTR pszKeyWord)
{
	return m_WordTree.Insert(CW2A(pszKeyWord))!=NULL;
}

////////////////////////////////////////////////////////////////////////////////////