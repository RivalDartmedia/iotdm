#ifndef koneksi_cred_H
#define koneksi_cred_H

  // String server_dom = "https://sgp1.blynk.cloud/external/api/"; // Server URL
  // String send_p = "batch/update?";
  // String get_p = "get?";
  // String token = "token=";
  // String berat_v = "&v0=";
  // String tpm_v = "&v1=";
  // String blink_v = "&v3";
  // send_message = server_dom + send_p + token + tokenid + berat_v + String(weigh) + tpm_v + String(tpm);

  String server_dom = "http://api.callmebot.com/"; // Server URL
  String send_p = "telegram/";
  String get_p = "group.php?";
  String token = "apikey=";
  String berat_v = "&v0=";
  String tpm_v = "&v1=";
  String blink_v = "&v3";

// http://api.callmebot.com/telegram/group.php?apikey=LTk2NTgzMjA5NA&text=Hello World no ssl
#endif // !koneksi_cred_H