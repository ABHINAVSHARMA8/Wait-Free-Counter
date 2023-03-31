
#include "counter.h"

double getVersion(){
    clock_t time=clock();
    return (double)time/CLOCKS_PER_SEC;
}



void announce(int x,double version,int tid){
    
    OpDesc *oldPI=platform[tid].load(memory_order_seq_cst);
    platform[tid].compare_exchange_strong(oldPI,new OpDesc(x,version));
    
}

void help(double version){
   
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
                //cout<<"help3"<<endl;
                if(!(oldValue->index==i && oldValue->state=="help")){

                    if(counter.compare_exchange_strong(oldValue,new node(oldValue->counter+oldPI->x,i,"help"))){
                        
                        platform[i].compare_exchange_strong(oldPI,NULL);
                        //cout<<"help1"<<endl;
                        break;

                    }
                    //cout<<"help2"<<endl;
                   

                }
                else break;
                
            }
        }
    }
    
    
}


void WFIncrement(int x,int tid){
    double version=getVersion();
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
        announce(x,version,tid);
    }
    
}

void threadIncrement(int x,int tid){
    
    for(int i=0;i<100000;i++)
        WFIncrement(x,tid);
}






int main(){
    counter=new node(0,-1,"");
    for(int i=0;i<C;i++)
        platform[i]=new OpDesc(0,-1);

    thread t1(threadIncrement,1,0);
    thread t2(threadIncrement,1,1);
    thread t3(threadIncrement,1,2);
    thread t4(threadIncrement,10,3);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    node *temp=counter.load(memory_order_seq_cst);
    cout<<temp->counter<<endl;

    return 0;
}