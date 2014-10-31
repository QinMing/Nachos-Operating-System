#include "whale.h"

Whale::Whale(char* debugName){
	name = debugName;
	male = new Semaphore("maleWhale",1);
	female = new Semaphore("femaleWhale",1);
	matchmaker = new Semaphore("matchmakerWhale",1);
	newMember = new Semaphore("newMember",0);
	match = 0; //number of whales currently waiting to be matched
	matched=0; //number of whales have been matched
	pairNO=1;  //number of matched
	maleNum = 0; //number of male whales waiting to be matched
	femaleNum = 0;//number of female whales waiting to be matched
	matchmakerNum = 0; //number of matchmaker whales waiting to be matched
}

Whale::~Whale(){

  delete male;
  delete female;
  delete matchmaker;
  delete newMember;
  
}


void Whale::Male(){
  DEBUG('x', "a male is going to acquire male lock\n");
  maleNum++;
  printf("COME: one male comes to match; %d male whale(s) to be matched now\n", maleNum);
  male->P();		//block other male
  DEBUG('x', "a male wake from male lock \n");
  match ++; 
  
  
  while(match<3){  	//only when there are three whales from different classes are waiting, can the match continues
    newMember->P();	//wait for the other two
  }
  DEBUG('x', "a male wake from newMember lock\n");

  if (matched < 2){
	  newMember->V(); 	//when the match completed, wake up the other two on the newMember list, one by one 
  }

  if(matched==2){	 //when the third whale completes the match, initialize the flags before it returns
    match=0;
    matched=0;
	maleNum--;
	DEBUG('x', "male->signal from male\n");
	printf("RETURN: one male whale return from match NO.%d; %d male whale(s) to be matched now\n", pairNO, maleNum);
	printf("match NO. %d completed\n", pairNO);//before the third whale return, send a signal imply a match is completed
	pairNO++;
    matchmaker->V();	//and let other whales continue the match
    female->V();
    male->V();
  }
  else{                 //the first two whales complete the match,don't initialize flags, they just return
    matched ++; 
	maleNum--;
	printf("RETURN: one male whale return from match NO.%d; %d male whale(s) to be matched now\n", pairNO, maleNum);
  }
}

//the same as Male(), simpley changed male to female
void Whale::Female(){
  femaleNum++;
  printf("COME: one female comes to match; %d female whale(s) to be matched now\n", femaleNum);
  female->P(); //block other female
  
  match ++;

  
  while(match<3){
    newMember->P();
  }
  
  if (matched < 2){
	  newMember->V();
  }

  if(matched==2){
    match=0;
    matched=0;
	femaleNum--;
	DEBUG('x', "male->signal from female\n");
	printf("RETURN: one female whale return from match NO.%d; %d female whale(s) to be matched now\n", pairNO, femaleNum);
	printf("match NO. %d completed\n", pairNO);
	pairNO++;
    matchmaker->V();
    female->V();
    male->V();	
  }
  else{
    matched ++;
	femaleNum--;
	printf("RETURN: one female whale return from match NO.%d; %d female whale(s) to be matched now\n", pairNO, femaleNum);
  }
  
  
}

//the same as Male(), simpley changed male to matchmaker
void Whale::Matchmaker(){
  matchmakerNum++;
  printf("COME: one matchmaker comes to match; %d matchmaker(s) to be matched now\n", matchmakerNum);
  matchmaker->P(); //block other matchmaker
  
  match ++;
    
  while(match<3){
    newMember->P();
  }
  
  if (matched < 2){
	  newMember->V();
  }

  if(matched==2){
    match=0;
    matched=0;
	matchmakerNum--;
	DEBUG('x', "male->signal from mm\n");
	printf("RETURN: one matchmaker whale return from match NO.%d; %d matchmaker(s) to be matched now\n", pairNO, matchmakerNum);
	printf("match NO. %d completed\n", pairNO);
	pairNO++;
    matchmaker->V();
    female->V();
    male->V();
  }
  else{
    matched ++;
	matchmakerNum--;
	printf("RETURN: one matchmaker whale return from match NO.%d; %d matchmaker(s) to be matched now\n", pairNO, matchmakerNum);
  }
}