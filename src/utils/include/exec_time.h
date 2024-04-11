#pragma once
#include <ctime>
#include <chrono>
namespace dte_utils {
	//miliseconds accuracy
	struct lpet {
		public:
			lpet();
			clock_t get_creation_time();
			clock_t get_start_time();
			clock_t get_ms_dt_weak();
			clock_t get_ms_dt_strong();
			clock_t set_start_time();
		protected:
			clock_t creation_time, start_time;
	};
	//nanoseconds accuracy
	struct hpet {
		public:
			hpet();
			std::chrono::steady_clock::time_point get_creation_time();
			std::chrono::steady_clock::time_point get_start_time();
			std::chrono::nanoseconds get_ns_dt_weak();
			std::chrono::microseconds get_mcs_dt_weak();
			std::chrono::milliseconds get_ms_dt_weak();
			std::chrono::nanoseconds get_ns_dt_strong();
			std::chrono::microseconds get_mcs_dt_strong();
			std::chrono::milliseconds get_ms_dt_strong();
			void set_start_time();
		protected:
			std::chrono::steady_clock::time_point creation_time, start_time;
	};
}