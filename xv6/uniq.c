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

int areStringsTheSame(struct line *line1, struct line *line2, int iFlag)
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
      if (line1->lineString[i] != line2->lineString[i] && !iFlag)
      {
        return 0;
      }
      else if (iFlag)
      {
        int diff;
        diff = line1->lineString[i] - line2->lineString[i];
        if (diff == 0)
        {
          continue;
        }
        else if (diff < 0)
        {
          diff *= -1;
        }
        if (diff != 32 && diff != 0)
        {
          return 0;
        }
      }
    }
  }
  return result;
}

void uniq(int fd, struct line *line1, struct line *line2, int iFlag, int cFlag, int dFlag)
{
  int n, addingLine1, addingLine2, counter;
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
        counter = 1;
      }
    }
    else if (addingLine2)
    {
      addTo(line2, &buf[0]);
      if (buf[0] == '\n')
      {
        addingLine1 = 0;
        addingLine2 = 0;
        if (areStringsTheSame(line1, line2, iFlag))
        {
          initializeLine(line2);
          addingLine2 = 1;
          counter++;
        }
        else
        {
          if (cFlag)
          {
            printf(1, "%d %s", counter, line1->lineString);
          }
          else if (!dFlag || counter > 1)
          {
            printf(1, "%s", line1->lineString);
          }
          counter = 1;
          free(line1->lineString);
          line1->lineString = line2->lineString;
          line1->lineSize = line2->lineSize;
          line1->lineCap = line2->lineCap;
          initializeLine(line2);
          addingLine2 = 1;
        }
      }
    }
  }

  if (n < 0)
  {
    printf(1, "uniq: read error\n");
    exit();
  }

  if (cFlag)
  {
    printf(1, "%d %s", counter, line1->lineString);
  }
  else if (!dFlag || counter > 1)
  {
    printf(1, "%s", line1->lineString);
  }

  free(line1->lineString);
  free(line2->lineString);
}

int main(int argc, char *argv[])
{
  int fd, i, iFlagBool, cFlagBool, dFlagBool;
  struct line line1, line2;

  iFlagBool = 0, cFlagBool = 0, dFlagBool = 0;
  char I_FLAG[3] = {'-', 'i', '\0'};
  char C_FLAG[3] = {'-', 'c', '\0'};
  char D_FLAG[3] = {'-', 'd', '\0'};

  initializeLine(&line1);
  initializeLine(&line2);

  if (argc <= 1)
  {
    uniq(0, &line1, &line2, iFlagBool, cFlagBool, dFlagBool);
    exit();
  }

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], I_FLAG) == 0)
    {
      iFlagBool = 1;
    }
    else if (strcmp(argv[i], C_FLAG) == 0)
    {
      cFlagBool = 1;
    }
    else if (strcmp(argv[i], D_FLAG) == 0)
    {
      dFlagBool = 1;
    }
    else if ((fd = open(argv[i], 0)) < 0)
    {
      printf(1, "uniq: cannot open %s\n", argv[i]);
      exit();
    }
  }

  uniq(fd, &line1, &line2, iFlagBool, cFlagBool, dFlagBool);
  close(fd);

  exit();
}
