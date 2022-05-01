#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    int syscal_number = *(int *) f->esp; // syscall num
    f->esp += sizeof(int); // get 1st arg (fd)

    if (syscal_number == SYS_WRITE){
        // int fd = *(int *) f->esp;
        int fd = (int) f->esp;
        if (fd != STDOUT_FILENO)
            thread_exit();
        // f->esp += sizeof(const void *);
        f->esp += sizeof(int); // get 2nd arg (buffer)
        // const char * buffer = *(const char *) f->esp;
        const char * buffer = (const char *) f->esp;
        f->esp += sizeof(const void *); // get 3rd arg (size)
        // unsigned size = *(unsigned *) f->esp;
        unsigned size = (unsigned) f->esp;
        putbuf(buffer, (size_t) size);
        f->eax = (int) size; // asummes all bytes were written
    }
    else if (syscal_number == SYS_EXIT){
        // int status = *(int *) f->esp;
        int status = (int) f->esp;
        thread_current()->exit_status = status;
        f->eax = status;
    }
    thread_exit();
  // printf ("system call!\n");
  // thread_exit ();
}
