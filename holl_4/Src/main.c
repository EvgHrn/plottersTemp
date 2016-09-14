/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/


/* USER CODE BEGIN Includes */

#include "main.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/



/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

//------------  ����������� ����������  -----------------------------------

uint16_t S0_RX_BASE;      //  ��������� ����� ������, ���������� � RX ������ ��� ������0
uint16_t S0_RX_MASK;      // ������ RX  ������ ������0
uint16_t S0_RX_OFFSET;    // ��������� �� ������ ��������� ����� ������ � RX ������ 
uint16_t S0_RX_Start_Addr; // ����������  ����� ������ �������� ����� ������ � RX ������ 
uint16_t S0_RX_RSR ;       //  ������ ��������� �� ������� ����� ������
uint16_t S0_TX_BASE;    // ��������� ����� ������, ���������� � �X ������ ��� ������0
uint16_t S0_TX_MASK;     // ������ �X  ������ ������0
uint16_t S0_TX_OFFSET ;  
												 // ��������� �� ���������  ����� � ������  �� ������, ���� ������� �������� ���� 
												 // ������ ��� �������� �������  
uint16_t S0_TX_Start_Addr;  
												 // ����������  ��������� ����� � ������ �� ������, ���� ������� �������� 
												 // ����  ������ ��� �������� �������  
uint16_t S0_TX_End_Addr; 
												 // ��������� �� �������� ����� � ������ �� ������, �� �������� W5100
												 //  ������ ����� ��� ���������� ������ ���������� �������

// --- ������ �������� ��� ������������� ��� HTTP  ��������� � ����-���������
// * ������  ������� [222] ���� � ������� � � �������. ����� ���������  ���������  �������� ����� ������
// ** CONST � �����  ����������� ������ � ����������� ������ ( ��� ����� �����)
uint32_t main_count = 0;

const char ERROR404[222] = { "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 39\r\n\r\n<HTML><CENTER>ERROR 404</CENTER></HTML>"};
// ������� ���  ��������  ��������� ������� 404�

int try=0;  //  ��������, ��� �������. ����� ��������� �������� �� �������




