# SimonSays
The Simon says game from Among Us recreated in real life.

## Teammates
&ensp;Joshua Liska
&ensp;Paysen Bramblett
&ensp;Fernando Hernandez-Borboa

## Video Demonstration
[![Alt text](https://img.youtube.com/vi/FVwlfHnqfag/0.jpg)](https://youtu.be/FVwlfHnqfag)

## Concept of Research
&ensp;The concept for this project was to recreate the minigame from Among Us known as Simon Says. In this game you must reply to the given sequence with the identical numbers. The LED array is used to denote 1-9 and the numpad follows in the same pattern. The pattern is non-repeatable in our game as we wished for the combination to be unique. The query is denoted as a five digit array with the sequence: 
array[0] ... response ... array[0], array[1] ... response ... array[0], array[1], array[2] ... array[0], array[1], array[2], , array[3] ... response ... array[0], array[1], array[2], , array[3], array[4] ... response ... success 
If the array is not properly responded to with the identical array at any point then the system is thrown into the failure state and must be reset.
The intent of this game was to create a simple game that was fun to play and fairly easy to build.

## Future Designs and Continuation
&ensp;The main continuation that would be made to this project would be recreating the UI the player interacts with. Changing the 9 digit button input into a non-digit interface with its own blank keycaps and the LED system to be identical to it so that it is more aesthetically pleasing.

## Images of Concepts
<img src="https://github.com/jjliska/SimonSays/blob/master/Media/Pictures/20201128_112800.jpg" width="320" height="480">
<img src="https://github.com/jjliska/SimonSays/blob/master/Media/Pictures/20201128_112838.jpg" width="320" height="480">
<img src="https://github.com/jjliska/SimonSays/blob/master/Media/Pictures/20201128_112850.jpg" width="320" height="480">
<img src="https://github.com/jjliska/SimonSays/blob/master/Media/Pictures/FinishedProduct.jpg" width="480" height="480">

## References
keypadExample01 from Arduino IDE  
Part_1_03_Playing_Music from Arduino in Teensy Library  
[RGB LED tutorial](https://howtomechatronics.com/tutorials/arduino/how-to-use-a-rgb-led-with-arduino/) 
