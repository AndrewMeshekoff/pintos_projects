#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"




#define STACK_LIMIT ((void *) 0x08048000)


struct file * get_file(int fd)
{
  struct thread *t = thread_current();
  struct list_elem *e;

  for (e = list_begin (&t->file_list); e != list_end (&t->file_list);
       e = list_next (e))
   {
        struct file_info * fi = list_entry (e, struct file_info, file_elem);
        if (fd == fi->file_des)
	    {
	      return fi->f;
	    }
   }
  
   return NULL;

}
struct file * close_file(int fd){

}
struct file * remove_file(int fd){}



static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&file_lock);
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
  int s = cur->child->process_status;


  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();

  return s;
}

pid_t sys_exec (const char * file) {
	validate_page(file);
	validate_ptr(file);

	char file_cpy[256];
	strlcpy(file_cpy, file, 256);
	printf("%s\n", file);
	pid_t pid = process_execute(file_cpy);
	struct child_process *cp;
	cp = get_child (pid);

	if (!cp)
		sys_exit(-1);

	while(cp->load_status == LOADING)
		barrier();

	if (cp->load_status == LOAD_PASSED)
		return pid;

	return -1;
}

int sys_wait (tid_t pid) {
	return process_wait(pid);
}

bool sys_create (const char *file, unsigned initial_size) {	
	validate_file(file);
	validate_page(file);
       
	bool file_created;
	lock_acquire(&file_lock);
	file_created = filesys_create(file, initial_size);
	lock_release(&file_lock);

	return file_created; //replace this with something usefull
}

bool sys_remove (const char *file) {
	return 0;
}

int sys_open (const char *file) {
	
	validate_file(file);
	validate_page(file);

	lock_acquire(&file_lock);
	
	struct file * f = filesys_open(file);
	
	if(!f){
		lock_release(&file_lock);
		return -1;
	}

	
	struct thread * cur = thread_current();
	struct file_info * opened = malloc(sizeof(struct file_info));
	strlcpy(opened->file_name, file, 17);
	opened->f = f;
	opened->file_des = cur->files;
	(cur->files)++;
	list_push_back(&cur->file_list, &opened->file_elem);

	lock_release(&file_lock);
	
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

	lock_acquire(&file_lock);


	lock_release(&file_lock);

	return 0; //replace this with something usefull
}

void sys_seek (int fd, unsigned position) {

}

unsigned sys_tell (int fd) {
	return 0; //replace this with something usefull
}

void sys_close (int fd) {
  struct thread *cur = thread_current();
  struct list_elem *it;

  if(fd == 1 || fd == 0) {
    return;
  }

  lock_acquire(&file_lock);
  for (it = list_begin(&cur->file_list); it != list_end(&cur->file_list);  it = list_next(it))
  {
      struct file_info * closing = list_entry (it, struct file_info, file_elem);
      if (closing->file_des == fd)
      {
          list_remove(it);
          file_close(closing->f);
          free(closing);
	  lock_release(&file_lock);
          return;
      }
  }
  lock_release(&file_lock);
}

