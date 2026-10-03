#include <stdlib.h>
#include <stdint.h>

#ifndef STACK_H
#define STACK_H

// *******************************
//   Type && struct definitions
// *******************************

typedef int stack_el_t;
typedef uint32_t stack_err_t;
typedef unsigned stack_signature_t;
typedef unsigned long long stack_canary_t;
typedef unsigned long long stack_hash_t;

typedef struct {
    stack_el_t* data;
    size_t size;
    size_t capacity;
#ifdef DEBUG
    stack_hash_t _data_hash;
    stack_hash_t _struct_hash;
#endif
} stack_t;

// *******************************
//          Error codes
// *******************************

#define STCK_OK 0b0

#define STCK_CANNOT_CREATE_PIPE *(const unsigned*)"PIPETS"

#define STCK_UNACCESSIBLE               (1 << 0)
#define STCK_DATA_UNACCESSIBLE          (1 << 1)
#define STCK_INVALID_SIGNATURE          (1 << 2)
#define STCK_WRONG_CAPACITY             (1 << 3)
#define STCK_WRONG_SIZE                 (1 << 4)
#define STCK_SIZE_GREATER_THAN_CAPACITY (1 << 5)
#define STCK_WRONG_FIRST_CANARY         (1 << 6)
#define STCK_WRONG_SECOND_CANARY        (1 << 7)
#define STCK_EMPTY                      (1 << 8)
#define STCK_WRONG_DATA_HASH            (1 << 9)
#define STCK_WRONG_STRUCT_HASH          (1 << 10)

// *******************************
//        Common constants
// *******************************

#ifdef DEBUG
#define ST_MODE "DEBUG_MODE"
#else
#define ST_MODE "Release mode"
#endif

#define ST_MAX_CAP (1 << 24)

#ifdef DEBUG
#define ST_LOG_LEVEL 0
#else
#define ST_LOG_LEVEL 1
#endif

#define TMP_BUFFER_SIZE 32
#define MESSAGE_BUFFER_SIZE 256

const unsigned st_signature = 0x4b435453; // "STCK"

const size_t st_signature_size = sizeof (stack_signature_t);

#ifdef DEBUG
const size_t st_canary_size = sizeof (stack_canary_t);
#else
const size_t st_canary_size = 0;
#endif

const size_t st_elem_size = sizeof (stack_el_t);

// *******************************
//         Stack functions
// *******************************

stack_err_t StackCtor (stack_t* st_ptr, size_t capacity);

stack_err_t StackPush (stack_t* st_ptr, stack_el_t elem);
stack_err_t StackPop (stack_el_t* dst_elem, stack_t* st_ptr);

stack_err_t StackDtor (stack_t* st_ptr);

const char* GetErrorString(stack_err_t errcode);

#endif // STACK_H