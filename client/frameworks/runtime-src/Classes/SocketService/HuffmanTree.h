#define  _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
using namespace std;
#include <string>
#include "Heap.h"

template<class T>
struct ThatLess
{
	bool operator()( T* left,  T* right, bool bLess = true)
	{
		if (bLess)
			return left->_weight < right->_weight;
		else
			return left->_weight > right->_weight;
	}
};


template<class T>
struct HuffmanTreeNode
{
public:
	HuffmanTreeNode()
	{}
	HuffmanTreeNode(const T& weight)
		:_left(NULL)
		,_right(NULL)
		,_parent(NULL)
		,_weight(weight)
	{
	}

	T _weight;							//Ȩֵ
	unsigned char		_ch;
	HuffmanTreeNode<T>* _parent;		//���׽��
	HuffmanTreeNode<T>* _left;			//����
	HuffmanTreeNode<T>* _right;			//�Һ���
	
};

template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
public:
	HuffmanTree()
	{}
	HuffmanTree(T* pObj, size_t size, const T& invalid)
	{
		std::vector<Node*> list;

		for (size_t i = 0; i<size; ++i)	//��С�ѣ�����С��ֵ���ڶѶ�
		{
			if (pObj[i] != invalid)
			{
				Node* node = new Node(pObj[i]);
				node->_ch = pObj[i]._ch;
				list.emplace_back(node);
			}
		}

		Heap<Node*, ThatLess<Node>> minheap(list, list.size());

		if (minheap.Size() >= 2)
		{
			//������������
			while (minheap.Size() > 1)
			{
				Node* left = minheap.Top();		//�ֱ�ȡ��С������ֵ��left��right
				minheap.Pop();
				Node* right = minheap.Top();
				minheap.Pop();

				Node* parent = new Node(left->_weight + right->_weight);	//�������ڵ�
				parent->_ch = 0;
				parent->_left = left;
				parent->_right = right;

				left->_parent = parent;
				right->_parent = parent;

				minheap.Push(parent);
			}

			_root = minheap.Top();
		}
	}

	~HuffmanTree()
	{
		_destoryNode(_root);
	}

	Node* GetRoot()
	{
		return _root;
	}
protected:

	void _destoryNode(Node* root)
	{
		if (root)
		{
			_destoryNode(root->_left);
			_destoryNode(root->_right);
			delete root;
			root = NULL;
		}
	}
protected:
	Node* _root;
};