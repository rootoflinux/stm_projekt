#include"stm32f3_discovery_lsm303dlhc.h"
#include"stm32f3_discovery.h"
#include "stm32f30x.h"

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

#define cas 100

  RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
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
            
           
            
            
        }
    
    
}

int main(){
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

	STM_EVAL_LEDOn(LED10); //pomocny bod
    
    while(1)
	{ 
	
	
	for(i=0; i<3; i++)
	{
                pfData[i]=0;
        }
    
	
	Demo_CompassReadAcc(pfData); //do pfData sa uloozia 3 cisla s 3 osi
    	STM_EVAL_LEDOn(LED7); // nezopina problem je v Demo_COmpassRead
   	vyblikaj(pfData);
	
	STM_EVAL_LEDOff(LED7); // test bod vyp

    	}

    
	STM_EVAL_LEDOn(LED4); // test bod


}
