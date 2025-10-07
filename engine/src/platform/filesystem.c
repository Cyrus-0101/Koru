#include "filesystem.h"

#include "core/logger.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @file filesystem.c
 *
 * @brief Implements a simple cross-platform filesystem abstraction layer.
 * This module allows for basic file operations such as checking for file existence,
 * opening, reading, writing, and closing files.
 */

b8 filesystem_exists(const char* path) {
#ifdef _MSC_VER
    struct _stat buffer;
    return _stat(path, &buffer);
#else
    struct stat buffer;
    return stat(path, &buffer) == 0;
#endif
}

b8 filesystem_open(const char* path, file_modes mode, b8 binary, file_handle* out_handle) {
    out_handle->is_valid = False;
    out_handle->handle = 0;
    const char* mode_str;

    if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "w+b" : "w+";
    } else if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) == 0) {
        mode_str = binary ? "rb" : "r";
    } else if ((mode & FILE_MODE_READ) == 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "wb" : "w";
    } else {
        KERROR("Invalid mode passed while trying to open file: '%s'", path);
        return False;
    }

    // Attempt to open the file.
    FILE* file = fopen(path, mode_str);
    if (!file) {
        KERROR("Error opening file: '%s'", path);
        return False;
    }

    out_handle->handle = file;
    out_handle->is_valid = True;

    return True;
}

void filesystem_close(file_handle* handle) {
    if (handle->handle) {
        fclose((FILE*)handle->handle);
        handle->handle = 0;
        handle->is_valid = False;
    }
}

b8 filesystem_size(file_handle* handle, u64* out_size) {
    if (handle->handle) {
        fseek((FILE*)handle->handle, 0, SEEK_END);
        *out_size = ftell((FILE*)handle->handle);

        // KTRACE("File size: %llu", *out_size);

        rewind((FILE*)handle->handle);

        return True;
    }
    return False;
}

b8 filesystem_read_line(file_handle* handle, u64 max_length, char** line_buf, u64* out_line_length) {
    // Checks if there is a handle, line_buff is not null, out_line_length is not null, and max_length is greater than 0.
    if (handle->handle && line_buf && out_line_length && max_length > 0) {
        char* buf = *line_buf;  // Dereference line_buf to get the actual buffer.

        // If fgets is successful, it returns the buffer; otherwise, it returns NULL.
        if (fgets(buf, max_length, (FILE*)handle->handle) != 0) {
            *out_line_length = strlen(*line_buf);
            return True;
        }
    }
    return False;
}

b8 filesystem_write_line(file_handle* handle, const char* text) {
    if (handle->handle) {
        i32 result = fputs(text, (FILE*)handle->handle);
        if (result != EOF) {
            result = fputc('\n', (FILE*)handle->handle);
        }

        // Make sure to flush the stream so it is written to the file immediately.
        // This prevents data loss in the event of a crash.
        fflush((FILE*)handle->handle);
        return result != EOF;
    }
    return False;
}

b8 filesystem_read(file_handle* handle, u64 data_size, void* out_data, u64* out_bytes_read) {
    if (handle->handle && out_data) {
        *out_bytes_read = fread(out_data, 1, data_size, (FILE*)handle->handle);
        if (*out_bytes_read != data_size) {
            return False;
        }
        return True;
    }
    return False;
}

b8 filesystem_read_all_bytes(file_handle* handle, u8* out_bytes, u64* out_bytes_read) {
    if (handle->handle && out_bytes && out_bytes_read) {
        // File size
        u64 size = 0;

        if (!filesystem_size(handle, &size)) {
            return False;
        }

        *out_bytes_read = fread(out_bytes, 1, size, (FILE*)handle->handle);

        return *out_bytes_read == size;
    }
    return False;
}

b8 filesystem_write(file_handle* handle, u64 data_size, const void* data, u64* out_bytes_written) {
    if (handle->handle) {
        *out_bytes_written = fwrite(data, 1, data_size, (FILE*)handle->handle);
        if (*out_bytes_written != data_size) {
            return False;
        }
        fflush((FILE*)handle->handle);
        return True;
    }
    return False;
}

b8 filesystem_read_all_text(file_handle* handle, char* out_text, u64* out_bytes_read) {
    if (handle->handle && out_text && out_bytes_read) {
        // File size
        u64 size = 0;
        if (!filesystem_size(handle, &size)) {
            return False;
        }

        *out_bytes_read = fread(out_text, 1, size, (FILE*)handle->handle);
        return *out_bytes_read == size;
    }
    return False;
}