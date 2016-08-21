// To create an effective multimap, each node in the binary search tree
// (BSTNode) points to a doubly-linked list of value nodes (VNode), so
// that each BSTNode can contain multiple values.

#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <string>

class MultiMap
{
private:
	struct VNode
	{
		unsigned int value;
		VNode* prev;
		VNode* next;
		
		VNode(unsigned int v)
		{
			value = v;
			prev = next = nullptr;
		}
	};

	struct BSTNode
	{
		std::string key;// Key
		VNode* v_head;	// Value
		VNode* v_tail;
		BSTNode* left;	// Children
		BSTNode* right;
		BSTNode* prev;	// Pointers to in-order prev/next
		BSTNode* next;
		
		BSTNode(std::string s, unsigned int v)
		{
			key = s;
			v_head = v_tail = new VNode(v);
			left = right = prev = next = nullptr;
		}
	};

	BSTNode* head;

public:	
	class Iterator
	{
	public:
		Iterator(BSTNode* root = nullptr, bool tail = false); // O(1)
		bool valid() const;				// O(1)
		std::string getKey() const;			// O(1)
		unsigned int getValue() const;			// O(1)
		bool next();					// O(1)
		bool prev();					// O(1)

	private:
		MultiMap::BSTNode* bst_ptr;
		MultiMap::VNode* v_ptr;
	};

	MultiMap();						// O(1)
	~MultiMap();						// O(NV) (v = # values per node)
	void clear();						// O(NV)
	void insert(std::string key, unsigned int value);	// O(log N)
	Iterator findEqual(std::string key) const;		// O(log N)
	Iterator findEqualOrSuccessor(std::string key) const;	// O(log N)
	Iterator findEqualOrPredecessor(std::string key) const;	// O(log N)

private:
	MultiMap(const MultiMap& other);			// prevent copying
	MultiMap& operator=(const MultiMap& rhs);		// prevent copying
	void removeAll(BSTNode* root);				// O(NV)
	int compare(std::string a, std::string b) const;	
};

#endif // MULTIMAP_H
