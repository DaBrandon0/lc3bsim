/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1:Brandon Lui
    Name 2:Vikranth Ragavan
    UTEID 1:BSL846
    UTEID 2:VSR439
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

void conditcode(int result);

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */

  
  int inst = MEMORY[(int)((unsigned int)CURRENT_LATCHES.PC >> 1)][0] + MEMORY[(int)((unsigned int)CURRENT_LATCHES.PC >> 1)][1]*256;
  int opcode = (int)((unsigned int)inst >> 12);
  int DR = (int)((unsigned int)inst >> 9)&7;
  int result;
  int next_pc = CURRENT_LATCHES.PC + 2;
  if (opcode == 1)//ADD
  {
    if((int)(((unsigned int)inst >> 5)&1) == 0)
      result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] + CURRENT_LATCHES.REGS[inst&7];
    else
    {
        if(inst&16)
            result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] - (16-(inst)&15);
        else
            result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] + (inst&15);
    }
    result = Low16bits(result);
    conditcode(result);
    NEXT_LATCHES.REGS[DR] = result;
  }
  else if (opcode == 5)//AND
  {
    if((int)(((unsigned int)inst >> 5)&1) == 0)
        result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] & CURRENT_LATCHES.REGS[inst&7];
    else 
        if(inst&16)
            result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] & (-(16-(inst)&15));
        else
            result = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7] & (inst&15);
    result = Low16bits(result);
    conditcode(result);
    NEXT_LATCHES.REGS[DR] = result;
  }
  else if (opcode == 0)//BR
  {
    if((CURRENT_LATCHES.N&((int)((unsigned int)inst >> 11)&1))||
    (CURRENT_LATCHES.Z&((int)((unsigned int)inst >> 10)&1))||
    (CURRENT_LATCHES.P&((int)((unsigned int)inst >> 9)&1))||
    (int)((unsigned int)inst >> 9) == 7)
    {
        if(inst&256)
            next_pc = next_pc + 2*(-(256-(inst)&255));
        else
            next_pc = next_pc + 2*(inst&255);
    }
  }
  else if (opcode == 12)//JMP
  {
    next_pc = CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)];
  }
  else if (opcode == 4)//JSR/R
  {
    result = next_pc;
    if (inst&2048)
    {
      if(inst&1024)
            next_pc = next_pc + 2*(-(1024-(inst)&1023));
        else
            next_pc = next_pc + 2*(inst&1023);
    }
    else
    {
      next_pc = CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)];
    }
    NEXT_LATCHES.REGS[7] = result;
  }
  else if (opcode == 2)//LDB
  {
    int byte = 0;
    if(inst&32)
    {
      byte = (MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] - (32-(inst)&31))/2][(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] - (32-(inst)&31))%2]);
    }
    else 
    {
      byte = MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] + (inst&31))/2][(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] + (inst&31))%2];
    }
    if(byte&128)
      byte = -(128-(byte)&127);
    else
      byte = byte&127;
    byte = Low16bits(byte);
    conditcode(byte);
    NEXT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)] = byte;
  }
  else if (opcode == 6)//LDW
  {
    if(inst&32)
      result = MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] - 2*(32-(inst)&31))/2][0] + MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] - 2*(32-(inst)&31))/2][1]*256;
    else
      result = MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] + 2*(inst&31))/2][0]+MEMORY[(CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)] + 2*(inst&31))/2][1]*256;
    result = Low16bits(result);
    conditcode(result);
    NEXT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)] = result;
  }
  else if (opcode == 14)//LEA
  {
    int number = inst;
    if(inst&256)
      number = (number&255) - 256;
    else 
      number = number&255; 
    number = number*2 + next_pc;
    number = Low16bits(number);
    NEXT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)] = number;
  }
  else if (opcode == 9)//XOR and NOT
  {
    if(inst&32)
    {
      if (inst&16)
        result = -(16 - (inst&15));
      else 
        result = inst&15;
      result = result^CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)];
    }
    else
    {
      result = CURRENT_LATCHES.REGS[inst&7]^CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)];
    }
    result = Low16bits(result);
    conditcode(result);
    NEXT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)] = result;
  }
  else if (opcode == 13)//SHF
  {
    int direction = ((int)((unsigned int)inst >> 4)&3);
    int16_t number = CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 6)&7)];
    if (direction == 0)
    { 
      result = number << (inst&15);
    }
    else if (direction == 1)
    {
      result = (int16_t)((uint16_t)number >> (inst&15));
    }
    else
    {
      result = number >> (inst&15);
    }
    result = Low16bits(result);
    conditcode(result);
    NEXT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)] = result;
  }
  else if (opcode == 3)//STB
  {
    int number;
    if(inst&32)
      number = -(32 - (inst&31));
    else 
      number = inst&31;
    number = (CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7]+number);
    number = Low16bits(number);
    result = CURRENT_LATCHES.REGS[((int)((unsigned int)inst >> 9)&7)];
    if(result&128)
      result = -(128-(result)&127);
    else
      result = result&127;
    result = result&255;
    MEMORY[number/2][number%2] = result;
  }
  else if (opcode == 7)//STW
  {
    if (inst&32)
    {
      result = -(32 - (inst&31));
    }
    else
    {
      result = inst&31;
    }
    result = 2*result + CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 6)&7];
    result = Low16bits(result);
    MEMORY[(result)/2][0] = CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 9)&7]&255;
    MEMORY[(result)/2][1] = (int16_t)((uint16_t)CURRENT_LATCHES.REGS[(int)((unsigned int)inst >> 9)&7] >> 8);
  }
  else if (opcode == 15)//TRAP
  {
    NEXT_LATCHES.REGS[7] = next_pc;
    next_pc = MEMORY[(inst&255)<<1][0]+(MEMORY[(inst&255)<<1][1])*255;
  }
  NEXT_LATCHES.PC = next_pc;


}

void conditcode (int result){
    if (result >= 32768)
    {
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.N = 1;
    }
    else if (result == 0)
    {
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.N = 0;
    }
    else
    {
        NEXT_LATCHES.P = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.N = 0;
    }
    return;
  } 