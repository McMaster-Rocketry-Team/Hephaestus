# Propulsion Controls
Repo for McMaster Rocketry Propulsion team controls. Will be used to interface with set of peripherals and communicate with rocket main controls.

This repo is setup to be used in VSCode with the "PlatformIO" extension. PlatformIO is a free and user-friendly embdedded software development platform that can be used for a variety of architechures. I hope to update this repo in the future so that it's not fully dependent on PlatformIO, and can be used with other platforms such as Keil.

## Getting Started
<ol>
    <li>Make sure VSCode is installed on your PC</li>
    <li>Clone this repo in onto your local machine</li>
    <li>Download the "PlatformIO" extension in VSCode</li>
    <li>In the extension homepage, click "Open Project" and select the Github repo you just cloned. The repo should be recognized as a PlatformIO project since it has the platformio.ini file.</li>
    <li>Build + Upload using the icons in the top right of your IDE</li>
</ol>

## Development
The framework selected when designing the PlatformIO workspace was "STM32Cube" for the board "nucleo_h723zg". This framework uses the CMSIS module which has a variety of packages to support development. The CMSIS module has two types of packages:
<ol>
    <li>LL packages make up the majority of CMSIS and defines addresses of things like registers and ports.</li>
    <li>HAL packages are more intuitve APIs that build off the LL packages. This makes development easier but can lead to a slower program. The HAL packages should ideally be used as a learning tool to understand how to play around with ports and registers and whatnot.</li>
</ol>
Header files that are part of HAL will say the word "hal" in it probably. Most development for interfacing with the STM32 Nucleo board will probably just the following LL file: stm32h723xx.h.
