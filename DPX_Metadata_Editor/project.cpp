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

// Project (C++ class)
// The main object: it holds all the parameters about the sound reconstruction
// The main function is "RenderSound()", below.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <deque>
#include <map>
#include <utility>
#include <algorithm>
#include <numeric>
//#include <climits>
#include <limits>
#include <cstdlib>     // srand, rand
#include <ctime>       // time

#include <cassert>

//#include <boost/typeof/typeof.hpp>

#include "project.h"



template <typename T>
std::string NumToStr(T num)
{
	std::ostringstream ss;
	ss << num;
	return ss.str();
}

std::vector< double > HalfHanning(unsigned int m);

//-----------------------------------------------------------------------------
Project::Project()
{
	this->Initialize();
}

//-----------------------------------------------------------------------------
Project::Project(std::string filename)
{
	this->Initialize();
	// XXX Read aeo project file
}

//-----------------------------------------------------------------------------
bool Project::SourceScan(std::string filename, SourceFormat fmt)
{
	if( this->inFile.Source(filename, fmt) == false)
		return false;

	this->firstFrameIndex = this->inFile.FirstFrame();
	this->lastFrameIndex = this->inFile.LastFrame();

	return true;
}

//-----------------------------------------------------------------------------
Project::~Project()
{
}

//-----------------------------------------------------------------------------
void Project::Initialize()
{
	// filename = default (empty)

	isChanged = true;

	// inFile = default

	firstFrameIndex = 0;
	lastFrameIndex = 0;
	framesPerSecond = 24;
	videoDuration = 0;
	overlapDuration = 0;
	audioDuration = 0;

	// soundBounds = default (empty array)


}


//-----------------------------------------------------------------------------
bool Project::Save(std::string filename)
{
	// save the stuff
	this->filename = filename;
	this->isChanged = false;
	return true;
}

//-----------------------------------------------------------------------------
bool Project::Save(void)
{
	if(!this->filename.empty())
	{
		// XXX: Query for filename
		this->filename = "test.aeo";

		if(!this->filename.empty()) return false;
	}
	return Save(this->filename);
}

