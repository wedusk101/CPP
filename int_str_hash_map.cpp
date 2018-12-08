#include <iostream>
#include <string>

struct HashMap
{
	struct Pair // defines the key value pair
	{
		int roll;
		std::string name;
		bool valid;
		
		Pair() : roll(0), name(""), valid(false) {}
		
	};
	
	int occupied; // counts the number of occupied slots in the map
	int size; // the total size of the map
	double loadFactor;	// occupied / size
	
	std::shared_ptr *map;
	
	HashMap(int size_) : size(size_), map(new Pair[size_]) {}
	
	/*~HashMap()
	{
		delete [] map;
	}*/
	
	int getHash(const int &x) const
	{
		return x % size;
	}
	
	bool insert(const Pair &p)
	{
	}
	
	std::string getValue(const int &key) const
	{
	}
	
	double getLoadFactor() const
	{
		return (size == 0 ? 0 : occupied / (double) size);
	}
};
	