#include "rbtree.h"


#include <stdlib.h>
#include <stdio.h>

static void left_rotate(rbtree *t, node_t *x);
static void right_rotate(rbtree *t, node_t *x);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));  // 트리 구조체 만들기
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t)); // nil 노드 만들기
  nil_node->color = RBTREE_BLACK;
  nil_node->left = nil_node;
  nil_node->right = nil_node;
  nil_node->parent = nil_node;

  p->root = nil_node;   // 루트도 nil로 초기화
  p->nil = nil_node;    // 트리에 nil 노드 등록

  return p;
}

// (정리) 후위순회로 트리의 모든 노드를 메모리 해제
static void delete_node(node_t *node, node_t *nil) {
  if (node == nil) return;
  // 왼쪽, 오른쪽 자식부터 먼저 해제
  delete_node(node->left, nil);
  delete_node(node->right, nil);
  // 자기 자신 해제
  free(node);
}

// (정리) 트리 전체 및 nil 노드까지 해제
void delete_rbtree(rbtree *t) {
  delete_node(t->root, t->nil); // 모든 노드 해제
  free(t->nil);                 // nil 노드 해제
  free(t);                      // 트리 구조체 해제
}

static void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right; // x의 오른쪽 자식을 y로 설정

  x->right = y->left; // y의 왼쪽 서브트리를 x의 오른쪽 자식으로 이동

  if (y->left != t->nil) {
    y->left->parent = x; // y의 왼쪽 자식이 nil이 아니면 그 부모를 x로 설정
  }

  y->parent = x->parent; // y의 부모를 x의 부모로 갱신

  if (x->parent == t->nil) {
    t->root = y; // x가 루트였으면 트리의 루트를 y로 갱신
  } else if (x == x->parent->left) {
    x->parent->left = y; // x가 왼쪽 자식이면 해당 포인터를 y로
  } else {
    x->parent->right = y; // 오른쪽 자식이면 해당 포인터를 y로
  }

  y->left = x; // y의 왼쪽 자식으로 x 설정
  x->parent = y; // x의 부모를 y로 설정
}

static void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left; // x의 왼쪽 자식을 y로 설정

  x->left = y->right; // y의 오른쪽 서브트리를 x의 왼쪽 자식으로 이동

  if (y->right != t->nil) {
    y->right->parent = x; // y의 오른쪽 자식이 nil이 아니면 그 부모를 x로 설정
  }

  y->parent = x->parent; // y의 부모를 x의 부모로 갱신

  if (x->parent == t->nil) {
    t->root = y; // x가 루트였으면 트리의 루트를 y로 갱신
  } else if (x == x->parent->right) {
    x->parent->right = y; // x가 오른쪽 자식이면 해당 포인터를 y로
  } else {
    x->parent->left = y; // 왼쪽 자식이면 해당 포인터를 y로
  }

  y->right = x; // y의 오른쪽 자식으로 x 설정
  x->parent = y; // x의 부모를 y로 설정
}



void insert_fixup(rbtree *t, node_t *z) {
  // 삽입 후 레드-블랙 트리의 속성을 복구하는 함수
  // z: 삽입된 노드

  // 부모가 RED인 경우에만 속성이 깨질 수 있으므로 반복
  while (z->parent->color == RBTREE_RED) {

    // 부모가 조부모의 왼쪽 자식인 경우
    if (z->parent == z->parent->parent->left) {
      node_t *y = z->parent->parent->right; // 삼촌 노드

      // case 1: 삼촌이 RED인 경우
      // 부모와 삼촌을 BLACK으로, 조부모를 RED로 변경 후 z를 조부모로 올림
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        // case 2: 삼촌이 BLACK이고, z가 부모의 오른쪽 자식인 경우
        // 왼쪽 회전하여 case 3으로 변환
        if (z ==  z->parent->right) {
          z = z->parent;
          left_rotate(t, z);
        }

        // case 3: 삼촌이 BLACK이고, z가 부모의 왼쪽 자식인 경우
        // 부모를 BLACK, 조부모를 RED로 바꾼 후 조부모 기준으로 오른쪽 회전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    // 부모가 조부모의 오른쪽 자식인 경우 (대칭)
    else {
      node_t *y = z->parent->parent->left; // 삼촌 노드

      // case 1: 삼촌이 RED인 경우
      // 부모와 삼촌을 BLACK으로, 조부모를 RED로 변경 후 z를 조부모로 올림
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        // case 2: 삼촌이 BLACK이고, z가 부모의 왼쪽 자식인 경우
        // 오른쪽 회전하여 case 3으로 변환
        if (z ==  z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }

        // case 3: 삼촌이 BLACK이고, z가 부모의 오른쪽 자식인 경우
        // 부모를 BLACK, 조부모를 RED로 바꾼 후 조부모 기준으로 왼쪽 회전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  // 루트 노드는 항상 BLACK이어야 하므로 색상 보정
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 새 노드 생성 및 초기화
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  if (z == NULL) {
      return NULL;
  }
  z->key = key;
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;

  // 트리 탐색을 위한 포인터 초기화
  node_t *x = t->root;
  node_t *y = t->nil;

  // 삽입할 위치 탐색
  while (x != t->nil) {
    y = x;
    if (key == x->key) {
      free(z);            // 새로 만든 중복 노드 해제
      return x;           // 기존 노드 반환
    }
    else if (key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  // 부모 설정 및 삽입
  z->parent = y;

  if (y == t->nil) {
    t->root = z;         // 트리가 비어 있던 경우
  } else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  insert_fixup(t, z);    // 삽입 후 레드-블랙 트리 속성 복구
  return z;              // 삽입된 노드 반환
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // 루트부터 탐색 시작
  node_t *x = t->root;

  // nil 노드에 도달하거나 키를 찾을 때까지 반복
  while (x != t->nil) {
    // 키가 일치하면 해당 노드 반환
    if (x->key == key) {
      return x;
    }
    // 키가 현재 노드보다 작으면 왼쪽 서브트리로 이동
    else if (key < x->key) {
      x = x->left;
    }
    // 키가 현재 노드보다 크면 오른쪽 서브트리로 이동
    else {
      x = x->right;
    }
  }
  // 찾지 못하면 nil 노드 반환
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  // 가장 왼쪽 노드까지 이동 (최솟값)
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  // 가장 오른쪽 노드까지 이동 (최댓값)
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
