/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/
/* Simple Windows INotifyWait-like equivalent */
#ifndef _WIN32
int watch(const char *path) { return 1; }
#else
#include <windows.h>
#include <stdio.h>
int watch(const char *path) {
    HANDLE hDir = CreateFile(
        path,                         // Directory to monitor
        FILE_LIST_DIRECTORY,          // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
        NULL,                         // Security attributes
        OPEN_EXISTING,                // Open existing directory
        FILE_FLAG_BACKUP_SEMANTICS,   // Required to open directories
        NULL                          // No template file
    );
    if (hDir == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Unable to open directory for monitoring. Code: %lu\n", GetLastError());
        return 1;
    }
    char buffer[1024];
    DWORD bytesReturned;
    BOOL cont = 1;
    while (cont) {
        if (ReadDirectoryChangesW(
                hDir,                        // Directory handle
                &buffer,                     // Buffer to store change information
                sizeof(buffer),              // Size of buffer
                TRUE,                        // Monitor subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | 
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | 
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION, // Change types
                &bytesReturned,              // Bytes returned
                NULL,                        // No overlapped I/O
                NULL                         // No completion routine
            )) {
            FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
                int name_len = pNotify->FileNameLength / sizeof(wchar_t);
            do {
                switch (pNotify->Action) {
          case FILE_ACTION_ADDED: {
            wprintf(L"CREATE %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;

          case FILE_ACTION_REMOVED: {
            wprintf(L"DELETE %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;

          case FILE_ACTION_MODIFIED: {
              wprintf(L"MODIFY %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;

          case FILE_ACTION_RENAMED_OLD_NAME: {
            wprintf(L"MOVED_FROM %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;

          case FILE_ACTION_RENAMED_NEW_NAME: {
            wprintf(L"MOVED_TO %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;

          default: {
            wprintf(L"UNKNOWN_EVT %.*ls\n", name_len, pNotify->FileName);
              cont = 0;
          } break;
                }
                if (pNotify->NextEntryOffset == 0) {
                    break;
                }
                pNotify = (FILE_NOTIFY_INFORMATION*)((char*)pNotify + pNotify->NextEntryOffset);
            } while (1);
        } else {
            fprintf(stderr, "Error: Failed to read directory changes. Code: %lu\n", GetLastError());
            break;
        }
    }
    CloseHandle(hDir);
    return 0;
}
#endif
