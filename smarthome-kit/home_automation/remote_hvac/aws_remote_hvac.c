/*
 * Amazon FreeRTOS MQTT Remote HVAC Demo V1.0.0
 */

/**
 * @file aws_remote_hvac.c
 * @brief 
 * 
 *              ----
 *
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "../aws_home_automation_demo.h"

#include "../module_common.h"
#include "module_sensor.h"
#include "module_hvac.h"
#include "module_display.h"

// --------------------------------------------------------------------- MACROS

//  MQTT client ID. Note that it must be unique per MQTT broker.

#define mqttCLIENT_ID           ((const uint8_t *)"RemoteHVAC")

//  The topic that the MQTT client publishes to.

#define mqttSTATUS_TOPIC_NAME   ((const uint8_t *)"clickdemo/remotehvac/status")

//  The topic that the MQTT client subscribes to.

#define mqttCONFIG_TOPIC_NAME   ((const uint8_t *)"clickdemo/remotehvac/config")

// ---------------------------------------------------------------------- TYPES

enum 
{
    DISCONNECTED,
    CONNECTED,
};

typedef struct
{
    MODULE_STATE        state;

} CONNECTION_DATA;

// ------------------------------------------------------------------ VARIABLES

CONNECTION_DATA         connectionData;

//  The handle of the MQTT client object used by the MQTT echo demo.
 
static MQTTAgentHandle_t        xMQTTHandle        = NULL;

//  Common connection parameters

static MQTTAgentConnectParams_t xConnectParameters;

//  Common subscribe parameters

static MQTTAgentSubscribeParams_t xSubscribeParams;

//  Common publish parameters

static MQTTAgentPublishParams_t xPublishParameters;

//  Common error code used to pring message via logger.

static MQTTAgentReturnCode_t    xErrorCode;

// ---------------------------------------------- PRIVATE FUNCTION DECLARATIONS

static void connector_task ( void );

static MODULE_RETURN create_client ( void );

static MODULE_RETURN delete_client ( void );

static MODULE_RETURN connect_to_broker ( void );

static MODULE_RETURN disconnect_from_broker ( void );

static MODULE_RETURN subscribe_to_topic ( void );

static MODULE_RETURN publish_message ( char *message );

static MQTTBool_t prvMQTTCallback ( void * pvUserData,
                    const MQTTPublishData_t * const pxPublishParameters );

// ----------------------------------------------------------- PUBLIC FUNCTIONS

void vStartRemoteHVACDemo ( void )
{
    configPRINTF( ( "Creating Connector Task...\r\n" ) );
    
    connectionData.state = MODULE_STATE_INIT;

    (void) xTaskCreate(     
                            (TaskFunction_t) connector_task, 
                            "Connector Task",
                            CONNECTOR_TASK_STACK_SIZE, 
                            NULL, 
                            CONNECTOR_TASK_PRIORITY, 
                            NULL 
                        );
}

// -------------------------------------------- PRIVATE FUNCTION IMPLEMENTATION

static void connector_task ( void )
{
    for ( ; ; )
    {
        switch ( connectionData.state )
        {
            case MODULE_STATE_INIT:
            {
                //  Attempt to create client.

                if ( create_client( ) == MODULE_OK )
                {
                    connectionData.state = MODULE_STATE_PREACTIVE;
                }
                else
                {
                    configPRINTF( ( "Failed to create client. [ERROR : %d]\r\n", 
                            xErrorCode ) );
                }
    
                break;
            }
            case MODULE_STATE_PREACTIVE:
            {
                //  Attempt to connect to broker.

                if ( connect_to_broker( ) == MODULE_OK )
                {
                    /*  
                        If connection to broker was successful attempt 
                        subscription to config topic.
                    */

                    if ( subscribe_to_topic( ) == MODULE_OK )
                    {
                        connectionData.state = MODULE_STATE_ACTIVE;
                    }
                    else
                    {
                        configPRINTF( ( "Failed to subscribe. [ERROR : %d]\r\n", 
                                xErrorCode ) );
                    }
                }
                else
                {
                    configPRINTF( ( "Failed to connect. [ERROR : %d]\r\n", 
                                xErrorCode ) );
                }
    
                break;
            }
            case MODULE_STATE_ACTIVE:
            {
                char            cDataBuffer[ 256 ];
    
                SENSOR_VALUE    sensorv;
                FAN_STATE       fanv;
                AIRCON_STATE    airconv;
                float           targetv;
                
                //  New fan data received - publish it to status topic.

                if ( xQueueReceive( qCONN_Fan, (FAN_STATE *) &fanv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttFAN_PAYLOAD, 
                                    jsonFAN_REFERENCE, 
                                    FAN_STATE_STRING[ fanv ] );
                    
                    if ( publish_message( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }

                //  New aircon data received - publish it to status topic.
    
                if ( xQueueReceive( qCONN_Aircon, (AIRCON_STATE *) &airconv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttAIRCON_PAYLOAD, 
                                    jsonAIRCON_REFERENCE, 
                                    AIRCON_STATE_STRING[ airconv ] );   
                    
                    if ( publish_message( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }

                //  New sensor data received - publish it to status topic.
    
                if ( xQueueReceive( qCONN_Sensor, (SENSOR_VALUE *) &sensorv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttSENSOR_PAYLOAD, 
                                    jsonSENSOR_T_REFERENCE, sensorv.temperature, 
                                    jsonSENSOR_H_REFERENCE, sensorv.humidity );
                    
                    if ( publish_message( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }

                //  New target data received - publish it to status topic.
    
                if ( xQueueReceive( qCONN_TargetT, (float *) &targetv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttTARGET_PAYLOAD, 
                                    jsonTARGET_T_REFERENCE, targetv );
                    
                    if ( publish_message( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }
                
                break;
            }
            case MODULE_STATE_POSTACTIVE:
            {
                //  Attempt to disconnect from broker.

                if ( disconnect_from_broker( ) == MODULE_OK )
                {
                    connectionData.state = MODULE_STATE_INACTIVE;
                }
                else
                {
                    configPRINTF( ( "Failed to disconnect. [ERROR : %d]\r\n",
                            xErrorCode ) );
                }
    
            }
            case MODULE_STATE_INACTIVE:
            default:
    
    
            break;
        }
        
        vTaskDelay( CONNECTOR_TASK_DELAY / portTICK_PERIOD_MS );
    }
}

static MODULE_RETURN create_client ( void )
{
    MODULE_RETURN xReturn = MODULE_ERROR;
    
    //  Fill connection parameters according to default configuration.
    
    xConnectParameters.pcURL              = clientcredentialMQTT_BROKER_ENDPOINT;
    xConnectParameters.xFlags             = democonfigMQTT_AGENT_CONNECT_FLAGS;
    xConnectParameters.xURLIsIPAddress    = pdFALSE;
    xConnectParameters.usPort             = clientcredentialMQTT_BROKER_PORT;
    xConnectParameters.pucClientId        = mqttCLIENT_ID;
    xConnectParameters.xSecuredConnection = pdFALSE;
    xConnectParameters.pvUserData         = NULL;
    xConnectParameters.pxCallback         = NULL;
    xConnectParameters.pcCertificate      = NULL;
    xConnectParameters.ulCertificateSize  = 0;
    xConnectParameters.usClientIdLength   = 
            (uint16_t) strlen( (const char *) mqttCLIENT_ID );
    
    //  Check this function has not already been executed.

    configASSERT( xMQTTHandle == NULL );

    /* 
        The MQTT client object must be created before it can be used. The
        maximum number of MQTT client objects that can exist simultaneously
        is set by mqttconfigMAX_BROKERS.
    */

    xErrorCode = MQTT_AGENT_Create( &xMQTTHandle );

    if ( xErrorCode == eMQTTAgentSuccess )
    {   
        xReturn = MODULE_OK;
    }
    
    return xReturn;
}

static MODULE_RETURN delete_client ( void )
{
    MODULE_RETURN xReturn = MODULE_ERROR;
    
    if ( MQTT_AGENT_Delete( xMQTTHandle ) == eMQTTAgentSuccess )
    {
        xReturn = MODULE_OK;
    }

    return xReturn;
}

static MODULE_RETURN connect_to_broker ( void )
{
    MODULE_RETURN xReturn = MODULE_ERROR;
    
    configPRINTF( ( "Attempting connection to %s.\r\n", 
            clientcredentialMQTT_BROKER_ENDPOINT ) );
    
    xErrorCode = MQTT_AGENT_Connect( xMQTTHandle, &xConnectParameters,
                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

    if ( xErrorCode == eMQTTAgentSuccess )
    {
        int tmp = CONNECTED;
        
        if ( xQueueSend( qDISPLAY_Conn, &tmp, RTOS_NO_BLOCKING ) )
        {
            //  TODO : Handle error.
        }
        
        configPRINTF( ( "Successfully connected to broker.\r\n" ) );
        xReturn = MODULE_OK;
    }
    
    return xReturn;
}

static MODULE_RETURN disconnect_from_broker ( void )
{
    MODULE_RETURN xReturn = MODULE_ERROR;
    
    configPRINTF( ( "Sttempting to disconnect from %s.\r\n", 
            clientcredentialMQTT_BROKER_ENDPOINT ) );
    
    xErrorCode = MQTT_AGENT_Disconnect( xMQTTHandle, democonfigMQTT_TIMEOUT );
    
    if ( xErrorCode == eMQTTAgentSuccess )
    {
        int tmp = DISCONNECTED;
        
        if ( xQueueSend( qDISPLAY_Conn, &tmp, RTOS_NO_BLOCKING ) )
        {
            //  TODO : Handle error.
        }
        
        configPRINTF( ( "Successfully disconnected.\r\n" ) );
        xReturn = MODULE_OK;
    }
    
    return xReturn;
}

static MODULE_RETURN subscribe_to_topic ( void )
{
    MODULE_RETURN xReturn = MODULE_ERROR;
   
    //  Setup subscribe parameters to subscribe to mqttTOPIC_NAME topic.
    
    xSubscribeParams.pucTopic = mqttCONFIG_TOPIC_NAME;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    xSubscribeParams.pxPublishCallback = prvMQTTCallback;
    xSubscribeParams.xQoS = eMQTTQoS1;
    xSubscribeParams.usTopicLength = 
            (uint16_t) strlen( (const char *) mqttCONFIG_TOPIC_NAME );

    //  Subscribe to the topic.
    
    xErrorCode = MQTT_AGENT_Subscribe( xMQTTHandle, &xSubscribeParams,
                        democonfigMQTT_TIMEOUT );

    if ( xErrorCode == eMQTTAgentSuccess )
    {
        configPRINTF( ( "Subscribed to %s\r\n", mqttCONFIG_TOPIC_NAME ) );
        xReturn = MODULE_OK;
    }

    return xReturn;
}

static MODULE_RETURN publish_message ( char *message )
{
    MODULE_RETURN xReturn = MODULE_ERROR;

    /* 
        Check this function is not being called before the MQTT client object 
        has been created. 
    */

    configASSERT( xMQTTHandle != NULL );

    //  Setup the publish parameters.

    memset( &xPublishParameters, 0x00, sizeof( xPublishParameters ) );

    xPublishParameters.pucTopic = mqttSTATUS_TOPIC_NAME;
    xPublishParameters.pvData = message;
    xPublishParameters.xQoS = eMQTTQoS1;
    xPublishParameters.ulDataLength = (uint32_t) strlen( message );
    xPublishParameters.usTopicLength = 
            (uint16_t) strlen( (const char *) mqttSTATUS_TOPIC_NAME );

    // Publish the message.

    xErrorCode = MQTT_AGENT_Publish( xMQTTHandle, &(xPublishParameters),
                        democonfigMQTT_TIMEOUT );

    if ( xErrorCode == eMQTTAgentSuccess )
    {
        configPRINTF( ( "Published %s\r\n", message ) );
        xReturn = MODULE_OK;
    }

    return xReturn;
}

static MQTTBool_t prvMQTTCallback ( void * pvUserData,
                        const MQTTPublishData_t * const pxPublishParameters )
{
    char plBuffer[ 256 ] = { 0 };
    
    //  Remove warnings about the unused parameters.

    (void) pvUserData;   
    //memset( plBuffer, 0, 256 );

    if ( pxPublishParameters->ulDataLength < 256 )
    {
        char *tmp;

        memcpy( plBuffer, pxPublishParameters->pvData, 
                (size_t) pxPublishParameters->ulDataLength );

        /*  
            Parse message received and forward it to HVAC module. Only FAN and
            Target temperature are configurable from the "outside" so only 
            that two references are searched through the received string.
        */

        // Target temerature reference search.

        if ( ( tmp = strstr( plBuffer, jsonTARGET_T_REFERENCE ) ) != NULL )
        {
            float       tmpV;
            char        *valS;
            char        *valE;
            char        tmpS[ 32 ];

            tmp += strlen( jsonTARGET_T_REFERENCE );
            valS = strchr( tmp + 1, '\"' );
            valE = strchr( valS + 1, '\"' );

            memset( tmpS, 0, 32 );
            memcpy( tmpS, valS + 1, valE - valS - 1 );
            sscanf (tmpS, "%f", &tmpV );

            //  Forward new target value to HVAC module.

            if ( xQueueSend( qHVAC_TargetT, &tmpV, RTOS_NO_BLOCKING ) )
            {
                //  TODO : Handle error.
            }
        }

        // Fan reference search.

        if ( ( tmp = strstr( plBuffer, jsonFAN_REFERENCE ) ) != NULL )
        {
            int         c;
            char        *valS;
            char        *valE;
            char        tmpS[32];

            tmp += strlen( jsonFAN_REFERENCE );
            valS = strchr( tmp + 1, '\"' );
            valE = strchr( valS + 1, '\"' );

            memset( tmpS, 0, 32 );
            memcpy( tmpS, valS + 1, valE - valS - 1 );

            for ( c = 0; c < 3; c++ )
            {
                //  Now check wich predefined value is received.

                if ( !strcmp( tmpS, FAN_STATE_STRING[ c ] ) )
                {
                    //  Forward new FAN value to HVAC module.

                    if ( xQueueSend( qHVAC_Fan, &c, RTOS_NO_BLOCKING ) )
                    {
                        //  TODO : Handle error.
                    }

                    break;
                }
            }
        }

        configPRINTF( ( "Received %s\r\n", plBuffer ) );
    }
    else
    {
        configPRINTF( ( "Droped message.\r\n" ) );
    }
    
    return eMQTTFalse;
}

/*----------------------------------------------------------------------------*/