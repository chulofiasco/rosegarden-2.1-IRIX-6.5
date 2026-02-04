#include <stdlib.h>
#include <errno.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#else
#ifdef _MSC_VER
#include <io.h>
#endif
#endif
#include <iomanip.h>
#include <assert.h>

#include "GusPatch.h"

GusHeader::GusHeader(const GusHeader &h)
{

	memcpy(this, &h, sizeof(GusHeader));
}

GusHeader *
GusHeader::Dup(void) const
{
	GusHeader *h;


	h = new GusHeader(*this);
	return (h);
}

int
GusHeader::Read(int fd, ostrstream &error)
{
	unsigned char block[129];

	if (read(fd, block, 129) != 129) {
		error << "Couldn't read patch header: " << strerror(errno)
		    << ends;
		return (0);
	}
	memcpy(header, &block[0], 12);
	header[12] = '\0';
	memcpy(gravis_id, &block[12], 10);
	gravis_id[10] = '\0';
	memcpy(description, &block[22], 60);
	description[60] = '\0';
	memcpy(&instruments, &block[82], 1);
	memcpy(&voices, &block[83], 1);
	memcpy(&channels, &block[84], 1);
	memcpy(&wave_forms, &block[85], 2);
	memcpy(&master_volume, &block[87], 2);
	memcpy(&data_size, &block[89], 4);
	memcpy(&reserved, &block[93], 36);
	return (1);
}

int
GusHeader::Read(Tcl_Channel channel, ostrstream &error)
{
	char block[129];

	if (Tcl_Read(channel, block, 129) == -1) {
		error << "Couldn't read patch header: " << strerror(errno)
		    << ends;
		return (0);
	}
	memcpy(header, &block[0], 12);
	header[12] = '\0';
	memcpy(gravis_id, &block[12], 10);
	gravis_id[10] = '\0';
	memcpy(description, &block[22], 60);
	description[60] = '\0';
	memcpy(&instruments, &block[82], 1);
	memcpy(&voices, &block[83], 1);
	memcpy(&channels, &block[84], 1);
	memcpy(&wave_forms, &block[85], 2);
	memcpy(&master_volume, &block[87], 2);
	memcpy(&data_size, &block[89], 4);
	memcpy(reserved, &block[93], 36);
	return (1);
}

GusHeader &
GusHeader::operator=(const GusHeader &h)
{

	memcpy(this, &h, sizeof(GusHeader));
	return (*this);
}

ostream &
operator<<(ostream &os, const GusHeader &gh)
{

	os << "Header: " << gh.header << "\n"
	    << "Gravis ID: " << gh.gravis_id << "\n"
	    << "Description: " << gh.description << "\n"
	    << "Instruments: " << (int)gh.instruments << "\n"
	    << "Voices: " << (int)gh.voices << "\n"
	    << "Channels: " << (int)gh.channels << "\n"
	    << "Wave Forms: " << gh.wave_forms << "\n"
	    << "Master Volume: " << gh.master_volume << "\n"
	    << "Data Size: " << gh.data_size << endl;
	return (os);
}

GusLayer::GusLayer(const GusLayer &l)
{

	memcpy(this, &l, sizeof(GusLayer));
}

GusLayer *
GusLayer::Dup(void) const
{
	GusLayer *l;

	l = new GusLayer(*this);
	return (l);
}

int
GusLayer::Read(int fd, ostrstream &error)
{
	unsigned char block[47];

	if (read(fd, block, 47) != 47) {
		error << "Couldn't read layer: " << strerror(errno) << ends;
		return (0);
	}
	memcpy(&layer_duplicate, &block[0], 1);
	memcpy(&layer, &block[1], 1);
	memcpy(&layer_size, &block[2], 4);
	memcpy(&samples, &block[6], 1);
	memcpy(reserved, &block[7], 40);
	return (1);
}

int
GusLayer::Read(Tcl_Channel channel, ostrstream &error)
{
	char block[47];

	if (Tcl_Read(channel, block, 47) == -1) {
		error << "Couldn't read layer: " << strerror(errno) << ends;
		return (0);
	}
	memcpy(&layer_duplicate, &block[0], 1);
	memcpy(&layer, &block[1], 1);
	memcpy(&layer_size, &block[2], 4);
	memcpy(&samples, &block[6], 1);
	memcpy(reserved, &block[7], 40);
	return (1);
}

GusLayer &
GusLayer::operator=(const GusLayer &l)
{

	memcpy(this, &l, sizeof(GusLayer));
	return (*this);
}

