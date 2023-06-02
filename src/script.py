import sys
import can


Bus = can.Bus(interface='socketcan', channel='vcan0', bitrate=500000)

def request_data():
  with Bus as bus:
    message = can.Message(arbitration_id=123,
                          data=[0x01, 0x01])
    bus.send()
    for message in bus:
      print(f"{message.arbitration_id:X}: {message.data}")
    
if __name__ == "__main__":
  request_data()
  sys.exit()