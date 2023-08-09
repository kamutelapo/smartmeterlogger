#ifndef METER_DATA_H
#define METER_DATA_H

#include "Arduino.h"

class MeterData {
  private:
    String meterName;
    String meterCosemName;
    String meterId;
    String time;
    int    tariff;
    String state;
    float  maxPower;
    float  importPower1;
    float  importPower2;
    float  importPower3;
    float  importPower4;
    float  importPower5;
    float  exportPower1;
    float  exportPower2;
    float  exportPower3;
    float  exportPower4;
    float  exportPower5;
    float  importReactivePower;
    float  exportReactivePower;
    float  reactivePower1;
    float  reactivePower2;
    float  reactivePower3;
    float  reactivePower4;
    float  effectivePower;
    float  voltage1;
    float  voltage2;
    float  voltage3;
    int    current1;
    int    current2;
    int    current3;
    float  powerFactor;
    float  powerFactor1;
    float  powerFactor2;
    float  powerFactor3;
    float  frequency;
    float  currentImport;
    float  currentExport;
    float  currentReactivePower1;
    float  currentReactivePower2;
    float  currentReactivePower3;
    float  currentReactivePower4;

    String lastMonthTime;
    float  lastMonthImportPower1;
    float  lastMonthImportPower2;
    float  lastMonthImportPower3;
    float  lastMonthExportPower1;
    float  lastMonthExportPower2;
    float  lastMonthExportPower3;
    float  lastMonthImportReactivePower;
    float  lastMonthExportReactivePower;
    float  lastMonthReactivePower1;
    float  lastMonthReactivePower2;
    float  lastMonthReactivePower3;
    float  lastMonthReactivePower4;
    float  lastMonthEffectivePower;
    float  lastMonthMaxImportPower1;
    float  lastMonthMaxImportPower2;
    float  lastMonthMaxImportPower3;
    float  lastMonthMaxExportPower1;
    float  lastMonthMaxExportPower2;
    float  lastMonthMaxExportPower3;

    String message;

    unsigned int attributes = 0;
    unsigned int attributes2 = 0;

  public:
    bool isValid();
    String toJSON();
    void parseLine(const char *);

  private:
    String removeLeadingZeros(const String &);
    String removeTrailingString(const String &str, const char * trail);
    String fetchArg(String &);
};

#endif /* METER_DATA_H */
