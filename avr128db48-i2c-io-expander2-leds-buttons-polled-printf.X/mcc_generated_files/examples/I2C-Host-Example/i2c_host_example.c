/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
/**
 * I2C_HOST EXAMPLE Generated Driver File
 * 
 * @file i2c_host example.c
 * 
 * @ingroup i2c_host example
 * 
 * @version I2C_HOST EXAMPLE Example Version 1.0.0
 *
 * @brief Generated file for
 *        Example:           3. I2C IO Expander 2 - LEDs and Buttons 
 *        Implementation:    Polled 
 *        Visualization:     Printf   
 *        MCU Device family: AVR
*/
/* ------------------------------------------------------------------

#include "mcc_generated_files/system/system.h"

// Note:  MCP23008 - 8-Bit I2C I/O Expander with I2C (& SPI) Serial Interface(s) 
//        Reference to the MCP23008 data sheet:  https://www.microchip.com/DS20001919 
//        MCP23008 register addresses are in Table 1-2 (page 5) of the data sheet
//        The Curiosity Nano Explorer (EV58G97A) has 2 x MCP23008 IO Expanders

#define MCP23008_1_I2C_ADDRESS 0x25
#define MCP23008_2_I2C_ADDRESS 0x24

#define MCP23008_IODIR      0x00
#define MCP23008_IPOL       0x01 
#define MCP23008_GPINTEN    0x02  // INTERRUPT-ON-CHANGE CONTROL (GPINTEN) REGISTER,  GPINTEN â?? enables the individual inputs
#define MCP23008_DEFVAL     0x03  // DEFAULT COMPARE (DEFVAL) â?? holds the values that are compared against the associated input port values
#define MCP23008_INTCON     0x04  // INTERRUPT CONTROL (INTCON) - Controls if the input values are compared against DEFVAL or the previous values on the portREGISTER: 
#define MCP23008_IOCON      0x05  // IOCON (ODR and INPOL) â?? configures the INT pin as push-pull, open-drain and active-level
#define MCP23008_GPPU       0x06  
#define MCP23008_INTF       0x07  // INTERRUPT FLAG (INTF) REGISTER: 1 = Pin caused interrupt
#define MCP23008_INTCAP     0X08
#define MCP23008_GPIO       0x09
#define MCP23008_OLAT       0x0A

extern void DELAY_milliseconds(uint16_t milliseconds);  
uint8_t MCP23008_Read(uint8_t i2c_address, uint8_t reg, uint8_t* data);
uint8_t MCP23008_Write(uint8_t i2c_address, uint8_t reg, uint8_t data);
void printButtonPressed(int pressedButton);
void button_led_control(void);

static volatile bool resetFlag = false; 
// All joystick pins re-mapped as they are not connected to Curiosity Nano GPIO by default
static const char* buttonLabels[8] = {"J-UP", "J-LEFT", "J-DOWN", "J-RIGHT", "J-PUSH", "SW3", "SW2", "SW1"};

uint8_t MCP23008_Read(uint8_t i2c_address, uint8_t reg, uint8_t* data)
{
    i2c_host_error_t errorState = I2C_ERROR_NONE;
    size_t txLength = 1;
    size_t rxLength = 1;
    uint8_t txBuffer[1] = {0};

    txBuffer[0] = reg;  // I2C Client register to read from

    uint8_t waitCounter = 100;
    if(I2C_Host.WriteRead((uint16_t)(i2c_address), txBuffer, txLength, data, rxLength))
    {   
        while (I2C_Host.IsBusy() && (waitCounter > 0U)) 
        {
            I2C_Host.Tasks();
            waitCounter--; 
        }
    }

    errorState = I2C_Host.ErrorGet();
    return errorState;
}


uint8_t MCP23008_Write(uint8_t i2c_address, uint8_t reg, uint8_t data)
{
    i2c_host_error_t errorState = I2C_ERROR_NONE;
    size_t txLength = 2;
    uint8_t txBuffer[2] = {0};

    txBuffer[0] = reg;  // I2C Client register to write to
    txBuffer[1] = data;

    uint8_t waitCounter = 100;
    if(I2C_Host.Write((uint16_t)i2c_address, txBuffer, txLength))
    {   
        while (I2C_Host.IsBusy() && (waitCounter > 0U)) 
        {
            I2C_Host.Tasks();
            waitCounter--; 
        }
    }
    
    errorState = I2C_Host.ErrorGet();
    return errorState;
}

void printButtonPressed(int pressedButton) {
    if (pressedButton >= 0 && pressedButton < 8) {
        (int) printf("Button pressed: %s\n", buttonLabels[pressedButton]);
    } else {
        (int) printf("Invalid button pressed\n");
    }
}

void button_led_control(void)
{      
    uint8_t mcp23008_1_activeLEDs = 0;
    uint8_t mcp23008_2_previousInputs = 0;
    uint8_t mcp23008_2_currentInputs = 0;

    (uint8_t) MCP23008_Write(MCP23008_1_I2C_ADDRESS, MCP23008_IODIR, 0x00);
    (uint8_t) MCP23008_Write(MCP23008_1_I2C_ADDRESS, MCP23008_GPIO, 0x00);    // After this line of code, the LEDs should be on. 
   
    (uint8_t) MCP23008_Write(MCP23008_2_I2C_ADDRESS, MCP23008_IODIR, 0xFF);   // Set IO-expander 2 pins as inputs
    (uint8_t) MCP23008_Write(MCP23008_2_I2C_ADDRESS, MCP23008_GPPU, 0xFF);    // Set pull-up for all pins of IO-expander 2   
    (uint8_t) MCP23008_Write(MCP23008_2_I2C_ADDRESS, MCP23008_IOCON, 0x20);   // Set SEQOP bit to 1 for Byte mode

    // Read the default state of the inputs
    (uint8_t) MCP23008_Read(MCP23008_2_I2C_ADDRESS, MCP23008_GPIO, &mcp23008_2_previousInputs);
    (int) printf("Default inputs: 0x%02X\n", mcp23008_2_previousInputs);

    while ((IO_Reset_GetValue() != 0) && !resetFlag)         // Run until Reset SW is pressed
    {
        (uint8_t) MCP23008_Read(MCP23008_2_I2C_ADDRESS, MCP23008_GPIO, &mcp23008_2_currentInputs); // Read GPIO register to get the state of the inputs
        
        uint8_t changed_inputs = mcp23008_2_previousInputs ^ mcp23008_2_currentInputs; // XOR to find changed bits
        if (!(changed_inputs == 0U)) // Check if there is any change from the default state
        {
            for (uint8_t i = 0U; i < 8U; i++) // Check each bit to see if a button is pressed
            {
                if (!(changed_inputs == 0U)) // Check if there is any change from the default state
                {
                    if ((mcp23008_2_currentInputs & (1U << i)) == 0U) // Button pressed (active low)
                    {
                        uint8_t pressedButton = i;
                        mcp23008_1_activeLEDs = (mcp23008_1_activeLEDs | (1U << pressedButton));
                        (uint8_t) MCP23008_Write(MCP23008_1_I2C_ADDRESS, MCP23008_GPIO, mcp23008_1_activeLEDs); // Set IO-expander 1 pins low
                        printButtonPressed(pressedButton); 
                    }
                }
            }
        }
        DELAY_milliseconds(100); // Add a small delay to avoid bouncing issues, presses longer than 100ms will register > once
    }
    resetFlag = true;
}

int main(void)
{
    DELAY_milliseconds(200); // Prevent program running when programming
    SYSTEM_Initialize();    
    (int) printf("Example: 3. I2C IO Expander 2 - LEDs and Buttons, Implementation: Polled, Visualization: Printf \r\n");
    (int) printf("MCU Device family: AVR \r\n");
    (int) printf("Press Curiosity Nano Explorer touch buttons and use Joystick to turn off all LEDS. Press SW0 on Curiosity Nano to exit. \r\n");
    button_led_control();     
   
    while(1)
    {
        IO_LED_SetLow();        //Turn off LED during button_led_control() 
        button_led_control();  
        
        if (resetFlag)         // Check if the reset flag is set
        {
            (int) printf("Reset flag set, restarting button_led_control() \r\n\r\n");
            resetFlag = false; // Clear the reset flag
        }
    }
    return 0;                   // Shouldn't get here!!
}

------------------------------------------------------------------ */
