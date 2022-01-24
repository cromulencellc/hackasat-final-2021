/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
**  File: cfe_version.h
**
**  Purpose:
**     Provide version identifiers for the cFE core.
**
**     Version Number Semantics:
**
**     The version number is a sequence of four numbers, generally separated by dots when written. These are, in order,
**     the Major number, the Minor number, the Implementation Revision number, and the Mission Revision number. At
**     their option, Missions may modify the Mission Revision information as needed to suit their needs.
**
**     The Major number shall be incremented on release to indicate when there is a change to an API
**     that may cause existing correctly-written cFS components to stop working. It may also be incremented for a
**     release that contains changes deemed to be of similar impact, even if there are no actual changes to the API.
**
**     The Minor number shall be incremented on release to indicate the addition of features to the API,
**     which do not break the existing code.  It may also be incremented for a release that contains changes deemed
**     to be of similar impact, even if there are no actual updates to the API.
**
**     The Implementation Revision Version number shall be incremented on changes to software in the master branch,
**     or other changes that benefit from unique identification. It is used for identifying open source development
**     versions.
**
**     The Major, Minor, and Implementation Revision numbers are provided in this header file as part of the API
**     definition; this macro must expand to a simple integer value, so that it can be used in simple #if directives
**     by the macro preprocessor.
**
**     The Mission Version number shall be set to zero in all officially released packages, and is entirely reserved
**     for the use of the mission.  The Mission Version is provided as a simple macro defined in the cfe_platform_cfg.h
**     header file.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

#ifndef _cfe_version_
#define _cfe_version_

/*
 * The target config contains extended version information within it.
 * This information is generated automatically by the build using
 * git to determine the most recent tag and commit id.
 */
#include <target_config.h>


/*
** Macro Definitions
*/
#define CFE_MAJOR_VERSION         6
#define CFE_MINOR_VERSION         7
#define CFE_REVISION              10


#endif  /* _cfe_version_ */
