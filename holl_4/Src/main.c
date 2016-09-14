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

//------------  собственные переменные  -----------------------------------

uint16_t S0_RX_BASE;      //  начальный адрес пам€ти, выделенной в RX буфере дл€ —окета0
uint16_t S0_RX_MASK;      // –ј«ћ≈– RX  буфера —окета0
uint16_t S0_RX_OFFSET;    // указатель на начало прин€того блока данных в RX буфере 
uint16_t S0_RX_Start_Addr; // ‘»«»„≈— »…  адрес начала принтого блока данных в RX буфере 
uint16_t S0_RX_RSR ;       //  –ј«ћ≈– прин€того от  лиента блока данных
uint16_t S0_TX_BASE;    // начальный адрес пам€ти, выделенной в “X буфере дл€ —окета0
uint16_t S0_TX_MASK;     // –ј«ћ≈– “X  буфера —окета0
uint16_t S0_TX_OFFSET ;  
												 // указатель на начальный  адрес в пам€ти  “’ буфера, куда следует записать блок 
												 // данных дл€ передачи  лиенту  
uint16_t S0_TX_Start_Addr;  
												 // ‘»«»„≈— »…  Ќачальный адрес в пам€ти “’ буфера, куда следует записать 
												 // блок  данных дл€ передачи  лиенту  
uint16_t S0_TX_End_Addr; 
												 // указатель на  онечный адрес в пам€ти “’ буфера, до которого W5100
												 //  должен дойти при считывании данных отсылаемых  лиенту

// --- строки символов дл€ использовани€ как HTTP  заголовки и Ђвеб-страничкиї
// * размер  массива [222] вз€т с потолка и с запасом. ѕотом программа  расчитает  реальную длину данных
// ** CONST Ц чтобы  расположить строку в ѕ–ќ√–јћћЌќ… пам€ти ( где места полно)
uint32_t main_count = 0;

const char ERROR404[222] = { "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 39\r\n\r\n<HTML><CENTER>ERROR 404</CENTER></HTML>"};
// символы дл€  создани€  странички Ђошибка 404Ф

