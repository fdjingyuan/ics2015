#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "cpu/reg.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "monitor/elf.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

//define a new struct to describe the stack
//every member should be read by function swaddr_read()
typedef struct{
	//next point: address of user
	swaddr_t prev_ebp;
	//address of return 
	swaddr_t ret_addr;
	//the parameter of function
	uint32_t args[4];
}PartOfStackFrame;


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

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_bt(char *args);

static int cmd_d(char *args);

static int cmd_b(char *args);



 
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
	{ "p", "Caculate and print expresstion", cmd_p },
	{ "w", "Set new watchpoint", cmd_w },
	{ "bt", "Print the stack frame chain", cmd_bt},
	{ "d", "Delete watchpoint", cmd_d },
	{ "b", "Set new breakpoint", cmd_b },
	
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
		if(strcmp(arg,"r")==0)//equal:return 0;<:return negative number;>:return positive number
		{
			int i;
			//eip
			printf("eip\t%x\n",cpu.eip);
			//32bit register
			for(i=0;i<8;i++){
				printf("%s:\t0x%08x\t%d\n",regsl[i],reg_l(i),reg_l(i));}
		  printf("\n");
			//16bit register
			for(i=0;i<8;i++){
				printf("%s:\t0x%04x\t%d\n",regsw[i],reg_w(i),reg_w(i));}
		  printf("\n");
			//8bit register
			for(i=0;i<4;i++){
		    printf("%s:\t0x%02x\t%d\t",regsb[i|4],reg_b(i|4),reg_b(i|4));
				printf("%s:\t0x%02x\t%d\n",regsb[i],reg_b(i),reg_b(i));}			
		}
		//print the information of the watchpoint
		else if(strcmp(arg,"w")==0)
		{
			bool succ=true;
			bool* psucc=&succ;		  
			WP* pHead=head;
			if(pHead==NULL)
				printf("No watchpoint now!You can add one.\n");
			while(pHead!=NULL){
				printf("%s NO.%d, expr: %s=%u\t0x%x\n",pHead->type,pHead->NO,pHead->str,expr(pHead->str,psucc),expr(pHead->str,psucc));
				pHead=pHead->next;
			}
		}
		else 		  
			printf("Invalid Instruction\n");
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
			bool succ=true;
			bool* psucc=&succ;
			swaddr_t expression;
			expression=expr(addr,psucc);
			//printf("0x%08x:\t",expression);
			int i;
			for(i=0;i<ins;i++){
				printf("%d\t0x%08x:0x%08x\n",i+1,expression,swaddr_read(expression,4));
				expression+=4;
			}
		}
	}
	return 0;
}

static int cmd_p(char* args){
	char* arg = strtok(NULL," ");
	bool succ=true;
	bool* psucc=&succ;
	uint32_t result=expr(arg,psucc);
	printf("%s=%u\t0x%x\n",arg,result,result);
	return 0;
}


static int cmd_w(char* args){
	char* arg=strtok(NULL," ");
	bool succ=true;
	bool* psucc=&succ;
	if(arg==NULL)
		printf("please input the expression\n");
	else{
		WP* pHead=new_wp();
		pHead->result=expr(arg,psucc);
		strcpy(pHead->str,arg);
		strcpy(pHead->type,"watchpoint");
		printf("Set watchpoint NO:%d, expr:%s=%u\t0x%x\n",pHead->NO,pHead->str,pHead->result,pHead->result);
	}
	return 0;
}


static int cmd_bt(char* args){
	PartOfStackFrame cur_ebp;
	swaddr_t addr=reg_l(R_EBP);
	cur_ebp.ret_addr=addr;
	
	int j=0;
	while(addr > 0)
	{
		//print the begining address
		printf("#%d	0x%08x in ",j++, cur_ebp.ret_addr);
		
		printf("call flags\n");
		int flag=search_addr(cur_ebp.ret_addr);

		//read PartofStackFrame member
		current_sreg = R_SS;
		printf("current_sreg\n");
		//read current address
		cur_ebp.prev_ebp = swaddr_read(addr,4);
		//read return address
		cur_ebp.ret_addr = swaddr_read(addr+4,4);
		int i;
		//read four parameter
		for(i=0;i<5;i++)
		{
			cur_ebp.args[i] = swaddr_read(addr+8+4*i,4);
		}
		printf("read\n");
		if (flag == 1)
			printf ("( )\n");
		else 
			printf ("( %d , %d , %d , %d )\n", cur_ebp.args[0],cur_ebp.args[1],cur_ebp.args[2],cur_ebp.args[3]);
		printf("printf\n");
		addr = cur_ebp.prev_ebp;
	}
	return 0;
}


static int cmd_d(char* args){
	char* arg=strtok(NULL," ");
	int NO;
	if(arg==NULL){//delete all watchpoint
		WP* temp=head;
			while(temp!=NULL)
			{
				free_wp(temp);
				printf("watchpoint NO:%d has been deleted\n",temp->NO);
				temp=temp->next;
			}
		printf("all watchpoint has been deleted\n");
	}
	else{
		if(sscanf(arg,"%d",&NO)==0)
			printf("please input correct number\n");
		else{
			WP* temp=head;
			while(temp!=NULL)
			{
				if(temp->NO==NO)
				{
					free_wp(temp);
					printf("watchpoint NO:%d has been deleted\n",temp->NO);
					break;
				}
				temp=temp->next;
			}
			if(temp==NULL)
				printf("don't have NO:%d in watchpoint pool\n",NO);
		}
	}
	
	return 0;
}


static int cmd_b(char* args){
	char* arg=strtok(NULL," ");
	bool succ=true;
	bool* psucc=&succ;
	if(arg==NULL)
		printf("please input address.\n");
	else
	{
		WP* pHead=new_wp();
		char *eip="$eip==";
		char *str=malloc(strlen(eip)+strlen(arg));
		strcpy(str,eip);	
		strcat(str,arg);
		strcpy(pHead->str,str);
		pHead->result=expr(str,psucc);
		strcpy(pHead->type,"breakpoint");
		printf("Set breakpoint NO:%d at %s\n",pHead->NO,arg);
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
