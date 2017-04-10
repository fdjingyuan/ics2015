#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *free_;

void init_wp_pool() {
	int i;
	for(i = 1; i < NR_WP+1; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(){
	//extract a new watchpoint from free_(not used)
	WP*temp;
	WP*temp2=head;
	if(free_==NULL)
		assert(0);//no space for more watchpoint
	else
	{
		temp=free_;
		free_=free_->next;
	}
	//add this watchpoint to head(used)
	if(temp2==NULL)//there is no watchpoint before	
	{
		head=temp;
		head->next=NULL;
	}
	else
	{
		while(temp2->next!=NULL)
			temp2=temp2->next;
		temp2->next=temp;
		temp->next=NULL;
	}

	return temp;
}
	

void free_wp(WP* wp){
	//extract watchpoint from head to free_
	WP*temp=head;
	WP*temp2=free_;
	if(temp2==NULL)
	{
		free_=wp;
		free_->next=NULL;
	}
	else
	{
		while(temp2->next!=NULL)
			temp2=temp2->next;
		temp2->next=wp;
		wp->next=NULL;
	}

	if(temp==wp)
		head=NULL;
	else
	{
		while(temp->next!=wp)
			temp=temp->next;
		temp->next=temp->next->next;
	}
}
	







