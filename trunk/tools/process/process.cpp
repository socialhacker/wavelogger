/*
 * Copyright 2009 Anton Staaf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "process/wavefile.h"

#include "libs/error/error.h"
#include "libs/files/path.h"
#include "libs/command_line/command_line.h"

#include <math.h>

using namespace Err;
using namespace Files;

/**********************************************************************************************************************/
namespace CommandLine
{
    static Scalar<Path>		file    ("file",     false, Path("."),
					 "Wavedata file to process.");

    static Scalar<Path>		output  ("output",   true,  Path("."),
					 "Filename for processed CSV data.");

    static Scalar<uint64>	start   ("start",    true,  0x0000000000000000,
					 "Timestamp to start dumping from.");

    static Scalar<uint64>	stop    ("stop",     true,  0x7fffffffffffffffll,
					 "Timestamp to stop dumping at.");

    static Scalar<bool>		average ("average",  true,  false,
					 "Output average of each block of 200 samples.  Also output "
					 "battery and temperature statistics.");

    static Scalar<float>	one_pos ("one-pos",  true,  1.0f,
					 "First axis positive calibration factor in Antons/Newton.");

    static Scalar<float>	one_neg ("one-neg",  true,  1.0f,
					 "First axis negative calibration factor in Antons/Newton.");

    static Scalar<int>		one_zero("one-zero", true,  512,
					 "First axis zero in Antons.");

    static Scalar<float>	two_pos ("two-pos",  true,  1.0f,
					 "Second axis positive calibration factor in Antons/Newton.");

    static Scalar<float>	two_neg ("two-neg",  true,  1.0f,
					 "Second axis negative calibration factor in Antons/Newton.");

    static Scalar<int>		two_zero("two-zero", true,  512,
					 "Second axis zero in Antons.");

    static Argument	*arguments[] = { &file,
					 &output,
					 &start,
					 &stop,
					 &average,
					 &one_pos,
					 &one_neg,
					 &one_zero,
					 &two_pos,
					 &two_neg,
					 &two_zero,
					 null };
}
/**********************************************************************************************************************/
bool	average      = false;
bool	newtons      = false;
bool	output       = false;

uint64	start        = 0;
uint64	stop         = 0;

float	one_pos      = 1.0f;
float	one_neg      = 1.0f;
float	two_pos      = 1.0f;
float	two_neg      = 1.0f;

int	one_zero     = 512;
int	two_zero     = 512;

FILE	*output_file = null;

int	count        = 0;
int	total        = 0;

