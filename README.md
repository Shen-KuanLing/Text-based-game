# Text-based-game

### ● Project Description

This is a game that allows player to go anywhere on the map. Player may 
encounter different things on the map, and he/she has to make decision or do 
some actions to them.

For example:
| **Things player encounters**| **Actions that player could do** |
|-----------------------------|:--------------------------------:|
| Meet the monster   | Player has to choose which level of the monster he/she will fight |
| Visit the village  | Player could:<br>(1) Buy a magic sword(strength++)<br>(2) Go to the bar(blood++) |
| Encounter a gun    | Player gets a gun(strength++)  |
| Encounter a trap   | Player falls into a trap(blood--) |


### ● Playing Method

When the game starts, player need to first decide the size of the map by entering the row and column values. Then we’ll initialize locations for village, traps, monsters, and guns. Moreover, we ask the player to set up the location for himself/herself. After that, the player can start traveling on the map. The player can go up, down, left, right by entering the 1, 2, 3, 4 number. 


If the player encounters the monster, we ask the player to choose the level of the monster he/she wants to fight. The level ranges from 1 to 4, which the player chooses by entering the corresponding number.
If the player visits the village, he/she can choose whether to buy a magic sword by entering 1, to go to the bar by entering 2, or to leave the village by entering 3.


The game ends either when the player enters 5 to exit the game, or when his/her blood is equal to or less than 0, which means the player deads so that he/she can’t go on traveling on the map.


### ● Version History

0.1 - Initial Release
