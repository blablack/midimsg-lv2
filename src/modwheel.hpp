#ifndef MODWHEEL
#define MODWHEEL

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define modwheel_URI "http://github.com/blablack/midimsg.lv2/modwheel"

typedef struct
{
		LV2_URID midi_Event;
} modwheelURIs;

static inline void
map_modwheel_uris(LV2_URID_Map* map, modwheelURIs* uris)
{
	uris->midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
}

typedef enum
{
	CONTROLLER_INPUT = 0,

	CONTROLLER_LOGARITHMIC,

	CONTROLLER_MINIMUM,
	CONTROLLER_MAXIMUM,
	CONTROLLER_OUTPUT,
} PortIndex;

#endif
