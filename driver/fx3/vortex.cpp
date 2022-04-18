#include <vortex.h>
#include <VX_config.h>
#include "../common/vx_utils.h"
#include "../common/vx_malloc.h"
#include "./include/usbXfer.h"
#include "fx3_fpga_wrapper.h"

class vx_device {
public:
    vx_device() 
        : mem_allocator(
            ALLOC_BASE_ADDR, 
            ALLOC_BASE_ADDR + LOCAL_MEM_SIZE,
            4096,            
            CACHE_BLOCK_SIZE)
    {}
    
    ~vx_device() {}

    fpga_handle fpga;
    usbxfer::usbpipe* p_usbpipe_handle = NULL;
    vortex::MemoryAllocator mem_allocator;
    unsigned version;
    unsigned num_cores;
    unsigned num_warps;
    unsigned num_threads;
};

typedef struct vx_buffer_ {
    uint64_t wsid;
    void* host_ptr;
    uint64_t io_addr;
    vx_device_h hdevice;
    uint64_t size;
} vx_buffer_t;

extern int vx_dev_open(vx_device_h* hdevice) {
    if (nullptr == hdevice)
        return  -1;
    fpga_handle accel_handle;
    vx_device* device;

    fpgaOpen(NULL, &accel_handle, 0);

    // Device Init
    device = new vx_device();
    if (nullptr == device) {
        printf("[ERROR:vortex.cpp] Device Instantiation Failed\n");
        fpgaClose(accel_handle);
        return -1;
    }
    device->p_usbpipe_handle = new usbxfer::usbpipe();
    if (!device->p_usbpipe_handle->open()) {
        printf("[ERROR:vortex.cpp] FX3 USB Device Open Failed\n");
        return -1;
    }
    device->version = 120;
    device->num_cores = 64;
    device->num_warps  = 4;
    device->num_threads = 32;
    *hdevice = device;
    printf("[INFO:vortex.cpp] FX3 USB Device Open Succeed\n");
    return 0;
}

extern int vx_dev_close(vx_device_h hdevice) {
    vx_device* device;
    device = (vx_device*) &hdevice;
    device->p_usbpipe_handle->close();
    delete device->p_usbpipe_handle;
    return 0;
    //return -1;
}

extern int vx_dev_caps(vx_device_h hdevice, uint32_t caps_id, uint64_t* value) {
    if (nullptr == hdevice)
        return -1;

    vx_device *device = ((vx_device*)hdevice);

    switch (caps_id) {
    case VX_CAPS_VERSION:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_VERSION\n");
        *value = device->version;
        break;
    case VX_CAPS_MAX_CORES:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_MAX_CORES\n");
        *value = device->num_cores;
        break;
    case VX_CAPS_MAX_WARPS:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_MAX_WARPS\n");
        *value = device->num_warps;
        break;
    case VX_CAPS_MAX_THREADS:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_MAX_THREADS\n");
        *value = device->num_threads;
        break;
    case VX_CAPS_CACHE_LINE_SIZE:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_CACHE_LINE_SIZE\n");
        *value = CACHE_BLOCK_SIZE;
        break;
    case VX_CAPS_LOCAL_MEM_SIZE:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_LOCAL_MEM_SIZE\n");
        *value = LOCAL_MEM_SIZE;
        break;
    case VX_CAPS_ALLOC_BASE_ADDR:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_ALLOC_BASE_ADDR\n");
        *value = ALLOC_BASE_ADDR;
        break;
    case VX_CAPS_KERNEL_BASE_ADDR:
        printf("[INFO:vortex.cpp] Driver:vx_dev_caps:VX_CAPS_KERNEL_BASE_ADDR\n");
        *value = STARTUP_ADDR;
        break;
    default:
        fprintf(stderr, "[VXDRV] Error: invalid caps id: %d\n", caps_id);
        std::abort();
        return -1;
    }

    return 0;
}

extern int vx_mem_alloc(vx_device_h hdevice, uint64_t size, uint64_t* dev_maddr) {
    printf("[INFO:vortex.cpp] Driver:vx_mem_alloc\n");

    if (nullptr == hdevice 
     || nullptr == dev_maddr
     || 0 >= size)
        return -1;

    vx_device *device = ((vx_device*)hdevice);
    return device->mem_allocator.allocate(size, dev_maddr);
}

