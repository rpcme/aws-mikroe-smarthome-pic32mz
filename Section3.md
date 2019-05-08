# Section 3: Telematics with AWS IoT Core

In this section, you will take temperature and humidity readings from the Weather click sensors and send the readings to AWS IoT Core for web console based visualization and data harvesting.  By doing so, you will learn how to organize code and cross-Task messaging.

- How to decouple sensing tasks effectively
- Publish messages to AWS IoT Core
- Capture messages from the IoT Core broker and put into database

## Configure for Section 3

In this section, we will run code for working with the temperature and humidity sensor that is part of the kit.

Modify ```aws_home_automation_demo.h``` to include the capabilities for this section.  After modification, the ```#define``` code should look similar to the following.

```c
...
#define AWS_WORKSHOP 1

#if AWS_WORKSHOP == 1
#define AWS_WORKSHOP_SECTION_2_CONN_1    1
#define AWS_WORKSHOP_SECTION_2_CONN_2    1
#define AWS_WORKSHOP_SECTION_3_TELEMETRY 1
#define AWS_WORKSHOP_SECTION_4_SHADOW    0
#define AWS_WORKSHOP_SECTION_5_OTA       0
#else
...
```

## Monitor the IoT Core topic

When the code runs for sending data to AWS IoT Core, it will be sending the data to a specific topic to the device.  The topic is defined in ```aws_remote_hvac.c``` on line 45.

Let us subscribe to the topic in the test client to monitor data coming into AWS IoT Core from the device.

1. Identify the name of your Client ID by the value of ```clientcredentialIOT_THING_NAME``` from file ```aws_clientcredential.h```.
1. Login to the AWS Console using your IAM credentials if not logged in already.
2. From the title bar, choose **Services > IoT Core**.
3. From the left hand side, choose **Test**.
4. In the MQTT client, click **Subscribe to a topic**.
5. In the Subscription topic text field, and replacing CLIENT_ID with your Client ID, input:

    ```text
    thermostats/CLIENT_ID/status"
    ```
    
    And click the **Subscribe to topic** button.  


## Review the Temperature task

The code for the temperature sensor is in ```module_sensor.c```.  The sensing code block runs as a FreeRTOS task.  The task runs a state machine that takes the readings from the sensor and sends the readings to two different queues.  You can view the code for the readings on lines 112 and 120.

- ```qHVAC_Sensor```: The temperature and humidity readings from the sensor
- ```qHVAC_TargetT```: Initial reading for /target temperature/, which will be used more in Section 4 by using the Rotary.

## Review the HVAC Task Queue Consumer

The HVAC task manages a state machine.  When in the ```MODULE_STATE_ACTIVE``` state, it actively listens to the ```qHVAC_Sensor``` and ```qHVAC_TargetT``` queues.

In ```module_hvac.c```, line 232, the task seeks the ```qHVAC_Sensor``` queue.   When a message is found, it passes the data to function ```hvac_update_sensor```, which then passes the data to the ```qCONN_Sensor``` and ```qDISPLAY_Sensor``` queues.

In the same file, line 248, the tasks seeks the ```qHVAC_TargetT``` queue.  When a message is found, it passes the data to function ```hvac_update_target_t```, which then passes the data to the ```qCONN_TargetT``` and ```qDISPLAY_TargetT``` queues.

As you can see, the HVAC is the heart of the system, and decouples the sensor readings from the AWS Cloud connectivity and UI update tasks.  In the next subsection, we will focus on the connectivity update part.

## Review the Connectivity Task Queue Consumer

When the message is passed on the ```qCONN_Sensor``` queue, there is expectation that there is a queue consumer to pick up the messages and do something useful.  In ```aws_remote_hvac.c```, the ```xQueueReceive``` call to pick up messages from the queue is on **line 241**.

When a message is found on the queue, the message is passed along to a user defined function named ```publish_message``` (line 248).

The user defined function ```publish_message``` (line 430) performs the work for publishing messages to AWS IoT Core.  The code sets up the values for the ```MQTTAgentPublishParams_t``` structure declared as ```xPublishParameters```.  The ```MQTTAgentPublishParams_t``` structure is defined in the MQTT Client code, part of Amazon FreeRTOS.

Once the structure is setup, the function ```MQTT_AGENT_Publish``` (line 452) is made which triggers the MQTTAgent task to consume the message and attempt publishing to AWS IoT Core.

## Demonstrate the Publish

In this subsection, we will run the code to understand the code flow between tasks.

1. Set breakpoints at the following locations.

   | File | Line |
   |------+------|   
   | module_sensor.c | line 112 |
   | module_hvac.c | line 374 |
   | aws\_remote\_hvac.c | line 248 |

2. Start debugging by clicking **Debug Project**.
3. The first breakpoint will be hit at line 112.  The sensor module sends readings at a predefined interval to the queue.  Look at the contents of the structure to see the readings made by the sensor.

   Press the Continue button or F5.
