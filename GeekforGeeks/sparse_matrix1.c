#include<stdio.h>
int main()
{
int tri[100][3],tri1[100][3],i,j,r,k=0,m=0;
printf("Enter the range of square matrix:\t");
scanf("%d",&r);
printf("\nEnter the elements for the 1st matrix :\n");
int temp=0;
for(i=0;i<r;i++)
{
for(j=0;j<r;j++)
{
scanf("%d",&temp);
if(temp!=0)
{ tri[k][0]=i;
tri[k][1]=j;
tri[k][2]=temp;
k++;}
}
}
printf("\nEnter the elements for the 2st matrix :\n");
for(i=0;i<r;i++)
{
for(j=0;j<r;j++)
{
scanf("%d",&temp);
if(temp!=0)
{ tri1[m][0]=i;
tri1[m][1]=j;
tri1[m][2]=temp;
m++;}
}
}
printf("\nThe Sparse Matrix triplet Representation is:\n");
printf(" MATRIX 1 \n");
for(i=0;i<k;i++)
{
for(j=0;j<3;j++)
{
printf("%d\t",tri[i][j]);
}
printf("\n");
}
printf(" MATRIX 2 \n");
for(i=0;i<m;i++)
{
for(j=0;j<3;j++){
printf("%d\t",tri1[i][j]);
}
printf("\n");
}
int l=0,tra[100][3];
if(k>0)
{
for(j=0;j<r;j++)
{
for(i=0;i<k;i++)
{
if(tri[i][1]==j)
{
tra[l][0]=tri[i][1];
tra[l][1]=tri[i][0];
tra[l][2]=tri[i][2];
l++;
}
}
}
}
printf("\nThe Transposed Sparse Matrix of 1st Matrix in triplet Representation is:\n");
for(i=0;i<k;i++)
{
for(j=0;j<3;j++)
{
printf("%d\t",tra[i][j]);
}
printf("\n");
}
l=0;
for(i=0;i<r;i++)
{
for(j=0;j<k;j++)
{
if(tri[j][1]==i || tri1[j][1]==i)
{ if(tri[j][1]==i && tri1[j][1]==i && (tri[j][2]+tri1[j][2])!=0)
{ tra[l][0]=tri[j][1];
tra[l][1]=tri[j][0];
tra[l][2]=tri[j][2]+tri1[j][2];
l++;
}
else if(tri[j][1]==i)
{ tra[l][0]=tri[j][1];
tra[l][1]=tri[j][0];
tra[l][2]=tri[j][2];l++;}
else
{ tra[l][0]=tri1[j][0];
tra[l][1]=tri1[j][1];
tra[l][2]=tri1[j][2];l++;}}
}
}
printf("\nAddition of Matrix (A+B) :\n");
for(i=0;i<l;i++)
{
for(j=0;j<3;j++)
{
printf("%d\t",tra[i][j]);
}
printf("\n");
}
return 0;
}