ostream &
operator<<(ostream &os, const GusLayer &gl)
{

	os << "Layer Duplicate: " << (int)gl.layer_duplicate << "\n"
	    << "Layer: " << (int)gl.layer << "\n"
	    << "Layer Size: " << gl.layer_size << "\n"
	    << "Samples: " << (int)gl.samples << endl;
	return (os);
}

GusInstrument::GusInstrument(const GusInstrument &i)
{
	int j;

	memcpy(this, &i, sizeof(GusInstrument));
	gus_layers = new GusLayer[layers];
	assert(gus_layers != 0);
	for (j = 0; j < layers; j++)
		gus_layers[j] = i.gus_layers[j];
}

GusInstrument *
GusInstrument::Dup(void) const
{
	GusInstrument *i;

	i = new GusInstrument(*this);
	return (i);
}

int
GusInstrument::Read(int fd, ostrstream &error)
{
	char block[63];
	int i;

	delete [] gus_layers;

	if (read(fd, block, 63) != 63) {
		error << "Couldn't read instrument: " << strerror(errno)
		    << ends;
		return (0);
	}
	memcpy(&instrument, &block[0], 2);
	memcpy(instrument_name, &block[2], 16);
	instrument_name[16] = '\0';
	memcpy(&instrument_size, &block[18], 4);
	memcpy(&layers, &block[22], 1);
	memcpy(reserved, &block[23], 40);

	gus_layers = new GusLayer [layers];
	if (gus_layers == 0) {
		error << "Out of memory" << ends;
		return (0);
	}
	for (i = 0; i < layers; i++) {
		if (!gus_layers[i].Read(fd, error)) {
			delete [] gus_layers;
			gus_layers = 0;
			return (0);
		}
	}
	return (1);
}

int
GusInstrument::Read(Tcl_Channel channel, ostrstream &error)
{
	char block[63];
	int i;

	delete [] gus_layers;

	if (Tcl_Read(channel, block, 63) == -1) {
		error << "Couldn't read instrument: " << strerror(errno)
		    << ends;
		return (0);
	}
	memcpy(&instrument, &block[0], 2);
	memcpy(instrument_name, &block[2], 16);
	instrument_name[16] = '\0';
	memcpy(&instrument_size, &block[18], 4);
	memcpy(&layers, &block[22], 1);
	memcpy(reserved, &block[23], 40);

	gus_layers = new GusLayer [layers];
	if (gus_layers == 0) {
		error << "Out of memory" << ends;
		return (0);
	}
	for (i = 0; i < layers; i++) {
		if (!gus_layers[i].Read(channel, error)) {
			delete [] gus_layers;
			gus_layers = 0;
			return (0);
		}
	}
	return (1);
}

GusInstrument &
GusInstrument::operator=(const GusInstrument &i)
{
	int j;

	delete [] gus_layers;

	memcpy(this, &i, sizeof(GusInstrument));
	gus_layers = new GusLayer[layers];
	assert(gus_layers != 0);
	for (j = 0; j < layers; j++)
		gus_layers[j] = i.gus_layers[j];
	return (*this);
}

ostream &
operator<<(ostream &os, const GusInstrument &gi)
{
	int i;

	os << "Instrument: " << gi.instrument << "\n"
	    << "Instrument Name: " << gi.instrument_name << "\n"
	    << "Instrument Size: " << gi.instrument_size << "\n"
	    << "Layers: " << (int)gi.layers << endl;
	for (i = 0; i < gi.layers; i++)
		os << gi.gus_layers[i] << endl;
	return (os);
}

GusWave::GusWave(const GusWave &w)
{

	memcpy(this, &w, sizeof(GusWave));
	wave_data = new unsigned char[wave_size];
	assert(wave_data != 0);
	memcpy(wave_data, w.wave_data, wave_size);
}

GusWave *
GusWave::Dup(void) const
{
	GusWave *w;

	w = new GusWave(*this);
	return (w);
}

