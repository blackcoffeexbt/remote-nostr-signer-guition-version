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
  [ "Nostr Remote Signer", "index.html", [
    [ "Nostr Remote Signer Device", "index.html", "index" ],
    [ "Nostr Remote Signer Device Architecture", "md__a_r_c_h_i_t_e_c_t_u_r_e.html", [
      [ "Project Overview", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md14", [
        [ "Key Features", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md15", null ],
        [ "Technology Stack", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md16", null ]
      ] ],
      [ "Architecture Overview", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md18", null ],
      [ "File Structure and Module Descriptions", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md20", [
        [ "Core Application Files", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md21", [
          [ "<tt>src/main.cpp</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md22", null ],
          [ "<tt>src/app.cpp</tt> / <tt>src/app.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md23", null ]
        ] ],
        [ "Display and UI Modules", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md24", [
          [ "<tt>src/display.cpp</tt> / <tt>src/display.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md25", null ],
          [ "<tt>src/ui.cpp</tt> / <tt>src/ui.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md26", null ]
        ] ],
        [ "Connectivity Modules", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md27", [
          [ "<tt>src/wifi.cpp</tt> / <tt>src/wifi.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md28", null ],
          [ "<tt>src/remote_signer.cpp</tt> / <tt>src/remote_signer.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md29", null ]
        ] ],
        [ "Configuration and Storage", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md30", [
          [ "<tt>src/settings.cpp</tt> / <tt>src/settings.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md31", null ],
          [ "<tt>src/config.h</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md32", null ]
        ] ],
        [ "External Libraries", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md33", [
          [ "<tt>lib/TFT_eSPI/</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md34", null ],
          [ "<tt>lib/aes/</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md35", null ],
          [ "<tt>lib/nostr/</tt>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md36", null ]
        ] ]
      ] ],
      [ "Data Flow and Module Interactions", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md38", [
        [ "1. <strong>Application Startup</strong>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md39", null ],
        [ "2. <strong>Client Connection Flow</strong>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md40", null ],
        [ "3. <strong>Event Signing Flow</strong>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md41", null ],
        [ "4. <strong>WiFi Configuration Flow</strong>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md42", null ],
        [ "5. <strong>Settings Management Flow</strong>", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md43", null ]
      ] ],
      [ "Communication Protocols", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md45", [
        [ "NIP-46 (Nostr Connect) Protocol", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md46", null ],
        [ "Nostr Protocol Integration", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md47", null ],
        [ "External APIs", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md48", null ]
      ] ],
      [ "Hardware Configuration", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md50", [
        [ "ESP32 Pin Mapping", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md51", null ],
        [ "Power Management", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md52", null ]
      ] ],
      [ "Security Features", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md54", [
        [ "Cryptographic Security", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md55", null ],
        [ "Network Security", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md56", null ]
      ] ],
      [ "Development and Build", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md58", [
        [ "PlatformIO Configuration", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md59", null ],
        [ "Key Dependencies", "md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md60", null ]
      ] ]
    ] ],
    [ "Modules", "modules.html", "modules" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", "globals_defs" ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_a_x_s15231_b__touch_8cpp.html",
"globals_m.html",
"md__a_r_c_h_i_t_e_c_t_u_r_e.html#autotoc_md34",
"namespace_u_i.html#ae20486f26830f5a8d6312a32e1a5dfec",
"nostr_8h_source.html",
"ui_8cpp.html#a256c4320af439732def9402342903462"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';