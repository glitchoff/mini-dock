#include <unistd.h>    // sethostname, execvp, getpid
#include <sched.h>     // clone() and the CLONE_* namespace flags
#include <sys/wait.h>  // waitpid, WIFEXITED/WEXITSTATUS
#include <sys/mount.h> // mount() for /proc
#include <cstdio>      // printf, fprintf, perror
#include <string>      // std::string
#include <vector>      // std::vector

using namespace std;

// Namespaces for the child, plus SIGCHLD so the parent gets signaled on exit.
constexpr int FLAGS = CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD;

vector<char*> to_c_argv(const vector<string>& cmd) {
    vector<char*> argv;
    for (const auto& a : cmd) argv.push_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);
    return argv;
}

// Runs in the new namespaces after clone() forks.
static int child(void* arg) {
    auto& cmd = *static_cast<vector<string>*>(arg);

    sethostname("minidock", 8);
    mount("proc", "/proc", "proc", 0, nullptr);

    auto argv = to_c_argv(cmd);
    execvp(argv[0], argv.data());
    perror("execvp");
    return 1;
}

bool parse_args(int argc, char* argv[], vector<string>& cmd) {
    for (int i = 1; i < argc; i++) {
        if (string{argv[i]} == "run") continue;
        if (cmd.empty() && argv[i][0] == '-') {
            fprintf(stderr, "unknown flag: %s\n", argv[i]);
            return false;
        }
        cmd.push_back(argv[i]);
    }
    if (cmd.empty()) {
        fprintf(stderr, "usage: minidock run <command> [args...]\n");
        return false;
    }
    return true;
}


int main(int argc, char* argv[]) {
    vector<string> cmd;
    if (!parse_args(argc, argv, cmd)) return 1;

    printf("MiniDock: container runtime\nCommand: %s\n", cmd[0].c_str());

    constexpr size_t STACK = 1 << 20;
    static char stack[STACK];

    pid_t pid = clone(child, stack + STACK, FLAGS, &cmd);
    if (pid == -1) { perror("clone"); return 1; }

    int status;
    if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return 1; }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}