# Workshop: Smart Home Thermostat using PIC32MZ

This repository holds a workshop that leads you through a series of learning experiences using Amazon FreeRTOS and Mikroelektronika Flip N Click for PIC32MZ.   In working through the workshop labs you learn how you can leverage AWS IoT and Amazon FreeRTOS fora  specific use case. 

You can purchase the [Smart Home PIC32MZ Click Kit for AWS](https://www.mikroe.com/smart-home-pic32mz-click-kit-for-aws) or purchase components individually if you already have some of the parts.  See [Section 1](./Section1.md) for details on the individual parts.

![flip-kit](https://cdn1-shop.mikroe.com/img/product/smart-home-pic32mz-click-kit-for-aws/smart-home-pic32mz-click-kit-for-aws-thickbox_default-12x.jpg)


After this workshop, you will have a concrete understanding of how to develop for telematics  and command & control using both fundamental FreeRTOS concepts as well as mechanisms used for advanced IoT development.

## Section 1: Setup the Environment

In this section, you will setup the Microchip MPLAB development environment and the AWS Command Line Interface which is required to complete other sections in this workshop.  

[Go to Section 1](./Section1.md)

## Section 2: Establish Amazon FreeRTOS Connectivity

In this section, you will establish AWS IoT Core connectivity using Amazon FreeRTOS on the PIC32MZEF. You will experience basic connectivity by running the built-in demo program. You will then experience how to transition basic connectivity to a smart thermostat application.

[Go to Section 2](./Section2.md)

## Section 3: Telematics with AWS IoT Core

In this section, you will take temperature and humidity readings from the Weather click sensors and send the readings to AWS IoT Core for web console based visualization and data harvesting.  By doing so, you will learn how to organize code and cross-Task messaging.

[Go to Section 3](./Section3.md)

## Section 4: Command & Control with Device Shadow

In this section, you will learn the Device Shadow relevance to command and control requirements when implementing IoT applications. You will be able to identify the methods for acquiring the right data at the right time during the device operational lifecycle, including synchronization between local device operation and remote operation.

[Go to Section 4](./Section4.md)

## Section 5: Over-the-Air (OTA) Updates with Device Management

In this section, you will learn why Over the Air (OTA) Updates are important to the long term device viability and value, and how to implement workflows to facilitate OTA from both local device and AWS Cloud perspectives.

[Go to Section 5](./Section5.md)
