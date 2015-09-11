#include "bstree.hpp"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>

#define MAXIMUM 1e9
pthread_mutex_t writeM;
class avlTree: public BST_int{
    private:
        bool avl_contains(node * root, int n){

            if(root == NULL)
                return false;
            else{
                //std::cout << "What have we here" <<std::endl;
                //pthread_mutex_lock(&lock);
                if(root->value == n){
                    //pthread_mutex_unlock(&lock);
                    return true;
                }
                bool val =  n > root->value ? avl_contains(root->right,n) : avl_contains(root->left,n);
                //pthread_mutex_unlock(&lock);
                return val;
            }
        }
        /*void add_avl(node* root, int n){
            //Go right
            if(n > root->value){
                //Can We go right?
                if(root->right != NULL){
                    add_avl(root->right, n);

                    pthread_mutex_lock(&(root->lock));
                    root->right = avl_rebalance(root->right);
                    pthread_mutex_unlock(&(root->lock));
                }
                //Put it on the right
                else{
                    pthread_mutex_lock(&root->lock);
                    if(root->right == NULL){
                        root->right = (node *)malloc(sizeof(node));
                        root->right->value = n;
                        root->right->height = 1;
                        root->right->left = NULL;
                        root->right->right = NULL;
                        root->right->parent = root;
                        pthread_mutex_init(&(root->right->lock), NULL);
                        pthread_mutex_unlock(&(root->lock));
                    }
                    else{
                        pthread_mutex_unlock(&(root->lock));
                        add_avl(root->right, n);

                        pthread_mutex_lock(&(root->lock));
                        root->right = avl_rebalance(root->right);
                        pthread_mutex_unlock(&(root->lock));
                    }
                }
            }
            //Go left
            else{
                //Can We go left?
                if(root->left != NULL){
                    add_avl(root->left, n);

                    pthread_mutex_lock(&(root->lock));
                    root->left = avl_rebalance(root->left);
                    pthread_mutex_unlock(&(root->lock));
                }
                //Put it on the right
                else{
                    pthread_mutex_lock(&(root->lock));
                    if(root->left == NULL){//By the time we get the lock, there might be a node in the spot
                        root->left = (node *)malloc(sizeof(node));
                        root->left->value = n;
                        root->left->height = 1;
                        root->left->left = NULL;
                        root->left->right = NULL;
                        root->left->parent = root;
                        pthread_mutex_init(&(root->left->lock), NULL);
                        pthread_mutex_unlock(&(root->lock));
                    }
                    else{
                        pthread_mutex_unlock(&(root->lock));
                        add_avl(root->left, n);

                        pthread_mutex_lock(&(root->lock));
                        root->left = avl_rebalance(root->left);
                        pthread_mutex_unlock(&(root->lock));
                        //root->left = rebalance(root->left);
                    }
                }
            }
        }*/
        node* avl_rebalance(node* root){
            /*
            * While inside this function it is assumed
            * root is locked.
            */
            fixheight(root);

            if(balance_factor(root) >= 2){
                if(balance_factor(root->right) < 0){
                    root->right = rotateright(root->right);
                }
                //pthread_mutex_unlock(&(root->lock));
                return avl_rotateleft(root);
            }
            if(balance_factor(root) <= -2){
                if(balance_factor(root->left) > 0){
                    root->left = rotateleft(root->left);
                }
                //pthread_mutex_unlock(&(root->lock));
                return avl_rotateright(root);
            }
            //pthread_mutex_lock(&(root->lock));
            return root;
        }
        node* avl_rotateright(node* p){
            //pthread_mutex_lock(&(p->lock));
            //pthread_mutex_lock(&(p->left->lock));
            node* q = p->left;

            p->left = q->right;
            q->right = p;
            fixheight(p);
            fixheight(q);

            //pthread_mutex_unlock(&(p->lock));
            //pthread_mutex_unlock(&(q->lock));
            return q;
        }
        node* avl_rotateleft(node* q){
            //pthread_mutex_lock(&(q->lock));
            //pthread_mutex_lock(&(q->right->lock));
            node* p = q->right;

            q->right = p->left;
            p->left = q;
            fixheight(q);
            fixheight(p);

            //pthread_mutex_unlock(&(q->lock));
            //pthread_mutex_unlock(&(p->lock));
            return p;
        }
    public:
        pthread_mutex_t lock;
        avlTree(){
            root = NULL;
            nNodes = 0;
            //pthread_mutex_init(&lock, NULL);
        }
        bool avl_contains(int n){
            pthread_mutex_lock(&lock);
            //bool b = searchTree(root, n);
            bool b = searchTree(n);
            pthread_mutex_unlock(&lock);
            return true;
        };
        void avl_add(int n){
            pthread_mutex_lock(&lock);
            //add_avl(root,n);
            add(n);
            pthread_mutex_unlock(&lock);
            //add_avl(root,n);
            return;
        };
        void avl_delete(int n){
            /*if(!avl_contains(n))
                return;
            else{*/
                pthread_mutex_lock(&lock);
                delNode(n);
                pthread_mutex_unlock(&lock);
            //}
        }
};
typedef struct package{
    avlTree *tree;
    int id;
    int maxOps;
    long timeTaken;
}pkg;
void *perform_work(void *argument);
long trial(avlTree * tree, int numOps,bool explain);
int main(void){
    std::ofstream outFile;
    outFile.open("con_coarse_avl_100000000_10_3.csv");
    for(int i = 1;i<=64;i*=2){
        outFile << i << ",";
        std::cout << "Began trial " << i <<std::endl;
        for(int j=0;j<10;j++){
            int NUM_THREADS = i;
            pthread_t workers[NUM_THREADS];
            int result_code, index;

            pkg getStarted[NUM_THREADS];
            //pthread_mutex_init(&writeM, NULL);
            avlTree tree;
            pthread_mutex_init(&tree.lock, NULL);
            tree.init(-1);//Sentinel value

            for(index=0;index<NUM_THREADS;index++){
                printf("In main: creating thread %d\n", index);
                getStarted[index].id = index;
                getStarted[index].tree = &tree;
                getStarted[index].maxOps = (int)((int)100000000/(int)i);
                result_code = pthread_create(&workers[index], NULL, perform_work, (void *) &getStarted[index]);
                assert(0 == result_code);
            }
            long maxTime = 0;
            for(index=0;index < NUM_THREADS; index++){
                result_code = pthread_join(workers[index], NULL);
                assert(0 == result_code);

                printf("\tIn main: thread %d has complteted\n", index);

            }

            for(index=0;index< NUM_THREADS; index++){
                if(getStarted[index].timeTaken > maxTime)
                    maxTime = getStarted[index].timeTaken;
            }
            outFile << maxTime << ",";
            //tree.printTree(1);
            tree.delTree();
            pthread_mutex_destroy(&tree.lock);
        }
        std::cout << "End Trial " << i <<"." << std::endl;
        outFile << std::endl;
    }
    outFile.close();
    return 0;
}
void *perform_work(void *argument){
    pkg  * p = (pkg *) argument;
    avlTree  * tree = p->tree;
    int id = p->id;
    p->timeTaken = trial(tree, p->maxOps,false);
    /*for(int i=0;i<100;i++){
        std::cout << "\tThread " << id << " adding" << i << std::endl;
        //tree.contains(i);
        tree.add(i*3+id);
        /*if(id == 0){
            tree.add(2*i+1);
        }
        else{
            tree.add(2*i);
        }
    }*/
    return NULL;
}
long trial(avlTree * tree, int numOps,bool explain){
    srand(time(NULL));
    //std::cout << numOps << std::endl;
    time_t startT,endT;
    //BST_int tree;
    int victim, choice;
    time(&startT);
    for(int i=0;i<numOps;i++){
        //std::cout<< i << std::endl;
        choice = rand()%100;
        victim = rand()%(int)MAXIMUM;
        if(choice < 90){
            if(explain)
                std::cout << "Checking: " << victim << std::endl;
            tree->avl_contains(victim);
        }
        else if(choice < 99){
            if(explain)
                std::cout << "Adding: " << victim << std::endl;
            tree->avl_add(victim);
        }
        else{
            if(explain)
                std::cout << "Deleting: " << victim << std::endl;
            //tree->avl_delete(victim);
        }
    }
    time(&endT);
    //tree.delTree();
    //std::cout << difftime(endT, startT);
    return (long)(difftime(endT, startT));
}
