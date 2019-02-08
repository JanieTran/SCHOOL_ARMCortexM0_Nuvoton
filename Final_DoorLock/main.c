#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"

#include "picture.h"
#include "system_func.h"
#include "system_const.h"

//------------------------------------------------------------------------------------------------------------------------------------
// Constants definition
//------------------------------------------------------------------------------------------------------------------------------------
#define WIDTH_MIN 0
#define WIDTH_MAX 127
#define HEIGHT_MIN 8
#define HEIGHT_MAX 63

//------------------------------------------------------------------------------------------------------------------------------------
// Functions declaration
//------------------------------------------------------------------------------------------------------------------------------------
void Print_Key_Holder(void);
void Get_Key(void);
void Input_Key(uint8_t digit, uint8_t value);
void Clear_Digit(uint8_t digit);
uint8_t Is_Correct_Key(void);
void Store_New_Key(void);
void Check_Back(void);

// Interrupt handler
void EINT1_IRQHandler(void);

//------------------------------------------------------------------------------------------------------------------------------------
// Type definition
//------------------------------------------------------------------------------------------------------------------------------------
typedef enum {main_menu, unlock, welcome, wrong_key, change_key, change_key_success} STATES;

//------------------------------------------------------------------------------------------------------------------------------------
// Variables declaration
//------------------------------------------------------------------------------------------------------------------------------------
STATES state, option;
int8_t key;
uint8_t digit;
uint8_t back;
char current_password[6] = {'1','2','3','4','5','6'};
char input[6];

//------------------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------------------

int main(void) {
    //--------------------------------
    //System initialization
    //--------------------------------
    System_Config();
    Timer0_Config();
    KeyPadEnable();

    //--------------------------------
    //SPI3 initialization
    //--------------------------------
    SPI3_Config();

    //--------------------------------
    //SPI3 initialization
    //--------------------------------
    Interrupt_Config();

    //--------------------------------
    //LCD initialization
    //--------------------------------    
    LCD_start();
    LCD_clear();

    //--------------------------------
    //Variables initialisation
    //--------------------------------
    state = main_menu;
    option = main_menu;
    key = 0;
    digit = 0;
    back = 0;

    //--------------------------------
    //LCD dynamic content
    //--------------------------------
    while (1) {
        // Display course name and Assignment name
        printS_5x7(1, 0, "EEET2481_Door Lock System");

        // State machine
        switch (state) {
            case main_menu:
                draw_LCD(LOCK);
                printS_5x7(1, 0, "EEET2481_Door Lock System");
                printS_5x7(31, 48, "PLEASE SELECT");
                printS_5x7(1, 56, "1. Unlock   2. Change key");

                Get_Key();
                
                // Get user selection to proceed
                switch (key) {
                    // Press 1 to Unlock
                    case 1:
                        state = unlock;
                        option = unlock;
                        break;

                    // Press 2 to Change key
                    case 2:
                        state = unlock;
                        option = change_key;
                        break;

                    default: break;
                }

                // Clear the screen and reset Key value
                LCD_clear();
                key = 0;
                break;

            case unlock:
                printS_5x7(11, 16, "Please input your key");
                printS_5x7(36, 32, "_ _ _ _ _ _");

                // Get input password as 6 keys
                while (digit < 6) {
                    Get_Key();

                    // If push button is pressed instead of key matrix, break the loop
                    if (key == -1) break;
                    // Else get number from user
                    if (key != 0) Input_Key(digit, key);

                    // Reset key and move to next digit
                    key = 0;
                    digit ++;
                }

                // Key matrix is pressed
                if (key != -1) {
                    // Correct password
                    if (Is_Correct_Key()) {
                        // If initial option is Unlock, proceed to Welcome screen
                        if (option == unlock)
                            state = welcome;
                        // If initial option is Change key, proceed to Change key screen
                        else if (option == change_key)
                            state = change_key;
                    }
                    // Incorrect password, show Wrong key screen
                    else {
                        state = wrong_key;
                    }
                }
                // Push button is pressed, return to main menu
                else state = main_menu;

                // Clear the screen and reset Key value
                LCD_clear();
                digit = 0;
                break;

            case welcome:
                // Display message for 5s and return to main menu
                printS_5x7(33, 16, "WELCOME HOME");
                Start_Timer(5000);
                state = main_menu;
                LCD_clear();
                break;

            case wrong_key:
                // Display message for 2s and return to main menu
                printS_5x7(21, 16, "THE KEY IS WRONG!");
                printS_5x7(11, 32, "System restarts in 2s");
                printS_5x7(41, 40, "Thank you");
                Start_Timer(2000);
                LCD_clear();
                state = main_menu;
                break;

            case change_key:
                printS_5x7(11, 16, "Please input new key");
                printS_5x7(36, 32, "_ _ _ _ _ _");

                // Get input password as 6 keys
                while (digit < 6) {
                    Get_Key();

                    // If push button is pressed instead of key matrix, break the loop
                    if (key == -1) break;
                    // Else get number from user
                    if (key != 0) Input_Key(digit, key);

                    // Reset key and move to next digit
                    key = 0;
                    digit ++;
                }

                // Key matrix is pressed
                if (key != -1) {
                    // If new key is the same as current key, invalid
                    if (Is_Correct_Key()) {
                        printS_5x7(36, 40, "Invalid key!");
                        Start_Timer(1000);
                    } 
                    // Else, change key successfully, save new password
                    else {
                        state = change_key_success;
                        Store_New_Key();
                    }
                }
                // Push button is pressed, return to main menu
                else state = main_menu;

                // Clear the screen and reset Key value
                LCD_clear();
                digit = 0;
                break;

            case change_key_success:
                // Display message for 2s and return to main menu
                printS_5x7(2, 16, "Key changed successfully!");
                Start_Timer(2000);
                state = main_menu;
                LCD_clear();
                break;

            default: break;
        }
    }       
}

//------------------------------------------------------------------------------------------------------------------------------------
// Interrupt handler
//------------------------------------------------------------------------------------------------------------------------------------
void EINT1_IRQHandler(void) {
    back = 1;
    PB -> ISRC |= 1ul << 15;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Helper methods
//------------------------------------------------------------------------------------------------------------------------------------
void Print_Key_Holder(void) {
    uint8_t i;
    uint8_t x_start = 36;
    for (i = 0; i < 6; i++) {
        printS_5x7(x_start + 5 * i * 2, 32,  "_");
    }
}

void Get_Key(void) {
    while (key == 0) {
        key = KeyPadScanning();
        Check_Back();
    }
    Start_Timer(200);
}

void Input_Key(uint8_t digit, uint8_t value) {
    uint8_t x_start = 36;
    uint8_t digit_x = x_start + 5 * digit * 2;
    char value_char[1];

    // Store input in char array
    sprintf(value_char, "%d", value);
    input[digit] = value_char[0];

    // Display pressed number for 0.2s
    Clear_Digit(digit);
    printS_5x7(digit_x, 32, value_char);
    Start_Timer(200);

    // Number becomes * symbol
    Clear_Digit(digit);
    printS_5x7(digit_x, 32, "*");
}

void Clear_Digit(uint8_t digit) {
    uint8_t x = 36 + 5 * digit * 2;
    uint8_t y = 32;
    fill_Rectangle(x, y, x + 5, y + 7, 0, 0);
}

uint8_t Is_Correct_Key(void) {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        if (input[i] != current_password[i])
            return 0;
    }
    return 1;
}

void Store_New_Key(void) {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        current_password[i] = input[i];
    }
}

void Check_Back(void) {
    if (back) {
        key = -1;
        back = 0;
    }
}
