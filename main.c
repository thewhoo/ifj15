/*
 * Course IFJ @ FIT VUT Brno, 2015
 * IFJ15 Interpreter Project
 *
 * Authors:
 * Lukas Osadsky  - xosads00
 * Pavol Plaskon  - xplask00
 * Pavel Pospisil - xpospi88
 * Matej Postolka - xposto02
 *
 * Unless otherwise stated, all code is licensed under a
 * Creative Commons Attribution-ShareAlike 4.0 International License
 *
 */

#include <stdio.h>
#include "string.h"
#include "enums.h"
#include "lex.h"
#include "galloc.h"

int main()
{
  gcInit();

  TString s;
  initString(&s, STR_DEFAULT_SIZE);

  for(int i = 65; i < 70; i++)
      insertIntoString(&s, i);
  insertIntoString(&s, 0);

  printf("%s\n", s.data);

  //freeString(&s);

  for(int i=0; i<10000; i++)
  {
	  int *p = gmalloc(sizeof(int));
	  *p = i;
	  printf("%d\n", *p);
  }

  int **ptr2 = gmalloc(10*sizeof(int*));
  for (int i=0; i<10; i++)
  {
	  ptr2[i]=gmalloc(sizeof(int));
	  *ptr2[i] = i;
  }
  for (int i=0; i<10; i++)
	  printf("%d\n", *ptr2[i]);

  ptr2 = grealloc(ptr2, 10000*sizeof(int*));
  ptr2[9999]=gmalloc(sizeof(int));
  *ptr2[9999]=42;
  printf("%d\n", *ptr2[9999]);

  gcDestroy();

  return 0;

}
