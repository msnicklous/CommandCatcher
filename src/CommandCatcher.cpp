#include "Stream.h"
#include <CommandCatcher.h>

// Activate tracing by uncommenting the following line and
// make sure that BufferedTrace is available and properly initialized.
// #define DEBUG

#ifdef DEBUG
  #include <BufferedTrace.h>
#endif


/**
 ** CommandCatcher module provides functions to receive command information through the serial interface.
*/

CommandCatcher CCatcher;  // global convenience object

CommandCatcher::CommandCatcher() {
}

void CommandCatcher::init(Stream& serial, uint8_t buflen, uint8_t maxListeners) {
  
  ser = serial;
  this->maxListeners = maxListeners;

  // should not be called multiple times, but if so, avoid memory leaks
  free(buffer);
  this->buflen = 0;

  buffer = (char *)malloc(buflen);
  if (buffer == NULL) {
    
#ifdef DEBUG
    ser.print("Bad parameter. buflen=");
    ser.println(buflen);
#endif

    return;
  }
  
  this->buflen = buflen;
  close();
  initialized=true;
  
}

void CommandCatcher::setTerminator(char term) {
  terminator = term;
}

void CommandCatcher::setSeparator(char sep) {
  separator = sep;
}

void CommandCatcher::update(bool complete) {
  if (initialized) {
    if (updateBuffer()) {
      notifyListeners();
      if (complete) {
        close();
      }
    }
  }
}

void CommandCatcher::addListener(CCListener l) {
  if (numListeners < maxListeners) {
    if (listeners == NULL) {
      listeners = (CCListener*)malloc(maxListeners * sizeof(CCListener));
      if (listeners == NULL) {
        
#ifdef DEBUG
        ser.print("Could not allocate memory for listeners: ");
        ser.println(maxListeners);
#endif

        return;
      }
    }
    listeners[numListeners++] = l;
  }
}

void CommandCatcher::close() {
  cmdlen=0;
  reclen=0;
  memset(buffer, 0, buflen);
  cmdrdy = false;
}

bool CommandCatcher::ready() {
  return cmdrdy;
}

char* CommandCatcher::getCommand() {
  return buffer;
}

char* CommandCatcher::getParameter() {
  return (cmdlen < buflen-1) ? &buffer[cmdlen+1] : &buffer[buflen-1];
}


/** Private methods **/

bool CommandCatcher::updateBuffer() {
  
  if (ser.available() && cmdrdy) {
    // more stuff incoming before command processed, so throw old command away.
    close();
  }
  
  while(ser.available() && reclen < (buflen-1)) {
    char c = ser.read();

#ifdef DEBUG
    char tb[2];
    tb[0] = (c == 10) ? '%' : c;
    tb[1] = '\0';
    Trace.trace("c", tb);
#endif    
    
    // ignore leading terminators and separators
    if ((c != separator && c != terminator) || reclen > 0) {
      buffer[reclen] = c;
      if (c == separator && cmdlen == 0) {
        cmdlen = reclen;
        buffer[cmdlen] = '\0';
      } else if (c == terminator) {
        buffer[reclen] = '\0';
        if (cmdlen == 0) cmdlen = reclen;
        cmdrdy = true;
      }
      if (++reclen == buflen-1) {
        // end of buffer, so throw the rest away.
        cmdrdy = true;
        if (cmdlen == 0) cmdlen = reclen;
        while(ser.available()) c = ser.read();
      }
    }
  }
  return cmdrdy;
}

void CommandCatcher::notifyListeners() {
  if (listeners != NULL && numListeners > 0) {
    for (int ii=0; ii<numListeners; ii++) {
      listeners[ii](getCommand(), getParameter());
    }
  }
}



// **********************
