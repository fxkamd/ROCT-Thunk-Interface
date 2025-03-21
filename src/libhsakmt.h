/*
 * Copyright © 2014 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including
 * the next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef LIBHSAKMT_H_INCLUDED
#define LIBHSAKMT_H_INCLUDED

#include "linux/kfd_ioctl.h"
#include "hsakmt.h"
#include <pthread.h>
#include <stdint.h>
#include <limits.h>

extern int kfd_fd;
extern unsigned long kfd_open_count;
extern bool hsakmt_forked;
extern pthread_mutex_t hsakmt_mutex;
extern bool is_dgpu;

extern HsaVersionInfo kfd_version_info;

#undef HSAKMTAPI
#define HSAKMTAPI __attribute__((visibility ("default")))

/*Avoid pointer-to-int-cast warning*/
#define PORT_VPTR_TO_UINT64(vptr) ((uint64_t)(unsigned long)(vptr))

/*Avoid int-to-pointer-cast warning*/
#define PORT_UINT64_TO_VPTR(v) ((void*)(unsigned long)(v))

#define CHECK_KFD_OPEN() \
	do { if (kfd_open_count == 0 || hsakmt_forked) return HSAKMT_STATUS_KERNEL_IO_CHANNEL_NOT_OPENED; } while (0)

#define CHECK_KFD_MINOR_VERSION(minor)					\
	do { if ((minor) > kfd_version_info.KernelInterfaceMinorVersion)\
		return HSAKMT_STATUS_NOT_SUPPORTED; } while (0)

extern int PAGE_SIZE;
extern int PAGE_SHIFT;

/* VI HW bug requires this virtual address alignment */
#define TONGA_PAGE_SIZE 0x8000

/* 64KB BigK fragment size for TLB efficiency */
#define GPU_BIGK_PAGE_SIZE (1 << 16)

/* 2MB huge page size for 4-level page tables on Vega10 and later GPUs */
#define GPU_HUGE_PAGE_SIZE (2 << 20)
#define GPU_GIANT_PAGE_SIZE (1 << 30)

#define CHECK_PAGE_MULTIPLE(x) \
	do { if ((uint64_t)PORT_VPTR_TO_UINT64(x) % PAGE_SIZE) return HSAKMT_STATUS_INVALID_PARAMETER; } while(0)

#define ALIGN_UP(x,align) (((uint64_t)(x) + (align) - 1) & ~(uint64_t)((align)-1))
#define ALIGN_UP_32(x,align) (((uint32_t)(x) + (align) - 1) & ~(uint32_t)((align)-1))
#define PAGE_ALIGN_UP(x) ALIGN_UP(x,PAGE_SIZE)
#define BITMASK(n) ((n) ? (UINT64_MAX >> (sizeof(UINT64_MAX) * CHAR_BIT - (n))) : 0)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

