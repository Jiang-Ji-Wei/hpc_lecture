#include <cstdio>
#include <cstdlib>
#include <vector>

int main() {
  int n = 50;
  int range = 5;
  std::vector<int> key(n);
  #pragma omp parallel for 
  for (int i=0; i<n; i++)
    key[i] = rand() % range;//random number 0~4
  #pragma omp single
  for (int i = 0; i < n; ++i)
    printf("%d ", key[i]);
  printf("\n");

  std::vector<int> bucket(range,0);//bucket(0,0,0,0,0) 
  #pragma omp parallel for shared(bucket)
  for (int i=0; i<n; i++)       //bucket[i]=number of i in key()
  #pragma omp atomic update
    bucket[key[i]]++;

  std::vector<int> offset(range,0);// offset(0,0,0,0,0)
  std::vector<int> offset_out(range,0);
  #pragma omp parallel
  {
  //I tried but failed.
  //#pragma omp single
  //for (int i=1; i<range; i++)//offset[i]:loaction of num change on key
  //  offset[i] = offset[i-1] + bucket[i-1];
  for (int j = 1; j < range; j<<=1){
  #pragma omp for 
    for (int i = 0; i < range; i++)
        offset_out[i] = offset[i];
  #pragma omp for
    for (int i = j; i < range; i++)
        offset[i] += offset_out[i-j];
  }
  #pragma omp for
    for (int i = 1; i < range; i++)
        offset[i] = offset[i] + bucket[i-1];

  #pragma omp single
  for (int i=0; i<range; i++) {
    int j = offset[i];
    for (; bucket[i]>0; bucket[i]--) {
      key[j++] = i;
    }
  }
  #pragma omp single
  for (int i=0; i<n; i++) {
    printf("%d ",key[i]);
  }
  #pragma omp single
  printf("\n");
  }
}
