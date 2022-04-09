```c++
#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

  uuids::uuid id = uuids::uuid_system_generator{}();
  auto sid = uuids::to_string(id);
  fmt::print("{}\n", sid);

  fmt::print("{}\n", uuids::to_string(uuids::uuid_system_generator{}()));
  fmt::print("{}\n", uuids::to_string(uuids::uuid_system_generator{}()));
  fmt::print("{}\n", uuids::to_string(uuids::uuid_system_generator{}()));


```
