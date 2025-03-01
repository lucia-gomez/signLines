#include "Arduino.h"
#include "Colors.h"
#include <vector>

extern int activeColor;

class ColorPattern {
  public:
    std::vector<Color> colors;
    std::vector<long> durations;
    size_t size;
    unsigned long previousMillis = 0;
    int colorIndex = 0;
  
    ColorPattern(std::vector<Color> c, std::vector<long> d) {
      colors = c;
      durations = d;
      size = c.size();
    }

    void startPattern() {
      previousMillis = millis();
    }

    int updateColorPattern() {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= durations[colorIndex]) {
        previousMillis = currentMillis;

        colorIndex = (colorIndex + 1) % size;
        activeColor = static_cast<Color>(colors[colorIndex]);

        Serial.print("Color set to: ");
        switch (activeColor) {
          case RED: Serial.println("RED"); break;
          case ORANGE: Serial.println("ORANGE"); break;
          case YELLOW: Serial.println("YELLOW"); break;
          case GREEN: Serial.println("GREEN"); break;
          case BLUE: Serial.println("BLUE"); break;
          case PURPLE: Serial.println("PURPLE"); break;
          case WATER: Serial.println("WATER"); break;
        }
      }
    }
};
