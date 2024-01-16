[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/SQs7pKlr)
# Word Bomb

### gaor-chine

Rachel Gao  
Edmund Chin
       
### Project Description:

This is a command line recreation of the word bomb game where players take turns guessing words that contain a randomly generated phrase until one player is left standing.
  
### Instructions:

###### How does the user install/compile/run the program.  
To install the program, git clone from https://github.com/Stuycs-K/project03-final-9-gaor-chine  
To compile the program, make compile  
To run the program as the server: make server or ./server  
To run the client and connect to a local server: make compile or ./client  
To run the client and connect to a server on another computer: make compile ARGS="insert_ip_here" or ./client insert_ip_here
###### How does the user interact with this program?  
clients connecting to a server will be prompted to enter a name initially. Once players have done that, they will be placed in a lobby where players can chat by typing into the cli and execute commands such as /start, /howtoplay, and /help. Once the game starts, players can enter word entries if it's their turn, or chat if it isn't their turn. The game will continue until one player is left.
