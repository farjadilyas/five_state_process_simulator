#include<sstream>
#include<iostream>
#include<unistd.h>
#include<iomanip>
#include<climits>
#include<ctime>
#include<cstring>
#include<pthread.h>

using namespace std;

enum ALGOS
{
	ERR = 0, FCFS = 1, RR = 2, SJF = 3, SRTF = 4
};

enum ORDER
{
	ARRIVAL = 1, BURST = 2, REM_JOB = 3
};



//-------------------------- [[ VECTOR DATA STRUCTURE ]] -----------------------


#ifndef VECTOR_H
template<class T>
class Vector
{
private:
	T* data;
	int capacity;
	int size;
public:
	Vector()
	{
		data = nullptr;
		capacity = 0;
		size = 0;
	}
	Vector(const int& capacity)
	{
		data = new T[capacity];
		this->capacity = capacity;
		size = 0;
	}
	Vector(const Vector<T>& obj)
	{
		data = nullptr;
		Initialize(obj);
	}
	void Initialize(const Vector<T>& obj)
	{
		if (this != &obj)
		{
			if (size != 0)
				delete[] data;
			capacity = obj.getCapacity();
			size = obj.Size();
			data = new T[capacity];
			for (int a = 0; a < size; ++a)
			{
				data[a] = obj[a];
			}
		}
	}
	T& operator[](const int& indx) const
	{
		if (indx < capacity)
			return data[indx];
		else
			cout << "---[[ Warning (operator[]): Index too large ]]---\n";

	}
	int getCapacity() const
	{
		return capacity;
	}
	int Size() const
	{
		return size;
	}
	void push_back(const T& new_item)
	{
		if (capacity == 0)
		{
			capacity = 10;
			data = new T[capacity];
		}
		if (size == capacity)
		{
			resize(capacity * 2);
		}
		data[size++] = new_item;
	}
	void pop_back()
	{
		--size;
		if (size < capacity / 2)
		{
			resize(capacity / 2);
		}
	}
	void print() const
	{
		if (size > 0)
			cout << *this;
		else
			printf("\nThe vector is empty\n");
	}
	friend ostream& operator<<(ostream& out, const Vector<T> obj)
	{
		for (int i = 0; i < obj.size; ++i)
		{
			cout << obj.data[i] << " ";
		}
		return out;
	}
	void clear()
	{
		capacity = 0;
		size = 0;
		delete[] data;
	}
	void setSize(int size)
	{
		this->size = size;
	}
	void resize(int new_capacity)
	{
		T* newarr = new T[new_capacity];
		if (new_capacity < capacity) { capacity = new_capacity; }
		for (int a = 0; a < size; ++a)
		{
			newarr[a] = data[a];
		}
		if (size != 0)
		{
			delete[] data;
		}
		data = newarr;
		capacity = new_capacity;
	}
	void operator=(const Vector<T>& obj)
	{
		Initialize(obj);
	}
	int find(T item)
	{
		for (int i = 0; i < size; ++i)
		{
			if (data[i] == item)
				return 1;
		}
		return -1;
	}
	bool is_empty() const
	{
		return (size == 0);
	}
	~Vector()
	{
		clear();
	}
};

#endif // VECTOR_H








//-------------------------------------------------- [[ HEAP DATA STRUCTURE ]]----------------------------------


#ifndef HEAP_H
#define HEAP_H



template<class Type>
struct HeapNode
{
	Type data;
	HeapNode()
	{
		//data = -1;
	}
	HeapNode(const Type& data)
	{
		this->data = data;
	}
	HeapNode(const HeapNode& obj)
	{
		Initialize(obj);
	}
	void Initialize(const HeapNode& obj)
	{
		data = obj.data;
	}
	bool operator<(const HeapNode<Type>& obj)
	{
		return (data < obj.data);
	}
	bool operator<=(const HeapNode<Type>& obj)
	{
		return (data <= obj.data);
	}
	bool operator>(const HeapNode<Type>& obj)
	{
		return (data > obj.data);
	}
	bool operator>=(const HeapNode<Type>& obj)
	{
		return (data >= obj.data);
	}
	bool operator==(const HeapNode<Type>& obj)
	{
		return (data == obj.data);
	}
	bool operator!=(const HeapNode<Type>& obj)
	{
		return (data != obj.data);
	}
	void operator=(const HeapNode<Type>& obj)
	{
		Initialize(obj);
	}
	void swap(const HeapNode<Type>& obj)
	{
		Type temp = data;
		data = obj.data;
		obj.data = temp;
	}
	friend ostream& operator<<(ostream& out, const HeapNode& obj)
	{
		out << obj.data;
		return out;
	}
};

