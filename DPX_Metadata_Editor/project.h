//-----------------------------------------------------------------------------
// This file is part of AEO-Light
//
// Copyright (c) 2016 University of South Carolina
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// AEO-Light is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Funding for AEO-Light development was provided through a grant from the
// National Endowment for the Humanities
//-----------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <string>
#include <vector>

#include <QTextStream>

#ifdef USEBOOST
#include <boost/format.hpp> // for printf-like formatting
#include <boost/timer/timer.hpp>
#endif

#include "FilmScan.h"


#define Warndlg(s) fprintf(stderr, s)
#define Errordlg(s) { fprintf(stderr, s); exit(1); }

enum SmoothingMethod {
	SMOOTHING_METHOD_MOVING_AVERAGE,
	SMOOTHING_METHOD_POLYNOMIAL_FIT,
	SMOOTHING_METHOD_NUM
};

enum RegionType {
	REGION_SOUNDTRACK_VARIABLE_DENSITY,
	REGION_SOUNDTRACK_VARIABLE_AREA,
	REGION_PERFORATION, // used for registration only
	REGION_IMAGE_STRIP, // used for registration only
	REGION_NUM
};

class FrameRegion {
private:
	RegionType rType;
	unsigned int left;
	unsigned int right;
public:
	FrameRegion() : rType(REGION_SOUNDTRACK_VARIABLE_DENSITY) {};
	FrameRegion(int l, int r) : rType(REGION_SOUNDTRACK_VARIABLE_DENSITY),
		left(l), right(r) {} ;
	FrameRegion(RegionType t, int l, int r) : rType(t), left(l), right(r) {} ;

	RegionType Type() const { return rType; }
	int Left() const { return left; }
	int Right() const { return right; }
	int Width() const { return right - left + 1; }
};

typedef std::vector< SoundSignal > SoundSignalArr;
typedef std::vector< SoundSignalArr > SoundSignalBatches;
typedef std::vector< SoundSignalBatches > MultiChannelSoundSignal;

typedef unsigned char ui8;
typedef std::vector< double > LampMask;

class FilmType {
private:
	std::string name;
	int frameOffset;

	// default is [830,980], except for 16mm, which is [139,225]
	FrameRegion soundtrackArea; // in permille (thousandths) of film width

public:
	FilmType(std::string n, int o) : name(n), frameOffset(o),
		soundtrackArea(830,980) {} ;
	FilmType(std::string n, int o, int l, int r) : name(n), frameOffset(o),
		soundtrackArea(l,r) {} ;
};

#define PRINT_VECTOR(f,s,v,sep) {\
	(f) << (s); \
	if((v).size() > 0) (f) << v[0]; \
	for(int i=1; i<(v).size(); ++i) (f) << (sep) << v[i]; \
	(f) << "\n"; \
}

class Project {
public:
	std::string filename; // file name of this project file
	bool isChanged;
	QTextStream *logger;

	FilmScan inFile;
	unsigned int firstFrameIndex; // = 0
	unsigned int lastFrameIndex; // = 0
	double framesPerSecond; // = 24
	double videoDuration; // audio may be shorter due to offset
	double overlapDuration; // = 0
	double audioDuration; // = 0

	std::vector<FrameRegion> soundBounds;

	std::string scratchDir[2];

	// Registration parameters
	bool doGuessRegistration; // = true
	unsigned int guessRegNumFrames; // = 400

	unsigned int overlapMaxPercentage; // default = 75
	unsigned int overlapSearchRadius; // default = 20;
	unsigned int overlapValidationRadius; // default = 5;
	unsigned int overlapThreshold; // default = 20
	int overlapTrack; // default = -1 = use all tracks
	int overlapErrorMetric; // default = 1, meaning L1
	                        // 2 = L2, -2 = L(1/2)
	                        // 4 = L4, -4 = L(1/4)

	bool cheatOverlap;
	bool doOverlapByImage;

	// ---------------------------
	// From "setUpFactoryPresets":
	// ---------------------------

	// TODO: flags for cleaning up directories

	// TODO: isAutomaticOn (auto determine batch size?)

	unsigned int framesPerBatch; // default = 1

	// Lamphouse calibration parameters
	bool doCalibrate; // default = true


public:
	Project();
	Project(std::string filename);
	~Project();

	void Initialize();

	bool SourceScan(std::string filename, SourceFormat fmt=SOURCE_UNKNOWN);


	bool Save(std::string filename);
	bool Save(void);

	// have changes been made since last save?
	bool IsChanged() const { return isChanged; }

	bool RenderSound(std::string filename);

private:

};

class Export {
private:
	double frameRate; // defaults to video frame rate or to 24 for frames
	double maxAmplitude; // default = 0.4
	unsigned int bitDepth; // default = 16
	bool doSmoothing; // default = false
	unsigned int smoothingHalfSpan; // = 2
	unsigned int width; // = 480
	unsigned int height; // = 360
	bool doPreserveAspectRatio; // = true
};


#endif
