########################################################################
# Usage: AX_LIBCLANG([min-abi], [min-api])                             #
########################################################################

AC_DEFUN([AX_LIBCLANG],
[

#
# First try and use llvm-config if available, to add needed include and library paths
#

	AC_PATH_TOOL([LLVM_CONFIG], [llvm-config], [no])
	AS_IF([test "x$LLVM_CONFIG" = "xno"],
	[
		AC_MSG_WARN([unable to find `llvm-config'. if libclang is not installed in default system paths, you will need to set the `CPPFLAGS' and `LDFLAGS' environment variables manually. no version checks can be performed, compilation or linking may fail.])
	], [
		libclang_cv_INCLUDEDIR=$(${LLVM_CONFIG} --includedir)
		libclang_cv_LIBDIR=$(${LLVM_CONFIG} --libdir)
		CPPFLAGS="$CPPFLAGS -I$libclang_cv_INCLUDEDIR"
		LDFLAGS="-L$libclang_cv_LIBDIR"
	])

#
# Then make sure the header and library is actually available
#

	AC_CHECK_HEADER([clang-c/Index.h], [], [
		AC_MSG_ERROR([cannot locate libclang's `clang-c/Index.h' header file, please install the `llvm-config' utility or manually adjust the `CPPFLAGS' environment variable to add the include path where the header is available.])
	])
	AC_CHECK_LIB([clang], [clang_getClangVersion], [], [
		AC_MSG_ERROR([cannot locate libclang library, please install the `llvm-config' utility or manually adjust the `LDFLAGS' environment variable to add the library path where the library is available.])
	])

#
# Finally if try and compile with it to see if the versions are new enough
#

# First the ABI version

	AS_IF([test "x$1" != "x"], [
		libclang_min_abi=$1
		AC_MSG_CHECKING([whether libclang is at least ABI version $libclang_min_abi])
		libclang_backup_CPPFLAGS=$CPPFLAGS
		CPPFLAGS="$CPPFLAGS -DMIN_LIBCLANG_ABI=$libclang_min_abi"
		AC_COMPILE_IFELSE([
			AC_LANG_SOURCE([[
#include <clang-c/Index.h>
#if CINDEX_VERSION_MAJOR < MIN_LIBCLANG_ABI
#error "minimum ABI version not met"
#endif
			]])
		], [
			AC_MSG_RESULT([yes])
			CPPFLAGS=$libclang_backup_CPPFLAGS
		], [
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([the libclang header found does not provide the minimum ABI version $libclang_min_abi])
		])
	])

# Then the API version

	AS_IF([test "x$2" != "x"], [
		libclang_min_api=$2
		AC_MSG_CHECKING([whether libclang is at least API version $libclang_min_api])
		libclang_backup_CPPFLAGS=$CPPFLAGS
		CPPFLAGS="$CPPFLAGS -DMIN_LIBCLANG_API=$libclang_min_api"
		AC_COMPILE_IFELSE([
			AC_LANG_SOURCE([[
#include <clang-c/Index.h>
#if CINDEX_VERSION_MINOR < MIN_LIBCLANG_API
#error "minimum API version not met"
#endif
			]])
		], [
			AC_MSG_RESULT([yes])
			CPPFLAGS=$libclang_backup_CPPFLAGS
		], [
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([the libclang header found does not provide the minimum API version $libclang_min_api])
		])
	])
])i