int try=0;  //  временно, дл€ отладки. Ќомер обращени€ браузера Цк серверу




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
	
	Init();   // инициализаци€  W5100  и системы ( сервера)
	
	OpenSocket0:
	
	//ooooooooooooooooooo ќ“ –џ¬ј≈ћ —ќ ≈“ 0 oooooooooooooooooooooooo
	if ( ! Open_Socket0() ) goto OpenSocket0; // цикл пока —окет не откроетс€
	
	//ooooooooooooooooo  —Ћ”Ўј≈ћ —ќ ≈“  ooooooooooooooooooooooooo
	if ( Listen_Socket0() == 0 ) goto OpenSocket0; //если  сокет не Ђпрослушиваетс€ї - уходим заново на открытие сокета
	
	//oooooooooooooo —оединение ”—“јЌќ¬Ћ≈Ќќ ? oooooooooooooo

	CheckConnection:

  if (Socket0_Connection_Established() == 0 ) goto CheckConnection;
	// цикл пока соединение не установитс€ ( тоесть пока не придет запрос от браузера)
  char messsage_check[] = "Connection Established... \r\n";
	uprintf(messsage_check);
	// дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№             

	//ooooooo( соединение уже установлено)  в прин€тых пакетах -  ƒјЌЌџ≈? oooooo                                 
  if  ( Socket0_Received_Data_Size() == 0 ) {
		char messsage_check2[] = "\r\n> (Zero) Received Data size is: %Lu (bytes) \r\n";
		uprintf(messsage_check2);
		// дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
		goto CheckFIN;   // раз ƒјЌЌџ’ нет о и передавать в отет нечего
															// поэтому сразу идем на проверку флагаFIN
  } // раз данных нет ( размер=0) то уходим на этап проверки закрыти€ соединени€
   else    
   {
		 
		uprintf("(NonZero) Received Data size is: %Lu (bytes) \r\n");
                   // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
   }
  
	// oooooooooo ( ƒанные в пакете есть) ѕроцесс обработки*   ooooooooooooooo 

	//на самом деле просто  выводит на терминал прин€тые данные
	// после отладки этот кусок можно удалить
	//Socket0_Received_Data_Reading();   
	// дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

	// oooooooooooooo  ѕроцесс ѕ≈–≈ƒј„» данных  ooooooooooooooooooooooooooooooo
	// тоесть отсылаем браузеру HTTP заголовок и HTML страничку
 
	if ( Socket0_Received_Request_is_index_html() ==  1)   Socket0_Send_index_html ();
   // если  клиент запрашивает "index.html"  то остылаем ему  index.html

	else  Socket0_Send_404_error (); 
   //  если запрос любого другого файла  то  отсылаем  Ђстраничкуї  Ђошибка 404ї
  
	//ooooooooooo    ѕолучен флаг  FIN ? ooooooooooooooooooooooooooo
	//  тоесть проверка требует ли   лиент разрыва соединени€

	CheckFIN:

	if ( Socket0_FIN_Received() == 1) goto  CloseConnection; //если FIN  пришел  - то  уходим закрывать —ќ ≈“0

	//ooooooooooo    –азрыв соединени€   ooooooooooooooooooooooo 
  Socket0_Disconnect();    //разрыв —ќ≈ƒ»Ќ≈Ќ»я —окета0

	//ooooooooooo   —окет «ј –џ“ ?    ooooooooooooooooooooooooooooooo
	if (Socket0_Closed() == 1 )  goto CloseConnection; // если —окет закрыт Ц уходим закрывать —оединение

	//ooooooooooo   Ќе наступил ли тайм-аут по соединению  ?  ooooooooooooooooooo
	if ( Socket0_Connection_Timeout() == 1)  goto CloseConnection;
	//сокет не закрыт, но наступил тайм-аут, Ц уходим закрывать сокет

	CloseConnection:

	//ooooooooooo   «акрываем —окет    ooooooooooooooooooooooooooooooooooo
  Socket0_Closing();  
	//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

	goto OpenSocket0;
	// соединение с  лиентом отработано, цикл завершилс€.
	//  ”ходим в начало и запускаем все по новой ( ждем новых запросов Ђвеб-сайтаї )
	

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
	output_low(RESET); //генерируем Ђ—бросї дл€ јрдуиновского Ethernet  шилда 
	HAL_Delay(5);
	output_high(RESET);
	//output_low(SCK);  //готовим пины (уровни) дл€  SPI
	output_high(SS) ;

	//программный сброс чипа W5100.
	SetW5100register(MR, 0x80);   // пишем код  RST  в W5100 Mode Register

	//------------ Ќастраиваем  пам€ть (W5100) дл€ —окета 0 -------------
	SetW5100register(RMSR, 0x55);   // настраиваем RX буфер: по 2 Ѕ под каждый сокет 
	S0_RX_BASE = 0x6000;     //  базовый адрес RX  буфера    дл€ —окета0
	S0_RX_MASK = 0x07FF ;   // (2048 -1 )= 0x07FF, RX ћаска ( = длина_буфера Ц 1 )
	SetW5100register(TMSR, 0x55);   // настраиваем “X буфер: по 2 Ѕ под каждый сокет 
	S0_TX_BASE = 0x4000;  // базовый адрес TX буфера  дл€ —окета0
	S0_TX_MASK = 0x07FF;  // (2048 -1 )= 0x07FF, “’ ћаска ( = длина_буфера Ц 1 )

	//------------ прописываем свой ћј— адрес   --------------
	//  т.е.  просто закидываем в ћј— регистры W5100 
	//    Ц свои переменные  с определенными ранее величинами
	SetW5100register(SHAR0, MAC0);  
	SetW5100register(SHAR1, MAC1); 
	SetW5100register(SHAR2, MAC2);
	SetW5100register(SHAR3, MAC3);
	SetW5100register(SHAR4, MAC4);
	SetW5100register(SHAR5, MAC5);
		
	//------------  прописываем свой  IP  --------------
	// так же -  раскидываем в регистры W5100 Ц свои переменные
	SetW5100register(SIPR0, SERVER_IP0);
	SetW5100register(SIPR1, SERVER_IP1);
	SetW5100register(SIPR2, SERVER_IP2);
	SetW5100register(SIPR3, SERVER_IP3);
	 
	//------------  прописываем свой PORT  --------------
	// так же Ц свои константы Ц в регистры
	SetW5100register(S0_PORT0, SERVER_PORT0);
	SetW5100register(S0_PORT1, SERVER_PORT1); 

	//------------ прописываем  Gateway addr  --------------
	// так же Ц свои константы Ц в регистры
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
	// дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
}

//--------------------------------------------------------------
//-------------  ќ“ –џ“»≈ —ќ ≈“ј 0  ----------------------------
int Open_Socket0(void)
{
	// дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
	char message_open_1[] = "TRY\r\n";
	uprintf(message_open_1);
	if (try ==255) try =0;
	else try++;
	char message_open_2[] = "Start Open Socket\r\n";
	uprintf(message_open_2);
						
	// устанавливаем —окет Ц в режим “—–, остальные опции отключаем 
	SetW5100register(S0_MR, 0x01);   

	// засылаем в регистр команд Ц команду ќ“ –џ“№ ( сокет)
	SetW5100register(S0_CR, OPEN);  

	// провер€ем или сокет открылс€ успешно
	// * в режиме “—– , по  ЂSOCK_INITї  можно проверить или сокет открылс€
	if (GetW5100register(S0_SR) != SOCK_INIT)  // провер€ем STATUS регистр
		 {
				SetW5100register(S0_CR, CLOSE); // если не открылс€ то закрываем
				return 0 ;                //  и выходим с кодом 0
		 }
	return 1;     //открылс€ успешно.  ¬ыходим с кодом 1
}




