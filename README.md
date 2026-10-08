# IS1200-EXPO-PONG

## Instructions for installing and playing the game

You need:
- DTEK-V board
- VGA cable
- VGA display
- USB printer cable

### Running the game on the DTEK-V board

1. Open the directory containing the project files.
2. Run the command:
   ```bash
   make
   ```
3. Run the command:
   ```bash
   jtagd --user-start
   ```
4. Press the reset button on the board.
5. Run the command:
   ```bash
   dtekv-run main.bin
   ```
### Game controls

To move the paddles, use switches **8 and 9** for the left paddle and **0 and 1** for the right paddle.

- **Move up:** Both switches ON.
- **Move down:** Both switches OFF.
- **Stop moving:** One switch ON and the other OFF.

To reset the score, use **switch 6**. To pause the game, use **switch 7**.
