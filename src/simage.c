/* 
 * Copyright (c) 2016, Michael W. Bauer. All rights reserved.
 * 
 * “Singularity” Copyright (c) 2016, The Regents of the University of California,
 * through Lawrence Berkeley National Laboratory (subject to receipt of any
 * required approvals from the U.S. Dept. of Energy).  All rights reserved.
 * 
 * This software is licensed under a customized 3-clause BSD license.  Please
 * consult LICENSE file distributed with the sources of this project regarding
 * your rights to use or distribute this software.
 * 
 * NOTICE.  This Software was developed under funding from the U.S. Department of
 * Energy and the U.S. Government consequently retains certain rights. As such,
 * the U.S. Government has been granted for itself and others acting on its
 * behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software
 * to reproduce, distribute copies to the public, prepare derivative works, and
 * perform publicly and display publicly, and to permit other to do so. 
 * 
 */


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <errno.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <fcntl.h>
#include <grp.h>
#include <libgen.h>
#include <linux/limits.h>

#include "config.h"
#include "lib/singularity.h"
#include "util/file.h"
#include "util/util.h"

int main(int argc, char ** argv) {
    if ( argv[1] == NULL ) {
        fprintf(stderr, "USAGE: simage command args\n");
        return(1);
    
    }

    //Loop through argv, each time chopping off argv[0], until argv[1] is a relevant shell script or is empty
    singularity_priv_init(); //Make sure user is running as root before we add SUID code
    while ( 1 ) {
        singularity_message(DEBUG, "Running %s %s workflow\n", argv[0], argv[1]);

        singularity_priv_escalate();
        if ( argv[1] == NULL ) {
            singularity_message(DEBUG, "Finished running simage command and returning\n");
            return(0);

        } else if ( strcmp(argv[1], "mount") == 0 ) {
            if ( singularity_image_mount(argc - 1, &argv[1]) != 0 ) {
                singularity_priv_drop_perm();
                return(1);
            }
        
        } else if ( strcmp(argv[1], "bind") == 0 ) {
            if ( singularity_image_bind(argc - 1, &argv[1]) != 0 ) {
                singularity_priv_drop_perm();
                return(1);
            }
        
        } else if ( strcmp(argv[1], "create") == 0 ) {
            if ( singularity_image_extern_create(argc - 1, &argv[1]) != 0 ) {
                singularity_priv_drop_perm();
                return(1);
            } else {
                singularity_priv_drop_perm();
                return(0);
            }
        
        } else if ( strcmp(argv[1], "expand") == 0 ) {
            if ( singularity_image_extern_expand(argc - 1, &argv[1]) != 0 ) {
                singularity_priv_drop_perm();
                return(1);
            }
        
        } else {
            singularity_priv_drop_perm(); //Drop all privs permanently and return to calling user
            return(singularity_fork_exec(&argv[1])); //Can NOT run this with root privs
        }
      
        argv++;
        argc--;
        singularity_priv_drop();
    }
}