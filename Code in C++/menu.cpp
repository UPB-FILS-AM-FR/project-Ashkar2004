#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "ssd1306.hpp"  // Your SSD1306 driver header (adjust if different)
#include <array>
#include <string>

// Screen dimensions
constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;

// GPIO pins for buttons
constexpr uint UP_PIN = 2;
constexpr uint DOWN_PIN = 3;
constexpr uint LEFT_PIN = 4;
constexpr uint RIGHT_PIN = 5;
constexpr uint BUTTON1_PIN = 6;
constexpr uint BUTTON2_PIN = 7;

// Buzzer pin
constexpr uint BUZZER_PIN = 18;

// List of game names
const std::array<const char*, 8> GAMELIST = {
    "Pong", "Snake", "Space Invaders", "Dino",
    "2048", "Tetris", "Full Speed", "Lunar Module"
};

void buzzer_beep() {
    // Initialize PWM slice for buzzer on BUZZER_PIN
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 65535);
    pwm_init(slice_num, &config, true);

    // Set freq approx 1kHz
    uint16_t level = 20000; // duty cycle
    pwm_set_gpio_level(BUZZER_PIN, level);

    sleep_ms(100);

    pwm_set_gpio_level(BUZZER_PIN, 0);
    pwm_deinit(slice_num);
}

int main() {
    stdio_init_all();

    // Small delay to avoid startup errors like Python time.sleep(0.2)
    sleep_ms(200);

    // Initialize buttons with pull-ups
    gpio_init(UP_PIN); gpio_set_dir(UP_PIN, GPIO_IN); gpio_pull_up(UP_PIN);
    gpio_init(DOWN_PIN); gpio_set_dir(DOWN_PIN, GPIO_IN); gpio_pull_up(DOWN_PIN);
    gpio_init(LEFT_PIN); gpio_set_dir(LEFT_PIN, GPIO_IN); gpio_pull_up(LEFT_PIN);
    gpio_init(RIGHT_PIN); gpio_set_dir(RIGHT_PIN, GPIO_IN); gpio_pull_up(RIGHT_PIN);
    gpio_init(BUTTON1_PIN); gpio_set_dir(BUTTON1_PIN, GPIO_IN); gpio_pull_up(BUTTON1_PIN);
    gpio_init(BUTTON2_PIN); gpio_set_dir(BUTTON2_PIN, GPIO_IN); gpio_pull_up(BUTTON2_PIN);

    // Initialize I2C0 on GPIO 14 (SDA) and 15 (SCL)
    i2c_init(i2c1, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);

    // Initialize OLED display
    SSD1306 display(i2c1, SCREEN_WIDTH, SCREEN_HEIGHT); // Adjust constructor per your driver

    int current = 0;
    int game_selected = -1;

    while (true) {
        display.clear();

        // Draw the list of games with highlight
        for (size_t row = 0; row < GAMELIST.size(); row++) {
            if ((int)row == current) {
                display.fillRect(0, row * 8, SCREEN_WIDTH, 7, SSD1306::Color::White);
                display.setTextColor(SSD1306::Color::Black);
            } else {
                display.setTextColor(SSD1306::Color::White);
            }

            int text_x = SCREEN_WIDTH / 2 - (strlen(GAMELIST[row]) * 8) / 2;
            display.setCursor(text_x, row * 8);
            display.print(GAMELIST[row]);
        }

        display.display();

        sleep_ms(200);

        bool buttonPressed = false;

        while (!buttonPressed) {
            // Read buttons (active low)
            if ((gpio_get(DOWN_PIN) == 0 || gpio_get(RIGHT_PIN) == 0) && current < (int)GAMELIST.size() - 1) {
                current++;
                buttonPressed = true;
            }
            else if ((gpio_get(UP_PIN) == 0 || gpio_get(LEFT_PIN) == 0) && current > 0) {
                current--;
                buttonPressed = true;
            }
            else if (gpio_get(BUTTON1_PIN) == 0 || gpio_get(BUTTON2_PIN) == 0) {
                buttonPressed = true;
                game_selected = current;
            }
            sleep_ms(10); // debounce delay
        }

        // Make beep sound on button press
        buzzer_beep();

        if (game_selected >= 0) {
            display.clear();
            display.display();

            // Call the selected game's main function here
            switch (game_selected) {
                case 0:
                    // pico_pong_main();
                    break;
                case 1:
                    // pico_snake_main();
                    break;
                case 2:
                    // pico_invaders_main();
                    break;
                case 3:
                    // pico_dino_main();
                    break;
                case 4:
                    // pico_2048_main();
                    break;
                case 5:
                    // pico_tetris_main();
                    break;
                case 6:
                    // pico_full_speed_main();
                    break;
                case 7:
                    // pico_lunar_module_main();
                    break;
            }

            game_selected = -1;
        }
    }
    return 0;
}
