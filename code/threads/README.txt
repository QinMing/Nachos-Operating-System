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


Mailbox
--------------------
    We used locks, and condition variables canSend and canReceive as well as a
    bool named full. we ensure interleaving by inversion calling the condition
    variables in both send and receive. Variable buff holds the word, and full
    ensures that only one sender interacts with one receiver.



Join
--------------------
    


Priorities
--------------------
    We worked on priorities by utilizing the SortedInsert() function in list.
    However, this function originally sorts by ascending order. We want descending
    order. For this, we simply pass in a negation of the priority to the function
    to achieve the expected results.


Whales
--------------------

