#!/usr/bin/perl

use strict;

use lib "/mounts/Users/student/uli/implement/csl/trunk/Cislex";
use Cislex;

use IPC::Open2;

my $alphFile = shift;
my $dicFile = shift;
my $origFile = shift;

# Create a Cislex object.
my $lex = new Cislex( alphFile => $alphFile,
		      dicFile => $dicFile
		      );

if( !$lex ) {
    printf( "Obvioulsly the Cislex constructor has failed.\n");
    die;
}

open(ORIG, $origFile) or die $!;
while( my $line = <ORIG> ) {
    chomp $line;
    my( $key, $value ) = split( /\./, $line );
    my $output = $lex->lookup( $key );

    if( $output ne $value ) {
	print $line, "\n";
    }

}






# open2(*README, *WRITEME, "../bin/lookupCislex ");
# print WRITEME "here's your input\n";
# $output = <README>;
# close(WRITEME);
# close(README);




