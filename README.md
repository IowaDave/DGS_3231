# DGS_3231
A registers-based approach to managing an accurate Real Time Clock

This library takes a fresh approach to writing Arduino code for a DS3231 Real Time Clock module. For the sake of simplicity and transparency:

* A copy of the hardware registers inside the device is downloaded to local storage in the Arduino. 
* Macros makes every field in the device registers explicitly available to the code writer.
* Functions are provided for the tasks that most programmers may be likely to pursue. An effort was made to keep the functions simple, "atomic", and to give them self-descriptive names.

"Atomic" means a function does just one thing or returns one value. Short, atomic may be conducive to writing code that is easier to maintain over time.

Interestingly, the type of "class" used to formulate this library is declared as a union, rather than as a class or a struct. C++ supports this usage, and it made sense in this case.

The .h header file is influenced somewhat by the CMSIS library used with SAM D21 Cortex-M0-Plus-based Arduinos such as the Zero, the MKR series, and the Nano 33 IoT. It's also inspired by some of Tom Almy's wonderful "Far Inside Arduino" books, where he demonstrates similar coding techniques for direct access to the "pins" registers of various microcontrollers.

Why write a new library for DS3231 when we already have some tried-and-true ones? I should know such libraries exist because I recently re-wrote the documentation for the well-regarded DS3231 library published by Andrew Wickert on his NorthernWidget pages at github.

Did I think I was smarter than the developers of that library? No, not at all. It was because I encountered a problem with it that I am too dumb to solve.

I was checking out the DS3231 on a recently acquired XIAO SAMD21 board from Seeed Studio. All went well until I declared a DateTime variable, as that data type is defined in the NorthernWidget library. Suddenly, the XIAO lost its Serial port on my Mac. No communications, and I had to double-tap the reset pin just to load another program. 

Troubleshooting narrowed it down to the DateTime data type. Now, I spent a lot of time this past summer studying the DateTime code in that library. There is no way I can see how that code could interfere with Serial communications. Yet, the trouble occurred.

I was able to pursue the project of that day by avoiding the use of DateTime.

Why would I desire to use an instance of DateTime, anyway? Because the NorthernWidget uses a DateTime instance in connection with a multi-byte read of the data registers in the DS3231. Its RTClib::now() function downloads all of the time registers in a single go and returns their values in a DateTime.

Pulling all seven of the time-related registers as a group ensures that all of the time/date values obtained from the DS3231 relate to the same tick of the clock. How to do that without a DateTime that seems somehow to impair the Serial port?

I spent Christmas weekend answering that question. Here is the answer. DS3231 maintains its data and controls in a sequence of 19, 8-bit registers. Download them via I2C, as a single block, into local storage. Once there, read or modify their values as needed. Then upload the bytes back to the device.

800 lines of code later, it seems to work.

There's more to do. Right now, the library is implemented within the Arduino programs provided in the "examples" folder. At some point as the files mature I will move them to the "src" folder where the Arduino Library Manager will expect to find them.

But that's not yet. So if you are interested, please look at the ".h" and ".cpp" files that appear inside each of the example project folders.

Thanks
David
December 25, 2022