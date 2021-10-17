# max17043 Fuel Gauge (Sparkfun)
#Libraries
from max17043 import max17043

# uses I2C conection, loock at library

# restart the module
m.quickStart()
# init a variable
m = max17043()

while True:
    try:
        # read and print the voltage of the battery
        print(m.getVCell())

        # read and print the capacity of the battery
        print(m.getVoc())

        # print everything about the battery and the max17043 module
        print(m)
        time.sleep(5)
    except KeyboardInterrupt:
        print('\nkeyboard interrupt!')

        # close the connection to the module
        m.deinit()

        sys.exit()
