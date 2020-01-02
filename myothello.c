#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "view_othello.h"  // ����ط��Υإå���

/*
  ����ѥ���:
  gcc -I/usr/X11R6/include -Wall   -c -o myothello.o myothello.c
  gcc -I/usr/X11R6/include -Wall   -c -o view_othello.o view_othello.c
  gcc -I/usr/X11R6/include -Wall -L/usr/X11R6/lib -lX11  -o myothello
  myothello.o view_othello.o
*/

/****
view_othello.h ����ǰʲ��Τ褦���������Ƥ��롥

#define EMPTY (-1)
#define WHITE (0)
#define BLACK (1)

struct othello{// �������ѹ�¤��
        int n; // �ܡ��ɤΥ�����
        int score[2]; // ��������Ͽ������(ɬ�פ˱���������)
        int pass_flag[2]; // �ѥ��������ɤ�����Ͽ��������(ɬ�פ˱���������)
        int **board; // ������Υܡ���
};

����ϡ�������Υܡ��ɤ��󼡸�����(board)��ɽ�����롥
�㤨�С�(1,5) �Υޥ������Ф��֤���������
board[0][4] = WHITE �Ȥ�����ɤ���
******/

void saveBoard(int pos, struct othello *o) {
  int ix, iy;
  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      o->board_log[pos][ix][iy] = o->board[ix][iy];
    }
  }
}

void restoreBoard(int pos, struct othello *o) {
  int ix, iy;
  pos = pos < 0 ? 0 : pos;
  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      o->board[ix][iy] = o->board_log[pos][ix][iy];
    }
  }
}
void countStone(struct othello *o) {
  int ix = 0, iy = 0;
  int black = 0, white = 0;
  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      if (o->board[ix][iy] == WHITE) white += 1;
      if (o->board[ix][iy] == BLACK) black += 1;
    }
  }
  o->score[0] = black;
  o->score[1] = white;
}

void paintStone(int color, int startX, int startY, int endX, int endY,
                int **board) {
  if (startX == endX && startY == endY) return;
  int dx = endX - startX == 0 ? 0 : (endX - startX) / abs(endX - startX);
  int dy = endY - startY == 0 ? 0 : (endY - startY) / abs(endY - startY);
  board[startX + dx][startY + dy] = color;
  paintStone(color, startX + dx, startY + dy, endX, endY, board);
}

bool checkPos(int color, int vector, int startX, int startY, int *endX,
              int *endY, struct othello *o, int count) {
  int dx = 0, dy = 0;
  switch (vector) {
    case 0:
      dx = -1, dy = 0;
      break;
    case 1:
      dx = -1, dy = 1;
      break;
    case 2:
      dx = 0, dy = 1;
      break;
    case 3:
      dx = 1, dy = 1;
      break;
    case 4:
      dx = 1, dy = 0;
      break;
    case 5:
      dx = 1, dy = -1;
      break;
    case 6:
      dx = 0, dy = -1;
      break;
    case 7:
      dx = -1, dy = -1;
      break;
    default:
      printf("vector error.\n");
      return false;
      break;
  }
  int x = startX + dx;
  int y = startY + dy;
  if (x < 0 || y < 0 || x >= o->n || y >= o->n) return false;
  if (o->board[x][y] == -1) return false;
  if (o->board[x][y] == !color) {
    if (!checkPos(color, vector, x, y, endX, endY, o, count + 1)) return false;
  }
  if (o->board[x][y] == color) {
    if (count == 0) return false;
    *endX = x;
    *endY = y;
  }
  return true;
}

void algo_4619023(int color, bool **puttablePos, struct othello *o) {
  int count = 0, ix = 0, iy = 0;
  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      if (puttablePos[ix][iy]) count += 1;
    }
  }
  srand((unsigned int)time(NULL));
  int posCount = rand() % count;
  count = 0;
  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      if (puttablePos[ix][iy]) {
        if (posCount == count) {
          int i = 0;
          for (i = 0; i < 8; i++) {
            int endX = -1, endY = -1;
            if (checkPos(color, i, ix, iy, &endX, &endY, o, 0)) {
              paintStone(color, ix, iy, endX, endY, o->board);
            }
          }
          o->board[ix][iy] = color;
        }
        count += 1;
      }
    }
  }
}

