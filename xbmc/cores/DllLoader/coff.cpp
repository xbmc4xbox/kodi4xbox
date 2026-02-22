/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "coff.h"
#include "coffldr.h"

//#define DUMPING_DATA 1

#ifndef __GNUC__
#pragma warning (disable:4806)
#endif

#include "utils/log.h"

const char *DATA_DIR_NAME[16] =
  {
    "Export Table",
    "Import Table",
    "Resource Table",
    "Exception Table",
    "Certificate Table",
    "Base Relocation Table",
    "Debug",
    "Architecture",
    "Global Ptr",
    "TLS Table",
    "Load Config Table",
    "Bound Import",
    "IAT",
    "Delay Import Descriptor",
    "COM+ Runtime Header",
    "Reserved"
  };


CoffLoader::CoffLoader()
{
  CoffFileHeader = 0;
  OptionHeader = 0;
  WindowsHeader = 0;
  Directory = 0;
  SectionHeader = 0;
  SymTable = 0;
  StringTable = 0;
  SectionData = 0;

  NumberOfSymbols = 0;
  SizeOfStringTable = 0;
  NumOfDirectories = 0;
  NumOfSections = 0;
  FileHeaderOffset = 0;
  hModule = NULL;
}

CoffLoader::~CoffLoader()
{
  if ( hModule )
  {
    VirtualFree(hModule, 0, MEM_RELEASE);
    hModule = NULL;
  }
  if ( SymTable )
  {
    delete [] SymTable;
    SymTable = 0;
  }
  if ( StringTable )
  {
    delete [] StringTable;
    StringTable = 0;
  }
  if ( SectionData )
  {
    delete [] SectionData;
    SectionData = 0;
  }
}

// Has nothing to do with the coff loader itself
// it can be used to parse the headers of a dll
// already loaded into memory
int CoffLoader::ParseHeaders(void* hModule)
{
  if (strncmp((char*)hModule, "MZ", 2) != 0)
    return 0;

  int* Offset = (int*)((char*)hModule+0x3c);
  if (*Offset <= 0)
    return 0;

  if (strncmp((char*)hModule+*Offset, "PE\0\0", 4) != 0)
    return 0;

  FileHeaderOffset = *Offset + 4;

  CoffFileHeader = (COFF_FileHeader_t *) ( (char*)hModule + FileHeaderOffset );
  NumOfSections = CoffFileHeader->NumberOfSections;

  OptionHeader = (OptionHeader_t *) ( (char*)CoffFileHeader + sizeof(COFF_FileHeader_t) );
  WindowsHeader = (WindowsHeader_t *) ( (char*)OptionHeader + OPTHDR_SIZE );
  EntryAddress = OptionHeader->Entry;
  NumOfDirectories = WindowsHeader->NumDirectories;

  Directory = (Image_Data_Directory_t *) ( (char*)WindowsHeader + WINHDR_SIZE);
  SectionHeader = (SectionHeader_t *) ( (char*)Directory + sizeof(Image_Data_Directory_t) * NumOfDirectories);

  if (CoffFileHeader->MachineType != IMAGE_FILE_MACHINE_I386)
    return 0;

#ifdef DUMPING_DATA
  PrintFileHeader(CoffFileHeader);
#endif

  if ( CoffFileHeader->SizeOfOptionHeader == 0 ) //not an image file, object file maybe
    return 0;

  // process Option Header
  if (OptionHeader->Magic == OPTMAGIC_PE32P)
  {
    CLog::Log(LOGDEBUG, "PE32+ not supported\n");
    return 0;
  }
  else if (OptionHeader->Magic == OPTMAGIC_PE32)
  {

#ifdef DUMPING_DATA
    PrintOptionHeader(OptionHeader);
    PrintWindowsHeader(WindowsHeader);
#endif

  }
  else
  {
    //add error message
    return 0;
  }

#ifdef DUMPING_DATA
  for (int DirCount = 0; DirCount < NumOfDirectories; DirCount++)
  {
    CLog::Log(LOGDEBUG, "Data Directory {:02}: {}\n", DirCount + 1, DATA_DIR_NAME[DirCount]);
    CLog::Log(LOGDEBUG, "                    RVA:  {:08X}\n", Directory[DirCount].RVA);
    CLog::Log(LOGDEBUG, "                    Size: {:08X}\n\n", Directory[DirCount].Size);
  }
#endif

  return 1;

}

