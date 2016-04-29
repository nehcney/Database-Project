#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <string>

class MultiMap
{
private:
	struct VNode
	{
		VNode(unsigned int v)
		{
			value = v;
			prev = next = nullptr;
		}
		unsigned int value;
		VNode* prev;
		VNode* next;
	};

	struct BSTNode
	{
		BSTNode(std::string s, unsigned int v)
		{
			key = s;
			v_head = v_tail = new VNode(v);
			left = right = prev = next = nullptr;
		}
		std::string key;
		VNode* v_head;
		VNode* v_tail;
		BSTNode* left;
		BSTNode* right;
		BSTNode* prev;
		BSTNode* next;
	};

	BSTNode* head;

public:	
	class Iterator
	{
	public:
		Iterator(BSTNode* root = nullptr, bool tail = false);
		bool valid() const;
		std::string getKey() const;
		unsigned int getValue() const;
		bool next();
		bool prev();

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
	MultiMap(const MultiMap& other);		// prevent copying
	MultiMap& operator=(const MultiMap& rhs);	// prevent copying
	void removeAll(BSTNode* root);
	int compare(std::string a, std::string b) const;
};

#endif // MULTIMAP_H
