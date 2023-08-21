// A naive implementation of a Trie data structure

#include <iostream>
#include <string>
#include <queue>

#define ALPHABET_SIZE 128

struct Node
{
	Node() : value('\0')
	{
		children = new Node*[ALPHABET_SIZE];
		
		for (size_t i = 0; i < ALPHABET_SIZE; ++i)
			children[i] = nullptr;
	}
	
	Node(char c, bool isTerminal_ = false) : value(c), isTerminal(isTerminal_)
	{
		children = new Node*[ALPHABET_SIZE];
		
		for (size_t i = 0; i < ALPHABET_SIZE; ++i)
			children[i] = nullptr;
	}
	
	~Node()
	{
		for (size_t i = 0; i < ALPHABET_SIZE; ++i)
			delete children[i];
		
		delete[] children;
		children = nullptr;
	}
	
	Node** children = nullptr;
	bool isTerminal = false;
	char value;
};

class Trie
{
	
public: 

	Trie() : root(new Node()) {}
	
	~Trie()
	{
		delete root;
		root = nullptr;
	}
	
	bool insert(const std::string& input)
	{	
		Node* bak = root;
		bool isInserted = false;
				
		for (size_t i = 0; i < input.size(); ++i)
		{
			if (root->children[input[i]] == nullptr)
			{
				root->children[input[i]] = new Node(input[i]);
				root = root->children[input[i]];
				isInserted = true;
			}
			else if (root->children[input[i]]->value == input[i] && !root->isTerminal)
			{
				root = root->children[input[i]];
				isInserted = true;
			}
			else
				root = root->children[input[i]];
		}
		
		if (root->isTerminal)
		{
			root = bak;
			return false;
		}
		
		root->isTerminal = true;	
		root = bak;
		
		if (isInserted)
			++entryCount;
		
		return isInserted;
	}
	
	bool find(const std::string& key) const
	{
		Node* bak = root;
				
		for (size_t i = 0; i < key.size(); ++i)
		{
			if (bak->children[key[i]] == nullptr)
				return false;			

			if (bak->children[key[i]]->value != key[i])
				return false;
			
			bak = bak->children[key[i]];
		}
		
		return bak->isTerminal;
	}
	
	bool remove(const std::string& key)
	{
		if (!this->find(key))
			return false;
		
		Node* bak = root;
		
		for (size_t i = 0; i < key.size(); ++i)
			root = root->children[key[i]];
		
		root->isTerminal = false;
		root = bak;
		
		--entryCount;
		return true;
	}
	
	/*
	
	void printInOrder() const
	{
		if (this->isEmpty())
		{
			std::cout << "No entries in the trie. Nothing to display." << std::endl;
			return;
		}

		displayPreorder(root);
	}
	
	*/
	
	void printInOrder() const
	{
		if (this->isEmpty())
		{
			std::cout << "No entries in the trie. Nothing to display." << std::endl;
			return;
		}
		
		std::queue<Node*> nodeQueue;
		
		// add nodes connected to root
		for (size_t i = 0; i < ALPHABET_SIZE; ++i)
		{
			if (root->children[i])
				nodeQueue.push(root->children[i]);
		}
		
		while (!nodeQueue.empty())
		{
			isWord = false;
			Node* node = nodeQueue.front();
			for (size_t i = 0; i < ALPHABET_SIZE; ++i)
			{
				if (node->children[i])
				{
					nodeQueue.push(node->children[i]);
					
					if (i > 0)
						prefix += node->value;
				}
			}
			
			suffix.clear();
			this->updateSuffix(node);
			
			if (isWord)
				std::cout << prefix + suffix << std::endl;
			
			nodeQueue.pop();
		}
		
	} 
	
private:

	bool isEmpty() const
	{
		return (entryCount == 0);
	}				

	void updateSuffix(const Node* rootPtr) const
	{	
		if (rootPtr->isTerminal)
		{
			suffix += rootPtr->value;
			isWord = true;
			return;
		}
		
		suffix += rootPtr->value;
		
		for (size_t i = 0; i < ALPHABET_SIZE; ++i)
			if (rootPtr->children[i])
				updateSuffix(rootPtr->children[i]);
	}
	
	/*
	
	void displayPreorder(Node* rootPtr) const
	{
		if (!rootPtr)
		{
			word += rootPtr->value;
			
			if (rootPtr->isTerminal)
			{
				std::cout << word << std::endl;
				return;
			}
			
			for (size_t i = 0; i < ALPHABET_SIZE; ++i)
				displayPreorder(rootPtr->children[i]);
		}
	}		
	
	*/
	
	Node *root = nullptr;
	
	size_t entryCount = 0;
	
	mutable bool isWord = false;
	mutable std::string prefix;
	mutable std::string suffix;	
	// mutable std::string word;
};

int main()
{
	Trie trie;	
	int choice = 1;
	std::string input;
	bool result = false;
	
	std::cout << "This program implements a trie data structure with the following basic operations." << std::endl;
	
	do
	{	
		std::cout << "\n----------- MENU -------------\n" << std::endl;
		std::cout << "1. Insert a string." << std::endl;
		std::cout << "2. Find a string." << std::endl;
		std::cout << "3. Remove a string." << std::endl;
		std::cout << "4. Display the contents of the trie in alphabetical order." << std::endl; // TODO
		std::cout << "0. EXIT." << std::endl;
		std::cout << "\n------------------------------\n" << std::endl;
		
		std::cout << "Please enter your choice: " << std::endl;
		std::cin >> choice;
		
		switch (choice)
		{
			case 1:			
				std::cout << "Please enter a string to insert." << std::endl;
				std::cin >> input;
			
				result = trie.insert(input);
				
				if (result)
					std::cout << "String inserted successfully." << std::endl;
				else
					std::cout << "String already exists." << std::endl;
				break;
				
			case 2:
				std::cout << "Please enter a string to find." << std::endl;
				std::cin >> input;
				
				result = trie.find(input);
				
				if (result)
					std::cout << "String found!" << std::endl;
				else
					std::cout << "String not found." << std::endl;
				break;
				
			case 3:
				std::cout << "Please enter a string to remove." << std::endl;
				std::cin >> input;
				
				result = trie.remove(input);
				
				if (result)
					std::cout << "String removed successfully." << std::endl;
				else
					std::cout << "Failed to remove string." << std::endl;
				break;
				
			case 4:
				trie.printInOrder();
				break;
				
			case 0:
				break;
				
			default:
				break;		
		}
		
	} while (choice != 0);
	
	std::cout << "Thank you." << std::endl;
}