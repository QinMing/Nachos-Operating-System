/*
 * snake.c -- draw a snake randomly moving around on the screen
 *
 * Geoff Voelker
 * 11/20/04
 */

#include "syscall.h"

char cmd[16];

/* #define WRITE(_cmd, _len) write(1, _cmd, _len) */
#define WRITE(_cmd, _len) Write(_cmd, _len, 1)

#define CMD3(_a, _b, _c) cmd[0] = 0x1B; cmd[1] = _a; cmd[2] = _b; cmd[3] = _c; cmd[4] = '\0'; WRITE(cmd, 5);
#define CMD7(_a, _b, _c, _d, _e, _f, _g) cmd[0] = 0x1B; cmd[1] = _a; cmd[2] = _b; cmd[3] = _c; cmd[4] = _d; cmd[5] = _e; cmd[6] = _f; cmd[7] = _g; cmd[8] = '\0'; WRITE(cmd, 9)

#define putch(_a) cmd[0] = _a; cmd[1] = '\0'; WRITE(cmd, 2)

#define MAX_ROWS 23
#define MAX_COLS 79

#define NUM_WORM_SEGS 4

#define DOWN  0
#define UP    1
#define LEFT  2
#define RIGHT 3

typedef struct worm_t {
  int head;
  int nsegs;
  int ticks;
  int dir;
  char row[NUM_WORM_SEGS];
  char col[NUM_WORM_SEGS];
} worm_t;

unsigned int seed;

void
irand_init()
{
  seed = 93186752;
}

/* a very poor random number generator */
unsigned int
irand(int l, int h)
{
  unsigned int a = 1588635695, m = 429496U, q = 2, r = 1117695901;
  unsigned int val;

  seed = a*(seed % q) - r*(seed / q);
  val = (seed / m) % (h - l) + l;
 
  return val;
}

#define clear_screen() CMD3('[', '2', 'J'); 

void
position(char row, char col)
{
  int rowh, rowl, colh, coll;

  /* split into two digits, convert to ascii digits */
  rowh = row / 10; rowl = row % 10; 
  rowh = '0' + rowh; rowl = '0' + rowl;
  colh = col / 10; coll = col % 10; 
  colh = '0' + colh; coll = '0' + coll;

  CMD7('[', rowh, rowl, ';', colh, coll, 'H');
}

void
worm_init(worm_t *worm)
{
  int i, j;

  worm->nsegs = NUM_WORM_SEGS;
  worm->head = worm->nsegs - 1;
  worm->ticks = irand(2, 6);
  worm->dir = RIGHT;
  for (i = 0; i < worm->nsegs; i++) {
    worm->row[i] = 1;
    worm->col[i] = i + 6;
  }
}

void
worm_draw(worm_t *worm)
{
  int w;

  for (w = 0; w < worm->nsegs; w++) {
    position(worm->row[w], worm->col[w]);
    putch('X');
  }
}

void
worm_erase(worm_t *worm)
{
  int w;

  for (w = 0; w < worm->nsegs; w++) {
    position(worm->row[w], worm->col[w]);
    putch(' ');
  }
}

void
worm_redirect(worm_t *worm)
{
  int head = worm->head;

  worm->ticks--;

  /* if we hit a wall, immediately change direction */
  if (worm->row[head] == 0 && worm->dir == UP)
    worm->ticks = 0;
  if (worm->row[head] == MAX_ROWS && worm->dir == DOWN)
    worm->ticks = 0;
  if (worm->col[head] == 0 && worm->dir == LEFT)
    worm->ticks = 0;
  if (worm->col[head] == MAX_COLS && worm->dir == RIGHT)
    worm->ticks = 0;

  if (worm->ticks <= 0) {
    /* choose a new direction */

    int newdir;

    while (1) {
      newdir = irand(0, 4);

      /* bounce off walls */
      if (worm->row[head] == 0 && newdir == UP)
	continue;
      if (worm->row[head] == MAX_ROWS && newdir == DOWN)
	continue;
      if (worm->col[head] == 0 && newdir == LEFT)
	continue;
      if (worm->col[head] == MAX_COLS && newdir == RIGHT)
	continue;

      /* do not go in reverse */
      if (worm->dir == UP && newdir == DOWN)
	continue;
      if (worm->dir == DOWN && newdir == UP)
	continue;
      if (worm->dir == LEFT && newdir == RIGHT)
	continue;
      if (worm->dir == RIGHT && newdir == LEFT)
	continue;

      break;
    }

    worm->dir = newdir;
    worm->ticks = irand(2, 6);
  }
}

void
worm_advance(worm_t *worm)
{
  int tail;

  /* clear tail */
  tail = (worm->head + 1) % worm->nsegs;

  worm_erase(worm);
  worm_redirect(worm);

  /* advance head (tail position is the new head position) */
  switch (worm->dir) {
  case UP:
    worm->row[tail] = worm->row[worm->head] - 1;
    worm->col[tail] = worm->col[worm->head];
    break;
  case DOWN:
    worm->row[tail] = worm->row[worm->head] + 1;
    worm->col[tail] = worm->col[worm->head];
    break;
  case LEFT:
    worm->row[tail] = worm->row[worm->head];
    worm->col[tail] = worm->col[worm->head] - 1;
    break;
  case RIGHT:
    worm->row[tail] = worm->row[worm->head];
    worm->col[tail] = worm->col[worm->head] + 1;
    break;
  default:
    break;
  }

  worm->head = tail;
  worm_draw(worm);
}

int
main (int argc, char *argv[])
{
  int i, j, k;
  worm_t worm_data, *worm = &worm_data ;

  irand_init();

  clear_screen();

  worm_init(worm);
  worm_draw(worm);

  while (1) {
    /* spin */
    for (j = 0; j < 15000; j++);
    /* next move */
    worm_advance(worm);
  }
}
