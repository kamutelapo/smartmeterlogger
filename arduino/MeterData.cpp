#include "MeterData.h"

typedef enum {
  HAS_NAME                     = 0x00000001,
  HAS_COSEM_NAME               = 0x00000002,
  HAS_METER_ID                 = 0x00000004,
  HAS_TIME                     = 0x00000008,
  HAS_TARIFF                   = 0x00000010,
  HAS_STATE                    = 0x00000020,
  HAS_MAX_POWER                = 0x00000040,
  HAS_IMPORT_POWER_1           = 0x00000080,
  HAS_IMPORT_POWER_2           = 0x00000100,
  HAS_IMPORT_POWER_3           = 0x00000200,
  HAS_IMPORT_POWER_4           = 0x00000400,
  HAS_IMPORT_POWER_5           = 0x00000800,
  HAS_EXPORT_POWER_1           = 0x00001000,
  HAS_EXPORT_POWER_2           = 0x00002000,
  HAS_EXPORT_POWER_3           = 0x00004000,
  HAS_EXPORT_POWER_4           = 0x00008000,
  HAS_EXPORT_POWER_5           = 0x00010000,
  HAS_IMPORT_REACTIVE_POWER    = 0x00020000,
  HAS_EXPORT_REACTIVE_POWER    = 0x00040000,
  HAS_REACTIVE_POWER_1         = 0x00080000,
  HAS_REACTIVE_POWER_2         = 0x00100000,
  HAS_REACTIVE_POWER_3         = 0x00200000,
  HAS_REACTIVE_POWER_4         = 0x00400000,
  HAS_REACTIVE_POWER_5         = 0x00800000,
  HAS_EFFECTIVE_POWER          = 0x01000000,
  HAS_VOLTAGE_1                = 0x02000000,
  HAS_VOLTAGE_2                = 0x04000000,
  HAS_VOLTAGE_3                = 0x08000000,
  HAS_CURRENT_1                = 0x10000000,
  HAS_CURRENT_2                = 0x20000000,
  HAS_CURRENT_3                = 0x40000000,
} RECEIVED_ATTRS;

typedef enum {
  HAS_POWER_FACTOR             = 0x00000001,
  HAS_POWER_FACTOR_1           = 0x00000002,
  HAS_POWER_FACTOR_2           = 0x00000004,
  HAS_POWER_FACTOR_3           = 0x00000008,
  HAS_FREQUENCY                = 0x00000010,
  HAS_CURRENT_IMPORT           = 0x00000020,
  HAS_CURRENT_EXPORT           = 0x00000040,
  HAS_CURRENT_REACTIVE_POWER_1 = 0x00000080,
  HAS_CURRENT_REACTIVE_POWER_2 = 0x00000100,
  HAS_CURRENT_REACTIVE_POWER_3 = 0x00000200,
  HAS_CURRENT_REACTIVE_POWER_4 = 0x00000400,
  HAS_LAST_MONTH_DATA          = 0x00000800,
  HAS_MESSAGE                  = 0x00001000,
} RECEIVED_ATTRS_2;



#define REQUIRED_ATTRIBS (HAS_NAME | HAS_METER_ID | HAS_TIME | HAS_IMPORT_POWER_1 | HAS_EXPORT_POWER_1)

String MeterData::removeLeadingZeros(const String &str) {
  const char * strbuf = str.c_str();

  while ((strbuf[0] == '0') && (isdigit(strbuf[1]))) {
    strbuf++;
  }

  return String(strbuf);
}

String MeterData::removeTrailingString(const String &str, const char * trail) {
  if ( str.endsWith(trail) ) {
    return str.substring(0, str.length() - strlen(trail));
  }
  return str;
}

String MeterData::fetchArg(String &str) {
  char * bcopen = strchr(str.c_str(), '(');
  char * bcclose = strchr(str.c_str(), ')');

  if (( bcopen != 0 ) && (bcclose != 0)) {
    String linestr = str.c_str();
    int ndx = bcopen - str.c_str();
    int ndx2 = bcclose - str.c_str();

    String args = linestr.substring(ndx + 1, ndx2);
    String remn = str.substring(ndx2 + 1);
    str = remn;
    return args;
  }
  return String();
}

