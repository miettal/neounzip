#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_INFO1 1
#define DEBUG_LEVEL_INFO2 2

#define DEBUG DEBUG_LEVEL_INFO2

#if DEBUG >= DEBUG_LEVEL_ERROR
#define error(...) printf("[ERROR]:"__VA_ARGS__)
#else
#define error(...)
#endif

#if DEBUG >= DEBUG_LEVEL_INFO1
#define info1(...) printf("[INFO1]:"__VA_ARGS__)
#else
#define info1(...)
#endif

#if DEBUG >= DEBUG_LEVEL_INFO2
#define info2(...) printf("[INFO2]:"__VA_ARGS__)
#else
#define info2(...)
#endif

#endif   /* DEBUG_H_INCLUDED */
