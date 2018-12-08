/*
   main.c - handle command line arguments, load
   game settings, manage main data structures
*/

#include "core.h"
#include "file_io.h"
#include "random.h"
#include "unistd.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
// #include "tests.h"

using namespace std;

bool file_exists(const char *filename)
{
    struct stat buffer = {0};
    return stat(filename, &buffer) == 0;
}

int main(int argc, char *argv[])
{
    debug_init();
    log_info("--- shiromino start ---\n");
    
    coreState cs;
    coreState_initialize(&cs);
    struct settings *s = NULL;
    const char path[] = "app0:";
    
    string calling_path {path};
    string cfg = "game.cfg";
    string slash = "/";
    string cfg_filename;

    game_t *distr_test = NULL;

    cs.calling_path = (char *)malloc(strlen(path) + 1);
    strcpy(cs.calling_path, path);

    g123_seeds_init();
    /*
       g2_output_seed_syncs();
       goto error;

       g2_output_sync_histogram();
       goto error;
    */

    cfg_filename = calling_path;
    cfg_filename.append(slash);
    cfg_filename.append(cfg);

    if(!file_exists(cfg_filename.c_str()))
    {
        log_err("Couldn't find configuration file , aborting\n");
        goto error;
    }

    s = parse_cfg(cfg_filename.c_str());
    if(!s)
    {
        log_info("Using default settings\n");
    }

    cs.cfg_filename = (char *)(cfg_filename.c_str());

    log_info("Finished reading configuration file: %s\n", cs.cfg_filename);

    if(init(&cs, s))
    {
        log_err("Initialization failed, aborting.\n");
        quit(&cs);
        coreState_destroy(&cs);
        return 1;
    }

    run(&cs);

    quit(&cs);
    coreState_destroy(&cs);

    return 0;

error:
    coreState_destroy(&cs);
    return 1;
}
