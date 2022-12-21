#include <stdio.h>

#include "src/foxar.h"

int main(int argc, char **argv) {
    FoxArchive archive;
    char mode = *argv[1];
    switch (mode) {
        case 'c':
            archive = FoxArchive_New(argv[2]);
            for (int i = 3; i < argc; i++) {
                FoxArchive_Add(archive, argv[i]);
            }
            FoxArchive_Save(archive);
            break;
        case 'r':
            FoxArFile file;
            archive = FoxArchive_Read(argv[2]);
            FoxArchive_Next(archive, &file);
            
            while (file.header.path) {
                printf("FoxArchive holds: %s\n", file.header.path);
                FoxArchive_Next(archive, &file);
            }
    }
    return 0;
}