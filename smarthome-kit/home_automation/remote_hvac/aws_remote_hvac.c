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
#include "jsmn.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "../aws_home_automation_demo.h"

#include "../module_common.h"
#include "module_sensor.h"
#include "module_display.h"
#include "aws_ota_agent.h"

#include "aws_application_version.h"

static void App_OTACompleteCallback( OTA_JobEvent_t eEvent );
void vOTAUpdateDemoTask( void * pvParameters );

static const char * pcStateStr[ eOTA_NumAgentStates ] =
{
    "Not Ready",
    "Ready",
    "Active",
    "Shutting down"
};

// --------------------------------------------------------------------- MACROS

//  MQTT client ID. Note that it must be unique per MQTT broker.

#define mqttCLIENT_ID           clientcredentialIOT_THING_NAME

//  The topic that the MQTT client publishes to.

#define mqttSTATUS_TOPIC_NAME   "thermostats/" mqttCLIENT_ID "/status"

//  The topic that the MQTT client subscribes to.

#define mqttCONFIG_TOPIC_NAME   "thermostats/" mqttCLIENT_ID "/config"
#define mqttSHADOW_GET          "$aws/things/" mqttCLIENT_ID "/shadow/get"
#define mqttSHADOW_UPDATE       "$aws/things/" mqttCLIENT_ID "/shadow/update"
#define mqttSHADOW_UPDATE_ACPT  "$aws/things/" mqttCLIENT_ID "/shadow/update/accepted"
#define mqttSHADOW_UPDATE_REJT  "$aws/things/" mqttCLIENT_ID "/shadow/update/rejected"
#define mqttSHADOW_UPDATE_DELTA "$aws/things/" mqttCLIENT_ID "/shadow/update/delta"


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

//  Common error code used to print message via logger.

static MQTTAgentReturnCode_t    xErrorCode;

// ---------------------------------------------- PRIVATE FUNCTION DECLARATIONS

static void connector_task ( void );

static MODULE_RETURN create_client ( void );

static MODULE_RETURN delete_client ( void );

static MODULE_RETURN connect_to_broker ( void );

static MODULE_RETURN disconnect_from_broker ( void );

static MODULE_RETURN subscribe_to_topic ( void );

static MODULE_RETURN publish_message ( char *message );

static MODULE_RETURN publish_shadow_update ( char *message );

static MQTTBool_t prvMQTTCallback ( void * pvUserData,
                    const MQTTPublishData_t * const pxPublishParameters );

// ----------------------------------------------------------- PUBLIC FUNCTIONS

