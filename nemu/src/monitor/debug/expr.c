#include "nemu.h"
#include "common.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, 
	EQ,NEQ, AND, OR, 
	DEREF,NEG,NOT,
	REG, INT_d, INT_x,


	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},					// equal
	{"!=", NEQ},
	{"-", '-'},					//subtract or negative 
	{"\\*", '*'},					//mutiply or address
	{"/", '/'},					//divide
	{"&&", AND},
	{"\\|\\|", OR},
	{"!", NOT},
	{"-", NEG},					//negative
	{"\\*", DEREF},					//pointer derefence
	{"\\(", '('},
	{"\\)", ')'},
	{"\\$[a-z]+", REG},				//register	eg:$eax
	{"0x[0-9a-fA-F]+", INT_x},			//hexadecimal number
	{"[0-9]+", INT_d},				//decimal number

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];
uint32_t eval(uint32_t p,uint32_t q);
bool check_parentheses(uint32_t p,uint32_t q);
uint32_t dominOp(uint32_t p, uint32_t q);


/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];//store the mistaken information
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		//int regcomp(regex_t *compiled,const char*pattern, int cflags)
		//function:compile the specified regular expression "pattern" to a data formate "complied"
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);//if true, return 0
		if(ret != 0) {
			//when regcomp or regexec makes mistakes, use regerror to return a string consists of mistaken information
			//size_t regerrot(int errcode, regex_t*compiled, char*buffer,size_t length)
			regerror(ret, &re[i], error_msg, 128);
			//void assert(int expression)
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;//record the type of token
	char str[32];//record the corresponding substring of token
} Token;

//the array is to store the information of token that has been recognized in order
Token tokens[32];
//indicates the number of tokens that have been identified 
int nr_token;

//to identify the token in expression
static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
				//debug:printf("i=%d,NR_REGEX=%lu\n",i,NR_REGEX);
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				//debug:printf("start=%c,len=%d\n",*substr_start,substr_len);

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:break;
					case INT_x:
						tokens[nr_token].type = rules[i].token_type;
						//copy substr_len byte to the memory that tokens indicate
						memcpy(tokens[nr_token].str,substr_start,substr_len);
						//debug:printf("tokens[nr_token].str=%s",tokens[nr_token].str);
						nr_token++;
						break;
					case INT_d:
						assert(substr_len<32);
						tokens[nr_token].type = rules[i].token_type;
						memcpy(tokens[nr_token].str,substr_start,substr_len);
						//debug:printf("tokens[nr_token].str=%s",tokens[nr_token].str);
						nr_token++;
						break;
					case REG:
						tokens[nr_token].type = rules[i].token_type;
						//remove $
					  memcpy(tokens[nr_token].str,substr_start+1,substr_len-1);
						nr_token++;
						break;
					case '*'://determine whether the * represent pointer dereference
						if(nr_token==0 || ((tokens[nr_token-1].type!=INT_d) && (tokens[nr_token-1].type!=INT_x) && (tokens[nr_token-1].type!=')')))
						{
								tokens[nr_token].type=DEREF;
								nr_token++;
								break;
						}
					case '-':
						if(nr_token==0 || ((tokens[nr_token-1].type!=INT_d) && (tokens[nr_token-1].type!=INT_x) && (tokens[nr_token-1].type!=')')))
						{
								tokens[nr_token].type=NEG;
								nr_token++;
								break;
						}	
					case '+':
					case '/':
					case '(':
					case ')':
					case EQ:
					case NEQ:
					case AND:
					case OR:
					case NOT:
						 tokens[nr_token].type = rules[i].token_type;
						 nr_token++;
						 break;

					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");//???
			return false;
		}
		assert(nr_token<32);//the limit number of token
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	return eval(0,nr_token-1);
}