void MeterData::parseLine(const char *line) {
  if (line[0] == '/') {
    meterName = String(line + 1);
    meterName.trim();
    attributes |= HAS_NAME;
    return;
  }
  if ((line[0] == '!') || (line[0] == 0) || (line[0] == '\n') || ((line[0] == '\r') && (line[1] == '\n'))) {
    return;
  }


  char * bcopen = strchr(line, '(');
  char * bcclose = strchr(line, ')');

  if (( bcopen != 0 ) && (bcclose != 0)) {
    String linestr = line;
    int ndx = bcopen - line;
    int ndx2 = bcclose - line;

    String token = linestr.substring(0, ndx);
    token.trim();
    String args = linestr.substring(ndx + 1, ndx2);

    if ( token == "0-0:42.0.0" ) {
      meterCosemName = args;
      attributes |= HAS_COSEM_NAME;
      return;
    }
    else if ( token == "0-0:96.1.0" ) {
      meterId = args;
      attributes |= HAS_METER_ID;
      return;
    }
    else if ( token == "0-0:1.0.0" ) {
      this->time = args;
      attributes |= HAS_TIME;
      return;
    }
    else if ( token == "0-0:96.14.0" ) {
      args = removeLeadingZeros(args);
      tariff = atoi(args.c_str());
      attributes |= HAS_TARIFF;
      return;
    }
    else if ( token == "0-0:96.50.68" ) {
      state = args;
      attributes |= HAS_STATE;
      return;
    }
    else if ( token == "0-0:17.0.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kW"));
      maxPower = atof(args.c_str());
      attributes |= HAS_MAX_POWER;
      return;
    }
    else if ( token == "1-0:1.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      importPower1 = atof(args.c_str());
      attributes |= HAS_IMPORT_POWER_1;
      return;
    }
    else if ( token == "1-0:1.8.1" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      importPower2 = atof(args.c_str());
      if ( importPower2 != 0.0 ) {
        attributes |= HAS_IMPORT_POWER_2;
      }
      return;
    }
    else if ( token == "1-0:1.8.2" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      importPower3 = atof(args.c_str());
      if ( importPower3 != 0.0 ) {
        attributes |= HAS_IMPORT_POWER_3;
      }
      return;
    }
    else if ( token == "1-0:1.8.3" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      importPower4 = atof(args.c_str());
      if ( importPower4 != 0.0 ) {
        attributes |= HAS_IMPORT_POWER_4;
      }
      return;
    }
    else if ( token == "1-0:1.8.4" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      importPower5 = atof(args.c_str());
      if ( importPower5 != 0.0 ) {
        attributes |= HAS_IMPORT_POWER_5;
      }
      return;
    }
    else if ( token == "1-0:2.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      exportPower1 = atof(args.c_str());
      attributes |= HAS_EXPORT_POWER_1;
      return;
    }
    else if ( token == "1-0:2.8.1" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      exportPower2 = atof(args.c_str());
      if ( exportPower2 != 0.0 ) {
        attributes |= HAS_EXPORT_POWER_2;
      }
      return;
    }
    else if ( token == "1-0:2.8.2" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      exportPower3 = atof(args.c_str());
      if ( exportPower3 != 0.0 ) {
        attributes |= HAS_EXPORT_POWER_3;
      }
      return;
    }
    else if ( token == "1-0:2.8.3" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      exportPower4 = atof(args.c_str());
      if ( exportPower4 != 0.0 ) {
        attributes |= HAS_EXPORT_POWER_4;
      }
      return;
    }
    else if ( token == "1-0:2.8.4" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      exportPower5 = atof(args.c_str());
      if ( exportPower5 != 0.0 ) {
        attributes |= HAS_EXPORT_POWER_5;
      }
      return;
    }
    else if ( token == "1-0:3.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      importReactivePower = atof(args.c_str());
      attributes |= HAS_IMPORT_REACTIVE_POWER;
      return;
    }
    else if ( token == "1-0:4.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      exportReactivePower = atof(args.c_str());
      attributes |= HAS_EXPORT_REACTIVE_POWER;
      return;
    }
    else if ( token == "1-0:5.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      reactivePower1 = atof(args.c_str());
      attributes |= HAS_REACTIVE_POWER_1;
      return;
    }
    else if ( token == "1-0:6.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      reactivePower2 = atof(args.c_str());
      attributes |= HAS_REACTIVE_POWER_2;
      return;
    }
    else if ( token == "1-0:7.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      reactivePower3 = atof(args.c_str());
      attributes |= HAS_REACTIVE_POWER_3;
      return;
    }
    else if ( token == "1-0:8.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvarh"));
      reactivePower4 = atof(args.c_str());
      attributes |= HAS_REACTIVE_POWER_4;
      return;
    }
    else if ( token == "1-0:15.8.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kWh"));
      effectivePower = atof(args.c_str());
      attributes |= HAS_EFFECTIVE_POWER;
      return;
    }
    else if ( token == "1-0:32.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*V"));
      voltage1 = atof(args.c_str());
      attributes |= HAS_VOLTAGE_1;
      return;
    }
    else if ( token == "1-0:52.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*V"));
      voltage2 = atof(args.c_str());
      attributes |= HAS_VOLTAGE_2;
      return;
    }
    else if ( token == "1-0:72.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*V"));
      voltage3 = atof(args.c_str());
      attributes |= HAS_VOLTAGE_3;
      return;
    }
    else if ( token == "1-0:31.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*A"));
      current1 = atoi(args.c_str());
      attributes |= HAS_CURRENT_1;
      return;
    }
    else if ( token == "1-0:51.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*A"));
      current2 = atoi(args.c_str());
      attributes |= HAS_CURRENT_2;
      return;
    }
    else if ( token == "1-0:71.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*A"));
      current3 = atoi(args.c_str());
      attributes |= HAS_CURRENT_3;
      return;
    }
    else if ( token == "1-0:13.7.0" ) {
      powerFactor = atof(args.c_str());
      attributes2 |= HAS_POWER_FACTOR;
      return;
    }
    else if ( token == "1-0:33.7.0" ) {
      powerFactor1 = atof(args.c_str());
      attributes2 |= HAS_POWER_FACTOR_1;
      return;
    }
    else if ( token == "1-0:53.7.0" ) {
      powerFactor2 = atof(args.c_str());
      attributes2 |= HAS_POWER_FACTOR_2;
      return;
    }
    else if ( token == "1-0:73.7.0" ) {
      powerFactor3 = atof(args.c_str());
      attributes2 |= HAS_POWER_FACTOR_3;
      return;
    }
    else if ( token == "1-0:14.7.0" ) {
      args = removeTrailingString(args, "*Hz");
      frequency = atof(args.c_str());
      attributes2 |= HAS_FREQUENCY;
      return;
    }
    else if ( token == "1-0:1.7.0" ) {
      args = removeTrailingString(args, "*kW");
      currentImport = atof(args.c_str());
      attributes2 |= HAS_CURRENT_IMPORT;
      return;
    }
    else if ( token == "1-0:2.7.0" ) {
      args = removeTrailingString(args, "*kW");
      currentExport = atof(args.c_str());
      attributes2 |= HAS_CURRENT_EXPORT;
      return;
    }
    else if ( token == "1-0:5.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvar"));
      currentReactivePower1 = atof(args.c_str());
      attributes2 |= HAS_CURRENT_REACTIVE_POWER_1;
      return;
    }
    else if ( token == "1-0:6.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvar"));
      currentReactivePower2 = atof(args.c_str());
      attributes2 |= HAS_CURRENT_REACTIVE_POWER_2;
      return;
    }
    else if ( token == "1-0:7.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvar"));
      currentReactivePower3 = atof(args.c_str());
      attributes2 |= HAS_CURRENT_REACTIVE_POWER_3;
      return;
    }
    else if ( token == "1-0:8.7.0" ) {
      args = removeLeadingZeros(removeTrailingString(args, "*kvar"));
      currentReactivePower4 = atof(args.c_str());
      attributes2 |= HAS_CURRENT_REACTIVE_POWER_4;
      return;
    }
    else if ( token == "0-0:98.1.0" ) {
      String allargs = line;
      args = fetchArg(allargs);

      lastMonthTime = args;

      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthImportPower1 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthImportPower2 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthImportPower3 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthExportPower1 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthExportPower2 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthExportPower3 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthImportReactivePower = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthExportReactivePower = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthReactivePower1 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthReactivePower2 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthReactivePower3 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kvarh"));
      lastMonthReactivePower4 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kWh"));
      lastMonthEffectivePower = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxImportPower1 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxImportPower2 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxImportPower3 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxExportPower1 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxExportPower2 = atof(args.c_str());
      args = removeLeadingZeros(removeTrailingString(fetchArg(allargs), "*kW"));
      lastMonthMaxExportPower3 = atof(args.c_str());

      attributes2 |= HAS_LAST_MONTH_DATA;
      return;
    }
    else if ( token == "0-0:96.13.0" ) {
      char msgs[strlen(args.c_str()) + 1];
      strcpy(msgs, args.c_str());

      char * ptr = msgs;
      while (*ptr) {
        if (*ptr == 255) {
          *ptr = ' ';
        }
        ptr++;
      }
      message = msgs;
      message.trim();

      if ( message.length() > 0) {
        attributes2 |= HAS_MESSAGE;
      }
      return;
    }

    Serial.print("Unknown token: ");
    Serial.print(token);
    Serial.print(" -> ");
    Serial.println(args);
    return;
  }

  Serial.print("Unknown line: ");
  Serial.println(line);
}

