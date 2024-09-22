#ifndef _WORKER_H_
#define _WORKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
//#include <zephyr/kernel/thread_stack.h>
#include <zephyr/kernel.h>
typedef uint32_t eventmask_t;
#define	EVENT_MASK(eid) ((eventmask_t)(1 << (eid)))

typedef struct worker {
//  thread_descriptor_t *desc;
  bool critical;
  bool autostart;
//  thread_t *tp;
  struct worker *next;
  struct worker *prev;
} worker_t;

#define THREAD_MGR_WA_SIZE 0x100
#define THREAD_MGR_PRIORITY 1
#define CO_NMT_OPERATIONAL 1 // temporary so it will compile

void thread_mgr(void *, void *, void *);

/*
K_THREAD_STACK_DEFINE(thread_mgr_wa, THREAD_MGR_WA_SIZE);
K_THREAD_DEFINE(
  thread_mgr_tid, THREAD_MGR_WA_SIZE,
  thread_mgr, NULL, NULL, NULL,
  THREAD_MGR_PRIORITY, 0, 0
);
//*/

/*
K_THREAD_STACK_DEFINE(blinky_stack_area, BLINKY_STACK_SIZE);
K_THREAD_DEFINE(
  blinky_tid, BLINKY_STACK_SIZE,
  blinky_entry, NULL, NULL, NULL,
  BLINKY_PRIORITY, 0, 0
);
*/
//extern THD_WORKING_AREA(thread_mgr_wa, 0x100);
//extern THD_FUNCTION(thread_mgr, arg);

/* OreSat worker thread API */
//void reg_worker(worker_t *wp, thread_descriptor_t *desc, bool critical, bool autostart);
//void unreg_worker(worker_t *wp);
//thread_t *start_worker(worker_t *wp);
//msg_t stop_worker(worker_t *wp);
void start_workers(void);
void stop_workers(bool stop_crit);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*_WORKER_H_*/
