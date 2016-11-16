#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int* p;

int main() {
  char cmd[100];
  while (!feof(stdin)) {
    printf("> "); fflush(stdout);
    fgets(cmd, sizeof(cmd), stdin);
    if (!strcmp(cmd, "\n") || !strcmp(cmd, ""))
      continue;
    else if (!strcmp(cmd, "quit\n"))
      break;
    else if (!strcmp(cmd, "abort\n"))
      abort();
    else if (!strcmp(cmd, "segv\n"))
      printf("the thing at null is %d\n", *p);
    else
      printf("unknown command. try abort / segv / quit\n");
  }
}
