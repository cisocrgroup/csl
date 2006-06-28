#!/usr/bin/perl

use strict;
use lib "/files/uli/cis/csl/tools";
use Freqhash;
use Getopt::Long;

$| = 1;


my $perm;
my $help;
my $sortFile;

GetOptions(
	   'help' => \$help,
	   'perm=s' =>\$perm,
	   'sortFile=s' =>\$sortFile,
	   );

if($help) {
    shift;
    print <<TEXT;
##########
re_sort.pl
##########

Reads a multi-token .lex file and resorts the items. Finally it brings the list into alphabetical order.
To resort in alphabetical order:
re_sort.pl source.lex > new.lex

To resort according to a certain score defined in score.txt:
re_sort.pl -f score.txt source.lex > new.lex

score.txt is expected to look like:
<token1>#<score1>
<token2>#<score2>

TEXT

    exit;
    
}

my $freqlist_file;
my $freqhash;
my $cmp;


if($sortFile) {
$freqhash = new Freqhash($sortFile);
$cmp = \&frqcmp;
}
else {
    $cmp = sub {return $a cmp $b};
}

my $partsDelimiter = '$';
my $tokensDelimiter = ',';
my $partsDelimRex = '\\'.$partsDelimiter;
my $tokensDelimRex = '\\'.$tokensDelimiter;
my $allDelimRex = "(?:$partsDelimRex|$tokensDelimRex?)";





my @lines;
my $lineCount = 0;

while(my $line=<>) {
    ++$lineCount; # lineCount is 1 for 1st line (not 0) !!!
    unless($lineCount % 10000) {print STDERR "\r$lineCount lines processed.";}
    $line=~s/\r\n$/\n/;
    chomp $line;

    if($perm eq 'tokens') {
	my @parts= split(/$partsDelimRex/,$line);
	for(my $i=0;$i<@parts; ++$i) {
	    my @toks = sort $cmp split(/,/,$parts[$i]);
	    $parts[$i] = join($tokensDelimiter,@toks);
	}
	push(@lines,[(join($partsDelimiter,@parts)."\$"),$lineCount]);
    }

}

print STDERR "Start sort.\n";
@lines = sort {$a->[0] cmp $b->[0]} @lines;

print STDERR "Start output.\n";

my $firstLine = shift @lines; #take 1st line
my $values = $firstLine->[1]; 
my $lastLine = $firstLine->[0]; 

for my $line (@lines) {
    if($line->[0] ne $lastLine) {
	print $lastLine,"#",$values,"\n";
	$values = "";
	$lastLine = $line->[0];
    }
    $values.= $line->[1].",";
}
print $lastLine,"#",$values,"\n";


sub frqcmp {
    my $x;
    if($x =($freqhash->lookup($a) <=> $freqhash->lookup($b))) {
	return $x;
#	return -$x;
    }
    else {
	return ($a cmp $b);
    }
}