int
GusWave::Read(int fd, ostrstream &error)
{
	char block[96];

	delete wave_data;

	if (read(fd, block, 96) != 96) {
		error << "Couldn't read wave: " << strerror(errno) << ends;
		return (0);
	}
	memcpy(wave_name, &block[0], 7);
	wave_name[7] = '\0';
	memcpy(&fractions, &block[7], 1);
	memcpy(&wave_size, &block[8], 4);
	memcpy(&start_loop, &block[12], 4);
	memcpy(&end_loop, &block[16], 4);
	memcpy(&sample_rate, &block[20], 2);
	memcpy(&low_frequency, &block[22], 4);
	memcpy(&high_frequency, &block[26], 4);
	memcpy(&root_frequency, &block[30], 4);
	memcpy(&tune, &block[34], 2);
	memcpy(&balance, &block[36], 1);
	memcpy(envelope_rate, &block[37], 6);
	memcpy(envelope_offset, &block[43], 6);
	memcpy(&tremolo_sweep, &block[49], 1);
	memcpy(&tremolo_rate, &block[50], 1);
	memcpy(&tremolo_depth, &block[51], 1);
	memcpy(&vibrato_sweep, &block[52], 1);
	memcpy(&vibrato_rate, &block[53], 1);
	memcpy(&vibrato_depth, &block[54], 1);
	memcpy(&modes, &block[55], 1);
	memcpy(&scale_frequency, &block[56], 2);
	memcpy(&scale_factor, &block[58], 2);
	memcpy(reserved, &block[60], 36);

	wave_data = new unsigned char [wave_size];
	if (wave_data == 0) {
		error << "Out of memory" << ends;
		return (0);
	}

	if (read(fd, wave_data, wave_size) != wave_size) {
		error << "Couldn't read wave data: " << strerror(errno)
		    << ends;
		delete wave_data;
		return (0);
	}
	return (1);
}

int
GusWave::Read(Tcl_Channel channel, ostrstream &error)
{
	char block[96];

	delete wave_data;

	if (Tcl_Read(channel, block, 96) == -1) {
		error << "Couldn't read wave: " << strerror(errno) << ends;
		return (0);
	}
	memcpy(wave_name, &block[0], 7);
	wave_name[7] = '\0';
	memcpy(&fractions, &block[7], 1);
	memcpy(&wave_size, &block[8], 4);
	memcpy(&start_loop, &block[12], 4);
	memcpy(&end_loop, &block[16], 4);
	memcpy(&sample_rate, &block[20], 2);
	memcpy(&low_frequency, &block[22], 4);
	memcpy(&high_frequency, &block[26], 4);
	memcpy(&root_frequency, &block[30], 4);
	memcpy(&tune, &block[34], 2);
	memcpy(&balance, &block[36], 1);
	memcpy(envelope_rate, &block[37], 6);
	memcpy(envelope_offset, &block[43], 6);
	memcpy(&tremolo_sweep, &block[49], 1);
	memcpy(&tremolo_rate, &block[50], 1);
	memcpy(&tremolo_depth, &block[51], 1);
	memcpy(&vibrato_sweep, &block[52], 1);
	memcpy(&vibrato_rate, &block[53], 1);
	memcpy(&vibrato_depth, &block[54], 1);
	memcpy(&modes, &block[55], 1);
	memcpy(&scale_frequency, &block[56], 2);
	memcpy(&scale_factor, &block[58], 2);
	memcpy(reserved, &block[60], 36);

	wave_data = new unsigned char [wave_size];
	if (wave_data == 0) {
		error << "Out of memory";
		return (0);
	}

	if (Tcl_Read(channel, (char *)wave_data, wave_size) == -1) {
		error << "Couldn't read wave data: " << strerror(errno)
		    << ends;
		delete wave_data;
		return (0);
	}
	return (1);
}

GusWave &
GusWave::operator=(const GusWave &w)
{

	delete wave_data;

	memcpy(this, &w, sizeof(GusWave));
	wave_data = new unsigned char[wave_size];
	assert(wave_data != 0);
	memcpy(wave_data, w.wave_data, wave_size);
	return (*this);
}

ostream &
operator<<(ostream &os, const GusWave &gw)
{
	int i, j;

	os << "Wave Name: " << gw.wave_name << "\n"
	    << "Fractions: " << (int)gw.fractions << "\n"
	    << "Wave Size: " << gw.wave_size << "\n"
	    << "Start Loop: " << gw.start_loop << "\n"
	    << "End Loop: " << gw.end_loop << "\n"
	    << "Sample Rate: " << gw.sample_rate << "\n"
	    << "Low Frequency: " << gw.low_frequency << "\n"
	    << "High Frequency: " << gw.high_frequency << "\n"
	    << "Root Frequency: " << gw.root_frequency << "\n"
	    << "Tune: " << gw.tune << "\n"
	    << "Balance: " << (int)gw.balance << "\n"
	    << "Envelope Rate:";
	for (i = 0; i < 6; i++)
		os << " " << (int)gw.envelope_rate[i];
	os << "\n";
	os << "Envelope Offset:";
	for (i = 0; i < 6; i++)
		os << " " << (int)gw.envelope_offset[i];
	os << "\n";
	os << "Tremolo Sweep: " << (int)gw.tremolo_sweep << "\n"
	    << "Tremolo Rate: " << (int)gw.tremolo_rate << "\n"
	    << "Tremolo Depth: " << (int)gw.tremolo_depth << "\n"
	    << "Vibrato Sweep: " << (int)gw.vibrato_sweep << "\n"
	    << "Vibrato Rate: " << (int)gw.vibrato_rate << "\n"
	    << "Vibrato Depth: " << (int)gw.vibrato_depth << "\n"
	    << "Modes: 0x" << hex << setw(2) << setfill('0') <<
	    (int)gw.modes << dec << "\n"
	    << "Scale Frequency: " << gw.scale_frequency << "\n"
	    << "Scale Factor: " << gw.scale_factor << "\n";
	for (i = 0; i < gw.wave_size; i += 8) {
		for (j = 0; j < 8; j++)
			os << hex << setw(2) << setfill('0')
			    << (int)gw.wave_data[i + j] << dec << " ";
		os << "\n";
	}
	os << endl;

	return (os);
}

