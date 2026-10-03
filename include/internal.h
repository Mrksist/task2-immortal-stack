#include <stdio.h>
#include "mystack.h"

#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define ONDEBUG(x) x
#else
#define ONDEBUG(x) ;
#endif

// *********************************************
//              Adress functions
// *********************************************

stack_el_t* GetFirstElemPtr (void* data);
char* GetFirstCanaryPtr (void* data);
char* GetSecondCanaryPtr (void* data, size_t size);
size_t GetStackDataSize (size_t capacity);


// *********************************************
//          Stack debugging functions
// *********************************************

void StackDump (stack_t* st);
void $DebugBytesDump (const unsigned char* data, size_t nbytes);

// *********************************************
//               Mitigation functions
// *********************************************

const char* GetErrorString(unsigned errcode);
stack_err_t StackCheck (stack_t* st_ptr);
stack_canary_t GetCanary (unsigned canary_no);

// *********************************************
//                    Hashing
// *********************************************

stack_hash_t CalcHash(unsigned char *data, size_t size);

// *********************************************
//                     Logger
// *********************************************

void LogWrite (int level, const char* format, const char* function, stack_t* stack, int nargs, ...);

// *******************************
//           Log levels
// *******************************

#define L_DEBUG 0
#define L_WARNING 1
#define L_ERROR 2
#define L_CRITICAL 3

// *********************************************
//      UNICODE ESCAPE FORMATTING DEFINES
// *********************************************

#define ESC_RESET "\e[0m"

#define ESC_BOLD "\e[1m"
#define ESC_UNDERLINE "\e[4m"

#define ESC_COLOR_RED         "\e[31;49m"
#define ESC_COLOR_GREEN       "\e[32;49m"
#define ESC_COLOR_YELLOW      "\e[33;49m"
#define ESC_COLOR_BLUE        "\e[34;49m"
#define ESC_COLOR_MAGENTA     "\e[35;49m"
#define ESC_COLOR_CYAN        "\e[36;49m"
#define ESC_COLOR_WHITE       "\e[37;49m"
#define ESC_COLOR_GREY        "\e[90;49m"


#define NUMBER_OUTPUT_FORMAT "%-4d"

// *********************************************
//      SOME INTERESTING DEBUGGING DEFINES
// *********************************************