4. The second breakpoint will be hit at line 374.  The message has been received to the HVAC module.  The HVAC module performs any necessary processing and forwards the data to the UI and Connection modules along the ```qCONN_Sensor``` and ```qDISPLAY_Sensor``` queues.  

   Press the Continue button or F5.
5. The third breakpoint will be hit at line 248.  The message has been received to the connectivity module along the ```qCONN_Sensor``` queue.

   Press the **Step Into (F7)** button. 
6. Walk through the lines in the ```publish_message``` code block by clicking **Step Over (F8)** until reaching the ```MQTT_AGENT_Publish``` (line 452) call.
7. Ensure the AWS IoT Console test client is setup to listen to the status topic ```thermostats/CLIENT_ID/status```, where CLIENT_ID is replaced with your Client ID.
8. Press Step Over (F8) and note a message should come through the IoT Console.

Remove all breakpoints and press Continue (F5).

## Persisting Data to DynamoDB

In this subsection, a DynamoDB table will be setup to capture the temperature and sensor data.  When sensor data comes through IoT Core an AWS IoT Rule Engine rule takes the payload and persists it to a DynamoDB table.

### Creating the DynamoDB Table

The interest is reporting which clients are emitting events. In such case, the composite primary key will be clientId (HASH) and timestamp (RANGE).

We will start by creating AWS objects from the leaf of the runtime dependency chain.  Start by creating the DynamoDB Table where the Lifecycle Event data will be persisted.

```bash
aws dynamodb create-table                                             \
    --table-name             $PREFIX_ThermostatData                   \
    --attribute-definitions  AttributeName=clientId,AttributeType=S   \
                             AttributeName=timestamp,AttributeType=N  \
    --key-schema             AttributeName=clientId,KeyType=HASH      \
                             AttributeName=timestamp,KeyType=RANGE    \
    --billing-mode           PAY_PER_REQUEST
```

### Setup AWS IoT Rule Engine Topic Rules

To record all of the Lifecycle Events, we can create four separate Topic Rules or a single Topic Rule.  In this case, we create a Topic Rule capturing all Lifecycle Events.  There is no need to setup a Rule for each client; the rule will be setup to work for any clientId reporting a Lifecycle Event.

First we'll enable actions from AWS IoT to DynamoDB by adding permissions to our Role Policy. Create the following permissions document named ```iotbc-iot-role-permission-ddb.json```.  **IMPORTANT**: change the Region and Account Number in the ARN.

```json
{
  "Version": "2012-10-17",
  "Statement": {
    "Effect": "Allow",
    "Action": "dynamodb:PutItem",
    "Resource": "arn:aws:dynamodb:us-east-1:012345678910:table/PREFIX_ThermostatData"
  }
}
```

Now add the permissions policy to the =iot-bootcamp= Role.

```bash
aws iam put-role-policy \
    --role-name iot-bootcamp \
    --policy-name iot-ddb-IotLifecycleEvents \
    --policy-document file://iotbc-iot-role-permission-ddb.json
```

Create the =connected= rule.  The rule SQL uses the =#= wildcard to capture all events.

Create the Rule JSON file named ```DynamoDBRule.json```.  **IMPORTANT**: change the Region and Account Number in the ARN.

```json
{
  "sql": "select * from '$aws/events/#'",
  "description": "Get lifecycle events and put them to DynamoDB",
  "ruleDisabled": false,
  "awsIotSqlVersion": "2016-03-23",
  "actions": [
    {
      "dynamoDB": {
        "tableName": "IotLifecycleEvents",
        "roleArn": "arn:aws:iam::012345678910:role/iot-bootcamp",
        "hashKeyField": "clientId",
        "hashKeyValue": "${clientId}",
        "hashKeyType": "STRING",
        "rangeKeyField": "timestamp",
        "rangeKeyValue": "${timestamp}",
        "rangeKeyType": "NUMBER"
      }
    }
  ]
}
```

Create the rule, naming it *LifecycleEvents* and using the Topic Rule payload that was created in the previous step.

```bash
aws iot create-topic-rule \
    --rule-name LifecycleEvents \
    --topic-rule-payload file://DynamoDBRule.json
```
   
### Test the Rule

Run the demo program to receive events, source them from a Rule, and send them to the DynamoDB table.

1. From MPLAB X, run **Debug Project**.  Eventually, the code connects to IoT Core.
2. From MPLAB X, click the Finish Debugger project (or SHIFT-F5).

In the AWS Console:

1. Navigate to the DynamoDB console.
2. On the left-hand side, click *Tables*.
3. In the table list, click IotLifecycleEvents.
4. On the right-hand side, click the *Items* tab.
5. Verify that the messages have been received.

## Outcomes

In this section, you learned how to:

- Separate concerns through Tasks and Queues
- Use the Amazon FreeRTOS MQTT Agent to publish messages to AWS IoT Core
- Persist telemetry to a DynamoDB table so you can later do something useful with it.

Now on to Section 4 where you will learn about Device Shadows!

[Next Section](./Section4.md)

[Index](./README.md)

Copyright (C) 2019 Amazon.com, Inc.  All Rights Reserved.
