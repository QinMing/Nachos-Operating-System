#include "synchconsole.h"


/* Interrupt handler called when a character arrives from the keyboard.
   Increments the read semaphore. */
static void readHandler(void *console) {
  (SynchConsole*)console->ReadAvail();
}

/* Interrupt handler called when console's PutChar I/O completes.
   Increments the write semaphore. */
static void writeHandler(void *console) {
  (SynchConsole*)console->WriteDone();
}

SynchConsole::SynchConsole() {
  write = new Semaphore("Done Writing", 0);
  read = new Semaphore("Can Read", 0);

  writeLock = new Lock("Synch Console Writer Lock");
  readLock = new Lock("Synch Console Reader Lock");

  /* Initialize the console and register the interrupt handlers.
   * NULL values tell the console to use stdin/stdout.
   */
  console = new Console(NULL, NULL, readHandler, writeHandler, this); 
}

SynchConsole::~SynchConsole() {
  delete write;
  delete read;

  delete writeLock;
  delete readLock;

  delete console;
}

char SynchConsole::ReadChar() {
  readLock->Acquire();
  
  read->P(); // decrement semaphore
  char c = console->getChar(); // get character from console

  readLock->Release();
  return c;
}

void SynchConsole::ReadLine(char *buffer) {
  readLock->Acquire();
  char c;

  // read all characters and write to buffer
  while ( (c = console->GetChar()) != EOF ) {
    read->P(); // decrement semaphore
    *buffer = c;
    *buffer++;
    
  }

  readLock->Release();
}

void SynchConsole::Read(char *buffer, int size) {
  readLock->Acquire();
  char c;

  // read {size} characters and write to buffer
  for (int i = 0; i < size; ++i) {
    read->P(); // decrement semaphore
    *buffer = c;
    *buffer++;
  }

  readLock->Release();
}

void SynchConsole::WriteChar(char c) {
  writeLock->Acquire();

  console->PutChar(c);
  write->P(); // decrement semaphore

  writeLock->Release();
}

void SynchConsole::WriteLine(char *buffer) {
  writeLock->Acquire();

  // write buffer to console until null character is reached
  while (*buffer != '\0') {
    console->PutChar(buffer[0]);
    buffer++;
    write->P(); // decrement semaphore
  }

  writeLock->Release();
}

void SynchConsole::Write(char *buffer, int size) {
  writeLock->Acquire();

  // write {size} characters from buffer to console
  for (int i = 0; i < size; ++i) {
    console->PutChar(buffer[0]);
    buffer++;
    write->P(); // decrement semaphore
  }
}

void SynchConsole::CheckCharAvail() {
  read->V();
}

void SynchConsole::WriteDone() {
  write->V();
}