template<class Type>
class Heap
{
public:
	Vector<HeapNode<Type>>queue;
public:
	Heap(int size = 10) : queue(size)
	{
		HeapNode<Type> temp;
		queue.push_back(temp);
	}
	void insert_data(const Type &data)
	{
		HeapNode<Type> new_elem(data);
		HeapNode<Type> temp;

		int parent,i;
		queue.push_back(new_elem);
		i = queue.Size() - 1;			// going to the index where element is inserted
		parent = i;
		while (1)						// loop until the element reaches the place where it is less than it's child
		{
			parent = parent / 2;
	
			if (parent <= 0)
				break;
			if (!(queue[parent] <= queue[i]))// swap if parent>child	//
			{
				temp = queue[i];
				queue[i] = queue[parent];
				queue[parent] = temp;
				i = parent;

			}
		}
	}

	void display()
	{
		for (int i = 1;i < queue.Size();i++)
			cout << queue[i] << "   ";
		cout << "\n";
	}
	Type getMinData()
	{
		return (queue.Size()>1)? queue[1].data : 0 ;
	}
	Type delete_min()
	{
		int last = queue.Size() - 1;

		int parent = 1;
		HeapNode<Type> hold;
		hold = queue[parent];
		queue[parent] = queue[last];
		queue[last] = hold;
		
		queue.pop_back();// removing root

		if (isEmpty())
			return hold.data;

		int new_last = queue.Size() - 1;

		//cout << "last element =" << queue[queue.Size()-1] << endl;
		int left_child;
		int right_child;
 
		while (1)
		{
			left_child=parent* 2;
			right_child =(parent*2)+1;
			//cout << left_child;

				//case for right child//
			if (right_child < queue.Size() && (queue[right_child] < queue[left_child]) && (queue[parent] > queue[right_child]))
			{
				HeapNode<Type> temp = queue[right_child];
				queue[right_child] = queue[parent];
				queue[parent] = temp;
				parent = right_child;
			}

			//case for left child //
			else if (left_child < queue.Size() && queue[left_child] < queue[parent] && (right_child >= queue.Size() || queue[left_child] < queue[right_child]))
			{

				HeapNode<Type> temp = queue[left_child];
				queue[left_child] = queue[parent];
				queue[parent] = temp;
				parent = left_child;
			}
			else
				break;
				
		}
		return hold.data;
	}
	int getSize() const
	{
		return queue.Size();
	}
	bool isEmpty() const
	{
		return (queue.Size() == 1);
	}


};


#endif


//------------------------------------ [[ END OF HEAP DATA STRUCTURE ]] ---------------------------------------


/*
class BlockVector : public Vector<BlockedItem>
{

public:
	void incBlockTime()
	{
		for (int i = 0 ; i < size; ++i)
		{
			++data[i].block_t;
		}
	}

	Vector<BlockedItem> extractUnblocked()
	{
		for (int i = 0 ; i < size; ++i)
		{

		}
	}
};
*/




//------------------------------------------ [[ PROC DATA STRUCTURE ]] -------------------------------

struct Proc 
{
    //Public members

    char name[8];
    int job_t;
    int arrival_t;
    int burst_t;
	int latest_arrival_t;
    Vector<int> burst_start_t;
    Vector<int> burst_end_t;
    int priority;
    int waiting_t;
    int completion_t;
	int order_by;

    //Constructors

