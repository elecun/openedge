

#include "shmem.hpp"

// #include <unistd.h>
// #include <fcntl.h>
#include <sys/mman.h>

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>

namespace oe {
    namespace core {

        static const char*  _shmem_magic_("smem");
        static const size_t _shmem_header_size_(64);

        // forward declarations.
        struct shmem_header_t;
        static uint32_t			shmem_calc_total_size(size_t size);
        static shmem_header_t*	shmem_get_header(shmem_t* shm);
        static shmem*			shmem_connect(const char* name, size_t size);
        static bool				shmem_platform_startup(shmem_t* shm, uint32_t size);
        static void				shmem_platform_shutdown(shmem_t* shm);
        static void				strfree(char* str);
        static char*			strclone(const char* str);
        static char*			strjoin(const char* strA, const char* strB);

        //create shared memory
        shmem_t* shmem_create_or_open(const char* name, size_t size){
            
        }

        // shmem_t* shmem_create_or_open(const char* name, size_t size){
	    //     return shmem_connect(name, size);
        // }

        // void shmem_close(shmem_t* shm){
	    //     if(!shm )
        //         return;

        //     shmem_platform_shutdown(shm);
        //     strfree(shm->name);
        //     free(shm);
        // }

        // void* shmem_ptr(shmem_t* shm){
        //     assert(shm);
	    //     if(!shm)
		//         return nullptr;

	    //     assert(shm->mapped);
	    //     if(!shm->mapped)
		//         return nullptr;

	    //     return (void*)(shm->mapped + _shmem_header_size_);
        // }

        // const char* shmem_get_name(shmem_t* shm){
        //     assert(shm);
        //     if(!shm)
        //         return "";
        //     return shm->name;
        // }

        // size_t	shmem_get_size(shmem_t* shm){
	    //     assert(shm);
	    //     if(!shm)
		//         return 0;
	    //     return shm->size;
        // }

        // bool shmem_was_created(shmem_t* shm){
	    //     assert(shm);
	    //     if(!shm)
		//         return false;
	    //     return shm->created;
        // }

    } //namespace core

} //namespace oe