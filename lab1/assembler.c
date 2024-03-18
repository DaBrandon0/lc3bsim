/*
	Name 1: Brandon Lui
	Name 2: Vikranth Ragavan
	UTEID 1: BSL846
	UTEID 2: VSR439
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

FILE* infile = NULL;
FILE* outfile = NULL;
FILE * pOutfile;

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define MAX_LINE_LENGTH 255

	enum
	{
	   DONE, OK, EMPTY_LINE
	};

typedef struct 
    {
	    int address;
	    char label[MAX_LABEL_LEN + 1];	/* Question for the reader: Why do we need to add 1? */
    } TableEntry;
  TableEntry symbolTable[MAX_SYMBOLS];

int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
int main(int argc, char * argv[]);
int toNum(char* pStr);
int isOpcode(char* OP);
char* toHex(int num);
char out[] = "x0000";
int labelcomp (char*, int symbols, int, TableEntry symbol_T[], int);

int main(int argc, char* argv[])
{ 
  /* open the source file */
  infile = fopen(argv[1], "r");
  outfile = fopen(argv[2], "w");
	pOutfile = fopen( argv[2], "w" );
  if (!infile) 
    {
     printf("Error: Cannot open file %s\n", argv[1]);
     exit(4);
	  }
  if (!outfile) 
    {
      printf("Error: Cannot open file %s\n", argv[2]);
      exit(4);
    }
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,*lArg2, *lArg3, *lArg4;
  int lRet;
  FILE * lInfile;
  lInfile = fopen( argv[1], "r" );
  int repeat = 0;
  int buildsymbol = 1;
  int symbolcount = 0;
  int current_address;
  do
  {
    do
	  {
		  lRet = readAndParse( lInfile, lLine, &lLabel,&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
      int inst_num = 0;
		  if( lRet != DONE && lRet != EMPTY_LINE )
	    {
        if (buildsymbol == 1)          
        {
          if (strcmp(lOpcode,".orig") == 0)
            {
              current_address = toNum(lArg1)-2;
            }
           if (strcmp(lLabel,"")!=0)
           {
            symbolTable[symbolcount].address = current_address;
            for (int i = 0; lLabel[i] != '\0'; i++)
              symbolTable[symbolcount].label[i] = lLabel[i];
            symbolcount++;
            }
        }
        else 
        {
          if (strcmp(lOpcode,".orig") == 0)
            {
              current_address = toNum(lArg1);
              toHex(current_address);
              fprintf(pOutfile, "0%s\n", out);
              current_address = current_address - 2;
            }
          else if (strcmp(lOpcode,"add") == 0)
            {
              inst_num = inst_num + 4096;
              inst_num = inst_num + 512*((int)lArg1[1]-'0');
              inst_num = inst_num + 64*((int)lArg2[1]-'0');
              if (lArg3[0] == 'r')
                inst_num = inst_num + 1*((int)lArg3[1]-'0');
              else
                {
                  if (toNum(lArg3)<0)
                    inst_num = inst_num + (16+toNum(lArg3)) + 32 + 16;
                  else
                    inst_num = inst_num + (toNum(lArg3)) + 32;
                }
              toHex(inst_num);
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"and") == 0)
            {
              inst_num = inst_num + 20480;
              inst_num = inst_num + 512*((int)lArg1[1]-'0');
              inst_num = inst_num + 64*((int)lArg2[1]-'0');
              if (lArg3[0] == 'r')
                inst_num = inst_num + 1*((int)lArg3[1]-'0');
              else
                {
                  if (toNum(lArg3)<0)
                    inst_num = inst_num + (16+toNum(lArg3)) + 32 + 16;
                  else
                    inst_num = inst_num + (toNum(lArg3)) + 32;
                }
              toHex(inst_num);
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brn") == 0)
            {
              inst_num = inst_num + 2048;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brz") == 0)
            {
              inst_num = inst_num + 1024;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brp") == 0)
            {
              inst_num = inst_num + 512;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brzp") == 0)
            {
              inst_num = inst_num + 1024+512;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brnp") == 0)
            {
              inst_num = inst_num + 2048 + 512;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brnz") == 0)
            {
              inst_num = inst_num + 2048 + 1024;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"brnzp") == 0 || strcmp(lOpcode,"br") == 0)
            {
              inst_num = inst_num + 2048 + 1024 + 512;
              toHex(labelcomp(lArg1,symbolcount,inst_num,symbolTable,current_address));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"jmp") == 0 )
            {
              toHex(32768 + 16384 + 64*((int)lArg1[1]-'0'));
              fprintf( pOutfile, "0%s\n", out);
            }
          else if (strcmp(lOpcode,"ret") == 0 )
          {
            toHex(32768 + 16384 + 64 + 128 + 256);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"jsr") == 0 )
          {
            inst_num = 16384 + 2048;
            for (int i = 0; i < symbolcount; i++)
            {
            if(strcmp(symbolTable[i].label,lArg1) == 0)
              if(current_address-symbolTable[i].address < 0)
                inst_num = (symbolTable[i].address-current_address)/2 + inst_num - 1;
              else
                inst_num = inst_num + 1024 + (1024-(current_address-symbolTable[i].address)/2)-1;
            }
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"jsrr") == 0 )
          {
            toHex(16384 + 64*((int)lArg1[1]-'0'));
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"ldb") == 0)
          {
            inst_num = 8192+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0');
            if (toNum(lArg3)<0)
              inst_num = inst_num + (32+toNum(lArg3)) + 32;
              else
              inst_num = inst_num + (toNum(lArg3));
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"ldw") == 0)
          {
            inst_num = 16384+8192+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0');
            if (toNum(lArg3)<0)
              inst_num = inst_num + (32+toNum(lArg3)) + 32;
              else
              inst_num = inst_num + (toNum(lArg3));
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"lea") == 0)
          {
            inst_num = 32768+16384+8192+512*((int)lArg1[1]-'0');
            for (int i = 0; i < symbolcount; i++)
            {
            if(strcmp(symbolTable[i].label,lArg2) == 0)
              if(current_address-symbolTable[i].address < 0)
                inst_num = (symbolTable[i].address-current_address)/2 + inst_num - 1;
              else
                inst_num = inst_num + 256 + (256-(current_address-symbolTable[i].address)/2)-1;
            }
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"not") == 0)
          {
            toHex(32768+4096+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0')+32+16+8+4+2+1);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"rti") == 0)
          {
            toHex(32768);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"lshf") == 0 || strcmp(lOpcode,"rshfl") == 0 || strcmp(lOpcode,"rshfa") == 0)
          {
            inst_num = 32768 + 16384 + 4096 + 512*((int)lArg1[1]-'0') + 64*((int)lArg2[1]-'0');
            if (strcmp(lOpcode,"rshfl") == 0)
              inst_num = inst_num + 16;
            else if (strcmp(lOpcode,"rshfa") == 0)
              inst_num = inst_num + 16 + 32;
            toHex(inst_num + toNum(lArg3));
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"stb") == 0)
          {
            inst_num = 4096+8192+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0');
            if (toNum(lArg3)<0)
              inst_num = inst_num + (32+toNum(lArg3)) + 32;
              else
              inst_num = inst_num + (toNum(lArg3));
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"stw") == 0)
          {
            inst_num = 16384+4096+8192+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0');
            if (toNum(lArg3)<0)
              inst_num = inst_num + (32+toNum(lArg3)) + 32;
              else
              inst_num = inst_num + (toNum(lArg3));
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"trap") == 0 || strcmp(lOpcode,"halt") == 0)
          {
            if (strcmp(lOpcode,"halt") == 0)
              toHex(32768+16384+4096+8192+toNum("x25"));
            else
              toHex(32768+16384+4096+8192+toNum(lArg1));
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"xor") == 0 || strcmp(lOpcode,"not") == 0)
          {
            inst_num = 32768+4096+512*((int)lArg1[1]-'0')+64*((int)lArg2[1]-'0');
            if(strcmp(lOpcode,"xor") == 0)
              if(lArg3[0] == 'r')
                inst_num = inst_num + (int)lArg3[1]-'0';
              else
                if (toNum(lArg3)<0)
                  inst_num = inst_num + (16+toNum(lArg3)) + 32 + 16;
                else
                  inst_num = inst_num + (toNum(lArg3))+32;
            else
              inst_num = inst_num + 63;
            toHex(inst_num);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,"nop") == 0)
          {
            toHex(0);
            fprintf( pOutfile, "0%s\n", out);
          }
          else if (strcmp(lOpcode,".fill") == 0)
          {
            toHex(toNum(lArg1));
            fprintf( pOutfile, "0%s\n", out);
          }

        





        }
          current_address = current_address + 2;
		    }
        } 
    while( lRet != DONE );
    repeat++;
    buildsymbol--;
    rewind(lInfile);
    } 
    while (repeat < 2);
    fclose(infile);
    fclose(outfile);
}

