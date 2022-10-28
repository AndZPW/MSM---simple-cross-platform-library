

#ifndef MSM_H
#define MSM_H


#include <string>

#ifdef _WIN32

# include <windows.h>

#else
# include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#endif

namespace msm {
    auto create_read_mapping(const std::string &file_name) -> std::string {
#ifdef _WIN32
        HANDLE hFile = CreateFile(TEXT(file_name.c_str()), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, nullptr);

        HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);

        auto input = (LPSTR) MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        std::string copy_str = (std::string) input;
        UnmapViewOfFile(input);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return copy_str;
#else
        char*f;
        int size;
        struct stat s;

        int fd = open (file_name.c_str(), O_RDONLY);


        int status = fstat (fd, & s);
        size = s.st_size;

        f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        return std::string(f);
#endif
    }

    auto create_write_mapping(const std::string &file_name, const std::string &text) -> void {
#ifdef _WIN32
        HANDLE file = CreateFile(TEXT(file_name.c_str()), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        const char *my_chars = text.c_str();

        HANDLE hMapping = CreateFileMapping(file, nullptr, PAGE_READWRITE, 0, (strlen(my_chars) * sizeof(char)),
                                            nullptr);

        auto output = (LPSTR) MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, (strlen(my_chars) * sizeof(char)));

        CopyMemory((LPVOID) output, (LPVOID) my_chars, (strlen(my_chars) * sizeof(char)));

        UnmapViewOfFile(output);
        CloseHandle(hMapping);
        CloseHandle(file);

#else
        int fd = open(file_name.c_str(), O_RDWR | O_CREAT, (mode_t)0600);
        text=text.c_str();
        size_t textsize = strlen(text) + 1;
        lseek(fd, textsize-1, SEEK_SET);
        write(fd, "", 1);
        char *map = (char*)mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        memcpy(map, text, strlen(text));
        msync(map, textsize, MS_SYNC);
        munmap(map, textsize);
#endif
    }

}
#endif //MSM_H