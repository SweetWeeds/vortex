#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include "fx3_fpga_wrapper.h"
#include "fx3_memory.h"
#include <VX_config.h>

using namespace vortex;

fx3_memory* p_fx3_mem = nullptr;

extern fpga_result fpgaOpen(fpga_token token, fpga_handle *handle, int flags) {
    if (NULL == handle || flags != 0)
        return FPGA_INVALID_PARAM;
    printf("[INFO:fx3_fpga_wrapper:fpgaOpen] \n");
    p_fx3_mem = new fx3_memory();
    return FPGA_OK;
}

extern fpga_result fpgaClose(fpga_handle handle) {
    if (NULL == handle)
        return FPGA_INVALID_PARAM;
    delete p_fx3_mem;
    return FPGA_OK;
}

extern fpga_result fpgaPrepareBuffer(fpga_handle handle, uint64_t len, void **buf_addr, uint64_t *wsid, int flags) {
    if (NULL == handle || len == 0 || buf_addr == NULL || wsid == NULL) {
        printf("[INFO:fx3_fpga_wrapper:fpgaPrepareBuffer] FPGA_INVALID_PARAM\n");
        return FPGA_INVALID_PARAM;
    }

    //auto sim = reinterpret_cast<opae_sim*>(handle);
    //int ret = sim->prepare_buffer(len, buf_addr, wsid, flags);
    //if (ret != 0)
    //  return FPGA_NO_MEMORY;
    printf("[INFO:fx3_fpga_wrapper:fpgaPrepareBuffer] p_fx3_mem->prepare_buffer\n");
    p_fx3_mem->prepare_buffer(len, buf_addr, wsid, flags);
    printf("[INFO:fx3_fpga_wrapper:fpgaPrepareBuffer] len:%d, buf_addr:%d, wsid:%d, flags:%d\n", len, buf_addr, wsid, flags);

    return FPGA_OK;
}

extern fpga_result fpgaReleaseBuffer(fpga_handle handle, uint64_t wsid) {
    if (NULL == handle) 
        return FPGA_INVALID_PARAM;

    //auto sim = reinterpret_cast<opae_sim*>(handle);
    //sim->release_buffer(wsid);
    printf("[INFO:fx3_fpga_wrapper:fpgaReleaseBuffer] handle: %d, wsid: %d\n", handle, wsid);
    p_fx3_mem->release_buffer(wsid);

    return FPGA_OK;
}

extern fpga_result fpgaGetIOAddress(fpga_handle handle, uint64_t wsid, uint64_t *ioaddr) {
  if (NULL == handle || ioaddr == NULL) 
    return FPGA_INVALID_PARAM;

  //auto sim = reinterpret_cast<opae_sim*>(handle);
  //sim->get_io_address(wsid, ioaddr);
  p_fx3_mem->get_io_address(wsid, ioaddr);
  printf("[INFO:fx3_fpga_wrapper:fpgaGetIOAddress] handle: %d, wsid: %d, ioaddr: %d\n", handle, wsid, *ioaddr);

  return FPGA_OK;
}

extern fpga_result fpgaWriteMMIO64(fpga_handle handle, uint32_t mmio_num, uint64_t offset, uint64_t value) {
  if (NULL == handle || mmio_num != 0) 
    return FPGA_INVALID_PARAM;

  //auto sim = reinterpret_cast<opae_sim*>(handle);
  //sim->write_mmio64(mmio_num, offset, value);
  printf("[INFO:fx3_fpga_wrapper.cpp:fpgaReadMMIO64] Write MIMO64\n");
  p_fx3_mem->write_mmio64(mmio_num, offset, value);

  return FPGA_OK;
}

extern fpga_result fpgaReadMMIO64(fpga_handle handle, uint32_t mmio_num, uint64_t offset, uint64_t *value) {
  if (NULL == handle || mmio_num != 0 || value == NULL) 
    return FPGA_INVALID_PARAM;

  //auto sim = reinterpret_cast<opae_sim*>(handle);
  //sim->read_mmio64(mmio_num, offset, value);
  printf("[INFO:fx3_fpga_wrapper.cpp:fpgaReadMMIO64] Read MIMO64\n");
  p_fx3_mem->read_mmio64(mmio_num, offset, value);

  return FPGA_OK;
}

extern const char *fpgaErrStr(fpga_result e) {
  return "";
}