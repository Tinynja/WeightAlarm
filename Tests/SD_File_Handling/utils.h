#include <string.h>

#define dprint(var) Serial.print(#var ": ");\
                    Serial.println(var);

int endswith(const char *str, const char *suffix);