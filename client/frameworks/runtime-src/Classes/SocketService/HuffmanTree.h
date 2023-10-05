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

	T _weight;							//权值
	unsigned char		_ch;
	HuffmanTreeNode<T>* _parent;		//父亲结点
	HuffmanTreeNode<T>* _left;			//左孩子
	HuffmanTreeNode<T>* _right;			//右孩子
	
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

		for (size_t i = 0; i<size; ++i)	//建小堆，将最小的值放在堆顶
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
			//构建哈夫曼树
			while (minheap.Size() > 1)
			{
				Node* left = minheap.Top();		//分别取最小的两个值给left和right
				minheap.Pop();
				Node* right = minheap.Top();
				minheap.Pop();

				Node* parent = new Node(left->_weight + right->_weight);	//构建父节点
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