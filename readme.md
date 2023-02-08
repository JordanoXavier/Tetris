# Tetris game recreated using "Canvas 2D", a graphical API created by Professor Cesar Tadeu Pozzer, over the OpenGl/Glut API.


## Commands:

 Arrow left and right: move the piece in the direction of the arrow
 Down arrow: moves the piece down
 Up Arrow: Rotate the part (clockwise)


<div align="center">
<img src="https://user-images.githubusercontent.com/112017191/217627969-15a982cc-db7a-41b6-9a51-85c7b8cd15a9.png" width="700px" />
</div>


## Code operation:

 The game board is a 20x10 matrix that receives the following values:
 0: empty tiles (nothing is drawn on the screen)
 negative values: represent the moving pieces, when touching the base they are multiplied by -1 and become fixed pieces
 -1 to -5: these are the 5 game pieces
 -12 to -15: are the rotational axis (square does not have it) of the pieces of their respective units (they have the same color as these)
 positive values: represent fixed parts
 1 to 5: are the 5 fixed game pieces
 12 to 15: were the rotational axis of the parts of their respective units when they were in
 Each piece has its own color, which is the same for its negative and positive values and its axis of rotation

 Rotation is done by generating and rotating a matrix of smaller size around the axis of rotation of the moving part
 There are different rotation patterns:
 Square: don't rotate because it's not necessary
 line Part: a 5x5 matrix is generated around its axis
 Other pieces: a 3x3 matrix is generated around


## Implemented features:

 Game scenario
 Part movement and rotation
 Point counter
 Removal of complete lines
 Pieces above a removed line are lowered by one level.
 Main game start menu.
 Adjustment of the speed that the piece falls (increasing as the game progresses)
 Variable and responsive screen size
 Game over and reset
 Next tile covered in the corner of the screen

