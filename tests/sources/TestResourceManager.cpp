#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>

#include <golxzn/core/utils/error.hpp>
#include <golxzn/core/resources/manager.hpp>

TEST_CASE("Resource Manager", "[golxzn][core][tests]") {
	using namespace golxzn;

	SECTION("initilization") {
		core::res_man::initialize("core");
	}
}
