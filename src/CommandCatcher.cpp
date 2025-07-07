#include "Stream.h"
#include "CommandCatcher.h"
/**
 ** CommandCatcher module provides functions to receive command information through the serial interface.
*/

CommandCatcher CCatcher;  // global convenience object

CommandCatcher::CommandCatcher() {
}

void CommandCatcher::init(Stream& serial, uint8_t buflen) {
  
  ser = serial;

  // should not be called multiple times, but if so, avoid memory leaks
  free(buffer);
  this->buflen = 0;

  buffer = (char *)malloc(buflen);
  if (buffer == NULL) {
    ser.print("Bad parameter. buflen=");
    ser.println(buflen);
    return;
  }
  
  this->buflen = buflen;
  memset(buffer, 0, buflen);
  cmdlen=0;
  reclen=0;
  cmdrdy = false;
  initialized=true;
}

void CommandCatcher::setTerminator(char term) {
  terminator = term;
}

void CommandCatcher::setSeparator(char sep) {
  separator = sep;
}

void CommandCatcher::update(bool close) {
  if (initialized) {
    if (updateBuffer()) {
      notifyListeners();
      if (close) {
        cmdlen=0;
        reclen=0;
        buffer[0] = '\0';
        cmdrdy = false;
      }
    }
  }
}

bool CommandCatcher::updateBuffer() {
  bool commandReady = false;
  while(ser.available() && reclen < (buflen-1)) {
    char c = ser.read();
    // ignore leading terminators and separators
    if ((c != separator && c != terminator) || reclen > 0) {
      buffer[reclen++] = c;
      if (c == separator && cmdlen == 0) {
        cmdlen = reclen-1;
        buffer[cmdlen] = '\0';
      } else if (c == terminator) {
        buffer[reclen-1] = '\0';
        commandReady = true;
      }
    }
  }
  return commandReady;
}

void CommandCatcher::notifyListeners() {
}



// **********************
