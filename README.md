# DGS_3231
A fresh approach to managing the DS3231 Real Time Clock

This library adds some resources that may be hard to find in other libraries supporting the super-accurate DS3231 Real Time Clock module. For the sake of simplicity and transparency:

* A copy of the hardware registers inside the device is downloaded to local storage in the Arduino. 
* Macros make every field in the device registers explicitly available to the code writer.
* Functions are provided for the tasks that most programmers may be likely to pursue. An effort was made to keep the functions simple, "atomic", and to give them self-descriptive names.

"Atomic" means a function does just one thing or returns one value. Short, atomic statements may be conducive to writing code that is easier to maintain over time.

## Library files location
Right now, the library is implemented within the Arduino programs provided in the "examples" folder.  The library files are located inside the folder for each example program. They load into the Arduino IDE editor alongside the corresponding .ino file. 

There are two drafts of the .h and .cpp files for this library:

1. the first draft, found in the Alarm-Every-Minute project
2. the latest draft, found in the Alarm-Match-Seconds project.

At some point as the library matures I will move the .h and .cpp files into the "src" folder where the Arduino Library Manager will expect to find them.

Interestingly, the type of "class" used to formulate this library is declared as a union, rather than as a class or a struct. C++ supports this usage, and it made sense in this case.

The data definition scheme in the .h header file is influenced somewhat by the CMSIS library at the heart of coding for SAM D21 Cortex-M0-Plus-based Arduinos such as the Zero, the MKR series, and the Nano 33 IoT. It's also inspired by some of Tom Almy's wonderful "Far Inside Arduino" books, where he demonstrates similar coding techniques. The approach impressed me as a good, disciplined way to organize and maintain clarity in the data field names.

## But, why?

Why write a new library for DS3231 when we already have some tried-and-true ones? I should know such libraries exist because I recently re-wrote the documentation for the well-regarded DS3231 library published by Andrew Wickert on his NorthernWidget pages at github.

Did I think I was smarter than the developers of that library? No, not at all. It was because I encountered a problem with it that I am too dumb to diagnose.

I was checking out the DS3231 on a recently acquired XIAO SAMD21 board from Seeed Studio. All went well until I declared a DateTime variable, as that data type is defined in the NorthernWidget library. Suddenly, the XIAO lost its Serial port on my Mac. No communications, and I had to double-tap the reset pin just to load another program. 

Troubleshooting narrowed it down to the DateTime data type. Now, I spent a lot of time this past summer studying the DateTime code in that library. There is no way I can see how that code could interfere with Serial communications. Yet, the trouble occurred.

I was able to pursue the project of that day by avoiding the use of DateTime.

Why would I desire to use an instance of DateTime, anyway? Because the NorthernWidget library uses a DateTime instance in a desirable way. Its RTClib::now() function downloads all of the time registers in a single go and returns their values in a DateTime.

Pulling all seven of the time-related registers as a group ensures that all of the time/date values obtained from the DS3231 relate to the same tick of the clock. But I failed to see how to do that within the existing code of the NorthernWidget DS3231 library.

Sometimes, starting over can be the quickest solution. That's why.

## OK then, how?

How to obtain the benefit of all-at-once access to the time registers inside the device, without a DateTime that seems to associate with weirdness on the Serial port for my XIAO SAMD21 board?

I spent Christmas weekend 2022 answering that question. Here is the answer. Use the Wire library's capability to read or write multiple bytes as a group. 

DS3231 maintains its data and controls in a sequence of 19, 8-bit registers, grouped as follows:

* seven for date and time
* four for Alarm 1
* three for Alarm 2
* one each for important control and status bits:
    * Control
    * Status
    * Aging (which this library does not address)
* two for reporting the device's temperature.

Download a group via I2C, as a single block, into local storage. Once there, read or modify their values as needed. Then upload the bytes back to the device.

Most of the registers contain more than one data field. Every field is made available to read and write directly, by name. That's the fresh approach. It lends clarity to the code. Of course, a set of functions is provided also, for code writers who prefer to work that way.

If you are interested, please look at the ".h" and ".cpp" files that appear inside each of the example project folders.

Thanks

David

P.S. I named the library "DGS_3231" as a mashup of DS3231 and my initials. In old age, fun with names is about all the fun a fella can still have! <grin>