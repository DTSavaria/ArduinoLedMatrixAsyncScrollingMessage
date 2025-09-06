/*
 * ArduinoLedMatrixAsyncScrollingMessage BasicExample
 * Copyright (c) 2025 Daniel Savaria
 * Demonstrates scrolling a message asynchronously
 * Additionally shows how to make the message loop
 */

// these are the required built-in libraries
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <TextAnimation.h>

// this has to be done before including AsyncScrollingMessage.
// can make this number smaller to use less memory, but messages
// created using "new AsyncScrollingMessage" will have to be shorter,
// messages created using generateMessages can be any length
#define MAX_CHARS 100
TEXT_ANIMATION_DEFINE(anim, MAX_CHARS)

// after TEXT_ANIMATION_DEFINE, include the AsyncScrollingMessage class
#include <AsyncScrollingMessage.hpp>

// Create a connection to the matrix
ArduinoLEDMatrix matrix;

// this flag is used to indicate when the message is done scrolling
bool requestNext = true;

// a pointer to the message
AsyncScrollingMessage* message;

// a pointer to the current message to show
AsyncScrollingMessage* current;

// CUSTOMIZATION NOTE: set this to false to display the message once and then stop
// set it to true to loop the message over and over
bool loopMessage = true;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize the led matrix
  // callback will be called when a scrolling message is done
  matrix.begin();
  matrix.beginDraw();
  matrix.textScrollSpeed(60);
  matrix.setCallback(matrixCallback);

  // create a short message with the constructor, it will be limited to
  // the size defined by MAX_CHARS, see the official Arduino built-in example
  // for more info: LED_Matrix > TextWithArduinoGraphicsAsynchronous
  // generateMessages could also have be used here,
  // see BasicLongExample for how to use that
  message = new AsyncScrollingMessage(
    "   Hello, from Async ", matrix, Font_5x7);
  current = message;
}

// this is called automatically when the async message is done scrolling
// it is used to set a flag that will be handled in the loop function
void matrixCallback() {
  requestNext = true;
}

// these variables are for quickly blinking an led in an async.
// this is to show arduino can do other things while the message is scrolling.
unsigned long previousBlink = 0;
const long blinkInterval = 250;
PinStatus ledState = LOW;

void loop() {
  // check the flag if ready for the next message. this flag is false
  // while the message is scrolling, but will become true when the message
  // has completed scrolling
  if (requestNext) {
    requestNext = false;  // mark that the message was handled

    // in this example, if loopMessage was set to false, this if statement will
    // be true only once, and the message will be scrolled once, then become false
    // if loopMessage was set to true, next will evaluate true every time
    // and the message will scroll over and over while the arduino is running
    if (current != nullptr) {

      // make sure the callback is set so that the requestNext flag is properly set
      matrix.setCallback(matrixCallback);

      // show the scrolling message
      message->showMessage();

      // in this demo, if it isn't set to loopMessage, set current to nullptr,
      // then it will know there a no more messages when it is done scrolling once
      // if it is set to loop messages, then don't do anything here
      // when the message scroll completes, it will scroll again
      if (!loopMessage) {
        current = nullptr;
      }
    } else {
      // in this demo, if current is nullptr, then there's no more messages
      // delete the message memory and show a static message
      showText(":D");
      delete message;
      message = nullptr;
    }
  }

  // get the current processor time
  // check the led timer to see if it should blink
  unsigned long currentTime = millis();
  if (currentTime - previousBlink >= blinkInterval) {
    previousBlink = currentTime;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(LED_BUILTIN, ledState);
  }
}

// helper code to show a short static text
void showText(const String& text) {
  // when showing a non-scrolling message, the callback must be cleared
  matrix.setCallback(nullptr);

  // basic code to some characters to the LED Matrix
  matrix.beginDraw();
  matrix.clear();
  matrix.stroke(0xFFFFFFFF);
  matrix.beginText(1, 1, 0xFFFFFF);
  matrix.print(text);
  matrix.endText();
  matrix.endDraw();
}
