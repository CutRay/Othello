#include "view_othello.h"

void view_init(struct view *v) {
  if (!(v->display = XOpenDisplay(NULL))) {
    exit(1);
  }
  v->screen_number = DefaultScreen(v->display);
  v->parent = RootWindow(v->display, v->screen_number);
  v->window = XCreateSimpleWindow(v->display, v->parent, 0, 0, v->w, v->h, 0,
                                  BlackPixel(v->display, v->screen_number),
                                  WhitePixel(v->display, v->screen_number));
  v->gc = DefaultGC(v->display, v->screen_number);
  XSetForeground(v->display, v->gc, BlackPixel(v->display, v->screen_number));

  v->event_mask = ExposureMask;
  v->event_mask |= KeyPressMask;
  XSelectInput(v->display, v->window, v->event_mask);
  XMapWindow(v->display, v->window);
  XFlush(v->display);
  while (!0) {
    XNextEvent(v->display, &v->event);
    switch (v->event.type) {
      case Expose:
        return;
        break;
    }
  }
}

void view_out(struct view *v) {
  XUnmapWindow(v->display, v->window);
  XCloseDisplay(v->display);
}

void view_draw(struct view *v, void *a) {
  struct othello *o = (struct othello *)a;
  int i, j;
  char buf[BUFSIZ];
  double dx, sdx, ssdx;

  XClearWindow(v->display, v->window);

  dx = v->h / (o->n + 1);
  sdx = dx / 2;
  ssdx = sdx / 2;

  for (i = 0; i < o->n; i++) {
    sprintf(buf, "%02d", i + 1);
    XDrawString(v->display, v->window, v->gc, 5, 25 + dx / 2 + i * dx, buf, 2);
    for (j = 0; j < o->n; j++) {
      if (i == 0) {
        sprintf(buf, "%02d", j + 1);
        XDrawString(v->display, v->window, v->gc, 20 + dx / 2 + j * dx - 1, 15,
                    buf, 2);
      }
      XDrawRectangle(v->display, v->window, v->gc, 20 + j * dx, 20 + i * dx, dx,
                     dx);
      if (o->board[i][j] != EMPTY) {
        if (o->board[i][j] == BLACK) {  //¹õ´Ý¤òÉÁ¤¯
          XFillArc(v->display, v->window, v->gc, 20 + j * dx + ssdx,
                   20 + i * dx + ssdx, sdx, sdx, 0, 360 * 64);
        } else {  //Çò´Ý¤òÉÁ¤¯
          XDrawArc(v->display, v->window, v->gc, 20 + j * dx + ssdx,
                   20 + i * dx + ssdx, sdx, sdx, 0, 360 * 64);
        }
      }
    }
  }
  XFlush(v->display);
}

void view_loop(struct view *v, struct othello *o) {
  int step;

  init_othello(o);
  step = 0;
  saveBoard(step, o);
  view_draw(v, o);
  while (!0) {
    usleep(10 * 100);
    myothello(&step, o);
    step++;
    view_draw(v, o);
  }
}

void init_othello(struct othello *o) {
  int i, j;

  for (i = 0; i < o->n; i++) {
    for (j = 0; j < o->n; j++) {
      o->board[i][j] = EMPTY;
    }
  }
  o->board[o->n / 2][o->n / 2] = BLACK;
  o->board[o->n / 2 - 1][o->n / 2 - 1] = BLACK;

  o->board[o->n / 2 - 1][o->n / 2] = WHITE;
  o->board[o->n / 2][o->n / 2 - 1] = WHITE;
}
