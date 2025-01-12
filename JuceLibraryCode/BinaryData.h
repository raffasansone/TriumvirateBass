/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Settings_icon_EFEFEF_40x40_png;
    const int            Settings_icon_EFEFEF_40x40_pngSize = 926;

    extern const char*   led_off_new_png;
    const int            led_off_new_pngSize = 4811;

    extern const char*   led_on_new_png;
    const int            led_on_new_pngSize = 4905;

    extern const char*   triumvirate_new_png;
    const int            triumvirate_new_pngSize = 509688;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 4;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
