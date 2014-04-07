#include "document.h"
#include "html.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define IUNIT 1024
#define OUNIT 64
#define MAXLINE 4096

#define VACANT "无题"

typedef struct data {
    char    *date;
    char    *title;
} type;

typedef struct node {
    type *data;
    struct node *prev;
    struct node *next;
} node, *position, *list;

/* list function */
list
list_init();

void
list_add(list sen, type *data);

void
list_free(list sen);

/* quick sort */
void
quicksort(position p, position r);

position
partition(position p, position r);

void
exchange(position x, position y);

/* string function */
char *
str_rep(const char *line, const char *sub, const char *str);

/* date function */
int
date_cmp(const void *x, const void *y);

void
date_sort(list sen);

/* file process */
int
do_file(char *name, list sen);

hoedown_buffer *
hoedown(char *file);

char *
get_date(char *name);

char *
get_title(hoedown_buffer *buf);

void
build_index(list sen);
