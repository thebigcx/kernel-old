#pragma once

fs_file_t* fs_open(const char* path);
int fs_close(fs_file_t* file);
int fs_read(fs_file_t* file, size_t cnt, void* buf);
int fs_write(fs_file_t* file, size_t cnt, void* buf);
int fs_seek(fs_file_t* file, uint64_t off, int whence);