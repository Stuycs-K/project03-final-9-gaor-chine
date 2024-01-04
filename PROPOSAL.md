# Final Project Proposal

## Group Members:

Rachel Gao  
Edmund Chin
       
# Intentions:

We are recreating the Word Bomb game in which many players take turns to submit words that contain a certain sequence of characters.
    
# Intended usage:
 
The player can input "commands" in the command line to:
- see available rooms
- join/create a room
- exit a room
- start the game (as host)  

The players play the game by submitting words in the command line.  
  
# Technical Details:
   
## Topics Used:  
- Allocating memory by creating a struct for players, containing information about their name and the lives they have.  
- Working with files and processes (exec) to parse/read through a text file of words to check for valid words.  
- Fork to create subservers that handle each room.  
- Sockets to communicate information between the servers and the clients/players.  

## Breakdown of Project:  
General:  
- Creating connection between server and client using sockets (Rachel)
- Implementing struct for player, including name and number of lives (Edmund)

Server-side:  
- Forking servers to create rooms (Rachel)
- Implementing timer for the bomb to explode and deduct lives (Edmund)
- Server recognizing win condition (one player left alive) and close the room (Edmund)

Client-side:  
- Client commands to start the game and send words to the server (Rachel)
- Implementing exec to parse through the text file of words to check for valid words (Edmund)

## Data Structures:  
- Char arrays (strings): send information between server and client, parse through a text file, player name
- int: to record the number of lives, number of players, timer for the bomb
- user-defined struct "player": player name, to record the number of lives

## Algorithms: N/A
    
# Intended pacing:

## Timeline:  
- 1/5: Creating connection between server and client using sockets
- 1/5: Implementing exec to parse through the text file of words to check for valid words
- 1/7: Client commands to start the game and send words to the server
- 1/8: Implementing struct for player, including name and number of lives
- 1/11: Implementing timer for the bomb to explode and deduct lives
- 1/13: Server recognizing win condition (one player left alive) and close the room
- 1/15: Forking servers to create rooms, allow clients to join/leave rooms
