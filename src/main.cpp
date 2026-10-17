//
// Adapted from Infineon sample source
// https://github.com/Infineon/mtb-example-xmc-uart-transmit
//

#include <Arduino.h>

#define LED_PORT XMC_GPIO_PORT5
#define LED_PIN 8

#define TICKS_PER_SECOND 1000
#define TICKS_WAIT 1000

static constexpr uint8_t message[] = "Hello world!!\r\n";
static constexpr uint8_t clear_screen[] = "\x1b[2J\x1b[;H";

static unsigned long previous_millis = millis();

XMC_GPIO_CONFIG_t led_config = {.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
                                .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
                                .output_strength =
                                    XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE};

XMC_GPIO_CONFIG_t tx_config = {.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
                               .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
                               .output_strength =
                                   XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE};

XMC_UART_CH_CONFIG_t uart_config = {.baudrate = 115200, // Default setting
                                    .data_bits = 8,
                                    .frame_length = 8,
                                    .stop_bits = 1,
                                    .oversampling = 16,
                                    .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE};

void uart_init() {

  /*Initialize and configure UART0 on channel 0 */
  XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);

  /*Configure TX*/
  XMC_GPIO_Init(P1_5, &tx_config);

  /*Set input source path*/
  XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX0, 1U);

  /*Configure transmit FIFO*/
  XMC_USIC_CH_TXFIFO_Configure(XMC_UART0_CH0, 16U,
                               XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);

  /*Configure receive FIFO*/
  XMC_USIC_CH_RXFIFO_Configure(XMC_UART0_CH0, 0U, XMC_USIC_CH_FIFO_SIZE_16WORDS,
                               15U);

  /*Start UART */
  XMC_UART_CH_Start(XMC_UART0_CH0);
}

void update() {

  auto current_millis = millis();
  if (current_millis - previous_millis >= TICKS_WAIT) {
    // Toggle the LED
    XMC_GPIO_ToggleOutput(LED_PORT, LED_PIN);

    for (int32_t i = 0; i < sizeof(message) - 1; i++) {
      // Send a message via UART periodically
      XMC_UART_CH_Transmit(XMC_UART0_CH0, message[i]);
    }
    previous_millis = current_millis;
  }
}

void setup(void) {
  // LED pin initialization
  XMC_GPIO_Init(LED_PORT, LED_PIN, &led_config);

  // UART inialization
  uart_init();

  // Send a message to clear the screen
  for (uint32_t i = 0; i < sizeof(clear_screen) - 1; i++) {
     XMC_UART_CH_Transmit(XMC_UART0_CH0, clear_screen[i]);
  }
} 

void loop(void) {
  update(); 
}