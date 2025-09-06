# Arduino LED Matrix Async Scrolling Message
A library to make it easier to display scrolling messages on the Arudino Uno R4 LED matrix asynchronously--meaning the Arudino can run other code while the message is scrolling.

This library builds on top of the functionality that is built into the official Arduino API (see the LED_Matrix > TextWithArduinoGraphicsAsynchronous example in the Arduino IDE). This library simplifies the process a bit and adds additional functionality such as looping text and creating messages that are longer than the normal limit.

To use this library, place this directory into your `Arduino/libraries` directory. 
Then select it in the Arudino IDE by going to Sketch > Include Library > (Contributed libraries) > ArduinoLedMatrixAsyncScrollingMessage  

Examples can be found by using the Arduino IDE to go to Files > Examples > (Examples from Custom Libraries) > ArduinoLedMatrixAsyncScrollingMessage

This has been tested with the Arduino Uno R4 Wifi.  