GusPatchFile::GusPatchFile(const GusPatchFile &p)
{
	int i;

	memcpy(this, &p, sizeof(GusPatchFile));
	instruments = new GusInstrument[num_instruments];
	assert(instruments != 0);
	for (i = 0; i < num_instruments; i++)
		instruments[i] = p.instruments[i];
	waves = new GusWave[num_instruments];
	assert(waves != 0);
	for (i = 0; i < num_instruments; i++)
		waves[i] = p.waves[i];
}

GusPatchFile *
GusPatchFile::Dup(void) const
{
	GusPatchFile *p;

	p = new GusPatchFile(*this);
	return (p);
}

int
GusPatchFile::Read(int fd, ostrstream &error)
{
	int i;

	delete [] instruments;
	delete [] waves;

	if (!header.Read(fd, error))
		return (0);
	num_instruments = header.GetInstruments();
	instruments = new GusInstrument [num_instruments];
	if (instruments == 0) {
		error << "Out of memory" << ends;
		return (0);
	}
	for (i = 0; i < num_instruments; i++) {
		if (!instruments[i].Read(fd, error)) {
			delete [] instruments;
			instruments = 0;
			return (0);
		}
	}

	num_waves = header.GetWaveForms();
	waves = new GusWave [num_waves];
	if (waves == 0) {
		error << "Out of memory" << ends;
		delete [] instruments;
		instruments = 0;
		return (0);
	}
	for (i = 0; i < num_waves; i++) {
		if (!waves[i].Read(fd, error)) {
			delete [] instruments;
			instruments = 0;
			delete [] waves;
			waves = 0;
			return (0);
		}
	}

	return (1);
}

int
GusPatchFile::Read(Tcl_Channel channel, ostrstream &error)
{
	int i;

	delete [] instruments;
	delete [] waves;

	if (!header.Read(channel, error))
		return (0);
	num_instruments = header.GetInstruments();
	instruments = new GusInstrument [num_instruments];
	if (instruments == 0) {
		error << "Out of memory" << ends;
		return (0);
	}
	for (i = 0; i < num_instruments; i++) {
		if (!instruments[i].Read(channel, error)) {
			delete [] instruments;
			instruments = 0;
			return (0);
		}
	}

	num_waves = header.GetWaveForms();
	waves = new GusWave [num_waves];
	if (waves == 0) {
		error << "Out of memory" << ends;
		delete [] instruments;
		instruments = 0;
		return (0);
	}
	for (i = 0; i < num_waves; i++) {
		if (!waves[i].Read(channel, error)) {
			delete [] instruments;
			instruments = 0;
			delete [] waves;
			waves = 0;
			return (0);
		}
	}

	return (1);
}

GusPatchFile &
GusPatchFile::operator=(const GusPatchFile &p)
{
	int i;

	memcpy(this, &p, sizeof(GusPatchFile));
	instruments = new GusInstrument[num_instruments];
	assert(instruments != 0);
	for (i = 0; i < num_instruments; i++)
		instruments[i] = p.instruments[i];
	waves = new GusWave[num_instruments];
	assert(waves != 0);
	for (i = 0; i < num_instruments; i++)
		waves[i] = p.waves[i];
	return (*this);
}

ostream &
operator<<(ostream &os, const GusPatchFile &gpf)
{
	int i;

	os << "Header: " << gpf.header << "\n";
	for (i = 0; i < gpf.num_instruments; i++)
		os << "Instrument: " << gpf.instruments[i] << "\n";
	for (i = 0; i < gpf.num_waves; i++)
		os << "Wave: " << gpf.waves[i] << "\n";
	os << endl;
	return(os);
}
