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

//��
template<class T, class Compare = Less<T>>	//Ĭ��Ϊ���
class Heap
{
public:
	Heap() {}
	Heap(std::vector<T> &list, size_t size)
	{
		//���ô�С
		_heap.reserve(size);

		//��ʼ��
		for(int i=0; i<size; ++i)
		{
			_heap.push_back(list[i]);
		}
		//����
		int j=(((_heap.size())-2)/2)+1;
		do{
			AdjustDown(--j);
		}while( j > 0);
	}

	//���µ���
	void AdjustDown(int root)	//���µ���
	{
		int parent = root;
		size_t child = 2 * root | 1;
		while (child < _heap.size())
		{
			Compare cmp;
			//ѡ���Һ����нϴ��һ��
			if ((child+1) < _heap.size() && cmp(_heap[child+1], _heap[child]))
			{
				++child;
			}
			//ѡ���ڵ��뺢�ӽڵ��нϴ��һ��
			if (cmp(_heap[child], _heap[parent]))
			{
				//����
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
		_heap.push_back(x);			//��������һ������
		if(_heap.size() >= 5)
			AdjustUp(_heap.size()-1);	//�����������ֵ�λ�����ϵ���
	}
	void Pop()
	{
		swap(_heap[0],_heap[_heap.size()-1]);	//�����Ѷ������һ��Ԫ��
		_heap.pop_back();	//ɾ�����һ��Ԫ��(ɾ���˶Ѷ���Ԫ��)
		AdjustDown(0);		//�ӵ�һ�������µ���
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
	
	void AdjustUp(int child)		//���ϵ���
	{
		
		int nCount = 0;
		do
		{
			nCount = child - 1;
			Compare cmp;
			int parent = (child - 1) / 2;
			if (cmp(_heap[parent], _heap[child], false))	//���ӽ����ڸ��ڵ�ͽ���
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