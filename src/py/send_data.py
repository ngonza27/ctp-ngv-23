from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import logging
import time
import argparse
import json
import ast

AllowedActions = ['both', 'publish', 'subscribe']
file_path = "../History.log"
faults = []
fault_type = ""

# Read in command-line parameters
parser = argparse.ArgumentParser()
parser.add_argument("-e", "--endpoint", action="store", required=True, dest="host", help="a28yobe9j1e4my-ats.iot.us-east-2.amazonaws.com")
parser.add_argument("-r", "--rootCA", action="store", required=True, dest="rootCAPath", help="x509root.crt")
parser.add_argument("-c", "--cert", action="store", dest="certificatePath", help="f9f5eadeff-certificate.pem.crt")
parser.add_argument("-k", "--key", action="store", dest="privateKeyPath", help="f9f5eadeff-private.pem.key")
parser.add_argument("-p", "--port", action="store", dest="port", type=int, help="Port number override")
parser.add_argument("-w", "--websocket", action="store_true", dest="useWebsocket", default=False,
                    help="Use MQTT over WebSocket")
parser.add_argument("-id", "--clientId", action="store", dest="clientId", default="basicPubSub",
                    help="Targeted client id")
parser.add_argument("-t", "--topic", action="store", dest="topic", default="topic/getData", help="Targeted topic")
parser.add_argument("-m", "--mode", action="store", dest="mode", default="both",
                    help="Operation modes: %s"%str(AllowedActions))
parser.add_argument("-M", "--message", action="store", dest="message", default="Hello World!",
                    help="Message to publish")

args = parser.parse_args()
host = args.host
rootCAPath = args.rootCAPath
certificatePath = args.certificatePath
privateKeyPath = args.privateKeyPath
port = args.port
useWebsocket = args.useWebsocket
clientId = args.clientId
topic = args.topic

if args.mode not in AllowedActions:
    parser.error("Unknown --mode option %s. Must be one of %s" % (args.mode, str(AllowedActions)))
    exit(2)

if args.useWebsocket and args.certificatePath and args.privateKeyPath:
    parser.error("X.509 cert authentication and WebSocket are mutual exclusive. Please pick one.")
    exit(2)

if not args.useWebsocket and (not args.certificatePath or not args.privateKeyPath):
    parser.error("Missing credentials for authentication.")
    exit(2)

# Port defaults
if args.useWebsocket and not args.port:  # When no port override for WebSocket, default to 443
    port = 443
if not args.useWebsocket and not args.port:  # When no port override for non-WebSocket, default to 8883
    port = 8883

# Configure logging
logger = logging.getLogger("AWSIoTPythonSDK.core")
logger.setLevel(logging.DEBUG)
streamHandler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
streamHandler.setFormatter(formatter)
logger.addHandler(streamHandler)

# Init AWSIoTMQTTClient
myAWSIoTMQTTClient = None
if useWebsocket:
    myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId, useWebsocket=True)
    myAWSIoTMQTTClient.configureEndpoint(host, port)
    myAWSIoTMQTTClient.configureCredentials(rootCAPath)
else:
    myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId)
    myAWSIoTMQTTClient.configureEndpoint(host, port)
    myAWSIoTMQTTClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)

# AWSIoTMQTTClient connection configuration
myAWSIoTMQTTClient.configureAutoReconnectBackoffTime(1, 32, 20)
myAWSIoTMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myAWSIoTMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myAWSIoTMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myAWSIoTMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

# Connect and subscribe to AWS IoT
myAWSIoTMQTTClient.connect()
time.sleep(5)

try:
  with open(file_path, 'r') as file:
    idx = 0  
    while True:
      file.seek(0)  
      for i in range(idx):
        file.readline()

      new_lines = file.readlines()
      if new_lines:
        for l in new_lines:
          idx += 1 
          l = l.strip()
          if l.startswith("-"):
            if len(faults) > 0:
              if args.mode == 'both' or args.mode == 'publish':
                  data = json.dumps({'deviceID': "OBD-II_Dongle", "data": { 'fault': fault_type, "codes": faults}})
                  try:
                    myAWSIoTMQTTClient.publish(topic, data, 1)
                  except Exception as error:
                    print('Error while sending data to DB: {}'.format(error))
                    myAWSIoTMQTTClient.connect()
                    time.sleep(5)
                    myAWSIoTMQTTClient.publish(topic, data, 1)
              if args.mode == 'publish':
                print('Published topic %s: %s\n' % (topic, data))
              fault_type, faults = "", []
          else:
              sidx = l.find('>') + 1
              eidx = l.find(';')
              if l.startswith('DTC'):
                dtc = l[sidx:eidx].strip()
                faults.append(dtc)
              elif l.startswith('20'):
                fault_type = l[sidx:eidx].strip()
      else:
        time.sleep(0.5)
        continue

except FileNotFoundError:
    print(f"File '{file_path}' not found.")
except IOError as e:
    print(f"Error reading file: {e}")
