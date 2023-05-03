# Project-Shangri-La ⛩ 
> **Note**:
> This page is meant to be a transparent documentation of this project, with some sections more detailed and story-like to provide a more personal account into what I have been up to these past few months as well as highlight some of the lessons/insights this project has taught me, both about myself and the subjects I've gained knowledge in.      
<img align="right" width="220" height="220" src="https://github.com/dooiee/Project-Shangri-La/blob/main/images/Koi_Vinyl_Rescaled.png?raw=true">

Project Shangri-La is a showcase of my efforts to restore and revamp a neglected concrete pond into a semi-automated, self-sustaining koi pond. I chose the name because I felt it was a perfect representation of what I ultimately hoped to acheive. I wanted this area to become more than the definition of an imaginary faraway haven, or hideaway of idyllic beauty and tranquility(dictionary definition). I wanted to run with my idealism - of what I dreamed doing to this pond area to make it both visually appealing and technologically advanced, but actually create and build it.
I wanted to test myself to see if I could prove out some of the engineering solutions I could so easily spin up in my mind.

So I worked to do just that...

I worked to satisfy 3 main ideas that seemed to cross my mind the most:

1. Cultivate the neglected, overgrown, and hideous landscape into a beautiful, self-sustaining area with a zen garden style that requires nearly zero maintenence to upkeep, for when I no longer live here. 
2. House beautiful koi fish in an environment that promotes good health and allows them to thrive
3. Tap into my curiosities of making a hardware/software solution to develop my very own state-of-the-art monitoring system to ensure water quality parameters are within acceptable ranges for the koi, and allow that to be easily observed.
    
To acheive this I used multiple microcontrollers and sensors to collect and upload data in real-time to a database - that data is then relayed to an application I developed to allow me to monitor and control the system from my phone, anywhere in the world (so long as I have internet access). In the following sections I provide more detail about the features of my system, how I got there, and many of the failures I had to face along the way to reach a successful solution. 

This personal project has taught me so much about how things in our world operate and communicate on a fundamental level, and has given me a deep appreciation for the efforts of individuals all over the world. The efforts and acheivements humanity has made in the fields of electronics, software, and telecommunications are astounding, and have allowed me to take action on my ideas and turn them into a real, working physical system. I am pleased with all of the knowledge I have been exposed to and absorbed as a result of this project, and I am forever appreciative of the challenges it has faced me with, and the lessons it has taught me about myself, mainly: 
- how to adapt and face adversities when they inevitably arise
- how to continue driving forward when efforts aren't working and a solution is still unknown.

This project is ultimately far from total completion and there are still many many more ideas I would love to implement, but for now, it's safe to say V1 is complete and has been a successful effort. 
***

