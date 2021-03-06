#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

#include "threads/synch.h"

#define EXITED 0
#define KILLED -1

#define LOADING 0
#define LOAD_PASSED 1
#define LOAD_FAILED 2

struct child_proc
{
  tid_t pid;
  bool exit;
  int process_status;
  int load_status;
  struct list_elem child_elem;
};


tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
