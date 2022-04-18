#include "fx3_memory.h"

#include <iostream>
#include <fstream>
#include <iomanip>
//#include <mem.h>

#include <VX_config.h>
//#include <vortex_afu.h>

#include <future>
#include <list>
#include <queue>
#include <unordered_map>

#ifndef MEMORY_BANKS 
  #ifdef PLATFORM_PARAM_LOCAL_MEMORY_BANKS
    #define MEMORY_BANKS PLATFORM_PARAM_LOCAL_MEMORY_BANKS
  #else
    #define MEMORY_BANKS 2
  #endif
#endif

#ifndef MEM_CYCLE_RATIO
#define MEM_CYCLE_RATIO -1
#endif

#define CACHE_BLOCK_SIZE  64

#define CCI_LATENCY  8
#define CCI_RAND_MOD 8
#define CCI_RQ_SIZE 16
#define CCI_WQ_SIZE 16

#ifndef TRACE_START_TIME
#define TRACE_START_TIME 0ull
#endif

#ifndef TRACE_STOP_TIME
#define TRACE_STOP_TIME -1ull
#endif

#define RAM_PAGE_SIZE 4096

using namespace vortex;


static uint64_t timestamp = 0;

static void *__aligned_malloc(size_t alignment, size_t size) {
  // reserve margin for alignment and storing of unaligned address
  size_t margin = (alignment-1) + sizeof(void*);
  void *unaligned_addr = malloc(size + margin);
  void **aligned_addr = (void**)((uintptr_t)(((uint8_t*)unaligned_addr) + margin) & ~(alignment-1));
  aligned_addr[-1] = unaligned_addr;
  return aligned_addr;
}

static void __aligned_free(void *ptr) {
  // retreive the stored unaligned address and use it to free the allocation
  void* unaligned_addr = ((void**)ptr)[-1];
  free(unaligned_addr);
}

///////////////////////////////////////////////////////////////////////////////

fx3_memory::fx3_memory() : stop_(false), host_buffer_ids_(0) {
    printf("[INFO:fx3_memory.cpp:fx3_memory::fx3_memory()] \n");
    // launch execution thread
    future_ = std::async(std::launch::async, [&]{                 
        while (!stop_) {
            std::lock_guard<std::mutex> guard(mutex_);
            //this->tick();
        }
    }); 
}

fx3_memory::~fx3_memory() {
    printf("[INFO:fx3_memory:~fx3_memory]\n");
    //delete impl_;
}

int fx3_memory::prepare_buffer(uint64_t len, void **buf_addr, uint64_t *wsid, int flags) {
    printf("[INFO:fx3_memory:prepare_buffer] Preparing Buffer\n");
    auto alloc = __aligned_malloc(CACHE_BLOCK_SIZE, len);
    if (alloc == NULL) {
        printf("[INFO:fx3_memory:prepare_buffer] Memory Allocation Failed\n");
        return -1;
    }
    host_buffer_t buffer;
    buffer.data   = (uint64_t*)alloc;
    buffer.size   = len;
    buffer.ioaddr = uintptr_t(alloc); 
    printf("[INFO:fx3_memory:prepare_buffer] buffer.ioaddr = uintptr_t(alloc); host_buffer_ids:%d\n", host_buffer_ids_);
    auto buffer_id = host_buffer_ids_++;
    printf("[INFO:fx3_memory:prepare_buffer] auto buffer_id = host_buffer_ids_++;\n");
    host_buffers_.emplace(buffer_id, buffer);
    printf("[INFO:fx3_memory:prepare_buffer] host_buffers_.emplace(buffer_id, buffer);\n");
    *buf_addr = alloc;
    *wsid = buffer_id;
    printf("[INFO:fx3_memory:prepare_buffer] Buffer Allocation Succeed\n");
    return 0;
    //return impl_->prepare_buffer(len, buf_addr, wsid, flags);
}

void fx3_memory::release_buffer(uint64_t wsid) {
    printf("[INFO:fx3_memory:release_buffer] Start\n");
    auto it = host_buffers_.find(wsid);
    if (it != host_buffers_.end()) {
      __aligned_free(it->second.data);
      host_buffers_.erase(it);
    }
    printf("[INFO:fx3_memory:release_buffer] End\n");
}

void fx3_memory::get_io_address(uint64_t wsid, uint64_t *ioaddr) {
    printf("[INFO:fx3_memory:get_io_address] Start\n");
    //impl_->get_io_address(wsid, ioaddr);
}

void fx3_memory::write_mmio64(uint32_t mmio_num, uint64_t offset, uint64_t value) {
    printf("[INFO:fx3_memory:write_mmio64] Start\n");
    //impl_->write_mmio64(mmio_num, offset, value);
}

void fx3_memory::read_mmio64(uint32_t mmio_num, uint64_t offset, uint64_t *value) {
    printf("[INFO:fx3_memory:read_mmio64] Start\n");
    //impl_->read_mmio64(mmio_num, offset, value);
}