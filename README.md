# Proximity-arduino-alarm
Distributed proximity alarm system with 4 boards, all arduino Unos.

## Operation 
Each board has a couple of components attached to it

The master board controls the communication with other boards via I2C, compares ultrasonic sensor readings with potentiometer determined distances and activates the buzzer in case of a breach, includes a button to disarm the alarm in case of a false alarm(mistakenly unmentioned in the YouTube video).

The Ultrasonic sensor board detects any movement within 4 meters,sends values back to the master to allow it to compare with the set threshold.

The potentiometer board, used to set a value used to compare for proximity, set by the user, in a range of ____. Sends a threshold value used by the master to compare with the ultrasonic sensor readings.

The LCD board, displays status information to the user, including the set threshold and the current reading on the ultrasonic sensor

## Connection 
all 4 boards were connected using pins A4 and A5 for I2C, with one master and 3 slaves.
