// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

/**
 *  This class represent thread safe priority queue with random access to the items with the help of the provided key.
 *  Under te hood, this class is an AVL tree fro the queue with TMap for the the random access 
 */
template <typename TKey, typename TValue>
class MINECRAFT_API FPriorityQueue
{
private:
	struct QueueNode
	{
		QueueNode(int32 Priority, const TKey& Key, const TValue& Value);
		/*
		 * Add value to the existing node
		 */
		void AddValue(const TKey& Key, const TValue& Value);
		/*
		 * Remove value from the existing node
		 */
		void RemoveValue(const TKey& Key);

		/*
		 * Checks whether the node is empty (contains no values)
		 */
		bool IsEmpty() const;

		int32 Priority;
		TMap<TKey, TValue> Values;

		int32 Height;
		QueueNode* Left;
		QueueNode* Right;
	};

	/*
	 * Get height of the node in the AVL tree
	 */
	int32 GetNodeHeight(const QueueNode* Node);

	/*
	 * Get balance of the node in the AVL tree
	 */
	int32 GetNodeBalance(const QueueNode* Node);

	/*
	 * Perform right rotation of the AVL tree branch with the given node as the root of the branch
	 */
	QueueNode* RightRotate(QueueNode* Node);

	/*
	 * Perform left rotation of the AVL tree branch with the given node as the root of the branch
	 */
	QueueNode* LeftRotate(QueueNode* Node);

	/*
	 * Add new node to the AVL tree
	 */
	QueueNode* AddNode(QueueNode* Node, int32 Priority, const TKey& Key, const TValue& Value);

	/*
	 * Remove given node from the AVL tree
	 */
	QueueNode* DeleteNode(QueueNode* Node, int32 Priority);

	/*
	 * Find node with the minimal value, starting at the given node
	 */
	QueueNode* MinValueNode(QueueNode* Node) const;

	/*
	 * Find node with the maximal value, starting at the given node
	 */
	QueueNode* MaxValueNode(QueueNode* Node) const;

	/*
	 * Helper to copy the AVL tree
	 */
	void CopyTree(const QueueNode* RootNode);

	/*
	 * Helper to delete the AVL tree
	 */
	void DeleteTree(const QueueNode* RootNode);

private:
	FCriticalSection ValuesMapLock;
	TMap<TKey, QueueNode*> ValuesMap;

	FCriticalSection TreeLock;

	QueueNode* Root;
	QueueNode* MinNode;
	QueueNode* MaxNode;

public:
	FPriorityQueue();
	FPriorityQueue(const FPriorityQueue& Queue);
	~FPriorityQueue();
	FPriorityQueue& operator=(FPriorityQueue Queue);
	template <class U, class V>
	friend void Swap(FPriorityQueue<U, V>& Lhs, FPriorityQueue<U, V>& Rhs);

	/*
	 * Add new value to the priority queue
	 */
	void Add(int32 Priority, const TKey& Key, const TValue& Value);

	/*
	 * Remove value from the priority value (if exists)
	 */
	void Remove(const TKey& Key);

	/*
	 * Pops the value with the minimal priority from the queue
	 */
	bool Pop(TValue& Value);

	/*
	 * Check whether the queue contains value with the given key
	 */
	bool Contains(const TKey& Key);

	/*
	 * Check whether the queue is empty
	 */
	bool IsEmpty();

	/*
	 * If value with the given key exists, changes its priority
	 */
	void ChangePriority(const TKey& Key, int32 Priority);

	/*
	 * Returns the minimal present priority in the queue
	 */
	int32 GetMinPriority() const;

	/*
	 * Returns the maximal present priority in the queue
	 */
	int32 GetMaxPriority() const;

	/*
	 * Clears the whole queue
	 */
	void Clear();
};

