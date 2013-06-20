#include <math.h>
#include <string.h>
#include <iostream>

#include "lv2/lv2plug.in/ns/ext/state/state.h"
#include "lv2/lv2plug.in/ns/ext/log/log.h"
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "controller.hpp"

using namespace std;

typedef struct
{
	LV2_URID_Map* map;

	/* URIs */
	controllerURIs uris;
	LV2_Atom_Forge forge;

	char *bundle_path;

	LV2_Atom_Sequence* input_port;

	float* output_cv;
	float* output_control;

	const float* controllerNumber;
	const float* logarithmic;

	const float* minimum;
	const float* maximum;

	float lastOutput;
} controller;

//---------------------------------- INSANTIATE PLUGIN --------------------------------------------

static LV2_Handle instantiate(const LV2_Descriptor* descriptor,	double rate, const char* bundle_path, const LV2_Feature* const* features)
{
	controller* self = (controller*)malloc(sizeof(controller));

	self->lastOutput = 0;

	/* Get host features */
	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_URID__map))
			self->map = (LV2_URID_Map*)features[i]->data;
	}

	if (!self->map)
		cout << endl << "Missing feature urid:map." << endl;

	/* Map URIs and initialise forge */
	map_controller_uris(self->map, &self->uris);
	lv2_atom_forge_init(&self->forge, self->map);

	// store the bundle_path string to "self"
	self->bundle_path = (char*)malloc(strlen(bundle_path)+1);
	memcpy (self->bundle_path,bundle_path,strlen(bundle_path)+1);

	return (LV2_Handle)self;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	controller* self = (controller*)instance;

	switch ((PortIndex)port)
	{
	case CONTROLLER_INPUT:
		self->input_port = (LV2_Atom_Sequence*)data;
		break;

	case CONTROLLER_OUTPUT_CV:
		self->output_cv = (float*)data;
		break;
	case CONTROLLER_OUTPUT_CONTROL:
		self->output_control = (float*)data;
		break;

	case CONTROLLER_CONTROLLERNUMBER:
		self->controllerNumber = (float*)data;
		break;
	case CONTROLLER_LOGARITHMIC:
		self->logarithmic = (float*)data;
		break;
	case CONTROLLER_MINIMUM:
		self->minimum = (float*)data;
		break;
	case CONTROLLER_MAXIMUM:
		self->maximum = (float*)data;
		break;
	}
}

static void activate(LV2_Handle instance)
{
	controller* self = (controller*)instance;
}

static void deactivate(LV2_Handle instance)
{
}

static void cleanup(LV2_Handle instance)
{
	controller* self = (controller*)instance;

	free(self->bundle_path);

	delete(self);
}

static void run(LV2_Handle instance, uint32_t n_samples)
{
	controller* self = (controller*)instance;

	/* Read incoming events */
	LV2_ATOM_SEQUENCE_FOREACH(self->input_port, ev)
	{
		if (ev->body.type == (&self->uris)->midi_Event)
		{
			const uint8_t* buf = (const uint8_t*)LV2_ATOM_BODY(&ev->body);
			if (ev->body.size >= 3 && lv2_midi_message_type(buf) == LV2_MIDI_MSG_CONTROLLER)
				if(int(buf[1] == floor(*(self->controllerNumber))))
				{
					self->lastOutput = (int)buf[2];
					//cout << self->lastOutput << endl;
				}
		}
	}

	float scaled_value;

	float maximum = *(self->maximum);
	float minimum = *(self->minimum);

	if (*(self->logarithmic) > 0)
	{
		// haaaaack, stupid negatives and logarithms
		float log_offset = 0;
		if (minimum < 0)
			log_offset = fabs(minimum);
		const float min = log(minimum + 1 + log_offset);
		const float max = log(maximum + 1 + log_offset);
		scaled_value = expf((float)self->lastOutput/127 * (max - min) + min) - 1 - log_offset;
	}
	else
		scaled_value = ((float)self->lastOutput/127 * (maximum - minimum)) + minimum;

	for (uint32_t s = 0; s < n_samples; s++)
		self->output_cv[s] = scaled_value;

	*self->output_control = scaled_value;
}

const void* extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
		controller_URI,
		instantiate,
		connect_port,
		activate,
		run,
		deactivate,
		cleanup,
		extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	switch (index)
	{
	case 0:
		return &descriptor;
	default:
		return NULL;
	}
}
