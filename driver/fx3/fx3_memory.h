#pragma once
#ifndef FX3_MEMORY_H
#define FX3_MEMORY_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>

#include <future>
#include <list>
#include <queue>
#include <unordered_map>

#include "../../sim/vlsim/vortex_afu.h"

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

#undef MEM_BLOCK_SIZE
#define MEM_BLOCK_SIZE (PLATFORM_PARAM_LOCAL_MEMORY_DATA_WIDTH / 8)

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

#ifndef VERILATOR_RESET_VALUE
#define VERILATOR_RESET_VALUE 2
#endif

#define RAM_PAGE_SIZE 4096

namespace vortex {
    typedef struct {
        bool ready;  
        std::array<uint8_t, MEM_BLOCK_SIZE> data;
        uint32_t addr;
    } mem_rd_req_t;

    typedef struct {
        int cycles_left;  
        std::array<uint8_t, CACHE_BLOCK_SIZE> data;
        uint64_t addr;
        uint32_t mdata;
    } cci_rd_req_t;

    typedef struct {
        int cycles_left;  
        uint32_t mdata;
    } cci_wr_req_t;

    typedef struct {    
        uint64_t* data;
        size_t    size;
        uint64_t  ioaddr;  
    } host_buffer_t;

    class fx3_memory {
    public:
        fx3_memory();
        virtual ~fx3_memory();
        int prepare_buffer(uint64_t len, void **buf_addr, uint64_t *wsid, int flags);
        void release_buffer(uint64_t wsid);
        void get_io_address(uint64_t wsid, uint64_t *ioaddr);
        void write_mmio64(uint32_t mmio_num, uint64_t offset, uint64_t value);
        void read_mmio64(uint32_t mmio_num, uint64_t offset, uint64_t *value);
    private:
        std::future<void> future_;
        bool stop_;
        std::unordered_map<int64_t, host_buffer_t> host_buffers_;
        int64_t host_buffer_ids_;
        std::mutex mutex_;
    };
}

#endif
