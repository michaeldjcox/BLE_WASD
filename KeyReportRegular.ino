// Stores the parts of the USB HID key command

KeyReport keyReport;

/**
 * Updates the key report given a key press
 * 
 * Up to six keys can be simultaneously pressed plus the modifier keys
 * 
 */
void report_add(uint8_t hidKey) {
	uint8_t i;
		if (hidKey >= HID_LEFT_CTRL) {
			keyReport.modifiers |= 1 << (hidKey - HID_LEFT_CTRL);
		} else if (keyReport.keys[0] != hidKey && keyReport.keys[1] != hidKey
				&& keyReport.keys[2] != hidKey && keyReport.keys[3] != hidKey
				&& keyReport.keys[4] != hidKey && keyReport.keys[5] != hidKey) {
			for (i = 0; i < 6; ++i) {
				if (keyReport.keys[i] == 0) {
					keyReport.keys[i] = hidKey;
					break;
				}
			}
		}
}

/**
 * Updates the key report given a key release
 */
void report_remove(uint8_t hidKey) {
	uint8_t i;
	if (hidKey >= HID_LEFT_CTRL) {
		keyReport.modifiers &= ~(1 << (hidKey - HID_LEFT_CTRL));
	} else {
		for (i = 0; i < 6; ++i) {
			if (keyReport.keys[i] == hidKey) {
				keyReport.keys[i] = 0;
				break;
			}
		}
	}
}

/**
 * Test modifiers to see if shift is currently pressed
 */
bool is_shift() {
	return keyReport.modifiers == 0x02;
}

/**
 * Test modifiers to see if control is currently pressed
 */
bool is_control() {
	return keyReport.modifiers == 0x01;
}

/**
 * Test modifiers to see if control and shift are currently pressed
 */
bool is_control_shift() {
	return keyReport.modifiers == 0x03;
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_modifiers() {
	keyReport.modifiers = 0;
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_key_report() {
	keyReport.modifiers = 0x00;
	keyReport.keys[0] = 0x00;
	keyReport.keys[1] = 0x00;
	keyReport.keys[2] = 0x00;
	keyReport.keys[3] = 0x00;
	keyReport.keys[4] = 0x00;
	keyReport.keys[5] = 0x00;
}




