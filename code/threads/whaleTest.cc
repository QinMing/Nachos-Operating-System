//We used 4 semaphores to implement whale matching.There are 3 functions with similar
//codes male, female and matchmaker representing whales from each class.There are four
//semaphores thus four wait lists, one for each whale class to make sure only one
//whale from each class is waiting to be matched, and another for whales to wait for being
//matched.Only when 3 whales from each of the 3 classes come, can they form a match and then
//return, otherwise wait.To make things clear, we add a variable to indicate the number 
//of match, when a male whale returns, the program will print "male whale return from match
//NO.#", when female and matchmaker returns the program will print similar message.And when
//a match    completed it will print "match NO.# completed"
//test switch case numbers used :
//30 : 9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker, three matches succeed
//31 : 8 whales, 3 male, 3 female, 2 matchmaker, only two matches succeed
//32 : 7 whales, 3 male, 1 female, 3 matchmaker, only one match succeed
//33 : 9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker, in a different order, three matches                    succeed
//we also tried - rs while testing and our program works fine in random switch circumstance


#include "whaleTest.h"

//m stands for male, f stands for female, mm stands for matchmaker


Whale* whale;


void m1(int param){
  DEBUG('w', "m1:0\n");
  whale->Male();
  DEBUG('w', "m1:1\n");
}

void f1(int param){
  DEBUG('w', "f1:0\n");
  whale->Female();
  DEBUG('w', "f1:1\n");
}

void mm1(int param){
  DEBUG('w', "mm1:0\n");
  whale->Matchmaker();  
  DEBUG('w', "mm1:1\n");
}


void m2(int param){
  DEBUG('w', "m2:0\n");
  whale->Male();
  DEBUG('w', "m2:1\n");
}

void f2(int param){
  DEBUG('w', "f2:0\n");
  whale->Female();
  DEBUG('w', "f2:1\n");
}

void mm2(int param){
  DEBUG('w', "mm2:0\n");
  whale->Matchmaker();  
  DEBUG('w', "mm2:1\n");
}

void m3(int param){
  DEBUG('w', "m3:0\n");
  whale->Male();
  DEBUG('w', "m3:1\n");
}

void f3(int param){
  DEBUG('w', "f3:0\n");
  whale->Female();
  DEBUG('w', "f3:1\n");
}

void mm3(int param){
  DEBUG('w', "mm3:0\n");
  whale->Matchmaker();  
  DEBUG('w', "mm3:1\n");
}


void WhaleTest::start(int testnum){
  
    DEBUG('t', "Entering WhaleMatchingTest");
    
    whale = new Whale("test");

	Thread *t;
    
	switch (testnum){ 
		case 30:				//9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker
								//three matches succeed
			t = new Thread("m1");
			t->Fork(m1, 0);
			t = new Thread("m2");
			t->Fork(m2, 0);
			t = new Thread("m3");
			t->Fork(m3, 0);
			t = new Thread("f1");
			t->Fork(f1, 0);
			t = new Thread("f2");
			t->Fork(f2, 0);
			t = new Thread("f3");
			t->Fork(f3, 0);
			t = new Thread("mm1");
			t->Fork(mm1, 0);
			t = new Thread("mm2");
			t->Fork(mm2, 0);
			t = new Thread("mm3");
			t->Fork(mm3, 0);
			break;
		case 31:			   //8 whales , 3 male, 3 female, 2 matchmaker
							   // only two matches succeed
			t = new Thread("m1");
			t->Fork(m1, 0);
			t = new Thread("m2");
			t->Fork(m2, 0);
			t = new Thread("m3");
			t->Fork(m3, 0);
			t = new Thread("f1");
			t->Fork(f1, 0);
			t = new Thread("f2");
			t->Fork(f2, 0);
			t = new Thread("f3");
			t->Fork(f3, 0);
			t = new Thread("mm1");
			t->Fork(mm1, 0);
			t = new Thread("mm2");
			t->Fork(mm2, 0);
			break;
		case 32:			   //7 whales , 3 male, 1 female, 3 matchmaker
							   // only one match succeed
			t = new Thread("m1");
			t->Fork(m1, 0);
			t = new Thread("m2");
			t->Fork(m2, 0);
			t = new Thread("m3");
			t->Fork(m3, 0);
			t = new Thread("f1");
			t->Fork(f1, 0);
			t = new Thread("mm1");
			t->Fork(mm1, 0);
			t = new Thread("mm2");
			t->Fork(mm2, 0);
			t = new Thread("mm3");
			t->Fork(mm3, 0);
			break;
		case 33 :				//9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker, in a different order
								////three matches succeed
			t = new Thread("m1");
			t->Fork(m1, 0);
			t = new Thread("f1");
			t->Fork(f1, 0);
			t = new Thread("mm1");
			t->Fork(mm1, 0);
			t = new Thread("f2");
			t->Fork(f2, 0);
			t = new Thread("m2");
			t->Fork(m2, 0);
			t = new Thread("f3");
			t->Fork(f3, 0);
			t = new Thread("mm2");
			t->Fork(mm2, 0);
			t = new Thread("mm3");
			t->Fork(mm3, 0);
			t = new Thread("m3");
			t->Fork(m3, 0);
			break;
		default:;
	}
}