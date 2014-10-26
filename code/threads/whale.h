#ifndef WHALE_H
#define WHALE_H


#include "copyright.h"
#include "synch.h"
#include "list.h"

class Whale{

  public:
	  Whale(char* debugName);
	  ~Whale();
	  char* getName(){
		return name;
	 }
	  void Male();
	  void Female();
	  void Matchmaker();
    
  private:  
	  char* name;
	  Semaphore* male;
	  Semaphore* female;
	  Semaphore* matchmaker;
	  Semaphore* newMember;
	  int match;
	  int matched;

  
};



#endif //WHALE_H