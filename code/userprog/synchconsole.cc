#include "synchconsole.h"


/* Interrupt handler called when a character arrives from the keyboard.
   Increments the read semaphore. */
static void readHandler(int console) {
	( (SynchConsole*)console )->CheckCharAvail();
}

/* Interrupt handler called when console's PutChar I/O completes.
   Increments the write semaphore. */
static void writeHandler(int console) {
	( (SynchConsole*)console )->WriteDone();
}

SynchConsole::SynchConsole() {

	write = new Semaphore("Done Writing", 1);
	read = new Semaphore("Can Read", 0);

	writeLock = new Lock("Synch Console Writer Lock");
	readLock = new Lock("Synch Console Reader Lock");

	/* Initialize the console and register the interrupt handlers.
	 * NULL values tell the console to use stdin/stdout.
	 */
	console = new Console(NULL, NULL, readHandler, writeHandler, ( int )this);
}

SynchConsole::~SynchConsole() {
	delete write;
	delete read;

	delete writeLock;
	delete readLock;

	delete console;
}

/* Synchronously read a character from the console */
char SynchConsole::ReadChar() {
	readLock->Acquire();

	read->P(); // decrement semaphore
	char c = console->GetChar(); // get character from console

	readLock->Release();
	return c;
}

/* Synchronously read a line from the console */
void SynchConsole::ReadLine(char *buffer) {
	readLock->Acquire();
	char c;

	// read all characters into buffer
	while (( c = console->GetChar() ) != EOF) {
		read->P(); // decrement semaphore
		*buffer++ = c;
	}

	readLock->Release();
}

/* Synchronously read {size} characters from the console */
void SynchConsole::Read(char *buffer, int size) {
	readLock->Acquire();
	char c;

	// read {size} characters into buffer
	for (int i = 0; i < size; ++i) {
		read->P(); // decrement semaphore
		c = console->GetChar();
		*buffer++ = c;
	}

	readLock->Release();
}

/* Synchronously write a character to the console */
void SynchConsole::WriteChar(char c) {
	writeLock->Acquire();

	write->P(); // decrement semaphore
	console->PutChar(c);

	writeLock->Release();
}

/* Synchronously write a line to the console */
void SynchConsole::WriteLine(char *buffer) {
	writeLock->Acquire();

	// write buffer to console until null character is reached
	while (*buffer != '\0') {
		write->P(); // decrement semaphore
		console->PutChar(*buffer);
		buffer++;
	}

	writeLock->Release();
}

/* Synchronously write {size} character to the console */
void SynchConsole::Write(char *buffer, int size) {
	writeLock->Acquire();

	// write {size} characters from buffer to console
	for (int i = 0; i < size; ++i) {
		write->P(); // decrement semaphore
		console->PutChar(buffer[i]);
	}

	writeLock->Release();
}

/* Increments the read semaphore */
void SynchConsole::CheckCharAvail() {
	read->V();
}

/* Increments the write semaphore */
void SynchConsole::WriteDone() {
	write->V();
}
