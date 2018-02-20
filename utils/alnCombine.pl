#!/usr/bin/perl
#
# last change Time-stamp: <2018-02-20 18:30:07 adonath> 
#
# AUTHOR: Alexander Donath
# <a [dot] donath [theSymbol] zfmk [dot] de>

use strict;
use warnings;
use FileHandle;
use Pod::Usage;
use Getopt::Long;
use vars qw($USAGE);

my ($opt_n,$opt_c,$opt_o,$opt_m) = ("?","?","aln.cat.aln",0);
my ($h,$man) = (0,0);

&GetOptions(
    'n=s'    => \$opt_n,
    'c=s'    => \$opt_c,
    'o=s'    => \$opt_o,
    'm'      => \$opt_m,
    'h'      => \$h,
    'man'    => \$man    
    ) or pod2usage(2);

my @files = @ARGV;

&pod2usage(-verbose => 3) if $man;
&pod2usage(-verbose => 0) if ($h || !@files);

my %bigAlignment = ();
my @desc = ();
my $new;
my $length = 0;

for my $nextFile (@files) {

    my %nextAln = ();
    if(-e $nextFile) {
	%nextAln = fastaHash($nextFile);
    } else {
	die "File ".$nextFile." does not exist.\n".$USAGE;
    }

    if (!$opt_m) {
	while (my ($id, $seq) = each(%nextAln)) {
	    $nextAln{$id} = masking($seq,$opt_c);
	}
    }

    $new = 1;

    while ( my ($id, $seq) = each(%nextAln)) {
	if ($new) {
	    $length += length($seq);
#	    print STDERR "length: ".$length."\n";
	    $new = 0;
	}
	if (exists $bigAlignment{$id}) {
	    if ((length($bigAlignment{$id})-1) == ($length - length($seq)) ) {
		$bigAlignment{$id} = $bigAlignment{$id} . $seq;
	    } else {
		my $gap = $opt_n x ($length - length($bigAlignment{$id}) - length($seq));
		$bigAlignment{$id} = $bigAlignment{$id} . $gap . $seq;
	    }
	} else {
	    my $gap = $opt_n x ($length - length($seq));
	    $bigAlignment{$id} = $gap . $seq;
	}
    }

    while (my ($id, $seq) = each(%bigAlignment)) {
	if ($length != (length($seq)-1)) {
	    my $gap = $opt_n x ($length - length($seq));
	    $bigAlignment{$id} = $bigAlignment{$id} . $gap;
	} else {
	}
    }
}
print STDERR "length: ".$length."\n";

my $fh = FileHandle->new;
if ($fh->open("> $opt_o")) {
    while ( my ($id, $seq) = each(%bigAlignment) ) {
	print $fh ">$id";
	print $fh "\n".$seq."\n";
    }
    $fh->close;
} else {
    die "ERROR: Can not open file='$opt_o'!";
}


### SUBS ###

##############################################################
# masking():    masks leading and trailing gaps              #
# Author:       Alexander Donath                             #
# Parameters:   seq: the sequences, char: gap-replacement    #
# Returns:      the masked sequence                          #
##############################################################
sub masking {

    my ($seq, $char) = @_;
    $seq =~ s/^(-*)//;
    my $i = length($1); 
    $seq =~ s/(-*)$//; 
    my $j = length($1); 

    if ($i) {
	my $nseq = $char x $i;;
	$seq = $nseq . $seq;
    }
    if ($j) {
	my $nseq = $char x $j;
	$seq .= $nseq;
    }	
    return $seq;
}   


##############################################################
# fastaHash():  read in a fasta file as hash                 #
# Author:       Alexander Donath                             #
# Parameters:   file: the fasta file                         #
# Returns:      a hash containing the fasta file             #
##############################################################
sub fastaHash {

    my ($file) = @_;
    my %hash;
    my ($key, $desc) = ("", "");
    my $fh = FileHandle->new();

    if ($fh->open("< $file")) {
        while (my $line = $fh->getline()) {
	    chomp $line;

	    if ($line =~ /^>(\S+)(.*)$/) {
		$key = $1;
		$desc = $2;
		$hash{"$key$desc"}="";
	    }
	    elsif($line=~/^\n/ || $line=~/^\s/) {
		next;
	    }
	    else {
		$hash{"$key$desc"} .= $line;
	    }
        }
	$fh->close;
    } else {
	die "ERROR: Can not open file: '$file'!";
    }
    return %hash;
}




##############################################################
# fastaArray(): read in a fasta file as array of arrays      #
# Author:	Alexander Donath                             #
# Parameters:   file: the fasta file                         #
# Returns:	an array of arrays containing the file       #
#              	0. key, 1. sequence                          #
##############################################################
sub fastaArray {

	my ($file) = @_;
	my @a=();
	my $fh = FileHandle->new();
	
	if ($fh->open("< $file")) {
	    
	    while(my $line = $fh->getline()) {
		chomp $line;
		my $seqno = -1;
		
		if($line =~ /^>(\S+)(.+)$/){
#		    my @header;
#		    $header[0] = $1.$2;
		    push @a, [$1.$2];
		    $seqno++;
		}
		else {
		    $a[$seqno][1] .= $line;
		}
	    }
	    
	    $fh->close();
	}else {
	    die "ERROR: Can not open file: '$file'!";
	}

	return @a;
}


__END__



=head1 NAME

 alnCombine.pl - Concatenation of MSAs in FASTA format.

=head1 SYNOPSIS

 alnCombine.pl [options] file1 file2 .. fileN

   -n    Char to use if taxa has no sequence. [Default: ?]
   -o    Outfile [Default: aln.cat.aln]
   -m    No masking of alignment ends. [Default: masking]
   -c    Char to use for masking. [Default: ?]
   -h    Print help message.
   -man  Full documentation.

=head1 OPTIONS AND ARGUMENTS
    
 -n    Char to use if taxa has no sequence.
        [Default: ?]

 -o    Outfile
        [Default: aln.cat.aln]

 -m    No masking of alignment ends.
        [Default: masking]

 -c    Char to use for masking.
        [Default: ?]

 -h    Print help message.
        [OPTIONAL]

 -man  Print full documentation.

=head1 DESCRIPTION

 Concatenates sequences of different multiple (fasta) alignments
 beloning to the same species. Make sure sequence IDs are unique
 within files and identical between files. If a sequence doesn't
 appear in all files gap characters ('-n') are used. Masking ('-m')
 changes leading and trailing gaps (see '-c').

=head1 EXAMPLES

 alnCombine.pl [options] alnfile_1 alnfile_2 [...] alnfile_n
 alnCombine.pl [options] alnfile_*

=head1 DISCLAIMER

 The author is not responsible for any loss of data, wrong 
 research results or anything else which may be caused by using 
 this tool.

 This script was written for my own needs. Depending on your specific
 situation, it might be not suitable for you. If this is the case,
 please see below.

=head1 SEE ALSO

 If the script does not suit your needs, have a look at FASconCAT-G

 https://doi.org/10.1186/s12983-014-0081-x

 https://github.com/PatrickKueck/FASconCAT-G

=head1 FEEDBACK

=head2 Reporting Bugs

 This script does not, as far as known, obey Sturgeons law, 
 which says: 90% of everything is crud.

 In case you have found any bugs, please report them to the author.

=head1 AUTHOR

 Alexander Donath 

 <a [dot] donath [theSymbol] zfmk [dot] de>

=head1 VERSION

 v0.1 (Feb 2016)

 Please visit <https://github.com/alexdonath/gappy> for 
 possible updates of this perl script.

=cut
