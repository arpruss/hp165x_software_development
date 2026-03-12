#include <hp165x.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "extmod/vfs.h"       // Core VFS logic
#include "extmod/vfs_posix.h" // POSIX-specific structures

// Allocate memory for the MicroPython GC heap.
static char heap[100000];
uint16_t _ticksPerSecond;
int __errno;


int main(int argc, char **argv) {
	initScreen(392,WRITE_BLACK);
	patchVBL();
	_ticksPerSecond = ticksPerSecond();
	initKeyboard(1);
    // Initialise the MicroPython runtime.
    mp_cstack_init_with_sp_here(16384);
    gc_init(heap, heap + sizeof(heap));
    mp_init();
	
	mp_obj_t vfs_obj = mp_obj_new_instance(&mp_type_vfs_posix, NULL);
	mp_obj_t args[2] = {
            MP_OBJ_TYPE_GET_SLOT(&mp_type_vfs_posix, make_new)(&mp_type_vfs_posix, 0, 0, NULL),
            MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
	};
	mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);	
	MP_STATE_VM(vfs_cur) = MP_VFS_ROOT;
    
	pyexec_friendly_repl();

    // Deinitialise the runtime.
    gc_sweep_all();
    mp_deinit();
	reload();
    return 0;
}

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

// Do a garbage collection cycle.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

// There is no filesystem so opening a file raises an exception.
//mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
 //   mp_raise_OSError(MP_ENOENT);
//}

int mkdir(const char *path, mode_t mode) {
	return -1;
}

int chdir(const char *path) {
	return 0;
}

char *getcwd(char *buf, size_t size) {
	strncpy(buf, "/", size);
	return buf;
}

int rmdir(const char *path) {
	return -1;
}

int fstat(int fildes, struct stat *buf) {
	return -1;
}

