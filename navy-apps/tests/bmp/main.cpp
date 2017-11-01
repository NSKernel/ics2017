#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  printf("Started\n");
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("malloc\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("Bitmap loaded\n");
  assert(bmp->pixels);
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("Display opened\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("Rect drawed\n");
  NDL_Render();
  printf("Rendered\n");
  NDL_CloseDisplay();
  while (1);
  return 0;
}
