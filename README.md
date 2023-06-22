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
│   ├── sequence_diagram.png<br />
│   └── strucutre_diagram.png<br />
├── README.md<br />
├── scripts<br />
│   ├── disable_vcan0.sh<br />
│   ├── enable_vcan0.sh<br />
│   └── test_services.txt<br />
├── src<br />
│   ├── dtc_decoder.c<br />
│   ├── main.c<br />
│   ├── Makefile<br />
│   ├── receiveMessage.c<br />
│   ├── sendMessage.c<br />
│   ├── service01.c<br />
│   ├── service02.c<br />
│   └── service0307.c<br />
└── test<br />
    ├── can_simulator.c<br />
    └── Makefile<br />

## How to run

-  Enable the virtual can interface.
  Note: Check that the script has executable perimissions.
```sh
./scripts/enable_vcan0.sh
```

-  Disable the virtual can interface.
  Note: Check that the script has executable permissions.
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



