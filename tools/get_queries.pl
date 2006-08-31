#!/usr/bin/perl

use strict;
use Getopt::Long;

#####CONFIG###########
# These are default values - most of them can be overwritten by command-line flags
my $partsDelimiter = '$'; ## Caution: there are hard-wired , and $ as delimiters around line 100
my $tokensDelimiter = ',';

my @permute = (); # 0 / 1
my $groundtruth = 0; # 0 / 1

my $symbol_error_rate = 0;   # 0 <= $symbol_error_rate <= 1
my $errors = 0;
my $glob = 0;
#####################

my $help;
GetOptions(
	   'help' => \$help,
	   'error_rate=f' => \$symbol_error_rate,
	   'errors=i' => \$errors,
	   'glob=i' => \$glob,
	   'permute=s' => \@permute,
	   'groundtruth!' => \$groundtruth,
	   'tokensDelimiter=s' => \$tokensDelimiter,
	   'partsDelimiter=s' => \$partsDelimiter,
	   );

if(@ARGV<3 or $help) {
    print STDERR <<TXT;
Use like: ./get_queries.pl <alph_file> <text_db_file> <nr_of_queries>

Flags:
--help             display this message and quit

--error_rate N     specify a symbol-error rate (0 <= N <= 1, default 0)
--errors N         specify number of errors for each line (N>=0, default 0)
--glob N           transform one area of N characters into a single '_'

--permute Str      Str may be 'tokens' or 'parts' to permute either the tokens 
                   inside parts or the parts among each other. 
		    Default: no permutations

--groundtruth      print pairs "groundtruth<SPACE>query";
--nogroundtruth    do not add the groundtruth (default)

TXT
    exit(1);
}

my $alph_file = $ARGV[0];
my $db_file = $ARGV[1];
my $nr_of_queries = $ARGV[2];

my $partsDelimRex = '\\'.$partsDelimiter;
my $tokensDelimRex = '\\'.$tokensDelimiter;
my $allDelimRex = "($partsDelimRex|$tokensDelimRex)";


open(ALPH,"<$alph_file") or die $!;
my @alph;
my @alph_in = split(//,<ALPH>);

#remove delimiter from alphabet
for my $c (@alph_in) {
    if(($c ne $tokensDelimiter) && ($c ne $partsDelimiter)) {
	push(@alph,$c);
    }
}

close(ALPH);

open(DB,"<$db_file") or die $!;
my @db = <DB>;
close(DB);


for(1..$nr_of_queries) {
    my $rand_index = int(rand($#db +1));

    chomp $db[$rand_index];
    $db[$rand_index] =~ s/\#.*//g;

    my $entry = $db[$rand_index];

    if($symbol_error_rate > 0) { # create symbol errors by error rate
	my @entry = split(//,$entry); #split into characters
	for my $c (@entry) {
	    if($c !=~ m/$allDelimRex/) {
		$_ = $alph[int(rand($#alph))];
	    }
	}
	$entry = join('',@entry);
    }


    if($glob > 0) {
	my $randPos = int(rand(length($entry) - $glob + 1)) ;
	substr($entry,$randPos,$glob) = 'x'x$glob;
	
    }


    for my $permuteMode (@permute) {
	if($permuteMode eq 'tokens') { #permute some tokens inside parts
	    my @parts = split(/$partsDelimRex/,$entry); #split into parts
	    $entry = '';
	    my $temp;
	    my $i1, my $i2;
	    my @newParts;
	    my $newPart;
	    for my $part(@parts) {
		my @tokens = split(/$tokensDelimRex/,$part); #split into tokens
		while(@tokens) {
		    # take a random elt out of @tokens, add it to $entry
		    $entry .= splice(@tokens,int(rand(@tokens)),1).$tokensDelimiter;
		}
		substr($entry,-1,1,$partsDelimiter);
	    }
	}

	if($permuteMode eq 'parts') { #permute some parts
	    my @parts = split(/$partsDelimRex/,$entry); #split into parts
	    $entry = '';
	    while(@parts) {
		# take a random elt out of @parts, add it to $entry
		$entry .= splice(@parts,int(rand(@parts)),1).$partsDelimiter;
	    }
	}
    }


    # do this at the end so you can throw away the different delimiters
    if($errors > 0) { # create symbol errors by number (not so terribly correct, caus deletions wrack the seen-array ... :-/)
	my @tokens=split(/\,|\$/,$entry);
	for(@tokens) {
	    $_ = addErrors($_,$errors);
	}
	$entry = join($tokensDelimiter, @tokens);

    }


    $entry =~ s/$partsDelimRex/$tokensDelimiter/g; # Finally remove all structure from the query

    if($groundtruth) {
	print STDOUT "$db[$rand_index]\t$entry\n";
    }
    else {
	print STDOUT $entry,"\n";
    }
}

sub randomChar {
    return $alph[int(rand($#alph))];
}

sub addErrors {
    my $word = shift;
    my $errors = shift;
    my @seen;
    for (1..$errors) {
	my $rand = rand(3);
	$rand = 2;
	my $randPos = int(rand(length($word)));
	
	if($rand < 1) { #insertion
	    substr($word,$randPos,0) = randomChar();
	    $seen[$randPos] = 1;
	}
	elsif($rand<2) { #deletion
	    unless(length($word)<2) { # don't kill a very last char
		substr($word,$randPos,1) = "";
	    }
	}
	elsif($_ <= length($word)) { # substitution (don't try to sub. more chars than there are in $word)
	    substr($word,$randPos,1) = randomChar();
	    $seen[$randPos] = 1;
	}
    }
    return $word;
}
