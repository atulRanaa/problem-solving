 #include <stdio.h>
int main(){
#if __cplusplus==201402L
  printf("C++14\n");
#elif __cplusplus==201103L
  printf("C++11\n");
#else
  printf("C++\n");
#endif
  return 0;
}