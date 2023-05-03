#ifndef VERSION_H
#define VERSION_H

// this file has been symlinked to each main folder in the MCUs directory to allow for easy versioning of the software
// ran the commands 'cd /path/to/MCUs/{REPLACE-WITH-SPECIFIC-BOARD}/main' followed by `ln -s ../../src/version.h version.h` to create the symlinks

// Variable to store the release version of the software running on the MCUs
#define RELEASE_VERSION "2.0.1" 

#endif // VERSION_H