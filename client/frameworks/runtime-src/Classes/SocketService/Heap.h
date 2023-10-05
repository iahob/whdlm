#pragma once

#include <vector>

template<class T>
struct Less
{
	bool operator()(const T& left, const T& right, bool bLess = false)
	{
		if(bLess)
			return left < right;
		else
			return left > right;
	}
};

//堆
template<class T, class Compare = Less<T>>	//默认为大堆
class Heap
{
public:
	Heap() {}
	Heap(std::vector<T> &list, size_t size)
	{
		//重置大小
		_heap.reserve(size);

		//初始化
		for(int i=0; i<size; ++i)
		{
			_heap.push_back(list[i]);
		}
		//建堆
		int j=(((_heap.size())-2)/2)+1;
		do{
			AdjustDown(--j);
		}while( j > 0);
	}

	//向下调整
	void AdjustDown(int root)	//向下调整
	{
		int parent = root;
		size_t child = 2 * root | 1;
		while (child < _heap.size())
		{
			Compare cmp;
			//选左右孩子中较大的一个
			if ((child+1) < _heap.size() && cmp(_heap[child+1], _heap[child]))
			{
				++child;
			}
			//选父节点与孩子节点中较大的一个
			if (cmp(_heap[child], _heap[parent]))
			{
				//交换
				swap(_heap[child], _heap[parent]);

				parent = child;
				child = 2 * parent + 1;
			}
			else
				break;
		}
	}
	void Push(const T& x)
	{
		_heap.push_back(x);			//在最后插入一个数字
		if(_heap.size() >= 5)
			AdjustUp(_heap.size()-1);	//从最后插入数字的位置向上调整
	}
	void Pop()
	{
		swap(_heap[0],_heap[_heap.size()-1]);	//交换堆顶与最后一个元素
		_heap.pop_back();	//删除最后一个元素(删除了堆顶的元素)
		AdjustDown(0);		//从第一个数向下调整
	}

	size_t Size()
	{
		size_t nSize = _heap.size();
		return nSize;
	}
	const T& Top()
	{
		return _heap[0];
	}
protected:
	
	void AdjustUp(int child)		//向上调整
	{
		
		int nCount = 0;
		do
		{
			nCount = child - 1;
			Compare cmp;
			int parent = (child - 1) / 2;
			if (cmp(_heap[parent], _heap[child], false))	//孩子结点大于父节点就交换
			{
				swap(_heap[parent], _heap[child]);
				child = (child - 1) / 2;
			}
			else
				break;
		} while (nCount > 1);
	}
protected:
	vector<T> _heap;
};