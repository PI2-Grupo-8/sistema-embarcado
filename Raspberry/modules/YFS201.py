# YF-S201 Flow sensor
#Libraries
import RPi.GPIO as GPIO
import time, sys

global count
count = 0

def countPulse(channel):
   global count
   if start_counter == 1:
      count = count+1

def flow_sensor_setup():
    FLOW_SENSOR_GPIO = 13

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(FLOW_SENSOR_GPIO, GPIO.IN, pull_up_down = GPIO.PUD_UP)

    GPIO.add_event_detect(FLOW_SENSOR_GPIO, GPIO.FALLING, callback=countPulse)


def get_flow():
    start_counter = 1
    time.sleep(1)
    start_counter = 0
    flow = (count / 7.5) # Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    #print("The flow is: %.3f Liter/min" % (flow))
    count = 0
    time.sleep(5)
    return flow


    """ except KeyboardInterrupt:
        print('\nkeyboard interrupt!')
        GPIO.cleanup()
        sys.exit() """
