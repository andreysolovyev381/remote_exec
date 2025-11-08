// memfd_exec.cpp

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>

#include <vector>
#include <string>
#include <iostream>

// Fallback if linux/memfd.h lacks definitions
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif

#ifndef SYS_memfd_create
#ifdef __x86_64__
#define SYS_memfd_create 319
#elif defined(__aarch64__)
#define SYS_memfd_create 279
#else
#error "Please define SYS_memfd_create for your arch"
#endif
#endif

[[noreturn]] void die(const char* what, int fd) {
    std::cerr << what << ": " << strerror(errno) << "\n";
    if (fd != -1) {
        close(fd);
    }
    std::_Exit(127);
}

int main(int argc, char** argv, char** envp) {
    if (prctl(PR_SET_DUMPABLE, 0) == -1) {
        die("prctl(PR_SET_DUMPABLE)", -1);
    }
    int fd = syscall(SYS_memfd_create, "remote_exec", 0);
    if (fd == -1) {
        die("memfd_create", fd);
    }

    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        die("fcntl(F_GETFD)", fd);
    }
    flags &= ~FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, flags) == -1) {
        die("fcntl(F_SETFD)", fd);
    }

    char buf[1 << 20];
    ssize_t r;
    while ((r = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        ssize_t w = 0;
        while (w < r) {
            ssize_t nw = write(fd, buf + w, static_cast<size_t>(r - w));
            if (nw == -1) {
                if (errno == EINTR) continue;
                die("write", fd);
            }
            w += nw;
        }
    }
    if (r < 0) {
        die("read", fd);
    }

#ifdef F_ADD_SEALS
    if (fcntl(fd, F_ADD_SEALS, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_WRITE) == -1) {
        if (errno != ENOSYS && errno != EINVAL && errno != EPERM) {
            die("F_ADD_SEALS", fd);
        }
    }
#endif

    if (fchmod(fd, 0755) == -1) {
        die("fchmod", fd);
    }

    std::string const path = "/proc/self/fd/" + std::to_string(fd);

    std::vector<char*> exec_argv;
    exec_argv.push_back(const_cast<char*>(path.c_str()));
    for (int i = 1; i < argc; ++i) {
        exec_argv.push_back(argv[i]);
    }
    exec_argv.push_back(nullptr);

#if defined(__GLIBC__)
    if (fexecve(fd, exec_argv.data(), envp) != 0) {
        // fallback to execve below
    }
#endif

    execve(path.c_str(), exec_argv.data(), envp);
    die("execve", fd);
}
