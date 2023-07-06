import time
import re

file_path = "../History.log"
fault_type = ""
faults = []

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
          result = re.search(r'>\s*(.*?):', line)
          print(f"Line {line_number}: {line} {result}")
      else:
        time.sleep(1)
        continue

except FileNotFoundError:
    print(f"File '{file_path}' not found.")
except IOError as e:
    print(f"Error reading file: {e}")
