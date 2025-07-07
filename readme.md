# CommandCatcher - a simple Arduino library for getting commands through the serial interface

by [Martin Scott Nicklous](https://github.com/msnicklous/)
| 
[Github Project Page](https://github.com/msnicklous/CommandCatcher/)

Released under the MIT License

---
## Overview

Sometimes when your arduino is connected to a serial interface, you want to be able to obtain and process commands through
that interface. For example, yo u might want to send commands to a test harness to cause certain actions to happen.

If you have a more complicated program that maybe uses a number of class and header files, all compiled and linked together,
you might want individual classes or modules to process their own commands in order to retain separation of concerns.

The CommandCatcher can help you do such things.

See [API Documentation](https://msnicklous.github.io/CommandCatcher/)

---

## Concepts

### General Operation

The CommandCatcher monitors the serial interface, receiving ASCII characters as they arrive. When the terminating character 
('\n', configurable) arrives, the CommandCatcher parses the command into command and parameter strings and calls registered
listeners for command processing. After all listeners have been called, the internal buffer will be reinitialized for the 
subsequent command. 

If no listeners are registered, the sketch can poll CommandCatcher to determine if a 
command is available for processing and use CommandCatcher methods to obtain the information and also to mark command 
processing as completed.

If a command arrives, but is not processed before the next command begins arriving, the first command will be overwritten
and lost. This is usually only a problem if no command listeners are registered.

The sketch calls the CommandCatcher.update() method at the top of the loop() function in order to make the magic work. 

The CommandCatcher expects to be the only module monitoring serial input. If other code is also monitoring and manipulating 
incoming serial data, there might occur tricky-to-find problems.

### Command Format



---

## Arduino Code


### Getting Started
To begin, include the CommandCatcher Library at the top of your sketch.

```
#include <CommandCatcher.h>
```

This will provide a global instance of the CommandCatcher class called CCatcher for your convenience. By default,
CommandCatcher uses the standard Serial stream for input, but a method is provided for using a different stream
if you need to. CommandCatcher needs to be initialized during the setup() function of your sketch, and since it 
uses Serial (or other stream), that needs to be initialized as well:

```
setup() {
  ...
  Serial.begin(115200);
  CCatcher.init();
  ...
}
```

If the init() function is not called, commands will neither be received nor processed. Calls to other CommandCatcher methods 
will be ignored.

If needed, you can also suppy a serial stream other than the standard Serial object to CommandCatcher during init().
Also, CommandCatcher uses a default buffer size of 16 characters, but if you want a different size, you can specify that
in the init call as well:

```
setup() {
  ...
  SoftwareSerial mySerial (rxPin, txPin);
  mySerial.begin(115200);
  Trace.init(mySerial, 32);
  ...
}
```


### Buffered Tracing

To used buffered tracing, you bracket the Trace.trace() calls between the Trace.open() and Trace.close() calls.
Trace.open() resets the buffer and starts collecting trace data. Trace.close() causes all trace data collected by the Trace.trace() 
calls to be written to the serial interface. I usually put Trace.open() at the top of the loop() block and Trace.close()
at the end. 

```
void loop() {
  static int ref0 = analogRead(A0);
  long start = millis();
  
  Trace.open();

  if (digitalRead(SWITCH_PIN)) {
    ref0 = analogRead(A0);
    Trace.trace("ref", ref0);
  }
  
  // ... do interesting stuff ...
  
  long t = millis() - start;
  Trace.trace("time", t);

  Trace.close();
}
```
If you call Trace.trace() either before calling Trace.open() or after calling Trace.close(), any data will be ignored.

### Immediate Tracing

The Trace.itrace() function will immediately write a string to the serial interface without regards for contents
of the buffer.

```
void loop() {
  static int ref0 = analogRead(A0);
  long start = millis();
  
  Trace.open();

  if (digitalRead(SWITCH_PIN)) {
  	Trace.itrace("button pressed!!");
  }
  
  // ... do interesting stuff ...
  
  long t = millis() - start;
  Trace.trace("time", t);

  Trace.close();
}
```


