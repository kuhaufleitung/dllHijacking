#!/usr/bin/env python3
 
import pefile
import sys
import os.path
from pathlib import Path
 
target_dll = 'C:\\\\Windows\\\\System32\\\\XInput1_4.dll'
target_dll_no_extension = target_dll.split('.')[0]
 
dll = pefile.PE(target_dll)
dll_basename = Path(target_dll).stem
 
print("#pragma once")
 
for export in dll.DIRECTORY_ENTRY_EXPORT.symbols:
    if export.name:
        print('#pragma comment(linker,"/export:{}='.format(export.name.decode()) + target_dll_no_extension  +  '.{},@{}")'.format(export.name.decode(), export.ordinal))
 