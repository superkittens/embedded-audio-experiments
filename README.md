# Embedded Audio Experiments

A collection of very basic examples in embedded audio processing.
The code in this repository is a result of my learnings as I explore the field of embedded audio.  Therefore, code in this repository is not at all refined and only exists to illustrate the basic concept of its respective topic of interest.  It should never be used for production software.  I supplied the code here in hopes that it will interest you in embedded audio and help you get started.  Some of the concepts covered may also have a corresponding blog post which goes into a bit more detail.  Feel free to copy and/or modify the code for your own purposes.
  
  
## Running

The code in this repository is targeted for the Silabs [Pearl Gecko PG12 Starter Kit](https://www.silabs.com/products/development-tools/mcu/32-bit/efm32-pearl-gecko-pg12-starter-kit).  To run this code, the easiest way is to download [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio), create a new project and copy and paste the code into your project.  Some tips for creating a new project can be found [here](https://www.meoworkshop.org/silly-audio-processing-2/).  

Note that you will need an appropriate hardware setup to connect audio input and outputs.  
  
  
## Topics

This repository contains example code for the following topics:  
  
### Audio Passthrough  
[Silly Audio Processing 3](https://www.meoworkshop.org/silly-audio-processing-3/)  
An example that configures the ADC, Timer and DAC to read in an audio sample and directly feed it to output.  No processing is done on the sample.  
  
  
### Audio Passthrough with Buffers  
[Silly Audio Processing 4](https://www.meoworkshop.org/silly-audio-processing-4/)  
The same as the Audio Passthrough example above but using a system of buffers to create additional processing time.
  
  
## Additional Links and Reading Material

Throughout my embedded audio explorations, I found the following resources helpful.  I hope you find them useful as well:  

* R.G. Lyons, _Understanding Digital Signal Processing_, Prentice Hall, 2010
* [Introduction to Digital Filters](https://ccrma.stanford.edu/~jos/filters/), J.O. Smith 
* [Physical Audio Signal Processing](https://ccrma.stanford.edu/~jos/pasp/), J.O. Smith
* [Spectral Audio Signal Processing](https://ccrma.stanford.edu/~jos/sasp/), J.O. Smith
