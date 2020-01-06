/*
* Windows kernel-mode driver for controlling access to various input devices.
*
* MIT License
*
* Copyright (c) 2016-2019 Nefarius Software Solutions e.U.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


#pragma once

#include <ntintsafe.h>

EXTERN_C_START

#define MAX_HARDWARE_ID_SIZE        0x400

//
// Returns the current caller process id.
// 
#define CURRENT_PROCESS_ID() ((DWORD)((DWORD_PTR)PsGetCurrentProcessId() & 0xFFFFFFFF))

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    WDFMEMORY       HardwareIDsMemory;

    PCWSTR          HardwareIDs;

    size_t          HardwareIDsLength;
       
    //
    // Queue for incoming create requests
    // 
    WDFQUEUE        CreateRequestsQueue;

    //
    // Queue for pending create requests (for pickup by Cerberus)
    // 
    WDFQUEUE        PendingCreateRequestsQueue;

    //
    // Queue for pending create requests (while waiting for answer)
    // 
    WDFQUEUE        PendingAuthQueue;

    WDFQUEUE        NotificationsQueue;

    //
    // Linked list containing cached Process IDs and their access state
    // 
    PPID_LIST_NODE  StickyPidList;

    //
    // Default behavior for requests unguarded by Cerberus
    // 
    BOOLEAN         AllowByDefault;

    BOOLEAN         IsShuttingDown;

    WCHAR           DeviceID[MAX_DEVICE_ID_SIZE];

    WCHAR           InstanceID[MAX_INSTANCE_ID_SIZE];

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

typedef struct _CREATE_REQUEST_CONTEXT
{
    ULONG RequestId;

    ULONG ProcessId;

} CREATE_REQUEST_CONTEXT, *PCREATE_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CREATE_REQUEST_CONTEXT, CreateRequestGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
HidGuardianCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DEVICE_CONTEXT_CLEANUP HidGuardianEvtDeviceContextCleanup;
EVT_WDF_FILE_CLEANUP EvtFileCleanup;
EVT_WDF_DEVICE_RELEASE_HARDWARE EvtWdfDeviceReleaseHardware;

NTSTATUS BusQueryId(
    _In_ WDFDEVICE Device, 
    _In_ BUS_QUERY_ID_TYPE IdType, 
    _Out_ PWCHAR Buffer, 
    _In_ ULONG BufferLength
);

EXTERN_C_END
