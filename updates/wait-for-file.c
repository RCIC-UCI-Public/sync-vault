/* This program waits for a file to appear. It began as a query to perplexity and then was modified
 *
 * This file is open source and can be freely  copied without attribution  
 * 
 * Intent: blocking wait for a file to appear (uses inotify). In context of this repository, it can used to wait for
 *         an intializing secret to be placed at specific location so that a vault client can communicate with the vault server
 *
 * Call as:
 *       wait-for-file  <directory>  <filename>
 *  OR
 *       timeout <timeout-secs> wait-for-file <directory> <filename> 
 *
 *  returns 0 if file already exists or succesfully appears
 *  Note: printed diagnostics appear on standard err and can be sent to /dev/null for a no-message, script-friendly invocation
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define EVENT_BUF_LEN (1024 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory_to_watch> <target_filename>\n", argv[0]);
        return 1;
    }

    const char *dir_to_watch = argv[1];
    const char *target_filename = argv[2];
    int fd, wd;
    struct stat statbuf;
    char * fullpath;

    /* Check if the file exists */
    fullpath = (char *) calloc((strlen(dir_to_watch) + strlen(target_filename) + 3),sizeof(char));
    strcpy(fullpath,dir_to_watch);
    strcat(fullpath,"/");
    strcat(fullpath,target_filename);
    if (stat(fullpath,&statbuf) == 0)
    {
       fprintf(stderr, "-- %s -- already exists. Exiting\n",fullpath);
       return(0);
    }
	
    /* File isn't available yet, so use inotify to wait for appearance */

    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return 1;
    }

    wd = inotify_add_watch(fd, dir_to_watch, IN_CREATE);
    if (wd < 0) {
        perror("inotify_add_watch");
        close(fd);
        return 1;
    }

    fprintf(stderr, "Waiting for %s to appear in %s...\n", target_filename, dir_to_watch);

    char buffer[EVENT_BUF_LEN];
    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->mask & IN_CREATE) {
                if (strcmp(event->name, target_filename) == 0) {
                    fprintf(stderr, "File %s appeared\n", target_filename);
                    close(fd);
                    return 0;
                }
            }
            i += sizeof(struct inotify_event) + event->len;
        }
    }

    close(fd);
    return 1;
}

