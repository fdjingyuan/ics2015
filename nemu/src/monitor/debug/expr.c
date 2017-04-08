#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NEQ, AND, OR, DEREF, REG, INT_d, INT_x

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
	{"!", '!'},
	{"\\(", '('},
	{"\\)", ')'},
	{"\\$[a-z]+", REG},				//register	eg:$eax
	{"[0-9]+", INT_d},				//decimal number
	{"0[xX][0-9a-fA-F]+", INT_x}			//hexadecimal number

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
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
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:break;
					case INT_x:
						assert(substr_len<32);
						tokens[nr_token].type = rules[i].token_type;
						//copy substr_len byte to the memory that tokens indicate
						memcpy(tokens[nr_token].str,substr_start,substr_len);
						nr_token++;
						break;
					case INT_d:
						assert(substr_len<32);
						tokens[nr_token].type = rules[i].token_type;
						memcpy(tokens[nr_token].str,substr_start,substr_len);
						nr_token++;
						break;
					case REG:
						tokens[nr_token].type = rules[i].token_type;
						memcpy(tokens[nr_token].str,substr_start+1,substr_len-1);
						nr_token++;
						break;
					case '*'://determine whether the * represent pointer dereference
						if(nr_token==0|| (tokens[nr_token-1].type!=INT_d && tokens[nr_token-1].type!=INT_x && tokens[nr_token-1].type!=')'))
						{
								tokens[nr_token].type=DEREF;
								nr_token++;
								break;
						}
					case '+':
					case '-':
					case '/':
					case '(':
					case ')':
					case EQ:
					case NEQ:
					case AND:
					case OR:
					case '!':
						 tokens[nr_token].type = rules[i].token_type;
						 nr_token++;
						 break;

					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
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
	panic("please implement me");
	return 0;
}

