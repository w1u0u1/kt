#pragma once
#include <ntifs.h>
#include <windef.h>

typedef enum _KAPC_ENVIRONMENT
{
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment,
    InsertApcEnvironment
} KAPC_ENVIRONMENT;

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation,
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemVdmInstemulInformation,
    SystemVdmBopInformation,
    SystemFileCacheInformation,
    SystemPoolTagInformation,
    SystemInterruptInformation,
    SystemDpcBehaviorInformation,
    SystemFullMemoryInformation,
    SystemLoadGdiDriverInformation,
    SystemUnloadGdiDriverInformation,
    SystemTimeAdjustmentInformation,
    SystemSummaryMemoryInformation,
    SystemNextEventIdInformation,
    SystemEventIdsInformation,
    SystemCrashDumpInformation,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemExtendServiceTableInformation,
    SystemPrioritySeperation,
    SystemPlugPlayBusInformation,
    SystemDockInformation
    //SystemPowerInformation,
    //SystemProcessorSpeedInformation,
    //SystemCurrentTimeZoneInformation,
    //SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS;

typedef  enum  _THREAD_STATE
{
    StateInitialized,
    StateReady,
    StateRunning,
    StateStandby,
    StateTerminated,
    StateWait,
    StateTransition,
    StateUnknown
}THREAD_STATE;

//0xe0 bytes (sizeof)
typedef struct _AUX_ACCESS_DATA
{
    struct _PRIVILEGE_SET* PrivilegesUsed;                                  //0x0
    struct _GENERIC_MAPPING GenericMapping;                                 //0x8
    ULONG AccessesToAudit;                                                  //0x18
    ULONG MaximumAuditMask;                                                 //0x1c
    struct _GUID TransactionId;                                             //0x20
    VOID* NewSecurityDescriptor;                                            //0x30
    VOID* ExistingSecurityDescriptor;                                       //0x38
    VOID* ParentSecurityDescriptor;                                         //0x40
    VOID(*DeRefSecurityDescriptor)(VOID* arg1, VOID* arg2);                //0x48
    VOID* SDLock;                                                           //0x50
    struct _ACCESS_REASONS AccessReasons;                                   //0x58
    UCHAR GenerateStagingEvents;                                            //0xd8
} AUX_ACCESS_DATA, * PAUX_ACCESS_DATA;

typedef struct _CALLBACK_ENTRY
{
    UINT16 Version;
    UINT16 OperationRegistrationCount;
    UINT32 unk1;
    PVOID RegistrationContext;
    UNICODE_STRING Altitude;
} CALLBACK_ENTRY, * PCALLBACK_ENTRY;

typedef struct _OBJECT_CALLBACK_ENTRY
{
    LIST_ENTRY CallbackList;
    OB_OPERATION Operations;
    ULONG Active;
    /*OB_HANDLE*/ PCALLBACK_ENTRY CallbackEntry;
    POBJECT_TYPE ObjectType;
    POB_PRE_OPERATION_CALLBACK  PreOperation;
    POB_POST_OPERATION_CALLBACK PostOperation;
} OBJECT_CALLBACK_ENTRY, * POBJECT_CALLBACK_ENTRY;

