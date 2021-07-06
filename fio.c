/*
 * fio - the flexible io tester
 *
 * Copyright (C) 2005 Jens Axboe <axboe@suse.de>
 * Copyright (C) 2006-2012 Jens Axboe <axboe@kernel.dk>
 *
 * The license below covers all files distributed with fio unless otherwise
 * noted in the file itself.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 */
#include "fio.h"

int main(int argc, char *argv[], char *envp[]) {
    int ret = 1;
    {
        int err;
        struct stat st;

        bufSrcfd = ts_open("/home/xzjin/FIO_COPY_SRC", O_RDONLY);
        err = errno;
        if(bufSrcfd<=0){
            printf("Open buf src file error, %s\n", strerror(err));
            return -1;
        }
    
        if(fstat(bufSrcfd, &st)){
            err = errno;
    		printf("Get file stat failed %s.\n", strerror(err));
            return -1;
        }

        MAXMEMCPY = st.st_size;
        bufSrc = ts_mmap(0, MAXMEMCPY, PROT_READ,
            MAP_PRIVATE, bufSrcfd, 0);
    }
    compiletime_assert(TD_NR <= TD_ENG_FLAG_SHIFT, "TD_ENG_FLAG_SHIFT");

    if (initialize_fio(envp)) return 1;

#if !defined(CONFIG_GETTIMEOFDAY) && !defined(CONFIG_CLOCK_GETTIME)
#error "No available clock source!"
#endif

    if (fio_server_create_sk_key()) goto done;

    if (parse_options(argc, argv)) goto done_key;

    /*
     * line buffer stdout to avoid output lines from multiple
     * threads getting mixed
     */
    setvbuf(stdout, NULL, _IOLBF, 0);

    fio_time_init();

    if (nr_clients) {
        set_genesis_time();
        printf("nr_clints\n");

        if (fio_start_all_clients()) goto done_key;
        ret = fio_handle_clients(&fio_client_ops);
    } else
        printf("fio_backend.\n");
        ret = fio_backend(NULL);

done_key:
    fio_server_destroy_sk_key();
done:
    deinitialize_fio();
    return ret;
}
