/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "uartRingBuffer.h"
#include "NMEA.h"
#include "e22900t22d.h"
#include "LORA.h"
#include <stdlib.h>  // Para rand()
/* USER CODE END Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

int transmissionCounter = 0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//char GGA[200];
//char RMC[200];
////char NMEA[100];

//GPSSTRUCT gpsData;
LORA lora;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  Ringbuf_init();
  HAL_Delay(500);


  LORA_Init(&lora);
  LORA_Begin(&lora, &huart2, LORA_M0_GPIO_Port, LORA_M0_Pin,
             LORA_M1_GPIO_Port, LORA_M1_Pin);
  LORA_SetAddress(&lora, 0x000A, 0x000C);
  LORA_SetChannel(&lora, 23);

  printf("\r\n========================================\r\n");
  printf("STM32F401 LoRa + GPS Transmitter\r\n");
  printf("========================================\r\n");

  uint32_t lastTransmissionTime = 0;
  uint32_t lastTransmissionTime_SD = 0;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	    LORA_Manager(&lora);

	    // ===== TRANSMISSÃO A CADA 2 SEGUNDOS =====
	    if ((HAL_GetTick() - lastTransmissionTime) >= 2000) {
	      transmissionCounter++;

	      printf("\r\n========================================\r\n");
	      printf("TRANSMISSAO LoRa #%d\r\n", transmissionCounter);
	      printf("========================================\r\n");


	      typedef struct {
			 float time; //-> timestamp do GPS
			 float lat;
			 float lon;
			 float alt;
			 float veld; //-> taxa de perca de altitude (down velocity)
			 float veln; //-> north velocity
			 float vele; //-> east velocity
			 float yaw;
			 float pitch;
			 float roll;
			 float bar;
			 bool parachute; //-> é um booleano que indica se o paraquedas abriu o não
		 }CompactPayload;


	      // Cria payload com dados simulados (você pode usar dados reais do GPS)
	      CompactPayload payload;

	      srand(HAL_GetTick());

          // --- Preenche a struct com valores aleatórios ---

           // Gera um timestamp aleatório, por exemplo, entre 0 e 10000 segundos
           payload.time = (float)rand() / (float)(RAND_MAX / 10000.0f);

           // Gera uma latitude aleatória entre -90 e 90 graus
           payload.lat = ((float)rand() / (float)RAND_MAX) * 180.0f - 90.0f;

           // Gera uma longitude aleatória entre -180 e 180 graus
           payload.lon = ((float)rand() / (float)RAND_MAX) * 360.0f - 180.0f;

           // Gera uma altitude aleatória, por exemplo, entre 0 e 10000 metros
           payload.alt = ((float)rand() / (float)RAND_MAX) * 10000.0f;

           // Gera uma velocidade de descida aleatória, por exemplo, entre 0 e 100 m/s
           payload.veld = ((float)rand() / (float)RAND_MAX) * 100.0f;

           // Gera uma velocidade norte aleatória, por exemplo, entre -50 e 50 m/s
           payload.veln = ((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f;

           // Gera uma velocidade leste aleatória, por exemplo, entre -50 e 50 m/s
           payload.vele = ((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f;

           // Gera um yaw (guinada) aleatório entre 0 e 360 graus
           payload.yaw = ((float)rand() / (float)RAND_MAX) * 360.0f;

           // Gera um pitch (arfagem) aleatório entre -90 e 90 graus
           payload.pitch = ((float)rand() / (float)RAND_MAX) * 180.0f - 90.0f;

           // Gera um roll (rolagem) aleatório entre -180 e 180 graus
           payload.roll = ((float)rand() / (float)RAND_MAX) * 360.0f - 180.0f;

           // Gera uma leitura de barômetro aleatória, por exemplo, entre 950 e 1050 hPa
           payload.bar = 950.0f + ((float)rand() / (float)RAND_MAX) * 100.0f;

           // Gera um valor booleano (0 ou 1) para o paraquedas
           payload.parachute = (bool)(rand() % 2);

	      // Envia estrutura via LoRa

          LORA_SendStruct(&lora, &payload, sizeof(payload));

         /* EXEMPLO 6: Enviar para destino específico */
         // LORA_SendStringTo(&lora, "Mensagem especial", 0x000D, 15);

         /* EXEMPLO 7: Enviar dados do barômetro */
         // if (bar.is_valid) {
         //     LORA_SendFormatted(&lora, "Pressao: %.2f hPa, Alt: %.2f m",
         //                        bar.pressure_hpa, bar.altitude_m);
         // }

         printf("Timestamp: %lu ms / Tamanho: %d bytes\r\n", HAL_GetTick(), sizeof(payload));
         printf("========================================\r\n");

         HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);
         lastTransmissionTime = HAL_GetTick();
}

//	  if (Wait_for("GGA") == 1)
//	  {
//		Copy_upto("*", GGA);
//
//	    // printf("GGA: %s\r\n", GGA);
//
//		decodeGGA(GGA, &gpsData.ggastruct);
//
//		printf("================================= \r\n");
//		printf("Horário      : %02d:%02d:%02d\r\n", gpsData.ggastruct.tim.hour, gpsData.ggastruct.tim.min, gpsData.ggastruct.tim.sec);
//		printf("Latitude     : %f %c\r\n", gpsData.ggastruct.lcation.latitude, gpsData.ggastruct.lcation.NS);
//		printf("Longitude    : %f %c\r\n", gpsData.ggastruct.lcation.longitude, gpsData.ggastruct.lcation.EW);
//		printf("Altitude     : %.2f m\r\n", gpsData.ggastruct.alt.altitude);
//
//		if (gpsData.ggastruct.isfixValid)
//		{
//			printf("Fix GPS      : Válido\r\n");
//		}
//		else
//		{
//			printf("Fix GPS      : Inválido / Procurando...\r\n");
//		}
//		printf("Satélites    : %d\r\n", gpsData.ggastruct.numofsat);
//
//	  }
//
//	  if (Wait_for("RMC") == 1)
//	  {
//		Copy_upto("*", RMC);
//
//		// printf("RMC: %s\r\n", RMC);
//
//		decodeRMC(RMC, &gpsData.rmcstruct);
//
//		printf("Data         : %02d/%02d/20%02d\r\n", gpsData.rmcstruct.date.Day, gpsData.rmcstruct.date.Mon, gpsData.rmcstruct.date.Yr);
//		printf("Velocidade   : %.2f m/s\r\n\r\n", gpsData.rmcstruct.speed);
//		printf("================================= \r\n\r\n");
//	  }
//  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();  // Se necessário

	  /* USER CODE BEGIN MX_GPIO_Init_2 */

	  // Configure pinos M0 e M1 do LoRa (ajuste conforme seu hardware)
	  // Exemplo: PA4 para M0, PA5 para M1
	  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_6;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Retargets the C library printf function to the USART.
  *   None
  * @retval None
  */
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART1 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
//
//  return ch;
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
