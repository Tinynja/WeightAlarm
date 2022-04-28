#include <string.h>

#define dprint(var) Serial.print(#var ": ");\
                    Serial.println(var);

bool endswith(const char *str, const char *suffix);
