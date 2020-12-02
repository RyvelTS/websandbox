//Basic method used for Continuous memory allocation
//allocate freeMemSize(constant) Bytes in memory, use this memory
//region as the virtual machine memory;
//Use two link list to store the free memory block and the allocated
//memory block
//every time when a process need allocate new memory, it will find
//a free memory block in the freeLinkListHead
//if a block not used any more, it will delete form the usedLinkListHead
//and add to the freeLinkListHead, if two block are Adjacent, it will be
//mergence to one continuous block.
#include <stdio.h>
#include <stdlib.h>

//data structure used to store memory block information
typedef struct _memBlcokNode{  
  char *base;                                            //block start addr
  int len;                                                  //block len
  struct _memBlcokNode *next;                //next block address
}memBlockNode;

//the total memory size
#define freeMemSize  (1024)               

//limit allocate size per time
#define maxAllocMemSizePerTime  50

//total memory start address, for calculate the index of the byte array.
char *memBaseAddr=NULL;

//illustrate the memory allocated state
char memMap[freeMemSize];  // ‘U' for used , ‘.' for free

//free memory block
memBlockNode *freeLinkListHead=NULL;

//allcated memory block
memBlockNode *usedLinkListHead=NULL;

//allocate freeMemSize memory bytes as the virtual machine memory
void initMemBlock(void);

//allocate size bytes from the virtual machine memory
void allocMem(int size);

//for simple, release the node ‘index' of the link list, free the memory
void freeMem(int index);

//linklist base operation, add, delete, equal
void addNodeToList(memBlockNode** list,memBlockNode *newNode);
void delNodeFrmList(memBlockNode** list,memBlockNode *newNode);
int equalNode(memBlockNode *n1,memBlockNode*n2);

//display the virtual machine memory allocated result
void showBitmap(void);

//free all memmory;
void freeAllMem();

//sort the linklist
void BubbleSort(memBlockNode *head);