void vStartRemoteHVACDemo ( void )
{
    
    /* Initialize the Application */
#if AWS_WORKSHOP_SECTION_2_CONN_2 == 1
    DISPLAY_Initialize();
#endif
#if AWS_WORKSHOP_SECTION_3_TELEMETRY == 1
    SENSOR_Initialize();
    HVAC_Initialize();
#endif
#if AWS_WORKSHOP_SECTION_4_SHADOW == 1
    THERMOSTAT_Initialize();
#endif

    configPRINTF( ( "Creating Connector Task...\r\n" ) );
    
    connectionData.state = MODULE_STATE_INIT;

    (void) xTaskCreate( (TaskFunction_t) connector_task, 
                        "Connector Task",
                        CONNECTOR_TASK_STACK_SIZE, 
                        NULL, 
                        CONNECTOR_TASK_PRIORITY, 
                        NULL 
                        );

#if AWS_WORKSHOP_SECTION_5_OTA == 1
    (void) xTaskCreate( vOTAUpdateDemoTask,
                        "OTA",
                        democonfigOTA_UPDATE_TASK_STACK_SIZE,
                        NULL,
                        democonfigOTA_UPDATE_TASK_TASK_PRIORITY,
                        NULL );
#endif
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
                    (void) sprintf( cDataBuffer, mqttFAN_DESIRED_PAYLOAD, 
                                    jsonFAN_REFERENCE, 
                                    FAN_STATE_STRING[ fanv ] );
                    
                    if ( publish_shadow_update( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }

                //  New aircon data received - publish it to status topic.
    
                if ( xQueueReceive( qCONN_Aircon, (AIRCON_STATE *) &airconv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttAIRCON_DESIRED_PAYLOAD, 
                                    jsonAIRCON_REFERENCE, 
                                    AIRCON_STATE_STRING[ airconv ] );   
                    
                    if ( publish_shadow_update( cDataBuffer ) != MODULE_OK )
                    {
                        configPRINTF( ( "Failed to publish %s. [ERROR: %d]\r\n", 
                            cDataBuffer, xErrorCode ) );
                    }
                }

                //  New sensor data received - publish it to status topic.
    
                if ( xQueueReceive( qCONN_Sensor, (SENSOR_VALUE *) &sensorv, 
                            RTOS_NO_BLOCKING ) )
                {
                    (void) sprintf( cDataBuffer, mqttSENSOR_PAYLOAD, clientcredentialIOT_THING_NAME, xTaskGetTickCount(),
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
                    (void) sprintf( cDataBuffer, mqttTARGET_DESIRED_PAYLOAD, 
                                    jsonTARGET_T_REFERENCE, targetv );
                    
                    if ( publish_shadow_update( cDataBuffer ) != MODULE_OK )
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
    
    configPRINTF( ( "Attempting to disconnect from %s.\r\n", 
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

/* Subscribe to a topic with a named callback.  At the time of writing we are 
   using this method to subscribe to shadow topics
 */
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

static MODULE_RETURN publish_shadow_update ( char *message )
{
    MODULE_RETURN xReturn = MODULE_ERROR;

    /* 
        Check this function is not being called before the MQTT client object 
        has been created. 
    */

    configASSERT( xMQTTHandle != NULL );

    //  Setup the publish parameters.

    memset( &xPublishParameters, 0x00, sizeof( xPublishParameters ) );

    xPublishParameters.pucTopic = mqttSHADOW_UPDATE;
    xPublishParameters.pvData = message;
    xPublishParameters.xQoS = eMQTTQoS1;
    xPublishParameters.ulDataLength = (uint32_t) strlen( message );
    xPublishParameters.usTopicLength = 
            (uint16_t) strlen( (const char *) mqttSHADOW_UPDATE );

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

        // Target temperature reference search.

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
        configPRINTF( ( "Dropped message.\r\n" ) );
    }
    
    return eMQTTFalse;
}

static void App_OTACompleteCallback( OTA_JobEvent_t eEvent )
{
    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    if( eEvent == eOTA_JobEvent_Activate )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" ) );
        OTA_ActivateNewImage();
    }
    else if( eEvent == eOTA_JobEvent_Fail )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" ) );
        /* Nothing special to do. The OTA agent handles it. */
    }
    else if( eEvent == eOTA_JobEvent_StartTest )
    {
        /* This demo just accepts the image since it was a good OTA update and networking
         * and services are all working (or we wouldn't have made it this far). If this
         * were some custom device that wants to test other things before calling it OK,
         * this would be the place to kick off those tests before calling OTA_SetImageState()
         * with the final result of either accepted or rejected. */
        configPRINTF( ( "Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n" ) );
        xErr = OTA_SetImageState( eOTA_ImageState_Accepted );

        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( " Error! Failed to set image state as accepted.\r\n" );
        }
    }
}

void vOTAUpdateDemoTask( void * pvParameters )
{
    OTA_State_t eState;
    OTA_AgentInit( xMQTTHandle,
                   ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ),
                   App_OTACompleteCallback, ( TickType_t ) ~0 );

    while( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_NotReady )
    {
        /* Wait forever for OTA traffic but allow other tasks to run and output statistics only once per second. */
        vTaskDelay( pdMS_TO_TICKS( 1000UL ) );
        configPRINTF( ( "State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n", pcStateStr[ eState ],
                        OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() ) );
    }
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if ( tok->type == JSMN_STRING &&
             (int) strlen(s) == tok->end - tok->start &&
             strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}
void jsonSimpleKeyValue(jsmn_parser p, char * payload, char * parent_key, char * target_key, char * result);
void jsonSimpleKeyValue(jsmn_parser p, char * payload, char * parent_key, char * target_key, char * result)
{
    int idx;
    int r;
    int target_parent_found = 0;
    int target_parent_end = 0; // where the token ends
    jsmntok_t tokens[128];

  #ifdef JSMN_PARSING_DEBUG
    char debug_token[128];
    int  debug_idx;
  #endif

    r = jsmn_parse( &p, ( char *) payload,  strlen(payload), tokens,
                    ( sizeof( tokens ) / sizeof( tokens[0] ) ) );

    for ( idx = 0; idx < r; idx++)
    {

  #ifdef JSMN_PARSING_DEBUG
      for (debug_idx=0; debug_idx < 128; debug_idx++) debug_token[debug_idx] = '\0';
      (void) strncpy(debug_token, (payload + tokens[idx].start), (tokens[idx].end - tokens[idx].start));
      printf("type: %d, start: %d, end: %d, size: %d, token: %s\n",
             tokens[ idx ].type,
             tokens[ idx ].start,
             tokens[ idx ].end,
             tokens[ idx ].size,
             debug_token
        );
  #endif

      // if the current object has key we want, then following string(s) contain key
      // we are searching for.
      if ( target_parent_found == 0 )
      {
        if ( tokens[idx].type == JSMN_OBJECT &&
             tokens[idx].size >  0           &&
             jsoneq( payload, &tokens[ idx - 1 ], parent_key ) == 0)
        {
          target_parent_found = 1;
          target_parent_end = tokens[ idx ].end;
        }
        continue;
      }

      if ( tokens[idx].end > target_parent_end ) return;

      // if the current is JSMN_OBJECT, skip.
      if ( tokens[ idx ].type == JSMN_OBJECT )
        continue;
      // if the current is JSMN_PRIMITIVE, then it is a value. skip.
      if ( tokens[ idx ].type == JSMN_PRIMITIVE )
        continue;

      // if the current is JSMN_STRING but has NO SIZE, then it is a VALUE.  skip.
      if ( tokens[ idx ].type == JSMN_STRING &&
           tokens[ idx ].size == 0 )
        continue;

      // if the current matches target_key, then retrieve value from idx+1 and return.
      // if idx+1 is a primitive, then convert to string.  we will only return char *.
      if ( ! ( tokens[ idx ].type == JSMN_STRING &&
               jsoneq( payload, &tokens[ idx ], target_key ) == 0 ) )
        continue;

      (void) strncpy(result, (payload + tokens[idx + 1].start), (tokens[idx + 1].end - tokens[idx + 1].start));
      return;
    }
}

static MQTTBool_t prvMqttShadowDeltaCb( void * pvUserData,
                                          const MQTTPublishData_t * const pxPublishParameters )
{
    jsmn_parser xJSMNParser;
    xShadowProperties shadowProperties;

    /* Silence compiler warnings about unused variables. */
    ( void ) pvUserData;

    char cBuffer[ 128 ];
    uint32_t ulBytesToCopy = ( 128 - 1 );
    memset( cBuffer, 0x00, sizeof( cBuffer ) );
    memcpy( cBuffer, pxPublishParameters->pvData, ( size_t ) ulBytesToCopy );

    jsmn_init( &xJSMNParser );
    memset( &shadowProperties, 0x00, sizeof( xShadowProperties ) );

    uint8_t target_temp, fan, aircon;
    char reading[4];

    memset( reading, 0x00, sizeof( reading ) );
    ( void ) jsonSimpleKeyValue( xJSMNParser,
                                 (char *) cBuffer,
                                 "state",  // TODO: remove magic string
                                 "TARGET_T",  // TODO: remove magic string
                                 reading);
    target_temp = atoi(reading);

    shadowProperties.target_temp = target_temp;

    // Brightness - if the thing does not handle brightness, this will be 0x00.
    memset( reading, 0x00, sizeof( reading ) );
    ( void ) jsonSimpleKeyValue( xJSMNParser,
                                 (char *) cBuffer,
                                 "state",  // TODO: remove magic string
                                 "FAN",  // TODO: remove magic string
                                 reading);
    fan = atoi(reading);
    shadowProperties.fan = fan;

    // Brightness - if the thing does not handle brightness, this will be 0x00.
    memset( reading, 0x00, sizeof( reading ) );
    ( void ) jsonSimpleKeyValue( xJSMNParser,
                                 (char *) cBuffer,
                                 "state",  // TODO: remove magic string
                                 "AIRCON",  // TODO: remove magic string
                                 reading);
    aircon = atoi(reading);
    shadowProperties.aircon = aircon;

    if( xQueueSendToBack( qCONN_ShadowReported, &shadowProperties, RTOS_NO_BLOCKING ) == pdTRUE )
    {
        configPRINTF( ( "Successfully added new reported state to update queue.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Update queue full, deferring reported state update.\r\n" ) );
    }

    return eMQTTFalse;
}

static void prvShadowHandlerTask( void * pvParameters );
static void prvShadowHandlerTask( void * pvParameters )
{
    xShadowProperties shadow;

    while(1)
    {
        if ( xQueueReceive( qCONN_ShadowReported,
                            &shadow, portMAX_DELAY) == pdFAIL) continue;

        configPRINTF(("Received message from queue: [%d]\r\n", shadow.target_temp));

        // Send the data out.
        // Send Fan setting to the Fan queue for the HVAC.
        xQueueSend( ( QueueHandle_t ) qHVAC_Fan, &( shadow.fan ), portMAX_DELAY);
        
        // Send Target Temperature to the TARGET_T queue for the HVAC.
        xQueueSend( ( QueueHandle_t ) qHVAC_TargetT, &( shadow.target_temp ), portMAX_DELAY);

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

static MQTTBool_t prvMqttShadowAcceptedCb( void * pvUserData,
                                        const MQTTPublishData_t * const pxPublishParameters );
static MQTTBool_t prvMqttShadowAcceptedCb( void * pvUserData,
                                        const MQTTPublishData_t * const pxPublishParameters )
{
        configPRINTF(("** Your update to the Device Shadow was accepted!\r\n"));
}

static MQTTBool_t prvMqttShadowRejectedCb( void * pvUserData,
                                        const MQTTPublishData_t * const pxPublishParameters );
static MQTTBool_t prvMqttShadowRejectedCb( void * pvUserData,
                                        const MQTTPublishData_t * const pxPublishParameters )
{
        configPRINTF(("** Your update to the Device Shadow was rejected!\r\n"));
}


/*----------------------------------------------------------------------------*/