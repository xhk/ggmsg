#pragma once
#include <ctime>

struct time_t_traits
{
	// The time type.
	typedef std::time_t time_type;

	// The duration type.
	struct duration_type
	{
		duration_type() : value(0) {}
		duration_type(std::time_t v) : value(v) {}
		std::time_t value;
	};

	// Get the current time.
	static time_type now()
	{
		return std::time(0);
	}

	// Add a duration to a time.
	static time_type add(const time_type& t, const duration_type& d)
	{
		return t + d.value;
	}

	// Subtract one time from another.
	static duration_type subtract(const time_type& t1, const time_type& t2)
	{
		return duration_type(t1 - t2);
	}

	// Test whether one time is less than another.
	static bool less_than(const time_type& t1, const time_type& t2)
	{
		return t1 < t2;
	}

	// Convert to POSIX duration type.
	static boost::posix_time::time_duration to_posix_duration(
		const duration_type& d)
	{
		return boost::posix_time::seconds(d.value);
	}
};


typedef boost::asio::basic_deadline_timer<
	std::time_t, time_t_traits> time_t_timer;
