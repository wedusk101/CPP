#include <iostream>

int main()
{
	int order = 0, iStep = 0, jStep = 0, i = 0, j = 0, count = 0, loop = 0;
	bool left = false, right = false, top = false, bottom = false;
	std::cout << "Please enter the order of the matrix." << std::endl;
	std::cin >> order;
	int arr[order][order];
	
	for(i =0; i < order; i++)
		for(j = 0; j < order; j++)
			arr[i][j] = 0;
		
	for(i = 0, j = -1, right = true; count != order * order;)
	{
		if(right)
		{
			if(j == order - 1 || arr[i][j + 1] != 0)
			{
				bottom = true;
				left = right = top = false;
				continue;
			}
			arr[i][++j] = ++count;
		}
		
		if(bottom)
		{
			if(i == order - 1 || arr[i + 1][j] != 0)
			{
				left = true;
				bottom = right = top = false;
				continue;
			}
			arr[++i][j] = ++count;
		}
		
		if(left)
		{
			if(j == 0 || arr[i][j - 1] != 0)
			{
				top = true;
				left = right = bottom = false;
				continue;
			}	
			arr[i][--j] = ++count;
		}
		
		if(top)
		{
			if(i == 0 || arr[i - 1][j] != 0)
			{
				right = true;
				left = bottom = top = false;
				continue;
			}	
			arr[--i][j] = ++count;
		}
	}

	for(i =0; i < order; i++)
	{
		for(j = 0; j < order; j++)
			std::cout << arr[i][j] << "  ";
		std::cout << "\n";
	}	
}