	bool operator<(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t < obj.arrival_t);
			case(2):
				return (burst_t < obj.burst_t);
			case(3):
				return (job_t < obj.job_t);
		}
	}

	bool operator<=(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t <= obj.arrival_t);
			case(2):
				return (burst_t <= obj.burst_t);
			case(3):
				return (job_t <= obj.job_t);
		}
	}

	bool operator>(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t > obj.arrival_t);
			case(2):
				return (burst_t > obj.burst_t);
			case(3):
				return (job_t > obj.job_t);
		}
	}

	bool operator>=(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t >= obj.arrival_t);
			case(2):
				return (burst_t >= obj.burst_t);
			case(3):
				return (job_t >= obj.job_t);
		}
	}

	bool operator==(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t == obj.arrival_t);
			case(2):
				return (burst_t == obj.burst_t);
			case(3):
				return (job_t == obj.job_t);
		}
	}

	bool operator!=(const Proc &obj) const
	{
		switch(order_by)
		{
			case(1):
				return (arrival_t != obj.arrival_t);
			case(2):
				return (burst_t != obj.burst_t);
			case(3):
				return (job_t != obj.job_t);
		}
	}

    Proc()
    {
        Initialize(NULL, 0, 0,0,0);
    }
    Proc(char *name, int job, int arrival = 0, int burst = 0, int priority = 0)
    {
        Initialize(name, job, arrival, burst, priority);
    }
    void addBurst(int start_t, int end_t)
    {
		//printf("\nWaiting time: %d, Start time: %d, Latest arrival time: %d\n", waiting_t, start_t, latest_arrival_t);
        waiting_t += start_t - latest_arrival_t;
        job_t -= end_t - start_t;
    }

    bool complete(int cur_t = 0)
    {
        return (job_t - cur_t == 0);
    }
    Proc(const Proc &obj)
    {
        Initialize(obj);
    }
    void Initialize(const Proc &obj)
    {
        Initialize(obj.name, obj.job_t, obj.arrival_t, obj.burst_t, obj.priority);
		waiting_t = obj.waiting_t;
		completion_t = obj.completion_t;
		latest_arrival_t = obj.latest_arrival_t;
		order_by = obj.order_by;
        //burst_start_t = obj.burst_start_t;
        //burst_end_t = obj.burst_end_t;
    }
    void Initialize(const char *name, int job, int arrival, int burst, int priority)
    {
        if (name)
            for (int i = 0 ; i < 8; ++i)
                this->name[i] = name[i];
        
        job_t = job;
        arrival_t = arrival;
        burst_t = burst;
        this->priority = priority;
        waiting_t = 0;
        completion_t = 0;
		latest_arrival_t = arrival;
		order_by = 1;				// NOTICE THIS ORDER BY IF YOURE HAVING PROBLEMS
		//fcfs = true;
    }

	void setLatestArrivalTime(int latest_arrival_t)
	{
		this->latest_arrival_t = latest_arrival_t;
	}

    void operator=(const Proc &obj)
    {
        Initialize(obj);
    }

	void display()
	{
		cout<<setfill(' ')<<setw(10)<<left<<name<<setw(9)<<job_t<<setw(13)<<arrival_t<<setw(11)<<burst_t<<setw(10)<<waiting_t<<setw(12)<<completion_t<<"\n";
		//printf("--- --- --- [[ %s ]] --- --- ---\nJob time: %i\nArrival time: %i\nLatest Arrival Time: %d\nBurst time: %i\nPriority: %i\nWaiting time: %i\nCompletion time: %i\n",name,job_t,arrival_t,latest_arrival_t, burst_t,priority,waiting_t,completion_t);
	}

    friend ostream& operator<<(ostream &out, const Proc &obj)
    {
		if (obj.job_t == 0)
		{
			out<<setfill(' ')<<setw(10)<<left<<obj.name<<setw(13)<<obj.arrival_t<<setw(11)<<obj.burst_t<<setw(10)<<obj.waiting_t<<setw(12)<<obj.completion_t<<setw(16)<<obj.completion_t - obj.arrival_t<<"\n";
		}
		else
		{
			out<<setfill(' ')<<setw(10)<<left<<"P_NAME"<<setw(9)<<"JOB_T"<<setw(13)<<"ARRIVAL_T"<<setw(11)<<"BURST_T"<<setw(10)<<"WAIT_T"<<setw(12)<<"FINISH_T"<<"\n"
			<<setfill(' ')<<setw(10)<<left<<obj.name<<setw(9)<<obj.job_t<<setw(13)<<obj.arrival_t<<setw(11)<<obj.burst_t<<setw(10)<<obj.waiting_t<<setw(12)<<obj.completion_t<<"\n";
		}

		/*
		out
        <<"--- --- --- [[ "<<obj.name<<" ]] --- --- ---\nJob time: "<<obj.job_t
        <<"\nArrival Time: "<<obj.arrival_t<<"\nLatest Arrival Time: "<<obj.latest_arrival_t
		<<"\nBurst Time: "<<obj.burst_t<<"\nPriority: "<<obj.priority
        <<"\nWaiting Time: "<<obj.waiting_t<<"\nCompletion Time: "
        <<obj.completion_t<<"\n\n";
		*/
		
        return out;
    }
};




//------------------------------------------ [[ BLOCKED ITEM ]] -----------------------------------


struct BlockedItem
{
	Proc process;
	int block_t;
	int block_lim;

	BlockedItem()
	{
		block_t = 0;
		block_lim = 0;
	}
	BlockedItem(const Proc &obj, int block_t, int block_lim)
	{
		Initialize(obj, block_t, block_lim);
	}
	BlockedItem(const BlockedItem &obj)
	{
		Initialize(obj.process, obj.block_t, obj.block_lim);
	}
	void Initialize(const Proc &obj, int block_t, int block_lim)
	{
		//printf("ERROR 1 --------------------------\n");
		//cout<<obj;
		process = obj;
		//printf("ERROR 2 --------------------------\n");
		this->block_t = block_t;
		//printf("ERROR 3 --------------------------\n");
		this->block_lim = block_lim;
		//printf("ERROR 4 --------------------------\n");
	}
	void Initialize(const BlockedItem &obj)
	{
		Initialize(obj.process, obj.block_t, obj.block_lim);
	}

	bool unblocked()
	{
		return (block_t >= block_lim - 1 && block_lim != 0);
	}

	void setBlockDetails(int time)
	{
		block_lim = time;
		block_t = 0;
		process.latest_arrival_t = process.latest_arrival_t + time;
	}

	void operator=(const BlockedItem &obj)
	{
		Initialize(obj);
	}

	friend ostream& operator<<(ostream &out, const BlockedItem &obj)
	{
		out<<obj.process<<"Blocked time: "<<obj.block_t<<"\nBlock Limit: "<<obj.block_lim<<"\n\n";
		return out;
	}
};





//------------------------------------ [[ QUEUE DATA STRUCTURE ]] ----------------------------------------

//===================================
//              QUEUE NODE
//===================================

