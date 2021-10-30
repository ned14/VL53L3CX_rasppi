# Port of ST VL53L3CX (2020) driver library to Raspberry Pi

This is a port of the support library to Raspberry Pi for the VL53L3CX
Time-of-Flight ranging sensor with advanced multi-object detection.
Thanks to the advanced API of the 2020 edition of ST's library,
you can extract a great deal of subtle information from what the
laser sensor can see within its ~25 degree <= 5000 mm (indoor) <=
1600 mm (outdoor) field of view.

ST supply the original copy of this library at
https://www.st.com/en/imaging-and-photonics-solutions/vl53l3cx.html#tools-software
where the STSW-IMG015 download is this support library for the STM32
Nucleo board. Also at the same link is the STSW-IMG021 download which
is a Linux kernel driver formulation of the same code. This port takes
the Nucleo library and repoints it at userspace I2C for Raspberry Pi,
as a kernel driver is way overkill for this sensor.

I should emphasise that this is not a high quality port, I only did
just enough porting to get the example to compile and link, and I
didn't bother with rigourous error handling either. But if all you
need is the same high level ranging API the example uses, it should
be enough.

Related alternatives:

- https://github.com/stm32duino/VL53L3CX is for STM32 and Arduino,
however the VL53L3CX library they used as their base is the 2018
edition, which is a completely different codebase and much less
sophisticated. It exposes considerably less detail.

- https://github.com/cassou/VL53L0X_rasp is pretty much a straight
wrap of the STSW-IMG005 ST VL53L0X sensor driver library for Raspberry
Pi. Note that the VL53L0X sensor is much simpler than the VL53L3CX,
it can only track the distance of a single object, whereas the
VL53L3CX reports more of a statistical dispersion to be quantitatively
analysed.

- https://github.com/pimoroni/VL53L0X-python is a Raspberry Pi Python
library for the VL53L0X sensor, essentially a higher level convenience
wrap into Python of the preceding library. Enterprising readers may
wish to consider forking their repo and enhancing it to support this
repo.

## Build instructions

I personally find the Raspberry Pi Zero fast enough to build on
device (takes a minute or two):

```
git clone https://github.com/ned14/VL53L3CX_rasppi.git
cd VL53L3CX_rasppi
make
```

The ST original distro comes with an example program to demonstrate
the sensor in action. I have hack-ported this program to Raspberry Pi.
Assuming that your sensor is connected as follows:

- VDD on sensor is left unconnected.
- VIN on sensor to pin 1 3V3 on Raspberry Pi.
- GND on sensor to pin 9 GND on Raspberry Pi.
- SDA on sensor to pin 3 SDA on Raspberry Pi.
- SCL on sensor to pin 5 SCL on Raspberry Pi.
- XSHUT on sensor is left unconnected (it defaults pull up to VDD
  i.e. sensor is enabled).
- GPIO1 on sensor is left unconnected (this goes high when a
  new measurement is ready).

To build and run this example:

```
make example
bin/main
```

The demonstration program will print output such as:

```
Count=  226, #Objs=2 status=7, D=  713mm, S=1734144mm, Signal=0.09 Mcps, Ambient=0.20 Mcps
                     status=0, D= 1685mm, S= 686080mm, Signal=0.34 Mcps, Ambient=0.20 Mcps
Count=  227, #Objs=2 status=0, D=  720mm, S=2694144mm, Signal=0.04 Mcps, Ambient=0.21 Mcps
                     status=0, D= 1715mm, S= 764928mm, Signal=0.31 Mcps, Ambient=0.21 Mcps
```

This is actually of my head from the end of my arm, it gives the
distances of my head and of the wall behind my head. The VL53L3CX
can distinguish between up to four objects in its field of view at
a time, and it also provides information on what it can see:

- `status` is zero when the reading is valid. Other values usually
mean reading is flawed somehow (e.g. 12 is when the sensor knows
there is something within distance, but the SnR is too low to estimate
distance reliably. You can find a table of possible values in the docs).

- `S` is a measure of the standard deviation of the estimated distance
(sigma). Note the large uncertainty above on my head above as compared
to the wall behind me (my head will scatter a lot of laser, and my
hair won't reflect laser as uniformly as paint on the wall).

- `Signal` is a measure of how reflective-big the object is.

- `Ambient` is a measure of the ambient light. Note this is a very
high quality low infrared measure useful for outdoor heat estimation
e.g. sunshine.

These are the results from statistically analysing the raw histogram
returned by the sensor which is essentially how many laser photons
were reflected back to the sensor over time after emission. `Signal`
is therefore a function of both size and reflectivity to 940 nm light
of the object at a given distance, and whilst one cannot disambiguate
between size and reflectivity, if one knows that human skin has a
reflectivity to 940 nm light of about 0.5, then one can detect a face
sized patch of human skin, albeit that double the area of a material
with 0.25 reflectivity would read exactly the same at the same distance,
and only what enters the sensor's field of view is counted.

One of the really great things about the VL53L3CX sensor over any
other distance sensor is that one can take a measure of the size
of the object in the field of view. For example, if pointing at a bed
from directly above, this sensor can tell the difference between
someone standing near the bed (two objects one very near one very far),
sitting on the bed (three objects: head, legs, bed), or lying on the
bed (two objects, higher within half a metre of bed). It can also
differentiate between two people in the bed (`Signal` doubles), or
a child in the bed (`Signal` halves), and depending on the reflectivity
of the duvet, whether the duvet is drawn over a person lying down or
not. This, combined with its ability to make thousands of estimations
per second, makes it unique amongst distance sensors in this price range.
Indeed, put it on top of a spinning motor and you've got yourself
a LIDAR!

Enjoy your sensor!

