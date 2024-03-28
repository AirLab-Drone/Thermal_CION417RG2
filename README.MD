
This is for thermal camera model CION417RG2
ubuntu 22.04 usb3.0 SDK

----


It look like:
https://github.com/AirLab-Drone/Thermal_CION417RG2/blob/main/doc/thermal_img.png


### build
```
git clone https://github.com/AirLab-Drone/Thermal_CION417RG2.git/.
mkdir build && cd build
cmake ..
make
```

### official Demo
```
./Demo
```


### Run with image
```
./test_thermal
```

### Run with ros2 node
```
./test_thermal_node
```