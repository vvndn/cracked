// MyHash.h

// Skeleton for the MyHash class template.  You must implement the first seven
// member functions; we have implemented the eighth.

template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    int getNumItems() const;
    double getLoadFactor() const;

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;

private:
	struct Node
	{
		KeyType m_key;
		ValueType m_value;
		Node* next;
		Node(KeyType key, ValueType value)
			: m_key(key), m_value(value), next(nullptr)
		{}
	};
	
	// Helper functions
	void clearHashTable();
	unsigned int getBucketNum(const KeyType& k, unsigned int bucketNum) const;

	Node** m_hashTable;		// dynamic array
	int m_numItems;
	double m_maxLoadFactor;
	unsigned int m_numBuckets;

};

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor)
	: m_numBuckets(100), m_numItems(0)
{
	// check the bounds of the maximum load factor passed in
	m_hashTable = new Node*[100]();
	if (maxLoadFactor < 0.5)
		maxLoadFactor = 0.5;
	else if (maxLoadFactor > 2.0)
		maxLoadFactor = 2.0;
	
	m_maxLoadFactor = maxLoadFactor;
}

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::~MyHash()
{
	clearHashTable();
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset()
{
	clearHashTable();								// clear the hashtable
	// reset all the data members and create a new table
	m_numBuckets = 100;
	m_numItems = 0;
	m_hashTable = new Node*[m_numBuckets]();
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType &key, const ValueType &value)
{
	ValueType* valuePtr = find(key);

	if (valuePtr == nullptr)		// if key is not found
	{
		Node* n = new Node(key, value);
	
		// this should work even when the bucket is only a nullptr
		// assigns the new node as the "head" node after assigning the current head as n->next so no pointers are lost
		n->next = m_hashTable[getBucketNum(key, m_numBuckets)];
		m_hashTable[getBucketNum(key, m_numBuckets)] = n;
		m_numItems++;			// increment number of items in hash table
	}
	else
	{
		*valuePtr = value;
	}

	if (getLoadFactor() > m_maxLoadFactor)		// if load factor has exceed the maximum
	{
		// dynamically allocate a new array that is twice the size of the current array
		unsigned int newNumBuckets = 2 * m_numBuckets;
		Node** newArray = new Node*[newNumBuckets]();

		for (unsigned int i = 0; i < m_numBuckets; i++)
		{
			// loop through m_hashTable and copy every key in hashtable into newArray
			// Make sure to check for collisions!
			Node* p = m_hashTable[i];
			while (p != nullptr)
			{
				Node* temp = p->next;	// holds the pointer to the next Node so it is not lost
				
				// the following 3 lines of code will deal with collisions
				Node* addNew = new Node(p->m_key, p->m_value);
				addNew->next = newArray[getBucketNum(p->m_key, newNumBuckets)];
				newArray[getBucketNum(p->m_key, newNumBuckets)] = addNew;

				delete p;
				p = temp;
			}	
		}
		delete[] m_hashTable;		// delete the old hash table

		// have m_hashTable store the pointer to the new array and update the bucket count
		m_hashTable = newArray;
		m_numBuckets = newNumBuckets;
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const
{
	unsigned int bucketNum = getBucketNum(key, m_numBuckets);		// get the bucket number that this key maps to

	Node* p = m_hashTable[bucketNum];				
	if (p != nullptr)												// if the current bucket is not empty, iterate through until you find your key
	{
		while (p != nullptr && p->m_key != key)
		{
			p = p->next;
		}
	}

	if (p == nullptr)												// if the bucket is empty or the iterative search ends in no word found
		return nullptr;												// return nullptr
	else
		return &(p->m_value);										// otherwise, return a pointer to the value

}

template<typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const
{
	return m_numItems;		// returns number of items in hash table
}

template<typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const
{
	return (m_numItems * 1.0 / m_numBuckets);		// returns current load factor of hash table
}

//////////////////////////////////////////
// Helper functions
//////////////////////////////////////////

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::clearHashTable()
{
	for (unsigned int i = 0; i < m_numBuckets; i++)		// for all buckets in hash table
	{
		Node* p = m_hashTable[i];							
		while (p != nullptr)							// iterate through the linked list and delete all dynamically allocated nodes
		{
			Node* q = p;
			p = p->next;
			delete q;
		}
	}
	delete[] m_hashTable;								// delete the array itself
}

template<typename KeyType, typename ValueType>
unsigned int MyHash<KeyType, ValueType>::getBucketNum(const KeyType& k, unsigned int numBuckets) const
{
	unsigned int hasher(const KeyType& k);  // prototype
	return hasher(k) % numBuckets;
}