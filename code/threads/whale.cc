#include "whale.h"

Whale::Whale(char* debugName){
	name = debugName;
	male = new Semaphore("maleWhale",1);
	female = new Semaphore("femaleWhale",1);
	matchmaker = new Semaphore("matchmakerWhale",1);
	newMember = new Semaphore("newMember",0);
	match = 0;
	matched=0;
}

Whale::~Whale(){

  delete male;
  delete female;
  delete matchmaker;
  delete newMember;
  
}


void Whale::Male(){
  
  male->P(); //block other male
  
  match ++;
  
  
  while(match<3){
    newMember->P();
  }
  newMember->V();
  newMember->V();
  
  if(matched==2){
    match=0;
    matched=0;
    matchmaker->V();
    female->V();
    male->V();
    printf("one match completed\n");
  }
  else
    matched ++;
  
  
}

void Whale::Female(){
  
  female->P(); //block other female
  
  match ++;

  
  while(match<3){
    newMember->P();
  }
  
  newMember->V();
  newMember->V();
  
  if(matched==2){
    match=0;
    matched=0;
    matchmaker->V();
    female->V();
    male->V();
    printf("one match completed\n");
  }
  else
    matched ++;
  
  
  
}

void Whale::Matchmaker(){
  
  matchmaker->P(); //block other matchmaker
  
  match ++;
    
  while(match<3){
    newMember->P();
  }
  
  newMember->V();
  newMember->V();
  
  if(matched==2){
    match=0;
    matched=0;
    matchmaker->V();
    female->V();
    male->V();
    printf("one match completed\n");
  }
  else
    matched ++;
   
  
}