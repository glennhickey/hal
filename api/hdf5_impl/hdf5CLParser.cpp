/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include <cassert>
#include <iostream>
#include <cstdlib>
#include <deque>
#include "hdf5CLParser.h"

using namespace hal;
using namespace std;
using namespace H5;

const hsize_t HDF5CLParser::DefaultChunkSize = 3500;
const hsize_t HDF5CLParser::DefaultDeflate = 2;
const hsize_t HDF5CLParser::DefaultCacheMDCElems = 113;
const hsize_t HDF5CLParser::DefaultCacheRDCElems = 10009;
const hsize_t HDF5CLParser::DefaultCacheRDCBytes = 10485760;
const double HDF5CLParser::DefaultCacheW0 = 0.75;

HDF5CLParser::HDF5CLParser(bool createOptions) :
  CLParser()
{
  if (createOptions)
  {
    addOption("chunk", "hdf5 chunk size", DefaultChunkSize);
    addOption("deflate", "hdf5 compression factor [0:none - 9:max]", 
              DefaultDeflate);
  }
  addOption("cacheMDC", "number of metadata slots in hdf5 cache",
            DefaultCacheMDCElems);
  addOption("cacheRDC", "number of regular slots in hdf5 cache",
            DefaultCacheRDCElems);
  addOption("cacheBytes", "maximum size in bytes of regular hdf5 cache",
            DefaultCacheRDCBytes);
  addOption("cacheW0", "w0 parameter fro hdf5 cache", DefaultCacheW0);
}

HDF5CLParser::~HDF5CLParser()
{
}

void HDF5CLParser::applyToDCProps(DSetCreatPropList& dcprops) const
{
  if (hasOption("chunk"))
  {
    hsize_t chunk = getOption<hsize_t>("chunk");
    hsize_t deflate = getOption<hsize_t>("deflate");
    dcprops.setChunk(1, &chunk);
    dcprops.setDeflate(deflate);
  }
}

void HDF5CLParser::applyToAProps(H5::FileAccPropList& aprops) const
{
  hsize_t mdc = getOption<hsize_t>("cacheMDC");
  hsize_t rdc = getOption<hsize_t>("cacheRDC");
  hsize_t rdcb = getOption<hsize_t>("cacheBytes");
  double w0 = getOption<double>("cacheW0");
  aprops.setCache(mdc, rdc, rdcb, w0);
}