template <typename TKey, typename TValue>
FPriorityQueue<TKey, TValue>::QueueNode::QueueNode(int32 Priority, const TKey& Key, const TValue& Value)
	: Priority(Priority), Height(1), Left(nullptr), Right(nullptr)
{
	Values.Add({Key, Value});
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::QueueNode::AddValue(const TKey& Key, const TValue& Value)
{
	Values.Add({Key, Value});
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::QueueNode::RemoveValue(const TKey& Key)
{
	Values.Remove(Key);
}

template <typename TKey, typename TValue>
bool FPriorityQueue<TKey, TValue>::QueueNode::IsEmpty() const
{
	return Values.IsEmpty();
}

template <typename TKey, typename TValue>
int32 FPriorityQueue<TKey, TValue>::GetNodeHeight(const QueueNode* Node)
{
	if (!Node)
	{
		return 0;
	}
	return Node->Height;
}

template <typename TKey, typename TValue>
int32 FPriorityQueue<TKey, TValue>::GetNodeBalance(const QueueNode* Node)
{
	if (!Node)
	{
		return 0;
	}
	return GetNodeHeight(Node->Left) - GetNodeHeight(Node->Right);
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::RightRotate(QueueNode* Node)
{
	QueueNode* X = Node->Left;
	QueueNode* Y = X->Right;

	// Rotate
	X->Right = Node;
	Node->Left = Y;

	// Update heights
	Node->Height = UKismetMathLibrary::Max(GetNodeHeight(Node->Left), GetNodeHeight(Node->Right)) + 1;
	X->Height = UKismetMathLibrary::Max(GetNodeHeight(X->Left), GetNodeHeight(X->Right)) + 1;

	return X;
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::LeftRotate(QueueNode* Node)
{
	QueueNode* X = Node->Right;
	QueueNode* Y = X->Left;

	// Rotate
	X->Left = Node;
	Node->Right = Y;

	Node->Height = UKismetMathLibrary::Max(GetNodeHeight(Node->Left), GetNodeHeight(Node->Right)) + 1;
	X->Height = UKismetMathLibrary::Max(GetNodeHeight(X->Left), GetNodeHeight(X->Right)) + 1;

	return X;
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::AddNode(QueueNode* Node, int32 Priority, const TKey& Key,
                                                                                        const TValue& Value)
{
	if (!Node)
	{
		QueueNode* NewNode = new QueueNode(Priority, Key, Value);
		ValuesMap.Add(Key, NewNode);
		MinNode = !MinNode ? NewNode : Priority < MinNode->Priority ? NewNode : MinNode;
		MaxNode = !MaxNode ? NewNode : Priority > MaxNode->Priority ? NewNode : MaxNode;
		return NewNode;
	}

	if (Priority == Node->Priority)
	{
		Node->AddValue(Key, Value);
		ValuesMap.Add(Key, Node);
		return Node;
	}
	if (Priority < Node->Priority)
	{
		Node->Left = AddNode(Node->Left, Priority, Key, Value);
	}
	else
	{
		Node->Right = AddNode(Node->Right, Priority, Key, Value);
	}

	// Update height of ancestor
	Node->Height = 1 + UKismetMathLibrary::Max(GetNodeHeight(Node->Left), GetNodeHeight(Node->Right));

	int32 Balance = GetNodeBalance(Node);

	// Balance the tree

	if (Balance > 1)
	{
		// Left Left Case
		if (Priority < Node->Left->Priority)
		{
			return RightRotate(Node);
		}
		// Left Right Case
		Node->Left = LeftRotate(Node->Left);
		return RightRotate(Node);
	}
	if (Balance < -1)
	{
		// Right Right Case
		if (Priority > Node->Right->Priority)
		{
			return LeftRotate(Node);
		}
		// Right Left Case
		Node->Right = RightRotate(Node->Right);
		return LeftRotate(Node);
	}

	return Node;
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::DeleteNode(QueueNode* Node, int32 Priority)
{
	if (!Node)
	{
		return Node;
	}

	if (Priority < Node->Priority)
	{
		Node->Left = DeleteNode(Node->Left, Priority);
	}

	else if (Priority > Node->Priority)
	{
		Node->Right = DeleteNode(Node->Right, Priority);
	}
	else
	{
		// Node with zero or one child
		if (!Node->Left || !Node->Right)
		{
			QueueNode* Temp = Node->Left ? Node->Left : Node->Right;
			if (Temp)
			{
				*Node = *Temp;
				for (auto KeyValue : Node->Values)
				{
					ValuesMap[KeyValue.Key] = Node;
				}
			}
			else
			{
				Temp = Node;
				Node = nullptr;
			}
			delete Temp;
		}
		else
		{
			QueueNode* Temp = MinValueNode(Node->Right);
			Node->Priority = Temp->Priority;
			Node->Values = Temp->Values;
			for (auto KeyValue : Node->Values)
			{
				ValuesMap[KeyValue.Key] = Node;
			}
			Node->Right = DeleteNode(Node->Right, Temp->Priority);
		}
	}
	if (!Node)
	{
		return Node;
	}

	// Update height of ancestor
	Node->Height = 1 + UKismetMathLibrary::Max(GetNodeHeight(Node->Left), GetNodeHeight(Node->Right));

	int32 Balance = GetNodeBalance(Node);

	// Balance the tree

	if (Balance > 1)
	{
		// Left Left Case
		if (GetNodeBalance(Node->Left) >= 0)
		{
			return RightRotate(Node);
		}
		// Left Right Case
		Node->Left = LeftRotate(Node->Left);
		return RightRotate(Node);
	}
	if (Balance < -1)
	{
		// Right Right Case
		if (GetNodeBalance(Node->Right) <= 0)
		{
			return LeftRotate(Node);
		}
		// Right Left Case
		Node->Right = RightRotate(Node->Right);
		return LeftRotate(Node);
	}

	return Node;
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::MinValueNode(QueueNode* Node) const
{
	if (!Node)
	{
		return nullptr;
	}
	QueueNode* Current = Node;
	while (Current->Left)
	{
		Current = Current->Left;
	}
	return Current;
}

template <typename TKey, typename TValue>
typename FPriorityQueue<TKey, TValue>::QueueNode* FPriorityQueue<TKey, TValue>::MaxValueNode(QueueNode* Node) const
{
	if (!Node)
	{
		return nullptr;
	}
	QueueNode* Current = Node;
	while (Current->Right)
	{
		Current = Current->Right;
	}
	return Current;
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::CopyTree(const QueueNode* RootNode)
{
	if (!RootNode)
	{
		return;
	}
	CopyTree(RootNode->Left);
	CopyTree(RootNode->Right);

	for (auto& KeyValues : RootNode->Values)
	{
		Add(RootNode->Priority, KeyValues.Key, KeyValues.Value);
	}
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::DeleteTree(const QueueNode* RootNode)
{
	if (!RootNode)
	{
		return;
	}

	DeleteTree(RootNode->Left);
	DeleteTree(RootNode->Right);

	delete RootNode;
	RootNode = nullptr;
}

template <typename TKey, typename TValue>
FPriorityQueue<TKey, TValue>::FPriorityQueue() : Root(nullptr), MinNode(nullptr), MaxNode(nullptr)
{
}

template <typename TKey, typename TValue>
FPriorityQueue<TKey, TValue>::FPriorityQueue(const FPriorityQueue& Queue) : Root(nullptr), MinNode(nullptr), MaxNode(nullptr)
{
	FScopeLock Lock(&TreeLock);
	FScopeLock QueueLock(&Queue.TreeLock);
	CopyTree(Queue.Root);
}

template <typename TKey, typename TValue>
FPriorityQueue<TKey, TValue>::~FPriorityQueue()
{
	DeleteTree(Root);
}

template <class U, class V>
void Swap(FPriorityQueue<U, V>& Lhs, FPriorityQueue<U, V>& Rhs)
{
	Swap(Lhs.Root, Rhs.Root);
	Swap(Lhs.MinNode, Rhs.MinNode);
	Swap(Lhs.MaxNode, Rhs.MaxNode);
}

template <typename TKey, typename TValue>
FPriorityQueue<TKey, TValue>& FPriorityQueue<TKey, TValue>::operator=(FPriorityQueue Queue)
{
	Swap(*this, Queue);
	return *this;
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::Add(int32 Priority, const TKey& Key, const TValue& Value)
{
	FScopeLock Lock(&TreeLock);
	if (ValuesMap.Contains(Key))
	{
		ValuesMap[Key]->RemoveValue(Key);
		if (ValuesMap[Key]->IsEmpty())
		{
			bool IsMin = ValuesMap[Key]->Priority == MinNode->Priority;
			bool IsMax = ValuesMap[Key]->Priority == MaxNode->Priority;
			Root = DeleteNode(Root, ValuesMap[Key]->Priority);
			if (IsMin)
			{
				MinNode = MinValueNode(Root);
			}
			if (IsMax)
			{
				MaxNode = MaxValueNode(Root);
			}
		}
	}
	Root = AddNode(Root, Priority, Key, Value);
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::Remove(const TKey& Key)
{
	FScopeLock Lock(&TreeLock);
	if (!ValuesMap.Contains(Key))
	{
		return;
	}

	ValuesMap[Key]->RemoveValue(Key);
	if (ValuesMap[Key]->IsEmpty())
	{
		bool SetMin = ValuesMap[Key]->Priority == MinNode->Priority;
		bool SetMax = ValuesMap[Key]->Priority == MaxNode->Priority;
		Root = DeleteNode(Root, ValuesMap[Key]->Priority);
		if (SetMin)
		{
			MinNode = MinValueNode(Root);
		}
		if (SetMax)
		{
			MaxNode = MaxValueNode(Root);
		}
	}
	ValuesMap.Remove(Key);
	MinNode = MinValueNode(Root);
	MaxNode = MaxValueNode(Root);
}

template <typename TKey, typename TValue>
bool FPriorityQueue<TKey, TValue>::Pop(TValue& Value)
{
	FScopeLock Lock(&TreeLock);
	if (!MinNode)
	{
		return false;
	}

	TKey Key;
	for (auto& KeyValue : MinNode->Values)
	{
		Value = KeyValue.Value;
		Key = KeyValue.Key;
	}
	ValuesMap[Key]->RemoveValue(Key);
	if (ValuesMap[Key]->IsEmpty())
	{
		Root = DeleteNode(Root, ValuesMap[Key]->Priority);
	}
	ValuesMap.Remove(Key);
	MinNode = MinValueNode(Root);
	MaxNode = MaxValueNode(Root);
	return true;
}

template <typename TKey, typename TValue>
bool FPriorityQueue<TKey, TValue>::Contains(const TKey& Key)
{
	FScopeLock Lock(&TreeLock);
	return ValuesMap.Contains(Key);
}

template <typename TKey, typename TValue>
bool FPriorityQueue<TKey, TValue>::IsEmpty()
{
	FScopeLock Lock(&TreeLock);
	return Root == nullptr;
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::ChangePriority(const TKey& Key, int32 Priority)
{
	FScopeLock Lock(&TreeLock);
	if (!ValuesMap.Contains(Key))
	{
		return;
	}

	if (ValuesMap[Key]->Priority == Priority)
	{
		return;
	}

	TValue Value = ValuesMap[Key]->Values[Key];
	ValuesMap[Key]->RemoveValue(Key);

	if (ValuesMap[Key]->IsEmpty())
	{
		Root = DeleteNode(Root, ValuesMap[Key]->Priority);
	}
	Root = AddNode(Root, Priority, Key, Value);
	MinNode = MinValueNode(Root);
	MaxNode = MaxValueNode(Root);
}

template <typename TKey, typename TValue>
int32 FPriorityQueue<TKey, TValue>::GetMinPriority() const
{
	if (MinNode)
	{
		return MinNode->Priority;
	}
	return 99999;
}

template <typename TKey, typename TValue>
int32 FPriorityQueue<TKey, TValue>::GetMaxPriority() const
{
	if (MaxNode)
	{
		return MaxNode->Priority;
	}
	return -99999;
}

template <typename TKey, typename TValue>
void FPriorityQueue<TKey, TValue>::Clear()
{
	FScopeLock Lock(&TreeLock);
	DeleteTree(Root);
}
