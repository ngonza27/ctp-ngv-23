# DEVELOPMENT OF A SYSTEM TO EXTRACT VEHICLE PARAMETERS
_Author: **Nicolas Gonzalez Vallejo**_ <br />
_Supervisors: **Stefano Longo**, **Salvatore Gariuolo**_ <br />
Contact: {s.gariuolo, s.longo, n.gonzalezvallejo.869}@cranfield.ac.uk <br />

## Index
* [Project structure](https://github.com/ngonza27/ctp-ngv-23/#project-structure)
* [How to run](https://github.com/ngonza27/ctp-ngv-23/#how-to-run)


## Project structure
.
├── assets
│   ├── current_work.txt
│   ├── sequence_diagram.png
│   └── strucutre_diagram.png
├── README.md
├── scripts
│   ├── disable_vcan0.sh
│   ├── enable_vcan0.sh
│   └── test_services.txt
├── src
│   ├── dtc_decoder.c
│   ├── main.c
│   ├── Makefile
│   ├── receiveMessage.c
│   ├── sendMessage.c
│   ├── service01.c
│   ├── service02.c
│   └── service0307.c
└── test
    ├── can_simulator.c
    └── Makefile

## How to run

-  Enable the virtual can interface.
  Note: Check that the script has executable perimissions.
```sh
./scripts/enable_vcan0.sh
```

-  Disable the virtual can interface.
  Note: Check that the script has executable perimissions.
```sh
./scripts/enable_vcan0.sh
```

-  Run the malfunction detection system.
```sh
cd src
make
./main
```

-  Run can diagnostics simulator.
```sh
cd test
make
./can_simulator
```



