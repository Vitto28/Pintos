#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/palloc.h"
#include "userprog/process.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

typedef void (*handler) (struct intr_frame *);
static void syscall_exit (struct intr_frame *f);
static void syscall_write (struct intr_frame *f);
static void syscall_wait (struct intr_frame *f);
static void syscall_exec (struct intr_frame *f);

// 20 = how many syscalls we can have
#define SYSCALL_MAX_CODE 19 // array of 20
static handler call[SYSCALL_MAX_CODE + 1];

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  /* Any syscall not registered here should be NULL (0) in the call array. */
  memset(call, 0, SYSCALL_MAX_CODE + 1);

  /* Check file lib/syscall-nr.h for all the syscall codes and file
   * lib/user/syscall.c for a short explanation of each system call. */
  call[SYS_EXIT]  = syscall_exit;   // Terminate this process.
  call[SYS_WRITE] = syscall_write;  // Write to a file.
  call[SYS_WAIT] = syscall_wait;    // Calling process blocks and waits for child
  call[SYS_EXEC] = syscall_exec;    // Runs executable
}

static void
syscall_handler (struct intr_frame *f)
{
  // take 1st el from stack (f->esp)
  int syscall_code = *((int*)f->esp);
  call[syscall_code](f);
}

/* Project: Here we implement the various syscalls */

static void
syscall_exit (struct intr_frame *f)
{
  int *stack = f->esp;
  struct thread* t = thread_current ();
  t->exit_status = *(stack+1); // set to 2nd el in stack
  thread_exit ();
}

static void
syscall_write (struct intr_frame *f)
{
  int *stack = f->esp;
  ASSERT (*(stack+1) == 1); // file desc == 1 (console) ?
  char * buffer = *(stack+2);
  int    length = *(stack+3);
  putbuf (buffer, length);
  f->eax = length; // # bytes written
}

static void
syscall_wait (struct intr_frame *f)
{
  // TODO: doesnt work
  int *stack = f->esp;
  tid_t child_tid = *(stack+1);
  f->eax = process_wait(child_tid);
}

static void
syscall_exec(struct intr_frame * f) {
  // TODO
  int *stack = f->esp;
  const char * file = *(stack+1);
  f->eax = process_execute(file);
}
