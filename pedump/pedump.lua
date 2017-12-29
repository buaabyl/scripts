local function readOnly (obj)
    local proxy = {};

    local mtReadOnly = {};
    mtReadOnly.__index = obj;
    mtReadOnly.__newindex = function(t, k, v)
        error("attempt to update a read-only table");
    end;
    mtReadOnly.__pairs = function(t)
        return pairs(obj);
    end;

    setmetatable(proxy, mtReadOnly);
    return proxy;
end
local _G = readOnly(_G);
local _ENV = readOnly(_ENV);

local M = {};

M.Const = {};
M.Const.Magic = {};
M.Const.Machine = {};
M.Const.CharacteristicsFlags = {};
M.Const.Subsystem = {};
M.Const.DllCharacteristicsFlags = {};
M.Const.Sections = {};
M.Const.Sections.CharacteristicsFlags = {};

local M_Const = M.const;
local M_Const_Magic = M.Const.Magic;
local M_Const_Machine = M.Const.Machine;
local M_Const_CharacteristicsFlags = M.Const.CharacteristicsFlags;
local M_Const_Subsystem = M.Const.Subsystem;
local M_Const_DllCharacteristicsFlags = M.Const.DllCharacteristicsFlags;
local M_Const_Section = M.Const.Section;
local M_Const_Sections_CharacteristicsFlags = M.Const.Sections.CharacteristicsFlags;

M_Const_Magic[0x10b] = "PE32";
M_Const_Magic[0x20b] = "PE32+";

M_Const_Machine[0x0000] = "IMAGE_FILE_MACHINE_UNKNOWN";
M_Const_Machine[0x01d3] = "IMAGE_FILE_MACHINE_AM33";
M_Const_Machine[0x8664] = "IMAGE_FILE_MACHINE_AMD64";
M_Const_Machine[0x01c0] = "IMAGE_FILE_MACHINE_ARM";
M_Const_Machine[0x01c4] = "IMAGE_FILE_MACHINE_ARMNT";
M_Const_Machine[0xaa64] = "IMAGE_FILE_MACHINE_ARM64";
M_Const_Machine[0x0ebc] = "IMAGE_FILE_MACHINE_EBC";
M_Const_Machine[0x014c] = "IMAGE_FILE_MACHINE_I386";
M_Const_Machine[0x0200] = "IMAGE_FILE_MACHINE_IA64";
M_Const_Machine[0x9041] = "IMAGE_FILE_MACHINE_M32R";
M_Const_Machine[0x0266] = "IMAGE_FILE_MACHINE_MIPS16";
M_Const_Machine[0x0366] = "IMAGE_FILE_MACHINE_MIPSFPU";
M_Const_Machine[0x0466] = "IMAGE_FILE_MACHINE_MIPSFPU16";
M_Const_Machine[0x01f0] = "IMAGE_FILE_MACHINE_POWERPC";
M_Const_Machine[0x01f1] = "IMAGE_FILE_MACHINE_POWERPCFP";
M_Const_Machine[0x0166] = "IMAGE_FILE_MACHINE_R4000";
M_Const_Machine[0x01a2] = "IMAGE_FILE_MACHINE_SH3";
M_Const_Machine[0x01a3] = "IMAGE_FILE_MACHINE_SH3DSP";
M_Const_Machine[0x01a6] = "IMAGE_FILE_MACHINE_SH4";
M_Const_Machine[0x01a8] = "IMAGE_FILE_MACHINE_SH5";
M_Const_Machine[0x01c2] = "IMAGE_FILE_MACHINE_THUMB";
M_Const_Machine[0x0169] = "IMAGE_FILE_MACHINE_WCEMIPSV2";

