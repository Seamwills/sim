#include "library.h"

int
main(int argc, char *argv[])
{
    DIR     *dp;
    struct dirent   *dirp;
    list sen;

    if ((dp = opendir("markdown")) == NULL) {
        perror("open error");
        return 1;
    }

    /* render syntax, write to file and get title */
    sen = list_init();
    while ((dirp = readdir(dp)) != NULL) {
        if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
            continue;
        if (do_file(dirp->d_name, sen) != 0) {
            printf("hoedown error\n");
            return 1;
        }
    }

    /* sort and build index */
    build_index(sen);
    
    /* clean up */
    list_free(sen);

    return 0;
}
