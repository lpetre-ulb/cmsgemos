#ifndef XHAL_PACKAGEINFO_H
#define XHAL_PACKAGEINFO_H

#ifndef DOXYGEN_IGNORE_THIS

#define XHAL_REQUIRED_PACKAGE_LIST reedmuller,xerces-c
#define XHAL_BASE_REQUIRED_PACKAGE_LIST reedmuller,xerces-c
#define XHAL_CLIENT_REQUIRED_PACKAGE_LIST xhal-base,reedmuller,xerces-c
#define XHAL_SERVER_REQUIRED_PACKAGE_LIST xhal-base,reedmuller,xerces-c,lmdb

#define XHAL_BUILD_REQUIRED_PACKAGE_LIST reedmuller-devel,gem-peta-stage-ctp7
#define XHAL_BASE_BUILD_REQUIRED_PACKAGE_LIST reedmuller-devel,gem-peta-stage-ctp7
#define XHAL_CLIENT_BUILD_REQUIRED_PACKAGE_LIST reedmuller-devel,gem-peta-stage-ctp7
#define XHAL_SERVER_BUILD_REQUIRED_PACKAGE_LIST reedmuller-devel,lmdb-devel,gem-peta-stage-ctp7

#endif

#endif
