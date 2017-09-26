#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_, *tail, *free_tail;

extern uint32_t expr(char *e, bool *success);

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  tail = NULL;
  free_ = wp_pool;
  free_tail = &wp_pool[NR_WP - 1];
}

WP* new_wp() {
  WP* retval = free_;
  
  if (free_ != NULL) {
    if (free_ == free_tail)
      free_ = free_tail = NULL;
    else
      free_ = free_->next;
  }
  return retval;
}

void free_wp(WP *wp) {
  wp->next = NULL;
  wp->NO = -1;
  if(free_tail != NULL)
    free_tail->next = wp;
  else
    free_ = wp;
  free_tail = wp;
}

void create_wp(char* expr, int val) {
  WP* wp = new_wp();
  if (wp == NULL) {
    printf("Exception: You have reached the maximum amount of watchpoints. Cannot assign new watchpoint.\n");
    return;
  }
  
  wp->next = NULL;
  strcpy(wp->expr, expr);
  wp->val = val;
  if (tail != NULL) {
    wp->NO = tail->NO + 1;
    tail->next = wp;
    tail = wp;
  }
  else {
    wp->NO = 0;
    head = tail = wp;
  }
}

void remove_wp(int NO) {
  WP* current = head;
  WP* inum;

  if (tail == NULL || tail->NO < NO || NO < 0) {
    printf("Exception: Watchpoint number out of range.\n");
    return;
  }
  
  if (NO == 0){
    if (head == tail) {
      free_wp(head);
      head = tail = NULL;
      printf("Watchpoint number %d has been deleted.\n", NO);
      return;
    }
    inum = head->next;
    free_wp(head);
    head = inum;
    while (inum != NULL) {
      inum->NO -= 1;
      inum = inum->next;
    }
    printf("Watchpoint number %d has been deleted.\n", NO);
    return;
  }
  
  while (current->next != NULL) {
    if (current->next->NO == NO) {
      if (tail == current->next) {
        tail = current;
        free_wp(current->next);
        current->next = NULL;
        printf("Watchpoint number %d has been deleted.\n", NO);
        return;
      }
      
      inum = current->next->next;
      free_wp(current->next);
      current->next = inum;
      while (inum != NULL) {
        inum->NO -= 1;
        inum = inum->next;
      }
      printf("Watchpoint number %d has been deleted.\n", NO);
      return;
    }
    current = current->next;
  }
  printf("Watchpoint number %d has been deleted.\n", NO);
  return;
}

void print_wp() {
  WP* inum = head;
  
  if (head == NULL) {
    printf("No watchpoint.\n");
    return;
  }
  
  printf("There %s %d watchpoint%s.\nNO\tExpression\n", ((tail->NO == 0) ? "is" : "are"), tail->NO + 1, ((tail->NO == 0) ? "" : "s"));
  while(inum != NULL) {
    printf("%d\t%s\n", inum->NO, inum->expr);
    inum = inum->next;
  }
}

bool eval_wp() {
  WP* inum = head;
  bool success;
  bool retval = false;
  int val;
  
  while (inum != NULL) {
    val = expr(inum->expr, &success);
    if (inum->val != val) {
      printf("Hit watchpoint number %d, value changed from %d to %d.\n", inum->NO, inum->val, val);
      inum->val = val;
      retval = true;
    }
    inum = inum->next;
  }
  return retval;
}


