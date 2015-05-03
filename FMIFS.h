
typedef struct {
	DWORD Lines;
	PCHAR Output;
} TEXTOUTPUT, *PTEXTOUTPUT;


typedef enum {
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	UNKNOWN7,
	UNKNOWN8,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS
} CALLBACKCOMMAND;


typedef BOOLEAN (__stdcall *PFMIFSCALLBACK)( CALLBACKCOMMAND Command, DWORD SubAction, PVOID ActionInfo ); 


typedef VOID (__stdcall *PCHKDSK)( PWCHAR DriveRoot, 
						PWCHAR Format,
						BOOL CorrectErrors, 
						BOOL Verbose, 
						BOOL CheckOnlyIfDirty,
						BOOL ScanDrive, 
						PVOID Unused2, 
						PVOID Unused3,
						PFMIFSCALLBACK Callback );


#define FMIFS_HARDDISK 0xC
#define FMIFS_FLOPPY   0x8

typedef VOID (__stdcall *PFORMATEX)( PWCHAR DriveRoot,
						  DWORD MediaFlag,
						  PWCHAR Format,
						  PWCHAR Label,
						  BOOL QuickFormat,
						  DWORD ClusterSize,
						  PFMIFSCALLBACK Callback );


typedef BOOLEAN (__stdcall *PENABLEVOLUMECOMPRESSION)(PWCHAR DriveRoot,
							BOOL Enable );
