#include <stdio.h>
#include <string>
#include <queue>
#include <algorithm>
#include <iostream>
using namespace std;
typedef int ElementType;
struct node
{
    ElementType data;
    int layer;
    int height;
    node *lchild;
    node *rchild;
};

node *newNode(ElementType v)
{
    node *Node = new node;
    Node->data = v;
    Node->height = 1;
    Node->lchild = Node->rchild = NULL;
    return Node;
}

int getHeight(node *root)
{
    if (root == NULL)
        return 0;
    return root->height;
}

int getBalanceFactor(node *root)
{
    return getHeight(root->lchild) - getHeight(root->rchild);
}

void updateHeight(node *root)
{
    root->height = max(getHeight(root->lchild), getHeight(root->rchild)) + 1;
}

void LR(node *&root)
{ //左旋
    node *temp = root->rchild;
    root->rchild = temp->lchild;
    temp->lchild = root;
    updateHeight(root);
    updateHeight(temp);
    root = temp;
}

void RR(node *&root)
{ //右旋
    node *temp = root->lchild;
    root->lchild = temp->rchild;
    temp->rchild = root;
    updateHeight(root);
    updateHeight(temp);
    root = temp;
}

void insertByBST(node *&root, ElementType x)
{
    if (root == NULL)
    {
        root = newNode(x);
        return;
    }
    //按二叉搜索树定义插入(左子树小于等于根节点)?
    if (x < root->data)
        insertByBST(root->lchild, x);
    else
        insertByBST(root->rchild, x);
}

void insertByAVL(node *&root, ElementType x)
{ //按照二叉平衡树规则插入
    if (root == NULL)
    {
        root = newNode(x);
        return;
    }
    if (x < root->data)
    {
        insertByAVL(root->lchild, x);
        updateHeight(root);
        if (getBalanceFactor(root) == 2)
        {
            if (getBalanceFactor(root->lchild) == 1)
            { //LL型就右旋
                RR(root);
            }
            else if (getBalanceFactor(root->rchild) == -1)
            { //LR型就先对左孩子左旋变为LL型，再右旋
                LR(root->lchild);
                RR(root);
            }
        }
    }
    else
    {
        insertByAVL(root->rchild, x);
        updateHeight(root);
        if (getBalanceFactor(root) == -2)
        { //RR型，左旋
            if (getBalanceFactor(root->rchild) == -1)
                LR(root);
        }
        else if (getBalanceFactor(root->rchild) == 1)
        { //RL型，先将右孩子右旋变为RR型，再把根左旋
            RR(root->rchild);
            LR(root);
        }
    }
}

node *CreateBST(ElementType data[], int n)
{
    node *root = NULL;
    for (int i = 0; i < n; i++)
    {
        insertByBST(root, data[i]);
    }
    return root;
}

node *CreateAVL(ElementType data[], int n)
{
    node *root = NULL;
    for (int i = 0; i < n; i++)
    {
        insertByAVL(root, data[i]);
    }
    return root;
}

node *findMax(node *root)
{ //查找二叉搜索树中的最大节点
    while (root->rchild != NULL)
    {
        root = root->rchild;
    }
    return root;
}

node *findMin(node *root)
{ //查找二叉搜索树中的最小节点
    while (root->lchild != NULL)
    {
        root = root->lchild;
    }
    return root;
}

void deleteNode(node *&root, int x)
{
    if (x > root->data)
        deleteNode(root->rchild, x);
    else if (x < root->data)
        deleteNode(root->lchild, x);
    else if (x == root->data)
    {
        if (root->lchild == NULL && root->rchild == NULL)
            root = NULL;
        else if (root->lchild != NULL)
        {
            node *pre = findMax(root->lchild);
            root->data = pre->data;
            deleteNode(root->lchild, pre->data);
        }
        else if (root->rchild != NULL)
        {
            node *next = findMin(root->rchild);
            root->data = next->data;
            deleteNode(root->rchild, next->data);
        }
    }
    else
        return; //没有值为x的节点
}

