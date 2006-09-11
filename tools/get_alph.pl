#!/usr/bin/perl
#
# This script reads from STDIN and returns to STDOUT all the occuring characters in alphabetical 
# order. 
#
# Author: Uli Reffle, <uli@reffle.de>
# Sept 2004

use strict;
use Getopt::Long;
use open "IN" => ":bytes",
    "OUT" =>":utf8";


my $delimiter = '';
GetOptions( 'delimiter=s' => \$delimiter,);

my %alph;
while(<>) {
    chomp;
    if( $delimiter ) {
	s/$delimiter.*//;
    }

    s/\r\n$/\n/;
    for(split(//)) {$alph{$_} = 1;}
}


delete($alph{'\n'});
delete($alph{'\r'});
for (sort keys(%alph)) {
    print;
}