template<class T>
class QueueNode
{
public:
	QueueNode* next;
	T data;
	QueueNode();
	QueueNode(T data);
	QueueNode(const QueueNode<T>&);
	void operator=(const QueueNode<T>&);
    bool operator<(const QueueNode<T>&);
    bool operator>(const QueueNode<T>&);
    bool operator<=(const QueueNode<T>&);
    bool operator>=(const QueueNode<T>&);
    bool operator==(const QueueNode<T>&);
    bool operator!=(const QueueNode<T>&);
	void Initialize(const QueueNode<T>&);
	void display();
};


template<class T>
QueueNode<T>::QueueNode()
{
	next = nullptr;
}
template<class T>
QueueNode<T>::QueueNode(T data)
{
	this->data = data;
	next = nullptr;
}

template<class T>
void QueueNode<T>::display()
{
	std::cout << data;
}

template<class T>
QueueNode<T>::QueueNode(const QueueNode<T>& obj)
{
	Initialize(obj);
}

template<class T>
bool QueueNode<T>::operator<(const QueueNode<T>& obj)
{
    return (data < obj.data);
}

template<class T>
bool QueueNode<T>::operator>(const QueueNode<T>& obj)
{
    return (data > obj.data);
}

template<class T>
bool QueueNode<T>::operator==(const QueueNode<T>& obj)
{
    return (data == obj.data);
}

template<class T>
bool QueueNode<T>::operator!=(const QueueNode<T>& obj)
{
    return (data != obj.data);
}

template<class T>
bool QueueNode<T>::operator<=(const QueueNode<T>& obj)
{
    return (data <= obj.data);
}

template<class T>
bool QueueNode<T>::operator>=(const QueueNode<T>& obj)
{
    return (data >= obj.data);
}

template<class T>
void QueueNode<T>::Initialize(const QueueNode<T>& obj)
{
	this->data = obj.data;
	next = nullptr;
}

template<class T>
void QueueNode<T>::operator=(const QueueNode<T> & obj)
{
	Initialize(obj);
}




//====================================
//              QUEUE DS
//====================================

#pragma once
#ifndef QUEUE_H
#define QUEUE_H
template<class T>
class Queue
{
private:
	QueueNode<T>* QueueNodes;
public:
	QueueNode<T>* front;
	QueueNode<T>* rear;
	int length;
	Queue();
	T& getFront();
	void enqueue();
	void enqueue(const QueueNode<T>& newDataItem);
	void enqueue(T data);
	QueueNode<T> dequeue();
	void clear();
	int getLength() const;
	bool isEmpty() const;
	void display() const;
	void operator=(const Queue<T>& obj);
	Queue<T>& operator+(const QueueNode<T>& newDataItem);
	Queue<T>& operator+(const T& data);
	Queue<T>& operator--(int);
	~Queue();
};

template<class T>
Queue<T>::Queue()
{
	front = nullptr; //dequeue
	rear = nullptr; //enqueue
	QueueNodes = nullptr;
	length = -1;
}

template<class T>
T& Queue<T>::getFront()
{
	return front->data;
}

template<class T>
void Queue<T>::enqueue()
{
	QueueNode<T>* newQueueNode = new QueueNode<T>;
	if (isEmpty())
	{
		rear = newQueueNode;
		front = newQueueNode;
	}
	else
	{
		front->next = newQueueNode;
		front = newQueueNode;
	}
	++length;
}
template<class T>
void Queue<T>::enqueue(const QueueNode<T>& newDataItem)
{
	QueueNode<T>* newQueueNode = new QueueNode<T>(newDataItem);
	if (isEmpty())
	{
		rear = newQueueNode;
		front = newQueueNode;
	}
	else
	{
		front->next = newQueueNode;
		front = newQueueNode;
	}
	++length;
}

template<class T>
void Queue<T>::enqueue(T data)
{
	QueueNode<T>* newQueueNode = new QueueNode<T>(data);
	if (isEmpty())
	{
		rear = newQueueNode;
		front = newQueueNode;
	}
	else
	{
		front->next = newQueueNode;
		front = newQueueNode;
	}
	++length;
}

template<class T>
QueueNode<T> Queue<T>::dequeue()
{
	if (!isEmpty())
	{
		QueueNode<T> obj(*rear);
		QueueNode<T>* del_this = rear;
		rear = rear->next;
		if (!rear)
		{
			front = nullptr;
		}
		delete del_this;
		--length;
		return obj;
	}
}

template<class T>
int Queue<T>::getLength() const
{
	return length;
}

template<class T>
void Queue<T>::clear()
{
	QueueNode<T>* iter = rear;
	QueueNode<T>* placeholder = nullptr;
	while (iter)
	{
		placeholder = iter;
		iter = iter->next;
		delete placeholder;
	}
	rear = nullptr;
	front = nullptr;
}
template<class T>
bool Queue<T>::isEmpty() const
{
	if (front == nullptr && rear == nullptr)
	{
		return true;
	}
	return false;
}

