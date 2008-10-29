#!/usr/bin/perl

use strict;
use Data::Dumper;

use lib '/mounts/data/proj/impact/software/Vaam/perl';
use Vaam;

use lib '/mounts/Users/student/annette/impl/libChecked';
use komposita;

if( @ARGV < 3 ) {

    print STDERR <<HELP;
 Use like: \.\/vaamDemo.pl <distance> <dicFile> <patternFile>

 A small demo program for the perl Vaam interface.
 Query a word and get a Data::Dumper output of the answer aggregate.
 Note that the non-asscii characters are messed up by Data::Dumper, but
 are ok in the data structure. 

HELP

    exit( 1 );
}


my $vaam = new Vaam( distance => shift @ARGV,
		     dicFile =>  shift @ARGV,
		     patternFile =>  shift @ARGV,
		     
    ) or die $!;

$vaam->setSlimMode( 1 );

my $komposita = komposita->new();

while ( my $word = <> ) {
    chomp $word;
    my $answer = $vaam->lookup( $word );

#    print Dumper $answer;

    if( $answer->{vaamString} =~ m/NONE/ ) {

	# $word ist direkt im CISLEX
	if( $komposita->simplex($word)){
	    print "$word:$word+[],dist=0\n";
	    warn "vaamFilter.pl: Looks like inconsistent lexicon data ($word)";
	}
	# $word kann als Kompositum zerlegt werden
	elsif( $komposita->zerlege($word)){
	    print "$word:$word+[],dist=0\n";
	}
	# $word ist komisch.
	else{
	    print $answer->{vaamString}, "\n";
	}

    }
    else {
	print $answer->{vaamString}, "\n";
    }
}