M_Const_CharacteristicsFlags[0x0001] = "IMAGE_FILE_RELOCS_STRIPPED";
M_Const_CharacteristicsFlags[0x0002] = "IMAGE_FILE_EXECUTABLE_IMAGE";
M_Const_CharacteristicsFlags[0x0004] = "IMAGE_FILE_LINE_NUMS_STRIPPED";
M_Const_CharacteristicsFlags[0x0008] = "IMAGE_FILE_LOCAL_SYMS_STRIPPED";
M_Const_CharacteristicsFlags[0x0010] = "IMAGE_FILE_AGGRESSIVE_WS_TRIM";
M_Const_CharacteristicsFlags[0x0020] = "IMAGE_FILE_LARGE_ADDRESS_AWARE";
M_Const_CharacteristicsFlags[0x0040] = "Reserved for future use";
M_Const_CharacteristicsFlags[0x0080] = "IMAGE_FILE_BYTES_REVERSED_LO";
M_Const_CharacteristicsFlags[0x0100] = "IMAGE_FILE_32BIT_MACHINE";
M_Const_CharacteristicsFlags[0x0200] = "IMAGE_FILE_DEBUG_STRIPPED";
M_Const_CharacteristicsFlags[0x0400] = "IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP";
M_Const_CharacteristicsFlags[0x0800] = "IMAGE_FILE_NET_RUN_FROM_SWAP";
M_Const_CharacteristicsFlags[0x1000] = "IMAGE_FILE_SYSTEM";
M_Const_CharacteristicsFlags[0x2000] = "IMAGE_FILE_DLL";
M_Const_CharacteristicsFlags[0x4000] = "IMAGE_FILE_UP_SYSTEM_ONLY";
M_Const_CharacteristicsFlags[0x8000] = "IMAGE_FILE_BYTES_REVERSED_HI";

M_Const_Subsystem[0x0] = "IMAGE_SUBSYSTEM_UNKNOWN";
M_Const_Subsystem[0x1] = "IMAGE_SUBSYSTEM_NATIVE";
M_Const_Subsystem[0x2] = "IMAGE_SUBSYSTEM_WINDOWS_GUI";
M_Const_Subsystem[0x3] = "IMAGE_SUBSYSTEM_WINDOWS_CUI";
M_Const_Subsystem[0x7] = "IMAGE_SUBSYSTEM_POSIX_CUI";
M_Const_Subsystem[0x9] = "IMAGE_SUBSYSTEM_WINDOWS_CE_GUI";
M_Const_Subsystem[0xa] = "IMAGE_SUBSYSTEM_EFI_APPLICATION";
M_Const_Subsystem[0xb] = "IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER";
M_Const_Subsystem[0xc] = "IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER";
M_Const_Subsystem[0xd] = "IMAGE_SUBSYSTEM_EFI_ROM";
M_Const_Subsystem[0xe] = "IMAGE_SUBSYSTEM_XBOX";

M_Const_DllCharacteristicsFlags[0x0040] = "IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE";
M_Const_DllCharacteristicsFlags[0x0080] = "IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY";
M_Const_DllCharacteristicsFlags[0x0100] = "IMAGE_DLL_CHARACTERISTICS_NX_COMPAT";
M_Const_DllCharacteristicsFlags[0x0200] = "IMAGE_DLLCHARACTERISTICS_NO_ISOLATION";
M_Const_DllCharacteristicsFlags[0x0400] = "IMAGE_DLLCHARACTERISTICS_NO_SEH";
M_Const_DllCharacteristicsFlags[0x0800] = "IMAGE_DLLCHARACTERISTICS_NO_BIND";
M_Const_DllCharacteristicsFlags[0x2000] = "IMAGE_DLLCHARACTERISTICS_WDM_DRIVER";
M_Const_DllCharacteristicsFlags[0x8000] = "IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE";

