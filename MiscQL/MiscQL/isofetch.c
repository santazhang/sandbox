//
//  isofetch.c
//  Code extracted from libcdio
//
//  Created by Santa Zhang on 4/3/13.
//  Copyright (c) 2013 Santa Zhang. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <ctype.h>

#define GNUC_PACKED             __attribute__((__packed__))
#define ISODCL(from, to)        ((to) - (from) + 1)
#define false                   0
#define true                    1
#define EMPTY_ARRAY_SIZE        0
#define CDIO_CD_FRAMESIZE_RAW   2352
#define M2RAW_SECTOR_SIZE       2336
#define CDIO_CD_SYNC_SIZE       12
#define ISO_STANDARD_ID         "CD001"
#define ISO_VD_PRIMARY          1
#define ISO_XA_MARKER_OFFSET    1024
#define ISO_XA_MARKER_STRING    "CD-XA001"
#define CDIO_CD_HEADER_SIZE     4
#define CDIO_CD_SUBHEADER_SIZE  8
#define ISO_VD_END              255
#define ISO_VD_SUPPLEMENTARY    2
#define ISO_EXTENSION_NONE      0x00
#define ISO_DIRECTORY           2
#define ISO_EXTENSION_JOLIET_LEVEL1     0x01
#define ISO_EXTENSION_JOLIET_LEVEL2     0x02
#define ISO_EXTENSION_JOLIET_LEVEL3     0x04

/*! \brief Maximum number of characters in a publisher id. */
#define ISO_MAX_PUBLISHER_ID    128

/*! \brief Maximum number of characters in an application id. */
#define ISO_MAX_APPLICATION_ID  128

/*! \brief Maximum number of characters in a volume id. */
#define ISO_MAX_VOLUME_ID       32

/*! \brief Maximum number of characters in a volume-set id. */
#define ISO_MAX_VOLUMESET_ID    128

/** 16-bit big-endian to little-endian */
#define UINT16_SWAP_LE_BE_C(val) ((uint16_t) ( \
(((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) | \
(((uint16_t) (val) & (uint16_t) 0xff00U) >> 8)))

