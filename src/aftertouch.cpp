#include <math.h>
#include <string.h>
#include <iostream>

#include "lv2/lv2plug.in/ns/ext/state/state.h"
#include "lv2/lv2plug.in/ns/ext/log/log.h"
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "aftertouch.hpp"

using namespace std;


static LV2_Handle instantiate(const LV2_Descriptor* descriptor,	double rate, const char* bundle_path, const LV2_Feature* const* features)
{
	aftertouch* self = (aftertouch*)malloc(sizeof(aftertouch));

	self->lastOutput = 0.0;
	self->lastScaledValue = 0.0;

	/* Get host features */
	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_URID__map))
			self->map = (LV2_URID_Map*)features[i]->data;
	}

	if (!self->map)
		cout << endl << "Missing feature urid:map." << endl;

	/* Map URIs and initialise forge */
	map_aftertouch_uris(self->map, &self->uris);
	lv2_atom_forge_init(&self->forge, self->map);

	// store the bundle_path string to "self"
	self->bundle_path = (char*)malloc(strlen(bundle_path)+1);
	memcpy (self->bundle_path,bundle_path,strlen(bundle_path)+1);

	return (LV2_Handle)self;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	aftertouch* self = (aftertouch*)instance;

	switch ((PortIndex)port)
	{
	case AFTERTOUCH_INPUT:
		self->input_port = (LV2_Atom_Sequence*)data;
		break;

	case AFTERTOUCH_OUTPUT_CV:
		self->output_cv = (float*)data;
		break;
	case AFTERTOUCH_OUTPUT_CONTROL:
		self->output_control = (float*)data;
		break;

	case AFTERTOUCH_LOGARITHMIC:
		self->logarithmic = (float*)data;
		break;
	case AFTERTOUCH_MINIMUM:
		self->minimum = (float*)data;
		break;
	case AFTERTOUCH_MAXIMUM:
		self->maximum = (float*)data;
		break;
	}
}

static void activate(LV2_Handle instance)
{
	aftertouch* self = (aftertouch*)instance;
}

static void deactivate(LV2_Handle instance)
{
}

static void cleanup(LV2_Handle instance)
{
	aftertouch* self = (aftertouch*)instance;

	free(self->bundle_path);

	delete(self);
}

static void run(LV2_Handle instance, uint32_t n_samples)
{
	aftertouch* self = (aftertouch*)instance;

	bool p_eventOccured = false;
	/* Read incoming events */
	LV2_ATOM_SEQUENCE_FOREACH(self->input_port, ev)
	{
		if (ev->body.type == (&self->uris)->midi_Event)
		{
			const uint8_t* buf = (const uint8_t*)LV2_ATOM_BODY(&ev->body);
			//cout<<ev->body.size<<" - " << lv2_midi_message_type(buf) << " - " << LV2_MIDI_MSG_CHANNEL_PRESSURE << endl;
			if (ev->body.size >= 2 && lv2_midi_message_type(buf) == LV2_MIDI_MSG_CHANNEL_PRESSURE)
			{
				//cout << (int)buf[2] << " - " << (int)buf[1] << endl;
				self->lastOutput = (int)buf[1];
				p_eventOccured = true;
			}
		}
	}

	if(p_eventOccured)
	{
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
			self->lastScaledValue = expf((float)self->lastOutput/127 * (max - min) + min) - 1 - log_offset;
		}
		else
			self->lastScaledValue = ((float)self->lastOutput/127 * (maximum - minimum)) + minimum;
	}

	for (uint32_t s = 0; s < n_samples; s++)
		self->output_cv[s] = self->lastScaledValue;

	*self->output_control = self->lastScaledValue;

}

const void* extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
		aftertouch_URI,
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
