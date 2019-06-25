#pragma once

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

namespace async_io {

struct FileSpan {
  size_t begin;
  size_t end;

  size_t size() {
    if (end < begin) {
      return 0;
    } else {
      return end - begin;
    }
  }
};

struct AsyncRead {
  AsyncRead(const std::string& fname, FileSpan span)
            : buffer_(span.size()), span_(span) {
    int fd = open(fname.c_str(), O_RDONLY);
    if (fd == -1) {
      throw std::runtime_error("AsyncRead could not read file \"" + fname + "\"");
    }

    if (span.size() == 0) {
      ready_ = true;
    } else {
      aio_request_.aio_fildes = fd;
      aio_request_.aio_buf = buffer_.data();
      aio_request_.aio_nbytes = span.size();
      // TODO: what should reqprio be?
      aio_request_.aio_reqprio = 0;
      aio_request_.aio_offset = span.begin;
      aio_request_.aio_sigevent.sigev_notify = SIGEV_NONE;

      int status = aio_read(&aio_request_);

      if (status != 0) {
        // Failed to read!
        throw std::runtime_error("Could not read from fd " + std::to_string(fd));
      }
    }
  }

  template<class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    if (is_ready()) {
      return std::future_status::ready;
    } else if (timeout_duration > std::chrono::seconds(0)) {
      auto begin = std::chrono::high_resolution_clock::now();
      auto end = begin;
      do {
        if (is_ready()) {
          return std::future_status::ready;
        }
        end = std::chrono::high_resolution_clock::now();
      } while (std::chrono::duration<double>(end - begin) < timeout_duration);
      return std::future_status::timeout;
    }
  }

  bool is_ready() const {
    if (ready_) {
      return true;
    }
    int return_value = aio_error(&aio_request_);
    return (return_value != EINPROGRESS);
  }

  std::string get() {
    if (ready_) {
      return std::string(buffer_.data(), span_.size());
    }

    while (!is_ready()) {
      // TODO: backoff
    }

    ready_ = true;

    int return_value = aio_return(&aio_request_);
    if (return_value == -1) {
      throw std::runtime_error("Asynchronous read error'd out.");
    }
    if (return_value == 0) {
      return std::string("");
    } else {
      return std::string(buffer_.data(), return_value);
    }
  }

  aiocb aio_request_;
  std::vector<char> buffer_;
  bool ready_ = false;
  FileSpan span_;
};

} // end async_io
