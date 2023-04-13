#ifndef __CHILD_COLLECTOR_H__
#define __CHILD_COLLECTOR_H__

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

typedef struct child_node {
  struct child_node *next;
  pid_t pid;
} child_node_t;

child_node_t * root = NULL;


void child_add(pid_t pid) {
  if(root == NULL) {
    root = (child_node_t*)malloc(sizeof(child_node_t));
    root->next = NULL;
    root->pid = pid;
    return;
  }

  child_node_t *it = root;
  while(it->next != NULL) {
    it = it->next;
  }

  it->next = (child_node_t*)malloc(sizeof(child_node_t));
  it->next->next = NULL;
  it->next->pid = pid;
}

void child_kill(int sig) {
  child_node_t *it = root;
  while(it != NULL) {
    kill(it->pid, sig);
    it = it->next;
  }
}

void child_remove(pid_t pid) {
  if(root == NULL) return;

  if(root->pid == pid) {
    child_node_t* tmp = root->next;
    free(root);
    root = tmp;
    return;
  }

  child_node_t* it = root;

  while(it->next != NULL) {
    if(it->next->pid == pid) {
      child_node_t* after = it->next->next;
      free(it->next);
      it->next = after;
      return;
    }
  }
}

void child_clean() {
  while(root != NULL) {
    child_node_t* tmp = root->next;
    free(root);
    root = tmp;
  }
}

#endif // __CHILD_COLLECTOR_H__