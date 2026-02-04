#ifndef GUSPATCH_H
#define GUSPATCH_H

extern "C" {
#include <tcl.h>
}
#include <iostream.h>
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif
class GusHeader {
	friend ostream &operator<<(ostream &os, const GusHeader &gph);
public:
	GusHeader() {}
	GusHeader(const GusHeader &h);

	GusHeader *Dup(void) const;

	int Read(int fd, ostrstream &error);
#if defined(TCL_7_5)
	int Read(Tcl_Channel channel, ostrstream &error);
#endif

	const char *GetHeader() const {return (header);}
	const char *GetID() const {return (gravis_id);}
	const char *GetDescription() const {return (description);}
	unsigned char GetInstruments() const {return (instruments);}
	char GetVoices() const {return (voices);}
	char GetChannels() const {return (channels);}
	unsigned short GetWaveForms() const {return (wave_forms);}
	unsigned short GetMasterVolume() const {return (master_volume);}
	unsigned long GetDataSize() const {return (data_size);}

	GusHeader &operator=(const GusHeader &h);
private:
	char header[13];	// +1 for '\0'
	char gravis_id[11];	// +1 for '\0'
	char description[61];	// +1 for '\0'
	unsigned char instruments;
	char voices;
	char channels;
	unsigned short wave_forms;
	unsigned short master_volume;
	unsigned long data_size;
	char reserved[36];
};


class GusLayer {
	friend ostream &operator<<(ostream &os, const GusLayer &gl);
public:
	GusLayer() {}
	GusLayer(const GusLayer &l);

	GusLayer *Dup(void) const;

	int Read(int fd, ostrstream &error);
#if defined(TCL_7_5)
	int Read(Tcl_Channel channel, ostrstream &error);
#endif
	char GetLayerDuplicate() const {return (layer_duplicate);}
	char GetLayer() const {return (layer);}
	long GetLayerSize() const {return (layer_size);}
	char GetSamples() const {return (samples);}

	GusLayer &operator=(const GusLayer &l);
private:
	char layer_duplicate;
	char layer;
	long layer_size;
	char samples;
	char reserved[40];
};

class GusInstrument {
	friend ostream &operator<<(ostream &os, const GusInstrument &gi);
public:
	GusInstrument() : gus_layers(0) {}
	GusInstrument(const GusInstrument &i);
	~GusInstrument() {delete [] gus_layers;}

	GusInstrument *Dup(void) const;

	int Read(int fd, ostrstream &error);
#if defined(TCL_7_5)
	int Read(Tcl_Channel channel, ostrstream &error);
#endif

	unsigned short GetInstrument() const {return (instrument);}
	const char *GetInstrumentName() const {return (instrument_name);}
	long GetInstrumentSize() const {return (instrument_size);}
	char GetLayers() const {return (layers);}

	GusInstrument &operator=(const GusInstrument &i);
private:
	unsigned short instrument;
	char instrument_name[17];	// +1 for '\0'
	long instrument_size;
	char layers;
	char reserved[40];

	GusLayer *gus_layers;
};

/*
 * Wave mode bit masks
 */
const int GUS_WAVE_16_BIT = (1 << 0);
const int GUS_WAVE_UNSIGNED = (1 << 1);
const int GUS_WAVE_LOOPING = (1 << 2);
const int GUS_WAVE_BIDIR = (1 << 3);
const int GUS_WAVE_REVERSE = (1 << 4);
const int GUS_WAVE_SUSTAINING = (1 << 5);
const int GUS_WAVE_ENVELOPE = (1 << 6);


class GusWave {
	friend ostream &operator<<(ostream &os, const GusWave &gw);
public:
	GusWave() : wave_data(0) {}
	GusWave(const GusWave &w);
	~GusWave() {delete [] wave_data;}

	GusWave *Dup(void) const;

	int Read(int fd, ostrstream &error);
#if defined(TCL_7_5)
	int Read(Tcl_Channel channel, ostrstream &error);
#endif

	const char *GetWaveName() const {return (wave_name);}
	unsigned char GetFractions() const {return (fractions);}
	long GetWaveSize() const {return (wave_size);}
	long GetStartLoop() const {return (start_loop);}
	long GetEndLoop() const {return (end_loop);}
	unsigned short GetSampleRate() const {return (sample_rate);}
	long GetLowFrequency() const {return (low_frequency);}
	long GetHighFrequency() const {return (high_frequency);}
	long GetRootFrequency() const {return (root_frequency);}
	short GetTune() const {return (tune);}
	unsigned char GetBalance() const {return (balance);}
	const unsigned char *GetEnvelopeRate() const {return (envelope_rate);}
	const unsigned char *GetEnvelopeOffset() const {return (envelope_offset);}
	unsigned char GetTremoloSweep() const {return (tremolo_sweep);}
	unsigned char GetTremoloRate() const {return (tremolo_rate);}
	unsigned char GetTremoloDepth() const {return (tremolo_depth);}
	unsigned char GetVibratoSweep() const {return (vibrato_sweep);}
	unsigned char GetVibratoRate() const {return (vibrato_rate);}
	unsigned char GetVibratoDepth() const {return (vibrato_depth);}
	char GetModes() const {return (modes);}
	unsigned short GetScaleFrequency() const {return (scale_frequency);}
	unsigned short GetScaleFactor() const {return (scale_factor);}

	const unsigned char *GetWaveData() const {return (wave_data);}

	GusWave &operator=(const GusWave &w);

private:
	char wave_name[8];	// +1 for '\0'
	unsigned char fractions;
	long wave_size;
	long start_loop;
	long end_loop;
	unsigned short sample_rate;
	long low_frequency;
	long high_frequency;
	long root_frequency;
	short tune;
	unsigned char balance;
	unsigned char envelope_rate[6];
	unsigned char envelope_offset[6];
	unsigned char tremolo_sweep;
	unsigned char tremolo_rate;
	unsigned char tremolo_depth;
	unsigned char vibrato_sweep;
	unsigned char vibrato_rate;
	unsigned char vibrato_depth;
	char modes;
	short scale_frequency;
	unsigned short scale_factor;
	char reserved[36];

	unsigned char *wave_data;
};

class GusPatchFile {
	friend ostream &operator<<(ostream &os, const GusPatchFile &gpf);
public:
	GusPatchFile() : instruments(0), waves(0) {}
	GusPatchFile(const GusPatchFile &p);
	~GusPatchFile() {
		delete [] instruments;
		delete [] waves;
	}

	GusPatchFile *Dup(void) const;

	int Read(int fd, ostrstream &error);
#if defined(TCL_7_5)
	int Read(Tcl_Channel channel, ostrstream &error);
#endif

	const GusHeader *GetHeader(void) const {return (&header);}
	int GetNumInstruments(void) const {return (num_instruments);}
	const GusInstrument *GetInstruments(void) const {return (instruments);}
	int GetNumWaves(void) const {return (num_waves);}
	const GusWave *GetWaves(void) const {return (waves);}

	GusPatchFile &operator=(const GusPatchFile &p);
private:
	GusHeader header;
	int num_instruments;
	GusInstrument *instruments;
	int num_waves;
	GusWave *waves;
};
#endif
