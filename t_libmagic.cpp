#include <stdio.h>
#include <stdlib.h>

#include <magic.h>

void
test_libmagic(const char *filename)
{
    auto cookie = magic_open(MAGIC_NONE);
    if (not cookie) {
        fprintf(stderr, "could not open magic cookie\n");
        return;
    }

    if (magic_load(cookie, NULL) != 0) {
        fprintf(stderr, "could not load magic database\n");
        return;
    }

    const char *cstr = NULL;
    cstr = magic_file(cookie, filename);
    printf("%s is of type %s\n", filename, cstr);
    magic_close(cookie);
}

int
main(int argc, char **argv)
{
    if (argc != 1 + 1) {
        printf("usage: %s file\n", argv[0]);
        return -1;
    }
    test_libmagic(argv[1]);
    return 0;
}
