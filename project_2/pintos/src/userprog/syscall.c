#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

struct lock sys_lock;


void
syscall_init (void) 
{
  lock_init(&sys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


//whats a intr frame?
static void
syscall_handler (struct intr_frame *f UNUSED) 
{


  //printf ("system call!\n");
  thread_exit ();
}


