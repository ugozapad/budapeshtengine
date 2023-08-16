#ifndef LOGGER_H
#define LOGGER_H

void LogOpen(const char* filename);
void LogClose();

ENGINE_API void Msg(const char* msg, ...);

#endif