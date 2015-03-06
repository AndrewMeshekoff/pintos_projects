#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

#include "threads/synch.h"


/*Child_Process Status DEFINES*/

#define C_WAIT 0
#define C_EXIT 1
#define UNDEFINED -1
/*Load Status ?? */

struct child_process
{
  tid_t pid;
  
  bool wait;
  bool exit;

  int load_status;
  struct list_elem child_elem;
  struct lock child_lock;
};

struct child_process * add_child_to_cur_parent (int pid);
struct child_process * get_child(int pid);
void remove_child (struct child_process *cp);
void remove_all_cur_children (void);



tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
