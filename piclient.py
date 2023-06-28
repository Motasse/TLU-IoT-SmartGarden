import json
import paho.mqtt.client as mqtt
import requests
rpi = mqtt.Client("p1")
onl = mqtt.Client("flogro", transport="websockets")
rpi.connect("localhost", 1883)
onl.connect("broker.hivemq.com", 8000)

def on_connect(rpi, obj, flags, rc):
    print("rc: " + str(rc))
    rpi.subscribe("flogro/json")
def onl_on_connect(onl, obj, flags, rc):
    print("onl_rc: " + str(rc))
    onl.subscribe("flogro/control")
    
def on_message(rpi, obj, msg):
    print("Messages arrived:" + str(msg.payload))
    onl.publish("flogro/json", msg.payload)
    js=json.loads(str(msg.payload))
    with open('data.json', 'w') as outfile:
        json.dump(js, outfile, sort_keys = True, indent=4, separators= (',', ':'))
def onl_on_message(onl, obj, msg):
    rpi.publish("flogro/control", msg.payload)

rpi.on_connect = on_connect
rpi.on_message = on_message
onl.on_connect = onl_on_connect
onl.on_message = onl_on_message
while True:
    rpi.loop()
    onl.loop()