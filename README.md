# 3D Capacitor Client

This project contains the code necessary to use a 3D capacitor as a mouse input device. This was the software component of a project created by Mason Souther, Ethan McCurley, and Tyler Veness in the Project Design Lab class at Allan Hancock College.

* [sketch_3dCapClient](sketch_3dCapClient) contains the driver code for an Arudino.
* [3DCapacitorBananaPianoJournal.pdf](3DCapacitorBananaPianoJournal.pdf) has a more detailed project description, pictures, and electrical schematics.

## Desktop application

### Build

```bash
cmake -B build
cmake --build build
cmake --install build --prefix app
```

### Usage

The desktop application shows the hand's position within the 3D capacitor cube as a yellow sphere. The circle in the top-right corner indicates the serial port connection status.

| Color  | Status                              |
|--------|-------------------------------------|
| Red    | Disconnected                        |
| Yellow | Connected but received invalid data |
| Green  | Connected and received valid data   |

The desktop application will automatically connect to an Arduino that's plugged in.
