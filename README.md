# DEVELOPMENT OF A SYSTEM TO EXTRACT VEHICLE PARAMETERS
_Author: **Nicolas Gonzalez Vallejo**_ <br />
_Supervisors: **Stefano Longo**, **Salvatore Gariuolo**_ <br />
Contact: {s.gariuolo, s.longo, n.gonzalezvallejo.869}@cranfield.ac.uk <br />

## Index
* [Project structure](https://github.com/ngonza27/ctp-ngv-23/#project-structure)
* [How to run](https://github.com/ngonza27/ctp-ngv-23/#how-to-run)


## Project structure
.<br />
├── assets<br />
│   ├── current_work.txt<br />
│   ├── results<br />
│   │   └── results_malfunction_system.png<br />
│   ├── sequence_diagram.png<br />
│   └── strucutre-class.drawio (1).png<br />
├── README.md<br />
├── scripts<br />
│   ├── disable_vcan0.sh<br />
│   ├── enable_vcan0.sh<br />
│   ├── optimized_make.sh<br />
│   ├── run_main.sh<br />
│   └── test_services.txt<br />
├── src<br />
│   ├── dtc_decoder.c<br />
│   ├── History.log<br />
│   ├── main<br />
│   ├── main.c<br />
│   ├── Makefile<br />
│   ├── py<br />
│   │   ├── notes.txt<br />
│   │   ├── read_data.py<br />
│   │   ├── send_data.py<br />
│   │   └── us-east-1<br />
│   │       ├── AmazonRootCA1.pem<br />
│   │       ├── certificate.pem.crt<br />
│   │       └── private.pem.key<br />
│   ├── receiveMessage.c<br />
│   ├── sendMessage.c<br />
│   ├── service01.c<br />
│   ├── service02.c<br />
│   └── service0307.c<br />
└── test<br />
    ├── can_simulator<br />
    ├── can_simulator.c<br />
    └── Makefile<br />

## Prerequisites
Install the following libraries:
```sh
$ sudo apt-get install can-utils
$ pip install AWSIoTPythonSDK
$ curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o      "awscliv2.zip"
unzip awscliv2.zip
sudo ./aws/install
```

## How to run

-  Enable the virtual can interface.
  Note: Check that the script has executable perimissions.
```sh
$ ./scripts/enable_vcan0.sh
```

-  Disable the virtual can interface.
  Note: Check that the script has executable permissions.
```sh
$ ./scripts/enable_vcan0.sh
```

-  Run the malfunction detection system.
```sh
$ cd src
$ make
$ ./main
```

-  Run can diagnostics simulator.
```sh
$ cd test
$ make
$ ./can_simulator
```

-  Run the python script for the data transmision.
```sh
$ python3 send_data.py --endpoint {id}-ats.iot.us-east-1.amazonaws.com --r ./us-east-1/AmazonRootCA1.pem --cert ./us-east-1/certificate.pem.crt --key ./us-east-1/private.pem.key
```
