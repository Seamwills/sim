#include "library.h"

int
main(int argc, char *argv[])
{
    list sen;

    /* read source files and process them */
    sen = list_init();
    handle_source(sen);

    /* sort and build index */
    build_index(sen);
    
    /* clean up */
    list_free(sen);

    return 0;
}
