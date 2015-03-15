#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

#include "threads/synch.h"


/*Child_Process Status DEFINES*/

#define NOT_LOADED 0
#define LOAD_PASSED 1
#define LOAD_FAILED -1
/*Load Status ?? */



struct child_process
{
  tid_t pid;
  
  bool wait;
  bool exit;

  int process_status;

  int load_status;
  struct list_elem child_elem;
  struct lock child_lock;
};


tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
