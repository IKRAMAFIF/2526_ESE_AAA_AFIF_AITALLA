/**
* @file shell.c
* @brief Implementation file for the shell module.
*
* This file contains the implementation of the shell module, which provides a command-line interface.
*/

#include "user_interface/shell.h"
#include <stdlib.h>
#include "motor_control/motor.h"
#include "acquisition/input_analog.h"
#include "acquisition/input_encoder.h"

h_shell_t hshell1;
/**
* @brief Transmit function for shell driver using UART2
*/



/**
* @brief Checks if a character is valid for a shell command.
*
* This function checks if a character is valid for a shell command. Valid characters include alphanumeric characters and spaces.
*
* @param c The character to check.
* @return 1 if the character is valid, 0 otherwise.
*/

static int is_character_valid(char c)
{
return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') ||
		(c == ' ') || (c == '=');
}

static int is_string_valid(char* str)
{
int reading_head = 0;
while(str[reading_head] != '\0'){
if(!is_character_valid(str[reading_head])){
if(reading_head == 0){
return 0;
}
else{
str[reading_head] = '\0';
return 1;
}
}
reading_head++;
}
return 1;
}

/**
* @brief Help command implementation.
*
* This function is the implementation of the help command. It displays the list of available shell commands and their descriptions.
*
* @param h_shell The pointer to the shell instance.
* @param argc The number of command arguments.
* @param argv The array of command arguments.
* @return 0 on success.
*/
static int sh_help(h_shell_t* h_shell, int argc, char** argv)
{
int i, size;
size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Code \t | Description \r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "----------------------\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);

for (i = 0; i < h_shell->func_list_size; i++){
size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "%s \t | %s\r\n", h_shell->func_list[i].string_func_code, h_shell->func_list[i].description);
h_shell->drv.transmit(h_shell->print_buffer, size);
}
return 0;
}
/**
 * @brief Commande pour lire le courant via ADC en Polling
 * Usage: current
 */
static int sh_current_read(h_shell_t* h_shell, int argc, char** argv)
{
    float courant = CurrentSensor_Read();

    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                        "Courant mesure = %f A\r\n", courant);

    h_shell->drv.transmit(h_shell->print_buffer, size);

    return 0;
}
/**
* @brief Commande pour démarrer le moteur
* Usage: start
*/
static int sh_motor_start(h_shell_t* h_shell, int argc, char** argv)
{
Motor_Start();

float initial_alpha = 0.5f;
Motor_SetSpeed(initial_alpha);

//  utilisateur
int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Moteur demarre (PWM ON) a alpha = %.2f (50%%)\r\n", initial_alpha);
h_shell->drv.transmit(h_shell->print_buffer, size);
return 0;
}


static int sh_motor_speed(h_shell_t* h_shell, int argc, char** argv)
{
if (argc < 2)
{
int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Erreur: Usage = speed <0 a 100>\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
return -1;
}



// Conversion de la chaine de caractères en float
float percent_speed = (float)atof(argv[1]);

// Validation de la plage de pourcentage
if (percent_speed < 0.0f || percent_speed > 100.0f)
{
int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Erreur: Le pourcentage doit etre entre 0 et 100.\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
return -1;
}

 // Conversion du pourcentage en alpha
float alpha = percent_speed / 100.0f;


// Application de la vitesse
Motor_SetSpeed(alpha);





// Retour utilisateur
int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Vitesse definie a %s%% (alpha = %f)\r\n", argv[1], alpha);
h_shell->drv.transmit(h_shell->print_buffer, size);

return 0;
}
static int sh_motor_stop(h_shell_t* h_shell, int argc, char** argv)
{
Motor_Stop();
int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Moteur arrete (PWM OFF)\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
return 0;
}

static int sh_test_list(h_shell_t* h_shell, int argc, char** argv)
{
int size;
for(int arg=0; arg<argc; arg++){
size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Arg %d \t %s\r\n", arg, argv[arg]);
h_shell->drv.transmit(h_shell->print_buffer, size);
}
return 0;
}




static int sh_speed_read(h_shell_t* h_shell, int argc, char** argv)
{
    float speed_rps = encoder_speed_rps;
    float speed_abs;
    char direction;
    char unit_str[10];

    // Détermination de la direction
    if (speed_rps >= 0) {
        direction = '+';
        speed_abs = speed_rps;
    } else {
        direction = '-';
        speed_abs = fabsf(speed_rps);
    }

    float display_value;

    // Détermination du format d'affichage
    if (argc >= 2 && strcmp(argv[1], "rps") == 0)
    {
        display_value = speed_abs;
        snprintf(unit_str, 10, "RPS");
    }
    else if (argc >= 2 && strcmp(argv[1], "tps") == 0)
    {
        display_value = speed_abs * (float)ENCODER_TICKS_PER_REVOLUTION;
        snprintf(unit_str, 10, "Ticks/s");
    }
    else
    {
        display_value = speed_abs * 60.0f;
        snprintf(unit_str, 10, "RPM");
    }

    // Affichage
    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                        "Vitesse mesuree = %c%.2f %s\r\n",
                        direction, display_value, unit_str);

    h_shell->drv.transmit(h_shell->print_buffer, size);

    return 0;
}