M_Const_Sections_CharacteristicsFlags[0x00000008] = "IMAGE_SCN_TYPE_NO_PAD";
M_Const_Sections_CharacteristicsFlags[0x00000020] = "IMAGE_SCN_CNT_CODE";
M_Const_Sections_CharacteristicsFlags[0x00000040] = "IMAGE_SCN_CNT_INITIALIZED_DATA";
M_Const_Sections_CharacteristicsFlags[0x00000080] = "IMAGE_SCN_CNT_UNINITIALIZED_ DATA";
M_Const_Sections_CharacteristicsFlags[0x00000100] = "IMAGE_SCN_LNK_OTHER";
M_Const_Sections_CharacteristicsFlags[0x00000200] = "IMAGE_SCN_LNK_INFO";
M_Const_Sections_CharacteristicsFlags[0x00000800] = "IMAGE_SCN_LNK_REMOVE";
M_Const_Sections_CharacteristicsFlags[0x00001000] = "IMAGE_SCN_LNK_COMDAT";
M_Const_Sections_CharacteristicsFlags[0x00008000] = "IMAGE_SCN_GPREL";
M_Const_Sections_CharacteristicsFlags[0x00020000] = "IMAGE_SCN_MEM_PURGEABLE";
M_Const_Sections_CharacteristicsFlags[0x00020000] = "IMAGE_SCN_MEM_16BIT";
M_Const_Sections_CharacteristicsFlags[0x00040000] = "IMAGE_SCN_MEM_LOCKED";
M_Const_Sections_CharacteristicsFlags[0x00080000] = "IMAGE_SCN_MEM_PRELOAD";
M_Const_Sections_CharacteristicsFlags[0x00100000] = "IMAGE_SCN_ALIGN_1BYTES";
M_Const_Sections_CharacteristicsFlags[0x00200000] = "IMAGE_SCN_ALIGN_2BYTES";
M_Const_Sections_CharacteristicsFlags[0x00300000] = "IMAGE_SCN_ALIGN_4BYTES";
M_Const_Sections_CharacteristicsFlags[0x00400000] = "IMAGE_SCN_ALIGN_8BYTES";
M_Const_Sections_CharacteristicsFlags[0x00500000] = "IMAGE_SCN_ALIGN_16BYTES";
M_Const_Sections_CharacteristicsFlags[0x00600000] = "IMAGE_SCN_ALIGN_32BYTES";
M_Const_Sections_CharacteristicsFlags[0x00700000] = "IMAGE_SCN_ALIGN_64BYTES";
M_Const_Sections_CharacteristicsFlags[0x00800000] = "IMAGE_SCN_ALIGN_128BYTES";
M_Const_Sections_CharacteristicsFlags[0x00900000] = "IMAGE_SCN_ALIGN_256BYTES";
M_Const_Sections_CharacteristicsFlags[0x00a00000] = "IMAGE_SCN_ALIGN_512BYTES";
M_Const_Sections_CharacteristicsFlags[0x00b00000] = "IMAGE_SCN_ALIGN_1024BYTES";
M_Const_Sections_CharacteristicsFlags[0x00c00000] = "IMAGE_SCN_ALIGN_2048BYTES";
M_Const_Sections_CharacteristicsFlags[0x00d00000] = "IMAGE_SCN_ALIGN_4096BYTES";
M_Const_Sections_CharacteristicsFlags[0x00e00000] = "IMAGE_SCN_ALIGN_8192BYTES";
M_Const_Sections_CharacteristicsFlags[0x01000000] = "IMAGE_SCN_LNK_NRELOC_OVFL";
M_Const_Sections_CharacteristicsFlags[0x02000000] = "IMAGE_SCN_MEM_DISCARDABLE";
M_Const_Sections_CharacteristicsFlags[0x04000000] = "IMAGE_SCN_MEM_NOT_CACHED";
M_Const_Sections_CharacteristicsFlags[0x08000000] = "IMAGE_SCN_MEM_NOT_PAGED";
M_Const_Sections_CharacteristicsFlags[0x10000000] = "IMAGE_SCN_MEM_SHARED";
M_Const_Sections_CharacteristicsFlags[0x20000000] = "IMAGE_SCN_MEM_EXECUTE";
M_Const_Sections_CharacteristicsFlags[0x40000000] = "IMAGE_SCN_MEM_READ";
M_Const_Sections_CharacteristicsFlags[0x80000000] = "IMAGE_SCN_MEM_WRITE";

local function printf(fmt, ...)
    local str = string.format(fmt, ...);
    print(str);
end

