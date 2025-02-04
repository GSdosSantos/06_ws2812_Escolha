
#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include "numeros.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define TEMPO 1500

#define botaoA 5
#define botaoB 6
#define ledver 13
uint8_t led_r = 0; // Vermelho
uint8_t led_g = 0; // Verde
uint8_t led_b = 50; // Azul
uint32_t events;
uint64_t t_int;
uint64_t t_bro =20000;// tempo de broucing 0,75 s
int num=0;
int irq =0;
bool estado = true;


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

void contamais() {
    num++;
    if(num == 10){
        num=0;
    }
    set_number(num);
}

void contamenos() {
    num--;
    if(num == -1){
        num=9;
    }
    set_number(num);
}

void olhar(uint gpio, uint32_t events0) {
        if(irq==0){
        t_int = time_us_64();
    }
    irq++;
    if( (time_us_64()- t_int)>t_bro){
        //inicio função chamad
        if(gpio==botaoA){
        contamais();
        }
        if(gpio==botaoB){
        contamenos();
        }
        irq=0;
    }
}
bool repeating_timer_callback(struct repeating_timer *t) {
    gpio_put(ledver,estado);
    estado = !estado;
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
    gpio_init(ledver);
    gpio_set_dir(ledver,GPIO_OUT);

    gpio_set_irq_enabled_with_callback(botaoA,GPIO_IRQ_EDGE_RISE,true, &olhar);
    gpio_set_irq_enabled_with_callback(botaoB,GPIO_IRQ_EDGE_RISE,true, &olhar);
    repeating_timer_t timer;
    add_repeating_timer_ms(100, repeating_timer_callback,NULL, &timer);

    set_number(num);

    while (true) {

    }

    return 0;
}
