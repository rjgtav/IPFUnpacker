/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "zlib.h"
#include "dbg/dbg.h"


// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------
int
zlibCompress (
    Zlib *self,
    void *data,
    size_t dataSize
) {
    z_stream stream;

    stream.next_in = data;
    stream.avail_in = dataSize;
    stream.avail_out = sizeof(self->buffer);
    stream.next_out = self->buffer;
    stream.total_in = 0;
    stream.total_out = 0;
    stream.zalloc = 0;
    stream.zfree = 0;

    int result;
    if ((result = deflateInit (&stream, 1)) != Z_OK) {
        warning("Can't init compression : error code = %x.", result);
        return 0;
    }

    if ((result = deflate (&stream, Z_FINISH) != Z_STREAM_END)) {
        warning("Can't compress : error code = %x.", result);
        return 0;
    }

    if ((result = deflateEnd (&stream) != Z_OK)) {
        warning("Can't end compression : error code = %x", result);
        return 0;
    }

    self->header.magic = ZLIB_MAGIC_HEADER;
    self->header.size = stream.total_out;

    return 1;
}

int
zlibDecompress (
    Zlib *self,
    void *data,
    size_t dataSize
) {
    z_stream stream;

    stream.next_in = data;
    stream.avail_in = dataSize;
    stream.avail_out = sizeof(self->buffer);
    stream.next_out = self->buffer;
    stream.total_in = 0;
    stream.total_out = 0;
    stream.zalloc = 0;
    stream.zfree = 0;

    if (inflateInit2_(&stream, -15, zlibVersion(), sizeof(z_stream))) {
        buffer_print(data, dataSize, "Buffer : ");
        error("Can't init decompression.");
        return 0;
    }

    int ret;
    if ((ret = inflate(&stream, Z_FINISH)) != Z_STREAM_END) {
        // buffer_print(data, dataSize, "Buffer : ");
        error("Can't decompress. Error : %d", ret);
        return 0;
    }

    if (inflateEnd(&stream)) {
        buffer_print(data, dataSize, "Buffer : ");
        error("Can't end decompression.");
        return 0;
    }

    self->header.magic = ZLIB_MAGIC_HEADER;
    self->header.size = stream.total_out;

    return 1;
}