typedef struct _OBJECT_HANDLE_FLAG_INFORMATION
{
    BOOLEAN Inherit;
    BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_FLAG_INFORMATION, * POBJECT_HANDLE_FLAG_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO  Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

// SystemModuleInformation
typedef struct _SYSTEM_MODULE_INFORMATION {
#ifdef _WIN64
    ULONG   Reserved[4];
#else
    ULONG   Reserved[2];
#endif
    PVOID   Base;
    ULONG   Size;
    ULONG   Flags;
    USHORT  Index;
    USHORT  Unknown;
    USHORT  LoadCount;
    USHORT  ModuleNameOffset;
    CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _SYSTEM_MODULE_LIST {
    ULONG ulCount;
    SYSTEM_MODULE_INFORMATION smi[1];
} SYSTEM_MODULE_LIST, * PSYSTEM_MODULE_LIST;

typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitchCount;
    ULONG State;
    KWAIT_REASON WaitReason;
#if defined(_M_X64) || defined(_M_ARM64) // TODO:ARM64
    LARGE_INTEGER unk;
#endif
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER Reserved[3];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE ParentProcessId;
    ULONG HandleCount;
    LPCWSTR Reserved2[2];
    ULONG PrivatePageCount;
    VM_COUNTERS VirtualMemoryCounters;
    IO_COUNTERS IoCounters;
    SYSTEM_THREAD_INFORMATION Threads[ANYSIZE_ARRAY];
}SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

//0xa8 bytes (sizeof)
struct _KAFFINITY_EX
{
    USHORT Count;                                                           //0x0
    USHORT Size;                                                            //0x2
    ULONG Reserved;                                                         //0x4
    ULONGLONG Bitmap[20];                                                   //0x8
};

//0x1 bytes (sizeof)
union _KEXECUTE_OPTIONS
{
    UCHAR ExecuteDisable : 1;                                                 //0x0
    UCHAR ExecuteEnable : 1;                                                  //0x0
    UCHAR DisableThunkEmulation : 1;                                          //0x0
    UCHAR Permanent : 1;                                                      //0x0
    UCHAR ExecuteDispatchEnable : 1;                                          //0x0
    UCHAR ImageDispatchEnable : 1;                                            //0x0
    UCHAR DisableExceptionChainValidation : 1;                                //0x0
    UCHAR Spare : 1;                                                          //0x0
    volatile UCHAR ExecuteOptions;                                          //0x0
    UCHAR ExecuteOptionsNV;                                                 //0x0
};

//0x4 bytes (sizeof)
union _KSTACK_COUNT
{
    LONG Value;                                                             //0x0
    ULONG State : 3;                                                          //0x0
    ULONG StackCount : 29;                                                    //0x0
};

//0x438 bytes (sizeof)
struct _KPROCESS
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    struct _LIST_ENTRY ProfileListHead;                                     //0x18
    ULONGLONG DirectoryTableBase;                                           //0x28
    struct _LIST_ENTRY ThreadListHead;                                      //0x30
    ULONG ProcessLock;                                                      //0x40
    ULONG ProcessTimerDelay;                                                //0x44
    ULONGLONG DeepFreezeStartTime;                                          //0x48
    struct _KAFFINITY_EX Affinity;                                          //0x50
    ULONGLONG AffinityPadding[12];                                          //0xf8
    struct _LIST_ENTRY ReadyListHead;                                       //0x158
    struct _SINGLE_LIST_ENTRY SwapListEntry;                                //0x168
    volatile struct _KAFFINITY_EX ActiveProcessors;                         //0x170
    ULONGLONG ActiveProcessorsPadding[12];                                  //0x218
    union
    {
        struct
        {
            ULONG AutoAlignment : 1;                                          //0x278
            ULONG DisableBoost : 1;                                           //0x278
            ULONG DisableQuantum : 1;                                         //0x278
            ULONG DeepFreeze : 1;                                             //0x278
            ULONG TimerVirtualization : 1;                                    //0x278
            ULONG CheckStackExtents : 1;                                      //0x278
            ULONG CacheIsolationEnabled : 1;                                  //0x278
            ULONG PpmPolicy : 3;                                              //0x278
            ULONG VaSpaceDeleted : 1;                                         //0x278
            ULONG ReservedFlags : 21;                                         //0x278
        };
        volatile LONG ProcessFlags;                                         //0x278
    };
    ULONG ActiveGroupsMask;                                                 //0x27c
    CHAR BasePriority;                                                      //0x280
    CHAR QuantumReset;                                                      //0x281
    CHAR Visited;                                                           //0x282
    union _KEXECUTE_OPTIONS Flags;                                          //0x283
    USHORT ThreadSeed[20];                                                  //0x284
    USHORT ThreadSeedPadding[12];                                           //0x2ac
    USHORT IdealProcessor[20];                                              //0x2c4
    USHORT IdealProcessorPadding[12];                                       //0x2ec
    USHORT IdealNode[20];                                                   //0x304
    USHORT IdealNodePadding[12];                                            //0x32c
    USHORT IdealGlobalNode;                                                 //0x344
    USHORT Spare1;                                                          //0x346
    union _KSTACK_COUNT StackCount;                                         //0x348
    struct _LIST_ENTRY ProcessListEntry;                                    //0x350
    ULONGLONG CycleTime;                                                    //0x360
    ULONGLONG ContextSwitches;                                              //0x368
    struct _KSCHEDULING_GROUP* SchedulingGroup;                             //0x370
    ULONG FreezeCount;                                                      //0x378
    ULONG KernelTime;                                                       //0x37c
    ULONG UserTime;                                                         //0x380
    ULONG ReadyTime;                                                        //0x384
    ULONGLONG UserDirectoryTableBase;                                       //0x388
    UCHAR AddressPolicy;                                                    //0x390
    UCHAR Spare2[71];                                                       //0x391
    VOID* InstrumentationCallback;                                          //0x3d8
    union
    {
        ULONGLONG SecureHandle;                                             //0x3e0
        struct
        {
            ULONGLONG SecureProcess : 1;                                      //0x3e0
            ULONGLONG Unused : 1;                                             //0x3e0
        } Flags;                                                            //0x3e0
    } SecureState;                                                          //0x3e0
    ULONGLONG KernelWaitTime;                                               //0x3e8
    ULONGLONG UserWaitTime;                                                 //0x3f0
    ULONGLONG EndPadding[8];                                                //0x3f8
};

