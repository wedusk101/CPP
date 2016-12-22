#include<iostream>
#include<cmath>

int main()
{
	using std::cout;
	using std::cin;
	using std::endl;	    
	int m, t=0, p=1, flag=0;
        cout<<"Enter a prime number."<<endl;
	cin>>m;
    	while(p <= m)
    	{
        	t=(int)pow(2,p);
        	if(t==(m+1))
        	{
            	cout<<"Number is a Mersenne Prime."<<endl;
            	flag=1;
            	break;
        	}
        p++;
    	}
    	if(flag!=1)
    	{
        	cout<<"Number is not a Mersenne Prime."<<endl;
    	}
	return 0;
}
