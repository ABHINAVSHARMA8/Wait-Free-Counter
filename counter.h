#define _GNU_SOURCE

#include <iostream>       
#include <atomic>         
#include <thread>         
#include <unordered_map>
#include <assert.h>
#include <chrono>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

#define THRESHOLD 500
#define C 4

struct node{
    int counter=0;
    int index=-1;
    string state;
    public:
    node(int a,int b,string c){
        counter=a;
        index=b;
        state=c;
    }
};

struct OpDesc{
    int x;
    double version=-1;
    public:
    OpDesc(int a,int b){
        x=a;
        version=b;
    }

};

atomic<node*> counter;
atomic<OpDesc*> platform[C];


double getVersion();
void announce(int,double,int);
void help(double);
void WFIncrement(int,int);
void threadIncrement(int,int);
