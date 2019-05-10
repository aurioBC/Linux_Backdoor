//dir_watch.h
#ifndef DIR_WATCH_H
#define DIR_WATCH_H

#include "../include/utils.h"
#include <stdio.h>
#include <pthread.h>

/* MACROS */
#define BUF_LEN	(1024 * (EVENT_SIZE + 16))
#define EVENT_SIZE (sizeof (struct inotify_event))

/* FUNCTION PROTOTYPES */
void *run_dir_watch(void *ptr);
void send_file_name(char *name, Server_Args *srv_args);

#endif
