#ifndef CONTROLLER
#define CONTROLLER

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define controller_URI "http://avwlv2.sourceforge.net/plugins/avw/controller"
//#define LV2_MIDI__MidiEvent "http://lv2plug.in/ns/ext/midi#MidiEvent"

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
	CONTROLLER_OUTPUT,
} PortIndex;

#endif
