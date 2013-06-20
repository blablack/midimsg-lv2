#ifndef CONTROLLER
#define CONTROLLER

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define controller_URI "http://github.com/blablack/midimsg.lv2/controller"

typedef struct
{
		LV2_URID midi_Event;
} controllerURIs;

static inline void
map_controller_uris(LV2_URID_Map* map, controllerURIs* uris)
{
	uris->midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
}

typedef enum
{
	CONTROLLER_INPUT = 0,

	CONTROLLER_CONTROLLERNUMBER,
	CONTROLLER_LOGARITHMIC,

	CONTROLLER_MINIMUM,
	CONTROLLER_MAXIMUM,
	CONTROLLER_OUTPUT_CV,
	CONTROLLER_OUTPUT_CONTROL,
} PortIndex;

#endif
