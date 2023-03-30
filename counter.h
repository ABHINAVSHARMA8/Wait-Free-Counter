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
atomic<int> version(0);
#define THRESHOLD 1000
unordered_map<thread::id,int> map;
#define C 3

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
    int version=-1;
    public:
    OpDesc(int a,int b){
        x=a;
        version=b;
    }

};
mutex lock2;
 atomic<node*> counter;
 atomic<OpDesc*> platform[C];

int getThreadId();
int getVersion();
void help(int);
void WFIncrement(int);
void announce(int,int);
int stick_this_thread_to_core(int);