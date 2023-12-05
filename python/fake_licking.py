from UdpComm import UdpComm
from time import sleep
import json

total_time  = 0;
comm = UdpComm("127.0.0.1", 5005)
behavior_comm = UdpComm("127.0.0.1", 5000)
position_message = json.dumps({'position':{'dy':25}})
lick_message = json.dumps({'lick':{'action':'start'}})
sensor_created_message = json.dumps({'sensor':{'pin': 1000, 'action': 'created'}})
sensor2_created_message = json.dumps({'sensor':{'pin': 2000, 'action': 'created'}})
sensor_start_message = json.dumps({'sensor':{'pin': 1000, 'action': 'start'}})
sensor_stop_message = json.dumps({'sensor':{'pin': 1000, 'action': 'stop'}})


lick_interval = 1
lick_delay = 1
lick_duration = 0.2

behavior_comm.sendMessage(sensor_created_message)
behavior_comm.sendMessage(sensor2_created_message)
while (total_time < 150):
    behavior_comm.sendMessage(lick_message)
    behavior_comm.sendMessage(sensor_start_message)
    sleep(lick_duration)
    behavior_comm.sendMessage(sensor_stop_message)

    lick_delay += lick_interval

    total_time += lick_interval
    sleep(lick_interval)
