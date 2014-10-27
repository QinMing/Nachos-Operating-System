#include "whaleTest.h"

Whale* whale;

void m1(int param){
  printf("m1:0\n");
  whale->Male();
  printf("m1:1\n");
}

void f1(int param){
  printf("f1:0\n");
  whale->Female();
  printf("f1:1\n");
}

void mm1(int param){
  printf("mm1:0\n");
  whale->Matchmaker();  
  printf("mm1:1\n");
}


void m2(int param){
  printf("m2:0\n");
  whale->Male();
  printf("m2:1\n");
}

void f2(int param){
  printf("f2:0\n");
  whale->Female();
  printf("f2:1\n");
}

void mm2(int param){
  printf("mm2:0\n");
  whale->Matchmaker();  
  printf("mm2:1\n");
}

void m3(int param){
  printf("m3:0\n");
  whale->Male();
  printf("m3:1\n");
}

void f3(int param){
  printf("f3:0\n");
  whale->Female();
  printf("f3:1\n");
}

void mm3(int param){
  printf("mm3:0\n");
  whale->Matchmaker();  
  printf("mm3:1\n");
}


void WhaleTest::start(){
  
    DEBUG('t', "Entering WhaleMatchingTest");
    
    whale = new Whale("test");
    
    Thread *t = new Thread("one");
    t->Fork(m1, 0);
    t = new Thread("two");
    t->Fork(mm2, 0);
    t = new Thread("three");
    t->Fork(m3, 0);
    t = new Thread("three");
    t->Fork(f1, 0);
    t = new Thread("four");
    t->Fork(f2, 0);
    t = new Thread("five");
    t->Fork(f3, 0);
    t = new Thread("six");
    t->Fork(mm1, 0);
    t = new Thread("seven");
    t->Fork(m2, 0);
    t = new Thread("eight");
    t->Fork(mm3, 0);
    t = new Thread("nine");
    
}