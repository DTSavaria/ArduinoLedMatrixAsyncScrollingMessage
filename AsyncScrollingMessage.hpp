#ifndef _ASYNC_SCROLLING_MESSAGE_HPP_
#define _ASYNC_SCROLLING_MESSAGE_HPP_

/**
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

  void showMessage() {
    matrix.textFont(font);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print(message);
    matrix.endTextAnimation(SCROLL_LEFT, anim);
    matrix.loadTextAnimationSequence(anim);
    matrix.play();
  }

  const String& getMessage() const {
    return message;
  }

  bool hasContinuation() const {
    return hContinuation;
  }

  bool isContinuation() const {
    return iContinuation;
  }

  bool hasNext() const {
    return next != nullptr;
  }

  AsyncScrollingMessage* getNext() {
    return next;
  }

  AsyncScrollingMessage* insertNext(AsyncScrollingMessage* nextMessage) {
    AsyncScrollingMessage* findLast = nextMessage;
    while (findLast->hasContinuation()) {
      findLast = findLast->getNext();
    }
    findLast->setNext(next);
    return setNext(nextMessage);
  }

  AsyncScrollingMessage* setNext(AsyncScrollingMessage* nextMessage) {
    next = nextMessage;
    return nextMessage;
  }

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
