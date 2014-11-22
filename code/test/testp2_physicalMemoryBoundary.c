#include "syscall.h"

int
main ()
{
    int result;
    
    result=Exec("../test/testp2_tableBoundary",0,0,0);
    
    //start again
    
    //result=Exec("../test/testp2_tableBoundary",0,0,0);
        
    Exit(result);
}
