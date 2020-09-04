/* ����һ�ݺܶ��ĵĴ��룬һ�ݴ�����Ҫʵ��3���ڵ�Ĺ��ܣ�ͨ��COORDINATOR,SENSOR_NODE,ACTUATOR_NODE����Ԥ����
 * ��������������
 * SENSOR_NODE: ��ʱ�ɼ����ݣ��㲥���͸�Э����
 * COORDINATOR: ����SENSOR_NODE�����ݣ�ͨ������ת����PC�����������ݲ���ָ��㲥���͸�ACTUATOR_NODE
 * ACTUATOR_NODE: ����COORDINATOR���͵�ָ�ִ����Ӧ�Ķ���
*/

#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"
#include "SampleApp.h"
#include "SampleAppHw.h"
#include "OnBoard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"

#include "General.h" //LED����

//���ݽڵ㲻ͬ���벻ͬ��ͷ�ļ�
#if defined (SENSOR_NODE) //���нڵ�
  #include "DHT11.h"            //��ʪ�ȴ���������
  #include "HCSR501.h"          //������⴫��������
  #include "LightIntensity.h"   //���մ���������
  #include "YL38.h"             //���洫��������
#elif defined (ACTUATOR_NODE) //ִ�нڵ�
  #include "Beep.h"             //����������
  #include "Motor.h"            //�����������
  #include "Relay.h"            //�̵�������
#endif

//���ݽڵ㲻ͬ���岻ͬ��ȫ�ֱ���
#if defined (ACTUATOR_NODE)
  static uint8 __light;       //�ƣ�1������0����
  static uint8 __beep;        //��������1���죬0������
  static uint8 __motor_dir;   //���ת��0����ת��1��˳ʱ�룬2����ʱ��
  static uint8 __motor_speed; //���ת�٣�[1,10]
#elif defined (COORDINATOR)
  static uint8 light_status;  //���ڼ�¼�ƹ��״̬
  static uint8 light_count;   //���ڵƹ����ʱϨ��
  static uint8 fire_status;   //���ڼ�¼���־�����״̬
  static uint8 fire_count;    //���ڻ��־�����������ʱ
#endif




uint8 AppTitle[] = "Smart Home"; //Ӧ�ó�������

const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID,
  SAMPLEAPP_P2P_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

endPointDesc_t SampleApp_epDesc;

uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr; //�㲥
afAddrType_t SampleApp_Flash_DstAddr;    //�鲥
afAddrType_t SampleApp_P2P_DstAddr;      //�㲥

aps_Group_t SampleApp_Group;
uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;


void SampleApp_SendFlashMessage( uint16 flashTime );

#if defined (SENSOR_NODE)
  void SensorNode_Send_Message(void);
#elif defined (COORDINATOR)
  void Coordinator_Receive_Data(afIncomingMSGPacket_t *pckt);
#elif defined (ACTUATOR_NODE)
  void ActuatorNode_Receive_Data(afIncomingMSGPacket_t *pckt);
#endif


void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
#if defined(COORDINATOR)
  /*���ڳ�ʼ��*/
  MT_UartInit();                  //���ڳ�ʼ��
  MT_UartRegisterTaskID(task_id); //ע�ᴮ������
  HalUARTWrite(0, "uart init\n", osal_strlen("uart init\n"));
  /*LED��ʼ��*/
  LEDInit();
  /*�ƹ������ʼ��*/
  light_status = 0;
  light_count  = 0;
  fire_status  = 0;
  fire_count   = 0;
#endif

#if defined (SENSOR_NODE)
  LEDInit();
  DHT11Init();
#endif

#if defined (ACTUATOR_NODE)
  LEDInit();
  BeepInit();
  StepMotorInit();
  RelayInit();
#endif


#if defined ( BUILD_ALL_DEVICES )
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  ZDOInitDevice(0);
#endif

  /*�㲥����*/ 
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
  
  /*�㲥����*/
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;  
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT; 
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;            

  /*�˵�����*/
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );
}


uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case AF_INCOMING_MSG_CMD:
          #if defined (COORDINATOR) //Э��������SENSOR_NODE������
            Coordinator_Receive_Data(MSGpkt);
          #elif defined (ACTUATOR_NODE) //ACTUATOR_NODE����Э������ָ��
            ActuatorNode_Receive_Data(MSGpkt);
          #endif
          break;

        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ((SampleApp_NwkState == DEV_END_DEVICE) )
          {
          #if defined (SENSOR_NODE) //ֻ�д��нڵ���Ҫ���ڷ�������
            //������ʱ����ÿ��1s�ɼ�һ�����ݲ�����
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );

          #endif
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    //ֻ��SensorNode������������
  #if defined (SENSOR_NODE)
    SensorNode_Send_Message();
  #endif
    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}


#if defined (SENSOR_NODE)
void SensorNode_Send_Message( void )
{
  uint8 temp,humid,light,human,flame;  //�¶ȡ�ʪ�ȡ����ա�������⡢����
  char str_temp[3];             //2���ַ����¶���ʾ����
  char str_humid[3];            //2���ַ���ʪ����ʾ����
  uint8 msg[5];                  //ͨ�����߷��͵�����


  DHT11(&temp, &humid); //��ȡ��ʪ��
  light = LightOn();    //��ȡ����ǿ��
  human = HCSR501On();  //��ȡ�������
  flame = YL38On();     //��ȡ����
  
  /*�����LCD��ʾ*/
  Color    = BLACK;
  Color_BK = WHITE;
  //�¶�
  str_temp[0] = (temp / 10) + 48;  //�¶�ʮλ
  str_temp[1] = (temp % 10) + 48;  //�¶ȸ�λ
  str_temp[2] = '\0';
  LCD_write_CN_string(7,  50, "�¶ȣ�");
  LCD_write_EN_string(49, 50, str_temp);
  LCD_write_CN_string(63, 50, "��");
  //ʪ��
  str_humid[0] = (humid / 10) + 48;  //ʪ��ʮλ
  str_humid[1] = (humid % 10) + 48;  //ʪ�ȸ�λ
  str_humid[2] = '\0';
  LCD_write_CN_string(7,  65, "ʪ�ȣ�");     
  LCD_write_EN_string(49, 65, str_humid );
  LCD_write_CN_string(63, 65, "��");
  //����
  LCD_write_CN_string(7, 80, "���գ�");
  if(light == 1)
    LCD_write_CN_string(49, 80, "����");
  else 
    LCD_write_CN_string(49, 80, "��ҹ");
  //�������
  LCD_write_CN_string(7,    95, "���⣺");
  if(human == 1)
    LCD_write_CN_string(49, 95, "����");
  else
    LCD_write_CN_string(49, 95, "����");
  //����
  LCD_write_CN_string(7,    110, "���棺");
  if(flame == 1)
    LCD_write_CN_string(49, 110, "��");
  else
    LCD_write_CN_string(49, 110, "��");



  /*�㲥���͸�Э����*/
  msg[0] = temp; msg[1] = humid; msg[2] = light; msg[3] = human; msg[4] = flame;
  if ( AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_P2P_CLUSTERID,
                       5,
                       msg,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }

  LED2FLIP;
}
#endif



