/**
 * Windowless Vimrun.exe
 *
 * Copyright © 2015 by John Wellesz (john.wellesz@teaser.fr)
 *
 * This program is a complete rewrite of 'vimrun-silent' originaly written by
 * Leonid Shevtsov and released under the MIT License.
 * vimrun-silent can be found at https://github.com/leonid-shevtsov/vimrun-silent
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>
#include <wchar.h>

#define UNUSED(x) (void)(x)


#define FATAL_ERROR(msg) ({MessageBox(NULL, msg, "vimrun error", MB_OK | MB_ICONERROR); return -1;})

#define MEMORY_ERROR_EXIT  FATAL_ERROR("Failed to allocate memory.")

#define PAUSE_ADDITION L" && pause"

//#define DEBUG

#ifdef DEBUG

#define DEBUGMW(msg, title) ({if (msg != NULL) MessageBoxW(NULL, msg, title, MB_OK);})
#define DEBUGM(msg, title)  ({if (msg != NULL) MessageBox (NULL, msg, title, MB_OK);})

#else

#define DEBUGMW(msg, title)
#define DEBUGM(msg, title)

#endif

/**
 * Split a string on its first space character ignoring those between quotes
 * (escaped quotes are not supported).
 *
 * Will insert a '\0' in `string` on the first Space found
 * Will set `part2` to the next non-space character in `string`
 *
 * if no space is found `part2` is set on `string`'s end so that `part2`
 * becomes an empty string.
 *
 * `string` and `part2` can point to the same address which results in trimming
 * the left part of `string` until a space is found.
 *
 */
void splitOnSpace (LPWSTR *string, LPWSTR *part2) {
    char    inQuote = 0;
    LPWSTR  part1   = *string;

    // Murphy's check
    if (string == NULL)
        return;

    for (;;) {
        // We've reached the end of part1 without finding any space to split on
        if (! part1[0]) {
            // set part2 to the last char of part1 ('\0')
            *part2 = part1;
            break;
        }

        if (*part1 == '"') {
            inQuote = !inQuote;
        } else if (!inQuote && *part1 == ' ') {

            // stop part 1
            *part1 = '\0';

            // more spaces?
            while (part1[1] && part1[1] == ' ')
                // skip them
                ++part1;

            // at this point part1 is either a null or a space
            // set the begining of part2 to the char right after that
            *part2 = &part1[1];

            break;

        }

        // ready to loop...
        ++part1;
    }

}

int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow
        ) {

    int hideCommandWindow = 0;

    LPWSTR full_command_line = NULL,
           command           = NULL,
           commandArguments  = NULL;


    SHELLEXECUTEINFOW sei;

    /*
     * Prevent 'unused parameter' warnings from compilers
     */
    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);
    UNUSED(nCmdShow);

    /*
     * Get a copy of our full command line so we can modify it freely
     */
    if (NULL == (full_command_line = (LPWSTR)calloc(wcslen(GetCommandLineW()) + 1, sizeof(wchar_t)))) {
        MEMORY_ERROR_EXIT;
    }

    wcscpy(full_command_line, GetCommandLineW());

    DEBUGMW(full_command_line, L"full_command_line");


    /*
     * Remove vimrun's path
     */
    splitOnSpace(&full_command_line, &command);

    DEBUGMW(command, L"command");

    /*
     * Check for "-s" argument (silence).
     */
    if (command[0] == '-' && command[1] == 's') {
        hideCommandWindow = 1;
        // skip the -s and any space after it
        splitOnSpace(&command, &command);

        DEBUGMW(command, L"command minus /-s\\s*/");
    }

    if (!command[0])
        FATAL_ERROR("No command given");

    /*
     * Separate the requested command and its arguments
     */
    splitOnSpace(&command, &commandArguments);

    if (!hideCommandWindow && commandArguments[0]) {
        /*
         * Just add "&& pause" to the cmd.exe command
         * XXX will only work with cmd.exe...
         */
        LPWSTR arguments_with_pause;

        if (NULL == (arguments_with_pause = (LPWSTR)calloc(wcslen(commandArguments) + wcslen(PAUSE_ADDITION) + 1, sizeof(wchar_t)))) {
            MEMORY_ERROR_EXIT;
        }

        wcscat(arguments_with_pause, commandArguments);
        wcscat(arguments_with_pause, PAUSE_ADDITION);
        commandArguments = arguments_with_pause;
    }

    DEBUGMW(commandArguments, L"args");

    /*
     * Prepare ShellExecuteInfo
     */
    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize       = sizeof(sei);
    sei.nShow        = hideCommandWindow ? SW_HIDE : SW_SHOW;
    sei.lpVerb       = L"open";
    sei.lpFile       = command;
    sei.lpParameters = commandArguments;
    sei.fMask        = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;

    // And finally run it!
    ShellExecuteExW(&sei);

    // Has a process started?
    if (sei.hProcess) {
        DWORD exitCode = 0;

        // Wait for the process to stop
        if (WaitForSingleObject(sei.hProcess, INFINITE) == WAIT_FAILED)
            FATAL_ERROR("Failed to wait for process...");

        // Grab its exit code
        GetExitCodeProcess(sei.hProcess, &exitCode);

        return exitCode;
    } else {
        return -1;
    }
}
