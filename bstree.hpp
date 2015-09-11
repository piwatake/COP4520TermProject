#ifndef BSTREE_H
#define BSTREE_H
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#define HARD_NODE_CAP 1000000
struct node{
        node* parent;
        node* left;
        node* right;
        bool marked;
        int value;
        int height;
        //pthread_mutex_t lock;
};

class BST_int{
    protected: //I know it's implicit but it feels weird.
        node * root;
        int nNodes;
        /**********************
        *    AVL Functions    *
        ***********************/
        int balance_factor(node * root){
            return height(root->right)-height(root->left);
        }
        int height(node* root){
            return root ? root->height : 0;
        }
        void fixheight(node* root){
            int hLeft = height(root->left);
            int hRight = height(root->right);
            root->height = (hLeft > hRight ? hLeft : hRight)+1;
        }
        node* rotateright(node* p){
            node* q = p->left;
            p->left = q->right;
            q->right = p;
            fixheight(p);
            fixheight(q);
            return q;
        }
        node* rotateleft(node* q){
            node* p = q->right;
            q->right = p->left;
            p->left = q;
            fixheight(q);
            fixheight(p);
            return p;
        }
        node *  rebalance(node * root){
            fixheight(root);
            if(balance_factor(root)==2){
                if(balance_factor(root->right) < 0){
                    root->right = rotateright(root->right);
                }
                return rotateleft(root);
            }
            if(balance_factor(root) == -2){
                if(balance_factor(root->left) > 0){
                    root->left = rotateleft(root->left);
                }
                return rotateright(root);
            }
            return root;
        }
        int numNodes(node * root){
            return root ? 1+numNodes(root->left)+numNodes(root->right) : 0;
        }
    public:
        BST_int(){
            root = NULL;
            nNodes = 0;
        }
        void init(int n){
            root = (node *)malloc(sizeof(node));
            root->left = NULL;//nullptr;
            root->right = NULL;//nullptr;
            root->parent = NULL;
            root->value = n;
            root-> height = 1;
        }

        /*********************
        *   Tree functions   *
        **********************/
        void add(int n){
            if(nNodes >= HARD_NODE_CAP)
                return;
            if(root != NULL)
                addNode(root, n);
            else
                init(n);
            nNodes++;
        }
        void addNode(node * root, int n){
            //Go right
            if(n > root->value){
                //Can We go right?
                if(root->right != NULL){
                    addNode(root->right, n);
                    root->right = rebalance(root->right);
                }
                //Put it on the right
                else{
                    root->right = (node *)malloc(sizeof(node));
                    root->right->value = n;
                    root->right->height = 1;
                    root->right->left = NULL;
                    root->right->right = NULL;
                    root->right->parent = root;
                }
            }
            //Go left
            else{
                //Can We go left?
                if(root->left != NULL){
                    addNode(root->left, n);
                    root->left = rebalance(root->left);
                }
                //Put it on the right
                else{
                    root->left = (node *)malloc(sizeof(node));
                    root->left->value = n;
                    root->left->height = 1;
                    root->left->left = NULL;
                    root->left->right = NULL;
                    root->left->parent = root;
                }
            }
        }
        node * findMax(node * root){
            return root->right == NULL ? root : findMax(root->right);
        }
        node * findMin(node * root){
            return root->left == NULL ? root : findMin(root->left);
        }
        node * removeMin(node * root){
            if(root->left == NULL)
                return root->right;
            root->left = removeMin(root->left);
            return rebalance(root);
        }
        void delNode(int n){
            nNodes--;
            root = delNode(root, n);
        }
        node * delNode(node * root, int n){
            if(root == NULL)
                return NULL;
            if(n < root->value)
                root->left = delNode(root->left, n);
            else if(n > root->value)
                root->right = delNode(root->right,n);
            else{
                node * tmp1 = root->left;
                node * tmp2 = root->right;
                //pthread_mutex_destroy(&(root->lock));
                free(root);
                if(tmp2 == NULL)
                    return tmp1;
                node * minNode = findMin(tmp2);
                minNode->right = removeMin(tmp2);
                minNode->left = tmp1;
                return rebalance(minNode);
            }
        }
        bool searchTree(int n){
            return searchTree(root, n);
        }
        bool searchTree(node * root, int n){
            if(root == NULL)
                return false;
            else if(root->value == n)
                return true;
            if(n > root->value)
                return searchTree(root->right, n);
            else
                return searchTree(root->left, n);
        }
        void delTree(){
            delAllNodes(root);
        }
        void delAllNodes(node * root){
            if(root == NULL)
                return;
            delAllNodes(root->left);
            delAllNodes(root->right);
            free(root);
            return;
        }
        int numNodes(){
            return numNodes(root);
        }
        /**********************
        *   Print Functions   *
        ***********************/
        void  printTree(int n){
            switch(n){
            case 1:
                printTreeNodesPreOrder(root);
                break;
            case 2:
                printTreeNodesInOrder(root);
                break;
            case 3:
                printTreeNodesPostOrder(root);
                break;
            }
            std::cout << std::endl;
        }
        void  printTreeNodesPreOrder(node * root){
            if(root == NULL)
                return;
            std::cout << root->value << " ";
            if(root->left != NULL || root->right != NULL)
                std::cout <<"[";
            printTreeNodesPreOrder(root->left);
            std::cout << " ";
            printTreeNodesPreOrder(root->right);
            if(root->left != NULL || root->right != NULL)
                std::cout << "]";
        }
        void  printTreeNodesInOrder(node * root){
            if(root == NULL)
                return;
            printTreeNodesInOrder(root->left);
            std::cout << root->value << " ";
            printTreeNodesInOrder(root->right);
        }
        void  printTreeNodesPostOrder(node * root){
            if(root == NULL)
                return;
            printTreeNodesPostOrder(root->left);
            printTreeNodesPostOrder(root->right);
            std::cout << root->value << " ";
        }
};

#endif