#if defined (COORDINATOR)
void Coordinator_Receive_Data(afIncomingMSGPacket_t *pkt)
{
  uint8 control_cmd[4]; //��������ֱ�Ϊ__light,__beep,__motor_dir,__motor_speed��ֵ
  uint8 uart_data[10];   //ͨ��UART���ڷ��͸�PC�������ݣ�����Ϊpkt->cmd.Data��control_cmd��ֵ���Լ�ǰ9�����ĺ�(��������У��)
  switch(pkt->clusterId)
  {
    case SAMPLEAPP_P2P_CLUSTERID: //Э�����յ����нڵ�ĵ㲥����
      /*�򵥵Ŀ����㷨*/
      //�ƹ�
      if( (pkt->cmd.Data[2] == 0) && (pkt->cmd.Data[3] == 1) ) //��ҹ�������򿪵�
      {
        control_cmd[0] = 1;
        light_status = 1;
      }
      else //�������ҹ�����˵�������������������⴫������̽�����⣬������ѿ�����ʱ60s�ٹ�
      {
        if( (light_status == 1) && (light_count < 60) )
        {
          light_count++;
          control_cmd[0] = 1;
        }
        else
        {
          light_status = 0;
          light_count = 0;
          control_cmd[0] = 0;
        }
      }
      //������
      if(pkt->cmd.Data[4] == 1) //���洫������⵽����
      {
        fire_status = 1;
        control_cmd[1] = 1;
      }
      else
      {
        if((fire_status == 1) && (fire_count < 60) ) //��������60s
        {
          fire_count++;
          control_cmd[1] = 1;
        }
        else
        {
          fire_status = 0;
          fire_count  = 0;
          control_cmd[1] = 0;
        }
      }
      
      //�������
      if(pkt->cmd.Data[0] < 15) //����
      {
        control_cmd[2] = 1;
        if(pkt->cmd.Data[0] > 10)
          control_cmd[3] = 1;
        else if(pkt->cmd.Data[0] > 5)
          control_cmd[3] = 2;
        else
          control_cmd[3] = 3;
      }
      else if(pkt->cmd.Data[0] > 25) //����
      {
        control_cmd[2] = 2;
        if(pkt->cmd.Data[0] <30)
          control_cmd[3] = 1;
        else if(pkt->cmd.Data[0] < 35)
          control_cmd[3] = 2;
        else
          control_cmd[3] = 3;
      }
      else //ͣת
      {
        control_cmd[2] = 0;
        control_cmd[3] = 0;
      }

      /*������ָ��ͨ���㲥���͵�Actuator Node*/
      AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       4,
                       control_cmd,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );

      /*������ת����PC��*/
      uart_data[0] = pkt->cmd.Data[0]; 
      uart_data[1] = pkt->cmd.Data[1]; 
      uart_data[2] = pkt->cmd.Data[2]; 
      uart_data[3] = pkt->cmd.Data[3]; 
      uart_data[4] = pkt->cmd.Data[4];
      uart_data[5] = control_cmd[0];
      uart_data[6] = control_cmd[1];
      uart_data[7] = control_cmd[2];
      uart_data[8] = control_cmd[3];
      uart_data[9] = uart_data[0] + uart_data[1] + uart_data[2] + uart_data[3] + uart_data[4] + uart_data[5] + uart_data[6] + uart_data[7] + uart_data[8];
      HalUARTWrite(0, uart_data, 10);

      LED2FLIP;
      break;
  }
}
#endif



#if defined (ACTUATOR_NODE)
void ActuatorNode_Receive_Data(afIncomingMSGPacket_t *pkt)
{
  uint8 str_speed[3];

  switch(pkt->clusterId) //ִ�������յ�Э�����㲥������
  {
    case SAMPLEAPP_PERIODIC_CLUSTERID:
      /*�޸�������Ʊ�����ֵ��������LCD����ʾ*/
      __light       = pkt->cmd.Data[0];
      __beep        = pkt->cmd.Data[1];
      __motor_dir   = pkt->cmd.Data[2];
      __motor_speed = pkt->cmd.Data[3];
      str_speed[0] = (__motor_speed / 10) + 48;
      str_speed[1] = (__motor_speed % 10) + 48;
      str_speed[2] = '\0';
      //�ƹ�
      LCD_write_CN_string(  7,  50, "�ƹ⣺");
      if(__light)
      {
        RelayOn();
        LCD_write_CN_string(49, 50, "��");
      }
      else
      {
        RelayOff();
        LCD_write_CN_string(49, 50, "��");
      }
      //��
      LCD_write_CN_string(  7,  65, "�𾯣�");
      if(__beep)
      {
        LCD_write_CN_string(49, 65, "��");
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);      
      }
      else
      {
        LCD_write_CN_string(49, 65, "��");
      }
      //�յ�
      LCD_write_CN_string(    7, 80, "�յ���");
      if(__motor_dir)
      {
        if(__motor_dir == 1)
          LCD_write_CN_string(49, 80, "����");
        else
          LCD_write_CN_string(49, 80, "����");
      }
      else
      {
        LCD_write_CN_string(  49, 80, "�ر�");
      }
      //
      LCD_write_CN_string(7, 95, "���ʣ�");
      LCD_write_EN_string(49, 95, str_speed);
      if(__motor_dir)
      {
        StepMotor(__motor_dir - 1, __motor_speed, 250);
      }
		
      LED2FLIP;
      break;
      
  }
}
#endif



