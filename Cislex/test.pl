#!/usr/bin/perl

use strict;

use lib "/mounts/Users/student/uli/implement/csl/trunk/Cislex";
use Cislex;

use IPC::Open2;

my $alphFile = shift;
my $lexFile = shift;

# Create a Cislex object.
my $lex = new Cislex( alphFile => $alphFile,
		      lexFile => $lexFile
		      );

if( !$lex ) {
    printf( "Obvioulsly the Cislex constructor has failed.\n");
    exit;
}

while( my $key = <> ) {
    chomp $key;
    my $output = $lex->lookup( $key );

    if( $output ) {
	print $output, "\n";
    }
    else {
	print "Key not found\n";
    }

}






# open2(*README, *WRITEME, "../bin/lookupCislex ");
# print WRITEME "here's your input\n";
# $output = <README>;
# close(WRITEME);
# close(README);




