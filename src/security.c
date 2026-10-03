#include <assert.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>

#include "mystack.h"
#include "internal.h"

#ifdef DEBUG
__attribute__((aligned(4096)))
static unsigned long long bird0 = 0;
static unsigned long long bird1 = 0;
static unsigned long long hashkey = 0;
#pragma GCC diagnostic ignored "-Wunused-variable"
static unsigned long long unwriteable[509] = {};
#pragma GCC diagnostic warning "-Wunused-variable"
#endif

__attribute__((constructor)) static void canary_generate () {
    ONDEBUG({
        // bird0 = 0xEE'AA'CE'0F'BA'5E'AC'DC;
        bird0 = 0xDC'AC'5E'BA'0F'CE'AA'EE;
        // bird1 = 0xCA'FE'C0'FF'EE'13'37'67;
        bird1 = 0x67'37'13'EE'FF'C0'FE'CE;

        hashkey = 0;

        int mprotect_r = mprotect(&bird0, 8, PROT_READ);
    
        assert (mprotect_r == 0);
    });
}

static void cat_err (char* dest, const char* message) {
    if (dest[0] != '\0') {
        strcat (dest, "|");
    }
    strcat (dest, message);
}

const char* GetErrorString (unsigned errcode) {
    if (errcode == STCK_OK) {
        return "STCK_OK";
    }

    char* message = (char*) calloc (MESSAGE_BUFFER_SIZE, 1);

    if (errcode & STCK_UNACCESSIBLE) {
        cat_err (message, "STCK_UNACCESSIBLE");
        errcode ^= STCK_UNACCESSIBLE;
    }

    if (errcode & STCK_DATA_UNACCESSIBLE) {
        cat_err (message, "STCK_DATA_UNACCESSIBLE");
        errcode ^= STCK_DATA_UNACCESSIBLE;
    }

    if (errcode & STCK_INVALID_SIGNATURE) {
        cat_err (message, "STCK_INVALID_SIGNATURE");
        errcode ^= STCK_INVALID_SIGNATURE;
    }

    if (errcode & STCK_WRONG_CAPACITY) {
        cat_err (message, "STCK_WRONG_CAPACITY");
        errcode ^= STCK_WRONG_CAPACITY;
    }

    if (errcode & STCK_WRONG_SIZE) {
        cat_err (message, "STCK_WRONG_SIZE");
        errcode ^= STCK_WRONG_SIZE;
    }

    if (errcode & STCK_SIZE_GREATER_THAN_CAPACITY) {
        cat_err (message, "STCK_SIZE_GREATER_THAN_CAPACITY");
        errcode ^= STCK_SIZE_GREATER_THAN_CAPACITY;
    }

    if (errcode & STCK_WRONG_FIRST_CANARY) {
        cat_err (message, "STCK_WRONG_FIRST_CANARY");
        errcode ^= STCK_WRONG_FIRST_CANARY;
    }

    if (errcode & STCK_WRONG_SECOND_CANARY) {
        cat_err (message, "STCK_WRONG_SECOND_CANARY");
        errcode ^= STCK_WRONG_SECOND_CANARY;
    }

    if (errcode & STCK_EMPTY) {
        cat_err (message, "STCK_EMPTY");
        errcode ^= STCK_EMPTY;
    }

    if (errcode & STCK_WRONG_DATA_HASH) {
        cat_err (message, "STCK_WRONG_DATA_HASH");
        errcode ^= STCK_WRONG_DATA_HASH;
    }

    if (errcode & STCK_WRONG_STRUCT_HASH) {
        cat_err (message, "STCK_WRONG_STRUCT_HASH");
        errcode  ^= STCK_WRONG_STRUCT_HASH;
    }

    if (errcode) {
        cat_err (message, "STCK_CODE_UNKNOWN");
    }

    return message;
}

#ifdef DEBUG
unsigned long long GetCanary (unsigned canary_no) {
    assert (canary_no <= 1);
    return (canary_no == 0 ? bird0 : bird1);
}

unsigned long long CalcHash(unsigned char *data, size_t size) {
    unsigned long long hash = 5381;
    int c = 0;
    for (unsigned i = 0; i < size; i++) {
        c = *(data++);
        hash = ((hash << 5) + hash) + (unsigned long long)c;
    }
    return hash ^ hashkey;
}
#endif