local function dump_flags(flags, tbFlags)
    local r = {};
    for k,v in pairs(tbFlags) do
        if (k & flags ~= 0) then
            r[#r+1]=v;
        end
    end
    return table.concat(r, " | ");
end

local function ReadFile(filepath)
    local f, err = io.open(filepath, "rb");
    local buf = f:read("*all");
    f:close();
    return buf;
end

local function ReadUInt(buf, offset, size)
    local data = buf:sub(offset+1, offset+size);
    assert(data:len() == size, "assert:" .. offset .. " " .. data:len() .. "~=" .. size);
    if (size==4) then
        data = string.unpack("=L", data);
    elseif (size==2) then
        data = string.unpack("=H", data);
    elseif (size==1) then
        data = string.unpack("=B", data);
    elseif (size==8) then
        data = string.unpack("=I8", data);
    end
    return offset+size, data;
end

--文件头
local function DumpFileHdr(buf, offset)
    local FileHdr = {};
    offset, FileHdr.Machine = ReadUInt(buf, offset, 2);
    printf("%-24s %s", "Machine", M_Const_Machine[FileHdr.Machine]);

    offset, FileHdr.NumberOfSections = ReadUInt(buf, offset, 2);
    printf("%-24s %X", "NumberOfSections", FileHdr.NumberOfSections);

    offset, FileHdr.TimeDateStamp = ReadUInt(buf, offset, 4);
    printf("%-24s %X %s", "TimeDateStamp", FileHdr.TimeDateStamp, os.date("!%Y-%m-%d %H:%M:%S",FileHdr.TimeDateStamp));

    offset, FileHdr.PointerToSymbolTable = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "PointerToSymbolTable", FileHdr.PointerToSymbolTable);

    offset, FileHdr.NumberOfSymbols = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "NumberOfSymbols", FileHdr.NumberOfSymbols);

    offset, FileHdr.SizeOfOptionalHeader= ReadUInt(buf, offset, 2);
    printf("%-24s %X", "SizeOfOptionalHeader", FileHdr.SizeOfOptionalHeader);

    offset, FileHdr.Characteristics= ReadUInt(buf, offset, 2);
    printf("%-24s %s", "Characteristics", dump_flags(FileHdr.Characteristics, M_Const_CharacteristicsFlags));

    return offset, FileHdr;
end

