#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../../../include/memory/vaddr.h"

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//add
static int cmd_s(char *args);
static int info(char *args);
//static int cmd_x(char *args);


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "s", "Step", cmd_s},
  { "info", "info regs", info},
//  { "x N EXPR", "x", cmd_x} ,

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

//add
static int cmd_s(char *args){
	int n;
	if( args == NULL)
		n = 1;
	else{
		char *arg = strtok(NULL," ");
		n = atoi(arg);
	}
	cpu_exec(n);
	return 0;
}

static int info (char *args){

	char *arg = strtok(NULL, " ");
	if(!strcmp(arg,"r"))
		isa_reg_display();
//	else if(!strcmp(arg,"w"))
//		isa_

/*	if (args[0] == 'r'){
		int i;
		for(i = R_EAX; i <= R_EDI; i++){
			printf("$%s\t0x%08x\n"),regsl[i], reg_l(i));
		}
		printf("$eip\t0x%08x\n", cpu.eip);
	}
*/
	return 0;
}


/*
static int cmd_x(char *args){
	if(args == NULL){
		printf("Wrong\n");
		return 0;
	}
	int num,exprs;
	sscanf(args, "%d%x", &num,&exprs);
	int i;
	for(i=0;i<num;i++){
		printf("0x%8x   0x%x\n",exprs + i*32, vaddr_read(exprs + i*32,32));
	}
	return 0;



	char *arg1 = strtok(NULL," ");
	int n = atoi(arg1);
	char *arg2 = strtok(NULL," ");
	int expr = 0;
	

}
*/




static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
