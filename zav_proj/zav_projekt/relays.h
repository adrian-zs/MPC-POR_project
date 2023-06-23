
#ifndef RELAYS_H_
#define RELAYS_H_

#include <stdbool.h>

void relayInit();
void relayControl(uint8_t relayNumber, bool action);
bool relayStatus(uint8_t relayNumber);

#endif /* RELAYS_H_ */