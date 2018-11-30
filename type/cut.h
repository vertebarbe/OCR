#ifndef CUT_H
#define CUT_H
#include <stdio.h>
#include <stdlib.h>
#include "image.h"

Rect CutBorder(Image image);
void CopyImage(Image image, Image *destination);
void FreeImage(Image image);
void cutLine(Image image, Rect rect, FILE *f, float *w1, float *w2, char **text);
void cutLine2(Image image, Rect rect, ListHead *list);
void CutChar(Image image, Rect line, FILE *f, float *w1, float *w2, char **text);
void CutChar2(Image image, Rect line, ListHead *list);
void CharProcess(Image image, Rect rect, FILE *f, float *w1, float *w2, char **text);
void DrawRect(Rect rect, Image image, int hor_val, int ver_val);
void DrawRect_hor(Rect rect, Image image, int val);
void DrawRect_ver(Rect rect, Image image, int val);
int GetLineThresold(Image image, Rect line);
Image Parse_Image(Image image, char **text, float *w1, float *w2);
Image cut(char *path, char *text);
Image cut_old(char *path, char *text);
Image cut_new(char *path, char *text, int learnIterations);
void CorrectRect(Image i, Rect *r);
#endif
