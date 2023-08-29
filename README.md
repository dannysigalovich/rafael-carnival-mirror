# Rafael-Carnival

This repo contains the code for the STM32H753ZI side

## How to use

To flash, run and debug the STM, you will need to install the STM32CubeIDE. You can download it from [here](https://www.st.com/en/development-tools/stm32cubeide.html).

### Building

After installing the IDE, you can open the project by clicking on `File > Open Projects from File System...` and selecting the `Refael-carnival` folder.
when you import the folder you will see two project option to import, you need to import only the one with the suffix `/STM32CubeIDE`.
Now you can build the project by clicking on the hammer icon on the top bar.

### Flashing

To flash the STM, you will need to connect the STM to the computer using a micro USB cable (the port on the stm is the fareset from the ethernet port). Then, click on the arrow next to the hammer icon, to debug click on the bug icon.

## Changelog

- [v1.0.0](https://github.com/Trego-LTD/Rafael-carnival/releases/tag/v1.0.0) - Initial release:

  - INSPVA and INSSTDVE receving seperate over TCP from CPT7
  - Support INSPVA and INSSTDVE messages
  - Save received FireFlyStatus
  - Send launchCmd with dummy missions depending on PF7 pin state and received FireFlyStatus

- [v1.0.1](https://github.com/Trego-LTD/Rafael-carnival/releases/tag/v1.0.1)
  - INSPVA and INSSTDVE receving seperate moved to UDP from CPT7
  - Get Missions and Secret words from json packet over UDP, A dedicated example for the json and the sending is [here](https://github.com/Trego-LTD/Rafael-carnival/tree/4e284e8158536d2b7552f9da39d4ae8b14343897/STM32CubeIDE/Doc)
  - LaunchCmd is now sending real missions got from the json packet still depending on PF7 pin state and received FireFlyStatus (if there is no missions it won't send mission but zero)
  - Sending secret words as part of SecretCmd,
    if there is no secret words it won't send secret words but zero's
