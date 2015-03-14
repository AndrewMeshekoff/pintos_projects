#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "process.h"
#include "userprog/pagedir.h"

#define STACK_LIMIT ((void *) 0x08048000)


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&sys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void * get_NULL (char * arg) {
	int i;
	for (i = 0; arg[i] != '\0'; i++) {}
	return (void *) arg;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	validate_ptr( (const void*) f->esp);
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
	else {
		
		return ;
	}

	switch (call_num) {
		case SYS_HALT:
			sys_halt();
		break;
		case SYS_EXIT:
			 sys_exit(*(int *) argv[0]);
		break;
		case SYS_EXEC:
			f->eax = sys_exec(*(char **) argv[0]);
		break;
		case SYS_WAIT:
			f->eax = sys_wait(*(pid_t *) argv[0]);
		break;
		case SYS_CREATE:
			f->eax = sys_create(*(char **) argv[0], *(unsigned *) argv[1]);
		break;
		case SYS_REMOVE:
			f->eax = sys_remove(*(char **) argv[0]);
		break;
		case SYS_OPEN:
			f->eax = sys_open(*(char **) argv[0]);
		break;
		case SYS_FILESIZE:
			f->eax = sys_filesize(*(int *) argv[0]);
		break;
		case SYS_READ:
			f->eax = sys_read(*(int *) argv[0], *(void **) (argv[1]), *(unsigned *) argv[2]);
		break;
		case SYS_WRITE:
			f->eax = sys_write(*(int *) argv[0], *(void **) (argv[1]), *(unsigned *) argv[2]);
		break;
		case SYS_SEEK:
			 sys_seek(*(int *) argv[0], *(unsigned *) argv[1]);
		break;
		case SYS_TELL:
			f->eax = sys_tell(*(int *) argv[0]);
		break;
		case SYS_CLOSE:
			sys_close(*(int *) argv[0]);
		break;
		default:
			//An error occured. Do something.
		break;
	}
}

void validate_ptr (void * ptr) {


	if( is_user_vaddr(ptr) && ptr >= STACK_LIMIT ){ 
		return;	
	
	}

	sys_exit(-1);
}

bool validate_file(const char *file){
	if(!file)
		sys_exit(-1);
	
	return; 
}

bool validate_page(const char *file){
	void * page = pagedir_get_page(thread_current()->pagedir, file);
	if (!page)
	{
		sys_exit(-1);
	}	
}

void sys_halt (void) {
	shutdown_power_off();
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
		//printf ( "process_execute failed !!!!\n");
		sys_exit(-1);
	}


	return pid;
}

int sys_wait (tid_t pid) {
	return process_wait(pid);
}

bool sys_create (const char *file, unsigned initial_size) {	
	validate_file(file);
	validate_page(file);
       
	bool file_created;
	lock_acquire(&sys_lock);
	file_created = filesys_create(file, initial_size);
	lock_release(&sys_lock);

	return file_created; //replace this with something usefull
}

bool sys_remove (const char *file) {
	return 0; //replace this with something usefull
}

int sys_open (const char *file) {
	
	validate_file(file);
	validate_page(file);

	lock_acquire(&sys_lock);
	
	struct file * f = filesys_open(file);
	
	if(!f){
		lock_release(&sys_lock);
		return -1;
	}

	struct thread * cur = thread_current();
	struct file_info * opened = malloc(sizeof(struct file_info));
	strlcpy(opened->file_name, file, 17);
	opened->f = f;
	opened->file_des = cur->files;
	(cur->files)++;
	list_push_back(&cur->file_list, &opened->file_elem);
	

	printf("file success: %s\n", opened->file_name);
	lock_release(&sys_lock);
	
	return opened->file_des;
}

int sys_filesize (int fd) {

	return 0; //replace this with something usefull
}

int sys_read (int fd, void *buffer, unsigned size) {
	return 0; //replace this with something usefull
}

int sys_write (int fd, const void *buffer, unsigned size) {
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

