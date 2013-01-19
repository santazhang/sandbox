#include <stdio.h>
#include <puzzle.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <image_fpath>\n", argv[0]);
        return 1;
    }

    char* img_fpath = argv[1];

    PuzzleContext context;
    PuzzleCvec cvec;

    puzzle_init_context(&context);
    puzzle_init_cvec(&context, &cvec);

    if (puzzle_fill_cvec_from_file(&context, &cvec, img_fpath) != 0) {
        fprintf(stderr, "Unable to read [%s]\n", img_fpath);
        return 1;
    }

    int i;
    for (i = 0; i < cvec.sizeof_vec; i++) {
        printf("%d ", cvec.vec[i]);
    }
    printf("\n");

    puzzle_free_cvec(&context, &cvec);
    puzzle_free_context(&context);

    return 0;
}

