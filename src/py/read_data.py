import time

file_path = "../History.log"
faults = []
fault_type = ""

def send_data(fault_type, faults):
  print("Sending Data")
  print(f"{fault_type}: {faults}\n")
  

def get_data_from_file():
  global fault_type, faults
  try:
    with open(file_path, 'r') as file:
      line_number = 0  
      lines = file.readlines()
      total_lines = len(lines)

      while True:
        file.seek(0)  
        for _ in range(line_number):
          file.readline()

        new_lines = file.readlines()
        if new_lines:
          for line in new_lines:
            line_number += 1 
            line = line.strip()
            if line.startswith("-"):
              if len(faults) > 0:
                send_data(fault_type, faults)
                fault_type, faults = "", []
            else:
                sidx = line.find('>') + 1
                eidx = line.find(';')
                if line.startswith('DTC'):
                  dtc = line[sidx:eidx].strip()
                  faults.append(dtc)
                elif line.startswith('20'):
                  fault_type = line[sidx:eidx].strip()
        else:
          print("%s," % (time.time() - start_time))
          time.sleep(0.5)
          continue
          return 0

  except FileNotFoundError:
      print(f"File '{file_path}' not found.")
  except IOError as e:
      print(f"Error reading file: {e}")

start_time = time.time()
get_data_from_file()