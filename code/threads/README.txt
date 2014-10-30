Members of Group_44
                Ming Qin        Xinyu Qian        Evan Carey        Kevin Caasi
                --------        ----------        ----------        -----------
 Worked on:     Join()           Whales()          Join()            Join()
                Mailbox()        CV                 CV               CV
                Testing          Mailbox()         Locks()           Locks()
                Priority()       Testing           Testing           Testing



Locks And Conditions
--------------------
    We implemented acquire and release using interrupts, asserts, wait and
    ready queue. Similarly, condition variables use interrupts, wait and ready
    queues, and locks. Instead of appending to the wait queue, we use
    SortedInsert() to prioritize the threads.
    Test switch case numbers used:
        2: Expects to work successfully
        3: Assert should fail: Cannot delete a held lock by own thread
        4: Assert should fail: Cannot delete a held lock by another thread
        5: Assert Should fail: Cannot wait without holding a lock
        6: Expects to work successfully
        7: Expects to work successfully
        8: Assert fails: Cannot delete while threads on queue


Mailbox
--------------------
    We used locks, and condition variables canSend and canReceive as well as a
    bool named full. we ensure interleaving by inversion calling the condition
    variables in both send and receive. Variable buff holds the word, and full
    ensures that only one sender interacts with one receiver.
    test switch case numbers used:
        9: Expects to work succesfully as sender comes first
        10:Expects to work succesffully as receiver comes first
        11:Expects to work. Only sender is available
        12:Expects to work. Only receiver is available
        13:Expects to work. Interleaving between senders and receivers


Join
--------------------
   

   test switch case numbers used:
        21: a thread that will be joined only is destroyed once Join has been called on it
        22: if a parent calls Join on a child and the child is still executing, the parent waits
        23: if a parent calls Join on a child and the child has finished executing, the parent does not block
        24: a thread does not call join on itself 
        25: join is only invoked on threads created to be joined
        26: join is only called on a thread taht has forked
        27: join is not called more than once on a thread


Priorities
--------------------
    We worked on priorities by utilizing the SortedInsert() function in list.
    However, this function originally sorts by ascending order. We want descending
    order. For this, we simply pass in a negation of the priority to the function
    to achieve the expected results.
    test switch case numbers used:
        19: Expects to work successfully. threads run in priority order


Whales
--------------------


