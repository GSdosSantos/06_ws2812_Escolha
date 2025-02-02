//#include <stdio.h>
//#include <stdlib.h>
#include "pico/stdlib.h"
//#include "hardware/pio.h"
//#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "numeros.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define TEMPO 1500

#define botaoA 5
#define botaoB 6
uint8_t led_r = 255; // Vermelho
uint8_t led_g = 0; // Verde
uint8_t led_b = 0; // Azul
uint32_t events;
uint64_t t_int;
uint64_t t_bro =370000;// tempo de broucing 0,75 s
int num=0;
int irq =0;


static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_number(int num) {
    uint32_t color = urgb_u32(led_r, led_g, led_b);
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (numeros[num][ordem[i]]) {
            put_pixel(color);
        } else {
            put_pixel(0);
        }
    }
}
 void contamais(uint gpio, uint32_t events1) {
    //if(botaoA==gpio){
    if(irq==0){
        t_int = time_us_64();
    }
    irq++;
    if( (time_us_64()- t_int)>t_bro){
        //inicio função chamad
        num++;
        if(num == 10){
            num=0;
        }
        set_number(num);
        //fim da função chamada
        irq=0;
    }
    //}
    
    
}
 void contamenos(uint gpio, uint32_t events0) {
    //if(botaoB==gpio){
    if(irq==0){
        t_int = time_us_64();
    }
    irq++;
    if( (time_us_64()- t_int)>t_bro){
        //inicio função chamad
        num--;
        if(num == -1){
            num=9;
        }
        set_number(num);
        //fim da função chamada
        irq=0;
    }
    //}
}

int main() {
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_init(botaoA);
    gpio_set_dir(botaoA,GPIO_IN);
    gpio_pull_up(botaoA);
    gpio_init(botaoB);
    gpio_set_dir(botaoB,GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB,GPIO_IRQ_EDGE_FALL,true, &contamenos);
    gpio_set_irq_enabled_with_callback(botaoA,GPIO_IRQ_EDGE_RISE,true, &contamais);
    
    

    set_number(num);

    while (1) {
    /*for (int cont=0;cont<25;cont++){
        set_number(cont);
        sleep_ms(1000);
    }*/
    }

    return 0;
}
