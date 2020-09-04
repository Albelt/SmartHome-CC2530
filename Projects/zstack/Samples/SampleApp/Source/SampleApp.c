/* 这是一份很恶心的代码，一份代码需要实现3个节点的功能，通过COORDINATOR,SENSOR_NODE,ACTUATOR_NODE三个预处理
 * 命令来进行区别。
 * SENSOR_NODE: 定时采集数据，点播发送给协调器
 * COORDINATOR: 接收SENSOR_NODE的数据，通过串口转发给PC机，根据数据产生指令并广播发送给ACTUATOR_NODE
 * ACTUATOR_NODE: 接收COORDINATOR传送的指令，执行相应的动作
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

#include "General.h" //LED驱动

//根据节点不同导入不同的头文件
#if defined (SENSOR_NODE) //传感节点
  #include "DHT11.h"            //温湿度传感器驱动
  #include "HCSR501.h"          //人体红外传感器驱动
  #include "LightIntensity.h"   //光照传感器驱动
  #include "YL38.h"             //火焰传感器驱动
#elif defined (ACTUATOR_NODE) //执行节点
  #include "Beep.h"             //蜂鸣器驱动
  #include "Motor.h"            //步进电机驱动
  #include "Relay.h"            //继电器驱动
#endif

//根据节点不同定义不同的全局变量
#if defined (ACTUATOR_NODE)
  static uint8 __light;       //灯，1：亮，0：灭
  static uint8 __beep;        //蜂鸣器，1：响，0：不响
  static uint8 __motor_dir;   //电机转向，0：不转，1：顺时针，2：逆时针
  static uint8 __motor_speed; //电机转速，[1,10]
#elif defined (COORDINATOR)
  static uint8 light_status;  //用于记录灯光的状态
  static uint8 light_count;   //用于灯光的延时熄灭
  static uint8 fire_status;   //用于记录火灾警报的状态
  static uint8 fire_count;    //用于火灾警报的鸣响延时
#endif




uint8 AppTitle[] = "Smart Home"; //应用程序名称

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

afAddrType_t SampleApp_Periodic_DstAddr; //广播
afAddrType_t SampleApp_Flash_DstAddr;    //组播
afAddrType_t SampleApp_P2P_DstAddr;      //点播

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
  /*串口初始化*/
  MT_UartInit();                  //串口初始化
  MT_UartRegisterTaskID(task_id); //注册串口任务
  HalUARTWrite(0, "uart init\n", osal_strlen("uart init\n"));
  /*LED初始化*/
  LEDInit();
  /*灯光变量初始化*/
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

  /*广播配置*/ 
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
  
  /*点播配置*/
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;  
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT; 
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;            

  /*端点配置*/
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
          #if defined (COORDINATOR) //协调器接收SENSOR_NODE的数据
            Coordinator_Receive_Data(MSGpkt);
          #elif defined (ACTUATOR_NODE) //ACTUATOR_NODE接收协调器的指令
            ActuatorNode_Receive_Data(MSGpkt);
          #endif
          break;

        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ((SampleApp_NwkState == DEV_END_DEVICE) )
          {
          #if defined (SENSOR_NODE) //只有传感节点需要定期发送数据
            //启动定时器，每隔1s采集一次数据并发送
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
    //只有SensorNode主动发送数据
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
  uint8 temp,humid,light,human,flame;  //温度、湿度、光照、人体红外、火焰
  char str_temp[3];             //2个字符的温度显示数据
  char str_humid[3];            //2个字符的湿度显示数据
  uint8 msg[5];                  //通过无线发送的数据


  DHT11(&temp, &humid); //获取温湿度
  light = LightOn();    //获取光照强度
  human = HCSR501On();  //获取人体红外
  flame = YL38On();     //获取火焰
  
  /*输出到LCD显示*/
  Color    = BLACK;
  Color_BK = WHITE;
  //温度
  str_temp[0] = (temp / 10) + 48;  //温度十位
  str_temp[1] = (temp % 10) + 48;  //温度个位
  str_temp[2] = '\0';
  LCD_write_CN_string(7,  50, "温度：");
  LCD_write_EN_string(49, 50, str_temp);
  LCD_write_CN_string(63, 50, "℃");
  //湿度
  str_humid[0] = (humid / 10) + 48;  //湿度十位
  str_humid[1] = (humid % 10) + 48;  //湿度个位
  str_humid[2] = '\0';
  LCD_write_CN_string(7,  65, "湿度：");     
  LCD_write_EN_string(49, 65, str_humid );
  LCD_write_CN_string(63, 65, "％");
  //光照
  LCD_write_CN_string(7, 80, "光照：");
  if(light == 1)
    LCD_write_CN_string(49, 80, "白天");
  else 
    LCD_write_CN_string(49, 80, "黑夜");
  //人体红外
  LCD_write_CN_string(7,    95, "红外：");
  if(human == 1)
    LCD_write_CN_string(49, 95, "有人");
  else
    LCD_write_CN_string(49, 95, "无人");
  //火焰
  LCD_write_CN_string(7,    110, "火焰：");
  if(flame == 1)
    LCD_write_CN_string(49, 110, "有");
  else
    LCD_write_CN_string(49, 110, "无");



  /*点播发送给协调器*/
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
  uint8 control_cmd[4]; //控制命令，分别为__light,__beep,__motor_dir,__motor_speed的值
  uint8 uart_data[10];   //通过UART串口发送给PC机的数据，依次为pkt->cmd.Data和control_cmd的值，以及前9个数的和(用于数据校验)
  switch(pkt->clusterId)
  {
    case SAMPLEAPP_P2P_CLUSTERID: //协调器收到传感节点的点播数据
      /*简单的控制算法*/
      //灯光
      if( (pkt->cmd.Data[2] == 0) && (pkt->cmd.Data[3] == 1) ) //黑夜且有人则开灯
      {
        control_cmd[0] = 1;
        light_status = 1;
      }
      else //不满足黑夜且有人的条件，可能是人体红外传感器的探测问题，如果灯已开，延时60s再关
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
      //蜂鸣器
      if(pkt->cmd.Data[4] == 1) //火焰传感器检测到火焰
      {
        fire_status = 1;
        control_cmd[1] = 1;
      }
      else
      {
        if((fire_status == 1) && (fire_count < 60) ) //连续警报60s
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
      
      //步进电机
      if(pkt->cmd.Data[0] < 15) //制热
      {
        control_cmd[2] = 1;
        if(pkt->cmd.Data[0] > 10)
          control_cmd[3] = 1;
        else if(pkt->cmd.Data[0] > 5)
          control_cmd[3] = 2;
        else
          control_cmd[3] = 3;
      }
      else if(pkt->cmd.Data[0] > 25) //制冷
      {
        control_cmd[2] = 2;
        if(pkt->cmd.Data[0] <30)
          control_cmd[3] = 1;
        else if(pkt->cmd.Data[0] < 35)
          control_cmd[3] = 2;
        else
          control_cmd[3] = 3;
      }
      else //停转
      {
        control_cmd[2] = 0;
        control_cmd[3] = 0;
      }

      /*将操作指令通过广播发送到Actuator Node*/
      AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       4,
                       control_cmd,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );

      /*将数据转发到PC机*/
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

  switch(pkt->clusterId) //执行器接收到协调器广播的数据
  {
    case SAMPLEAPP_PERIODIC_CLUSTERID:
      /*修改外设控制变量的值，并且在LCD上显示*/
      __light       = pkt->cmd.Data[0];
      __beep        = pkt->cmd.Data[1];
      __motor_dir   = pkt->cmd.Data[2];
      __motor_speed = pkt->cmd.Data[3];
      str_speed[0] = (__motor_speed / 10) + 48;
      str_speed[1] = (__motor_speed % 10) + 48;
      str_speed[2] = '\0';
      //灯光
      LCD_write_CN_string(  7,  50, "灯光：");
      if(__light)
      {
        RelayOn();
        LCD_write_CN_string(49, 50, "开");
      }
      else
      {
        RelayOff();
        LCD_write_CN_string(49, 50, "关");
      }
      //火警
      LCD_write_CN_string(  7,  65, "火警：");
      if(__beep)
      {
        LCD_write_CN_string(49, 65, "开");
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);
        Beep(255); Beep(255); Beep(255); Beep(255); Beep(255);      
      }
      else
      {
        LCD_write_CN_string(49, 65, "关");
      }
      //空调
      LCD_write_CN_string(    7, 80, "空调：");
      if(__motor_dir)
      {
        if(__motor_dir == 1)
          LCD_write_CN_string(49, 80, "制热");
        else
          LCD_write_CN_string(49, 80, "制冷");
      }
      else
      {
        LCD_write_CN_string(  49, 80, "关闭");
      }
      //
      LCD_write_CN_string(7, 95, "功率：");
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