bool MeterData::isValid() {
  return (attributes & REQUIRED_ATTRIBS) == REQUIRED_ATTRIBS;
}

String MeterData::toJSON() {
  char printbuff[30];

  if ((attributes & REQUIRED_ATTRIBS) == REQUIRED_ATTRIBS) {
    String s = "{";

    s += "\"MeterName\":\"" + meterName + "\",";
    if (attributes & HAS_COSEM_NAME) {
      s += "\"CosemName\":\"" + meterCosemName + "\",";
    }
    s += "\"MeterID\":\"" + meterId + "\",";

    s += "\"Time\":\"" + this->time + "\",";

    if (attributes & HAS_TARIFF) {
      s += "\"Tariff\":" + String(tariff) + ",";
    }

    if (attributes & HAS_STATE) {
      s += "\"State\":\"" + state + "\",";
    }

    if (attributes & HAS_MAX_POWER) {
      snprintf(printbuff, sizeof(printbuff), "%2.3f", maxPower);
      s += "\"MaxPower\":" + String(printbuff) + ",";
    }

    if (!(attributes & HAS_IMPORT_POWER_2)) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", importPower1);
      s += "\"ImportPower\":" + String(printbuff) + ",";
    } else {
      snprintf(printbuff, sizeof(printbuff), "%.3f", importPower1);
      s += "\"ImportPower\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", importPower2);
      s += "," + String(printbuff);

      if ( attributes & HAS_IMPORT_POWER_3 ) {
        snprintf(printbuff, sizeof(printbuff), "%.3f", importPower3);
        s += "," + String(printbuff);
        if ( attributes & HAS_IMPORT_POWER_4 ) {
          snprintf(printbuff, sizeof(printbuff), "%.3f", importPower4);
          s += "," + String(printbuff);
          if ( attributes & HAS_IMPORT_POWER_5 ) {
            snprintf(printbuff, sizeof(printbuff), "%.3f", importPower5);
            s += "," + String(printbuff);
          }
        }
      }

      s += "],";
    }

    if (!(attributes & HAS_EXPORT_POWER_2)) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower1);
      s += "\"ExportPower\":" + String(printbuff) + ",";
    } else {
      snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower1);
      s += "\"ExportPower\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower2);
      s += "," + String(printbuff);

      if ( attributes & HAS_EXPORT_POWER_3 ) {
        snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower3);
        s += "," + String(printbuff);
        if ( attributes & HAS_EXPORT_POWER_4 ) {
          snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower4);
          s += "," + String(printbuff);
          if ( attributes & HAS_EXPORT_POWER_5 ) {
            snprintf(printbuff, sizeof(printbuff), "%.3f", exportPower5);
            s += "," + String(printbuff);
          }
        }
      }

      s += "],";
    }

    if (attributes & HAS_IMPORT_REACTIVE_POWER) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", importReactivePower);
      s += "\"ImportReactivePower\":" + String(printbuff) + ",";
    }

    if (attributes & HAS_EXPORT_REACTIVE_POWER) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", exportReactivePower);
      s += "\"ExportReactivePower\":" + String(printbuff) + ",";
    }

    if (attributes & HAS_REACTIVE_POWER_1) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", reactivePower1);
      s += "\"ReactivePower\":[" + String(printbuff);

      if ( attributes & HAS_REACTIVE_POWER_2 ) {
        snprintf(printbuff, sizeof(printbuff), "%.3f", reactivePower2);
        s += "," + String(printbuff);
        if ( attributes & HAS_REACTIVE_POWER_3 ) {
          snprintf(printbuff, sizeof(printbuff), "%.3f", reactivePower3);
          s += "," + String(printbuff);
          if ( attributes & HAS_REACTIVE_POWER_4 ) {
            snprintf(printbuff, sizeof(printbuff), "%.3f", reactivePower4);
            s += "," + String(printbuff);
          }
        }
      }

      s += "],";
    }

    if (attributes & HAS_EFFECTIVE_POWER) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", effectivePower);
      s += "\"EffectivePower\":" + String(printbuff) + ",";
    }

    if (attributes & HAS_VOLTAGE_1) {
      snprintf(printbuff, sizeof(printbuff), "%.1f", voltage1);
      if ( ! (attributes & HAS_VOLTAGE_2) ) {
        s += "\"Voltage\":" + String(printbuff) + ",";
      } else {
        s += "\"Voltage\":[" + String(printbuff);
        snprintf(printbuff, sizeof(printbuff), "%.1f", voltage2);
        s += "," + String(printbuff);
        if ( attributes & HAS_VOLTAGE_3 ) {
          snprintf(printbuff, sizeof(printbuff), "%.1f", voltage3);
          s += "," + String(printbuff);
        }
        s += "],";
      }
    }

    if (attributes & HAS_CURRENT_1) {
      if ( ! (attributes & HAS_CURRENT_2) ) {
        s += "\"Current\":" + String(current1) + ",";
      } else {
        s += "\"Current\":[" + String(current1);
        s += "," + String(current2);
        if ( attributes & HAS_CURRENT_3 ) {
          s += "," + String(current3);
        }
        s += "],";
      }
    }

    if (attributes2 & HAS_POWER_FACTOR) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", powerFactor);
      s += "\"PowerFactor\":" + String(printbuff) + ",";
    }

    if (attributes2 & HAS_POWER_FACTOR_1) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", powerFactor1);
      s += "\"PowerFactorsByPhase\":[" + String(printbuff);
      if ( attributes2 & HAS_POWER_FACTOR_2) {
        snprintf(printbuff, sizeof(printbuff), "%.3f", powerFactor2);
        s += "," + String(printbuff);
        if ( attributes2 & HAS_POWER_FACTOR_3 ) {
          snprintf(printbuff, sizeof(printbuff), "%.3f", powerFactor3);
          s += "," + String(printbuff);
        }
      }
      s += "],";
    }

    if (attributes2 & HAS_FREQUENCY) {
      snprintf(printbuff, sizeof(printbuff), "%.2f", frequency);
      s += "\"Frequency\":" + String(printbuff) + ",";
    }

    if (attributes2 & HAS_CURRENT_IMPORT) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", currentImport);
      s += "\"CurrentImport\":" + String(printbuff) + ",";
    }

    if (attributes2 & HAS_CURRENT_EXPORT) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", currentExport);
      s += "\"CurrentExport\":" + String(printbuff) + ",";
    }

    if (attributes2 & HAS_CURRENT_REACTIVE_POWER_1) {
      snprintf(printbuff, sizeof(printbuff), "%.3f", currentReactivePower1);
      s += "\"CurrentReactivePower\":[" + String(printbuff);
      if ( attributes2 & HAS_CURRENT_REACTIVE_POWER_2) {
        snprintf(printbuff, sizeof(printbuff), "%.3f", currentReactivePower2);
        s += "," + String(printbuff);
        if ( attributes2 & HAS_CURRENT_REACTIVE_POWER_3 ) {
          snprintf(printbuff, sizeof(printbuff), "%.3f", currentReactivePower3);
          s += "," + String(printbuff);
          if ( attributes2 & HAS_CURRENT_REACTIVE_POWER_4 ) {
            snprintf(printbuff, sizeof(printbuff), "%.3f", currentReactivePower4);
            s += "," + String(printbuff);
          }
        }
      }
      s += "],";
    }

    if (attributes2 & HAS_LAST_MONTH_DATA) {
      s += "\"LastMonth\":{";
      s += "\"Time\":\"" + lastMonthTime + "\",";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthImportPower1);
      s += "\"ImportPower\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthImportPower2);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthImportPower3);
      s += "," + String(printbuff) + "],";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthExportPower1);
      s += "\"ExportPower\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthExportPower2);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthExportPower3);
      s += "," + String(printbuff) + "],";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthImportReactivePower);
      s += "\"ImportReactivePower\":" + String(printbuff) + ",";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthExportReactivePower);
      s += "\"ExportReactivePower\":" + String(printbuff) + ",";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthReactivePower1);
      s += "\"ReactivePower\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthReactivePower2);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthReactivePower3);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthReactivePower4);
      s += "," + String(printbuff) + "],";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthEffectivePower);
      s += "\"EffectivePower\":" + String(printbuff) + ",";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxImportPower1);
      s += "\"MaxImport\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxImportPower2);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxImportPower3);
      s += "," + String(printbuff) + "],";
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxExportPower1);
      s += "\"MaxExport\":[" + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxExportPower2);
      s += "," + String(printbuff);
      snprintf(printbuff, sizeof(printbuff), "%.3f", lastMonthMaxExportPower3);
      s += "," + String(printbuff) + "]";

      s += "}";
    }

    if (attributes2 & HAS_MESSAGE) {
      s += "\"Message\":\"" + message + "\",";
    }

    if ( s.endsWith(",") ) {
      s = s.substring(0, s.length() - 1);
    }
    s += "}";
    return s;
  }
  return String();
}
