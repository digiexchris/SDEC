#pragma once
#include <string>
#include <string_view>
#include <etl/string.h>
#include <etl/format_spec.h>
#include <etl/string_stream.h>
#include "etl_profile.h"
#include <fmt/format.h>



template <typename... Args>
etl::istring dyna_print(etl::string_view rt_fmt_str, Args &&...args)
{
	return fmt::format(rt_fmt_str, fmt::make_format_args(args...));
}