local function DumpOptHdr(buf, offset)
    local OptHdr = {};

    offset, OptHdr.Magic = ReadUInt(buf, offset, 2);
    printf("%-24s %s", "Magic", M_Const_Magic[OptHdr.Magic]);

    offset, OptHdr.MajorLinkerVersion = ReadUInt(buf, offset, 1);
    offset, OptHdr.MinorLinkerVersion = ReadUInt(buf, offset, 1);
    printf("%-24s %u.%u", "LinkerVersion", OptHdr.MajorLinkerVersion, OptHdr.MinorLinkerVersion);

    offset, OptHdr.SizeOfCode= ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SizeOfCode", OptHdr.SizeOfCode);

    offset, OptHdr.SizeOfInitializedData= ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SizeOfInitializedData", OptHdr.SizeOfInitializedData);

    offset, OptHdr.SizeOfUninitializedData= ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SizeOfUninitializedData", OptHdr.SizeOfUninitializedData);

    offset, OptHdr.AddressOfEntryPoint= ReadUInt(buf, offset, 4);
    printf("%-24s %X", "AddressOfEntryPoint", OptHdr.AddressOfEntryPoint);

    offset, OptHdr.BaseOfCode= ReadUInt(buf, offset, 4);
    printf("%-24s %X", "BaseOfCode", OptHdr.BaseOfCode);

    if (M_Const_Magic[OptHdr.Magic] == "PE32") then
        offset, OptHdr.BaseOfData = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "BaseOfData", OptHdr.BaseOfData);

        offset, OptHdr.ImageBase = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "ImageBase", OptHdr.ImageBase);
    elseif (M_Const_Magic[OptHdr.Magic] == "PE32+") then
        offset, OptHdr.ImageBase = ReadUInt(buf, offset, 8);
        printf("%-24s %X", "ImageBase", OptHdr.ImageBase);
    else
        assert(false);
    end

    offset, OptHdr.SectionAlignment = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SectionAlignment", OptHdr.SectionAlignment);

    offset, OptHdr.FileAlignment = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "FileAlignment", OptHdr.FileAlignment);

    offset, OptHdr.MajorOperatingSystemVersion = ReadUInt(buf, offset, 2);
    offset, OptHdr.MinorOperatingSystemVersion = ReadUInt(buf, offset, 2);
    printf("%-24s %u.%u", "OperatingSystemVersion", OptHdr.MajorOperatingSystemVersion, OptHdr.MinorOperatingSystemVersion);

    offset, OptHdr.MajorImageVersion = ReadUInt(buf, offset, 2);
    offset, OptHdr.MinorImageVersion = ReadUInt(buf, offset, 2);
    printf("%-24s %u.%u", "ImageVersion", OptHdr.MajorImageVersion, OptHdr.MinorImageVersion);

    offset, OptHdr.MajorSubsystemVersion = ReadUInt(buf, offset, 2);
    offset, OptHdr.MinorSubsystemVersion = ReadUInt(buf, offset, 2);
    printf("%-24s %u.%u", "SubsystemVersion", OptHdr.MajorSubsystemVersion, OptHdr.MinorSubsystemVersion);

    offset, OptHdr.Win32VersionValue = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "Win32VersionValue", OptHdr.Win32VersionValue);

    offset, OptHdr.SizeOfImage = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SizeOfImage", OptHdr.SizeOfImage);

    offset, OptHdr.SizeOfHeaders = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "SizeOfHeaders", OptHdr.SizeOfHeaders);

    offset, OptHdr.CheckSum = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "CheckSum", OptHdr.CheckSum);

    offset, OptHdr.Subsystem = ReadUInt(buf, offset, 2);
    printf("%-24s %s", "Subsystem", M_Const_Subsystem[OptHdr.Subsystem]);

    offset, OptHdr.DllCharacteristics = ReadUInt(buf, offset, 2);
    printf("%-24s %s", "DllCharacteristics", dump_flags(OptHdr.DllCharacteristics,M_Const_DllCharacteristicsFlags));

    if (M_Const_Magic[OptHdr.Magic] == "PE32") then
        offset, OptHdr.SizeOfStackReserve = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "SizeOfStackReserve", OptHdr.SizeOfStackReserve);

        offset, OptHdr.SizeOfStackCommit = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "SizeOfStackCommit", OptHdr.SizeOfStackCommit);

        offset, OptHdr.SizeOfHeapReserve = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "SizeOfHeapReserve", OptHdr.SizeOfHeapReserve);

        offset, OptHdr.SizeOfHeapCommit = ReadUInt(buf, offset, 4);
        printf("%-24s %X", "SizeOfHeapCommit", OptHdr.SizeOfHeapCommit);
    elseif (M_Const_Magic[OptHdr.Magic] == "PE32+") then
        offset, OptHdr.SizeOfStackReserve = ReadUInt(buf, offset, 8);
        printf("%-24s %X", "SizeOfStackReserve", OptHdr.SizeOfStackReserve);

        offset, OptHdr.SizeOfStackCommit = ReadUInt(buf, offset, 8);
        printf("%-24s %X", "SizeOfStackCommit", OptHdr.SizeOfStackCommit);

        offset, OptHdr.SizeOfHeapReserve = ReadUInt(buf, offset, 8);
        printf("%-24s %X", "SizeOfHeapReserve", OptHdr.SizeOfHeapReserve);

        offset, OptHdr.SizeOfHeapCommit = ReadUInt(buf, offset, 8);
        printf("%-24s %X", "SizeOfHeapCommit", OptHdr.SizeOfHeapCommit);
    else
        assert(false);
    end

    offset, OptHdr.LoaderFlags = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "LoaderFlags", OptHdr.LoaderFlags);

    offset, OptHdr.NumberOfRvaAndSizes = ReadUInt(buf, offset, 4);
    printf("%-24s %X", "NumberOfRvaAndSizes", OptHdr.NumberOfRvaAndSizes);

    return offset, OptHdr;
end

