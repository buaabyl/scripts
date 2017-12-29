PEDUMP
======

TODO:
1. 获取不同windows的核心dll的导出函数表
2. 对比不同版本windows的差异，pedump可以判断最小的windows运行版本


一、文件头
-----------

在发布Windows版的程序时，在动态链接的时候如何找到依赖的DLL？
其实可以直接解析EXE和DLL程序，得到所有的依赖关系。
Windows的PE文件格式如下，在一个NT的文件格式前附加了一个DOS兼容文件头：

|    EXE/DLL    |
|:-------------:|
|  DOS Header   |
|  NT  Magic    |
|  NT  Header   |
|    Data       |

例如一个Windows的EXE文件的前128个字节数据如下：

```txt
Offset      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F

00000000   4D 5A 90 00 03 00 00 00  04 00 00 00 FF FF 00 00   MZ..............
00000010   B8 00 00 00 00 00 00 00  40 00 00 00 00 00 00 00   ................
00000020   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ................
                                               +-----------+
00000030   00 00 00 00 00 00 00 00  00 00 00 00|80 00 00 00|  ................
                                               +-----------+
00000040   0E 1F BA 0E 00 B4 09 CD  21 B8 01 4C CD 21 54 68   ..............Th
00000050   69 73 20 70 72 6F 67 72  61 6D 20 63 61 6E 6E 6F   is program canno
00000060   74 20 62 65 20 72 75 6E  20 69 6E 20 44 4F 53 20   t be run in DOS 
00000070   6D 6F 64 65 2E 0D 0D 0A  24 00 00 00 00 00 00 00   mode............
```

对于Windows下的EXE和DLL程序，这128个字节基本就是一样的，前64个字节（0x00000000-0x0000003F）就是DOS文件头，
紧接着的64个字节是一个DOS程序，如果在DOS下运行，就会输出"This program cannot be run in DOS mode."
这段话。

在MinGW GCC自带的 `winnt.h` 里可以看到IMAGE_DOS_HEADER的定义：

```c
typedef struct _IMAGE_DOS_HEADER {
  WORD e_magic;
  WORD e_cblp;
  WORD e_cp;
  WORD e_crlc;
  WORD e_cparhdr;
  WORD e_minalloc;
  WORD e_maxalloc;
  WORD e_ss;
  WORD e_sp;
  WORD e_csum;
  WORD e_ip;
  WORD e_cs;
  WORD e_lfarlc;
  WORD e_ovno;
  WORD e_res[4];
  WORD e_oemid;
  WORD e_oeminfo;
  WORD e_res2[10];
  LONG e_lfanew;
} IMAGE_DOS_HEADER,*PIMAGE_DOS_HEADER;
```

其中magic固定为"MZ"，e_lfanew（已经在上面的例子里画了一个框）指向新的NT文件头的位置，
也就是0x00000080，数据如下：

```txt
Offset      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F

00000080   50 45 00 00 64 86 10 00  C1 E9 C7 57 00 5A 00 00   PE..............
00000090   DC 03 00 00 F0 26 20                               ....
```

NT的文件头定义如下，包含一个Magic，一个文件头，一个扩展的文件头，几个Sections：

|    PE File     |
|:--------------:|
|  NT Magic      |
|  NT Header     |
|  NT Opt Header |
|  Sections Table|
|  Sections Data |

前4个字节对应PE的magic，为"PE\x00\x00"，IMAGE_FILE_HEADER的定义如下：

```c
typedef struct _IMAGE_FILE_HEADER {
  WORD Machine;                             //目标机器的类型
  WORD NumberOfSections;                    //PE有多少个Sections
  DWORD TimeDateStamp;                      //PE的创建时间
  DWORD PointerToSymbolTable;               //符号表的位置
  DWORD NumberOfSymbols;                    //符号表的个数
  WORD SizeOfOptionalHeader;                //扩展文件头的大小（字节）
  WORD Characteristics;                     //特殊标记
} IMAGE_FILE_HEADER,*PIMAGE_FILE_HEADER;
```

那么如何判断是32位还是64位呢？从OptionalHeader可以知道，先看看具体的定义，先看32位的：