template<class T>
void Queue<T>::operator=(const Queue<T>& obj)
{
	if (&obj != this)
	{
		clear();
		while (!obj.isEmpty())
		{
			//enqueue(obj.dequeue().data);
		}
	}
}

template<class T>
Queue<T>& Queue<T>::operator+(const QueueNode<T>& newDataItem)
{
	enqueue(newDataItem);
	return *this;
}

template<class T>
Queue<T>& Queue<T>::operator+(const T& data)
{
	enqueue(data);
	return *this;
}

template<class T>
Queue<T>& Queue<T>::operator--(int)
{
	dequeue();
	return *this;
}

template<class T>
void Queue<T>::display() const
{
	QueueNode<T>* iter = rear;
	std::cout << "[ ";
	while (iter)
	{
		iter->display();
		iter = iter->next;
		std::cout << ((iter) ? ", " : " ");
	}
	std::cout << "]\n";
}

template<class T>
Queue<T>::~Queue()
{
	clear();
}


class BlockQueue : public Queue<BlockedItem>
{
	public:
	void incBlockTime()
	{
		int len = getLength();
		QueueNode<BlockedItem>* iter = rear;

		while (iter)
		{
			(*iter).data.block_t = (*iter).data.block_t + 1;
			iter = iter->next;
		}
	}

	Vector<BlockedItem> extractUnblocked()
	{
		Vector<BlockedItem> ret;
		QueueNode<BlockedItem> * iter = rear, *del_this = nullptr, *prev = nullptr;
		while (iter)
		{
			if ((*iter).data.unblocked())
			{
				//printf("\nThis is happening!\n");
				if (iter == rear)
				{
					//printf("\nAjeeb\n");
					del_this = iter;
					iter = iter->next;
					rear = iter;
					prev = nullptr;

					//printf("\nHEREEEE 1\n");
					ret.push_back((*del_this).data);
					//printf("\nHEREEEEE 2\n");
					//delete del_this;
					//printf("\nHEREEEE 3\n");
				}
				else
				{
					//printf("\nWot??\n");
					del_this = iter;
					iter = iter->next;
					prev->next = iter;
					ret.push_back((*del_this).data);
					//delete del_this;
				}

				//printf("\nThis is the vector\n");
				//cout<<ret;

				if (!rear)
					front = nullptr;
				
				--length;
			}
			else
			{
				prev = iter;
				iter = iter->next;
			}
		}

		return ret;
	}
};





//====================================
//              PRIORITY QUEUE DS
//====================================

template<class T>
class PriorityQueue
{
private:
	QueueNode<T>* QueueNodes;
public:
	QueueNode<T>* front;
	QueueNode<T>* rear;
	int length;
	PriorityQueue();
	T& getFront();
	T& getRear();
	void enqueue();
	void enqueue(const QueueNode<T>& newDataItem);
	void enqueue(T data);
	QueueNode<T> dequeue();
	void clear();
	int getLength() const;
	bool isEmpty() const;
	void display() const;
	void operator=(const PriorityQueue<T>& obj);
	PriorityQueue<T>& operator+(const QueueNode<T>& newDataItem);
	PriorityQueue<T>& operator+(const T& data);
	PriorityQueue<T>& operator--(int);
	~PriorityQueue();
};

template<class T>
PriorityQueue<T>::PriorityQueue()
{
	front = nullptr; //dequeue
	rear = nullptr; //enqueue
	QueueNodes = nullptr;
	length = -1;
}

template<class T>
T& PriorityQueue<T>::getFront()
{
	return front->data;
}

template<class T>
T& PriorityQueue<T>::getRear()
{
	return rear->data;
}

template<class T>
void PriorityQueue<T>::enqueue(const QueueNode<T>& newDataItem)
{
	QueueNode<T>* newQueueNode = new QueueNode<T>(newDataItem);

    QueueNode<T>* iter = rear, *prev = nullptr;
    bool elem_found = false;

    while (iter && !elem_found)
    {
        if (*iter < *newQueueNode)
        {
            prev = iter;
            iter = iter->next;
        }
        else
        {
            elem_found = true;
        }
    }

    if (iter == nullptr)
    {
        if (prev == nullptr)
        {
            rear = newQueueNode;
            front = newQueueNode;
        }
        else
        {
            iter = newQueueNode;
            prev->next = iter;
        }
    }
    else
    {
        if (prev == nullptr)
        {
            newQueueNode->next = rear;
            rear = newQueueNode;
        }
        else
        {
            newQueueNode->next = iter;
            prev->next = newQueueNode;
        }
    }

	++length;
}

template<class T>
void PriorityQueue<T>::enqueue(T data)
{
	QueueNode<T>* newQueueNode = new QueueNode<T>(data);
	QueueNode<T>* iter = rear, *prev = nullptr;
    bool elem_found = false;

    while (iter && !elem_found)
    {
        if (*iter < *newQueueNode)
        {
            prev = iter;
            iter = iter->next;
        }
        else
        {
            elem_found = true;
        }
    }

    if (iter == nullptr)
    {
        if (prev == nullptr)
        {
            rear = newQueueNode;
            front = newQueueNode;
        }
        else
        {
            iter = newQueueNode;
            prev->next = iter;
        }
    }
    else
    {
        if (prev == nullptr)
        {
            newQueueNode->next = rear;
            rear = newQueueNode;
        }
        else
        {
            newQueueNode->next = iter;
            prev->next = newQueueNode;
        }
    }

	++length;
}

