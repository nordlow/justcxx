#include "../kdTree.h"
#include "../timing.h"
#include "../ptrarray_grid.h"
#include "../stdio_x.h"

/* ========================================================================= */

const int show_bars = FALSE;
const double fac = 200;

void
test_kdTree_ins_shape(kdTree * tree,
                      vis_t ** grid, size_t m, size_t n, int show)
{
  size_t k;
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  ptimer_tic(&tmr);

  for (k = 0; k < m * n; k++) {
    vis_t * elm = grid[k];

    if (show) {
      printf("inserting ");
      vis_fprint(stdout, elm);
      printf(" nr:%zd/%zd...", k, m * n);
    }

    kdTree_ins_shape(tree, elm);

    if (show) {
      kdTree_fprint(stdout, tree, TRUE);
      putsep(stdout);
    }

    if (show) {
      endline();
    }
  }

  ptimer_toc(&tmr);

  printf("- Insertion: "); ptimer_print_sec_usec9(tmr); printf("[s]");

  if (show_bars) {
    int x = (int)rint(ptimer_get_sec(&tmr) * fac);
    putchar(' ');
    mputchar(stdout, x, '#');
  }

  endline();
}

void
test_kdTree_find_shape(kdTree * tree,
                       vis_t ** grid, size_t m, size_t n, int show)
{
  size_t k;
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  ptimer_tic(&tmr);

  kdTree_reset_fbs(tree);

  size_t hitnum = 0;
  for (k = 0; k < m * n; k++) {
    const vis_t * elm = grid[k];

    if (show) {
      printf("finding ");
      vis_fprint(stdout, elm);
      printf(" nr:%zd/%zd...", k, m * n);
    }

    int hit = kdTree_find_shape(tree, elm);
    if (hit) { hitnum++; }

    if (show) {
      printf("%s", hit ? "HIT: " : "MISS");
      endline();
    }
  }

  ptimer_toc(&tmr);

  printf("- Find: "); ptimer_print_sec_usec9(tmr); printf("[s]");
  printf(" (%zd) %s ", hitnum, (hitnum == m * n) ? "SUCCESS" : "FAILURE");

  if (show_bars) {
    int x = (int)rint(ptimer_get_sec(&tmr) * fac);
    mputchar(stdout, x, '#');
  }

  endline();
}

void
test_kdTree_rbal(kdTree * tree, int show)
{
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  ptimer_tic(&tmr);
  kdTree_rbal(tree);
  ptimer_toc(&tmr);

  printf("- Balance: "); ptimer_print_sec_usec9(tmr); printf("[s]");

  if (show_bars) {
    int x = (int)rint(ptimer_get_sec(&tmr) * fac);
    mputchar(stdout, x, '#');
  }

  endline();
}

void
test_kdTree_rm_shape(kdTree * tree,
                     vis_t ** grid, size_t m, size_t n, int show)
{
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  ptimer_tic(&tmr);

  size_t k, rmnum = 0;
  for (k = 0; k < m * n; k++) {
    vis_t * elm = grid[k];

    if (show) {
      printf("removing ");
      vis_fprint(stdout, elm);
      printf(" nr:%zd/%zd...", k, m * n);
    }

    uint ok = kdTree_rm_shape(tree, elm);

    if (ok) {
      rmnum++;
    }

    if (show) {
      printf("%s\n", ok ? "HIT" : "MISS");
    }
  }

  ptimer_toc(&tmr);

  printf("- Remove: "); ptimer_print_sec_usec9(tmr); printf("[s]");
  printf(" (%zd) %s ", rmnum, (rmnum == m * n) ? "SUCCESS" : "FAILURE");

  if (show_bars) {
    int x = (int)rint(ptimer_get_sec(&tmr) * fac);
    mputchar(stdout, x, '#');
  }

  endline();
}

void
test_kdTree_major(float step, float dim,
                  size_t m,
                  size_t n,
                  int random,
                  int shuffle, int show, int end_show)
{
  size_t k, l;

  /* we need a timer for benchmarkings */
  pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID);

  vis_t ** grid = ptrarrayvis_newGrid(step, dim, m, n, random);

  /* shuffle the boxes around */
  if (shuffle) {
    ptimer_tic(&tmr);
    ptrarray_shuffle((void**)grid, m * n);
    ptimer_toc(&tmr);
    printf("- Shuffle: "); ptimer_print_sec_usec9(tmr);  printf("[s]"); endline();
  }

  /* create the tree */
  kdTree * tree;
  tree = kdTree_new();

  /* standard search  */
  ptimer_tic(&tmr);
  for (k = 0; k < m * n; k++) {
    for (l = 0; l < n * n; l++) {
      if (grid[k] == grid[l]) {
        break;
      }
    }
  }
  ptimer_toc(&tmr);

  printf("- Scan: "); ptimer_print_sec_usec9(tmr); printf("[s]"); endline();

  /* insert them into tree */
  test_kdTree_ins_shape(tree, grid, m, n, show);

  /* search them in the tree */
  test_kdTree_find_shape(tree, grid, m, n, show);

  /* print it */
  printf("- Printing: "); kdTree_fprint(stdout, tree, end_show);

  /* recursively balance tree */
  test_kdTree_rbal(tree, show);

  /* search them again in the balanced tree */
  test_kdTree_find_shape(tree, grid, m, n, show);

  /* print it */
  printf("- Printing: "); kdTree_fprint(stdout, tree, end_show);

  /* remove them from the tree */
  test_kdTree_rm_shape(tree, grid, m, n, show);

  /* print it */
  printf("- Printing: "); kdTree_fprint(stdout, tree, end_show);

  /* remove boxes */
  ptimer_tic(&tmr);
  ptrarrayvis_clrGrid(grid, m, n);
  ptimer_toc(&tmr);
  printf("- Remove (grid): "); ptimer_print_sec_usec9(tmr);  printf("[s]"); endline();

  ptimer_tic(&tmr);
  kdTree_delete(tree, FALSE);
  ptimer_toc(&tmr);
  printf("- kdTree delete: "); ptimer_print_sec_usec9(tmr);  printf("[s]"); endline();

  endline();
}

int
main(int argc, char *argv[])
{
  {
    size_t i, m = 4, n = 4;
    int random = FALSE, shuffle = TRUE;

    printf("m: ");
    if (scanf("%zd", &m) != 1) {
      eprintf("could not scan m");
      return -1;
    }

    printf("n: ");
    if (scanf("%zd", &n) != 1) {
      eprintf("could not scan n");
      return -1;
    }

    printf("shuffle: ");
    if (scanf("%d", &shuffle) != 1) {
      eprintf("could not scan shuffle");
      return -1;
    }

    float step = 2.0, dim = 1.0;

    printf("step: ");
    if (scanf("%f", &step) != 1) {
      eprintf("could not scan step");
      return -1;
    }

    printf("dim: ");
    if (scanf("%f", &dim) != 1) {
      eprintf("could not scan dim");
      return -1;
    }

    int part_show = FALSE;

    printf("part_show: ");
    if (scanf("%d", &part_show) != 1) {
      eprintf("could not scan part_show");
      return -1;
    }

    int end_show = FALSE;

    printf("end_show: ");
    if (scanf("%d", &end_show) != 1) {
      eprintf("could not scan end_show");
      return -1;
    }

    size_t niters = 1;

    printf("niters: ");
    if (scanf("%zd", &niters) != 1) {
      eprintf("could not scan niters");
      return -1;
    }

    for (i = 0; i < niters; i++) {
      test_kdTree_major(step, dim, m, n,
                        random,
                        shuffle, part_show, end_show);
    }

  }

  return 0;

}
