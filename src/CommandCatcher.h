#include "Arduino.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#ifndef CommandCatcher_h
#define CommandCatcher_h

/**
 ** @copyright Copyright (C) 2025 Martin Scott Nicklous
 ** @class CommandCatcher
 ** @brief Project: CommandCatcher for Arduino 
 ** 
 ** @details The CommandCatcher class provides a wrapper to the Serial object in order to receive commands through
 ** the serial interface.
 **
 ** This header file also provides a shared global object CCatcher of class CommandCatcher
 ** for convenience.
 **
*/

class CommandCatcher {
public:

  /**
   * @brief default constructor
   */
  CommandCatcher();

  /**
   ** @brief Initializes the serial interface for communication. 
   ** @details The init method is meant to
   ** be called from a central location, for example in the top-level sketch. If init
   ** is not called, the serial stream will not be monitored.
   ** @param serial - the serial stream to be used  
   ** @param bufsize - the desired buffer size
   **/
  void init(Stream& serial=Serial, uint8_t bufsize=16);
  

  /**
   ** @brief Sets the terminator character
   ** @details Should be called during the setup() function.
   ** @param term - terminator character to be used.
   **/
  void setTerminator(char term);

  /**
   ** @brief Sets the separator character
   ** @details Should be called during the setup() function.
   ** @param sep - separator character to be used.
   **/
  void setSeparator(char sep);

  /**
   ** @brief checks the Serial input stream and processes any command info available.
   ** @details Should be called at the beginning of the loop() function.
   ** @param If true, the command will be closed out and discarded after the listener functions are called.
   ** If you want to poll for commands rather than be notified, call update(false).
   **/
  void update(bool close=true);

  
private:
  Stream& ser = Serial;
  bool initialized = false;

  char*   buffer = NULL;
  uint8_t buflen = 0;
  uint8_t reclen = 0;
  uint8_t cmdlen = 0;
  bool    cmdrdy = false;
  
  char  terminator = '\n';
  char  separator = ' ';
  
  // returns true if command is available
  bool updateBuffer();
  void notifyListeners();
};

// global trace object for convenience
extern CommandCatcher CCatcher;

// **********************
#endif
