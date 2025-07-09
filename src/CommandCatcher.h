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
   ** The listener function must accept two strings - the command and the parameter string - and return void.
   **/
   typedef void (*CCListener)(char*, char*);

  /**
   ** @brief default constructor
   ** @details Generally, you should use the predefined global instance 'CCatcher' rather than create your own instance.
   */
  CommandCatcher();

  /**
   ** @brief Initializes the serial interface for communication. 
   ** @details The init method is meant to
   ** be called from a central location, for example in the top-level sketch. If init
   ** is not called, the serial stream will not be monitored.
   **
   ** @param serial - the serial stream to be used  
   ** @param bufsize - the desired buffer size
   ** @param maxListeners - the maximum allowed number of listeners. Attempts to add more than this 
   ** number of listeners will be ignored.
   **/
  void init(Stream& serial=Serial, uint8_t bufsize=16, uint8_t maxListeners=4);
  
  /**
   ** @brief checks the Serial input stream and processes any command info available.
   ** @details Should be called at the beginning of the loop() function.
   **
   ** @param If true, the command will be closed out and discarded after the listener functions are called.
   ** If you want to poll for commands rather than be notified, call update(false).
   **/
  void update(bool close=true);

  /**
   ** @brief Adds a listener to be called when a command is available
   ** @details Should generally be called during the setup() function. The listener function must be a function 
   ** returning void that accepts two char* parameters: 
   **
   ** @code
   **   void myListener(char* command, char* parameter)
   ** @endcode
   **
   ** @param myListener - The listener to be added
   **/
  void addListener(CCListener myListener);

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
   ** @brief Closes out the current command.
   ** @details If you use update(false), you should call close() after processing the command.
   **/
  void close();

  /**
   ** @brief Returns true if a command is ready to be processed. 
   ** @details To be used when polling for commands.
   ** @return true if a command is ready to be processed.
   **/
  bool ready();

  /**
   ** @brief Returns the command
   ** @details To be used when polling for commands.
   ** @return the command string
   **/
  char* getCommand();

  /**
   ** @brief Returns the paraeter string
   ** @details To be used when polling for commands.
   ** @return the parameter string
   **/
  char* getParameter();

  
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
  
  CCListener* listeners = NULL;
  int         numListeners = 0;
  int         maxListeners = 0;
  
  // returns true if command is available
  bool updateBuffer();
  void notifyListeners();
};

// global command catcher object for convenience
extern CommandCatcher CCatcher;

// **********************
#endif
