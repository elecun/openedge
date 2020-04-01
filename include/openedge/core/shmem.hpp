

/**
 * @file    shmem.hpp
 * @brief   Shared Memory
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_SHMEM_HPP_
#define _OPENEDGE_CORE_SHMEM_HPP_

#include <cstddef>
#include <stdint.h>

namespace oe {
    namespace core {

        typedef struct shmem {
            char*       name;
            uint8_t*    mapped;
            int fd;
            size_t      size;
            bool        created;
        } shmem_t;

        struct shmem_header_t {
            char    magic[4];
            char    pad[4];
            uint64_t    size;
        };


        //functions
        //shmem_t*    shmem_create(const char* name, size_t size);
        // shmem_t*    shmem_open(const char* name, size_t size);
        shmem_t*    shmem_create_or_open(const char* name, size_t size);
        // void        shmem_close(shmem_t* shm);
        // void*       shmem_ptr(shmem_t* shm);
        // const char* shmem_get_name(shmem_t* shm);
        // size_t      shmem_get_size(shmem_t* shm);
        // bool        shmem_was_created(shmem_t* shm);


    } //namespace core
} //namespace oe

#endif