/* HSA Thunk logging usage */
extern int hsakmt_debug_level;
#define hsakmt_print(level, fmt, ...) \
	do { if (level <= hsakmt_debug_level) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#define HSAKMT_DEBUG_LEVEL_DEFAULT	-1
#define HSAKMT_DEBUG_LEVEL_ERR		3
#define HSAKMT_DEBUG_LEVEL_WARNING	4
#define HSAKMT_DEBUG_LEVEL_INFO		6
#define HSAKMT_DEBUG_LEVEL_DEBUG	7
#define pr_err(fmt, ...) \
	hsakmt_print(HSAKMT_DEBUG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) \
	hsakmt_print(HSAKMT_DEBUG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	hsakmt_print(HSAKMT_DEBUG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) \
	hsakmt_print(HSAKMT_DEBUG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define pr_err_once(fmt, ...)                   \
({                                              \
        static bool __print_once;               \
        if (!__print_once) {                    \
                __print_once = true;            \
                pr_err(fmt, ##__VA_ARGS__);     \
        }                                       \
})
#define pr_warn_once(fmt, ...)                  \
({                                              \
        static bool __print_once;               \
        if (!__print_once) {                    \
                __print_once = true;            \
                pr_warn(fmt, ##__VA_ARGS__);    \
        }                                       \
})

/* Expects gfxv (full) in decimal */
#define HSA_GET_GFX_VERSION_MAJOR(gfxv)   (((gfxv) / 10000) % 100)
#define HSA_GET_GFX_VERSION_MINOR(gfxv)   (((gfxv) / 100) % 100)
#define HSA_GET_GFX_VERSION_STEP(gfxv)    ((gfxv) % 100)

/* Expects HSA_ENGINE_ID.ui32, returns gfxv (full) in hex */
#define HSA_GET_GFX_VERSION_FULL(ui32) \
	(((ui32.Major) << 16) | ((ui32.Minor) << 8) | (ui32.Stepping))

enum full_gfx_versions {
	GFX_VERSION_KAVERI		= 0x070000,
	GFX_VERSION_HAWAII		= 0x070001,
	GFX_VERSION_CARRIZO		= 0x080001,
	GFX_VERSION_TONGA		= 0x080002,
	GFX_VERSION_FIJI		= 0x080003,
	GFX_VERSION_POLARIS10		= 0x080003,
	GFX_VERSION_POLARIS11		= 0x080003,
	GFX_VERSION_POLARIS12		= 0x080003,
	GFX_VERSION_VEGAM		= 0x080003,
	GFX_VERSION_VEGA10		= 0x090000,
	GFX_VERSION_RAVEN		= 0x090002,
	GFX_VERSION_VEGA12		= 0x090004,
	GFX_VERSION_VEGA20		= 0x090006,
	GFX_VERSION_ARCTURUS		= 0x090008,
	GFX_VERSION_ALDEBARAN		= 0x09000A,
	GFX_VERSION_RENOIR		= 0x09000C,
	GFX_VERSION_NAVI10		= 0x0A0100,
	GFX_VERSION_NAVI12		= 0x0A0101,
	GFX_VERSION_NAVI14		= 0x0A0102,
	GFX_VERSION_CYAN_SKILLFISH	= 0x0A0103,
	GFX_VERSION_SIENNA_CICHLID	= 0x0A0300,
	GFX_VERSION_NAVY_FLOUNDER	= 0x0A0301,
	GFX_VERSION_DIMGREY_CAVEFISH	= 0x0A0302,
	GFX_VERSION_VANGOGH	 	= 0x0A0303,
	GFX_VERSION_BEIGE_GOBY	 	= 0x0A0304,
	GFX_VERSION_YELLOW_CARP	 	= 0x0A0305,
};

struct hsa_gfxip_table {
	uint16_t device_id;		// Device ID
	unsigned char major;		// GFXIP Major engine version
	unsigned char minor;		// GFXIP Minor engine version
	unsigned char stepping;		// GFXIP Stepping info
	const char *amd_name;		// CALName of the device
};

HSAKMT_STATUS init_kfd_version(void);

#define IS_SOC15(gfxv) ((gfxv) >= GFX_VERSION_VEGA10)

HSAKMT_STATUS validate_nodeid(uint32_t nodeid, uint32_t *gpu_id);
HSAKMT_STATUS gpuid_to_nodeid(uint32_t gpu_id, uint32_t* node_id);
uint32_t get_gfxv_by_node_id(HSAuint32 node_id);
bool prefer_ats(HSAuint32 node_id);
uint16_t get_device_id_by_node_id(HSAuint32 node_id);
uint16_t get_device_id_by_gpu_id(HSAuint32 gpu_id);
uint32_t get_direct_link_cpu(uint32_t gpu_node);
int get_drm_render_fd_by_gpu_id(HSAuint32 gpu_id);
HSAKMT_STATUS validate_nodeid_array(uint32_t **gpu_id_array,
		uint32_t NumberOfNodes, uint32_t *NodeArray);

HSAKMT_STATUS topology_sysfs_get_node_props(uint32_t node_id, HsaNodeProperties *props,
					uint32_t *gpu_id,
					bool *p2p_links, uint32_t *num_p2pLinks);
HSAKMT_STATUS topology_sysfs_get_system_props(HsaSystemProperties *props);
void topology_setup_is_dgpu_param(HsaNodeProperties *props);
bool topology_is_svm_needed(HSA_ENGINE_ID EngineId);

HSAuint32 PageSizeFromFlags(unsigned int pageSizeFlags);

void* allocate_exec_aligned_memory_gpu(uint32_t size, uint32_t align,
				       uint32_t NodeId, bool NonPaged,
				       bool DeviceLocal, bool Uncached);
void free_exec_aligned_memory_gpu(void *addr, uint32_t size, uint32_t align);
HSAKMT_STATUS init_process_doorbells(unsigned int NumNodes);
void destroy_process_doorbells(void);
HSAKMT_STATUS init_device_debugging_memory(unsigned int NumNodes);
void destroy_device_debugging_memory(void);
HSAKMT_STATUS init_counter_props(unsigned int NumNodes);
void destroy_counter_props(void);
uint32_t *convert_queue_ids(HSAuint32 NumQueues, HSA_QUEUEID *Queues);

extern int kmtIoctl(int fd, unsigned long request, void *arg);

/* Void pointer arithmetic (or remove -Wpointer-arith to allow void pointers arithmetic) */
#define VOID_PTR_ADD32(ptr,n) (void*)((uint32_t*)(ptr) + n)/*ptr + offset*/
#define VOID_PTR_ADD(ptr,n) (void*)((uint8_t*)(ptr) + n)/*ptr + offset*/
#define VOID_PTR_SUB(ptr,n) (void*)((uint8_t*)(ptr) - n)/*ptr - offset*/
#define VOID_PTRS_SUB(ptr1,ptr2) (uint64_t)((uint8_t*)(ptr1) - (uint8_t*)(ptr2)) /*ptr1 - ptr2*/

#define MIN(a, b) ({				\
	typeof(a) tmp1 = (a), tmp2 = (b);	\
	tmp1 < tmp2 ? tmp1 : tmp2; })

#define MAX(a, b) ({				\
	typeof(a) tmp1 = (a), tmp2 = (b);	\
	tmp1 > tmp2 ? tmp1 : tmp2; })

void clear_events_page(void);
void fmm_clear_all_mem(void);
void clear_process_doorbells(void);
uint32_t get_num_sysfs_nodes(void);

bool is_forked_child(void);

/* Calculate VGPR and SGPR register file size per CU */
#define VGPR_SIZE_PER_CU(gfxv)			\
	(((gfxv) == GFX_VERSION_ARCTURUS ||	\
	  (gfxv) == GFX_VERSION_ALDEBARAN) ? 0x80000 : 0x40000)
#define SGPR_SIZE_PER_CU 0x4000
#endif
