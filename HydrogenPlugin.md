# Hydrogen Plugin #

![http://wiki.synthclone.googlecode.com/git/images/HydrogenPlugin-screenshot.png](http://wiki.synthclone.googlecode.com/git/images/HydrogenPlugin-screenshot.png)

[Hydrogen](http://www.hydrogen-music.org) is an advanced drum machine for GNU/Linux.  A Hydrogen drum kit consists of 1-1000 instruments, each of which has its own ADSR envelope, low pass filter, volume control, mute settings, panning, and random pitch factor.  Each instrument can contain up to 16 velocity layers, each of which has a sample, a velocity range, and gain and pitch controls.

The Hydrogen plugin for `synthclone` creates an instrument for each unique combination of channel, note, channel pressure, aftertouch, and control values.  If more than one zone has the same above data, then those zones will become velocity layers for the same instrument, sorted by velocity.

The actual velocity ranges are determined by the layer algorithm.  There are three layer algorithms that can be used when generating Hydrogen drum kits from `synthclone`:

1.) _Linear interpolation of velocity values_

Velocity ranges will be created by sorting the zones by velocity, and taking the average velocity between two different zones for minimum and maximum velocity values.  For example, if four layers are in the same instrument with velocity values 32, 64, 96, and 127, then the ranges will be:

| **Zone Velocity** | **MIDI Velocity Minimum** | **MIDI Velocity Maximum** | **Hydrogen Velocity Minimum** | **Hydrogen Velocity Maximum** |
|:------------------|:--------------------------|:--------------------------|:------------------------------|:------------------------------|
| 32                | 0                         | 47                        | 0.0                           | 0.377953                      |
| 64                | 48                        | 79                        | 0.377953                      | 0.629921                      |
| 96                | 80                        | 111                       | 0.629921                      | 0.714286                      |
| 127               | 112                       | 127                       | 0.714286                      | 1.0                           |

2.) _Velocity values interpreted as maximums_

Ranges will be created by interpreting the given velocity value of any zone as a maximum.  Using the same four layers from the above example, the ranges would be:

| **Zone Velocity** | **MIDI Velocity Minimum** | **MIDI Velocity Maximum** | **Hydrogen Velocity Minimum** | **Hydrogen Velocity Maximum** |
|:------------------|:--------------------------|:--------------------------|:------------------------------|:------------------------------|
| 32                | 0                         | 32                        | 0.0                           | 0.285714                      |
| 64                | 33                        | 64                        | 0.285714                      | 0.571428                      |
| 96                | 65                        | 96                        | 0.571428                      | 0.857142                      |
| 127               | 97                        | 127                       | 0.857142                      | 1.0                           |

3.) _Velocity values interpreted as minimums_

Ranges will be created by interpreting the given velocity value of any zone as a minimum.  Using the same four layers from the above example, the ranges would be:

| **Zone Velocity** | **MIDI Velocity Minimum** | **MIDI Velocity Maximum** | **Hydrogen Velocity Minimum** | **Hydrogen Velocity Maximum** |
|:------------------|:--------------------------|:--------------------------|:------------------------------|:------------------------------|
| 32                | 0                         | 63                        | 0.0                           | 0.571428                      |
| 64                | 64                        | 95                        | 0.571428                      | 0.857142                      |
| 96                | 96                        | 126                       | 0.857142                      | 1.0                           |
| 127               | 127                       | 127                       | 1.0                           | 1.0                           |