template<class T>
QueueNode<T> PriorityQueue<T>::dequeue()
{
	if (!isEmpty())
	{
		QueueNode<T> obj(*rear);
		QueueNode<T>* del_this = rear;
		rear = rear->next;
		if (!rear)
		{
			front = nullptr;
		}
		delete del_this;
		--length;
		return obj;
	}
}

template<class T>
int PriorityQueue<T>::getLength() const
{
	return length;
}

template<class T>
void PriorityQueue<T>::clear()
{
	QueueNode<T>* iter = rear;
	QueueNode<T>* placeholder = nullptr;
	while (iter)
	{
		placeholder = iter;
		iter = iter->next;
		delete placeholder;
	}
	rear = nullptr;
	front = nullptr;
}
template<class T>
bool PriorityQueue<T>::isEmpty() const
{
	if (front == nullptr && rear == nullptr)
	{
		return true;
	}
	return false;
}

template<class T>
void PriorityQueue<T>::operator=(const PriorityQueue<T>& obj)
{
	if (&obj != this)
	{
		clear();
		while (!obj.isEmpty())
		{
			//enqueue(obj.dequeue().data);
		}
	}
}

template<class T>
void PriorityQueue<T>::display() const
{
	QueueNode<T>* iter = rear;
	std::cout << "[ ";
	while (iter)
	{
		iter->display();
		iter = iter->next;
		std::cout << ((iter) ? ", " : " ");
	}
	std::cout << "]\n";
}

template<class T>
PriorityQueue<T>::~PriorityQueue()
{
	clear();
}


#endif






//--------------------------- [[ LIST DATA STRUCTURE ]] -------------------------------------


//====================================
//              LIST NODE
//====================================


template<class T>
class Node
{
private:

public:
	T data;
	Node* next;
	Node(T data);
	Node(const Node& obj);
	void Initialize(T data);
	void Initialize(const Node& obj);
	void operator=(const Node& obj);
	bool operator==(const Node& obj);
	bool operator<(const Node& obj);
	bool operator>(const Node& obj);
	friend ostream& operator<<(ostream& out, const Node<T>& obj)
	{
		out << obj.data;
		return out;
	}
	void printNode();
};

template<class T>
Node<T>::Node(T dt) : data(dt)
{
	next = nullptr;
}

template<class T>
Node<T>::Node(const Node& obj)
{
	data = obj.data;
	next = nullptr;
}

template<class T>
void Node<T>::Initialize(T data)
{
	this->data = data;
}

template<class T>
void Node<T>::Initialize(const Node& obj)
{
	this->data = obj.data;
}

template<class T>
void Node<T>::operator=(const Node& obj)
{
	data = obj.data;
	next = obj.next;
}

template<class T>
bool Node<T>::operator==(const Node& obj)
{
	if (data == obj.data)
	{
		return true;
	}
	else
	{
		return false;
	}

}

template<class T>
bool Node<T>::operator<(const Node& obj)
{
	if (data < obj.data)
	{
		return true;
	}
	else
	{
		return false;
	}

}

template<class T>
bool Node<T>::operator>(const Node& obj)
{
	if (data > obj.data)
	{
		return true;
	}
	else
	{
		return false;
	}

}

template<class T>
void Node<T>::printNode()
{
	cout << *this;
}




//====================================
//              LIST DS 
//====================================


// List Iterator class implementation

template<class T>
class ListIter
{
private:
	Node<T>* iter;
public:
	

	// Constructors
	ListIter()
	{
		iter = nullptr;
	}
	ListIter(Node<T>* const p)
	{
		iter = p;
	}

	// Operator overloading

	void operator=(Node<T>* const p)
	{
		iter = p;
	}
	void operator++()
	{
		iter = iter->next;
	}
	bool operator!=(const ListIter<T>& obj)
	{
		return (iter != obj.iter);
	}
	bool operator==(const ListIter<T>& obj)
	{
		return (iter == obj.iter);
	}
	bool operator>(const ListIter<T>& obj)
	{
		return (iter > obj.iter);
	}
	bool operator>=(const ListIter<T>& obj)
	{
		return (iter >= obj.iter);
	}
	bool operator<(const ListIter<T>& obj)
	{
		return (iter < obj.iter);
	}
	bool operator<=(const ListIter<T>& obj)
	{
		return (iter <= obj.iter);
	}
	T& operator*()
	{
		return iter->data;
	}
};
template<class T>
class List
{
private:
	Node<T>* nodes;
	Node<T>* head;
	int size;
	Node<T>* current;
public:
	List();
	List(const List& obj);
	void Initialize(const List& obj);
	bool isEmpty();
	void insert(const Node<T>& newDataItem);
	void insertAt(const Node<T>& newDataItem, int index);
	void remove();
	void removeAt(int index);
	void replace(const Node<T>& newDataItem);
	void replaceAt(const Node<T>& newDataItem, int index);
	void clear();
	bool gotoNext();
	bool gotoPrior();
	Node<T> getCursor();
	bool moveCursor(int index);
	bool moveCursortoEnd();
	void splitList(int index);
	void display() const;
	void sort();
	void nodesort();
	bool duplicate();
	void operator=(const List<T>&);
	friend ostream& operator<<(ostream& out, const List<T>& obj)
	{
		Node<T>* iter = obj.head;
		while (iter != NULL)
		{
			out << *iter;
			iter = iter->next;
		}
		out << "\n";
		return out;
	}
	int getSize();
	T& operator[](const int& index);
	ListIter<T> begin();
	ListIter<T> end();
	~List();
};

