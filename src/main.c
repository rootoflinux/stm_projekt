/*#include"stm32f3_discovery_lsm303dlhc.h"
#include"stm32f3_discovery.h"
#include "stm32f30x.h"
*/
#include "main.h"


#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

#define cas 50


/* Private variables ---------------------------------------------------------*/
  RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}



void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
	//TimingDelay_Decrement();
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


void Demo_CompassConfig(void)
{
  LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
  LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
  LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;
  
  /* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */
  LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
  LSM303DLHC_InitStructure.MagOutput_DataRate =LSM303DLHC_ODR_30_HZ ;
  LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_8_1_GA;
  LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
  LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);
  
   /* Fill the accelerometer structure */
  LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
  LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
  LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
  LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
  LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
  LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
  LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
  /* Configure the accelerometer main parameters */
  LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);
  
  /* Fill the accelerometer LPF structure */
  LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

  /* Configure the accelerometer LPF main parameters */
  LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

void Demo_CompassReadAcc(float* pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2];
  uint8_t buffer[6], cDivider;
  uint8_t i = 0;
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
  
  /* Read the register content */
  LSM303DLHC_Read( ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx, 2);
  LSM303DLHC_Read( ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);
   
  if(ctrlx[1]&0x40)
    cDivider=64;
  else
    cDivider=16;

  /* check in the control register4 the data alignment*/
  if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
  {
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider;
    }
  }
  else /* Big Endian Mode */
  {
    for(i=0; i<3; i++)
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
  }
  /* Read the register content */
  LSM303DLHC_Read( ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx, 2);


  if(ctrlx[1]&0x40)
  {
    /* FIFO mode */
    LSM_Acc_Sensitivity = 0.25;
  }
  else
  {
    /* normal mode */
    /* switch the sensitivity value set in the CRTL4*/
    switch(ctrlx[0] & 0x30)
    {
    case LSM303DLHC_FULLSCALE_2G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
      break;
    case LSM303DLHC_FULLSCALE_4G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
      break;
    case LSM303DLHC_FULLSCALE_8G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
      break;
    case LSM303DLHC_FULLSCALE_16G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
      break;
    }
  }

  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)pnRawData[i]/LSM_Acc_Sensitivity;
  }
	return;
}

void vyblikaj(float* pfData){
    int i;
    
    if(pfData==0){
        while(1){
            STM_EVAL_LEDOn(LED10);
            STM_EVAL_LEDOn(LED3);
            STM_EVAL_LEDOn(LED6);
            STM_EVAL_LEDOn(LED7);
            STM_EVAL_LEDOn(LED4);
            STM_EVAL_LEDOn(LED8);
            STM_EVAL_LEDOn(LED9);
            STM_EVAL_LEDOn(LED5);
            Delay(cas); 
            }
        }
        
        for(i = 0; i < 3; i++){

	//STM_EVAL_LEDOn(LED4); //testovaci 
            
                if(pfData[i] == 0){
                    
                        STM_EVAL_LEDOn(LED10);
                        STM_EVAL_LEDOff(LED3);
                        STM_EVAL_LEDOff(LED6);
                        STM_EVAL_LEDOff(LED7);
                        STM_EVAL_LEDOff(LED4);
                        STM_EVAL_LEDOff(LED8);
                        STM_EVAL_LEDOff(LED9);
                        STM_EVAL_LEDOff(LED5);
                        Delay(cas); 
                        
                }
                
                 if(pfData[i] > 0){
                    
                        STM_EVAL_LEDOff(LED10);
                        STM_EVAL_LEDOff(LED3);
                        STM_EVAL_LEDOn(LED6);
                        STM_EVAL_LEDOff(LED7);
                        STM_EVAL_LEDOff(LED4);
                        STM_EVAL_LEDOn(LED8);
                        STM_EVAL_LEDOff(LED9);
                        STM_EVAL_LEDOff(LED5);
                        Delay(cas);
                }
                 if(pfData[i] < 0){
                    
                        STM_EVAL_LEDOff(LED10);
                        STM_EVAL_LEDOff(LED3);
                        STM_EVAL_LEDOff(LED6);
                        STM_EVAL_LEDOn(LED7);
                        STM_EVAL_LEDOff(LED4);
                        STM_EVAL_LEDOff(LED8);
                        STM_EVAL_LEDOn(LED9);
                        STM_EVAL_LEDOff(LED5);
                        Delay(cas);
                }
            
           
            STM_EVAL_LEDOff(LED10);
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED6);
            STM_EVAL_LEDOff(LED7);
            STM_EVAL_LEDOff(LED4);
            STM_EVAL_LEDOff(LED8);
            STM_EVAL_LEDOff(LED9);
            STM_EVAL_LEDOff(LED5);
            Delay(cas);
            
        }
    
   return; 
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


int main(void){
    float pfData[3];
	int i;

	RCC_GetClocksFreq(&RCC_Clocks);
 	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

	STM_EVAL_LEDInit(LED3);
  	STM_EVAL_LEDInit(LED4);
  	STM_EVAL_LEDInit(LED5);
 	STM_EVAL_LEDInit(LED6);
 	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);
  	STM_EVAL_LEDInit(LED9);
  	STM_EVAL_LEDInit(LED10);
	
	//STM_EVAL_LEDOn(LED10); //pomocny bod
    

	Demo_CompassConfig();
    while(1)
	{ 
	
	
	for(i=0; i<3; i++)
		{
                	pfData[i]=0.0;
       	 	}
    
	
	//Demo_CompassReadAcc(pfData); //do pfData sa uloozia 3 cisla s 3 osi
    	
	//vyblikaj(pfData);
	
	//nevyskoci z vyblikaj
	STM_EVAL_LEDOff(LED3);
	Delay(50);
	STM_EVAL_LEDOn(LED3);
	Delay(50);
        



    	}

    
	//STM_EVAL_LEDOn(LED4); // test bod

	return 0;
}
