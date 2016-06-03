Tiled Data Distribution System (TDDS) Zipfile Content README

Table of Contents

INTRODUCTION
PART 1: TILED DATA DISTRIBUTION SYSTEM (TDDS) - ORTHOIMAGERY INFORMATION
Part 2: FILE NAMING CONVENTION
PART 3: CONTENTS OF ZIPFILE
PART 4: DISTRIBUTION INFORMATION


INTRODUCTION:
A digital orthoimage is a geometrically accurate photographic record of landscape conditions at the time of 
the corresponding aerial photography. As such, High Resolution Orthoimagery is useful for a variety of applications, 
such as environmental monitoring, facility engineering/maintenance, city/county planning, property line review, etc. 
The digital orthoimage can be used alone or as a raster basemap for corresponding vector line mapping.  The detailed 
focus of High Resolution Orthoimagery provides emergency responders critical information in determining the best 
evacuation routes, alternative routes and safe access to aid.  High Resolution Orthoimagery assists law enforcement personnel 
in determining the best locations to place surveillance cameras in high-traffic urban areas and popular attractions.  The data assists Federal, State and local emergency responders in planning for homeland security efforts.  This data also supports The National Map. The purpose of this package of data is to provide the High Resolution Orthoimagery (most current) imagery and ancillary files.  Orthoimgery is acquired in cycles.  As new data becomes available, they will be added to TDDS. Data are compressed utilizing IAS software. The compression was JPEG2000 Lossy Compressed (~5:1). The file format created is .jp2

PART 1: TILED DATA DISTRIBUTION SYSTEM (TDDS) - ORTHOIMAGERY INFORMATION
Tiled Data Distribution System (TDDS) is a means to access Orthoimagery (most current) data. As Orthoimagery data are 
received they will be processed to a consistent projection and naming convention and available for web map services.  As newer data is received it will replace older (historical) versions, the historical data is removed from The Seamless Server map interface and web map services.  Then added to TDDS as a consistent projection and naming convention.  The grid and naming convention is utilizing the U.S. National Grid (USNG), which is a universal grid and coordinate system.  The processed data, projection files, shapefiles, and metadata are packaged in a zipfile.  The zipfile is then accessible and downloadable through a text based interface (http://extract.cr.usgs.gov/hrotdownloadtool/HROTDownloadTool.php), application services (http://gisdata.usgs.gov/app_services.php), and The National Map (http://nationalmap.gov/viewer.html).  


PART 2: FILE NAMING CONVENTION

The U.S. National GRID is utilized as the part of the naming convention due to being a nationally consistent grid reference system.  The USNG is an alpha-numeric reference system that overlays the UTM coordinate system.  More information concerning the U.S. National GRID can be found at http://www.fgdc.gov/usng
The file naming convention for the TDDS zipfile is as follows:

	uuuFFnnnnnn_YYYYMM_0xaaam_BB_v

		uuu       =  Grid Zone Designation (first 2 numbers indicate the utm zone)
		FF        =  100,000-meter Square Identification
		nnnnnn    =  Grid coordinates of some even number of digits ranging from 2 to 10.  Six are used for this 			     purpose. (Read Grid Coordinates right and then up)
		YYYY      =  Year imagery was acquired
		MM        =  Month imagery was acquired
                0xaaaam   =  Resolution in meters using the x as a decimal
		BB        =  Type of imagery. (i.e. CL for Natural Color, BW for Black & White, CR for Color Infrared, CI for  			     Colorized Color Infrared)
                v         =  Indicates version number of the image.  (Majority of the imagery will have a _1 version number, 			     but when an image covers the same grid and same date, the image will have _2 and so forth)

  	Example: 17RMW650470_200309_0x0750m_CL_1.zip 

			17R      =  UTM zone 17, GRID R
	 		MQ       =  100,000-meter Square Identification, within UTM zone 17 GRID R
			650470   =  Grid coordinates within the 100,000 meter square identification, within UTM zone 17 GRID                                      r -- 6 digits - locates a point with a precision of 100-meters (a soccer field size area)
                       	2003     =  Acquired in 2003
                     	09       =  Acquired in September
                        0x0750m  =  Resolution of 0.0750 meters
                        CL       =  Natural Color
                        1        =  Image version 1
 			           

PART 3: CONTENTS OF ZIPFILE

	
	.jp2  	=  JPEG2000 Lossy Compressed (~5:1)
	.j2w    =  JPEG2000 world file
	.jpg	=  Browse image 
	.jgw    =  World file for browse image
	.xml    =  FGDC Metadata file 
	.htm    =  HTML format FGDC metadata file suitable for reading and printing
	Shapefile
	 (.dbf, .prj, .sbn, .sbx, .shp, .shx)
		=  The shapefile (consisting of 6 files) represents the GRID Zone Designation and 100,000 meter square                    identification. Using the example file name above, the shapefile is the 17RMQ square.
	OrthoReadme.txt 
		=  The readme text file

PART 4: DISTRIBUTION INFORMATION

Access points (i.e. download tools, download applicatin services, and The National Map interface, kml) can be found at:
	(http://extract.cr.usgs.gov/hrotdownloadtool/HROTDownloadTool.php), (http://gisdata.usgs.gov/app_services.php), and
        (http://nationalmap.gov/viewer.html)
	
To acquire entire datasets via Bulk Data Distribution email bulkdatainfo@usgs.gov



Disclaimer:  Any use of trade, product, or firm names is for descriptive 
purposes only and does not imply endorsement by the U. S. Government.    


Publication Date:  January 2013