uint32_t eval(uint32_t p,uint32_t q){
	if (p>q)
		assert(0);//bad expression

	//single token:return the value of the number
	else if(p==q)
	{
		uint32_t n;
		if(tokens[q].type==INT_x)
			n = strtoull(tokens[q].str,NULL,16);
		else if(tokens[q].type==INT_d) 
			sscanf(tokens[q].str, "%d", &n);
		else
		{
			if(strcmp("eip",tokens[q].str)==0)
				return cpu.eip;
			int i=0;
			for(;i<8;i++)
			{
				if(strcmp(*(regsl+i),tokens[q].str)==0)
				{
					return reg_l(i);
				}
			}
			panic("error:reg cannot be find\n");
		}
		return n;
	}
	//the expression is surrounded by a matched pair of parentheses
	else if(check_parentheses(p,q) == true)
		return eval(p+1,q-1);//just throw the parentheses
	else if(tokens[p].type == NEG)
		{
		printf("eval=%d\n",-eval(p+1,q));
		return -eval(p+1,q);
	}
	//NOT
	else if(tokens[p].type == NOT)
		return !eval(p+1,q);
	//*:extract contents that the pointer point to 
	else if(tokens[p].type == DEREF)
		return swaddr_read(eval(p+1,q),4);
	//general caculation:using recursive caculation
	else
	{
		uint32_t op=dominOp(p,q);//get the dominate operator
		printf("op=%d type=%d\n",op,tokens[op].type);
		uint32_t val1=eval(p,op-1);
		uint32_t val2=eval(op+1,q);
		printf("val1=%d val2=%d\n",val1,val2);

		switch(tokens[op].type)
		{
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			case EQ:return val1==val2;
			case NEQ:return val1!=val2;
			case AND:return val1&&val2;
			case OR:return val1||val2; 
			default:assert(0);
		}
	}
}


bool check_parentheses(uint32_t p,uint32_t q){
	int pars=0;
	if(tokens[p].type!='(' || tokens[q].type!=')')
			return false;
	while(p<=q)
	{
		if(pars<0)
			return false;//')' is more than '('
		if(tokens[p].type=='(')
			pars++;
		if(tokens[p].type==')')
			pars--;
		
		if(pars==0 && p!=q)
			return false;//not the outermost pair of parentheses
		p++;
	}
	if(pars == 0)
		return true;

	return false;//'(' is more than ')'
}

uint32_t dominOp(uint32_t p, uint32_t q){
	//  NEG:2 / *:3; +,-:4; ==,!=:7; &&:11; ||:12; 	
	int pars=0;
	int op=p;
	int pri=0;
	for(;p<=q;p++)
	{
		//skip numbers, register and monocular operator
		if(tokens[p].type == INT_d || tokens[p].type == INT_x || tokens[p].type == NOT || tokens[p].type == DEREF || tokens[p].type == REG || tokens[p].type == NEG)
			continue;
		else if(tokens[p].type == '(')
		{
			pars++;
			p++;
			while(pars!=0)
			{
				if(tokens[p].type == '(')
					pars++;
				else if(tokens[p].type == ')')
					pars--;
				p++;
			}
			p--;
		}
		else if(tokens[p].type == NEG)
		{
			if(pri<=2)
			{
				op=p;
				pri=2;
			}
		}
		else if(tokens[p].type == '/' || tokens[p].type =='*')
		{
			if(pri<=3)
			{
				op=p;
				pri=3;
			}
		}
		else if(tokens[p].type == '+' || tokens[p].type =='-')
		{
			if(pri<=4)
			{
				op=p;
				pri=4;
			}
		}
		else if(tokens[p].type == EQ || tokens[p].type ==NEQ)
		{
			if(pri<=7)
			{
				op=p;
				pri=7;
			}
		}
		else if(tokens[p].type == AND)
		{
			if(pri<=11)
			{
				op=p;
				pri=11;
			}
		}
		else if(tokens[p].type == OR)
		{
			if(pri<=12)
			{
				op=p;
				pri=12;
			}
		}
	}
	return op;
	

}






























