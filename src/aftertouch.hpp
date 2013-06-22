#ifndef AFTERTOUCH
#define AFTERTOUCH

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define aftertouch_URI "http://github.com/blablack/midimsg.lv2/aftertouch"

typedef struct
{
	LV2_URID midi_Event;
} aftertouchURIs;

static inline void map_aftertouch_uris(LV2_URID_Map* map, aftertouchURIs* uris)
{
	uris->midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
}

typedef enum
{
	AFTERTOUCH_INPUT = 0,

	AFTERTOUCH_LOGARITHMIC,

	AFTERTOUCH_MINIMUM,
	AFTERTOUCH_MAXIMUM,
	AFTERTOUCH_OUTPUT_CV,
	AFTERTOUCH_OUTPUT_CONTROL,
} PortIndex;

typedef struct
{
	LV2_URID_Map* map;

	/* URIs */
	aftertouchURIs uris;
	LV2_Atom_Forge forge;

	char *bundle_path;

	LV2_Atom_Sequence* input_port;

	float* output_cv;
	float* output_control;

	const float* logarithmic;

	const float* minimum;
	const float* maximum;

	float lastOutput;
	float lastScaledValue;
} aftertouch;

#endif