//--------------------------------------------------------------
//-------------  ѕереводим —окет0 в режим  —Ћ”Ўј“№  ------------

int Listen_Socket0 (void)
{
	char message_list[] = "Sock opened. Go Listen\r\n";								
	uprintf(message_list); // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№


// засылаем в регистр команд команду перевода сокета в режим —Ћ”Ўј“№
// *—Ћ”Ўј“№ Ц т.к. мы Ц —ервер, а инициатива  исходит от  лиента

SetW5100register(S0_CR, LISTEN);  


if ( GetW5100register(S0_SR) != SOCK_LISTEN)  //  провер€ем  –егистр —осто€ни€
 {
  SetW5100register(S0_CR, CLOSE); //  если сокет не вошел в режим —Ћ”Ўј“ь
  return 0;                   // закрываем его и выходим с кодом 0   
 } 
 
return 1;               //  сокет в режиме —Ћ”Ўј“№,  выходим с  кодом 1
 
}


//--------------------------------------------------------------------------
//-------------   проверка ”становлено  ли  соединение с  —окетом0 ? --------
// * в смысле от клиента ( браузера)

int Socket0_Connection_Established(void)
{

// если W5100  получает от клиента запрос на соединение, он отсылает ему пакет 
// с  установленным флагом ACK и  измен€ет свой статуст на SOCK_ESTABLISHED
// (Ђсоединение с сокетом установлено)
// ѕроверить это можно либо  проверкой бита в регистре прерываний, 
// либо как здесь -  проверкой  регистра состо€ни€


if  ( GetW5100register(S0_SR) == SOCK_ESTABLISHED)  return 1 ;

// читаем –егистр —осто€ни€.  
// ≈сли  комбинаци€ битов (код) = ЂSOCK_ESTABLISHEDї то выходим с 1

else return 0;     // если нет Ц выходим с кодом 0

	char message_estab[] = "Connection Established\r\n";
	uprintf(message_estab);
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№


// *после того как соединение установлено,  сокет готов к приему/передаче данных

}



//----    проверка были ли прин€ты ƒјЌЌџ≈ -------------
//-----------------------------------------------------

// * Ђслужебныеї пакеты между сторонами  соединени€ могут и пересылатьс€
// но нас интересует были ли в них именно ƒјЌЌџ≈
//  (например, запрос  страницы/файла  с веб-сервера)


int Socket0_Received_Data_Size (void)  

// на самом деле  интересует   размер прин€тых данных = ЌќЋ№ или нет

{

S0_RX_RSR = make16 (GetW5100register(S0_RX_RSR0), GetW5100register(S0_RX_RSR1) );  
// читаем два  8-битных регистра размера прин€тых данных 
// RSR0 Ц старш байт, RSR1 Ц младш байт
// и Ђсобираемї 16-битное слово -  размер прин€тых данных


                uprintf("Received Data size is: %Lu (bytes) \r\n");
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

if (S0_RX_RSR == 0 ) return 0;   // нет ƒјЌЌџ’ ( т.е. размер=0 )
                                     // выходим с кодом 0

else  return 1;    // размер данных не нулевой ( т.е. данные  -есть)
                      // выходим с кодом 1



}

//---------- вывод на терминал прин€тых ƒјЌЌџ’ ---------------
//------------------------------------------------------------
// * функци€  полезна только при отладке.  ≈е ћќ∆Ќќ ”ƒјЋ»“№

