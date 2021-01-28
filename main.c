/**
 * @file    main.c
 * @version 01.00
 * @brief   Main module.
 *          This module contains user's application code.
 */
/**
 *@addtogroup main_module main module documentation
 *@{
 */
/* MODULE main */


/* Including needed modules to compile this module/procedure */

#if defined USING_OS_FREERTOS
extern void rtos_start(void);
#define PEX_RTOS_START rtos_start
#else
extern void main_impl(void);
#endif 

volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */

/**
 * @brief The main function for the project.
 *
 * @details The startup initialization sequence is the following:
 *  - __start (startup asm routine)
 *  - __init_hardware()
 *  - main()
 *   - PE_low_level_init()
 *    - Common_Init()
 *		- Peripherals_Init()
 */
int main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_INIT
    PEX_INIT();                 /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
#if !defined USING_OS_FREERTOS
    main_impl();
#endif

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
/** @} */ /* END main */
