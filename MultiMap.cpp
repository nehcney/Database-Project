#include "MultiMap.h"
using namespace std;

/////////////////////////////
// Iterator Implementations
/////////////////////////////

MultiMap::Iterator::Iterator(BSTNode* root, bool tail)
{
	bst_ptr = root;
	if (root != nullptr)
		v_ptr = tail ? root->v_tail : root->v_head;
}

bool MultiMap::Iterator::valid() const
{
	return bst_ptr != nullptr;
}

string MultiMap::Iterator::getKey() const
{
	return valid() ? bst_ptr->key : "";
}

unsigned int MultiMap::Iterator::getValue() const
{
	return valid() ? v_ptr->value : 0;
}

bool MultiMap::Iterator::next()
{
	if (!valid())
		return false;

	if (v_ptr == bst_ptr->v_tail)
	{
		bst_ptr = bst_ptr->next;
		v_ptr = bst_ptr ? bst_ptr->v_head : nullptr;
	}
	else
		v_ptr = v_ptr->next;

	return true;
}

bool MultiMap::Iterator::prev()
{
	if (!valid())
		return false;

	if (v_ptr == bst_ptr->v_head)
	{
		bst_ptr = bst_ptr->prev;
		v_ptr = bst_ptr ? bst_ptr->v_tail : nullptr;
	}
	else
		v_ptr = v_ptr->prev;

	return true;
}


/////////////////////////////
// MultiMap Implementations
/////////////////////////////

MultiMap::MultiMap()
{
	head = nullptr;
}

MultiMap::~MultiMap()
{
	clear();
}

void MultiMap::clear()
{
	removeAll(head);
}

// If no matching key is found, inserts a new BSTNode. Otherwise, since each BSTNode 
// contains a linked list of values as part of the multimap implementation, a new
// VNode is added to the matching BSTNode's value list. 
// --- The way we implemented iterator traversal requires that each BSTNode contains a
// pointer to the prev and next BSTNode. In order to keep track of which is prev/next,
// we create two temp BSTNode pointers: min and max. Each time we traverse left down the
// tree, we update max to be the node we last visited, and each traversal right updates
// min. This guarantees that we can always keep track of prev/next.
void MultiMap::insert(string key, unsigned int value)
{
	if (!head)
	{
		head = new BSTNode(key, value);
		return;
	}

	BSTNode *cur = head, *min = nullptr, *max = nullptr;
	for (;;)
	{
		int result = compare(key, cur->key);
		if (result == 0)  // (key == cur->key)
		{
			VNode* temp = new VNode(value);
			cur->v_tail->next = temp;
			temp->prev = cur->v_tail;
			cur->v_tail = temp;
			return;
		}
		if (result < 0) // (key < cur->key)
		{
			max = cur;
			if (cur->left)
				cur = cur->left;
			else
			{
				cur->left = new BSTNode(key, value);
				cur->left->prev = min;
				cur->left->next = cur;
				cur->prev = cur->left;
				if (min)
					min->next = cur->left;
				return;
			}
		}
		else if (result > 0) // (key > cur->key)
		{
			min = cur;
			if (cur->right)
				cur = cur->right;
			else
			{
				cur->right = new BSTNode(key, value);
				cur->right->prev = cur;
				cur->right->next = max;
				cur->next = cur->right;
				if (max)
					max->prev = cur->right;
				return;
			}
		}
	}
}

// Returns an iterator with its bst_ptr pointing to the BSTNode with the matching key,
// and its v_ptr pointing to the BSTNode's v_head (earliest value). If a matching key 
// cannot be found, returns an invalid iterator.
MultiMap::Iterator MultiMap::findEqual(string key) const
{
	BSTNode *cur = head;
	while (cur != nullptr)
	{
		int result = compare(key, cur->key);
		if (result < 0) // (key < cur->key)
			cur = cur->left;
		else if (result > 0) // (key > cur->key)
			cur = cur->right;
		else // found matching key
			return Iterator(cur);
	}
	return Iterator(nullptr);
}

// Returns an iterator with its bst_ptr pointing to the BSTNode with the matching key
// (or if one cannot be found, the next largest key), and its v_ptr pointing to the
// BSTNode's v_head (earliest value). If key is an empty string, returns iterator to the
// smallest BSTNode. If key is larger than the largest BSTNode, returns invalid iterator.
// --- Utlizes a BSTNode pointer "max" to keep track of the next largest key. Everytime
// we traverse left, the node we last visited will be the new "max".
MultiMap::Iterator MultiMap::findEqualOrSuccessor(string key) const
{
	BSTNode *cur = head, *max = nullptr;
	while (cur != nullptr)
	{
		int result = compare(key, cur->key);
		if (result < 0) // (key < cur->key)
		{
			max = cur;
			if (cur->left == nullptr)
				return Iterator(cur);
			cur = cur->left;
		}
		else if (result > 0) // (key > cur->key)
		{
			if (cur->right == nullptr)
				return Iterator(max);
			cur = cur->right;
		}
		else // found matching key
			return Iterator(cur);
	}
	return Iterator(nullptr);
}

// Returns an iterator with its bst_ptr pointing to the BSTNode with the matching key
// (or if one cannot be found, the next smallest key), and its v_ptr pointing to the
// BSTNode's v_tail (latest value). If key is an empty string, returns iterator to the
// largest BSTNode. If key is smaller than the smallest BSTNode, returns invalid iterator.
// --- Utlizes a BSTNode pointer "min" to keep track of the next smallest key. Everytime
// we traverse right, the node we last visited will be the new "min".
MultiMap::Iterator MultiMap::findEqualOrPredecessor(string key) const
{
	BSTNode *cur = head, *min = nullptr;
	while (cur != nullptr)
	{
		int result = compare(key, cur->key);
		if (key != "" && result < 0) // (key < cur->key)
		{
			if (cur->left == nullptr)
				return Iterator(min, true);
			cur = cur->left;
		}
		else if (key == "" || result > 0) // (key > cur->key)
		{
			min = cur;
			if (cur->right == nullptr)
				return Iterator(cur, true);
			cur = cur->right;
		}
		else // found matching key
			return Iterator(cur, true);
	}
	return Iterator(nullptr);
}


/////////////////////////////
// MultiMap Helper Functions
/////////////////////////////

void MultiMap::removeAll(BSTNode* root)
{
	if (root == nullptr)
		return;

	removeAll(root->left);
	removeAll(root->right);
	
	VNode* v_cur = root->v_head;
	while (v_cur != nullptr)
	{
		VNode* deleteMe = v_cur;
		v_cur = v_cur->next;
		delete deleteMe;
	}

	delete root;
}

int MultiMap::compare(string a, string b) const
{
	bool isNumber = a.find_first_not_of("0123456789.") == string::npos;
	// By design, a and b are from the same field type, no need to check b.
	if (isNumber)
	{
		int a_decimal = a.find('.'), b_decimal = b.find('.'),
			a_length = a_decimal == string::npos ? a.length() : a_decimal,
			b_length = b_decimal == string::npos ? b.length() : b_decimal;
		string* shorter_string = a_length < b_length ? &a : &b;
		shorter_string->insert(0, abs(a_length - b_length), '0');
	}
	if (a == b)
		return 0;
	else
		return a < b ? -1 : 1;
}