/**********************************************************************************************************************/
float get_newtons(Block *block, int index, int channel)
{
    int		sample   = block->sample(index, channel) & 0xfff;
    float	sample_N = 0;

    if (channel == 0)
    {
	sample_N = sample - one_zero;

	if (sample_N > 0.0f)	sample_N /= one_pos;
	else			sample_N /= one_neg;
    }
    else
    {
	sample_N = sample - two_zero;

	if (sample_N > 0.0f)	sample_N /= two_pos;
	else			sample_N /= two_neg;
    }

    return sample_N;
}
/**********************************************************************************************************************/
void process_block(Block *block)
{
    uint64	ticks = block->ticks();

    switch (block->type())
    {
	case Block::data:
	    ++count;

	    if (output && (ticks + 200) >= start && ticks < stop)
	    {
		if (average)
		{
		    if (newtons)
		    {
			float	sample[3];
			float	average[3];
			float	min[3];
			float	max[3];

			sample[0] = get_newtons(block, 0, 0);
			sample[1] = get_newtons(block, 0, 1);
			sample[2] = ::sqrt(sample[0] * sample[0] + sample[1] * sample[1]);

			for (uint i = 0; i < 3; ++i)
			{
			    min[i]     = sample[i];
			    max[i]     = sample[i];
			    average[i] = sample[i];
			}

			for (uint j = 1; j < 200; ++j)
			{
			    sample[0] = get_newtons(block, j, 0);
			    sample[1] = get_newtons(block, j, 1);
			    sample[2] = ::sqrt(sample[0] * sample[0] + sample[1] * sample[1]);

			    for (uint i = 0; i < 3; ++i)
			    {
				if (min[i] > sample[i]) min[i] = sample[i];
				if (max[i] < sample[i]) max[i] = sample[i];

				average[i] += sample[i];
			    }
			}

			for (uint i = 0; i < 3; ++i)
			    average[i] /= 200.0f;

			fprintf(output_file, "%lld, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				ticks,
				average[0], min[0], max[0],
				average[1], min[1], max[1],
				average[2], min[2], max[2]);
		    }
		    else
		    {
			float	average[2];
			int	min[2];
			int	max[2];

			for (int i = 0; i < 2; ++i)
			{
			    min[i]     = block->sample(0, i) & 0xfff;
			    max[i]     = min[i];
			    average[i] = min[i];

			    for (uint j = 1; j < 200; ++j)
			    {
				int	sample = block->sample(j, i) & 0xfff;

				if (min[i] > sample) min[i] = sample;
				if (max[i] < sample) max[i] = sample;

				average[i] += sample;
			    }

			    average[i] /= 200.0f;
			}

			fprintf(output_file, "%lld, %f, %d, %d, %f, %d, %d\n",
				ticks,
				average[0], min[0], max[0],
				average[1], min[1], max[1]);
		    }
		}
		else
		{
		    for (uint i = 0; i < 200; ++i)
		    {
			if (newtons)
			{
			    float	one_N = get_newtons(block, i, 0);
			    float	two_N = get_newtons(block, i, 1);

			    fprintf(output_file, "%lld, %f, %f\n", ticks + i, one_N, two_N);
			}
			else
			{
			    int		one   = block->sample(i, 0) & 0xfff;
			    int		two   = block->sample(i, 1) & 0xfff;

			    fprintf(output_file, "%lld, %d, %d\n", ticks + i, one, two);
			}
		    }
		}

		++total;
	    }
	    break;

	default:
	    break;
    }
}
/**********************************************************************************************************************/
int main(int argc, const char **argv)
{
    Error	check_error = success;
    Wavefile	wavefile;

    CheckCleanup(CommandLine::parse(argc, argv, CommandLine::arguments), parse_failure);

    average  = CommandLine::average.get();
    newtons  = (CommandLine::one_pos.set() |
		CommandLine::one_neg.set() |
		CommandLine::two_pos.set() |
		CommandLine::two_neg.set());
    output   = CommandLine::output.set();

    start    = CommandLine::start.get();
    stop     = CommandLine::stop.get();

    one_pos  = CommandLine::one_pos.get();
    one_neg  = CommandLine::one_neg.get();
    one_zero = CommandLine::one_zero.get();
    two_pos  = CommandLine::two_pos.get();
    two_neg  = CommandLine::two_neg.get();
    two_zero = CommandLine::two_zero.get();

    /*********************************************************************************************/
    if (output)
    {
	output_file = fopen(CommandLine::output.get().get().str(), "w");

	CheckCleanupStringB(output_file, failure, "Failed to open file (%s) for writing.",
			    CommandLine::output.get().get().str());

	{
	    const char	*units;

	    if (newtons) units = "Newtons";
	    else         units = "Antons";

	    if (average)
		fprintf(output_file, "timestamp, CH1 Avg (%s), CH1 Min (%s), CH1 Max (%s), CH2 Avg (%s), CH2 Min (%s), CH2 Max (%s)",
			units, units, units, units, units, units);
	    else
		fprintf(output_file, "timestamp, CH1 (%s), CH2 (%s)\n",
			units, units);

	    if (newtons)
		fprintf(output_file, ", Mag Avg (Newtons), Mag Min (Newtons), Mag Max (Newtons)\n");
	}
    }

    CheckCleanup(wavefile.read(CommandLine::file.get(), process_block), failure);

    wavefile.debug_print(0);

    printf("There were %d data blocks in the file, %d of them mattered.\n", count, total);

    if (output_file)
	fclose(output_file);

    /*********************************************************************************************/

    return 0;

  failure:
    error_stack_print();

  parse_failure:
    error_stack_clear();
    return -1;
}
/**********************************************************************************************************************/
