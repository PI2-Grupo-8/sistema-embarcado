from modules.BMP280 import read_temperature, BMP280_setup
from modules.sparkmax17043 import get_battery
from modules.YFS201 import flow_sensor_setup, get_flow

read_temperature()
get_battery()