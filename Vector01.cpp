//Vector01.cpp
#include <iostream>
#include "Vector.h"

using namespace std;

int main()
{
		double x = 0.0, y = 0.0, z = 0.0;
		int choice = 0, flag = 0;
		Vector v1, v2;
		cout<<"This program implements a class Vector with the following basic operations."<<endl;
		do
		{
			cout<<"1. Create a vector."<<endl;
			cout<<"2. Display a vector."<<endl;
			cout<<"3. Get the magnitude of a vector."<<endl;
			cout<<"4. Get the dot product of two vectors."<<endl;
			cout<<"5. EXIT"<<endl;
			cout<<"Please enter your choice."<<endl;
			cin>>choice;
			switch(choice)
			{
				case 1:		cout<<"Please enter the value of the x component."<<endl;
							cin>>x;
							cout<<"Please enter the value of the y component."<<endl;
							cin>>y;
							cout<<"Please enter the value of the z component."<<endl;
							cin>>z;
							v1.initVector(x, y ,z);
							flag = 1;
							cout<<"Vector created successfully."<<endl;
							break;
							
				case 2:		{
								if(flag == 0)
								{
									cout<<"First create a vector! Operation aborted."<<endl;
									break;
								}
								v1.display();
							}
							break;
							
				case 3: 	{
								if(flag == 0)
								{
									cout<<"First create a vector! Operation aborted."<<endl;
									break;
								}
								cout<<"The magnitude of the vector is "<<v1.magnitude()<<" units."<<endl;
							}
							break;
							
				case 4: 	{
								if(flag == 0)
								{
									cout<<"First create a vector! Operation aborted."<<endl;
									break;
								}
								cout<<"Please create a second vector."<<endl;
								cout<<"Please enter the value of the x component."<<endl;
								cin>>x;
								cout<<"Please enter the value of the y component."<<endl;
								cin>>y;
								cout<<"Please enter the value of the z component."<<endl;
								cin>>z;
								v2.initVector(x, y ,z);
								cout<<"vector created successfully."<<endl;
								cout<<"The dot product of the vectors is "<<v1.dotProduct(v2)<<" ."<<endl;
							}
							break;
							
				case 5:		cout<<"Thank you."<<endl;
							break;
							
				default:	cout<<"Invalid input!"<<endl;
							break;			
			}
		}while(choice != 5);
		return 0;		
}