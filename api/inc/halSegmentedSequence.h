/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALSEGMENTEDSEQUENCE_H
#define _HALSEGMENTEDSEQUENCE_H

#include "halDefs.h"

namespace hal {

/** 
 * Interface for a sequence of DNA that is also broken up into 
 * top and bottom segments.  This interface is extended by both
 * the Genome and Sequence classes. 
 *
 * Todo: Implenent the "Last" or maybe (right / left) iterator
 * notion, which will make looping easier and more general. 
 */
class SegmentedSequence
{
public:   

   /** Get the total length of the DNA sequence in the sequence*/
   virtual hal_size_t getSequenceLength() const = 0;
   
   /** Get the number of top segements 
    * (which form blocks with ancestor and siblings)
    * in the sequence */
   virtual hal_size_t getNumTopSegments() const = 0;

   /** Get the number of bottom segments
    * (which form blocks with the children)
    * in the sequence */
   virtual hal_size_t getNumBottomSegments() const = 0;

   /** Get a top segment iterator
    * @param position Index in segment array of returned iterator */
   virtual TopSegmentIteratorPtr getTopSegmentIterator(
     hal_index_t position = 0) = 0;

   /** Get a const top segment iterator
    * @param position Index in segment array of returned iterator */
   virtual TopSegmentIteratorConstPtr getTopSegmentIterator(
     hal_index_t position = 0) const = 0;

   /** Get a bottom segment iterator
    * @param position Index in segment array of returned iterator */
   virtual BottomSegmentIteratorPtr getBottomSegmentIterator(
     hal_index_t position = 0) = 0;

   /** Get a const bottom segment iterator
    * @param position Index in segment array of returned iterator */
   virtual BottomSegmentIteratorConstPtr getBottomSegmentIterator(
     hal_index_t position = 0) const = 0;

   /** Get a DNA iterator
    * @param position Index in segment array of returned iterator */
   virtual DNAIteratorPtr getDNAIterator(
     hal_index_t position = 0) = 0;

   /** Get a const DNA iterator
    * @param position Index in segment array of returned iterator */
   virtual DNAIteratorConstPtr getDNAIterator(
     hal_index_t position = 0) const = 0;

   /** Get the character string underlying the segmented sequence
    * @param outString String object into which we copy the result */
   virtual void getString(std::string& outString) const = 0;

  /** Set the character string underlying the segmented sequence
    * @param inString input string to copy */
   virtual void setString(const std::string& inString) = 0;

   /** Get the substring of character string underlying the 
    * segmented sequence
    * @param outString String object into which we copy the result
    * @param start First position of substring 
    * @param length Length of substring */
   virtual void getSubString(std::string& outString, hal_size_t start,
                             hal_size_t length) const = 0;

  /** Set the character string underlying the segmented sequence
    * @param inString input string to copy
    * @param start First position of substring 
    * @param length Length of substring */
   virtual void setSubString(const std::string& inString, 
                             hal_size_t start,
                             hal_size_t length) = 0;


protected:

   /** Destructor */
   virtual ~SegmentedSequence() = 0;
};

inline SegmentedSequence::~SegmentedSequence() {}

}
#endif