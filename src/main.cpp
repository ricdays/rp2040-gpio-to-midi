#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include <hardware/gpio.h>


#define INPUT_PIN 16
#define ALWAYS_SEND 0

enum UsbState {
  USB_NOT_MOUNTED = 0,
  USB_MOUNTED,
  USB_SUSPENDED,
};

enum ActivePreset
{
  Unknown = -1,
  Master = 0,
  Auxiliary = 1
};

UsbState currentUsbState = UsbState::USB_NOT_MOUNTED;
ActivePreset currentPreset = ActivePreset::Unknown;

// Preset messages
uint8_t pc_presetA[2] = { 0xC0, 0x00 };
uint8_t pc_presetB[2] = { 0xC0, 0x01 };
const uint32_t kPresetSendInterval_ms = 200;

// Debounce input
const int kDebounceWindow = 10;
int inputDebounceValue = kDebounceWindow / 2;
const uint32_t kDebouncePeriod_ms = 20;

// Blink frequency
const uint32_t kBlink_mountedA = 1000;
const uint32_t kBlink_mountedB = 250;

bool changedPreset = false;

// ---

void led_blinking_task(void);
void midi_task(void);
void gpio_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  tusb_init();

  gpio_init(INPUT_PIN);
  gpio_set_dir(INPUT_PIN, GPIO_IN);

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
    midi_task();
    gpio_task();
  }


  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  currentUsbState = UsbState::USB_MOUNTED;
  changedPreset = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  currentUsbState = UsbState::USB_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  currentUsbState = UsbState::USB_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  currentUsbState = UsbState::USB_MOUNTED;
  changedPreset = true;
}

void gpio_task(void)
{
  static uint32_t start_ms = 0;

  if (currentUsbState != UsbState::USB_MOUNTED)
  {
    // reset input debounce
    inputDebounceValue = kDebounceWindow / 2;
    currentPreset = ActivePreset::Unknown;
    return;
  }

  // send note periodically
  if (board_millis() - start_ms < kDebouncePeriod_ms) return; // not enough time
  start_ms += kDebouncePeriod_ms;

  bool val = gpio_get(INPUT_PIN);
  if (val && inputDebounceValue < kDebounceWindow)
  {
    inputDebounceValue++;
    if (inputDebounceValue == kDebounceWindow) {
      changedPreset = true;
      currentPreset = ActivePreset::Master;
    }
  } else if (!val && inputDebounceValue > 0) {
    inputDebounceValue--;
    if (inputDebounceValue == 0) {
      changedPreset = true;
      currentPreset = ActivePreset::Auxiliary;
    }
  }

}

//--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+

void midi_task(void)
{
  static uint32_t start_ms = 0;

  uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint

  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be read
  // (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while ( tud_midi_available() ) tud_midi_packet_read(packet);

  // send note periodically
  if (board_millis() - start_ms < kPresetSendInterval_ms) return; // not enough time
  start_ms += kPresetSendInterval_ms;

  if (currentUsbState == UsbState::USB_MOUNTED)
  {
    bool doSend = false;
#if ALWAYS_SEND
    doSend = true;
#else
    if (changedPreset)
    {
      doSend = true;
      changedPreset = false;
    }
#endif

    if (doSend)
    {
      if (currentPreset == ActivePreset::Master)
        tud_midi_stream_write(cable_num, pc_presetA, 2);
      else if (currentPreset == ActivePreset::Auxiliary)
        tud_midi_stream_write(cable_num, pc_presetB, 2);
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  uint32_t blink_interval_ms = -1;
  if (currentUsbState == UsbState::USB_MOUNTED)
  {
    if (currentPreset == ActivePreset::Master)
      blink_interval_ms = kBlink_mountedA;
    else if (currentPreset == ActivePreset::Auxiliary)
      blink_interval_ms = kBlink_mountedB;
  }

  if (blink_interval_ms < 0)
  {
    led_state = false;
    board_led_write(led_state);
    return;
  }

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