//0x430 bytes (sizeof)
struct _KTHREAD
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    VOID* SListFaultAddress;                                                //0x18
    ULONGLONG QuantumTarget;                                                //0x20
    VOID* InitialStack;                                                     //0x28
    VOID* volatile StackLimit;                                              //0x30
    VOID* StackBase;                                                        //0x38
    ULONGLONG ThreadLock;                                                   //0x40
    volatile ULONGLONG CycleTime;                                           //0x48
    ULONG CurrentRunTime;                                                   //0x50
    ULONG ExpectedRunTime;                                                  //0x54
    VOID* KernelStack;                                                      //0x58
    struct _XSAVE_FORMAT* StateSaveArea;                                    //0x60
    struct _KSCHEDULING_GROUP* volatile SchedulingGroup;                    //0x68
    //union _KWAIT_STATUS_REGISTER WaitRegister;                              //0x70
    CHAR    WaitRegister;
    volatile UCHAR Running;                                                 //0x71
    UCHAR Alerted[2];                                                       //0x72
    union
    {
        struct
        {
            ULONG AutoBoostActive : 1;                                        //0x74
            ULONG ReadyTransition : 1;                                        //0x74
            ULONG WaitNext : 1;                                               //0x74
            ULONG SystemAffinityActive : 1;                                   //0x74
            ULONG Alertable : 1;                                              //0x74
            ULONG UserStackWalkActive : 1;                                    //0x74
            ULONG ApcInterruptRequest : 1;                                    //0x74
            ULONG QuantumEndMigrate : 1;                                      //0x74
            ULONG UmsDirectedSwitchEnable : 1;                                //0x74
            ULONG TimerActive : 1;                                            //0x74
            ULONG SystemThread : 1;                                           //0x74
            ULONG ProcessDetachActive : 1;                                    //0x74
            ULONG CalloutActive : 1;                                          //0x74
            ULONG ScbReadyQueue : 1;                                          //0x74
            ULONG ApcQueueable : 1;                                           //0x74
            ULONG ReservedStackInUse : 1;                                     //0x74
            ULONG UmsPerformingSyscall : 1;                                   //0x74
            ULONG TimerSuspended : 1;                                         //0x74
            ULONG SuspendedWaitMode : 1;                                      //0x74
            ULONG SuspendSchedulerApcWait : 1;                                //0x74
            ULONG CetUserShadowStack : 1;                                     //0x74
            ULONG BypassProcessFreeze : 1;                                    //0x74
            ULONG Reserved : 10;                                              //0x74
        };
        LONG MiscFlags;                                                     //0x74
    };
    union
    {
        struct
        {
            ULONG ThreadFlagsSpare : 2;                                       //0x78
            ULONG AutoAlignment : 1;                                          //0x78
            ULONG DisableBoost : 1;                                           //0x78
            ULONG AlertedByThreadId : 1;                                      //0x78
            ULONG QuantumDonation : 1;                                        //0x78
            ULONG EnableStackSwap : 1;                                        //0x78
            ULONG GuiThread : 1;                                              //0x78
            ULONG DisableQuantum : 1;                                         //0x78
            ULONG ChargeOnlySchedulingGroup : 1;                              //0x78
            ULONG DeferPreemption : 1;                                        //0x78
            ULONG QueueDeferPreemption : 1;                                   //0x78
            ULONG ForceDeferSchedule : 1;                                     //0x78
            ULONG SharedReadyQueueAffinity : 1;                               //0x78
            ULONG FreezeCount : 1;                                            //0x78
            ULONG TerminationApcRequest : 1;                                  //0x78
            ULONG AutoBoostEntriesExhausted : 1;                              //0x78
            ULONG KernelStackResident : 1;                                    //0x78
            ULONG TerminateRequestReason : 2;                                 //0x78
            ULONG ProcessStackCountDecremented : 1;                           //0x78
            ULONG RestrictedGuiThread : 1;                                    //0x78
            ULONG VpBackingThread : 1;                                        //0x78
            ULONG ThreadFlagsSpare2 : 1;                                      //0x78
            ULONG EtwStackTraceApcInserted : 8;                               //0x78
        };
        volatile LONG ThreadFlags;                                          //0x78
    };
    volatile UCHAR Tag;                                                     //0x7c
    UCHAR SystemHeteroCpuPolicy;                                            //0x7d
    UCHAR UserHeteroCpuPolicy : 7;                                            //0x7e
    UCHAR ExplicitSystemHeteroCpuPolicy : 1;                                  //0x7e
    union
    {
        struct
        {
            UCHAR RunningNonRetpolineCode : 1;                                //0x7f
            UCHAR SpecCtrlSpare : 7;                                          //0x7f
        };
        UCHAR SpecCtrl;                                                     //0x7f
    };
    ULONG SystemCallNumber;                                                 //0x80
    ULONG ReadyTime;                                                        //0x84
    VOID* FirstArgument;                                                    //0x88
    struct _KTRAP_FRAME* TrapFrame;                                         //0x90
    union
    {
        struct _KAPC_STATE ApcState;                                        //0x98
        struct
        {
            UCHAR ApcStateFill[43];                                         //0x98
            CHAR Priority;                                                  //0xc3
            ULONG UserIdealProcessor;                                       //0xc4
        };
    };
    volatile LONGLONG WaitStatus;                                           //0xc8
    struct _KWAIT_BLOCK* WaitBlockList;                                     //0xd0
    union
    {
        struct _LIST_ENTRY WaitListEntry;                                   //0xd8
        struct _SINGLE_LIST_ENTRY SwapListEntry;                            //0xd8
    };
    struct _DISPATCHER_HEADER* volatile Queue;                              //0xe8
    VOID* Teb;                                                              //0xf0
    ULONGLONG RelativeTimerBias;                                            //0xf8
    struct _KTIMER Timer;                                                   //0x100
    union
    {
        struct _KWAIT_BLOCK WaitBlock[4];                                   //0x140
        struct
        {
            UCHAR WaitBlockFill4[20];                                       //0x140
            ULONG ContextSwitches;                                          //0x154
        };
        struct
        {
            UCHAR WaitBlockFill5[68];                                       //0x140
            volatile UCHAR State;                                           //0x184
            CHAR Spare13;                                                   //0x185
            UCHAR WaitIrql;                                                 //0x186
            CHAR WaitMode;                                                  //0x187
        };
        struct
        {
            UCHAR WaitBlockFill6[116];                                      //0x140
            ULONG WaitTime;                                                 //0x1b4
        };
        struct
        {
            UCHAR WaitBlockFill7[164];                                      //0x140
            union
            {
                struct
                {
                    SHORT KernelApcDisable;                                 //0x1e4
                    SHORT SpecialApcDisable;                                //0x1e6
                };
                ULONG CombinedApcDisable;                                   //0x1e4
            };
        };
        struct
        {
            UCHAR WaitBlockFill8[40];                                       //0x140
            struct _KTHREAD_COUNTERS* ThreadCounters;                       //0x168
        };
        struct
        {
            UCHAR WaitBlockFill9[88];                                       //0x140
            struct _XSTATE_SAVE* XStateSave;                                //0x198
        };
        struct
        {
            UCHAR WaitBlockFill10[136];                                     //0x140
            VOID* volatile Win32Thread;                                     //0x1c8
        };
        struct
        {
            UCHAR WaitBlockFill11[176];                                     //0x140
            struct _UMS_CONTROL_BLOCK* Ucb;                                 //0x1f0
            struct _KUMS_CONTEXT_HEADER* volatile Uch;                      //0x1f8
        };
    };
    union
    {
        volatile LONG ThreadFlags2;                                         //0x200
        struct
        {
            ULONG BamQosLevel : 8;                                            //0x200
            ULONG ThreadFlags2Reserved : 24;                                  //0x200
        };
    };
    ULONG Spare21;                                                          //0x204
    struct _LIST_ENTRY QueueListEntry;                                      //0x208
    union
    {
        volatile ULONG NextProcessor;                                       //0x218
        struct
        {
            ULONG NextProcessorNumber : 31;                                   //0x218
            ULONG SharedReadyQueue : 1;                                       //0x218
        };
    };
    LONG QueuePriority;                                                     //0x21c
    struct _KPROCESS* Process;                                              //0x220
    union
    {
        struct _GROUP_AFFINITY UserAffinity;                                //0x228
        struct
        {
            UCHAR UserAffinityFill[10];                                     //0x228
            CHAR PreviousMode;                                              //0x232
            CHAR BasePriority;                                              //0x233
            union
            {
                CHAR PriorityDecrement;                                     //0x234
                struct
                {
                    UCHAR ForegroundBoost : 4;                                //0x234
                    UCHAR UnusualBoost : 4;                                   //0x234
                };
            };
            UCHAR Preempted;                                                //0x235
            UCHAR AdjustReason;                                             //0x236
            CHAR AdjustIncrement;                                           //0x237
        };
    };
    ULONGLONG AffinityVersion;                                              //0x238
    union
    {
        struct _GROUP_AFFINITY Affinity;                                    //0x240
        struct
        {
            UCHAR AffinityFill[10];                                         //0x240
            UCHAR ApcStateIndex;                                            //0x24a
            UCHAR WaitBlockCount;                                           //0x24b
            ULONG IdealProcessor;                                           //0x24c
        };
    };
    ULONGLONG NpxState;                                                     //0x250
    union
    {
        struct _KAPC_STATE SavedApcState;                                   //0x258
        struct
        {
            UCHAR SavedApcStateFill[43];                                    //0x258
            UCHAR WaitReason;                                               //0x283
            CHAR SuspendCount;                                              //0x284
            CHAR Saturation;                                                //0x285
            USHORT SListFaultCount;                                         //0x286
        };
    };
    union
    {
        struct _KAPC SchedulerApc;                                          //0x288
        struct
        {
            UCHAR SchedulerApcFill0[1];                                     //0x288
            UCHAR ResourceIndex;                                            //0x289
        };
        struct
        {
            UCHAR SchedulerApcFill1[3];                                     //0x288
            UCHAR QuantumReset;                                             //0x28b
        };
        struct
        {
            UCHAR SchedulerApcFill2[4];                                     //0x288
            ULONG KernelTime;                                               //0x28c
        };
        struct
        {
            UCHAR SchedulerApcFill3[64];                                    //0x288
            struct _KPRCB* volatile WaitPrcb;                               //0x2c8
        };
        struct
        {
            UCHAR SchedulerApcFill4[72];                                    //0x288
            VOID* LegoData;                                                 //0x2d0
        };
        struct
        {
            UCHAR SchedulerApcFill5[83];                                    //0x288
            UCHAR CallbackNestingLevel;                                     //0x2db
            ULONG UserTime;                                                 //0x2dc
        };
    };
    struct _KEVENT SuspendEvent;                                            //0x2e0
    struct _LIST_ENTRY ThreadListEntry;                                     //0x2f8
    struct _LIST_ENTRY MutantListHead;                                      //0x308
    UCHAR AbEntrySummary;                                                   //0x318
    UCHAR AbWaitEntryCount;                                                 //0x319
    UCHAR AbAllocationRegionCount;                                          //0x31a
    CHAR SystemPriority;                                                    //0x31b
    ULONG SecureThreadCookie;                                               //0x31c
    struct _KLOCK_ENTRY* LockEntries;                                       //0x320
    struct _SINGLE_LIST_ENTRY PropagateBoostsEntry;                         //0x328
    struct _SINGLE_LIST_ENTRY IoSelfBoostsEntry;                            //0x330
    UCHAR PriorityFloorCounts[16];                                          //0x338
    UCHAR PriorityFloorCountsReserved[16];                                  //0x348
    ULONG PriorityFloorSummary;                                             //0x358
    volatile LONG AbCompletedIoBoostCount;                                  //0x35c
    volatile LONG AbCompletedIoQoSBoostCount;                               //0x360
    volatile SHORT KeReferenceCount;                                        //0x364
    UCHAR AbOrphanedEntrySummary;                                           //0x366
    UCHAR AbOwnedEntryCount;                                                //0x367
    ULONG ForegroundLossTime;                                               //0x368
    union
    {
        struct _LIST_ENTRY GlobalForegroundListEntry;                       //0x370
        struct
        {
            struct _SINGLE_LIST_ENTRY ForegroundDpcStackListEntry;          //0x370
            ULONGLONG InGlobalForegroundList;                               //0x378
        };
    };
    LONGLONG ReadOperationCount;                                            //0x380
    LONGLONG WriteOperationCount;                                           //0x388
    LONGLONG OtherOperationCount;                                           //0x390
    LONGLONG ReadTransferCount;                                             //0x398
    LONGLONG WriteTransferCount;                                            //0x3a0
    LONGLONG OtherTransferCount;                                            //0x3a8
    struct _KSCB* QueuedScb;                                                //0x3b0
    volatile ULONG ThreadTimerDelay;                                        //0x3b8
    union
    {
        volatile LONG ThreadFlags3;                                         //0x3bc
        struct
        {
            ULONG ThreadFlags3Reserved : 8;                                   //0x3bc
            ULONG PpmPolicy : 2;                                              //0x3bc
            ULONG ThreadFlags3Reserved2 : 22;                                 //0x3bc
        };
    };
    ULONGLONG TracingPrivate[1];                                            //0x3c0
    VOID* SchedulerAssist;                                                  //0x3c8
    VOID* volatile AbWaitObject;                                            //0x3d0
    ULONG ReservedPreviousReadyTimeValue;                                   //0x3d8
    ULONGLONG KernelWaitTime;                                               //0x3e0
    ULONGLONG UserWaitTime;                                                 //0x3e8
    union
    {
        struct _LIST_ENTRY GlobalUpdateVpThreadPriorityListEntry;           //0x3f0
        struct
        {
            struct _SINGLE_LIST_ENTRY UpdateVpThreadPriorityDpcStackListEntry; //0x3f0
            ULONGLONG InGlobalUpdateVpThreadPriorityList;                   //0x3f8
        };
    };
    LONG SchedulerAssistPriorityFloor;                                      //0x400
    ULONG Spare28;                                                          //0x404
    ULONGLONG EndPadding[5];                                                //0x408
};