int CoffLoader::LoadCoffHModule(FILE *fp)
{
  //test file signatures
  char Sig[4];
  rewind(fp);
  memset(Sig, 0, sizeof(Sig));
  fread(Sig, 1, 2, fp);
  if (strncmp(Sig, "MZ", 2) != 0)
    return 0;

  int Offset = 0;
  fseek(fp, 0x3c, SEEK_SET);
  fread(&Offset, sizeof(int), 1, fp);
  if (Offset <= 0)
    return 0;

  fseek(fp, Offset, SEEK_SET);
  memset(Sig, 0, sizeof(Sig));
  fread(Sig, 1, 4, fp);
  if (strncmp(Sig, "PE\0\0", 4) != 0)
    return 0;

  Offset += 4;
  FileHeaderOffset = Offset;

  // Load and process Header
  if (fseek(fp, FileHeaderOffset + sizeof(COFF_FileHeader_t) + OPTHDR_SIZE, SEEK_SET)) //skip to winows headers
    return 0;

  WindowsHeader_t tempWindowsHeader;
  size_t readcount = fread(&tempWindowsHeader, 1, WINHDR_SIZE, fp);
  if (readcount != WINHDR_SIZE) //test file size error
    return 0;

  // alloc aligned memory
#ifdef _XBOX
  hModule = VirtualAllocEx(0, (PVOID)tempWindowsHeader.ImageBase, tempWindowsHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (hModule == NULL)
    hModule = VirtualAlloc(0, tempWindowsHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
  hModule = VirtualAllocEx(GetCurrentProcess(), (PVOID)tempWindowsHeader.ImageBase, tempWindowsHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (hModule == NULL)
    hModule = VirtualAlloc(GetCurrentProcess(), tempWindowsHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
  if (hModule == NULL)
    return 0;   //memory allocation fails

  rewind(fp);
  readcount = fread(hModule, 1, tempWindowsHeader.SizeOfHeaders, fp);
  if (readcount != tempWindowsHeader.SizeOfHeaders)   //file size error
    return 0;

  CoffFileHeader = (COFF_FileHeader_t *) ( (char*)hModule + FileHeaderOffset );
  NumOfSections = CoffFileHeader->NumberOfSections;

  OptionHeader = (OptionHeader_t *) ( (char*)CoffFileHeader + sizeof(COFF_FileHeader_t) );
  WindowsHeader = (WindowsHeader_t *) ( (char*)OptionHeader + OPTHDR_SIZE );
  EntryAddress = OptionHeader->Entry;
  NumOfDirectories = WindowsHeader->NumDirectories;

  Directory = (Image_Data_Directory_t *) ( (char*)WindowsHeader + WINHDR_SIZE);
  SectionHeader = (SectionHeader_t *) ( (char*)Directory + sizeof(Image_Data_Directory_t) * NumOfDirectories);

  if (CoffFileHeader->MachineType != IMAGE_FILE_MACHINE_I386)
    return 0;

#ifdef DUMPING_DATA
  PrintFileHeader(CoffFileHeader);
#endif

  if ( CoffFileHeader->SizeOfOptionHeader == 0 ) //not an image file, object file maybe
    return 0;

  // process Option Header
  if (OptionHeader->Magic == OPTMAGIC_PE32P)
  {
    CLog::Log(LOGDEBUG, "PE32+ not supported\n");
    return 0;
  }
  else if (OptionHeader->Magic == OPTMAGIC_PE32)
  {

#ifdef DUMPING_DATA
    PrintOptionHeader(OptionHeader);
    PrintWindowsHeader(WindowsHeader);
#endif

  }
  else
  {
    //add error message
    return 0;
  }

#ifdef DUMPING_DATA
  for (int DirCount = 0; DirCount < NumOfDirectories; DirCount++)
  {
    CLog::Log(LOGDEBUG, "Data Directory {:02}: {}\n", DirCount + 1, DATA_DIR_NAME[DirCount]);
    CLog::Log(LOGDEBUG, "                    RVA:  {:08X}\n", Directory[DirCount].RVA);
    CLog::Log(LOGDEBUG, "                    Size: {:08X}\n\n", Directory[DirCount].Size);
  }
#endif

  return 1;

}

int CoffLoader::LoadSymTable(FILE *fp)
{
  int Offset = ftell(fp);

  if ( CoffFileHeader->PointerToSymbolTable == 0 )
    return 1;

  fseek(fp, CoffFileHeader->PointerToSymbolTable /* + CoffBeginOffset*/, SEEK_SET);
  SymbolTable_t *tmp = new SymbolTable_t[CoffFileHeader->NumberOfSymbols];
  if (!tmp)
  {
    CLog::Log(LOGDEBUG, "Could not allocate memory for symbol table!\n");
    return 0;
  }
  fread((void *)tmp, CoffFileHeader->NumberOfSymbols, sizeof(SymbolTable_t), fp);
  NumberOfSymbols = CoffFileHeader->NumberOfSymbols;
  SymTable = tmp;
  fseek(fp, Offset, SEEK_SET);
  return 1;
}

int CoffLoader::LoadStringTable(FILE *fp)
{
  int StringTableSize;
  char *tmp = NULL;
  int Offset = ftell(fp);

  if ( CoffFileHeader->PointerToSymbolTable == 0 )
    return 1;

  fseek(fp, CoffFileHeader->PointerToSymbolTable +
        CoffFileHeader->NumberOfSymbols * sizeof(SymbolTable_t),
        SEEK_SET);

  fread(&StringTableSize, 1, sizeof(int), fp);
  StringTableSize -= 4;
  if (StringTableSize != 0)
  {
    tmp = new char[StringTableSize];
    if (tmp == NULL)
    {
      CLog::Log(LOGDEBUG, "Could not allocate memory for string table\n");
      return 0;
    }
    fread((void *)tmp, StringTableSize, sizeof(char), fp);
  }
  SizeOfStringTable = StringTableSize;
  StringTable = tmp;
  fseek(fp, Offset, SEEK_SET);
  return 1;
}

int CoffLoader::LoadSections(FILE *fp)
{
  NumOfSections = CoffFileHeader->NumberOfSections;

  SectionData = new char * [NumOfSections];
  if ( !SectionData )
    return 0;

  // Bobbin007: for debug dlls this check always fails

  //////check VMA size!!!!!
  //unsigned long vma_size = 0;
  //for (int SctnCnt = 0; SctnCnt < NumOfSections; SctnCnt++)
  //{
  //  SectionHeader_t *ScnHdr = (SectionHeader_t *)(SectionHeader + SctnCnt);
  //  vma_size = max(vma_size, ScnHdr->VirtualAddress + ScnHdr->SizeOfRawData);
  //  vma_size = max(vma_size, ScnHdr->VirtualAddress + ScnHdr->VirtualSize);
  //}

  //if (WindowsHeader->SizeOfImage < vma_size)
  //  return 0;   //something wrong with file

  for (int SctnCnt = 0; SctnCnt < NumOfSections; SctnCnt++)
  {
    SectionHeader_t *ScnHdr = (SectionHeader_t *)(SectionHeader + SctnCnt);
    SectionData[SctnCnt] = ((char*)hModule + ScnHdr->VirtualAddress);

    fseek(fp, ScnHdr->PtrToRawData, SEEK_SET);
    fread(SectionData[SctnCnt], 1, ScnHdr->SizeOfRawData, fp);

#ifdef DUMPING_DATA
    //debug blocks
    char szBuf[128];
    char namebuf[9];
    for (int i = 0; i < 8; i++)
      namebuf[i] = ScnHdr->Name[i];
    namebuf[8] = '\0';
    sprintf(szBuf, "Load code Sections %s Memory %p,Length %x\n", namebuf,
            SectionData[SctnCnt], max(ScnHdr->VirtualSize, ScnHdr->SizeOfRawData));
    OutputDebugString(szBuf);
#endif

    if ( ScnHdr->SizeOfRawData < ScnHdr->VirtualSize )  //initialize BSS data in the end of section
    {
      memset((char*)((long)(SectionData[SctnCnt]) + ScnHdr->SizeOfRawData), 0, ScnHdr->VirtualSize - ScnHdr->SizeOfRawData);
    }

    if (ScnHdr->Characteristics & IMAGE_SCN_CNT_BSS)  //initialize whole .BSS section, pure .BSS is obsolete
    {
      memset(SectionData[SctnCnt], 0, ScnHdr->VirtualSize);
    }

#ifdef DUMPING_DATA
    PrintSection(SectionHeader + SctnCnt, SectionData[SctnCnt]);
#endif

  }
  return 1;
}

//FIXME:  Add the Free Resources functions

int CoffLoader::RVA2Section(unsigned long RVA)
{
  NumOfSections = CoffFileHeader->NumberOfSections;
  for ( int i = 0; i < NumOfSections; i++)
  {
    if ( SectionHeader[i].VirtualAddress <= RVA )
    {
      if ( i + 1 != NumOfSections )
      {
        if ( RVA < SectionHeader[i + 1].VirtualAddress )
        {
          if ( SectionHeader[i].VirtualAddress + SectionHeader[i].VirtualSize <= RVA )
            CLog::Log(LOGDEBUG, "Warning! Address outside of Section: {:x}!\n", RVA);
          //                    else
          return i;
        }
      }
      else
      {
        if ( SectionHeader[i].VirtualAddress + SectionHeader[i].VirtualSize <= RVA )
          CLog::Log(LOGDEBUG, "Warning! Address outside of Section: {:x}!\n", RVA);
        //                else
        return i;
      }
    }
  }
  CLog::Log(LOGDEBUG, "RVA2Section lookup failure!\n");
  return 0;
}

void* CoffLoader::RVA2Data(unsigned long RVA)
{
  int Sctn = RVA2Section(RVA);

  if( RVA < SectionHeader[Sctn].VirtualAddress
   || RVA >= SectionHeader[Sctn].VirtualAddress + SectionHeader[Sctn].VirtualSize)
  {
    // RVA2Section is lying, let's use base address of dll instead, only works if
    // DLL has been loaded fully into memory, wich we normally do
    return (void*)(RVA + (unsigned long)hModule);
  }
  return SectionData[Sctn] + RVA - SectionHeader[Sctn].VirtualAddress;
}

unsigned long CoffLoader::Data2RVA(void* address)
{
  for ( int i = 0; i < CoffFileHeader->NumberOfSections; i++)
  {
    if(address >= SectionData[i] && address < SectionData[i] + SectionHeader[i].VirtualSize)
      return (unsigned long)address - (unsigned long)SectionData[i] + SectionHeader[i].VirtualAddress;
  }

  // Section wasn't found, so use relative to main load of dll
  return (unsigned long)address - (unsigned long)hModule;
}

char *CoffLoader::GetStringTblIndex(int index)
{
  char *table = StringTable;

  while (index--)
    table += strlen(table) + 1;
  return table;
}

char *CoffLoader::GetStringTblOff(int Offset)
{
  return StringTable + Offset - 4;
}

char *CoffLoader::GetSymbolName(SymbolTable_t *sym)
{
  static char shortname[9];
  __int64 index = sym->Name.Offset;
  int low = (int)(index & 0xFFFFFFFF);
  int high = (int)((index >> 32) & 0xFFFFFFFF);

  if (low == 0)
  {
    return GetStringTblOff(high);
  }
  else
  {
    memset(shortname, 0, 9);
    strncpy(shortname, (char *)sym->Name.ShortName, 8);
    return shortname;
  }
}

char *CoffLoader::GetSymbolName(int index)
{
  SymbolTable_t *sym = &(SymTable[index]);
  return GetSymbolName(sym);
}

void CoffLoader::PrintStringTable(void)
{
  int size = SizeOfStringTable;
  int index = 0;
  char *table = StringTable;

  CLog::Log(LOGDEBUG, "\nSTRING TABLE\n");
  while (size)
  {
    CLog::Log(LOGDEBUG, "{:2}: {}\n", index++, table);
    size -= strlen(table) + 1;
    table += strlen(table) + 1;
  }
  CLog::Log(LOGDEBUG, "\n");
}


void CoffLoader::PrintSymbolTable(void)
{
  int SymIndex;

  CLog::Log(LOGDEBUG, "COFF SYMBOL TABLE\n");
  for (SymIndex = 0; SymIndex < NumberOfSymbols; SymIndex++)
  {
    CLog::Log(LOGDEBUG, "{:03X} ", SymIndex);
    CLog::Log(LOGDEBUG, "{:08X} ", SymTable[SymIndex].Value);

    if (SymTable[SymIndex].SectionNumber == IMAGE_SYM_ABSOLUTE)
      CLog::Log(LOGDEBUG, "ABS     ");
    else if (SymTable[SymIndex].SectionNumber == IMAGE_SYM_DEBUG)
      CLog::Log(LOGDEBUG, "DEBUG   ");
    else if (SymTable[SymIndex].SectionNumber == IMAGE_SYM_UNDEFINED)
      CLog::Log(LOGDEBUG, "UNDEF   ");
    else
    {
      CLog::Log(LOGDEBUG, "SECT{} ", SymTable[SymIndex].SectionNumber);
      if (SymTable[SymIndex].SectionNumber < 10)
        CLog::Log(LOGDEBUG, " ");
      if (SymTable[SymIndex].SectionNumber < 100)
        CLog::Log(LOGDEBUG, " ");
    }

    if (SymTable[SymIndex].Type == 0)
      CLog::Log(LOGDEBUG, "notype       ");
    else
    {
      CLog::Log(LOGDEBUG, "{:X}         ", SymTable[SymIndex].Type);
      if (SymTable[SymIndex].Type < 0x10)
        CLog::Log(LOGDEBUG, " ");
      if (SymTable[SymIndex].Type < 0x100)
        CLog::Log(LOGDEBUG, " ");
      if (SymTable[SymIndex].Type < 0x1000)
        CLog::Log(LOGDEBUG, " ");
    }

    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_END_OF_FUNCTION)
      CLog::Log(LOGDEBUG, "End of Function   ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_NULL)
      CLog::Log(LOGDEBUG, "Null              ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_AUTOMATIC)
      CLog::Log(LOGDEBUG, "Automatic         ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
      CLog::Log(LOGDEBUG, "External          ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_STATIC)
      CLog::Log(LOGDEBUG, "Static            ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_REGISTER)
      CLog::Log(LOGDEBUG, "Register          ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_EXTERNAL_DEF)
      CLog::Log(LOGDEBUG, "External Def      ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_LABEL)
      CLog::Log(LOGDEBUG, "Label             ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_UNDEFINED_LABEL)
      CLog::Log(LOGDEBUG, "Undefined Label   ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_MEMBER_OF_STRUCT)
      CLog::Log(LOGDEBUG, "Member Of Struct  ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_ARGUMENT)
      CLog::Log(LOGDEBUG, "Argument          ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_STRUCT_TAG)
      CLog::Log(LOGDEBUG, "Struct Tag        ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_MEMBER_OF_UNION)
      CLog::Log(LOGDEBUG, "Member Of Union   ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_UNION_TAG)
      CLog::Log(LOGDEBUG, "Union Tag         ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_TYPE_DEFINITION)
      CLog::Log(LOGDEBUG, "Type Definition  ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_UNDEFINED_STATIC)
      CLog::Log(LOGDEBUG, "Undefined Static  ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_ENUM_TAG)
      CLog::Log(LOGDEBUG, "Enum Tag          ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_MEMBER_OF_ENUM)
      CLog::Log(LOGDEBUG, "Member Of Enum    ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_REGISTER_PARAM)
      CLog::Log(LOGDEBUG, "Register Param    ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_BIT_FIELD)
      CLog::Log(LOGDEBUG, "Bit Field         ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_BLOCK)
      CLog::Log(LOGDEBUG, "Block             ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_FUNCTION)
      CLog::Log(LOGDEBUG, "Function          ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_END_OF_STRUCT)
      CLog::Log(LOGDEBUG, "End Of Struct     ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_FILE)
      CLog::Log(LOGDEBUG, "File              ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_SECTION)
      CLog::Log(LOGDEBUG, "Section           ");
    if (SymTable[SymIndex].StorageClass == IMAGE_SYM_CLASS_WEAK_EXTERNAL)
      CLog::Log(LOGDEBUG, "Weak External     ");

    CLog::Log(LOGDEBUG, "| {}", GetSymbolName(SymIndex));

    SymIndex += SymTable[SymIndex].NumberOfAuxSymbols;
    CLog::Log(LOGDEBUG, "\n");
  }
  CLog::Log(LOGDEBUG, "\n");

}

void CoffLoader::PrintFileHeader(COFF_FileHeader_t *FileHeader)
{
  CLog::Log(LOGDEBUG, "COFF Header\n");
  CLog::Log(LOGDEBUG, "------------------------------------------\n\n");

  CLog::Log(LOGDEBUG, "MachineType:            0x{:04X}\n", FileHeader->MachineType);
  CLog::Log(LOGDEBUG, "NumberOfSections:       0x{:04X}\n", FileHeader->NumberOfSections);
  CLog::Log(LOGDEBUG, "TimeDateStamp:          0x{:08X}\n", FileHeader->TimeDateStamp);
  CLog::Log(LOGDEBUG, "PointerToSymbolTable:   0x{:08X}\n", FileHeader->PointerToSymbolTable);
  CLog::Log(LOGDEBUG, "NumberOfSymbols:        0x{:08X}\n", FileHeader->NumberOfSymbols);
  CLog::Log(LOGDEBUG, "SizeOfOptionHeader:     0x{:04X}\n", FileHeader->SizeOfOptionHeader);
  CLog::Log(LOGDEBUG, "Characteristics:        0x{:04X}\n", FileHeader->Characteristics);

  if (FileHeader->Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_RELOCS_STRIPPED\n");

  if (FileHeader->Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_EXECUTABLE_IMAGE\n");

  if (FileHeader->Characteristics & IMAGE_FILE_LINE_NUMS_STRIPPED)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_LINE_NUMS_STRIPPED\n");

  if (FileHeader->Characteristics & IMAGE_FILE_LOCAL_SYMS_STRIPPED)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_LOCAL_SYMS_STRIPPED\n");

  if (FileHeader->Characteristics & IMAGE_FILE_AGGRESSIVE_WS_TRIM)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_AGGRESSIVE_WS_TRIM\n");

  if (FileHeader->Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_LARGE_ADDRESS_AWARE\n");

  if (FileHeader->Characteristics & IMAGE_FILE_16BIT_MACHINE)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_16BIT_MACHINE\n");

  if (FileHeader->Characteristics & IMAGE_FILE_BYTES_REVERSED_LO)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_BYTES_REVERSED_LO\n");

  if (FileHeader->Characteristics & IMAGE_FILE_32BIT_MACHINE)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_32BIT_MACHINE\n");

  if (FileHeader->Characteristics & IMAGE_FILE_DEBUG_STRIPPED)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_DEBUG_STRIPPED\n");

  if (FileHeader->Characteristics & IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP\n");

  if (FileHeader->Characteristics & IMAGE_FILE_SYSTEM)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_SYSTEM\n");

  if (FileHeader->Characteristics & IMAGE_FILE_DLL)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_DLL\n");

  if (FileHeader->Characteristics & IMAGE_FILE_UP_SYSTEM_ONLY)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_UP_SYSTEM_ONLY\n");

  if (FileHeader->Characteristics & IMAGE_FILE_BYTES_REVERSED_HI)
    CLog::Log(LOGDEBUG, "                        IMAGE_FILE_BYTES_REVERSED_HI\n");

  CLog::Log(LOGDEBUG, "\n");
}

void CoffLoader::PrintOptionHeader(OptionHeader_t *OptHdr)
{
  CLog::Log(LOGDEBUG, "Option Header\n");
  CLog::Log(LOGDEBUG, "------------------------------------------\n\n");

  CLog::Log(LOGDEBUG, "Magic:              0x{:04X}\n", OptHdr->Magic);
  CLog::Log(LOGDEBUG, "Linker Major Ver:   0x{:02X}\n", VERSION_MAJOR(OptHdr->LinkVersion));
  CLog::Log(LOGDEBUG, "Linker Minor Ver:   0x{:02X}\n", VERSION_MINOR(OptHdr->LinkVersion));
  CLog::Log(LOGDEBUG, "Code Size:          0x{:08X}\n", OptHdr->CodeSize);
  CLog::Log(LOGDEBUG, "Data Size:          0x{:08X}\n", OptHdr->DataSize);
  CLog::Log(LOGDEBUG, "BSS Size:           0x{:08X}\n", OptHdr->BssSize);
  CLog::Log(LOGDEBUG, "Entry:              0x{:08X}\n", OptHdr->Entry);
  CLog::Log(LOGDEBUG, "Code Base:          0x{:08X}\n", OptHdr->CodeBase);
  CLog::Log(LOGDEBUG, "Data Base:          0x{:08X}\n", OptHdr->DataBase);
  CLog::Log(LOGDEBUG, "\n");
}

void CoffLoader::PrintWindowsHeader(WindowsHeader_t *WinHdr)
{
  CLog::Log(LOGDEBUG, "Windows Specific Option Header\n");
  CLog::Log(LOGDEBUG, "------------------------------------------\n\n");

  CLog::Log(LOGDEBUG, "Image Base:         0x{:08X}\n", WinHdr->ImageBase);
  CLog::Log(LOGDEBUG, "Section Alignment:  0x{:08X}\n", WinHdr->SectionAlignment);
  CLog::Log(LOGDEBUG, "File Alignment:     0x{:08X}\n", WinHdr->FileAlignment);
  CLog::Log(LOGDEBUG, "OS Version:         {}.{:08}\n", BIGVERSION_MAJOR(WinHdr->OSVer), BIGVERSION_MINOR(WinHdr->OSVer));
  CLog::Log(LOGDEBUG, "Image Version:      {}.{:08}\n", BIGVERSION_MAJOR(WinHdr->ImgVer), BIGVERSION_MINOR(WinHdr->ImgVer));
  CLog::Log(LOGDEBUG, "SubSystem Version:  {}.{:08}\n", BIGVERSION_MAJOR(WinHdr->SubSysVer), BIGVERSION_MINOR(WinHdr->SubSysVer));
  CLog::Log(LOGDEBUG, "Size of Image:      0x{:08X}\n", WinHdr->SizeOfImage);
  CLog::Log(LOGDEBUG, "Size of Headers:    0x{:08X}\n", WinHdr->SizeOfHeaders);
  CLog::Log(LOGDEBUG, "Checksum:           0x{:08X}\n", WinHdr->CheckSum);
  CLog::Log(LOGDEBUG, "Subsystem:          0x{:04X}\n", WinHdr->Subsystem);
  CLog::Log(LOGDEBUG, "DLL Flags:          0x{:04X}\n", WinHdr->DLLFlags);
  CLog::Log(LOGDEBUG, "Sizeof Stack Resv:  0x{:08X}\n", WinHdr->SizeOfStackReserve);
  CLog::Log(LOGDEBUG, "Sizeof Stack Comm:  0x{:08X}\n", WinHdr->SizeOfStackCommit);
  CLog::Log(LOGDEBUG, "Sizeof Heap Resv:   0x{:08X}\n", WinHdr->SizeOfHeapReserve);
  CLog::Log(LOGDEBUG, "Sizeof Heap Comm:   0x{:08X}\n", WinHdr->SizeOfHeapCommit);
  CLog::Log(LOGDEBUG, "Loader Flags:       0x{:08X}\n", WinHdr->LoaderFlags);
  CLog::Log(LOGDEBUG, "Num Directories:    {}\n", WinHdr->NumDirectories);
  CLog::Log(LOGDEBUG, "\n");
}

void CoffLoader::PrintSection(SectionHeader_t *ScnHdr, char* data)
{
  char SectionName[9];

  strncpy(SectionName, (char *)ScnHdr->Name, 8);
  SectionName[8] = 0;
  CLog::Log(LOGDEBUG, "Section: {}\n", SectionName);
  CLog::Log(LOGDEBUG, "------------------------------------------\n\n");

  CLog::Log(LOGDEBUG, "Virtual Size:       0x{:08X}\n", ScnHdr->VirtualSize);
  CLog::Log(LOGDEBUG, "Virtual Address:    0x{:08X}\n", ScnHdr->VirtualAddress);
  CLog::Log(LOGDEBUG, "Sizeof Raw Data:    0x{:08X}\n", ScnHdr->SizeOfRawData);
  CLog::Log(LOGDEBUG, "Ptr To Raw Data:    0x{:08X}\n", ScnHdr->PtrToRawData);
  CLog::Log(LOGDEBUG, "Ptr To Relocations: 0x{:08X}\n", ScnHdr->PtrToRelocations);
  CLog::Log(LOGDEBUG, "Ptr To Line Nums:   0x{:08X}\n", ScnHdr->PtrToLineNums);
  CLog::Log(LOGDEBUG, "Num Relocations:    0x{:04X}\n", ScnHdr->NumRelocations);
  CLog::Log(LOGDEBUG, "Num Line Numbers:   0x{:04X}\n", ScnHdr->NumLineNumbers);
  CLog::Log(LOGDEBUG, "Characteristics:    0x{:08X}\n", ScnHdr->Characteristics);
  if (ScnHdr->Characteristics & IMAGE_SCN_CNT_CODE)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_CNT_CODE\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_CNT_DATA)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_CNT_DATA\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_CNT_BSS)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_CNT_BSS\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_LNK_INFO)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_LNK_INFO\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_LNK_REMOVE)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_LNK_REMOVE\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_LNK_COMDAT)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_LNK_COMDAT\n");

  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_1BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_1BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_2BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_2BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_4BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_4BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_8BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_8BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_16BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_16BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_32BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_32BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_64BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_64BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_128BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_128BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_256BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_256BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_512BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_512BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_1024BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_1024BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_2048BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_2048BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_4096BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_4096BYTES\n");
  if ((ScnHdr->Characteristics & IMAGE_SCN_ALIGN_MASK) == IMAGE_SCN_ALIGN_8192BYTES)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_ALIGN_8192BYTES\n");

  if (ScnHdr->Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_LNK_NRELOC_OVFL\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_DISCARDABLE\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_NOT_CACHED\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_NOT_PAGED)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_NOT_PAGED\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_SHARED)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_SHARED\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_EXECUTE)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_EXECUTE\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_READ)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_READ\n");
  if (ScnHdr->Characteristics & IMAGE_SCN_MEM_WRITE)
    CLog::Log(LOGDEBUG, "                    IMAGE_SCN_MEM_WRITE\n");
  CLog::Log(LOGDEBUG, "\n");

  // Read the section Data, Relocations, & Line Nums
  //    Save the offset

  if (ScnHdr->SizeOfRawData > 0)
  {
    unsigned int i;
    char ch;
    // Print the Raw Data

    CLog::Log(LOGDEBUG, "\nRAW DATA");
    for (i = 0; i < ScnHdr->VirtualSize; i++)
    {
      if ((i % 16) == 0)
        CLog::Log(LOGDEBUG, "\n  {:08X}: ", i);
      ch = data[i];
      CLog::Log(LOGDEBUG, "{:02X} ", (unsigned int)ch);
    }
    CLog::Log(LOGDEBUG, "\n\n");
  }

  /*
  #if 0
  if (ScnHdr->NumRelocations > 0)
  {
  // Print Section Relocations
  ObjReloc_t ObjReloc;

  fseek(fp, ScnHdr->PtrToRelocations/ * + CoffBeginOffset* /, SEEK_SET);
  CLog::Log(LOGDEBUG, "RELOCATIONS\n");
  CLog::Log(LOGDEBUG, "                     Symbol    Symbol\n");
  CLog::Log(LOGDEBUG, " Offset    Type      Index     Name\n");
  CLog::Log(LOGDEBUG, " --------  --------  --------  ------\n");
  for (int i = 0; i < ScnHdr->NumRelocations; i++)
  {
  fread(&ObjReloc, 1, sizeof(ObjReloc_t), fp);
  CLog::Log(LOGDEBUG, " {:08X}  ", ObjReloc.VirtualAddress);

  if (ObjReloc.Type == IMAGE_REL_I386_ABSOLUTE)
  CLog::Log(LOGDEBUG, "ABSOLUTE  ");
  if (ObjReloc.Type == IMAGE_REL_I386_DIR16)
  CLog::Log(LOGDEBUG, "DIR16     ");
  if (ObjReloc.Type == IMAGE_REL_I386_REL16)
  CLog::Log(LOGDEBUG, "REL16     ");
  if (ObjReloc.Type == IMAGE_REL_I386_DIR32)
  CLog::Log(LOGDEBUG, "DIR32     ");
  if (ObjReloc.Type == IMAGE_REL_I386_DIR32NB)
  CLog::Log(LOGDEBUG, "DIR32NB   ");
  if (ObjReloc.Type == IMAGE_REL_I386_SEG12)
  CLog::Log(LOGDEBUG, "SEG12     ");
  if (ObjReloc.Type == IMAGE_REL_I386_SECTION)
  CLog::Log(LOGDEBUG, "SECTION   ");
  if (ObjReloc.Type == IMAGE_REL_I386_SECREL)
  CLog::Log(LOGDEBUG, "SECREL    ");
  if (ObjReloc.Type == IMAGE_REL_I386_REL32)
  CLog::Log(LOGDEBUG, "REL32     ");
  CLog::Log(LOGDEBUG, "{:8X}  ", ObjReloc.SymTableIndex);
  CLog::Log(LOGDEBUG, "{}\n", GetSymbolName(ObjReloc.SymTableIndex));
  }
  CLog::Log(LOGDEBUG, "\n");
  }

  if (ScnHdr->NumLineNumbers > 0)
  {
  // Print The Line Number Info
  LineNumbers_t LineNumber;
  int LineCnt = 0;
  int BaseLineNum = -1;

  fseek(fp, ScnHdr->PtrToLineNums/ * + CoffBeginOffset* /, SEEK_SET);
  CLog::Log(LOGDEBUG, "LINE NUMBERS");
  for (int i = 0; i < ScnHdr->NumLineNumbers; i++)
  {
  int LNOffset = ftell(fp);

  fread(&LineNumber, 1, sizeof(LineNumbers_t), fp);
  if (LineNumber.LineNum == 0)
  {
  SymbolTable_t *Sym;
  int SymIndex;

  CLog::Log(LOGDEBUG, "\n");
  SymIndex = LineNumber.Type.SymbolTableIndex;
  Sym = &(SymTable[SymIndex]);
  if (Sym->NumberOfAuxSymbols > 0)
  {
  Sym = &(SymTable[SymIndex+1]);
  AuxFuncDef_t *FuncDef = (AuxFuncDef_t *)Sym;

  if (FuncDef->PtrToLineNumber == LNOffset)
  {
  Sym = &(SymTable[FuncDef->TagIndex]);
  if (Sym->NumberOfAuxSymbols > 0)
  {
  Sym = &(SymTable[FuncDef->TagIndex+1]);
  AuxBfEf_t *Bf = (AuxBfEf_t *)Sym;
  BaseLineNum = Bf->LineNumber;
  }
  }
  }
  CLog::Log(LOGDEBUG, " Symbol Index: {:8x} ", SymIndex);
  CLog::Log(LOGDEBUG, " Base line number: {:8}\n", BaseLineNum);
  CLog::Log(LOGDEBUG, " Symbol name = {}", GetSymbolName(SymIndex));
  LineCnt = 0;
  }
  else
  {
  if ((LineCnt%4) == 0)
  {
  CLog::Log(LOGDEBUG, "\n ");
  LineCnt = 0;
  }
  CLog::Log(LOGDEBUG, "{:08X}({:5})  ", LineNumber.Type.VirtualAddress,
  LineNumber.LineNum + BaseLineNum);
  LineCnt ++;
  }
  }
  CLog::Log(LOGDEBUG, "\n");
  }
  #endif
  */

  CLog::Log(LOGDEBUG, "\n");
}

int CoffLoader::ParseCoff(FILE *fp)
{
  if ( !LoadCoffHModule(fp) )
  {
    CLog::Log(LOGDEBUG, "Failed to load/find COFF hModule header\n");
    return 0;
  }
  if ( !LoadSymTable(fp) ||
       !LoadStringTable(fp) ||
       !LoadSections(fp) )
    return 0;

  PerformFixups();

#ifdef DUMPING_DATA
  PrintSymbolTable();
  PrintStringTable();
#endif
  return 1;
}

void CoffLoader::PerformFixups(void)
{
  int FixupDataSize;
  char *FixupData;
  char *EndData;

  EntryAddress = (unsigned long)RVA2Data(EntryAddress);

  if( (PVOID)WindowsHeader->ImageBase == hModule )
    return;

  if ( !Directory )
    return ;

  if ( NumOfDirectories <= BASE_RELOCATION_TABLE )
    return ;

  if ( !Directory[BASE_RELOCATION_TABLE].Size )
    return ;

  FixupDataSize = Directory[BASE_RELOCATION_TABLE].Size;
  FixupData = (char*)RVA2Data(Directory[BASE_RELOCATION_TABLE].RVA);
  EndData = FixupData + FixupDataSize;

  while (FixupData < EndData)
  {
    // Starting a new Fixup Block
    unsigned long PageRVA = *((unsigned long*)FixupData);
    FixupData += 4;
    unsigned long BlockSize = *((unsigned long*)FixupData);
    FixupData += 4;

    BlockSize -= 8;
    for (unsigned int i = 0; i < BlockSize / 2; i++)
    {
      unsigned short Fixup = *((unsigned short*)FixupData);
      FixupData += 2;
      int Type = (Fixup >> 12) & 0x0f;
      Fixup &= 0xfff;
      if (Type == IMAGE_REL_BASED_HIGHLOW)
      {
        unsigned long *Off = (unsigned long*)RVA2Data(Fixup + PageRVA);
        *Off = (unsigned long)RVA2Data(*Off - WindowsHeader->ImageBase);
      }
      else if (Type == IMAGE_REL_BASED_ABSOLUTE)
      {}
      else
      {
        CLog::Log(LOGDEBUG, "Unsupported fixup type!!\n");
      }
    }
  }
}
