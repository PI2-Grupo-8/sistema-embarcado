# max17043 Fuel Gauge (Sparkfun)
#Libraries
from max17043 import max17043

# uses I2C conection, loock at library

def get_battery():
    # restart the module
    m.quickStart()
    # init a variable
    m = max17043()
    
    return m.getSoc()
