#!/usr/bin/perl

use strict;
use locale;
use encoding 'utf8';


while( my $line = <> ) {
    my( $word, $patterns ) = ( $line =~ m/(.*)\|(.*)/ );

    while( $patterns =~ m/\[(.*?)\]/g ) { # for all readings
	my $instructions = $1;
	my $baseWord = $word;

	my @patterns = reverse( $instructions =~ m/\((.*?)\)/g );
	for my $pattern( @patterns ) { # for all patterns
	    my( $from, $to, $pos ) = ( $pattern =~ m/(.*)_(.*), (.*)/ );
	    substr( $baseWord, $pos, length( $from ), $to );
	}
	print "$word|\[$instructions\]|$baseWord\n";
    }

}
