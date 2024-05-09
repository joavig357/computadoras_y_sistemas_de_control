// Inclusion de librerias
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "lcd_i2c.h"
#include <math.h>

// GPIO del ADC
#define ANALOG_GPIO
// Canal del ADC
#define ANALOG_C

// Tiempo de refresco para el siete segmentos
#define SLEEP_MS 10
// Tiempo entre conversiones
#define ADC_DELAY_MS  500

// Constante de proporcionalidad para el NTC
#define BETA  3950
// Resistencia a 25 grados del NTC
#define RT0   10000
// Resistencia en serie al NTC
#define RS    10000

// Variable para almacenar el resultado del ADC
uint16_t adc_value = 0;
// Variable para guardar el valor de temperatura
float temperatura = 0.0;

/*
 
@brief Callback para la interrupcion de timer
@param t: puntero a repeating_timer
*/
bool muestreo_periodico(struct repeating_timer t) {
  // Lectura analogica (variable adc_value)
  uint16_t adc_value = adc_read();
  // Calcular valor de temperatura (variable temperatura)
  temperatura = 1 / (log(1 / (4095. / adc_value - 1)) / BETA + 1.0 / 298.15) - 273.15;
}

/*
 *
 *@brief Display temperature value in 7 segment display
 *@param temperature: value to display
*/
void display_temp(float temperatura) {
  // Variable para armar el string
  char str[16];
  // Armo string con temperatura
  sprintf(str, "Temp=%.2f C", temperatura);
  // Limpio display (si lo hubiese)
  lcd_clear();
  // Muestro (printf o lcd_string)
  lcd_string(str);
}

/*
 *
 *@brief Programa principal
*/
int main() {
  // Inicializacion de UART
  stdio_init_all();
  // Creo un repeating timer
  struct repeating_timer timer;
  // Creo un callback para la interrupcion del timer
  add_repeating_timer_ms(ADC_DELAY_MS, muestreo_periodico, NULL, &timer);
  // Inicializo ADC
  adc_init();
    // Inicializo GPIO27 como entrada analogica
  adc_gpio_init(27);
  // Selecciono canal analogico
  adc_select_input(1);
// Solo si se usa LCD con I2C
    // Configuro el I2C0 a 100 KHz de clock
  i2c_init(i2c0, 100 * 1000);
    // Elijo GPIO4 como linea de SDA
  gpio_set_function(4, GPIO_FUNC_I2C);
    // Elijo GPIO5 como linea de SCL
  gpio_set_function(5, GPIO_FUNC_I2C);
    // Activo pull-up en ambos GPIO, son debiles por lo que
    // es recomendable usar pull-ups externas
    gpio_pull_up(4);
    gpio_pull_up(5);
    // Inicializo display (si lo hubiese)
  lcd_init();

    // Bucle infinito
      while (true) {
        // Muestro temperatura
        display_temp(temperatura);
        // Espero
        sleep_ms(SLEEP_MS);
      }
}
