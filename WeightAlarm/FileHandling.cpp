#include "FileHandling.h"
#include <SdFat.h>
#include <string.h>

void getrandomfile(char* out_filefqn, char* out_filename, char* path, char* extension) {
  srand(TCNT2);
  getnthfilewithext(out_filename, path, rand()%countfileswithext(path, extension), extension);
  sprintf(out_filefqn, "%s/%s", path, out_filename);
}

unsigned int countfileswithext(char* path, char* extension) {
  FatFile dir, file;
  uint16_t count = 0;
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

bool getnthfilewithext(char* out_filename, char* path, unsigned int n, char* extension) {
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

bool endswith(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
