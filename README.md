# async_io
`async_io` provides a C++ asynchronous file IO API with futures.


```c++
  // Read bytes 10 -> 100 from "file.dat"
  auto future = async_io::read("file.dat", {10, 100});
  
  while (...) {
    // Do some work
  }
  
  // Block on previously issued IO
  auto data = future.get();
```

It's implemented using the [POSIX AIO](http://man7.org/linux/man-pages/man7/aio.7.html)
interface.
