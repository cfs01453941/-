#ifndef V_FILE_H
#define V_FILE_H
#include <stdio.h>
#include <string.h>

typedef struct _BlockofV{
    char block_type[15];
    char name[15];
    int pins[15];
    int num_of_pins;
    struct _BlockofV *next;
}BlockofV;

class V_file{
public:
    V_file();
    V_file(FILE*);
    void Print();
    BlockofV *v_head;
    BlockofV *v_cur;
    BlockofV *v_tail;
};
#endif
