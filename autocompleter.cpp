#include "autocompleter.h"

// Creates a new Autocompleter with an empty dictionary.
//
// Must run in O(1) time.
Autocompleter::Autocompleter()
{
	root = nullptr;
}

// Adds a string x to the dictionary.
// If x is already in the dictionary, does nothing.
//
// Must run in O(log(n)) time.
void Autocompleter::insert(string x, int freq)
{
	Entry * e = new Entry;
	e->s = x;
	e->freq = freq;
	if (root == nullptr)
	{
		root = new Node(*e);
		root->height++;
		return;
	}
	insert_recurse(*e, root);
}

// Returns the number of strings in the dictionary
// of possible completions. 
// 
// Must run in O(n) time.
int Autocompleter::size()
{
	return size_recurse(root);
}

// Fills the vector T with the three most-frequent completions of x.
// If x has less than three completions, then 
// T is filled with all completions of x.
// The completions appear in T from most to least frequent.
// 
// Must run in O(log(n) + k) time,
// where k is the number of completions of x in the dictionary.
void Autocompleter::completions(string x, vector<string> &T)
{
	T.clear();
	Node * walker = root;
	while (x != walker->e.s.substr(0, x.size()) && walker != nullptr)
	{
		if (x > walker->e.s.substr(0, x.size()))
		{
			if (walker->right == nullptr)
			{
				walker = nullptr;
				break;
			}
			walker = walker->right;
		}
		if (x < walker->e.s.substr(0, x.size()))
		{
			if (walker->left == nullptr)
			{
				walker = nullptr;
				break;
			}
			walker = walker->left;
		}
	}
	vector<Entry> V;
	completions_recurse(x, walker, V);
	if (V.size() == 0)
		return;
	else
	{
		int maxindex = 0;
		for (int i = 0; i < 3 && i < V.size(); i++)
		{
			int popular = 0;
			for (int j = 0; j < V.size(); j++)
			{
				if (V[j].freq > popular)
				{
					popular = V[j].freq;
					maxindex = j;
				}
			}
			T.push_back(V[maxindex].s);
			V[maxindex].freq *= (-1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Optional helper methods (you'll probably want them)

// Returns the size of the binary tree rooted at root.
//
// Should run in O(n) time.
int Autocompleter::size_recurse(Node* root)
{
	if (root == nullptr)
		return 0;
	else
		return 1 + size_recurse(root->left) + size_recurse(root->right);
}

// Fills T with the three most-frequent completions of x
// that are either:
// -In the BST rooted at root.
// -Already in T.
//
// Should run in O(log(n) + k), where
// -n is the size of the BST rooted at root.
// -k is the number of Entrys in the BST rooted at root
//  whose strings start with x.
// comment  by gilbert
void Autocompleter::completions_recurse(string x, Node* root, vector<Entry> &T)
{
	if (root == nullptr)
		return;
	else
	{
		if (x == root->e.s.substr(0, x.size()))
		{
			T.push_back(root->e);
			completions_recurse(x, root->left, T);
			completions_recurse(x, root->right, T);
		}
		if (x < root->e.s.substr(0, x.size()))
			completions_recurse(x, root->left, T);
		if (x > root->e.s.substr(0, x.size()))
			completions_recurse(x, root->right, T);
	}
}

// Inserts an Entry into an AVL tree rooted at root.
//
// Should run in O(log(n)) time.
void Autocompleter::insert_recurse(Entry e, Node* root)
{
	if (root->e.s == e.s)
		return;
	else if (root->e.s < e.s)
	{
		if (root->right != nullptr)
			insert_recurse(e, root->right);
		else
			root->right = new Node(e);
	}
	else
	{
		if (root->left != nullptr)
			insert_recurse(e, root->left);
		else
			root->left = new Node(e);
	}
	if (height(root->left) > height(root->right))
		root->height = height(root->left) + 1;
	else
		root->height = height(root->right) + 1;
	rebalance(root);
}

// Rebalances the AVL tree rooted at root.
// Helpful for insert(). 
// Should be called on every node visited during
// the search in reverse search order.
// 
// Should run in O(1) time. 
void Autocompleter::rebalance(Node* root)
{
	if (abs(height(root->left) - height(root->right)) <= 1)
		return;
	else
	{
		if (height(root->right) > height(root->left))
		{
			if (height(root->right->left) > height(root->right->right))
				right_rotate(root->right);
			left_rotate(root);
		}
		if (height(root->left) > height(root->right))
		{
			if (height(root->left->right) > height(root->left->left))
				left_rotate(root->left);
			right_rotate(root);
		}		
	}
}

// Perform left and right rotations around the root
// of an AVL tree (helpful for implementing rebalance).
//
// Should run in O(1) time.
void Autocompleter::right_rotate(Node* root)
{
	Node* rc = root->left;
	root->left = rc->left;
	rc->left = rc->right;
	rc->right = root->right;
	root->right = rc;
	swap(root->e.s, rc->e.s);
	swap(root->e.freq, rc->e.freq);
	if (height(root->left) > height(root->right))
		root->height = height(root->left) + 1;
	else
		root->height = height(root->right) + 1;
	if (height(rc->left) > height(rc->right))
		rc->height = height(rc->left) + 1;
	else
		rc->height = height(rc->right) + 1;
}
void Autocompleter::left_rotate(Node* root)
{
	Node* rc = root->right;
	root->right = rc->right;
	rc->right = rc->left;
	rc->left = root->left;
	root->left = rc;
	swap(root->e.s, rc->e.s);
	swap(root->e.freq, rc->e.freq);
	if (height(root->left) > height(root->right))
		root->height = height(root->left) + 1;
	else
		root->height = height(root->right) + 1;
	if (height(rc->left) > height(rc->right))
		rc->height = height(rc->left) + 1;
	else
		rc->height = height(rc->right) + 1;
}