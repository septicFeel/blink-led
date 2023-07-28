# blink-led
ESP32 starting project

Simple project to blink the LED situated at pin 12 on the ESP32 board. With a function to know how many times the board has had a restart and a posibility to restart this counter (example: restart counter will reset after 10 restarts), along with a BUZZ located at pin 13 signaling as follows: 
    - when restart counter is reseted to 0: one short buzz
    - when restart counter is equal to 3: one longer buzz