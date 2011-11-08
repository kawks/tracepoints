#include "trace.h"

#ifndef SIMPLE_TRACE

#define PAGE_SIZE 4096
#define PAGE_OF(_addr) ( ((uint64_t) (_addr)) & ~(PAGE_SIZE-1) )

static void unprotect(void *addr, size_t len) {
  uint64_t pg1 = PAGE_OF(addr),
           pg2 = PAGE_OF(addr + len - 1);
  if (mprotect((void *) pg1, pg2 - pg1 + PAGE_SIZE,
               PROT_READ | PROT_EXEC | PROT_WRITE)) {
    perror("mprotect");
    abort();
  }
}

void trace_init() {
  extern struct trace_desc trace_table_start[], trace_table_end[];
  struct trace_desc *desc;
  for (desc = trace_table_start; desc < trace_table_end; desc++)
    unprotect(desc->jump_from, 8);
}

void enable(tracepoint *point) {
  extern struct trace_desc trace_table_start[], trace_table_end[];
  struct trace_desc *desc;
  for (desc = trace_table_start; desc < trace_table_end; desc++) {
    if (desc->point != point)
      continue;

    int64_t offset = (desc->jump_to - desc->jump_from) - 5;
    if ((offset > INT32_MAX) || (offset < INT32_MIN)) {
      fprintf(stderr, "offset too big: %lx\n", offset);
      abort();
    }

    volatile uint64_t *dest = desc->jump_from;
    uint64_t instr = ((offset & ~((uint32_t)0)) << 8) | 0xe9;
    *dest = instr;
  }
}

void disable(tracepoint *point) {
  extern struct trace_desc trace_table_start[], trace_table_end[];
  struct trace_desc *desc;
  for (desc = trace_table_start; desc < trace_table_end; desc++) {
    if (desc->point != point)
      continue;

    volatile uint64_t *dest = desc->jump_from;
    *dest = 0x0000000000841f0fULL;
  }
}

#endif // not SIMPLE_TRACE
