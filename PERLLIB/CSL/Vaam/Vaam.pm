#!/usr/bin/perl

use strict;
use encoding 'utf8';
use Data::Dumper;



=head1 NAME

Vaam - An interface to the (V)ariant-(a)ware (a)pproximate (m)atcher

=head1 SYNOPSIS

 my $vaam = new Vaam( distance => 1,
                      dicFile => 'some_dict.mdic',
		      patternFile => 'some_patterns.txt' )

 my $answer = $vaam->lookup( "theyl" );

=head1 DESCRIPTION

This module uses the binary 'vaamFilter' (originally written in c++) to provide a perl interface for Vaam.

Comments, bug reports etc. are appreciated, write to: uli@cis.uni-muenchen.de

=cut

package Vaam;


use IPC::Open2;

=pod 

=head1 CONSTRUCTOR

The new() constructor uses 'key => value' syntax for its arguments, where the keys 'dicFile' and 'patternFile'
are obligatory.

All allowed keys are:

=over 3

=item B<dicFile> => 'some_dict.mdic'

path to a .mdic dictionary file

=item B<patternFile> => 'some_patterns.txt'

path to a plaintext file defining a set of patterns with each line simply giving left and right side separated by a SPACE

=item B<distance> => n

to allow up to n standard-levenshtein edit operations (apart from the variant patterns). Defaults to 0.

=item B<vaamBinary> => '/some/where/binary'

to choose a binary path other than the one on the CIS installation

=back

=cut

sub new {
    my $self = {};
    ( my $class, %$self ) = @_;

    $self->{distance} = $self->{distance} || 0;

    if( $self->{distance} != 0 ) {
	die "Perl::Vaam: Sorry, the module is not yet prepared for distance > 0! ";
    }


    if( ! $self->{vaamBinary} ) {
	$self->{vaamBinary} = '/mounts/data/proj/impact/software/Vaam/bin/vaamFilter';
	if( $ENV{HOSTNAME} =~ /(vector)|(butler)|(matrix)/) {
	    $self->{vaamBinary} .= '_'.$ENV{HOSTNAME};
	}
	else {
	    $self->{vaamBinary} .= '_workstation';
	}
	#print $self->{vaamBinary}, "\n"; exit;
    }

    if( !$self->{dicFile} || !$self->{patternFile} ) {
	$! = "Perl::Vaam: provide arguments 'dicFile' and 'patternFile' as arguments for the constructor.\n";
	return undef;
    }

    my $options ='';
    if( $self->{maxNrOfPatterns} ) {
	$options .= " --maxNrOfPatterns=". $self->{maxNrOfPatterns};
    }
    my $binary = "$$self{vaamBinary} $options $self->{distance} $self->{dicFile} $self->{patternFile}";
    
    open2( $self->{BINARY_OUT}, $self->{BINARY_IN}, $binary ) or die "Perl::Vaam: $!";
    

    if( defined $self->{encoding} ) {
	if( $self->{encoding} eq 'iso' ) {
	    $self->{encoding} = 'bytes';
	}
    }
    else { # if nothing is specified, use utf8
	$self->{encoding} = 'utf8';
    }

    binmode( $self->{BINARY_OUT}, ':'.$self->{encoding} );
    binmode( $self->{BINARY_IN}, ':'.$self->{encoding} );

    bless( $self, $class );
    return $self;
}


=pod 

=head1 METHODS

=over 1


=item setSlimMode( $bool )

set slimMode to true or false. In slimMode Vaam does not create an answer-object but just 
returns the string as it gets it from the binary.

=cut

sub setSlimMode {
    my( $self, $bool ) = @_;
    $self->{slimMode} = $bool;
}


=item lookup( $oldWord )

Returns a data-structure containing all found interpretations of $oldWord as a historical variant.

=cut

sub lookup {
    my($self, $word) = @_;


    print { $self->{BINARY_IN} } ( "$word\n" );

    my $output = readline( $self->{BINARY_OUT} );
    chomp $output;


    my @ints;

    my $answer = { query => $word, vaamString => $output };
    $answer->{foundInLex} = 0; # this might be changed!
    $answer->{interpretations} = \@ints unless( $self->{slimMode} );

    if( $output =~ m/\:NONE$/ ) {
	return $answer;
    }

    unless( $self->{slimMode} ) {
	for my $intString ( split( /\|/, $output ) ) { # for all interpretations
	    my %int; my $instruction;
	    
	    if( ( $int{candidate}, $int{modernWord}, $instruction, $int{distance} ) = ( $intString =~ m/(.+?):(.+?)\+\[(.*?)\],dist=(.+?)/ ) ) {
		
		@{$int{instruction}} = (); # see that instruction is defined even if it remains empty
		
		while( $instruction =~ m/\((.*?)_(.*?),(\d+)\)/g ) { # for all posPatterns of the instruction
		    push( @{$int{instruction}}, { left => $1, right => $2, pos => $3 } );
		}
		if( ! @{$int{instruction}} ) {
		    $answer->{foundInLex} = 1;
		}
		push( @ints, \%int );
		
	    }
	    else {
		# error?!
	    }
	}
    }


    return( $answer );
}

=item close()

Close pipe to binary 


=cut
sub close {
    close(BINARY_IN);
    close(BINARY_OUT);
}

=pod

=back

=head1 AUTHOR

Ulrich Reffle<uli@cis.uni-muenchen.de>, 2008

=cut

1;

__END__

