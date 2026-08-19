#include <bits/stdc++.h>
#include <iostream>
using namespace std;
struct flights{
  string s;
  int h;
  int m;
  int time;
  int price;
 };

int main(){
vector<flights> details;
  char ch;
  flights object;
  cin>>object.s;
  cin>>object.h>>ch>>object.m;
  cin>>object.time;
  cin>>object.price;
details.push_back(object);
   cout<<details[0].s<<" "<<details[0].h<<":"<<details[0].m<<" "<<details[0].time<<" "<<details[0].price;
  return 0;
}