/**
* @brief Commande pour lancer une rampe de vitesse.
* Usage: ramp <pourcentage>
* ATTENTION: Cette fonction est BLOQUANTE et monopolise le shell pendant la rampe.
*/


/**
* @brief Initializes the shell instance.
*
* This function initializes the shell instance by setting up the internal data structures and registering the help command.
*
* @param h_shell The pointer to the shell instance.
*/
void shell_init(h_shell_t* h_shell)
{
int size = 0;

h_shell->func_list_size = 0;

size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "\r\n=> Shell v0.2.2  <=\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
h_shell->drv.transmit(PROMPT, sizeof(PROMPT));

shell_add(h_shell, "help", sh_help, "Help");
shell_add(h_shell, "test", sh_test_list, "Test list");
shell_add(h_shell, "start", sh_motor_start, "Demarre le moteur (PWM)");
shell_add(h_shell, "stop", sh_motor_stop, "Arrete le moteur (Roue libre)");
shell_add(h_shell, "speed", sh_motor_speed, "Set vitesse en pourcent. Ex: speed 75");
shell_add(h_shell, "current", sh_current_read, "Mesure du courant (ADC Polling)");
shell_add(h_shell, "speedread", sh_speed_read, "Lit vitesse encodeur (RPM/RPS/Ticks)");
}

/**
* @brief Adds a shell command to the instance.
*
* This function adds a shell command to the shell instance.
*
* @param h_shell The pointer to the shell instance.
* @param c The character trigger for the command.
* @param pfunc Pointer to the function implementing the command.
* @param description The description of the command.
* @return 0 on success, or a negative error code on failure.
*/
int shell_add(h_shell_t* h_shell, char* string_func_code, shell_func_pointer_t pfunc, char* description)
{
if(is_string_valid(string_func_code))
{
if (h_shell->func_list_size < SHELL_FUNC_LIST_MAX_SIZE)
{
h_shell->func_list[h_shell->func_list_size].string_func_code = string_func_code;
h_shell->func_list[h_shell->func_list_size].func = pfunc;
h_shell->func_list[h_shell->func_list_size].description = description;
h_shell->func_list_size++;
return 0;
}
}
return -1;
}

/**
* @brief Executes a shell command.
*
* This function executes a shell command based on the input buffer.
*
* @param h_shell The pointer to the shell instance.
* @param buf The input buffer containing the command.
* @return 0 on success, or a negative error code on failure.
*/
static int shell_exec(h_shell_t* h_shell, char* buf)
{
int i, argc;
char* argv[SHELL_ARGC_MAX];
char* p;


argc = 1;
argv[0] = buf;
for (p = buf; *p != '\0' && argc < SHELL_ARGC_MAX; p++)
{
if (*p == ' ')
{
*p = '\0';
argv[argc++] = p + 1;
}
}

for (i = 0; i < h_shell->func_list_size; i++)
{
if(strcmp(h_shell->func_list[i].string_func_code, argv[0])==0)
{

return h_shell->func_list[i].func(h_shell, argc, argv);
}
}

int size;
size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "%s : no such command\r\n", argv[0]);
h_shell->drv.transmit(h_shell->print_buffer, size);
return -1;
}

/**
* @brief Runs the shell.
*
* This function runs the shell, processing user commands.
*
* @param h_shell The pointer to the shell instance.
* @return Never returns, it's an infinite loop.
*/
int shell_run(h_shell_t* h_shell)
{
static int cmd_buffer_index;
char c;
int size;

h_shell->drv.receive(&c, 1);

switch(c)
{
case '\r':

size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "\r\n");
h_shell->drv.transmit(h_shell->print_buffer, size);
h_shell->cmd_buffer[cmd_buffer_index++] = 0;
cmd_buffer_index = 0;
shell_exec(h_shell, h_shell->cmd_buffer);
h_shell->drv.transmit(PROMPT, sizeof(PROMPT));
break;

case '\b':
if (cmd_buffer_index > 0)
{
h_shell->cmd_buffer[cmd_buffer_index] = '\0';
cmd_buffer_index--;
h_shell->drv.transmit("\b \b", 3);
}
break;

default:

if (cmd_buffer_index < SHELL_CMD_BUFFER_SIZE)
{
if (is_character_valid(c))
{
h_shell->drv.transmit(&c, 1);
h_shell->cmd_buffer[cmd_buffer_index++] = c;
}
}
}
return 0;
}