unsigned StackCheck (stack_t* st_ptr) {
    int pfd[2];
    int pipe_cr_res = pipe(pfd);

    if (pipe_cr_res < 0) {
        LogWrite (L_CRITICAL, "Cannot create pipe", __FUNCTION__, st_ptr, 0);
        return STCK_CANNOT_CREATE_PIPE;
    }

    ssize_t write_res = write (pfd[1], st_ptr, 1);

    if (write_res == -1 && errno == EFAULT) {
        LogWrite (L_CRITICAL, "Stack pointer given to checker is unaccessible", __FUNCTION__, st_ptr, 0);
        return STCK_UNACCESSIBLE;
    }

    assert (write_res != -1);

    stack_t st = *st_ptr;

    write_res = write (pfd[1], st.data, 1);

    if (write_res == -1 && errno == EFAULT) {
        LogWrite (L_CRITICAL, "Stack data pointer %llx given to checker is unaccessible", __FUNCTION__, st_ptr, 2, (unsigned long long)st.data);
        return STCK_DATA_UNACCESSIBLE;
    }

    assert (write_res != -1);

    stack_err_t res = STCK_OK;

    unsigned signature = *(unsigned*)st.data;

    if (signature != st_signature) {
        LogWrite (L_CRITICAL, "Stack signature 0x%x is invalid", __FUNCTION__, st_ptr, 1, signature);
        res |= STCK_INVALID_SIGNATURE;
    }

    if (st.capacity > ST_MAX_CAP) {
        LogWrite (L_CRITICAL, "Stack capacity %u is invalid. Possible overflow", __FUNCTION__, st_ptr, 1, st.capacity);
        res |= STCK_WRONG_CAPACITY;
    }

    if (st.size > ST_MAX_CAP) {
        LogWrite (L_CRITICAL, "Stack size %u is invalid. Possible overflow", __FUNCTION__, st_ptr, 1, st.size);
        res |= STCK_WRONG_SIZE;
    }

    if (st.capacity < st.size) {
        LogWrite (L_CRITICAL, " Stack capacity %u is lower than stack size %u", __FUNCTION__, st_ptr, 2, st.capacity, st.size);
        res |= STCK_SIZE_GREATER_THAN_CAPACITY;
    }

    if (res & STCK_WRONG_CAPACITY || res & STCK_WRONG_SIZE || res & STCK_SIZE_GREATER_THAN_CAPACITY) {
        return res;
    }

    ONDEBUG ({
        if (memcmp (&bird0, GetFirstCanaryPtr (st.data), st_canary_size)) {
            stack_canary_t my_canary_val = 0;
            memcpy (&my_canary_val, GetFirstCanaryPtr (st.data), st_canary_size);
            LogWrite (L_CRITICAL, " First stack canary %llx is broken. Correct one is %llx", __FUNCTION__, st_ptr, 2, my_canary_val, bird0);
            res |= STCK_WRONG_FIRST_CANARY;
        }

        if (memcmp (&bird1, GetSecondCanaryPtr (st.data, st.size), st_canary_size)) {
            stack_canary_t my_canary_val = 0;
            memcpy (&my_canary_val, GetSecondCanaryPtr (st.data, st.size), st_canary_size);
            LogWrite (L_CRITICAL, " Second stack canary %llx is broken. Correct one is %llx", __FUNCTION__, st_ptr, 2, my_canary_val, bird1);
            res |= STCK_WRONG_SECOND_CANARY;
        }
    });

    ONDEBUG ({
        stack_hash_t data_hash = CalcHash ((unsigned char*)(*st_ptr).data, GetStackDataSize ((*st_ptr).capacity));
        stack_hash_t struct_hash = CalcHash ((unsigned char*)st_ptr, sizeof (stack_t) - sizeof (stack_hash_t));

        if (data_hash != (*st_ptr)._data_hash) {
            LogWrite (L_CRITICAL, "Stack data hash %llx is wrong. Correct one is", __FUNCTION__, st_ptr, 2, (*st_ptr)._data_hash, data_hash);
            res |= STCK_WRONG_DATA_HASH;
        }

        if (struct_hash != (*st_ptr)._struct_hash) {
            LogWrite (L_CRITICAL, "Stack struct hash %llx is wrong. Correct one is", __FUNCTION__, st_ptr, 2, (*st_ptr)._struct_hash, struct_hash);
            res |= STCK_WRONG_STRUCT_HASH;
        }
    });

    close(pfd[0]);
    close(pfd[1]);

    return res;
}