```c
typedef struct _IMAGE_OPTIONAL_HEADER {
  WORD Magic;
  BYTE MajorLinkerVersion;
  BYTE MinorLinkerVersion;
  DWORD SizeOfCode;
  DWORD SizeOfInitializedData;
  DWORD SizeOfUninitializedData;
  DWORD AddressOfEntryPoint;
  DWORD BaseOfCode;
  DWORD BaseOfData;
  DWORD ImageBase;
  DWORD SectionAlignment;
  DWORD FileAlignment;
  WORD MajorOperatingSystemVersion;
  WORD MinorOperatingSystemVersion;
  WORD MajorImageVersion;
  WORD MinorImageVersion;
  WORD MajorSubsystemVersion;
  WORD MinorSubsystemVersion;
  DWORD Win32VersionValue;
  DWORD SizeOfImage;
  DWORD SizeOfHeaders;
  DWORD CheckSum;
  WORD Subsystem;
  WORD DllCharacteristics;
  DWORD SizeOfStackReserve;
  DWORD SizeOfStackCommit;
  DWORD SizeOfHeapReserve;
  DWORD SizeOfHeapCommit;
  DWORD LoaderFlags;
  DWORD NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32,*PIMAGE_OPTIONAL_HEADER32;
```

再看看64位的定义：

```c
typedef struct _IMAGE_OPTIONAL_HEADER64 {
  WORD Magic;
  BYTE MajorLinkerVersion;
  BYTE MinorLinkerVersion;
  DWORD SizeOfCode;
  DWORD SizeOfInitializedData;
  DWORD SizeOfUninitializedData;
  DWORD AddressOfEntryPoint;
  DWORD BaseOfCode;
  ULONGLONG ImageBase;
  DWORD SectionAlignment;
  DWORD FileAlignment;
  WORD MajorOperatingSystemVersion;
  WORD MinorOperatingSystemVersion;
  WORD MajorImageVersion;
  WORD MinorImageVersion;
  WORD MajorSubsystemVersion;
  WORD MinorSubsystemVersion;
  DWORD Win32VersionValue;
  DWORD SizeOfImage;
  DWORD SizeOfHeaders;
  DWORD CheckSum;
  WORD Subsystem;
  WORD DllCharacteristics;
  ULONGLONG SizeOfStackReserve;
  ULONGLONG SizeOfStackCommit;
  ULONGLONG SizeOfHeapReserve;
  ULONGLONG SizeOfHeapCommit;
  DWORD LoaderFlags;
  DWORD NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64,*PIMAGE_OPTIONAL_HEADER64;
```

在两个结构的第一个字段都是Magic，具体的位置在 0x00000098 开始的2个字节，
对于32位是0x010B，对于64位是0x020B。以下这个就是64位的：

```txt
Offset      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F

00000090                            0B 02 02 19 00 1C 00 00   .................
000000A0   00 3C 00 00 00 0A 00 00  E0 13 00 00 00 10 00 00   .................
```

到此已经可以写一个判断EXE/DLL文件基本信息的程序了，DosHeader到FileHeader可以直接用fread读取，
OptionalHeader以后单独处理，在OptionalHeader读取SizeOfOptionalHeader后，再读取Sections。

```c
typedef struct pedump_t {
    IMAGE_DOS_HEADER        DosHeader;
    DWORD                   Signature;
    IMAGE_FILE_HEADER       FileHeader;
    union {
    IMAGE_OPTIONAL_HEADER32 Header32;
    IMAGE_OPTIONAL_HEADER64 Header64;
    } OptionalHeader;
    PIMAGE_SECTION_HEADER   sections;
}pedump_t;
```

打印信息的程序片段如下：

```c
if (pe->OptionalHeader.Header32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    printf("PE32 ");
} else if (pe->OptionalHeader.Header64.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    printf("PE32+ ");
}

if (pe->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) {
    printf("executable ");
}

if (pe->FileHeader.Characteristics & IMAGE_FILE_DLL) {
    printf("(DLL) ");
}

if (pe->FileHeader.Characteristics & IMAGE_FILE_32BIT_MACHINE) {
    printf("(32bits) ");
}

if (pe->FileHeader.Machine == IMAGE_FILE_MACHINE_I386) {
    if (pe->OptionalHeader.Header32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) {
        printf("(console) ");
    } else if (pe->OptionalHeader.Header32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI) {
        printf("(GUI) ");
    } else {
        printf("(Subsystem:0x%04x) ", pe->OptionalHeader.Header32.Subsystem);
    }
} else {
    if (pe->OptionalHeader.Header64.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) {
        printf("(console) ");
    } else if (pe->OptionalHeader.Header64.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI) {
        printf("(GUI) ");
    } else {
        printf("(Subsystem:0x%04x) ", pe->OptionalHeader.Header64.Subsystem);
    }
}

switch (pe->FileHeader.Machine) {
    case IMAGE_FILE_MACHINE_I386:   printf("i386"); break;
    case IMAGE_FILE_MACHINE_AMD64:  printf("x86_64"); break;
    default: printf(" Machine:0x%04x", pe->FileHeader.Machine);
}
```