void exchange(node *&root)
{
    if (root->lchild == NULL && root->rchild == NULL)
        return;
    else if (root->lchild != NULL && root->rchild == NULL)
    {
        root->rchild = root->lchild;
        root->lchild == NULL;
    }
    else if (root->lchild == NULL && root->rchild != NULL)
    {
        root->lchild = root->rchild;
        root->rchild = NULL;
    }
    else
    {
        node *temp = root->lchild;
        root->lchild = root->rchild;
        root->rchild = temp;
    }
}

void mirror(node *&root)
{
    if (root == NULL)
    {
        return;
    }
    exchange(root);
    mirror(root->lchild);
    mirror(root->rchild);
}

void preOrder(node *root)
{
    if (root == NULL)
    {
        return;
    }
    printf("%d ", root->data);
    preOrder(root->lchild);
    preOrder(root->rchild);
}

void inOrder(node *root)
{
    if (root == NULL)
    {
        return;
    }
    inOrder(root->lchild);
    printf("%d ", root->data);
    inOrder(root->rchild);
}

void postOrder(node *root)
{
    if (root == NULL)
    {
        return;
    }
    postOrder(root->lchild);
    postOrder(root->rchild);
    printf("%d ", root->data);
}

void layerOrder(node *root)
{
    queue<node *> q;
    root->layer = 1;
    q.push(root);
    while (!q.empty())
    {
        node *current = q.front();
        q.pop();
        printf("%d ", current->data);
        if (current->lchild != NULL)
        {
            current->lchild->layer = current->layer + 1;
            q.push(current->lchild);
        }
        if (current->rchild != NULL)
        {
            current->rchild->layer = current->layer + 1;
            q.push(current->rchild);
        }
    }
}

node *reBuild1(ElementType pre[], ElementType in[], int preL, int preR, int inL, int inR)
{ //已知先序遍历、中序遍历重建二叉树
    if (preL > preR)
        return NULL;
    node *root = new node;
    root->data = pre[preL];
    int rootIndex;
    for (rootIndex = inL; rootIndex <= inR; rootIndex++)
    {
        if (in[rootIndex] == pre[preL])
            break;
    }
    int numLeft = rootIndex - inL;
    root->lchild = reBuild1(pre, in, preL + 1, preL + numLeft, inL, rootIndex - 1);
    root->rchild = reBuild1(pre, in, preL + numLeft + 1, preR, rootIndex + 1, inR);
    return root;
}

node *reBuild2(ElementType in[], ElementType post[], int inL, int inR, int postL, int postR)
{ //已知中序遍历、后序遍历重建二叉树
    if (postL > postR)
        return NULL;
    node *root = new node;
    root->data = post[postR];
    int rootIndex;
    for (rootIndex = inL; rootIndex <= inR; rootIndex++)
    {
        if (in[rootIndex] == post[postR])
            break;
    }
    int numLeft = rootIndex - inL;
    root->lchild = reBuild2(in, post, inL, rootIndex - 1, postL, postL + numLeft - 1);
    root->rchild = reBuild2(in, post, rootIndex + 1, inR, postL + numLeft, postR - 1);
    return root;
}

int main()
{
    node *root = NULL;
    ElementType data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    root = CreateAVL(data, 9);
    printf("\n");
    preOrder(root);
    printf("\n");
    inOrder(root);
    printf("\n");
    postOrder(root);
    printf("\n");
    layerOrder(root);
}
/*
ElementType pre[] = {'A', 'B', 'D', 'E', 'F', 'G', 'C', 'H'};
ElementType in[] = {'D', 'B', 'F', 'E', 'G', 'A', 'C', 'H'};
ElementType post[] = {'D', 'F', 'G', 'E', 'B', 'H', 'C', 'A'};
*/