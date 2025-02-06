//! [null_pointer]
#include <signal.h>
#include <stdio.h>
#include <exceptions4c.h>

const struct e4c_exception_type SEGFAULT = {NULL, "Segmentation fault"};

void segfault(int _) {
  signal(SIGSEGV, segfault);
  THROW(SEGFAULT, NULL);
}

int main(void) {
  int * null_pointer = NULL;

  signal(SIGSEGV, segfault);

  TRY {
    *null_pointer = 123;
  } CATCH(SEGFAULT) {
    printf("Danger avoided!\n");
  }

  return EXIT_SUCCESS;
}
//! [null_pointer]