以上的输出格式和linux下的file命令输出一致。

```txt
demo-x86.exe: PE32 executable (32bits) (console) i386 (Thu Sep 22 16:07:15 2016)
demo-x64.exe: PE32+ executable (console) x86_64 (Thu Sep 22 16:26:42 2016)
```


二、导入表
---------------

想要知道依赖哪些DLL，需要关注两个信息，一个是Sections，一个是IMAGE_DATA_DIRECTORY。
大部分的程序是分为.text, .data, .bss等数据段的，Sections里存放了这些段的布局信息。

```c
typedef struct _IMAGE_SECTION_HEADER {
  BYTE Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
DWORD PhysicalAddress;
DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD NumberOfRelocations;
  WORD NumberOfLinenumbers;
  DWORD Characteristics;
} IMAGE_SECTION_HEADER,*PIMAGE_SECTION_HEADER;
```

例如用objdump输出文件的Sections信息：

```txt
Z:\>objdump -h demo.exe

demo.exe:     file format pei-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00001d38  0000000000401000  0000000000401000  00000400  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE, DATA
  1 .data         000000a8  0000000000403000  0000000000403000  00002200  2**4
                  CONTENTS, ALLOC, LOAD, DATA
  2 .rdata        00000b60  0000000000404000  0000000000404000  00002400  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  3 .pdata        00000264  0000000000405000  0000000000405000  00003000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .xdata        0000021c  0000000000406000  0000000000406000  00003400  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .bss          00000a50  0000000000407000  0000000000407000  00000000  2**6
```

其中需要关注的就是VMA和LMA，对应数据结构里的VirtualAddress和PointerToRawData。
前者是程序的虚拟地址，后者是加载地址，一般可以理解为文件偏移。
大部分情况下这个Sections表是按照升序排列的（除了恶意修改），
所以先不考虑乱序的问题。

```c
uint64_t rva2offset(const pedump_t* pe, uint64_t rva)
{
    int i;

    for (i = pe->FileHeader.NumberOfSections - 1;i >= 0;i--) {
        if (rva >= pe->sections[i].VirtualAddress) {
            return rva - pe->sections[i].VirtualAddress + pe->sections[i].PointerToRawData;
        }
    }

    return 0;
}
```

EXE/DLL的导入信息在IMAGE_OPTIONAL_HEADER的DataDirectory里：

```c
typedef struct {
  ......
  DWORD NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64,*PIMAGE_OPTIONAL_HEADER64;
```

IMAGE_DATA_DIRECTORY的定义如下：

```c
typedef struct _IMAGE_DATA_DIRECTORY {
  DWORD VirtualAddress;
  DWORD Size;
} IMAGE_DATA_DIRECTORY,*PIMAGE_DATA_DIRECTORY;
```

每个表的用途如下：

```c
#define IMAGE_DIRECTORY_ENTRY_EXPORT            0   //函数导出表
#define IMAGE_DIRECTORY_ENTRY_IMPORT            1   //函数导入表
#define IMAGE_DIRECTORY_ENTRY_RESOURCE          2   //资源表
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION         3
#define IMAGE_DIRECTORY_ENTRY_SECURITY          4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC         5
#define IMAGE_DIRECTORY_ENTRY_DEBUG             6
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE      7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR         8
#define IMAGE_DIRECTORY_ENTRY_TLS               9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG       10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT      11
#define IMAGE_DIRECTORY_ENTRY_IAT               12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT      13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR    14
```

本文只关心导入表，我们需要区别处理32位和64位格式，导入表的结构如下：

```c
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
  union {
    DWORD Characteristics;
        DWORD OriginalFirstThunk;
  } DUMMYUNIONNAME;
  DWORD TimeDateStamp;

  DWORD ForwarderChain;
  DWORD Name;
  DWORD FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;
```

每个导入项对应一个DLL，其中Name是虚拟地址偏移，需要转换为实际的文件偏移，
而OriginalFirstThunk对应的是导入函数列表，暂时不管，以下代码仅仅显示导入的DLL：

