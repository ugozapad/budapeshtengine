#ifndef DEBUG_H
#define DEBUG_H

void ENGINE_API assertBackend(const char* expression, const char* file, int line, const char* function, bool* ignore, const char* fmt, ...);

#define ENABLE_ASSERTS

#ifdef ENABLE_ASSERTS
#define  ASSERT(EXPR) do {																										\
							if (!(EXPR)) 																						\
							{ 																									\
								static bool ignore = false;																		\
								assertBackend(#EXPR, __FILE__, __LINE__, __FUNCTION__, &ignore, NULL);							\
							}																									\
						 } while (0)

#define  ASSERT_MSG(EXPR, MSG, ...) do {																						\
							if (!(EXPR)) 																						\
							{ 																									\
								static bool ignore = false;																		\
								assertBackend(#EXPR, __FILE__, __LINE__, __FUNCTION__, &ignore, MSG, __VA_ARGS__);				\
							}																									\
						 } while (0)
#else
#define ASSERT(EXPR) ((void)0)
#define ASSERT_MSG(EXPR, MSG, ...) ((void)0)
#endif // ENABLE_ASSERTS

#define FATAL(MSG, ...) assertBackend("<no expression>", __FILE__, __LINE__, __FUNCTION__, NULL, MSG, __VA_ARGS__);


#endif // !DEBUG_H
