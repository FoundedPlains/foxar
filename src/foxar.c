#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/stat.h>

#include "foxar.h"


FoxArchive FoxArchive_New(char *path) {
    return fopen(path, "wb");
}

FoxArchive FoxArchive_Read(char *path) {
    return fopen(path, "rb");
}

int FoxArchive_Add(FoxArchive archive, char *path) {
    FILE *temp = fopen(path, "rb");
    
    struct stat info;
    struct stat info2;
    if (!temp) {
        return -1;
    }

    stat(path, &info);
    lstat(path, &info2);

    FoxArHeader arInfo;
    memset(&arInfo, 0, sizeof(FoxArHeader));

    arInfo.type = NORMAL;
    arInfo.linkPath = NULL;
    arInfo.linkPathSize = 0;
    arInfo.pathSize = strlen(path) + 1;
    arInfo.path = (char*) malloc(arInfo.pathSize);
    memcpy(arInfo.path, path, arInfo.pathSize);

    mode_t x = info.st_mode;
    int userModes = ((x & S_IRUSR) + (x & S_IWUSR) + (x & S_IXUSR)) / 64;
    int groupModes = ((x & S_IRGRP) + (x & S_IWGRP) + (x & S_IXGRP)) / 8;
    int otherModes = ((x & S_IROTH) + (x & S_IWOTH) + (x & S_IXOTH));
    int final = 100 * userModes + 10 * groupModes + otherModes;

    arInfo.mode = final;
    arInfo.oid = info.st_uid;
    arInfo.gid = info.st_gid;
    arInfo.lastM = info.st_mtime;
    arInfo.size = info.st_size;

    int offset = offsetof(FoxArHeader, pathSize);

    unsigned char *ptr = (unsigned char*) &arInfo;
    ptr += offset;
    unsigned long checksum = 0;
    arInfo.checksum = 0;
    for (size_t i = 0; i < sizeof(FoxArHeader) - offset; i++) {
        checksum += ptr[i];
    }
    for (int i = 0; i < arInfo.pathSize; i++) {
        checksum += arInfo.path[i];
    }
    arInfo.checksum = checksum;

    writeData(&arInfo.type, sizeof(FoxArFileType), archive);
    writeData(&arInfo.pathSize, sizeof(int), archive);
    writeData(arInfo.path, arInfo.pathSize, archive);
    writeData(&arInfo.mode, sizeof(int), archive);
    writeData(&arInfo.oid, sizeof(int), archive);
    writeData(&arInfo.gid, sizeof(int), archive);
    writeData(&arInfo.lastM, sizeof(unsigned long), archive);
    writeData(&arInfo.checksum, sizeof(unsigned long), archive);
    writeData(&arInfo.size, sizeof(unsigned long), archive);

    unsigned char byte = 0;

    while((byte = fgetc(temp)) != 255) {
        writeData(&byte, sizeof(unsigned char), archive);
    }

    return 0;
}

void writeData(void *data, size_t size, FILE* file) {
    fwrite(data, size, 1, file);
}

int FoxArchive_Next(FoxArchive archive, FoxArFile *file) {
    memset(&file->header, 0, sizeof(FoxArHeader));

    int *helper = (int *) malloc(sizeof(int));
    size_t *helper2 = (size_t *) malloc(sizeof(size_t));
    char *helper3;

    file->header.type = NORMAL;
    file->header.linkPath = NULL;
    file->header.linkPathSize = 0;

    readData(helper, sizeof(FoxArFileType), archive);
    if (feof(archive))
        return -3;
    file->header.type = *helper;
    readData(helper, sizeof(int), archive);
    file->header.pathSize = *helper;
    helper3 = (char *) malloc(*helper);

    if (!helper3) {
        return -1;
    }

    readData(helper3, file->header.pathSize, archive);
    file->header.path = (char *) malloc(*helper);
    memcpy(file->header.path, helper3, *helper);
    free(helper3);

    readData(helper, sizeof(int), archive);
    file->header.mode = *helper;

    readData(helper, sizeof(int), archive);
    file->header.oid = *helper;

    readData(helper, sizeof(int), archive);
    file->header.gid = *helper;

    readData(helper2, sizeof(unsigned long), archive);
    file->header.lastM = *helper2;

    readData(helper2, sizeof(unsigned long), archive);
    unsigned long tempCheckSum = *helper2;
    readData(helper2, sizeof(unsigned long), archive);
    file->header.size = *helper2;

    int offset = offsetof(FoxArHeader, pathSize);
    unsigned char *ptr = (unsigned char *) &file->header;
    ptr += offset;
    unsigned long checksum = 0;

    file->header.checksum = 0;

    for (size_t i = 0; i < sizeof(FoxArHeader) - offset; i++) {
        checksum += ptr[i];
    }
    for (int i = 0; i < file->header.pathSize; i++) {
        checksum += file->header.path[i];
    }

    file->header.checksum = tempCheckSum;

    assert(checksum == file->header.checksum);

    file->content = (char*) malloc(file->header.size + 1);
    char *cptr = file->content;
    for (size_t i = 0; i < file->header.size; i++) {
        *cptr = fgetc(archive);
        cptr++;
    }
    return 0;
}

void readData(void *data, size_t size, FILE* file) {
    fread(data, size, 1, file);
}

void FoxArchive_Save(FoxArchive archive) {
    fclose(archive);
}
