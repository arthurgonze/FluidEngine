#define _USE_MATH_DEFINES
#include <cstdio>
#include <array>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>
#include <thread>

using namespace std;
using namespace chrono;

const size_t k_buffer_size = 80;
const char* k_gray_scale_table = " .:-=+*#%@";
const size_t k_gray_scale_table_size = sizeof(k_gray_scale_table) / sizeof(char);

void draw(const array<double, k_buffer_size>& height_field)
{
	string buffer(k_buffer_size, ' ');

	// Convert height field to grayscale
	for(size_t i = 0; i < k_buffer_size; ++i)
	{
		double height = height_field[i];
		size_t table_index = min(static_cast<size_t>(floor(k_gray_scale_table_size * height)), k_gray_scale_table_size - 1);
		buffer[i] = k_gray_scale_table[table_index];
	}

	// Clear old prints
	for(size_t i = 0; i < k_buffer_size; ++i)
	{
		printf("\b");
	}

	// Draw new buffer
	printf("%s", buffer.c_str());
	fflush(stdout);
}

void accumulate_wave_to_height_field(const double position, const double wave_length, const double max_height, array<double, k_buffer_size>* height_field)
{
	const double quarter_wave_length = 0.25 * wave_length;
	const int start = static_cast<int>((position - quarter_wave_length) * k_buffer_size);
	const int end = static_cast<int>((position + quarter_wave_length) * k_buffer_size);

	for(int i = start; i < end; ++i)
	{
		int iNew = i;
		if(i < 0)
		{
			iNew = -i - 1;
		}
		else if(i >= static_cast<int>(k_buffer_size))
		{
			iNew = 2 * k_buffer_size - i - 1;
		}

		double distance = fabs((i + 0.5) / k_buffer_size - position);
		double height = max_height * 0.5 * 
			(cos(min(distance * M_PI / quarter_wave_length, M_PI))+1.0);
		(*height_field)[iNew] += height;
	}
}

void inline update_wave(const double time_interval, double* position, double* speed)
{
	// Wave movement
	(*position) += time_interval * (*speed);

	// Boundary reflection
	if((*position) > 1.0)
	{
		(*speed) *= -1.0;
		(*position) = 1.0 + time_interval * (*speed);
	}
	else if((*position) < 0.0)
	{
		(*speed) *= -1.0;
		(*position) = time_interval * (*speed);
	}
}

/**
 * Wave state == position && velocity
 * X -> Wave one, Y -> Wave two
 */
int main()
{
	// Display Waves
	const double wave_length_x = 0.8;
	const double wave_length_y = 1.2;
	const double max_height_x = 0.5;
	const double max_height_y = 0.4;
	array<double, k_buffer_size> height_field;
	
	// Defining the Initial States
	double x = 0.0;
	double speed_x = 1.0;

	double y = 1.0;
	double speed_y = -0.5;

	// Computing Motion
	const int fps = 60;
	const double time_interval = 1.0 / fps; // seconds per frame
	
	for(int i = 0; i < 1000; ++i)
	{
		// Update Waves
		update_wave(time_interval, &x, &speed_x);
		update_wave(time_interval, &y, &speed_y);

		// Clear height field
		for(double& height : height_field)
		{
			height = 0.0;
		}

		// Accumulate waves for each center point
		accumulate_wave_to_height_field(x, wave_length_x, max_height_x, &height_field);
		accumulate_wave_to_height_field(y, wave_length_y, max_height_y, &height_field);

		// Draw height field
		draw(height_field);

		// Wait
		this_thread::sleep_for(milliseconds(1000/fps));
	}
	printf("\n");
	fflush(stdout);
	
	return 0;
}