/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include "hal.h"
#include "halCLParser.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;
using namespace hal;

static void printSequenceLine(ostream &outStream, const Sequence *sequence, hal_size_t start, hal_size_t length,
                              string &buffer, bool upper);
static void printSequence(ostream &outStream, const Sequence *sequence, hal_size_t lineWidth, hal_size_t start,
                          hal_size_t length, bool fullNames, bool upper);
static void printGenome(ostream &outStream, const Genome *genome, const Sequence *sequence, hal_size_t lineWidth,
                        hal_size_t start, hal_size_t length, bool fullNames, bool upper);

static const hal_size_t StringBufferSize = 1024;

static void initParser(CLParser &optionsParser) {
    optionsParser.addArgument("inHalPath", "input hal file");
    optionsParser.addArgument("genome", "genome to export");
    optionsParser.addOption("outFaPath", "output fasta file (stdout if none)", "stdout");
    optionsParser.addOptionFlag("ucscSequenceNames", "Use the UCSC convention of Genome.Sequence for names."
                                " By default, only sequence names are used",
                                false);
    optionsParser.addOption("lineWidth", "Line width for output", 80);
    optionsParser.addOption("sequence", "sequence name to export ("
                                        "all sequences by default)",
                            "\"\"");
    optionsParser.addOption("start", "coordinate within reference genome (or sequence"
                                     " if specified) to start at",
                            0);
    optionsParser.addOption("length", "length of the reference genome (or sequence"
                                      " if specified) to convert.  If set to 0,"
                                      " the entire thing is converted",
                            0);
    optionsParser.addOptionFlag("subtree", "Export all sequences in subtree rooted at <genome>", false);
    optionsParser.addOptionFlag("upper", "Convert all bases to uppercase", false);
    optionsParser.setDescription("Export sequences of genome or subtree of genomes from hal database to "
                                 "fasta file.");
}

