/*
    Name 1: Your Name
    UTEID 1: Your UTEID
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD2, IRD1, IRD0,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX1, ADDR1MUX0,
    ADDR2MUX2, ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    GATE_SSP,
    LD_SSP,
    SSPMUX1, SSPMUX0,
    GATE_USP,
    LD_USP,
    USPMUX1, USPMUX0,
    GATE_PSR,
    GATE_VECTOR,
    LD_PSR,
    LD_VECTOR,
    FLIP_MODE,
    ADDRESSED,
    LAST_STATE2, LAST_STATE1, LAST_STATE0,
    LD_VA,
    SET_PTE,
    LD_LS,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return((x[IRD2] << 2) + (x[IRD1] << 1) + x[IRD0]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return((x[ADDR1MUX1] << 1) + x[ADDR1MUX0]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX2] << 2) + (x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetGATE_SSP(int *x)         { return(x[GATE_SSP]); }
int GetLD_SSP(int *x)         { return(x[LD_SSP]); }
int GetSSPMUX(int *x)         { return((x[SSPMUX1]<<1)+x[SSPMUX0]); }
int GetGATE_USP(int *x)         {return(x[GATE_USP]);}
int GetLD_USP(int *x)         {return(x[LD_USP]);}
int GetUSPMUX(int *x)         { return((x[USPMUX1]<<1)+x[USPMUX0]); }
int GetGATE_PSR(int *x)         { return(x[GATE_PSR]); }
int GetGATE_VECTOR(int *x)         { return(x[GATE_VECTOR]); }
int GetLD_PSR(int *x)         { return(x[LD_PSR]); }
int GetLD_VECTOR(int *x)         { return(x[LD_VECTOR]); }
int GetFLIP_MODE(int *x)         { return(x[FLIP_MODE]); }
int GetADDRESSED(int *x)         { return(x[ADDRESSED]); }
int GetLAST_STATE(int *x)      { return((x[LAST_STATE2] << 2) + (x[LAST_STATE1] << 1) + x[LAST_STATE0]); }
int GetLD_VA(int *x)         { return(x[LD_VA]); }
int GetSET_PTE(int *x)         { return(x[SET_PTE]); }
int GetLD_LS(int *x)         { return(x[LD_LS]); }
/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
int INTsig;
int EXCsig;
int USP;
int PSR_15;
int vector;
/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
int last_state;
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

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

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
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
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.INTsig = 0;
    CURRENT_LATCHES.EXCsig = 0;
    CURRENT_LATCHES.USP = 0;
    CURRENT_LATCHES.PSR_15 = 1;
    CURRENT_LATCHES.vector = 0;
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
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int16_t GateMARMUX_result;
int16_t GatePC_result;
int16_t GateALU_result;
int16_t GateSHF_result;
int16_t GateMDR_result;
int16_t addradd_res;
int16_t GateSSP_result;
int16_t GateUSP_result;
uint16_t GatePSR_result;
int16_t GateVECTOR_result;

int16_t signext(int num, int bite);

void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */

  if (CYCLE_COUNT == 300)
    {
      NEXT_LATCHES.INTsig = 1;
      NEXT_LATCHES.INTV = 0x01;
    }
  int nextstate = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
  if(CURRENT_LATCHES.STATE_NUMBER == 10)
  {
      nextstate = 19;
      NEXT_LATCHES.EXCsig = 1;
      CURRENT_LATCHES.EXCsig = 1;
  }
  if(GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
  {
    nextstate = (int)((unsigned int)CURRENT_LATCHES.IR >> 12);
  }
  else if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
  {
    if(GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
  {
    if(CURRENT_LATCHES.BEN)
        nextstate = nextstate + 4;
  }
  else if (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
  {
    if(CURRENT_LATCHES.READY)
        nextstate = nextstate + 2;
  }
  else if (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
  {
    if(CURRENT_LATCHES.IR&2048)
        nextstate = nextstate + 1;
  }
  else if (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) == 4)
  {
    if((CURRENT_LATCHES.INTsig && CURRENT_LATCHES.PSR_15)||CURRENT_LATCHES.EXCsig)
        nextstate = nextstate + 8;
  }
  }
  else if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
  {
    if(CURRENT_LATCHES.MAR&1)
    {
        nextstate = 19;
        NEXT_LATCHES.EXCsig = 1;
        CURRENT_LATCHES.EXCsig = 1;
    }
  }
  else if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 4)
  {
    nextstate = CURRENT_LATCHES.last_state;
  }
  
  else if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
  {
    if((CURRENT_LATCHES.PSR_15 && !(CURRENT_LATCHES.MDR&8) && CURRENT_LATCHES.last_state != 28)||!(CURRENT_LATCHES.MDR&4))
    {
        nextstate = 19;
        NEXT_LATCHES.EXCsig = 1;
        CURRENT_LATCHES.EXCsig = 1;
    }
  }
  NEXT_LATCHES.STATE_NUMBER = nextstate;
  for (int i = 0; i<62; i++)
    NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[nextstate][i];

}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
  int static counter;
  if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    if(counter == 3)
    {
      NEXT_LATCHES.READY = 1;
      counter = 0;
    }
    else
    {
      if (NEXT_LATCHES.READY == 0)
      {
        counter = counter + GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
      }
      NEXT_LATCHES.READY = 0;
    }     
  }
}



