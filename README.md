# Simple Text Adventure Game

## How to execute?

Start by opening up your command line and navigate to the main folder.
Make the main.cpp:
```
g++ main.cpp -o text_adventure_game
```
Open the game by choosing a map as an argument:
```
text_adventure_game map4.json
```
Available maps by default: map1-6.json

## How to play?
Player interacts with the game using text commands. The game responds also with text commands. The objective of the game is not stated beforehand - the player has to explore the map.
You can find out the available commands by yourself or here is the full list:
<details>
  <summary>Spoiler warning</summary>
  
 ```
  look <item> 
  go <direction> 
  take <item>
  kill <enemy>
  quit
 ```
</details>

## Maps

For the starters there is 6 simple maps available. They are .json files, therefore one can create as many new maps with his own enemies, objectives and items, as long as they are kept in the same structure.

## Further development

Currently the project development is stopped. The next work would include enhancing the game by adding new functionalities, such as successful or unsuccessful messages which are in map5 or map6 and overall adding more structure to the game.
