#include "../include/exec_time.h"
using namespace dte_utils;
lpet::lpet() {
	creation_time = start_time = clock();
}
clock_t lpet::get_creation_time() {
	return creation_time;
}
clock_t lpet::get_start_time() {
	return start_time;
}
clock_t lpet::get_ms_dt_weak() {
	return clock() - start_time;
}
clock_t lpet::get_ms_dt_strong() {
	clock_t dt = get_ms_dt_weak();
	start_time = clock();
	return dt;
}
clock_t lpet::set_start_time() {
	start_time = clock();
	return start_time;
}
hpet::hpet() {
	start_time = std::chrono::high_resolution_clock::now(); //time(&creation_time);
}
std::chrono::steady_clock::time_point hpet::get_creation_time() {
	return creation_time;
}
std::chrono::steady_clock::time_point hpet::get_start_time() {
	return start_time;
}
std::chrono::nanoseconds hpet::get_ns_dt_weak() {
	return std::chrono::high_resolution_clock::now() - start_time;
}
std::chrono::microseconds hpet::get_mcs_dt_weak() {
	return std::chrono::duration_cast<std::chrono::microseconds>(get_ns_dt_weak());
}
std::chrono::milliseconds hpet::get_ms_dt_weak() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(get_ns_dt_weak());
}
std::chrono::nanoseconds hpet::get_ns_dt_strong() {
	std::chrono::steady_clock::time_point end_time = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds dt = end_time - start_time;
	start_time = end_time;
	return dt;
}
std::chrono::microseconds hpet::get_mcs_dt_strong() {
	return std::chrono::duration_cast<std::chrono::microseconds>(get_ns_dt_strong());
}
std::chrono::milliseconds hpet::get_ms_dt_strong() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(get_ns_dt_strong());
}
void hpet::set_start_time() {
	start_time = std::chrono::high_resolution_clock::now();
}