void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
  int16_t addradd_op1;
  int16_t addradd_op2;
  int16_t alu_op1;
  int16_t alu_op2;
  int16_t SR1;

  if(CURRENT_LATCHES.READY == 0 && GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
    return;
  if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
    SR1 = CURRENT_LATCHES.REGS[((int)((unsigned int)CURRENT_LATCHES.IR >> 9))&7];
  else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    SR1 = CURRENT_LATCHES.REGS[((int)((unsigned int)CURRENT_LATCHES.IR >> 6))&7];
  else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
    SR1 = 0x0200;
  else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
    SR1 = CURRENT_LATCHES.REGS[6];

  if(GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)==1)
    addradd_op2 = SR1;
  else if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)==0)
    addradd_op2 = CURRENT_LATCHES.PC;
  else if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)==2)
    addradd_op2 = CURRENT_LATCHES.PTBR&65280;
  else if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)==3)
    addradd_op2 = CURRENT_LATCHES.MDR&15872;
  if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
    addradd_op1 = 0;
  else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    addradd_op1 = signext(CURRENT_LATCHES.IR,6);
  else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
    addradd_op1 = signext(CURRENT_LATCHES.IR,9);
  else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
    addradd_op1 = signext(CURRENT_LATCHES.IR,11);
  else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 4)
    addradd_op1 = (int)((unsigned int)(CURRENT_LATCHES.VA&65024) >> 9);
  else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 5)
    addradd_op1 = CURRENT_LATCHES.VA&255;
  if(GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))
    addradd_op1 = addradd_op1 << 1;
  addradd_res = addradd_op1 + addradd_op2;
  if(GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    GateMARMUX_result = addradd_res;
  else
    GateMARMUX_result = ((CURRENT_LATCHES.IR)&255) << 1;
  
  
  alu_op1 = SR1;
  if (CURRENT_LATCHES.IR&32)
    alu_op2 = signext(CURRENT_LATCHES.IR,5); 
  else
    alu_op2 = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR&7];
  if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0)  
    GateALU_result = alu_op1 + alu_op2;
  else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    GateALU_result = alu_op1 & alu_op2;
  else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
    GateALU_result = alu_op1 ^ alu_op2;
  else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
    GateALU_result = alu_op1;
  
  
  if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
  {
  if((CURRENT_LATCHES.IR&48) == 48)
    GateSHF_result = SR1 >> (CURRENT_LATCHES.IR&15);
  else if ((CURRENT_LATCHES.IR&48) == 16) 
    GateSHF_result = (int16_t)((uint16_t)SR1 >> (CURRENT_LATCHES.IR&15));
  else
    GateSHF_result = SR1 << (CURRENT_LATCHES.IR&15);
    }
  
  
  if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
    GateMDR_result = CURRENT_LATCHES.MDR;
  else 
  {
    if(CURRENT_LATCHES.MAR&1)
        GateMDR_result = signext((int16_t)((uint16_t)CURRENT_LATCHES.MDR >> 8),8);
    else
        GateMDR_result = signext(CURRENT_LATCHES.MDR,8);
  }

  if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    GatePC_result = CURRENT_LATCHES.PC;
  }

  if(GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    GateSSP_result = CURRENT_LATCHES.SSP;
  }

  if(GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    if (CURRENT_LATCHES.PSR_15)
      GateUSP_result = CURRENT_LATCHES.USP;
    else 
      GateUSP_result = CURRENT_LATCHES.SSP;
  }
  
  if(GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    GatePSR_result = CURRENT_LATCHES.PSR_15*32768+CURRENT_LATCHES.N*4+CURRENT_LATCHES.Z*2+CURRENT_LATCHES.P;
  }

