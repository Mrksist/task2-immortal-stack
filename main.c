#include <stdio.h>

#include "mystack.h"

int main () {
    stack_t st = {};

    StackCtor (&st, 2);

    StackPush (&st, 2);
    StackPush (&st, 2);
    StackPush (&st, 2);
    StackPush (&st, 2);
    StackPush (&st, 2);
    StackPush (&st, 2);

    stack_el_t x;

    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);
    StackPop (&x, &st);

    StackDtor (&st);
}