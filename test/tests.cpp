// Copyright (C) 2021, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#include <vector>

#include <catch2/catch.hpp>

#include <ysqlpp.h>

#include "../src/services/TempStorage.h"

TEST_CASE("Create DB & executing some commands", "[ysqlpp]") {
  auto db = std::make_shared<y44::ysqlpp::DB>(y44::ysqlpp::open("test.db"));


  y44::ftt::service::TempStorage ts{db};
  auto v1 = ts.get_temp_for_time(1632570558);
  REQUIRE(v1.size() == 1);
  REQUIRE(v1[0].first == 1632570558);

  // add new record
  auto time = std::chrono::system_clock::now().time_since_epoch().count();
  ts.store_temp_at_time(time, 1234);
  auto v2 = ts.get_temp_for_time(time);
  REQUIRE(v2[0].first == time);
  REQUIRE(v2[0].second == 1234);
}