typedef
VOID
(NTAPI* PKNORMAL_ROUTINE)(
    _In_ PVOID NormalContext,
    _In_ PVOID SystemArgument1,
    _In_ PVOID SystemArgument2
    );

typedef
VOID
(NTAPI* PKKERNEL_ROUTINE)(
    _In_ PKAPC Apc,
    _Inout_ PKNORMAL_ROUTINE* NormalRoutine,
    _Inout_ PVOID* NormalContext,
    _Inout_ PVOID* SystemArgument1,
    _Inout_ PVOID* SystemArgument2
    );

typedef
VOID
(NTAPI* PKRUNDOWN_ROUTINE) (
    _In_ PKAPC Apc
    );

NTKERNELAPI
VOID
NTAPI
KeInitializeApc(
    _Out_ PRKAPC Apc,
    _In_ PETHREAD Thread,
    _In_ KAPC_ENVIRONMENT Environment,
    _In_ PKKERNEL_ROUTINE KernelRoutine,
    _In_opt_ PKRUNDOWN_ROUTINE RundownRoutine,
    _In_opt_ PKNORMAL_ROUTINE NormalRoutine,
    _In_opt_ KPROCESSOR_MODE ApcMode,
    _In_opt_ PVOID NormalContext
);

NTKERNELAPI
BOOLEAN
NTAPI
KeInsertQueueApc(
    _Inout_ PRKAPC Apc,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2,
    _In_ KPRIORITY Increment
);

