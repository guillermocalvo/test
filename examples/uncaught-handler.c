#include <signal.h>
#include <stdio.h>
#include <exceptions4c.h>

const struct e4c_exception_type MY_ERROR = {NULL, "My error"};

//! [uncaught_handler]
static void my_uncaught_handler(const struct e4c_exception * exception) {
  fprintf(stderr, "UNCAUGHT: %s\n", exception->message);
}

int main(int argc, char * argv[]) {
  e4c_get_context()->uncaught_handler = my_uncaught_handler;
  THROW(MY_ERROR, "Oops");
}
//! [uncaught_handler]
