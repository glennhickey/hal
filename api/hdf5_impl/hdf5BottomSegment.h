/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HDF5BOTTOMSEGMENT_H
#define _HDF5BOTTOMSEGMENT_H

#include <H5Cpp.h>
#include "halDefs.h"
#include "halBottomSegment.h"
#include "hdf5Genome.h"
#include "hdf5ExternalArray.h"

namespace hal {

class HDF5TopSegmentIterator;
class HDF5BottomSegmentIterator;

class HDF5BottomSegment : public BottomSegment
{
public:

   friend class HDF5TopSegmentIterator;
   friend class HDF5BottomSegmentIterator;

    /** Constructor 
    * @param genome Smart pointer to genome to which segment belongs
    * @param array HDF5 array containg segment
    * @param index Index of segment in the array */
   HDF5BottomSegment(HDF5Genome* genome,
                     HDF5ExternalArray* array,
                     hal_index_t index);

    /** Destructor */
   ~HDF5BottomSegment();

   /** Get the length of the segment (number of bases) */
   hal_size_t getLength() const;

   /** Set the length of the segment
    * @param length New length of segment */
   void setLength(hal_size_t length);

   /** Get the containing (read-only) genome */
   const Genome* getGenome() const;

   /** Get the containing genome */
   Genome* getGenome();

   /** Get the segment's start position in the genome */
   hal_index_t getStartPosition() const;

   /** Set the segment's start position in the genome 
    * @param startPos Start position */
   void setStartPosition(hal_index_t startPos);

   /** Get a copy of the string of DNA characters associated with 
    * this segment */
   std::string getSequence() const;

   /** Get index of the next paralogous segment in the genome */
   hal_index_t getNextParalogyIndex() const;

   /** Set index of the next paralogous segment in the genome 
    * @param parIdx of next segment in same genome that is 
    * homologous to this segment */
   void setNextParalogyIndex(hal_index_t parIdx);

   /** Get the number of child genomes (note this is a number of slots
    * and that the current segment could actually have fewer children) */
   hal_size_t getNumChildren() const;
   
   /** Get the index of a child segment (OR NULL_INDEX if none)
    * @param i index of child to query */
   hal_index_t getChildIndex(hal_size_t i) const;

   /** Set the index of a child segment (OR NULL_INDEX if none)
    * @param i index of child to set 
    * @param childIndex index of segment in child to set */
   void setChildIndex(hal_size_t i, hal_index_t childIndex);

   /** Get whether descent segment for ith child is mapped to the 
    * reverse complement of this segment 
    * @param i index of child to query */
   hal_bool_t getChildReversed(hal_size_t i) const;

   /** Set whether descent segment for ith child is mapped to the 
    * reverse complement of this segment 
    * @param i index of child to set 
    * @param isReverse flag */
   void setChildReversed(hal_size_t child, hal_bool_t isReversed);

   /** Get index of top segment in samge genome that contains
    * this segment's start coordinate */
   hal_index_t getTopParseIndex() const;

   /** Set index of top segment in samge genome that contains
    * this segment's start coordinate 
    * @param parParseIndex index */
   void setTopParseIndex(hal_index_t parseIndex);
   
   /** Get offset in associated top segment of start coordinate of 
    * this segment */
   hal_offset_t getTopParseOffset() const;

   /** Set offset in associated top segment of start coordinate of 
    * this segment 
    * @param parpArseOffset offset in parent */
   void setTopParseOffset(hal_offset_t parseOffset);

   /** Get the index of the segment in the segment array */
   hal_index_t getArrayIndex() const;

   static H5::CompType dataType(hal_size_t numChildren);
   static hal_size_t numChildrenFromDataType(
     const H5::DataType& dataType);

   
protected:

   static const size_t genomeIndexOffset;
   static const size_t lengthOffset;
   static const size_t topIndexOffset;
   static const size_t topOffsetOffset;
   static const size_t parIndexOffset;
   static const size_t firstChildOffset;
   static const size_t totalSize(hal_size_t numChildren);

   mutable HDF5ExternalArray* _array;
   mutable hal_index_t _index;
   mutable HDF5Genome* _genome;
};

//INLINE members


inline hal_index_t HDF5BottomSegment::getStartPosition() const
{
  assert(_index >= 0);
  return _array->getValue<hal_index_t>((hsize_t)_index, genomeIndexOffset);
}

inline void 
HDF5BottomSegment::setStartPosition(hal_index_t startPos)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, genomeIndexOffset, startPos);
}

inline hal_size_t HDF5BottomSegment::getLength() const
{
  assert(_index >= 0);
  return _array->getValue<hal_size_t>((hsize_t)_index, lengthOffset);
}

inline void 
HDF5BottomSegment::setLength(hal_size_t length)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, lengthOffset, length);
}

inline const Genome* HDF5BottomSegment::getGenome() const
{                                               
  return _genome;
}

inline Genome* HDF5BottomSegment::getGenome()
{
  return _genome;
}

inline std::string HDF5BottomSegment::getSequence() const
{
  return "todo";
}

inline hal_index_t HDF5BottomSegment::getNextParalogyIndex() const
{
  return _array->getValue<hal_index_t>(_index, parIndexOffset);
}

inline void HDF5BottomSegment::setNextParalogyIndex(hal_index_t parIdx)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, parIndexOffset, parIdx);
}

inline hal_size_t HDF5BottomSegment::getNumChildren() const
{
  return _genome->getNumChildren();
}

inline hal_index_t HDF5BottomSegment::getChildIndex(hal_size_t i) const
{
  assert(_index >= 0);
  return _array->getValue<hal_index_t>(
    (hsize_t)_index, firstChildOffset + 
    i * (sizeof(hal_index_t) + sizeof(hal_bool_t)));
}

inline void HDF5BottomSegment::setChildIndex(hal_size_t i, 
                                             hal_index_t childIndex)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, firstChildOffset + 
                   i * (sizeof(hal_index_t) + sizeof(hal_bool_t)), childIndex);
}

inline hal_bool_t HDF5BottomSegment::getChildReversed(hal_size_t i) const
{
  assert(_index >= 0);
  return _array->getValue<hal_bool_t>(
    (hsize_t)_index, firstChildOffset + 
    i * (sizeof(hal_index_t) + sizeof(hal_bool_t)) +
    sizeof(hal_index_t));
}

inline void HDF5BottomSegment::setChildReversed(hal_size_t i, 
                                                hal_bool_t isReversed)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, firstChildOffset + 
                   i * (sizeof(hal_index_t) + sizeof(hal_bool_t)) + 
                   sizeof(hal_index_t), isReversed);
}

inline hal_index_t HDF5BottomSegment::getTopParseIndex() const
{
  assert(_index >= 0);
  return _array->getValue<hal_index_t>(
    (hsize_t)_index, topIndexOffset);
}

inline void HDF5BottomSegment::setTopParseIndex(hal_index_t parseIndex)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, topIndexOffset, parseIndex);
}
   
inline hal_offset_t HDF5BottomSegment::getTopParseOffset() const
{
  assert(_index >= 0);
  return _array->getValue<hal_offset_t>((hsize_t)_index, topOffsetOffset);
}

inline void HDF5BottomSegment::setTopParseOffset(hal_offset_t parseOffset)
{
  assert(_index >= 0);
  _array->setValue((hsize_t)_index, topOffsetOffset, parseOffset);
}

inline hal_index_t HDF5BottomSegment::getArrayIndex() const
{
  return _index;
}
}

#endif