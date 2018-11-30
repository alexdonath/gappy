# Important

Please note that gappy (v1) is not supported anymore. Please use the successor [gappy2](https://github.com/alexdonath/gappy2).

# gappy manual

gappy identifies splids (split-inducing indels) from multiple sequence alignments.

## Installation

### System requirements

gappy should compile on all current Linux systems.

### Compile from source

1. Clone the gappy repository from github

```txt
git clone https://github.com/alexdonath/gappy.git
```

2. Change to the local copy of the gappy repository

```txt
cd gappy
```

3. Compile gappy by typing

```bash
make
```

Optional: install gappy by typing

```bash
make install
```

The gappy binary can now be found in `./bin/`.

4. Copy binary to a place of your choice and/or add gappy to your `$PATH`.

## Usage

Typing

```bash
gappy
```

will show all available options and parameters:

```txt
Please provide an alignment file [-m MAF | -f FASTA].

Usage: gappy [OPTIONS]

gappy extracts splids (split-inducing indels) from multiple sequence alignments.

OPTIONS:

Generic options:
  -h, --help                  Display this help and exit.
  --version                   Output version information and exit.
  -v, --verbose               Show what's being done.
  --debug                     Show even more informations on what's being done.

Configuration (default values in brackets):
  -f, --fasta                 Name of input alignment file in FASTA format.
  -m, --maf                   Name of input alignment file in MAF format.
  -o, --output-prefix         Prefix for the output files. [GAPPY]
  -l, --minsize               Minimum indel size. [2]
  -u, --maxsize               Maximum indel size. [no limit]
  -g, --gapsize               Only search for indels of this size. [not set]
  -z, --fuzzy                 Use fuzzy search. [off]
  -c, --unknownchar           Unknown sites. [?]
  --wstart                    Start search at this column. [1]
  --wend                      End search at this column. [end of alignment]
  -n, --nexus                 Write result in NEXUS format. Default: FASTA format
                                only.
  -p, --phylip                Write result in PHYLIP format. Default: FASTA format
                                only.
```

## Getting started

### Input

The only required input is an alignment in FASTA (`-f|--fasta`) or MAF
(`-m|--maf`) format.

### Parameter selection

By default, gappy extracts all splids >= 2 bp and <= the length of
the alignment. This is a reasonable choice. Smaller splids have a
higher chance to appear by chance. You can adjust these settings by
choosing a larger minimum (`-l|--minsize`) and/or a lower upper
boundary for the splid length (`-u|--maxsize`).

Alternatively, only splids that have a fixed size (`-g|--gapsize`) can
be identified. For example,

```bash
./gappy -l 10 -u 10 -f input.fasta
```

is equivalent to

```bash
./gappy -g 10 -f input.fasta
```

Additionally, one can limit the search region within the alignment by
providing lower (`--wstart`) and upper (`--wend`) boundaries. By
default, the complete alignment is used.

Splids are defined as insertions/deletions (indels) that define a
bipartition of the sequence set. According to this definition, overlap
of a splid with another indel is not allowed. However, sometimes it
might be desirable to ignore single residue indels because they appear
more often by chance. gappy offers a fuzzy search mode (`-z|--fuzzy`)
for this.

During a multiple sequence alignment, gaps can be added to the
beginning and end of a sequence. This is the case, for example,
whenever incomplete sequenced genes are aligned. In splid analyses, it
might be necessary to mask these gaps because they provide no valid
information about the presence and/or absence of residues in these
sequence(s). The user should mask these gaps using a specific
character (`-c|--unknownchar`) that indicates the difference between a
true missing residue (a deletion, `-`) and residues for which no
information is available (e.g., `?`; default in gappy). Masking needs
to be done by the user before running gappy.

### Output

The output files will be named according to the input file with the
prefix `GAPPY`. Furthermore, a suffix will be appended that indicates
the selected minimum and maximum splid size, the binary coding of the
result and the output file type. For example, if your input alignment
is called `alignment.aln` and splids >= 2 bp and <= 10 bp are
extracted from the alignment, the result will be written to a file
called `GAPPY_alignment.aln_2-10.01.FAS`.

Application of the fuzzy search mode will be indicated by an
additional `_z` in the suffix.

By default, results are written in FASTA format. Other output formats
are NEXUS (`-n|--nexus`) and PHYLIP (`-p|--phylip`) format.

### Example

A toy example alignment is provided in the example/ directory.

```bash
./gappy -f example.fasta
```

will create a file called `GAPPY_example.fasta_2-29.01.FAS` that
contains:

```bash
>a
10
>b
00
>c
01
>d
01
>e
10
>f
00
>g
00
>h
00
```

Likewise,

```bash
./gappy -f example.fasta -z
```

will create a file called `GAPPY_example.fasta_2-29_z.01.FAS` that
contains:

```bash
>a
101
>b
000
>c
011
>d
010
>e
100
>f
000
>g
00?
>h
000
```