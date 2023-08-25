# blink-led
ESP32 starting project

- blinks the LED located at PIN 12 on the ESP32 board
- uses a function that counts it's restarts (using NVS) along with a function that resets this counter
- after 10 restarts the counter resets
- one short BUZZ (located at PIN 13 on the board) signals the counter exceeded the number 10
- one longer BUZZ signals counter is at number 3
