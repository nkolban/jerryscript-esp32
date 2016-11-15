#load
Let us imagine an ESP32 running JerryScript.  Let us also imagine that it has
network access.  Our goal is to dynamically load a JavaScript source file and
run it.  We can imagine that the JavaScript is being edited on a desktop PC
while the ESP32 is ... well ... an ESP32.  We need to get that JavaScript 
from the PC into the ESP32.  And that is where "load" will come into play.

The HTTP protocol seems like an excellent choice for this.  Web servers are
responsible for serving up files over the network.  As such, if we can implement
an HTTP GET request to a remote Web Server that will serve up the requested
file we will likely be in good shape.

Looking at our story from an ESP32/JerryScript perspective, we can imagine a
global function called `load()` that takes as a parameter a string representation
of a URL.  This would then be the target of the request.  For extra function,
we might be able to take advantage of other URL specifiers such as `file://`
to do double duty of loading JavaScript from some future file system co-located
with the ESP32.

##Making HTTP requests
The initial thinking is to use the CURL library (`libcurl`) to make the request.
This means that we have to successfully merge in libcurl as an ESP32 component.
That proved not to be difficult.  The libcurl invocations are always synchronous so we need not worry
about async work.


##Setting up a web server
We aren't going to get far serving up JavaScript source without a web server.
For testing purposes, we can use Node.js to achieve this:

```
var express = require("express");
var app = express();
app.use(express.static("."));
app.listen(3000, function() {
  console.log("Express app started on port 3000");
});
```

##Examples of usage
```
load("http://192.168.5.1/test.js")
```