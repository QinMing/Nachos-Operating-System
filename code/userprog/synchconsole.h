#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "synch.h"
#include "console.h"

class SynchConsole {
 private:
  Semaphore *write; // semaphore used for writing
  Semaphore *read;  // semaphore used for reading
  Lock *writeLock;  // lock used for writing
  Lock *readLock;   // lock used for reading
  Console* console; // the underlying asynchronous console (see ../machine/console.h)

 public:
  SynchConsole();
  ~SynchConsole();

  /* Synchronously read a character from the console */
  char ReadChar();

  /* Synchronously read a line from the console */
  void ReadLine(char *buffer);

  /* Synchronously read {size} characters from the console */
  void Read(char* buffer, int size);

  /* Synchronously write a character to the console */
  void WriteChar(char ch);

  /* Synchronously write a line to the console */
  void WriteLine(char *buffer);

  /* Synchronously write {size} character to the console */
  void Write(char* buffer, int size);

  /* Increments the write semaphore */
  void WriteDone();

  /* Increments the read semaphore */
  void CheckCharAvail();

};

#endif // SYNCHCONSOLE_H
