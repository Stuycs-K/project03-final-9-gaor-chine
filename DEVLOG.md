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

### 2024-01-06 - Struct player and username
- changed array of socket descriptors to array of struct player pointers and adjusted code accordingly
- created a username input when client connects
- server knows username of clients and associated socket descriptor through struct player array
- added a welcome message with player name (will be expanded to include more information, ex. commands)
- known bug that server will block/wait for usernames when multiple clients connect (may need forks)

### 2024-01-08 - Commands and chat
- server can distinguish commands vs chat (commands start with prefix '/')
- server can send chat log to all clients
- created /help to show all commands, noted on welcome message

### 2024-01-09 - Start command
- discovered bug where clients chatting will interrupt viewing of other clients (create breaks)
- added game_status to track progress of game, changes behavior of functions
- tried to make start unavailable after game is in progress but parent program stalls and blocks commands

### 2024-01-10 - Timeout with select, added prompts
- created skeleton/beginnings of timer system using select timeout argument
- game status changes between select timer and regular timer
- created game end condition (1 client connected while game is in progress)
- compiled 2/3 letter prompts to prompts.txt for later use

### 2024-01-11 - Timer and testing early gameplay
- used timeout function of select to track while game is in progress
- created current player and a next player function to switch timeout effects
- created preliminary word checker for correct/incorrectness and effects
- bug where the timer is not reset when game starts (timer afterwards works as intended though)

### 2024-01-12 - Debugging and lives mechanic
- Most of all known bugs fixed:
  - timer is now reset when the game starts
  - connections between different computers is successful
  - fixed inputs being printed twice into client
  - fixed server printing help command
- added lives markers before names when chatting during a game (changes as lives change)
- edited some strings for better readability

### 2024-01-13 - Helper functions and more debugging
- created helper function for checking the word inputted
- spent a lot a lot of time trying to debug random characters being printed, unsucessful
  
### 2024-01-14 - Debugging
- spent a lot a lot of time trying to debug random characters being printed, unsucessful
- discovered another bug, not sure if related to above, random sockets are added

### 2024-01-15 - Debugging, more input checks, game end conditions, limit connections
- finally managed to fix the bug, honestly dont know where it went wrong
- added new command /howtoplay to show rules and how to play
- added checks for the minimum length of an input and for the input to contain non-case-sensitive prompt
- added disconnect upon losing all lives/death
- closed connections made after game is in session

## Edmund Chin

### 2024-01-02 - Found Partner

### 2024-01-03 - Wrote Up Proposal
I discussed possible project ideas in class (20 mins) with my partner. We set up a time to call and to discuss further about our plans for the project. We wrote up the proposal together in call, which lasted for 2 hours.

### 2024-01-04 ~ parsing files but better - almost done? (~1hr 30m)
- imported old exec lab code
- created a parse command to exec grep to search for a specific word in a file rather than a long while loop, will add more checks to make sure the entered word follows all rules
- learned about grep and its flags

### 2024-01-05 - piping grep
- decided to implement pipe to obtain the output of grep
- tried fixing bugs, still in process, ready to implement the last check for used words once bug is fixed

### 2024-01-08 - redirection is not redirecting
- tried to debug the interaction between grep and redirecting it into a text file
- used last project as a base for the code but still redirects a unidentifiable string