```c
int parse_import_directory(const pedump_t* pe, const uint8_t* buf) {
    IMAGE_DATA_DIRECTORY dir;
    PIMAGE_IMPORT_DESCRIPTOR imp;
    uint64_t offset;

    if (pe->FileHeader.Machine == IMAGE_FILE_MACHINE_I386) {
        dir = pe->OptionalHeader.Header32.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    } else {
        dir = pe->OptionalHeader.Header64.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }

    offset = rva2offset(pe, dir.VirtualAddress);
    imp = (PIMAGE_IMPORT_DESCRIPTOR)(buf + offset);

    for (;;imp++) {
        if (!imp->OriginalFirstThunk &&
            !imp->TimeDateStamp &&
            !imp->ForwarderChain &&
            !imp->Name &&
            !imp->FirstThunk)
        {
            break;
        }

        printf("  Import %s\n", (char*)(buf + rva2offset(pe, imp->Name)));
    }
}
```

最后的程序输出结果，explorer.exe：

```txt
c:\Windows\explorer.exe: PE32+ executable (GUI) x86_64 (Sat Nov 20 18:21:56 2010)
  Import ADVAPI32.dll
  Import KERNEL32.dll
  Import GDI32.dll
  Import USER32.dll
  Import msvcrt.dll
  Import ntdll.dll
  Import SHLWAPI.dll
  Import SHELL32.dll
  Import ole32.dll
  Import OLEAUT32.dll
  Import EXPLORERFRAME.dll
  Import UxTheme.dll
  Import POWRPROF.dll
  Import dwmapi.dll
  Import slc.dll
  Import gdiplus.dll
  Import Secur32.dll
  Import RPCRT4.dll
  Import PROPSYS.dll
```

shell32.dll

```txt
c:\Windows\System32\shell32.dll: PE32 executable (DLL) (32bits) (GUI) i386 (Sat Nov 20 20:06:54 2010)
  Import msvcrt.dll
  Import KERNELBASE.dll
  Import API-MS-Win-Core-Debug-L1-1-0.dll
  Import API-MS-Win-Core-ErrorHandling-L1-1-0.dll
  Import API-MS-Win-Core-File-L1-1-0.dll
  Import API-MS-Win-Core-Handle-L1-1-0.dll
  Import API-MS-Win-Core-Heap-L1-1-0.dll
  Import API-MS-Win-Core-Interlocked-L1-1-0.dll
  Import API-MS-Win-Core-IO-L1-1-0.dll
  Import API-MS-Win-Core-LibraryLoader-L1-1-0.dll
  Import API-MS-Win-Core-Localization-L1-1-0.dll
  Import API-MS-Win-Core-LocalRegistry-L1-1-0.dll
  Import API-MS-Win-Core-Memory-L1-1-0.dll
  Import API-MS-Win-Core-Misc-L1-1-0.dll
  Import API-MS-Win-Core-ProcessEnvironment-L1-1-0.dll
  Import API-MS-Win-Core-ProcessThreads-L1-1-0.dll
  Import API-MS-Win-Core-Profile-L1-1-0.dll
  Import API-MS-Win-Core-String-L1-1-0.dll
  Import API-MS-Win-Core-Synch-L1-1-0.dll
  Import API-MS-Win-Core-SysInfo-L1-1-0.dll
  Import API-MS-Win-Security-Base-L1-1-0.dll
  Import SHLWAPI.dll
  Import ntdll.dll
  Import USER32.dll
  Import GDI32.dll
  Import KERNEL32.dll
  Export SHELL32.dll
```

Qt5Core.dll:

```txt
Qt5Core.dll: PE32 executable (DLL) (32bits) (GUI) i386 (Fri Jun 10 15:20:20 2016)
  Import KERNEL32.dll
  Import USER32.dll
  Import SHELL32.dll
  Import ole32.dll
  Import ADVAPI32.dll
  Import WS2_32.dll
  Import MPR.dll
  Import MSVCP120.dll
  Import MSVCR120.dll
  Export Qt5Core.dll
```


- kernel32-amd64-6.1.7601.23677.dll.txt (Windows7 SP1 x64)
- kernel32-x86-5.1.2600.5512.dll.txt (WindowsXP SP3 x86)
- kernel32-x86-6.1.7601.23677.dll.txt (Windows7 SP1 WOW64)
- msvcrt-x86-7.0.2600.5512.dll.txt
- msvcrt-x86-7.0.7601.17744.dll.txt
- ntdll-amd64-6.1.7601.23677.dll.txt
- ntdll-x86-5.1.2600.5512.dll.txt
- ntdll-x86-6.1.7601.23677.dll.txt


