#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

typedef int tracepoint;


#ifdef SIMPLE_TRACE

#define TRACE(_point, _args...) \
  do {                          \
    if (_point) printf(_args);  \
  } while (0)

static inline void enable(tracepoint *point) {
  *point = 1;
}


#else // not SIMPLE_TRACE

#define NOP8 ".byte 0x0f, 0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00;"

#define TRACE(_point, _args...)          \
  do {                                   \
    asm goto (                           \
      "0: " NOP8                         \
      ".pushsection trace_table, \"a\";" \
      ".quad " #_point ", 0b, %l0;"      \
      ".popsection"                      \
      : : : : __lbl_##_point);           \
    if (0) {                             \
      __lbl_##_point: printf(_args);     \
    }                                    \
  } while (0)

struct trace_desc {
  tracepoint *point;
  void *jump_from;
  void *jump_to;
} __attribute__((packed));

void trace_init();
void enable(tracepoint *point);
void disable(tracepoint *point);

#endif // not SIMPLE_TRACE
