#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "process.h"

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

	int call_num = (*(int *)f -> esp);
	void * argv[3];
	int i;

	if (call_num == SYS_CREATE || call_num == SYS_SEEK) { // 2 arguments
		for (i = 0; i<2; i++) {
			void * arg_ptr = (void *) f -> esp + sizeof(uint32_t) * (i+1);
			validate_ptr(arg_ptr);
			argv[i] = arg_ptr;
		}
	}
	else if (call_num == SYS_READ || call_num == SYS_WRITE) { // 3 arguments
		for (i = 0; i<3; i++) {
			void * arg_ptr = (void *) f -> esp + sizeof(uint32_t) * (i+1);
			validate_ptr(arg_ptr);
			argv[i] = arg_ptr;
		}
	}
	else if (0 <= call_num && call_num <= 12) { // 1 argument
		void * arg_ptr = f -> esp + sizeof(uint32_t);
		validate_ptr(arg_ptr);
		argv[0] = arg_ptr;
	}
	else
		// error!

	switch (call_num) {
		case SYS_HALT:
			sys_halt();
		break;
		case SYS_EXIT:
			sys_exit(*(int *) argv[0]);
		break;
		case SYS_EXEC:
			sys_exec(*(char **) argv[0]);
		break;
		case SYS_WAIT:
			sys_wait(*(pid_t *) argv[0]);
		break;
		case SYS_CREATE:
			sys_create(*(char **) argv[0], *(unsigned *) argv[1]);
		break;
		case SYS_REMOVE:
			sys_remove(*(char **) argv[0]);
		break;
		case SYS_OPEN:
			sys_open(*(char **) argv[0]);
		break;
		case SYS_FILESIZE:
			sys_filesize(*(int *) argv[0]);
		break;
		case SYS_READ:
			sys_read(*(int *) argv[0], *(void **) (argv[1]), *(unsigned *) argv[2]);
		break;
		case SYS_WRITE:
			sys_write(*(int *) argv[0], *(void **) (argv[1]), *(unsigned *) argv[2]);
		break;
		case SYS_SEEK:
			sys_seek(*(int *) argv[0], *(unsigned *) argv[1]);
		break;
		case SYS_TELL:
			sys_tell(*(int *) argv[0]);
		break;
		case SYS_CLOSE:
			sys_close(*(int *) argv[0]);
		break;
		default:
			//An error occured. Do something.
		break;
	}
}

bool validate_ptr (void * ptr) {
	return true; // check that pointer is within user memory/legal
}

void sys_halt (void) {

}

void sys_exit (int status) {

  struct thread *cur = thread_current();
  cur->child->exit = true;

  printf ("%s: exit(%d)\n", cur->name, status);
  
  thread_exit();


}

pid_t sys_exec (const char *file) {

	int pid = process_execute(file);
	struct child_proccess * cp =  add_child_to_cur_parent (pid);

	if (!cp){
		printf ( "process_execute failed !!!!\n");
	}

	return pid;
}

int sys_wait (tid_t pid) {
	
	printf("WAITING\n");
	return process_wait(pid);
}

bool sys_create (const char *file, unsigned initial_size) {
	return 0; //replace this with something usefull
}

bool sys_remove (const char *file) {
	return 0; //replace this with something usefull
}

int sys_open (const char *file) {
	return 0; //replace this with something usefull
}

int sys_filesize (int fd) {
	return 0; //replace this with something usefull
}

int sys_read (int fd, void *buffer, unsigned size) {
	return 0; //replace this with something usefull
}

int sys_write (int fd, const void *buffer, unsigned size) {
	
	printf("fd = %u\n", fd);
	if( fd == 1){
		putbuf( buffer, size);
		return size;
	}


	return 0; //replace this with something usefull
}

void sys_seek (int fd, unsigned position) {

}

unsigned sys_tell (int fd) {
	return 0; //replace this with something usefull
}

void sys_close (int fd) {

}

