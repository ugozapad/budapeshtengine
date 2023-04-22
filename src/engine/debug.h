#ifndef DEBUG_H
#define DEBUG_H

void assertBackend(const char* msg, ...);

#define ENABLE_ASSERTS

#ifdef ENABLE_ASSERTS
#define  ASSERT(EXPR) do {																										\
							if (!(EXPR)) 																						\
							{ 																									\
								assertBackend("Assertion Failed: %s\nFile: %s\nLine: %d\nFunction: %s\n",						\
												#EXPR, __FILE__, __LINE__, __FUNCTION__ );										\
							}																									\
						 } while (0)
#else
#define ASSERT(EXPR) ((void)0)
#endif // ENABLE_ASSERTS


#endif // !DEBUG_H
