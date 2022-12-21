#ifndef FOXAR_H
#define FOXAR_H

#include <stdio.h>

typedef enum {
    NORMAL = 0,
    HARD_LINK,
    SOFT_LINK
} FoxArFileType;

typedef struct {
    char *path; // path 
    char *linkPath; // if link then path
    int pathSize; // size of path ( max 4096 bytes )
    int mode; // file mode - perms
    int oid; // owner id
    int gid; // group id
    unsigned long size; // size of file ( bytes )
    unsigned long lastM; // last modification UNIX time
    unsigned long checksum; // header checksum
    int linkPathSize; // size of path for link
    int type;  // normal, hardlink, softlink
} FoxArHeader;

// saving
// type, pathSize, path, if type link ( linkPathSize, linkPath ), mode, oid, gid, lastM, checksum, size
// validate checksum
// load content into specific file

typedef struct {
    FoxArHeader header;
    char *content;
} FoxArFile;

typedef FILE* FoxArchive;

FoxArchive FoxArchive_New(char *path);
FoxArchive FoxArchive_Read(char *path);

int FoxArchive_Add(FoxArchive archive, char *path);

int FoxArchive_Next(FoxArchive archive, FoxArFile *file);

void FoxArchive_Save(FoxArchive archive);

void writeData(void *data, size_t size, FILE* file);

void readData(void *data, size_t size, FILE* file);

#endif