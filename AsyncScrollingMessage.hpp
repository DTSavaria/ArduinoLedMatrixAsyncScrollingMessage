#ifndef _ASYNC_SCROLLING_MESSAGE_HPP_
#define _ASYNC_SCROLLING_MESSAGE_HPP_

/**
 * AsyncScrollingMessage
 * Copyright (c) 2025 Daniel Savaria
 *
 * A class to make it easier to display scrolling messages on the Arudino Uno
 * R4 LED matrix asynchronously--meaning the Arudino can run other code while
 * the message is scrolling.
 * 
 * This class builds on top of the functionality that is built into the
 * official Arduino API (see the LED_Matrix > TextWithArduinoGraphicsAsynchronous
 * example in the Arduino IDE). This library simplifies the process a bit and
 * adds additional functionality such as looping text and creating messages
 * that are longer than the normal limit.
 */
class AsyncScrollingMessage {
public:

  AsyncScrollingMessage(
    const String& message,
    ArduinoLEDMatrix& matrix,
    const Font& font)
    : message(message),
      matrix(matrix),
      font(font),
      hContinuation(false),
      iContinuation(false),
      next(nullptr) {
  }

  // removing copy and delete functionality to avoid bugs.
  // could be implemented in the future.
  AsyncScrollingMessage(const AsyncScrollingMessage&) = delete;
  AsyncScrollingMessage(AsyncScrollingMessage&&) = delete;
  AsyncScrollingMessage& operator=(const AsyncScrollingMessage&) = delete;
  AsyncScrollingMessage& operator=(AsyncScrollingMessage&&) = delete;

  ~AsyncScrollingMessage() {
    // not going to delete next memory
    next = nullptr;
  }

  /**
   * Show the message on the LED Matrix.
   * 
   * Before calling this. the matrix should be set up.
   * See the included example files for more detail.
   *   matrix.begin();
   *   matrix.beginDraw();
   *   matrix.textScrollSpeed(60);
   *   matrix.setCallback(matrixCallback);
   */
  void showMessage() {
    matrix.textFont(font);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print(message);
    matrix.endTextAnimation(SCROLL_LEFT, anim);
    matrix.loadTextAnimationSequence(anim);
    matrix.play();
  }

  /**
   * Get the message that will display
   */
  const String& getMessage() const {
    return message;
  }

  /**
   * Returns true if this message object has a continuation. This happens
   * when using generateMessages and the message is longer than a single
   * scroll can handle
   */
  bool hasContinuation() const {
    return hContinuation;
  }

  /**
   * Returns true if this message object is a continuation. This happens
   * when using generateMessages and the message is longer than a single
   * scroll can handle
   */
  bool isContinuation() const {
    return iContinuation;
  }

  /**
   * Returns true if there is a next message. The next message could be a
   * continuation or it could be a separate message to display next
   */
  bool hasNext() const {
    return next != nullptr;
  }

  /**
   * Returns a pointer to the next message if there is a next message.
   * The next message could be a continuation or it could be a separate
   * message to display next.
   */
  AsyncScrollingMessage* getNext() {
    return next;
  }

  /**
   * Inserts the given message to be the next message. If this message has a
   * continuation, this will automatically find the next message that does not
   * have a continuation, and insert the given message between that message and
   * that messsages current getNext if it exists. Return a pointer to nextMessage
   */
  AsyncScrollingMessage* insertNext(AsyncScrollingMessage* nextMessage) {
    AsyncScrollingMessage* findLast = nextMessage;
    while (findLast->hasContinuation()) {
      findLast = findLast->getNext();
    }
    findLast->setNext(next);
    return setNext(nextMessage);
  }

  /**
   * Simply set the next message to nextMessage and return a pointer to
   * nextMessage. The pointer to the current next message will be lost if not
   * stored before calling this.
   */
  AsyncScrollingMessage* setNext(AsyncScrollingMessage* nextMessage) {
    next = nextMessage;
    return nextMessage;
  }

  /**
   * Generate the minimum number of AsyncScrollingMessages required to display
   * a scroll of the entire given message and return a pointer to the first
   * object. Multiple objects may be required because of memory limitations
   * in the Arudino's built in scrolling code. If multiple objects are
   * required, the returned message object return true when calling
   * hasContinuation. It will return false if the message fits in a single
   * object.
   * 
   * Note that continued messages will have some overlapping characters, which
   * is required for scrolling to work smoothly.
   */
  static AsyncScrollingMessage* generateMessages(
    const String& message,
    ArduinoLEDMatrix& matrix,
    size_t animMaxChars,
    const Font& font) {
    return generateMessages(message, matrix, animMaxChars, font, false);
  }


private:

  AsyncScrollingMessage(
    const String& message,
    ArduinoLEDMatrix& matrix,
    const Font& font,
    bool hContinuation,
    bool iContinuation)
    : message(message),
      matrix(matrix),
      font(font),
      hContinuation(hContinuation),
      iContinuation(iContinuation),
      next(nullptr) {
  }

  static AsyncScrollingMessage* generateMessages(
    const String& message,
    ArduinoLEDMatrix& matrix,
    size_t animMaxChars,
    const Font& font,
    bool iContinuation) {

    // the following code determines if multiple AsyncScrollingMessage objects
    // are required to display the entire message. In the case where a message
    // has to be split up and a continuation is required, there will be some
    // overlap in the characters stored in each object. this is required to
    // scroll the message smoothly.
    //
    size_t screenChars = (matrix.width() / font.width);
    size_t maxFullyScrollChars = animMaxChars / font.width;
    size_t maxShownChars = maxFullyScrollChars + screenChars;

    bool needsContinue = message.length() > maxFullyScrollChars;

    size_t firstEnd = min(message.length(), maxShownChars);
    String sub = message.substring(0, firstEnd);
    AsyncScrollingMessage* am = new AsyncScrollingMessage(
      sub, matrix, font, needsContinue, iContinuation);

    if (!needsContinue) {
      return am;
    }

    AsyncScrollingMessage* last = am;
    size_t start = maxFullyScrollChars;
    size_t end = min(message.length(), start + maxShownChars);

    while (start < end) {
      needsContinue = (end < message.length());
      String nextSub = message.substring(start, end);
      last = last->setNext(new AsyncScrollingMessage(
        nextSub, matrix, font, needsContinue, true));

      start += maxFullyScrollChars;
      end = min(message.length(), start + maxShownChars);
    }

    return am;
  }

  const String message;
  ArduinoLEDMatrix& matrix;
  const Font& font;
  bool hContinuation;
  const bool iContinuation;
  AsyncScrollingMessage* next;
};

#endif