NTKERNELAPI
BOOLEAN
NTAPI
KeAlertThread(
    _Inout_ PKTHREAD Thread,
    _In_ KPROCESSOR_MODE AlertMode
);

NTKERNELAPI
BOOLEAN
NTAPI
KeTestAlertThread(
    _In_ KPROCESSOR_MODE AlertMode
);

NTKERNELAPI
NTSTATUS
NTAPI
ObCreateObject(
    IN KPROCESSOR_MODE      ObjectAttributesAccessMode OPTIONAL,
    IN POBJECT_TYPE         ObjectType,
    IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
    IN KPROCESSOR_MODE      AccessMode,
    IN OUT PVOID            ParseContext OPTIONAL,
    IN ULONG                ObjectSize,
    IN ULONG                PagedPoolCharge OPTIONAL,
    IN ULONG                NonPagedPoolCharge OPTIONAL,
    OUT PVOID* Object
);

NTKERNELAPI
NTSTATUS
NTAPI
ObReferenceObjectByName(
    __in PUNICODE_STRING ObjectName,
    __in ULONG Attributes,
    __in_opt PACCESS_STATE AccessState,
    __in_opt ACCESS_MASK DesiredAccess,
    __in POBJECT_TYPE ObjectType,
    __in KPROCESSOR_MODE AccessMode,
    __inout_opt PVOID ParseContext,
    __out PVOID* Object
);

