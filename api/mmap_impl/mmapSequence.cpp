#include "defaultColumnIterator.h"
#include "defaultRearrangement.h"
#include "defaultGappedTopSegmentIterator.h"
#include "defaultGappedBottomSegmentIterator.h"
#include "mmapSequence.h"
#include "mmapGenome.h"
#include "mmapDNAIterator.h"

using namespace std;
using namespace hal;

TopSegmentIteratorPtr MMapSequence::getTopSegmentIterator(
  hal_index_t position)
{
  hal_size_t idx = position + getTopSegmentArrayIndex();
  return _genome->getTopSegmentIterator(idx);
}

TopSegmentIteratorConstPtr MMapSequence::getTopSegmentIterator(
  hal_index_t position) const
{
  hal_size_t idx = position + getTopSegmentArrayIndex();
  return _genome->getTopSegmentIterator(idx);
}

TopSegmentIteratorConstPtr MMapSequence::getTopSegmentEndIterator() const
{
  return getTopSegmentIterator(getNumTopSegments());
}

BottomSegmentIteratorPtr MMapSequence::getBottomSegmentIterator(
  hal_index_t position)
{
  hal_size_t idx = position + getBottomSegmentArrayIndex();
  return _genome->getBottomSegmentIterator(idx);
}

BottomSegmentIteratorConstPtr MMapSequence::getBottomSegmentIterator(
  hal_index_t position) const
{
  hal_size_t idx = position + getBottomSegmentArrayIndex();
  return _genome->getBottomSegmentIterator(idx);
}

BottomSegmentIteratorConstPtr MMapSequence::getBottomSegmentEndIterator() const
{
  return getBottomSegmentIterator(getNumBottomSegments());
}

DNAIteratorPtr MMapSequence::getDNAIterator(hal_index_t position)
{
  hal_size_t idx = position + getStartPosition();
  MMapDNAIterator* newIt = new MMapDNAIterator(_genome, idx);
  return DNAIteratorPtr(newIt);
}

DNAIteratorConstPtr MMapSequence::getDNAIterator(hal_index_t position) const
{
  hal_size_t idx = position + getStartPosition();
  const MMapDNAIterator* newIt = new MMapDNAIterator(_genome, idx);
  return DNAIteratorConstPtr(newIt);
}

DNAIteratorConstPtr MMapSequence::getDNAEndIterator() const
{
  return getDNAIterator(getSequenceLength());
}

ColumnIteratorConstPtr MMapSequence::getColumnIterator(
  const std::set<const Genome*>* targets, hal_size_t maxInsertLength, 
  hal_index_t position, hal_index_t lastPosition, bool noDupes,
  bool noAncestors, bool reverseStrand, bool unique, bool onlyOrthologs) const
{
  hal_index_t idx = (hal_index_t)(position + getStartPosition());
  hal_index_t lastIdx;
  if (lastPosition == NULL_INDEX)
  {
    lastIdx = (hal_index_t)(getStartPosition() + getSequenceLength() - 1);
  }
  else
  {
    lastIdx = (hal_index_t)(lastPosition + getStartPosition());
  }
  if (position < 0 || 
      lastPosition >= (hal_index_t)(getStartPosition() + getSequenceLength()))
  {
    stringstream ss;
    ss << "MMapSequence::getColumnIterators: input indices "
       << "(" << position << ", " << lastPosition << ") out of bounds";
    throw hal_exception(ss.str());
  }
  const DefaultColumnIterator* newIt = 
     new DefaultColumnIterator(getGenome(), targets, idx, lastIdx, 
                               maxInsertLength, noDupes, noAncestors,
                               reverseStrand, unique, onlyOrthologs);
  return ColumnIteratorConstPtr(newIt);
}

void MMapSequence::getString(std::string& outString) const
{
  getSubString(outString, 0, getSequenceLength());
}

void MMapSequence::setString(const std::string& inString)
{
  setSubString(inString, 0, getSequenceLength());
}

void MMapSequence::getSubString(std::string& outString, hal_size_t start,
                                hal_size_t length) const
{
  hal_size_t idx = start + getStartPosition();
  outString.resize(length);
  MMapDNAIterator dnaIt(_genome, idx);
  dnaIt.readString(outString, length);
}

void MMapSequence::setSubString(const std::string& inString, 
                                hal_size_t start,
                                hal_size_t length)
{
  if (length != inString.length())
  {
    stringstream ss;
    ss << "setString: input string of length " << inString.length()
       << " has length different from target string in sequence " << getName() 
       << " which is of length " << length;
    throw hal_exception(ss.str());
  }
  hal_size_t idx = start + getStartPosition();
  MMapDNAIterator dnaIt(_genome, idx);
  dnaIt.writeString(inString, length);
}

RearrangementPtr MMapSequence::getRearrangement(hal_index_t position,
                                                hal_size_t gapLengthThreshold,
                                                double nThreshold,
                                                bool atomic) const
{
  TopSegmentIteratorConstPtr top = getTopSegmentIterator(position);  
  DefaultRearrangement* rea = new DefaultRearrangement(getGenome(),
                                                       gapLengthThreshold,
                                                       nThreshold,
                                                       atomic);
  rea->identifyFromLeftBreakpoint(top);
  return RearrangementPtr(rea);
}

GappedTopSegmentIteratorConstPtr MMapSequence::getGappedTopSegmentIterator(
  hal_index_t i, hal_size_t gapThreshold, bool atomic) const
{
  TopSegmentIteratorConstPtr top = getTopSegmentIterator(i);  
  DefaultGappedTopSegmentIterator* gt = 
     new DefaultGappedTopSegmentIterator(top, gapThreshold, atomic);
  return GappedTopSegmentIteratorConstPtr(gt);
}

GappedBottomSegmentIteratorConstPtr 
MMapSequence::getGappedBottomSegmentIterator(
  hal_index_t i, hal_size_t childIdx, hal_size_t gapThreshold,
  bool atomic) const
{
  BottomSegmentIteratorConstPtr bot = getBottomSegmentIterator(i);  
  DefaultGappedBottomSegmentIterator* gb = 
     new DefaultGappedBottomSegmentIterator(bot, childIdx, gapThreshold, 
                                            atomic);
  return GappedBottomSegmentIteratorConstPtr(gb);
}