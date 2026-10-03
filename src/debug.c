#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#include "mystack.h"
#include "internal.h"

void $DebugBytesDump (const unsigned char* data, size_t nbytes) {
    for (unsigned i = 0; i < nbytes; i++) {
        printf(" %x", data[i]);
    }
    putchar('\n');
}

void StackDump (stack_t* st_ptr) {
    if (st_ptr == 0) {
        printf ("%sError: stack pointer is null 0_o%s\n", ESC_COLOR_RED, ESC_RESET);
        return;
    }

    stack_t st = *st_ptr;

    int pfd[2];
    pipe(pfd);
    ssize_t is_data_accessible = write (pfd[1], st.data, 1);

    unsigned char* signtr_dbg_ptr = (unsigned char*)(st.data);

    unsigned char* canary1_dbg_ptr = (unsigned char*)(st.data) + st_signature_size;

    unsigned char* canary2_dbg_ptr = (unsigned char*)(st.data) + st_signature_size + st_canary_size + st.size * st_elem_size;

    stack_el_t* real_data = (stack_el_t*)((char*)st.data + st_signature_size + st_canary_size);

    printf ("╔═════════════════════════════╗\n");
    printf ("║  %s%sstack_t%s at %s%-16p%s║\n", ESC_BOLD, ESC_COLOR_CYAN, ESC_RESET, ESC_COLOR_YELLOW, st_ptr, ESC_RESET);
    printf ("╚═════════════════════════════╝\n");
    printf (" - Size        : %s%zu%s\n", ESC_COLOR_BLUE, st.size, ESC_RESET);
    printf (" - Capacity    : %s%zu%s\n", ESC_COLOR_RED, st.capacity, ESC_RESET);
    printf (" - Data_ptr    : %s0x%llx%s\n", ESC_COLOR_YELLOW, (unsigned long long)st.data, ESC_RESET);


    printf (" - Data   : \n");

    if (is_data_accessible == -1) {
        printf ("%sERROR: data is not accessible%s\n\n", ESC_COLOR_RED, ESC_RESET);
        return;
    }
    

    printf ("╔═\n");
    printf ("║  ");

    printf ("     SIGN     |           BIRD1           | ELEM");

    if (st.size > ST_MAX_CAP)
        printf ("                     ");
    else
        for (unsigned i = 0; i < st.size; i++) {
            printf("            ");
        }
    printf(" |           BIRD2           |\n");

    
    printf ("║  ");

    printf (" %s%02X'%02X'%02X'%02X%s  | ", ESC_COLOR_GREEN, signtr_dbg_ptr[0], signtr_dbg_ptr[1], signtr_dbg_ptr[2], signtr_dbg_ptr[3], ESC_RESET);

    printf (" %s%02X'%02X'%02X'%02X'%02X'%02X'%02X'%02X%s  |", ESC_COLOR_BLUE, canary1_dbg_ptr[0], canary1_dbg_ptr[1], canary1_dbg_ptr[2], canary1_dbg_ptr[3], canary1_dbg_ptr[4], canary1_dbg_ptr[5], canary1_dbg_ptr[6], canary1_dbg_ptr[7], ESC_RESET);

    if (st.size > ST_MAX_CAP) {
        printf (" %sError: Size is wrong%s", ESC_COLOR_RED, ESC_RESET);
    }
    else {
        printf ("%s", ESC_BOLD);
        for(unsigned i = 0; i < st.size; i++) {
            stack_el_t entry = 0;
            memcpy (&entry, (real_data + i), st_elem_size);
            printf ("%12d", entry);
        }
        printf ("%s", ESC_RESET);
    }
    

    printf ("      |");

    printf ("  %s%02X'%02X'%02X'%02X'%02X'%02X'%02X'%02X%s  | ", ESC_COLOR_RED, canary2_dbg_ptr[0], canary2_dbg_ptr[1], canary2_dbg_ptr[2], canary2_dbg_ptr[3], canary2_dbg_ptr[4], canary2_dbg_ptr[5], canary2_dbg_ptr[6], canary2_dbg_ptr[7], ESC_RESET);

    printf ("\n");

    printf("╚═\n");

}