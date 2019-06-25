#include <memory>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <future>

#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <async_io/async_io.hpp>

int main(int argc, char** argv) {
  async_io::read read("test.cpp", {5, 1000});
  while (!read.is_ready()) {
    printf("Waiting...\n");
  }
  std::string buf = read.get();
  std::cout << buf << std::endl;

  async_io::write write("test1.dat", std::vector<char>(buf.begin(), buf.end()), 0);
  write.get();

  return 0;
}