// ���δؿ����¤���Ƥ���������
void myothello(int *step, /* ���ߤΥ��ƥå׿�(����ͤ�0) */
               struct othello *o /* ������Υܡ��ɤʤɤξ����ޤ๽¤�� */
) {
  int x, y;  /* �ɤ��Υޥ����֤��Τ���Ͽ�����ѿ�
        �������� x���������� y �Ȥ��� */
  int color; /* �����򡤤ɤ���Υ����󤫵�Ͽ�����ѿ� */

  if (*step % 2 == 0) { /* �������ƥåפΤȤ��Ϲ����� */
    color = BLACK;
    printf("Black's turn\n");
  } else { /* ������ƥåפΤȤ�������� */
    color = WHITE;
    printf("White's turn\n");
  }

  bool hasUserPutPlace = false, hasEnemyPutPlace = false;
  int ix = 0, iy = 0, i = 0, j = 0;
  bool **puttablePos;
  puttablePos = malloc(sizeof(bool *) * o->n);
  for (i = 0; i < o->n; i++) {
    puttablePos[i] = malloc(sizeof(bool) * o->n);
    for (j = 0; j < o->n; j++) {
      puttablePos[i][j] = false;
    }
  }

  for (ix = 0; ix < o->n; ix++) {
    for (iy = 0; iy < o->n; iy++) {
      for (i = 0; i < 8; i++) {
        if (o->board[ix][iy] == -1) {
          int endX = -1, endY = -1;
          if (checkPos(color, i, ix, iy, &endX, &endY, o, 0)) {
            hasUserPutPlace = true;
            puttablePos[ix][iy] = true;
          }
          if (checkPos(!color, i, ix, iy, &endX, &endY, o, 0))
            hasEnemyPutPlace = true;
        }
      }
    }
  }

  if (!hasEnemyPutPlace && !hasUserPutPlace) {
    printf("Game Over.\n");
    printf("Black:%d\nWhite:%d\n", o->score[0], o->score[1]);
    if (o->score[1] > o->score[0]) printf("Winner is White!!\n");
    if (o->score[1] == o->score[0]) printf("Draw!!\n");
    if (o->score[1] < o->score[0]) printf("Winner is Black!!\n");
    return;
  }
  if (!hasUserPutPlace) {
    printf("You can not put anywhere.\nPass\n");
    return;
  }

  if (color == !o->player_color) {
    algo_4619023(color, puttablePos, o);
    for (i = 0; i < o->n; i++) {
      free(puttablePos[i]);
    }
    free(puttablePos);
    countStone(o);
    saveBoard(*step + 1, o);
  }

  if (color == o->player_color) {
    printf("Input cell (x y)\nIf you want to back,Input (0 m).\n");
    scanf("%d %d", &x, &y);
    if (x == 0) {
      restoreBoard(*step - y, o);
      *step -= (y + 1);
      *step = *step < -1 ? -1 : *step;
      return;
    }
    /* �����ֹ椬0�֤���Ϥޤ뤿�ᡥ */
    x--;
    y--;

    if (x < 0 || y < 0 || x >= o->n || y >= o->n) {
      printf("It is not valid input.\nPlease input again.\n");
      *step -= 1;
      return;
    }
    if (o->board[x][y] != -1) {
      printf("There has already been stone.\nPlease input again.\n");
      *step -= 1;
      return;
    }

    bool IsPuttable = false;
    for (i = 0; i < 8; i++) {
      int endX = -1, endY = -1;
      if (checkPos(color, i, x, y, &endX, &endY, o, 0)) {
        paintStone(color, x, y, endX, endY, o->board);
        IsPuttable = true;
      }
    }
    if (IsPuttable) {
      o->board[x][y] = color; /* ������ޥ����Ф��֤� */
      countStone(o);
      saveBoard(*step + 1, o);
    } else {
      printf("You can not put there.\nPlease input again.\n");
      *step -= 1;
    }
  }
}

/* ����Ū�� main �ؿ�����򤤤���ɬ�פϤ���ޤ���
   ���̤Υ��������Ѥ��������� 8 ����ʬ�򹥤��ʿ������ѹ����Ƥ���������*/
int main(int argc, char **argv) {
  if (argc != 1 && argc != 3) {
    printf("Invalid Arg.\n");
    return 2;
  }
  int n = 8;
  if (argc == 3) {
    if (strcmp("-n", argv[1]) == 0)
      n = atoi(argv[2]);
    else {
      printf("Invalid Argformat.\n");
      return 2;
    }
  }

  int i, j;
  struct view view =
                  {
                      512,
                      512,
                  },
              *v = &view; /* ���ޤ��ʤ� (������ɥ�) */
  srand((unsigned int)time(NULL));
  int playerColor = rand() % 2;
  if (playerColor == BLACK) printf("You are black.\n");
  if (playerColor == WHITE) printf("You are white.\n");
  struct othello othello_instance = {n,      playerColor, {2, 2},
                                     {0, 0}, NULL,        NULL},
                 *o = &othello_instance; /* 8�ϥܡ��ɤ���(��)����*/

  /* �ܡ��ɺ���(2��������γ���) */
  o->board = (int **)malloc(sizeof(int *) * o->n);
  for (i = 0; i < o->n; i++) {
    o->board[i] = (int *)malloc(sizeof(int) * o->n);
  }

  o->board_log = (int ***)malloc(sizeof(int **) * (o->n * o->n));
  for (i = 0; i < o->n * o->n; i++) {
    o->board_log[i] = (int **)malloc(sizeof(int *) * o->n);
    for (j = 0; j < o->n; j++)
      o->board_log[i][j] = (int *)malloc(sizeof(int) * o->n);
  }

  view_init(v);  // ���ޤ��ʤ� (X������ɥ�)
  view_loop(v, o);  // ����(���δؿ������ myothello �ؿ����ƤФ��)
  view_out(v);      // ���ޤ��ʤ� (X������ɥ�)

  // �ܡ��ɤκ��(2��������Υ�������)
  for (i = 0; i < o->n; i++) {
    free(o->board[i]);
  }
  free(o->board);

  for (i = 0; i < o->n * o->n; i++) {
    for (j = 0; j < o->n; j++) {
      free(o->board_log[i][j]);
    }
    free(o->board_log[i]);
  }
  free(o->board_log);

  return 0;
}