## Contents
1. [Features](#features)
1. [Demos](#demos)
1. [Application Screenshots](#application-screenshots)
1. [The Pond Before and Now + Progress Pics](#the-pond-before-and-now--progress-pics)
1. [Story Time](#story-time)
    1. [Failures](#failures-there-were-so-so-many)
1. [Hardware/Software Used](#hardwaresoftware-used)
1. [Code](#code)
1. [Improvements to be made to V2](#improvements-to-be-made-to-version-2)
1. [Future Ideas & Developments](#future-ideas--developments)
1. [Acknowledgements](#acknowledgements)

## Features
- Real-time data monitoring & long-term trend visualization
- Weather forecasts, current conditions, and expected rainfall API requests to provide insights to present and past data values.
- Wireless control of microcontrollers via phone application to troubleshoot if necessary 
- Wireless control of non-smart underwater LEDs via phone application and radio frequency signal transmission
- Pond water level automactically filled with collected rainwater when depth sensor moving average value crosses set threshold 
  - can also be triggered wirelessly via phone application

## Demos
https://user-images.githubusercontent.com/71622811/188991797-72c4ba37-2b89-409f-95e6-2039481f20bc.mp4

https://user-images.githubusercontent.com/71622811/189450137-4b5813f1-78b0-4a68-a956-98fa43d12203.mp4

https://user-images.githubusercontent.com/71622811/189450438-dd4cd04f-8a0b-4f7f-9acf-b6fe17a12255.mp4

/** Solenoid demo + system reset from phone videos to be added soon. 

## Application Screenshots
<img width="200" src="https://user-images.githubusercontent.com/71622811/189449200-38ace229-8d8e-48aa-a186-e808378520e1.jpeg?raw=true"/>    <img width="200" src="https://user-images.githubusercontent.com/71622811/189451111-f41b833f-cb4b-457c-82ea-f9e1a42e2083.jpeg?raw=true"/>    <img width="200" src="https://user-images.githubusercontent.com/71622811/189451517-78dd7e51-2ab2-45d4-823e-5aed73d0063c.jpeg?raw=true"/>    <img width="200" src="https://user-images.githubusercontent.com/71622811/189451938-5e37e84f-62b7-4f49-8dd6-3c0d6a29ba01.jpeg?raw=true"/>

## The Pond Before and Now + Progress Pics
\*I will also be uploading an animation of the over 250 pictures I took from start to finish from the same spot once complete.

![Alt text](https://github.com/dooiee/Project-Shangri-La/blob/main/Progress_Gifs/PanoramicStartToFinishProgress.gif)

<img width="350" src="https://user-images.githubusercontent.com/71622811/189450978-d58cb060-40e7-4df5-82b9-8dfa9a4d45da.jpeg?raw=true"/>    <img width="350" src="https://user-images.githubusercontent.com/71622811/189451202-2cd09bf6-2cb2-45cf-a671-2420ec180cc4.jpeg?raw=true"/>    
<img width="300" src="https://user-images.githubusercontent.com/71622811/189453065-c1d580b3-102f-4a6d-b2d3-4b2a9d2108db.jpeg?raw=true"/>    <img width="300" src="https://user-images.githubusercontent.com/71622811/189451401-fcc9abcf-1d32-4a45-908f-519bcdb88be5.jpeg?raw=true"/>    

<img width="500" src="https://user-images.githubusercontent.com/71622811/189453324-874c0459-3354-41c3-ab3b-9383e46dccb0.jpeg?raw=true"/> 

***

## Story Time
  
### **Failures** _(There were so so many...)_

At a few points during this it was actually laughable how many issues I ran into that cost me extra time, money, and caused me to deviate from the desired ideal plan I had in mind. Ultimately, I learned that it sucks when you don't know/have access to all necessary information to guarentee success of the effort, but you have to try it anyways. More likely than not, the imperfect solution will generate imperfections of its own that you must adapt to and face in order to reach the ultimate goal of getting something to successfully work. I learned that an imperfect working solution is much better than no solution regardless of how hard it felt to acheive.

> **Lesson Learned:** I'm not necessarily expecting problems to arise or the worst to happen, but when issues inevitably do arise due to the nature of working with things with greater than binary complexity, I better know how to keep frustration/annoyance/other emotions in check, adapt, and continue driving towards the desired outcome. Additionally, I've worked to improve my preparation of the secondary/tertiary downstream effects of a decision to better avoid/fix the possible pitfalls before they occur.

Let me name just a few of my failed efforts to paint an understanding of how hard it is to generate a working solution to a unique environment.
- **Water & Electronics**. They are not friends... I shorted and fried so many things that had to be replaced due to varying degrees of water ingress.

    **Story Time:** 
   > 
   > **TL;DR:** Everything went horribly wrong a week before I was going away for a month, and I needed my system working before I left.
   >
   > <img align="right" width="300" src="https://user-images.githubusercontent.com/71622811/187786590-1874616d-d01e-4960-9500-7e046a57f0cb.jpeg?raw=true">
   > I had planned an extended trip to visit my girlfriend. I was on a strict deadline to get my system connected and working before I left, so I could test the most important feature of the project: Could I monitor my system from anywhere? I had all of the software and sensors ready, now all that was left to do was get it hooked up to power roughly 10ft away. 
   >  

   >  The simple solution would have been to just feed an extension cord through the conduit piping, and plug it right into the microcontroller box, but unfortunately the conduit I had bought to connect to the existing line wasn't wide enough to fit a three pronged plug through so there goes that nice idea.
   >  
   >  I looked everywhere for extension cords with USB connections (to power the microcontroller units (MCUs) via micro USB), and nothing was long enough to be anything better than a solution that would only work until it rained. I decided I better at least try that to make sure the MCUs work when connected that way and stay connected.
   >  
   >  I hooked up the main MCU via USB to the extension cord (since there was only one USB slot)... Success! it worked. 
   >  
   >  Of course I'm going to need to power both of these things at the same time, I thought to myself, so I borrowed the breadboard power supply I was using to power the solenoid and wired them both up. Initially both were working, but then after a little time went by I noticed the main MCU was having some issues, it would continally reset every couple of minutes. I decided to mess around with the wiring to ensure it was not a connection issue. (**_Spoiler alert_**... That was a mistake). 
   >  
   >  I went to wire up the main MCU to the 3.3V breadboard channel and I forgot to unplug the USB currently providing it power... Instantly the LCD board flickered off and on, and then the board's status LED started flickering rapidly. I knew I had messed up... Sure enough, I bricked the board and it never outputted another a line of my code again. 
   >  
   >  I did some research and the only way to potentially unbrick the Arduino MKR board was to hook it up to another MKR board, and run some debug program on it. That's unfortunate, I thought to myself, but no big deal, we'll just order another one, it'll be here in two days and we'll be back in business (and we're just going to skip unbricking that first board for the time being).   
   >   
   >  Even though I didn't successfully power both boards via the breadboard power supply, I felt I had better shot at finding a long enough 5mm power adapter cord. Plus the 5mm cord fit through the conduit so this seemed like the perfect solution! Better yet, I found one in the drawers of useless cords my family has collected over the years (I guess hoarding those cords over the years wasn't so useless after all!). 
   >  
   >  
   > I ran the adapter cord through the conduit to an enclosure box then back through conduit up to where I'd place my MCU box, and I had about 3" to spare after running it under the turf and putting the box underneath a couple rocks to hide it. Next day the new board came in. I programmed it, wired it up in the box, and connected the cord to my box and we were in business! 
   > 
   > Or so I thought... 
   > 
   > That night, we experienced the worst rainfall I've ever seen the pond area have to endure. Flash flooding of nearly 2.5" completely drowned the entire area... I had seen the area contend with heavy rainfall before, but never this much so I was super nervous to see how things held up. I went out to assess the damages as soon as the rain even slightly eased up... yeah they weren't good... 
   > 
   > So much water seeped underneath the newly installed liner I thought forsure it would have to be completely redone which would have been quite hard now that fish were in there. I went over and lifted the rocks hiding the newly installed enclosure box, and I knew right away this thing was completely shot. Water was forcefully rushing over it. 
   > 
   > https://user-images.githubusercontent.com/71622811/188214547-5e792118-22b9-4b3c-b2b2-79054bad9bb2.mp4
   > 
   > https://user-images.githubusercontent.com/71622811/188216695-a946b85b-6601-4572-ab9d-9b75c1dd5ed6.mp4
   > 
   > The chances of that box inside being dry were about .2%.
   > 
   > <img align="center" height="225" src="https://user-images.githubusercontent.com/71622811/188222885-dcbbc15c-2174-41da-81a6-e5a1f6728b9a.jpeg?raw=true">
   > As expected, the box was toast and I now had two days before I had to leave... 
   > 
   > I was disheartened. But it wasn't time to give up because I still had time to make it work. 
   >    - Plus, things weren't all bad: The massive bulges of water underneath the liner completely receded, returning the liner to its original form! I guess the leaky pond (another failure I discuss) was now a blessing rather than a curse.  

   > 
   > I just had to find ANOTHER 5mm power adapter cord long enough. It was a shame the useless drawer of cords didn't have two of those perfectly sized adapters...
   > After scouring the internet I finally found an option that worked. I could use half of the one cord and the other half of the other cord to make a long enough cord, so I ordered both. 
   > 
   > The adapters came the afternoon before my flight, giving me the remainder of the day to get everything reconnected. This time, I moved the enclosure box to a slightly raised location (away from where any flood waters would reach), and as a little extra precaution, I decided to epoxy the two parts of the enclosure together to ensure that this power adapter wouldn't meet a single atom of water in its lifetime. 
   > 
   > Around 11 o'clock that night my system was officially (& finally) online!
   >  
   > <img align="center" height="400" src="https://user-images.githubusercontent.com/71622811/188236373-00363f39-8f7d-4585-abf9-e5cb2c35d171.jpeg?raw=true">
   > 
   > (Photo of the system taken next day, just before I left for the airport)

   > Now time for the real test. Will my system stay online? And will my system relay me all of the data so I can successfully monitor the water quality over the month?
   > 
   > Well, it did for a little bit. 

   > Things were running good at the airport. After I landed, things were still running smoothly. Sadly, not too long after, I stopped receiving realtime updates, and the system seemed to be having some connection issues. It appeared the MCUs got into states that my failsafes did not account for. So for the remainder of my trip my system unfortunately remained offline. Although that was certainly not how I hope things would go, I was not discouraged by the connection issues, knowing that those could be easily fixed with better failsafes. I was elated by the fact that my system initally worked exactly as was intended, sending me data values in realtime from two tiny microcontrollers a mere 2500 miles away. That was pretty dang cool!
   >
   > **Lesson Learned:** Sometimes the best way to keep something working, is to just keep it from failing. 
  
- **A leaky pond**

  In a cheap effort to raise the pond water level and stop water from seeping out of cracks in the concrete, I tried everything from flex seal, to more concrete, to roof sealing paint, and none of it held up. Eventually I had to come to terms with spending money on a more permanent solution of a pond liner even though I didnt want to. Although the pond liner did away with the desire to use the existing underground piping and concrete vault the previous owner had designed, it ultimately directed me to implement the rainwater collection and solenoid solution which acheived the same thing.
  - Since housing fish in the pond deep enough to endure the winters was a must, I knew long-term, this was money well spent. 
- **Bricked microcontrollers**

  Already mentioned bricking one Arduino, but there were a couple ESP boards I had bought and wasted days trying to troubleshoot why the boards would not properly burn my program (or any program) to them. After endless searching and no one person or forum being able to give me an exact answer as to why some ESP boards do not burn properly, I was forced to just buy a different ESP board and try again. Thankfully the new one did not have problems.  
  - Eventually, I learned that the burning issues are actually due to a problem with the board's architecture that was only fixed with a differently designed board. It is a nice reminder that even products developed by the minds of many intelligent people who devoted thousands and thousands of hours to its success still may experience failures that were not accounted for in the development and testing phase. 
- **Arduino IoT Cloud** 

  My initial solution to collect and display realtime data from the MCUs was to use the out of the box beta cloud service offered by Arduino. This effort proved much more difficult than intended due to the lack of documentation and limited API features due to it being a beta version. Although I had eventually gotten it to work, I found there to be a bug in their library that would cause my Arduino to reset from a watchdog timer that would trigger for unknown reasons, and then continually trigger as my system would be booting back up causing my system to never get back online. I decided to ultimately forgo using their cloud service and API, and go with Firebase and my own versions of features such as the watchdog/timeout timer, and this proved to be 100 times better after overcoming the small learning curve.
- **Material incompatability**
  
  This one is the best example of what I mean when I say it sucks when you have tried everything to garner the information necessary to guarentee success, but there is stil one question/concern you cannot get absolute proof on, but you must go forward with the effort anyways. In this instance, my concern was connnecting the waterfall liner to the newly installed pond liner. Although the pond liner stops the leaky pond issue, the whole effort would be useless if water was now just going to leak out between the two liners. There are many ways to connect two polymer liners together, but only certain methods work for certain materials. I learned that my type of liner must be bonded thermally with a heat gun. I tried my best to confirm my waterfall liner could also be bonded thermally, and not solvent bonded, and it appeared the former was the case. My only concern was that the two liners weren't the exact same material so I couldn't prove with absolute certainty they would bond to each other, even though they are both bonded to oneself thermally. 

  I absolutely dreaded moving forward with this effort because I could see exactly what I was going to have to do if they did not bond together, and it was not going to be fun... I would have to tear up the waterfall area that took a grueling effort to complete. I greatly resisted the thought of having to redo something I spent a great deal of time on to get just right. 
  
  > I noticed this pattern occur on a few other times throughout this project (typically when it involved undoing something I'd put effort in). I find this so ironic because I am constantly thinking to myself how I would improve things if I were building them, and that would basically involve completely redoing the thing, so why the mental block when I have to redo something of my own. When I think about creating someone else's efforts better I do it solely with the goal of better functionality in mind, so I know that is the goal when I need to redo something of mind. So ultimately, I need to stop getting in my own way, and remember that even though it may suck (& I need to also work on ways to make it suck less), better functionality is far more important than hanging onto old efforts that seem perfect (even though they are not).
    
  Unfortunately, those fears and dread did come to reality when the two materials did not adhere together:(. So of course I stewed on the thought of removing every one of those perfectly placed heavy rocks for a little (because apparently I like to put my mind through unnecessary mental ordeals??), but then I bit my tongue and got to work, removing the entire lower section of the waterfall out of the way to lay an extra piece of the pond liner underneath: to which I would bond with the pond liner because... Like binds with like!
  
  Once bonded, I returned the exhisting liner, river rocks, and mid-sized boulders to their previous spots, and the effort was complete. And guess what, it still looked as good as before (but now functional too!). I'm glad I did not succumb to the resistance to change it because I would have forever regretted a perfect looking waterfall that leaks.

***

## Hardware/Software Used
- [Firebase RTDB](https://firebase.google.com "Firebase Homepage")
- [Arduino MKR WiFi 1010s](https://docs.arduino.cc/hardware/mkr-wifi-1010)
- [MKR ETH Shield](https://docs.arduino.cc/hardware/mkr-eth-shield)
- [ESP-WROOM-32D](https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/stable/hw-reference/get-started-devkitc.html "Espressif ESP32-DevKitC V4")
- [Arduino Nano 33 IoT](https://docs.arduino.cc/hardware/nano-33-iot)
- MB102 3.3V/5V Breadboard Power Supply
- 433MHz RF Transmitter and Receiver Modules
- [Data sensors](MCUs/MKR-1010-Water-Quality-Monitor/main/main.ino) (link to MKR-1010-Water-Quality-Monitor code containing product names and specifics)
- [Solenoid Valve](https://www.amazon.com/gp/product/B07YTJYYML)
- [4-Port Relay Module](https://www.amazon.com/4-channel-relay-module/s?k=4+channel+relay+module)

### Software Libraries Used
#### Microcontrollers (coded with C/C++)
- [Atlas Scientific Gravity Sensor Library](https://files.atlas-scientific.com/gravity-pH-ardunio-code.pdf) (modified)
- [Adafruit_SleepyDog](https://github.com/adafruit/Adafruit_SleepyDog)
- [ArduinoBLE](https://github.com/arduino-libraries/ArduinoBLE)
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
- [FirebaseESP32](https://github.com/mobizt/Firebase-ESP32)
- [OneWire](https://github.com/PaulStoffregen/OneWire)
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home)
- [RCSwitch](https://github.com/sui77/rc-switch)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
- [EthernetLarge](https://github.com/OPEnSLab-OSU/EthernetLarge)
- [SSLClient](https://github.com/OPEnSLab-OSU/SSLClient)
- [RTCZero](https://github.com/arduino-libraries/RTCZero)
- ~~[Firebase-Arduino-WiFi101](https://github.com/mobizt/Firebase-Arduino-WiFi101)~~ (No longer used)
- ~~[WiFiNINA](https://github.com/arduino-libraries/WiFiNINA "Arduino WiFININA")~~ (^)

#### iOS app (coded with Swift & SwiftUI/WidgetKit Frameworks)
- [firebase-ios-sdk](https://github.com/firebase/firebase-ios-sdk)
- [swift-collections](https://github.com/apple/swift-collections)
- [ChartView](https://github.com/AppPear/ChartView "SwiftUICharts")

***

## Improvements to be made to Version 2
- [ ] Have MCUs push more logging values and current states to Firebase to gain better insights to what the MCU is doing 
    
    >(i.e. if it resets, how long does it typically take to get back online? Can I then improve that?)
    
- [X] MCU Code cleanup
  - [X] Add more specific functions to better respond to the MCU situation 
  
    > (i.e. reconnect Wi-Fi if connection is lost rather than just reset for any problem)
    
  - [X] ~~Improve response time to a remote action such as changing the underwater light color by programming microcontrollers much like a state machine~~ Rewrote ESP32 code to detect stream value changes faster to improve signal transmission and response time.
      
      > For instance, most times the action is taken within two seconds (because I have the MCU wait two seconds between each loop, but say I have it simply monitor a state and once that state is observed (like my phone displaying the remote control screen to change the lights), I could change the loop period that checks for a color signal to a fraction of a second time while I am interested in changing the color; thus improving controller response times by magnitudes.
  - [ ] Various small UI bug fixes

- [ ] iOS app code cleanup
- [ ] Be able to push OTA software updates to microcontrollers rather than having to physically remove the units from the box to modify 
- [X] ~~Better protect depth sensor so that rain does not temporarily cause erroneous measurements~~ (bought a waterproof depth sensor finally) 

## Future Ideas & Developments
- [ ] Autodose chemicals using peristaltic pumps
- [X] Add more sensors to better monitor water quality (dissolved oxygen levels, KH, ~~and a better pH sensor~~)
- [X] Install an underwater camera that streams realtime video to the application to monitor fish health and observe any abnormalities
    - [ ] release updated App IP camera integration allowing viewing and control via ONVIF/SOAP protocols
- [ ] Electrically rig automated feeder to be able to control and feed remotely

## Acknowledgements

**[Back to top](#project-shangri-la-)**
