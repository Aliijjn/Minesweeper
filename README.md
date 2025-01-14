# Minesweeper ⛏️🧨
Welcome to Minesweeper! My take on the classic game, using my own basic graphics library built on top of `Windows.h`. When starting the game, you're prompted to select a field size and difficulty. You can either choose a premade setting or make a custom one. Then the game starts, and you're shown a blank mine field. Left-click a square to uncover it, and don't worry, the first move is always on an empty square. After that, you can start looking for mines and "flag" them by right-clicking on them, this will ensure that you don't accidentally blow yourself to bits. The game ends once you uncover all non-mines, or when you hit a mine. Enjoy!

## What I learned from this project ✅
- How to use and build upon the `Windows.h` API
- How to parse, load and display images onto the screen
- How to handle input from a mouse
- Cleanly parsing user input in C++

## Images 📷
A simple level with some revealed squares

![afbeelding](https://github.com/user-attachments/assets/e7398848-ecbd-4076-9e81-243da220c697)

You can mark a blank square by pressing the right mouse button

![afbeelding](https://github.com/user-attachments/assets/82793d49-f197-41c8-b39d-1c74d077df8f)

Mistakes were made....

![afbeelding](https://github.com/user-attachments/assets/e3fa1688-91a8-4267-84b8-df6f533b024b)

My version of Minesweeper supports fields as big as your monitor can handle, so sweep to your heart's content!

![afbeelding](https://github.com/user-attachments/assets/c06e723f-99b5-4adb-a608-ad5d9f429c23)

As well as this, my Minesweeper has premade and custom field sizes and difficulties

![afbeelding](https://github.com/user-attachments/assets/29b0088a-76d5-4cdc-ac59-824ea1a1238a)

An easy 20x20 grid versus a hard 20x20 grid

![afbeelding](https://github.com/user-attachments/assets/a7c58f0d-6a75-4eb2-ba54-40772950bee2)
![afbeelding](https://github.com/user-attachments/assets/b43b396d-e3e6-4dc1-8c98-8e04a4f46c49)

And no matter the difficulty of the level, it's guaranteed that your first move leaves you intact (this is done by initialising the level only after the first click)

![afbeelding](https://github.com/user-attachments/assets/8d109560-8fc3-4507-a541-89de58119cbd)

## Made by Alijn Kuijer, September 2024
