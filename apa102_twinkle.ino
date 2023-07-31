#include <Adafruit_DotStar.h>

#define LED_PIN_CLOCK 16
#define LED_PIN_DATA 17

#define LED_COUNT 300

#define STARS_MAX (LED_COUNT >> 1)
#define STARS_LIFETIME_MIN 25
#define STARS_LIFETIME_MAX 250

Adafruit_DotStar strip = Adafruit_DotStar(LED_COUNT, LED_PIN_DATA, LED_PIN_CLOCK, DOTSTAR_BGR);

unsigned int stars = 0;
unsigned int stars_ls[STARS_MAX];
unsigned int stars_ms[STARS_MAX];
unsigned int stars_ts[STARS_MAX];
uint16_t stars_hs[STARS_MAX];

unsigned int v;

unsigned int s, m;

unsigned long t0, t1;

void selftest() {
  int      head  = 0, tail = -10;       // Index of first 'on' and 'off' pixels
  uint32_t color = 0xFF0000;            // 'On' color (starts red)

  while(1) {
    strip.setPixelColor(head, color);   // 'On' pixel at head
    strip.setPixelColor(tail, 0);       // 'Off' pixel at tail
    strip.show();                       // Refresh strip
    delay(10);                          // Pause 20 milliseconds (~50 FPS)

    if(++head >= LED_COUNT) {           // Increment head index.  Off end of strip?
      head = 0;                         //  Yes, reset head index to start
      if((color >>= 8) == 0)            //  Next color (R->G->B) ... past blue now?
        color = 0x000000;               //   Yes, reset to black
    }
    if(++tail >= LED_COUNT) {           // Increment tail index.  Off end of strip?
      tail = 0;                         //  Yes, reset tail index to start
      if (color == 0)
        break;
    }
  }

  strip.fill(0, 0, 0);
  strip.show();
}

void setup() {
  strip.begin();                        // Initialize pins for output
  strip.show();                         // Turn all LEDs off ASAP

  Serial.begin(74880);
  Serial.println("apa102_twinkle");
  randomSeed(analogRead(0));

  selftest();
  Serial.println("Selftest done");
}

void loop() {
  t0 = millis();
  strip.fill(0,0,0);

  if (stars < STARS_MAX && random(stars*4) < STARS_MAX) {
    stars_ts[stars] = 0;
    stars_ls[stars] = random(LED_COUNT);        // TODO: check free
    stars_ms[stars] = STARS_LIFETIME_MIN + random(STARS_LIFETIME_MAX - STARS_LIFETIME_MIN);
    stars_hs[stars] = random(65536);
    stars++;
  }

  for (s = stars; s > 0; s--) {
    if (stars_ts[s-1]++ < stars_ms[s-1]) {
      v = (stars_ts[s-1] << 1);

      if (v > stars_ms[s-1])
        v = (stars_ms[s-1] << 1) - v;

      strip.setPixelColor(stars_ls[s-1], strip.ColorHSV(stars_hs[s-1], 255, (255 * v) / stars_ms[s-1]));
    } else {
      // remove star
      // by moving all stars above it down one position
      for (m = s; m < stars; m++) {
        stars_ts[m-1] = stars_ts[m];
        stars_ls[m-1] = stars_ls[m];
        stars_ms[m-1] = stars_ms[m];
        stars_hs[m-1] = stars_hs[m];
      }
      stars--;
    }
  }

  strip.show();
  do {
    t1 = millis();
  } while (t0 <= t1 && t1 < t0 + 40);
}
