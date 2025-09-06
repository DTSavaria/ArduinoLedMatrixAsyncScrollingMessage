/*
 * ArduinoLedMatrixAsyncScrollingMessage TakeActionBetweenMessages Example
 * Copyright (c) 2025 Daniel Savaria
 * Demonstrates scrolling multiple messages asynchronously
 * and displaying a static message between each one
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
#include "AsyncScrollingMessage.hpp"

// Create a connection to the matrix
ArduinoLEDMatrix matrix;

// this flag is used to indicate when the message is done scrolling
bool requestNext = true;

// a pointer to the first message
AsyncScrollingMessage* messages = nullptr;

// pointers to the previous and current message
AsyncScrollingMessage* previous = nullptr;
AsyncScrollingMessage* current = nullptr;

// CUSTOMIZATION NOTE: set this to false to display the messages once and then stop
// set it to true to loop the messages over and over
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
  // the size defined by MAX_CHARS, see the official Arduino built in example
  // for more info: LED_Matrix > TextWithArduinoGraphicsAsynchronous
  // generateMessages could also have be used here
  messages = new AsyncScrollingMessage("   Hello, from async", matrix, Font_5x7);

  // create a long message with the generateMessage helper
  // this message is too long for one async call, so it will create multiple
  // messages that are marked as having a continuation
  messages->insertNext(AsyncScrollingMessage::generateMessages(
    "    123456789a123456789b123456789c1234567890d1234567890e1234567890g",
    matrix, MAX_CHARS, Font_4x6));

  current = messages;
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

// these are to display short text in between scrolling messages
bool showInterstitial = false;
bool showingInterstitial = false;
unsigned long stopInterstitial = 0;
const long interstitialInterval = 2000;
String interstitialText = ":)";

void loop() {
  // get the current processor time
  unsigned long currentTime = millis();

  // check the flag if ready for the next message. this flag is false
  // while the message is scrolling, but will become true when the message
  // has completed scrolling
  if (requestNext) {
    requestNext = false;  // mark that the message was handled

    // if not set to loop the messages, the previous message can be deleted
    // from memory since it won't be used anymore
    if (!loopMessage && previous != nullptr) {
      delete previous;
      previous = nullptr;
    }

    // check if it should show a short message between the scrolling messages
    // or if it's ready for the next scrolling message
    if (showInterstitial) {

      //set up the values for the short message
      showInterstitial = false;
      showingInterstitial = true;
      stopInterstitial = currentTime + interstitialInterval;
      showText(interstitialText);
    }

    // in this example, if loopMessage was set to false, this if statement will
    // evaluate true only until all messages have been scrolled once,
    // and then become false
    // if loopMessage was set to true, next will evaluate true every time
    // and the message will scroll over and over while the arduino is running
    else if (current != nullptr) {

      // first check if this is the middle of a multipart message
      // if it isn't, then set a flag to show a short text
      //  when this message is done scrolling
      showInterstitial = !current->hasContinuation();

      // make sure the callback is set so that the requestNext flag is properly set
      matrix.setCallback(matrixCallback);

      // show the scrolling message, also grab the next message.
      // in this demo, getNext will return a valid pointer until all
      // messages have been shown
      current->showMessage();
      previous = current;
      current = current->getNext();

      // if the next message in a nullptr, but loopMessage is true, then
      // queue up the first message to show everything again
      if (current == nullptr && loopMessage) {
        current = messages;
      }

    } else {

      // if current is a nullptr, (in this demo, this happens when the original
      // messages have finished scrolling and loopMessage was set to false).
      // then there are no more messages to display,
      // so show a static message
      showText(":D");
    }
  }

  // if the short text is being shown and it's timer is up, then
  // set the flags needed to display the next scrolling message
  if (showingInterstitial && currentTime > stopInterstitial) {
    showingInterstitial = false;
    requestNext = true;
  }

  // check the led timer to see if it should blink
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
