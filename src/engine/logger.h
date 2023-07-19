#ifndef LOGGER_H
#define LOGGER_H

void logOpen(const char* filename);
void logClose();

ENGINE_API void Msg(const char* msg, ...);

#endif