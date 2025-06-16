# car_Dodging_game
A simple 2D car dodging game built with SFML (Simple and Fast Multimedia Library) in C++. The player controls a car that must avoid oncoming traffic while racing down a road. The game includes a main menu, leaderboard tracking, and increasing difficulty over time.

# Controls
Key	Action  
W	Move up  
S	Move down  
A	Move left  
D	Move right  
↑ / ↓	Menu navigation  
Enter	Select / Confirm  
Esc	Exit the game  

# Game Logic Overview
Game Loop: Runs continuously until the window is closed. It updates enemy car positions, background scrolling, and collision detection.
Collision: If the player hits an enemy car, the game ends and the score is recorded.
Leaderboard: Top scores (name + score) are saved in scores.txt and shown in the "Leaderboard" menu.
Increasing Difficulty: Speed increases every 5 seconds.

# Author
Created by M.hashir

