#ifndef FOXAR_H
#define FOXAR_H

#include <stdio.h>

typedef enum {
    NORMAL,
    HARD_LINK,
    SOFT_LINK
} FoxArFileType;

typedef struct {
    char *path; // path - done
    char *linkPath; // if link then path - done
    int pathSize; // size of path ( max 4096 bytes ) - done
    int mode; // file mode - perms - done
    int oid; // owner id - done
    int gid; // group id - done
    unsigned long size; // size of file ( bytes ) - done
    unsigned long lastM; // last modification UNIX time - done
    int checksum; // header checksum - done
    int linkPathSize; // size of path for link - done
    FoxArFileType type;  // normal, hardlink, softlink - done
} FoxArHeader;

// saving
// type, pathSize, path, if type link ( linkPathSize, linkPath ), mode, oid, gid, lastM, checksum, size
// validate checksum
// load content into specific file

typedef struct {
    FoxArHeader header;
    char *content;
} FoxArFile;

typedef struct {
    FILE *archive;
    FoxArFile *files;

    unsigned long fileCount;

} FoxArchive;

#endif