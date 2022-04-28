#include <string.h>
#include <SdFat.h>
#include <sdios.h>
#include "FileHandling.h"
#include "utils.h"

int countfilesext(char* path, char* extension) {
  FatFile dir, file;
  int count = 0;
  char filename[13];
  dir.open(path);
  while (file.openNext(&dir, O_RDONLY)) {
    file.getName(filename, sizeof(filename));
    if (file.isFile() && endswith(filename, extension))
      count++;
    file.close();
  }
  return count;
}

bool getnthfileext(char* out_filename, char* path, uint16_t n, char* extension) {
  FatFile dir, file;
  uint16_t count = -1;

  dir.open(path);
  while (file.openNext(&dir, O_RDONLY)) {
    file.getName(out_filename, 13);
    if (file.isFile() && endswith(out_filename, extension))
      count++;
    file.close();
    if (count == n)
      return 0;
  }
  return 1;
}