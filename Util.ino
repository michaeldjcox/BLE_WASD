
/**
 * Returns a string representation of a key report
 */
String report_to_string(KeyReport report) {
  String cmd = hex_to_str(report.modifiers) + "-"
               + hex_to_str(report.reserved) + "-" + hex_to_str(report.keys[0])
               + "-" + hex_to_str(report.keys[1]) + "-"
               + hex_to_str(report.keys[2]) + "-" + hex_to_str(report.keys[3])
               + "-" + hex_to_str(report.keys[4]) + "-"
               + hex_to_str(report.keys[5]);
  return cmd;
}

/**
 * Create a two character hex string for debugging
 */
String hex_to_str(uint8_t hidKey) {
  String str = String(hidKey, HEX);
  if (hidKey < 16) {
    str = "0" + str;
  }
  str.toUpperCase();
  return str;
}

/**
 * Outputs a debug string for the current key report
 */
void log_key_report(KeyReport report) {
  if (DEBUG) {
    String cmd = "0xFD-" + report_to_string(report);
    Serial.println(cmd);
  }
}
