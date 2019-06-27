# Section 4: Command & Control with Device Shadow

In this section, you will learn the Device Shadow relevance to command and control requirements when implementing IoT applications. You will be able to identify the methods for acquiring the right data at the right time during the device operational lifecycle, including synchronization between local device operation and remote operation.

## Configure for Section 4

In this section, we will run code for working with the temperature and humidity sensor that is part of the kit.

Modify ```aws_home_automation_demo.h``` to include the capabilities for this section.  After modification, the ```#define``` code should look similar to the following.

```c
...
#define AWS_WORKSHOP 1

#if AWS_WORKSHOP == 1
#define AWS_WORKSHOP_SECTION_2_CONN_1    1
#define AWS_WORKSHOP_SECTION_2_CONN_2    1
#define AWS_WORKSHOP_SECTION_3_TELEMETRY 1
#define AWS_WORKSHOP_SECTION_4_SHADOW    1
#define AWS_WORKSHOP_SECTION_5_OTA       0
#else
...
```

## Monitor the IoT Core topics

When the code runs for sending data to AWS IoT Core, it will be sending the data to a specific topic to the device.  The topic is defined in ```aws_remote_hvac.c``` on lines 65-68.

Let us subscribe to the topic in the test client to monitor data coming into AWS IoT Core from the device.

1. Identify the name of your Client ID by the value of ```clientcredentialIOT_THING_NAME``` from file ```aws_clientcredential.h```.
1. Login to the AWS Console using your IAM credentials if not logged in already.
2. From the title bar, choose **Services > IoT Core**.
3. From the left hand side, choose **Test**.
4. In the MQTT client, click **Subscribe to a topic**.
5. In the Subscription topic text field, for each of these topics, and replacing CLIENT_ID with your Client ID, input:

    ```text
    $aws/things/CLIENT_ID/shadow/update
    $aws/things/CLIENT_ID/shadow/update/accepted
    $aws/things/CLIENT_ID/shadow/update/rejected
    $aws/things/CLIENT_ID/shadow/update/delta
    ```
    
    And click the **Subscribe to topic** button.  

These topics enable you to see what messages are sent to the Shadow topics.  The messages to **update** usually originate from devices whereas messages from the other three topics originate from IoT Core itself. 

## Shadow Subscriptions and their Callbacks

Amazon FreeRTOS, and more specifically the MQTT Agent, implements a callback mechanism for developers to write message event code.  The subscription functions register the intent with the MQTT Agent.  The API Call to the MQTT Client API is ```MQTT_AGENT_Subscribe```.  The subscriptions for the Shadow topics using this API is in the ```subscribe_to_topic``` function in ```aws_remote_hvac.c```, line 427.

Each subscription deserves its own callback.

The **delta** subscription callback usually has the most work to do since it receives messages relating to local resolution, usually through some kind of actuation like turning the temperature higher or the fan lower.  The implemented function for this callback is ```prvMqttShadowDeltaCb``` on line 745.  We will go into detail on this in the next subsection.

The **accepted** subscription usually has the least work to do and can be considered a bookkeeping function.  If the Shadow update succeeds - on to the next!

The **rejected** subscription may seldom receive a message, but signifies an important event where you tried to update the Shadow but the server rejected your update - usually meaning there was a shadow version conflict but sometimes malformed data.




Enable the Thermostat tasks
Enable the HVAC Tasks


## Understanding the Shadow Document

The device uses the AWS IoT Core **Device Shadow** to enable Command & Control operations.  The device shadow is a JSON document composing four parts:

- **Desired** state: The target state you *desire* the target device to effect.
- **Reported** state: THe last state *reported* by the target device
- **Delta** state: a dynamic property determined by any difference between the Desired and Reported state
- **Version**: a section containing the modify time and auto-incrementing number representing the document version.

In the Smart Home scenario, the application has two states:

- **Temperature**: the numeric value for the wanted room temperature.
- **Fan**: off, high, and low.

There is another 'state', but this is driven dynamically based on the temperature.  This this is not a Command and Control determined value, it is not defined in the Device Shadow.

