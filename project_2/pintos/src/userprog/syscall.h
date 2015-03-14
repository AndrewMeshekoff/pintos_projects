#include <user/syscall.h>
#include <list.h>

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

struct list file_list; //has to be initialized. where?

struct file_info {
	const char file_name[17];
	int file_des;
	struct file * f;
	struct list_elem file_elem;
};

void validate_ptr (void * ptr);
void syscall_init (void);
void sys_halt (void);
void sys_exit (int status);
pid_t sys_exec (const char *file);
int sys_wait (pid_t pid);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove (const char *file);
int sys_open (const char *file);
int sys_filesize (int fd);
int sys_read (int fd, void *buffer, unsigned size);
int sys_write (int fd, const void *buffer, unsigned size);
void sys_seek (int fd, unsigned position);
unsigned sys_tell (int fd);
void sys_close (int fd);

bool validate_file(const char *file);
bool validate_page(const char *file);

struct lock sys_lock;

#endif /* userprog/syscall.h */
