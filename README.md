# Smart Beogram

Have you ever dreamt about adding a turntable to your multiroom system? No problem! 

But what if you ALSO could voice-control your BeoGram turntable?

_NOTE: This code also enables simple HTTP GET control_


# How-to
In this guide we describe how to accomplish this with a cheap NodeMCU board and a few components.

**Video demo and guide for putting the code on the NodeMCU and setting up Adafruit and IFTTT here: https://youtu.be/vhII74I_9sY**

**Hardware description and Adafruit/HTTP commands in the PDF below: https://github.com/cklit/smartbeogram/blob/master/Hardware%20guide%20NodeMCU.pdf**

**Link for the _Additional Board Manager URL_ in Arduino IDE: http://arduino.esp8266.com/stable/package_esp8266com_index.json**


# Parts needed:
- The .ino file from in here (Click the green _Clone or Download_-button, and select _Download ZIP_)

- NodeMCU 1.0 (or Wemos D1 mini board - exact same installation procedure)

- Optocoupler 4N35 

- 220 ohm resistor

- Female DIN plug

- Different wires, depending on how you decide to connect it all together in the end :) Example in the Hardware guide-PDF.


# ESPHome
If you are using Home Assistant, you might be interested in using the fantastic ESPHome software instead.

- Copy the beogram-esphome.yaml file to your ESPHome folder

- Change SSID and password to your own

- Connect optocoupler to TX pin (GPIO1 on Wemos D1 Mini)

- Add the ESPHome integration to Home Assistant

- NOTE: This has not been tested yet, so if the code does not work, try changing Line 23: "inverted: true" to "inverted: false"

# Credit
A huge thanks to Maurizio for helping me realizing this idea :)

Disclaimer:
This solution is not in any way supported or maintained by Bang & Olufsen.