void Socket0_Received_Data_Reading (void) 
{
	uint16_t n; 
	int RXbyte;

	S0_RX_OFFSET = make16 ( GetW5100register(S0_RX_RD0), GetW5100register(S0_RX_RD1) );  
// из двух  8-разр€дных регистров  склеиваем  16 разр€дную переменную -
// ” ј«ј“≈Ћ№ на  начало  прин€тых данных в RX буфере  сокета0

  uprintf("S0_RX_RD (RX mem read pointer)\r\n");
	
  S0_RX_OFFSET = (S0_RX_OFFSET & S0_RX_MASK ) ;
 // отсекаем лишнее  чтобы  укладывалс€ в размеры выделенного под —окет0 буфера

                 uprintf("> S0_RX_Offset = S0_RX_RD & S0_RX_MASK = %LX \r\n\r\n");
    

  S0_RX_Start_Addr = S0_RX_OFFSET + S0_RX_BASE ;

//вычисл€ем ‘»«»„≈— »… адрес начала  области пам€ти хран€щей прин€тые данные


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


// --- провер€ем или от  лиента пришел запрос на INDEX.HTML  файл ------
//----------------------------------------------------------------------

int Socket0_Received_Request_is_index_html(void)  
{
int RXbyte=0;

 S0_RX_OFFSET = make16 ( GetW5100register(S0_RX_RD0), GetW5100register(S0_RX_RD1) );  
// из двух  8-разр€дных регистров  склеиваем  16 разр€дную переменную -
// ” ј«ј“≈Ћ№ на  начало  прин€тых данных в RX буфере  сокета0


 

 S0_RX_OFFSET = (S0_RX_OFFSET & S0_RX_MASK ) ;
// отсекаем лишнее  чтобы  укладывалс€ в размеры выделенного под —окет0 буфера

 
 S0_RX_Start_Addr = S0_RX_OFFSET + S0_RX_BASE ;  
//вычисл€ем ‘»«»„≈— »… адрес начала  области пам€ти хран€щей прин€тые данные

  
 
 
                uprintf("\r\n>----------- parsing HTTP header-------------\r\n");
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

 
 while (RXbyte != 0x2F)   // ищем первый  "/"  в HTTP  заголовке
 {
  
    if ( S0_RX_Start_Addr > (S0_RX_BASE + S0_RX_MASK)  )  S0_RX_Start_Addr = S0_RX_BASE;
// начало блока данных не об€зательно м располагатьс€ в начале  буфера
// провер€ем, не дошли  ли до  ќЌ÷ј  буфера
// если да Ц идем в самое Ќј„јЋќ буфера Ц данный продолжаютс€ именно с того места

   RXbyte = GetW5100register(S0_RX_Start_Addr);  
// читаем из буфера RX байт,  на который  указывает —тартовыйјƒрес
  
  
                      uprintf("%c");
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№


   S0_RX_Start_Addr++;  // инкрементируем јдрес Ц теперь он указывает на следующ юайт
 
 }
 
// раз мы здесь значит уже дошли (Ђотловилиї) до первого в HTTP заголовке символа У/Ф
// и сейчас јдрес  указывает на следующий за "/"  символ.  
// –ади него и затевалс€ весь сыр-бор

    RXbyte = GetW5100register(S0_RX_Start_Addr); //считываем этот  символ
 
 
 
          uprintf("\r\n> -------- END of parsing HTTP header -------\r\n");
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№


 
     if (RXbyte == 0x20)  return 1;  
// если это Ђпробелї  - значит клиент запрашивает файл без названи€, тоесть index.html
//  выходим с кодом подтверждени€ 1

     else return 0;                
// если это был не пробел а любой другой символ, значит  запрашиваетс€ Ќ≈ index.html
// выходим с кодом ошибки - 0

}





//------   отправка  лиенту страницы Ђошибка 404ї ------------
//-------------------------------------------------------------

// заполн€ем “’ буфер сокета0  блоком данных из HTTP заголовка и   HTML  кода
//  страницы Ђошибка 404ї,  затем  показываем W5100  начало и конец 
// этого блока данных в буфере,  и даем команду  ќ“ќ—Ћј“№

void Socket0_Send_404_error (void) 
{   uint16_t n;
char TXbyte;
uint16_t datalength;



  S0_TX_OFFSET = make16 ( GetW5100register(S0_TX_RD0), GetW5100register(S0_TX_RD1) );  // из двух  8-разр€дных регистров  склеиваем  16 разр€дную переменную - ” ј«ј“≈Ћ№ 
// на  место, откуда можно начать размещать блок данных в “X буфере  сокета0
  
  
  S0_TX_OFFSET = (S0_TX_OFFSET & S0_TX_MASK ) ;
// отсекаем лишнее  чтобы  укладывалс€ в размеры выделенного под —окет0 буфера
  
  
  S0_TX_Start_Addr = S0_TX_OFFSET + S0_TX_BASE ;
//вычисл€ем ‘»«»„≈— »… начальный адрес дл€ размещени€ блока данных в буфере “’
  

  //вычисл€ем ƒЋ»Ќ” строки  содержащей HTTP заголовки и HTML  коды
  datalength=0;
  while ( ERROR404[datalength] !=0) datalength++;  
// попросту  инкрементируем Ђdatalengthї пока не наткнемс€ на первый 0х00 в строке
//  (0х00 -  признак конца данных)
 
 


// ƒл€ того, чтобы W5100 передала блок ƒанных, его необходимо разместить 
// в “’ буфере —окета, начина€ с адреса, который ммы вычислили ранее.
// ѕосле этого, W5100 необходимо указать    ќЌ≈÷ блока данных в буфере.

// ”казание на конец нашего блока данных мы записываем в соотв регистр W5100:


// ¬ытаскиваем из пары регистров - указателей конца блока данных их текущее значение
// и склеиваем из них  2-байтный указатель
// *( в отличие от предыдущего  вычислени€ ‘»«»„≈— ќ√ќ адреса начала длока данных
// здесь мы имеем дело именно с Ђвнутреннимї указателем адреса)
  S0_TX_End_Addr = make16 ( GetW5100register(S0_TX_WR0), GetW5100register(S0_TX_WR1) );  

// добавл€ем к получившемус€ -  вычисленную ранее длину нашего блока данных
      S0_TX_End_Addr += datalength ;      // increment to  fatalength
   // получившеес€  значение ( указатель на конец наших данных) вновь Ђрасклеииваемї 
   // на  2 байта и  записываем в  соотв  регистры   
      SetW5100register(S0_TX_WR0, make8( S0_TX_End_Addr ,1) );  // старший байт 
      SetW5100register(S0_TX_WR1, make8( S0_TX_End_Addr,0) );  // младший байт
 
      
                 uprintf("\r\n>Data length is: %Lu \r\n");
                 uprintf("\r\n>--- Filling TX buffer  w  data: -----------\r\n");
                 // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
      
      
 // теперь приступаем к собственно заполнению  буфера “’ Ц нашими  данными
for (n=0; n < datalength; n++)  // цикл  на длину блока данных
  {
     TXbyte = ERROR404[n];  
// читаем текущий байт из строки,  в которой записан весь код Ђвеб страницыї
// ( HTTP заголовки и  HTML коды)
 

// поскольку Ђвыданныйї нам от W5100  адрес начала блока данных в “’ буфере  
// может не совпадать с  началом  буфера, а  находитьс€, например в 10 байтах
// от конца буфера, необходимо при записи каждого нового байта наших данных
// провер€ть, не вылезли  ли  мы за границы буфера
 
  if (S0_TX_Start_Addr  > (S0_TX_BASE + S0_TX_MASK)) S0_TX_Start_Addr = S0_TX_BASE;
 // .. и если  дошли до кра€ буфера Ц то   продолжать следует с Ќј„јЋј буфера

// * W5100  в курсе этих манипул€ций (это  вообще была  ее  иде€ а не наша ))и когда //будет передавать данные  - также, в  случае, когда дойдет до конца буфера  
//( именно Ѕ”‘≈–ј,  а Ќ≈ до указател€ на  ќЌ≈÷ ƒанных) Ц также будет продолжать
//  от начала буфера
 

 // с “≈ ”ў»ћ адресом  записи текущего байта мы разобрались выше 
// и сейчас просто пишем текущий байт данных Ц в  соотв  €чейку пам€ти буфера “’ 
  SetW5100register( S0_TX_Start_Addr, TXbyte ) ;   
  
               
                       //putc(TXbyte);   
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

  S0_TX_Start_Addr++ ;
  // инкрементируем текущий адрес (дл€ записи следующего байта данных)
  
  }     
      
                  uprintf("\r\n>--- end of  Filling  -----------\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

// все, все данные дл€ передачи мы разместили в буфере, 
// указали, где наши данные заканчиваютс€, теперь можно их и передавать    
  
    //  засылаем в –егистр  оманд сокета  - команду SEND  
    SetW5100register(S0_CR, SEND);       
      
                    uprintf("> Data was  sent \r\n\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

}


//-----------------------------------------------------------

//-- отправка  лиенту страницы Ђindex.htmlї ---------------
//-------------------------------------------------------------------
void Socket0_Send_index_html (void) // send index.html "page"  index.html
{
// расписывать не стану Ц все абсолютно так же как при посылке страницы Ђошибка 404ї
// только  Ђстрокаї  содержаща€  HTTP  заголовки и  HTML  коды Ц друга€

	uint16_t n;
	char TXbyte;
	uint16_t datalength;
	
	char count_string[10] = "";
	sprintf(count_string, "%d", main_count);
	char INDEX[222]  = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 43\r\n\r\n<HTML><meta http-equiv='refresh' content='2'/><CENTER>Passes: ";
	strcat (INDEX, count_string);
	strcat (INDEX, "</CENTER></HTML>");
	//char INDEX[222]  = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 43\r\n\r\n<HTML><meta http-equiv='refresh' content='5'/><CENTER>Passes: " + "</CENTER></HTML>";
// символы дл€  создани€  странички Ђindex.htmlФ

	uprintf("going to send INDEX.HTML.....\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

  S0_TX_OFFSET = make16 ( GetW5100register(S0_TX_RD0), GetW5100register(S0_TX_RD1) );  // склеиваем  2-байтный указатель на началь адрес дл€ размещени€ блока данных
  
  
  S0_TX_OFFSET = (S0_TX_OFFSET & S0_TX_MASK ) ;
  
  
  S0_TX_Start_Addr = S0_TX_OFFSET + S0_TX_BASE ;
  // вычисл€ем ‘»«»„≈— »…  начальный адрес  дл€ размещени€ блока данных

	uprintf("\r\n  INDEX[i], datalegth -------------\r\n");
      
  //вычисл€ем ƒЋ»Ќ” блока данных
  datalength=0;

  while ( INDEX[datalength] !=0) datalength++; 
 // считаем длину данных Ц пока не дойдем до первого  0х00 Ц признака конца данных      

// записываем в  W5100  ” ј«ј“≈Ћ№  на  ќЌ≈÷ блока данных на передачу
 S0_TX_End_Addr = make16 ( GetW5100register(S0_TX_WR0), GetW5100register(S0_TX_WR1) );  // Ђсклеиваемї вместе 2 байта текущего  значени€

      S0_TX_End_Addr += datalength ;      // increment to  fatalength
      // добавл€ем длину нашего блока данных

 // новое значение заново распихиваем по двум 1-байтным регистрам     
      SetW5100register(S0_TX_WR0, make8( S0_TX_End_Addr ,1) );  // старш байт
      SetW5100register(S0_TX_WR1, make8( S0_TX_End_Addr,0) );  // младш байт

                 
                  uprintf("\r\n>Data length is: %Lu \r\n");
                  uprintf("\r\n>--- Filling TX buffer  w  data: -----------\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
           
      
 // заполн€ем буфер Ц нашими данными на передачу
for (n=0; n < datalength; n++)
  {
   TXbyte= INDEX[n]; 
// читаем текущий байт из строки  содержащей Ђвеб страницуї  index.html
// (т.е.   HTTP Header  + HTML code)
  
  if (S0_TX_Start_Addr  > (S0_TX_BASE + S0_TX_MASK)) S0_TX_Start_Addr = S0_TX_BASE;
  // провер€ем не дошли ли до кра€ буфера, и если да Ц перескакиваем на начало
  
  SetW5100register( S0_TX_Start_Addr, TXbyte ) ;  
   // зарисываем текущий байт блока данных ( Ђвеб страницыї) Ц в  буфер “’ 
  

                //putc(TXbyte);  // printout to Terminal ( for testing purpose)
                      // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
   
  S0_TX_Start_Addr++ ;
  // переходим к следующему адресу “’ буфера
  
  }     
      
                 uprintf("\r\n>--- end of  Filling  -----------\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
    
  // буфер заполнен нашим блоком данных, можно их передавать  лиенту

    //  пишем в –егистр  оманд сокета - команду SEND  
    SetW5100register(S0_CR, SEND);       
      
                     uprintf("> Data was  sent \r\n\r\n");
                    // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

}

//--------------------------------------------------------

 

//--- содержит ли пакет, пришедший от  лиента,  установленный флаг FIN ? ------
//-----------------------------------------------------------------------------

int Socket0_FIN_Received(void)
{ 
// провер€ем  не пожелал ли  клиент разрыва отношений (тоесть соединени€)
// послав нам  флаг FIN  -  запрос на разрыв соединени€

// можно провер€ть через проверку бита в регистре прерываний
// либо как здесь Ц через проверку  –егистра —осто€ни€ —окета0


// выходим с  1  если  FIN пришел  и  с 0  если  FINа не было
if ( GetW5100register(S0_SR) == SOCK_CLOSE_WAIT) return 1;
else return 0;

}


//---------------------------------------------------------


 
// ------ разрываем —ќ≈ƒ»Ќ≈Ќ»≈ дл€  Socket0   -----------------
//-------------------------------------------------------------
void Socket0_Disconnect(void)
{


//  отключаем сокет от —ќ≈ƒ»Ќ≈Ќ»я с клиентом
// заслав соотв команду в –егистр  оманд  сокета0

SetW5100register(S0_CR, DISCON);

}





// ------  проверка или —окет0 «ј –џ“ --------------------
//--------------------------------------------------------
int Socket0_Closed(void)
{
// —окет мож быть закрыт после засылки нами в –егистр  оманд —окета
//  команды «ј –џ“ь (CLOSE), или  после тайм-аута, или  при разрыве соединени€

// проверка или  —окет0 действительно закрыт

// * можно делать через проверку  бита в регистре прерываний,
// либо как здесь -  проверкой –егистра —осто€ни€
 

if  ( GetW5100register(S0_SR) == SOCK_CLOSED) return 1;
else return 0;
// выходим с 1  если —окет (или —ќ≈ƒ»Ќ≈Ќ»≈ сокета0) закрыто
// либо с 0 если сокет  до сих пор не закрылс€


}





 
// -----  нет ли “ј…ћ-ј”“а по соединению —окета0 ? ------------
//-------------------------------------------------------------

int Socket0_Connection_Timeout(void)
{
// если на линии ошибки, или  клиент хочет закрыть соединение, или  от клиента давно
// ничего не приходит   итп Ц провер€ем соединение на тайм-аут
//  иначе будет некорректно,  если наш  сервер будет слать клиенту пакеты
// ( с  соответствующими TCP флагами ) в том пор€дке, который подразумеваетс€ 
// дл€ нормального процесса обмена пакетами


// тайм-аут провер€етс€ либо через биты в  регистре прерываний
// либо как у нас Ц через проверку –егистра —осто€ни€ сокета0

// ѕ–»≈„јЌ»≈: как видим, регистр провер€етс€ на состо€ние ЂSOCK_CLOSEDї
// - кака€ тут св€зь с тайм-аутом??
// ƒело в том что в –егистре —осто€ний нет отдельного кода дл€ тайм-аут,
// но в W5100  код ЂSOCK_CLOSEDї  св€зан также и  с тайм-аутом
// поэтому  проверка на этот код Ц вполне легитимна

if  ( GetW5100register(S0_SR) == SOCK_CLOSED) return 1;
else return 0;

// выходим с 1  если   тайм-аут  наступил, либо с 0 если тайм-аута не было

}



// ------------  «ј –џ“»≈ —окета0    ----------------------------
//----------------------------------------------------------------


void Socket0_Closing(void)
{
//  should be performed in case that connection is closed after data exchange,
// socket should be closed with Timeout occurrence, 
// or forcible disconnection is necessary due to abnormal operation etc.
  

                    uprintf(">going to Close Socket0 ..... \r\n");
                      // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№
           
// засылаем в –егистр  оманд сокета0  код на «ј –џ“»≈ сокета
SetW5100register(S0_CR, CLOSE);


                   uprintf(">  ------ Socket CLOSED    ----- \r\n\r\n");
                      // дл€ отладки (вывод на терминал)  ћќ∆Ќќ ”ƒјЋ»“№

}
 




// ---- «јѕ»—№ (посылка) байта  через SPI   -------------
//--------------------------------------------------------


//* у мен€ название функции ЂSSPIї оттого, что в компайлере есть собственна€ 
// готова€ йункци€ (SPIЕ )но она глючна€. ѕоэтому € написал свой вариант 
// а чтоб не было конфликтов (названий) в компайлере Ц дал ей другое им€

// назначение функции Ц послать ќƒ»Ќ Ѕј…“ через SPI   
//  более верхние уровни Ђпротоколаї  обмена (тоесть запись всех “–≈’ байтов)  реализует 
//  друга€ функци€  - функци€ дл€ записи в –≈√»—“–

//* состо€ние линаа данных -  валидное дл€ W5100 при  переходе клока из 0 в 1

void SSPI_write( int Data)
{
	HAL_SPI_Transmit (&hspi1, (uint8_t *)&Data, 1, 200);
//int i;
//int mask=0x80; // ставим маску на —“ј–Ў»… бит (тоесть начинаем со —“ј–Ў≈√ќ бита)
//                // так  как по  протоколу  SPI дл€ W5100 данные следуют 
//                // от —“ј–Ў≈√ќ бита -  к ћЋјƒЎ≈ћ”  ( MSB first)


////output_low(SCK);   // просто чтоб быть уверенными что клок Ц в Ђисходномї положении

//for (i=0; i<8;i++)    // цикл дл€ считывани€ 8 бит
// { 
//  //output_low(MOSI);   // просто выставл€ем Ђданныеї на линии -  в 0 

//  if ( (mask & Data) != 0) output_high(MOSI);   
//          // если  (маска & ƒјЌЌџ≈)  =1  то  выставл€ем на линию 1
//         // если нет Ц то на линии  так и остаетс€ 0, выставленный строчкой выше


//  output_high(SCK);   
//   // выдаем пульс клока ( из 0 в 1)  - именно по нему  происходит запись бита 
//   // с линии  данных SPI Ц в   регистры W5100
// 
//  mask = mask>>1; ;  // сдвигаем маску  на 1 бит вправо
//                    // *можно поставить  эту операцию и  после  цикла
//                    // но в этом месте этим заодно  обеспечиваетс€ 
//                    // некотор задержка между тактами (тоесть частота клока)

// 
//  output_low(SCK); // завершаем  клок, переводим его в Ђисходноеї состо€ние ( 0 )
// }
// // Ђ.. и так восэм  расї (с)

}
//----------------------------------------------------------



//----------- „“≈Ќ»≈ байта с линии   SPI  ------------------
//-----------------------------------------------------------


//* у мен€ название ЂSSPIї оттого, что в компайлере есть собственна€ 
// готова€ йункци€ (SPIЕ )но она глючна€. ѕоэтому € написал свой вариант 
// а чтоб не было конфликтов в компайлере Ц дал ей другое им€

// назначение функции Ц прочесть  ќƒ»Ќ Ѕј…“
//  более верхние уровни Ђпротоколаї реализует 
// собственно функци€  чтени€ –≈√»—“–ј

// *данные при чтении из регистров W5100 -  валидные при  переходе клока из 1 в 0

//* тоесть если при передаче  мы  сначала  выставл€ли данные на линию
//  и только после этого  подтверждали их валидность переводом клока из 0 в 1
// а  на прниеме  мы сначала  выставл€ем  клок в 1,  читаем данные
// и Ђзащелкиваемї данные  переходом клока из 1 в 0


uint8_t SSPI_read ( void)
{
	
int Data=0;
	
HAL_SPI_Receive (&hspi1, (uint8_t *)&Data, 1, 200);
	
//int i;
//int mask=0x80; // ставим маску на —“ј–Ў»… бит (тоесть начинаем со старшего бита)
//                // так  как по  протоколу  SPI дл€ W5100 данные следуют 
//                // от —“ј–Ў≈√ќ бита -  к ћЋјƒЎ≈ћ”  ( MSB first)


////output_low(SCK);   // просто чтоб быть уверенными что клок Ц в Ђисходномї положении

//for (i=0; i<8;i++)  // цикл дл€ считывани€ 8 бит

// { 
//  output_high(SCK);   // выдаем пульс клока ( из 0 в 1)      
//    
//  if ( input(MISO)!= 0) Data = Data | mask ; 
//  // если на линии 1, то  делаем »Ћ»  маски Ц и текущего  значени€ Ђсобираемогої байта
//  
//  mask = mask>>1; ;   // сдвигаем маску  на 1 бит вправо
//                    // *можно поставить и  после  цикла
//                    // но в этом месте этим заодно  обеспечиваетс€ 
//                    // некотор задержка между тактами (тоесть частота клока)

//  
//  output_low(SCK); // завершаем  клок, переводим его в Ђисходноеї состо€ние ( 0 )

// }

return Data;
}
//---------------------------------------------------------------



//-------  «јѕ»—№  ( установка)  регистров W5100 -------------
//--------------------------------------------------------------
void SetW5100register (uint16_t regaddr, uint8_t data)
// 2 аргумента: 16 бит адреса регистра и 8 бит данных дл€ записи

{
output_low(SS); // выставл€ем „ип —елект ( ставим в 0)

SSPI_write (0xF0);  // сперва  посылаем в регистр команду «јѕ»—№

                                   //*  make8 Ц преобразует 16 бит - в  8 бит
SSPI_write (  make8(regaddr,1) );  // выдел€ем из адреса старший байт  (MSB)
                                   // и пишем  его на SPI     


SSPI_write (  make8(regaddr,0) );  // выдел€ем из адреса младший байт  (LSB)
                                   // и шлем  его на SPI  

SSPI_write (data);   // пишем на SPI ƒјЌЌџ≈ дл€ записи в регистр 


output_high(SS);     //  снимаем „ип—елект  ( ставим в 1)

}
//---------------------------------------------------------




//-----  „“≈Ќ»≈ содержимого регистров W5100 ----------
//----------------------------------------------------
int  GetW5100register (uint16_t regaddr)
//аргумент Ц 2-байтовый адрес регистра
// возвращает: 1 байт считанных из регистра данных
{
int RegData;

output_low(SS); // выставл€ем „ип —елект ( ставим в 0)


SSPI_write (0x0F);  // сперва  засылаем команду „“≈Ќ»≈

                                   //*  make8 Ц преобразует 16 бит в  8 бит
SSPI_write (  make8(regaddr,1) );  //  выдел€ем из адреса старший байт  (MSB)
                                   // и выставл€ем его на SPI     

SSPI_write (  make8(regaddr,0) );  //  выдел€ем из адреса младший байт  (LSB)
                                   // и выставл€ем его на SPI  

RegData = SSPI_read ();    // теперь в ответ W5100  выдаст нам содержимое (8 бит) 
                           // регистра по засланному перед этим адресу


output_high(SS);    //  снимаем „ип—елект  ( ставим в 1)

return RegData;	// выходим с  прочитанным байтом

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
