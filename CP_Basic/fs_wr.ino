// Loads the configuration from a file
void load_config(fs::FS &fs, const char *filename, Config &config)
{
  // Open file for reading
  File file = fs.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<384> doc;

  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.print("Gagal load");
    Serial.println(error.c_str());
    state = error_state;
    return;
  }

  free(config.seri_infus);
  free(config.nama_pasien);
  free(config.kode_kamar);
  free(config.kode_bed);
  free(config.koneksi);
  free(config.wifi_ssid);
  free(config.wifi_pass);
  free(config.sim_ssid);
  free(config.sim_pass);

  config.seri_infus = (char *)malloc(sizeof(char) * strlen(doc["seri_infus"]));
  strcpy(config.seri_infus, doc["seri_infus"]);
  config.nama_pasien = (char *)malloc(sizeof(char) * strlen(doc["nama_pasien"]));
  strcpy(config.nama_pasien, doc["nama_pasien"]);
  config.kode_kamar = (char *)malloc(sizeof(char) * strlen(doc["kode_kamar"]));
  strcpy(config.kode_kamar, doc["kode_kamar"]);
  config.kode_bed = (char *)malloc(sizeof(char) * strlen(doc["kode_bed"]));
  strcpy(config.kode_bed, doc["kode_bed"]);
  config.koneksi = (char *)malloc(sizeof(char) * strlen(doc["koneksi"]));
  strcpy(config.koneksi, doc["koneksi"]);
  config.wifi_ssid = (char *)malloc(sizeof(char) * strlen(doc["wifi_ssid"]));
  strcpy(config.wifi_ssid, doc["wifi_ssid"]);
  config.wifi_pass = (char *)malloc(sizeof(char) * strlen(doc["wifi_pass"]));
  strcpy(config.wifi_pass, doc["wifi_pass"]);
  config.sim_ssid = (char *)malloc(sizeof(char) * strlen(doc["sim_ssid"]));
  strcpy(config.sim_ssid, doc["sim_ssid"]);
  config.sim_pass = (char *)malloc(sizeof(char) * strlen(doc["sim_pass"]));
  strcpy(config.sim_pass, doc["sim_pass"]);

  file.close();
}
//
// Write the configuration to a file
void write_config(fs::FS &fs, const char *filename, Config &config)
{
  // Delete existing file, otherwise the configuration is appended to the file
  fs.remove(filename);

  // Open file for writing
  File file = fs.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("Failed to create file"));
    state = error_state;
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<256> doc;

  doc["seri_infus"] = config.seri_infus;
  doc["nama_pasien"] = config.nama_pasien;
  doc["kode_kamar"] = config.kode_kamar;
  doc["kode_bed"] = config.kode_bed;
  doc["koneksi"] = config.koneksi;
  doc["wifi_ssid"] = config.wifi_ssid;
  doc["wifi_pass"] = config.wifi_pass;
  doc["sim_ssid"] = config.sim_ssid;
  doc["sim_pass"] = config.sim_pass;

  serializeJson(doc, file);

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("Failed to write to file"));
    state = error_state;
  }

  Serial.println("Config Saved");
  printconfig();
  // Close the file
  file.close();
}
