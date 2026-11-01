/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "HF-SFM Flow Meter Driver", "index.html", [
    [ "Table of contents", "index.html#autotoc_md1", null ],
    [ "Overview", "index.html#autotoc_md2", null ],
    [ "Features", "index.html#autotoc_md3", null ],
    [ "Quick start", "index.html#autotoc_md4", null ],
    [ "Documentation", "index.html#autotoc_md5", null ],
    [ "Examples", "index.html#autotoc_md6", null ],
    [ "Official references", "index.html#autotoc_md7", null ],
    [ "License", "index.html#autotoc_md8", null ],
    [ "API reference", "md_docs_2api__reference.html", [
      [ "Transport — sfm::I2cInterface<Derived>", "md_docs_2api__reference.html#autotoc_md10", null ],
      [ "Driver — sfm::Driver<I2cT>", "md_docs_2api__reference.html#autotoc_md11", null ],
      [ "Types (sfm_types.hpp)", "md_docs_2api__reference.html#autotoc_md12", null ],
      [ "Constants (sfm_commands.hpp)", "md_docs_2api__reference.html#autotoc_md13", null ]
    ] ],
    [ "CMake integration", "md_docs_2cmake__integration.html", [
      [ "Standalone consumer", "md_docs_2cmake__integration.html#autotoc_md15", null ],
      [ "Settings file only", "md_docs_2cmake__integration.html#autotoc_md16", null ],
      [ "Options", "md_docs_2cmake__integration.html#autotoc_md17", null ],
      [ "ESP-IDF", "md_docs_2cmake__integration.html#autotoc_md18", null ]
    ] ],
    [ "Examples", "md_docs_2examples.html", [
      [ "ESP32-S3 (examples/esp32)", "md_docs_2examples.html#autotoc_md21", null ],
      [ "Transport adapter", "md_docs_2examples.html#autotoc_md22", null ]
    ] ],
    [ "Hardware setup", "md_docs_2hardware__setup.html", [
      [ "Electrical", "md_docs_2hardware__setup.html#autotoc_md24", null ],
      [ "Address (SFM4300 ADDR pin)", "md_docs_2hardware__setup.html#autotoc_md25", null ],
      [ "Fittings (SFM4300)", "md_docs_2hardware__setup.html#autotoc_md26", null ],
      [ "Gas tables by variant", "md_docs_2hardware__setup.html#autotoc_md27", null ],
      [ "Pressure and orientation", "md_docs_2hardware__setup.html#autotoc_md28", null ]
    ] ],
    [ "I²C protocol (SF06 command set)", "md_docs_2i2c__protocol.html", [
      [ "Framing", "md_docs_2i2c__protocol.html#autotoc_md30", null ],
      [ "Commands", "md_docs_2i2c__protocol.html#autotoc_md31", null ],
      [ "Measurement frame (after a start command)", "md_docs_2i2c__protocol.html#autotoc_md32", null ],
      [ "Scaling (§4.5)", "md_docs_2i2c__protocol.html#autotoc_md33", null ],
      [ "Status word (§4.3.2)", "md_docs_2i2c__protocol.html#autotoc_md34", null ],
      [ "Timing", "md_docs_2i2c__protocol.html#autotoc_md35", null ]
    ] ],
    [ "HF-SFM driver documentation", "md_docs_2index.html", [
      [ "Documentation structure", "md_docs_2index.html#autotoc_md37", [
        [ "Getting started", "md_docs_2index.html#autotoc_md38", null ],
        [ "Hardware and integration", "md_docs_2index.html#autotoc_md39", null ],
        [ "Reference and examples", "md_docs_2index.html#autotoc_md40", null ],
        [ "Manufacturer", "md_docs_2index.html#autotoc_md41", null ]
      ] ]
    ] ],
    [ "Installation", "md_docs_2installation.html", [
      [ "Requirements", "md_docs_2installation.html#autotoc_md43", null ],
      [ "As a git submodule", "md_docs_2installation.html#autotoc_md44", null ],
      [ "Without add_subdirectory", "md_docs_2installation.html#autotoc_md45", null ],
      [ "Host tests", "md_docs_2installation.html#autotoc_md46", null ],
      [ "Repository layout", "md_docs_2installation.html#autotoc_md47", null ]
    ] ],
    [ "Quick start", "md_docs_2quickstart.html", [
      [ "First frame", "md_docs_2quickstart.html#autotoc_md49", null ],
      [ "Polling model", "md_docs_2quickstart.html#autotoc_md50", null ],
      [ "Result handling", "md_docs_2quickstart.html#autotoc_md51", null ]
    ] ],
    [ "Troubleshooting", "md_docs_2troubleshooting.html", [
      [ "NoData on every read", "md_docs_2troubleshooting.html#autotoc_md53", null ],
      [ "Crc errors", "md_docs_2troubleshooting.html#autotoc_md54", null ],
      [ "UnsupportedGas", "md_docs_2troubleshooting.html#autotoc_md55", null ],
      [ "BusWrite on ReadProductIdentifier()", "md_docs_2troubleshooting.html#autotoc_md56", null ],
      [ "NotSupported from SoftReset()", "md_docs_2troubleshooting.html#autotoc_md57", null ],
      [ "First readings a few % off", "md_docs_2troubleshooting.html#autotoc_md58", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"namespacesfm_1_1timing.html#a63541948d0919d738d9a0087c2a962e8"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';