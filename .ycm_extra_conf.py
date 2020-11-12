# This file is NOT licensed under the GPLv3, which is the license for the rest
# of YouCompleteMe.
#
# Here's the license text for this file:
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>


# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
'-Wall',
'-Wextra',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-fexceptions',
'-DNDEBUG',
# THIS IS IMPORTANT! Without the '-x' flag, Clang won't know which language to
# use when compiling headers. So it will guess. Badly. So C++ headers will be
# compiled as C headers. You don't want that so ALWAYS specify the '-x' flag.
# For a C project, you would set this to 'c' instead of 'c++'.
'-x',
'c++',
'-std=c++2a',
# '-fconcepts',
'-I', 'include',
'-I', 'src',

# conan dependencies from compile_comands.json
'-I/home/obrusvit/.conan/data/imgui-sfml/2.1/bincrafters/stable/package/63c4610e7441aaef8f3f40a9566ae2ecd6553581/include',
'-I/home/obrusvit/.conan/data/catch2/2.13.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include',
'-I/home/obrusvit/.conan/data/doctest/2.4.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include',
'-I/home/obrusvit/.conan/data/docopt.cpp/0.6.2/_/_/package/f54287fc3dbd09590f968b580bebb565ea4a4b17/include',
'-I/home/obrusvit/.conan/data/spdlog/1.7.0/_/_/package/2ac168a5a469cd6a81f11e00cf2589ff4bcb2ed0/include',
'-I/home/obrusvit/.conan/data/gsl-lite/0.37.0/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include',
'-I/home/obrusvit/.conan/data/sfml/2.5.1/bincrafters/stable/package/1d45e316e71aa76e122414be8f4c0d6688deabd4/include',
'-I/home/obrusvit/.conan/data/fmt/7.0.3/_/_/package/f54287fc3dbd09590f968b580bebb565ea4a4b17/include',
'-I/home/obrusvit/.conan/data/freetype/2.10.1/_/_/package/1381eb533150a52ee50820711faa223d436deba1/include',
'-I/home/obrusvit/.conan/data/freetype/2.10.1/_/_/package/1381eb533150a52ee50820711faa223d436deba1/include/freetype2',
'-I/home/obrusvit/.conan/data/stb/20200203/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include',
'-I/home/obrusvit/.conan/data/openal/1.19.1/_/_/package/395a697543c6d91c9cea7263a9a7a1fd3e6ce3fd/include',
'-I/home/obrusvit/.conan/data/openal/1.19.1/_/_/package/395a697543c6d91c9cea7263a9a7a1fd3e6ce3fd/include/AL',
'-I/home/obrusvit/.conan/data/flac/1.3.3/_/_/package/18e127a3cdb0f6639b6def6067ba912bdf1053a3/include',
'-I/home/obrusvit/.conan/data/vorbis/1.3.6/_/_/package/b64f670df847327ead52ac5bfa3722faedc2edc3/include',
'-I/usr/include/uuid',
'-I/usr/include/freetype2',
'-I/usr/include/libpng16',
'-I/home/obrusvit/.conan/data/libpng/1.6.37/_/_/package/1e8b7ff23bd5e7932ba7e8874349125fdf8e91ec/include',
'-I/home/obrusvit/.conan/data/bzip2/1.0.8/_/_/package/ff2ff6bb41ed84283a6634bada252c37f13eb93c/include',
'-I/home/obrusvit/.conan/data/libalsa/1.1.9/_/_/package/847849d4cc4b6352c8023780ae6930933f613ad9/include',
'-I/home/obrusvit/.conan/data/ogg/1.3.4/_/_/package/6b7ff26bfd4c2cf2ccba522bfba2d2e7820e40da/include',
'-I/home/obrusvit/.conan/data/zlib/1.2.11/_/_/package/6b7ff26bfd4c2cf2ccba522bfba2d2e7820e40da/include',

# conan sqlite stuff
'-isystem', '/home/obrusvit/.conan/data/sqlpp11-connector-sqlite3/0.30/_/_/package/bebbff8def6a9d9855163389e26007e877630caa/include',
'-isystem', '/home/obrusvit/.conan/data/sqlpp11/0.60/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include',
'-isystem', '/home/obrusvit/.conan/data/date/2.4.1/_/_/package/1e572d87bcbf8a65d93a47a06f1e2ad2c8edf647/include',
'-isystem', '/home/obrusvit/.conan/data/libcurl/7.69.1/_/_/package/a3c130aecdb433de1a0f956114f023a0ad0a546f/include',
'-isystem', '/home/obrusvit/.conan/data/openssl/1.1.1h/_/_/package/6b7ff26bfd4c2cf2ccba522bfba2d2e7820e40da/include',
'-isystem', '/home/obrusvit/.conan/data/sqlite3/3.32.3/_/_/package/b8c710d7fc0948f5843b11a21d9c8168742df6ae/include',

# copied from global ycm_extra_conf but with clang/10/include instead of gcc/
'-isystem', '/usr/include/c++/9',
'-isystem', '/usr/include/x86_64-linux-gnu/c++/9',
'-isystem', '/usr/include/c++/9/backward',
'-isystem', '/usr/lib/clang/10/include',
'-isystem', '/usr/local/include',
'-isystem', '/usr/include/x86_64-linux-gnu',
'-isystem', '/usr/include',
]

def Settings( **kwargs ):
  return {
    'flags': flags,
  }
