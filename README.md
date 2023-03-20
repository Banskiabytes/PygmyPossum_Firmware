[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/tomevison/">
    <img src="banner.png" alt="Logo" width="800">
  </a>

  <h3 align="center">The Pygmy Possum</h3>
  <p align="center">PIR sensor for camera traps</p>
</p>

<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Built With](#built-with)
* [Usage](#usage)
* [License](#license)
* [Contact](#contact)

<!-- ABOUT THE PROJECT -->
## About The Project

<p>
The Pygmy Possum is a battery powered PIR (Passive Infrared) sensor for triggering remote camera traps. 
</p>

<p>
A camera trap is a remotely activated camera that is equipped with a motion sensor or an infrared sensor, or uses a light beam as a trigger. Camera trapping is a method for capturing wild animals on film when researchers are not present, and has been used in ecological research for decades. In addition to applications in hunting and wildlife viewing, research applications include studies of nest ecology, detection of rare species, estimation of population size and species richness, as well as research on habitat use and occupation of human-built structures.[1](https://en.wikipedia.org/wiki/Camera_trap)
</p>
![](images/IMG-1918.png)

### How it works..
<p>
The Pygmy Possum is based on an 8 bit PIC microcontroller and custom PCB
<br />
Firmware is written in C (standard C99). The compiler being used is XC8 v2.30 by Microchip.
</p>
<p>
The microcontroller will instantly enter a sleep state to minimise current consumtion. The output from the HS-SR501 module is connected to an input pin on the MCU, A rising edge on this input will trigger an interrupt on the MCU, and wake it from sleep. In the interrupt service routine, the Pygmy Possum will read the settings from the DIP switches and send pulses to the output optocoupler. This ensures that the camera circuit is completely isolated from the Pygmy Possum circuit. The optocoupler will short the Tip to the Sheath of the TRS audio Jack. This will activate the shutter release on the connected camera.
</p>

## Built With
#### Hardware:
* [Microchip PIC16F18313](https://www.microchip.com/PIC16F18313) 8-bit MCU
* [HC-SR501 PIR module](https://lastminuteengineers.com/pir-sensor-arduino-tutorial/) low-cost passive infrared sensor board
* [Microchip PICKit4](https://www.microchip.com/PICKIT4) programmer
#### Software:
* [MPLAB X IDE v5.40](https://www.microchip.com/mplab/mplab-x-ide)

<!-- USAGE EXAMPLES -->
## Usage

Set the DIP switches on the front of the board to select a pre-programmed package. Currently there are 8 hard-coded packages that can be selected. Each package write values into the `camParams` Struct for:
* `numOfSnaps` - Number of Shots to take after a Event
* `snapPeriod` - Time Between Shots (ms)
* `minEventPeriod` - Minimum time between Events (s)

The default DIP settings are as follows:
|DIP3|DIP2|DIP1|Package Number|`numOfSnaps`|`snapPeriod (ms)`|`minEventPeriod (s)`|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|0|0|0|0|3|50|10|
|0|0|1|1|3|100|10|
|0|1|0|2|3|250|10|
|0|1|1|3|3|500|10|
|1|0|0|4|3|750|10|
|1|0|1|5|3|1000|10|
|1|1|0|6|3|1500|10|
|1|1|1|7|3|3000|10|

<!-- table can be generated with https://www.tablesgenerator.com/markdown_tables -->

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->
## Contact

* Tom Evison - tom@evisonengineering.com.au
* James Lidsey - james.lidsey93@gmail.com

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/tomevison/repo.svg?style=flat-square
[contributors-url]: https://github.com/tomevison/PYGMY/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/tomevison/repo.svg?style=flat-square
[forks-url]: https://github.com/tomevison/repo/network/members
[stars-shield]: https://img.shields.io/github/stars/tomevison/repo.svg?style=flat-square
[stars-url]: https://github.com/tomevison/repo/stargazers
[issues-shield]: https://img.shields.io/github/issues/tomevison/repo.svg?style=flat-square
[issues-url]: https://github.com/EmbeddedEnvironmental/PygmyPossum_Firmware/issues
[license-shield]: https://img.shields.io/github/license/tomevison/repo.svg?style=flat-square
[license-url]: https://github.com/EmbeddedEnvironmental/PygmyPossum_Firmware/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/thomas-evison-87b27073/