/*—————————————————————————*/
void freeAllMem(){
  memBlockNode* p=freeLinkListHead;
  while(p!=NULL){
    memBlockNode *t=p->next;
    free(p);
    p=t;
  }
  p=usedLinkListHead;
  while(p!=NULL){
    memBlockNode *t=p->next;
    free(p);
    p=t;
  }
  return ;
}
/*—————————————————————————*/
void BubbleSort(memBlockNode *head){
  memBlockNode *cur,*tail;
  cur=head;
  tail=NULL;
  if(cur==NULL||cur->next==NULL){
    return;
  }
  while(cur!=tail){
    while(cur->next!=tail){
      if(cur->base>cur->next->base){
        char * temp=cur->base;
        cur->base=cur->next->base;
        cur->next->base=temp;
        int temp1=cur->len;
        cur->len=cur->next->len;
        cur->next->len=temp1;
      }
      cur=cur->next;
    }
    tail=cur;
    cur=head;
  }
}
/*—————————————————————————*/
void showBitmap(void){
  int i=0;
  memBlockNode *p=usedLinkListHead;
  for(i=0;i<freeMemSize;i++){
    memMap[i]='.';
  }
  while(p!=NULL){
    for(i=0;i<p->len;i++){
      memMap[p->base-memBaseAddr+i]='U';
      //printf("%d",p->base-memBaseAddr+i);
    }
    p=p->next;
  }
  printf("\nMemory allocate bitmap is:\n");
    for(i=0;i<freeMemSize;i++){
      printf("%c",memMap[i]);
    }
    printf("\n");
}
/*—————————————————————————*/
int equalNode(memBlockNode *n1,memBlockNode*n2){
  if(n1->base==n2->base &&n1->len==n2->len)
    return 1;
  else
    return 0;
}
/*—————————————————————————*/
void delNodeFrmList(memBlockNode** list,memBlockNode *node){
  memBlockNode *pre;
  if(*list==NULL)
    return ;
  //if the first node is the delete node
  if(equalNode(node,*list)==1){
    *list=(*list)->next;         
    free(node);
    return ;
  }
  pre=*list;
  while(pre->next!=NULL)
  {          
    if(equalNode(pre->next,node)==1)
      break;
    pre=pre->next;
  }
  if(pre==NULL)
    return ;
  else{
    pre->next=node->next;
    free(node);
  }
}
/*—————————————————————————*/
//Add the node to the  memory linklist
void addNodeToList(memBlockNode** list,memBlockNode *newNode){
  if(*list==NULL)
    *list=newNode;
  else{
    memBlockNode *p=*list;
    while(p->next!=NULL){
      p=p->next;
    }
    p->next=newNode;
  }
}
/*—————————————————————————*/
void freeMem(int index){
  int i=0;
  memBlockNode* p=usedLinkListHead;
  while(p!=NULL){
    if(i==index)
      break;
    i++;                 
    p=p->next ;
  }
  if(p==NULL)
    return ;
  else{
    memBlockNode *p1=freeLinkListHead;
    printf("Free %d bytes memory \n",p->len);
    while(p1!=NULL){
      if((p1->base+p1->len)==p->base){
        p1->len=p1->len+p->len;
        delNodeFrmList(&usedLinkListHead,p);
        return ;
      }
      p1=p1->next;
    }
    memBlockNode * tmp= (memBlockNode *)malloc(sizeof(memBlockNode));
    if(tmp==NULL)
      exit(1);
    tmp->base =p->base;
    tmp->len =p->len;
    tmp->next=NULL;
    addNodeToList(&freeLinkListHead,tmp);
    delNodeFrmList(&usedLinkListHead,p);
    BubbleSort(freeLinkListHead);
  }
}
/*—————————————————————————*/
void allocMem(int size){
  //MODIFICATION
  int temp = 0,lowest= 9999;
  memBlockNode *pTemp=NULL;
  //
  memBlockNode *pNode=freeLinkListHead;
  memBlockNode *pNewNode=NULL;
  if(size>maxAllocMemSizePerTime)
    size=maxAllocMemSizePerTime;
  //find the first big enough free memory in list, you can use other algorithm
  while(pNode!=NULL){
    if(pNode->len >= size){
      //MODIFICATION
      temp=pNode->len;
      if (lowest>temp){
        lowest = temp;
        pTemp = pNode;
      }
    }//
      //break;
    pNode=pNode->next; 
  }pNode=pTemp;
  //if no memory, exit program
  if(pNode==NULL)
  {
    printf("there is no enough memory ,wants allocate %d bytes\n",size);
    //showBitmap();
    //exit(1) ;
    return ;
  }
  //create a new node in the used memory linklist
  pNewNode= (memBlockNode*)malloc(sizeof(memBlockNode));
  if(pNewNode==NULL)
  {
    printf("allocate memory failure\n");
    exit(1);
  }
  pNewNode->base=pNode->base;
  pNewNode->len=size;
  pNewNode->next=NULL;
  addNodeToList(&usedLinkListHead,pNewNode);
  if(pNode->len==size)
  {
    delNodeFrmList(&freeLinkListHead,pNode);
  }
  else  //update free memory info for the current node
  {
    pNode->base= pNode->base +size ;
    pNode->len=pNode->len-size;
  }
  printf("Allocate %d byte memory\n",size);
}
/*—————————————————————————*/
void initMemBlock(void){
  memBlockNode * node= (memBlockNode*)malloc(sizeof(memBlockNode));
  if(node==NULL)
  {
    printf("allocate memory failure\n");
    exit(1);
  }
  node->base =(char*)malloc(freeMemSize);
  memBaseAddr=node->base;
  if(node->base==NULL)
  {
    printf("allocate memory failure\n");
    exit(1);
  }
  node->len=freeMemSize;
  node->next=NULL;
  freeLinkListHead=node;
}
/*—————————————————————————*/
int main(){
  int count=0,i;
  initMemBlock();
  printf("Before allocate memory\n");
  showBitmap();
  for(i=0;i<100;i++){
    int size=rand()%maxAllocMemSizePerTime+1;
    allocMem(size);
    count++;
    showBitmap();
    if(count>5)
    {
      int index=rand()%count-1;
      freeMem(index);                       
    }
    usleep(300);
  }
  freeAllMem();
  free(memBaseAddr);
  return 0;
}