template<class T>
List<T>::List()
{
	head = nullptr;
	nodes = nullptr;
	current = nullptr;
	size = 0;
}


template<class T>
List<T>::List(const List& obj)
{
	if (this != &obj)
	{
		clear();
		Node<T>* cursor = obj.head;
		while (cursor)
		{
			insert(*cursor);
			cursor = cursor->next;
		}
	}
}

template<class T>
ListIter<T> List<T>::begin()
{
	ListIter<T> temp(head);
	return temp;
}

template<class T>
ListIter<T> List<T>::end()
{
	ListIter<T> temp(nullptr);
	return temp;
}

template<class T>
int List<T>::getSize()
{
	return size;
}

template<class T>
T& List<T>::operator[](const int& index)
{
	Node<T>* iter = head;
	for (int i = 0; i != index && iter != nullptr; ++i)
	{
		iter = iter->next;
	}
	return iter->data;
}

template<class T>
bool List<T>::isEmpty()
{
	if (head != nullptr)
	{
		return false;
	}
	return true;
}

template<class T>
void List<T>::Initialize(const List& obj)
{
	if (this != &obj)
	{
		clear();
		Node<T>* cursor = obj.head;
		while (cursor)
		{
			insert(*cursor);
			cursor = cursor->next;
		}
	}
}

template<class T>
void List<T>::operator=(const List<T>& obj)
{
	Initialize(obj);
}

template<class T>
void List<T>::insert(const Node<T>& newDataItem)
{

	if (isEmpty())
	{
		Node<T>* new_item = new Node<T>(newDataItem);
		head = new_item;
		current = new_item;
	}
	else
	{
		current = head;
		while (current->next != nullptr)
		{
			current = current->next;
		}
		Node<T>* new_item = new Node<T>(newDataItem);
		new_item->next = nullptr;
		current->next = new_item;
		current = new_item;
	}
	++size;
}

template<class T>
void List<T>::insertAt(const Node<T>& newDataItem, int index)
{
	Node<T>* new_item = new Node<T>(newDataItem);
	if (index == 0)
	{
		new_item->next = head;
		head = new_item;
	}
	else
	{
		int counter = 0;
		current = head;
		while (current != nullptr && counter != index)
		{
			current = current->next;
			++counter;
		}
		if (current)
		{
			new_item->next = current->next;
			current->next = new_item;
		}
		else
		{
			cout << "---[[ Warning (insertAt): The index provided is too large ]]---\n";
			return;
		}
	}
	++size;
}

template<class T>
void List<T>::remove()
{
	if (!isEmpty())
	{
		if (current == head)
		{
			head = head->next;
			delete current;
			current = head;
		}
		else
		{
			Node<T>* iter = head;
			while (iter->next != current)
			{
				iter = iter->next;
			}
			iter->next = current->next;
			delete current;
			if (iter->next == nullptr)
			{
				current = head;
			}
			else
			{
				current = iter->next;
			}
		}
	}
	--size;
}

template<class T>
void List<T>::removeAt(int index)
{
	if (!isEmpty())
	{
		if (index == 0)
		{
			Node<T>* temp;
			temp = head;
			head = head->next;
			delete temp;
		}
		else
		{
			int counter = 0;
			Node<T>* iter = head;
			while (iter != nullptr && counter != index - 1)
			{
				iter = iter->next;
				++counter;
			}
			if (counter == index - 1)
			{
				Node<T>* temp = iter->next;
				iter->next = iter->next->next;
				delete temp;
				if (iter->next == nullptr)
				{
					current = head;
				}
				else
				{
					current = iter->next;
				}
			}
			else
			{
				{
					cout << "---[[ Warning: Index provided is too large ]]---\n";
					return;
				}
			}
			--size;


		}
	}
}

template<class T>
void List<T>::replace(const Node<T>& newDataItem)
{
	if (!isEmpty())
	{
		current->Initialize(newDataItem);
	}
}

template<class T>
void List<T>::replaceAt(const Node<T>& newDataItem, int index)
{
	if (!isEmpty())
	{
		int counter = 0;
		current = head;
		while (current != nullptr && counter != index)
		{
			current = current->next;
			++counter;
		}
		if (counter == index)
		{
			current->Initialize(newDataItem);
		}
		else
		{
			cout << "---[[ Warning (replaceAt): Index out of range ]]---\n";
		}

	}
}

