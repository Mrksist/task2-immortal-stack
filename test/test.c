#include <assert.h>
#include <stdio.h>
#include <memory.h>

#include "internal.h"
#include "mystack.h"

void Test1NullPointer ();
void Test2NullData ();
void Test3CorruptedDataSignature ();
void Test4CorruptedDataCanaries ();
void Test5NegativeSize ();
void Test6NegativeCapacity ();
void Test7SizeIsGreaterThanCapacity ();
void Test8MoreCanaries ();
void Test9PopFromEmptyStack ();
void Test10Memset ();

int main () {
    Test1NullPointer ();
    Test2NullData ();
    Test3CorruptedDataSignature ();
    Test4CorruptedDataCanaries ();
    Test5NegativeSize ();
    Test6NegativeCapacity ();
    Test7SizeIsGreaterThanCapacity ();
    Test8MoreCanaries ();
    Test9PopFromEmptyStack ();
    Test10Memset ();
}

void Test1NullPointer () {
    printf ("============== 1 : Null Pointer ======================================================\n\n");
    
    unsigned res = StackPush (0, 0xBEDA);

    printf("Stack by address 0x0:\n");
    StackDump(0);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_UNACCESSIBLE;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, res, expected, GetErrorString(res), ESC_RESET);
    }
}

void Test2NullData () {
    printf ("============== 2 : Null Data =========================================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    st.data = 0; // Oops....

    printf("Constructed & Corrupted stack:\n");
    StackDump(&st);

    unsigned res = StackPush (&st, 0xEDA);
    
    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_DATA_UNACCESSIBLE;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test3CorruptedDataSignature () {
    printf ("============== 3 : Corrupted Data: Signature =========================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    assert (!StackPush (&st, 31));
    assert (!StackPush (&st, 32));
    assert (!StackPush (&st, 33));

    printf ("Constructed stack: \n");
    StackDump (&st);

    unsigned* sign_ptr = (unsigned*)st.data;
    *sign_ptr = 0xAAAADABE;

    printf ("Overwritten stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_INVALID_SIGNATURE | STCK_WRONG_DATA_HASH;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test4CorruptedDataCanaries () {
    printf ("============== 4 : Corrupted Data: Canaries ==========================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);
    
    assert (!StackPush (&st, 41));
    assert (!StackPush (&st, 42));
    assert (!StackPush (&st, 43));

    printf ("Constructed stack: \n");
    StackDump (&st);

    unsigned char* real_data_with_canaries_ptr = (unsigned char*)st.data + st_signature_size;
    for (unsigned char i = 0; i < 12; i++) {
        real_data_with_canaries_ptr[i] = i;
    }

    printf ("Corrupted stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_WRONG_FIRST_CANARY | STCK_WRONG_DATA_HASH;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test5NegativeSize () {
    printf ("============== 5 : Negative size =====================================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    st.size = (size_t)-1;

    printf ("Constructed&Corrupted stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_WRONG_SIZE | STCK_SIZE_GREATER_THAN_CAPACITY;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test6NegativeCapacity () {
    printf ("============== 6 : Negative capacity =================================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);
    
    assert (!StackPush (&st, 0x61));
    assert (!StackPush (&st, 0x62));
    assert (!StackPush (&st, 0x63));
    assert (!StackPush (&st, 0x64));
    assert (!StackPush (&st, 0x65));
    assert (!StackPush (&st, 0x66));

    st.capacity = (size_t)-1;

    printf ("Constructed&Corrupted stack: \n");
    StackDump (&st);
    
    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_WRONG_CAPACITY;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test7SizeIsGreaterThanCapacity () {
    printf ("============== 7 : Size is greater than capacity =====================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    StackPush (&st, 0x1);
    StackPush (&st, 0x2);
    StackPush (&st, 0x3);
    StackPush (&st, 0x4);
    StackPush (&st, 0x5);
    
    st.capacity = st.size - 1;

    printf ("Constructed&Corrupted stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_SIZE_GREATER_THAN_CAPACITY;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test8MoreCanaries () {
    printf ("============== 8 : More canaries =====================================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    assert (!StackPush (&st, 81));
    assert (!StackPush (&st, 82));
    assert (!StackPush (&st, 83));
    assert (!StackPush (&st, 84));
    assert (!StackPush (&st, 85));
    
    printf ("Constructed stack: \n");
    StackDump (&st);

    char* real_data_pointer = (char*)st.data + st_signature_size + st_canary_size;
    
    const char* superduper = "Super duper ultra important data that should be written to our stack ^-^";

    for (size_t i = 0; i < sizeof (stack_el_t) * 6; i++) {
        real_data_pointer[i] = superduper[i];
    }

    printf ("Overwritten stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_WRONG_SECOND_CANARY | STCK_WRONG_DATA_HASH;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test9PopFromEmptyStack () {
    printf ("============== 9 : Pop from empty stack ==============================================\n\n");

    stack_t st = {};

    StackCtor (&st, 12);

    assert (!StackPush (&st, 91));
    
    printf ("Constructed stack: \n");
    StackDump (&st);

    stack_el_t x = 0;

    assert (!StackPop (&x, &st));

    printf ("Empty stack: \n");
    StackDump (&st);

    unsigned res = StackPop (&x, &st);

    printf ("Stack after pop: \n");
    StackDump (&st);


    printf ("Pop errcode: 0x%x (%s), x = %d\n", res, GetErrorString(res), x);

    unsigned expected = STCK_EMPTY;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}

void Test10Memset () {
    printf ("============== 10 : Memset ===========================================================\n\n");

    stack_t st = {};

    StackCtor (&st, 10);

    assert (!StackPush (&st, 81));
    assert (!StackPush (&st, 82));
    assert (!StackPush (&st, 83));
    
    printf ("Constructed stack: \n");
    StackDump (&st);

    memset (&st, 0x67, sizeof (stack_t));

    printf ("Overwritten stack: \n");
    StackDump (&st);

    unsigned res = StackPush (&st, 0xEDA);

    printf ("Push errcode: 0x%x (%s)\n", res, GetErrorString(res));

    unsigned expected = STCK_DATA_UNACCESSIBLE;

    if (res == expected) {
        printf ("%sTest passed: 0x%x%s\n\n", ESC_COLOR_GREEN, res, ESC_RESET);
    }
    else {
        printf ("%sTest failed: errcode is 0x%x, expected one is 0x%x (%s). %s\n\n", ESC_COLOR_RED, expected, res, GetErrorString(expected), ESC_RESET);
    }
}