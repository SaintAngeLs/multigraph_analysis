// wiki

#include <cstddef>
#include <cassert>



enum color_t { BLACK, RED };

struct RBnode {     
  RBnode* parent;   
  RBnode* child[2]; 
    
    
    
  enum color_t color;
  int key;
};

#define NIL   NULL 
#define LEFT  0
#define RIGHT 1
#define left  child[LEFT]
#define right child[RIGHT]

struct RBtree { 
  RBnode* root; 
};



#define childDir(N) ( N == (N->parent)->right ? RIGHT : LEFT )

RBnode* RotateDirRoot(
    RBtree* T,   
    RBnode* P,   
    int dir) {   
  RBnode* G = P->parent;
  RBnode* S = P->child[1-dir];
  RBnode* C;
  assert(S != NIL); 
  C = S->child[dir];
  P->child[1-dir] = C; if (C != NIL) C->parent = P;
  S->child[  dir] = P; P->parent = S;
  S->parent = G;
  if (G != NULL)
    G->child[ P == G->right ? RIGHT : LEFT ] = S;
  else
    T->root = S;
  return S; 
}

#define RotateDir(N,dir) RotateDirRoot(T,N,dir)
#define RotateLeft(N)    RotateDirRoot(T,N,LEFT)
#define RotateRight(N)   RotateDirRoot(T,N,RIGHT)

void RBinsert1(
  RBtree* T,         
  struct RBnode* N,  
  struct RBnode* P,  
  int dir)           
{
  struct RBnode* G;  
  struct RBnode* U;  

  N->color = RED;
  N->left  = NIL;
  N->right = NIL;
  N->parent = P;
  if (P == NULL) {   
    T->root = N;     
    return; 
  }
  P->child[dir] = N; 
  
  do {
    if (P->color == BLACK) {
      
      return; 
    }
    
    if ((G = P->parent) == NULL)
      goto Case_I4; 
    
    dir = childDir(P); 
    U = G->child[1-dir]; 
    if (U == NIL || U->color == BLACK) 
      goto Case_I56; 
    
    P->color = BLACK;
    U->color = BLACK;
    G->color = RED;
    N = G; 
    
    
  } while ((P = N->parent) != NULL);
  
  
  
  return; 
Case_I4: 
  P->color = BLACK;
  return; 
Case_I56: 
  if (N == P->child[1-dir])
  { 
    RotateDir(P,dir); 
    N = P; 
    P = G->child[dir]; 
    
  }

  RotateDirRoot(T,G,1-dir); 
  P->color = BLACK;
  G->color = RED;
  return; 
} 
void RBdelete2(
  RBtree* T,         
  struct RBnode* N)  
 {
  struct RBnode* P = N->parent;  
  int dir;          
  struct RBnode* S;  
  struct RBnode* C;  
  struct RBnode* D;  

  
  dir = childDir(N); 
  
  P->child[dir] = NIL;
  goto Start_D;      

  
  do {
    dir = childDir(N);   
Start_D:
    S = P->child[1-dir]; 
    D = S->child[1-dir]; 
    C = S->child[  dir]; 
    if (S->color == RED)
      goto Case_D3;                  
    
    if (D != NIL && D->color == RED) 
      goto Case_D6;                  
    if (C != NIL && C->color == RED) 
      goto Case_D5;                  
    
    if (P->color == RED)
      goto Case_D4;                  

  return; 
 
    S->color = RED;
    N = P; 
    
    
  } while ((P = N->parent) != NULL);
  
  
Case_D3: 
  RotateDirRoot(T,P,dir); 
  P->color = RED;
  S->color = BLACK;
  S = C; 
  
  D = S->child[1-dir]; 
  if (D != NIL && D->color == RED)
    goto Case_D6;      
  C = S->child[  dir]; 
  if (C != NIL && C->color == RED)
    goto Case_D5;      
  
  
Case_D4: 
  S->color = RED;
  P->color = BLACK;
  return; 
Case_D5: 
  RotateDir(S,1-dir); 
  S->color = RED;
  C->color = BLACK;
  D = S;
  S = C;
  
  
Case_D6: 
  RotateDirRoot(T,P,dir); 
  S->color = P->color;
  P->color = BLACK;
  D->color = BLACK;
  return; 
} 

