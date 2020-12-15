# VFD_Clock
IV-22 based vacuum florecent display clock, for arduino nano, and ST Micro M41T93 RTC


I designed this clock display and driver board for my girlfriend based on Soviet NOS vacuum florescent display (VFD) tubes. VFDs require three different power supplies to function: a voltage applied to the grid, filament wire heaters, and voltages for each segment of the numeral. For input power, I use an unused laptop supply @19.5v. The segments are usually multiplexed / scanned, so a line is not required for each segment, however this necesitates higher grid voltages in pulses, up to 80v for these specific displays, as opposed to 20v-32v. I chose to drive each segment individually to allow use of more standard off-the-shelf boost regulators, and for code simplicity as I intended to try some fancy-looking segment switching (more later). 


Power Supplies:
The board also has a 5v supply for the logic circuits, a very simple LM2576-5 copied from the application notes in the 		dataheet, not much to remark.

For the filament heater I designed a 100 mA constant current source based on the LM2575-ADJ. I didnt feel that a constant voltage supply would be appropriate as the filament resistance could change after warming up. Also, I destroyed one tube in testing by accidentally giving too much voltage, allowing a high current through that easily destroyed the filament faster than the OCP on my bench supply could react (its a quality Kikusui supply, I dont believe it to be at fault). To provide current feedback, I selected the INA240A1 current sense amplifier from TI. This part was probably overkill for the application in terms of precision and capability, however the voltage applied by it to the feedback pin on the buck regulator worked perfectly as designed. After being generated, the current is pushed through a discrete H-bridge driver and the display filaments, connected in series. The intent of the H-bridge is to provide an AC current to prevent migration in the filaments, which can lead to fading and unneven displays over time. While prototyping the bridge, I selected mosfets I had on hand, hence the overkill TO-220 packages. I would switch these out for a simple, low power motor driver IC, being much easier and more compact, whilst still easily handling the 100mA. Switching is provided by a square wave output from the RTC IC, which also runs a hardware timer interrupt on the microproccessor to update the time.

I wanted the brightness of the clock to be adjusted automatically to fit the brightness of the room, so I designed an adjustable boost converter to increase the 19.5vin up to 32v, controlled by a simple dac (not PW Modulated). A Motorolla MC33063 switching regulator does this with standard feedback from the output voltage and also a buffered signal from a MCP4901 single channel 8-bit SPI DAC. I was very pleased when this turned out to work, as during load testing it was drooping quite heavily. The issue was I was pulling many times more current than I needed to, up to a few hundred mA, when it only needs to supply around 20mA.


Segment Switching:
To switch the segments, I choose the much more layout intensive path of directly driving each segment. Each display has a 74HC595 8-bit shift register that recieve data over SPI. These drive MIC2981 8-channel source driver array. The driver consists of 8 darlington pairs that allow for any logic-level 5v signal to switch on the 19.5v output. The pin mappign of the IC makes layout really easy, as each pair of pins are right across from eachother, allowing signals to pass right through on the board. The segments draw extremely little current, so the slightly conductive solder flux residue between pins allows a leakage current enough for segments to ghost on. Without an ultrasonic cleaner, this has been difficult to prevent so far, hopefully a stronger solvent can take off the flux I used. The 1 digit on the first tube is non-functional because I may have shorted the 19.5v supply pin to the segments 5v logic line while probing, killing that channel on the shift register. Oops. 

Avoiding multiplexing also simplified and sped up the switching in code. I intended to add an effect when changing digits to appear more 'analog', a bit like a flip-dot display that can only change one segment at a time. Each digit is defined by one byte in software. XOR'ing the old and new bytes gives a byte with a 1 where each segment will change. looping through this byte and switching the  segment with a 60ms delay gives a buttery smooth effect that looks super cool. At least thats how I justified buying 4 drivers per board ($2.83 in 1x qty).


Digital/Microprocessor:
For control of all functions, I used an arduino nano for its environment, libraries, and ease of both prototyping and programming. It controls almost all digital functions through the SPI bus as the only master  giving minimal overhead, not that the bus or code is that close to any conflicts anyways. Therefore most digital pins are used for chip select lines, with some additional reset lines and a display blanking just in case. I configured the 16-bit hardware timer/interrupt to respond to a 64Hz signal from the RTC, updating the time when necessary. Buttons to change the time havent been implemented yet, and the time is set when uploading the code to the board.

I selected the STMicro M41T93 SPI Real-Time Clock for its use of SPI (already heavily utilizing it, most others used I2C which I had no interest in adding), and many of its fancy features. Its actually a fairly intelligent IC, having multiple alarms, tracking milliseconds through to centuries, digitally tunable onboard 32.768kHz crystal, analog tuning, from -15 to +95ppm, and a factory calibration of +/-5 ppm accuracy. It can output a divided clock frequency down to 1Hz as a square wave. I used this to drive a hardware timer/interrupt on the atmega328p, generating an interrupt every minute to update the time. 


Conclusion:
Overall I'm beyond pleased with this project. It's my first major pcb design, and in my opinion quite complex, probaby more than it had to be. There are several things I would change though- more SMT components, they are more compact and seem easier to solder once you get the hang of it, from less board flipping. I would use a single IC for the H-bridge, something like the MP6513L. I would also use ribbon cables to connect segment drivers to the displays, and break out the cable to the socket on the PCB, as crimping all the pins wasnt that fun. It would also look much cleaner and fold over better.

If you've made it this far, thanks for bearing with me, I hope you like my project and writeup. In comparison to a few other projects on here, its pretty rudimentary, but I'm proud of my simple mechanical engineering brain for designing and implementing this much electrickery. 