local function DumpDataDir(buf, offset)
    local ddir = {};
    for i=1, 16 do
        ddir[i] = {};
        local ddiri = ddir[i];

        offset, ddiri.VirtualAddress = ReadUInt(buf, offset, 4);
        offset, ddiri.Size = ReadUInt(buf, offset, 4);

        printf("DataDirectory %.2d\tVirtualAddress = %.8X, Size = %.8X", i, ddiri.VirtualAddress, ddiri.Size);
    end
    return offset, ddir;
end

local function DumpSection(buf, offset, hdr_file)
    local Section = {};
    for i=1, hdr_file.NumberOfSections do
        local SectionI = {};
        Section[i] = SectionI;

        local data = nil;
        offset, data = ReadUInt(buf, offset, 8);
        SectionI.Name = string.pack("=I8", data);

        offset, data = ReadUInt(buf, offset, 4);
        SectionI.Misc_PhysicalAddress = data;
        SectionI.Misc_VirtualSize = data;

        offset, SectionI.VirtualAddress = ReadUInt(buf, offset, 4);
        offset, SectionI.SizeOfRawData = ReadUInt(buf, offset, 4);
        offset, SectionI.PointerToRawData = ReadUInt(buf, offset, 4);
        offset, SectionI.PointerToRelocations = ReadUInt(buf, offset, 4);
        offset, SectionI.PointerToLinenumbers = ReadUInt(buf, offset, 4);
        offset, SectionI.NumberOfRelocations = ReadUInt(buf, offset, 2);
        offset, SectionI.NumberOfLinenumbers = ReadUInt(buf, offset, 2);
        offset, SectionI.Characteristics = ReadUInt(buf, offset, 4);

        printf("%-24s %s", "SectionName", SectionI.Name);
        printf("%-24s %X", "Misc_PhysicalAddress", SectionI.Misc_PhysicalAddress);
        printf("%-24s %X", "Misc_VirtualSize", SectionI.Misc_VirtualSize);
        printf("%-24s %X", "VirtualAddress", SectionI.VirtualAddress);
        printf("%-24s %X", "SizeOfRawData", SectionI.SizeOfRawData);
        printf("%-24s %X", "PointerToRawData", SectionI.PointerToRawData);
        printf("%-24s %X", "PointerToRelocations", SectionI.PointerToRelocations);
        printf("%-24s %X", "PointerToLinenumbers", SectionI.PointerToLinenumbers);
        printf("%-24s %X", "NumberOfRelocations", SectionI.NumberOfRelocations);
        printf("%-24s %X", "NumberOfLinenumbers", SectionI.NumberOfLinenumbers);
        printf("%-24s %s", "Characteristics", dump_flags(SectionI.Characteristics,M_Const_Sections_CharacteristicsFlags));
        print();
    end

    return offset, Section;
end

local function DumpHdr(buf)
    --判断MZ头
    local offset, data = ReadUInt(buf, 0, 2);
    assert(data==0x5A4D, "MZ fail");

    --获取PE头偏移
    offset, data= ReadUInt(buf, 0x3C, 2);
    offset = data;

    --判断PE头
    offset, data = ReadUInt(buf, offset, 2);
    assert(data==0x4550, "PE fail");

    --跳过2字节
    offset, data = ReadUInt(buf, offset, 2);

    --读取文件头，可选头，数据目录和节目录
    local hdr = {};

    offset, hdr.file = DumpFileHdr(buf, offset);
    print();

    offset, hdr.opt = DumpOptHdr(buf, offset);
    print();

    offset, hdr.ddir = DumpDataDir(buf, offset);
    print();

    offset, hdr.sec = DumpSection(buf, offset, hdr.file);
    print();

    local hdr_sec = hdr.sec;

    --工具函数
    local helper = {};

    --相对偏移转文件偏移
    local function RVA2FOA(rva)
        local foa = nil;
        for i=1, #hdr_sec do
            local first = hdr_sec[i].VirtualAddress;
            local second = first + hdr_sec[i].SizeOfRawData;
            if ( (rva >= first) and (rva < second ) ) then
                foa = hdr_sec[i].PointerToRawData + ( rva - first );
                break;
            end
        end
        return foa;
    end

    --文件偏移转相对偏移
    local function FOA2RVA(foa)
        local rva = nil;
        for i=1, #hdr_sec do
            local first = hdr_sec[i].PointerToRawData;
            local second = first + hdr_sec[i].SizeOfRawData;
            if ( (foa >= first) and (foa < second ) ) then
                rva = hdr_sec[i].VirtualAddress + ( foa - first );
                break;
            end
        end
        return rva;
    end

    --读取字符串
    local function ReadStringByOffset(offset)
        local End = buf:find("\0", offset+1);
        return buf:sub(offset+1, End);
    end

    --读取字符串
    local function ReadStringByRVA(rva)
        local Offset = RVA2FOA(rva);
        return ReadStringByOffset(Offset);
    end

    --读取字符串
    local function ReadBufferByRVA(rva, size)
        local Offset = RVA2FOA(rva);
        return buf:sub(Offset+1, Offset+size);
    end

    helper.RVA2FOA = RVA2FOA;
    helper.FOA2RVA = FOA2RVA;
    helper.ReadStringByOffset = ReadStringByOffset;
    helper.ReadStringByRVA = ReadStringByRVA;
    helper.ReadBufferByRVA= ReadBufferByRVA;

    return offset, hdr, helper;
