Morsee Twitter Client
=====================
Morsee is a very simple Twitter client which can only send tweets, using [SuperTweet](http://supertweet.net) to authenticate with the service.

It runs on a **Texas Instruments Stellaris EKK-LM3S8962 Evaluation Kit**, which has a *ARM Cortex-M3* MCU, with some push buttons, a buzzer, ethernet and a tiny OLED display.

This project was developed at [Euskal Encounter 20](http://www.euskal.org) for the competition **Tu Gadget** (*Your Gadget*) in less than 36 hours, son don't expect a clean code, it just works.

All the code is released under the **BSD** license, unless it is said otherwise at the source file. It uses the following external pieces of code:
* The BSP (*Board Support Package*) provided by Texas Instruments with the evaluation kit.
* A library to use the peripherals from [Mondragon Unibertsitatea](http://www.mondragon.edu).
* [FreeRTOS](http://freertos.org) real time operating system.
* [uIP](http://sourceforge.net/projects/uip-stack/) TCP/IP network stack.

The account where we tested it, with some demonstration tweets is [MorseeBot](http://www.twitter.com/MorseeBot).