/** 32-bit big-endian to little-endian */
#define UINT32_SWAP_LE_BE_C(val) ((uint32_t) ( \
(((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
(((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
(((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
(((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

/** 64-bit big-endian to little-endian */
#define UINT64_SWAP_LE_BE_C(val) ((uint64_t) ( \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x00000000000000ff)) << 56) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x000000000000ff00)) << 40) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x0000000000ff0000)) << 24) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x00000000ff000000)) <<  8) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x000000ff00000000)) >>  8) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x0000ff0000000000)) >> 24) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0x00ff000000000000)) >> 40) | \
(((uint64_t) (val) & (uint64_t) UINT64_C(0xff00000000000000)) >> 56)))

#ifndef UINT16_SWAP_LE_BE
# define UINT16_SWAP_LE_BE UINT16_SWAP_LE_BE_C
#endif

#ifndef UINT32_SWAP_LE_BE
# define UINT32_SWAP_LE_BE UINT32_SWAP_LE_BE_C
#endif

#ifndef UINT64_SWAP_LE_BE
# define UINT64_SWAP_LE_BE UINT64_SWAP_LE_BE_C
#endif


typedef struct _iso9660_s iso9660_t;
typedef int bool;
typedef int32_t lsn_t;

typedef uint8_t  iso711_t; /*! See section 7.1.1 */
typedef int8_t   iso712_t; /*! See section 7.1.2 */
typedef uint16_t iso721_t; /*! See section 7.2.1 */
typedef uint16_t iso722_t; /*! See section 7.2.2 */
typedef uint32_t iso723_t; /*! See section 7.2.3 */
typedef uint32_t iso731_t; /*! See section 7.3.1 */
typedef uint32_t iso732_t; /*! See section 7.3.2 */
typedef uint64_t iso733_t; /*! See section 7.3.3 */

typedef char     achar_t;  /*! See section 7.4.1 */
typedef char     dchar_t;  /*! See section 7.4.1 */

typedef uint32_t posix_mode_t;
typedef uint32_t posix_nlink_t;
typedef uint32_t posix_uid_t;
typedef uint32_t posix_gid_t;
typedef uint16_t unicode16_t;

typedef uint8_t iso_extension_mask_t;

typedef enum  {
    nope  = 0,
    yep   = 1,
    dunno = 2
} bool_3way_t;

/*! The below isn't really an enumeration one would really use in a
 program; things are done this way so that in a debugger one can to
 refer to the enumeration value names such as in a debugger
 expression and get something. With the more common a \#define
 mechanism, the name/value assocation is lost at run time.
 */
extern enum iso_enum1_s {
    ISO_PVD_SECTOR      =   16, /**< Sector of Primary Volume Descriptor. */
    ISO_EVD_SECTOR      =   17, /**< Sector of End Volume Descriptor. */
    LEN_ISONAME         =   31, /**< Size in bytes of the filename
                                 portion + null byte. */
    ISO_MAX_SYSTEM_ID   =   32, /**< Maximum number of characters in a system
                                 id. */
    MAX_ISONAME         =   37, /**< Size in bytes of the filename
                                 portion + null byte. */
    ISO_MAX_PREPARER_ID =  128, /**< Maximum number of characters in a
                                 preparer id. */
    MAX_ISOPATHNAME     =  255, /**< Maximum number of characters in the
                                 entire ISO 9660 filename. */
    ISO_BLOCKSIZE       = 2048  /**< Number of bytes in an ISO 9660 block. */
} iso_enums1;

/*!
 \brief ISO-9660 longer-format time structure.

 Section 8.4.26.1 of ECMA 119. All values are encoded as character
 arrays, eg. '1', '9', '5', '5' for the year 1955 (no null terminated
 byte).

 @see iso9660_ltime
 */
struct  iso9660_ltime_s {
    char   lt_year        [ISODCL(   1,   4)];   /**< Add 1900 to value
                                                  for the Julian year */
    char   lt_month       [ISODCL(   5,   6)];   /**< Has value in range
                                                  1..12. Note starts
                                                  at 1, not 0 like a
                                                  tm struct. */
    char   lt_day         [ISODCL(   7,   8)];   /**< Day of month: 1..31 */
    char   lt_hour        [ISODCL(   9,   10)];  /**< hour: 0..23 */
    char   lt_minute      [ISODCL(  11,   12)];  /**< minute: 0..59 */
    char   lt_second      [ISODCL(  13,   14)];  /**< second: 0..59 */
    char   lt_hsecond     [ISODCL(  15,   16)];  /**< The value is in units of
                                                  1/100's of a second */
    iso712_t lt_gmtoff;  /**< Offset from Greenwich Mean Time in number
                          of 15 min intervals from -48 (West) to +52 (East)
                          recorded according to 7.1.2 numerical value */
} GNUC_PACKED;

typedef struct iso9660_ltime_s  iso9660_ltime_t;

/*!
 \brief ISO-9660 shorter-format time structure. See ECMA 9.1.5.

 @see iso9660_dtime
 */
struct  iso9660_dtime_s {
    iso711_t      dt_year;   /**< Number of years since 1900 */
    iso711_t      dt_month;  /**< Has value in range 1..12. Note starts
                              at 1, not 0 like a tm struct. */
    iso711_t      dt_day;    /**< Day of the month from 1 to 31 */
    iso711_t      dt_hour;   /**< Hour of the day from 0 to 23 */
    iso711_t      dt_minute; /**< Minute of the hour from 0 to 59 */
    iso711_t      dt_second; /**< Second of the minute from 0 to 59 */
    iso712_t      dt_gmtoff; /**< GMT values -48 .. + 52 in 15 minute
                              intervals */
} GNUC_PACKED;

typedef struct iso9660_dtime_s  iso9660_dtime_t;

typedef struct iso_rock_time_s {
    bool          b_used;     /**< If true, field has been set and  is valid.
                               Otherwise remaning fields are meaningless. */
    bool          b_longdate;  /**< If true date format is a iso9660_ltime_t.
                                Otherwise date is iso9660_dtime_t */
    union {
        iso9660_ltime_t ltime;
        iso9660_dtime_t dtime;
    } t;
} GNUC_PACKED iso_rock_time_t;

typedef struct iso_rock_statbuf_s {
    bool_3way_t   b3_rock;              /**< has Rock Ridge extension.
                                         If "yep", then the fields are used. */
    posix_mode_t  st_mode;              /**< protection */
    posix_nlink_t st_nlinks;            /**< number of hard links */
    posix_uid_t   st_uid;               /**< user ID of owner */
    posix_gid_t   st_gid;               /**< group ID of owner */
    uint8_t       s_rock_offset;
    int           i_symlink;            /**< size of psz_symlink */
    int           i_symlink_max;        /**< max allocated to psz_symlink */
    char         *psz_symlink;          /**< if symbolic link, name
                                         of pointed to file.  */
    iso_rock_time_t create;             /**< create time See ISO 9660:9.5.4. */
    iso_rock_time_t modify;             /**< time of last modification
                                         ISO 9660:9.5.5. st_mtime field of
                                         POSIX:5.6.1. */
    iso_rock_time_t access;             /**< time of last file access st_atime
                                         field of POSIX:5.6.1. */
    iso_rock_time_t attributes;         /**< time of last attribute change.
                                         st_ctime field of POSIX:5.6.1. */
    iso_rock_time_t backup;             /**< time of last backup. */
    iso_rock_time_t expiration;         /**< time of expiration; See ISO
                                         9660:9.5.6. */
    iso_rock_time_t effective;          /**< Effective time; See ISO
                                         9660:9.5.7. */
    uint32_t i_rdev;                    /**< the upper 16-bits is major device
                                         number, the lower 16-bits is the
                                         minor device number */

} iso_rock_statbuf_t;

typedef struct iso9660_xa_s
{
    uint16_t group_id;      /**< 0 */
    uint16_t user_id;       /**< 0 */
    uint16_t attributes;    /**< XA_ATTR_ */
    char     signature[2];  /**< { 'X', 'A' } */
    uint8_t  filenum;       /**< file number, see also XA subheader */
    uint8_t  reserved[5];   /**< zero */
} GNUC_PACKED iso9660_xa_t;

/*! \brief Unix stat-like version of iso9660_dir

 The iso9660_stat structure is not part of the ISO-9660
 specification. We use it for our to communicate information
 in a C-library friendly way, e.g struct tm time structures and
 a C-style filename string.

 @see iso9660_dir
 */
struct iso9660_stat_s { /* big endian!! */

    iso_rock_statbuf_t rr;              /**< Rock Ridge-specific fields  */

    struct tm          tm;              /**< time on entry - FIXME merge with
                                         one of entries above, like ctime? */
    lsn_t              lsn;             /**< start logical sector number */
    uint32_t           size;            /**< total size in bytes */
    uint32_t           secsize;         /**< number of sectors allocated */
    iso9660_xa_t       xa;              /**< XA attributes */
    enum { _STAT_FILE = 1, _STAT_DIR = 2 } type;
    bool               b_xa;
    char               filename[EMPTY_ARRAY_SIZE]; /**< filename */
};

typedef struct iso9660_stat_s   iso9660_stat_t;

/*! \brief Format of an ISO-9660 directory record

 Section 9.1 of ECMA 119.

 This structure may have an odd length depending on how many
 characters there are in the filename!  Some compilers (e.g. on
 Sun3/mc68020) pad the structures to an even length.  For this reason,
 we cannot use sizeof (struct iso_path_table) or sizeof (struct
 iso_directory_record) to compute on disk sizes.  Instead, we use
 offsetof(..., name) and add the name size.  See mkisofs.h of the
 cdrtools package.

 @see iso9660_stat
 */
struct iso9660_dir_s {
    iso711_t         length;          /*! Length of Directory record (9.1.1) */
    iso711_t         xa_length;       /*! XA length if XA is used. Otherwise
                                       zero. (9.1.2)  */
    iso733_t         extent;          /*! LBA of first local block allocated
                                       to the extent */
    iso733_t         size;            /*! data length of File Section. This
                                       does not include the length of
                                       any XA Records. (9.1.2) */
    iso9660_dtime_t  recording_time;  /*! Recording date and time (9.1.3) */
    uint8_t          file_flags;      /*! If no XA then zero. If a directory,
                                       then bits 2,3 and 7 are zero. (9.1.6) */
    iso711_t         file_unit_size;  /*! File Unit size for the File
                                       Section if the File Section
                                       is recorded in interleaved
                                       mode. Otherwise zero. (9.1.7) */
    iso711_t         interleave_gap;  /*! Interleave Gap size for the
                                       File Section if the File Section is
                                       interleaved. Otherwise zero. (9.1.8) */
    iso723_t volume_sequence_number;  /*! Ordinal number of the volume in the
                                       Volume Set on which the Extent described
                                       by this Directory Record is recorded.
                                       (9.1.9) */
    /*! MSVC compilers cannot handle a zero sized array in the middle
     of a struct, and iso9660_dir_s is reused within iso9660_pvd_s.
     Therefore, instead of defining:
     iso711_t filename_len;
     char     filename[];
     We leverage the fact that iso711_t and char are the same size
     and use an union. The only gotcha is that the actual string
     payload of filename.str[] starts at 1, not 0. */
    union {
        iso711_t        len;
        char            str[1];
    } filename;
} GNUC_PACKED;

typedef struct iso9660_dir_s    iso9660_dir_t;

/*!
 \brief ISO-9660 Primary Volume Descriptor.
 */
struct iso9660_pvd_s {
    iso711_t     type;                         /**< ISO_VD_PRIMARY - 1 */
    char         id[5];                        /**< ISO_STANDARD_ID "CD001" */
    iso711_t     version;                      /**< value 1 for ECMA 119 */
    char         unused1[1];                   /**< unused - value 0 */
    achar_t      system_id[ISO_MAX_SYSTEM_ID]; /**< each char is an achar */
    dchar_t      volume_id[ISO_MAX_VOLUME_ID]; /**< each char is a dchar */
    uint8_t      unused2[8];                   /**< unused - value 0 */
    iso733_t     volume_space_size;            /**< total number of sectors */
    uint8_t      unused3[32];                  /**< unused - value 0 */
    iso723_t     volume_set_size;              /**< often 1 */
    iso723_t     volume_sequence_number;       /**< often 1 */
    iso723_t     logical_block_size;           /**< sector size, e.g. 2048 */
    iso733_t     path_table_size;              /**< bytes in path table */
    iso731_t     type_l_path_table;        /**< first sector of L Path Table */
    iso731_t     opt_type_l_path_table;    /**< first sector of optional
                                            L Path Table */
    iso732_t     type_m_path_table;        /**< first sector of M Path table */
    iso732_t     opt_type_m_path_table;    /**< first sector of optional
                                            M Path table */
    iso9660_dir_t  root_directory_record;  /**< See 8.4.18 and section 9.1 of
                                            ISO 9660 spec. */
    char     root_directory_filename;  /**< Is '\\0' or root directory.
                                        Also pads previous field to 34 bytes */
    dchar_t  volume_set_id[ISO_MAX_VOLUMESET_ID]; /**< Volume Set of which the
                                                   volume is a member. See
                                                   section 8.4.19 */
    achar_t  publisher_id[ISO_MAX_PUBLISHER_ID];  /**< Publisher of volume. If
                                                   the first character is '_'
                                                   0x5F, the remaining bytes
                                                   specify a file containing
                                                   the user. If all bytes are
                                                   " " (0x20) no publisher is
                                                   specified. See section
                                                   8.4.20 of ECMA 119 */
    achar_t  preparer_id[ISO_MAX_PREPARER_ID];    /**< preparer of volume. If
                                                   the first character is '_'
                                                   0x5F, the remaining bytes
                                                   specify a file containing
                                                   the user. If all bytes are
                                                   " " (0x20) no preparer is
                                                   specified. See section
                                                   8.4.21 of ECMA 119 */
    achar_t  application_id[ISO_MAX_APPLICATION_ID]; /**< application use to
                                                      create the volume. If the
                                                      first character is '_'
                                                      0x5F, the remaining bytes
                                                      specify a file containing
                                                      the user. If all bytes
                                                      are " " (0x20) no
                                                      application is specified.
                                                      See section of 8.4.22 of
                                                      ECMA 119 */
    dchar_t  copyright_file_id[37];     /**< Name of file for copyright info.
                                         If all bytes are " " (0x20), then no
                                         file is identified. See section 8.4.23
                                         of ECMA 119 9660 spec. */
    dchar_t  abstract_file_id[37];      /**< See section 8.4.24 of ECMA 119. */
    dchar_t  bibliographic_file_id[37]; /**< See section 7.5 of ISO 9660
                                         spec. */
    iso9660_ltime_t  creation_date;     /**< date and time of volume creation.
                                         See section 8.4.26.1 of the ISO 9660
                                         spec. */
    iso9660_ltime_t  modification_date; /**< date and time of the most recent
                                         modification. See section 8.4.27 of
                                         the ISO 9660 spec. */
    iso9660_ltime_t  expiration_date;   /**< date and time when volume expires.
                                         See section 8.4.28 of the ISO 9660
                                         spec. */
    iso9660_ltime_t  effective_date;    /**< date and time when volume is
                                         effective. See section 8.4.29 of the
                                         ISO 9660 spec. */
    iso711_t         file_structure_version;    /**< value 1 for ECMA 119 */
    uint8_t          unused4[1];                /**< unused - value 0 */
    char             application_data[512];     /**< Application can put
                                                 whatever it wants here. */
    uint8_t          unused5[653];              /**< Unused - value 0 */
} GNUC_PACKED;

typedef struct iso9660_pvd_s  iso9660_pvd_t;

/*!
 \brief ISO-9660 Supplementary Volume Descriptor.

 This is used for Joliet Extentions and is almost the same as the
 the primary descriptor but two unused fields, "unused1" and "unused3
 become "flags and "escape_sequences" respectively.
 */
struct iso9660_svd_s {
    iso711_t     type;                         /**< ISO_VD_SUPPLEMENTARY - 2 */
    char         id[5];                        /**< ISO_STANDARD_ID "CD001" */
    iso711_t     version;                      /**< value 1 */
    char         flags;                        /**< Section 8.5.3 */
    achar_t      system_id[ISO_MAX_SYSTEM_ID]; /**< Section 8.5.4; each char
                                                is an achar */
    dchar_t      volume_id[ISO_MAX_VOLUME_ID]; /**< Section 8.5.5; each char
                                                is a dchar */
    char         unused2[8];
    iso733_t     volume_space_size;            /**< total number of sectors */
    char         escape_sequences[32];         /**< Section 8.5.6 */
    iso723_t     volume_set_size;              /**< often 1 */
    iso723_t     volume_sequence_number;       /**< often 1 */
    iso723_t     logical_block_size;           /**< sector size, e.g. 2048 */
    iso733_t     path_table_size;              /**< 8.5.7; bytes in path
                                                table */
    iso731_t     type_l_path_table;            /**< 8.5.8; first sector of
                                                little-endian path table */
    iso731_t     opt_type_l_path_table;        /**< 8.5.9; first sector of
                                                optional little-endian path
                                                table */
    iso732_t     type_m_path_table;            /**< 8.5.10; first sector of
                                                big-endian path table */
    iso732_t     opt_type_m_path_table;        /**< 8.5.11; first sector of
                                                optional big-endian path
                                                table */
    iso9660_dir_t    root_directory_record;    /**< See section 8.5.12 and
                                                9.1 of ISO 9660 spec. */
    char             root_directory_filename;  /**< Is '\\0' or root directory.
                                                Also pads previous field to 34
                                                bytes */
    dchar_t  volume_set_id[ISO_MAX_VOLUMESET_ID];  /**< 8.5.13; dchars */
    achar_t  publisher_id[ISO_MAX_PUBLISHER_ID];   /**< Publisher of volume. If
                                                    the first chararacter is
                                                    '_' 0x5F, the remaining
                                                    bytes specify a file
                                                    containing the user. If all
                                                    bytes are " " (0x20) no
                                                    publisher is specified. See
                                                    section 8.5.14 of ECMA
                                                    119 */
    achar_t  preparer_id[ISO_MAX_PREPARER_ID]; /**< Data preparer of volume. If
                                                the first character is '_'
                                                0x5F, the remaining bytes
                                                specify a file containing the
                                                user. If all bytes are " "
                                                (0x20) no preparer is
                                                specified. See section 8.5.15
                                                of ECMA 119 */
    achar_t  application_id[ISO_MAX_APPLICATION_ID]; /**< Application used to
                                                      create the volume. If the
                                                      first character is '_'
                                                      0x5F, the remaining bytes
                                                      specify a file containing
                                                      the user. If all bytes
                                                      are " " (0x20) no
                                                      application is specified.
                                                      See section of 8.5.16 of
                                                      ECMA 119 */
    dchar_t  copyright_file_id[37];     /**< Name of file for copyright info.
                                         If all bytes are " " (0x20), then no
                                         file is identified. See section 8.5.17
                                         of ECMA 119 9660 spec. */
    dchar_t  abstract_file_id[37];      /**< See section 8.5.18 of ECMA 119. */
    dchar_t  bibliographic_file_id[37]; /**< See section 8.5.19 of ECMA 119. */
    iso9660_ltime_t  creation_date;     /**< Date and time of volume creation.
                                         See section 8.4.26.1 of the ECMA 119
                                         spec. */
    iso9660_ltime_t  modification_date; /**< Date and time of the most recent
                                         modification. See section 8.4.27 of
                                         the ECMA 119 spec. */
    iso9660_ltime_t  expiration_date;   /**< Date and time when volume expires.
                                         See section 8.4.28 of the ECMA 119
                                         spec. */
    iso9660_ltime_t  effective_date;    /**< Date and time when volume is
                                         effective. See section 8.4.29 of the
                                         ECMA 119 spec. */
    iso711_t         file_structure_version;    /**< value 1 for ECMA 119 */
    uint8_t          unused4[1];                /**< unused - value 0 */
    char             application_data[512];     /**< 8.5.20 Application can put
                                                 whatever it wants here. */
    uint8_t          unused5[653];              /**< Unused - value 0 */
} GNUC_PACKED;

typedef struct iso9660_svd_s  iso9660_svd_t;

/* Implementation of iso9660_t type */
struct _iso9660_s {
    FILE *stream;             /* Stream pointer */
    bool_3way_t b_xa;         /* true if has XA attributes. */
    bool_3way_t b_mode2;      /* true if has mode 2, false for mode 1. */
    uint8_t  i_joliet_level;  /* 0 = no Joliet extensions.
                               1-3: Joliet level. */
    iso9660_pvd_t pvd;
    iso9660_svd_t svd;
    iso_extension_mask_t iso_extension_mask; /* What extensions we tolerate. */
    uint32_t i_datastart;     /* Usually 0 when i_framesize is ISO_BLOCKSIZE.
                               This is the normal condition. But in a fuzzy
                               read we may be reading a CD-image and not a true
                               ISO 9660 image this might be
                               CDIO_CD_SYNC_SIZE */
    uint32_t i_framesize;     /* Usually ISO_BLOCKSIZE (2048), but in a fuzzy
                               read, we may be reading a CD-image and not a
                               true ISO 9660 image this might be
                               CDIO_CD_FRAMESIZE_RAW (2352) or
                               M2RAW_SECTOR_SIZE (2336). */
    int i_fuzzy_offset;       /* Adjustment in bytes to make ISO_STANDARD_ID
                               ("CD001") come out as ISO_PVD_SECTOR (frame 16).
                               Normally this should be 0 for an ISO 9660 image,
                               but if one is say reading a BIN/CUE or cdrdao
                               BIN/TOC without having the CUE or TOC and trying
                               to extract an ISO-9660 filesystem inside that it
                               may be different. */
};

typedef iso9660_stat_t * (stat_root_t) (void *p_image);
typedef iso9660_stat_t * (stat_traverse_t) (const void *p_image,
                                            const iso9660_stat_t *_root,
                                            char **splitpath);


/*! String of bytes used to identify the beginning of a Mode 1 or
 Mode 2 sector. */
const uint8_t CDIO_SECTOR_SYNC_HEADER[CDIO_CD_SYNC_SIZE] =
{0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0};


/*!
 Seek to a position and then read n blocks. Size read is returned.
 */
static long int
iso9660_seek_read_framesize(const iso9660_t *p_iso, void *ptr, lsn_t start,
                            long int size, uint16_t i_framesize) {
    long int ret;
    int64_t i_byte_offset;

    if (!p_iso) {
        return 0;
    }
    i_byte_offset = (start * p_iso->i_framesize) + p_iso->i_fuzzy_offset +
    p_iso->i_datastart;

    ret = fseek(p_iso->stream, i_byte_offset, SEEK_SET);
    if (ret != 0) {
        return 0;
    }
    size_t r = fread(ptr, i_framesize, size, p_iso->stream);
    return r * i_framesize;
}

/*!
 Seek to a position and then read n blocks. Size read is returned.
 */
long int
iso9660_iso_seek_read(const iso9660_t *p_iso, void *ptr, lsn_t start,
                      long int size) {
    return iso9660_seek_read_framesize(p_iso, ptr, start, size, ISO_BLOCKSIZE);
}

static bool
check_pvd(const iso9660_pvd_t *p_pvd) {
    if (ISO_VD_PRIMARY != p_pvd->type) {
        return false;
    }

    if (strncmp(p_pvd->id, ISO_STANDARD_ID, strlen(ISO_STANDARD_ID))) {
        return false;
    }
    return true;
}

/*!
 Read the Primary Volume Descriptor for an ISO 9660 image.
 True is returned if read, and false if there was an error.
 */
static bool
iso9660_ifs_read_pvd(const iso9660_t *p_iso, /*out*/ iso9660_pvd_t *p_pvd) {
    if (0 == iso9660_iso_seek_read(p_iso, p_pvd, ISO_PVD_SECTOR, 1)) {
        return false;
    }
    return check_pvd(p_pvd);
}

/* Adjust the p_iso's i_datastart, i_byte_offset and i_framesize
 based on whether we find a frame header or not.
 */
static void
adjust_fuzzy_pvd(iso9660_t *p_iso) {
    long int i_byte_offset;

    if (!p_iso) {
        return;
    }

    i_byte_offset = (ISO_PVD_SECTOR * p_iso->i_framesize) +
    p_iso->i_fuzzy_offset + p_iso->i_datastart;

    /* If we have a raw 2352-byte frame then we should expect to see a sync
     frame and a header.
     */
    if (CDIO_CD_FRAMESIZE_RAW == p_iso->i_framesize) {
        char buf[CDIO_CD_SYNC_SIZE + CDIO_CD_HEADER_SIZE +
                 CDIO_CD_SUBHEADER_SIZE];

        i_byte_offset -= CDIO_CD_SYNC_SIZE + CDIO_CD_HEADER_SIZE +
        CDIO_CD_SUBHEADER_SIZE;

        fseek(p_iso->stream, i_byte_offset, SEEK_SET);

        if (sizeof(buf) == fread (buf, sizeof(buf), 1, p_iso->stream)) {
            /* Does the sector frame header suggest Mode 1 format? */
            if (!memcmp(CDIO_SECTOR_SYNC_HEADER,
                        buf + CDIO_CD_SUBHEADER_SIZE,
                        CDIO_CD_SYNC_SIZE)) {
                p_iso->b_mode2 = nope;
                p_iso->b_xa = nope;
            } else if (!memcmp(CDIO_SECTOR_SYNC_HEADER,
                               buf, CDIO_CD_SYNC_SIZE)) {
                /* Frame header indicates Mode 2 Form 1*/
                p_iso->b_mode2 = yep;
                /* Do do: check Mode 2 Form 2? */
            } else {
                /* Has no frame header */
                p_iso->i_framesize = M2RAW_SECTOR_SIZE;
                p_iso->i_fuzzy_offset =
                ((CDIO_CD_FRAMESIZE_RAW - M2RAW_SECTOR_SIZE) *
                 ISO_PVD_SECTOR) +
                p_iso->i_fuzzy_offset + p_iso->i_datastart;
                p_iso->i_datastart = 0;
            }
        }
    }
}

/*!
 Read the Super block of an ISO 9660 image but determine framesize and
 datastart and a possible additional offset. Generally here we are not reading
 an ISO 9660 image but a CD-Image which contains an ISO 9660 filesystem.
 */
bool
iso9660_ifs_fuzzy_read_superblock(iso9660_t *p_iso,
                                  iso_extension_mask_t iso_extension_mask,
                                  uint16_t i_fuzz) {
    /* Got some work to do to find ISO_STANDARD_ID ("CD001") */
    unsigned int i;

    for (i = 0; i < i_fuzz; i++) {
        unsigned int j;
        char *pvd = NULL;

        for (j = 0; j <= 1; j++) {
            lsn_t lsn;
            uint16_t k;
            const uint16_t framesizes[] =
                { ISO_BLOCKSIZE, CDIO_CD_FRAMESIZE_RAW, M2RAW_SECTOR_SIZE };

            /* We don't need to loop over a zero offset twice*/
            if (0 == i && j) {
                continue;
            }

            lsn = (j) ? ISO_PVD_SECTOR - i : ISO_PVD_SECTOR + i;

            for (k = 0; k < 3; k++) {
                char *p, *q;
                char frame[CDIO_CD_FRAMESIZE_RAW] = {'\0', };
                p_iso->i_framesize = framesizes[k];
                p_iso->i_datastart = (ISO_BLOCKSIZE == framesizes[k]) ?
                                     0 : CDIO_CD_SYNC_SIZE;
                p_iso->i_fuzzy_offset = 0;
                if (0 == iso9660_seek_read_framesize(p_iso, frame, lsn, 1,
                                                     p_iso->i_framesize)) {
                    return false;
                }

                q = memchr(frame, 'C', p_iso->i_framesize);
                for (p=q; p && p < frame + p_iso->i_framesize ; p = q + 1) {
                    q = memchr(p, 'C', p_iso->i_framesize - (p - frame));
                    if (!q || (pvd = strstr(q, ISO_STANDARD_ID))) {
                        break;
                    }
                }

                if (pvd) {
                    /* Yay! Found something */
                    p_iso->i_fuzzy_offset = (pvd - frame - 1) -
                        ((ISO_PVD_SECTOR - lsn) * p_iso->i_framesize);
                    /* But is it *really* a PVD? */
                    if (iso9660_ifs_read_pvd(p_iso, &(p_iso->pvd))) {
                        adjust_fuzzy_pvd(p_iso);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


/*!
 Read the Super block of an ISO 9660 image. This is the
 Primary Volume Descriptor (PVD) and perhaps a Supplemental Volume
 Descriptor if (Joliet) extensions are acceptable.
 */
bool
iso9660_ifs_read_superblock(iso9660_t *p_iso,
                            iso_extension_mask_t iso_extension_mask) {
    iso9660_svd_t p_svd;  /* Secondary volume descriptor. */
    int i;

    if (!p_iso || !iso9660_ifs_read_pvd(p_iso, &(p_iso->pvd))) {
        return false;
    }

    p_iso->i_joliet_level = 0;

    /* There may be multiple Secondary Volume Descriptors
     (eg. El Torito + Joliet) */
    for (i = 1;
         0 != iso9660_iso_seek_read(p_iso, &p_svd, ISO_PVD_SECTOR + i, 1);
         i++) {
        if (ISO_VD_END == p_svd.type) {
            /* Last SVD */
            break;
        }
        if (ISO_VD_SUPPLEMENTARY == p_svd.type) {
            /* We're only interested in Joliet => make sure the SVD isn't
             overwritten */
            if (p_iso->i_joliet_level == 0) {
                memcpy(&(p_iso->svd), &p_svd, sizeof(iso9660_svd_t));
            }
            if (p_svd.escape_sequences[0] == 0x25 &&
                    p_svd.escape_sequences[1] == 0x2f) {
                switch (p_svd.escape_sequences[2]) {
                    case 0x40:
                        if (iso_extension_mask & ISO_EXTENSION_JOLIET_LEVEL1) {
                            p_iso->i_joliet_level = 1;
                        }
                        break;
                    case 0x43:
                        if (iso_extension_mask & ISO_EXTENSION_JOLIET_LEVEL2) {
                            p_iso->i_joliet_level = 2;
                        }
                        break;
                    case 0x45:
                        if (iso_extension_mask & ISO_EXTENSION_JOLIET_LEVEL3) {
                            p_iso->i_joliet_level = 3;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return true;
}


/*!
 Open an ISO 9660 image for reading in either fuzzy mode or not.
 */
static iso9660_t *
iso9660_open_ext_private(const char *psz_path,
                         iso_extension_mask_t iso_extension_mask,
                         uint16_t i_fuzz, bool b_fuzzy) {
    iso9660_t *p_iso = (iso9660_t *) calloc(1, sizeof(iso9660_t));
    bool b_have_superblock;

    if (!p_iso) {
        return NULL;
    }

    p_iso->stream = fopen(psz_path , "rb");
    if (NULL == p_iso->stream) {
        goto error;
    }

    p_iso->i_framesize = ISO_BLOCKSIZE;

    b_have_superblock =
        (b_fuzzy) ?
          iso9660_ifs_fuzzy_read_superblock(p_iso, iso_extension_mask, i_fuzz)
        : iso9660_ifs_read_superblock(p_iso, iso_extension_mask);

    if (!b_have_superblock) {
        goto error;
    }

    /* Determine if image has XA attributes. */

    p_iso->b_xa = strncmp((char *) &(p_iso->pvd) + ISO_XA_MARKER_OFFSET,
                          ISO_XA_MARKER_STRING, sizeof(ISO_XA_MARKER_STRING))
                  ? nope : yep;

    p_iso->iso_extension_mask = iso_extension_mask;
    return p_iso;

error:
    if (p_iso && p_iso->stream) {
        fclose(p_iso->stream);
    }
    free(p_iso);

    return NULL;
}

/*!
 Open an ISO 9660 image for reading. Maybe in the future we will have a mode.
 NULL is returned on error.
 */
iso9660_t *
iso9660_open_ext(const char *psz_path,
                 iso_extension_mask_t iso_extension_mask) {
    return iso9660_open_ext_private(psz_path, iso_extension_mask, 0, false);
}

/*!
 Open an ISO 9660 image for reading. Maybe in the future we will have
 a mode. NULL is returned on error.
 */
iso9660_t *
iso9660_open(const char *psz_path /*, mode*/) {
    return iso9660_open_ext(psz_path, ISO_EXTENSION_NONE);
}

/*!
 Close previously opened ISO 9660 image.
 True is unconditionally returned. If there was an error false would
 be returned.
 */
bool
iso9660_close(iso9660_t *p_iso) {
    if (NULL != p_iso) {
        fclose(p_iso->stream);
        free(p_iso);
    }
    return true;
}

void
_cdio_strfreev(char **strv) {
    int n;

    for (n = 0; strv[n]; n++) {
        free(strv[n]);
    }

    free(strv);
}

char **
_cdio_strsplit(const char str[], char delim) {
    int n;
    char **strv = NULL;
    char *_str, *p;
    char _delim[2] = { 0, 0 };

    _str = strdup(str);
    _delim[0] = delim;

    n = 1;
    p = _str;
    while (*p) {
        if (*(p++) == delim) {
            n++;
        }
    }

    strv = calloc (1, sizeof (char *) * (n+1));

    n = 0;
    while ((p = strtok(n ? NULL : _str, _delim)) != NULL) {
        strv[n++] = strdup(p);
    }

    free(_str);

    return strv;
}

/*!
 Get file status for psz_path into stat. NULL is returned on error.
 pathname version numbers in the ISO 9660
 name are dropped, i.e. ;1 is removed and if level 1 ISO-9660 names
 are lowercased.
 */
static iso9660_stat_t *
fs_stat_translate(void *p_image, stat_root_t stat_root,
                  stat_traverse_t stat_traverse, const char psz_path[]) {
    iso9660_stat_t *p_root;
    char **p_psz_splitpath;
    iso9660_stat_t *p_stat;

    if (!p_image) {
        return NULL;
    }
    if (!psz_path) {
        return NULL;
    }

    p_root = stat_root(p_image);
    if (!p_root) {
        return NULL;
    }

    p_psz_splitpath = _cdio_strsplit(psz_path, '/');
    p_stat = stat_traverse(p_image, p_root, p_psz_splitpath);
    free(p_root);
    _cdio_strfreev(p_psz_splitpath);

    return p_stat;
}

uint8_t
iso9660_get_dir_len(const iso9660_dir_t *idr) {
    if (NULL == idr) {
        return 0;
    }
    return idr->length;
}

static
uint64_t uint64_swap_le_be(const uint64_t val) {
    return UINT64_SWAP_LE_BE(val);
}

/** Convert from ISO 9660 7.3.3 format to uint32_t */
static uint32_t
from_733(uint64_t p) {
    uint64_swap_le_be(p);
    return (UINT32_C(0xFFFFFFFF) & p);
}

static uint32_t
_cdio_len2blocks(uint32_t i_len, uint16_t i_blocksize) {
    uint32_t i_blocks;

    i_blocks = i_len / (uint32_t) i_blocksize;
    if (i_len % i_blocksize) {
        i_blocks++;
    }

    return i_blocks;
}

/*!
 Get time structure from structure in an ISO 9660 directory index
 record. Even though tm_wday and tm_yday fields are not explicitly in
 idr_date, the are calculated from the other fields.

 If tm is to reflect the localtime set b_localtime true, otherwise
 tm will reported in GMT.
 */
bool
iso9660_get_dtime(const iso9660_dtime_t *idr_date, bool b_localtime,
                  /*out*/ struct tm *p_tm) {
    if (!idr_date) {
        return false;
    }

    /*
     Section 9.1.5 of ECMA 119 says:
     If all seven numbers are zero, it shall mean that the date and
     time are not specified.

     HACK: However we've seen it happen that everything except gmtoff
     is zero and the expected date is the beginning of the epoch. So
     we accept 6 numbers being zero. I'm also not sure if using the
     beginning of the Epoch is also the right thing to do either.
     */

    if (0 == idr_date->dt_year   && 0 == idr_date->dt_month &&
        0 == idr_date->dt_day    && 0 == idr_date->dt_hour  &&
        0 == idr_date->dt_minute && 0 == idr_date->dt_second ) {
        time_t t = 0;
        struct tm temp_tm;
        localtime_r(&t, &temp_tm);

        memcpy(p_tm, &temp_tm, sizeof(struct tm));
        return true;
    }

    memset(p_tm, 0, sizeof(struct tm));

    p_tm->tm_year   = idr_date->dt_year;
    p_tm->tm_mon    = idr_date->dt_month - 1;
    p_tm->tm_mday   = idr_date->dt_day;
    p_tm->tm_hour   = idr_date->dt_hour;
    p_tm->tm_min    = idr_date->dt_minute;
    p_tm->tm_sec    = idr_date->dt_second - idr_date->dt_gmtoff * (15 * 60);
    p_tm->tm_isdst  = -1; /* information not available */

    /* Recompute tm_wday and tm_yday via mktime. mktime will also renormalize
     date values to account for the timezone offset. */
    {
        time_t t = 0;
        struct tm temp_tm;

        t = timegm(p_tm);

        if (b_localtime) {
            localtime_r(&t, &temp_tm);
        } else {
            gmtime_r(&t, &temp_tm);
        }

        memcpy(p_tm, &temp_tm, sizeof(struct tm));
    }

    return true;
}

static iso9660_stat_t *
_iso9660_dir_to_statbuf(iso9660_dir_t *p_iso9660_dir, bool_3way_t b_xa,
                        uint8_t i_joliet_level) {
    uint8_t dir_len = iso9660_get_dir_len(p_iso9660_dir);
    iso711_t i_fname;
    unsigned int stat_len;
    iso9660_stat_t *p_stat;

    if (!dir_len) {
        return NULL;
    }

    i_fname  = p_iso9660_dir->filename.len;

    /* .. string in statbuf is one longer than in p_iso9660_dir's listing
     '\1' */
    stat_len = sizeof(iso9660_stat_t) + i_fname + 2;

    p_stat = calloc(1, stat_len);
    if (!p_stat) {
        return NULL;
    }
    p_stat->type    = (p_iso9660_dir->file_flags & ISO_DIRECTORY)
                      ? _STAT_DIR : _STAT_FILE;
    p_stat->lsn     = from_733(p_iso9660_dir->extent);
    p_stat->size    = from_733(p_iso9660_dir->size);
    p_stat->secsize = _cdio_len2blocks(p_stat->size, ISO_BLOCKSIZE);
    p_stat->rr.b3_rock  = dunno; /*FIXME should do based on mask */
    p_stat->b_xa    = false;

    {
        char rr_fname[256] = "";

        int  i_rr_fname =
#ifdef HAVE_ROCK
        get_rock_ridge_filename(p_iso9660_dir, rr_fname, p_stat);
#else
        0;
#endif

        if (i_rr_fname > 0) {
            if (i_rr_fname > i_fname) {
                /* realloc gives valgrind errors */
                iso9660_stat_t *p_stat_new =
                    calloc(1, sizeof(iso9660_stat_t) + i_rr_fname + 2);
                if (!p_stat_new) {
                    return NULL;
                }
                memcpy(p_stat_new, p_stat, stat_len);
                free(p_stat);
                p_stat = p_stat_new;
            }
            strncpy(p_stat->filename, rr_fname, i_rr_fname+1);
        } else {
            if ('\0' == p_iso9660_dir->filename.str[1] && 1 == i_fname) {
                strncpy(p_stat->filename, ".", sizeof("."));
            } else if ('\1' == p_iso9660_dir->filename.str[1] && 1 == i_fname) {
                strncpy (p_stat->filename, "..", sizeof(".."));
            }
#ifdef HAVE_JOLIET
            else if (i_joliet_level) {
                int i_inlen = i_fname;
                cdio_utf8_t *p_psz_out = NULL;
                if (cdio_charset_to_utf8(&p_iso9660_dir->filename.str[1],
                                         i_inlen, &p_psz_out, "UCS-2BE")) {
                    strncpy(p_stat->filename, p_psz_out, i_fname);
                    free(p_psz_out);
                } else {
                    free(p_stat);
                    return NULL;
                }
            }
#endif /*HAVE_JOLIET*/
            else {
                strncpy(p_stat->filename,
                        &p_iso9660_dir->filename.str[1], i_fname);
            }
        }
    }


    iso9660_get_dtime(&(p_iso9660_dir->recording_time), true, &(p_stat->tm));

    if (dir_len < sizeof (iso9660_dir_t)) {
        free(p_stat->rr.psz_symlink);
        free(p_stat);
        return NULL;
    }


    {
        int su_length = iso9660_get_dir_len(p_iso9660_dir)
        - sizeof (iso9660_dir_t);
        su_length -= i_fname;

        if (su_length % 2) {
            su_length--;
        }

        if (su_length < 0 || su_length < sizeof (iso9660_xa_t)) {
            return p_stat;
        }

        if (nope == b_xa) {
            return p_stat;
        } else {
            iso9660_xa_t *xa_data =
                (void *) (((char *) p_iso9660_dir) +
                          (iso9660_get_dir_len(p_iso9660_dir) - su_length));

            if (xa_data->signature[0] != 'X' || xa_data->signature[1] != 'A') {
                return p_stat;
            }
            p_stat->b_xa = true;
            p_stat->xa   = *xa_data;
        }
    }
    return p_stat;
}


static iso9660_stat_t *
_ifs_stat_root(iso9660_t *p_iso) {
    iso9660_stat_t *p_stat;
    iso9660_dir_t *p_iso9660_dir;

#ifdef HAVE_JOLIET
    p_iso9660_dir = p_iso->i_joliet_level ?
          &(p_iso->svd.root_directory_record)
        : &(p_iso->pvd.root_directory_record);
#else
    p_iso9660_dir = &(p_iso->pvd.root_directory_record);
#endif

    p_stat = _iso9660_dir_to_statbuf(p_iso9660_dir,
                                     p_iso->b_xa, p_iso->i_joliet_level);
    return p_stat;
}


/*!
 Convert an ISO-9660 file name which is in the format usually stored
 in a ISO 9660 directory entry into what's usually listed as the
 file name in a listing.  Lowercase name if no Joliet Extension
 interpretation. Remove trailing ;1's or .;1's and turn the other
 ;'s into version numbers.

 @param psz_oldname the ISO-9660 filename to be translated.
 @param psz_newname returned string. The caller allocates this and
 it should be at least the size of psz_oldname.
 @param i_joliet_level 0 if not using Joliet Extension. Otherwise the
 Joliet level.
 @return length of the translated string is returned. It will be no greater
 than the length of psz_oldname.
 */
int
iso9660_name_translate_ext(const char *psz_oldname, char *psz_newname,
                           uint8_t i_joliet_level) {
    int len = strlen(psz_oldname);
    int i;

    if (0 == len) return 0;
    for (i = 0; i < len; i++) {
        unsigned char c = psz_oldname[i];
        if (!c) {
            break;
        }

        /* Lower case, unless we have Joliet extensions.  */
        if (!i_joliet_level && isupper(c)) {
            c = tolower(c);
        }

        /* Drop trailing '.;1' (ISO 9660:1988 7.5.1 requires period) */
        if (c == '.' && i == len - 3 && psz_oldname[i + 1] == ';'
                && psz_oldname[i + 2] == '1') {
            break;
        }

        /* Drop trailing ';1' */
        if (c == ';' && i == len - 2 && psz_oldname[i + 1] == '1') {
            break;
        }

        /* Convert remaining ';' to '.' */
        if (c == ';') {
            c = '.';
        }

        psz_newname[i] = c;
    }
    psz_newname[i] = '\0';
    return i;
}


static iso9660_stat_t *
_fs_iso_stat_traverse(iso9660_t *p_iso, const iso9660_stat_t *_root,
                      char **splitpath) {
    unsigned offset = 0;
    uint8_t *_dirbuf = NULL;
    int ret;

    if (!splitpath[0]) {
        iso9660_stat_t *p_stat;
        size_t len = sizeof(iso9660_stat_t) + strlen(_root->filename) + 1;
        p_stat = calloc(1, len);
        if (!p_stat) {
            return NULL;
        }
        memcpy(p_stat, _root, len);
        p_stat->rr.psz_symlink = calloc(1, p_stat->rr.i_symlink_max);
        memcpy(p_stat->rr.psz_symlink,
               _root->rr.psz_symlink, p_stat->rr.i_symlink_max);
        return p_stat;
    }

    if (_root->type == _STAT_FILE) {
        return NULL;
    }

    _dirbuf = calloc(1, _root->secsize * ISO_BLOCKSIZE);
    if (!_dirbuf) {
        return NULL;
    }

    ret = iso9660_iso_seek_read(p_iso, _dirbuf, _root->lsn, _root->secsize);

    if (ret != ISO_BLOCKSIZE*_root->secsize)
        return NULL;

    while (offset < (_root->secsize * ISO_BLOCKSIZE)) {
        iso9660_dir_t *p_iso9660_dir = (void *) &_dirbuf[offset];
        iso9660_stat_t *p_stat;
        int cmp;

        if (!iso9660_get_dir_len(p_iso9660_dir)) {
            offset++;
            continue;
        }

        p_stat = _iso9660_dir_to_statbuf(p_iso9660_dir,
                                         p_iso->b_xa,
                                         p_iso->i_joliet_level);

        cmp = strcmp(splitpath[0], p_stat->filename);

        if (0 != cmp && 0 == p_iso->i_joliet_level &&
            yep != p_stat->rr.b3_rock ) {
            char *trans_fname = NULL;
            int i_trans_fname = strlen(p_stat->filename);

            if (i_trans_fname) {
                trans_fname = calloc(1, i_trans_fname+1);
                if (!trans_fname) {
                    free(p_stat);
                    return NULL;
                }
                iso9660_name_translate_ext(p_stat->filename,
                                           trans_fname,
                                           p_iso->i_joliet_level);
                cmp = strcmp(splitpath[0], trans_fname);
                free(trans_fname);
            }
        }

        if (!cmp) {
            iso9660_stat_t *ret_stat =
            _fs_iso_stat_traverse(p_iso, p_stat, &splitpath[1]);
            free(p_stat->rr.psz_symlink);
            free(p_stat);
            free(_dirbuf);
            return ret_stat;
        }

        free(p_stat->rr.psz_symlink);
        free(p_stat);

        offset += iso9660_get_dir_len(p_iso9660_dir);
    }

    /* not found */
    free(_dirbuf);
    return NULL;
}

/*!
 Get file status for psz_path into stat. NULL is returned on error.
 pathname version numbers in the ISO 9660
 name are dropped, i.e. ;1 is removed and if level 1 ISO-9660 names
 are lowercased.
 */
iso9660_stat_t *
iso9660_ifs_stat_translate(iso9660_t *p_iso, const char psz_path[]) {
    return fs_stat_translate(p_iso,
                             (stat_root_t *) _ifs_stat_root,
                             (stat_traverse_t *) _fs_iso_stat_traverse,
                             psz_path);
}


char* isofetch(char* iso_fpath, char* entry_name, int* size) {
    iso9660_t* p_iso = iso9660_open(iso_fpath);

    if (p_iso == NULL) {
        return NULL;
    }

    iso9660_stat_t* p_statbuf = iso9660_ifs_stat_translate(p_iso, entry_name);
    if (p_statbuf == NULL) {
        iso9660_close(p_iso);
        return NULL;
    }

    *size = p_statbuf->size;
    char* data = (char *) malloc(*size);

    int i;

    for (i = 0; i < *size; i += ISO_BLOCKSIZE) {
        char buf[ISO_BLOCKSIZE];
        if (iso9660_iso_seek_read(p_iso, buf,
                                  p_statbuf->lsn + (i / ISO_BLOCKSIZE), 1)
                != ISO_BLOCKSIZE) {
            free(data);
            iso9660_close(p_iso);
            return NULL;
        }
        if (i + ISO_BLOCKSIZE <= *size) {
            memcpy(data + i, buf, ISO_BLOCKSIZE);
        } else {
            memcpy(data + i, buf, *size % ISO_BLOCKSIZE);
        }
    }

    iso9660_close(p_iso);
    return data;
}
