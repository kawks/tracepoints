#include "trace.h"

tracepoint fact_entry, fact_exit;

int fact(int n) {
  TRACE(fact_entry, "fact(%d)\n", n);

  int out = 1;
  if (n > 1)
    out = n * fact(n-1);

  TRACE(fact_exit, "fact(%d) = %d\n", n, out);
  return out;
}

int main() {
  trace_init();

  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      while (1)
      {
        #pragma omp task
        {
          printf("result 00 is %d\n\n", fact(5));

          enable(&fact_entry);
          printf("result 01 is %d\n\n", fact(5));

          enable(&fact_exit);
          printf("result 11 is %d\n\n", fact(5));

          disable(&fact_entry);
          printf("result 10 is %d\n\n", fact(5));

          disable(&fact_exit);
        }
      }
    }
  }

  return 0;
}
