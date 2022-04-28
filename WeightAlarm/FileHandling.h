void getrandomfile(char* out_filefqn, char* out_filename, char* path, char* extension);

unsigned int countfileswithext(char* path, char* extension);

bool getnthfilewithext(char* out_filename, char* path, unsigned int n, char* extension);

bool endswith(const char *str, const char *suffix);
