const int LIGHT_SENSOR = A0;
const int THRESHOLD_SUN_SHINING = 2500;
const int THRESHOLD_SUN_HIDING = 500;
const int REQUIRED_THRESHOLD_READING_COUNT = 10;
const int TIMEZONE_SYDNEY = 10;
const int DAILY_SUMMARY_HOUR = 19;

bool isSunShining = false;
int sunShiningReadingCount = 0;
int sunHidingReadingCount = 0;

double numTotalHoursSpentInSun;
int lastReading;
time_t timeWhenSunBeganToShine;

void setup() {
  Time.zone(TIMEZONE_SYDNEY);

  Particle.variable("isSunShining", &isSunShining, BOOLEAN);
  Particle.variable("numTotalHoursSpentInSun", &numTotalHoursSpentInSun, DOUBLE);
  Particle.variable("lastReading", &lastReading, INT);
  Particle.variable("sunShiningReadingCount", &sunShiningReadingCount, INT);
  Particle.variable("sunHidingReadingCount", &sunHidingReadingCount, INT);

  pinMode(LIGHT_SENSOR, INPUT);
}

void loop() {
  int lightLevel = analogRead(LIGHT_SENSOR);

  if (!isSunShining && lastReading >= THRESHOLD_SUN_SHINING) {
    sunShiningReadingCount++;
    if (sunShiningReadingCount == REQUIRED_THRESHOLD_READING_COUNT) {
      isSunShining = true;
      timeWhenSunBeganToShine = Time.now();
      sunShiningReadingCount = 0;
      Particle.publish("SunStatus", "shining");
    }
  } else if (isSunShining && lastReading <= THRESHOLD_SUN_HIDING) {
    sunHidingReadingCount++;
    if (sunHidingReadingCount == REQUIRED_THRESHOLD_READING_COUNT) {
      isSunShining = false;

      double timeDiffInSeconds = difftime(Time.now(), timeWhenSunBeganToShine);
      double timeDiffInHours = timeDiffInSeconds / 3600;
      numTotalHoursSpentInSun += timeDiffInHours;

      sunHidingReadingCount = 0;
      Particle.publish("SunStatus", "hiding");
    }
  }

  lastReading = lightLevel;

  if (Time.hour() >= DAILY_SUMMARY_HOUR) {
    Particle.publish("DailySummary", String::format("%d", round(numTotalHoursSpentInSun)));
  }

  delay(1000);
}
