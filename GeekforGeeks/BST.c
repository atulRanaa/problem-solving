#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

typedef struct Node 
{
    int data;
    struct Node *left;
    struct Node *right;
} Node;
struct QNode{
    Node *ptr;
    struct QNode *next;
};
struct Queue{
    struct QNode *front, *rear;
};
int insert_leaf_node(int data, Node **parent_ptr) 
{
  Node *node;
  if ((node = malloc(sizeof *node)) == NULL) return -1;
  node->data = data;
  node->left = NULL;
  node->right = NULL; 
  *parent_ptr = node;
  return 0;
}
int insert_node(int data, Node **parent_ptr) 
{
    Node *node=*parent_ptr;

    if(node==NULL) return insert_leaf_node(data, parent_ptr);
    else if(data<=node->data) return insert_node(data, &node->left);
    else return insert_node(data, &node->right);
}

void preorder(Node *bst){
   if(bst!=NULL){
     printf("%d ",bst->data);
     preorder(bst->left);
     preorder(bst->right);
   }
}
void inorder(Node *bst){
  if(bst!=NULL){
    inorder(bst->left);
    printf("%d ",bst->data);
    inorder(bst->right);
  }
}
void postorder(Node *bst){
  if(bst!=NULL){
   postorder(bst->left);
   postorder(bst->right);
   printf("%d ",bst->data);
  }
}
struct QNode* newNode(Node *nptr){
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->ptr =nptr;
    temp->next = NULL;
    return temp;
}
struct Queue *createQueue(){
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}
void enQueue(struct Queue *q,Node *ptr){
    struct QNode *temp = newNode(ptr);
    if (q->rear == NULL){
       q->front = q->rear = temp;
       return;
    }
    q->rear->next = temp;
    q->rear = temp;
}
struct QNode *deQueue(struct Queue *q)
{
    
    if (q->front == NULL) return NULL;  
    
    struct QNode *temp = q->front;
         q->front = q->front->next;
         
    if (q->front == NULL) q->rear = NULL;
    return temp;
}
 
void levelorder(Node *bst){
  struct Queue *q= createQueue();
  struct QNode *temp;
  enQueue(q,bst);
  while(1){
    temp=deQueue(q);
           if(temp==NULL) break;
    Node *t=temp->ptr;
    printf("%d ",temp->ptr->data);
    if(t->left !=NULL) enQueue(q,t->left);
    if(t->right !=NULL) enQueue(q,t->right);
  }
}
Node* search(Node *node,int data){
     if(node->data==data) return node;
     else if(node->data > data && node->left!=NULL){
           return search(node->left,data);
     }
     else if(node->data < data && node->right!=NULL){
      return search(node->right,data);
     }
     else {printf("\nelement not found\n");
  return NULL;}
}

int main(){
    int i,size,data;
    Node *root=NULL,*temp;

    printf("Number of nodes:\n");
    scanf("%d", &size);
    for (i=0;i<size;i++) {
          scanf("%d", &data);
        insert_node(data, &root);
    }
       printf("PREORDER: "); preorder(root);
       printf("\nINORDER: "); inorder(root); 
       printf("\nPOSTORDER: "); postorder(root);
       printf("\nLEVEL ORDER: "); levelorder(root);
    temp=search(root,5);
    if(temp!=NULL) printf("\nelement found\n");
    return 0;
}