int main(int argc, char **argv) {
    CLParser optionsParser;
    initParser(optionsParser);

    string halPath;
    string faPath;
    bool fullNames;    
    hal_size_t lineWidth;
    string genomeName;
    string sequenceName;
    hal_size_t start;
    hal_size_t length;
    bool subtree;
    bool upper;
    try {
        optionsParser.parseOptions(argc, argv);
        halPath = optionsParser.getArgument<string>("inHalPath");
        genomeName = optionsParser.getArgument<string>("genome");
        faPath = optionsParser.getOption<string>("outFaPath");
        fullNames = optionsParser.getFlag("ucscSequenceNames");
        lineWidth = optionsParser.getOption<hal_size_t>("lineWidth");
        sequenceName = optionsParser.getOption<string>("sequence");
        start = optionsParser.getOption<hal_size_t>("start");
        length = optionsParser.getOption<hal_size_t>("length");
        subtree = optionsParser.getFlag("subtree");
        upper = optionsParser.getFlag("upper");

        if (subtree) {
            if (start != 0) {
                throw hal_exception("--start cannot be used with --subtree");
            } else if (length != 0) {
                throw hal_exception("--length cannot be used with --subtree");
            } else if (sequenceName != "\"\"") {
                throw hal_exception("--sequence cannot be used with --subtree");
            }
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        optionsParser.printUsage(cerr);
        exit(1);
    }

    try {
        AlignmentConstPtr alignment(openHalAlignment(halPath, &optionsParser));
        if (alignment->getNumGenomes() == 0) {
            throw hal_exception("input hal alignmenet is empty");
        }

        ofstream ofile;
        ostream &outStream = faPath == "stdout" ? cout : ofile;
        if (faPath != "stdout") {
            ofile.open(faPath.c_str());
            if (!ofile) {
                throw hal_exception(string("Error opening output file ") + faPath);
            }
        }

        deque<string> bfsQueue = {genomeName};

        while (!bfsQueue.empty()) {

            string curName = bfsQueue.front();
            bfsQueue.pop_front();

            const Genome *genome = alignment->openGenome(curName);
            if (genome == NULL) {
                throw hal_exception(string("Genome ") + curName + " not found");
            }

            const Sequence *sequence = NULL;
            if (sequenceName != "\"\"") {
                sequence = genome->getSequence(sequenceName);
                if (sequence == NULL) {
                    throw hal_exception(string("Sequence ") + sequenceName + " not found");
                }
            }

            printGenome(outStream, genome, sequence, lineWidth, start, length, fullNames, upper);

            if (subtree) {
                vector<string> childs = alignment->getChildNames(curName);
                for (int i = 0; i < childs.size(); ++i) {
                    bfsQueue.push_back(childs[i]);
                }
            }

            alignment->closeGenome(genome);
        }

    } catch (hal_exception &e) {
        cerr << "hal exception caught: " << e.what() << endl;
        return 1;
    } catch (exception &e) {
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    }

    return 0;
}

void printSequenceLine(ostream &outStream, const Sequence *sequence, hal_size_t start, hal_size_t length, string &buffer, bool upper) {
    hal_size_t last = start + length;
    hal_size_t readLen;
    for (hal_size_t i = start; i < last; i += StringBufferSize) {
        readLen = std::min(StringBufferSize, last - i);
        sequence->getSubString(buffer, i, readLen);
        if (upper) {
            for (int j = 0; j < buffer.size(); ++j) {
                buffer[j] = std::toupper(buffer[j]);
            }
        }
        outStream << buffer;
    }
}

void printSequence(ostream &outStream, const Sequence *sequence, hal_size_t lineWidth, hal_size_t start, hal_size_t length, bool fullNames, bool upper) {
    hal_size_t seqLen = sequence->getSequenceLength();
    if (length == 0) {
        length = seqLen - start;
    }
    hal_size_t last = start + length;
    if (last > seqLen) {
        throw hal_exception("Specified range [" + std::to_string(start) + "," + std::to_string(length) + "] is" +
                            "out of range for sequence " + sequence->getName() + ", which has length " +
                            std::to_string(seqLen));
    }
    outStream << '>' << (fullNames ? sequence->getFullName() : sequence->getName()) << '\n';
    hal_size_t readLen;
    string buffer;
    for (hal_size_t i = start; i < last; i += lineWidth) {
        readLen = std::min(lineWidth, last - i);
        printSequenceLine(outStream, sequence, i, readLen, buffer, upper);
        outStream << '\n';
    }
}

void printGenome(ostream &outStream, const Genome *genome, const Sequence *sequence, hal_size_t lineWidth, hal_size_t start,
                 hal_size_t length, bool fullNames, bool upper) {
    if (sequence != NULL) {
        printSequence(outStream, sequence, lineWidth, start, length, fullNames, upper);
    } else {
        if (start + length > genome->getSequenceLength()) {
            throw hal_exception("Specified range [" + std::to_string(start) + "," + std::to_string(length) + "] is" +
                                "out of range for genome " + genome->getName() + ", which has length " +
                                std::to_string(genome->getSequenceLength()));
        }
        if (length == 0) {
            length = genome->getSequenceLength() - start;
        }

        hal_size_t runningLength = 0;
        for (SequenceIteratorPtr seqIt = genome->getSequenceIterator(); not seqIt->atEnd(); seqIt->toNext()) {
            const Sequence *sequence = seqIt->getSequence();
            hal_size_t seqLen = sequence->getSequenceLength();
            hal_size_t seqStart = (hal_size_t)sequence->getStartPosition();

            if (start + length >= seqStart && start < seqStart + seqLen && runningLength < length) {
                hal_size_t readStart = seqStart >= start ? 0 : seqStart - start;
                hal_size_t readLen = std::min(seqLen - start, length - runningLength);

                printSequence(outStream, sequence, lineWidth, readStart, readLen, fullNames, upper);
                runningLength += readLen;
            }
        }
    }
}
