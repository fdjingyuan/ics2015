#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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

//the statement of the command
static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);
/*statement of other command
static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_bt(char *args);
*/
 
 //the definition of the cmd_table
static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Single step the instruction", cmd_si },
	{ "info", "Print regInfo or watchPointInfo", cmd_info },
	{ "x", "Scan memory", cmd_x },
	/*other command
	{ "p", "Caculate and print expresstion", cmd_p },
	{ "w", "Set new watchpoint", cmd_w },
	{ "d", "Delete watchpoint", cmd_d },
	{ "bt", "Display the stack frame chain", cmd_bt },
	*/
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

//realizations of the command

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args){
		//split the input string
		char *arg=strtok(NULL," ");
		//the number of the single step  
		int n;

		//the default is 1 when N is not given	
		if(arg==NULL)
		  cpu_exec(1);
		else{
		  if(sscanf(arg,"%d",&n)==0){
		  //sscanf:if true,return the number of parameters
				printf("Invalid number\n");
		  }
			else if(n<=0){
				printf("Please input a positive number\n");
				return 0;
			}
		  else if(n>10){
				printf("the single step cannot exceed 10\n");
				return 0;
			}
			else{
				cpu_exec(n);
		  }
  }
		return 0;
}

static int cmd_info(char *args){
		char *arg=strtok(NULL," ");
		//forget to input r or w
		if(arg==NULL)
		{
		  printf("please input r or w after info\n");
		  return 0;
		}

		//print the state of the register
		if(strcmp(arg,"r")==0)
		{
			int i;
			//eip
			printf("eip\t%x\n",cpu.eip);
			//32bit register
			for(i=0;i<8;i++){
				printf("%s:\t0x%x\t%d\n",regsl[i],reg_l(i),reg_l(i));}
		  printf("\n");
			//16bit register
			for(i=0;i<8;i++){
				printf("%s:\t0x%x\t%d\n",regsw[i],reg_w(i),reg_w(i));}
		  printf("\n");
			//8bit register
			for(i=0;i<4;i++){
		    printf("%s:\t0x%x\t%d\t",regsb[i|4],reg_b(i|4),reg_b(i|4));
				printf("%s:\t0x%x\t%d\n",regsb[i],reg_b(i),reg_b(i));}			
		}
		/*//print the information of the watchpoint
		else if(strcmp(arg,"w")==0)
		{
		  WP* pHead=head;
			while(pHead!=NULL){
				printf("watchpoint NO.%d, expr is %s\n",pHead->NO,pHead->str)
				pHead=pHead->next;
			}
		}*/
		else 
		{		  
		  printf("Invalid Instruction\n");
		}
	return 0;
}

static int cmd_x(char* args){
  char* arg = strtok(NULL," ");
	int ins=0;
	if(arg == NULL){
		printf("Please input length\n");
		return 0;
	}
	else if(sscanf(arg,"%d",&ins)==0){
		printf("Invalid number\n");
		return 0;
	}
	else{
		char* addr = strtok(NULL," ");
		if(addr == NULL){
			printf("Please input start address\n");
			return 0;
		}
		else{
			swaddr_t start_addr;
			sscanf(addr,"%x",&start_addr);
			int i;
			for(i=0;i<ins;i++){
				printf("%d\t0x%08x:0x%08x\n",i+1,start_addr,swaddr_read(start_addr,4));
				start_addr+=4;
			}
		}
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
