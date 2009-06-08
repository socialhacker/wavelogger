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

using namespace Err;
using namespace Files;

/**********************************************************************************************************************/
namespace CommandLine
{
    static Scalar<Path>		file   ("file",    false, Path("."),
					"Wavedata file to process.");

    static Scalar<Path>		output ("output",  true,  Path("output.csv"),
					"Filename for processed CSV data.");

    static Scalar<uint64>	start  ("start",   true,  0,
					"Timestamp to start dumping from.");

    static Scalar<uint64>	stop   ("stop",    true,  0,
					"Timestamp to stop dumping at.");

    static Scalar<uint64>	samples("samples", true,  0,
					"Number of samples to dump.");

    static Argument	*arguments[] = { &file,
					 &output,
					 &start,
					 &stop,
					 &samples,
					 null };
}
/**********************************************************************************************************************/
int	count   = 0;
int	total   = 0;
bool	process = false;
FILE	*output = null;
uint64	start   = 0;
uint64	stop    = 0;
uint64	samples = 0;

void process_block(Block *block)
{
    uint64	ticks = block->ticks();

    switch (block->type())
    {
	case Block::data:
	    ++count;

	    if (process && (ticks + 200) >= start && ticks < stop)
	    {
		if (samples > 0)
		{
		    int	min[4];
		    int	max[4];

		    for (int i = 0; i < 4; ++i)
		    {
			min[i] = block->sample(0, i) & 0xfff;
			max[i] = min[i];
		    }

		    for (uint j = 1; j < 200; ++j)
		    {
			for (int i = 0; i < 4; ++i)
			{
			    int	sample = block->sample(j, i) & 0xfff;

			    if (min[i] > sample) min[i] = sample;
			    if (max[i] < sample) max[i] = sample;
			}
		    }

		    fprintf(output, "%lld, %d, %d, %d, %d, %d, %d, %d, %d\n",
			    ticks,
			    min[0], max[0],
			    min[1], max[1],
			    min[2], max[2],
			    min[3], max[3]);
		}
		else
		{
		    for (uint i = 0; i < 200; ++i)
			fprintf(output, "%lld, %d, %d, %d, %d\n",
				ticks + i,
				block->sample(i, 0) & 0xfff,
				block->sample(i, 1) & 0xfff,
				block->sample(i, 2) & 0xfff,
				block->sample(i, 3) & 0xfff);
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

    CheckCleanup(CommandLine::parse(argc, argv, CommandLine::arguments), failure);

    if (CommandLine::start.set() ^ CommandLine::stop.set())
    {
	printf("Start and Stop must both be specified if either is.\n");
	CommandLine::usage(argv[0], CommandLine::arguments);
	exit(-1);
    }

    if (!(CommandLine::start.set() || CommandLine::stop.set()) && CommandLine::samples.set())
    {
	printf("Start and Stop must be set if Samples is used.\n");
	CommandLine::usage(argv[0], CommandLine::arguments);
	exit(-1);
    }

    process = (CommandLine::start.set() || CommandLine::stop.set());

    if (process)
    {
	start   = CommandLine::start.get();
	stop    = CommandLine::stop.get();
	samples = CommandLine::samples.get();
	output  = fopen(CommandLine::output.get().get().str(), "w");

	CheckCleanupStringB(output, failure, "Failed to open file (%s) for writing.",
			    CommandLine::output.get().get().str());
    }

    CheckCleanup(wavefile.read(CommandLine::file.get(), process_block), failure);

    wavefile.debug_print(0);

    printf("There were %d data blocks in the file, %d of them mattered.\n", count, total);

    if (process)
    {
	fclose(output);
    }

    return 0;

  failure:
    error_stack_print();
    return -1;
}
/**********************************************************************************************************************/
