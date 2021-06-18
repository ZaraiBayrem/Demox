/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx_container.c
  * @author  Bayrem ZARAI
  * @brief   ThreadX applicative with container file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 Actia Engineering Services. 
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx_container.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD MainThread;
TX_THREAD ThreadOne;
TX_CONTAINER ContainerOne;
TX_GROUP GroupOne;
TX_EVENT_FLAGS_GROUP EventFlag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void ThreadOne_Entry(ULONG thread_input);
void ThreadTwo_Entry(ULONG thread_input);
void ThreadThree_Entry(ULONG thread_input);
void MainThread_Entry(ULONG thread_input);
void App_Delay(uint32_t Delay);

/* USER CODE END PFP */
/**
  * @brief  Application ThreadX with Container Initialization.
  * @param  memory_ptr: memory pointer
  * @retval int
  */

UINT App_ThreadX_Container_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_Init */
  CHAR *pointer;
  
  /* Allocate the stack for first container.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }
  
  /* Create container.  */
  if (tx_container_create(&ContainerOne, Container One, 0,  CONTAINER_STACK_SIZE, 
                         &GroupOne, TX_AUTO_START) != TX_SUCCESS)                 
  {
    ret = TX_THREAD_ERROR;
  }

  /* Allocate the stack for MainThread.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }
  
  /* Create MainThread.  */
  if (tx_thread_create(&MainThread, "Main Thread", MainThread_Entry, 0,  
                       pointer, APP_STACK_SIZE, 
                       MAIN_THREAD_PRIO, MAIN_THREAD_PREEMPTION_THRESHOLD,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Allocate the stack for MainThread.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }
  
  /* Create MainThread.  */
  if (tx_thread_create(&ThreeThread, "Three Thread", ThreeThread_Entry, 0,  
                       pointer, APP_STACK_SIZE, 
                       THREE_THREAD_PRIO, THREE_THREAD_PREEMPTION_THRESHOLD,
                       TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  
  /* Allocate the stack for ThreadOne.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }
  
  /* Create ThreadOne.  */
  if (tx_thread_create(&ThreadOne, "Thread One", ThreadOne_Entry, 0,  
                       pointer, APP_STACK_SIZE, 
                       THREAD_ONE_PRIO, THREAD_ONE_PREEMPTION_THRESHOLD,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Allocate the stack for ThreadTwo.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }
  
  /* Create ThreadTwo.  */
  if (tx_thread_create(&ThreadTwo, "Thread Two", ThreadTwo_Entry, 0,  
                       pointer, APP_STACK_SIZE, 
                       THREAD_TWO_PRIO, THREAD_TWO_PREEMPTION_THRESHOLD,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }
  
  /* Create the event flags group.  */
  if (tx_event_flags_create(&EventFlag, "Event Flag") != TX_SUCCESS)
  {
    ret = TX_GROUP_ERROR;
  }
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing the MainThread thread.
  * @param  thread_input: Not used 
  * @retval None
  * @ps     Main thread is considread as Container Manager
  */
void MainThread_Entry(ULONG thread_input)
{
  UINT old_prio = 0;
  UINT old_pre_threshold = 0;
  UINT status;
  ULONG   actual_flags = 0;
  uint8_t count = 0; 
  (void) thread_input;

  /* Add thread one to the Container one.   */
  if (tx_container_addthread(&ContainerOne,&ThreadOne)!=TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Add thread two to the Container one.   */
  if (tx_container_addthread(&ContainerOne,&ThreadTwo)!=TX_SUCCESS)
  {
    Error_Handler();
  }
  
  /* Resume container one because it was in the suspended state.   */
  if (tx_container_resume(&ContainerOne)!=TX_SUCCESS)
  {
    Error_Handler();
  }
  
  /* Get the status after resuming thread one and thread two.   */ 
  status = tx_thread_resume(&fx_thread_one) & tx_thread_resume(&fx_thread_two);

  if (status == TX_SUCCESS)
  {
    while (count < 3)
  {
    count++;
    if (tx_event_flags_get(&EventFlag, THREAD_ONE_EVT, TX_OR_CLEAR, 
                           &actual_flags, TX_WAIT_FOREVER) != TX_SUCCESS)
    {
      Error_Handler();
    }
    else
    {
      /* Update the priority and preemption threshold of ThreadTwo 
      to allow the preemption of ThreadOne */
      tx_thread_priority_change(&ThreadTwo, NEW_THREAD_TWO_PRIO, &old_prio);
      tx_thread_preemption_change(&ThreadTwo, NEW_THREAD_TWO_PREEMPTION_THRESHOLD, &old_pre_threshold);
      
      if (tx_event_flags_get(&EventFlag, THREAD_TWO_EVT, TX_OR_CLEAR, 
                             &actual_flags, TX_WAIT_FOREVER) != TX_SUCCESS)
      {
        Error_Handler();
      }
      else
      {
        /* Reset the priority and preemption threshold of ThreadTwo */ 
        tx_thread_priority_change(&ThreadTwo, THREAD_TWO_PRIO, &old_prio);
        tx_thread_preemption_change(&ThreadTwo, THREAD_TWO_PREEMPTION_THRESHOLD, &old_pre_threshold);
      }
    }
  }
  }
  /* Destroy ThreadOne and ThreadTwo */
  tx_thread_terminate(&ThreadOne);
  tx_thread_terminate(&ThreadTwo);

  /* Destroy ContainerOne */
  if( tx_container_terminate(&ContainerOne)!=TX_SUCCESS)
  {
    TX_TERMINATE_ERROR;
  }
  else
  {
    /* when container one is terminated then toggle yellow led for 10s.  */
    BSP_LED_Toggle(LED_YELLOW);
    /* Thread sleep for 1s */
    tx_thread_sleep(1000);
  }
   
  /* Infinite loop */
  while(1)
  {
    BSP_LED_Toggle(LED_GREEN);
    /* Thread sleep for 1s */
    tx_thread_sleep(100);
    BSP_LED_Toggle(LED_YELLOW);
    /* Thread sleep for 1s */
    tx_thread_sleep(100);
  }
}

/**
  * @brief  Function implementing the ThreadOne thread.
  * @param  thread_input: Not used 
  * @retval None
  */
void ThreadOne_Entry(ULONG thread_input)
{
  (void) thread_input;
  uint8_t count = 0;
  /* Infinite loop */
  while(1)
  {
    BSP_LED_Toggle(LED_GREEN);
    /* Delay for 500ms (App_Delay is used to avoid context change). */
    App_Delay(50);
    count ++;
    if (count == 10)
    {
      count = 0;
      if (tx_event_flags_set(&EventFlag, THREAD_ONE_EVT, TX_OR) != TX_SUCCESS)
      {
        Error_Handler();
      }
    }
  }
}

/**
  * @brief  Function implementing the ThreadTwo thread.
  * @param  thread_input: Not used 
  * @retval None
  */
void ThreadTwo_Entry(ULONG thread_input)
{
  (void) thread_input;
  uint8_t count = 0;
  /* Infinite loop */
  while (1)
  {
    BSP_LED_Toggle(LED_GREEN);
    /* Delay for 200ms (App_Delay is used to avoid context change). */
    App_Delay(20);
    count ++;
    if (count == 25)
    {
      count = 0;
      if (tx_event_flags_set(&EventFlag, THREAD_TWO_EVT, TX_OR) != TX_SUCCESS)
      {
        Error_Handler();
      }
    }
  }
}

/**
  * @brief  Application Delay function.
  * @param  Delay : number of ticks to wait
  * @retval None
  */
void App_Delay(uint32_t Delay)
{
  UINT initial_time = tx_time_get();
  while ((tx_time_get() - initial_time) < Delay);
}
/* USER CODE END 1 */