int vx_mem_free(vx_device_h hdevice, uint64_t dev_maddr) {
    if (nullptr == hdevice)
        return -1;

    vx_device *device = ((vx_device*)hdevice);
    return device->mem_allocator.release(dev_maddr);
}

extern int vx_buf_alloc(vx_device_h hdevice, uint64_t size, vx_buffer_h* hbuffer) {
    printf("[INFO:vortex.cpp] Driver:vx_buf_alloc\n");
    fpga_result res;
    void* host_ptr;
    uint64_t wsid;
    uint64_t io_addr;
    vx_buffer_t* buffer;

    if (nullptr == hdevice
     || 0 >= size
     || nullptr == hbuffer)
        return -1;

    vx_device *device = ((vx_device*)hdevice);

    size_t asize = aligned_size(size, CACHE_BLOCK_SIZE);
    buffer = (vx_buffer_t*)malloc(sizeof(vx_buffer_t));

    printf("[INFO:vortex.cpp:vx_buf_alloc] fpgaPrepareBuffer\n");
    res = fpgaPrepareBuffer(device->fpga, asize, &host_ptr, &wsid, 0);
    if (FPGA_OK != res) {
        return -1;
    }

    // Get the physical address of the buffer in the accelerator
    printf("[INFO:vortex.cpp:vx_buf_alloc] Get the physical address of the buffer in the accelerator\n");
    res = fpgaGetIOAddress(device->fpga, wsid, &io_addr);
    if (FPGA_OK != res) {
        fpgaReleaseBuffer(device->fpga, wsid);
        return -1;
    }

    // allocate buffer object
    printf("[INFO:vortex.cpp:vx_buf_alloc] allocate buffer object\n");
    buffer = (vx_buffer_t*)malloc(sizeof(vx_buffer_t));
    if (nullptr == buffer) {
        fpgaReleaseBuffer(device->fpga, wsid);
        return -1;
    }

    buffer->wsid     = wsid;
    buffer->host_ptr = host_ptr;
    buffer->io_addr  = io_addr;
    buffer->hdevice  = hdevice;
    buffer->size     = asize;

    *hbuffer = buffer;
    printf("[INFO:vortex.cpp:vx_buf_alloc] wsid: %d, host_ptr: %d, io_addr:%d, hdevice:%d, asize:%d\n",
                wsid, host_ptr, io_addr, hdevice, asize);

    return 0;
}

extern void* vx_host_ptr(vx_buffer_h hbuffer) {
    printf("[INFO:vortex.cpp] Driver:vx_host_ptr\n");

    if (nullptr == hbuffer) {
        printf("[INFO:vortex.cpp] Driver:vx_host_ptr:nullptr==hbuffer\n");
        return nullptr;
    }

    vx_buffer_t* buffer = ((vx_buffer_t*)hbuffer);
    printf("[INFO:vortex.cpp] Driver:vx_host_ptr:buffer->host_ptr: %d\n", buffer->host_ptr);
    return buffer->host_ptr;
}

extern int vx_buf_free(vx_buffer_h hbuffer) {
    printf("[INFO:vortex.cpp] Driver:vx_buf_free\n");

    if (nullptr == hbuffer)
        return -1;

    vx_buffer_t* buffer = ((vx_buffer_t*)hbuffer);
    vx_device *device = ((vx_device*)buffer->hdevice);

    fpgaReleaseBuffer(device->fpga, buffer->wsid);
    free(buffer);

    return 0;
}

extern int vx_copy_to_dev(vx_buffer_h hbuffer, uint64_t dev_maddr, uint64_t size, uint64_t src_offset) {
    printf("[INFO:vortex.cpp] Driver:vx_copy_to_dev\n");
    return 0;
}

extern int vx_copy_from_dev(vx_buffer_h /*hbuffer*/, uint64_t /*dev_maddr*/, uint64_t /*size*/, uint64_t /*dest_offset*/) {
    printf("[INFO:vortex.cpp] Driver:vx_copy_from_dev\n");
     return -1;
}

extern int vx_start(vx_device_h /*hdevice*/) {
    printf("[INFO:vortex.cpp] Driver:vx_start\n");
    return -1;
}

extern int vx_ready_wait(vx_device_h /*hdevice*/, uint64_t /*timeout*/) {
    printf("[INFO:vortex.cpp] Driver:vx_ready_wait\n");
    return -1;
}