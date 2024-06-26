#include <string.h> // For memset()
#include "TIC/DatasetExtractor.h"

TIC::DatasetExtractor::DatasetExtractor(FDatasetParserFunc onDatasetExtracted, void* onDatasetExtractedContext) :
sync(false),
onDatasetExtracted(onDatasetExtracted),
onDatasetExtractedContext(onDatasetExtractedContext),
nextWriteInCurrentDataset(0) {
    memset(this->currentDataset, 0, MAX_DATASET_SIZE);
}

unsigned int TIC::DatasetExtractor::pushBytes(const uint8_t* buffer, unsigned int len) {
    /* In a TIC frame, TIC labels follow the format:
    [LF]<dataset>[CR]
    Where:
    - LF is the ASCII character 0x0a (line-feed)
    - <dataset> is the TIC dataset we want to extract
    - [CR] is the ASCII character 0x0d (carriage return)

    See https://lucidar.me/fr/home-automation/linky-customer-tele-information/
    */
    if (len == 0)
        return 0;
    //std::cout << "de got " << len << " bytes starting with: " << std::hex << static_cast<unsigned int>(buffer[0]) << " while " << std::string(this->sync?"inside":"outside") << " of a dataset\n";
    unsigned int usedBytes = 0;
    if (!this->sync) {  /* We don't record bytes, we'll just look for a start of dataset */
        uint8_t* firstStartOfDataset = (uint8_t*)(memchr(buffer, TIC::DatasetExtractor::START_MARKER, len));
        if (firstStartOfDataset) {
            this->sync = true;
            unsigned int bytesToSkip =  firstStartOfDataset - buffer;  /* Bytes processed (but ignored) */
            usedBytes = bytesToSkip;
            if (bytesToSkip < len) {  /* we have at least one trailing byte */
                usedBytes++;
                firstStartOfDataset++; /* Skip LF marker (it won't be included inside the buffered dataset) */
                usedBytes += this->pushBytes(firstStartOfDataset, len - usedBytes);  /* Recursively parse the trailing bytes, now that we are in sync */
            }
        }
        else {
            /* Skip all bytes */
            usedBytes = len;
        }
    }
    else {
        /* We are inside a TIC dataset, search for the end of dataset marker */
        //FIXME: historical TIC uses LF, standard TIC uses CR
        //If we allow LD below, and we have transmission errors, we may become out of sync if we catch a wrong extraneous LF... we will be out of sync and get only empty datasets because we swap start and end markers
        //We should have a way to recover from this, for example by detecting 0 size datasets and thus understand we need to invert sync state
        uint8_t* probeEndOfDataset1 = nullptr; /* Attempt to detect the end of a dataset formatted with standard TIC */
        uint8_t* probeEndOfDataset2 = nullptr; /* Attempt to detect the end of a dataset formatted with historical TIC */
        uint8_t* endOfDataset = nullptr;
        probeEndOfDataset1 = (uint8_t*)(memchr(buffer, TIC::DatasetExtractor::END_MARKER_TIC_1, len));
        if (probeEndOfDataset1) {
            endOfDataset = probeEndOfDataset1;
        }
        else {
            probeEndOfDataset2 = (uint8_t*)(memchr(buffer, TIC::DatasetExtractor::END_MARKER_TIC_2, len)); /* Search for end of dataset formatted with historical TIC */
            if (probeEndOfDataset2) {
                endOfDataset = probeEndOfDataset2;
            }
        }
        
        if (endOfDataset) {  /* We have an end of dataset marker in the buffer, we can extract the full dataset */
            unsigned int leadingBytesInPreviousDataset = endOfDataset - buffer;
            usedBytes = this->processIncomingDatasetBytes(buffer, leadingBytesInPreviousDataset, true);  /* Copy the buffer up to (but exclusing the end of dataset marker), the dataset is complete */
            this->nextWriteInCurrentDataset = 0; /* Wipe any data in the current dataset, start over */
            leadingBytesInPreviousDataset++; /* Skip the end of dataset marker */
            usedBytes++;
            this->sync = false; /* Consider we are outside of a dataset now */
            if (leadingBytesInPreviousDataset < len) { /* We have at least one byte after the end of dataset marker */
                usedBytes += this->pushBytes(buffer + leadingBytesInPreviousDataset, len - leadingBytesInPreviousDataset); /* Process the trailing bytes (probably the next dataset */
            }
        }
        else { /* No end of dataset marker, copy the whole chunk */
            usedBytes = this->processIncomingDatasetBytes(buffer, len); /* Process these bytes as valid */
        }
    }
    return usedBytes;
}

unsigned int TIC::DatasetExtractor::processIncomingDatasetBytes(const uint8_t* buffer, unsigned int len, bool datasetComplete) {
    unsigned int maxCopy = this->getFreeBytes();
    unsigned int szCopy = len;
    if (szCopy > maxCopy) {  /* currentFrame overflow */
        szCopy = maxCopy; /* FIXME: Error case */
    }
    memcpy(this->currentDataset + this->nextWriteInCurrentDataset, buffer, szCopy);
    this->nextWriteInCurrentDataset += szCopy;

    if (datasetComplete) {
        this->processCurrentDataset();
    }
    return szCopy;
}

void TIC::DatasetExtractor::processCurrentDataset() {
    //std::vector<uint8_t> datasetContent(this->currentDataset, this->currentDataset+this->nextWriteInCurrentDataset);
    //std::cout << "New dataset extracted: " << vectorToHexString(datasetContent) << " (as string: \"" << std::string(datasetContent.begin(), datasetContent.end()) << "\")\n";
    if (this->onDatasetExtracted)
        this->onDatasetExtracted(this->currentDataset, this->nextWriteInCurrentDataset, this->onDatasetExtractedContext);
}

bool TIC::DatasetExtractor::isInSync() const {
    return this->sync;
}

unsigned int TIC::DatasetExtractor::getFreeBytes() const {
    return MAX_DATASET_SIZE - this->nextWriteInCurrentDataset;
}

void TIC::DatasetExtractor::reset() {
    this->sync = false;
    memset(this->currentDataset, 0, MAX_DATASET_SIZE);
    this->nextWriteInCurrentDataset = 0;
}

