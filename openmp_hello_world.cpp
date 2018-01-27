#include <iostream>
#include <omp.h>

int main()
{
	#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		std::cout<<"Hello "<<ID<<std::endl;
		std::cout<<"World "<<ID<<std::endl;
	}
}		