/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	
	
	

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();

  /* USER CODE BEGIN 2 */
	
	uint32_t count_temp = 0;
	char uart_data[20] = "";
	
	Init();   // �������������  W5100  � ������� ( �������)
	
	OpenSocket0:
	
	//ooooooooooooooooooo ��������� ����� 0 oooooooooooooooooooooooo
	if ( ! Open_Socket0() ) goto OpenSocket0; // ���� ���� ����� �� ���������
	
	//ooooooooooooooooo  ������� �����  ooooooooooooooooooooooooo
	if ( Listen_Socket0() == 0 ) goto OpenSocket0; //����  ����� �� ���������������� - ������ ������ �� �������� ������
	
	//oooooooooooooo ���������� ����������� ? oooooooooooooo

	CheckConnection:

  if (Socket0_Connection_Established() == 0 ) goto CheckConnection;
	// ���� ���� ���������� �� ����������� ( ������ ���� �� ������ ������ �� ��������)
  char messsage_check[] = "Connection Established... \r\n";
	uprintf(messsage_check);
	// ��� ������� (����� �� ��������)  ����� �������             

	//ooooooo( ���������� ��� �����������)  � �������� ������� -  ������? oooooo                                 
  if  ( Socket0_Received_Data_Size() == 0 ) {
		char messsage_check2[] = "\r\n> (Zero) Received Data size is: %Lu (bytes) \r\n";
		uprintf(messsage_check2);
		// ��� ������� (����� �� ��������)  ����� �������
		goto CheckFIN;   // ��� ������ ��� � � ���������� � ���� ������
															// ������� ����� ���� �� �������� �����FIN
  } // ��� ������ ��� ( ������=0) �� ������ �� ���� �������� �������� ����������
   else    
   {
		 
		uprintf("(NonZero) Received Data size is: %Lu (bytes) \r\n");
                   // ��� ������� (����� �� ��������)  ����� �������
   }
  
	// oooooooooo ( ������ � ������ ����) ������� ���������*   ooooooooooooooo 

	//�� ����� ���� ������  ������� �� �������� �������� ������
	// ����� ������� ���� ����� ����� �������
	//Socket0_Received_Data_Reading();   
	// ��� ������� (����� �� ��������)  ����� �������

	// oooooooooooooo  ������� �������� ������  ooooooooooooooooooooooooooooooo
	// ������ �������� �������� HTTP ��������� � HTML ���������
 
	if ( Socket0_Received_Request_is_index_html() ==  1)   Socket0_Send_index_html ();
   // ����  ������ ����������� "index.html"  �� �������� ���  index.html

	else  Socket0_Send_404_error (); 
   //  ���� ������ ������ ������� �����  ��  ��������  ����������  ������� 404�
  
	//ooooooooooo    ������� ����  FIN ? ooooooooooooooooooooooooooo
	//  ������ �������� ������� ��  ������ ������� ����������

	CheckFIN:

	if ( Socket0_FIN_Received() == 1) goto  CloseConnection; //���� FIN  ������  - ��  ������ ��������� �����0

	//ooooooooooo    ������ ����������   ooooooooooooooooooooooo 
  Socket0_Disconnect();    //������ ���������� ������0

	//ooooooooooo   ����� ������ ?    ooooooooooooooooooooooooooooooo
	if (Socket0_Closed() == 1 )  goto CloseConnection; // ���� ����� ������ � ������ ��������� ����������

	//ooooooooooo   �� �������� �� ����-��� �� ����������  ?  ooooooooooooooooooo
	if ( Socket0_Connection_Timeout() == 1)  goto CloseConnection;
	//����� �� ������, �� �������� ����-���, � ������ ��������� �����

	CloseConnection:

	//ooooooooooo   ��������� �����    ooooooooooooooooooooooooooooooooooo
  Socket0_Closing();  
	//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

	goto OpenSocket0;
	// ���������� � �������� ����������, ���� ����������.
	//  ������ � ������ � ��������� ��� �� ����� ( ���� ����� �������� ����-����� )
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		

				
				
		

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** NVIC Configuration
*/
void MX_NVIC_Init(void)
{
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  /* SPI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
}

/* USER CODE BEGIN 4 */

void uprintf (char * str){
	HAL_UART_Transmit (&huart3, (uint8_t *)str, 100, 100);
}


void Init(void) {
	output_low(RESET); //���������� ������ ��� ������������� Ethernet  ����� 
	HAL_Delay(5);
	output_high(RESET);
	//output_low(SCK);  //������� ���� (������) ���  SPI
	output_high(SS) ;

	//����������� ����� ���� W5100.
	SetW5100register(MR, 0x80);   // ����� ���  RST  � W5100 Mode Register

	//------------ �����������  ������ (W5100) ��� ������ 0 -------------
	SetW5100register(RMSR, 0x55);   // ����������� RX �����: �� 2�� ��� ������ ����� 
	S0_RX_BASE = 0x6000;     //  ������� ����� RX  ������    ��� ������0
	S0_RX_MASK = 0x07FF ;   // (2048 -1 )= 0x07FF, RX ����� ( = �����_������ � 1 )
	SetW5100register(TMSR, 0x55);   // ����������� �X �����: �� 2�� ��� ������ ����� 
	S0_TX_BASE = 0x4000;  // ������� ����� TX ������  ��� ������0
	S0_TX_MASK = 0x07FF;  // (2048 -1 )= 0x07FF, �� ����� ( = �����_������ � 1 )

	//------------ ����������� ���� ��� �����   --------------
	//  �.�.  ������ ���������� � ��� �������� W5100 
	//    � ���� ����������  � ������������� ����� ����������
	SetW5100register(SHAR0, MAC0);  
	SetW5100register(SHAR1, MAC1); 
	SetW5100register(SHAR2, MAC2);
	SetW5100register(SHAR3, MAC3);
	SetW5100register(SHAR4, MAC4);
	SetW5100register(SHAR5, MAC5);
		
	//------------  ����������� ����  IP  --------------
	// ��� �� -  ����������� � �������� W5100 � ���� ����������
	SetW5100register(SIPR0, SERVER_IP0);
	SetW5100register(SIPR1, SERVER_IP1);
	SetW5100register(SIPR2, SERVER_IP2);
	SetW5100register(SIPR3, SERVER_IP3);
	 
	//------------  ����������� ���� PORT  --------------
	// ��� �� � ���� ��������� � � ��������
	SetW5100register(S0_PORT0, SERVER_PORT0);
	SetW5100register(S0_PORT1, SERVER_PORT1); 

	//------------ �����������  Gateway addr  --------------
	// ��� �� � ���� ��������� � � ��������
	SetW5100register(GAR0, GATEWAY_IP0); 
	SetW5100register(GAR1, GATEWAY_IP1); 
	SetW5100register(GAR2, GATEWAY_IP2); 
	SetW5100register(GAR3, GATEWAY_IP3); 

	//------------ set Subnet Mask  --------------
	SetW5100register(SUBR0, SUBNET_MASK0); 
	SetW5100register(SUBR1, SUBNET_MASK1); 
	SetW5100register(SUBR2, SUBNET_MASK2); 
	SetW5100register(SUBR3, SUBNET_MASK3); 
	char message_init[] = "W5100web_init\r\n";
	uprintf(message_init);
	// ��� ������� (����� �� ��������)  ����� �������
}

//--------------------------------------------------------------
//-------------  �������� ������ 0  ----------------------------
int Open_Socket0(void)
{
	// ��� ������� (����� �� ��������)  ����� �������
	char message_open_1[] = "TRY\r\n";
	uprintf(message_open_1);
	if (try ==255) try =0;
	else try++;
	char message_open_2[] = "Start Open Socket\r\n";
	uprintf(message_open_2);
						
	// ������������� ����� � � ����� ���, ��������� ����� ��������� 
	SetW5100register(S0_MR, 0x01);   

	// �������� � ������� ������ � ������� ������� ( �����)
	SetW5100register(S0_CR, OPEN);  

	// ��������� ��� ����� �������� �������
	// * � ������ ��� , ��  �SOCK_INIT�  ����� ��������� ��� ����� ��������
	if (GetW5100register(S0_SR) != SOCK_INIT)  // ��������� STATUS �������
		 {
				SetW5100register(S0_CR, CLOSE); // ���� �� �������� �� ���������
				return 0 ;                //  � ������� � ����� 0
		 }
	return 1;     //�������� �������.  ������� � ����� 1
}




//--------------------------------------------------------------
//-------------  ��������� �����0 � �����  �������  ------------

int Listen_Socket0 (void)
{
	char message_list[] = "Sock opened. Go Listen\r\n";								
	uprintf(message_list); // ��� ������� (����� �� ��������)  ����� �������


// �������� � ������� ������ ������� �������� ������ � ����� �������
// *������� � �.�. �� � ������, � ����������  ������� �� �������

SetW5100register(S0_CR, LISTEN);  


if ( GetW5100register(S0_SR) != SOCK_LISTEN)  //  ���������  ������� ���������
 {
  SetW5100register(S0_CR, CLOSE); //  ���� ����� �� ����� � ����� �������
  return 0;                   // ��������� ��� � ������� � ����� 0   
 } 
 
return 1;               //  ����� � ������ �������,  ������� �  ����� 1
 
}


//--------------------------------------------------------------------------
//-------------   �������� �����������  ��  ���������� �  �������0 ? --------
// * � ������ �� ������� ( ��������)

int Socket0_Connection_Established(void)
{

// ���� W5100  �������� �� ������� ������ �� ����������, �� �������� ��� ����� 
// �  ������������� ������ ACK �  �������� ���� ������� �� SOCK_ESTABLISHED
// (����������� � ������� �����������)
// ��������� ��� ����� ����  ��������� ���� � �������� ����������, 
// ���� ��� ����� -  ���������  �������� ���������


if  ( GetW5100register(S0_SR) == SOCK_ESTABLISHED)  return 1 ;

// ������ ������� ���������.  
// ����  ���������� ����� (���) = �SOCK_ESTABLISHED� �� ������� � 1

else return 0;     // ���� ��� � ������� � ����� 0

	char message_estab[] = "Connection Established\r\n";
	uprintf(message_estab);
                 // ��� ������� (����� �� ��������)  ����� �������


// *����� ���� ��� ���������� �����������,  ����� ����� � ������/�������� ������

}



//----    �������� ���� �� ������� ������ -------------
//-----------------------------------------------------

// * ���������� ������ ����� ���������  ���������� ����� � ������������
// �� ��� ���������� ���� �� � ��� ������ ������
//  (��������, ������  ��������/�����  � ���-�������)


int Socket0_Received_Data_Size (void)  

// �� ����� ����  ����������   ������ �������� ������ = ���� ��� ���

{

S0_RX_RSR = make16 (GetW5100register(S0_RX_RSR0), GetW5100register(S0_RX_RSR1) );  
// ������ ���  8-������ �������� ������� �������� ������ 
// RSR0 � ����� ����, RSR1 � ����� ����
// � ��������� 16-������ ����� -  ������ �������� ������


                uprintf("Received Data size is: %Lu (bytes) \r\n");
                 // ��� ������� (����� �� ��������)  ����� �������

if (S0_RX_RSR == 0 ) return 0;   // ��� ������ ( �.�. ������=0 )
                                     // ������� � ����� 0

else  return 1;    // ������ ������ �� ������� ( �.�. ������  -����)
                      // ������� � ����� 1



}

//---------- ����� �� �������� �������� ������ ---------------
//------------------------------------------------------------
// * �������  ������� ������ ��� �������.  �� ����� �������

void Socket0_Received_Data_Reading (void) 
{
	uint16_t n; 
	int RXbyte;

	S0_RX_OFFSET = make16 ( GetW5100register(S0_RX_RD0), GetW5100register(S0_RX_RD1) );  
// �� ����  8-��������� ���������  ���������  16 ��������� ���������� -
// ��������� ��  ������  �������� ������ � RX ������  ������0

  uprintf("S0_RX_RD (RX mem read pointer)\r\n");
	
  S0_RX_OFFSET = (S0_RX_OFFSET & S0_RX_MASK ) ;
 // �������� ������  �����  ����������� � ������� ����������� ��� �����0 ������

                 uprintf("> S0_RX_Offset = S0_RX_RD & S0_RX_MASK = %LX \r\n\r\n");
    

  S0_RX_Start_Addr = S0_RX_OFFSET + S0_RX_BASE ;

//��������� ���������� ����� ������  ������� ������ �������� �������� ������


                 uprintf("> S0_RX_Start_Addr = S0_RX_OFFSET + S0_RX_BASE = %LX (physical)\r\n\r\n");
 

                  uprintf(">  Going  to print-out Received Data... \r\n\r\n");
                  uprintf("ooooooooooooooooooooooooooooooooooooooooooo\r\n");


for (n=0; n < S0_RX_RSR ; n++)   
 {
  
   if ( S0_RX_Start_Addr > (S0_RX_BASE + S0_RX_MASK)  )  S0_RX_Start_Addr = S0_RX_BASE;

   RXbyte = GetW5100register(S0_RX_Start_Addr);
  
  
                   uprintf("%c");

   S0_RX_Start_Addr++;
 
 }

                   uprintf("END of  received data oooooooooooooo\r\n\r\n");
                   
 
}


// --- ��������� ��� �� ������� ������ ������ �� INDEX.HTML  ���� ------
//----------------------------------------------------------------------

int Socket0_Received_Request_is_index_html(void)  
{
int RXbyte=0;

 S0_RX_OFFSET = make16 ( GetW5100register(S0_RX_RD0), GetW5100register(S0_RX_RD1) );  
// �� ����  8-��������� ���������  ���������  16 ��������� ���������� -
// ��������� ��  ������  �������� ������ � RX ������  ������0


 

 S0_RX_OFFSET = (S0_RX_OFFSET & S0_RX_MASK ) ;
// �������� ������  �����  ����������� � ������� ����������� ��� �����0 ������

 
 S0_RX_Start_Addr = S0_RX_OFFSET + S0_RX_BASE ;  
//��������� ���������� ����� ������  ������� ������ �������� �������� ������

  
 
 
                uprintf("\r\n>----------- parsing HTTP header-------------\r\n");
                 // ��� ������� (����� �� ��������)  ����� �������

 
 while (RXbyte != 0x2F)   // ���� ������  "/"  � HTTP  ���������
 {
  
    if ( S0_RX_Start_Addr > (S0_RX_BASE + S0_RX_MASK)  )  S0_RX_Start_Addr = S0_RX_BASE;
// ������ ����� ������ �� ����������� � ������������� � ������  ������
// ���������, �� �����  �� �� �����  ������
// ���� �� � ���� � ����� ������ ������ � ������ ������������ ������ � ���� �����

   RXbyte = GetW5100register(S0_RX_Start_Addr);  
// ������ �� ������ RX ����,  �� �������  ��������� ��������������
  
  
                      uprintf("%c");
                 // ��� ������� (����� �� ��������)  ����� �������


   S0_RX_Start_Addr++;  // �������������� ����� � ������ �� ��������� �� ������� ����
 
 }
 
// ��� �� ����� ������ ��� ����� (���������) �� ������� � HTTP ��������� ������� �/�
// � ������ �����  ��������� �� ��������� �� "/"  ������.  
// ���� ���� � ��������� ���� ���-���

    RXbyte = GetW5100register(S0_RX_Start_Addr); //��������� ����  ������
 
 
 
          uprintf("\r\n> -------- END of parsing HTTP header -------\r\n");
                 // ��� ������� (����� �� ��������)  ����� �������


 
     if (RXbyte == 0x20)  return 1;  
// ���� ��� �������  - ������ ������ ����������� ���� ��� ��������, ������ index.html
//  ������� � ����� ������������� 1

     else return 0;                
// ���� ��� ��� �� ������ � ����� ������ ������, ������  ������������� �� index.html
// ������� � ����� ������ - 0

}





//------   �������� ������� �������� ������� 404� ------------
//-------------------------------------------------------------

// ��������� �� ����� ������0  ������ ������ �� HTTP ��������� �   HTML  ����
//  �������� ������� 404�,  �����  ���������� W5100  ������ � ����� 
// ����� ����� ������ � ������,  � ���� �������  ��������

void Socket0_Send_404_error (void) 
{   uint16_t n;
char TXbyte;
uint16_t datalength;



  S0_TX_OFFSET = make16 ( GetW5100register(S0_TX_RD0), GetW5100register(S0_TX_RD1) );  // �� ����  8-��������� ���������  ���������  16 ��������� ���������� - ��������� 
// ��  �����, ������ ����� ������ ��������� ���� ������ � �X ������  ������0
  
  
  S0_TX_OFFSET = (S0_TX_OFFSET & S0_TX_MASK ) ;
// �������� ������  �����  ����������� � ������� ����������� ��� �����0 ������
  
  
  S0_TX_Start_Addr = S0_TX_OFFSET + S0_TX_BASE ;
//��������� ���������� ��������� ����� ��� ���������� ����� ������ � ������ ��
  

  //��������� ����� ������  ���������� HTTP ��������� � HTML  ����
  datalength=0;
  while ( ERROR404[datalength] !=0) datalength++;  
// ��������  �������������� �datalength� ���� �� ��������� �� ������ 0�00 � ������
//  (0�00 -  ������� ����� ������)
 
 


// ��� ����, ����� W5100 �������� ���� ������, ��� ���������� ���������� 
// � �� ������ ������, ������� � ������, ������� ��� ��������� �����.
// ����� �����, W5100 ���������� �������   ����� ����� ������ � ������.

// �������� �� ����� ������ ����� ������ �� ���������� � ����� ������� W5100:


// ����������� �� ���� ��������� - ���������� ����� ����� ������ �� ������� ��������
// � ��������� �� ���  2-������� ���������
// *( � ������� �� �����������  ���������� ����������� ������ ������ ����� ������
// ����� �� ����� ���� ������ � ����������� ���������� ������)
  S0_TX_End_Addr = make16 ( GetW5100register(S0_TX_WR0), GetW5100register(S0_TX_WR1) );  

// ��������� � ������������� -  ����������� ����� ����� ������ ����� ������
      S0_TX_End_Addr += datalength ;      // increment to  fatalength
   // ������������  �������� ( ��������� �� ����� ����� ������) ����� ������������� 
   // ��  2 ����� �  ���������� �  �����  ��������   
      SetW5100register(S0_TX_WR0, make8( S0_TX_End_Addr ,1) );  // ������� ���� 
      SetW5100register(S0_TX_WR1, make8( S0_TX_End_Addr,0) );  // ������� ����
 
      
                 uprintf("\r\n>Data length is: %Lu \r\n");
                 uprintf("\r\n>--- Filling TX buffer  w  data: -----------\r\n");
                 // ��� ������� (����� �� ��������)  ����� �������
      
      
 // ������ ���������� � ���������� ����������  ������ �� � ������  �������
for (n=0; n < datalength; n++)  // ����  �� ����� ����� ������
  {
     TXbyte = ERROR404[n];  
// ������ ������� ���� �� ������,  � ������� ������� ���� ��� ���� ���������
// ( HTTP ��������� �  HTML ����)
 

// ��������� ��������� ��� �� W5100  ����� ������ ����� ������ � �� ������  
// ����� �� ��������� �  �������  ������, �  ����������, �������� � 10 ������
// �� ����� ������, ���������� ��� ������ ������� ������ ����� ����� ������
// ���������, �� �������  ��  �� �� ������� ������
 
  if (S0_TX_Start_Addr  > (S0_TX_BASE + S0_TX_MASK)) S0_TX_Start_Addr = S0_TX_BASE;
 // .. � ����  ����� �� ���� ������ � ��   ���������� ������� � ������ ������

// * W5100  � ����� ���� ����������� (���  ������ ����  ��  ���� � �� ���� ))� ����� //����� ���������� ������  - �����, �  ������, ����� ������ �� ����� ������  
//( ������ ������,  � �� �� ��������� �� ����� ������) � ����� ����� ����������
//  �� ������ ������
 

 // � ������� �������  ������ �������� ����� �� ����������� ���� 
// � ������ ������ ����� ������� ���� ������ � �  �����  ������ ������ ������ �� 
  SetW5100register( S0_TX_Start_Addr, TXbyte ) ;   
  
               
                       //putc(TXbyte);   
                    // ��� ������� (����� �� ��������)  ����� �������

  S0_TX_Start_Addr++ ;
  // �������������� ������� ����� (��� ������ ���������� ����� ������)
  
  }     
      
                  uprintf("\r\n>--- end of  Filling  -----------\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������

// ���, ��� ������ ��� �������� �� ���������� � ������, 
// �������, ��� ���� ������ �������������, ������ ����� �� � ����������    
  
    //  �������� � ������� ������ ������  - ������� SEND  
    SetW5100register(S0_CR, SEND);       
      
                    uprintf("> Data was  sent \r\n\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������

}


//-----------------------------------------------------------

//-- �������� ������� �������� �index.html� ---------------
//-------------------------------------------------------------------
void Socket0_Send_index_html (void) // send index.html "page"  index.html
{
// ����������� �� ����� � ��� ��������� ��� �� ��� ��� ������� �������� ������� 404�
// ������  �������  ����������  HTTP  ��������� �  HTML  ���� � ������

	uint16_t n;
	char TXbyte;
	uint16_t datalength;
	
	char count_string[10] = "";
	sprintf(count_string, "%d", main_count);
	char INDEX[222]  = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 43\r\n\r\n<HTML><meta http-equiv='refresh' content='2'/><CENTER>Passes: ";
	strcat (INDEX, count_string);
	strcat (INDEX, "</CENTER></HTML>");
	//char INDEX[222]  = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 43\r\n\r\n<HTML><meta http-equiv='refresh' content='5'/><CENTER>Passes: " + "</CENTER></HTML>";
// ������� ���  ��������  ��������� �index.html�

	uprintf("going to send INDEX.HTML.....\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������

  S0_TX_OFFSET = make16 ( GetW5100register(S0_TX_RD0), GetW5100register(S0_TX_RD1) );  // ���������  2-������� ��������� �� ������ ����� ��� ���������� ����� ������
  
  
  S0_TX_OFFSET = (S0_TX_OFFSET & S0_TX_MASK ) ;
  
  
  S0_TX_Start_Addr = S0_TX_OFFSET + S0_TX_BASE ;
  // ��������� ����������  ��������� �����  ��� ���������� ����� ������

	uprintf("\r\n  INDEX[i], datalegth -------------\r\n");
      
  //��������� ����� ����� ������
  datalength=0;

  while ( INDEX[datalength] !=0) datalength++; 
 // ������� ����� ������ � ���� �� ������ �� �������  0�00 � �������� ����� ������      

// ���������� �  W5100  ���������  �� ����� ����� ������ �� ��������
 S0_TX_End_Addr = make16 ( GetW5100register(S0_TX_WR0), GetW5100register(S0_TX_WR1) );  // ���������� ������ 2 ����� ��������  ��������

      S0_TX_End_Addr += datalength ;      // increment to  fatalength
      // ��������� ����� ������ ����� ������

 // ����� �������� ������ ����������� �� ���� 1-������� ���������     
      SetW5100register(S0_TX_WR0, make8( S0_TX_End_Addr ,1) );  // ����� ����
      SetW5100register(S0_TX_WR1, make8( S0_TX_End_Addr,0) );  // ����� ����

                 
                  uprintf("\r\n>Data length is: %Lu \r\n");
                  uprintf("\r\n>--- Filling TX buffer  w  data: -----------\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������
           
      
 // ��������� ����� � ������ ������� �� ��������
for (n=0; n < datalength; n++)
  {
   TXbyte= INDEX[n]; 
// ������ ������� ���� �� ������  ���������� ���� ��������  index.html
// (�.�.   HTTP Header  + HTML code)
  
  if (S0_TX_Start_Addr  > (S0_TX_BASE + S0_TX_MASK)) S0_TX_Start_Addr = S0_TX_BASE;
  // ��������� �� ����� �� �� ���� ������, � ���� �� � ������������� �� ������
  
  SetW5100register( S0_TX_Start_Addr, TXbyte ) ;  
   // ���������� ������� ���� ����� ������ ( ���� ���������) � �  ����� �� 
  

                //putc(TXbyte);  // printout to Terminal ( for testing purpose)
                      // ��� ������� (����� �� ��������)  ����� �������
   
  S0_TX_Start_Addr++ ;
  // ��������� � ���������� ������ �� ������
  
  }     
      
                 uprintf("\r\n>--- end of  Filling  -----------\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������
    
  // ����� �������� ����� ������ ������, ����� �� ���������� �������

    //  ����� � ������� ������ ������ - ������� SEND  
    SetW5100register(S0_CR, SEND);       
      
                     uprintf("> Data was  sent \r\n\r\n");
                    // ��� ������� (����� �� ��������)  ����� �������

}

//--------------------------------------------------------

 

//--- �������� �� �����, ��������� �� �������,  ������������� ���� FIN ? ------
//-----------------------------------------------------------------------------

int Socket0_FIN_Received(void)
{ 
// ���������  �� ������� ��  ������ ������� ��������� (������ ����������)
// ������ ���  ���� FIN  -  ������ �� ������ ����������

// ����� ��������� ����� �������� ���� � �������� ����������
// ���� ��� ����� � ����� ��������  �������� ��������� ������0


// ������� �  1  ����  FIN ������  �  � 0  ����  FIN� �� ����
if ( GetW5100register(S0_SR) == SOCK_CLOSE_WAIT) return 1;
else return 0;

}


//---------------------------------------------------------


 
// ------ ��������� ���������� ���  Socket0   -----------------
//-------------------------------------------------------------
void Socket0_Disconnect(void)
{


//  ��������� ����� �� ���������� � ��������
// ������ ����� ������� � ������� ������  ������0

SetW5100register(S0_CR, DISCON);

}





// ------  �������� ��� �����0 ������ --------------------
//--------------------------------------------------------
int Socket0_Closed(void)
{
// ����� ��� ���� ������ ����� ������� ���� � ������� ������ ������
//  ������� ������� (CLOSE), ���  ����� ����-����, ���  ��� ������� ����������

// �������� ���  �����0 ������������� ������

// * ����� ������ ����� ��������  ���� � �������� ����������,
// ���� ��� ����� -  ��������� �������� ���������
 

if  ( GetW5100register(S0_SR) == SOCK_CLOSED) return 1;
else return 0;
// ������� � 1  ���� ����� (��� ���������� ������0) �������
// ���� � 0 ���� �����  �� ��� ��� �� ��������


}





 
// -----  ��� �� ����-���� �� ���������� ������0 ? ------------
//-------------------------------------------------------------

int Socket0_Connection_Timeout(void)
{
// ���� �� ����� ������, ���  ������ ����� ������� ����������, ���  �� ������� �����
// ������ �� ��������   ��� � ��������� ���������� �� ����-���
//  ����� ����� �����������,  ���� ���  ������ ����� ����� ������� ������
// ( �  ���������������� TCP ������� ) � ��� �������, ������� ��������������� 
// ��� ����������� �������� ������ ��������


// ����-��� ����������� ���� ����� ���� �  �������� ����������
// ���� ��� � ��� � ����� �������� �������� ��������� ������0

// ���������: ��� �����, ������� ����������� �� ��������� �SOCK_CLOSED�
// - ����� ��� ����� � ����-�����??
// ���� � ��� ��� � �������� ��������� ��� ���������� ���� ��� ����-���,
// �� � W5100  ��� �SOCK_CLOSED�  ������ ����� �  � ����-�����
// �������  �������� �� ���� ��� � ������ ���������

if  ( GetW5100register(S0_SR) == SOCK_CLOSED) return 1;
else return 0;

// ������� � 1  ����   ����-���  ��������, ���� � 0 ���� ����-���� �� ����

}



// ------------  �������� ������0    ----------------------------
//----------------------------------------------------------------


void Socket0_Closing(void)
{
//  should be performed in case that connection is closed after data exchange,
// socket should be closed with Timeout occurrence, 
// or forcible disconnection is necessary due to abnormal operation etc.
  

                    uprintf(">going to Close Socket0 ..... \r\n");
                      // ��� ������� (����� �� ��������)  ����� �������
           
// �������� � ������� ������ ������0  ��� �� �������� ������
SetW5100register(S0_CR, CLOSE);


                   uprintf(">  ------ Socket CLOSED    ----- \r\n\r\n");
                      // ��� ������� (����� �� ��������)  ����� �������

}
 




// ---- ������ (�������) �����  ����� SPI   -------------
//--------------------------------------------------------


//* � ���� �������� ������� �SSPI� ������, ��� � ���������� ���� ����������� 
// ������� ������� (SPI� )�� ��� �������. ������� � ������� ���� ������� 
// � ���� �� ���� ���������� (��������) � ���������� � ��� �� ������ ���

// ���������� ������� � ������� ���� ���� ����� SPI   
//  ����� ������� ������ ����������  ������ (������ ������ ���� ���� ������)  ��������� 
//  ������ �������  - ������� ��� ������ � �������

//* ��������� ����� ������ -  �������� ��� W5100 ���  �������� ����� �� 0 � 1

void SSPI_write( int Data)
{
	HAL_SPI_Transmit (&hspi1, (uint8_t *)&Data, 1, 200);
//int i;
//int mask=0x80; // ������ ����� �� ������� ��� (������ �������� �� �������� ����)
//                // ���  ��� ��  ���������  SPI ��� W5100 ������ ������� 
//                // �� �������� ���� -  � ��������  ( MSB first)


////output_low(SCK);   // ������ ���� ���� ���������� ��� ���� � � ��������� ���������

//for (i=0; i<8;i++)    // ���� ��� ���������� 8 ���
// { 
//  //output_low(MOSI);   // ������ ���������� ������� �� ����� -  � 0 

//  if ( (mask & Data) != 0) output_high(MOSI);   
//          // ����  (����� & ������)  =1  ��  ���������� �� ����� 1
//         // ���� ��� � �� �� �����  ��� � �������� 0, ������������ �������� ����


//  output_high(SCK);   
//   // ������ ����� ����� ( �� 0 � 1)  - ������ �� ����  ���������� ������ ���� 
//   // � �����  ������ SPI � �   �������� W5100
// 
//  mask = mask>>1; ;  // �������� �����  �� 1 ��� ������
//                    // *����� ���������  ��� �������� �  �����  �����
//                    // �� � ���� ����� ���� ������  �������������� 
//                    // ������� �������� ����� ������� (������ ������� �����)

// 
//  output_low(SCK); // ���������  ����, ��������� ��� � ��������� ��������� ( 0 )
// }
// // �.. � ��� �����  ��� (�)

}
//----------------------------------------------------------



//----------- ������ ����� � �����   SPI  ------------------
//-----------------------------------------------------------


//* � ���� �������� �SSPI� ������, ��� � ���������� ���� ����������� 
// ������� ������� (SPI� )�� ��� �������. ������� � ������� ���� ������� 
// � ���� �� ���� ���������� � ���������� � ��� �� ������ ���

// ���������� ������� � ��������  ���� ����
//  ����� ������� ������ ���������� ��������� 
// ���������� �������  ������ ��������

// *������ ��� ������ �� ��������� W5100 -  �������� ���  �������� ����� �� 1 � 0

//* ������ ���� ��� ��������  ��  �������  ���������� ������ �� �����
//  � ������ ����� �����  ������������ �� ���������� ��������� ����� �� 0 � 1
// �  �� �������  �� �������  ����������  ���� � 1,  ������ ������
// � ������������ ������  ��������� ����� �� 1 � 0


uint8_t SSPI_read ( void)
{
	
int Data=0;
	
HAL_SPI_Receive (&hspi1, (uint8_t *)&Data, 1, 200);
	
//int i;
//int mask=0x80; // ������ ����� �� ������� ��� (������ �������� �� �������� ����)
//                // ���  ��� ��  ���������  SPI ��� W5100 ������ ������� 
//                // �� �������� ���� -  � ��������  ( MSB first)


////output_low(SCK);   // ������ ���� ���� ���������� ��� ���� � � ��������� ���������

//for (i=0; i<8;i++)  // ���� ��� ���������� 8 ���

// { 
//  output_high(SCK);   // ������ ����� ����� ( �� 0 � 1)      
//    
//  if ( input(MISO)!= 0) Data = Data | mask ; 
//  // ���� �� ����� 1, ��  ������ ���  ����� � � ��������  �������� ������������ �����
//  
//  mask = mask>>1; ;   // �������� �����  �� 1 ��� ������
//                    // *����� ��������� �  �����  �����
//                    // �� � ���� ����� ���� ������  �������������� 
//                    // ������� �������� ����� ������� (������ ������� �����)

//  
//  output_low(SCK); // ���������  ����, ��������� ��� � ��������� ��������� ( 0 )

// }

return Data;
}
//---------------------------------------------------------------



//-------  ������  ( ���������)  ��������� W5100 -------------
//--------------------------------------------------------------
void SetW5100register (uint16_t regaddr, uint8_t data)
// 2 ���������: 16 ��� ������ �������� � 8 ��� ������ ��� ������

{
output_low(SS); // ���������� ��� ������ ( ������ � 0)

SSPI_write (0xF0);  // ������  �������� � ������� ������� ������

                                   //*  make8 � ����������� 16 ��� - �  8 ���
SSPI_write (  make8(regaddr,1) );  // �������� �� ������ ������� ����  (MSB)
                                   // � �����  ��� �� SPI     


SSPI_write (  make8(regaddr,0) );  // �������� �� ������ ������� ����  (LSB)
                                   // � ����  ��� �� SPI  

SSPI_write (data);   // ����� �� SPI ������ ��� ������ � ������� 


output_high(SS);     //  ������� ���������  ( ������ � 1)

}
//---------------------------------------------------------




//-----  ������ ����������� ��������� W5100 ----------
//----------------------------------------------------
int  GetW5100register (uint16_t regaddr)
//�������� � 2-�������� ����� ��������
// ����������: 1 ���� ��������� �� �������� ������
{
int RegData;

output_low(SS); // ���������� ��� ������ ( ������ � 0)


SSPI_write (0x0F);  // ������  �������� ������� ������

                                   //*  make8 � ����������� 16 ��� �  8 ���
SSPI_write (  make8(regaddr,1) );  //  �������� �� ������ ������� ����  (MSB)
                                   // � ���������� ��� �� SPI     

SSPI_write (  make8(regaddr,0) );  //  �������� �� ������ ������� ����  (LSB)
                                   // � ���������� ��� �� SPI  

RegData = SSPI_read ();    // ������ � ����� W5100  ������ ��� ���������� (8 ���) 
                           // �������� �� ���������� ����� ���� ������


output_high(SS);    //  ������� ���������  ( ������ � 1)

return RegData;	// ������� �  ����������� ������

}


void output_high (uint16_t pin){
	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);
}

void output_low (uint16_t pin){
	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);
}

uint8_t make8(uint16_t n, uint8_t high){
	if (high == 1) return ((n & 0xFF00) >> 8);
	else return (n & 0xFF);
}

uint16_t make16(uint8_t high, uint8_t low){
	return ((high<<8) | (low));

}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