template<class T>
void List<T>::clear()
{
	current = head;
	Node<T>* iter = head;
	while (current)
	{
		iter = current;
		current = current->next;
		delete iter;
	}
	current = nullptr;
	head = nullptr;
	size = 0;
}

template<class T>
bool List<T>::gotoNext()
{
	if (!isEmpty())
	{
		if (current->next)
		{
			current = current->next;
		}
		else
		{
			return false;
		}

	}
	else
	{
		cout << "---[[ Warning (gotoNext): List is Empty ]]---\n";
		return false;
	}
}

template<class T>
bool List<T>::gotoPrior()
{
	if (!isEmpty())
	{
		if (current == head)
		{
			return false;
		}
		else
		{
			Node<T>* iter = head;
			while (iter->next != current)
			{
				iter = iter->next;
			}
			current = iter;
		}

	}
	else
	{
		cout << "---[[ Warning (gotoPrior): List is Empty ]]---\n";
		return false;
	}
}

template<class T>
Node<T> List<T>::getCursor()
{
	if (!isEmpty())
	{
		return *current;
	}
	cout << "---[[ Warning (getCursor): List is Empty ]]---\n";
}

template<class T>
bool List<T>::moveCursor(int index)
{
	if (!isEmpty())
	{
		current = head;
		int counter = 0;
		while (current != nullptr && counter != index)
		{
			current = current->next;
			++counter;
		}
		if (counter == index)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

template<class T>
bool List<T>::moveCursortoEnd()
{
	if (isEmpty())
	{
		return false;
	}
	else
	{
		current = head;
		while (current->next != nullptr)
		{
			current = current->next;
		}
		return true;
	}
}

template<class T>
void List<T>::display() const
{
	//cout << "[";
	Node<T>* iter = head;
	while (iter != NULL)
	{
		iter->printNode();
		iter = iter->next;
	}
	cout << "\n";
	//cout << "]\n";
}



template<class T>
bool List<T>::duplicate()
{
	if (!isEmpty())
	{
		Node<T>* iterO = head;
		Node<T>* iterI;

		while (iterO)
		{
			iterI = head;
			while (iterI)
			{
				if (iterO != iterI && *iterO == *iterI)
				{
					return true;
				}
				iterI = iterI->next;
			}
			iterO = iterO->next;
		}
	}
	else
	{
		cout << "---[[ Warning (duplicate): The list is Empty ]]---\n";
	}
	return false;

}

template<class T>
void List<T>::splitList(int index)
{
	List<int>li;
	if (!isEmpty() && head->next != nullptr)
	{
		int counter = 0;
		current = head;
		while (current != nullptr && counter != index)
		{
			current = current->next;
			++counter;
		}
		current = current->next;
		if (counter == index)
		{
			while (current != head)
			{
				li.insert(*current);
				remove();
			}
			cout << "List #1: ";
			display();
			cout << "\nList #2: ";
			li.display();
			cout << "\n";
		}
		else
		{
			cout << "---[[ Warning (splitList): The index is too large ]]---\n";
		}
	}
}

template<class T>
void List<T>::sort()
{
	Node<T>* lowest = head;
	Node<T>* iterO = head;
	Node<T>* iterI;
	while (iterO != nullptr)
	{
		lowest = iterO;
		iterI = iterO;
		while (iterI != nullptr)
		{
			if (iterI->data < lowest->data)
			{
				lowest = iterI;
			}
			iterI = iterI->next;
		}
		T temp_data = lowest->data;
		lowest->Initialize(*iterO);
		iterO->Initialize(temp_data);
		iterO = iterO->next;
	}
}

template<class T>
void List<T>::nodesort()
{
	Node<T>* lowest = head;
	Node<T>* lowest_prev = nullptr;
	Node<T>* iterO = head;
	Node<T>* iterO_prev = nullptr;
	Node<T>* iterI;
	Node<T>* prevtemp = nullptr;		//keeps track of node before lowest
	while (iterO != nullptr)
	{
		lowest = iterO;
		iterI = iterO;
		prevtemp = nullptr;
		while (iterI != nullptr)
		{
			if (iterI->data <= lowest->data)
			{
				lowest_prev = prevtemp;
				lowest = iterI;
			}
			prevtemp = iterI;
			iterI = iterI->next;
		}
		if (lowest != iterO)
		{
			if (lowest == head)
				head = iterO;
			else
				lowest_prev->next = iterO;

			Node<T>* A_next = iterO->next;
			iterO->next = lowest->next;

			if (iterO == head)
				head = lowest;
			else
				iterO_prev->next = lowest;

			lowest->next = A_next;

			iterO_prev = lowest;
		}
		else
		{
			iterO_prev = iterO;
		}
		iterO = lowest->next;
	}
}

template<class T>
List<T>::~List()
{
	while (!isEmpty())
	{
		remove();
	}
}
