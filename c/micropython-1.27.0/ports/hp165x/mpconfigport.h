#include <stdint.h>
#include <errno.h>

#define TYPEOF_ERRNO int
#define MP_HAL_RETRY_SYSCALL(ret, syscall, raise) \
    do { \
        ret = syscall; \
        if (ret == -1) { \
            int err = errno; \
            raise; \
        } \
    } while (0)
		
// Python internal features.
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_HELPER_REPL                     (1)
#define MICROPY_ERROR_REPORTING                 (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_DOUBLE)

#define MICROPY_READER_POSIX        (1)
#define MICROPY_READER_VFS          (1)
#define MICROPY_VFS                 (1)
#define MICROPY_PY_VFS              (1)
#define MICROPY_PY_OS               (1)
#define MICROPY_PY_OS_STATVFS       (0)
#define MICROPY_PY_TIME             (1)
#define MICROPY_VFS_POSIX           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_HELPER_LEXER_UNIX   (1)

// Fine control over Python builtins, classes, modules, etc.
#define MICROPY_PY_ASYNC_AWAIT                  (0)
#define MICROPY_PY_BUILTINS_SET                 (0)
#define MICROPY_PY_ATTRTUPLE                    (0)
#define MICROPY_PY_COLLECTIONS                  (0)
#define MICROPY_PY_MATH                         (1)
//#define MICROPY_USE_INTERNAL_LIBM   (1)
#define MICROPY_PY_IO                           (1)
#define MICROPY_PY_STRUCT                       (1)

#define MP_ENDIANNESS_BIG (1)
#define MICROPY_ENDIANNESS_BIG (1)
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_A)
#define MICROPY_ROM_TEXT_COMPRESSION (0)
#define MICROPY_QSTR_BYTES_IN_LEN    (1)
#define MICROPY_QSTR_BYTES_IN_HASH   (1)
#define MICROPY_ROM_IS_STR_ALIGNED   (1)

#define MICROPY_OBJ_REPR (MICROPY_OBJ_REPR_A)
#define MICROPY_OBJ_BASE_ALIGNMENT __attribute__((aligned(4)))

#define MICROPY_ALLOC_PATH_MAX      (32)

typedef uint16_t mp_qstr_t;
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca().
#include <alloca.h>

// Define the port's name and hardware.
#define MICROPY_HW_BOARD_NAME "hp165x"
#define MICROPY_HW_MCU_NAME   "68000"

#define MP_STATE_PORT MP_STATE_VM

//extern const struct _mp_obj_module_t mp_module_os; // or uos, check your source

//#define MICROPY_PORT_BUILTIN_MODULES { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&mp_module_os }, 