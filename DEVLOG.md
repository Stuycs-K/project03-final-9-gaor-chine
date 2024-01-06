# Dev Log:

## Rachel Gao

### 2024-01-02 - Found Partner

### 2024-01-03 - Wrote Up Proposal
I discussed possible project ideas in class (20 mins) with my partner. We set up a time to call and to discuss further about our plans for the project. We wrote up the proposal together in call, which lasted for 2 hours.

### 2024-01-04 - Multiple Clients Somewhat Works (~2 hrs)
- imported old code from previous socket lab
- edited code to allow multiple clients to connect using select
- tested multiple clients and works but did not implement checking for when clients exit yet

### 2024-01-05 - Disconnections and client stdin
- added select to client to check for server status
- server and clients can tell if each other have disconnected (server removes client from fds, client exits)
- clients can write to server using stdin and server returns processed string


## Edmund Chin

### 2024-01-02 - Found Partner

### 2024-01-03 - Wrote Up Proposal
I discussed possible project ideas in class (20 mins) with my partner. We set up a time to call and to discuss further about our plans for the project. We wrote up the proposal together in call, which lasted for 2 hours.

### 2024-01-04 ~ parsing files but better - almost done? (~1hr 30m)
- imported old exec lab code
- created a parse command to exec grep to search for a specific word in a file rather than a long while loop, will add more checks to make sure the entered word follows all rules
- learned about grep and its flags