end

local function DumpExport(buf, header, helper)
    local hdr_ddir = header.ddir;

    local RVA2FOA = helper.RVA2FOA;
    local ReadStringByRVA = helper.ReadStringByRVA;

    local ExportSize = hdr_ddir[1].Size;
    local ExportAddress = hdr_ddir[1].VirtualAddress;

    local Exports = {};
    if (ExportAddress>0 and ExportSize>0) then
        local offset = RVA2FOA(ExportAddress);
        offset, Exports.Characteristics = ReadUInt(buf, offset, 4);
        offset, Exports.TimeDateStamp = ReadUInt(buf, offset, 4);
        offset, Exports.MajorVersion = ReadUInt(buf, offset, 2);
        offset, Exports.MinorVersion = ReadUInt(buf, offset, 2);
        offset, Exports.Name = ReadUInt(buf, offset, 4);
        offset, Exports.Base = ReadUInt(buf, offset, 4);
        offset, Exports.NumberOfFunctions = ReadUInt(buf, offset, 4);
        offset, Exports.NumberOfNames = ReadUInt(buf, offset, 4);
        offset, Exports.AddressOfFunctions = ReadUInt(buf, offset, 4);
        offset, Exports.AddressOfNames = ReadUInt(buf, offset, 4);
        offset, Exports.AddressOfNameOrdinals = ReadUInt(buf, offset, 4);

        print("Export", ReadStringByRVA(Exports.Name));

        Exports.AddressOfNames = RVA2FOA(Exports.AddressOfNames);
        Exports.AddressOfFunctions = RVA2FOA(Exports.AddressOfFunctions);
        Exports.AddressOfNameOrdinals = RVA2FOA(Exports.AddressOfNameOrdinals);

        local mapNumberName = {};
        for i=1, Exports.NumberOfNames do
            local apiNumber = nil;
            offset, apiNumber = ReadUInt(buf, Exports.AddressOfNameOrdinals+(i-1)*2, 2);
            apiNumber = apiNumber + Exports.Base;

            local apiName = nil;
            offset, apiName = ReadUInt(buf, Exports.AddressOfNames+(i-1)*4, 4);
            apiName = ReadStringByRVA(apiName);

            mapNumberName[apiNumber] = apiName;
        end

        local tbExports = {};
        for i=1, Exports.NumberOfFunctions do
            local apiAddress = nil;
            offset, apiAddress = ReadUInt(buf, Exports.AddressOfFunctions+(i-1)*4, 4);

            local apiNumber = Exports.Base-1 + i;
            local apiName = mapNumberName[apiNumber] or "";

            if ( apiAddress >= ExportAddress) and (apiAddress < ExportAddress+ExportSize) then
                apiAddress = ReadStringByRVA(apiAddress);
            end

            tbExports[1+#tbExports] = {["index"]=apiNumber, ["name"]=apiName, ["apiFun"]=apiAddress};
        end

        mapNumberName = nil;

        table.sort(tbExports,   function(l,r)
                        if (l.name~="" and r.name~="") then
                            return l.name < r.name;
                        elseif (l.name=="" and r.name=="") then
                            return l.index < r.index;
                        elseif (l.name~="" and r.name=="") then
                            return true;
                        elseif (l.name=="" and r.name~="") then
                            return false;
                        end
                    end);

        for _,v in ipairs(tbExports) do
            if (type(v.apiFun)=="string") then
                printf("%16d %24s %-48s", v.index, v.apiFun, v.name);
            elseif (v.apiFun ~= 0) then
                printf("%16d %24X %-48s", v.index, v.apiFun, v.name);
            end
        end -- for
    end -- if

    return Exports;
end

local function DumpImport(buf, header, helper)
    local hdr_ddir = header.ddir;
    local hdr_opt = header.opt;

    local RVA2FOA = helper.RVA2FOA;
    local FOA2RVA = helper.FOA2RVA;

    local ReadStringByRVA = helper.ReadStringByRVA;
    local ReadStringByOffset = helper.ReadStringByOffset;

    local ImportSize = hdr_ddir[2].Size;
    local ImportAddress = hdr_ddir[2].VirtualAddress;

    local offset = RVA2FOA(ImportAddress);

    local Imports = {};

    repeat
        if (ImportAddress<=0) then break; end
        if (ImportSize<=0) then break; end

        local Import = {};
        Imports[#Imports+1] = Import;

        offset, Import.Characteristics = ReadUInt(buf, offset, 4);
        offset, Import.TimeDateStamp = ReadUInt(buf, offset, 4);
        offset, Import.ForwarderChain = ReadUInt(buf, offset, 4);
        offset, Import.Name = ReadUInt(buf, offset, 4);
        offset, Import.FirstThunk = ReadUInt(buf, offset, 4);

        if (0==Import.Name) then break; end
        --if (0==Import.Characteristics) then break; end
        if (0==Import.FirstThunk) then break; end

        print("Import", ReadStringByRVA(Import.Name));

        --local offINT = RVA2FOA(Import.Characteristics);
        local offIAT = RVA2FOA(Import.FirstThunk);

        repeat
            local pINT = nil;
            local pIAT = nil;

            local rvaIAT = FOA2RVA(offIAT);
            if (M_Const_Magic[hdr_opt.Magic] == "PE32") then
                --offINT, pINT = ReadUInt(buf, offINT, 4);
                offIAT, pIAT = ReadUInt(buf, offIAT, 4);
            elseif (M_Const_Magic[hdr_opt.Magic] == "PE32+") then
                --offINT, pINT = ReadUInt(buf, offINT, 8);
                offIAT, pIAT = ReadUInt(buf, offIAT, 8);
            else
                assert(false);
            end

            --assert((pINT==0 and pIAT==0) or (pINT~=0 and pIAT~=0));

            --if (0 == pINT) then break; end
            if (0 == pIAT) then break; end

            pINT = pIAT;
            if (M_Const_Magic[hdr_opt.Magic] == "PE32") then
                if (0 == pINT & 0x80000000) then
                    pINT = RVA2FOA(pINT);
                    printf("%16X %s", rvaIAT, ReadStringByOffset(pINT+2));
                else
                    printf("%16X %d", rvaIAT, pINT-0x80000000);
                end
            elseif (M_Const_Magic[hdr_opt.Magic] == "PE32+") then
                if (0 == pINT & 0x8000000000000000) then
                    pINT = RVA2FOA(pINT);
                    printf("\t%8X\t%s", rvaIAT, ReadStringByOffset(pINT+2));
                else
                    printf("\t%8X\t%d", rvaIAT, pINT-0x8000000000000000);
                end
            end

        until (false);
    until (false);
end

local function parse(pe_filepath)
    --读入PE文件
    local buf = ReadFile(pe_filepath);
    if (not buf) then
        error("read fail");
    end

    local offset, header, helper = DumpHdr(buf);

    DumpImport(buf, header, helper);
    DumpExport(buf, header, helper);
end

parse(arg[1]);