int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;
   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

int labelcomp(char* label, int symbols, int inst_num, TableEntry symbolTable[], int addynow)
{
  for (int i = 0; i < symbols; i++)
  {
    if(strcmp(symbolTable[i].label,label) == 0)
      if(addynow-symbolTable[i].address < 0)
        inst_num = (symbolTable[i].address-addynow)/2 + inst_num - 1;
      else
        inst_num = inst_num + 256 + (256-(addynow-symbolTable[i].address)/2)-1;
  }
  return inst_num;
}

char* toHex(int num)
{
  if (num<0)
    return toHex (32768+(num+32768));
  for (int i = 0; i < 4; i++)
    out[i+1]='0';
  char digits[] = "0123456789ABCDEF";
  int spacesleft = 4;
  int letter;
  int check = 0;
  while (check == 0)
  {
    if (num/16 == 0)
      check = -1;
    letter = num%16;
    num = num/16;
    out[spacesleft] = digits[letter];
    spacesleft--;
  }
  return out;
}

int isOpcode(char* OP)
{
    if (strcmp(OP,"add") == 0 || strcmp(OP,"and") == 0 || strcmp(OP,"brn") == 0 ||
        strcmp(OP,"brz") == 0 || strcmp(OP,"brp") == 0 || strcmp(OP,"br") == 0 ||
        strcmp(OP,"brzp") == 0 || strcmp(OP,"brnp") == 0 || strcmp(OP,"brnz") == 0 ||
        strcmp(OP,"brnzp") == 0 || strcmp(OP,"jmp") == 0 || strcmp(OP,"ret") == 0 ||
        strcmp(OP,"jsr") == 0 || strcmp(OP,"jsrr") == 0 || strcmp(OP,"ldb") == 0 ||
        strcmp(OP,"lea") == 0 || strcmp(OP,"not") == 0 || strcmp(OP,"ret") == 0 ||
        strcmp(OP,"rti") == 0 || strcmp(OP,"lshf") == 0 || strcmp(OP,"rshfl") == 0 ||
        strcmp(OP,"rshfa") == 0 || strcmp(OP,"stb") == 0 || strcmp(OP,"stw") == 0 ||
        strcmp(OP,"trap") == 0 || strcmp(OP,"halt") == 0 || strcmp(OP,"xor") == 0 ||
        strcmp(OP,"not") == 0 || strcmp(OP, "nop") == 0 || strcmp(OP,"ldw") == 0 )
    {
        return 0;
    }
    else return -1;

}

int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, 
                    char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
	{
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );
        
	   if( isOpcode(lPtr) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	
           *pOpcode = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}


