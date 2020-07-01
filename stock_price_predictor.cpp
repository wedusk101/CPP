/*
	Returns the best day to buy and sell stocks based on given prices. 
	This is a sub-optimal O(n^2) solution.
	
	Example input: [10, 5, 8, 11, 9, 13]
	Output: 6 —> Buy on Day 2 (5) and Sell on Day 6 (13) for a profit of 8
*/

#include <cstdio>
#include <iostream>

int main()
{
	int buy = 0, sell = 0, profit = 0;
	int values[6];
	std::cout << "Enter the values " << std::endl;
	for (int i = 0; i < 6; i++)
		std::cin >> values[i];

	for (int i = 0; i < 5; i++)
	{	
		for (int j = i + 1; j < 6; j++)
		{
			if (values[j] - values[i] >= profit)
			{
				sell = j;
				buy = i;
				profit = values[sell] - values[buy];				
			}
		}			
	}
	
	printf("Buy on Day %d (%d) and Sell on Day %d (%d) for a profit of %d.\n", buy + 1, values[buy], sell + 1, values[sell], profit);
}