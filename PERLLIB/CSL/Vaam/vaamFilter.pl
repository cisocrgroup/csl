#!/usr/bin/perl

use strict;
use Data::Dumper;

use lib '/mounts/Users/student/uli/implement/csl/trunk/PERLLIB';
use CSL::Vaam;

if( @ARGV < 3 ) {

    print STDERR <<HELP;
 Use like: \.\/vaamDemo.pl <distance> <dicFile> <patternFile>

 A simple command line program using the perl Vaam interface.
 Query a word and get the answer aggregate printed to stdout.

HELP

    exit( 1 );
}


my $vaam = new Vaam( distance => shift @ARGV,
		     dicFile =>  shift @ARGV,
		     patternFile =>  shift @ARGV,
		     vaamBinary => '/mounts/Users/student/uli/implement/csl/trunk/build_platane/bin/vaamFilter'
    ) or die $!;

#$vaam->setSlimMode( 1 );


while ( my $word = <> ) {
    chomp $word;
    my $answer = $vaam->lookup( $word );

    print Dumper $answer;

}
