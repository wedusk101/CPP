#include<iostream>
 
 int main()
 {
	 using std::cin;
	 using std::cout;
	 using std::endl;
	 int i = 0, n = 0;
	 cout<<"How many numbers would you like to have on your list?"<<endl;
	 cin>>n;
	 int *list = new int [n];
	 cout<<"Please enter the inetegrs."<<endl;
	 for(i = 0; i < n; i++)
	 {
		 cin>>list[i];
 	 }
	 cout<<"The numbers you have added are ";
	 for(i = 0; i < n; i++)
	 {
		 cout<<*(list + i)<<" ";
	 }
	 cout<<"."<<endl;
	 delete [] list;
	 return 0;
 }