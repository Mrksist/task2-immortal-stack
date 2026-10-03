#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

#include "mystack.h"
#include "internal.h"

stack_el_t* GetFirstElemPtr (void* data) {
    return (stack_el_t*)((char*)data + st_signature_size + st_canary_size);
}

char* GetFirstCanaryPtr (void* data) {
    return ((char*)data + st_signature_size);
}

char* GetSecondCanaryPtr (void* data, size_t count) {
    return ((char*)data + st_signature_size + st_canary_size + st_elem_size * count);
}

size_t GetStackDataSize (size_t capacity) {
    return st_signature_size + st_canary_size * 2 + st_elem_size * capacity;
}

static size_t norm_to_2pow (size_t n) {
    return (1 << (sizeof (size_t) * 8 - (unsigned long long)__builtin_clzll(n)));
}

stack_err_t StackCtor (stack_t* st_ptr, size_t capacity) {
    stack_err_t res = STCK_OK;

    int pfd[2];
    int pipe_cr_res = pipe(pfd);
    if (pipe_cr_res < 0) {
        LogWrite (L_CRITICAL, "Cannot create pipe", __FUNCTION__, st_ptr, 0);
        return STCK_CANNOT_CREATE_PIPE;
    }
    ssize_t write_res = write (pfd[1], st_ptr, 1);
    if (write_res == -1 && errno == EFAULT) {
        LogWrite (L_CRITICAL, "Stack pointer given to checker is unaccessible", __FUNCTION__, st_ptr, 0);
        res |= STCK_UNACCESSIBLE;
        return res;
    }

    size_t capacity_norm = norm_to_2pow(capacity);

    if (capacity > ST_MAX_CAP) {
        res |= STCK_WRONG_CAPACITY;
        return res;
    }

    LogWrite (L_DEBUG, "Began initialization of stack with capacity of %d normalized to %d", __FUNCTION__, st_ptr, 2, capacity, capacity_norm);
    LogWrite (L_DEBUG, "Allocated %d byte(s)", __FUNCTION__, st_ptr, 1, GetStackDataSize (capacity_norm));

    stack_el_t* data = (stack_el_t*)calloc (GetStackDataSize (capacity_norm), 1);

    LogWrite (L_DEBUG, "Data pointer is 0x%llx", __FUNCTION__, st_ptr, 1, (unsigned long long)data);

    memcpy (data, &st_signature, st_signature_size);

    (*st_ptr).data = data;

    ONDEBUG ({
        stack_canary_t bird = GetCanary (0);
        memcpy (GetFirstCanaryPtr(data), &bird, st_canary_size);

        bird = GetCanary (1);
        memcpy (GetSecondCanaryPtr(data, 0), &bird, st_canary_size);
    });

    (*st_ptr).size = 0;
    (*st_ptr).capacity = capacity_norm;

    LogWrite (L_DEBUG, "Ended initialization of stack with capacity of %d normalized to %d", __FUNCTION__, st_ptr, 2, capacity, capacity_norm);

    if (res != STCK_OK) {
        LogWrite (L_CRITICAL, __FUNCTION__, "Error while initializing: %s", st_ptr, 1, GetErrorString (res));
    }

    ONDEBUG ({
        (*st_ptr)._data_hash = CalcHash ((unsigned char*)(*st_ptr).data, GetStackDataSize ((*st_ptr).capacity));
        (*st_ptr)._struct_hash = CalcHash ((unsigned char*)st_ptr, sizeof (stack_t) - sizeof (stack_hash_t));
    });

    return res;
}

stack_err_t StackPush (stack_t* st_ptr, stack_el_t elem) {
    stack_err_t res = StackCheck(st_ptr);

    if (res != STCK_OK) {
        return res;
    }

    stack_t st = *st_ptr;

    if (st.size == st.capacity) {
        (*st_ptr).data = (stack_el_t*)realloc (st.data, GetStackDataSize (st.capacity * 2));
        (*st_ptr).capacity *= 2;
        st = *st_ptr;
        LogWrite (L_DEBUG, "Stack capacity duplicated: now it's %d. Reallocated to %d bytes", __FUNCTION__, st_ptr, 1, st.capacity, GetStackDataSize (st.capacity));
    }

    ONDEBUG ({
        memmove (GetSecondCanaryPtr (st.data, st.size + 1), GetSecondCanaryPtr (st.data, st.size), st_canary_size);
    });

    stack_el_t* real_data = GetFirstElemPtr (st.data) + st.size;

    *real_data = elem;

    (*st_ptr).size++;

    ONDEBUG({
        (*st_ptr)._data_hash = CalcHash ((unsigned char*)(*st_ptr).data, GetStackDataSize ((*st_ptr).capacity));
        (*st_ptr)._struct_hash = CalcHash ((unsigned char*)st_ptr, sizeof (stack_t) - sizeof (stack_hash_t));
    });

    return res;
}

stack_err_t StackPop (stack_el_t* dst_elem, stack_t* st_ptr) {
    stack_err_t res = StackCheck(st_ptr);

    if (res) {
        return res;
    }

    stack_t st = *st_ptr;

    if (st.size == 0) {
        LogWrite (L_ERROR, "Pop an element from the stack unsucceed: size is 0", __FUNCTION__, st_ptr, 0);
        return STCK_EMPTY;
    }

    stack_el_t* elem_in_stack = GetFirstElemPtr (st.data) + st.size - 1;

    *dst_elem = *elem_in_stack;
    
    ONDEBUG({
        memmove (elem_in_stack, GetSecondCanaryPtr (st.data, st.size), st_canary_size);
    });

    (*st_ptr).size--;

    ONDEBUG({
        (*st_ptr)._data_hash = CalcHash ((unsigned char*)(*st_ptr).data, GetStackDataSize ((*st_ptr).capacity));
        (*st_ptr)._struct_hash = CalcHash ((unsigned char*)st_ptr, sizeof (stack_t) - sizeof (stack_hash_t));
    });

    LogWrite (L_DEBUG, "Pop an element from the stack succeed: %d. Current size is %d", __FUNCTION__, st_ptr, 2, *dst_elem, (*st_ptr).size);

    return STCK_OK;
}


stack_err_t StackDtor (stack_t* st_ptr) {
    stack_err_t res = StackCheck (st_ptr);

    if (res) {
        return res;
    }

    stack_t st = *st_ptr;

    const size_t data_size = GetStackDataSize (st.capacity) / 4;

    unsigned int* st_data_ptr = (unsigned int*)st.data;
    for (unsigned i = 0; i < data_size; i++) {
        st_data_ptr[i] = (unsigned int)rand();
    }

    free(st.data);

    return STCK_OK;
}