if(GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION))
  {
   int which_one = CURRENT_LATCHES.INTsig*2+CURRENT_LATCHES.EXCsig;
   if (which_one == 3)
   {
        GateVECTOR_result = CURRENT_LATCHES.vector;
   }
   else if (which_one == 2)
   {
        GateVECTOR_result = CURRENT_LATCHES.INTV;
   }
   else if (which_one == 1)
   {
        GateVECTOR_result = CURRENT_LATCHES.vector;
   }
   else if (which_one == 0)
   {
        GateVECTOR_result = 0;
   }
  }
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       
  if(CURRENT_LATCHES.READY == 0 && GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
    BUS = 0 ;
  else
  {
    if(GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateALU_result;
    else if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateMARMUX_result;
    else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateMDR_result;
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GatePC_result;
    else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateSHF_result;
    else if (GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateSSP_result;
    else if (GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateUSP_result;
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GatePSR_result;
    else if (GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION))
      BUS = (uint16_t)GateVECTOR_result;
    else
    BUS = 0;
  }       
}


void latch_datapath_values() {
 if(CURRENT_LATCHES.READY == 0 && GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
  return;
  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       
if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION))  
{   
    if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    else if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        NEXT_LATCHES.PC = BUS;
    else if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
        NEXT_LATCHES.PC = (uint16_t)addradd_res;
    else if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
        if(CURRENT_LATCHES.EXCsig)
          NEXT_LATCHES.PC = CURRENT_LATCHES.PC - 2;
        else
          NEXT_LATCHES.PC = CURRENT_LATCHES.PC;
}
if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION))
{
    if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        NEXT_LATCHES.REGS[7] = BUS;
    else if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
        NEXT_LATCHES.REGS[(int)((unsigned int)CURRENT_LATCHES.IR >> 9)&7] = BUS;
    else if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
        NEXT_LATCHES.REGS[6] = BUS; 
}
if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION))
{
    NEXT_LATCHES.IR = BUS;
}
if(GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
  {
      NEXT_LATCHES.MDR = MEMORY[CURRENT_LATCHES.MAR>>1][0]+MEMORY[CURRENT_LATCHES.MAR>>1][1]*256;
  }
  else
    {
      if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
      {
        NEXT_LATCHES.MDR = BUS;
      }
      else
      {
        NEXT_LATCHES.MDR = (BUS&255)+(BUS&255)*256;
      }
}
}

if(GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(((CURRENT_LATCHES.IR&2048)&&CURRENT_LATCHES.N)||
  ((CURRENT_LATCHES.IR&1024)&&CURRENT_LATCHES.Z)||
  ((CURRENT_LATCHES.IR&512)&&CURRENT_LATCHES.P)
  )
    NEXT_LATCHES.BEN = 1;
  else
    NEXT_LATCHES.BEN = 0;
}

if(GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION))
{
  NEXT_LATCHES.MAR = BUS;
}

