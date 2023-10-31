
## Naming Server (NM)
Upon initializing a naming server, two threads are created, 
-	`client_thread` handles the connections coming from clients on the client port 
-	`server_thread` handles the connections coming from SS  on the server port
-	Each of the above two threads in-turn create a new thread whenever a new connection has been made with their respective handlers.

## Defining entry struct for SS
Since structures can't be passed via the TCP socket without using pragma for packaging and defining a serealizing and deserealizing method for every form of communication proves to be a tedious task, we have resorted to making our entries of fixed size without the involvment of pointers. The same tradition is followed accross all the structures.

## Initializing SS
- Every SS has the port, id and absolute path in its ``main`` function
- These arguments are passed into the ```server_entry``` function which connects the SS to the NM
- Once this initialization is done, SS is ready to accept clients via the ```init_server``` method.
- **TODO**:  Permission mechanism

## Handeling Clients (SS)

The SS expects argc and argv from the clients. The first argument is the operation the client wants to perform typically, for now the second argument is the path (Second arguement maybe something else depending on the path first argument). It is assumed that the command is correct from the input checker on the client side

## Client input
Client input is handeled mainly via the struct of `command` , this is send to the NM, it contains the `path` its looking for as its last argument.
>Here we have finally defined that the commands will always have the **path** its looking for in the last argument of the input.

**TODO**-  The NM will now look up the last argument using `argc` variable, and check if this `path` is present in its entries or not and return the `ID` of the SS it belongs to. (Handeled in `cthreads.c`) 

**TEMP**- for testing purposes this is set on 0 by default when only one server is active.
