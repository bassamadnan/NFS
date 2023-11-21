
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

## Recieving and Sending
tldr; had to serialize and deserialize structs :(
As mentioned earlier, we avoided serializing/deserializing of structs, but this came with problems of their own. Functions `send/recv_entry/command()` have been added for now to deal with the sending and reciving of data. ACK `int(s)` have also been sent perhaps as a temporary palceholder for ACKS in the future (**TODO?**)
**REMEBMER** - premature free causing socket file descriptor to change. handle later
## Adding privileged commands

So far we have been treating all the commands of the client as privileged. That is not the case however since some commands require privilge to execute (`Copy/Delete/Create`). Functionalities to check if a command is privileged have been added alongside handling them directly via the NM. The NM finds the SS this command is intended for and forwards the command to the SS via the socket it had obtained.
> Sockets are tracked using SS_stat structure which gives the status of connections for every server having an entry with the NM. It contains two attributes for now, the socket of this SS and the connection status of this SS.

Threads for listening to NM commands have been added in the SS code which listens via the socket it initially connected to during its entry.
