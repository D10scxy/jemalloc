#include "jemalloc/internal/jemalloc_preamble.h"
#include "jemalloc/internal/jemalloc_internal_includes.h"

#include "jemalloc/internal/assert.h"
#include "jemalloc/internal/extent_mmap.h"

/******************************************************************************/
/* Data. */

bool	opt_retain =
#ifdef JEMALLOC_RETAIN
    true
#else
    false
#endif
    ;

static size_t extent_addr = 0x600000000000;

/******************************************************************************/

void *
extent_alloc_mmap(void *new_addr, size_t size, size_t alignment, bool *zero,
    bool *commit) {
	assert(alignment == ALIGNMENT_CEILING(alignment, PAGE));
    if(new_addr == NULL) {
        new_addr = ALIGNMENT_ADDR2BASE(extent_addr + alignment - 1, alignment);
    }
	void *ret = pages_map(new_addr, size, alignment, commit);
	if (ret == NULL) {
		return NULL;
	}
	assert(ret != NULL);
	if (*commit) {
		*zero = true;
	}
    extent_addr = new_addr + size;
	return ret;
}

void *
extent_alloc_mmap_default(void *new_addr, size_t size, size_t alignment, bool *zero,
    bool *commit) {
	assert(alignment == ALIGNMENT_CEILING(alignment, PAGE));
	void *ret = pages_map(new_addr, size, alignment, commit);
	if (ret == NULL) {
		return NULL;
	}
	assert(ret != NULL);
	if (*commit) {
		*zero = true;
	}
	return ret;
}

bool
extent_dalloc_mmap(void *addr, size_t size) {
	if (!opt_retain) {
		pages_unmap(addr, size);
	}
	return opt_retain;
}
