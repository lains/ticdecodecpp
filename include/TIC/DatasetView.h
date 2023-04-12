/**
 * @file DatasetView.h
 * @brief TIC label decoder
 */
#pragma once
#ifdef __TIC_LIB_USE_STD_STRING__
#include <string>
#endif
#include <stdint.h>

#ifndef ARDUINO
#define STATIC_CONSTEXPR static constexpr
#else
#define STATIC_CONSTEXPR static const
#endif

namespace TIC {
class Horodate {
public:
/* Types */
    typedef enum {
        Unknown = 0,
        Winter, /* Winter */
        Summer, /* Summer */
        Malformed,
    } Season;
/* Constants */
    STATIC_CONSTEXPR unsigned int HORODATE_SIZE = 13; /*!< Size for a horodate tag (in bytes) */

/* Methods */
    Horodate():
    isValid(false),
    season(Season::Unknown),
    degradedTime(true),
    year(0),
    month(0),
    day(0),
    hour(0),
    minute(0),
    second(0) { }

    static Horodate fromLabelBytes(const uint8_t* bytes, unsigned int count);

#ifdef __TIC_LIB_USE_STD_STRING__
    std::string toString() const;
#endif

public:
/* Attributes */
    bool isValid; /*!< Does this instance contain a valid horodate? */
    Season season; /*!< */
    bool degradedTime; /*!< The horodate was generated by a device with a degraded realtime-clock */
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class DatasetView {
public:
/* Types */
    typedef enum {
        Malformed = 0, /* Malformed dataset */
        WrongCRC,
        ValidHistorical, /* Default TIC on newly installed meters */
        ValidStandard,
    } DatasetType;

/* Constants */
    STATIC_CONSTEXPR uint8_t _HT = 0x09; /* Horizontal tab */
    STATIC_CONSTEXPR uint8_t _SP = 0x20; /* Space */

/* Methods */
    /**
     * @brief Construct a new TIC::DatasetView object from a dataset buffer
     * 
     * @param datasetBuf A pointer to a byte buffer containing a full dataset
     * @param datasetBufSz The number of valid bytes in @p datasetBuf
     */
    DatasetView(const uint8_t* datasetBuf, unsigned int datasetBufSz);

    /**
     * @brief Does the dataset buffer used at constructor contain a properly formatted dataset?
     * 
     * @return false if the buffer is malformed
     */
    bool isValid() const;

    /**
     * @brief Compute a unsigned int value from a dataset value buffer
     * 
     * @param buf A pointer to the dataset value buffer
     * @param cnt The number of digits in @p buf
     * @return The decoded unsigned int value, or -1 in case of errors
     */
    static uint32_t uint32FromValueBuffer(const uint8_t* buf, unsigned int cnt);

private:
    static uint8_t computeCRC(const uint8_t* bytes, unsigned int count);

public:
/* Attributes */
    DatasetType decodedType;  /*!< What is the resulting type of the parsed dataset? */
    const uint8_t* labelBuffer; /*!< A pointer to the label buffer */
    unsigned int labelSz; /*!< The size of the label in bytes */
    const uint8_t* dataBuffer; /*!< A pointer to the data buffer associated with the label */
    unsigned int dataSz; /*!< The size of the label in bytes */
    Horodate horodate; /*!< The horodate for this dataset */
};
} // namespace TIC