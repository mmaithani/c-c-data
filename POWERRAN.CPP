#include<iostream.h>
#include<conio.h>
#include<process.h>
#include<dos.h>

void main()
{
clrscr();
int sol,temp=1,num,power,i;
char n;
cout<<"\t\2\2\2\2Enter the number=>";
cin>>num;
 cout<<"\n Enter the power of numberifnot havepowerthan press n=> ";
 cin>>power;
 if(power==0)
 {temp=1;}
 else if(power==n)
 { sol=num*num;
 cout<<sol;
 exit(0);
  }

 for(i=1;i<=power;i++)
  {
  temp=temp*num;

  }
 cout<<temp;
 getch();
}












































