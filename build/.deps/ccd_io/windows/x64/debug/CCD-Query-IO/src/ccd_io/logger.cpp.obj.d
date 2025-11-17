{
    files = {
        [[CCD-Query-IO\src\ccd_io\logger.cpp]]
    },
    depfiles_format = "cl_json",
    depfiles = "{\
    \"Version\": \"1.2\",\
    \"Data\": {\
        \"Source\": \"e:\\\\research\\\\simulation\\\\datagenr\\\\sync\\\\ccd-query-io\\\\src\\\\ccd_io\\\\logger.cpp\",\
        \"ProvidedModule\": \"\",\
        \"Includes\": [\
            \"e:\\\\research\\\\simulation\\\\datagenr\\\\sync\\\\ccd-query-io\\\\src\\\\ccd_io\\\\logger.hpp\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\spdlog.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\common.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\null_mutex.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\atomic\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\yvals.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\yvals_core.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\sal.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\concurrencysal.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vadefs.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xkeycheck.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\crtdbg.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_new_debug.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_new.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\crtdefs.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\use_ansi.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstddef\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\stddef.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xtr1common\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstdint\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\stdint.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstring\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\string.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_memory.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_memcpy_s.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\errno.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_string.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wstring.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xatomic.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\type_traits\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\intrin0.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\intrin0.inl.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xatomic_wait.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstdlib\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\math.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_math.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\stdlib.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_malloc.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_search.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wstdlib.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\limits.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xthreads.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_threads_core.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\climits\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xtimec.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\ctime\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\time.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wtime.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\utility\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\initializer_list\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\compare\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\concepts\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\tweakme.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\chrono\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_chrono.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\limits\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cfloat\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\float.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cwchar\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstdio\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\stdio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wstdio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_stdio_config.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\wchar.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wconio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wctype.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wdirect.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_share.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_wprocess.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\sys\\\\stat.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\sys\\\\types.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\intrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\setjmp.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\immintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\wmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\nmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\smmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\tmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\pmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\emmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\mmintrin.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\malloc.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\zmmintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\ammintrin.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\ratio\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\system_error\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_system_error_abi.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cerrno\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\stdexcept\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\exception\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_exception.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\eh.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_terminate.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xstring\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_sanitizer_annotate_container.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_string_view.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\iosfwd\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xutility\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_iter_core.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xmemory\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\new\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\tuple\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xpolymorphic_allocator.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xcall_once.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xerrc.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xfilesystem_abi.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_tzdb.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cmath\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\format\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_format_ucd_tables.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_formatter.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_print.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_ranges_tuple_formatter.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\bit\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_bit_utils.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\iterator\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocale\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\memory\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\typeinfo\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_typeinfo.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xfacet\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocinfo\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_xlocinfo_types.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cctype\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\ctype.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\clocale\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\locale.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\charconv\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xbit_ops.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xcharconv.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xcharconv_ryu.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xcharconv_ryu_tables.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xcharconv_tables.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\locale\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocbuf\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\streambuf\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xiosbase\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\share.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocmes\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocmon\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xlocnum\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xloctime\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\forward_list\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\iomanip\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\istream\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_ostream.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\ios\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\optional\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xsmf_control.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\sstream\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\string\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vector\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\functional\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\unordered_map\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xhash\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\list\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\xnode_handle.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\fmt.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\bundled\\\\core.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\string_view\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\bundled\\\\format.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\bundled\\\\core.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\bundled\\\\format-inl.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\algorithm\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_heap_algorithms.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\__msvc_minmax.hpp\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cstdarg\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\stdarg.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\io.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_io.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\fmt\\\\bundled\\\\format.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\common-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\registry.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\periodic_worker.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\condition_variable\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\mutex\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\thread\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\process.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\corecrt_startup.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\math.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\vcruntime_startup.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\stop_token\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\periodic_worker-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\registry-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\logger.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\backtracer.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\circular_q.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\cassert\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\assert.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\log_msg_buffer.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\log_msg.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\log_msg-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\os.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\os-inl.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\array\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\windows_include.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\windows.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\winapifamily.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\winpackagefamily.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\sdkddkver.h\",\
            \"d:\\\\program files (x86)\\\\visual studio\\\\vc\\\\tools\\\\msvc\\\\14.44.35207\\\\include\\\\excpt.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\windef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\minwindef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\specstrings.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\specstrings_strict.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\specstrings_undef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\driverspecs.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\sdv_driverspecs.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winnt.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\kernelspecs.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\basetsd.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\guiddef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack4.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack4.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack4.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack8.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack1.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack1.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\apiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\ktmtypes.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winbase.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\apisetcconv.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\minwinbase.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\apiquery2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\processenv.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\fileapifromapp.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\fileapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\debugapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\utilapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\handleapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\errhandlingapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\fibersapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\namedpipeapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\profileapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\heapapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\ioapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\synchapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\interlockedapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\processthreadsapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\sysinfoapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\memoryapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\enclaveapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\threadpoollegacyapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\threadpoolapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\jobapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\jobapi2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\wow64apiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\libloaderapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\securitybaseapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\namespaceapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\systemtopologyapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\processtopologyapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\securityappcontainer.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\realtimeapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\winerror.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\timezoneapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\wingdi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack1.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack4.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack4.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winuser.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\pshpack2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\poppack.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\tvout.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winnls.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\datetimeapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\stringapiset.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winnls.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\wincon.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\wincontypes.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\consoleapi.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\consoleapi2.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\consoleapi3.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winver.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\verrsrc.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winreg.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\reason.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winnetwk.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\wnnc.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\shared\\\\stralign.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\winsvc.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\mcx.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\imm.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\um\\\\ime_cmodes.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.26100.0\\\\ucrt\\\\direct.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\log_msg_buffer-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\backtracer-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\logger-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\pattern_formatter.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\formatter.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\pattern_formatter-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\fmt_helper.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\sink.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\sink-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\wincolor_sink.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\console_globals.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\wincolor_sink-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\details\\\\synchronous_factory.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\version.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\spdlog-inl.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\stdout_color_sinks.h\",\
            \"c:\\\\users\\\\lenovo\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\s\\\\spdlog\\\\v1.13.0\\\\f7db3f4ee6c94208a389ab07cefee620\\\\include\\\\spdlog\\\\sinks\\\\stdout_color_sinks-inl.h\"\
        ],\
        \"ImportedModules\": [],\
        \"ImportedHeaderUnits\": []\
    }\
}",
    values = {
        [[D:\Program Files (x86)\visual studio\VC\Tools\MSVC\14.44.35207\bin\HostX64\x64\cl.exe]],
        {
            "-nologo",
            "-Zi",
            "-FS",
            [[-Fdbuild\windows\x64\debug\ccd_io.pdb]],
            "-Ox",
            "-std:c++latest",
            [[-ICCD-Query-IO\src]],
            [[-Irational-cpp\src]],
            [[-Igmp\include]],
            "/EHsc",
            "-external:W0",
            [[-external:IC:\Users\Lenovo\AppData\Local\.xmake\packages\s\spdlog\v1.13.0\f7db3f4ee6c94208a389ab07cefee620\include]],
            "-external:W0",
            [[-external:IC:\Users\Lenovo\AppData\Local\.xmake\packages\n\nlohmann_json\v3.11.3\826582eb66794908a16a49dbdd33bec4\include]],
            "/utf-8"
        }
    }
}