if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
  {
    MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR&255;
    MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR>>8)&255;
  }
  else 
  {
    MEMORY[CURRENT_LATCHES.MAR>>1][CURRENT_LATCHES.MAR&1] = CURRENT_LATCHES.MDR&255;
}
}

if(GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if (BUS >= 32768)
  {
    NEXT_LATCHES.N = 1;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
  }
  else if (BUS == 0)
  {
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 1;
    NEXT_LATCHES.P = 0;
  }
  else
  {
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 1;
  }
}     

if(GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
    NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP-2;
  if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP+2;
  if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
    NEXT_LATCHES.SSP = BUS;
}
if(GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(CURRENT_LATCHES.PSR_15)
  {
  if(GetUSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
    NEXT_LATCHES.USP = CURRENT_LATCHES.USP-2;
  if(GetUSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
    NEXT_LATCHES.USP = CURRENT_LATCHES.USP+2;
  if(GetUSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
    NEXT_LATCHES.USP = BUS;
  }
}
if(GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION))
{
  
    int middle;
    int priority = ((CURRENT_LATCHES.MDR&4)==0)+(CURRENT_LATCHES.PSR_15 && ((CURRENT_LATCHES.MDR&8) == 0))*2;
    int types = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    if (priority == 3)
    {
        middle = 0x04;
    }
    else if (priority == 2)
    {
        middle = 0x04;
    }
    else if (priority == 1)
    {
        middle = 0x02;
    }
    else if (priority == 0)
    {
        middle = CURRENT_LATCHES.vector;
    }
    if (types == 2)
    {
        middle = middle;
    }
    else if (types == 3)
    {
        middle = 0x03;
    }
    else if (types == 1)
    {
        middle = CURRENT_LATCHES.vector;
    }
    else if (types == 0)
    {
        middle = 0x05;
    }
    NEXT_LATCHES.vector = middle; 
}
if(GetFLIP_MODE(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        NEXT_LATCHES.PSR_15 = 0;
    }

if(GetADDRESSED(CURRENT_LATCHES.MICROINSTRUCTION))
    {
      if(CURRENT_LATCHES.EXCsig)
        NEXT_LATCHES.EXCsig = 0;
      else
        NEXT_LATCHES.INTsig = 0;
    }
if(GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if(BUS&32768)
    NEXT_LATCHES.PSR_15 = 1;
  else
    NEXT_LATCHES.PSR_15 = 0;
  if(BUS&4)
    NEXT_LATCHES.N = 1;
  else
    NEXT_LATCHES.N = 0;
  if(BUS&2)
    NEXT_LATCHES.Z = 1;
  else
    NEXT_LATCHES.Z = 0;
  if(BUS&1)
    NEXT_LATCHES.P = 1;
  else
    NEXT_LATCHES.P = 0;
}
if(GetLD_LS(CURRENT_LATCHES.MICROINSTRUCTION))
{
  if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==0)
  {
    NEXT_LATCHES.last_state = 24;
  }
  else if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==1)
  {
    NEXT_LATCHES.last_state = 23;
  }
  else if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==2)
  {
    NEXT_LATCHES.last_state = 25;
  }
  else if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==3)
  {
    NEXT_LATCHES.last_state = 29;
  }
  else if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==4)
  {
    NEXT_LATCHES.last_state = 28;
  }
  else if (GetLAST_STATE(CURRENT_LATCHES.MICROINSTRUCTION)==5)
  {
    NEXT_LATCHES.last_state = 33;
  }
}
if(GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION))
{
  NEXT_LATCHES.VA = NEXT_LATCHES.MAR;
}
if(GetSET_PTE(CURRENT_LATCHES.MICROINSTRUCTION))
{
  NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR|1;
  if(CURRENT_LATCHES.last_state == 23 || CURRENT_LATCHES.last_state == 24)
    NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR|2;
}
}

int16_t signext(int num, int bite)
{
    int base = 1;
    for (int i = 0; i<bite; i++)
    {
        base = base*2;
    }
    num = num&(base-1);
    if (num >= base/2)
        return -(base-(num)&(base-1));
    else
        return num&(base-1);
}