#include <stdio.h>
int* p;
int bad_function() { return *p; }
int main() {
  printf("1\n2\n3\n"); fflush(stdout); bad_function();
}