NTKERNELAPI
NTSTATUS
NTAPI
ObSetHandleAttributes(
    __in HANDLE Handle,
    __in POBJECT_HANDLE_FLAG_INFORMATION HandleFlags,
    __in KPROCESSOR_MODE PreviousMode
);

NTKERNELAPI
UCHAR*
NTAPI PsGetProcessImageFileName(
    _In_ PEPROCESS Process
);

NTKERNELAPI
PVOID
NTAPI
PsGetProcessWow64Process(
    _In_ PEPROCESS Process
);

NTKERNELAPI
BOOLEAN
NTAPI
PsIsProtectedProcess(
    _In_ PEPROCESS Process
);

NTKERNELAPI
USHORT
NTAPI
PsWow64GetProcessMachine(
    _In_ PEPROCESS Process
);

NTSYSAPI
PVOID
NTAPI
RtlImageDirectoryEntryToData(
    _In_ PVOID BaseOfImage,
    _In_ BOOLEAN MappedAsImage,
    _In_ USHORT DirectoryEntry,
    _Out_ PULONG Size
);

NTKERNELAPI
NTSTATUS
NTAPI
SeCreateAccessState(
    OUT PACCESS_STATE   AccessState,
    IN PVOID            AuxData,
    IN ACCESS_MASK      AccessMask,
    IN PGENERIC_MAPPING Mapping
);

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID               SystemInformation,
    IN ULONG                SystemInformationLength,
    OUT PULONG              ReturnLength OPTIONAL
);


extern POBJECT_TYPE* IoDeviceObjectType;
extern POBJECT_TYPE* IoDriverObjectType;
extern POBJECT_TYPE* IoFileObjectType;
extern POBJECT_TYPE* CmpKeyObjectType;
extern POBJECT_TYPE* PsJobType;
extern POBJECT_TYPE* PsProcessType;
extern POBJECT_TYPE* PsThreadType;