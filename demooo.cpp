#include "mbed.h"
#include <stdint.h>

DigitalOut leds[] = {PTC1, PTC2, PTC3, PTC4, PTC5, PTC7, PTC8};
int led_index = 0; // Index aktuální LED v poli
int brightness = 100; // Počáteční jas nastaven na maximum

DigitalIn g_but_PTC9(PTC9);  // Tlačítko 1
DigitalIn g_but_PTC10(PTC10); // Tlačítko 2

void control_from_ticker() {
    static int l_ticks = 0;
    int32_t l_period = 500; // T = 0.5 sec

    // Řízení jasu LED
    static int button_pressed = 0;
    if (g_but_PTC9 == 0 && !button_pressed) { // Zkontrolujte tlačítko a zajistěte, aby nebylo již stisknuto
        button_pressed = 1; // Nastavte příznak stisknutí tlačítka
        brightness -= 10; // Snížit jas o 10 % při stisknutí tlačítka
        if (brightness < 0) {
            brightness = 0; // Zajistěte, aby jas nešel do záporných hodnot
        }
    } else if (g_but_PTC9 == 1 && button_pressed) { // Zkontrolujte, zda je tlačítko uvolněno a příznak je nastaven
        button_pressed = 0; // Resetujte příznak stisknutí tlačítka
    }

    // Výpočet duty cycle na základě jasu
    int duty_cycle = (brightness * l_period) / 100;

    // Aktualizace jasu LED na základě duty cycle
    if (l_ticks < duty_cycle) {
        leds[led_index] = 1; // LED zapnuta
    } else {
        leds[led_index] = 0; // LED vypnuta
    }

    l_ticks++; // Inkrementujte ticky
    if (l_ticks >= l_period) { // Resetujte ticky, pokud překročí periodu
        l_ticks = 0;
    }
}

void switch_led() {
    // Přepínání mezi LED
    led_index = (led_index + 1) % 7; // Zvýšíme index, pokud je možné, jinak se vrátíme na začátek pole
}

int main() {
    printf("LED demo program started...\n");

    // moderní přístup s časovačem (a přerušením)
    Ticker l_ticker;
    l_ticker.attach(control_from_ticker, 1ms);

    while (1) {
        // Přepínání LED pomocí tlačítka PTC10
        if (g_but_PTC10 == 0) {
            switch_led();
            while (g_but_PTC10 == 0); // Počkej, dokud není tlačítko uvolněno
            ThisThread::sleep_for(300ms); // Čekáme 300 ms, abychom zabránili rychlému přepínání LED
        }

        // Nastavení jasu pomocí tlačítka PTC9
        // řízeno uvnitř funkce control_from_ticker

        ThisThread::sleep_for(10ms); // Uspání vlákna na krátkou dobu, abychom snížili zatížení procesoru
    }
}
