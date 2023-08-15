#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from paho.mqtt import client as mqtt_client
import json
from datetime import datetime
import os
import traceback
import configparser

CONFIG_FILE=os.path.dirname(__file__) + "/mqtt.properties"

config = configparser.ConfigParser()
config.read(CONFIG_FILE)

MQTT_HOST=config['MQTT']['host']
MQTT_PORT=int(config['MQTT']['port'])
MQTT_USER=config['MQTT']['user']
MQTT_PASS=config['MQTT']['pass']
MQTT_CLIENT_ID="mymqttclient"


RT_BASE_DIR=config['OUTPUT']['dir'] + "/rt/"
SM_RT_DIR=RT_BASE_DIR + "sm"
LOGS_RT_DIR=RT_BASE_DIR + "logs"

if not os.path.exists(RT_BASE_DIR):
    os.mkdir(RT_BASE_DIR)
if not os.path.exists(SM_RT_DIR):
    os.mkdir(SM_RT_DIR)
if not os.path.exists(LOGS_RT_DIR):
    os.mkdir(LOGS_RT_DIR)

def decode_smart_meter(body):
    data = json.loads(body)
    timestr = data["Time"][:-1]
    dst = data["Time"][-1:]
    time = datetime.strptime(timestr, "%y%m%d%H%M%S")
    
    filename = SM_RT_DIR + '/' + time.strftime("%Y-%m-%d") + '.csv'

    haspath = os.path.exists(filename)
    output = ''
    
    if not haspath:
        output += 'MeterId,Time,DST,Import,Export,TotalImport,TotalExport\n'

    output += data["MeterID"] + "," + time.strftime("%Y-%m-%d %H:%M:%S") + "," + dst + "," + \
        str(data["CurrentImport"]) + "," + str(data["CurrentExport"]) + "," + \
        str(data["ImportPower"][0]) + "," + str(data["ExportPower"][0]) + "\n"
    
    with open(filename, 'a+') as f:
        f.write(output)

    filenameraw = SM_RT_DIR + '/' + time.strftime("%Y-%m-%d") + '.txt'
    with open(filenameraw, 'a+') as f:
        f.write(body)
        f.write("\n")


def save_logs(body, topic):
    time = datetime.now()

    filenameraw = LOGS_RT_DIR + '/' + time.strftime("%Y-%m-%d") + '.txt'
    with open(filenameraw, 'a+') as f:
        f.write(time.strftime("%Y-%m-%d %H:%M:%S"))
        f.write(",")
        f.write(topic)
        f.write("\n")
        f.write(body)
        f.write("\n")
        f.write("\n")

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
            client.subscribe("#")
        else:
            print("Failed to connect, return code %d\n", rc)
            
    def on_message(client, userdata, message):
        try:
            body = str(message.payload.decode("utf-8"))
            topic = message.topic
        
            if topic == "tele/smartmeter/SENSOR":
                decode_smart_meter(body)

            save_logs(body, topic)
        except:
            traceback.print_exc()
    
    # Set Connecting Client ID
    client = mqtt_client.Client(MQTT_CLIENT_ID)
    
    if (len(MQTT_USER) > 0) or (len(MQTT_PASS) > 0):
      client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_pre_connect = None
    client.on_message = on_message
    client.connect(MQTT_HOST, MQTT_PORT, 30)
    return client

def run():
    client = connect_mqtt()
    client.loop_forever()

if __name__ == '__main__':
    run()
