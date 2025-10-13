The goal was to make a GameBoy game based on the old Pokemon ice slide puzzles. You walk onto ice and continue sliding until you hit a wall, and then you stop and get to pick a new direction to slide, repeat until you reach the goal.

<img width="700" height="516" alt="image" src="https://github.com/user-attachments/assets/5a4c0ce0-5fdb-4451-9447-13d85b2aaa14" />

It's coded in assembly. The game waits for input, then it will continuously move in that direction until it hits a wall, then it will wait for input again. 

I learned A LOT for this project. I had to research a lot about the Gameboy and its hardware. The level data is stored in a linked list of structs that hold references to locations that hold actual graphics data. Big endian vs. little endian confused me for a while. 

<img width="368" height="362" alt="image" src="https://github.com/user-attachments/assets/2b0e61c8-c40d-4c05-a7a5-313260b87c59" />

<img width="368" height="362" alt="image" src="https://github.com/user-attachments/assets/68138af8-f301-43d1-a136-4f24061b2c6f" />

