#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "op_atomic.h"

a_uint32_t thread_count = 0;
static atomic_flag lock[SLOT] = {ATOMIC_FLAG_INIT};
extern uint64_t counts[];

void* test(void *arg)
{
  const int power2 = *(int*) arg;
  uint64_t bound = 1L << power2;
  uint32_t tid = atomic_fetch_add_explicit(&thread_count, 1,
                                           memory_order_relaxed);
  uint32_t slot = tid % SLOT;
  for (uint64_t i = 0; i < bound; i++)
    {
      while (atomic_flag_test_and_set_explicit(&lock[slot], memory_order_acquire))
        ;
      counts[slot]++;
      atomic_flag_clear_explicit(&lock[slot], memory_order_release);
    }
  return NULL;
}