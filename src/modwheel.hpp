#ifndef MODWHEEL
#define MODWHEEL

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define modwheel_URI "http://github.com/blablack/midimsg-lv2/modwheel"

typedef struct
{
	LV2_URID midi_Event;
} modwheelURIs;

static inline void map_modwheel_uris(LV2_URID_Map* map, modwheelURIs* uris)
{
	uris->midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
}

typedef enum
{
	MODWHEEL_INPUT = 0,

	MODWHEEL_LOGARITHMIC,

	MODWHEEL_MINIMUM,
	MODWHEEL_MAXIMUM,
	MODWHEEL_OUTPUT_CV,
	MODWHEEL_OUTPUT_CONTROL,
} PortIndex;

typedef struct
{
	LV2_URID_Map* map;

	/* URIs */
	modwheelURIs uris;
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
} modwheel;

#endif