- **Aircon**: off, heat, cool




The following is an example structure of the JSON document.

```json
{
  "state": {
  	"desired": {
  	  "temp": 25.0,
  	  "fan": "off" },
  	"reported": {
  	  "temp": 25.0,
  	  "fan": "off" }
  }
}  	  
```

## Setting the Desired State

The *Desired State* of the device in the Device Shadow, for example wanting to turn the fan to ```low``` from the ```off``` state, can be set either by human interaction with the physical device by pressing the left user button or by changing the device shadow in the cloud by some other user application such as a web portal or mobile application.

In the case of local interation with the button, the local device can perform the actuation instantaneously, but also updates the device shadow so user applications can be aware that the desired state has changed.

What if a user application changes the shadow document at the same time the human changes the physical device?  In order to overcome the potential for having a race condition, the device shadow operations in the device should be version-aware.

When setting the temperature with the Rotary, we are setting a *desired* state.  The desired payload is structured the following way.

```c
#define mqttTARGET_DESIRED_PAYLOAD  ("{ \"state\":{\"desired\":{\"%s\":\"%.1f\"}}}")
```

When constructing the payload, the message will look similar to the following as if it were pretty printed.

```json
{
  "state": {
    "desired": {
      "temp": 22.0
    }
  }
}
```

When pressing the left hand button to change the fan, we are setting a *desired* state.  The desired state message is consumed by the hvac_task.  At the same time, we want to notify the Device Shadow that a new desired state has been 
 

### Check the Desired State

## Setting the Reported State

There are two sources where the reported state can be sourced:

- The temperature reading performed by the ```temperature_task``` where the code sends the reading along to the queue ```qHVAC_Sensor```.  The ```hvac_task``` then consumes the reading, performs work, and sends the value to the ```qCONN_Sensor``` queue, which is then consumed by the ```connector_task```.
- The button press event is checked in the ```hvac_task```.  When found that the button has been pressed, and the work of changing the fan state has been done, a message is sent along to queue ```qCONN_Fan```, which is then consumed by the ```connector_task```.  To understand what triggers the fan event *without* button press (desired state changed from cloud) see the previous section.

When the ```connector_task``` is in an ```MODULE_STATE_ACTIVE``` state, the task can then issue telemetry to the AWS Cloud.

For temperature, currently the reading is performed every five seconds.  Also, we don't need to update the shadow if the value is the same as the last reading.

```c
#define mqttSENSOR_PAYLOAD          ("{ \"state\":{\"reported\":{\"%s\":\"%s\",\"%s\":\"%s\"}}}")
```

When constructing this payload, the result will look similar to the following after being "pretty printed".

```json
{
  "state": {
    "reported": {
      "temp": 26.2,
      "humidity": 7.4
    }
  }
}
```

For fan state, whenever the state change has been made, an update will be made to the AWS Cloud.  In effect, the message structure is the following:

```c
#define mqttFAN_REPORTED_PAYLOAD    ("{ \"state\":{\"reported\":{\"%s\":\"%s\"}}}")
```

Remember, this message would only be sent out until *after* the fan change has been made by the HVAC system.

```json
{
  "state": {
    "reported": {
      "fan": "low"
    }
  }
}
```

### Check the Reported State

You can check the reported state by viewing the changes in the AWS Console.

1. In the AWS Console, click the top menu bar item Services and click IoT Core.
2. On the left hand side, choose **Manage** and click **Things**.
3. Locate your Thing on the right hand side and click it.
4.  


## Outcomes

In this section, you learned how to implement Command & Control with Device Shadows in AWS IoT Core and Amazon FreeRTOS.  You learned about the callbacks that need to be implemented in order to effectively interoperate with Device Shadows.  You also learned how to reuse queue pathways for clean data flow regardless of the command coming from the local system or the Device Shadow in the AWS Cloud. 

[Next Lab](./Section5.md)

[Index](./README.md)

Copyright (C) 2019 Amazon.com, Inc.  All Rights Reserved.
