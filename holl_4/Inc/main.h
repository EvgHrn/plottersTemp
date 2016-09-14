#include "stm32f1xx_hal.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "stdio.h"

// ���������� ���� PICa  ��� ����������� � Ethernet  Shield

#define RESET   GPIO_PIN_4       // *active LOW
#define SS      GPIO_PIN_3       // output.  SlaveSelect,  Active LOW

//********** ����������� ��������   W5100  *********************
//**************************************************************

#define MR    0x0000  // MODE register

#define GAR0  0x0001  // GATEWAY Addr register0   (MSB)
#define GAR1  0x0002  // GATEWAY Addr register1
#define GAR2  0x0003  // GATEWAY Addr register2
#define GAR3  0x0004  // GATEWAY Addr register3   (LSB)

#define SUBR0 0x0005  // SUBNET MASK Addr register0  (MSB)
#define SUBR1 0x0006  // SUBNET MASK Addr register1
#define SUBR2 0x0007  // SUBNET MASK Addr register2
#define SUBR3 0x0008  // SUBNET MASK Addr register3   (LSB)

// MAC ����� �������
#define SHAR0 0x0009  // SOURCE HARDWARE  Addr register0 (MSB)
#define SHAR1 0x000A  // SOURCE HARDWARE  Addr register1
#define SHAR2 0x000B  // SOURCE HARDWARE  Addr register2
#define SHAR3 0x000C  // SOURCE HARDWARE  Addr register3
#define SHAR4 0x000D  // SOURCE HARDWARE  Addr register4
#define SHAR5 0x000E  // SOURCE HARDWARE  Addr register5  (LSB)

#define SIPR0 0x000F  // Source IP  Addr register0  (MSB)
#define SIPR1 0x0010  // Source IP  Addr register1
#define SIPR2 0x0011  // Source IP  Addr register2
#define SIPR3 0x0012  // Source IP  Addr register3  (LSB)

#define RMSR  0x001A  // RX memory size  (1K,2K,4K or 8K per socket, from total 8K)
#define TMSR  0x001B  // TX memory size  (1K,2K,4K or 8K per socket, from total 8K)

//-- ��������  Socket0 (* � ������� ������������ ������ ���� ����� )

#define S0_MR   0x0400   // Socket0 MODE register
#define S0_CR   0x0401   // Socket0 COMMAND register
 
#define S0_SR   0x0403   // Socket0 STATUS register

#define S0_PORT0   0x0404   // Socket0 SOURCE Port register0 (H byte)
#define S0_PORT1   0x0405   // Socket0 SOURCE Port register1 (L byte)

#define S0_TX_FSR0    0x0420   // Socket0 TX Free SIZE register0
#define S0_TX_FSR1    0x0421   // Socket0 TX Free SIZE register1

#define S0_TX_RD0    0x0422   // Socket0 TX Read POINTER register0
#define S0_TX_RD1    0x0423   // Socket0 TX Read POINTER register1

#define S0_TX_WR0    0x0424   // Socket0 TX Write POINTER register0
#define S0_TX_WR1    0x0425   // Socket0 TX Write POINTER register1

#define S0_RX_RSR0  0x0426   // Socket0 RX Received SIZE register0 (H byte)
#define S0_RX_RSR1  0x0427   // Socket0 RX Received SIZE register1 ( L byte)

#define S0_RX_RD0  0x0428   // Socket0 RX Read POINTER0  (H byte)
#define S0_RX_RD1  0x0429   // Socket0 RX Read POINTER1  (L byte)

// ----- ���� ������ ( ������������ �  �������� ������ ������0 ) -----

#define OPEN     0x01
#define LISTEN   0x02
#define CONNECT  0x04
#define DISCON   0x08
#define CLOSE    0x10
#define SEND     0x20
#define SEND_MAC   0x21
#define SEND_KEEP   0x02
#define RECV     0x40

// ----- ���� ��������� ( ������������ � �������� STATUS  ������0 ) ---
#define SOCK_CLOSED     0x00
#define SOCK_INIT       0x13
#define SOCK_LISTEN     0x14
#define SOCK_ESTABLISHED   0x17
#define SOCK_CLOSE_WAIT    0x1C

// ---------- ���������� ����� ����������  -----------
#define SERVER_IP0   192   //  ��� ������ �����  192.168.0.155
#define SERVER_IP1   168
#define SERVER_IP2    0    
#define SERVER_IP3   155    

#define SERVER_PORT0   0   //  ��� ���� �����       :80
#define SERVER_PORT1   80
 
#define GATEWAY_IP0   192  // ������� �����. ���� � ��� ������ - ��������
#define GATEWAY_IP1   168
#define GATEWAY_IP2   0
#define GATEWAY_IP3   254

#define SUBNET_MASK0   255  // ����� �������  ( �������)
#define SUBNET_MASK1   255
#define SUBNET_MASK2   255
#define SUBNET_MASK3   0

#define MAC0   0x00  // ��� ����� �����, �������, ����� � ����� ����
#define MAC1   0x1A  // �� ���� ���������  � ����� ��  ���
#define MAC2   0x2B
#define MAC3   0x3C
#define MAC4   0x4D
#define MAC5   0x5E

//----------- ��������� ����������� �������  ---------------

void SSPI_write(int Data);      // ������  1 ����� � W5100   ����� SPI
uint8_t SSPI_read ( void);         // ������ 1 ����� �� W5100 ����� SPI 
void SetW5100register (uint16_t regaddr, uint8_t data);  
																// ���������(������) ���������� W5100  ����� SPI
int  GetW5100register (uint16_t regaddr);               
																// ������ ����� �� ��������� W5100 ����� SPI
void Init(void);          // ������������� W5100  � �������
int Open_Socket0(void);    // ��������� ����� � ���������� ������ (������/��������)
int Listen_Socket0(void);   // ������� ����� � ���������� ������ (������/��������)
int Socket0_Connection_Established (void);   
															// �������� ��� ���������� ����������� � ���������� ������  (��/���)
int Socket0_Received_Data_Size (void);       
																//  ���������� ������ ����� ������, �������� �� ������� ( 0 ���� ������ ���) 
void Socket0_Received_Data_Reading (void);
															// ������� �� �������� �������� ������
															// ����� ������� �-��� ����� ���� �������
int  Socket0_FIN_Received(void);      
																	// ���������  ������ �� �� ������� ���� FIN  � ���������� ��/���
void Socket0_Disconnect(void);  
																		// ����� � ������� W5100  ������� �� ������ ����������
int  Socket0_Closed(void);    // �������� ��� ����� ������ � ���������� ��/���          
int  Socket0_Connection_Timeout(void);  
																// ��������� �� ��������� �� ����-��� ����������  � ���������� ��/���
void Socket0_Closing(void);           // ��������� �����
int  Socket0_Received_Request_is_index_html(void);  
																	// ��������� ���  � ������ �������� �� ������� ������������� ���� Index.html
																	//  � ���������� ��/���
void Socket0_Send_index_html (void);  // �������� �������  ����������  index.html
void Socket0_Send_404_error (void); // �������� �������  ����������  ������� 404�
void uprintf (char * str);

void output_high (uint16_t pin);
void output_low (uint16_t pin);

uint8_t make8(uint16_t n, uint8_t high);
uint16_t make16(uint8_t high, uint8_t low);

uint8_t is_holl_detected(void);