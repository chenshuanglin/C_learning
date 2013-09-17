/*
 * 下面这些代码除了结构体numlist不是使用inux/list.h中的内容
 * 其他都是复制粘帖过来的
 *
 */

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)


#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member))) 
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)


struct list_head{
	struct list_head *next,*prev;
};


void __list_add(struct list_head *new,  struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
 }


 void INIT_LIST_HEAD(struct list_head *list)
 {
	 list->next = list;
	 list->prev = list;
 }


void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

void __list_del(struct list_head * prev,struct list_head * next)
{
	next->prev=prev;
	prev->next=next;
}
void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}


