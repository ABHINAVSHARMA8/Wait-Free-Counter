
#include "counter.h"



int getVersion(){
    return atomic_fetch_add(&version,1);
}

int getThreadId(){
    return map[this_thread::get_id()];

}


void announce(int x,int version){
    int tid=getThreadId();
    OpDesc *oldPI=platform[tid].load(memory_order_seq_cst);
    platform[tid].compare_exchange_strong(oldPI,new OpDesc(x,version));
    
}

void help(int version){
    
    for(int i=0;i<C;i++){
        OpDesc *oldPI=platform[i].load(memory_order_seq_cst);
        if(oldPI!=NULL && version>oldPI->version){
            while(1){
                node *oldValue=counter.load(memory_order_seq_cst);
                /*if(!(oldValue.index==i && oldValue.state=="help") && 
                atomic_compare_exchange_strong(&counter,&oldValue,new node(counter.counter+oldPI.x,i,"help"))){
                   
                    atomic_compare_exchange_strong(&platform[i],&oldPI,NULL);
                    break;
                }

                if(oldValue.index==i && oldValue.state=="help") break; 
                */
                if(!(oldValue->index==i && oldValue->state=="help")){

                    if(counter.compare_exchange_strong(oldValue,new node(oldValue->counter+oldPI->x,i,"help"))){
                        
                        platform[i].compare_exchange_strong(oldPI,NULL);
                        
                        break;

                    }

                }
                else break;
                
            }
        }
    }
    
}


void WFIncrement(int x){
    int version=getVersion();
    int tid=getThreadId();
    int tried=0;
    help(version);
    
    node *temp=new node(0,tid,"increment"); //constructor
    while(tried<THRESHOLD){
        node *oldValue=counter.load(memory_order_seq_cst);
        int index=oldValue->index;
        
        OpDesc *oldPI=platform[index].load(memory_order_seq_cst);
        if(oldPI!=NULL && oldValue->state=="help"){
            if(platform[index].compare_exchange_strong(oldPI,NULL))
                break;
        }//if premepted,then not incremented
        
        
        temp->counter=oldValue->counter+x;
        if(counter.compare_exchange_strong(oldValue,temp))
            break;

        tried++;
    }
    
    if(tried==THRESHOLD) {
        delete temp;
        announce(x,version);
    }
    
}

void threadIncrement(int x,int tid){
    lock2.lock();
    map[this_thread::get_id()]=tid;
    lock2.unlock();
    //stick_this_thread_to_core(tid);
    for(int i=0;i<100000;i++)
        WFIncrement(x);
}

/*int stick_this_thread_to_core(int core_id) {
   
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core_id, &cpuset);

   pthread_t current_thread = pthread_self();    
   return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}*/




int main(){
    counter=new node(0,-1,"");
    for(int i=0;i<C;i++)
        platform[i]=new OpDesc(0,-1);
    thread t1(threadIncrement,1,0);
    thread t2(threadIncrement,2,1);
    thread t3(threadIncrement,3,2);

    t1.join();
    t2.join();
    t3.join();
    node *temp=counter.load(memory_order_seq_cst);
    cout<<temp->counter<<endl;

    return 0;
}