#ifndef LOGGER_H
#define LOGGER_H

void logOpen(const char* filename);
void logClose();

void Msg(const char* msg, ...);

#endif