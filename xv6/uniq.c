#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1];

struct line
{
  char *lineString;
  int lineCap;
  int lineSize;
};

void initializeLine(struct line *givenLine)
{
  givenLine->lineString = malloc(2);
  givenLine->lineString[1] = '\0';
  givenLine->lineCap = 1;
  givenLine->lineSize = 0;
}

void addTo(struct line *givenLine, char *newChar)
{
  if (givenLine->lineSize == givenLine->lineCap)
  {
    char *newLine;
    int newCap, i;

    newCap = ((givenLine->lineCap + 1) * 2) - 1;
    newLine = malloc(newCap + 1);
    newLine[newCap] = '\0';
    for (i = 0; i < givenLine->lineSize + 1; i++)
    {
      newLine[i] = givenLine->lineString[i];
    }
    givenLine->lineString = newLine;
    givenLine->lineCap = newCap;
  }
  givenLine->lineString[givenLine->lineSize] = *newChar;
  givenLine->lineString[++(givenLine->lineSize)] = '\0';
}

int areStringsTheSame(struct line *line1, struct line *line2)
{
  int result, i;
  result = 1;

  if (line1->lineSize != line2->lineSize)
  {
    result = 0;
  }
  else
  {
    for (i = 0; i < line1->lineSize; i++)
    {
      if (line1->lineString[i] != line2->lineString[i])
      {
        result = 0;
      }
    }
  }
  return result;
}

void uniq(int fd, struct line *line1, struct line *line2)
{
  int n, addingLine1, addingLine2;
  addingLine1 = 1;
  addingLine2 = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0)
  {
    if (addingLine1)
    {
      addTo(line1, &buf[0]);
      if (buf[0] == '\n')
      {
        addingLine1 = 0;
        addingLine2 = 1;
      }
    }
    else if (addingLine2)
    {
      addTo(line2, &buf[0]);
      if (buf[0] == '\n')
      {
        addingLine1 = 0;
        addingLine2 = 0;
      }
    }
    if (!(addingLine2) && !(addingLine1))
    {
      if (areStringsTheSame(line1, line2))
      {
        initializeLine(line2);
        addingLine2 = 1;
      }
      else
      {
        printf(1, "%s", line1->lineString);
        free(line1->lineString);
        line1->lineString = line2->lineString;
        line1->lineSize = line2->lineSize;
        line1->lineCap = line2->lineCap;
        initializeLine(line2);
        addingLine2 = 1;
      }
      //  addTo(line2, &buf[0]);
    }
  }

  if (n < 0)
  {
    printf(1, "uniq: read error\n");
    exit();
  }
  printf(1, "%s", line1->lineString);
  printf(1, "%s", line2->lineString);
  free(line1->lineString);
  free(line2->lineString);
}

int main(int argc, char *argv[])
{
  int fd;
  struct line line1, line2;

  initializeLine(&line1);
  initializeLine(&line2);

  if (argc <= 1)
  {
    uniq(0, &line1, &line2);
    exit();
  }

  if ((fd = open(argv[1], 0)) < 0)
  {
    printf(1, "uniq: cannot open %s\n", argv[1]);
    exit();
  }

  uniq(fd, &line1, &line2);
  close(fd);

  exit();
}