#define $D(x)                                                                  \
  printf("(" #x " : int) = %d\n", x);                                          \
  fflush(stdout)
#define $U(x)                                                                  \
  printf("(" #x " : uint) = %d\n", x);                                         \
  fflush(stdout)
#define $LD(x)                                                                 \
  printf("(" #x " : long) = %ld\n", x);                                        \
  fflush(stdout)
#define $ULD(x)                                                                \
  printf("(" #x " : ulong) = %uld\n", x);                                      \
  fflush(stdout)

#define $C(x)                                                                  \
  printf("(" #x " : char) = <%c> (%d)\n", x, x);                               \
  fflush(stdout)
#define $X(x)                                                                  \
  printf("(" #x " : hex) = 0x%X\n", x);                                        \
  fflush(stdout)
#define $S(x)                                                                  \
  printf("(" #x " : str) = <%s>\n", x);                                        \
  fflush(stdout)
#define $SD(x)                                                                 \
  printf("(" #x " :str = < ");                                                 \
  int $DEBUG##x = 0;                                                           \
  while (x[$DEBUG##x] != 0) {                                                  \
    printf("0x%X ", (unsigned char)x[$DEBUG##x]);                              \
    $DEBUG##x++;                                                               \
  }                                                                            \
  printf(">\n");                                                               \
  fflush(stdout)

#define $MD(x)                                                                 \
  printf("%s:%d: (" #x " : int) = %d\n", __FILE__, __LINE__, x);               \
  fflush(stdout)
#define $MU(x)                                                                 \
  printf("%s:%d: (" #x " : uint) = %d\n", __FILE__, __LINE__, x);              \
  fflush(stdout)
#define $MLD(x)                                                                \
  printf("%s:%d: (" #x " : long) = %ld\n", __FILE__, __LINE__, x);             \
  fflush(stdout)
#define $MULD(x)                                                               \
  printf("%s:%d: (" #x " : ulong) = %uld\n", __FILE__, __LINE__, x);           \
  fflush(stdout)

#define $MC(x)                                                                 \
  printf("%s:%d: (" #x " : char) = <%c> (%d)\n", __FILE__, __LINE__, x, x);    \
  fflush(stdout)
#define $MX(x)                                                                 \
  printf("%s:%d: (" #x " : hex) = 0x%X\n", __FILE__, __LINE__, x);             \
  fflush(stdout)
#define $MS(x)                                                                 \
  printf("%s:%d: (" #x " : str) = <%s>\n", __FILE__, __LINE__, x);             \
  fflush(stdout)

#define $MSD(x)                                                                \
  {                                                                            \
    printf("%s:%d: (" #x " : str) = < ", __FILE__, __LINE__);                  \
    int $DEBUG_x = -1;                                                         \
    do {                                                                       \
      $DEBUG_x++;                                                              \
      printf("0x%X ", (unsigned char)tolower(x[$DEBUG_x]));                    \
    }  while (x[$DEBUG_x] != '\0');                                            \
    printf(">\n");                                                             \
    fflush(stdout);                                                            \
  } 

#define $MEOW()                                                                \
  printf("%s:%d\n", __FILE__, __LINE__);                                       \
  fflush(stdout)


// *********************************************
//        DED32'S EXTRA COMPILER WARNINGS
// *********************************************

#pragma GCC diagnostic ignored "-Wpragmas"

#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Weffc++"
#pragma GCC diagnostic warning "-Wextra"

#pragma GCC diagnostic warning "-Waggressive-loop-optimizations"
#pragma GCC diagnostic warning "-Walloc-zero"
#pragma GCC diagnostic warning "-Walloca"
#pragma GCC diagnostic warning "-Walloca-larger-than=8192"
#pragma GCC diagnostic warning "-Warray-bounds"
#pragma GCC diagnostic warning "-Wcast-align"
#pragma GCC diagnostic warning "-Wcast-qual"
#pragma GCC diagnostic warning "-Wchar-subscripts"
#pragma GCC diagnostic warning "-Wconditionally-supported"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wctor-dtor-privacy"
#pragma GCC diagnostic warning "-Wdangling-else"
#pragma GCC diagnostic warning "-Wduplicated-branches"
#pragma GCC diagnostic warning "-Wempty-body"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wformat-nonliteral"
#pragma GCC diagnostic warning "-Wformat-overflow=2"
#pragma GCC diagnostic warning "-Wformat-security"
#pragma GCC diagnostic warning "-Wformat-signedness"
#pragma GCC diagnostic warning "-Wformat-truncation=2"
#pragma GCC diagnostic warning "-Wformat=2"
#pragma GCC diagnostic warning "-Wlarger-than=8192"
#pragma GCC diagnostic warning "-Wlogical-op"
#pragma GCC diagnostic warning "-Wnarrowing"
#pragma GCC diagnostic warning "-Wnon-virtual-dtor"
#pragma GCC diagnostic warning "-Wnonnull"
#pragma GCC diagnostic warning "-Wopenmp-simd"
#pragma GCC diagnostic warning "-Woverloaded-virtual"
#pragma GCC diagnostic warning "-Wpacked"
#pragma GCC diagnostic warning "-Wpointer-arith"
#pragma GCC diagnostic warning "-Wredundant-decls"
#pragma GCC diagnostic warning "-Wrestrict"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wsign-promo"
#pragma GCC diagnostic warning "-Wstack-usage=8192"
#pragma GCC diagnostic warning "-Wstrict-aliasing"
#pragma GCC diagnostic warning "-Wstrict-null-sentinel"
#pragma GCC diagnostic warning "-Wstrict-overflow=2"
#pragma GCC diagnostic warning "-Wstringop-overflow=4"
#pragma GCC diagnostic warning "-Wsuggest-attribute=noreturn"
#pragma GCC diagnostic warning "-Wsuggest-final-methods"
#pragma GCC diagnostic warning "-Wsuggest-final-types"
#pragma GCC diagnostic warning "-Wsuggest-override"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wsync-nand"
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wunused"
#pragma GCC diagnostic warning "-Wvarargs"
#pragma GCC diagnostic warning "-Wvariadic-macros"
#pragma GCC diagnostic warning "-Wvla-larger-than=8192"

#endif // DEBUG_H